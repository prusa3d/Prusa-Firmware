## Build instructions for Original Prusa i3 MK2/S/MM Firmware

### Step 1 - Download and Install/Unpack Arduino

Download and install the Arduino IDE. The most recent version 1.8.5 is available here:
<a href="https://www.arduino.cc/en/Main/Software" target="_blank">https://www.arduino.cc/en/Main/Software</a>

If you already use another Arduino IDE version for other projects and don't want to upgrade to the latest setup, you may want to install a stand-alone (ZIP file) version rather than using the "Windows Installer".

### Step 2 - Prepare the Arduino toolchain

1. Open the IDE
2. Open file/preferences and set additional boards manager URL to the RAMBo repository:<br/> <a href="https:// raw.githubusercontent.com/ultimachine/ArduinoAddons/master/package_ultimachine_index.json" target="_blank">https:// raw.githubusercontent.com/ultimachine/ArduinoAddons/master/package_ultimachine_index.json</a>
3. Open Tools/Boards/Boards Manager and install the RAMBo board:<br/>"<small>**RepRap Arduino-compatible Mother Board (RAMBo)** by **UltiMachine**</small>"
4. Close the IDE.
5. Open the IDE and select the RAMBo as target board from the Tools submenu.
6. Close the IDE

### Step 3 - Download PRUSA i3 MK2 Firmware

You can either clone the prusa3d/Prusa-Firmware repository to a local path or download the ZIP file from and unpack it from here:
<a href="https://github.com/prusa3d/Prusa-Firmware/tree/MK2" target="_blank">https://github.com/prusa3d/Prusa-Firmware/tree/MK2</a>

Specific released versions of source code in either **.zip** or **.tar.gz** packages can be found here:
<a href="https://github.com/prusa3d/Prusa-Firmware/releases" target="_blank">https://github.com/prusa3d/Prusa-Firmware/releases</a>

### Step 4 - Configure the Printer Firmware - select variant

The following configurations are availble in the __Prusa-Firmware/Firmware/variants__ directory.
(See **How-to-choose-firmware.pdf** for help choosing configuration.)

* 1_75mm_MK1-RAMBo10a-E3Dv6full.h
* 1_75mm_MK1-RAMBo13a-E3Dv6full.h
* 1_75mm_MK2-MultiMaterial-RAMBo10a-E3Dv6full.h
* 1_75mm_MK2-MultiMaterial-RAMBo13a-E3Dv6full.h
* 1_75mm_MK2-RAMBo10a-E3Dv6full.h
* 1_75mm_MK2-RAMBo13a-E3Dv6full.h

Copy and rename the file representing your configuration to the ``Firmware`` directory as ``Configuration_prusa.h`` Example:
> $ cp Firmware/variants/1_75mm_MK1-RAMBo13a-E3Dv6full.h Firmware/Configuration_prusa.h

### Step 5 - Build the Firmware

1. Open the IDE
2. Open firmware project with File -> Open... Navigate to and open:<br/>
 ``Prusa-Firmware/Firmware/Firmware.ino``
3. Compile the firmware with Sketch -> Verify/Compile (CTRL-R) --or--
4. Export the compiled binary with Sketch -> Export compiled Binary (CTRL-ALT-S).<br/>
You will find the exported binary in ``Prusa-Firmware/Firmware/Firmware.ino.rambo.hex``


### Step 6 - Upload the Firmware

You can upload the new firmare to the printer using either:

* The Prusa3D Firmware Updater (download from <a href="https://www.prusa3d.com/drivers/" target="_blank">https://www.prusa3d.com/drivers/</a>)

* Directly via the Arduino IDE (Select Tools -> Port and 115200 baud rate)

* avrdude (either direct or thru OctoPrint, etc.). Example command-line:
>$ avrdude -v -p m2560 -c wiring -P /dev/ttyUSB0 -b 115200 -U "flash:w:**firmware.hex**:i" -D
