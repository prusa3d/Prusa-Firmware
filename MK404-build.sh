#!/bin/bash 
# This bash script is used to compile automatically the MK404 simulator
# 
# Supported OS:  Linux64 bit
#
# Linux:
# Linux Ubuntu
# 1. Follow these instructions
# 2. Open Ubuntu bash and get latest updates with 'sudo apt-get update'
# 3. Install latest updates with 'sudo apt-get upgrade'
# 4.
#
# Version: 0.1-Build_1
# Change log:
# 11 Feb 2021, 3d-gussner, Inital
# 11 Feb 2021, 3d-gussner, Optional flags to check for updates


while getopts c:u:?h flag
    do
        case "${flag}" in
            c) check_flag=${OPTARG};;
            u) update_flag=${OPTARG};;
            ?) help_flag=1;;
            h) help_flag=1;;
        esac
    done
echo "$check_flag"
echo "$update_flag"


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
		exit 1
	fi
else
	echo "$(tput setaf 1)This script doesn't support your Operating system!"
	echo "Please use Linux 64-bit"
	echo "Read the notes of build.sh$(tput sgr0)"
	exit 1
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
    exit 4
fi
#### End Check MK404 dependencies

#### Set build environment 
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
MK404_URL="https://github.com/vintagepc/MK404.git"
MK404_PATH="$SCRIPT_PATH/../MK404"
MK404_BUILD_PATH="$MK404_PATH/build"


# List few useful data
echo
echo "Script path :" $SCRIPT_PATH
echo "OS          :" $OS
echo "OS type     :" $TARGET_OS
echo ""
echo "MK404 path  :" $MK404_PATH

if [ ! -d $MK404_PATH ]; then
    git clone $MK404_URL $MK404_PATH
fi

    cd $MK404_PATH
    git submodule init
    git submodule update


mkdir -p $MK404_BUILD_PATH
if [ ! -f "$MK404_BUILD_PATH/Makefile" ]; then
    cmake -B $MK404_BUILD_PATH
fi

cd $MK404_BUILD_PATH
if [ ! -f "$MK404_BUILD_PATH/MK404" ]; then
    make
fi

if [ "$check_flag" == "1" ]; then
    current_version=$( command ./MK404 --version | grep "MK404" | cut -f 4 -d " ")
    echo "Current version: $current_version"
fi