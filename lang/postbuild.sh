#!/bin/sh
#
# postbuild.sh - multi-language support script
#  Generate binary with secondary language.
#
# Input files:
#  $OUTDIR/Firmware.ino.elf
#  $OUTDIR/sketch/*.o (all object files)
#
# Output files:
#  text.sym
#  $PROGMEM.sym (progmem1.sym)
#  $PROGMEM.lss (...)
#  $PROGMEM.hex
#  $PROGMEM.chr
#  $PROGMEM.var
#  $PROGMEM.txt
#  textaddr.txt
#
#
# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi
#
# Selected language:
LANG=$1
#if [ -z "$LANG" ]; then LANG='cz'; fi
#
# Params:
IGNORE_MISSING_TEXT=1


finish()
{
 echo
 if [ "$1" = "0" ]; then
  echo "postbuild.sh finished with success" >&2
 else
  echo "postbuild.sh finished with errors!" >&2
 fi
 case "$-" in
  *i*) echo "press enter key"; read ;;
 esac
 exit $1
}

echo "postbuild.sh started" >&2

#check input files
echo " checking files:" >&2
if [ ! -e $OUTDIR ]; then echo "  folder '$OUTDIR' not found!" >&2; finish 1; fi
echo "  folder  OK" >&2
if [ ! -e $INOELF ]; then echo "  elf file '$INOELF' not found!" >&2; finish 1; fi
echo "  elf     OK" >&2
if ! ls $OBJDIR/*.o >/dev/null 2>&1; then echo "  no object files in '$OBJDIR/'!" >&2; finish 1; fi
echo "  objects OK" >&2

#run progmem.sh - examine content of progmem1
echo -n " running progmem.sh..." >&2
./progmem.sh 1 2>progmem.out
if [ $? -ne 0 ]; then echo "NG! - check progmem.out file" >&2; finish 1; fi
echo "OK" >&2

#run textaddr.sh - map progmem addreses to text identifiers
echo -n " running textaddr.sh..." >&2
./textaddr.sh 2>textaddr.out
if [ $? -ne 0 ]; then echo "NG! - check progmem.out file" >&2; finish 1; fi
echo "OK" >&2

#check for messages declared in progmem1, but not found in lang_en.txt
echo -n " checking textaddr.txt..." >&2
if cat textaddr.txt | grep "^ADDR NF" >/dev/null; then
 echo "NG! - some texts not found in lang_en.txt!"
 if [ $IGNORE_MISSING_TEXT -eq 0 ]; then
  finish 1
 else
  echo "  missing text ignored!" >&2
 fi
else
 echo "OK" >&2
fi

#update progmem1 id entries in binary file
echo -n " extracting binary..." >&2
$OBJCOPY -I ihex -O binary $INOHEX ./firmware.bin
echo "OK" >&2

#update binary file
echo " updating binary:" >&2

#update progmem1 id entries in binary file
echo -n "  primary language ids..." >&2
cat textaddr.txt | grep "^ADDR OK" | cut -f3- -d' ' | sed "s/^0000/0x/" |\
 awk '{ id = $2 - 1; hi = int(id / 256); lo = int(id - 256 * hi); printf("%d \\\\x%02x\\\\x%02x\n", strtonum($1), lo, hi); }' |\
 while read addr data; do
  /bin/echo -n -e $data | dd of=./firmware.bin bs=1 count=2 seek=$addr conv=notrunc oflag=nonblock 2>/dev/null
 done
echo "OK" >&2

#update _SEC_LANG in binary file if language is selected
echo -n "  secondary language data..." >&2
if [ ! -z "$LANG" ]; then
 ./update_lang.sh $LANG 2>./update_lang.out
 if [ $? -ne 0 ]; then echo "NG! - check update_lang.out file" >&2; finish 1; fi
 echo "OK" >&2
 finish 0
else
 echo "Updating languages:" >&2
 if [ -e lang_cz.bin ]; then
  echo -n " Czech  : " >&2
  ./update_lang.sh cz 2>./update_lang_cz.out 1>/dev/null
  if [ $? -eq 0 ]; then echo 'OK' >&2; else echo 'NG!' >&2; fi
 fi
 if [ -e lang_de.bin ]; then
  echo -n " German : " >&2
  ./update_lang.sh de 2>./update_lang_de.out 1>/dev/null
  if [ $? -eq 0 ]; then echo 'OK' >&2; else echo 'NG!' >&2; fi
 fi
 if [ -e lang_it.bin ]; then
  echo -n " Italian: " >&2
  ./update_lang.sh it 2>./update_lang_it.out 1>/dev/null
  if [ $? -eq 0 ]; then echo 'OK' >&2; else echo 'NG!' >&2; fi
 fi
 if [ -e lang_es.bin ]; then
  echo -n " Spanish: " >&2
  ./update_lang.sh es 2>./update_lang_es.out 1>/dev/null
  if [ $? -eq 0 ]; then echo 'OK' >&2; else echo 'NG!' >&2; fi
 fi
 if [ -e lang_pl.bin ]; then
  echo -n " Polish : " >&2
  ./update_lang.sh pl 2>./update_lang_pl.out 1>/dev/null
  if [ $? -eq 0 ]; then echo 'OK' >&2; else echo 'NG!' >&2; fi
 fi
# echo "skipped" >&2
fi

#convert bin to hex
#echo -n " converting to hex..." >&2
#$OBJCOPY -I binary -O ihex ./firmware.bin ./firmware.hex
#echo "OK" >&2

finish 0
