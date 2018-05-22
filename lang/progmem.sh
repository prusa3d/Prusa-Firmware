#!/bin/sh
#
# Multi-language support postbuild script
# Description of proces:
#  0. remove output files
#  1. list symbol table of section '.text' from output elf file to text.sym (sorted by address)
#  2. list symbol table of section '.$PROGMEM' from all output object files to $PROGMEM.sym
#  3. filter only $PROGMEM symbols from text.sym and store it back to $PROGMEM.sym with absolute address
#  4. calculate start and stop address of section '.$PROGMEM'
#  5. extract string data from elf file to $PROGMEM.dat
#  6. prepare string data for character check and conversion (output to $PROGMEM.chr)
#  7. perform character check and conversion (output to $PROGMEM.var and $PROGMEM.txt)
#
# Input files:
#  Firmware.ino.elf
#  *.o (all object files)
#
# Output files:
#  text.sym
#  $PROGMEM.sym
#  $PROGMEM.lss
#  $PROGMEM.dat
#  $PROGMEM.chr
#  $PROGMEM.var
#  $PROGMEM.txt
#
# Program memory used
PROGMEM=progmem$1
if [ -z "$1" ]; then PROGMEM=progmem1; fi
#
# Output folder and elf file:
OUTDIR="../../output"
OUTELF="$OUTDIR/Firmware.ino.elf"
#
# AVR gcc tools used:
OBJDUMP=C:/arduino-1.6.8/hardware/tools/avr/bin/avr-objdump.exe
#READELF=C:/arduino-1.6.8/hardware/tools/avr/bin/avr-readelf.exe


# (0)
echo "step 0 - removing output files"
#remove output files if exists
if [ -e text.sym ]; then rm text.sym; fi
if [ -e $PROGMEM.sym ]; then rm $PROGMEM.sym; fi
if [ -e $PROGMEM.lss ]; then rm $PROGMEM.lss; fi
if [ -e $PROGMEM.dat ]; then rm $PROGMEM.dat; fi
if [ -e $PROGMEM.chr ]; then rm $PROGMEM.chr; fi
if [ -e $PROGMEM.var ]; then rm $PROGMEM.var; fi
if [ -e $PROGMEM.txt ]; then rm $PROGMEM.txt; fi

# (1)
echo "step 1 - listing symbol table of section '.text'"
#list symbols from section '.text' into file text.sym (only address, size and name)
$OBJDUMP -t -j ".text" $OUTELF | tail -n +5 | grep -E "^[0-9a-f]{8} [gl]     O" | cut -c1-9,28-36,37- | sed "/^$/d" | sort >> text.sym

# (2)
echo "step 2 - listing symbol table of section '.$PROGMEM'"
#loop over all object files
ls "$OUTDIR"/sketch/*.o | while read fn; do
 echo " processing $fn"
 #list symbols from section $PROGMEM (only address, size and name)
 $OBJDUMP -t -j ".$PROGMEM" $fn | tail -n +5 | cut -c1-9,28-36,37- | sed "/^$/d" | sort >> $PROGMEM.sym
done 2>/dev/null

# (3)
echo "step 3 - filtering $PROGMEM symbols"
#create list of $PROGMEM symbol names separated by '\|'
progmem=$(cut -c19- $PROGMEM.sym)
progmem=$(echo $progmem | sed "s/ /\\\|/g")
#filter $PROGMEM symbols from section '.text' (result file will contain list sorted by absolute address) 
cat text.sym | grep $progmem > $PROGMEM.sym

# (4)
echo "step 4 - calculating start and stop address"
#calculate start addres of section ".$PROGMEM"
PROGMEM_BEG=$(head -n1 $PROGMEM.sym | while read offs size name; do echo "0x"$offs; done)
#calculate stop addres of section ".$PROGMEM"
PROGMEM_END=$(tail -n1 $PROGMEM.sym | while read offs size name; do printf "0x%x" $(("0x"$offs + "0x"$size)); done)
echo " START address = "$PROGMEM_BEG
echo " STOP address  = "$PROGMEM_END

# (5)
echo "step 5 - extracting string data from elf"
#dump $PROGMEM data in hex format, cut textual data (keep hex data only)
$OBJDUMP -d -j ".text" -w -z --start-address=$PROGMEM_BEG --stop-address=$PROGMEM_END $OUTELF | cut -c1-57 > $PROGMEM.lss
#convert $PROGMEM.lss to $PROGMEM.dat:
# replace empty lines with '|' (variables separated by empty lines)
# remove address from multiline variables (keep address at first variable line only)
# remove '<' and '>:', remove whitespace at end of lines
# remove line-endings, replace separator with '\n' (join hex data lines - each line will contain single variable)
# filter progmem symbols
cat $PROGMEM.lss | tail -n +7 | sed -E 's/^$/|/;s/^........:\t/ /;s/<//g;s/>:/ /g;s/[ \t]*$//' |\
 tr -d '\n' | sed "s/[|]/\n/g" | grep $progmem > $PROGMEM.dat

# (6)
echo "step 6 - preparing string data"
#convert $PROGMEM.dat to $PROGMEM.chr (prepare string data for character check and conversion) 
# replace first space with tab
# replace second space with tab and space
# replace all remaining spaces with '\x'
# replace all tabs with spaces
cat $PROGMEM.dat | sed 's/ /\t/;s/ /\t /;s/ /\\x/g;s/\t/ /g' > $PROGMEM.chr

# (7)
#convert $PROGMEM.chr to $PROGMEM.var (convert data to text)
cat $PROGMEM.chr | \
 sed 's/\\x22/\\\\\\x22/g;' | \
 sed 's/\\x1b/\\\\\\x1b/g;' | \
 sed 's/\\x01/\\\\\\x01/g;' | \
 sed 's/\\x00$/\\x0d/;s/^/printf "/;s/$/"/' | sh > $PROGMEM.var
cat $PROGMEM.var | sed 's/\r/\n/g' |sed -E 's/^[0-9a-f]{8} [^ ]* //' >$PROGMEM.txt

read
