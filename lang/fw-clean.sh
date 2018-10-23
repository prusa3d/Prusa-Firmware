#!/bin/sh
#
# fw-clean.sh - multi-language support script
#  Remove all firmware output files from lang folder.
#

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
rm_if_exists firmware_cz.hex
rm_if_exists firmware_de.hex
rm_if_exists firmware_es.hex
rm_if_exists firmware_fr.hex
rm_if_exists firmware_it.hex
rm_if_exists firmware_pl.hex
rm_if_exists progmem.out
rm_if_exists textaddr.out
rm_if_exists update_lang.out
rm_if_exists update_lang_cz.out
rm_if_exists update_lang_de.out
rm_if_exists update_lang_es.out
rm_if_exists update_lang_fr.out
rm_if_exists update_lang_it.out
rm_if_exists update_lang_pl.out
rm_if_exists lang.bin
rm_if_exists lang.hex

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
