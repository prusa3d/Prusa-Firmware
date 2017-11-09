# Changelog of local customizations

## Version 9-Nov-2017
### New features

- Add 8-point Mesh Bed Correction offsets (+/- 500um) support (optional: _MBC_8POINT_)<br/>
Points labeled **a** thru **h** starting at the front/home position. Supported by G80 command and persisted to EEPROM via LCD menus.
```
       B A C K
    +-----------+
    | f   g   h |
    | d   X   e |
    | a   b   c |
    +-----------+
      F R O N T
```

- Replace LiquidCrystal display driver with recent version (experimental)
- Add ability to build single language version (see ``langtool.pl``). This feature saves ~32K program space.
- Add custom G-code to dump and restore EEPROM (optional: _EEPROM_SAVE_RESTORE_)
```
   M767: Dump EEPROM to serial (suitable for restore) or to SD File
     Ex: M767 S --> dump to SD card 'eesave.hex'
         M767 P<addrs> --> Write M768 P<addrs> V<value> to SERIAL
         M767 X --> dump entire EEPROM to SERIAL
   M768: Set EEPROM location from value
     Ex: M768 P<addrs> V<value>
         M768 R --> restore from SD card
```
- Allow embedded colons (':') in G-code. Ex: "`M117 Time is 11:56`" now works!
- Reduce surface artifacts caused by XY skew compensation (experimental). Ref: [Issue #47](https://github.com/prusa3d/Prusa-Firmware/issues/147)
- Clarify temperature and PID calibration menu items. Setup Wizard display name changed.

### Bug fixes

- Complete cleanup of build warnings.
- Removal of dead code and unreferenced variables.
- Fixed a couple of array out-of-bounds references.
