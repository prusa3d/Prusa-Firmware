#!/bin/sh
#
# update_lang.sh - multi-language support script
#  Update secondary language in binary file.
#
# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$OBJCOPY" ]; then echo 'variable OBJCOPY not set!' >&2; exit 1; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi
#
# Selected language:
LNG=$1
if [ -z "$LNG" ]; then LNG='cz'; fi
#

finish()
{
 echo
 if [ "$1" = "0" ]; then
  echo "update_lang.sh finished with success" >&2
 else
  echo "update_lang.sh finished with errors!" >&2
 fi
 case "$-" in
  *i*) echo "press enter key" >&2; read ;;
 esac
 exit $1
}

echo "update_lang.sh started" >&2
echo " selected language=$LNG" >&2

echo -n " checking files..." >&2
if [ ! -e text.sym ]; then echo "NG!  file text.sym not found!" >&2; finish 1; fi
if [ ! -e lang_$LNG.bin ]; then echo "NG!  file lang_$LNG.bin not found!" >&2; finish 1; fi
if [ ! -e firmware.bin ]; then echo "NG!  file firmware.bin not found!" >&2; finish 1; fi
echo "OK" >&2

echo -n " checking symbols..." >&2
#find symbol _SEC_LANG in section '.text'
sec_lang=$(cat text.sym | grep -E "\b_SEC_LANG\b")
if [ -z "$sec_lang" ]; then echo "NG!\n  symbol _SEC_LANG not found!" >&2; finish 1; fi
#find symbol _PRI_LANG_SIGNATURE in section '.text'
pri_lang=$(cat text.sym | grep -E "\b_PRI_LANG_SIGNATURE\b")
if [ -z "$pri_lang" ]; then echo "NG!\n  symbol _PRI_LANG_SIGNATURE not found!" >&2; finish 1; fi
echo "OK" >&2

echo " calculating vars:" >&2
#get pri_lang addres
pri_lang_addr='0x'$(echo $pri_lang | cut -f1 -d' ')
echo "  pri_lang_addr   =$pri_lang_addr" >&2
#get addres and size
sec_lang_addr='0x'$(echo $sec_lang | cut -f1 -d' ')
sec_lang_size='0x'$(echo $sec_lang | cut -f2 -d' ')
echo "  sec_lang_addr   =$sec_lang_addr" >&2
echo "  sec_lang_size   =$sec_lang_size" >&2
#calculate lang_table_addr (aligned to 256byte page)
lang_table_addr=$((256*$((($sec_lang_addr + 255) / 256))))
printf "  lang_table_addr =0x%04x\n" $lang_table_addr >&2
#calculate lang_table_size
lang_table_size=$((256*$((($sec_lang_size - ($lang_table_addr - $sec_lang_addr))/256))))
printf "  lang_table_size =0x%04x (=%d bytes)\n" $lang_table_size $lang_table_size >&2

#get lang_xx.bin file size
lang_file_size=$(wc -c lang_$LNG.bin | cut -f1 -d' ')
printf "  lang_file_size  =0x%04x (=%d bytes)\n" $lang_file_size $lang_file_size >&2

if [ $lang_file_size -gt $lang_table_size ]; then echo "Lanaguage binary file size too big!" >&2; finish 1; fi

echo "updating 'firmware.bin'..." >&2

dd if=lang_$LNG.bin of=firmware.bin bs=1 seek=$lang_table_addr conv=notrunc 2>/dev/null

#convert bin to hex
echo "converting to hex..." >&2
$OBJCOPY -I binary -O ihex ./firmware.bin ./firmware_$LNG.hex

finish 0
