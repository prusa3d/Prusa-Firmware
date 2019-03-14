#!/bin/bash 
# This bash script is used to compile automatically the Prusa firmware with a dedecated build enviroment and settings
# 
# Supported OS: Windows 10, Linux64 bit
#
# Linux:
#
# Windows:
# To excecute this sciprt you gonna need few things on your Windows machine
#
# Linux Subsystem Ubuntu
# 1. Follow these instructions
# 2. Open Ubuntu bash and get latest updates with 'apt-get upgate'
# 3. Install zip with 'apt-get install zip'
# 4. Add at top of ~/.bashrc following lines by using 'sudo nano ~/.bashrc'
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
# Usefull things to edit and compare your custom Firmware
# 1. Download and install current and correct (64bit or 32bit) Notepad++ version https://notepad-plus-plus.org/download
# 2. Another great tool to compare your custom mod and stock firmware is WinMerge http://winmerge.org/downloads/?lang=en
# 
# Example for MK3: open git bash and chage to your Firmware directory 
# <username>@<machinename> MINGW64 /<drive>/path
# bash build.sh 1_75mm_MK3-EINSy10a-E3Dv6full
#
# Example for MK25: open git bash and chage to your directory 
# gussner@WIN01 MINGW64 /d/Data/Prusa-Firmware/MK3
# bash build.sh 1_75mm_MK25-RAMBo13a-E3Dv6full
#
# The compiled hex files can be found in the folder above like from the example
# gussner@WIN01 MINGW64 /d/Data/Prusa-Firmware
# FW351-Build1778-1_75mm_MK25-RAMBo13a-E3Dv6full.hex
#
# Version: 1.0.1-Build_8
# Change log:
# 12 Jan 2019, 3d-gussner, Fixed "compiler.c.elf.flags=-w -Os -Wl,-u,vfprintf -lprintf_flt -lm -Wl,--gc-sections" in 'platform.txt'
# 16 Jan 2019, 3d-gussner, Build_2, Added development check to modify 'Configuration.h' to prevent unwanted LCD messages that Firmware is uknown
# 17 Jan 2019, 3d-gussner, Build_3, Check for OS Windows or Linux and use the right build enviroment
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
# 13 Mar 2019, 3d-gussner, MKbel updated the linux build enviromentto version 1.0.2 with an Fix maximum firmware flash size.
#                          So did I


###Check if OSTYPE is supported
if [ $OSTYPE == "msys" ]; then
	if [ $(uname -m) == "x86_64" ]; then
		echo "$(tput setaf 2)Windows 64-bit found$(tput sgr0)"
	fi
elif [ $OSTYPE == "linux-gnu" ]; then
	if [ $(uname -m) == "x86_64" ]; then
		echo "$(tput setaf 2)Linux 64-bit found$(tput sgr0)"
	fi
else
	echo "$(tput setaf 1)This script doesn't support your Operating system!"
	echo "Please use Linux 64-bit or Windows 10 64-bit with Linux subsystem / git-bash"
	echo "Read the notes of build.sh$(tput sgr0)"
	exit
fi
sleep 2
###Prepare bash enviroment and check if wget and zip are availible
if ! type wget > /dev/null; then
	echo "$(tput setaf 1)Missing 'wget' which is important to run this script"
	echo "Please follow these instructions https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058 to install wget$(tput sgr0)"
	exit
fi
if ! type zip > /dev/null; then
	if [ $OSTYPE == "msys" ]; then
		echo "$(tput setaf 1)Missing 'zip' which is important to run this script"
		echo "Download and install 7z-zip from its official website https://www.7-zip.org/"
		echo "By default, it is installed under the directory /c/Program Files/7-Zip in Windows 10 as my case."
		echo "Run git Bash under Administrator privilege and"
		echo "navigate to the directory /c/Program Files/Git/mingw64/bin,"
		echo "you can run the command $(tput setaf 2)ln -s /c/Program Files/7-Zip/7z.exe zip.exe$(tput sgr0)"
		exit
	elif [ $OSTYPE == "linux-gnu" ]; then
		echo "$(tput setaf 1)Missing 'zip' which is important to run this script"
		echo "install it with the command $(tput setaf 2)'sudo apt-get install zip'$(tput sgr0)"
		exit
	fi
fi
###End prepare bash enviroment

BUILD_ENV="1.0.2"
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"

# List few useful data
echo
echo "Script path:" $SCRIPT_PATH
echo "OS         :" $OS
echo "OS type    :" $OSTYPE
echo ""

#### Start prepare building

#Check if build exists and creates it if not
if [ ! -d "../build-env" ]; then
    mkdir ../build-env || exit 2
fi

cd ../build-env || exit 3

# Check if PF-build-env-<version> exists and downloads + creates it if not
# The build enviroment is based on the Arduino IDE 1.8.5 portal vesion with some changes
if [ ! -d "../PF-build-env-$BUILD_ENV" ]; then
	echo "$(tput setaf 6)PF-build-env-$BUILD_ENV is missing ... creating it now for you$(tput sgr 0)"
	mkdir ../PF-build-env-$BUILD_ENV
	sleep 5
fi

if [ $OSTYPE == "msys" ]; then
	if [ ! -f "PF-build-env-Win-$BUILD_ENV.zip" ]; then
		echo "$(tput setaf 6)Downloding Windows build enviroment...$(tput setaf 2)"
		sleep 2
		wget https://github.com/3d-gussner/PF-build-env/releases/download/Win-$BUILD_ENV/PF-build-env-Win-$BUILD_ENV.zip || exit 4
		#cp -f ../../PF-build-env/PF-build-env-Win-$BUILD_ENV.zip PF-build-env-Win-$BUILD_ENV.zip || exit4
		echo "$(tput sgr 0)"
	fi
	if [ ! -d "../PF-build-env-$BUILD_ENV/$OSTYPE" ]; then
		echo "$(tput setaf 6)Unzipping Windows build enviroment...$(tput setaf 2)"
		sleep 2
		unzip PF-build-env-Win-$BUILD_ENV.zip -d ../PF-build-env-$BUILD_ENV/$OSTYPE || exit 4
		echo "$(tput sgr0)"
	fi
	
fi

if [ $OSTYPE == "linux-gnu" ]; then
	if [ ! -f "PF-build-env-Linux64-$BUILD_ENV.zip" ]; then
		echo "$(tput setaf 6)Downloading Linux 64 build enviroment...$(tput setaf 2)"
		sleep 2
		wget https://github.com/mkbel/PF-build-env/releases/download/$BUILD_ENV/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
		echo "$(tput sgr0)"
	fi

	if [ ! -d "../PF-build-env-$BUILD_ENV/$OSTYPE" ]; then
		echo "$(tput setaf 6)Unzipping Linux build enviroment...$(tput setaf 2)"
		sleep 2
		unzip PF-build-env-Linux64-$BUILD_ENV.zip -d ../PF-build-env-$BUILD_ENV/$OSTYPE || exit 4
		echo "$(tput sgr0)"
	fi
fi
cd $SCRIPT_PATH

# First argument defines which varaint of the Prusa Firmware will be compiled 
if [ -z "$1" ] ; then
	# Select which varaint of the Prusa Firmware will be compiled, like
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
					exit 1
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
		exit
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
		echo "$(tput setaf 1)Language agrument is wrong!$(tput sgr0)"
		echo "Only $(tput setaf 2)'ALL'$(tput sgr0) or $(tput setaf 2)'EN_ONLY'$(tput sgr0) are allowed as 2nd argument!"
		exit
	fi
fi
#Check if DEV_STATUS is selected via argument 3
if [ ! -z "$3" ] ; then
	if [[ "$3" == "GOLD" || "$3" == "RC" || "$3" == "BETA" || "$3" == "ALPHA" || "$3" == "DEVEL" || "$3" == "DEBUG" ]] ; then
		DEV_STATUS_SELECTED=$3
	else
		echo "$(tput setaf 1)Development argument is wrong!$(tput sgr0)"
		echo "Only $(tput setaf 2)'GOLD', 'RC', 'BETA', 'ALPHA', 'DEVEL' or 'DEBUG'$(tput sgr0) are allowed as 3rd argument!$(tput sgr0)"
		exit
	fi
fi

#Set BUILD_ENV_PATH
cd ../PF-build-env-$BUILD_ENV/$OSTYPE || exit 5
BUILD_ENV_PATH="$( pwd -P )"

cd ../..

#Checkif BUILD_PATH exisits and if not creates it
if [ ! -d "Prusa-Firmware-build" ]; then
    mkdir Prusa-Firmware-build  || exit 6
fi

#Set the BUILD_PATH for Arduino IDE
cd Prusa-Firmware-build || exit 7
BUILD_PATH="$( pwd -P )"

for v in ${VARIANTS[*]}
do
	VARIANT=$(basename "$v" ".h")
	# Find firmware version in Configuration.h file and use it to generate the hex filename
	FW=$(grep --max-count=1 "\bFW_VERSION\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d '"' -f2|sed 's/\.//g')
	# Find build version in Configuration.h file and use it to generate the hex filename
	BUILD=$(grep --max-count=1 "\bFW_COMMIT_NR\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d ' ' -f3)
	# Check if the motherboard is an EINSY and if so the only one hex file will generated
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
	if [ ! -d "$SCRIPT_PATH/../Hex-files/FW$FW-Build$BUILD/$MOTHERBOARD" ]; then
		mkdir -p $SCRIPT_PATH/../Hex-files/FW$FW-Build$BUILD/$MOTHERBOARD || exit 10
	fi
	OUTPUT_FOLDER="Hex-files/FW$FW-Build$BUILD/$MOTHERBOARD"
	
	#Check if exacly the same hexfile already exsits
	if [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex"  &&  "$LANGUAGES" == "ALL" ]]; then
		echo ""
		ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex | xargs -n1 basename
		echo "$(tput setaf 6)This hex file to be comiled already exsits! To cancle this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
		read -t 10 -p "Press Enter to continue..."
	elif [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-EN_ONLY.hex"  &&  "$LANGUAGES" == "EN_ONLY" ]]; then
		echo ""
		ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-EN_ONLY.hex | xargs -n1 basename
		echo "$(tput setaf 6)This hex file to be comiled already exsits! To cancle this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
		read -t 10 -p "Press Enter to continue..."
	fi
	if [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip"  &&  "$LANGUAGES" == "ALL" ]]; then
		echo ""
		ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip | xargs -n1 basename
		echo "$(tput setaf 6)This zip file to be comiled already exsits! To cancle this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
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
		cp -f $SCRIPT_PATH/Firmware/variants/$VARIANT.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 11
	else
		echo "$(tput setaf 6)Configuration_prusa.h already exist it will be overwritten in 10 seconds by the chosen variant.$(tput sgr 0)"
		read -t 10 -p "Press Enter to continue..."
		cp -f $SCRIPT_PATH/Firmware/variants/$VARIANT.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 11
	fi

	#Prepare Configuration.h to use the correct FW_DEV_VERSION to prevent LCD messages when connecting with OctoPrint
	sed -i -- "s/#define FW_DEV_VERSION FW_VERSION_UNKNOWN/#define FW_DEV_VERSION FW_VERSION_$DEV_STATUS/g" $SCRIPT_PATH/Firmware/Configuration.h

	# set FW_REPOSITORY
	sed -i -- 's/#define FW_REPOSITORY "Unknown"/#define FW_REPOSITORY "Prusa3d"/g' $SCRIPT_PATH/Firmware/Configuration.h

	#Prepare english only or multilanguage version to be build
	if [ $LANGUAGES == "ALL" ]; then
		echo " "
		echo "Multi-language firmware will be build"
		echo " "
	else
		echo " "
		echo "English only language firmware will be build"
		echo " "
	fi
		
	#Check if compiler flags are set to Prusa specific needs for the rambo board.
	if [ $OSTYPE == "msys" ]; then
		RAMBO_PLATFORM_FILE="rambo/hardware/avr/1.0.1/platform.txt"
		COMP_FLAGS="compiler.c.elf.flags={compiler.warning_flags} -Os -g -flto -fuse-linker-plugin -Wl,-u,vfprintf -lprintf_flt -lm -Wl,--gc-sections"
		CHECK_FLAGS=$(grep --max-count=1 "$COMP_FLAGS" $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE)
		if [ -z "$CHECK_FLAGS" ]; then
			echo "Compiler flags not found, adding flags"
			if [ ! -f $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE.bck ]; then
				echo "making a backup"
				ls -1 $BUILD_ENV_PATH/portable/packages/rambo/hardware/avr/1.0.1/
				cp -f $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE.bck
			fi
			echo $COMP_FLAGS >> $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE
		else
			echo "Compiler flags are set in rambo platform.txt" $CHECK_FLAGS
		fi
	fi	
	
	#### End of Prepare building
		
	#### Start building
		
	export ARDUINO=$BUILD_ENV_PATH
	#echo $BUILD_ENV_PATH
	export BUILDER=$ARDUINO/arduino-builder

	echo
	#read -t 5 -p "Press Enter..."
	echo 

	if [ $OSTYPE == "msys" ]; then
		echo "Start to build Prusa Firmware under Windows..."
		echo "Using variant $VARIANT$(tput setaf 3)"
		sleep 2
		#$BUILDER -dump-prefs -logger=machine -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=rambo:avr:rambo -ide-version=10805 -build-path=$BUILD_PATH -warnings=none -quiet $SCRIPT_PATH/Firmware/Firmware.ino || exit 12
		#$BUILDER -compile -logger=machine -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=rambo:avr:rambo -ide-version=10805 -build-path=$BUILD_PATH -warnings=none -quiet $SCRIPT_PATH/Firmware/Firmware.ino || exit 13
		$BUILDER -compile -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=rambo:avr:rambo -ide-version=10805 -build-path=$BUILD_PATH -warnings=default $SCRIPT_PATH/Firmware/Firmware.ino || exit 14
		echo "$(tput sgr 0)"
	fi
	if [ $OSTYPE == "linux-gnu" ] ; then
		echo "Start to build Prusa Firmware under Linux 64..."
		echo "Using variant $VARIANT$(tput setaf 3)"
		sleep 2
		$BUILD_ENV_PATH/arduino $SCRIPT_PATH/Firmware/Firmware.ino --verify --board rambo:avr:rambo --pref build.path=$BUILD_PATH || exit 14
		echo "$(tput sgr 0)"
	fi

	if [ $LANGUAGES ==  "ALL" ]; then
		echo "$(tput setaf 2)"
		echo "Building mutli language firmware" $MULTI_LANGUAGE_CHECK
		echo "$(tput sgr 0)"
		sleep 2
		cd $SCRIPT_PATH/lang
		echo "$(tput setaf 3)"
		./config.sh || exit 15
		echo "$(tput sgr 0)"
		# Check if privious languages and firmware build exist and if so clean them up
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
		./lang-build.sh || exit 16
		# Combine compiled firmware with languages 
		./fw-build.sh || exit 17
		echo "$(tput sgr 0)"
		# Check if the motherboard is an EINSY and if so the only one hex file will generated
		MOTHERBOARD=$(grep --max-count=1 "\bMOTHERBOARD\b" $SCRIPT_PATH/Firmware/variants/$VARIANT.h | sed -e's/  */ /g' |cut -d ' ' -f3)
		# If the motherboard is an EINSY just copy one hexfile
		if [ "$MOTHERBOARD" = "BOARD_EINSY_1_0a" ]; then
			echo "$(tput setaf 2)Copying multi language firmware for MK3/Einsy board to Hex-files folder$(tput sgr 0)"
			cp -f firmware.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex
		else
			echo "$(tput setaf 2)Zip multi language firmware for MK2.5/miniRAMbo board to Hex-files folder$(tput sgr 0)"
			cp -f firmware_cz.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-cz.hex
			cp -f firmware_de.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-de.hex
			cp -f firmware_es.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-es.hex
			cp -f firmware_fr.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-fr.hex
			cp -f firmware_it.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-it.hex
			cp -f firmware_pl.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-pl.hex
			if [ $OSTYPE == "msys" ]; then 
				zip a $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-??.hex
				rm $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-??.hex
			elif [ $OSTYPE == "linux-gnu" ]; then
				zip -m -j ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.zip ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-??.hex
			fi
		fi
		# Cleanup after build
		echo "$(tput setaf 3)"
		./fw-clean.sh || exit 18
		./lang-clean.sh || exit 19
		echo "$(tput sgr 0)"
	else
		echo "$(tput setaf 2)Copying English only firmware to Hex-files folder$(tput sgr 0)"
		cp -f $BUILD_PATH/Firmware.ino.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-EN_ONLY.hex || exit 20
	fi

	# Cleanup Firmware
	rm $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 17
	sed -i -- "s/^#define FW_DEV_VERSION FW_VERSION_$DEV_STATUS/#define FW_DEV_VERSION FW_VERSION_UNKNOWN/g" $SCRIPT_PATH/Firmware/Configuration.h
	sed -i -- 's/^#define FW_REPOSITORY "Prusa3d"/#define FW_REPOSITORY "Unknown"/g' $SCRIPT_PATH/Firmware/Configuration.h
	echo $MULTI_LANGUAGE_CHECK
	#sed -i -- "s/^#define LANG_MODE * /#define LANG_MODE              $MULTI_LANGUAGE_CHECK/g" $SCRIPT_PATH/Firmware/config.h
	sed -i -- "s/^#define LANG_MODE *1/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
	sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
	sleep 5
done

# Cleanup compiler flags are set to Prusa specific needs for the rambo board.
#if [ $OSTYPE == "msys" ]; then
#	echo " "
#	echo "Restore Windows platform.txt"
#	echo " "
#	cp -f $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE.bck $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE
#fi

# Switch to hex path and list build files
cd $SCRIPT_PATH
cd ..
echo "$(tput setaf 2) "
echo " "
echo "Build done, please use Slic3rPE > 1.41.0 to upload the firmware"
echo "more information how to flash firmware https://www.prusa3d.com/drivers/ $(tput sgr 0)"
#### End building
