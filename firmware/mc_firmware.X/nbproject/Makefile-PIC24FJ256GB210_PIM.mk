#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-PIC24FJ256GB210_PIM.mk)" "nbproject/Makefile-local-PIC24FJ256GB210_PIM.mk"
include nbproject/Makefile-local-PIC24FJ256GB210_PIM.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=PIC24FJ256GB210_PIM
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=interrupts/ext_callbacks.c interrupts/ext_interrupts.c interrupts/timer_callbacks.c interrupts/timer_interrupts.c peripherals/dio_pins.c peripherals/waveform_generators.c peripherals/waveform_dac.c system/system.c system/traps.c usb/usb_descriptors.c usb/usb_device.c usb/usb_device_hid.c usb/usb_events.c usb/usb_hal_16bit.c usb/app_device_custom_hid.c usb/command_parser.c rasterscan.c main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/interrupts/ext_callbacks.o ${OBJECTDIR}/interrupts/ext_interrupts.o ${OBJECTDIR}/interrupts/timer_callbacks.o ${OBJECTDIR}/interrupts/timer_interrupts.o ${OBJECTDIR}/peripherals/dio_pins.o ${OBJECTDIR}/peripherals/waveform_generators.o ${OBJECTDIR}/peripherals/waveform_dac.o ${OBJECTDIR}/system/system.o ${OBJECTDIR}/system/traps.o ${OBJECTDIR}/usb/usb_descriptors.o ${OBJECTDIR}/usb/usb_device.o ${OBJECTDIR}/usb/usb_device_hid.o ${OBJECTDIR}/usb/usb_events.o ${OBJECTDIR}/usb/usb_hal_16bit.o ${OBJECTDIR}/usb/app_device_custom_hid.o ${OBJECTDIR}/usb/command_parser.o ${OBJECTDIR}/rasterscan.o ${OBJECTDIR}/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/interrupts/ext_callbacks.o.d ${OBJECTDIR}/interrupts/ext_interrupts.o.d ${OBJECTDIR}/interrupts/timer_callbacks.o.d ${OBJECTDIR}/interrupts/timer_interrupts.o.d ${OBJECTDIR}/peripherals/dio_pins.o.d ${OBJECTDIR}/peripherals/waveform_generators.o.d ${OBJECTDIR}/peripherals/waveform_dac.o.d ${OBJECTDIR}/system/system.o.d ${OBJECTDIR}/system/traps.o.d ${OBJECTDIR}/usb/usb_descriptors.o.d ${OBJECTDIR}/usb/usb_device.o.d ${OBJECTDIR}/usb/usb_device_hid.o.d ${OBJECTDIR}/usb/usb_events.o.d ${OBJECTDIR}/usb/usb_hal_16bit.o.d ${OBJECTDIR}/usb/app_device_custom_hid.o.d ${OBJECTDIR}/usb/command_parser.o.d ${OBJECTDIR}/rasterscan.o.d ${OBJECTDIR}/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/interrupts/ext_callbacks.o ${OBJECTDIR}/interrupts/ext_interrupts.o ${OBJECTDIR}/interrupts/timer_callbacks.o ${OBJECTDIR}/interrupts/timer_interrupts.o ${OBJECTDIR}/peripherals/dio_pins.o ${OBJECTDIR}/peripherals/waveform_generators.o ${OBJECTDIR}/peripherals/waveform_dac.o ${OBJECTDIR}/system/system.o ${OBJECTDIR}/system/traps.o ${OBJECTDIR}/usb/usb_descriptors.o ${OBJECTDIR}/usb/usb_device.o ${OBJECTDIR}/usb/usb_device_hid.o ${OBJECTDIR}/usb/usb_events.o ${OBJECTDIR}/usb/usb_hal_16bit.o ${OBJECTDIR}/usb/app_device_custom_hid.o ${OBJECTDIR}/usb/command_parser.o ${OBJECTDIR}/rasterscan.o ${OBJECTDIR}/main.o

# Source Files
SOURCEFILES=interrupts/ext_callbacks.c interrupts/ext_interrupts.c interrupts/timer_callbacks.c interrupts/timer_interrupts.c peripherals/dio_pins.c peripherals/waveform_generators.c peripherals/waveform_dac.c system/system.c system/traps.c usb/usb_descriptors.c usb/usb_device.c usb/usb_device_hid.c usb/usb_events.c usb/usb_hal_16bit.c usb/app_device_custom_hid.c usb/command_parser.c rasterscan.c main.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-PIC24FJ256GB210_PIM.mk dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ256GB210
MP_LINKER_FILE_OPTION=,--script="hid_boot_p24FJ256GB210.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/interrupts/ext_callbacks.o: interrupts/ext_callbacks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/ext_callbacks.o.d 
	@${RM} ${OBJECTDIR}/interrupts/ext_callbacks.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/ext_callbacks.c  -o ${OBJECTDIR}/interrupts/ext_callbacks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/ext_callbacks.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/ext_callbacks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts/ext_interrupts.o: interrupts/ext_interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/ext_interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts/ext_interrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/ext_interrupts.c  -o ${OBJECTDIR}/interrupts/ext_interrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/ext_interrupts.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/ext_interrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts/timer_callbacks.o: interrupts/timer_callbacks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/timer_callbacks.o.d 
	@${RM} ${OBJECTDIR}/interrupts/timer_callbacks.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/timer_callbacks.c  -o ${OBJECTDIR}/interrupts/timer_callbacks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/timer_callbacks.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/timer_callbacks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts/timer_interrupts.o: interrupts/timer_interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/timer_interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts/timer_interrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/timer_interrupts.c  -o ${OBJECTDIR}/interrupts/timer_interrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/timer_interrupts.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/timer_interrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/peripherals/dio_pins.o: peripherals/dio_pins.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/peripherals" 
	@${RM} ${OBJECTDIR}/peripherals/dio_pins.o.d 
	@${RM} ${OBJECTDIR}/peripherals/dio_pins.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peripherals/dio_pins.c  -o ${OBJECTDIR}/peripherals/dio_pins.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/peripherals/dio_pins.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/peripherals/dio_pins.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/peripherals/waveform_generators.o: peripherals/waveform_generators.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/peripherals" 
	@${RM} ${OBJECTDIR}/peripherals/waveform_generators.o.d 
	@${RM} ${OBJECTDIR}/peripherals/waveform_generators.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peripherals/waveform_generators.c  -o ${OBJECTDIR}/peripherals/waveform_generators.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/peripherals/waveform_generators.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/peripherals/waveform_generators.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/peripherals/waveform_dac.o: peripherals/waveform_dac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/peripherals" 
	@${RM} ${OBJECTDIR}/peripherals/waveform_dac.o.d 
	@${RM} ${OBJECTDIR}/peripherals/waveform_dac.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peripherals/waveform_dac.c  -o ${OBJECTDIR}/peripherals/waveform_dac.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/peripherals/waveform_dac.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/peripherals/waveform_dac.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system/system.o: system/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/system" 
	@${RM} ${OBJECTDIR}/system/system.o.d 
	@${RM} ${OBJECTDIR}/system/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system/system.c  -o ${OBJECTDIR}/system/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system/system.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/system/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system/traps.o: system/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/system" 
	@${RM} ${OBJECTDIR}/system/traps.o.d 
	@${RM} ${OBJECTDIR}/system/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system/traps.c  -o ${OBJECTDIR}/system/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system/traps.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/system/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_descriptors.o: usb/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_descriptors.c  -o ${OBJECTDIR}/usb/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_descriptors.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_device.o: usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device.c  -o ${OBJECTDIR}/usb/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_device_hid.o: usb/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device_hid.c  -o ${OBJECTDIR}/usb/usb_device_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device_hid.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_events.o: usb/usb_events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_events.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_events.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_events.c  -o ${OBJECTDIR}/usb/usb_events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_events.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_hal_16bit.o: usb/usb_hal_16bit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_hal_16bit.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_hal_16bit.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_hal_16bit.c  -o ${OBJECTDIR}/usb/usb_hal_16bit.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_hal_16bit.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_hal_16bit.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/app_device_custom_hid.o: usb/app_device_custom_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/app_device_custom_hid.o.d 
	@${RM} ${OBJECTDIR}/usb/app_device_custom_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/app_device_custom_hid.c  -o ${OBJECTDIR}/usb/app_device_custom_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/app_device_custom_hid.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/app_device_custom_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/command_parser.o: usb/command_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/command_parser.o.d 
	@${RM} ${OBJECTDIR}/usb/command_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/command_parser.c  -o ${OBJECTDIR}/usb/command_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/command_parser.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/command_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/rasterscan.o: rasterscan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/rasterscan.o.d 
	@${RM} ${OBJECTDIR}/rasterscan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  rasterscan.c  -o ${OBJECTDIR}/rasterscan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/rasterscan.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/rasterscan.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/interrupts/ext_callbacks.o: interrupts/ext_callbacks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/ext_callbacks.o.d 
	@${RM} ${OBJECTDIR}/interrupts/ext_callbacks.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/ext_callbacks.c  -o ${OBJECTDIR}/interrupts/ext_callbacks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/ext_callbacks.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/ext_callbacks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts/ext_interrupts.o: interrupts/ext_interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/ext_interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts/ext_interrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/ext_interrupts.c  -o ${OBJECTDIR}/interrupts/ext_interrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/ext_interrupts.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/ext_interrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts/timer_callbacks.o: interrupts/timer_callbacks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/timer_callbacks.o.d 
	@${RM} ${OBJECTDIR}/interrupts/timer_callbacks.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/timer_callbacks.c  -o ${OBJECTDIR}/interrupts/timer_callbacks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/timer_callbacks.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/timer_callbacks.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts/timer_interrupts.o: interrupts/timer_interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/interrupts" 
	@${RM} ${OBJECTDIR}/interrupts/timer_interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts/timer_interrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts/timer_interrupts.c  -o ${OBJECTDIR}/interrupts/timer_interrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts/timer_interrupts.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/interrupts/timer_interrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/peripherals/dio_pins.o: peripherals/dio_pins.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/peripherals" 
	@${RM} ${OBJECTDIR}/peripherals/dio_pins.o.d 
	@${RM} ${OBJECTDIR}/peripherals/dio_pins.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peripherals/dio_pins.c  -o ${OBJECTDIR}/peripherals/dio_pins.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/peripherals/dio_pins.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/peripherals/dio_pins.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/peripherals/waveform_generators.o: peripherals/waveform_generators.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/peripherals" 
	@${RM} ${OBJECTDIR}/peripherals/waveform_generators.o.d 
	@${RM} ${OBJECTDIR}/peripherals/waveform_generators.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peripherals/waveform_generators.c  -o ${OBJECTDIR}/peripherals/waveform_generators.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/peripherals/waveform_generators.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/peripherals/waveform_generators.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/peripherals/waveform_dac.o: peripherals/waveform_dac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/peripherals" 
	@${RM} ${OBJECTDIR}/peripherals/waveform_dac.o.d 
	@${RM} ${OBJECTDIR}/peripherals/waveform_dac.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  peripherals/waveform_dac.c  -o ${OBJECTDIR}/peripherals/waveform_dac.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/peripherals/waveform_dac.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/peripherals/waveform_dac.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system/system.o: system/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/system" 
	@${RM} ${OBJECTDIR}/system/system.o.d 
	@${RM} ${OBJECTDIR}/system/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system/system.c  -o ${OBJECTDIR}/system/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system/system.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/system/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system/traps.o: system/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/system" 
	@${RM} ${OBJECTDIR}/system/traps.o.d 
	@${RM} ${OBJECTDIR}/system/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system/traps.c  -o ${OBJECTDIR}/system/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system/traps.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/system/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_descriptors.o: usb/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_descriptors.c  -o ${OBJECTDIR}/usb/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_descriptors.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_device.o: usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device.c  -o ${OBJECTDIR}/usb/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_device_hid.o: usb/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device_hid.c  -o ${OBJECTDIR}/usb/usb_device_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device_hid.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_events.o: usb/usb_events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_events.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_events.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_events.c  -o ${OBJECTDIR}/usb/usb_events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_events.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_hal_16bit.o: usb/usb_hal_16bit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_hal_16bit.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_hal_16bit.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_hal_16bit.c  -o ${OBJECTDIR}/usb/usb_hal_16bit.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_hal_16bit.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_hal_16bit.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/app_device_custom_hid.o: usb/app_device_custom_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/app_device_custom_hid.o.d 
	@${RM} ${OBJECTDIR}/usb/app_device_custom_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/app_device_custom_hid.c  -o ${OBJECTDIR}/usb/app_device_custom_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/app_device_custom_hid.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/app_device_custom_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/command_parser.o: usb/command_parser.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/command_parser.o.d 
	@${RM} ${OBJECTDIR}/usb/command_parser.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/command_parser.c  -o ${OBJECTDIR}/usb/command_parser.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/command_parser.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/command_parser.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/rasterscan.o: rasterscan.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/rasterscan.o.d 
	@${RM} ${OBJECTDIR}/rasterscan.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  rasterscan.c  -o ${OBJECTDIR}/rasterscan.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/rasterscan.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/rasterscan.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -O0 -falign-arrays -I"." -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    hid_boot_p24FJ256GB210.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG   -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x800:0x81B -mreserve=data@0x81C:0x81D -mreserve=data@0x81E:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x827 -mreserve=data@0x82A:0x84F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   hid_boot_p24FJ256GB210.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/mc_firmware.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/PIC24FJ256GB210_PIM
	${RM} -r dist/PIC24FJ256GB210_PIM

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
