////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SAIMScannerV3 - an open-source microscope controller providing an         //
//                  embedded solution for hardware synchronization.           //
//                  This library provides a compiler independent interface    //
//                  with the controller hardware on Windows systems.          //
//                  Many functions are Scanning Angle Interference Microscopy //
//                  (SAIM) specific, however the hardware and underlying      //
//                  functionality is designed to be versatile and applicable  //
//                  in a variety of applications.                             //
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
////////////////////////////////////////////////////////////////////////////////


#include "SAIMScannerV3.h"
#include "hidapi\hidapi.h"
#include <iostream>
#include <vector>
#include <sstream>

#define SendAndListen(err, nCheck, retry) {Transmit((err), (nCheck), (retry), __FUNCTION__);}

#define OK_ SSV3ERROR::SSV3ERROR_OK

namespace SSV3
{

   struct Node
   {
      int _sequence;
      int _exSetting;
      unsigned char number;
   };

   const unsigned char _maxNumExcitationProfiles{ 32 };
   const unsigned char _maxNumAngleSequences{ 32 };
   const unsigned char _maxNumExperiments{ 32 };

   class ScanCard : public Controller
   {
   public:
      ScanCard()
      {
         _dev = hid_open(1240, 61722, NULL);
      }

      ScanCard(wchar_t *sn)
      {
         _dev = hid_open(1240, 61722, sn);
      }

      ScanCard(bool demo)
      {
         _demo = true;
      }

      ~ScanCard(){}

      /////////////////////////////////////////////////////////////////////////
      /*  API functions                                                      */
      /////////////////////////////////////////////////////////////////////////

      void Destroy()
      {
         //Make sure to stop any experiments, center the galvos
         // and close the shutter
         SSV3ERROR ret = OK_;
         _oBuffer[0] = 0x8F;
         SendAndListen(&ret, 1, true);
         _oBuffer[0] = 0x1F;
         SendAndListen(&ret, 1, true);
         _oBuffer[0] = 0x45;
         SendAndListen(& ret, 1, true);
         if(!_demo)
            hid_close(_dev);
         delete[] _errStr;
         delete this;
      }

      const wchar_t * HidError() { return hid_error(_dev); }

      SSV3ERROR Initialize()
      {
         SSV3ERROR ret{ OK_ };
         if (!_demo)
         {
            //Set the read attempts to a reasonable large value in case the
            //  controller is a little slow
            _readAttempts = 100;
            ret = Visor();
            _readAttempts = 2;
            if (_detailedReporting)
            {
               _errMsg.str("");
               wchar_t string[64];
               std::wstring str;
               if (hid_get_manufacturer_string(_dev, string, 255) != 0)
               {
                  _errMsg << "No response to manufacturer inquiry\n";
                  _newErr = true;
               }
               if (hid_get_product_string(_dev, string, 255) != 0)
               {
                  _errMsg << "No response to product inquiry\n";
                  _newErr = true;
               }
            }
         }
         for (int i = 0; i < 8; i++)
            _currentExcitation.push_back(0);
         for (int i = 0; i < 32; i++)
         {
            _illuminationProfiles.push_back(std::vector<unsigned short>());
            _angleSequences.push_back(std::vector<unsigned short>());
         }
         return ret;
      }

      void Timeout(unsigned int ms) { _timeout = ms; }

      void ReadRetries(unsigned int attempts) { _readAttempts = attempts; }

      SSV3ERROR Visor()
      {
         SSV3ERROR ret{ OK_ };
         _oBuffer[0] = 0x01;
         SendAndListen(&ret, 1, true);
         return ret;
      }

      SSV3ERROR CenterPark()
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;
         _oBuffer[0] = 0x1F;
         SendAndListen(&ret, 1, true);
         if (ret != OK_)
            return ret;
         _oBuffer[0] = 0x45;
         SendAndListen(&ret, 1, true);
         return ret;
      }

      SSV3ERROR Shutter(bool state)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;
         _oBuffer[0] = state ? 0x44 : 0x45;
         SendAndListen(&ret, 1, true);
         return ret;
      }

      SSV3ERROR AOTFBlank(bool state)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;
         _oBuffer[0] = state ? 0x40 : 0x41;
         SendAndListen(&ret, 1, true);
         return ret;
      }

      SSV3ERROR SingleLaserPower(unsigned char line, unsigned short value)
      {
         //Load the output buffer
         _oBuffer[0] = 0x42;
         _oBuffer[1] = line;
         _oBuffer[2] = HByte(value);
         _oBuffer[3] = LByte(value);
         SSV3ERROR ret{ OK_ };
         SendAndListen(&ret, 4, true);
         if (ret != OK_)
            return ret;
         //Update the current excitation values in case these will be assigned a profile
         _currentExcitation[line] = value;
         return ret;
      }

      SSV3ERROR MakeExcitationProfile(const unsigned char profile, unsigned short *values = nullptr)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         if (profile > 31)
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_OUT_OF_RANGE;
         _illuminationProfiles.at(profile).clear();

         unsigned short power;
         unsigned char *msg = _oBuffer;
         *msg++ = 0x4c;
         *msg++ = profile;
         for (int i = 0; i < 8; i++)
         {
            if (values == nullptr)
               power = 0;
            else
               power = values[i];
            _illuminationProfiles[profile].push_back(power);
            *msg++ = HByte(power);
            *msg++ = LByte(power);
         }
         SendAndListen(&ret, 0, true);
         if (_iBuffer[0] == 1)
            return SSV3ERROR::SSV3ERROR_ALLOC_FAIL;
         _experimentModified = true;
         return ret;
      }

      SSV3ERROR SetProfilePower(const unsigned char profile, const unsigned char line, const unsigned short value)
      {
         if (profile > 31 || _illuminationProfiles.at(profile).empty())
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST;
         unsigned short newVal = value;

         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         //Check that the requested value is valid and adjust if needed
         newVal = newVal > 0x03ff ? 0x03ff : newVal;
         //Update the line value
         _illuminationProfiles.at(profile).at(line) = newVal;

         //Put the new values in the output buffer
         unsigned char *msg = _oBuffer;
         *msg++ = 0x4c;
         *msg++ = profile;
         //Load the entire value array into the output buffer
         for (int i = 0; i < 8; i++)
         {
            *msg++ = HByte(_illuminationProfiles[profile][i]);
            *msg++ = LByte(_illuminationProfiles[profile][i]);
            _currentExcitation.at(i) = _illuminationProfiles[profile][i];
         }
         SendAndListen(&ret, 0, true);
         if (ret != OK_)
            return ret;
         if (_iBuffer[0] == 1)
            return SSV3ERROR::SSV3ERROR_ALLOC_FAIL;
         for (int i = 1; i < 18; i++)
            if (_iBuffer[i] != _oBuffer[i])
               return SSV3ERROR::SSV3ERROR_UNEXPECTED_RETURN;

         msg = _oBuffer;
         *msg++ = 0x43;
         *msg = profile;
         SendAndListen(&ret, 1, true);
         if (ret != OK_)
            return ret;
         _usingExcitation = profile;

         _experimentModified = true;
         return ret;
      }

      SSV3ERROR LoadExcitationProfile(const unsigned char profile)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         if (profile > 31 || _illuminationProfiles[profile].empty())
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST;

         _oBuffer[0] = 0x43;
         _oBuffer[1] = profile;
         SendAndListen(&ret, 1, true);
         if (ret != OK_)
            return ret;
         if (_iBuffer[2] == 0x01)
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST;
         else if (_iBuffer[2] == 0x02)
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_OUT_OF_RANGE;
         _usingExcitation = profile;

         for (int i = 0; i < 8; i++)
            _currentExcitation.at(i) = _illuminationProfiles.at(profile).at(i);
         return ret;
      }

      SSV3ERROR MakeProfileFromCurrentExcitation(const unsigned char profile)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         //Max number of profiles exists
         if (profile > 31)
            return SSV3ERROR::SSV3ERROR_EXCITATION_FULL;
         else
            _illuminationProfiles.at(profile).clear();

         unsigned char *msg = _oBuffer;
         *msg++ = 0x4c;
         *msg++ = profile;
         for (int i = 0; i < 8; i++)
         {
            _illuminationProfiles.at(profile).push_back(_currentExcitation[i]);
            *msg++ = HByte(_currentExcitation[i]);
            *msg++ = LByte(_currentExcitation[i]);
         }
         SendAndListen(&ret, 1, true);
         if (ret != OK_)
            return ret;
         if (_iBuffer[0] == 1)
            return SSV3ERROR::SSV3ERROR_ALLOC_FAIL;

         _usingExcitation = profile;
         _experimentModified = true;
         return ret;
      }

      SSV3ERROR Fire(bool state)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         _oBuffer[0] = state ? 0x50 : 0x51;
         SendAndListen(&ret, 1, true);
         return ret;
      }

      SSV3ERROR ClearExcitation()
      {
         SSV3ERROR ret{ OK_ };
         //Loop through all 8 lines and set to 0;
         for (unsigned char i = 0; i < 8; i++)
         {
            unsigned char* msg = _oBuffer;
            *msg++ = 0x42;
            *msg++ = i;
            *msg++ = 0x00;
            *msg++ = 0x00;
            _currentExcitation[i] = 0;
            SendAndListen(&ret, 4, true);
            if (ret != OK_)
               return ret;
         }
         return ret;
      }

      SSV3ERROR AdjustPhase(unsigned short phase)
      {
         SSV3ERROR ret{ OK_ };
         unsigned char *msg = _oBuffer;
         *msg++ = 0x22;
         *msg++ = HByte(phase);
         *msg++ = LByte(phase);
         SendAndListen(&ret, 3, true);
         return ret;
      }

      SSV3ERROR AdjustFrequency(unsigned short frequency)
      {
         SSV3ERROR ret{ OK_ };
         unsigned short value = frequency > 0x2e23 ? 0x2e23 : frequency;
         unsigned char *msg = _oBuffer;
         *msg++ = 0x20;
         *msg++ = HByte(value);
         *msg++ = LByte(value);
         SendAndListen(&ret, 3, true);
         return ret;
      }

      SSV3ERROR YAmpCorrection(unsigned short value)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;
         //Just calculate the signed offset value and update radius
         _yOffset = (float)value / (float)0x7fff;
         //Send x-radius, y offset will be applied in ScanRadius()
         return SetRadius(_currentRadius);
      }

      SSV3ERROR ScanRadius(unsigned short radius)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;
         unsigned short value = radius > 0x8000 ? 0x8000 : radius;
         _currentRadius = value;
         return SetRadius(value);
      }

      SSV3ERROR ScanCenter(bool axis, unsigned short value)
      {
         SSV3ERROR ret{ OK_ };
         if (axis)
            _scanCenter[0] = value;
         else
            _scanCenter[1] = value;
         unsigned char *msg = _oBuffer;
         *msg++ = 0x11;
         *msg++ = HByte(_scanCenter[0]);
         *msg++ = LByte(_scanCenter[0]);
         *msg++ = HByte(_scanCenter[1]);
         *msg = LByte(_scanCenter[1]);
         SendAndListen(&ret, 5, true);
         return ret;
      }

      SSV3ERROR LocationPark(unsigned short *location)
      {
         SSV3ERROR ret{ OK_ };
         unsigned char *msg = _oBuffer;
         *msg++ = 0x15;
         *msg++ = HByte(location[0]);
         *msg++ = LByte(location[0]);
         *msg++ = HByte(location[1]);
         *msg = LByte(location[1]);
         SendAndListen(&ret, 5, true);
         return ret;
      }

      SSV3ERROR TIRF(unsigned short value = 0)
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         unsigned short newVal = value > 0x8000 ? 0x8000 : value;
         //If we just want to go to the currently saved TIRF value
         if (newVal == 0)
         {
            //Pass the current x TIR value, yoffset will be applied
            _oBuffer[0] = 0x14;
            SendAndListen(&ret, 1, true);
            return ret;
         }
         //Otherwise we will program a new TIR value into the scancard
         _tirRadius = newVal;
         unsigned short yVal = (unsigned short)((float)newVal * _yOffset);
         unsigned char *msg = _oBuffer;
         *msg++ = 0x12;
         *msg++ = HByte(newVal);
         *msg++ = LByte(newVal);
         *msg++ = HByte(yVal);
         *msg++ = LByte(yVal);
         SendAndListen(&ret, 5, true);
         return ret;
      }

      SSV3ERROR LoadAngles(const unsigned char sequence, const unsigned short length, unsigned short *values)
      {
         SSV3ERROR ret{ OK_ };
         if (_demo)
            return ret;
         int readWriteVal{};
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         if (sequence > 31)
            return SSV3ERROR::SSV3ERROR_SEQUENCE_DOESNT_EXIST;


         //Calculate the number of packets required to transmit the sequence
         //Each angle requires 4 bytes (2 for x, 2 for y)
         //Maximum packet length is 64 bytes (32 2-byte DAC values)
         unsigned short nPackets = (unsigned short)ceil(((float)length * 4.0) / 64.0);
         unsigned char *msg = _oBuffer;
         *msg++ = 0x80;
         *msg++ = sequence;
         *msg++ = HByte(length);
         *msg++ = LByte(length);
         *msg++ = HByte(nPackets);
         *msg++ = LByte(nPackets);

         SendAndListen(&ret, 0, true);
         if (ret != OK_)
            return ret;
         if (_iBuffer[0] == 1)
            return SSV3ERROR::SSV3ERROR_SEQUENCE_ALLOCATION_FAIL;
         else if (_iBuffer[0] == 2)
            return SSV3ERROR::SSV3ERROR_SEQUENCE_LENGTH_ZERO;

         _angleSequences.at(sequence).clear();
         int counter = 0;
         _newErr = false;
         for (int i = 0; i < (nPackets - 1); i++)
         {
            msg = _oBuffer;
            for (int j = 0; j < 16; j++)
            {
               _angleSequences.at(sequence).push_back(values[counter]);
               unsigned short yval = (unsigned short)((float)values[counter] * _yOffset);
               *msg++ = HByte(values[counter]);
               *msg++ = LByte(values[counter]);
               *msg++ = HByte(yval);
               *msg++ = LByte(yval);
               counter++;
            }
            SendAndListen(&ret, 64, false);
            if (ret != OK_)
               return ret;
         }

         int remaining = length - counter; //Number of pairs left to send
         msg = _oBuffer;
         for (int i = 0; i < remaining; i++)
         {
            _angleSequences.at(sequence).push_back(values[counter]);
            unsigned short yval = (unsigned short)((float)values[counter] * _yOffset);
            *msg++ = HByte(values[counter]);
            *msg++ = LByte(values[counter]);
            *msg++ = HByte(yval);
            *msg++ = LByte(yval);
            counter++;
         }
         SendAndListen(&ret, 4 * remaining, false);
         if (ret != OK_)
            return ret;

         _experimentModified = true;
         if (hid_read(_dev, _iBuffer, 64) < 64)
            return SSV3ERROR::SSV3ERROR_NO_RESPONSE;
         if (_iBuffer[0] != 0)
            return SSV3ERROR::SSV3ERROR_SEQUENCE_LOAD_FAILED;
         return ret;
      }

      SSV3ERROR AddExperimentStep(const unsigned char sequence, const unsigned char excitation, const int step = -1)
      {
         //If the default -1 or some other negative value was passed
         // set the step number to one greater than the last valid index
         int stepNum = step;
         if (stepNum < 0)
            stepNum = (int)_experimentList.size();

         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         if (sequence > 31 || sequence > _angleSequences.size())
            return SSV3ERROR::SSV3ERROR_SEQUENCE_DOESNT_EXIST;
         else if (_angleSequences[sequence].empty())
            return SSV3ERROR::SSV3ERROR_SEQUENCE_DOESNT_EXIST;
         if (excitation > 31 || excitation > _illuminationProfiles.size())
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST;
         else if (_illuminationProfiles[excitation].empty())
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST;

         if (stepNum > _experimentList.size())
            return SSV3ERROR::SSV3ERROR_STEP_OUTSIDE_EXPERIMENT_RANGE;           

         Node newNode;
         newNode.number = stepNum;
         newNode._sequence = sequence;
         newNode._exSetting = excitation;

         if (stepNum == _experimentList.size())
            _experimentList.emplace_back(newNode);
         else
            _experimentList.emplace(_experimentList.begin() + step, newNode);
         _experimentModified = true;
         return ret;
      }

      SSV3ERROR ClearExperiment()
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;
         _experimentList.clear();
         _loopOnOff = false;
         _loopTo = 0;
         _experimentModified = true;
         return ret;
      }

      SSV3ERROR Loop(bool onOff, const unsigned int loopTo = 0)
      {

         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;

         if (_experimentList.size() == 0)
            return SSV3ERROR::SSV3ERROR_NO_EXPERIMENT;
         if (loopTo < 0 || loopTo >= _experimentList.size())
            return SSV3ERROR::SSV3ERROR_INVALID_LOOP;

         _loopOnOff = onOff;
         _loopTo = (unsigned char)loopTo;
         _experimentModified = true;
         return ret;
      }

      SSV3ERROR StartExperiment()
      {

         SSV3ERROR ret{ OK_ };
         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;
         if (_experimentList.size() == 0)
            return SSV3ERROR::SSV3ERROR_NO_EXPERIMENT;

         //This ensures that the current experiment design is programmed
         if (_experimentModified)
         {
            ret = ResendExperiment();
         }

         unsigned char *msg = _oBuffer;
         *msg++ = 0x87;
         *msg++ = 0x01;
         *msg++ = _defaultExperiment;
         *msg++ = 0x00;
         *msg++ = 0x00;
         *msg++ = _loopOnOff;
         *msg++ = 0x00;
         *msg++ = 0x00;
         SendAndListen(&ret, 0, true);
         if (ret != OK_)
            return ret;
         if (_iBuffer[0] == 1)
            return SSV3ERROR::SSV3ERROR_NO_EXPERIMENT;
         if (_iBuffer[0] != 0)
            return SSV3ERROR::SSV3ERROR_UNEXPECTED_RETURN;
         _experimentRunning = true;
         return ret;
      }

      SSV3ERROR StopExperiment()
      {
         SSV3ERROR ret{ OK_ };
         if (!_experimentRunning)
            return ret;
         _oBuffer[0] = 0x8F;
         SendAndListen(&ret, 1, true);
         if (ret != OK_)
            return ret;
         _experimentRunning = false;
         for (unsigned char i = 0; i < 8; i++)
         {
            SingleLaserPower(i, _currentExcitation[i]);
         }
         SetRadius(_currentRadius);
         Fire(true);
         return ret;
      }

      SSV3ERROR SendSWTrigger(unsigned short period = 0xffff)
      {
         SSV3ERROR ret{ OK_ };
         unsigned short resetVal = 0xffff - period;
         unsigned char* msg = _oBuffer;
         *msg++ = 0x5f;
         *msg++ = HByte(resetVal);
         *msg++ = LByte(resetVal);
         SendAndListen(&ret, 0, true);
         return ret;
      }

      SSV3ERROR SendArray(unsigned char* msg, const size_t length = 0)
      {
         SSV3ERROR ret{ OK_ };
         for (size_t i = 0; i < length; i++)
            _oBuffer[i] = msg[i];
         SendAndListen(&ret, 0, true);
         if (ret != OK_)
            return ret;
         for (size_t i = 0; i < length; i++)
            msg[i] = _iBuffer[i];
         return ret;
      }

      SSV3ERROR QueryInternalSettings(
         unsigned short *xCenter,
         unsigned short *yCenter,
         unsigned short *tirRadius,
         unsigned short *phase,
         unsigned short *frequency)
      {
         SSV3ERROR ret{ OK_ };
         _oBuffer[0] = 0xF1;
         SendAndListen(&ret, 1, true);
         if (ret != OK_)
            return ret;
         *xCenter = _iBuffer[1];
         *xCenter = (*xCenter << 8) | (_iBuffer[2] & 0xff);
         *yCenter = _iBuffer[3];
         *yCenter = (*yCenter << 8) | (_iBuffer[4] & 0xff);
         *tirRadius = _iBuffer[5];
         *tirRadius = (*tirRadius << 8) | (_iBuffer[6] & 0xff);
         *phase = _iBuffer[7];
         *phase = (*phase << 8) | (_iBuffer[8] & 0xff);
         *frequency = _iBuffer[9];
         *frequency = (*frequency << 8) | (_iBuffer[10] & 0xff);
         return ret;
      }

      SSV3ERROR QueryDevVer(unsigned char *brdMajor, unsigned char *brdMinor, unsigned char *fwMajor, unsigned char *fwMinor)
      {
         SSV3ERROR ret{ OK_ };
         _oBuffer[0] = 0xF2;
         SendAndListen(&ret, 1, true);
         if (ret != OK_)
            return ret;
         *brdMajor = _iBuffer[1];
         *brdMinor = _iBuffer[2];
         *fwMajor = _iBuffer[3];
         *fwMinor = _iBuffer[4];
         return ret;
      }

      SSV3ERROR DetailedErrorReporting(bool onOff)
      {
         _detailedReporting = onOff;
         return OK_;
      }

      const char * GetLastError() 
      { 
         if (_newErr)
         {
            if (_errStr != nullptr)
               delete[] _errStr;
            _errStr = new char[_errMsg.str().size()];
            strcpy(_errStr, _errMsg.str().c_str());
            _newErr = false;
            return _errStr;
         }
         else
            return NULL;
      }

      void Reset()
      {
         _oBuffer[0] = 0xff;
         hid_write(_dev, _xmit, 65);
      }

   private:
      /////////////////////////////////////////////////////////////////////////
      /*  Member variables                                                   */
      /////////////////////////////////////////////////////////////////////////
      unsigned char _xmit[65]{ 0 };
      unsigned char *_oBuffer{ &_xmit[1] };
      unsigned char _iBuffer[64]{ 0 };
      unsigned int _timeout{ 2000 };
      unsigned char _usingExcitation{0xff};
      hid_device *_dev;
      float _yOffset{ 1 };
      unsigned short _currentRadius{ 0 };
      unsigned short _scanCenter[2]{ 0x7fff, 0x7fff };
      unsigned short _tirRadius{ 0x2d00 };
      unsigned int _readAttempts{ 2 };
      unsigned char _defaultExperiment{ 16 };
      int _loopTo{ 0 };
      bool _loopOnOff{ false };
      bool _experimentRunning{ false };
      std::vector<unsigned short> _currentExcitation;
      std::vector<std::vector<unsigned short>> _angleSequences;
      std::vector<std::vector<unsigned short>> _illuminationProfiles;
      std::vector<Node> _experimentList;
      bool _experimentModified{ false };
      bool _demo{ false };
      bool _detailedReporting{ false };
      std::stringstream _errMsg;
      bool _newErr{ false };
      char *_errStr{ nullptr };
      

      /////////////////////////////////////////////////////////////////////////
      /*  Internal Functions                                                 */
      /////////////////////////////////////////////////////////////////////////

      //Send the contents of the output buffer and wait for a response
      void Transmit(SSV3ERROR *err, int nCheck, bool retry, const char *fun)
      {
         if (_demo)
         {
            *err = OK_;
            return;
         }
         //Ensure that the report sent is 0x00
         _xmit[0] = 0x00;

         *err = OK_;
         int ret = 0;
         bool success{ false };
         if (!retry)
            success = true;

         int nRet = hid_write(_dev, _xmit, 65);
         if (nRet < 65)
         {
            if (_detailedReporting)
            {
               _newErr = true;
               _errMsg.str("");
               _errMsg << "Error in function ";
               _errMsg << fun;
               if (nRet == -1)
               {
                  _errMsg << " - Write failed.\n";
               }
               else
               {
                  _errMsg << " - Wrote " << nRet << " of 65 bytes.\n";
               }
               _errMsg << "Attempted to write: ";
               for (int i = 0; i < 64; i++)
                  _errMsg << (int)_oBuffer[i] << " ";
            }
            *err = SSV3ERROR::SSV3ERROR_XMIT_FAIL;
            return;
         }

         //Read the received buffer
         unsigned int rAttempts = 1;
         do {
            ret = hid_read_timeout(_dev, _iBuffer, 64, _timeout);
            //When we get the response break both loops
            if (ret == 64)
            {
               success = true;
               break;
            }
            //Otherwise, keep trying to read
            rAttempts++;
            if (rAttempts > _readAttempts)
               break;
         } while (!success);

         //If there was no response
         if (!success)
         {
            if (_detailedReporting)
            {
               _newErr = true;
               _errMsg.str("");
               _errMsg << "No reponse from device in function " << fun << "\n";
            }
            *err = SSV3ERROR::SSV3ERROR_NO_RESPONSE;
         }

         //If the response is truncated
         else if (ret < 64)
         {
            if (_detailedReporting)
            {
               _newErr = true;
               _errMsg.str("");
               _errMsg << "Error in function " << fun << " received " << ret << " of 64 bytes.\n";
            }
            *err = SSV3ERROR::SSV3ERROR_READ_TOO_SHORT;
         }

         //If a response check was specified check the number of bytes indicated
         else if (nCheck > 0)
         {
            bool mismatch{ false };
            for (int i = 0; i < nCheck; i++)
               if (_iBuffer[i] != _oBuffer[i])
                  mismatch = true;
            if (mismatch && _detailedReporting)
            {
               _newErr = true;
               _errMsg.str("");
               _errMsg << "Error in function " << fun << " response did not match expected response.\n";
               *err = SSV3ERROR::SSV3ERROR_UNEXPECTED_RETURN;
            }
         }
         if (*err != OK_)
         {
            _errMsg << "Wrote :";
            for (int i = 0; i < 64; i++)
               _errMsg << (int)_oBuffer[i] << " ";
            _errMsg << "\nReceived :";
            for (int i = 0; i < 64; i++)
               _errMsg << (int)_iBuffer[i] << " ";
            return;
         }
         return;
      }

      unsigned char HByte(unsigned short x) { return (unsigned char)(x >> 8); }
      unsigned char LByte(unsigned short x) { return (unsigned char)x; }

      //Set a new scan radius
      SSV3ERROR SetRadius(unsigned short value)
      {
         SSV3ERROR ret{ OK_ };
         unsigned short yVal = (unsigned short)((float)value * _yOffset);
         yVal = yVal > 0x8000 ? 0x8000 : yVal;
         unsigned char *msg = _oBuffer;
         *msg++ = 0x10;
         *msg++ = HByte(value);
         *msg++ = LByte(value);
         *msg++ = HByte(yVal);
         *msg++ = LByte(yVal);
         SendAndListen(&ret, 5, true);
         return ret;
      }

      //Reprograms the current experiment
      //This is necessary after any call that changes the excitation profiles
      // or angle sequences
      SSV3ERROR ResendExperiment()
      {
         SSV3ERROR ret{ OK_ };
         if (_experimentList.size() == 0)
            return ret;

         if (_experimentRunning)
            ret = StopExperiment();
         if (ret != OK_)
            return ret;


         //Basically we just reattach all the nodes
         //Otherwise the controller may have nonexistent pointers to the data
         unsigned char *msg = _oBuffer;
         *msg++ = 0x83;  //Add a node
         *msg++ = _defaultExperiment;
         *msg++ = _experimentList[0]._sequence;
         *msg++ = _experimentList[0]._exSetting;
         SendAndListen(&ret, 0, false);
         if (ret != OK_)
            return ret;
         if (_iBuffer[0] == 1)
            return SSV3ERROR::SSV3ERROR_ALLOC_FAIL;
         if (_iBuffer[0] == 3)
            return SSV3ERROR::SSV3ERROR_SEQUENCE_DOESNT_EXIST;
         if (_iBuffer[0] == 4)
            return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST;

         //Now build the rest of the experiment
         for (std::vector<Node>::iterator itr(_experimentList.begin() + 1); itr != _experimentList.end(); itr++)
         {
            msg = _oBuffer;
            *msg++ = 0x85;
            *msg++ = _defaultExperiment;
            *msg++ = itr->_sequence;
            *msg = itr->_exSetting;
            SendAndListen(&ret, 0, false);
            if (ret != OK_)
               return ret;
            if (_iBuffer[0] == 1)
               return SSV3ERROR::SSV3ERROR_ALLOC_FAIL;
            if (_iBuffer[0] == 3)
               return SSV3ERROR::SSV3ERROR_SEQUENCE_DOESNT_EXIST;
            if (_iBuffer[0] == 4)
               return SSV3ERROR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST;
         }

         //Rebuild the loop
         if (_loopOnOff)
         {
            if (_loopTo < _experimentList.size())
            {
               msg = _oBuffer;
               *msg++ = 0x86;
               *msg++ = _defaultExperiment;
               *msg++ = _loopTo;
               SendAndListen(&ret, 0, true);
               if (ret != OK_)
                  return ret;
               if (_iBuffer[0] == 1)
                  ret = SSV3ERROR::SSV3ERROR_NO_EXPERIMENT;
               if (_iBuffer[0] == 2)
                  ret = SSV3ERROR::SSV3ERROR_INVALID_LOOP;
            }
            else
               ret = SSV3ERROR::SSV3ERROR_INVALID_LOOP;
         }
         _experimentModified = false;
         return ret;
      }
   };


#ifdef __cplusplus
   extern "C" {
#endif
      SSV3API_ SSV3Controller __cdecl CreateDevice() 
      {
         SSV3Controller p = new ScanCard;
         return p; 
      }

      SSV3API_ SSV3Controller __cdecl CreateDeviceFromSN(wchar_t *sn)
      {
         SSV3Controller p = new ScanCard(sn);
         return p;
      }

      SSV3API_ SSV3Controller __cdecl CreateDemoDevice(bool demo)
      {
         SSV3Controller p = new ScanCard(demo);
         return p;
      }
#ifdef __cplusplus
   }
#endif
}