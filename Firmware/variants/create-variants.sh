#!/bin/bash

################################################################################
# Description:
# Creates special variants
################################################################################

# Constants
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CURRENTDIR="$( pwd )"
TODAY=$(date +'%Y%m%d')

##### MK3 Variants
BASE="1_75mm_MK3-EINSy10a-E3Dv6full.h"

VARIANT="1_75mm_MK3-BEAR-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3 BEAR"/' ${VARIANT}

## Bondtech Prusa Extruder
VARIANT="1_75mm_MK3-Bondtech-16-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3 BPE"/' ${VARIANT}
# E Steps 
sed -i -e 's/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,280}*/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,415}/' ${VARIANT}
# Microsteps
sed -i -e 's/#define TMC2130_USTEPS_E    32*/#define TMC2130_USTEPS_E    16/' ${VARIANT}
# Filament Load Distances (BPE gears are farther from the hotend)
sed -i -e 's/#define LOAD_FILAMENT_2 "G1 E40 F100"*/#define LOAD_FILAMENT_2 "G1 E50 F100"/' ${VARIANT}
sed -i -e 's/#define UNLOAD_FILAMENT_1 "G1 E-80 F7000"*/#define UNLOAD_FILAMENT_1 "G1 E-100 F7000"/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALRETRACT -80*/#define FILAMENTCHANGE_FINALRETRACT -100/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALFEED 25*/#define FILAMENTCHANGE_FINALFEED 35/' ${VARIANT}

##### MK3S Variants
BASE="1_75mm_MK3S-EINSy10a-E3Dv6full.h"

## Bear
VARIANT="1_75mm_MK3S-BEAR-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3S"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3S BEAR"/' ${VARIANT}
sed -i -e 's/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 9.0*/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 2.0/' ${VARIANT}

## Bondtech Prusa Extruder
VARIANT="1_75mm_MK3S-Bondtech-16-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3S"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK3S BPE"/' ${VARIANT}
sed -i -e 's/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 9.0*/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 2.0/' ${VARIANT}
# E Steps 
sed -i -e 's/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,280}*/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,415}/' ${VARIANT}
# Microsteps
sed -i -e 's/#define TMC2130_USTEPS_E    32*/#define TMC2130_USTEPS_E    16/' ${VARIANT}
# Filament Load Distances (BPE gears are farther from the hotend)
sed -i -e 's/#define LOAD_FILAMENT_2 "G1 E40 F100"*/#define LOAD_FILAMENT_2 "G1 E50 F100"/' ${VARIANT}
sed -i -e 's/#define UNLOAD_FILAMENT_1 "G1 E-80 F7000"*/#define UNLOAD_FILAMENT_1 "G1 E-100 F7000"/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALRETRACT -80*/#define FILAMENTCHANGE_FINALRETRACT -100/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALFEED 25*/#define FILAMENTCHANGE_FINALFEED 35/' ${VARIANT}

##### MK2.5S Variants
#### RAMBo13a
## Bear
BASE="1_75mm_MK25S-RAMBo13a-E3Dv6full.h"
VARIANT="1_75mm_MK25S-BEAR-RAMBo13a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5S"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5S BEAR"/' ${VARIANT}
sed -i -e 's/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 9.0*/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 2.0/' ${VARIANT}

## Bondtech Prusa Extruder
VARIANT="1_75mm_MK25S-Bondtech-16-RAMBo13a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5S"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5S BPE"/' ${VARIANT}
sed -i -e 's/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 9.0*/#define Z_MAX_POS_XYZ_CALIBRATION_CORRECTION 2.0/' ${VARIANT}
# E Steps 
sed -i -e 's/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,133}*/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,415}/' ${VARIANT}
# Filament Load Distances (BPE gears are farther from the hotend)
sed -i -e 's/#define LOAD_FILAMENT_2 "G1 E40 F100"*/#define LOAD_FILAMENT_2 "G1 E50 F100"/' ${VARIANT}
sed -i -e 's/#define UNLOAD_FILAMENT_1 "G1 E-80 F7000"*/#define UNLOAD_FILAMENT_1 "G1 E-100 F7000"/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALRETRACT -80*/#define FILAMENTCHANGE_FINALRETRACT -100/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALFEED 25*/#define FILAMENTCHANGE_FINALFEED 35/' ${VARIANT}

#### RAMBo10a
## BEAR 
BASE="1_75mm_MK25S-BEAR-RAMBo13a-E3Dv6full.h"
VARIANT="1_75mm_MK25S-BEAR-RAMBo10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define MOTHERBOARD BOARD_RAMBO_MINI_1_3*/#define MOTHERBOARD BOARD_RAMBO_MINI_1_0/' ${VARIANT}

## Bondtech Prusa Extruder
BASE="1_75mm_MK25S-Bondtech-16-RAMBo13a-E3Dv6full.h"
VARIANT="1_75mm_MK25S-Bondtech-16-RAMBo10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define MOTHERBOARD BOARD_RAMBO_MINI_1_3*/#define MOTHERBOARD BOARD_RAMBO_MINI_1_0/' ${VARIANT}

##### MK25
#### RAMBo13a
## Bear
BASE="1_75mm_MK25-RAMBo13a-E3Dv6full.h"
VARIANT="1_75mm_MK25-BEAR-RAMBo13a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5 BEAR"/' ${VARIANT}

## Bondtech Prusa Extruder
VARIANT="1_75mm_MK25-Bondtech-16-RAMBo13a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5"*/#define CUSTOM_MENDEL_NAME "Prusa i3 MK2.5 BPE"/' ${VARIANT}
# E Steps 
sed -i -e 's/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,133}*/#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200\/8,415}/' ${VARIANT}
# Filament Load Distances (BPE gears are farther from the hotend)
sed -i -e 's/#define LOAD_FILAMENT_2 "G1 E40 F100"*/#define LOAD_FILAMENT_2 "G1 E50 F100"/' ${VARIANT}
sed -i -e 's/#define UNLOAD_FILAMENT_1 "G1 E-80 F7000"*/#define UNLOAD_FILAMENT_1 "G1 E-100 F7000"/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALRETRACT -80*/#define FILAMENTCHANGE_FINALRETRACT -100/' ${VARIANT}
sed -i -e 's/#define FILAMENTCHANGE_FINALFEED 25*/#define FILAMENTCHANGE_FINALFEED 35/' ${VARIANT}

#### RAMBo10a
## BEAR 
BASE="1_75mm_MK25-BEAR-RAMBo13a-E3Dv6full.h"
VARIANT="1_75mm_MK25-BEAR-RAMBo10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define MOTHERBOARD BOARD_RAMBO_MINI_1_3*/#define MOTHERBOARD BOARD_RAMBO_MINI_1_0/' ${VARIANT}

## Bondtech Prusa Extruder
BASE="1_75mm_MK25-Bondtech-16-RAMBo13a-E3Dv6full.h"
VARIANT="1_75mm_MK25-Bondtech-16-RAMBo10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define MOTHERBOARD BOARD_RAMBO_MINI_1_3*/#define MOTHERBOARD BOARD_RAMBO_MINI_1_0/' ${VARIANT}

##### 320Z Variants
## MK3
BASE="1_75mm_MK3-Bondtech-16-EINSy10a-E3Dv6full.h"
VARIANT="1_75mm_MK3-Bondtech-320Z-16-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define Z_MAX_POS 210*/#define Z_MAX_POS 310/' ${VARIANT}

BASE="1_75mm_MK3-EINSy10a-E3Dv6full.h"
VARIANT="1_75mm_MK3-320Z-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define Z_MAX_POS 210*/#define Z_MAX_POS 310/' ${VARIANT}

## MK3S
BASE="1_75mm_MK3S-Bondtech-16-EINSy10a-E3Dv6full.h"
VARIANT="1_75mm_MK3S-Bondtech-320Z-16-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define Z_MAX_POS 210*/#define Z_MAX_POS 310/' ${VARIANT}

BASE="1_75mm_MK3S-EINSy10a-E3Dv6full.h"
VARIANT="1_75mm_MK3S-320Z-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define Z_MAX_POS 210*/#define Z_MAX_POS 310/' ${VARIANT}

BASE="1_75mm_MK3S-BEAR-EINSy10a-E3Dv6full.h"
VARIANT="1_75mm_MK3S-BEAR-320Z-EINSy10a-E3Dv6full.h"
cp ${BASE} ${VARIANT}
sed -i -e 's/#define Z_MAX_POS 210*/#define Z_MAX_POS 310/' ${VARIANT}
