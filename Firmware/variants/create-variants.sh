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
# MOD 		= Modification			: BE   = Bondtech Extruder for Prusa
#                                     BM   = Bondtech Extruder for Prusa with Mosquito Hotend
#                                     BMH  = Bondetch Extruder for Prusa with Mosquito and Slice Hight temp Thermistor
#                                     BMM  = Bondtech Extruder for Prusa with Mosquito Magnum Hotend
#                                     BMMH = Bondetch Extruder for Prusa with Mosquito Magnum and Slice Hight temp Thermistor
# TypesArray is an array of printer types
# HeightsArray is an array of printer hights
# ModArray is an array of printer mods
#
#
# Version 1.0.7
################################################################################
# 3 Jul 2019, vertigo235, Inital varaiants script
# 8 Aug 2019, 3d-gussner, Modified for Zaribo needs
# 14 Sep 2019, 3d-gussner, Added MOD BMSQ and BMSQHT Bondtech Mosquito / High Temperature
# 20 Sep 2019, 3d-gussner, New Naming convention for the variants.
#                          As we just support EINSy10a and RAMBo13a boards
# 01 Oct 2019, 3d-gussner, Fixed MK2.5 issue
# 12 Oct 2019, 3d-gussner, Add OLED display support
# 12 Nov 2019, 3d-gussner, Update Bondtech Extruder variants, as they are longer than Zaribo Extruder
#                          Also implementing Bondtech Mosquito MMU length settings
# 14 Nov 2019, 3d-gussner, Merge OLED as default
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
HEIGHT=210
BASE="1_75mm_$TYPE-$BOARD-E3Dv6full.h"
BMGHeightDiff=-3 #Bondtech extruders are bit higher than stock one

# Arrays
declare -a TypesArray=( "MK3" "MK3S" "MK25" "MK25S" )
declare -a HeightsArray=( 220 320 420)
declare -a ModArray=( "BE" "BM" "BMM" "BMH" "BMMH")
#

# Cleanup old Zaribo variants
ls Zaribo*
echo " "
echo "Existing Zaribo varaiants will be deleted. Press CRTL+C to stop"
sleep 5
rm Zaribo*

##### MK25/MK25S/MK3/MK3S Variants
echo "Start Zaribo"
for TYPE in ${TypesArray[@]}; do
	echo "Type: $TYPE"
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
		# Modify printer name
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
echo "End Zaribo"

## MODS
echo "Start BE"
MOD="BE" ##Bondtech Prusa Edition Extruder for MK25/MK25S/MK3/MK3S
for TYPE in ${TypesArray[@]}; do
	echo "Type: $TYPE Mod: $MOD"
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
		BMGHEIGHT=$(( $HEIGHT + $BMGHeightDiff ))
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		#echo $BMGHEIGHT
		echo $VARIANT
		# Modify printer name
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Printer Height
		sed -i -e "s/^#define Z_MAX_POS ${HEIGHT}*/#define Z_MAX_POS ${BMGHEIGHT}/g" ${VARIANT}
		# E Steps 
		sed -i -e 's/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,280}*/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,415}/' ${VARIANT}
		# Microsteps
		sed -i -e 's/#define TMC2130_USTEPS_E    32*/#define TMC2130_USTEPS_E    16/' ${VARIANT}
		# Filament Load Distances (BPE gears are farther from the hotend)
		sed -i -e 's/#define LOAD_FILAMENT_1 "G1 E70 F100"*/#define LOAD_FILAMENT_2 "G1 E80 F100"/' ${VARIANT}
		sed -i -e 's/#define UNLOAD_FILAMENT_1 "G1 E-80 F7000"*/#define UNLOAD_FILAMENT_1 "G1 E-95 F7000"/' ${VARIANT}
		sed -i -e 's/#define FILAMENTCHANGE_FINALRETRACT -80*/#define FILAMENTCHANGE_FINALRETRACT -95/' ${VARIANT}
		sed -i -e 's/#define FILAMENTCHANGE_FINALFEED 70*/#define FILAMENTCHANGE_FINALFEED 80/' ${VARIANT}
		# Display Type 
		sed -i -e "s/\/\/#define WEH002004_OLED*/#define WEH002004_OLED/g" ${VARIANT}
		# Enable Bondtech E3d MMU settings
		sed -i -e "s/\/\/#define BONDTECH_MK3S*/#define BONDTECH_MK3S/g" ${VARIANT}
	done
	echo
done
echo "End BE"

echo "Start BM"
BASE_MOD=BE
MOD="BM" ##Bondtech Prusa Mosquito Edition for MK2.5S and MK3S
declare -a BMQArray=( "MK3S" "MK25S")
for TYPE in ${BMQArray[@]}; do
	echo "Type: $TYPE Base_MOD: $BASE_MOD MOD: $MOD"
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
		BASE="Zaribo_$TYPE-$BASE_MOD-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		# Modify printer name
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$BASE_MOD'-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Hotend Type 
		sed -i -e 's/#define NOZZLE_TYPE "E3Dv6full"*/#define NOZZLE_TYPE "Mosquito"/' ${VARIANT}
		# Enable Bondtech Mosquito MMU settings
		sed -i -e "s/#define BONDTECH_MK3S*/\/\/#define BONDTECH_MK3S/g" ${VARIANT}
		sed -i -e "s/\/\/#define BONDTECH_MOSQUITO*/#define BONDTECH_MOSQUITO/g" ${VARIANT}
	done
	echo
done
echo "End BM"

echo "Start BMH"
BASE_MOD=BM
MOD="BMH" ##Bondtech Prusa Mosquito Edition for MK2.5S and MK3S with Slice High Temperature Thermistor
for TYPE in ${BMQArray[@]}; do
	echo "Type: $TYPE Base_MOD: $BASE_MOD MOD: $MOD"
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
		BASE="Zaribo_$TYPE-$BASE_MOD-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		# Modify printer name
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$BASE_MOD'-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Enable Slice High Temperature Thermistor
		sed -i -e "s/\/\/#define SLICE_HT_EXTRUDER*/#define SLICE_HT_EXTRUDER/g" ${VARIANT}
		# Change mintemp for Slice High Temperature Thermistor
		sed -i -e "s/#define HEATER_0_MINTEMP 15*/#define HEATER_0_MINTEMP 5/g" ${VARIANT}
	done
	echo
done
echo "End BMQH"

BASE_MOD=BM
MOD="BMM" ##Bondtech Prusa Mosquito Magnum Edition for MK2.5S and MK3S
declare -a BMGOArray=( "MK3S")
for TYPE in ${BMGOArray[@]}; do
	echo "Type: $TYPE Base_MOD: $BASE_MOD MOD: $MOD"
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
		BASE="Zaribo_$TYPE-$BASE_MOD-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		# Modify printer name
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$BASE_MOD'-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Hotend Type 
		sed -i -e 's/#define NOZZLE_TYPE "Mosquito"*/#define NOZZLE_TYPE "Mosquito Magnum"/' ${VARIANT}
		# Enable Bondtech Mosquito MMU settings
		sed -i -e "s/#define BONDTECH_MOSQUITO*/\/\/#define BONDTECH_MOSQUITO/g" ${VARIANT}
		sed -i -e "s/\/\/#define BONDTECH_MOSQUITO_MAGNUM*/#define BONDTECH_MOSQUITO_MAGNUM/g" ${VARIANT}
	done
	echo
done

BASE_MOD=BMM
MOD="BMMH" ##Bondtech Prusa Mosquito Magnum Edition with Slice High Temperature Thermistor
declare -a BMGOArray=( "MK3S")
for TYPE in ${BMGOArray[@]}; do
	echo "Type: $TYPE Base_MOD: $BASE_MOD MOD: $MOD"
	if [ "$TYPE" == "MK3S" ]; then
		BOARD="EINSy10a"
	else
		echo "Unsupported controller"
		exit 1
	fi
	for HEIGHT in ${HeightsArray[@]};
	do
		BASE="Zaribo_$TYPE-$BASE_MOD-$HEIGHT.h"
		VARIANT="Zaribo_$TYPE-$MOD-$HEIGHT.h"
		#echo $BASE
		#echo $TYPE
		#echo $HEIGHT
		echo $VARIANT
		cp ${BASE} ${VARIANT}
		sed -i -e 's/^#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$BASE_MOD'-'$HEIGHT'"*/#define CUSTOM_MENDEL_NAME "Zaribo '$TYPE'-'$MOD'-'$HEIGHT'"/g' ${VARIANT}
		# Enable Slice High Temperature Thermistor
		sed -i -e "s/\/\/#define SLICE_HT_EXTRUDER*/#define SLICE_HT_EXTRUDER/g" ${VARIANT}
		# Change mintemp for Slice High Temperature Thermistor
		sed -i -e "s/#define HEATER_0_MINTEMP 15*/#define HEATER_0_MINTEMP 5/g" ${VARIANT}
	done
	echo
done

