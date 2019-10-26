#!/bin/bash

################################################################################
# Description:
# Creates special variants
# TYPE		= Printer type			: MK25, MK25S, MK3, MK3S
#
# BOARD		= Controller board		: EINSy10a, RAMBo13a
# 
# HEIGHT	= Height of printer		: 210, 220, 320, 420
#
# MOD 		= Modification			: BMG   = Bondtech Extruder for Prusa
#                                     BMQ  = Bondtech Extruder for Prusa with Mosquito Hotend
#                                     BMQH  = Bondetch Extruder for Prusa with Mosquito and Slice Hight temp Thermistor
#                                     OLED  = WEH002004 OLED Display instead of HD77480 LCD screen
#                                     BMGO  = Bondtech Extruder for Prusa and WEH002004 OLED Dispaly
#                                     BMQO  = Bondtech Extruder for Prusa, Mosquito Hotend and WEH002004 OLED Dispaly
#                                     BQHO  = Bondtech Extruder for Prusa, Mosquito Hotend, Slice Hight temp Thermistor and WEH002004 OLED Dispaly
# TypesArray is an array of printer types
# HeightsArray is an array of printer hights
# ModArray is an array of printer mods
#
#
# Version 1.0.5
################################################################################
# 3 Jul 2019, vertigo235, Inital varaiants script
# 8 Aug 2019, 3d-gussner, Modified for Zaribo needs
# 14 Sep 2019, 3d-gussner, Added MOD BMSQ and BMSQHT Bondtech Mosquito / High Temperature
# 20 Sep 2019, 3d-gussner, New Naming convention for the variants.
#                          As we just support EINSy10a and RAMBo13a boards
# 01 Oct 2019, 3d-gussner, Fixed MK2.5 issue
# 12 Oct 2019, 3d-gussner, Add OLED display support
################################################################################

# Constants
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CURRENTDIR="$( pwd )"
TODAY=$(date +'%Y%m%d')
#
# Set default constants
TYPE="MK3"
MOD=""
BOARD="EINSy10a"
HEIGHT="210"
BASE="1_75mm_$TYPE-$BOARD-E3Dv6full.h"

# Arrays
declare -a TypesArray=( "MK3" "MK3S" "MK25" "MK25S" )
declare -a HeightsArray=( 220 320 420)
declare -a ModArray=( "BMG" "BMQ" "BMQH" "OLED" "BMGO" "BMQO" "BQHO")
#

##### MK25/MK25S/MK3/MK3S Variants

for TYPE in ${TypesArray[@]}; do
	echo $TYPE
	if [[ "$TYPE" == "MK3" || "$TYPE" == "MK3S" ]]; then
		BOARD="EINSy10a"
	elif [[ $TYPE == "MK25" || $TYPE == "MK25S" ]]; then
		BOARD="RAMBo13a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	BASE="1_75mm_$TYPE-$BOARD-E3Dv6full.h"
	for HEIGHT in ${HeightsArray[@]};
	do
		VARIANT="Zaribo_$TYPE-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		# Printer Display Name
		if [ $TYPE == "MK25" ]; then
			PRUSA_TYPE="MK2.5"
		elif [ $TYPE == "MK25S" ]; then
			PRUSA_TYPE="MK2.5S"
		else
			PRUSA_TYPE=$TYPE
		fi
		# Zaribo Branding
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Prusa i3 '$PRUSA_TYPE'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$HEIGHT'"/g' ${VARIANT}
		# Inverted Y-Motor
		sed -i -e "s/^#define INVERT_Y_DIR 0*/#define INVERT_Y_DIR 1/g" ${VARIANT}
		# Printer Height
		sed -i -e "s/^#define Z_MAX_POS 210*/#define Z_MAX_POS ${HEIGHT}/g" ${VARIANT}
		# Disable PSU_Delta
		sed -i -e "s/^#define PSU_Delta*/\/\/#define PSU_Delta/g" ${VARIANT}
	done
	echo
done

## MODS 
MOD="BMG" ##Bondtech Prusa Edition Extruder for MK25/MK25S/MK3/MK3S
for TYPE in ${TypesArray[@]}; do
	echo $TYPE
	if [[ "$TYPE" == "MK3" || "$TYPE" == "MK3S" ]]; then
		BOARD="EINSy10a"
	elif [[ $TYPE == "MK25" || $TYPE == "MK25S" ]]; then
		BOARD="RAMBo13a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	for HEIGHT in ${HeightsArray[@]};
	do
		BASE="Zaribo_$TYPE-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# E Steps 
		sed -i -e 's/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,280}*/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,415}/' ${VARIANT}
		# Microsteps
		sed -i -e 's/#define TMC2130_USTEPS_E    32*/#define TMC2130_USTEPS_E    16/' ${VARIANT}
		# Filament Load Distances (BPE gears are farther from the hotend)
		sed -i -e 's/#define LOAD_FILAMENT_2 "G1 E40 F100"*/#define LOAD_FILAMENT_2 "G1 E50 F100"/' ${VARIANT}
		sed -i -e 's/#define UNLOAD_FILAMENT_1 "G1 E-80 F7000"*/#define UNLOAD_FILAMENT_1 "G1 E-100 F7000"/' ${VARIANT}
		sed -i -e 's/#define FILAMENTCHANGE_FINALRETRACT -80*/#define FILAMENTCHANGE_FINALRETRACT -100/' ${VARIANT}
		sed -i -e 's/#define FILAMENTCHANGE_FINALFEED 25*/#define FILAMENTCHANGE_FINALFEED 35/' ${VARIANT}
	done
	echo
done

MOD="BMQ" ##Bondtech Prusa Mosquito Edition for MK2.5S and MK3S
declare -a BMQArray=( "MK3S" "MK25S")
for TYPE in ${BMSQArray[@]}; do
	echo $TYPE
	if [ "$TYPE" == "MK3S" ]; then
		BOARD="EINSy10a"
	elif [ $TYPE == "MK25S" ]; then
		BOARD="RAMBo13a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	for HEIGHT in ${HeightsArray[@]};
	do
		BASE="Zaribo_$TYPE-BMG-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-BMG-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Hotend Type 
		sed -i -e 's/#define NOZZLE_TYPE "E3Dv6full"*/#define NOZZLE_TYPE "Mosquito"/' ${VARIANT}
	done
	echo
done

MOD="BMQH" ##Bondtech Prusa Mosquito Edition for MK2.5S and MK3S High Temperature
for TYPE in ${BMSQArray[@]}; do
	echo $TYPE
	if [ "$TYPE" == "MK3S" ]; then
		BOARD="EINSy10a"
	elif [ $TYPE == "MK25S" ]; then
		BOARD="RAMBo13a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	for HEIGHT in ${HeightsArray[@]};
	do
		BASE="Zaribo_$TYPE-BMQ-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-BMQ-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Enable Slice High Temperature Thermistor
		sed -i -e "s/\/\/#define SLICE_HT_EXTRUDER*/#define SLICE_HT_EXTRUDER/g" ${VARIANT}
	done
	echo
done

MOD="BMGO" ##Bondtech Prusa Mosquito Edition with WEH002004 OLED Display for MK3S
declare -a BMGOArray=( "MK3S")
for TYPE in ${BMGOArray[@]}; do
	echo $TYPE
	if [ "$TYPE" == "MK3S" ]; then
		BOARD="EINSy10a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	for HEIGHT in ${HeightsArray[@]};
	do
		BASE="Zaribo_$TYPE-BMG-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-BMG-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Display Type 
		sed -i -e "s/\/\/#define WEH002004_OLED*/#define WEH002004_OLED/g" ${VARIANT}
	done
	echo
done

MOD="OLED" ##Bondtech Prusa Mosquito Edition with WEH002004 OLED Display for MK3S
declare -a OLEDArray=( "MK3S")
for TYPE in ${OLEDArray[@]}; do
	echo $TYPE
	if [ "$TYPE" == "MK3S" ]; then
		BOARD="EINSy10a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	for HEIGHT in ${HeightsArray[@]};
	do
		BASE="Zaribo_$TYPE-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Display Type 
		sed -i -e "s/\/\/#define WEH002004_OLED*/#define WEH002004_OLED/g" ${VARIANT}
	done
	echo
done
MOD="BMQO" ##Bondtech Prusa Mosquito Edition with WEH002004 OLED Display for MK3S
declare -a BMGOArray=( "MK3S")
for TYPE in ${BMGOArray[@]}; do
	echo $TYPE
	if [ "$TYPE" == "MK3S" ]; then
		BOARD="EINSy10a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	for HEIGHT in ${HeightsArray[@]};
	do
		BASE="Zaribo_$TYPE-BMGO-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-BMG-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Display Type 
		sed -i -e "s/\/\/#define WEH002004_OLED*/#define WEH002004_OLED/g" ${VARIANT}
	done
	echo
done

