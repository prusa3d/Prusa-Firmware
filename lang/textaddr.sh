#!/bin/sh
#
# textaddr.sh - multi-language support script
#  Compile progmem1.var and lang_en.txt files to textaddr.txt file (mapping of progmem addreses to text idenifiers)
#
# Input files:
#  progmem1.var
#  lang_en.txt
#
# Output files:
#  textaddr.txt
#
#
# Dscription of process:
#  check if input files exists
#  create sorted list of strings from progmem1.var and lang_en.txt
#  lines from progmem1.var will contain addres (8 chars) and english text
#  lines from lang_en.txt will contain linenumber and english text
#  after sort this will generate pairs of lines (line from progmem1 first)
#  result of sort is compiled with simple script and stored into file textaddr.txt
#

echo "textaddr.sh started" >&2

if [ ! -e progmem1.var ]; then echo 'textaddr.sh - file progmem1.var not found!' >&2; exit 1; fi 
if [ ! -e lang_en.txt ]; then echo 'textaddr.sh - file lang_en.txt not found!' >&2; exit 1; fi 
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
   if [ "$text" = "$txt" ]; then
    addr="$addr $num"
   else
    echo "ADDR NF $addr $text"
    addr=$num
   fi
  fi
  text=$txt   
 else
  if [ -z "$addr" ]; then
   if ! [ -z "$num" ]; then echo "TEXT NF $num $txt"; fi
  else
   if [ "$text" = "$txt" ]; then
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
    if ! [ -z "$num" ]; then echo "TEXT NF $num $txt"; fi
   fi
  fi
 fi
done > textaddr.txt

echo "textaddr.sh finished" >&2

exit 0