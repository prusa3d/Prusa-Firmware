# Original Prusa i3 MK2 Firmware with 7x7 Mesh added

## General instructions
7x7 Requires:
1. Metal Sheet
2. Flash Firmware
3. Edit Starting Gcode

Pre-compiled hex output on NOBRA3D site: https://nobra3d.com/?product=original-prusa-marlin-3-1-files
Just download unzip and flash it to the electronics

1: About the Metal sheet
Since the bed on our printers only have designated areas for the PINDA to trigger the 7x7 wont work without a metal sheet. Prusa uses the fixed triggerpoints to calibrate Skew and I would strongly recommend a removable metal sheet since you never know when a new XYZ-calibration is required.

I ordered my sheet from a great AliExpress with PEI on it. 3D Energetic even notified me that the first sheet was without PEI and asked if i wanted to change my order into one that included PEI. I ordered a big sheet for my Raptor 2.0 as well and they shipped it with DHL Express super fast.
https://www.aliexpress.com/item/ENERGETIC-PEI-Flexible-3D-printer-heated-bed-PEI-sheet-3D-Printer-Platform-Removable-3D-Printer-Surface/1000007017716.html?spm=a2g0s.9042311.0.0.947d4c4dQsWZX0

2: Flash Firmware
Slic3r version 1.4 and newer has a built in tool to flash the firmware, but before you can use this you need to pick the right HEX-file. Use the Prusa guide to select the correcet HEX-file for your machine, I added 7x7 in front of the original HEX-names to make it easier.
PS! The file you download is a zip-file, you need to uncompress it before you can flash the hex-file that is inside the zip.

3: Edit Starting Gcode
With the new firmware you can change from 3x3 to 7x7 grid by editing your Starting Gcode. In Slic3r select "Printer Settings" and "Custom G-code"
In the "Start G-code" section scroll down to "G80" and simply add "N7" behind it to enable 7x7 grid. Your new value on that line should be:
G80 N7 ; mesh bed leveling
Simply remove the N7 to go back to 3x3


Kudos
All i did was copy into the original fw 3.1.0 from the great work done here:
https://github.com/prusa3d/Prusa-Firmware/compare/MK3...stahlfabrik:HD-MBL?diff=unified


## Build instructions for advanced users
I used these instructions
https://github.com/prusa3d/Prusa-Firmware
