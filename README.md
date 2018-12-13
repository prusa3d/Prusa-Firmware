# Table of contents

<!--ts-->
   * [Development environment preparation](#1-development-environment-preparation)
   * [Source code compilation](#2-source-code-compilation)
   * [Automated tests](#3-automated-tests)
   * [Documentation](#4-documentation)
<!--te-->

# 1. Development environment preparation

   1. install `"Arduino Software IDE"` for your preferred operating system  
`https://www.arduino.cc -> Software->Downloads`  
it is recommended to use older version `"1.6.9"`, as it is used on out build server to produce official builds.  
_note: in the case of persistent compilation problems, check the version of the currently used C/C++ compiler (GCC) - should be `4.8.1`; version can be verified by entering the command  
`avr-gcc --version`  
if you are not sure where the file is placed (depends on how `"Arduino Software IDE"` was installed), you can use the search feature within the file system_  
_note: name collision for `"LiquidCrystal"` library known from previous versions is now obsolete (so there is no need to delete or rename original file/-s)_

   2. add (`UltiMachine`) `RAMBo` board into the list of Arduino target boards  
`File->Preferences->Settings`  
into text field `"Additional Boards Manager URLs"`  
type location  
`"https://raw.githubusercontent.com/ultimachine/ArduinoAddons/master/package_ultimachine_index.json"`  
or you can 'manually' modify the item  
`"boardsmanager.additional.urls=....."`  
at the file `"preferences.txt"` (this parameter allows you to write a comma-separated list of addresses)  
_note: you can find location of this file on your disk by following way:  
`File->Preferences->Settings`  (`"More preferences can be edited in file ..."`)_  
than do it  
`Tools->Board->BoardsManager`  
from viewed list select an item `"RAMBo"` (will probably be labeled as `"RepRap Arduino-compatible Mother Board (RAMBo) by UltiMachine"`  
_note: select this item for any variant of board used in printers `'Prusa i3 MKx'`, that is for `RAMBo-mini x.y` and `EINSy x.y` to_  
'clicking' the item will display the installation button; select choice `"1.0.1"` from the list(last known version as of the date of issue of this document)  
_(after installation, the item is labeled as `"INSTALLED"` and can then be used for target board selection)_  

   3. modify platform.txt to enable float printf support:  
add "-Wl,-u,vfprintf -lprintf_flt -lm" to "compiler.c.elf.flags=" before existing flag "-Wl,--gc-sections"  
example:  
`"compiler.c.elf.flags=-w -Os -Wl,-u,vfprintf -lprintf_flt -lm -Wl,--gc-sections"`

# 2. Source code compilation

place the source codes corresponding to your printer model obtained from the repository into the selected directory on your disk  
`https://github.com/prusa3d/Prusa-Firmware/`  
in the subdirectory `"Firmware/variants/"` select the configuration file (`.h`) corresponding to your printer model, make copy named `"Configuration_prusa.h"` (or make simple renaming) and copy them into `"Firmware/"` directory  

run `"Arduino IDE"`; select the file `"Firmware.ino"` from the subdirectory `"Firmware/"` at the location, where you placed the source codes  
`File->Open`  
make the desired code customizations; **all changes are on your own risk!**  

select the target board `"RAMBo"`  
`Tools->Board->RAMBo`  
_note: it is not possible to use any of the variants `"Arduino Mega …"`, even though it is the same MCU_  

run the compilation  
`Sketch->Verify/Compile`  

upload the result code into the connected printer  
`Sketch->Upload`  

or you can also save the output code to the file (in so called `HEX`-format) `"Firmware.ino.rambo.hex"`:  
`Sketch->ExportCompiledBinary`  
and then upload it to the printer using the program `"FirmwareUpdater"`  
_note: this file is created in the directory `"Firmware/"`_  

# 3. Automated tests
## Prerequisites
c++11 compiler e.g. g++ 6.3.1

cmake

build system - ninja or gnu make

## Building
Create folder where you want to build tests.

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
