#!/bin/bash 
# This bash script is used to compile automatically the Prusa firmware with a dedicated build environment and settings
# 
# Supported OS: Windows 10, Linux64 bit
# Beta OS: Linux32 bit
#
# Linux:
#
# Windows:
# To execute this script you gonna need few things on your Windows machine
#
# Linux Subsystem Ubuntu
# 1. Follow these instructions
# 2. Open Ubuntu bash and get latest updates with 'apt-get update'
# 3. Install zip with 'apt-get install zip'
# 4. Install python3 with 'apt-get install python3'
# 5. Add command 'ln -sf /usr/bin/python3.5 /usr/bin/python' to link python3 to python.
#    Donnot istall 'python' as python 2.x has end of life see https://pythonclock.org/
# 6. Add at top of ~/.bashrc following lines by using 'sudo nano ~/.bashrc'
#
#    export OS="Linux"
#    export JAVA_TOOL_OPTIONS="-Djava.net.preferIPv4Stack=true"
#    export GPG_TTY=$(tty)
#
#    and confirm them. Restart Ubuntu bash
#
# Or GIT-BASH
# 1. Download and install the correct (64bit or 32bit) Git version https://git-scm.com/download/win
# 2. Also follow these instructions https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058
# 3. Download and install 7z-zip from its official website.
#    By default, it is installed under the directory /c/Program Files/7-Zip in Windows 10 as my case.
# 4. Run git Bash under Administrator privilege and navigate to the directory /c/Program Files/Git/mingw64/bin,
#    you can run the command ln -s /c/Program Files/7-Zip/7z.exe zip.exe
#
# Useful things to edit and compare your custom Firmware
# 1. Download and install current and correct (64bit or 32bit) Notepad++ version https://notepad-plus-plus.org/download
# 2. Another great tool to compare your custom mod and stock firmware is WinMerge http://winmerge.org/downloads/?lang=en
# 
# Example for MK3: open git bash and change to your Firmware directory 
# <username>@<machinename> MINGW64 /<drive>/path
# bash build.sh 1_75mm_MK3-EINSy10a-E3Dv6full
#
# Example for MK25: open git bash and change to your directory 
# gussner@WIN01 MINGW64 /d/Data/Prusa-Firmware/MK3
# bash build.sh 1_75mm_MK25-RAMBo13a-E3Dv6full
#
# The compiled hex files can be found in the folder above like from the example
# gussner@WIN01 MINGW64 /d/Data/Prusa-Firmware
# FW351-Build1778-1_75mm_MK25-RAMBo13a-E3Dv6full.hex
#
# Why make Arduino IDE portable?
# To have a distinguished Prusa Firmware build environment I decided to use Arduino IDE in portable mode.
# - Changes made to other Arduino instances do not change anything in this build environment.
#   By default Arduino IDE uses "users" and shared library folders which is useful as soon you update the Software.
#   But in this case we need a stable and defined build environment, so keep it separated it kind of important.
#   Some may argue that this is only used by a script, BUT as soon someone accidentally or on purpose starts Arduino IDE
#   it will use the default Arduino IDE folders and so can corrupt the build environment.
#
# Version: 1.0.6-Build_13
# Change log:
# 12 Jan 2019, 3d-gussner, Fixed "compiler.c.elf.flags=-w -Os -Wl,-u,vfprintf -lprintf_flt -lm -Wl,--gc-sections" in 'platform.txt'
# 16 Jan 2019, 3d-gussner, Build_2, Added development check to modify 'Configuration.h' to prevent unwanted LCD messages that Firmware is unknown
# 17 Jan 2019, 3d-gussner, Build_3, Check for OS Windows or Linux and use the right build environment
# 10 Feb 2019, ropaha, Pull Request, Select variant from list while using build.sh
# 10 Feb 2019, ropaha, change FW_DEV_VERSION automatically depending on FW_VERSION RC/BETA/ALPHA
# 10 Feb 2019, 3d-gussner, 1st tests with english only 
# 10 Feb 2019, ropaha, added compiling of all variants and english only
# 10 Feb 2019, 3d-gussner, Set OUTPUT_FOLDER for hex files
# 11 Feb 2019, 3d-gussner/ropaha, Minor changes and fixes
# 11 Feb 2019, 3d-gussner, Ready for RC
# 12 Feb 2019, 3d-gussner, Check if wget and zip are installed. Thanks to Bernd to point it out
# 12 Feb 2019, 3d-gussner, Changed OS check to OSTYPE as it is not supported on Ubuntu
#                          Also added different BUILD_ENV folders depending on OS used so Windows
#                          Users can use git-bash AND Windows Linux Subsystems at the same time
#                          Cleanup compiler flags is only depends on OS version.
# 12 Feb 2019, 3d-gussner, Added additional OSTYPE check
# 15 feb 2019, 3d-gussner, Added zip files for miniRAMbo multi language hex files
# 15 Feb 2019, 3d-gussner, Added more checks if
#                                              Compiled Hex-files
#                                              Configuration_prusa.h
#                                              language build files
#                                              multi language firmware files exist and clean them up
# 15 Feb 2019, 3d-gussner, Fixed selction GOLD/UNKNOWN DEV_STATUS for ALL variants builds, so you have to choose only once
# 15 Feb 2019, 3d-gussner, Added some colored output
# 15 Feb 2019, 3d-gussner, troubleshooting and minor fixes
# 16 Feb 2019, 3d-gussner, Script can be run using arguments
#                          $1 = variant, example "1_75mm_MK3-EINSy10a-E3Dv6full.h" at this moment it is not possible to use ALL
#                          $2 = multi language OR english only [ALL/EN_ONLY]
#                          $3 = development status [GOLD/RC/BETA/ALPHA/DEVEL/DEBUG]
#                          If one argument is wrong a list of valid one will be shown
# 13 Mar 2019, 3d-gussner, MKbel updated the linux build environment to version 1.0.2 with an Fix maximum firmware flash size.
#                          So did I
# 11 Jul 2019, deliopoulos,Updated to v1.0.6 as Prusa needs a new board definition for Firmware 3.8.x86_64
#						   - Splitted the Download of Windows Arduino IDE 1.8.5 and Prusa specific part
#                            --> less download volume needed and saves some time
#
# 13 Jul 2019, deliopoulos,Splitting of Ardunio IDE and Prusa parts also for Linux64
# 13 Jul 2019, 3d-gussner, Added Linux 32-bit version (untested yet)
#                          MacOS could be added in future if needs
# 14 Jul 2019, 3d-gussner, Update preferences and make it really portable
# 15 Jul 2019, 3d-gussner, New PF-build-env gihub branch
# 16 Jul 2019, 3d-gussner, New Arduino_boards github fork
# 17 Jul 2019, 3d-gussner, Final tests under Windows 10 and Linux Subsystem for Windows   
# 18 Jul 2019, 3d-gussner, Added python check
# 18 Jul 2019, deliopoulos, No need more for changing 'platform.txt' file as it comes with the Arduino Boards.
# 18 Jul 2019, deliopoulos, Modified 'PF_BUILD_FILE_URL' to use 'BUILD_ENV' variable
# 22 Jul 2019, 3d-gussner, Modiffied checks to check folder and/or installation output exists.
# 22 Jul 2019, 3d-gussner, Added check if Arduino IDE 1.8.5 boards have been updated
# 22 Jul 2019, 3d-gussner, Changed exit numbers 1-13 for prepare build env 21-28 for prepare compiling 31-36 compiling
# 22 Jul 2019, 3d-gussner, Changed BOARD_URL to DRracers respository after he pulled my PR https://github.com/DRracer/Arduino_Boards/pull/1
# 23 Jul 2019, 3d-gussner, Changed Build-env path to "PF-build-dl" as requested in PR https://github.com/prusa3d/Prusa-Firmware/pull/2028
#                          Changed Hex-files folder to PF-build-hex as requested in PR
# 23 Jul 2019, 3d-gussner, Added Finding OS version routine so supporting new OS should get easier
# 26 Jul 2019, 3d-gussner, Change JSON repository to prusa3d after PR https://github.com/prusa3d/Arduino_Boards/pull/1 was merged
# 23 Sep 2019, 3d-gussner, Prepare PF-build.sh for comming Prusa3d/Arduino_Boards version 1.0.2 Pull Request
# 17 Oct 2019, 3d-gussner, Changed folder and check file names to have seperated build enviroments depening on Arduino IDE version and
#                          board-versions.
# 15 Dec 2019, 3d-gussner, Prepare for switch to Prusa3d/PF-build-env repository
# 15 Dec 2019, 3d-gussner, Fix Audrino user preferences for the chosen board.
# 17 Dec 2019, 3d-gussner, Fix "timer0_fract = 0" warning by using Arduino_boards v1.0.3
#### Start check if OSTYPE is supported
OS_FOUND=$( command -v uname)

case $( "${OS_FOUND}" | tr '[:upper:]' '[:lower:]') in
  linux*)
    TARGET_OS="linux"
   ;;
  msys*|cygwin*|mingw*)
    # or possible 'bash on windows'
    TARGET_OS='windows'
   ;;
  nt|win*)
    TARGET_OS='windows'
    ;;
  *)
    TARGET_OS='unknown'
    ;;
esac
# Windows
if [ $TARGET_OS == "windows" ]; then
	if [ $(uname -m) == "x86_64" ]; then
		echo "$(tput setaf 2)Windows 64-bit found$(tput sgr0)"
		Processor="64"
	elif [ $(uname -m) == "i386" ]; then
		echo "$(tput setaf 2)Windows 32-bit found$(tput sgr0)"
		Processor="32"
	else
		echo "$(tput setaf 1)Unsupported OS: Windows $(uname -m)"
		echo "Please refer to the notes of build.sh$(tput sgr0)"
		exit 1
	fi
# Linux
elif [ $TARGET_OS == "linux" ]; then
	if [ $(uname -m) == "x86_64" ]; then
		echo "$(tput setaf 2)Linux 64-bit found$(tput sgr0)"
		Processor="64"
	elif [[ $(uname -m) == "i386" || $(uname -m) == "i686" ]]; then
		echo "$(tput setaf 2)Linux 32-bit found$(tput sgr0)"
		Processor="32"
	else
		echo "$(tput setaf 1)Unsupported OS: Linux $(uname -m)"
		echo "Please refer to the notes of build.sh$(tput sgr0)"
		exit 1
	fi
else
	echo "$(tput setaf 1)This script doesn't support your Operating system!"
	echo "Please use Linux 64-bit or Windows 10 64-bit with Linux subsystem / git-bash"
	echo "Read the notes of build.sh$(tput sgr0)"
	exit 1
fi
sleep 2
#### End check if OSTYPE is supported

#### Prepare bash environment and check if wget, zip and other needed things are available
# Check wget
if ! type wget > /dev/null; then
	echo "$(tput setaf 1)Missing 'wget' which is important to run this script"
	echo "Please follow these instructions https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058 to install wget$(tput sgr0)"
	exit 2
fi

# Check for zip
if ! type zip > /dev/null; then
	if [ $TARGET_OS == "windows" ]; then
		echo "$(tput setaf 1)Missing 'zip' which is important to run this script"
		echo "Download and install 7z-zip from its official website https://www.7-zip.org/"
		echo "By default, it is installed under the directory /c/Program Files/7-Zip in Windows 10 as my case."
		echo "Run git Bash under Administrator privilege and"
		echo "navigate to the directory /c/Program Files/Git/mingw64/bin,"
		echo "you can run the command $(tput setaf 2)ln -s /c/Program Files/7-Zip/7z.exe zip.exe$(tput sgr0)"
		exit 3
	elif [ $TARGET_OS == "linux" ]; then
		echo "$(tput setaf 1)Missing 'zip' which is important to run this script"
		echo "install it with the command $(tput setaf 2)'sudo apt-get install zip'$(tput sgr0)"
		exit 3
	fi
fi
# Check python ... needed during language build
if ! type python > /dev/null; then
	if [ $TARGET_OS == "windows" ]; then
		echo "$(tput setaf 1)Missing 'python' which is important to run this script"
		exit 4
	elif [ $TARGET_OS == "linux" ]; then
		echo "$(tput setaf 1)Missing 'python' which is important to run this script"
		echo "As Python 2.x will not be maintained from 2020 please,"
		echo "install it with the command $(tput setaf 2)'sudo apt-get install python3'."
		echo "Check which version of Python3 has been installed using 'ls /usr/bin/python3*'"
		echo "Use 'sudo ln -sf /usr/bin/python3.x /usr/bin/python' (where 'x' is your version number) to make it default.$(tput sgr0)"
		exit 4
	fi
fi

#### End prepare bash / Linux environment


#### Set build environment 
ARDUINO_ENV="1.8.5"
BUILD_ENV="1.0.6"
BOARD="prusa_einsy_rambo"
BOARD_PACKAGE_NAME="PrusaResearch"
BOARD_VERSION="1.0.3"
#BOARD_URL="https://raw.githubusercontent.com/3d-gussner/Arduino_Boards/Prusa_Merge_v1.0.3/IDE_Board_Manager/package_prusa3d_index.json"
BOARD_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/package_prusa3d_index.json"
BOARD_FILENAME="prusa3dboards"
#BOARD_FILE_URL="https://raw.githubusercontent.com/3d-gussner/Arduino_Boards/Prusa_Merge_v1.0.3/IDE_Board_Manager/prusa3dboards-1.0.3.tar.bz2"
BOARD_FILE_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/prusa3dboards-1.0.3.tar.bz2"
#PF_BUILD_FILE_URL="https://github.com/3d-gussner/PF-build-env-1/releases/download/$BUILD_ENV-WinLin/PF-build-env-WinLin-$BUILD_ENV.zip"
PF_BUILD_FILE_URL="https://github.com/prusa3d/PF-build-env/releases/download/$BUILD_ENV-WinLin/PF-build-env-WinLin-$BUILD_ENV.zip"
LIB="PrusaLibrary"
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"

# List few useful data
echo
echo "Script path :" $SCRIPT_PATH
echo "OS          :" $OS
echo "OS type     :" $TARGET_OS
echo ""
echo "Ardunio IDE :" $ARDUINO_ENV
echo "Build env   :" $BUILD_ENV
echo "Board       :" $BOARD
echo "Package name:" $BOARD_PACKAGE_NAME
echo "Board v.    :" $BOARD_VERSION
echo "Specific Lib:" $LIB
echo ""

#### Start prepare building environment

#Check if build exists and creates it if not
if [ ! -d "../PF-build-dl" ]; then
    mkdir ../PF-build-dl || exit 5
fi

cd ../PF-build-dl || exit 6
BUILD_ENV_PATH="$( cd "$(dirname "$0")" ; pwd -P )"

# Check if PF-build-env-<version> exists and downloads + creates it if not
# The build environment is based on the supported Arduino IDE portable version with some changes
if [ ! -d "../PF-build-env-$BUILD_ENV" ]; then
	echo "$(tput setaf 6)PF-build-env-$BUILD_ENV is missing ... creating it now for you$(tput sgr 0)"
	mkdir ../PF-build-env-$BUILD_ENV
	sleep 5
fi

# Download and extract supported Arduino IDE depending on OS
# Windows
if [ $TARGET_OS == "windows" ]; then
	if [ ! -f "arduino-$ARDUINO_ENV-windows.zip" ]; then
		echo "$(tput setaf 6)Downloading Windows 32/64-bit Arduino IDE portable...$(tput setaf 2)"
		sleep 2
		wget https://downloads.arduino.cc/arduino-$ARDUINO_ENV-windows.zip || exit 7
		echo "$(tput sgr 0)"
	fi
	if [[ ! -d "../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" && ! -e "../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]]; then
		echo "$(tput setaf 6)Unzipping Windows 32/64-bit Arduino IDE portable...$(tput setaf 2)"
		sleep 2
		unzip arduino-$ARDUINO_ENV-windows.zip -d ../PF-build-env-$BUILD_ENV || exit 7
		mv ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor
		echo "# arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" >> ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
		echo "$(tput sgr0)"
	fi
fi
# Linux
if [ $TARGET_OS == "linux" ]; then
# 32 or 64 bit version
	if [ ! -f "arduino-$ARDUINO_ENV-linux$Processor.tar.xz" ]; then
		echo "$(tput setaf 6)Downloading Linux $Processor Arduino IDE portable...$(tput setaf 2)"
		sleep 2
		wget --no-check-certificate https://downloads.arduino.cc/arduino-$ARDUINO_ENV-linux$Processor.tar.xz || exit 8
		echo "$(tput sgr 0)"
	fi
	if [[ ! -d "../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" && ! -e "../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]]; then
		echo "$(tput setaf 6)Unzipping Linux $Processor Arduino IDE portable...$(tput setaf 2)"
		sleep 2
		tar -xvf arduino-$ARDUINO_ENV-linux$Processor.tar.xz -C ../PF-build-env-$BUILD_ENV/ || exit 8
		mv ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor
		echo "# arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" >> ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
		echo "$(tput sgr0)"
	fi
fi
# Make Arduino IDE portable
if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/
fi

if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/output/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/output
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook/libraries/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook/libraries
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/staging/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/staging
fi

# Change Arduino IDE preferences
if [ ! -e ../PF-build-env-$BUILD_ENV/Preferences-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt ]; then
	echo "$(tput setaf 6)Setting $ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor Arduino IDE preferences for portable GUI usage...$(tput setaf 2)"
	sleep 2
	echo "update.check"
	sed -i 's/update.check = true/update.check = false/g' ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
	echo "board"
	sed -i "s/board = uno/board = $BOARD/g" ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
	echo "editor.linenumbers"
	sed -i 's/editor.linenumbers = false/editor.linenumbers = true/g' ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
	echo "boardsmanager.additional.urls"
	echo "boardsmanager.additional.urls=$BOARD_URL" >>../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
	echo "build.verbose=true" >>../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
	echo "compiler.cache_core=false" >>../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
	echo "compiler.warning_level=all" >>../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
	echo "# Preferences-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" >> ../PF-build-env-$BUILD_ENV/Preferences-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
	echo "$(tput sgr0)"
fi

# Download and extract Prusa Firmware related parts
# Download and extract PrusaResearchRambo board
if [ ! -f "$BOARD_FILENAME-$BOARD_VERSION.tar.bz2" ]; then
	echo "$(tput setaf 6)Downloading Prusa Research AVR MK3 RAMBo EINSy build environment...$(tput setaf 2)"
	sleep 2
	wget $BOARD_FILE_URL || exit 9
fi
if [[ ! -d "../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION" || ! -e "../PF-build-env-$BUILD_ENV/$BOARD_FILENAME-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]]; then
	echo "$(tput setaf 6)Unzipping $BOARD_PACKAGE_NAME Arduino IDE portable...$(tput setaf 2)"
	sleep 2
	tar -xvf $BOARD_FILENAME-$BOARD_VERSION.tar.bz2 -C ../PF-build-env-$BUILD_ENV/ || exit 10
	if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME ]; then
		mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME
	fi
	if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME ]; then
		mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME
	fi
	if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware ]; then
		mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware
	fi
	if [ ! -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr ]; then
		mkdir ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr
	fi
	
	mv ../PF-build-env-$BUILD_ENV/$BOARD_FILENAME-$BOARD_VERSION ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION
	echo "# $BOARD_FILENAME-$BOARD_VERSION" >> ../PF-build-env-$BUILD_ENV/$BOARD_FILENAME-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt

	echo "$(tput sgr 0)"
fi	

# Download and extract Prusa Firmware specific library files
if [ ! -f "PF-build-env-WinLin-$BUILD_ENV.zip" ]; then
	echo "$(tput setaf 6)Downloading Prusa Firmware build environment...$(tput setaf 2)"
	sleep 2
	wget $PF_BUILD_FILE_URL || exit 11
	echo "$(tput sgr 0)"
fi
if [ ! -e "../PF-build-env-$BUILD_ENV/PF-build-env-$BUILD_ENV-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]; then
	echo "$(tput setaf 6)Unzipping Prusa Firmware build environment...$(tput setaf 2)"
	sleep 2
	unzip -o PF-build-env-WinLin-$BUILD_ENV.zip -d ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor || exit 12
	echo "# PF-build-env-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor-$BUILD_ENV" >> ../PF-build-env-$BUILD_ENV/PF-build-env-$BUILD_ENV-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
	echo "$(tput sgr0)"
fi

# Check if User updated Arduino IDE 1.8.5 boardsmanager and tools
if [ -d "../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools" ]; then
	echo "$(tput setaf 6)Arduino IDE boards / tools have been manually updated...$"
	echo "Please don't update the 'Arduino AVR boards' as this will prevent running this script (tput setaf 2)"
	sleep 2
fi	
if [ -d "../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2" ]; then
	echo "$(tput setaf 6)PrusaReasearch compatible tools have been manually updated...$(tput setaf 2)"
	sleep 2
	echo "$(tput setaf 6)Copying Prusa Firmware build environment to manually updated boards / tools...$(tput setaf 2)"
	sleep 2
	cp -f ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/hardware/tools/avr/avr/lib/ldscripts/avr6.xn ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/avr/lib/ldscripts/avr6.xn
	echo "# PF-build-env-portable-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor-$BUILD_ENV" >> ../PF-build-env-$BUILD_ENV/PF-build-env-portable-$BUILD_ENV-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
	echo "$(tput sgr0)"
fi	
if [ -d "../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools/avr-gcc/5.4.0-atmel3.6.1-arduino2" ]; then
	echo "$(tput setaf 1)Arduino IDE tools have been updated manually to a non supported version!!!"
	echo "Delete ../PF-build-env-$BUILD_ENV and start the script again"
	echo "Script will not continue until this have been fixed $(tput setaf 2)"
	sleep 2
	echo "$(tput sgr0)"
	exit 13
fi


#### End prepare building


#### Start 
cd $SCRIPT_PATH

# First argument defines which variant of the Prusa Firmware will be compiled 
if [ -z "$1" ] ; then
	# Select which variant of the Prusa Firmware will be compiled, like
	PS3="Select a variant: "
	while IFS= read -r -d $'\0' f; do
		options[i++]="$f"
	done < <(find Firmware/variants/ -maxdepth 1 -type f -name "*.h" -print0 )
	select opt in "${options[@]}" "All" "Quit"; do
		case $opt in
			*.h)
				VARIANT=$(basename "$opt" ".h")
				VARIANTS[i++]="$opt"
				break
				;;
			"All")
				VARIANT="All"
				VARIANTS=${options[*]}
				break
				;;
			"Quit")
				echo "You chose to stop"
					exit
					;;
			*)
				echo "$(tput setaf 1)This is not a valid variant$(tput sgr0)"
				;;
		esac
	done
else
	if [ -f "$SCRIPT_PATH/Firmware/variants/$1" ] ; then 
		VARIANTS=$1
	else
		echo "$(tput setaf 1)$1 could not be found in Firmware/variants please choose a valid one$(tput setaf 2)"
		ls -1 $SCRIPT_PATH/Firmware/variants/*.h | xargs -n1 basename
		echo "$(tput sgr0)"
		exit 21
	fi
fi

#Second argument defines if it is an english only version. Known values EN_ONLY / ALL
#Check default language mode
MULTI_LANGUAGE_CHECK=$(grep --max-count=1 "^#define LANG_MODE *" $SCRIPT_PATH/Firmware/config.h|sed -e's/  */ /g'|cut -d ' ' -f3)

if [ -z "$2" ] ; then
	PS3="Select a language: "
	echo
	echo "Which lang-build do you want?"
	select yn in "Multi languages" "English only"; do
		case $yn in
			"Multi languages")
				LANGUAGES="ALL"
				sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              1/g" $SCRIPT_PATH/Firmware/config.h
				break
				;;
			"English only") 
				LANGUAGES="EN_ONLY"
				sed -i -- "s/^#define LANG_MODE *1/#define LANG_MODE              0/g" $SCRIPT_PATH/Firmware/config.h
				break
				;;
			*)
				echo "$(tput setaf 1)This is not a valid language$(tput sgr0)"
				;;
		esac
	done
else
	if [[ "$2" == "ALL" || "$2" == "EN_ONLY" ]] ; then
		LANGUAGES=$2
	else
		echo "$(tput setaf 1)Language argument is wrong!$(tput sgr0)"
		echo "Only $(tput setaf 2)'ALL'$(tput sgr0) or $(tput setaf 2)'EN_ONLY'$(tput sgr0) are allowed as 2nd argument!"
		exit 22
	fi
fi
#Check if DEV_STATUS is selected via argument 3
if [ ! -z "$3" ] ; then
	if [[ "$3" == "GOLD" || "$3" == "RC" || "$3" == "BETA" || "$3" == "ALPHA" || "$3" == "DEVEL" || "$3" == "DEBUG" ]] ; then
		DEV_STATUS_SELECTED=$3
	else
		echo "$(tput setaf 1)Development argument is wrong!$(tput sgr0)"
		echo "Only $(tput setaf 2)'GOLD', 'RC', 'BETA', 'ALPHA', 'DEVEL' or 'DEBUG'$(tput sgr0) are allowed as 3rd argument!$(tput sgr0)"
		exit 23
	fi
fi

#Set BUILD_ENV_PATH
cd ../PF-build-env-$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor || exit 24
BUILD_ENV_PATH="$( pwd -P )"

cd ../..

#Checkif BUILD_PATH exists and if not creates it
if [ ! -d "Prusa-Firmware-build" ]; then
    mkdir Prusa-Firmware-build  || exit 25
fi

#Set the BUILD_PATH for Arduino IDE
cd Prusa-Firmware-build || exit 26
BUILD_PATH="$( pwd -P )"

for v in ${VARIANTS[*]}
do
	VARIANT=$(basename "$v" ".h")
	# Find firmware version in Configuration.h file and use it to generate the hex filename
	FW=$(grep --max-count=1 "\bFW_VERSION\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d '"' -f2|sed 's/\.//g')
	# Find build version in Configuration.h file and use it to generate the hex filename
	BUILD=$(grep --max-count=1 "\bFW_COMMIT_NR\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d ' ' -f3)
	# Check if the motherboard is an EINSY and if so only one hex file will generated
	MOTHERBOARD=$(grep --max-count=1 "\bMOTHERBOARD\b" $SCRIPT_PATH/Firmware/variants/$VARIANT.h | sed -e's/  */ /g' |cut -d ' ' -f3)
	# Check development status
	DEV_CHECK=$(grep --max-count=1 "\bFW_VERSION\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d '"' -f2|sed 's/\.//g'|cut -d '-' -f2)
	if [ -z "$DEV_STATUS_SELECTED" ] ; then
		if [[ "$DEV_CHECK" == "RC1"  ||  "$DEV_CHECK" == "RC2" ]] ; then
			DEV_STATUS="RC"
		elif [[ "$DEV_CHECK" == "ALPHA" ]]; then
			DEV_STATUS="ALPHA"
		elif [[ "$DEV_CHECK" == "BETA" ]]; then
			DEV_STATUS="BETA"
		elif [[ "$DEV_CHECK" == "DEVEL" ]]; then
			DEV_STATUS="DEVEL"
		elif [[ "$DEV_CHECK" == "DEBUG" ]]; then
			DEV_STATUS="DEBUG"
		else
			DEV_STATUS="UNKNOWN"
			echo
			echo "$(tput setaf 5)DEV_STATUS is UNKNOWN. Do you wish to set DEV_STATUS to GOLD?$(tput sgr0)"
			PS3="Select YES only if source code is tested and trusted: "
			select yn in "Yes" "No"; do
				case $yn in
					Yes)
						DEV_STATUS="GOLD"
						DEV_STATUS_SELECTED="GOLD"
						break
						;;
					No) 
						DEV_STATUS="UNKNOWN"
						DEV_STATUS_SELECTED="UNKNOWN"
						break
						;;
					*)
						echo "$(tput setaf 1)This is not a valid DEV_STATUS$(tput sgr0)"
						;;
				esac
			done
		fi
	else
		DEV_STATUS=$DEV_STATUS_SELECTED
	fi
	#Prepare hex files folders
	if [ ! -d "$SCRIPT_PATH/../PF-build-hex/FW$FW-Build$BUILD/$MOTHERBOARD" ]; then
		mkdir -p $SCRIPT_PATH/../PF-build-hex/FW$FW-Build$BUILD/$MOTHERBOARD || exit 27
	fi
	OUTPUT_FOLDER="PF-build-hex/FW$FW-Build$BUILD/$MOTHERBOARD"
	
	#Check if exactly the same hexfile already exists
	if [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex"  &&  "$LANGUAGES" == "ALL" ]]; then
		echo ""
		ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex | xargs -n1 basename
		echo "$(tput setaf 6)This hex file to be compiled already exists! To cancel this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
		read -t 10 -p "Press Enter to continue..."
	elif [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-EN_ONLY.hex"  &&  "$LANGUAGES" == "EN_ONLY" ]]; then
		echo ""
		ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-EN_ONLY.hex | xargs -n1 basename
		echo "$(tput setaf 6)This hex file to be compiled already exists! To cancel this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
		read -t 10 -p "Press Enter to continue..."
	fi
	if [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip"  &&  "$LANGUAGES" == "ALL" ]]; then
		echo ""
		ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip | xargs -n1 basename
		echo "$(tput setaf 6)This zip file to be compiled already exists! To cancel this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
		read -t 10 -p "Press Enter to continue..."
	fi
	
	#List some useful data
	echo "$(tput setaf 2)$(tput setab 7) "
	echo "Variant    :" $VARIANT
	echo "Firmware   :" $FW
	echo "Build #    :" $BUILD
	echo "Dev Check  :" $DEV_CHECK
	echo "DEV Status :" $DEV_STATUS
	echo "Motherboard:" $MOTHERBOARD
	echo "Languages  :" $LANGUAGES
	echo "Hex-file Folder:" $OUTPUT_FOLDER
	echo "$(tput sgr0)"

	#Prepare Firmware to be compiled by copying variant as Configuration_prusa.h
	if [ ! -f "$SCRIPT_PATH/Firmware/Configuration_prusa.h" ]; then
		cp -f $SCRIPT_PATH/Firmware/variants/$VARIANT.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 28
	else
		echo "$(tput setaf 6)Configuration_prusa.h already exist it will be overwritten in 10 seconds by the chosen variant.$(tput sgr 0)"
		read -t 10 -p "Press Enter to continue..."
		cp -f $SCRIPT_PATH/Firmware/variants/$VARIANT.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 28
	fi

	#Prepare Configuration.h to use the correct FW_DEV_VERSION to prevent LCD messages when connecting with OctoPrint
	sed -i -- "s/#define FW_DEV_VERSION FW_VERSION_UNKNOWN/#define FW_DEV_VERSION FW_VERSION_$DEV_STATUS/g" $SCRIPT_PATH/Firmware/Configuration.h

	# set FW_REPOSITORY
	sed -i -- 's/#define FW_REPOSITORY "Unknown"/#define FW_REPOSITORY "Prusa3d"/g' $SCRIPT_PATH/Firmware/Configuration.h

	#Prepare english only or multilanguage version to be build
	if [ $LANGUAGES == "ALL" ]; then
		echo " "
		echo "Multi-language firmware will be built"
		echo " "
	else
		echo " "
		echo "English only language firmware will be built"
		echo " "
	fi
		
	#Check if compiler flags are set to Prusa specific needs for the rambo board.
#	if [ $TARGET_OS == "windows" ]; then
#		RAMBO_PLATFORM_FILE="PrusaResearchRambo/avr/platform.txt"
#	fi	
	
	#### End of Prepare building
		
	#### Start building
		
	export ARDUINO=$BUILD_ENV_PATH
	#echo $BUILD_ENV_PATH
	#export BUILDER=$ARDUINO/arduino-builder

	echo
	#read -t 5 -p "Press Enter..."
	echo 

	echo "Start to build Prusa Firmware ..."
	echo "Using variant $VARIANT$(tput setaf 3)"
	sleep 2
	#$BUILD_ENV_PATH/arduino-builder -dump-prefs -debug-level 10 -compile -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=$BOARD_PACKAGE_NAME:avr:$BOARD -build-path=$BUILD_PATH -warnings=all $SCRIPT_PATH/Firmware/Firmware.ino || exit 14
	$BUILD_ENV_PATH/arduino-builder -compile -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=$BOARD_PACKAGE_NAME:avr:$BOARD -build-path=$BUILD_PATH -warnings=all $SCRIPT_PATH/Firmware/Firmware.ino || exit 14
	echo "$(tput sgr 0)"

	if [ $LANGUAGES ==  "ALL" ]; then
		echo "$(tput setaf 2)"

		echo "Building multi language firmware" $MULTI_LANGUAGE_CHECK
		echo "$(tput sgr 0)"
		sleep 2
		cd $SCRIPT_PATH/lang
		echo "$(tput setaf 3)"
		./config.sh || exit 31
		echo "$(tput sgr 0)"
		# Check if previous languages and firmware build exist and if so clean them up
		if [ -f "lang_en.tmp" ]; then
			echo ""
			echo "$(tput setaf 6)Previous lang build files already exist these will be cleaned up in 10 seconds.$(tput sgr 0)"
			read -t 10 -p "Press Enter to continue..."
			echo "$(tput setaf 3)"
			./lang-clean.sh
			echo "$(tput sgr 0)"
		fi
		if [ -f "progmem.out" ]; then
			echo ""
			echo "$(tput setaf 6)Previous firmware build files already exist these will be cleaned up in 10 seconds.$(tput sgr 0)"
			read -t 10 -p "Press Enter to continue..."
			echo "$(tput setaf 3)"
			./fw-clean.sh
			echo "$(tput sgr 0)"
		fi
		# build languages
		echo "$(tput setaf 3)"
		./lang-build.sh || exit 32
		# Combine compiled firmware with languages 
		./fw-build.sh || exit 33
		echo "$(tput sgr 0)"
		# Check if the motherboard is an EINSY and if so only one hex file will generated
		MOTHERBOARD=$(grep --max-count=1 "\bMOTHERBOARD\b" $SCRIPT_PATH/Firmware/variants/$VARIANT.h | sed -e's/  */ /g' |cut -d ' ' -f3)
		# If the motherboard is an EINSY just copy one hexfile
		if [ "$MOTHERBOARD" = "BOARD_EINSY_1_0a" ]; then
			echo "$(tput setaf 2)Copying multi language firmware for MK3/Einsy board to PF-build-hex folder$(tput sgr 0)"
			cp -f firmware.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex
		else
			echo "$(tput setaf 2)Zip multi language firmware for MK2.5/miniRAMbo board to PF-build-hex folder$(tput sgr 0)"
			cp -f firmware_cz.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-cz.hex
			cp -f firmware_de.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-de.hex
			cp -f firmware_es.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-es.hex
			cp -f firmware_fr.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-fr.hex
			cp -f firmware_it.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-it.hex
			cp -f firmware_pl.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-pl.hex
			if [ $TARGET_OS == "windows" ]; then 
				zip a $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-??.hex
				rm $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-??.hex
			elif [ $TARGET_OS == "linux" ]; then
				zip -m -j ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-??.hex
			fi
		fi
		# Cleanup after build
		echo "$(tput setaf 3)"
		./fw-clean.sh || exit 34
		./lang-clean.sh || exit 35
		echo "$(tput sgr 0)"
	else
		echo "$(tput setaf 2)Copying English only firmware to PF-build-hex folder$(tput sgr 0)"
		cp -f $BUILD_PATH/Firmware.ino.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-EN_ONLY.hex || exit 34
	fi

	# Cleanup Firmware
	rm $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 36
	sed -i -- "s/^#define FW_DEV_VERSION FW_VERSION_$DEV_STATUS/#define FW_DEV_VERSION FW_VERSION_UNKNOWN/g" $SCRIPT_PATH/Firmware/Configuration.h
	sed -i -- 's/^#define FW_REPOSITORY "Prusa3d"/#define FW_REPOSITORY "Unknown"/g' $SCRIPT_PATH/Firmware/Configuration.h
	echo $MULTI_LANGUAGE_CHECK
	#sed -i -- "s/^#define LANG_MODE * /#define LANG_MODE              $MULTI_LANGUAGE_CHECK/g" $SCRIPT_PATH/Firmware/config.h
	sed -i -- "s/^#define LANG_MODE *1/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
	sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
	sleep 5
done

# Switch to hex path and list build files
cd $SCRIPT_PATH
cd ..
echo "$(tput setaf 2) "
echo " "
echo "Build done, please use Slic3rPE > 1.41.0 to upload the firmware"
echo "more information how to flash firmware https://www.prusa3d.com/drivers/ $(tput sgr 0)"
#### End building
