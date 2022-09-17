#!/bin/bash
# TODO: write some up-to-date description

# Config:
if [ -z "$CONFIG_OK" ]; then source config.sh; fi
if [ -z "$CONFIG_OK" -o "$CONFIG_OK" -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr0)" >&2; exit 1; fi

# Community languages
if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
    LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi

color 2 "fw-build.sh started" >&2
echo -n "fw-build languages: " >&2
color 2 "$LANGUAGES" >&2

finish()
{
    echo >&2
    if [ "$1" = "0" ]; then
        color 2 "fw-build.sh finished with success" >&2
    else
        color 1 "fw-build.sh finished with errors!" >&2
    fi
    case "$-" in
    *i*)
        echo "press enter key"; read ;;
    esac
    exit $1
}

# Clean the temporary directory
TMPDIR=$(dirname "$0")/tmp
rm -rf "$TMPDIR"
mkdir -p "$TMPDIR"
BIN=$TMPDIR/firmware.bin
MAP=$TMPDIR/firmware.map
VARIANT=$(grep '^#define \+PRINTER_TYPE ' "$SRCDIR/Firmware/Configuration_prusa.h"|cut -d '_' -f3)


# Extract and patch the symbol table/language map
color 4 "generating firmware symbol map" >&2
"$OBJCOPY" -I ihex -O binary "$INOHEX" "$BIN"
./lang-map.py "$INOELF" "$BIN" > "$MAP"
cp -f $MAP firmware_$VARIANT.map

# Get the maximum size of a single language table
maxsize=$(grep '^#define \+LANG_SIZE_RESERVED \+' "$SRCDIR/Firmware/config.h" | sed -e 's/\s\+/ /g' | cut -d ' ' -f3)

# Build language catalogs
for lang in $LANGUAGES; do
    pofile="po/Firmware_$lang.po"
    binfile="$TMPDIR/lang_$lang.bin"

    color 4 "compiling language \"$lang\" from $pofile" >&2
    ./lang-check.py --map "$MAP" "$pofile" --no-suggest
    if [ "$?" != 0 ]; then
        color 1 "$pofile: NG! - translation contains warnings or errors" >&2
    fi

    ./lang-build.py "$MAP" "$pofile" "$binfile"

    # ensure each catalog fits the reserved size
    currentsize=$(stat -c '%s' "$binfile")
    if [[ $currentsize -gt $maxsize ]]; then
        color 1 "$pofile: NG! - language data exceeds $maxsize bytes, it uses $currentsize" >&2
        finish 1
    fi
done

# Detect the printer type and choose the language type
if grep -q '^#define \+PRINTER_TYPE \+PRINTER_\(MK25\|MK25S\)\b' "$SRCDIR/Firmware/Configuration_prusa.h"; then
    has_xflash=0
else
    has_xflash=1
fi

if [ "$has_xflash" = 1 ]; then
    # Build the final hex file with XFLASH support (catalogs appended to a single hex file)
    OUTHEX="${INTLHEX}.hex"

    color 4 "assembling final firmware image" >&2
    "$OBJCOPY" -I binary -O ihex "$BIN" "$OUTHEX"
    truncate -s0 "$TMPDIR/lang.bin"
    individual_lang_reserved_hex=$(grep --max-count=1 "^#define LANG_SIZE_RESERVED *" $SRCDIR/Firmware/config.h|sed -e's/  */ /g'|cut -d ' ' -f3|cut -d 'x' -f2)
    individual_lang_reserved=$((16#$individual_lang_reserved_hex))
    for lang in $LANGUAGES; do
        cat "$TMPDIR/lang_$lang.bin" >> "$TMPDIR/lang.bin"
        lang_size=$(stat -c '%s' "$TMPDIR/lang_$lang.bin")
        lang_size_pad=$(( ($lang_size+256-1) / 256 * 256 ))
        lang_free=$(($individual_lang_reserved - $lang_size))
        echo >&2
        echo -n "  size usage" >&2
        [ $lang_size -gt $individual_lang_reserved ] && c=1 || c=2
        color $c " $lang $lang_size_pad ($lang_size)" >&2
        echo -n "  reserved size " >&2
        color 2 "$individual_lang_reserved" >&2
        echo -n "     free bytes " >&2
        color 2 "$lang_free" >&2
    if [ $lang_size_pad -gt $individual_lang_reserved ]; then
        color 1 "NG! - language $lang data too large" >&2
        finish 1
    fi
    done
    "$OBJCOPY" -I binary -O ihex "$TMPDIR/lang.bin" "$TMPDIR/lang.hex"
    cat "$TMPDIR/lang.hex" >> "$OUTHEX"

    # Check that the language data doesn't exceed the reserved XFLASH space
    lang_size=$(stat -c '%s' "$TMPDIR/lang.bin")
    lang_size_pad=$(( ($lang_size+4096-1) / 4096 * 4096 ))

    # TODO: hard-coded! get value by preprocessing LANG_SIZE from xflash_layout.h!
    lang_reserved=249856

    echo >&2
    echo -n "  total size usage: " >&2
    [ $lang_size_pad -gt $lang_reserved ] && c=1 || c=2
    color $c "$lang_size_pad ($lang_size)" >&2
    echo -n "     reserved size: " >&2
    color 2 "$lang_reserved" >&2
    if [ $lang_size_pad -gt $lang_reserved ]; then
        color 1 "NG! - language data too large" >&2
        finish 1
    fi

    echo -n "  multilanguage fw: " >&2
    color 2 "$OUTHEX" >&2
else
    # Build one hex file for each secondary language
    color 4 "assembling final firmware images" >&2
    echo >&2
    echo -n "  maximum size: " >&2
    color 2 "$(( $maxsize ))" >&2

    for lang in $LANGUAGES; do
        OUTHEX="${INTLHEX}-en_${lang}.hex"
        catfile="$TMPDIR/lang_$lang.bin"
        bintmp="$TMPDIR/fw-en_$lang.bin"

        # patch the secondary language table
        cp "$BIN" "$bintmp"
        ./lang-patchsec.py "$INOELF" "$catfile" "$bintmp"
        "$OBJCOPY" -I binary -O ihex "$bintmp" "$OUTHEX"

        # print some stats
        catsize=$(stat -c '%s' "$catfile")
        echo -n "  $lang: " >&2
        color 2 "$(printf "%5d %s" "$catsize" "$OUTHEX")" >&2
    done
fi

finish 0
