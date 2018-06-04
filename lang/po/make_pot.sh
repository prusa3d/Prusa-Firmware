#!/bin/sh
#
# make_pot.sh - multi-language support script
#  Generate lang.pot
#
SRCDIR="../../Firmware"
#
#


echo "make_pot.sh started" >&2

#remove output file if exists
if [ -e lang.pot ]; then rm lang.pot; fi

lang_name="English"
lang_short="en"

po_date=$(date)

#cat ../lang_en_cz.txt | sed "/^$/d;/^#/d" | sed '{N;s/\n/ /}' | sed -n '/\"\\x00\"$/p' | sed 's/ \"\\x00\"$//' > lang_pot_cz.tmp
#cat ../lang_en_de.txt | sed "/^$/d;/^#/d" | sed '{N;s/\n/ /}' | sed -n '/\"\\x00\"$/p' | sed 's/ \"\\x00\"$//' > lang_pot_de.tmp
#cat ../lang_en_es.txt | sed "/^$/d;/^#/d" | sed '{N;s/\n/ /}' | sed -n '/\"\\x00\"$/p' | sed 's/ \"\\x00\"$//' > lang_pot_es.tmp
#cat ../lang_en_it.txt | sed "/^$/d;/^#/d" | sed '{N;s/\n/ /}' | sed -n '/\"\\x00\"$/p' | sed 's/ \"\\x00\"$//' > lang_pot_it.tmp
#cat ../lang_en_pl.txt | sed "/^$/d;/^#/d" | sed '{N;s/\n/ /}' | sed -n '/\"\\x00\"$/p' | sed 's/ \"\\x00\"$//' > lang_pot_pl.tmp

#cat lang_pot_cz.tmp lang_pot_de.tmp lang_pot_es.tmp lang_pot_it.tmp lang_pot_pl.tmp | sort -u > lang_pot.tmp

#cat ../lang_en.txt | sed "/^$/d;/^#/d" > lang_pot.tmp


#write po header
echo "# Translation of Prusa-Firmware into $lang_name." > lang.pot
echo "#" >> lang.pot
echo 'msgid ""' >> lang.pot
echo 'msgstr ""' >> lang.pot
echo '"MIME-Version: 1.0\n"' >> lang.pot
echo '"Content-Type: text/plain; charset=UTF-8\n"' >> lang.pot
echo '"Content-Transfer-Encoding: 8bit\n"' >> lang.pot
echo '"Language: '$lang_short'\n"' >> lang.pot
echo '"Project-Id-Version: Prusa-Firmware\n"' >> lang.pot
echo '"POT-Creation-Date: '$po_date'\n"' >> lang.pot
echo '"PO-Revision-Date: '$po_date'\n"' >> lang.pot
echo '"Language-Team: \n"' >> lang.pot
echo '"X-Generator: Poedit 2.0.7\n"' >> lang.pot
echo '"X-Poedit-SourceCharset: UTF-8\n"' >> lang.pot
echo '"Last-Translator: \n"' >> lang.pot
echo '"Plural-Forms: nplurals=3; plural=(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;\n"' >> lang.pot
echo >> lang.pot

#list .cpp, .c and .h files
files=$(ls "$SRCDIR"/*.cpp "$SRCDIR"/*.c "$SRCDIR"/*.h)

num_texts=$(grep '^#' -c ../lang_en.txt)
#num_texts_nt=$(grep '^"' -c lang_en.txt)
#echo " $num_texts texts, $num_texts_nt not translated" >&2
echo " $num_texts texts" >&2

#loop over all messages
s0=''
s1=''
num=1
cat ../lang_en.txt | sed "s/\\\\/\\\\\\\\/g" | while read -r s; do
 if [ -z "$s" ]; then
  echo "  processing $num of $num_texts" >&2
  search=$(/bin/echo -e "$s0")
  found=$(grep -m1 -n -F "$search" $files | head -n1 | cut -f1-2 -d':' | sed "s/^.*\///")
  echo "$s1" | sed 's/ c=0//;s/ r=0//;s/^#/# /'
  echo "#: $found"
  #echo "#, fuzzy"
  /bin/echo -e "msgid $s0"
  echo 'msgstr ""'
  echo
  num=$((num+1))
 fi
 s1=$s0
 s0=$s
done >> lang.pot

#replace LF with CRLF
sync
sed -i 's/$/\r/' lang.pot

echo "make_pot.sh finished" >&2
read
exit 0
