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

#ifndef SAIM_MODEL_CPU_H
#define SAIM_MODEL_CPU_H

#include <vector>
#include <tbb/tbb.h>
#include <mkl.h>

namespace cv
{
   class Mat;
};

namespace cpu_model
{

   class CPUModel
   {
   public:
      CPUModel();
      ~CPUModel();

      int RegisterImages(std::vector<cv::Mat> &input);

      int SetGrainSize(int);

      int InitializeBuffers();

      int ReleaseBuffers();

      int CalculateConstants(double wavelength, double dOx, double nB, double nOx, double nSi, double *angles);

      int RunFit(void);

      int ParforRunFit(void);
      
      int ThreadedRunFit(void);

      std::vector<cv::Mat> GetImages(void);

      struct FitTask
      {
      public:
         FitTask(CPUModel *, int, int, int);
         ~FitTask();
         volatile void operator()(const tbb::blocked_range<int> &index) const;
         void operator()(int);

         extern friend void objective(MKL_INT *n, MKL_INT *m, double *, double *, void *);

      private:
         CPUModel *l_parent;
         int l_count;
         double *l_xvec, *l_fvec, *l_jvec;
         MKL_INT l_nVars{ 3 };
         MKL_INT l_nPoints{ 0 };
         MKL_INT l_iterations{ 1000 };
         MKL_INT l_stepIterations{ 100 };
         double l_initialStep{ 00.0 };
         MKL_INT l_rciRequest{ 0 };
         MKL_INT l_successful{ 0 };
         MKL_INT l_actualIterations{ 0 };
         MKL_INT l_stopCrit{ 0 };
         double l_initialRes{ 0 }, l_finalRes{ 0 };
         MKL_INT l_counter{ 0 };
         MKL_INT l_fitInfo[6];
         double l_eps[6] = { 0.000000001, 0.000000001, 0.000000001, 0.000000001, 0.000000001, 0.000000001 };
         double l_jeps{ 0.000000001 };
      };

      /*************************************************************************
      * @brief Calculates the function value at the current xvec
      *************************************************************************/
      int CalculateFunction(int, double *, double *);

      /*************************************************************************
      * @brief Calculates the Jacobian value at the current xvec
      *************************************************************************/
      int CalculateJacobian(int, double *, double *);

   private:
      std::vector<cv::Mat> _rawImgs, _outputImgs;
      volatile int _n;
      volatile int _m;
      int _grainSize, _nGrains, _emptyPixels;
      bool _initialized;
      size_t _datasz, _fnsz, _xsz, _jacsz;
      unsigned short  *_data;
      double *_constvec;
      double _guesses[3]{ 0.8, 1.0, 6.0 };
   };
}

#endif //SAIM_MODEL_CPU_H