#!/bin/sh
FILES=$@
[ -z "$FILES" ] && FILES=po/*.po

for file in $FILES; do
    # convert the po file to unix to avoid garbage with msgmerge
    dos2unix "$file"

    # merge from the template
    msgmerge -U -s -N --suffix=".bak" "$file" po/Firmware.pot

    # ... and back
    unix2dos "$file"
done
