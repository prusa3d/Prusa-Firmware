# Custom Prusa i3 MK2 Firmware [![Build Status](https://travis-ci.org/thess/Prusa-Firmware.svg?branch=private-build2)](https://travis-ci.org/thess/Prusa-Firmware)

## About this Firmware

This custom firmware is forked from the Original Prusa i3 MK2 Firmware (MK2 branch). All changes to the MK2 branch in the Prusa Firmware repository are merged here regularly. It also includes changes like:

- PRs from Prusa which have been submitted, but not yet merged into the Prusa-Firmware repo.
- Experimental features which may or may not work.
- Fixed bugs as appropriate.

Custom features and local changes are documented in: [CUSTOM.md](https://github.com/thess/Prusa-Firmware/blob/private-build2/CUSTOM.md)
 

## Disclaimer

Please note that this is not the source repository for the Original Prusa i3 MK2 Firmware and should you decide to use it, you will do so at your own risk! This firmware is provided "AS IS" without warranty of any kind, either expressed or implied.


## Handling Issues and Pull-Requests

If you have an issue with this firmware version, please check first if it also happens with an official released version of the Prusa i3 MK2 Firmware. If the problem still persists in the official release, open an issue at: <a href="https://github.com/prusa3D/Prusa-Firmware/issues" target="_blank">https://github.com/prusa3D/Prusa-Firmware/issues</a>. Otherwise, if the problem is not present in the official firmware release, please you may open an issue here.

If you want to have some feature added to this customized firmware version and do not want to submit the changes to Prusa, you may submit a _Pull Request_ to this repository for consideration. In general, you should submit changes to the official Prusa Firmware repository so everyone can benefit from them.

## Download of precompiled .hex files

A _.zip_ file containing pre-built versions of each private firmware release can be found in the Github reposiotory [here](https://github.com/thess/Prusa-Firware/releases)

Intermediate firmware versions of individual checkins are provided by the Travis CI builds. The URL of a _.zip_ file containing the  firmware can be found toward the end of the Travis build log. Click on the button in the right-hand corner of the log to move to the end. [Travis CI output](https://travis-ci.org/thess/Prusa-Firmware) **Note:** The Travis CI build results are only kept for 14-days.


Just download, unzip and choose the version for your configuation, then flash it to the printer.


## Build instructions

See [BUILDING.md](https://github.com/thess/Prusa-Firmware/blob/private-build2/BUILDING.md)
