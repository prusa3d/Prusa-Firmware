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
#    Do not install 'python' as python 2.x has end of life see https://pythonclock.org/
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
# <username>@<machine name> MINGW64 /<drive>/path
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
# Version: 2.0.2-Build_80
# Change log:
# 12 Jan 2019, 3d-gussner, Fixed "compiler.c.elf.flags=-w -Os -Wl,-u,vfprintf -lprintf_flt -lm -Wl,--gc-sections" in 'platform.txt'
# 16 Jan 2019, 3d-gussner, Build_2, Added development check to modify 'Configuration.h' to prevent unwanted LCD messages that Firmware is unknown
# 17 Jan 2019, 3d-gussner, Build_3, Check for OS Windows or Linux and use the right build environment
# 10 Feb 2019, ropaha, Pull Request, Select variant from list while using build.sh
# 10 Feb 2019, ropaha, change FW_DEV_VERSION automatically depending on FW_VERSION RC/BETA/ALPHA
# 10 Feb 2019, 3d-gussner, 1st tests with English only
# 10 Feb 2019, ropaha, added compiling of all variants and English only
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
# 15 Feb 2019, 3d-gussner, Fixed selection GOLD/UNKNOWN DEV_STATUS for ALL variants builds, so you have to choose only once
# 15 Feb 2019, 3d-gussner, Added some colored output
# 15 Feb 2019, 3d-gussner, troubleshooting and minor fixes
# 16 Feb 2019, 3d-gussner, Script can be run using arguments
#                          $1 = variant, example "1_75mm_MK3-EINSy10a-E3Dv6full.h" at this moment it is not possible to use ALL
#                          $2 = multi language OR English only [ALL/EN_FARM]
#                          $3 = development status [GOLD/RC/BETA/ALPHA/DEVEL/DEBUG]
#                          If one argument is wrong a list of valid one will be shown
# 13 Mar 2019, 3d-gussner, MKbel updated the Linux build environment to version 1.0.2 with an Fix maximum firmware flash size.
#                          So did I
# 11 Jul 2019, deliopoulos,Updated to v1.0.6 as Prusa needs a new board definition for Firmware 3.8.x86_64
#                           - Split the Download of Windows Arduino IDE 1.8.5 and Prusa specific part
#                            --> less download volume needed and saves some time
#
# 13 Jul 2019, deliopoulos,Splitting of Arduino IDE and Prusa parts also for Linux64
# 13 Jul 2019, 3d-gussner, Added Linux 32-bit version (untested yet)
#                          MacOS could be added in future if needs
# 14 Jul 2019, 3d-gussner, Update preferences and make it really portable
# 15 Jul 2019, 3d-gussner, New PF-build-env GitHub branch
# 16 Jul 2019, 3d-gussner, New Arduino_boards GitHub fork
# 17 Jul 2019, 3d-gussner, Final tests under Windows 10 and Linux Subsystem for Windows
# 18 Jul 2019, 3d-gussner, Added python check
# 18 Jul 2019, deliopoulos, No need more for changing 'platform.txt' file as it comes with the Arduino Boards.
# 18 Jul 2019, deliopoulos, Modified 'PF_BUILD_FILE_URL' to use 'BUILD_ENV' variable
# 22 Jul 2019, 3d-gussner, Modified checks to check folder and/or installation output exists.
# 22 Jul 2019, 3d-gussner, Added check if Arduino IDE 1.8.5 boards have been updated
# 22 Jul 2019, 3d-gussner, Changed exit numbers 1-13 for prepare build env 21-28 for prepare compiling 31-36 compiling
# 22 Jul 2019, 3d-gussner, Changed BOARD_URL to DRracers repository after he pulled my PR https://github.com/DRracer/Arduino_Boards/pull/1
# 23 Jul 2019, 3d-gussner, Changed Build-env path to "PF-build-dl" as requested in PR https://github.com/prusa3d/Prusa-Firmware/pull/2028
#                          Changed Hex-files folder to PF-build-hex as requested in PR
# 23 Jul 2019, 3d-gussner, Added Finding OS version routine so supporting new OS should get easier
# 26 Jul 2019, 3d-gussner, Change JSON repository to prusa3d after PR https://github.com/prusa3d/Arduino_Boards/pull/1 was merged
# 23 Sep 2019, 3d-gussner, Prepare PF-build.sh for coming Prusa3d/Arduino_Boards version 1.0.2 Pull Request
# 17 Oct 2019, 3d-gussner, Changed folder and check file names to have separated build environments depending on Arduino IDE version and
#                          board-versions.
# 15 Dec 2019, 3d-gussner, Prepare for switch to Prusa3d/PF-build-env repository
# 15 Dec 2019, 3d-gussner, Fix Arduino user preferences for the chosen board.
# 17 Dec 2019, 3d-gussner, Fix "timer0_fract = 0" warning by using Arduino_boards v1.0.3
# 28 Apr 2020, 3d-gussner, Added RC3 detection
# 03 May 2020, deliopoulos, Accept all RCx as RC versions
# 05 May 2020, 3d-gussner, Make a copy of `not_tran.txt`and `not_used.txt` as `not_tran_$VARIANT.txt`and `not_used_$VARIANT.txt`
#                          After compiling All multi-language variants it makes it easier to find missing or unused translations.
# 12 May 2020, DRracer   , Cleanup double MK2/s MK25/s `not_tran` and `not_used` files
# 13 May 2020, leptun    , If cleanup files do not exist don't try to.
# 01 Oct 2020, 3d-gussner, Bug fix if using argument EN_FARM. Thank to @leptun for pointing out.
#                          Change Build number to script commits 'git rev-list --count HEAD PF-build.sh'
# 02 Oct 2020, 3d-gussner, Add UNKNOWN as argument option
# 05 Oct 2020, 3d-gussner, Disable pause and warnings using command line with all needed arguments
#                          Install needed apps under linux if needed.
#                          Clean PF-Firmware build when changing git branch
# 02 Nov 2020, 3d-gussner, Check for "gawk" on Linux
#                          Add argument to change build number automatically to current commit or define own number
#                          Update exit numbers 1-13 for prepare build env 21-29 for prepare compiling 30-36 compiling
# 08 Jan 2021, 3d-gussner, Comment out 'sudo' auto installation
#                          Add '-?' '-h' help option
# 27 Jan 2021, 3d-gussner, Add `-c`, `-p` and `-n` options
# 12 Feb 2021, 3d-gussner, Add MK404-build.sh
# 13 Feb 2021, 3d-gussner, Indentations
# 13 Feb 2021, 3d-gussner, MK404 improvements like "flash" MK3, MK3S languages files to MK404 xflash.
# 27 Feb 2021, 3d-gussner, Add './lang-community.sh' and update exits
# 03 May 2021, 3d-gussner, Update exit numbers
#                          01-14 prepare build env
#                          21-32 prepare compiling
#                          41-47 compiling
#                          51-54 cleanup
#                          61-62 MK404
# 03 May 2021, 3d-gussner, Update documentation and change version to v1.2.0
# 03 May 2021, 3d-gussner, Add SIM atmega404
# 17 Jun 2021, 3d-gussner, Update PF-build.sh to work after DRracer Remove FW version parsing PR
# 17 Jun 2021, 3d-gussner, Save ELF files for FW3.10.1 debugging PRs
# 17 Jun 2021, 3d-gussner, Add verbose_IDE to output more information during build
# 17 Jun 2021, 3d-gussner, Change version to v2.0.0
# 18 Jun 2021, 3d-gussner, Use atmega404 if extanded RAM or FLASH size are chosen
# 18 Jun 2021, 3d-gussner, Remove MK404 copy of lang file as it has been fixed in MK404
# 21 Jun 2021, 3d-gussner, Change atmega404 board flash argument to y
#                          Use newer version of MK404-build.sh instead start the program
# 22 Jun 2021, 3d-gussner, Added _RAM-<value> and _FLASH-<value> as OUTPUT_FILE_SUFFIX if someone builds an atmega404 firmware
#                          Added some logic if IDE 1.8.13 is used
#                          Added some logic if Arduino_boards 1.0.4 is used
#                          Fixed typo
#                          Restore original `Configuration.h` and `config.h` in case of cannceled script or failed compiling during next start of this script.
#                          use function
# 23 Jun 2021, 3d-gussner, Improve MK404 usage
# 24 Jun 2021, 3d-gussner, Fix MK404 user interaction not to show if compiling 'All' variants
# 24 Jun 2021, 3d-gussner, MK404 is only supported on Linux at this moment.
# 03 Jan 2022, 3d-gussner, Remove calling lang-community.sh as not needed anymore
# 21 Jan 2022, 3d-gussner, Sort variants
#                          Add Arduino 1.8.19 as an option
# 25 Jan 2022, 3d-gussner, Allow upper and lower case for MK404
# 09 Feb 2022, 3d-gussner, Add community language firmware files for MK2.5/S
#                          Add selection of language in MK404 for MK2.5/S
# 10 Feb 2022, 3d-gussner, Add SRCDIR for compatibility with build server
# 13 Feb 2022, leptun    , Fix -o for "Restoring" messages after failure
# 24 Feb 2022, 3d-gussner, Change to Arduino IDE 1.8.19 and Arduino boards 1.0.5
#                          Fix DEV_STATUS to set correctly on RC/BETA/ALPHA/DEVEL
#                          Fix atmegaMK404 Board mem and flash modifications
#                          Limit atmegaMK404 boards mem to 8,16,32
# 20 Jun 2022, 3d-gussner, Change to Ardunio_boards v1.0.5-1
# 20 Jun 2022, wavexx    , New PO-based language translation support
# 20 Jun 2022, 3d-gussner, fix Mk2.5/S zip after change PO-based language translation support
# 06 Jul 2022, 3d-gussner, Change to v1.0.8 and Ardunio_boards v1.0.5-2
# 06 Jul 2022, 3d-gussner, Fix branch check
# 12 Jul 2022, 3d-gussner, Check if FW_FLAVAVOR and FW_FLAVERSION are correct

SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
export SRCDIR=$SCRIPT_PATH

#### Start: Failures
failures()
{
case "$1" in
    0) echo "$(tput setaf 2)PF-build.sh finished with success$(tput sgr0)" ;;
    1) echo "$(tput setaf 1)This script doesn't support your Operating system!$(tput sgr0)" ; exit 1 ;;
    4) echo "$(tput setaf 5)Follow the instructions above $(tput sgr0)" ; exit 4 ;;
    5) echo "$(tput setaf 5)Invalid argument $(tput sgr0)" ; exit 5 ;;
    8) echo "$(tput setaf 5)Failed to download $(tput sgr0)" ; exit 8 ;;
    9) echo "$(tput setaf 5)Failed to create folder $(tput sgr0)" ; exit 9 ;;
    10) echo "$(tput setaf 5)Failed to change folder $(tput sgr0)"; exit 10 ;;
    11) echo "$(tput setaf 5)Failed to unzip $(tput sgr0)" ; exit 11 ;;
    12) echo "$(tput setaf 5)Failed to copy file $(tput sgr0)" ; exit 12 ;;
    13) echo "$(tput setaf 5)Failed to delete $(tput sgr0)" ; exit 13 ;;
    20) echo "$(tput setaf 2)Conditional stop initiated by user $(tput sgr0)" ; exit 20 ;;
    21) echo "$(tput setaf 1)PF-build.sh has been interrupted/failed. $(tput setaf 6)Restoring 'Configuration.h'$(tput sgr0)" ; if [ $OUTPUT == "1" ] ; then sleep 5 ; fi ;;
    22) echo "$(tput setaf 1)PF-build.sh has been interrupted/failed. $(tput setaf 6)Restoring 'config.h'$(tput sgr0)" ; if [ $OUTPUT == "1" ] ; then sleep 5 ; fi ;;
    24) echo "$(tput setaf 1)PF-build.sh stopped due to compiling errors! Try to restore modified files.$(tput sgr0)"; check_script_failed_nr1 ; check_script_failed_nr2 ; cleanup_firmware ; exit 24 ;;
    25) echo "$(tput setaf 1)Failed to execute $(tput sgr0)" ; exit 25 ;;
    26) echo "$(tput setaf 1)FW_FLAVAVOR or FW_FLAVERSION commented out in 'Firmware/Configuration.h'$(tput sgr0)"; check_script_failed_nr1 ; check_script_failed_nr2 ; cleanup_firmware ; exit 26 ;;
esac
}
#### End: Failures

#### Start: Check for options/flags
##check_options()
##{
while getopts b:c:d:g:h:i:j:l:m:n:o:p:v:x:y:?h flag
    do
        case "${flag}" in
            b) build_flag=${OPTARG};;
            c) clean_flag=${OPTARG};;
            d) devel_flag=${OPTARG};;
            g) mk404_graphics_flag=${OPTARG};;
            h) help_flag=1;;
            i) IDE_flag=${OPTARG};;
            j) verbose_IDE_flag=${OPTARG};;
            l) language_flag=${OPTARG};;
            m) mk404_flag=${OPTARG};;
            n) new_build_flag=${OPTARG};;
            o) output_flag=${OPTARG};;
            p) prusa_flag=${OPTARG};;
            v) variant_flag=${OPTARG};;
            x) board_mem_flag=${OPTARG};;
            y) board_flash_flag=${OPTARG};;
            ?) help_flag=1;;
        esac
    done

#
# '?' 'h' argument usage and help
if [ "$help_flag" == "1" ] ; then
echo "***************************************"
echo "* PF-build.sh Version: 2.0.2-Build_80 *"
echo "***************************************"
echo "Arguments:"
echo "$(tput setaf 2)-b$(tput sgr0) Build/commit number"
echo "$(tput setaf 2)-c$(tput sgr0) Do not clean up lang build"
echo "$(tput setaf 2)-d$(tput sgr0) Devel build"
echo "$(tput setaf 2)-g$(tput sgr0) Start MK404 graphics"
echo "$(tput setaf 2)-i$(tput sgr0) Arduino IDE version"
echo "$(tput setaf 2)-j$(tput sgr0) Arduino IDE verbose output"
echo "$(tput setaf 2)-l$(tput sgr0) Languages"
echo "$(tput setaf 2)-m$(tput sgr0) Start MK404 sim"
echo "$(tput setaf 2)-n$(tput sgr0) New fresh build"
echo "$(tput setaf 2)-o$(tput sgr0) Output"
echo "$(tput setaf 2)-p$(tput sgr0) Keep Configuration_prusa.h"
echo "$(tput setaf 2)-v$(tput sgr0) Variant"
echo "$(tput setaf 2)-x$(tput sgr0) Board memory size"
echo "$(tput setaf 2)-y$(tput sgr0) Board flash size"
echo "$(tput setaf 2)-?$(tput sgr0) Help"
echo
echo "Brief USAGE:"
echo "  $(tput setaf 2)./PF-build.sh$(tput sgr0) [-b] [-c] [-d] [-g] [-i] [-j] [-l] [-m] [-n] [-o] [-p ] -[v] [-x] [-y] [-h] [-?]"
echo
echo "  -b : '$(tput setaf 2)Auto$(tput sgr0)' needs git or a number"
echo "  -c : '$(tput setaf 2)0$(tput sgr0)' clean up, '$(tput setaf 2)1$(tput sgr0)' keep"
echo "  -d : '$(tput setaf 2)GOLD$(tput sgr0)', '$(tput setaf 2)RC$(tput sgr0)', '$(tput setaf 2)BETA$(tput sgr0)', '$(tput setaf 2)ALPHA$(tput sgr0)', '$(tput setaf 2)DEBUG$(tput sgr0)', '$(tput setaf 2)DEVEL$(tput sgr0)' and '$(tput setaf 2)UNKNOWN$(tput sgr0)'"
echo "  -g : '$(tput setaf 2)0$(tput sgr0)' no '$(tput setaf 2)1$(tput sgr0)' lite '$(tput setaf 2)2$(tput sgr0)' fancy  '$(tput setaf 2)3$(tput sgr0)' lite  with Quad_HR '$(tput setaf 2)4$(tput sgr0)' fancy with Quad_HR"
echo "  -i : '$(tput setaf 2)1.8.5$(tput sgr0)', '$(tput setaf 2)1.8.19$(tput sgr0)'"
echo "  -j : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' yes"
echo "  -l : '$(tput setaf 2)ALL$(tput sgr0)' for multi language or '$(tput setaf 2)EN_FARM$(tput sgr0)' for English only"
echo "  -m : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' yes '$(tput setaf 2)2$(tput sgr0)' with MMU2"
echo "  -n : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' yes"
echo "  -o : '$(tput setaf 2)1$(tput sgr0)' force or '$(tput setaf 2)0$(tput sgr0)' block output and delays"
echo "  -p : '$(tput setaf 2)0$(tput sgr0)' no, '$(tput setaf 2)1$(tput sgr0)' yes"
echo "  -v : '$(tput setaf 2)All$(tput sgr0)' or variant file name"
echo "  -x : '$(tput setaf 2)8$(tput sgr0)','$(tput setaf 2)16$(tput sgr0)'or'$(tput setaf 2)32$(tput sgr0)' Kb."
echo "  -y : '$(tput setaf 2)256$(tput sgr0)','$(tput setaf 2)384$(tput sgr0)','$(tput setaf 2)512$(tput sgr0)','$(tput setaf 2)1024$(tput sgr0)''$(tput setaf 2)32M$(tput sgr0)'"
echo
echo "Example:"
echo "  $(tput setaf 2)./PF-build.sh -v All -l ALL -d GOLD$(tput sgr0)"
echo "  Will build all variants as multi language and final GOLD version"
echo
echo "  $(tput setaf 2) ./PF-build.sh -v 1_75mm_MK3S-EINSy10a-E3Dv6full.h -b Auto -l ALL -d GOLD -o 1 -c 1 -p 1 -n 1$(tput sgr0)"
echo "  Will build MK3S multi language final GOLD firmware "
echo "  with current commit count number and output extra information,"
echo "  not delete lang build temporary files, keep Configuration_prusa.h and build with new fresh build folder."
echo
exit 6
fi

#Check if verbose_IDE is selected with argument '-j'

if [ ! -z "$verbose_IDE_flag" ]; then
    if [ $verbose_IDE_flag == "1" ]; then
        verbose_IDE="1"
    elif [ $verbose_IDE_flag == "0" ]; then
        verbose_IDE="0"
    else
        echo "Only '0' and '1' are valid verbose_IDE values."
        failures 5
    fi
else
    verbose_IDE="0"
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
        OUTPUT_FILENAME_SUFFIX="${OUTPUT_FILENAME_SUFFIX}_FLASH-$board_flash_flag"

    elif [ "$board_flash_flag" == "512" ] ; then
        BOARD_FLASH="0x7FFFF"
        BOARD_maximum_size="516096"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
        OUTPUT_FILENAME_SUFFIX="${OUTPUT_FILENAME_SUFFIX}_FLASH-$board_flash_flag"
    elif [ "$board_flash_flag" == "1024" ] ; then
        BOARD_FLASH="0xFFFFF"
        BOARD_maximum_size="1040384"
        echo "Board flash size :   $board_flash_flag Kb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
        OUTPUT_FILENAME_SUFFIX="${OUTPUT_FILENAME_SUFFIX}_FLASH-$board_flash_flag"
    elif [[ "$board_flash_flag" == "32M" || "$board_flash_flag" == "32768" ]] ; then
        BOARD_FLASH="0x1FFFFFF"
        BOARD_maximum_size="33546240"
        echo "Board flash size :    32 Mb, $BOARD_maximum_size bytes, $BOARD_FLASH (hex)"
        OUTPUT_FILENAME_SUFFIX="${OUTPUT_FILENAME_SUFFIX}_FLASH-$board_flash_flag"
    else
        echo "Unsupported board flash size chosen. Only '256', '384', '512', '1024' and '32M' are allowed."
        failures 5
    fi
fi

#Check if Build is selected with argument '-x'
if [ ! -z "$board_mem_flag" ] ; then
    if [ "$board_mem_flag" == "8" ] ; then
        BOARD_MEM="0x21FF"
        echo "Board mem size   :     $board_mem_flag Kb, $BOARD_MEM (hex)"
    elif [ "$board_mem_flag" == "16" ] ; then
        BOARD_MEM="0x3DFF"
        echo "Board mem size   :    $board_mem_flag Kb, $BOARD_MEM (hex)"
        OUTPUT_FILENAME_SUFFIX="${OUTPUT_FILENAME_SUFFIX}_RAM-$board_mem_flag"
    elif [ "$board_mem_flag" == "32" ] ; then
        BOARD_MEM="0x7DFF"
        echo "Board mem size   :    $board_mem_flag Kb, $BOARD_MEM (hex)"
        OUTPUT_FILENAME_SUFFIX="${OUTPUT_FILENAME_SUFFIX}_RAM-$board_mem_flag"
    else
        echo "Unsupported board mem size chosen. Only '8', '16' and '32' are allowed."
        failures 5
    fi
fi

#Start: Check if Arduino IDE version is correct
if [ ! -z "$IDE_flag" ]; then
    if [[ "$IDE_flag" == "1.8.5" || "$IDE_flag" == "1.8.19" ]]; then
        ARDUINO_ENV="${IDE_flag}"
    else
        ARDUINO_ENV="1.8.19"
    fi
else
    ARDUINO_ENV="1.8.19"
fi
#End: Check if Arduino IDE version is correct

#Start: Check if Output is selecetd via argument '-o'
if [[ -z "$output_flag" || "$output_flag" == 1 ]] ; then
    OUTPUT=1
else
    OUTPUT=0
fi
#End: Check if Output is selecetd via argument '-o'
##}
#### End: Check for options/flags

#### Start: Make backup of Configuration.h
make_backup1()
{
if [ ! -f "$SCRIPT_PATH/Firmware/Configuration.tmp" ]; then
    cp -f $SCRIPT_PATH/Firmware/Configuration.h $SCRIPT_PATH/Firmware/Configuration.tmp
fi
}
#### End: Make backup of Configuration.h

#### Start: Make backup of config.h
make_backup2()
{
if [ ! -f "$SCRIPT_PATH/Firmware/config.tmp" ]; then
    cp -f $SCRIPT_PATH/Firmware/config.h $SCRIPT_PATH/Firmware/config.tmp
fi
}
#### End: Make backup of config.h

#### Start: Check Check if script has been canceled or failed nr1.
check_script_failed_nr1()
{
#Check for "Configuration.tmp"
if [ -f "$SCRIPT_PATH/Firmware/Configuration.tmp" ]; then
    cp -f $SCRIPT_PATH/Firmware/Configuration.tmp $SCRIPT_PATH/Firmware/Configuration.h
    #echo "Found Configuration.tmp restore Configuration.h"
    failures 21
fi
}
#### End: Check Check if script has been canceled or failed nr1.

#### Start: Check if script has been canceled or failed nr2.
check_script_failed_nr2()
{
#Check for "config.tmp"
    #echo "No config.tmp"
if [ -f "$SCRIPT_PATH/Firmware/config.tmp" ]; then
    cp -f $SCRIPT_PATH/Firmware/config.tmp $SCRIPT_PATH/Firmware/config.h
    #echo "Found config.tmp restore config.h"
    failures 22
fi
}
#### End: Check if script has been canceled or failed nr2.

#### Start: Check if OSTYPE is supported
check_OS()
{
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
        failures 1
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
        failures 1
    fi
else
    #echo "$(tput setaf 1)This script doesn't support your Operating system!"
    #echo "Please use Linux 64-bit or Windows 10 64-bit with Linux subsystem / git-bash"
    #echo "Read the notes of build.sh$(tput sgr0)"
    failures 1
fi
if [ $OUTPUT == "1" ] ; then
    sleep 2
fi
}
#### End: Check if OSTYPE is supported

#### Start: Prepare bash environment and check if wget, zip and other needed things are available
# Start: Check wget
check_wget()
{
if ! type wget > /dev/null; then
    echo "$(tput setaf 1)Missing 'wget' which is important to run this script"
    echo "Please follow these instructions https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058 to install wget$(tput sgr0)"
    failures 4
fi
}
# End: Check wget

# Start: Check for zip
check_zip()
{
if ! type zip > /dev/null; then
    if [ $TARGET_OS == "windows" ]; then
        echo "$(tput setaf 1)Missing 'zip' which is important to run this script"
        echo "Download and install 7z-zip from its official website https://www.7-zip.org/"
        echo "By default, it is installed under the directory /c/Program Files/7-Zip in Windows 10 as my case."
        echo "Run git Bash under Administrator privilege and"
        echo "navigate to the directory /c/Program Files/Git/mingw64/bin,"
        echo "you can run the command $(tput setaf 2)ln -s /c/Program Files/7-Zip/7z.exe zip.exe$(tput sgr0)"
        failures 4
    elif [ $TARGET_OS == "linux" ]; then
        echo "$(tput setaf 1)Missing 'zip' which is important to run this script"
        echo "install it with the command $(tput setaf 2)'sudo apt-get install zip'$(tput sgr0)"
        failures 4
    fi
fi
}
# End: Check for zip

# Start: Check python ... needed during language build
check_python()
{
if ! type python3 > /dev/null; then
    if [ $TARGET_OS == "windows" ]; then
        echo "$(tput setaf 1)Missing 'python3' which is important to run this script"
        failures 4
    elif [ $TARGET_OS == "linux" ]; then
        echo "$(tput setaf 1)Missing 'python3' which is important to run this script"
        echo "Install it with the command $(tput setaf 2)'sudo apt-get install python3'."
        failures 4
    fi
fi
}
# End: Check python ... needed during language build


#### Start: Set build environment
set_build_env_variables()
{
BUILD_ENV="1.0.8"
BOARD="prusa_einsy_rambo"
BOARD_PACKAGE_NAME="PrusaResearch"
if [ "$ARDUINO_ENV" == "1.8.19" ]; then
    BOARD_VERSION="1.0.6"
else
    BOARD_VERSION="1.0.4"
fi
if [ "$ARDUINO_ENV" == "1.8.19" ]; then
    BOARD_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/package_prusa3d_index.json"
    #BOARD_URL="https://raw.githubusercontent.com/3d-gussner/Arduino_Boards/master/IDE_Board_Manager/package_prusa3d_index.json"
else
    BOARD_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/package_prusa3d_index.json"
fi
BOARD_FILENAME="prusa3dboards"
if [ "$ARDUINO_ENV" == "1.8.19" ]; then
    BOARD_FILE_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/prusa3dboards-$BOARD_VERSION.tar.bz2"
    #BOARD_FILE_URL="https://raw.githubusercontent.com/3d-gussner/Arduino_Boards/master/IDE_Board_Manager/prusa3dboards-$BOARD_VERSION.tar.bz2"
else
    BOARD_FILE_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/prusa3dboards-$BOARD_VERSION.tar.bz2"
fi
#PF_BUILD_FILE_URL="https://github.com/3d-gussner/PF-build-env-1/releases/download/$BUILD_ENV-WinLin/PF-build-env-WinLin-$BUILD_ENV.zip"
if [[ "$BOARD_VERSION" == "1.0.3" || "$BOARD_VERSION" == "1.0.2" || "$BOARD_VERSION" == "1.0.1" ]]; then
    PF_BUILD_FILE_URL="https://github.com/prusa3d/PF-build-env/releases/download/$BUILD_ENV-WinLin/PF-build-env-WinLin-$BUILD_ENV.zip"
fi
LIB="PrusaLibrary"
}
#### End: Set build environment

#### Start: List few useful data
output_useful_data()
{
echo
echo "Script path :" $SCRIPT_PATH
echo "OS          :" $OS
echo "OS type     :" $TARGET_OS
echo ""
echo "Arduino IDE :" $ARDUINO_ENV
echo "Build env   :" $BUILD_ENV
echo "Board       :" $BOARD
if [ ! -z "$BOARD_FLASH" ] ; then
    echo "Board flash :" $BOARD_FLASH
fi
if [ ! -z "$BOARD_MEM" ] ; then
    echo "Board mem   :" $BOARD_MEM
fi
echo "Package name:" $BOARD_PACKAGE_NAME
echo "Board v.    :" $BOARD_VERSION
echo "Specific Lib:" $LIB
echo ""
}
#### End: List few useful data

#### Start: Prepare building environment

# Start: Check if build exists and creates it if not
check_create_build_folders()
{
# Check if PF-build-env/<version> exists and downloads + creates it if not
# The build environment is based on the supported Arduino IDE portable version with some changes
if [ ! -d "../PF-build-env/$BUILD_ENV" ]; then
    echo "$(tput setaf 6)PF-build-env/$BUILD_ENV is missing ... creating it now for you$(tput sgr 0)"
    mkdir -p ../PF-build-env/$BUILD_ENV
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
fi
}
# End: Check if build exists and creates it if not

# Start: Download and extract supported Arduino IDE depending on OS
download_prepare_arduinoIDE()
{
if [ ! -d "../PF-build-dl" ]; then
    mkdir ../PF-build-dl || failures 9
fi

cd ../PF-build-dl || failures 10
BUILD_ENV_PATH="$( cd "$(dirname "$0")" ; pwd -P )"

# Windows
if [ $TARGET_OS == "windows" ]; then
    if [ ! -f "arduino-$ARDUINO_ENV-windows.zip" ]; then
        echo "$(tput setaf 6)Downloading Windows 32/64-bit Arduino IDE portable...$(tput setaf 2)"
        if [ $OUTPUT == "1" ] ; then
            sleep 2
        fi
        wget https://downloads.arduino.cc/arduino-$ARDUINO_ENV-windows.zip || failures 8
        echo "$(tput sgr 0)"
    fi
    if [[ ! -d "../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" && ! -e "../PF-build-env/$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]]; then
        echo "$(tput setaf 6)Unzipping Windows 32/64-bit Arduino IDE portable...$(tput setaf 2)"
        if [ $OUTPUT == "1" ] ; then
            sleep 2
        fi
        unzip arduino-$ARDUINO_ENV-windows.zip -d ../PF-build-env/$BUILD_ENV || failures 11
        mv ../PF-build-env/$BUILD_ENV/arduino-$ARDUINO_ENV ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor
        echo "# arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" >> ../PF-build-env/$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
        echo "$(tput sgr0)"
    fi
fi
# Linux
if [ $TARGET_OS == "linux" ]; then
# 32 or 64 bit version
    if [ ! -f "arduino-$ARDUINO_ENV-linux$Processor.tar.xz" ]; then
        echo "$(tput setaf 6)Downloading Linux $Processor Arduino IDE portable...$(tput setaf 2)"
        if [ $OUTPUT == "1" ] ; then
            sleep 2
        fi
        wget --no-check-certificate https://downloads.arduino.cc/arduino-$ARDUINO_ENV-linux$Processor.tar.xz || failures 8
        echo "$(tput sgr 0)"
    fi
    if [[ ! -d "../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" && ! -e "../PF-build-env/$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]]; then
        echo "$(tput setaf 6)Unzipping Linux $Processor Arduino IDE portable...$(tput setaf 2)"
        if [ $OUTPUT == "1" ] ; then
            sleep 2
        fi
        tar -xvf arduino-$ARDUINO_ENV-linux$Processor.tar.xz -C ../PF-build-env/$BUILD_ENV/ || failures 11
        mv ../PF-build-env/$BUILD_ENV/arduino-$ARDUINO_ENV ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor
        echo "# arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" >> ../PF-build-env/$BUILD_ENV/arduino-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
        echo "$(tput sgr0)"
    fi
fi
}
# End: Download and extract supported Arduino IDE depending on OS

# Start: Make Arduino IDE portable
portable_ArduinoIDE()
{
if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/ ]; then
    mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/
fi

if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/ ]; then
    mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable
fi
if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/output/ ]; then
    mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/output
fi
if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/ ]; then
    mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages
fi
if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook/ ]; then
    mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook
fi
if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook/libraries/ ]; then
    mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/sketchbook/libraries
fi
if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/staging/ ]; then
    mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/staging
fi
}
# End: Make Arduino IDE portable

# Start: Change Arduino IDE preferences
change_ArduinoIDEpreferances()
{
if [ ! -e ../PF-build-env/$BUILD_ENV/Preferences-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt ]; then
    echo "$(tput setaf 6)Setting $ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor Arduino IDE preferences for portable GUI usage...$(tput setaf 2)"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    echo "update.check"
    sed -i 's/update.check = true/update.check = false/g' ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
    echo "board"
    sed -i "s/board = uno/board = $BOARD/g" ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
    echo "editor.linenumbers"
    sed -i 's/editor.linenumbers = false/editor.linenumbers = true/g' ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
    echo "boardsmanager.additional.urls"
    echo "boardsmanager.additional.urls=$BOARD_URL" >>../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
    echo "build.verbose=true" >>../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
    echo "compiler.cache_core=false" >>../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
    echo "compiler.warning_level=all" >>../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/lib/preferences.txt
    echo "# Preferences-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor" >> ../PF-build-env/$BUILD_ENV/Preferences-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
    echo "$(tput sgr0)"
fi
}
# End: Change Arduino IDE preferences

# Start: Download and extract Prusa Firmware related parts
download_prepare_Prusa_build_files()
{
# Download and extract PrusaResearchRambo board
if [ ! -f "$BOARD_FILENAME-$BOARD_VERSION.tar.bz2" ]; then
    echo "$(tput setaf 6)Downloading Prusa Research AVR MK3 RAMBo EINSy build environment...$(tput setaf 2)"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    wget $BOARD_FILE_URL || failures 8
fi
if [[ ! -d "../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION" || ! -e "../PF-build-env/$BUILD_ENV/$BOARD_FILENAME-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]]; then
    echo "$(tput setaf 6)Unzipping $BOARD_PACKAGE_NAME Arduino IDE portable...$(tput setaf 2)"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    tar -xvf $BOARD_FILENAME-$BOARD_VERSION.tar.bz2 -C ../PF-build-env/$BUILD_ENV/ || failures 11
    if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME ]; then
        mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME
    fi
    if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME ]; then
        mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME
    fi
    if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware ]; then
        mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware
    fi
    if [ ! -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr ]; then
        mkdir ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr
    fi

    mv ../PF-build-env/$BUILD_ENV/$BOARD_FILENAME-$BOARD_VERSION ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION
    echo "# $BOARD_FILENAME-$BOARD_VERSION" >> ../PF-build-env/$BUILD_ENV/$BOARD_FILENAME-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt

    echo "$(tput sgr 0)"
fi

# Download and extract Prusa Firmware specific library files
if [[ "$BOARD_VERSION" == "1.0.3" || "$BOARD_VERSION" == "1.0.2" || "$BOARD_VERSION" == "1.0.1" ]]; then
    if [ ! -f "PF-build-env-WinLin-$BUILD_ENV.zip" ]; then
        echo "$(tput setaf 6)Downloading Prusa Firmware build environment...$(tput setaf 2)"
        if [ $OUTPUT == "1" ] ; then
            sleep 2
        fi
        wget $PF_BUILD_FILE_URL || failures 8
        echo "$(tput sgr 0)"
    fi
    if [ ! -e "../PF-build-env/$BUILD_ENV/PF-build-env/$BUILD_ENV-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt" ]; then
        echo "$(tput setaf 6)Unzipping Prusa Firmware build environment...$(tput setaf 2)"
        if [ $OUTPUT == "1" ] ; then
            sleep 2
        fi
        unzip -o PF-build-env-WinLin-$BUILD_ENV.zip -d ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor || failures 11
        echo "# PF-build-env-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor-$BUILD_ENV" >> ../PF-build-env/$BUILD_ENV/PF-build-env/$BUILD_ENV-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
        echo "$(tput sgr0)"
    fi
fi
}
# End: Download and extract Prusa Firmware related parts

# Start: Check if User updated Arduino IDE 1.8.5 boardsmanager and tools
check_ArduinoIDE_User_interaction()
{
if [ -d "../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools" ]; then
    echo "$(tput setaf 6)Arduino IDE boards / tools have been manually updated...$"
    echo "Please don't update the 'Arduino AVR boards' as this will prevent running this script (tput setaf 2)"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
fi
if [ -d "../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2" ]; then
    echo "$(tput setaf 6)PrusaReasearch compatible tools have been manually updated...$(tput setaf 2)"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    echo "$(tput setaf 6)Copying Prusa Firmware build environment to manually updated boards / tools...$(tput setaf 2)"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    cp -f ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/hardware/tools/avr/avr/lib/ldscripts/avr6.xn ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/avr/lib/ldscripts/avr6.xn
    echo "# PF-build-env-portable-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor-$BUILD_ENV" >> ../PF-build-env/$BUILD_ENV/PF-build-env-portable-$BUILD_ENV-$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor.txt
    echo "$(tput sgr0)"
fi
if [ -d "../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor/portable/packages/arduino/tools/avr-gcc/5.4.0-atmel3.6.1-arduino2" ]; then
    echo "$(tput setaf 1)Arduino IDE tools have been updated manually to a non supported version!!!"
    echo "Delete ../PF-build-env/$BUILD_ENV and start the script again"
    echo "Script will not continue until this have been fixed $(tput setaf 2)"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    echo "$(tput sgr0)"
    failures 4
fi
}
# End: Check if User updated Arduino IDE 1.8.5 boardsmanager and tools
#
#### End: prepare bash / Linux environment
#### End prepare building

#### Start: Getting arguments for command line compiling
get_arguments()
{

# Check if git is available
if type git > /dev/null; then
    git_available="1"
fi

#
# '-v' argument defines which variant of the Prusa Firmware will be compiled
if [ -z "$variant_flag" ] ; then
    # Select which variant of the Prusa Firmware will be compiled, like
    PS3="Select a variant: "
    while IFS= read -r -d $'\0' f; do
        options[i++]="$f"
    done < <(find Firmware/variants/ -maxdepth 1 -type f -name "*.h" -print0 )
    IFS=$'\n' sorted=($(sort -n <<<"${options[*]}")); unset IFS
    select opt in "${sorted[@]}" "All" "Quit"; do
        case $opt in
            *.h)
                VARIANT=$(basename "$opt" ".h")
                VARIANTS[i++]="$opt"
                break
                ;;
            "All")
                VARIANT="All"
                VARIANTS=${options[*]}
                variant_flag="All"
                break
                ;;
            "Quit")
                echo "You chose to stop"
                failures 20
                ;;
            *)
                echo "$(tput setaf 1)This is not a valid variant$(tput sgr0)"
                ;;
        esac
    done
else
    if [ -f "$SCRIPT_PATH/Firmware/variants/$variant_flag" ] ; then
        VARIANTS=$variant_flag
    elif [ "$variant_flag" == "All" ] ; then
        while IFS= read -r -d $'\0' f; do
            options[i++]="$f"
        done < <(find Firmware/variants/ -maxdepth 1 -type f -name "*.h" -print0 )
        VARIANT="All"
        VARIANTS=${options[*]}
    else
        echo "$(tput setaf 1)Argument $variant_flag could not be found in Firmware/variants please choose a valid one.$(tput sgr0)"
        echo "Only $(tput setaf 2)'All'$(tput sgr0) and file names below are allowed as variant '-v' argument.$(tput setaf 2)"
        ls -1 $SCRIPT_PATH/Firmware/variants/*.h | xargs -n1 basename
        echo "$(tput sgr0)"
        failures 4
    fi
fi

#'-l' argument defines if it is an English only version. Known values EN_FARM / ALL
#Check default language mode
MULTI_LANGUAGE_CHECK=$(grep --max-count=1 "^#define LANG_MODE *" $SCRIPT_PATH/Firmware/config.h|sed -e's/  */ /g'|cut -d ' ' -f3)

if [ -z "$language_flag" ] ; then
    PS3="Select a language: "
    echo
    echo "Which lang-build do you want?"
    select yn in "Multi languages" "English only"; do
        case $yn in
            "Multi languages")
                LANGUAGES="ALL"
                break
                ;;
            "English only")
                LANGUAGES="EN_FARM"
                break
                ;;
            *)
                echo "$(tput setaf 1)This is not a valid language$(tput sgr0)"
                ;;
        esac
    done
else
    if [[ "$language_flag" == "ALL" || "$language_flag" == "EN_FARM" ]] ; then
        LANGUAGES=$language_flag
    else
        echo "$(tput setaf 1)Language argument is wrong!$(tput sgr0)"
        echo "Only $(tput setaf 2)'ALL'$(tput sgr0) or $(tput setaf 2)'EN_FARM'$(tput sgr0) are allowed as language '-l' argument!"
        failures 5
    fi
fi
#Check if DEV_STATUS is selected via argument '-d'
if [ ! -z "$devel_flag" ] ; then
    if [[ "$devel_flag" == "GOLD" || "$devel_flag" == "RC" || "$devel_flag" == "BETA" || "$devel_flag" == "ALPHA" || "$devel_flag" == "DEVEL" || "$devel_flag" == "DEBUG" || "$devel_flag" == "UNKNOWN" ]] ; then
        DEV_STATUS_SELECTED=$devel_flag
    elif [[ "$devel_flag" == "atmega404" || "$devel_flag" == "atmega404_no_bootloader" ]] ; then
        MK404_DEBUG=$devel_flag
    else
        echo "$(tput setaf 1)Development argument is wrong!$(tput sgr0)"
        echo "Only $(tput setaf 2)'GOLD', 'RC', 'BETA', 'ALPHA', 'DEVEL', 'DEBUG' or 'UNKNOWN' $(tput sgr0) are allowed as devel '-d' argument!$(tput sgr0)"
        failures 5
    fi
fi

#Check if Build is selected via argument '-b'
if [ ! -z "$build_flag" ] ; then
    if [[ "$build_flag" == "Auto" && "$git_available" == "1" ]] ; then
        BUILD=$(git rev-list --count HEAD)
    elif [[ $build_flag =~ ^[0-9]+$ ]] ; then
        BUILD=$build_flag
    else
        echo "$(tput setaf 1)Build number argument is wrong!$(tput sgr0)"
        echo "Only $(tput setaf 2)'Auto' (git needed) or numbers $(tput sgr0) are allowed as build '-b' argument!$(tput sgr0)"
        failures 5
    fi
fi

#Check git branch has changed
if [ ! -z "git_available" ]; then
    BRANCH=$(git branch --show-current)
    echo "Current branch is:" $BRANCH
    if [ ! -f "$SCRIPT_PATH/../PF-build.branch" ]; then
        #echo "$BRANCH" >| $SCRIPT_PATH/../PF-build.branch
        echo "created PF-build.branch file"
    else
        PRE_BRANCH=$(cat "$SCRIPT_PATH/../PF-build.branch")
        #echo "Previous branch was:" $PRE_BRANCH
        if [ ! "$BRANCH" == "$PRE_BRANCH" ] ; then
            CLEAN_PF_FW_BUILD=1
            echo "$BRANCH" >| $SCRIPT_PATH/../PF-build.branch
        fi
    fi
else
    BRANCH=""
    CLEAN_PF_FW_BUILD=0
fi
}
#### End: Getting arguments for command line compiling

#### Start: Set needed Paths
set_paths()
{
cd ../PF-build-env/$BUILD_ENV/$ARDUINO_ENV-$BOARD_VERSION-$TARGET_OS-$Processor || failures 10
BUILD_ENV_PATH="$( pwd -P )"

cd ../../..

#Checkif BUILD_PATH exists and if not creates it
if [ ! -d "Prusa-Firmware-build" ]; then
    mkdir Prusa-Firmware-build  || failures 9
fi

#Set the BUILD_PATH for Arduino IDE
cd Prusa-Firmware-build || failures 10
BUILD_PATH="$( pwd -P )"
}
#### End: Set needed Paths

#### Start: Check git branch has changed
check_branch_changed()
{
if [ "$CLEAN_PF_FW_BUILD" == "1" ]; then
    read -t 10 -p "Branch changed, cleaning Prusa-Firmware-build folder"
    rm -r *
else
    echo "Nothing to clean up"
fi
}
#### End: Check git branch has changed

#### Start: Prepare code for compiling

prepare_code_for_compiling()
{
    VARIANT=$(basename "$v" ".h")
    MK404_PRINTER=$(grep --max-count=1 "\bPRINTER_TYPE\b" $SCRIPT_PATH/Firmware/variants/$VARIANT.h | sed -e's/  */ /g' |cut -d ' ' -f3 | cut -d '_' -f2)
    # Find firmware version in Configuration.h file and use it to generate the hex filename
    FW_MAJOR=$(grep --max-count=1 "\bFW_MAJOR\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d ' ' -f3)
    FW_MINOR=$(grep --max-count=1 "\bFW_MINOR\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d ' ' -f3)
    FW_REVISION=$(grep --max-count=1 "\bFW_REVISION\b" $SCRIPT_PATH/Firmware/Configuration.h| sed -e's/  */ /g'|cut -d ' ' -f3)
    FW="$FW_MAJOR$FW_MINOR$FW_REVISION"
    #FW=$(grep --max-count=1 "\bFW_VERSION\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d '"' -f2|sed 's/\.//g')
    if [ -z "$BUILD" ] ; then
        # Find build version in Configuration.h file and use it to generate the hex filename
        BUILD=$(grep --max-count=1 "\bFW_COMMIT_NR\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d ' ' -f3)
    else
        # Find and replace build version in Configuration.h file
        BUILD_ORG=$(grep --max-count=1 "\bFW_COMMIT_NR\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d ' ' -f3)
        echo "Original build number: $BUILD_ORG"
        if [ "$BUILD_ORG" != "$BUILD" ]; then
            echo "New build number     : $BUILD"
            sed -i -- "s/^#define FW_COMMIT_NR.*/#define FW_COMMIT_NR $BUILD/g" $SCRIPT_PATH/Firmware/Configuration.h
        fi
    fi
    # Check if the motherboard is an EINSY and if so only one hex file will generated
    MOTHERBOARD=$(grep --max-count=1 "\bMOTHERBOARD\b" $SCRIPT_PATH/Firmware/variants/$VARIANT.h | sed -e's/  */ /g' |cut -d ' ' -f3)
    # Check development status
    FW_FLAV=$(grep --max-count=1 "//#define FW_FLAVOR\b" $SCRIPT_PATH/Firmware/Configuration.h|cut -d ' ' -f1)
    FW_FLAVREV=$(grep --max-count=1 "//#define FW_FLAVERSION\b" $SCRIPT_PATH/Firmware/Configuration.h|cut -d ' ' -f1)
    if [[ "$FW_FLAV" == "//#define" && -z $FW_FLAVREV ]];then
        failures 26
    fi
    #echo "FLAV:$FW_FLAV"
    if [[ "$FW_FLAV" != "//#define" ]] ; then
        FW_FLAVOR=$(grep --max-count=1 "\bFW_FLAVOR\b" $SCRIPT_PATH/Firmware/Configuration.h| sed -e's/  */ /g'|cut -d ' ' -f3)
        #echo "FLAVOR:$FW_FLAVOR"
        if [[ "$FW_FLAVREV" != "//#define" ]] ; then
            FW_FLAVERSION=$(grep --max-count=1 "\bFW_FLAVERSION\b" $SCRIPT_PATH/Firmware/Configuration.h| sed -e's/  */ /g'|cut -d ' ' -f3)
            FW="$FW-$FW_FLAVOR"
            DEV_CHECK="$FW_FLAVOR"
            echo "DEV:$DEV_CHECK"
            if [ ! -z "$FW_FLAVERSION" ] ; then
                FW="$FW$FW_FLAVERSION"
            fi
        else
            failures 26
        fi
    fi
    #DEV_CHECK=$(grep --max-count=1 "\bFW_VERSION\b" $SCRIPT_PATH/Firmware/Configuration.h | sed -e's/  */ /g'|cut -d '"' -f2|sed 's/\.//g'|cut -d '-' -f2)
    if [ -z "$DEV_STATUS_SELECTED" ] ; then
        if [[ "$DEV_CHECK" == *"RC"* ]] ; then
            DEV_STATUS="RC"
        elif [[ "$DEV_CHECK" == *"ALPHA"* ]]; then
            DEV_STATUS="ALPHA"
        elif [[ "$DEV_CHECK" == *"BETA"* ]]; then
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
}
#### End: Prepare code for compiling

prepare_hex_folders()
{
    if [ ! -d "$SCRIPT_PATH/../PF-build-hex/FW$FW-Build$BUILD/$MOTHERBOARD" ]; then
        mkdir -p $SCRIPT_PATH/../PF-build-hex/FW$FW-Build$BUILD/$MOTHERBOARD || failures 9
    fi
    OUTPUT_FOLDER="PF-build-hex/FW$FW-Build$BUILD/$MOTHERBOARD"
    if [ "$BOARD" != "prusa_einsy_rambo" ]; then
        if [ ! -d "$SCRIPT_PATH/../PF-build-hex/FW$FW-Build$BUILD/$BOARD" ]; then
            mkdir -p $SCRIPT_PATH/../PF-build-hex/FW$FW-Build$BUILD/$BOARD || failures 9
        fi
        OUTPUT_FOLDER="PF-build-hex/FW$FW-Build$BUILD/$BOARD"
    fi

    #Define OUTPUT_FILENAME
    OUTPUT_FILENAME=FW$FW-Build$BUILD-$VARIANT
    #Check for OUTPUT_FILENAME_SUFFIX and add it
    if [ ! -z $OUTPUT_FILENAME_SUFFIX ]; then
        OUTPUT_FILENAME="${OUTPUT_FILENAME}$OUTPUT_FILENAME_SUFFIX"
    fi

    #Check if exactly the same hexfile already exists
    if [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.hex"  &&  "$LANGUAGES" == "ALL" ]]; then
        echo ""
        ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.hex | xargs -n1 basename
        echo "$(tput setaf 6)This hex file to be compiled already exists! To cancel this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
        if [ $OUTPUT == "1" ] ; then
            read -t 10 -p "Press Enter to continue..."
        fi
    elif [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-EN_FARM.hex"  &&  "$LANGUAGES" == "EN_FARM" ]]; then
        echo ""
        ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-EN_FARM.hex | xargs -n1 basename
        echo "$(tput setaf 6)This hex file to be compiled already exists! To cancel this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
        if [ $OUTPUT == "1" ] ; then
            read -t 10 -p "Press Enter to continue..."
        fi
    fi
    if [[ -f "$SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.zip"  &&  "$LANGUAGES" == "ALL" ]]; then
        echo ""
        ls -1 $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.zip | xargs -n1 basename
        echo "$(tput setaf 6)This zip file to be compiled already exists! To cancel this process press CRTL+C and rename existing hex file.$(tput sgr 0)"
        if [ $OUTPUT == "1" ] ; then
            read -t 10 -p "Press Enter to continue..."
        fi
    fi
}
#### End: Prepare hex files folder

#### Start: List usefull data
list_usefull_data()
{
    echo "$(tput setaf 2)$(tput setab 7) "
    echo "Printer        :" $MK404_PRINTER
    echo "Variant        :" $VARIANT
    echo "Firmware       :" $FW
    echo "Build #        :" $BUILD
    echo "Dev Check      :" $DEV_CHECK
    echo "DEV Status     :" $DEV_STATUS
    echo "Motherboard    :" $MOTHERBOARD
    echo "Board flash    :" $BOARD_FLASH
    echo "Board mem      :" $BOARD_MEM
    echo "Languages      :" $LANGUAGES
    echo "Hex-file Folder:" $OUTPUT_FOLDER
    echo "Hex filename   :" $OUTPUT_FILENAME
    echo "$(tput sgr0)"

}
#### End: List usefull data

#### Start: Prepare Firmware to be compiled
prepare_variant_for_compiling()
{
    # Copy variant as Configuration_prusa.h
    if [ ! -f "$SCRIPT_PATH/Firmware/Configuration_prusa.h" ]; then
        cp -f $SCRIPT_PATH/Firmware/variants/$VARIANT.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || failures 12
    else
        echo "$(tput setaf 6)Configuration_prusa.h already exist it will be overwritten in 10 seconds by the chosen variant.$(tput sgr 0)"
        if [ $OUTPUT == "1" ] ; then
            read -t 10 -p "Press Enter to continue..."
        fi
        cp -f $SCRIPT_PATH/Firmware/variants/$VARIANT.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || failures 12
    fi

    #Prepare Configuration.h to use the correct FW_DEV_VERSION to prevent LCD messages when connecting with OctoPrint
    sed -i -- "s/#define FW_DEV_VERSION FW_VERSION_.*/#define FW_DEV_VERSION FW_VERSION_$DEV_STATUS/g" $SCRIPT_PATH/Firmware/Configuration.h

    # set FW_REPOSITORY
    sed -i -- 's/#define FW_REPOSITORY "Unknown"/#define FW_REPOSITORY "Prusa3d"/g' $SCRIPT_PATH/Firmware/Configuration.h

    #Prepare English only or multi-language version to be build
    if [ $LANGUAGES == "EN_FARM" ]; then
        echo " "
        echo "English only language firmware will be built"
        sed -i -- "s/^#define LANG_MODE *1/#define LANG_MODE              0/g" $SCRIPT_PATH/Firmware/config.h
        echo " "
    else
        echo " "
        echo "Multi-language firmware will be built"
        sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              1/g" $SCRIPT_PATH/Firmware/config.h
        echo " "
    fi

    #Check if compiler flags are set to Prusa specific needs for the rambo board.
    #if [ $TARGET_OS == "windows" ]; then
       #RAMBO_PLATFORM_FILE="PrusaResearchRambo/avr/platform.txt"
    #fi

    #New fresh PF-Firmware-build
    if [ "$new_build_flag" == "1" ]; then
        rm -r -f $BUILD_PATH/* || failures 13
    fi

}
#### End: Prepare Firmware to be compiled
#### End of Prepare building

#### Start: Compiling EN Prusa Firmware
compile_en_firmware()
{
    ## Check board mem size
    CURRENT_BOARD_MEM=$(grep "#define RAMEND" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h | sed -e's/.* //g'|cut -d ' ' -f2 |tr -d ' \t\n\r')
    if [ $CURRENT_BOARD_MEM != "0x21FF" ] ; then
        echo "$(tput setaf 1)Board mem has been modified or not reset$(tput sgr 0)"
        echo "Current:" $CURRENT_BOARD_MEM
        PS3="Select $(tput setaf 2)Yes$(tput sgr 0) if you want to reset it."
        select yn in "Yes" "No"; do
            case $yn in
                Yes)
                    echo "Resetting board mem size"
                    sed -i -- "s/^#define RAMEND .*$/#define RAMEND          0x21FF/g" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h
                    BOARD_MEM_MODIFIED=0
                    break
                    ;;
                *)
                    echo "Continuing with modified mem size"
                    BOARD_MEM_MODIFIED=1
                    break
                    ;;
            esac
        done
    else
        BOARD_MEM_MODIFIED=0
    fi
    ## Modify board mem size
    if [[ ! -z $BOARD_MEM && "$BOARD_MEM" != "0x21FF" ]] ; then
        echo "$(tput setaf 3)Modifying board memory size (hex):$(tput sgr 0)"
        echo "Old:" $CURRENT_BOARD_MEM
        echo "New:" $BOARD_MEM
        read -t 5 -p "To cancel press $(tput setaf 1)CRTL+C$(tput sgr 0)"
        echo ""
        sed -i -- "s/^#define RAMEND          0x21FF/#define RAMEND          ${BOARD_MEM}/g" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h
        BOARD_MEM_MODIFIED=1
    fi

    ## Check board flash size
    CURRENT_BOARD_FLASH=$(grep "#define FLASHEND" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h | sed -e's/.* //g'|cut -d ' ' -f2 |tr -d ' \t\n\r')
    CURRENT_BOARD_maximum_size=$(grep "prusa_einsy_rambo.upload.maximum_size" $BUILD_ENV_PATH/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION/boards.txt |cut -d '=' -f2|tr -d ' \t\n\r')
    if [[ $CURRENT_BOARD_FLASH != "0x3FFFF" || $CURRENT_BOARD_maximum_size != "253952" ]] ; then
        echo "$(tput setaf 1)Board flash has been modified or not reset$(tput sgr 0)"
        echo "Current flash size:" $CURRENT_BOARD_FLASH
        echo "Current max.  size:" $CURRENT_BOARD_maximum_size
        PS3="Select $(tput setaf 2)Yes$(tput sgr 0) if you want to reset it."
        select yn in "Yes" "No"; do
            case $yn in
                Yes)
                    echo "$(tput setaf 1)Resetting board flash size$(tput sgr 0)"
                    sed -i -- "s/^#define FLASHEND .*$/#define FLASHEND        0x3FFFF/g" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h
                    sed -i -- "s/^prusa_einsy_rambo.upload.maximum_size.*/prusa_einsy_rambo.upload.maximum_size=253952/g" $BUILD_ENV_PATH/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION/boards.txt
                    BOARD_FLASH_MODIFIED=0
                    break
                    ;;
                *)
                    echo "$(tput setaf 3)Continuing with modified flash size$(tput sgr 0)"
                    BOARD_FLASH_MODIFIED=1
                    break
                    ;;
            esac
        done
    else
        BOARD_FLASH_MODIFIED=1
    fi
    ## Modify boad flash size
    if [[ ! -z $BOARD_FLASH && "$BOARD_FLASH" != "0x3FFFF" ]] ; then
        echo "$(tput setaf 3)Modifying board flash size (hex):$(tput sgr 0)"
        echo "Old flash size:" $CURRENT_BOARD_FLASH
        echo "New flash size:" $BOARD_FLASH
        echo "Old max.  size:" $CURRENT_BOARD_maximum_size
        echo "New max.  size:" $BOARD_maximum_size
        read -t 5 -p "To cancel press $(tput setaf 1)CRTL+C$(tput sgr 0)"
        sed -i -- "s/^#define FLASHEND .*/#define FLASHEND        ${BOARD_FLASH}/g" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h
        sed -i -- "s/^prusa_einsy_rambo.upload.maximum_size.*/prusa_einsy_rambo.upload.maximum_size=${BOARD_maximum_size}/g" $BUILD_ENV_PATH/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION/boards.txt
        BOARD_FLASH_MODIFIED=1
    fi

    #Check if compiler flags are set to Prusa specific needs for the rambo board.
#    if [ $TARGET_OS == "windows" ]; then
#        RAMBO_PLATFORM_FILE="PrusaResearchRambo/avr/platform.txt"
#    fi

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
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi

    if [ $verbose_IDE == "1" ]; then
        $BUILD_ENV_PATH/arduino-builder -dump-prefs -debug-level 10 -compile -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=$BOARD_PACKAGE_NAME:avr:$BOARD -build-path=$BUILD_PATH -warnings=all $SCRIPT_PATH/Firmware/Firmware.ino || failures 24
    fi
    $BUILD_ENV_PATH/arduino-builder -compile -hardware $ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools $ARDUINO/tools-builder -tools $ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -built-in-libraries $ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -fqbn=$BOARD_PACKAGE_NAME:avr:$BOARD -build-path=$BUILD_PATH -warnings=all -verbose=$verbose_IDE $SCRIPT_PATH/Firmware/Firmware.ino || failures 24
}
#### End: Compiling EN Prusa Firmware

#### Start: Create and save Multi Language Prusa Firmware
create_multi_firmware()
{
    #if [ $LANGUAGES ==  "ALL" ]; then
        echo "$(tput setaf 2)"

        echo "Building multi language firmware" $MULTI_LANGUAGE_CHECK
        echo "$(tput sgr 0)"
        if [ $OUTPUT == "1" ] ; then
            sleep 2
        fi
        cd $SCRIPT_PATH/lang
        echo "$(tput setaf 3)"
        ./config.sh || failures 25
        echo "$(tput sgr 0)"
        # Check if previous languages and firmware build exist and if so clean them up
        if [ -f "lang_en.tmp" ]; then
            echo ""
            echo "$(tput setaf 6)Previous lang build files already exist these will be cleaned up in 10 seconds.$(tput sgr 0)"
            if [ $OUTPUT == "1" ] ; then
                read -t 10 -p "Press Enter to continue..."
            fi
            echo "$(tput setaf 3)"
            ./lang-clean.sh
            echo "$(tput sgr 0)"
        fi
        if [ -f "progmem.out" ]; then
            echo ""
            echo "$(tput setaf 6)Previous firmware build files already exist these will be cleaned up in 10 seconds.$(tput sgr 0)"
            if [ $OUTPUT == "1" ] ; then
                read -t 10 -p "Press Enter to continue..."
            fi
            echo "$(tput setaf 3)"
            ./fw-clean.sh
            echo "$(tput sgr 0)"
        fi
        # Combine compiled firmware with languages
        echo "$(tput setaf 3)"
        ./fw-build.sh || failures 25
        echo "$(tput sgr 0)"
        # Check if the motherboard is an EINSY and if so only one hex file will generated
        MOTHERBOARD=$(grep --max-count=1 "\bMOTHERBOARD\b" $SCRIPT_PATH/Firmware/variants/$VARIANT.h | sed -e's/  */ /g' |cut -d ' ' -f3)
        # If the motherboard is an EINSY just copy one hexfile
        if [ "$MOTHERBOARD" = "BOARD_EINSY_1_0a" ]; then
            echo "$(tput setaf 2)Copying multi language firmware for MK3/Einsy board to PF-build-hex folder$(tput sgr 0)"
            # End of "lang.bin" for MK3 and MK3S copy
            cp -f Firmware-intl.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.hex
            cp -f $BUILD_PATH/Firmware.ino.elf $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.elf
        else
            #Search for created firmware languages
            langs=$(find Firmware-intl-en_*.hex | cut -d "_" -f2 | cut -d "." -f1)
            #Copy found firmware_*.hex files
                for la in $langs; do
                    cp -f Firmware-intl-en_$la.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-$la.hex
                done
            cp -f $BUILD_PATH/Firmware.ino.elf $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.elf
            echo "$(tput setaf 2)Zip multi language firmware for MK2.5/miniRAMbo board to PF-build-hex folder$(tput sgr 0)"
            if [ $TARGET_OS == "windows" ]; then
                zip a $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.zip $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-??.hex
                #rm $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-??.hex
            elif [ $TARGET_OS == "linux" ]; then
                zip -j ../../$OUTPUT_FOLDER/$OUTPUT_FILENAME.zip ../../$OUTPUT_FOLDER/$OUTPUT_FILENAME-??.hex
            fi
        fi

    # Cleanup after build
    if [[ -z "$clean_flag" || "$clean_flag" == "0" ]]; then
        echo "$(tput setaf 3)"
        ./fw-clean.sh || failures 25
        echo "$(tput sgr 0)"
    fi
}
#### End: Create and save Multi Language Prusa Firmware

#### Start: Save EN_FARM language Prusa Firmware
save_en_firmware()
{
    #else
        echo "$(tput setaf 2)Copying English only firmware to PF-build-hex folder$(tput sgr 0)"
        cp -f $BUILD_PATH/Firmware.ino.hex $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-EN_FARM.hex || failures 12
        echo "$(tput setaf 2)Copying English only elf file to PF-build-hex folder$(tput sgr 0)"
        cp -f $BUILD_PATH/Firmware.ino.elf $SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-EN_FARM.elf || failures 12
    #fi
}
#### End: Save EN_FARM language Prusa Firmware

#### Start: Cleanup Firmware
cleanup_firmware()
{
    if [[ -z "$prusa_flag" || "$prusa_flag" == "0" ]]; then
        if [ -e "$SCRIPT_PATH/Firmware/Configuration_prusa.h" ]; then
            rm $SCRIPT_PATH/Firmware/Configuration_prusa.h || failures 13
        fi
    fi
    # Delete dupblicates
    if find $SCRIPT_PATH/lang/ -name '*RAMBo10a*.txt' -printf 1 -quit | grep -q 1
    then
        rm $SCRIPT_PATH/lang/*RAMBo10a*.txt || failures 13
    fi
    # MK2 not supported in this branch
    if find $SCRIPT_PATH/lang/ -name '*MK2-RAMBo13a*' -printf 1 -quit | grep -q 1
    then
        rm $SCRIPT_PATH/lang/*MK2-RAMBo13a*.txt || failures 13
    fi
    if find $SCRIPT_PATH/lang/ -name 'not_tran.txt' -printf 1 -quit | grep -q 1
    then
        rm $SCRIPT_PATH/lang/not_tran.txt || failures 13
    fi
    if find $SCRIPT_PATH/lang/ -name 'not_used.txt' -printf 1 -quit | grep -q 1
    then
        rm $SCRIPT_PATH/lang/not_used.txt || failures 13
    fi

    #New fresh PF-Firmware-build
    if [ "$new_build_flag" == "1" ]; then
        rm -r -f $BUILD_PATH/* || failures 13
    fi

    # Restore files to previous state
    sed -i -- "s/^#define FW_DEV_VERSION FW_VERSION_.*/#define FW_DEV_VERSION FW_VERSION_UNKNOWN/g" $SCRIPT_PATH/Firmware/Configuration.h
    sed -i -- 's/^#define FW_REPOSITORY.*/#define FW_REPOSITORY "Unknown"/g' $SCRIPT_PATH/Firmware/Configuration.h
    if [ ! -z "$BUILD_ORG" ] ; then
        sed -i -- "s/^#define FW_COMMIT_NR.*/#define FW_COMMIT_NR $BUILD_ORG/g" $SCRIPT_PATH/Firmware/Configuration.h
    fi
    echo $MULTI_LANGUAGE_CHECK
    #sed -i -- "s/^#define LANG_MODE * /#define LANG_MODE              $MULTI_LANGUAGE_CHECK/g" $SCRIPT_PATH/Firmware/config.h
    sed -i -- "s/^#define LANG_MODE *1/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
    sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              ${MULTI_LANGUAGE_CHECK}/g" $SCRIPT_PATH/Firmware/config.h
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    #Check for "Configuration.tmp" and delete it
    if [ -e "$SCRIPT_PATH/Firmware/Configuration.tmp" ]; then
        rm $SCRIPT_PATH/Firmware/Configuration.tmp
    fi
    #Check for "config.tmp" and delete it
    if [ -e "$SCRIPT_PATH/Firmware/config.tmp" ]; then
        rm $SCRIPT_PATH/Firmware/config.tmp
    fi
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi

    # Restore build env files to previous state
    if [ "$BOARD_MEM_MODIFIED" == "1" ]; then
        sed -i -- "s/^#define FLASHEND .*$/#define FLASHEND        0x3FFFF/g" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h
        echo "$(tput setaf 2)Restored Board Mem$(tput sgr 0)"
    fi
    if [ "$BOARD_FLASH_MODIFIED" == "1" ]; then
        sed -i -- "s/^prusa_einsy_rambo.upload.maximum_size.*/prusa_einsy_rambo.upload.maximum_size=253952/g" $BUILD_ENV_PATH/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION/boards.txt
        sed -i -- "s/^#define RAMEND.*/#define RAMEND          0x21FF/g" $BUILD_ENV_PATH/hardware/tools/avr/avr/include/avr/iom2560.h
        echo "$(tput setaf 2)Restored Board Flash$(tput sgr 0)"
    fi

}
#### End: Prepare code for compiling
#done

#### Start: Finish script
# List hex path and list build files
finish_pf-build()
{
cd $SCRIPT_PATH
cd ..
echo "$(tput setaf 2) "
echo " "
failures 0
echo "Build done, please use Slic3rPE > 1.41.0 to upload the firmware"
echo "more information how to flash firmware https://www.prusa3d.com/drivers/ $(tput sgr 0)"
echo
echo "Files:"
ls -r -h $SCRIPT_PATH/../PF-build-hex/FW$FW-Build$BUILD/*
}
#### End: Finish script
#### End: building


####Start: MK404 Simulator
MK404_SIM()
{
cd $SCRIPT_PATH
# Check/compile MK404 sim
if [[ "$output_flag" == "1" || -z "$output_flag" ]]; then
    if [[ -z "$mk404_flag" && "$variant_flag" != "All" ]]; then
        echo
        read -t 10 -n 1 -p "Do you want to start MK404? Y/$(tput setaf 2)n$(tput sgr 0)" mk404_start
        if [[ "$mk404_start" == "Y" || "$mk404_start" == "y" ]]; then
            echo
            read -t 10 -n 1 -p "Do you want to start MK404 with or without MMU2S? $(tput setaf 2)1$(tput sgr 0)/2" mk404_choose1
            if [ "$mk404_choose1" == "1" ]; then
                mk404_flag=1
            elif [ "$mk404_choose1" == "2" ]; then
                mk404_flag=2
            else
                mk404_flag=1 #default
            fi
            echo
            echo "Choose MK404 graphics"
            echo "0 = no graphics"
            echo "1 = lite"
            echo "2 = fancy"
            echo "3 = lite with Quad_HR"
            echo "4 = fancy lite with Quad_HR"
            read -t 10 -n 1 -p "Which one do you want? $(tput setaf 2)0$(tput sgr 0)/1/2/3/4" mk404_choose2
            if [ "$mk404_choose2" == "1" ]; then
                mk404_graphics_flag=1
            elif [ "$mk404_choose2" == "2" ]; then
                mk404_graphics_flag=2
            elif [ "$mk404_choose2" == "3" ]; then
                mk404_graphics_flag=3
            elif [ "$mk404_choose2" == "4" ]; then
                mk404_graphics_flag=4
            elif [ "$mk404_choose2" == "0" ]; then
                mk404_graphics_flag=""
            else
                mk404_graphics_flag="" #default
            fi
        fi
        echo
    fi
fi

# For MMU2S
    if [[ "$mk404_flag" == "2" || "$mk404_flag" == "MMU2" || "$mk404_flag" == "MMU2S" ]]; then # Check if MMU2 is selected only for MK3/S
        mk404_flag=2
    fi

if [[ ! -z "$mk404_flag" && "$variant_flag" != "All " ]]; then

# Run MK404 with 'debugcore' and/or 'bootloader-file'
    if [ ! -z "$board_mem_flag" ]; then
        MK404_options="-x $board_mem_flag"
    fi
    if [ ! -z "$board_flash_flag" ]; then
        MK404_options="${MK404_options} -y $board_flash_flag"
    fi

# Run MK404 with graphics
    if [ ! -z "$mk404_graphics_flag" ]; then
        if [[ "$mk404_graphics_flag" == "1" || "$mk404_graphics_flag" == "2" || "$mk404_graphics_flag" == "3" || "$mk404_graphics_flag" == "4" ]]; then
            MK404_options="${MK404_options}  -g $mk404_graphics_flag"
        else
            echo "$(tput setaf 1)Unsupported MK404 graphics option $mk404_graphics_flag$(tput sgr 0)"
        fi
    fi

# Output some useful data
    echo "Printer: $MK404_PRINTER"
    echo "Options: $MK404_options"
    echo ""
    #read -t 5 -p "Press $(tput setaf 2)Enter$(tput sgr 0) to start MK404"
    echo ""

# Change to MK404 build folder
    #cd ../MK404/master/build


#Decide which hex file to use EN_FARM or Multi language
    if [ "$LANGUAGES" == "ALL" ]; then
        if [[ "$MK404_PRINTER" == "MK3" || "$MK404_PRINTER" == "MK3S" ]]; then
            MK404_firmware_file=$SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME.hex
        else
            PS3="Select a language:"
            select lan in ${langs[@]}
            do
                MK404_firmware_file=$SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-$lan.hex
                break
            done
        fi
    else
        MK404_firmware_file=$SCRIPT_PATH/../$OUTPUT_FOLDER/$OUTPUT_FILENAME-EN_FARM.hex
    fi

# Start MK404
# default with serial output and terminal to manipulate it via terminal
    #echo ""
    #echo "./MK404 Prusa_$MK404_PRINTER -s --terminal $MK404_options -f $MK404_firmware_file"
    #sleep 5
    #./MK404 Prusa_$MK404_PRINTER -s --terminal $MK404_options -f $MK404_firmware_file || exit 62
    echo ""
    echo "./MK404-build.sh -m $mk404_flag -p $MK404_PRINTER $MK404_options -f $MK404_firmware_file"
    if [ $OUTPUT == "1" ] ; then
        sleep 2
    fi
    ./MK404-build.sh -m $mk404_flag -p $MK404_PRINTER $MK404_options -f $MK404_firmware_file|| failures 25
fi
}
#### End: MK404 Simulator


#### Check OS and needed apps
echo "Check OS"
check_OS
check_wget
check_zip
check_python

#### Check for options/flags
echo "Check for options"

#### Prepare build environment
echo "Prepare build env"
get_arguments
set_build_env_variables
output_useful_data

#### Download/set needed apps and dependencies
if [ ! -d "../PF-build-env/$BUILD_ENV" ]; then
    check_create_build_folders
    download_prepare_arduinoIDE
    portable_ArduinoIDE
    change_ArduinoIDEpreferances
    download_prepare_Prusa_build_files
    check_ArduinoIDE_User_interaction
fi

#### Start
set_paths
check_branch_changed

for v in ${VARIANTS[*]}
do
    check_script_failed_nr1
    check_script_failed_nr2
    make_backup1
    make_backup2
    prepare_code_for_compiling
    prepare_hex_folders
    list_usefull_data
    prepare_variant_for_compiling
    compile_en_firmware
    if [ $LANGUAGES ==  "ALL" ]; then
        create_multi_firmware
    else
        save_en_firmware
    fi
    cleanup_firmware
done
finish_pf-build
if [ $TARGET_OS == "linux" ]; then
    MK404_SIM
fi
