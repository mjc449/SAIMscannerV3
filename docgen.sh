#!/usr/bin/env bash
cwd=$(pwd)
#Build the software documentation
cd software/SSv3_Driver
doxygen Doxyfile
#build the bootloader documentation
cd $cwd
cd firmware/mc_bootloader.X/
doxygen Doxyfile
#build the firmware documentation
cd $cwd
cd firmware/mc_firmware.X/
doxygen Doxyfile
cd $cwd