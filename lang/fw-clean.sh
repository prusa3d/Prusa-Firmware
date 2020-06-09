#!/bin/bash
#
# Version 1.0.1
#
# fw-clean.sh - multi-language support script
#  Remove all firmware output files from lang folder.
#
#############################################################################
# Change log:
# 9 June 2020, 3d-gussner, Added version and Change log
# 9 June 2020, 3d-gussner, colored output
#############################################################################

result=0

rm_if_exists()
{
 if [ -e $1 ]; then
  echo -n "$(tput sgr0) removing '$1'...$(tput sgr0)" >&2
  if rm $1; then
   echo "$(tput setaf 2)OK" >&2
  else
   echo "$(tput setaf 1)NG!" >&2
   result=1
  fi
 fi
}

echo "$(tput setaf 2)fw-clean.sh started$(tput sgr0)" >&2

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
rm_if_exists update_lang_nl.out
rm_if_exists update_lang_pl.out
rm_if_exists lang.bin
rm_if_exists lang.hex
#Community language support
#Dutch
rm_if_exists firmware_nl.hex

#Use the 2 lines below as a template and replace 'qr'
##New language
#rm_if_exists firmware_qr.hex

echo -n "fw-clean.sh finished" >&2
if [ $result -eq 0 ]; then
 echo " with success$(tput sgr0)" >&2
else
 echo " with errors!$(tput sgr0)" >&2
fi

case "$-" in
 *i*) echo "press enter key"; read ;;
esac

exit $result
