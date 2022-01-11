#!/bin/bash
#
# Version 1.0.2 Build 12
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
#############################################################################
# Change log:
# 31 May  2018, XPila,      Initial
# 17 Dec. 2021, 3d-gussner, Use one config file for all languages
# 11 Jan. 2022, 3d-gussner, Add check for not translated messages using a
#                           parameter
#                           Added version and Change log
#                           colored output
#                           Add Community language support
#                           Use `git rev-list --count HEAD fw-build.sh`
#                           to get Build Nr
#############################################################################
#
# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr0)" >&2; exit 1; fi
#
# Selected language:
LNG=$1

#Set default to ignore missing text
CHECK_MISSING_TEXT=0
#Check if script should check for missing messages in the source code aren't translated by using parameter "--check-missing-text"
if [ "$1" = "--check-missing-text" ]; then
 CHECK_MISSING_TEXT=1
fi

# List of supported languages
if [ -z "$LANGUAGES" ]; then
 LANGUAGES="cz de es fr it pl"
fi

# Community languages
if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi
echo "$(tput setaf 2)fw-build.sh started$(tput sgr 0)" >&2
echo "fw-build languages:$(tput setaf 2)$LANGUAGES$(tput sgr 0)" >&2

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

#check input files
echo " checking files:" >&2
if [ ! -e $OUTDIR ]; then echo "$(tput setaf 1)  folder '$OUTDIR' not found!$(tput sgr 0)" >&2; finish 1; fi
echo "$(tput setaf 2)  folder  OK$(tput sgr 0)" >&2
if [ ! -e $INOELF ]; then echo "$(tput setaf 1)  elf file '$INOELF' not found!$(tput sgr 0)" >&2; finish 1; fi
echo "$(tput setaf 2)  elf     OK$(tput sgr 0)" >&2
if ! ls $OBJDIR/*.o >/dev/null 2>&1; then echo "$(tput setaf 1)  no object files in '$OBJDIR/'!$(tput sgr 0)" >&2; finish 1; fi
echo "$(tput setaf 2)  objects OK$(tput sgr 0)" >&2

#run progmem.sh - examine content of progmem1
echo -n " running progmem.sh..." >&2
./progmem.sh 1 2>progmem.out
if [ $? -ne 0 ]; then echo "$(tput setaf 1)NG! - check progmem.out file$(tput sgr 0)" >&2; finish 1; fi
echo "$(tput setaf 2)OK$(tput sgr 0)" >&2

#run textaddr.sh - map progmem addreses to text identifiers
echo -n " running textaddr.sh..." >&2
./textaddr.sh 2>textaddr.out
if [ $? -ne 0 ]; then echo "$(tput setaf 1)NG! - check progmem.out file$(tput sgr 0)" >&2; finish 1; fi
echo "$(tput setaf 2)OK$(tput sgr 0)" >&2

#check for messages declared in progmem1, but not found in lang_en.txt
echo -n " checking textaddr.txt..." >&2
cat textaddr.txt | grep "^TEXT NF" | sed "s/[^\"]*\"//;s/\"$//" >not_used.txt
cat textaddr.txt | grep "^ADDR NF" | sed "s/[^\"]*\"//;s/\"$//" >not_tran.txt
if cat textaddr.txt | grep "^ADDR NF" >/dev/null; then
 echo "$(tput setaf 1)NG! - some texts not found in lang_en.txt!$(tput sgr 0)"
 if [ $CHECK_MISSING_TEXT -eq 1 ]; then
  echo "$(tput setaf 1)Missing text found, please update the language files!$(tput setaf 6)" >&2
  cat not_tran.txt >&2
  finish 1
 else
  echo "$(tput setaf 3)  missing text ignored!$(tput sgr 0)" >&2
 fi
else
 echo "$(tput setaf 2)OK$(tput sgr 0)" >&2
fi

#extract binary file
echo -n " extracting binary..." >&2
$OBJCOPY -I ihex -O binary $INOHEX ./firmware.bin
echo "$(tput setaf 2)OK$(tput sgr 0)" >&2

#update binary file
echo " updating binary:" >&2

#update progmem1 id entries in binary file
echo -n "  primary language ids..." >&2
cat textaddr.txt | grep "^ADDR OK" | cut -f3- -d' ' | sed "s/^0000/0x/" |\
 awk '{ id = $2 - 1; hi = int(id / 256); lo = int(id - 256 * hi); printf("%d \\\\x%02x\\\\x%02x\n", strtonum($1), lo, hi); }' |\
 while read addr data; do
  /bin/echo -n -e $data | dd of=./firmware.bin bs=1 count=2 seek=$addr conv=notrunc oflag=nonblock 2>/dev/null
 done
echo "$(tput setaf 2)OK$(tput sgr 0)" >&2

#update primary language signature in binary file
echo -n "  primary language signature..." >&2
if [ -e lang_en.bin ]; then
 #find symbol _PRI_LANG_SIGNATURE in section '.text'
 pri_lang=$(cat text.sym | grep -E "\b_PRI_LANG_SIGNATURE\b")
 if [ -z "$pri_lang" ]; then echo "$(tput setaf 1)NG!\n  symbol _PRI_LANG_SIGNATURE not found!$(tput sgr 0)" >&2; finish 1; fi
 #get pri_lang address
 pri_lang_addr='0x'$(echo $pri_lang | cut -f1 -d' ')
 #read header from primary language binary file
 header=$(dd if=lang_en.bin bs=1 count=16 2>/dev/null | xxd | cut -c11-49 | sed 's/\([0-9a-f][0-9a-f]\)[\ ]*/\1 /g')
 #read checksum and count data as 4 byte signature
 chscnt=$(echo $header | cut -c18-29 | sed "s/ /\\\\x/g")
 /bin/echo -e -n "$chscnt" |\
  dd of=firmware.bin bs=1 count=4 seek=$(($pri_lang_addr)) conv=notrunc 2>/dev/null
 echo "$(tput setaf 2)OK$(tput sgr 0)" >&2
else
 echo "$(tput setaf 1)NG! - file lang_en.bin not found!$(tput sgr 0)" >&2;
 finish 1
fi

#convert bin to hex
echo -n " converting primary to hex..." >&2
$OBJCOPY -I binary -O ihex ./firmware.bin ./firmware.hex
echo "$(tput setaf 2)OK$(tput sgr 0)" >&2

#update _SEC_LANG in binary file if language is selected
echo -n " secondary language data..." >&2
if [ ! -z "$LNG" ]; then
 ./update_lang.sh $LNG 2>./update_lang.out
 if [ $? -ne 0 ]; then echo "$(tput setaf 1)NG! - check update_lang.out file$(tput sgr 0)" >&2; finish 1; fi
 echo "$(tput setaf 2)OK$(tput sgr 0)" >&2
 finish 0
else
 echo >&2
 echo "  Updating languages:" >&2
 for lang in $LANGUAGES; do
  if [ -e lang_$lang.bin ]; then
   echo -n "   $lang  : " >&2
   ./update_lang.sh $lang 2>./update_lang_$lang.out 1>/dev/null
   if [ $? -eq 0 ]; then echo "$(tput setaf 2)OK$(tput sgr 0)" >&2; else echo "$(tput setaf 1)NG!$(tput sgr 0)" >&2; finish 1; fi
  fi
 done 
fi

#create binary file with all languages
rm -f lang.bin
for lang in $LANGUAGES; do
 if [ -e lang_$lang.bin ]; then cat lang_$lang.bin >> lang.bin; fi
done

# Check that the language data doesn't exceed the reserved XFLASH space
echo " checking language data size:"
lang_size=$(wc -c lang.bin | cut -f1 -d' ')
lang_size_pad=$(( ($lang_size+4096-1) / 4096 * 4096 ))

# TODO: hard-coded! get value by preprocessing LANG_SIZE from xflash_layout.h!
lang_reserved=249856

echo -n "  total size usage: " >&2
if [ $lang_size_pad -gt $lang_reserved ]; then
  echo -n "$(tput setaf 1)" >&2
else
  echo -n "$(tput setaf 2)" >&2
fi
echo "$lang_size_pad ($lang_size)$(tput sgr 0)" >&2
echo "  reserved size:    $(tput setaf 2)$lang_reserved$(tput sgr 0)" >&2
if [ $lang_size_pad -gt $lang_reserved ]; then
  echo "$(tput setaf 1)NG! - language data too large$(tput sgr 0)" >&2
  finish 1
fi

#convert lang.bin to lang.hex
echo -n " converting multi language to hex..." >&2
$OBJCOPY -I binary -O ihex ./lang.bin ./lang.hex
echo "$(tput setaf 2)OK$(tput sgr 0)" >&2

#append languages to hex file
cat ./lang.hex >> firmware.hex

finish 0
