#!/bin/bash
#
# lang_check.sh - multi-language support script
#  check lang_xx.bin (language binary file)
#
# Input files:
#  lang_$1.bin
#  lang_en.txt or lang_en_$1.txt
#  
#

#set 'cz'

#dictionary txt file
fn_t=lang_en_$1.txt
if [ "$1" = "en" ]; then fn_t=lang_en.txt; fi
#binary file to check
fn_b=lang_$1.bin

#check txt dictionary file
echo -n "dictionary file: $fn_t"
if [ -e $fn_t ]; then echo " - OK"; else echo " - Not found!"; exit 1; fi

#create lang_xx.tmp - different processing for 'en' language
if [ "$1" = "en" ]; then
 #remove comments and empty lines
 cat lang_en.txt | sed '/^$/d;/^#/d'
else
 #remove comments and empty lines, print lines with translated text only
 cat lang_en_$1.txt | sed '/^$/d;/^#/d' | sed -n 'n;p'
fi | sed 's/^\"\\x00\"$/\"\"/' > lang_$1.tmp

count_txt=$(grep -c '^"' lang_$1.tmp)

echo -n "language bin file: $fn_b"
if [ -e $fn_b ]; then echo " - OK"; else echo " - Not found!"; exit 1; fi

#read header and convert to hex
header=$(dd if=$fn_b bs=1 count=16 2>/dev/null | xxd | cut -c11-49 | sed 's/\([0-9a-f][0-9a-f]\)[\ ]*/\1 /g')
echo "header='$header'"
magic=0x$(echo $header | tr -d ' ' | cut -c1-8)
echo "magic='$magic'"
size=$(echo $header | tr -d ' ' | cut -c9-12)
size=0x${size:2:2}${size:0:2}
echo "size='$size' ($(($size)))"
count=$(echo $header | tr -d ' ' | cut -c13-16)
count=0x${count:2:2}${count:0:2}
echo "count='$count' ($(($count)))"
o=0
l=0
#create lang_xx_1.tmp (temporary text file from binary data)
(dd if=$fn_b bs=1 count=$((2*$count)) skip=16 2>/dev/null | xxd | cut -c11-49 | tr ' ' "\n" |\
 sed 's/\([0-9a-f][0-9a-f]\)\([0-9a-f][0-9a-f]\)/\2\1 /g;/^$/d'; printf "%04x\n" $(($size)) ) |\
 while read offs; do
  if [ $o -ne 0 ]; then
   l=$((0x$offs - $o))
   echo -n '"'
   dd if=$fn_b bs=1 count=$((l-1)) skip=$o 2>/dev/null
   echo '"'
  fi
  o=$((0x$offs))
 done > lang_$1_1.tmp
#create lang_xx_2.tmp (temporary text file from dictionary)
cat lang_$1.tmp | sed 's/^\"/printf \"\\x22/;s/"$/\\x22\\x0a\"/' | sh >lang_$1_2.tmp
#compare temporary files
diff -a lang_$1_1.tmp lang_$1_2.tmp >lang_$1_check.dif
dif=$(cat lang_$1_check.dif)
if [ -z "$dif" ]; then
 echo 'binary data OK'
else
 echo 'binary data NG!'
fi

read
exit
