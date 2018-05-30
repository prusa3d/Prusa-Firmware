#!/bin/sh
#
# make_po.sh - multi-language support script
#  for generating lang_xx.po
#
SRCDIR="../../Firmware"
#
LANG=$1
if [ -z "$LANG" ]; then LANG=cz; fi
#

echo "make_po.sh started" >&2

#remove output file if exists
if [ -e lang_$LANG.po ]; then rm lang_$LANG.po; fi

langname=$(\
 case "$LANG" in
  *en*) echo "English" ;;
  *cz*) echo "Czech" ;;
  *de*) echo "German" ;;
  *it*) echo "Italian" ;;
  *es*) echo "Spanish" ;;
  *pl*) echo "Polish" ;;
 esac)

#write po header
echo "# Translation into $langname." > lang_$LANG.po
echo "#" >> lang_$LANG.po
echo 'msgid ""' >> lang_$LANG.po
echo 'msgstr ""' >> lang_$LANG.po
echo '"MIME-Version: 1.0\n"' >> lang_$LANG.po
echo '"Content-Type: text/plain; charset=UTF-8\n"' >> lang_$LANG.po
echo '"Content-Transfer-Encoding: 8bit\n"' >> lang_$LANG.po
echo >> lang_$LANG.po

#list .cpp, .c and .h files
files=$(ls "$SRCDIR"/*.cpp "$SRCDIR"/*.c "$SRCDIR"/*.h)

#loop over all messages, print only untranslated (=="\x00")
s0=''
s1=''
s2=''
cat ../lang_en_$LANG.txt | sed "s/\\\\/\\\\\\\\/g;s/^#/#: /" | while read -r s; do
 if [ "$s" == "" ]; then
  if [ "$s0" == "\"\\\\x00\"" ]; then
   search=$(echo -e "$s1")
   found=$(grep -m1 -n -F "$search" $files | head -n1 | cut -f1-2 -d':' | sed "s/^.*\///")
   echo "#: $found"
   echo -e "msgid $s1"
   echo 'msgstr ""'
   echo
  fi
 fi
 s2=$s1
 s1=$s0
 s0=$s 
done >> lang_$LANG.po

echo "make_po.sh finished" >&2
exit 0
