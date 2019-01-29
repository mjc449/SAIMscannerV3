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

#ifndef SAIM_MODEL_GPU_H
#define SAIM_MODEL_GPU_H

#include <stdio.h>
#include <mkl.h>
#include <vector>
#include <opencv2/core/core.hpp>


namespace saim_model_gpu
{
   class GPUModel
   {
   public:
      GPUModel();
      ~GPUModel();

      /*************************************************************************
      * @brief Registers the raw data and output images with the fitter
      *************************************************************************/
      int RegisterImages(std::vector<cv::Mat> &input);

      /*************************************************************************
      * @brief Set the size of the fit grain in pixels
      *************************************************************************/
      int SetGrainSize(int);

      /*************************************************************************
      * @brief Allocates and initializes the buffers
      *************************************************************************/
      int InitializeBuffers();

      /*************************************************************************
      * @brief Frees all the buffers and cleans up
      *************************************************************************/
      int ReleaseBuffers(void);

      /*************************************************************************
      * @brief Calculates the fit constants (rTE and phi)
      *************************************************************************/
      int CalculateConstants(double wavelength, double dOx, double nb, double nox, double nsi, double *angles);

      /*************************************************************************
      * @brief Runs the fitting algorithm
      *************************************************************************/
      int RunFit(void);

   private:
      std::vector<cv::Mat> _rawImgs, _outputImgs, _residualImgs;
      int _m, _n, _blocksPerGrid, _emptyThreads, _emptyPixels, _ngrains, _grainSize{ 256 };
      bool _initialized;
      size_t _datasz, _fnsz, _xsz, _jacsz;
      unsigned short *_h_data, *_d_data;
      double *_h_xvec, *_d_xvec, *_h_fvec, *_d_fvec, *_h_jvec, *_d_jvec, *_h_constvec, *_d_constvec;
      _TRNSP_HANDLE_t _solverHandle;
      MKL_INT _nVars{ 0 };
      MKL_INT _mPoints{ 0 };
      MKL_INT _iterations{ 1000 };
      MKL_INT _stepIterations{ 100 };
      double _initialStep{ 0.0 };
      MKL_INT _rciRequest{ 0 };
      MKL_INT _successful{ 0 };
      MKL_INT _actualIterations{ 0 };
      MKL_INT _stopCrit{ 0 };
      double _initialRes{ 0 }, _finalRes{ 0 };
      MKL_INT _counter{ 0 };
      MKL_INT _fitInfo[6];
      double _eps[6] = { 0.00001, 0.00001, 0.00001, 0.00001, 0.00001, 0.00001 };

      /*************************************************************************
      * @brief Calculates the function value at the current xvec
      *************************************************************************/
      int CalculateFunction(int, int);

      /*************************************************************************
      * @brief Calculates the Jacobian value at the current xvec
      *************************************************************************/
      int CalculateJacobian(int, int);

   };
}



#endif //SAIM_MODEL_H