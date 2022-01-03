#!/bin/bash
#
# fw-clean.sh - multi-language support script
#  Remove all firmware output files from lang folder.
#

# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi

if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi
echo "fw-clean languages:$LANGUAGES" >&2

result=0

rm_if_exists()
{
 if [ -e $1 ]; then
  echo -n " removing '$1'..." >&2
  if rm $1; then
   echo "OK" >&2
  else
   echo "NG!" >&2
   result=1
  fi
 fi
}

echo "fw-clean.sh started" >&2

rm_if_exists text.sym
rm_if_exists progmem1.sym
rm_if_exists progmem1.lss
rm_if_exists progmem1.hex
rm_if_exists progmem1.chr
rm_if_exists progmem1.var
rm_if_exists progmem1.txt
rm_if_exists textaddr.txt
rm_if_exists firmware.bin
rm_if_exists firmware.hex
rm_if_exists progmem.out
rm_if_exists textaddr.out
rm_if_exists update_lang.out
rm_if_exists lang.bin
rm_if_exists lang.hex


for lang in $LANGUAGES; do
 rm_if_exists firmware_$lang.hex
 rm_if_exists update_lang_$lang.out
done

echo -n "fw-clean.sh finished" >&2
if [ $result -eq 0 ]; then
 echo " with success" >&2
else
 echo " with errors!" >&2
fi

case "$-" in
 *i*) echo "press enter key"; read ;;
esac

exit $result
