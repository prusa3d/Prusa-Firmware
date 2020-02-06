# Table of contents

<!--ts-->
   * [Linux build](#linux)
   * Windows build
     * [Using Arduino](#using-arduino)
     * [Using Linux subsystem](#using-linux-subsystem-under-windows-10-64-bit)
     * [Using Git-bash](#using-git-bash-under-windows-10-64-bit)
   * [Automated tests](#3-automated-tests)
   * [Documentation](#4-documentation)
   * [FAQ](#5-faq)
<!--te-->


# Build
## Linux

1. Clone this repository and checkout the correct branch for your desired release version.

2. Set your printer model. 
   - For MK3 --> skip to step 3. 
   - If you have a different printer model, follow step [2.b](#2b) from Windows build
   
3. Run `sudo ./build.sh`
   - Output hex file is at `"PrusaFirmware/lang/firmware.hex"` . In the same folder you can hex files for other languages as well.

4. Connect your printer and flash with PrusaSlicer ( Configuration --> Flash printer firmware ) or Slic3r PE.
   - If you wish to flash from Arduino, follow step [2.c](#2c) from Windows build first.


_Notes:_

The script downloads Arduino with our modifications and Rambo board support installed, unpacks it into folder `PF-build-env-\<version\>` on the same level, as your Prusa-Firmware folder is located, builds firmware for MK3 using that Arduino in Prusa-Firmware-build folder on the same level as Prusa-Firmware, runs secondary language support scripts. Firmware with secondary language support is generated in lang subfolder. Use firmware.hex for MK3 variant. Use `firmware_\<lang\>.hex` for other printers. Don't forget to follow step [2.b](#2b) first for non-MK3 printers.

## Windows
### Using Arduino
_Note: Multi language build is not supported._

#### 1. Development environment preparation

**a.** Install `"Arduino Software IDE"` from the official website `https://www.arduino.cc -> Software->Downloads` 
   
   _It is recommended to use version `"1.8.5"`, as it is used on out build server to produce official builds._

**b.** Setup Arduino to use Prusa Rambo board definition

* Open Arduino and navigate to File -> Preferences -> Settings
* To the text field `"Additional Boards Manager URLSs"` add `https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/package_prusa3d_index.json`
* Open Board manager (`Tools->Board->Board manager`), and install `Prusa Research AVR Boards by Prusa Research`

**c.** Modify compiler flags in `platform.txt` file
     
* The platform.txt file can be found in Arduino instalation directory, or after Arduino has been updated at: `"C:\Users\(user)\AppData\Local\Arduino15\packages\arduino\hardware\avr\(version)"` If you can locate the file in both places, file from user profile is probably used.
       
* Add `"-Wl,-u,vfprintf -lprintf_flt -lm"` to `"compiler.c.elf.flags="` before existing flag "-Wl,--gc-sections"  

    For example:  `"compiler.c.elf.flags=-w -Os -Wl,-u,vfprintf -lprintf_flt -lm -Wl,--gc-sections"`
   
_Notes:_


_In the case of persistent compilation problems, check the version of the currently used C/C++ compiler (GCC) - should be at leas `4.8.1`; 
If you are not sure where the file is placed (depends on how `"Arduino Software IDE"` was installed), you can use the search feature within the file system_

_Name collision for `"LiquidCrystal"` library known from previous versions is now obsolete (so there is no need to delete or rename original file/-s)_

#### 2. Source code compilation

**a.** Clone this repository`https://github.com/prusa3d/Prusa-Firmware/` to your local drive.

**b.**<a name="2b"></a> In the subdirectory `"Firmware/variants/"` select the configuration file (`.h`) corresponding to your printer model, make copy named `"Configuration_prusa.h"` (or make simple renaming) and copy it into `"Firmware/"` directory.  

**c.**<a name="2c"></a> In file `"Firmware/config.h"` set LANG_MODE to 0.

**d.** Run `"Arduino IDE"`; select the file `"Firmware.ino"` from the subdirectory `"Firmware/"` at the location, where you placed the source code `File->Open` Make the desired code customizations; **all changes are on your own risk!**  

**e.** Select the target board `"Tools->Board->PrusaResearch Einsy RAMBo"`  

**f.** Run the compilation `Sketch->Verify/Compile`  

**g.** Upload the result code into the connected printer `Sketch->Upload`  

* or you can also save the output code to the file (in so called `HEX`-format) `"Firmware.ino.rambo.hex"`:  `Sketch->ExportCompiledBinary` and then upload it to the printer using the program `"FirmwareUpdater"`  
_note: this file is created in the directory `"Firmware/"`_  

### Using Linux subsystem under Windows 10 64-bit
_notes: Script and instructions contributed by 3d-gussner. Use at your own risk. Script downloads Arduino executables outside of Prusa control. Report problems [there.](https://github.com/3d-gussner/Prusa-Firmware/issues) Multi language build is supported._
- follow the Microsoft guide https://docs.microsoft.com/en-us/windows/wsl/install-win10
  You can also use the 'prepare_winbuild.ps1' powershell script with Administrator rights
- Tested versions are at this moment
  - Ubuntu other may different
  - After the installation and reboot please open your Ubuntu bash and do following steps
  - run command `apt-get update`
  - to install zip run `apt-get install zip`
  - add few lines at the top of `~/.bashrc` by running `sudo nano ~/.bashrc`
	
	export OS="Linux"
	export JAVA_TOOL_OPTIONS="-Djava.net.preferIPv4Stack=true"
	export GPG_TTY=$(tty)
	
	use `CRTL-X` to close nano and confirm to write the new entries
  - restart Ubuntu bash
Now your Ubuntu subsystem is ready to use the automatic `PF-build.sh` script and compile your firmware correctly

#### Some Tips for Ubuntu
- Linux is case sensetive so please don't forget to use capital letters where needed, like changing to a directory
- To change the path to your Prusa-Firmware location you downloaded and unzipped
  - Example: You files are under `C:\Users\<your-username>\Downloads\Prusa-Firmware-MK3`
  - use under Ubuntu the following command `cd /mnt/c/Users/<your-username>/Downloads/Prusa-Firmware-MK3`
    to change to the right folder
- Unix and windows have different line endings (LF vs CRLF), try dos2unix to convert
  - This should fix the `"$'\r': command not found"` error
  - to install run `apt-get install dos2unix`
- If your Windows isn't in English the Paths may look different
  Example in other languages
  - English `/mnt/c/Users/<your-username>/Downloads/Prusa-Firmware-MK3` will be on a German Windows`/mnt/c/Anwender/<your-username>/Downloads/Prusa-Firmware-MK3`
#### Compile Prusa-firmware with Ubuntu Linux subsystem installed
- open Ubuntu bash
- change to your source code folder (case sensitive)
- run `./PF-build.sh`
- follow the instructions

### Using Git-bash under Windows 10 64-bit
_notes: Script and instructions contributed by 3d-gussner. Use at your own risk. Script downloads Arduino executables outside of Prusa control. Report problems [there.](https://github.com/3d-gussner/Prusa-Firmware/issues) Multi language build is supported._
- Download and install the 64bit Git version https://git-scm.com/download/win
- Also follow these instructions https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058
- Download and install 7z-zip from its official website https://www.7-zip.org/
  By default, it is installed under the directory /c/Program\ Files/7-Zip in Windows 10
- Run `Git-Bash` under Administrator privilege
- navigate to the directory /c/Program\ Files/Git/mingw64/bin
- run `ln -s /c/Program\ Files/7-Zip/7z.exe zip.exe`
- If your Windows isn't in English the Paths may look different
  Example in other languages
  - English `/mnt/c/Users/<your-username>/Downloads/Prusa-Firmware-MK3` will be on a German Windows`/mnt/c/Anwender/<your-username>/Downloads/Prusa-Firmware-MK3`
  - English `ln -s /c/Program\ Files/7-Zip/7z.exe zip.exe` will be on a Spanish Windows `ln -s /c/Archivos\ de\ programa/7-Zip/7z.exe zip.exe`
#### Compile Prusa-firmware with Git-bash installed
- open Git-bash
- change to your source code folder
- run `bash PF-build.sh`
- follow the instructions


# 3. Automated tests
## Prerequisites
* c++11 compiler e.g. g++ 6.3.1
* cmake
* build system - ninja or gnu make

## Building
Create a folder where you want to build tests.

Example:

`cd ..`

`mkdir Prusa-Firmware-test`

Generate build scripts in target folder.

Example:

`cd Prusa-Firmware-test`

`cmake -G "Eclipse CDT4 - Ninja" ../Prusa-Firmware`

or for DEBUG build:

`cmake -G "Eclipse CDT4 - Ninja" -DCMAKE_BUILD_TYPE=Debug ../Prusa-Firmware`

Build it.

Example:

`ninja`

## Runing
`./tests`

# 4. Documentation
run [doxygen](http://www.doxygen.nl/) in Firmware folder
or visit https://prusa3d.github.io/Prusa-Firmware-Doc for doxygen generated output

# 5. FAQ
Q:I built firmware using Arduino and I see "?" instead of numbers in printer user interface.

A:Step 1.c was ommited or you updated Arduino and now platform.txt located somewhere in your user profile is used.

Q:I built firmware using Arduino and printer now speaks Klingon (nonsense characters and symbols are displayed @^#$&*°;~ÿ)

A:Step 2.c was omitted.

Q:What environment does Prusa use to build the firmware in the first place?

A:Our production builds are 99.9% equivalent to https://github.com/prusa3d/Prusa-Firmware#linux this is also easiest way to build as only one step is needed - run single script, which downloads patched Arduino from github, builds using it, then extracts translated strings and creates language variants (for MK2x) or language hex file for external SPI flash (MK3x). But you need Linux or Linux in virtual machine. This is also what happens when you open pull request to our repository - all variants are built by Travis http://travis-ci.org/ (to check for compilation errors). You can see, what is happening in .travis.yml. It would be also possible to get hex built by travis, only deploy step is missing in .travis.yml. You can get inspiration how to deploy hex by travis and how to setup travis in https://github.com/prusa3d/MM-control-01/ repository. Final hex is located in ./lang/firmware.hex Community reproduced this for Windows in https://github.com/prusa3d/Prusa-Firmware#using-linux-subsystem-under-windows-10-64-bit or https://github.com/prusa3d/Prusa-Firmware#using-git-bash-under-windows-10-64-bit .

Q:Why are build instructions for Arduino mess.

Y:We are too lazy to ship proper board definition for Arduino. We plan to swich to cmake + ninja to be inherently multiplatform, easily integrate build tools, suport more IDEs, get 10 times shorter build times and be able to update compiler whenewer we want.
