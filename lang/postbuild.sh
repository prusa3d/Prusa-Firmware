#!/bin/sh
# postbuild.sh
#
# Output folder and elf file:
OUTDIR="../../output"
OUTELF="$OUTDIR/Firmware.ino.elf"
#
# AVR gcc tools used:
OBJCOPY=C:/arduino-1.6.8/hardware/tools/avr/bin/avr-objcopy.exe

function finish
{
 echo
 echo "postbuild.sh finished... press enter key"
 read
 exit
}

echo "postbuild.sh started..."

#check input files
echo " checking files:"
if [ ! -e $OUTDIR ]; then echo "  folder '$OUTDIR' not found!"; finish; fi
echo "  folder OK"
if [ ! -e $OUTELF ]; then echo "  elf file '$OUTELF' not found!"; finish; fi
echo "  elf OK"
if ! ls $OUTDIR/sketch/*.o >/dev/null 2>&1; then echo "  no object files in '$OUTDIR/sketch/'!"; finish; fi
echo "  objects OK"

#run progmem.sh to examine content of progmem1
./progmem.sh 1

#create sorted list of strings from progmem1.var and lang_en.txt
#lines from progmem1.var will contain addres (8 chars) and english text
#lines from lang_en.txt will contain linenumber and english text
#after sort this will generate pairs of lines (line from progmem1 first)
#result of sort is compiled with simple script and stored to file textaddr.txt
echo "compiling progmem1.var and lang_en.txt"
addr=''
text=''
(cat progmem1.var | sed -E "s/^([^ ]*) ([^ ]*) (.*)/\1 \"\3\"/";\
 cat lang_en.txt | sed "/^$/d;/^#/d" | sed = | sed '{N;s/\n/ /}') |\
 sort -k2 |\
 sed "s/\\\/\\\\\\\/g" | while read num txt; do
 if [ ${#num} -eq 8 ]; then
  if [ -z "$addr" ]; then
   addr=$num
  else
   if [ "$text" == "$txt" ]; then
    addr="$addr $num"
   else
    echo "ADDR NF $addr $text"
    addr=$num
   fi
  fi
  text=$txt   
 else
  if [ -z "$addr" ]; then
   echo "TEXT NF $num $txt"
  else
   if [ "$text" == "$txt" ]; then
    if [ ${#addr} -eq 8 ]; then
     echo "ADDR OK $addr $num"
    else
     echo "$addr" | sed "s/ /\n/g" | while read ad; do
      echo "ADDR OK $ad $num"
     done
    fi
    addr=''
    text=''
   else
    echo "TEXT NF $num $txt"
   fi
  fi
 fi
done > textaddr.txt

#check for messages declared in progmem1, but not found in lang_en.txt
echo "checking textaddr.txt..."
if cat textaddr.txt | grep "^ADDR NF"; then echo "Some strings not found in lang_en.txt!"; finish; fi

#update progmem1 id entries in binary file
echo "extracting binary..."
$OBJCOPY -I ihex -O binary $OUTDIR/Firmware.ino.hex ./firmware.bin

#update progmem1 id entries in binary file
echo "updating binary..."
#dd if=/dev/zero of=test.bin bs=512 count=64 2>/dev/null
time cat textaddr.txt | grep "^ADDR OK" | cut -f3- -d' ' | sed "s/^0000/0x/" |\
 awk '{ hi = int($2 / 256); lo = int($2 - 256 * hi); printf("%d \\\\x%02x\\\\x%02x\n", strtonum($1), hi, lo); }' |\
 while read addr data; do
  echo -n -e $data | dd of=./firmware.bin bs=1 count=2 seek=$addr conv=notrunc oflag=nonblock 2>/dev/null
 done

#update progmem1 id entries in binary file
echo "converting to hex..."
$OBJCOPY -I binary -O ihex ./firmware.bin ./firmware.hex

finish
