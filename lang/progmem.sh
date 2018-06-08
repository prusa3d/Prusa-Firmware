#!/bin/sh
#
# progmem.sh - multi-language support script
#  Examine content of progmem sections (default is progmem1).
#
# Input files:
#  $OUTDIR/Firmware.ino.elf
#  $OUTDIR/sketch/*.o (all object files)
#
# Output files:
#  text.sym     - formated symbol listing of section '.text'
#  $PROGMEM.sym - formated symbol listing of section '.progmemX'
#  $PROGMEM.lss - disassembly listing file
#  $PROGMEM.hex - variables - hex
#  $PROGMEM.chr - variables - char escape
#  $PROGMEM.var - variables - strings
#  $PROGMEM.txt - text data only (not used)
#
#
# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$OUTDIR" ]; then echo 'variable OUTDIR not set!' >&2; exit 1; fi
if [ -z "$OBJDIR" ]; then echo 'variable OBJDIR not set!' >&2; exit 1; fi
if [ -z "$INOELF" ]; then echo 'variable INOELF not set!' >&2; exit 1; fi
if [ -z "$OBJDUMP" ]; then echo 'variable OBJDUMP not set!' >&2; exit 1; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi
#
# Program memory used
PROGMEM=progmem$1
if [ -z "$1" ]; then PROGMEM=progmem1; fi
#
# Description of process:
#  0. check input files
#  1. remove output files
#  2. list symbol table of section '.text' from output elf file to text.sym (sorted by address)
#  3. list symbol table of section '.$PROGMEM' from all output object files to $PROGMEM.sym
#  4. filter only $PROGMEM symbols from text.sym and store it back to $PROGMEM.sym with absolute address
#  5. calculate start and stop address of section '.$PROGMEM'
#  6. extract string data from elf file to $PROGMEM.hex
#  7. prepare string data for character check and conversion (output to $PROGMEM.chr)
#  8. perform character check and conversion (output to $PROGMEM.var and $PROGMEM.txt)
#

echo "progmem.sh started" >&2

# (0)
echo " progmem.sh (0) - checking input files" >&2
if [ ! -e $OUTDIR ]; then echo "progmem.sh - file '$INOELF' not found!" >&2; exit 1; fi

# (1)
echo " progmem.sh (1) - removing output files" >&2
#remove output files if exists
if [ -e text.sym ]; then rm text.sym; fi
if [ -e $PROGMEM.sym ]; then rm $PROGMEM.sym; fi
if [ -e $PROGMEM.lss ]; then rm $PROGMEM.lss; fi
if [ -e $PROGMEM.hex ]; then rm $PROGMEM.hex; fi
if [ -e $PROGMEM.chr ]; then rm $PROGMEM.chr; fi
if [ -e $PROGMEM.var ]; then rm $PROGMEM.var; fi
if [ -e $PROGMEM.txt ]; then rm $PROGMEM.txt; fi

# (2)
echo " progmem.sh (2) - listing symbol table of section '.text'" >&2
#list symbols from section '.text' into file text.sym (only address, size and name)
$OBJDUMP -t -j ".text" $INOELF | tail -n +5 | grep -E "^[0-9a-f]{8} [gl]     O" | cut -c1-9,28-36,37- | sed "/^$/d" | sort >> text.sym

# (3)
echo " progmem.sh (3) - listing symbol table of section '.$PROGMEM'" >&2
#loop over all object files
ls "$OBJDIR"/*.o | while read fn; do
 echo "  processing $fn" >&2
 #list symbols from section $PROGMEM (only address, size and name)
 $OBJDUMP -t -j ".$PROGMEM" $fn 2>/dev/null | tail -n +5 | cut -c1-9,28-36,37- | sed "/^$/d" | sort >> $PROGMEM.sym
done

# (4)
echo " progmem.sh (4) - filtering $PROGMEM symbols" >&2
#create list of $PROGMEM symbol names separated by '\|'
progmem=$(cut -c19- $PROGMEM.sym)
progmem=$(echo $progmem | sed "s/ /\\\b\\\|\\\b/g")
progmem='\b'$progmem'\b'
#filter $PROGMEM symbols from section '.text' (result file will contain list sorted by absolute address) 
cat text.sym | grep $progmem > $PROGMEM.sym

# (5)
echo " progmem.sh (5) - calculating start and stop address" >&2
#calculate start addres of section ".$PROGMEM"
PROGMEM_BEG=$(head -n1 $PROGMEM.sym | while read offs size name; do echo "0x"$offs; done)
#calculate stop addres of section ".$PROGMEM"
PROGMEM_END=$(tail -n1 $PROGMEM.sym | while read offs size name; do printf "0x%x" $((0x$offs + 0x$size)); done)
echo "  START address = "$PROGMEM_BEG >&2
echo "  STOP  address = "$PROGMEM_END >&2

# (6)
echo " progmem.sh (6) - extracting string data from elf" >&2
#dump $PROGMEM data in hex format, cut textual data (keep hex data only)
$OBJDUMP -d -j ".text" -w -z --start-address=$PROGMEM_BEG --stop-address=$PROGMEM_END $INOELF | cut -c1-57 > $PROGMEM.lss
#convert $PROGMEM.lss to $PROGMEM.hex:
# replace empty lines with '|' (variables separated by empty lines)
# remove address from multiline variables (keep address at first variable line only)
# remove '<' and '>:', remove whitespace at end of lines
# remove line-endings, replace separator with '\n' (join hex data lines - each line will contain single variable)
# filter progmem symbols
cat $PROGMEM.lss | tail -n +7 | sed -E 's/^$/|/;s/^........:\t/ /;s/<//g;s/>:/ /g;s/[ \t]*$//' |\
 tr -d '\n' | sed "s/[|]/\n/g" | grep $progmem > $PROGMEM.hex

# (7)
echo " progmem.sh (7) - preparing string data" >&2
#convert $PROGMEM.hex to $PROGMEM.chr (prepare string data for character check and conversion) 
# replace first space with tab
# replace second space with tab and space
# replace all remaining spaces with '\x'
# replace all tabs with spaces
cat $PROGMEM.hex | sed 's/ /\t/;s/ /\t /;s/ /\\x/g;s/\t/ /g' > $PROGMEM.chr


# (8)
#convert $PROGMEM.chr to $PROGMEM.var (convert data to text, TODO: rewrite as awk script)
echo " progmem.sh (8) - converting string data" >&2
cat $PROGMEM.chr | \
 sed 's/ \\xff\\xff/ /;' | \
 sed 's/\\x22/\\\\\\x22/g;' | \
 sed 's/\\x1b/\\\\\\x1b/g;' | \
 sed 's/\\x01/\\\\\\x01/g;' | \
 sed 's/\\xf8/\\\\\\xf8/g;' | \
 sed 's/\\x00$//;s/^/\/bin\/echo -e "/;s/$/"/' | sh > $PROGMEM.var

#this step can be omitted because .txt file is not used
#cat $PROGMEM.var | sed 's/\r/\n/g' | sed -E 's/^[0-9a-f]{8} [^ ]* //' >$PROGMEM.txt

echo "progmem.sh finished" >&2

exit 0