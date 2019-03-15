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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=usb/usb_device.c usb/usb_device_hid.c usb/usb_descriptors.c app_hid_boot_loader.c system.c app_led_usb_status.c main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/usb/usb_device.o ${OBJECTDIR}/usb/usb_device_hid.o ${OBJECTDIR}/usb/usb_descriptors.o ${OBJECTDIR}/app_hid_boot_loader.o ${OBJECTDIR}/system.o ${OBJECTDIR}/app_led_usb_status.o ${OBJECTDIR}/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/usb/usb_device.o.d ${OBJECTDIR}/usb/usb_device_hid.o.d ${OBJECTDIR}/usb/usb_descriptors.o.d ${OBJECTDIR}/app_hid_boot_loader.o.d ${OBJECTDIR}/system.o.d ${OBJECTDIR}/app_led_usb_status.o.d ${OBJECTDIR}/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/usb/usb_device.o ${OBJECTDIR}/usb/usb_device_hid.o ${OBJECTDIR}/usb/usb_descriptors.o ${OBJECTDIR}/app_hid_boot_loader.o ${OBJECTDIR}/system.o ${OBJECTDIR}/app_led_usb_status.o ${OBJECTDIR}/main.o

# Source Files
SOURCEFILES=usb/usb_device.c usb/usb_device_hid.c usb/usb_descriptors.c app_hid_boot_loader.c system.c app_led_usb_status.c main.c


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
	${MAKE}  -f nbproject/Makefile-PIC24FJ256GB210_PIM.mk dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ256GB210
MP_LINKER_FILE_OPTION=,--script="hid_boot_p24FJ256GB210.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/usb/usb_device.o: usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device.c  -o ${OBJECTDIR}/usb/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_device_hid.o: usb/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device_hid.c  -o ${OBJECTDIR}/usb/usb_device_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device_hid.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_descriptors.o: usb/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_descriptors.c  -o ${OBJECTDIR}/usb/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_descriptors.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/app_hid_boot_loader.o: app_hid_boot_loader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_hid_boot_loader.o.d 
	@${RM} ${OBJECTDIR}/app_hid_boot_loader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app_hid_boot_loader.c  -o ${OBJECTDIR}/app_hid_boot_loader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/app_hid_boot_loader.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/app_hid_boot_loader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system.o: system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/system.o.d 
	@${RM} ${OBJECTDIR}/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system.c  -o ${OBJECTDIR}/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/app_led_usb_status.o: app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_led_usb_status.o.d 
	@${RM} ${OBJECTDIR}/app_led_usb_status.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app_led_usb_status.c  -o ${OBJECTDIR}/app_led_usb_status.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/app_led_usb_status.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/app_led_usb_status.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/usb/usb_device.o: usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device.c  -o ${OBJECTDIR}/usb/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_device_hid.o: usb/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_device_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_device_hid.c  -o ${OBJECTDIR}/usb/usb_device_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_device_hid.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_device_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/usb/usb_descriptors.o: usb/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/usb" 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/usb/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  usb/usb_descriptors.c  -o ${OBJECTDIR}/usb/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/usb/usb_descriptors.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/usb/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/app_hid_boot_loader.o: app_hid_boot_loader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_hid_boot_loader.o.d 
	@${RM} ${OBJECTDIR}/app_hid_boot_loader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app_hid_boot_loader.c  -o ${OBJECTDIR}/app_hid_boot_loader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/app_hid_boot_loader.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/app_hid_boot_loader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/system.o: system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/system.o.d 
	@${RM} ${OBJECTDIR}/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  system.c  -o ${OBJECTDIR}/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/system.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/app_led_usb_status.o: app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_led_usb_status.o.d 
	@${RM} ${OBJECTDIR}/app_led_usb_status.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  app_led_usb_status.c  -o ${OBJECTDIR}/app_led_usb_status.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/app_led_usb_status.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/app_led_usb_status.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -fno-short-double -Os -falign-arrays -I"." -I"usb" -DPIC24FJ256GB210_PIM -msmart-io=1 -msfr-warn=off  
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
dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    hid_boot_p24FJ256GB210.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG   -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x800:0x81B -mreserve=data@0x81C:0x81D -mreserve=data@0x81E:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x827 -mreserve=data@0x82A:0x84F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,-D__BOOTLOADER,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   hid_boot_p24FJ256GB210.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_PIC24FJ256GB210_PIM=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,-D__BOOTLOADER,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/mc_bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
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
