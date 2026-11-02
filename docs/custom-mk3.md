# Custom MK3 changes

This fork of [prusa3d/Prusa-Firmware](https://github.com/prusa3d/Prusa-Firmware) carries one custom
commit on branch `MK3`: `99bbc4c0`. Everything else is upstream.

| Change | Files | Effect |
|---|---|---|
| Thermal limits | `Firmware/temperature.cpp`, `Firmware/variants/MK3S.h` | Raises thermal error thresholds for more temperature headroom |
| 5x5 mesh bed leveling | `Firmware/mesh_bed_leveling.cpp`, `.h`, `Firmware/Marlin_main.cpp`, `Firmware/ultralcd.cpp` | Adds a 5-point option; the LCD setting cycles 3, 5, 7 |
| Startup song | `Firmware/Marlin_main.cpp`, `Firmware/tone04.c`, `Firmware/sound.cpp` | Plays "Ode to Joy" at boot |
| Quieter beeper | `Firmware/sound.cpp` | Master beeper volume reduced to 1/4 |
| More filament load | `Firmware/Marlin_main.cpp` | Fast feed 70 to 105 mm, slow feed 25 to 37 mm |

## Safety

Raised thermal limits weaken a safety margin, so stay near the printer when testing them.

## Updating from upstream

Rebase or reapply this commit onto newer upstream code, and check the changes above by hand. For
build instructions and everything else, see the root [README](../README.md) and upstream's docs.
