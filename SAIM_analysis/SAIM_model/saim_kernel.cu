/**/////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//  Copyright(c) 2018, Marshall Colville mjc449@cornell.edu                   //
//  All rights reserved.                                                      //
//                                                                            //
//  Redistribution and use in source and binary forms, with or without        //
//  modification, are permitted provided that the following conditions are    //
//  met :                                                                     //
//                                                                            //
//  1. Redistributions of source code must retain the above copyright notice, //
//  this list of conditions and the following disclaimer.                     //
//  2. Redistributions in binary form must reproduce the above copyright      //
//  notice, this list of conditions and the following disclaimer in the       //
//  documentation and/or other materials provided with the distribution.      //
//                                                                            //
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       //
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED //
//  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           //
//  PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER   //
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  //
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       //
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        //
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    //
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      //
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        //
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              //
//                                                                            //
//  The views and conclusions contained in the software and documentation are //
//  those of the authors and should not be interpreted as representing        //
//  official policies, either expressed or implied, of the SAIMScannerV3      //
//  project, the Paszek Research Group, or Cornell University.                //
//////////////////////////////////////////////////////////////////////////////*/

#include "saim_model_gpu.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#define __CUDA_INTERNAL_COMPILATION__
//#include <common_functions.h>
//#include <math_functions.h>
#include <math_constants.h>
#undef __CUDA_INTERNAL_COMPILATION__
#include <complex>

namespace saim_model_gpu
{
#define checkCuda(ans){cudaAssert((ans), __FILE__, __LINE__);}
   inline void cudaAssert(
      cudaError_t code,
      const char *file,
      int line,
      bool abort = true)
   {
      if (code != cudaSuccess)
      {
         fprintf(stderr, "GPUassert: %s, %s, %d\n", cudaGetErrorString(code), file, line);
         if (abort)
            exit(code);
      }
   }

   __global__ void EvaluateFunction(int emptyThreads, int samples, unsigned short *data, double *xvec, double *fvec, double *constants)
   {
      if ((blockIdx.x + 1 == gridDim.x) && (threadIdx.x > blockDim.x - emptyThreads))
         return;
      int tid = threadIdx.x + blockDim.x * blockIdx.x;
      int frame = (tid % samples);
      int pixel = tid / samples;
      double A{ xvec[pixel] }, B{ xvec[pixel + 1] }, H{ xvec[pixel + 2] };
      double c{ constants[frame * 3] }, d{ constants[frame * 3 + 1] }, phi{ constants[frame * 3 + 2] };
      double fval = A * (1 + 2 * c * cos(phi * H) - 2 * d * sin(phi * H) + c * c + d * d) + B;
      fvec[tid] = fval - data[tid];
   }

   __global__ void EvaluateJacobian(int emptyThreads, int samples, double *xvec, double *jvec, double *constants)
   {
      if ((blockIdx.x + 1 == gridDim.x) && (threadIdx.x > blockDim.x - emptyThreads))
         return;
      int tid = threadIdx.x + blockDim.x * blockIdx.x;
      int frame = (tid % samples);
      int pixel = tid / samples;
      int totalPixels = blockDim.x * gridDim.x / samples;
      int jidx = (pixel * 3) + (totalPixels * 3) * (pixel * samples + frame);
      double A{ xvec[pixel] }, H{ xvec[pixel + 2] };
      double c{ constants[frame * 3] }, d{ constants[frame * 3 + 1] }, phi{ constants[frame * 3 + 2] };
      jvec[jidx] = 1 + 2 * c * cos(phi * H) - 2 * d * sin(phi * H) + c * c + d * d;
      jvec[jidx + 1] = 1;
      jvec[jidx + 2] = -2 * A * phi * (c * sin(phi * H) + d * cos(phi * H));
   }

   GPUModel::GPUModel()
   {
      _initialized = false;
   }

   GPUModel::~GPUModel()
   {

   }

   int GPUModel::RegisterImages(std::vector<cv::Mat> &input)
   {
      _rawImgs = input;
      for (int i = 0; i < 3; i++)
      {
         _outputImgs.push_back(cv::Mat(_rawImgs[0].rows, _rawImgs[0].cols, CV_64F));
      }
      return 0;
   }

   int GPUModel::SetGrainSize(int val)
   {
      if (_initialized)
         return 1;
      _grainSize = val;
      return 0;
   }

   int GPUModel::InitializeBuffers()
   {
      //Setup the device - assumes that the system has 2 devices and uses the second
      checkCuda(cudaSetDevice(1));
      checkCuda(cudaSetDeviceFlags(cudaDeviceBlockingSync | cudaDeviceMapHost));

      _m = _rawImgs[0].rows * _rawImgs[0].cols;
      _n = _rawImgs.size();
      //Buffer sizes
      _datasz = _m * _n;
      _ngrains = _datasz % _grainSize == 0 ? _datasz / _grainSize : _datasz / _grainSize + 1;
      _xsz = _grainSize * 3;
      _fnsz = _grainSize * _n;
      _jacsz = _fnsz * _grainSize * 3;
      //Host side allocations in pinned memory
      checkCuda(cudaHostAlloc((void **)&_h_data, _datasz * sizeof(unsigned short), cudaHostAllocMapped | cudaHostAllocWriteCombined));
      checkCuda(cudaHostAlloc((void **)&_h_xvec, _xsz * sizeof(double), cudaHostAllocMapped | cudaHostAllocWriteCombined));
      checkCuda(cudaHostAlloc((void **)&_h_fvec, _fnsz * sizeof(double), cudaHostAllocMapped));
      checkCuda(cudaHostAlloc((void **)&_h_jvec, _jacsz * sizeof(double), cudaHostAllocMapped));
      checkCuda(cudaHostAlloc((void **)&_h_constvec, 3 * _n * sizeof(double), cudaHostAllocMapped | cudaHostAllocWriteCombined));
      //Device side allocations
      checkCuda(cudaHostGetDevicePointer(&_d_data, _h_data, 0));
      checkCuda(cudaHostGetDevicePointer(&_d_xvec, _h_xvec, 0));
      checkCuda(cudaHostGetDevicePointer(&_d_fvec, _h_fvec, 0));
      checkCuda(cudaHostGetDevicePointer(&_d_jvec, _h_jvec, 0));
      checkCuda(cudaHostGetDevicePointer(&_d_constvec, _h_constvec, 0));

      for (int i = 0; i < _jacsz; i++)
      {
         _h_jvec[i] = 0.0;
      }

      //Calculate the number of leftover threads for the FN and Jac kernel launches
      if (_fnsz % 512 == 0)
      {
         _blocksPerGrid = _fnsz / 512;
         _emptyThreads = 0;
      }
      else
      {
         _blocksPerGrid = _fnsz / 512 + 1;
         _emptyThreads = 512 - (_fnsz % 512);
      }

      if (_datasz % _grainSize == 0)
      {
         _ngrains = _datasz / _grainSize;
         _emptyPixels = 0;
      }
      else
      {
         _ngrains = _datasz / _grainSize + 1;
         _emptyPixels = _grainSize - _datasz % _grainSize;
      }
      _nVars = _grainSize * 3;
      _mPoints = _fnsz;
      
      for (int i = 0; i < _rawImgs.size(); i++)
      {
         unsigned short *ptr = _rawImgs[i].ptr<unsigned short>();
         for (int j = 0; j < _m; j++)
         {
            _h_data[j * _n + i] = ptr[j];
         }
      }

      _initialized = true;
      return 0;
   }

   int GPUModel::ReleaseBuffers(void)
   {
      checkCuda(cudaFreeHost(_h_data));
      checkCuda(cudaFreeHost(_h_xvec));
      checkCuda(cudaFreeHost(_h_fvec));
      checkCuda(cudaFreeHost(_h_jvec));
      checkCuda(cudaFreeHost(_h_constvec));
      _initialized = false;
      return 0;
   }

   int GPUModel::CalculateConstants(double wavelength, double dOx, double nB, double nOx, double nSi, double *angles)
   {
      double angleOx, angleSi, pB, pOx, pSi, kOx, m11, m12, m21, m22;
      std::complex<double> num, denom, rTE;
      for (int i = 0; i < _n; i++)
      {
         angleOx = asin(sin(angles[i]) * nB / nOx);
         angleSi = asin(sin(angleOx) * nOx / nSi);
         kOx = 2.0 * CUDART_PI * nOx / wavelength;
         pB = nB * cos(angles[i]);
         pOx = nOx * cos(angleOx);
         pSi = nSi * cos(angleSi);
         m11 = cos(kOx * dOx * cos(angleOx));
         m12 = -1.0 / pOx * sin(kOx * dOx * cos(angleOx));
         m21 = -pOx * sin(kOx * dOx * cos(angleOx));
         m22 = m11;
         num = std::complex<double>(m11 * pB - m22 * pSi, m12 * pSi * pB + m21);
         denom = std::complex<double>(m11 * pB + m22 * pSi, m12 * pSi * pB + m21);
         rTE = num / denom;
         _h_constvec[i * 3] = rTE.real();
         _h_constvec[i * 3 + 1] = rTE.imag();
         _h_constvec[i * 3 + 2] = 4 * CUDART_PI * nB * cos(angles[i]) / wavelength;
      }
      return 0;
   }

   int GPUModel::CalculateFunction(int grain, int emptyThreads)
   {
      cudaEvent_t stop, start;
      checkCuda(cudaEventCreateWithFlags(&start, cudaEventBlockingSync));
      checkCuda(cudaEventCreateWithFlags(&stop, cudaEventBlockingSync));
      checkCuda(cudaEventRecord(start));

      unsigned short *dataptr = _d_data + grain * _grainSize;

      EvaluateFunction<<<_blocksPerGrid, 512>>>(emptyThreads, _n, dataptr, _d_xvec, _d_fvec, _d_constvec);

      checkCuda(cudaEventRecord(stop));
      checkCuda(cudaEventSynchronize(stop));
      float milliseconds;
      checkCuda(cudaEventElapsedTime(&milliseconds, start, stop));
      //std::cout << "Function calculation took: " << milliseconds << " milliseconds.\n";

      return 0;
   }

   int GPUModel::CalculateJacobian(int grain, int emptyThreads)
   {
      cudaEvent_t stop, start;
      checkCuda(cudaEventCreateWithFlags(&stop, cudaEventBlockingSync));
      checkCuda(cudaEventCreateWithFlags(&start, cudaEventBlockingSync));
      checkCuda(cudaEventRecord(start));

      EvaluateJacobian<<<_blocksPerGrid, 512>>>(emptyThreads, _n, _d_xvec, _d_jvec, _d_constvec);

      checkCuda(cudaEventRecord(stop));
      checkCuda(cudaEventSynchronize(stop));
      float milliseconds;
      checkCuda(cudaEventElapsedTime(&milliseconds, start, stop));
      //std::cout << "Jacobian calculation took: " << milliseconds << " milliseconds.\n";

      return 0;
   }

   int GPUModel::RunFit(void)
   {
      std::chrono::high_resolution_clock::time_point earlier, later;
      std::chrono::duration<double> timeTaken;
      //Run the solver on each grain
      for (int i = 0; i < _ngrains; i++)
      {
         earlier = std::chrono::high_resolution_clock::now();
         for (int i = 0; i < _grainSize; i++)
         {
            _h_xvec[i * 3] = 200;
            _h_xvec[i * 3 + 1] = 100;
            _h_xvec[i * 3 + 2] = 6.5;
         }

         if (dtrnlsp_init(&_solverHandle, &_nVars, &_mPoints, _h_xvec, _eps, &_iterations, &_stepIterations, &_initialStep) != TR_SUCCESS)
         {
            std::cerr << "Error initializing solver" << std::endl;
            MKL_Free_Buffers();
            return 1;
         }
         if (dtrnlsp_check(&_solverHandle, &_nVars, &_mPoints, _h_jvec, _h_fvec, _eps, _fitInfo) != TR_SUCCESS)
         {
            std::cerr << "Error checking solver" << std::endl;
            MKL_Free_Buffers();
            return 2;
         }
         else
         {
            if (_fitInfo[0] != 0 ||
               _fitInfo[1] != 0 ||
               _fitInfo[2] != 0 ||
               _fitInfo[3] != 0)
            {
               std::cerr << "Invalid array passed to solver: " << std::endl;
               MKL_Free_Buffers();
               return 3;
            }
         }
         _successful = 0;
         _counter = 0;
         while (_successful == 0)
         {
            int emptyThreads;
            if (i == _ngrains - 1)
               emptyThreads = _emptyThreads;
            else
               emptyThreads = 0;
            if (dtrnlsp_solve(&_solverHandle, _h_fvec, _h_jvec, &_rciRequest) != TR_SUCCESS)
            {
               std::cerr << "Error solving solver" << std::endl;
               MKL_Free_Buffers();
               return 3;
            }
            if (_rciRequest == -1 ||
               _rciRequest == -2 ||
               _rciRequest == -3 ||
               _rciRequest == -4 ||
               _rciRequest == -5 ||
               _rciRequest == -6)
               _successful = 1;
            if (_rciRequest == 1)
               CalculateFunction(i, emptyThreads);
            if (_rciRequest == 2)
               CalculateJacobian(i, emptyThreads);
            //std::cout << "RCI cycle: " << _counter++ << std::endl;
         }
         if (dtrnlsp_get(&_solverHandle, &_actualIterations, &_stopCrit, &_initialRes, &_finalRes) != TR_SUCCESS)
         {
            std::cerr << "Error getting solver results" << std::endl;
            MKL_Free_Buffers();
            return 4;
         }
         if (dtrnlsp_delete(&_solverHandle) != TR_SUCCESS)
         {
            std::cerr << "Error deleting the solver" << std::endl;
            MKL_Free_Buffers();
            return 5;
         }

         MKL_Free_Buffers();
         double *aptr, *bptr, *hptr;
         aptr = _outputImgs[0].ptr<double>() + i * _grainSize;
         bptr = _outputImgs[1].ptr<double>() + i * _grainSize;
         hptr = _outputImgs[2].ptr<double>() + i * _grainSize;
         for (int j = 0; j < _grainSize; j++)
         {
            int xidx = j * 3;
            aptr[j] = _h_xvec[xidx];
            bptr[j] = _h_xvec[xidx + 1];
            hptr[j] = _h_xvec[xidx + 2];
         }
         later = std::chrono::high_resolution_clock::now();
         timeTaken = later - earlier;
         std::cout << "Grain " << i << " of " << _ngrains << " finished in " << std::chrono::duration_cast<std::chrono::milliseconds>(timeTaken).count() <<" milliseconds." << std::endl;
      }

      return 0;
   }
}