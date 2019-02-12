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

###Check if OSTYPE is supported
if [ $OSTYPE == "msys" ]; then
	if [ $(uname -m) == "x86_64" ]; then
		echo "Windows 64-bit found"
	fi
elif [ $OSTYPE == "linux-gnu" ]; then
	if [ $(uname -m) == "x86_64" ]; then
		echo "Linux 64-bit found"
	fi
else
	echo "This script doesn't support your Operating system!"
	echo "Please use Linux 64-bit or Windows 10 64-bit with Linux subsystem / git-bash"
	echo "Read the notes of build.sh"
	exit
fi
sleep 2
###Prepare bash enviroment and check if wget and zip are availible
if ! type wget > /dev/null; then
	echo "Missing 'wget' which is important to run this script"
	echo "Please follow these instructions https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058 to install wget"
	exit
fi
if ! type zip > /dev/null; then
	if [ $OSTYPE == "msys" ]; then
		echo "Missing 'zip' which is important to run this script"
		echo "Download and install 7z-zip from its official website https://www.7-zip.org/"
		echo "By default, it is installed under the directory /c/Program Files/7-Zip in Windows 10 as my case."
		echo "Run git Bash under Administrator privilege and"
		echo "navigate to the directory /c/Program Files/Git/mingw64/bin,"
		echo "you can run the command ln -s /c/Program Files/7-Zip/7z.exe zip.exe"
		exit
	elif [ $OSTYPE == "linux-gnu" ]; then
		echo "Missing 'zip' which is important to run this script"
		echo "install it with the command 'sudo apt-get install zip'"
		exit
	fi
fi
###End prepare bash enviroment

BUILD_ENV="1.0.1"
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"

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
				echo "This is not a valid variant"
				;;
		esac
	done
else
	VARIANT=$1
fi

#Second argument defines if it is an english only version. Known values EN_ONLY / ALL
#Check default language mode
MULTI_LANGUAGE_CHECK=$(grep --max-count=1 "^#define LANG_MODE *" $SCRIPT_PATH/Firmware/config.h|sed -e's/  */ /g'|cut -d ' ' -f3)

if [ -z "$2" ] ; then
	PS3="Select a language: "
	echo
	echo "Which lang-build do you want?"
	select yn in "All" "English only"; do
		case $yn in
			"All")
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
				echo "This is not a valid language"
				;;
		esac
	done
else
	LANGUAGES=$2
fi

# List few useful data
echo
echo "Script path:" $SCRIPT_PATH
echo "OS         :" $OS
echo "OS type    :" $OSTYPE

#### Start prepare building

#Check if build exists and creates it if not
if [ ! -d "../build-env" ]; then
    mkdir ../build-env || exit 2
fi

cd ../build-env || exit 3

# Check if PF-build-env-<version> exists and downloads + creates it if not
# The build enviroment is based on the Arduino IDE 1.8.5 portal vesion with some changes
if [ ! -d "../PF-build-env-$BUILD_ENV" ]; then
	echo "PF-build-env-$BUILD_ENV is missing ... creating it now for you"
	mkdir ../PF-build-env-$BUILD_ENV
	sleep 5
fi

if [ $OSTYPE == "msys" ]; then
	if [ ! -f "PF-build-env-Win-$BUILD_ENV.zip" ]; then
		echo "Downloding Windows build enviroment..."
		sleep 2
		wget https://github.com/3d-gussner/PF-build-env/releases/download/Win-$BUILD_ENV/PF-build-env-Win-$BUILD_ENV.zip || exit 4
		#cp -f ../../PF-build-env/PF-build-env-Win-$BUILD_ENV.zip PF-build-env-Win-$BUILD_ENV.zip || exit4
	fi
	if [ ! -d "../PF-build-env-$BUILD_ENV/$OSTYPE" ]; then
		echo "Unzipping Windows build enviroment..."
		sleep 2
		unzip PF-build-env-Win-$BUILD_ENV.zip -d ../PF-build-env-$BUILD_ENV/$OSTYPE || exit 4
	fi
	
fi

if [ $OSTYPE == "linux-gnu" ]; then
	if [ ! -f "PF-build-env-Linux64-$BUILD_ENV.zip" ]; then
		echo "Downloading Linux 64 build enviroment..."
		sleep 2
		wget https://github.com/mkbel/PF-build-env/releases/download/$BUILD_ENV/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
	fi

	if [ ! -d "../PF-build-env-$BUILD_ENV/$OSTYPE" ]; then
		echo "Unzipping Linux build enviroment..."
		sleep 2
		unzip PF-build-env-Linux64-$BUILD_ENV.zip -d ../PF-build-env-$BUILD_ENV/$OSTYPE || exit 4
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
		echo "DEV_STATUS is UNKNOWN. Do you wish to set DEV_STATUS to GOLD?"
		select yn in "Yes" "No"; do
			case $yn in
				Yes)
					DEV_STATUS="GOLD"
					break
					;;
				No) 
					DEV_STATUS="UNKNOWN"
					break
					;;
				*)
					echo "This is not a valid DEV_STATUS"
					;;
			esac
		done
	fi

	#Prepare hex files folders
	if [ ! -d "../Hex-files" ]; then
		mkdir ../Hex-files || exit 8
	fi
	if [ ! -d "../Hex-files/FW$FW-Build$BUILD" ]; then
		mkdir ../Hex-files/FW$FW-Build$BUILD || exit 9
	fi
	if [ ! -d "../Hex-files/FW$FW-Build$BUILD/$MOTHERBOARD" ]; then
		mkdir ../Hex-files/FW$FW-Build$BUILD/$MOTHERBOARD || exit 10
	fi
	OUTPUT_FOLDER="Hex-files/FW$FW-Build$BUILD/$MOTHERBOARD"
	
	#Check if exacly the same hexfile already exsits
	if [ -f "../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex" ]; then
		ls ../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex
		read -t 5 -p "This hex file to be comiled already exsits! To cancle this process press CRTL+C and rename existing hex file."
	fi
	
	#echo $OUTPUT_FOLDER
	#ls ../$OUTPUT_FOLDER
	#sleep 2
	
	#List some useful data
	echo " "
	echo "Variant    :" $VARIANT
	echo "Firmware   :" $FW
	echo "Build #    :" $BUILD
	echo "Dev Check  :" $DEV_CHECK
	echo "DEV Status :" $DEV_STATUS
	echo "Motherboard:" $MOTHERBOARD
	echo "Languages  :" $LANGUAGES
	echo "Hex-file Folder:" $OUTPUT_FOLDER
	echo " "

	#Prepare Firmware to be compiled by copying variant as Configuration_prusa.h
	if [ ! -f "$SCRIPT_PATH/Firmware/Configuration_prusa.h" ]; then
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
				ls $BUILD_ENV_PATH/portable/packages/rambo/hardware/avr/1.0.1/
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
	#read -t 5 -p "Press any key..."
	echo 

	if [ $OSTYPE == "msys" ]; then
		echo "Start to build Prusa Firmware under Windows..."
		echo "Using variant $VARIANT"
		sleep 2
		#$BUILDER -dump-prefs -logger=machine -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=rambo:avr:rambo -ide-version=10805 -build-path=$BUILD_PATH -warnings=none -quiet $SCRIPT_PATH/Firmware/Firmware.ino || exit 12
		#$BUILDER -compile -logger=machine -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=rambo:avr:rambo -ide-version=10805 -build-path=$BUILD_PATH -warnings=none -quiet $SCRIPT_PATH/Firmware/Firmware.ino || exit 13
		$BUILDER -compile -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=rambo:avr:rambo -ide-version=10805 -build-path=$BUILD_PATH -warnings=none -quiet $SCRIPT_PATH/Firmware/Firmware.ino || exit 14
	fi
	if [ $OSTYPE == "linux-gnu" ] ; then
		echo "Start to build Prusa Firmware under Linux 64..."
		echo "Using variant $VARIANT"
		sleep 2
		$BUILD_ENV_PATH/arduino $SCRIPT_PATH/Firmware/Firmware.ino --verify --board rambo:avr:rambo --pref build.path=$BUILD_PATH || exit 14
	fi

	if [ $LANGUAGES ==  "ALL" ]; then
		echo " "
		echo "Building mutli language firmware" $MULTI_LANGUAGE_CHECK
		echo " "
		sleep 2
		cd $SCRIPT_PATH/lang
		./config.sh || exit 15
		# build languages
		./lang-build.sh || exit 16
		# Combine compiled firmware with languages 
		./fw-build.sh || exit 17
		# Check if the motherboard is an EINSY and if so the only one hex file will generated
		MOTHERBOARD=$(grep --max-count=1 "\bMOTHERBOARD\b" $SCRIPT_PATH/Firmware/variants/$VARIANT.h | sed -e's/  */ /g' |cut -d ' ' -f3)
		# If the motherboard is an EINSY just copy one hexfile
		if [ "$MOTHERBOARD" = "BOARD_EINSY_1_0a" ]; then
			echo "Copying multi language firmware for MK3/Einsy board to Hex-files folder"
			cp -f firmware.hex ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT.hex
		else
			echo "Copying multi language firmware for MK2.5/miniRAMbo board to Hex-files folder"
			cp -f firmware_cz.hex ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-cz.hex
			cp -f firmware_de.hex ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-de.hex
			cp -f firmware_es.hex ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-es.hex
			cp -f firmware_fr.hex ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-fr.hex
			cp -f firmware_it.hex ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-it.hex
			cp -f firmware_pl.hex ../../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-pl.hex
		fi
		# Cleanup after build
		./fw-clean.sh || exit 18
		./lang-clean.sh || exit 19
	else
		echo "Copying English only firmware to Hex-files folder"
		cp -f $BUILD_PATH/Firmware.ino.hex ../$OUTPUT_FOLDER/FW$FW-Build$BUILD-$VARIANT-EN_ONLY.hex || exit 20
	fi

	# Cleanup Firmware
	rm $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 17
	sed -i -- "s/^#define FW_DEV_VERSION FW_VERSION_$DEV_STATUS/#define FW_DEV_VERSION FW_VERSION_UNKNOWN/g" $SCRIPT_PATH/Firmware/Configuration.h
	sed -i -- 's/^#define FW_REPOSITORY "Prusa3d"/#define FW_REPOSITORY "Unknown"/g' $SCRIPT_PATH/Firmware/Configuration.h
	echo $MULTI_LANGUAGE_CHECK
	#sed -i -- "s/^#define LANG_MODE * /#define LANG_MODE              $MULTI_LANGUAGE_CHECK/g" $SCRIPT_PATH/Firmware/config.h
	sed -i -- "s/^#define LANG_MODE *1/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
	sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
	sleep 2
done

# Cleanup compiler flags are set to Prusa specific needs for the rambo board.
if [ $OSTYPE == "msys" ]; then
	echo " "
	echo "Restore Windows platform.txt"
	echo " "
	cp -f $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE.bck $BUILD_ENV_PATH/portable/packages/$RAMBO_PLATFORM_FILE
fi

# Switch to hex path and list build files
cd $SCRIPT_PATH
cd ..
echo " "
echo "List all build hex files:"
ls $OUTPUT_FOLDER/FW*.hex
echo " "
echo " "
echo "Build done, please use Slic3rPE > 1.41.0 to upload the firmware"
echo "more information how to flash firmware https://www.prusa3d.com/drivers/"
#### End building
