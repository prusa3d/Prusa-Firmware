#!/bin/bash
# TODO: write some up-to-date description

# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr0)" >&2; exit 1; fi

# Community languages
if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi
echo "$(tput setaf 2)fw-build.sh started$(tput sgr 0)" >&2
echo "fw-build languages: $(tput setaf 2)$LANGUAGES$(tput sgr 0)" >&2

finish()
{
 echo
 if [ "$1" = "0" ]; then
  echo "$(tput setaf 2)fw-build.sh finished with success$(tput sgr 0)" >&2
 else
  echo "$(tput setaf 1)fw-build.sh finished with errors!$(tput sgr 0)" >&2
 fi
 case "$-" in
  *i*) echo "press enter key"; read ;;
 esac
 exit $1
}

# Clean the temporary directory
TMPDIR=$(dirname "$0")/tmp
rm -rf "$TMPDIR"
mkdir -p "$TMPDIR"
BIN=$TMPDIR/firmware.bin
MAP=$TMPDIR/firmware.map

# Extract and patch the symbol table/language map
echo "$(tput setaf 4)generating firmware symbol map$(tput sgr 0)" >&2
"$OBJCOPY" -I ihex -O binary "$INOHEX" "$BIN"
./lang-map.py "$INOELF" "$BIN" > "$MAP"

# Build language catalogs
for lang in $LANGUAGES; do
    pofile="po/Firmware_$lang.po"
    echo "$(tput setaf 4)compiling language \"$lang\" from $pofile$(tput sgr 0)" >&2
    ./lang-check.py --warn-empty "$pofile"
    ./lang-build.py "$MAP" "$pofile" "$TMPDIR/lang_$lang.bin"
done

# Build the final hex file
echo "$(tput setaf 4)assembling final firmware image$(tput sgr 0)" >&2
"$OBJCOPY" -I binary -O ihex "$BIN" "$OUTHEX"
cat "$TMPDIR"/lang_*.bin > "$TMPDIR/lang.bin"
"$OBJCOPY" -I binary -O ihex "$TMPDIR/lang.bin" "$TMPDIR/lang.hex"
cat "$TMPDIR/lang.hex" >> "$OUTHEX"

# Check that the language data doesn't exceed the reserved XFLASH space
lang_size=$(wc -c "$TMPDIR/lang.bin" | cut -f1 -d' ')
lang_size_pad=$(( ($lang_size+4096-1) / 4096 * 4096 ))

# TODO: hard-coded! get value by preprocessing LANG_SIZE from xflash_layout.h!
lang_reserved=249856

echo
echo -n "  total size usage: " >&2
if [ $lang_size_pad -gt $lang_reserved ]; then
  echo -n "$(tput setaf 1)" >&2
else
  echo -n "$(tput setaf 2)" >&2
fi
echo "$lang_size_pad ($lang_size)$(tput sgr 0)" >&2
echo "     reserved size: $(tput setaf 2)$lang_reserved$(tput sgr 0)" >&2
if [ $lang_size_pad -gt $lang_reserved ]; then
  echo "$(tput setaf 1)NG! - language data too large$(tput sgr 0)" >&2
  finish 1
fi

echo "  multilanguage fw: $(tput setaf 2)$OUTHEX$(tput sgr 0)" >&2
finish 0
