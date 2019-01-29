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

#include "saim_model_cpu.h"

#include <chrono>
#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>

namespace cpu_model
{
   CPUModel::CPUModel() {};
   CPUModel::~CPUModel() {};

   int CPUModel::RegisterImages(std::vector<cv::Mat> &imStack)
   {
      _rawImgs = imStack;
      _outputImgs.clear();
      for (int i = 0; i < 7; i++)
      {
         _outputImgs.push_back(cv::Mat(_rawImgs[0].rows, _rawImgs[0].cols, CV_32F));
      }
      return 0;
   }

   int CPUModel::SetGrainSize(int grain)
   {
      _grainSize = grain;
      return 0;
   }

   int CPUModel::InitializeBuffers()
   {
      _m = _rawImgs[0].rows * _rawImgs[0].cols;
      _n = _rawImgs.size();
      _nGrains = _m % _grainSize == 0 ? _m / _grainSize : _m / _grainSize + 1;
      _emptyPixels = _m % _grainSize == 0 ? 0 : _grainSize - (_m % _grainSize);
      _datasz = _m * _n;
      _fnsz = _n;
      _jacsz = _n * 3;
      _data = (unsigned short *)MKL_malloc(_datasz * sizeof(unsigned short), 64);
      if (_data == nullptr)
      {
         _initialized = false;
         return 1;
      }
      _constvec = (double *)MKL_malloc(_n * 3 * sizeof(double), 64);
      if (_constvec == nullptr)
      {
         mkl_free(_data);
         _initialized = false;
         return 1;
      }
      for (int i = 0; i < _rawImgs.size(); i++)
      {
         unsigned short *ptr = _rawImgs[i].ptr<unsigned short>();
         for (int j = 0; j < _m; j++)
         {
            _data[j * _n + i] = *ptr++;
         }
      }
      _initialized = true;
      return 0;
   }

   int CPUModel::ReleaseBuffers(void)
   {
      if (_data != nullptr)
      {
         mkl_free(_data);
         _data = nullptr;
      }
      if (_constvec != nullptr)
      {
         mkl_free(_constvec);
         _constvec = nullptr;
      }
      _initialized = false;
      return 0;
   }

   int CPUModel::CalculateConstants(double wavelength, double dOx, double nB, double nOx, double nSi, double *angles)
   {
      double angleOx, angleSi, pB, pOx, pSi, kOx, m11, m12, m21, m22;
      std::complex<double> num, denom, rTE;
      for (int i = 0; i < _n; i++)
      {
         angleOx = asin(sin(angles[i]) * nB / nOx);
         angleSi = asin(sin(angleOx) * nOx / nSi);
         kOx = 2.0 * CV_PI * nOx / wavelength;
         pB = nB * cos(angles[i]);
         pOx = nOx * cos(angleOx);
         pSi = nSi * cos(angleSi);
         m11 = cos(kOx * dOx * cos(angleOx));
         m12 = -1.0 / pOx * sin(kOx * dOx * cos(angleOx));
         m21 = -pOx * sin(kOx * dOx * cos(angleOx));
         m22 = m11;
         num = std::complex<double>(m11 * pB - m22 * pSi, m12 * pSi * pB - m21);
         denom = std::complex<double>(m11 * pB + m22 * pSi, m12 * pSi * pB + m21);
         rTE = num / denom;
         _constvec[i * 3] = rTE.real();
         _constvec[i * 3 + 1] = rTE.imag();
         _constvec[i * 3 + 2] = 4 * CV_PI * nB * cos(angles[i]) / wavelength;
      }
      return 0;
   }

   int CPUModel::RunFit(void)
   {
      FitTask task(this, _n, 0, 1);
      for (int i = 0; i < _m; i++)
         task(i);
      return 0;
   }

   int CPUModel::ParforRunFit(void)
   {
      std::chrono::high_resolution_clock::time_point earlier, later;
      std::chrono::duration<double> timeTaken;
      earlier = std::chrono::high_resolution_clock::now();
      FitTask task(this, _n, 0, 1);
      tbb::parallel_for(tbb::blocked_range<int>(0, _m), FitTask(this, _n, 0, 1));
      later = std::chrono::high_resolution_clock::now();
      timeTaken = later - earlier;
      std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(timeTaken).count() << std::endl;
      return 0;
   }

   int CPUModel::ThreadedRunFit(void)
   {
      return 0;
   }
   
   int CPUModel::CalculateFunction(int pixel, double *xvec, double *fvec)
   {
      double A{ xvec[0] }, B{ xvec[1] }, H{ xvec[2] };
      //double *dataVec = new double[_n];
      //double *funVec = new double[_n];
      for (int i = 0; i < _n; i++)
      {
         double c{ _constvec[3 * i] }, d{ _constvec[3 * i + 1] }, phi{ _constvec[3 * i + 2] };
         double value = A * (1.0 + 2.0 * c * cos(phi * H) - 2.0 * d * sin(phi * H) + c * c + d * d) + B;
         fvec[i] = (double)_data[_n * pixel + i] - value;
         //dataVec[i] = (double)_data[_n * pixel + i];
         //funVec[i] = value;
      }
      //delete[] dataVec;
      //delete[] funVec;
      return 0;
   }

   int CPUModel::CalculateJacobian(int pixel, double *xvec, double *jvec)
   {
      double A{ xvec[0] }, H{ xvec[2] };
      for (int i = 0; i < _n; i++)
      {
         double c{ _constvec[3 * i] }, d{ _constvec[3 * i + 1] }, phi{ _constvec[3 * i + 2] };
         jvec[i] = -1.0 * (1.0 + 2.0 * c * cos(phi * H) - 2.0 * d * sin(phi * H) + c * c + d * d);
         jvec[i + _n] = -1.0;
         jvec[i + 2 * _n] = 2.0 * A * phi * (c * sin(phi * H) + d * cos(phi * H));
      }

      return 0;
   }

   std::vector<cv::Mat> CPUModel::GetImages(void)
   {
      return _outputImgs;
   }

   CPUModel::FitTask::FitTask(CPUModel *parent, int frames, int startIdx, int count) : l_parent(parent), l_nPoints(frames), l_count(count) {}

   CPUModel::FitTask::~FitTask() {}

   void CPUModel::FitTask::operator()(int index)
   {
      l_xvec = (double *)mkl_malloc(3 * sizeof(double), 64);
      if (l_xvec == nullptr)
         return;
      l_fvec = (double *)mkl_malloc(l_parent->_fnsz * sizeof(double), 64);
      if (l_fvec == nullptr)
      {
         mkl_free(l_xvec);
         return;
      }
      l_jvec = (double *)mkl_malloc(l_parent->_jacsz * sizeof(double), 64);
      if (l_jvec == nullptr)
      {
         mkl_free(l_xvec);
         mkl_free(l_fvec);
         return;
      }
      std::chrono::high_resolution_clock::time_point earlier, later;
      std::chrono::duration<double> timeTaken;
      for (int i = 0; i < l_count; i++)
      {
         earlier = std::chrono::high_resolution_clock::now();

         l_successful = 0;
         l_xvec[0] = l_parent->_guesses[0];
         l_xvec[1] = l_parent->_guesses[1];
         l_xvec[2] = l_parent->_guesses[2];
         int pixel = index + i;

         _TRNSP_HANDLE_t solverHandle;

         if (dtrnlsp_init(&solverHandle, &l_nVars, &l_nPoints, l_xvec, l_eps, &l_iterations, &l_stepIterations, &l_initialStep) != TR_SUCCESS)
         {
            std::cerr << "Error initializing solver" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }
         if (dtrnlsp_check(&solverHandle, &l_nVars, &l_nPoints, l_jvec, l_fvec, l_eps, l_fitInfo) != TR_SUCCESS)
         {
            std::cerr << "Error checking solver" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }
         else
         {
            if (l_fitInfo[0] != 0 ||
               l_fitInfo[1] != 0 ||
               l_fitInfo[2] != 0 ||
               l_fitInfo[3] != 0)
            {
               std::cerr << "Invalid array passed to solver: " << std::endl;
               MKL_Thread_Free_Buffers();
               return;
            }
         }
         l_successful = 0;
         l_counter = 0;
         while (l_successful == 0)
         {
            if (dtrnlsp_solve(&solverHandle, l_fvec, l_jvec, &l_rciRequest) != TR_SUCCESS)
            {
               std::cerr << "Error solving solver" << std::endl;
               MKL_Thread_Free_Buffers();
               return;
            }
            if (l_rciRequest == -1 ||
               l_rciRequest == -2 ||
               l_rciRequest == -3 ||
               l_rciRequest == -4 ||
               l_rciRequest == -5 ||
               l_rciRequest == -6)
               l_successful = 1;
            if (l_rciRequest == 1)
            {
               l_parent->CalculateFunction(pixel, l_xvec, l_fvec);
            }
            if (l_rciRequest == 2)
               l_parent->CalculateJacobian(pixel, l_xvec, l_jvec);
            //std::cout << "RCI cycle: " << _counter++ << std::endl;
            l_counter++;
         }
         if (dtrnlsp_get(&solverHandle, &l_actualIterations, &l_stopCrit, &l_initialRes, &l_finalRes) != TR_SUCCESS)
         {
            std::cerr << "Error getting solver results" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }
         if (dtrnlsp_delete(&solverHandle) != TR_SUCCESS)
         {
            std::cerr << "Error deleting the solver" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }

         *(l_parent->_outputImgs[0].ptr<double>() + pixel) = l_xvec[0];
         *(l_parent->_outputImgs[1].ptr<double>() + pixel) = l_xvec[1];
         *(l_parent->_outputImgs[2].ptr<double>() + pixel) = l_xvec[2];
         *(l_parent->_outputImgs[3].ptr<double>() + pixel) = l_stopCrit;
         *(l_parent->_outputImgs[4].ptr<double>() + pixel) = l_finalRes;

         later = std::chrono::high_resolution_clock::now();
         timeTaken = later - earlier;
         std::cout << "Pixel " << pixel << " finished " << l_counter << " cycles in " << std::chrono::duration_cast<std::chrono::microseconds>(timeTaken).count() << " microseconds." << std::endl;
      }
      mkl_free(l_xvec);
      mkl_free(l_fvec);
      mkl_free(l_jvec);
      MKL_Thread_Free_Buffers();
      l_xvec = l_fvec = l_jvec = nullptr;
   }

   volatile void CPUModel::FitTask::operator()(const tbb::blocked_range<int> &index) const
   {
      double *xvec = (double *)mkl_malloc(3 * sizeof(double), 64);
      if (l_xvec == nullptr)
         return;
      double *fvec = (double *)mkl_malloc(l_parent->_fnsz * sizeof(double), 64);
      if (l_fvec == nullptr)
      {
         mkl_free(xvec);
         return;
      }
      double *jvec = (double *)mkl_malloc(l_parent->_jacsz * sizeof(double), 64);
      if (l_jvec == nullptr)
      {
         mkl_free(xvec);
         mkl_free(fvec);
         return;
      }
      int fitInfo[6]{ 0, 0, 0, 0, 0, 0 };
      std::chrono::high_resolution_clock::time_point earlier, later;
      std::chrono::duration<double> timeTaken;
      for (size_t i = index.begin(); i != index.end(); i++)
      {
         if (l_parent->_data[i * l_nPoints] == 0)
            continue;
         earlier = std::chrono::high_resolution_clock::now();
         double maxval = (double)l_parent->_data[i * l_nPoints];
         double minval{ maxval };
         for (int j = 0; j < l_nPoints; j++)
         {
            fvec[j] = 0;
            jvec[j] = 0;
            jvec[j + l_nPoints] = 0;
            jvec[j + l_nPoints * 2] = 0;
            double thisval = (double)l_parent->_data[i * l_nPoints + j];
            maxval = maxval < thisval ? thisval : maxval;
            minval = minval > thisval ? thisval : minval;
         }

         int successful = 0;
         xvec[0] = l_parent->_guesses[0] * (maxval - minval);
         xvec[1] = l_parent->_guesses[1] * minval;
         xvec[2] = l_parent->_guesses[2];
         int pixel = i;

         _TRNSP_HANDLE_t solverHandle;

         if (dtrnlsp_init(&solverHandle, &l_nVars, &l_nPoints, xvec, l_eps, &l_iterations, &l_stepIterations, &l_initialStep) != TR_SUCCESS)
         {
            std::cerr << "Error initializing solver" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }
         if (dtrnlsp_check(&solverHandle, &l_nVars, &l_nPoints, jvec, fvec, l_eps, fitInfo) != TR_SUCCESS)
         {
            std::cerr << "Error checking solver" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }
         else
         {
            if (fitInfo[0] != 0 ||
               fitInfo[1] != 0 ||
               fitInfo[2] != 0 ||
               fitInfo[3] != 0)
            {
               std::cerr << "Invalid array passed to solver: " << fitInfo[0] << fitInfo[1] << fitInfo[2] << fitInfo[3] << std::endl;
               MKL_Thread_Free_Buffers();
               return;
            }
         }
         successful = 0;
         int counter = 0;
         int rciRequest = 0;
         while (successful == 0)
         {
            if (dtrnlsp_solve(&solverHandle, fvec, jvec, &rciRequest) != TR_SUCCESS)
            {
               std::cerr << "Error solving solver" << std::endl;
               MKL_Thread_Free_Buffers();
               return;
            }
            if (rciRequest == -1 ||
               rciRequest == -2 ||
               rciRequest == -3 ||
               rciRequest == -4 ||
               rciRequest == -5 ||
               rciRequest == -6)
               successful = 1;
            if (rciRequest == 1)
               l_parent->CalculateFunction(pixel, xvec, fvec);
            if (rciRequest == 2)
               l_parent->CalculateJacobian(pixel, xvec, jvec);
            //std::cout << "RCI cycle: " << _counter++ << std::endl;
         }
         MKL_INT actualIterations{ 0 };
         MKL_INT stopCrit{ 0 };
         double initialRes{ 0 }, finalRes{ 0 };
         if (dtrnlsp_get(&solverHandle, &actualIterations, &stopCrit, &initialRes, &finalRes) != TR_SUCCESS)
         {
            std::cerr << "Error getting solver results" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }
         if (dtrnlsp_delete(&solverHandle) != TR_SUCCESS)
         {
            std::cerr << "Error deleting the solver" << std::endl;
            MKL_Thread_Free_Buffers();
            return;
         }

         double res{ 0.0 }, avg{ 0.0 }, sst{ 0.0 }, ssr{ 0.0 }, ssc{ 0.0 };

         for (int j = 0; j < l_nPoints; j++)
         {
            avg += l_parent->_data[pixel * l_nPoints + j];
            ssr += fvec[j] * fvec[j];
         }
         avg /= l_nPoints;
         for (int j = 0; j < l_nPoints; j++)
         {
            double dataval = l_parent->_data[pixel * l_nPoints + j];
            dataval -= avg;
            sst += dataval * dataval;
         }
         for (int j = 1; j < l_nPoints; j++)
         {
            double scval = fvec[j] - fvec[j - 1];
            ssc += scval * scval;
         }

         double rmsNoise{ 0.0 }, rmsSignal{ 0.0 }, snr{ 0.0 };
         double *prediction = new double[l_nPoints];

         for (int j = 0; j < l_nPoints; j++)
         {
            double c{ l_parent->_constvec[3 * j] }, d{ l_parent->_constvec[3 * j + 1] }, phi{ l_parent->_constvec[3 * j + 2] };
            prediction[j] = xvec[0] * (1.0 + 2.0 * c * cos(phi * xvec[2]) - 2.0 * d * sin(phi * xvec[2]) + c * c + d * d);
            rmsSignal += prediction[j] * prediction[j];
         }
         
         double d, r;
         d = ssc / ssr;
         r = 1 - ssr / sst;
         rmsNoise = sqrt(ssr / l_nPoints);
         rmsSignal = sqrt(rmsSignal / l_nPoints);
         snr = (rmsSignal * rmsSignal) / (rmsNoise * rmsNoise);

         *(l_parent->_outputImgs[0].ptr<float>() + pixel) = (float)xvec[0];
         *(l_parent->_outputImgs[1].ptr<float>() + pixel) = (float)xvec[1];
         *(l_parent->_outputImgs[2].ptr<float>() + pixel) = (float)xvec[2];
         *(l_parent->_outputImgs[3].ptr<float>() + pixel) = (float)stopCrit;
         *(l_parent->_outputImgs[4].ptr<float>() + pixel) = (float)r;
         *(l_parent->_outputImgs[5].ptr<float>() + pixel) = (float)d;
         *(l_parent->_outputImgs[6].ptr<float>() + pixel) = (float)snr;

         later = std::chrono::high_resolution_clock::now();
         timeTaken = later - earlier;
         delete[] prediction;
         //std::cout << "Pixel " << pixel << " finished in " << std::chrono::duration_cast<std::chrono::microseconds>(timeTaken).count() << " microseconds." << std::endl;
      }
      mkl_free(xvec);
      mkl_free(fvec);
      mkl_free(jvec);
      MKL_Thread_Free_Buffers();
      xvec = fvec = jvec = nullptr;
   }

   void objective(MKL_INT *pixel, MKL_INT *m, double *x, double *f, void *instance)
   {
      CPUModel::FitTask *task = (CPUModel::FitTask *)instance;
      task->l_parent->CalculateFunction(*pixel, task->l_xvec, task->l_fvec);
      f = task->l_fvec;
   }
}