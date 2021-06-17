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
# Version: 0.1-Build_3
# Change log:
# 11 Feb 2021, 3d-gussner, Inital
# 11 Feb 2021, 3d-gussner, Optional flags to check for updates
# 12 Feb 2021, 3d-gussner, Update cmake
# 13 Feb 2021, 3d-gussner, Auto build SD cards

while getopts c:u:f:m:g:?h flag
    do
        case "${flag}" in
            c) check_flag=${OPTARG};;
            u) update_flag=${OPTARG};;
            f) force_flag=${OPTARG};;
            m) mk404_flag=${OPTARG};;
            g) graphics_flag=${OPTARG};;
            ?) help_flag=1;;
            h) help_flag=1;;
        esac
    done
echo "$check_flag"
echo "$update_flag"
echo "$force_flag"
echo "$mk404_flag"
echo "$graphics_flag"


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
MK404_SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
MK404_URL="https://github.com/vintagepc/MK404.git"
MK404_owner="vintagepc"
MK404_project="MK404"
MK404_PATH="$MK404_SCRIPT_PATH/../MK404/master"
MK404_BUILD_PATH="$MK404_PATH/build"


# List few useful data
echo
echo "Script path :" $MK404_SCRIPT_PATH
echo "OS          :" $OS
echo "OS type     :" $TARGET_OS
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

# Check for updates ... WIP

# Check MK404
if [ "$force_flag" == "1" ]; then
    check_flag=1
    update_flag=1
fi
if [ "$update_flag" == "1" ]; then
    check_flag=1
fi
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
    echo "Current version         : $MK404_current_version"
    echo ""
    echo "Current local hash      : $MK404_local_GIT_COMMIT_HASH"
    echo "Current local commit nr : $MK404_local_GIT_COMMIT_NUMBER"
    if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_remote_GIT_COMMIT_HASH" ]; then
        echo "$(tput setaf 1)"
    else
        echo "$(tput sgr 0)"
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
# Check for updates
if [ "$update_flag" == "1" ]; then
    if [ "$MK404_local_GIT_COMMIT_HASH" != "$MK404_remote_GIT_COMMIT_HASH" ]; then
        echo ""
        git fetch --all
        read -t 10 -p "$(tput setaf 2)Updating MK404 !$(tput sgr 0)"
        echo ""
        git reset --hard origin/master
        read -t 10 -p "$(tput setaf 2)Compiling MK404 !$(tput sgr 0)"
        echo ""
        force_flag=1
    fi
fi

# Prepare MK404
mkdir -p $MK404_BUILD_PATH
if [[ ! -f "$MK404_BUILD_PATH/Makefile" || "$force_flag" == "1" ]]; then
# Init and update submodules
    git submodule init
    git submodule update
    cmake -B$MK404_BUILD_PATH -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
fi

# Make MK404
cd $MK404_BUILD_PATH
if [[ ! -f "$MK404_BUILD_PATH/MK404" || "$force_flag" == "1" ]]; then
    make
fi

# Make SDcards
if [[ ! -f "$MK404_BUILD_PATH/Prusa_MK3S_SDcard.bin" || "$force_flag" == "1" ]]; then
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3S_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK25_13_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK25S_13_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3SMMU2_SDcard.bin
    cmake --build $MK404_BUILD_PATH --config Release --target Prusa_MK3MMU2_SDcard.bin
fi

