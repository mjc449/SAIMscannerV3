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
#include <stdio.h>
#include <iostream>
#include <vector>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <mkl.h>

namespace gpumodel = saim_model_gpu;


int main(int argc, char **argv)
{
   if (argc < 2)
   {
      std::cout << "Need to include a file name";
      return 1;
   }

   std::vector<cv::Mat> imstack;
   cv::imreadmulti(argv[1], imstack, CV_LOAD_IMAGE_ANYDEPTH);
   cv::namedWindow("first frame", cv::WINDOW_KEEPRATIO);
   cv::imshow("first frame", imstack[0]);

   int imHeight = imstack[0].rows;
   int imWidth = imstack[0].cols;
   size_t samples = imstack.size();
   int pixelCount = imHeight * imWidth;
   unsigned short *data{ nullptr };
   double *x{ nullptr }, *fn{ nullptr }, *jac{ nullptr };

   double *angles = new double[samples];
   double step = 1.613724;
   for (int i = 0; i < samples; i++)
      angles[i] = (i * step) * 2 * CV_PI / 360;

   gpumodel::GPUModel model;
   model.RegisterImages(imstack);
   model.SetGrainSize(128);
   model.InitializeBuffers();
   model.CalculateConstants(560.0, 1915.167, 1.34, 1.463, 4.3638, angles);
   model.RunFit();
   model.ReleaseBuffers();

   delete[] angles;

   return 0;
}