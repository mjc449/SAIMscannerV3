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

#include <stdio.h>
#include <iostream>
#include <vector>

#include <boost\filesystem.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgcodecs\imgcodecs.hpp>

#include "saim_model_cpu.h"

namespace fs = boost::filesystem;

int main(int argc, char **argv)
{
   if (argc < 2)
   {
      std::cerr << "Need to include a file name";
      return 1;
   }

   fs::path inputPath = argv[1];

   std::vector<cv::Mat> imstack;
   cv::imreadmulti(inputPath.string(), imstack, CV_LOAD_IMAGE_ANYDEPTH);

   int imHeight = imstack[0].rows;
   int imWidth = imstack[0].cols;
   int samples = imstack.size();
   int pixelCount = imHeight * imWidth;
   unsigned short *data;
   double *x, *fn, *jac;
   /*
   double *angles = new double[samples];
<<<<<<< HEAD
   double step = 45.0 / 31.0;
   double firstAngle = 0;

=======
   double step = 0.952380952;
   double firstAngle = step;
   */

   double linangles[31] = { -39.911025,
-37.25029,
-34.589555,
-31.92882,
-29.268085,
-26.60735,
-23.946615,
-21.28588,
-18.625145,
-15.96441,
-13.303675,
-10.64294,
-7.982205,
-5.32147,
-2.660735,
0.0,
2.660735,
5.32147,
7.982205,
10.64294,
13.303675,
15.96441,
18.625145,
21.28588,
23.946615,
26.60735,
29.268085,
31.92882,
34.589555,
37.25029,
39.911025,
   };
   
   double angles[31] =
   { -43.81494106,
   - 39.98737856,
   - 36.28932555,
   - 32.72078203,
   - 29.281748,
   - 25.97222346,
   - 22.79220841,
   - 19.74170285,
   - 16.82070678,
   - 14.02922019,
   - 11.3672431,
   - 8.834775503,
   - 6.431817392,
   - 4.158368772,
   - 2.014429641,
   0.0,
   2.014429641,
   4.158368772,
   6.431817392,
   8.834775503,
   11.3672431,
   14.02922019,
   16.82070678,
   19.74170285,
   22.79220841,
   25.97222346,
   29.281748,
   32.72078203,
   36.28932555,
   39.98737856,
   43.81494106 };

   for (int i = 0; i < 31; i++)
      linangles[i] = linangles[i] * 2.0 * CV_PI / 360.0;

   /*
>>>>>>> 3519e08061486c7d7904e57dd88568bdbaa23dfd
   double start = firstAngle * 2.0 * CV_PI / 360.0;
   for (int i = 0; i < samples; i++)
      angles[i] = start + i * step * 2.0 * CV_PI / 360.0;
   */

   cpu_model::CPUModel model;

   model.RegisterImages(imstack);
   model.SetGrainSize(1);
   model.InitializeBuffers();
   model.CalculateConstants(560.0, 1910.5, 1.34, 1.463, 4.3638, linangles);
   model.ParforRunFit();
   //model.RunFit();
   std::vector<cv::Mat> outputs = model.GetImages();
   model.ReleaseBuffers();

   //Fit A
   outputs[0].convertTo(outputs[0], CV_16UC1);
   //Fit B
   outputs[1].convertTo(outputs[1], CV_16UC1);
   //Multiply H by 100 to get 2 decimals
   outputs[2] *= 100;
   outputs[2].convertTo(outputs[2], CV_16UC1);
   //Fit stop criteria
   outputs[3].convertTo(outputs[3], CV_16UC1);
   //Multiply R2 by 1000 to get 3 decimals
   outputs[4] *= 1000;
   outputs[4].convertTo(outputs[4], CV_16UC1);
   //Multiply d by 1000 to get 3 decimals
   outputs[5] *= 1000;
   outputs[5].convertTo(outputs[5], CV_16UC1);
   //Multiply S/N by 100 to get 2 decimals
   outputs[6] *= 100;
   outputs[6].convertTo(outputs[6], CV_16UC1);

   fs::path outputPath = inputPath.parent_path() /= inputPath.stem();
   cv::imwrite(outputPath.string() + "_A.png", outputs[0]);
   cv::imwrite(outputPath.string() + "_B.png", outputs[1]);
   cv::imwrite(outputPath.string() + "_H.png", outputs[2]);
   cv::imwrite(outputPath.string() + "_stopCrit.png", outputs[3]);
   cv::imwrite(outputPath.string() + "_R2.png", outputs[4]);
   cv::imwrite(outputPath.string() + "_d.png", outputs[5]);
   cv::imwrite(outputPath.string() + "_snr.png", outputs[6]);

   //delete[] angles;
   return 0;
}