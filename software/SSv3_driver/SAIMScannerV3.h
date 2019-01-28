/**/////////////////////////////////////////////////////////////////////////////
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
//                  While devices can be created in applications using a      //
//                  variety of methods, the recommended approach is using a   //
//                  smart pointer, e.g.:                                      //
//                                                                            //
//                  #include <functional>                                     //
//                  typedef std::shared_ptr<SSV3::Controller> pScanCard;      //
//                  pScanCard scancard(SSV3::CreateDevice(),                  //
//                     std::mem_fn(&(SSV3::Controller::Destroy)));            //
//                                                                            //
//                  In this way the controller is shutdown to a safe state    //
//                  when the application ends or in the case of an error      //
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


#ifndef SAIMSCANNERV3_H_
#define SAIMSCANNERV3_H_

#ifdef MAKE_DLL_
#define SSV3API_ __declspec(dllexport)
#else
#define SSV3API_ __declspec(dllimport)
#endif  //MAKE_DLL_


namespace SSV3
{
   typedef class Manager * SSV3Manager;
   typedef class Controller * SSV3Controller;

   
   /**Base SSv3 controller class wraps the HID communications into opaque\n
   //function calls facilitating rapid, easy integration*/
   class Controller
   {
   public:
      virtual ~Controller() = default;
      
      /**Return error codes*/
      enum class SSV3ERROR
      {
         SSV3ERROR_OK,
         SSV3ERROR_NO_DEVICES_FOUND,
         SSV3ERROR_DEVICE_UNAVAILABLE,
         SSV3ERROR_NO_RESPONSE,
         SSV3ERROR_UNEXPECTED_RETURN,
         SSV3ERROR_XMIT_FAIL,
         SSV3ERROR_READ_TOO_SHORT,
         SSV3ERROR_EXCITATION_FULL,
         SSV3ERROR_ALLOC_FAIL,
         SSV3ERROR_EXCITATION_PROFILE_OUT_OF_RANGE,
         SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST,
         SSV3ERROR_SEQUENCES_FULL,
         SSV3ERROR_SEQUENCE_ALLOCATION_FAIL,
         SSV3ERROR_SEQUENCE_LENGTH_ZERO,
         SSV3ERROR_SEQUENCE_DOESNT_EXIST,
         SSV3ERROR_SEQUENCE_LOAD_FAILED,
         SSV3ERROR_NO_EXPERIMENT,
         SSV3ERROR_INVALID_LOOP,
         SSV3ERROR_STEP_OUTSIDE_EXPERIMENT_RANGE,
         SSV3ERROR_COULDNT_OPEN_ERR_LOG
      };

      /**Disconnects the controller and destroys the instance.\n
      //Handle must not be used after calling Destroy\n
      //To reattach the controller create a new instance*/
      virtual void Destroy() = 0;

      /**Check for hidapi errors\n
      //Returns a descriptive string of the error or NULL*/
      virtual const wchar_t * HidError() = 0;

      /**Sets the controller to a safe beginning state\n
      //and prints the manufacturer and product strings to the command out*/
      virtual SSV3ERROR Initialize() = 0;

      /**Change the response timeout value
      //@param ms = timeout value in ms*/
      virtual void Timeout(unsigned int ms) = 0;

      /**Set the maximum number of retries on read calls
      //@param attempts = number or attemps before failure, default is 2*/
      virtual void ReadRetries(unsigned int attempts) = 0;

      /**Runs a quick visor across the status LEDS on the front\n
      //of the controller*/
      virtual SSV3ERROR Visor() = 0;

      /**Centers the beam, stops scanning, and closes the shutter*/
      virtual SSV3ERROR CenterPark() = 0;

      /**Stops scanning and parks the beam\n
      //location[0] = x inclination, location[1] = y inclination\n
      //@param location = park location in DAC units*/
      virtual SSV3ERROR LocationPark(unsigned short *location) = 0;

      /**Open or close the mechanical shutter\n
      //@param state - true for open, false for close*/
      virtual SSV3ERROR Shutter(bool state) = 0;

      /**Blanks all laser lines via the AOTF\n
      //@param state = false blanks lasers (off), true enables lasers (on)*/
      virtual SSV3ERROR AOTFBlank(bool state) = 0;

      /**Set the absolute intensity for a specific laser line\n
      //@param line = laser to be adjusted\n
      //@param value = power (max 1023)*/
      virtual SSV3ERROR SingleLaserPower(unsigned char line, unsigned short value) = 0;

      /**Create an excitation profile object\n
      //The newly initialized profile will be all 0s (all lasers off)\n
      //If no number for the profile is specified the next available\n
      //number will be used,\n
      //If all profiles are full and no number is specified the call will fail\n
      //@param profile = the number ID of the profile being created\n
      //@param values = optional pointer to array of 8 laser values\n*/
      virtual SSV3ERROR MakeExcitationProfile(const unsigned char profile, unsigned short *values = nullptr) = 0;

      /**Set the value of a line in a excitation profile and switch to\n
      // the profile\n
      //@param profile = handle for the profile being changed\n
      //@param line = laser line number\n
      //@param value = new value (max 1023)\n*/
      virtual SSV3ERROR SetProfilePower(const unsigned char profile, const unsigned char line, const unsigned short value) = 0;

      /**Load an excitation profile\n
      //@param profile = excitation settings handle to load*/
      virtual SSV3ERROR LoadExcitationProfile(const unsigned char profile) = 0;

      /**Create a new profile with the current laser settings\n
      //If profile already exists the settings will be overwritten\n
      //@param profile = handle to the new excitation profile*/
      virtual SSV3ERROR MakeProfileFromCurrentExcitation(const unsigned char profile) = 0;

      /**Turn on or off the camera triggered excitation\n
      //@param state = true for on, false for off*/
      virtual SSV3ERROR Fire(bool state) = 0;

      /**Zeros all 8 laser lines without changing any of the stored profiles*/
      virtual SSV3ERROR ClearExcitation() = 0;

      /**Adjust the phase value\n
      //0 deg = 0x000, 90 deg ~0x400, 180 deg ~0x800\n
      //@param phase = waveform phase offset*/
      virtual SSV3ERROR AdjustPhase(unsigned short phase) = 0;

      /**Adjust the scan frequency\n
      //0 Hz = 0x00, 1 kHz = 0x29f1\n
      //Maximum allowed value is 1 kHz\n
      //@param frequency = circle scan frequency*/
      virtual SSV3ERROR AdjustFrequency(unsigned short frequency) = 0;

      /**Set scan amplitude offset  in the y axis.\n
      //Used to correct elipticity in the scan profile.\n
      //0x7fff corresponds to 0 offset\n
      //@param value = relative scaling of the Y-axis*/
      virtual SSV3ERROR YAmpCorrection(unsigned short value) = 0;

      /**Update the current scan radius\n
      //Radius is limited to 0x4200 to protect the galvos from being overdriven\n
      //@param radius = desired scan radius in DAC units*/
      virtual SSV3ERROR ScanRadius(unsigned short radius) = 0;

      /**Changes the DC bias on one of the galvos\n
      //@param axis = axis to move, true for x, false for y\n
      //@param value = center (0 bias) is 0x7fff*/
      virtual SSV3ERROR ScanCenter(bool axis, unsigned short value) = 0;

      /**Change the TIR scan value\n
      //Also update current scan radius to new TIR value\n
      //If value == 0 or default TIR value is not update and the scan\n
      // radius is set to the current TIR value\n
      //@param value = new scan radius (limitied to 0x4000)*/
      virtual SSV3ERROR TIRF(unsigned short value = 0) = 0;

      /**Load a set of radii into the controller memory\n
      //The radii are specified as a 2-byte DAC value corresponding to the\n
      // x-galvo amplitude.  The y-offset will be used to calculate\n
      // the y-galvo amplitude internally.\n
      //Returns an error the number parameter is specified and sequence\n
      // doesn't exist\n
      //@param sequence = sequence number in internal memory\n
      //@param length = size of the sequence (number of steps)\n
      //@param values = array of values to be loaded*/
      virtual SSV3ERROR LoadAngles(const unsigned char sequence, const unsigned short length, unsigned short *values) = 0;

      /**Add an angle sequence and a excitation setting to the end of the\n
      // experiment.\n
      //@param sequence = set of angles to add\n
      //@param excitation = excitation profile to add\n
      //@param step = the step number in the sequence if step < 0 step will be inserted at the end if a step already exists it will be overwritten*/
      virtual SSV3ERROR AddExperimentStep(const unsigned char sequence, const unsigned char excitation, const int step = -1) = 0;

      /**Clears the experiment of all steps*/
      virtual SSV3ERROR ClearExperiment() = 0;

      /**Create or destroy a loop in the experiment.\n
      //When the end of the experiment list is reached it will loop back to the\n
      // step specified.\n
      //@param onOff = turn the loop on or off\n
      //@param loopTo = number of the experiment step to loop to*/
      virtual SSV3ERROR Loop(bool onOff, const unsigned int loopTo = 0) = 0;

      /**Begin the currently programmed experiment.*/
      virtual SSV3ERROR StartExperiment() = 0;

      /**Stop the current experiment and return to the state at the beginning\n
      // of the experiment*/
      virtual SSV3ERROR StopExperiment() = 0;

      /**Send a single pulse to simulate a camera frame (positive polarity)\n
      //@param period = the pulse length (0x00 ~80 us, 0xff ~32 ms)*/
      virtual SSV3ERROR SendSWTrigger(unsigned short period = 0xffff) = 0;

      /**Send the input array to the controller.\n
      //Low level direct access to the controller functions.\n
      //Not meant for general use\n
      //No check is made on the response other than that it was received\n
      //The *msg argument is overwritten with the returned values\n
      //@param array = byte array to be sent\n
      //@param length = length of the packet, must not be greater than 64*/
      virtual SSV3ERROR SendArray(unsigned char *msg, const size_t length = 0) = 0;

      /**Queries the device for the current scan settings from the controller onboard memory.  All parameters are returned in DAC units.  This is particularly useful at startup so that the current state of the controller can be indentified.
      @param xCenter = scan center x value (DC offset of waveform)
      @param yCenter = scan center y value (DC offset of waveform)
      @param tirRadius = stored TIR value
      @param phase = stored phase difference between the output waveforms
      @param frequency = stored scan (waveform generator) frequency*/
      virtual SSV3ERROR QueryInternalSettings(unsigned short *xCenter, unsigned short *yCenter, unsigned short *tirRadius, unsigned short *phase, unsigned short *frequency) = 0;

      /**Queries the harware and firmware versions of the controller
      @param brdMajor = major hardware revision
      @param brdMinor = minor hardware revision
      @param fwMajor = firmware major revision
      @param fwMinor = firmware minor revision*/
      virtual SSV3ERROR QueryDevVer(unsigned char *brdMajor, unsigned char *brdMinor, unsigned char *fwMajor, unsigned char *fwMinor) = 0;

      /**Detailed error logging for debugging.  This adds overhead to API function calls so should be left off in normal operation*/
      virtual SSV3ERROR DetailedErrorReporting(bool onOff) = 0;

      /**Get the last error description, returns null if there are no errors*/
      virtual const char * GetLastError() = 0;

      /**Resets the device erasing all data and calls the deleter Destroy().\n
      //The controller instance is no longer valid after call.\n
      //It is the caller's responsibility to destroy any references.*/
      virtual void Reset() = 0;
   };


   /**Simple wrapper around the hidapi library to add specificity for the SSv3 device.  Makes attaching and tracking multiple devices easier.*/
   class Manager
   {
   public:
      /**Return error codes*/
      enum class SSV3MANAGER_ERROR
      {
         SSV3MANAGER_ERROR_OK,
         SSV3MANAGER_ERROR_INIT_FAILED,
         SSV3MANAGER_ERROR_NO_DEVICES,
         SSV3MANAGER_ERROR_DEVICE_INVALID
      };

      virtual ~Manager() = default;

      /**Disconnects the manager and frees the enumerated devices.
      It is the caller's responsibility to ensure that the manager is 
      no longer referenced*/
      virtual void Destroy() = 0;

      /**Initializes the SSv3 device manager
      @param nDevs = number of detected devices*/
      virtual SSV3MANAGER_ERROR Enumerate(int *nDevs) = 0;

      /**Get the number of attached devices
      @param nDevs = number of devices found*/
      virtual SSV3MANAGER_ERROR DeviceCount(int *nDevs) = 0;

      /**Query the device ID
      @param dev = device number
      @param man = manufacturer string
      @param prod = product string
      @param sn = serial number string*/
      virtual SSV3MANAGER_ERROR GetDeviceInfo(const int dev, wchar_t *man, wchar_t *prod, wchar_t *sn) = 0;

      /**Refreshes the device list.  
      Previous device counts and information may no longer be valid
      @param nDevs = number of devices found*/
      virtual SSV3MANAGER_ERROR RefreshDevices(int *nDevs) = 0;
   };

#ifdef __cplusplus
   extern "C" {
#endif
      SSV3API_ SSV3Controller __cdecl CreateDevice();
      SSV3API_ SSV3Controller __cdecl CreateDeviceFromSN(wchar_t *sn);
      SSV3API_ SSV3Controller __cdecl CreateDemoDevice(bool demo = true);
      SSV3API_ SSV3Manager __cdecl CreateManager();
#ifdef __cplusplus
   }
#endif
}

#endif //SAIMSCANNERV3_H_