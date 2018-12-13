#!/bin/sh
#
# lang-import.sh - multi-language support script
#  for importing translated xx.po

LNG=$1
if [ -z "$LNG" ]; then exit -1; fi

# language code (iso639-1) is equal to LNG
LNGISO=$LNG
# exception for 'cz' (code='cs')
if [ "$LNG" = "cz" ]; then LNGISO=cs; fi

# cd to input folder
cd po/new

# check if input file exists
if ! [ -e $LNGISO.po ]; then
 echo "Input file $LNGISO.po not found!" >&2
 exit -1
fi

#convert '\\e' sequencies to 'x1b' and '\\' to '\'
cat $LNGISO.po | sed 's/\\e/\\x1b/g;s/\\\\/\\/g' > $LNG'_filtered.po'

#replace '\n' with ' ' (single space)
sed -i 's/ \\n/ /g;s/\\n/ /g' $LNG'_filtered.po'

#replace in czech translation
if [ "$LNG" = "cz" ]; then
 #replace 'ž' with 'z'
 sed -i 's/\xc5\xbe/z/g' $LNG'_filtered.po'
 #replace 'ì' with 'e'
 sed -i 's/\xc4\x9b/e/g' $LNG'_filtered.po'
 #replace 'í' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace 'ø' with 'r'
 sed -i 's/\xc5\x99/r/g' $LNG'_filtered.po'
 #replace 'è' with 'c'
 sed -i 's/\xc4\x8d/c/g' $LNG'_filtered.po'
 #replace 'á' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

#replace in german translation
if [ "$LNG" = "de" ]; then
 #replace 'ä' with 'ae'
 sed -i 's/\xc3\xa4/ae/g' $LNG'_filtered.po'
 #replace 'ü' with 'ue'
 sed -i 's/\xc3\xbc/ue/g' $LNG'_filtered.po'
 #replace 'ö' with 'oe'
 sed -i 's/\xc3\xb6/oe/g' $LNG'_filtered.po'
fi

#replace in spain translation
if [ "$LNG" = "es" ]; then
 #replace 'á' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace '?' with '?'
 sed -i 's/\xc2\xbf/?/g' $LNG'_filtered.po'
 #replace 'ó' with 'o'
 sed -i 's/\xc3\xb3/o/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'í' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace '!' with '!'
 sed -i 's/\xc2\xa1/!/g' $LNG'_filtered.po'
 #replace 'n~' with 'n'
 sed -i 's/\xc3\xb1/n/g' $LNG'_filtered.po'
fi

#replace in french translation
if [ "$LNG" = "fr" ]; then
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'É' with 'E'
 sed -i 's/\xc3\x89/E/g' $LNG'_filtered.po'
 #replace 'é' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'á' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
fi

#replace in italian translation
if [ "$LNG" = "it" ]; then
 #replace 'é' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'á' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace 'ó' with 'o' (left)
 sed -i 's/\xc3\xb2/o/g' $LNG'_filtered.po'
 #replace 'ú' with 'u' (left)
 sed -i 's/\xc3\xb9/u/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'É' with 'E' (left)
 sed -i 's/\xc3\x88/E/g' $LNG'_filtered.po'
fi

#replace in polish translation
#if [ "$LNG" = "pl" ]; then
#fi

#check for nonasci characters
if grep --color='auto' -P -n '[^\x00-\x7F]' $LNG'_filtered.po' >nonasci.txt; then
 exit
fi

#join lines with multi-line string constants
cat $LNG'_filtered.po' | sed ':a;N;$!ba;s/\x22\n\x22//g' > $LNG'_new.po'

#generate new dictionary
cat ../../lang_en.txt | sed 's/\\/\\\\/g' | while read -r s; do
 /bin/echo -e "$s"
 if [ "${s:0:1}" = "\"" ]; then
#  /bin/echo -e "$s"
  s=$(/bin/echo -e "$s")
  s2=$(grep -F -A1 -B0  "$s" "$LNG"_new.po | tail -n1 | sed 's/^msgstr //')
  if [ -z "$s2" ]; then
   echo '"\x00"'
  else
   echo "$s2"
  fi
#  echo
 fi
done > lang_en_$LNG.txt
