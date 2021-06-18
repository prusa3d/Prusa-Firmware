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
# Version: 1.0.0-Build_6
# Change log:
# 11 Feb 2021, 3d-gussner, Inital
# 11 Feb 2021, 3d-gussner, Optional flags to check for updates
# 12 Feb 2021, 3d-gussner, Update cmake
# 13 Feb 2021, 3d-gussner, Auto build SD cards
# 18 Jun 2021, 3d-gussner, Documentation and version number
# 18 Jun 2021, 3d-gussner, Added some arguments and checks

while getopts c:f:g:m:n:p:u:v:x:?h flag
    do
        case "${flag}" in
            c) check_flag=${OPTARG};;
            f) board_flash_flag=${OPTARG};;
            g) graphics_flag=${OPTARG};;
            h) help_flag=1;;
            m) mk404_flag=${OPTARG};;
            p) mk404_printer_flag=${OPTARG};;
            n) new_build_flag=${OPTARG};;
            u) update_flag=${OPTARG};;
            v) firmware_version_flag=${OPTARG};;
            x) board_mem_flag=${OPTARG};;
            ?) help_flag=1;;
        esac
    done
#Debug echos
#echo "$check_flag"
#echo "$update_flag"
#echo "$new_build_flag"
#echo "$mk404_flag"
#echo "$graphics_flag"
#echo "$mk404_printer_flag"

# '?' 'h' argument usage and help
if [ "$help_flag" == "1" ] ; then
echo "***************************************"
echo "* MK404-build.sh Version: 1.0.0-Build_6 *"
echo "***************************************"
echo "Arguments:"
echo "$(tput setaf 2)-c$(tput sgr0) Check for update '$(tput setaf 2)0$(tput sgr0)' no '$(tput setaf 2)1$(tput sgr0)' yes"
echo "$(tput setaf 2)-f$(tput sgr0) Board flash size '$(tput setaf 2)256$(tput sgr0)','$(tput setaf 2)384$(tput sgr0)','$(tput setaf 2)512$(tput sgr0)','$(tput setaf 2)1024$(tput sgr0)''$(tput setaf 2)32M$(tput sgr0)'"
echo "$(tput setaf 2)-g$(tput sgr0) Start MK404 grafics '$(tput setaf 2)0$(tput sgr0)' no '$(tput setaf 2)1$(tput sgr0)' lite '$(tput setaf 2)2$(tput sgr0)' fancy"
echo "$(tput setaf 2)-h$(tput sgr0) Help"
echo "$(tput setaf 2)-g$(tput sgr0) Start MK404 grafics '$(tput setaf 2)0$(tput sgr0)' no '$(tput setaf 2)1$(tput sgr0)' lite '$(tput setaf 2)2$(tput sgr0)' fancy"
echo "$(tput setaf 2)-m$(tput sgr0) Start MK404 sim '$(tput setaf 2)0$(tput sgr0)' no '$(tput setaf 2)1$(tput sgr0)' yes '$(tput setaf 2)2$(tput sgr0)' with MMU2"
echo "$(tput setaf 2)-n$(tput sgr0) Force new build '$(tput setaf 2)0$(tput sgr0)' no '$(tput setaf 2)1$(tput sgr0)' yes"
echo "$(tput setaf 2)-p$(tput sgr0) MK404 Printer '$(tput setaf 2)MK25$(tput sgr0)', '$(tput setaf 2)MK25S$(tput sgr0)', '$(tput setaf 2)MK3$(tput sgr0)' or '$(tput setaf 2)MK3S$(tput sgr0)'"
echo "$(tput setaf 2)-u$(tput sgr0) Start MK404 grafics '$(tput setaf 2)0$(tput sgr0)' no '$(tput setaf 2)1$(tput sgr0)' lite '$(tput setaf 2)2$(tput sgr0)' fancy"
echo "$(tput setaf 2)-v$(tput sgr0) Prusa-Firmware version '$(tput setaf 2)path+file name$(tput sgr0)'"
echo "$(tput setaf 2)-x$(tput sgr0) Board memory size '$(tput setaf 2)8$(tput sgr0)' or '$(tput setaf 2)64$(tput sgr0)' Kb."
echo "$(tput setaf 2)-?$(tput sgr0) Help"
echo 
echo "Brief USAGE:"
echo "  $(tput setaf 2)./MK404-build.sh$(tput sgr0)  [-c] [-f] [-g] [-m] [-n] [-p] [-u] [-h] [-?]"
echo
echo "Example:"
echo "  $(tput setaf 2)./MK404-build.sh -f 1$(tput sgr0)"
echo "  Will force an update and rebuild the MK404 SIM"
echo
echo "  $(tput setaf 2)./MK404-build.sh -m 1 -g 1 -v ../../../../Prusa-Firmware/PF-build-hex/FW3100-Build4481/BOAD_EINSY_1_0a/FW3100-Build4481-1_75mm_MK3S-EINSy10a-E3Dv6full.hex$(tput sgr0)"
echo "  Will start MK404 with Prusa_MK3S and Prusa-Firmware 3.10.0-Build4481"
exit 1
fi

#### Start check if OSTYPE is supported
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
	#	echo "$(tput setaf 2)Linux 32-bit found$(tput sgr0)"
	#	Processor="32"
	else
		echo "$(tput setaf 1)Unsupported OS: Linux $(uname -m)"
		echo "Please refer to the notes of build.sh$(tput sgr0)"
		exit 2
	fi
else
	echo "$(tput setaf 1)This script doesn't support your Operating system!"
	echo "Please use Linux 64-bit"
	echo "Read the notes of build.sh$(tput sgr0)"
	exit 2
fi
sleep 2
#### End check if OSTYPE is supported

#### Check MK404 dependencies
packages=(
"libelf-dev"
"gcc-7"
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
    exit 3
fi
#### End Check MK404 dependencies

#### Set build environment 
MK404_SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
MK404_URL="https://github.com/vintagepc/MK404.git"
MK404_owner="vintagepc"
MK404_project="MK404"
MK404_PATH="$MK404_SCRIPT_PATH/../MK404/master"
MK404_BUILD_PATH="$MK404_PATH/build"


# List few useful data
echo
echo "Script path :" $MK404_SCRIPT_PATH
echo "OS          :" $TARGET_OS
echo ""
echo "MK404 path  :" $MK404_PATH

# Clone MK404 if needed
if [ ! -d $MK404_PATH ]; then
    #release_url=$(curl -Ls -o /dev/null -w %{url_effective} https://github.com/$MK404_owner/$MK404_project/releases/latest)
    #release_tag=$(basename $release_url)
    #git clone -b $release_tag -- https://github.com/$MK404_owner/$MK404_project.git $MK404_PATH
    git clone $MK404_URL $MK404_PATH
fi

# 
cd $MK404_PATH

#Check MK404 agruments
#Check mk404_printer_flag
if [ ! -z $mk404_printer_flag ]; then
    if [[ "$mk404_printer_flag" == "MK3" || "$mk404_printer_flag" == "MK3S" || "$mk404_printer_flag" == "MK25" || "$mk404_printer_flag" == "MK25S" ]]; then
        MK404_PRINTER_TEMP=$mk404_printer_flag
    else
        echo "Following Printers are supported: MK25, MK25S, MK3 and MK3S"
        exit 4
    fi
fi

#Check if Build is selected with argument '-f'
if [ ! -z "$board_flash_flag" ] ; then
    if [ "$board_flash_flag" == "256" ] ; then
        BOARD_FLASH="0x3FFFF"
        BOARD_maximum_size="253952"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
    elif [ "$board_flash_flag" == "384" ] ; then
        BOARD_FLASH="0x5FFFF"
        BOARD_maximum_size="385024"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
    elif [ "$board_flash_flag" == "512" ] ; then
        BOARD_FLASH="0x7FFFF"
        BOARD_maximum_size="516096"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
    elif [ "$board_flash_flag" == "1024" ] ; then
        BOARD_FLASH="0xFFFFF"firmware_version_flag
        BOARD_maximum_size="1040384"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
    elif [[ "$board_flash_flag" == "32M" || "$board_flash_flag" == "32768" ]] ; then
        BOARD_FLASH="0x1FFFFFF"
        BOARD_maximum_size="33546240"
        echo "Board flash size :    32 Mb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
    else
        echo "Unsupported board flash size chosen. Only '256', '384', '512', '1024' and '32M' are allowed."
        exit 5
    fi
fi

#Check if Build is selected with argument '-x'
if [ ! -z "$board_mem_flag" ] ; then
    if [ "$board_mem_flag" == "8" ] ; then
        BOARD_MEM="0x21FF"
        echo "Board mem size   :     $board_mem_flag Kb, $BOARD_MEM (hex)"
    elif [ "$board_mem_flag" == "64" ] ; then
        BOARD_MEM="0xFFFF"
        echo "Board mem size   :    $board_mem_flag Kb, $BOARD_MEM (hex)"
    else
        echo "Unsupported board mem size chosen. Only '8', '64' are allowed."
        exit 6
    fi
fi


if [ "$new_build_flag" == "1" ]; then
    check_flag=1
    update_flag=1
fi
if [ "$update_flag" == "1" ]; then
    check_flag=1
fi

#End Check MK404 agruments

#Check for updates
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
# Get remote Commit_Hash
    MK404_remote_GIT_COMMIT_HASH=$(git ls-remote --heads $(git config --get remote.origin.url) | grep "refs/heads/master" | cut -f 1)
# Get remote Commit_Number
    MK404_remote_GIT_COMMIT_NUMBER=$(git rev-list origin/master --count)
# Output
    echo ""
    echo "Current version         : $MK404_current_version"
    echo ""
    echo "Current local hash      : $MK404_local_GIT_COMMIT_HASH"
    echo "Current local commit nr : $MK404_local_GIT_COMMIT_NUMBER"
    if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_remote_GIT_COMMIT_HASH" ]; then
        echo "$(tput setaf 1)"
    else
        echo "$(tput setaf 2)"
    fi
    echo "Current remote hash     : $MK404_remote_GIT_COMMIT_HASH"
    echo "Current remote commit nr: $MK404_remote_GIT_COMMIT_NUMBER"
    echo "$(tput sgr 0)"

# Check for updates
    if [[ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_remote_GIT_COMMIT_HASH" && -z "$update_flag" ]]; then
        echo "$(tput setaf 2)Update is availible.$(tput sgr 0)"
        read -t 10 -n 1 -p "$(tput setaf 3)Update now Y/n$(tput sgr 0)" update_answer
        if [ "$update_answer" == "Y" ]; then
            update_flag=1
        fi
        echo ""
    fi
fi

# Fetch updates and force new build
if [ "$update_flag" == "1" ]; then
    if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_remote_GIT_COMMIT_HASH" ]; then
        echo ""
        git fetch --all
        read -t 10 -p "$(tput setaf 2)Updating MK404 !$(tput sgr 0)"
        echo ""
        git reset --hard origin/master
        read -t 10 -p "$(tput setaf 2)Compiling MK404 !$(tput sgr 0)"
        echo ""
        new_build_flag=1
    fi
fi

# Prepare MK404
mkdir -p $MK404_BUILD_PATH
if [[ ! -f "$MK404_BUILD_PATH/Makefile" || "$new_build_flag" == "1" ]]; then
# Init and update submodules
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


# Prepare run MK404
#Check MK404_Printer
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
if [ -z "$MK404_PRINTER" ]; then
    echo "Tried to determine MK404 printer from hex file, but failed!"
    echo "Add argument -p with 'MK25', 'MK25S', 'MK3' or 'MK3S' to start MK404"
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

if [ -z $MK404_PRINTER ]; then
    exit 7
fi

if [[ "$MK404_PRINTER" == "MK25" || "$MK404_PRINTER" == "MK25S" ]]; then
    MK404_PRINTER="${MK404_PRINTER}_mR13"
else
    if [[ "$mk404_flag" == "2" || "$mk404_flag" == "MMU2" || "$mk404_flag" == "MMU2S" ]]; then # Check if MMU2 is selected only for MK3/S
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

# Run MK404 with grafics
    if [ ! -z "$graphics_flag" ]; then
        if [ ! -z "$MK404_options" ]; then
            MK404_options="${MK404_options} --colour-extrusion --extrusion Quad_HR -g "
        else
            MK404_options="--colour-extrusion --extrusion Quad_HR -g "
        fi
        if [[ "$graphics_flag" == "1" || "$graphics_flag" == "lite" ]]; then
            MK404_options="${MK404_options}lite"
        elif [[ "$graphics_flag" == "2" || "$graphics_flag" == "fancy" ]]; then
            MK404_options="${MK404_options}fancy"
        else
        echo "$(tput setaf 1)Unsupported MK404 graphics option $graphics_flag$(tput sgr 0)"
        fi
    fi
if [ ! -z $firmware_version_flag ]; then
    MK404_firmware_file=$firmware_version_flag
fi

#Run MK404 SIM
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
    echo "./MK404 Prusa_$MK404_PRINTER -s --terminal $MK404_options -f $MK404_firmware_file"
    sleep 5
    ./MK404 Prusa_$MK404_PRINTER -s --terminal $MK404_options -f $MK404_firmware_file || exit 8
fi
#### End of MK404 Simulator