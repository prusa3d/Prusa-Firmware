# Original Prusa i3 MK2 Firmware

## General instructions

Pre-compiled hex files for all printers by PRUSA RESEARCH are available in hex_files folder.

Just clone the repo and flash it to the firmware.


## Build instructions




### Step 1 - Download and Install/Unpack Arduino

Download and install Arduino; suggested and verified version is the 1.6.8 from:
https://www.arduino.cc/en/Main/OldSoftwareReleases#previous

If you also use Arduino for other purposes, it is suggested to use a stand-alone version rather than a system-wide install (e.g., use the "Windows" version instead of "Windows Installer"), and use it only for Prusa i3 MK2 firmware generation.



### Step 2 - Download PRUSA i3 MK2 Firmware

Clone the prusa3d/Prusa-Firmware (Branch: MK2) repository to a local path.

If you are not an expert git user, it is suggested to Download it as ZIP from:
https://github.com/prusa3d/Prusa-Firmware/tree/MK2



### Step 3 - Prepare the Arduino toolchain


#### Step 3a - Remove the LiquidCrystal library

Due to name collision, you have to remove the Liquid Crystal library of the original Arduino installation. To do so, you may rename the "Arduino\libraries\LiquidCrystal" directory to "Arduino\libraries\LiquidCrystal_", or remove it altogether.


#### Step 3b - Add RAMBo hardware / library support

Copy the contents of the ArduinoAddons/Arduino_1.6.x directory from the downloaded firmware repository to the root of the installed arduino path.


### Step 3c - Add the bootloader

Download the bootloader image from:
https://github.com/arduino/Arduino-stk500v2-bootloader/blob/master/goodHexFiles/stk500boot_v2_mega2560.hex

Save a copy of it on each of the following directories:
* Arduino/hardware/arduino/avr/bootloaders/stk500v2/
* Arduino/hardware/marlin/avr/bootloaders/

Double-check that the file has the .hex extension and not .txt.


### Step 4 - Prepare the Arduino Firmware

Copy and rename the file Prusa-Firmware/Firmware/variants/1_75mm_MK2-RAMBo13a-E3Dv6full.h file to Firmware/Configuration_prusa.h (note that you may have to use the 1_75mm_MK2-RAMBo10a-E3Dv6full.h if you have an older RAMBo board.



### Step 5 - Load and configure the Arduino IDE

Load the Arduino IDE and select the RAMBo as target board from the Tools submenu. You are now ready to modify and build the firmware!


### Step 6 [Optional] - Edit the Firmware

Make some modifications to the firmware.

For example, edit the Configuration_prusa.h file and edit (at your own risk):
* Extruder PID tuning: DEFAULT_Kp, DEFAULT_Ki and DEFAULT_Kd values
* Bed PID tuning: DEFAULT_bedKp, DEFAULT_bedKi, DEFAULT_bedKd values
* Extruder fan noise: reduce EXTRUDER_AUTO_FAN_SPEED from 255 to 96 (experiment a bit)


### Step 7 - Build the Firmware

You can now build the firmware:
* Compile the firmware with Sketch -> Build (CTRL-R).
* Export the compiled binary with Sketch -> Export compiled sketch (CTRL-ALT-S).

You will find the exported binary in the Prusa-Firmware/Firmware directory:
* Firmware.ino.with_bootloader.rambo.hex


#### Step 7a - Known errors
If you encounter the "using typedef-name 'fpos_t' after 'struct'" problem, you either have a wrong Arduino version (need 1.6.8 - check Step 1) or need to manually modify fpos_t to fpos_t2 in the files SDBasefile.cpp and SDBasefile.h.

If you are using Linux AND using a cloned git repo, you'll encounter the "generate_version_header_for_marlin execute file not found in $path" error; download and install the script from the following website:
https://github.com/Wackerbarth/Marlin/issues/1
You'll also have to make the script executable (chmod a+x path/to/file)


### Step 8 - Upload the Firmware

Upload the new firmare to the printer using the standard firmware update tool included in original Prusa firmware:
http://manual.prusa3d.com/Guide/Upgrading+firmware/66#_ga=1.48414886.1605935719.1473440900

_Originally posted by @pboschi in https://github.com/prusa3d/Prusa-Firmware/issues/29#issuecomment-268985294_
