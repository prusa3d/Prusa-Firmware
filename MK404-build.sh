#!/bin/bash 
# This bash script is used to compile automatically and run the MK404 simulator
# 
# Supported OS:  Linux64 bit
#
# Linux:
# Linux Ubuntu
# 1. Follow these instructions
# 2. Open Ubuntu bash and get latest updates with 'sudo apt-get update'
# 3. Install latest updates with 'sudo apt-get upgrade'
# 
#
# Version: 1.0.0-Build_18
# Change log:
# 11 Feb 2021, 3d-gussner, Inital
# 11 Feb 2021, 3d-gussner, Optional flags to check for updates
# 12 Feb 2021, 3d-gussner, Update cmake
# 13 Feb 2021, 3d-gussner, Auto build SD cards
# 18 Jun 2021, 3d-gussner, Documentation and version number
# 18 Jun 2021, 3d-gussner, Added some arguments and checks
# 18 Jun 2021, 3d-gussner, Default extrusion graphics to line. Thanks @vintagepc point it out
# 18 Jun 2021, 3d-gussner, Added -g 3 and 4 for more details extrusion lines
# 18 Jun 2021, 3d-gussner, Check for updates is default. Fix update if internet connection is lost.
# 21 Jun 2021, 3d-gussner, Change board_flash argument to 'y' and firmware_version to 'f'
# 25 Jan 2022, 3d-gussner, Allow upper and lower case in selection
#                          Add update option to release OR devel
# 01 Jul 2022, 3d-gussner, Limit atmegaMK404 boards mem to 8,16,32

#### Start: Failures
failures()
{
case "$1" in
    0) echo "$(tput setaf 2)MK404-build.sh finished with success$(tput sgr0)" ;;
    2) echo "$(tput setaf 1)Unsupported OS: Linux $(uname -m)" ; echo "Please refer to the notes of MK404-build.sh$(tput sgr0)" ; exit 2 ;;
    3) echo "$(tput setaf 1)This script doesn't support your Operating system!"; echo "Please use Linux 64-bit"; echo "Read the notes of MK404-build.sh$(tput sgr0)" ; exit 2 ;;
    4) echo "$(tput setaf 1)Some packages are missing please install these!$(tput sgr0)" ; exit 4 ;;
    5) echo "$(tput setaf 1)Wrong printer chosen.$(tput sgr0) Following Printers are supported: MK25, MK25S, MK3 and MK3S" ; exit 5 ;;
    6) echo "$(tput setaf 1)Unsupported board flash size chosen.$(tput sgr0) Only '256', '384', '512', '1024' and '32M' are allowed." ; exit 6 ;;
    7) echo "$(tput setaf 1)Unsupported board mem size chosen.$(tput sgr0) Only '8', '16' and '32' are allowed." ; exit 7 ;;
    8) echo "$(tput setaf 1)No firmware version file selected!$(tput sgr0)" ; echo "Add argument -f with path and hex filename to start MK404" ; exit 8 ;;
    9) echo "$(tput setaf 1)Tried to determine MK404 printer from hex file, but failed!$(tput sgr0)" ; "Add argument -p with 'MK25', 'MK25S', 'MK3' or 'MK3S' to start MK404" ; exit 9 ;;
    10) echo "$(tput setaf 1)Missing printer$(tput sgr0)" ; exit 10 ;;
esac
}
#### End: Failures

#### Start: Check options
##check_options()
##{
while getopts c:f:g:m:n:p:u:x:y:?h flag
    do
        case "${flag}" in
            c) check_flag=${OPTARG};;
            f) firmware_version_flag=${OPTARG};;
            g) mk404_graphics_flag=${OPTARG};;
            h) help_flag=1;;
            m) mk404_flag=${OPTARG};;
            n) new_build_flag=${OPTARG};;
            p) mk404_printer_flag=${OPTARG};;
            u) update_flag=${OPTARG};;
            x) board_mem_flag=${OPTARG};;
            y) board_flash_flag=${OPTARG};;
            ?) help_flag=1;;
        esac
    done
#Debug echos
#echo "c: $check_flag"
#echo "f: $firmware_version_flag"
#echo "g: $mk404_graphics_flag"
#echo "m: $mk404_flag"
#echo "n: $new_build_flag"
#echo "p: $mk404_printer_flag"
#echo "u: $update_flag"
#echo "x: $board_mem_flag"
#echo "y: $board_flash_flag"

# '?' 'h' argument usage and help
if [ "$help_flag" == "1" ] ; then
echo "***************************************"
echo "* MK404-build.sh Version: 1.0.0-Build_18 *"
echo "***************************************"
echo "Arguments:"
echo "$(tput setaf 2)-c$(tput sgr0) Check for update"
echo "$(tput setaf 2)-f$(tput sgr0) Prusa-Firmware version"
echo "$(tput setaf 2)-g$(tput sgr0) Start MK404 graphics"
echo "$(tput setaf 2)-h$(tput sgr0) Help"
echo "$(tput setaf 2)-m$(tput sgr0) Start MK404 sim"
echo "$(tput setaf 2)-n$(tput sgr0) Force new build"
echo "$(tput setaf 2)-p$(tput sgr0) MK404 Printer"
echo "$(tput setaf 2)-u$(tput sgr0) Update MK404"
echo "$(tput setaf 2)-x$(tput sgr0) Board memory size"
echo "$(tput setaf 2)-y$(tput sgr0) Board flash size"
echo "$(tput setaf 2)-?$(tput sgr0) Help"
echo 
echo "Brief USAGE:"
echo "  $(tput setaf 2)./MK404-build.sh$(tput sgr0)  [-c] [-f] [-g] [-m] [-n] [-p] [-u] [-v] [-x] [-h] [-?]"
echo
echo "  -c : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' yes"
echo "  -f : '$(tput setaf 2)path+file name$(tput sgr0)'"
echo "  -g : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' lite, '$(tput setaf 2)2$(tput sgr0)' fancy, '$(tput setaf 2)3$(tput sgr0)' lite with Quad_HR, '$(tput setaf 2)4$(tput sgr0)' fancy with Quad_HR"
echo "  -m : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' yes '$(tput setaf 2)2$(tput sgr0)' with MMU2"
echo "  -n : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' yes"
echo "  -p : '$(tput setaf 2)MK25$(tput sgr0)', '$(tput setaf 2)MK25S$(tput sgr0)', '$(tput setaf 2)MK3$(tput sgr0)' or '$(tput setaf 2)MK3S$(tput sgr0)'"
echo "  -u : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' release ', '$(tput setaf 2)2$(tput sgr0)' devel '"
echo "  -x : '$(tput setaf 2)8$(tput sgr0)',$(tput setaf 2)16$(tput sgr0)' or $(tput setaf 2)32$(tput sgr0)' Kb."
echo "  -y : '$(tput setaf 2)256$(tput sgr0)','$(tput setaf 2)384$(tput sgr0)','$(tput setaf 2)512$(tput sgr0)','$(tput setaf 2)1024$(tput sgr0)''$(tput setaf 2)32M$(tput sgr0)'"
echo
echo "Example:"
echo "  $(tput setaf 2)./MK404-build.sh -f 1$(tput sgr0)"
echo "  Will force an update and rebuild the MK404 SIM"
echo
echo "  $(tput setaf 2)./MK404-build.sh -m 1 -g 1 -f ../../../../Prusa-Firmware/PF-build-hex/FW3100-Build4481/BOARD_EINSY_1_0a/FW3100-Build4481-1_75mm_MK3S-EINSy10a-E3Dv6full.hex$(tput sgr0)"
echo "  Will start MK404 with Prusa_MK3S and Prusa-Firmware 3.10.0-Build4481"
exit 1
fi

#Check MK404 agruments
#Set Check for updates as default
check_flag=1
#Start: Check mk404_printer_flag
if [ ! -z $mk404_printer_flag ]; then
    if [[ "$mk404_printer_flag" == "MK3" || "$mk404_printer_flag" == "MK3S" || "$mk404_printer_flag" == "MK25" || "$mk404_printer_flag" == "MK25S" ]]; then
        MK404_PRINTER_TEMP=$mk404_printer_flag
    else
        failures 5
    fi
fi
#End: Check mk404_printer_flag

#Start: Check if Build is selected with argument '-f'
if [ ! -z "$board_flash_flag" ] ; then
    if [ "$board_flash_flag" == "256" ] ; then
        BOARD_FLASH="0x3FFFF"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_FLASH (hex)"
    elif [ "$board_flash_flag" == "384" ] ; then
        BOARD_FLASH="0x5FFFF"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_FLASH (hex)"
    elif [ "$board_flash_flag" == "512" ] ; then
        BOARD_FLASH="0x7FFFF"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_FLASH (hex)"
    elif [ "$board_flash_flag" == "1024" ] ; then
        BOARD_FLASH="0xFFFFF"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_FLASH (hex)"
    elif [[ "$board_flash_flag" == "32M" || "$board_flash_flag" == "32768" ]] ; then
        BOARD_FLASH="0x1FFFFFF"
        echo "Board flash size :    32 Mb, $BOARD_FLASH (hex)"
    else
        failures 6
    fi
fi
#End: Check if Build is selected with argument '-f'

#Start: Check if Build is selected with argument '-x'
if [ ! -z "$board_mem_flag" ] ; then
    if [ "$board_mem_flag" == "8" ] ; then
        BOARD_MEM="0x21FF"
        echo "Board mem size   :     $board_mem_flag Kb, $BOARD_MEM (hex)"
    elif [ "$board_mem_flag" == "16" ] ; then
        BOARD_MEM="0x3DFF"
        echo "Board mem size   :    $board_mem_flag Kb, $BOARD_MEM (hex)"
    elif [ "$board_mem_flag" == "32" ] ; then
        BOARD_MEM="0x7DFF"
        echo "Board mem size   :    $board_mem_flag Kb, $BOARD_MEM (hex)"
    else
        failures 7
    fi
fi
#End: Check if Build is selected with argument '-x'

#Start: Check if new build is selected
if [ "$new_build_flag" == "1" ]; then
    check_flag=1
fi
if [[ "$update_flag" == "1" || "$update_flag" == "2" ]]; then
    check_flag=1
fi
#End: Check if new build is selected

# Prepare run MK404
#Check MK404_Printer
if [ ! -z $firmware_version_flag ]; then
    MK404_PRINTER_TEMP=$(echo $firmware_version_flag | sed 's/\(.*\)\///' | grep 'MK3')
    if [ ! -z $MK404_PRINTER_TEMP ]; then
        MK404_PRINTER=MK3
    fi
    MK404_PRINTER_TEMP=$(echo $firmware_version_flag | sed 's/\(.*\)\///' | grep 'MK3S')
    if [ ! -z $MK404_PRINTER_TEMP ]; then
        MK404_PRINTER=MK3S
    fi
    MK404_PRINTER_TEMP=$(echo $firmware_version_flag | sed 's/\(.*\)\///' | grep 'MK25')
    if [ ! -z $MK404_PRINTER_TEMP ]; then
        MK404_PRINTER=MK25
    fi
    MK404_PRINTER_TEMP=$(echo $firmware_version_flag | sed 's/\(.*\)\///' | grep 'MK25S')
    if [ ! -z $MK404_PRINTER_TEMP ]; then
        MK404_PRINTER=MK25S
    fi
elif [[ ! -z $new_build_flag || ! -z $update_flag || ! -z $check_flag ]]; then
        echo "continue"
else
    failures 8
fi

if [[ -z $MK404_PRINTER && -z $new_build_flag && -z $update_flag && -z $check_flag ]]; then
    failures 9
fi

if [ ! -z $mk404_printer_flag ]; then
    if [ "$mk404_printer_flag" != "$MK404_PRINTER" ]; then
        echo "$(tput setaf 3)You defined a different printer type than the firmware!"
        echo "This can cause unexpected issues.$(tput sgr 0)"
        echo 
        PS3="Select $(tput setaf 2)printer$(tput sgr 0) you want to use."
        select which in "$(tput setaf 2)$MK404_PRINTER$(tput sgr 0)" "$mk404_printer_flag"; do
            case $which in
                $MK404_PRINTER)
                    echo "Set $MK404_PRINTER as printer"
                    break
                    ;;
                $mk404_printer_flag)
                    echo "Set $(tput setaf 3)$mk404_printer_flag$(tput sgr 0) as printer"
                    echo "$(tput setaf 3)This firmware file isn't correct for this printer!!!$(tput sgr 0)"
                    echo 
                    MK404_PRINTER=$mk404_printer_flag
                    read -p "Press Enter to continue."
                    break
                    ;;
                *)
                    break
                    ;;
            esac
        done
    fi
fi

if [[ -z $MK404_PRINTER && -z $new_build_flag && -z $update_flag && -z $check_flag ]]; then
    failures 10
fi

if [[ "$MK404_PRINTER" == "MK25" || "$MK404_PRINTER" == "MK25S" ]]; then
    MK404_PRINTER="${MK404_PRINTER}_mR13"
else
    if [ "$mk404_flag" == "2" ]; then # Check if MMU2 is selected only for MK3/S
        MK404_PRINTER="${MK404_PRINTER}MMU2"
    fi
fi

# Run MK404 with 'debugcore' and/or 'bootloader-file'
    if [[ ! -z $MK404_DEBUG && "$MK404_DEBUG" == "atmega404" || ! -z $BOARD_MEM && "$BOARD_MEM" == "0xFFFF" ]]; then
        MK404_options="--debugcore"
    fi
    if [[ ! -z $MK404_DEBUG && "$MK404_DEBUG" == "atmega404_no_bootloader"  || ! -z $BOARD_FLASH && "$BOARD_FLASH" != "0x3FFFF" ]]; then
        MK404_options='--debugcore --bootloader-file ""'
    fi

# Run MK404 with graphics
    if [ ! -z "$mk404_graphics_flag" ]; then
        if [ ! -z "$MK404_options" ]; then
            MK404_options="${MK404_options} -g "
        else
            MK404_options=" -g "
        fi
        if [[ "$mk404_graphics_flag" == "1" || "$mk404_graphics_flag" == "lite" || "$mk404_graphics_flag" == "3" ]]; then
            MK404_options="${MK404_options}lite"
        elif [[ "$mk404_graphics_flag" == "2" || "$mk404_graphics_flag" == "fancy" || "$mk404_graphics_flag" == "4" ]]; then
            MK404_options="${MK404_options}fancy"
        else
            echo "$(tput setaf 1)Unsupported MK404 graphics option $mk404_graphics_flag$(tput sgr 0)"
        fi
        if [[ "$mk404_graphics_flag" == "3" || "$mk404_graphics_flag" == "4" ]]; then
            MK404_options="${MK404_options} --colour-extrusion --extrusion Quad_HR"
        else
            MK404_options="${MK404_options} --extrusion Line"
        fi
    fi
if [ ! -z $firmware_version_flag ]; then
    MK404_firmware_file=" -f $firmware_version_flag"
fi

#End: Check MK404 agruments
##}
#### End: Check for options/flags

#### Start: Check if OSTYPE is supported
check_OS()
{
OS_FOUND=$( command -v uname)
case $( "${OS_FOUND}" | tr '[:upper:]' '[:lower:]') in
  linux*)
    TARGET_OS="linux"
   ;;
  *)
    TARGET_OS='unknown'
    ;;
esac
# Linux
if [ $TARGET_OS == "linux" ]; then
    if [ $(uname -m) == "x86_64" ]; then
        echo "$(tput setaf 2)Linux 64-bit found$(tput sgr0)"
        Processor="64"
    #elif [[ $(uname -m) == "i386" || $(uname -m) == "i686" ]]; then
    #    echo "$(tput setaf 2)Linux 32-bit found$(tput sgr0)"
    #    Processor="32"
    else
        failures 2
    fi
else
    failures 3 
fi
sleep 2
}
#### End: Check if OSTYPE is supported

#### Start: Check MK404 dependencies
check_packages()
{
packages=(
"libelf-dev"
"gcc"
"gcc-avr"
"libglew-dev"
"freeglut3-dev"
"libsdl-sound1.2-dev"
"libpng-dev"
"cmake"
"zip"
"wget"
"git"
"build-essential"
"lcov"
"mtools"
)

for check_package in ${packages[@]}; do
    if dpkg-query -W -f'${db:Status-Abbrev}\n' $check_package 2>/dev/null \
 | grep -q '^.i $'; then
        echo "$(tput setaf 2)$check_package: Installed$(tput sgr0)"
    else
        echo "$(tput setaf 1)$check_package: Not installed use $(tput setaf 3)'sudo apt install $check_package'$(tput setaf 1) to install missing package$(tput sgr0)"
        not_installed=1;
    fi
done

if [ "$not_installed" = "1" ]; then
    failures 4
fi
}
#### End: Check MK404 dependencies

#### Start: Set build environment 
set_build_env_variables()
{
MK404_SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
MK404_URL="https://github.com/vintagepc/MK404.git"
MK404_owner="vintagepc"
MK404_project="MK404"
MK404_PATH="$MK404_SCRIPT_PATH/../MK404/master"
MK404_BUILD_PATH="$MK404_PATH/build"
}
#### End: Set build environment

#### Start: List few useful data
output_useful_data()
{
echo
echo "Script path :" $MK404_SCRIPT_PATH
echo "OS          :" $TARGET_OS
echo ""
echo "MK404 path  :" $MK404_PATH
}
#### End: List few useful data

#### Start: Clone MK404 if needed
get_MK404()
{
if [ ! -d $MK404_PATH ]; then
    #release_url=$(curl -Ls -o /dev/null -w %{url_effective} https://github.com/$MK404_owner/$MK404_project/releases/latest)
    #release_tag=$(basename $release_url)
    #git clone -b $release_tag -- https://github.com/$MK404_owner/$MK404_project.git $MK404_PATH
    git clone $MK404_URL $MK404_PATH
fi
}
#### End: Clone MK404 if needed

#### Start: Check for updates
check_for_updates()
{
if [ "$check_flag" == "1" ]; then
    if [ -d $MK404_BUILD_PATH ]; then
        cd $MK404_BUILD_PATH
        MK404_current_version=$( command ./MK404 --version | grep "MK404" | cut -f 4 -d " ")
        cd $MK404_PATH
    else
        echo "Cannot check current version as it has not been build."
    fi
# Get local Commit_Hash
    MK404_local_GIT_COMMIT_HASH=$(git log --pretty=format:"%H" -1)
# Get local Commit_Number
    MK404_local_GIT_COMMIT_NUMBER=$(git rev-list HEAD --count)
# Get latest release
    MK404_release_url=$(curl -Ls -o /dev/null -w %{url_effective} https://github.com/$MK404_owner/$MK404_project/releases/latest)
    MK404_release_tag=$(basename $MK404_release_url)
# Get release Commit_Hash
    MK404_release_GIT_COMMIT_HASH=$(git ls-remote | grep "refs/tags/$MK404_release_tag"  | cut -f 1)
# Get release Commit_Number
    MK404_release_GIT_COMMIT_NUMBER=$(git rev-list $MK404_release_tag --count)
# Get latest development Commit_Hash
    MK404_devel_GIT_COMMIT_HASH=$(git for-each-ref refs/remotes/origin/master | cut -d" " -f 1)
# Get latest development Commit_Number
    MK404_devel_GIT_COMMIT_NUMBER=$(git rev-list refs/remotes/origin/master --count)
# Output
    echo ""
    echo "Current version         : $MK404_current_version"
    echo ""
    echo "Current local hash      : $MK404_local_GIT_COMMIT_HASH"
    echo "Current local commit nr : $MK404_local_GIT_COMMIT_NUMBER"
    if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_release_GIT_COMMIT_HASH" ]; then
        echo "$(tput setaf 1)"
    else
        echo "$(tput setaf 2)"
    fi
    echo "Latest release tag      : $MK404_release_tag"
    echo "Latest release hash     : $MK404_release_GIT_COMMIT_HASH"
    echo "Latest release commit nr: $MK404_release_GIT_COMMIT_NUMBER"
    if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_devel_GIT_COMMIT_HASH" ]; then
        echo "$(tput setaf 1)"
    else
        echo "$(tput setaf 2)"
    fi
    echo "Latest devel hash       : $MK404_devel_GIT_COMMIT_HASH"
    echo "Latest devel commit nr  : $MK404_devel_GIT_COMMIT_NUMBER"
    echo "$(tput sgr 0)"

# Check for updates
    if [ ! -z $MK404_release_GIT_COMMIT_HASH ]; then
        if [[ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_release_GIT_COMMIT_HASH" && -z "$update_flag" ]]; then
            echo "$(tput setaf 2)Update to release is availible.$(tput sgr 0)"
            read -t 10 -n 1 -p "$(tput setaf 3)Update to release now Y/n$(tput sgr 0)" update_answer
            if [[ "$update_answer" == "Y" || "$update_answer" == "y" ]]; then
                update_flag=1
            fi
            echo ""
        fi
    fi
    if [ ! -z $MK404_devel_GIT_COMMIT_HASH ]; then
        if [[ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_devel_GIT_COMMIT_HASH" && -z "$update_flag" ]]; then
            echo "$(tput setaf 2)Update to devel is availible.$(tput sgr 0)"
            read -t 10 -n 1 -p "$(tput setaf 3)Update to devel now Y/n$(tput sgr 0)" update_answer
            if [[ "$update_answer" == "Y" || "$update_answer" == "y" ]]; then
                update_flag=2
            fi
            echo ""
        fi
    fi
fi
}
#### End: Check for updates

#### Start: Fetch updates and force new build
fetch_updates()
{
if [ "$update_flag" == "1" ]; then
    if [ ! -z $MK404_release_GIT_COMMIT_HASH ]; then
        if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_release_GIT_COMMIT_HASH" ]; then
            echo ""
            git fetch --all
            read -t 5 -p "$(tput setaf 2)Updating MK404 to release!$(tput sgr 0)"
            echo ""
            git reset --hard $MK404_release_tag
            read -t 5 -p "$(tput setaf 2)Compiling MK404 release!$(tput sgr 0)"
            echo ""
            new_build_flag=1
        fi
    fi
elif [ "$update_flag" == "2" ]; then
    if [ ! -z $MK404_devel_GIT_COMMIT_HASH ]; then
        if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_devel_GIT_COMMIT_HASH" ]; then
            echo ""
            git fetch --all
            read -t 5 -p "$(tput setaf 2)Updating MK404 to devel!$(tput sgr 0)"
            echo ""
            git reset --hard origin/master
            read -t 5 -p "$(tput setaf 2)Compiling MK404 devel!$(tput sgr 0)"
            echo ""
            new_build_flag=1
        fi
    fi
fi
}
#### End: Fetch updates and force new build

#### Start: Prepare MK404 build
prepare_MK404()
{
if [ ! -d $MK404_BUILD_PATH ]; then
    mkdir -p $MK404_BUILD_PATH
fi
}
#### End: Prepare MK404 build

#### Start: Build MK404
build_MK404()
{
if [[ ! -f "$MK404_BUILD_PATH/Makefile" || "$new_build_flag" == "1" ]]; then
# Init and update submodules
    if [ -d $MK404_BUILD_PATH ]; then
        rm -rf $MK404_BUILD_PATH
        mkdir -p $MK404_BUILD_PATH
    fi
    git submodule init
    git submodule update
    cmake -B$MK404_BUILD_PATH -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
fi

# Make MK404
cd $MK404_BUILD_PATH
if [[ ! -f "$MK404_BUILD_PATH/MK404" || "$new_build_flag" == "1" ]]; then
    make
fi

# Make SDcards
if [[ ! -f "$MK404_BUILD_PATH/Prusa_MK3S_SDcard.bin" || "$new_build_flag" == "1" ]]; then
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3S_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK25_13_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK25S_13_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3SMMU2_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3MMU2_SDcard.bin
fi
}
#### End: Build MK404


#### Start: Run MK404 SIM
run_MK404_SIM()
{
if [ ! -z $mk404_flag ]; then
    # Output some useful data
    echo "Printer     : $MK404_PRINTER"
    echo "Options     : $MK404_options"
    echo ""
    read -t 5 -p "Press $(tput setaf 2)Enter$(tput sgr 0) to start MK404"
    echo ""

    # Change to MK404 build folder
    cd $MK404_BUILD_PATH

    # Start MK404
    # default with serial output and terminal to manipulate it via terminal
    echo ""
    echo "./MK404 Prusa_$MK404_PRINTER -s --terminal $MK404_options $MK404_firmware_file"
    sleep 5
    ./MK404 Prusa_$MK404_PRINTER -s --terminal $MK404_options $MK404_firmware_file || exit 10
fi
}
#### End: Run MK404 SIM

#### Check OS and needed packages
echo "Check OS"
check_OS
check_packages

#### Check for options/flags
echo "Check for options"

#### Prepare build environment
echo "Prepare build env"
set_build_env_variables
output_useful_data
get_MK404

# 
cd $MK404_PATH

check_for_updates
fetch_updates
prepare_MK404
build_MK404
run_MK404_SIM
#### End of MK404 Simulator
