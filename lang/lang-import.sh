#!/bin/bash
#
# Version 1.0.1 Build 13
#
# lang-import.sh - multi-language support script
#  for importing translated xx.po
#
#############################################################################
# Change log:
#  9 Nov  2018, Xpilla,     Initial
#  9 June 2020, 3d-gussner, Added version and Change log
#  9 June 2020, 3d-gussner, colored output
#  1 Mar. 2021, 3d-gussner, Add Community language support
#  2 Apr. 2021, 3d-gussner, Use `git rev-list --count HEAD lang-export.sh`
#                           to get Build Nr
#############################################################################

LNG=$1
# if no arguments, 'all' is selected (all po and also pot will be generated)
if [ -z "$LNG" ]; then LNG=all; fi

# if 'all' is selected, script will generate all po files and also pot file
if [ "$LNG" = "all" ]; then
 ./lang-import.sh cz
 ./lang-import.sh de
 ./lang-import.sh es
 ./lang-import.sh fr
 ./lang-import.sh it
 ./lang-import.sh pl
#DO NOT add Community languages here !!!
 exit 0
fi

# language code (iso639-1) is equal to LNG
LNGISO=$LNG
# exception for 'cz' (code='cs')
if [ "$LNG" = "cz" ]; then LNGISO=cs; fi

# cd to input folder
cd po/new

# check if input file exists
if ! [ -e $LNGISO.po ]; then
 echo "$(tput setaf 1)Input file $LNGISO.po not found!$(tput sgr 0)" >&2
 exit -1
fi

#convert '\\e' sequencies to 'x1b' and '\\' to '\'
cat $LNGISO.po | sed 's/\\e/\\x1b/g;s/\\\\/\\/g' > $LNG'_filtered.po'

#replace '\n' with ' ' (single space)
sed -i 's/ \\n/ /g;s/\\n/ /g' $LNG'_filtered.po'

#replace in czech translation
if [ "$LNG" = "cz" ]; then
 #replace '�' with 'z'
 sed -i 's/\xc5\xbe/z/g' $LNG'_filtered.po'
 #replace '�' with 'e'
 sed -i 's/\xc4\x9b/e/g' $LNG'_filtered.po'
 #replace '�' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace '�' with 'r'
 sed -i 's/\xc5\x99/r/g' $LNG'_filtered.po'
 #replace '�' with 'c'
 sed -i 's/\xc4\x8d/c/g' $LNG'_filtered.po'
 #replace '�' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace '�' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

#replace in german translation https://en.wikipedia.org/wiki/German_orthography
if [ "$LNG" = "de" ]; then
 #replace '�' with 'ae'
 sed -i 's/\xc3\xa4/ae/g' $LNG'_filtered.po'
 #replace '�' with 'Ae'
 sed -i 's/\xc3\x84/Ae/g' $LNG'_filtered.po'
 #replace '�' with 'ue'
 sed -i 's/\xc3\xbc/ue/g' $LNG'_filtered.po'
 #replace '�' with 'Ue'
 sed -i 's/\xc3\x9c/Ue/g' $LNG'_filtered.po'
 #replace '�' with 'oe'
 sed -i 's/\xc3\xb6/oe/g' $LNG'_filtered.po'
 #replace '�' with 'Oe'
 sed -i 's/\xc3\x96/Oe/g' $LNG'_filtered.po'
 #replace '�' with 'ss'
 sed -i 's/\xc3\x9f/ss/g' $LNG'_filtered.po'
fi

#replace in spain translation
if [ "$LNG" = "es" ]; then
 #replace '�' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace '�' with '?'
 sed -i 's/\xc2\xbf/?/g' $LNG'_filtered.po'
 #replace '�' with 'o'
 sed -i 's/\xc3\xb3/o/g' $LNG'_filtered.po'
 #replace '�' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace '�' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace '!' with '!'
 sed -i 's/\xc2\xa1/!/g' $LNG'_filtered.po'
 #replace 'n~' with 'n'
 sed -i 's/\xc3\xb1/n/g' $LNG'_filtered.po'
fi

#replace in french translation https://en.wikipedia.org/wiki/French_orthography
if [ "$LNG" = "fr" ]; then
 #replace '�' with 'a' (right)
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace '�' with 'A' (right)
 sed -i 's/\xc3\x81/A/g' $LNG'_filtered.po'
 #replace '�' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace '�' with 'A' (left)
 sed -i 's/\xc3\x80/A/g' $LNG'_filtered.po'
 #replace '�' with 'e' (right)
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace '�' with 'E' (right)
 sed -i 's/\xc3\x89/E/g' $LNG'_filtered.po'
 #replace '�' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace '�' with 'E' (left)
 sed -i 's/\xc3\x88/E/g' $LNG'_filtered.po'
fi

#replace in italian translation
if [ "$LNG" = "it" ]; then
 #replace '�' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace '�' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace '�' with 'o' (left)
 sed -i 's/\xc3\xb2/o/g' $LNG'_filtered.po'
 #replace '�' with 'u' (left)
 sed -i 's/\xc3\xb9/u/g' $LNG'_filtered.po'
 #replace '�' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace '�' with 'E' (left)
 sed -i 's/\xc3\x88/E/g' $LNG'_filtered.po'
fi

#replace in dutch translation according to https://nl.wikipedia.org/wiki/Accenttekens_in_de_Nederlandse_spelling
if [ "$LNG" = "nl" ]; then
 #replace '�' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace '�' with 'i'
 sed -i 's/\xc3\xaf/i/g' $LNG'_filtered.po'
 #replace '�' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace '�' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace '�' with 'o' (left)
 sed -i 's/\xc3\xb6/o/g' $LNG'_filtered.po'
 #replace '�' with 'e' (left)
 sed -i 's/\xc3\xaa/e/g' $LNG'_filtered.po'
 #replace '�' with 'u' (left)
 sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
 #replace '�' with 'c' (left)
 sed -i 's/\xc3\xa7/c/g' $LNG'_filtered.po'
 #replace '�' with 'a' (left)
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace '�' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace '�' with 'a' (left)
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace '�' with 'u' (left)
 sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
 #replace '�' with 'i' (left)
 sed -i 's/\xc3\xae/i/g' $LNG'_filtered.po'
 #replace '�' with 'i' (left)
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace '�' with 'o' (left)
 sed -i 's/\xc3\xb4/o/g' $LNG'_filtered.po'
 #replace '�' with 'u' (left)
 sed -i 's/\xc3\xba/u/g' $LNG'_filtered.po'
 #replace '�' with 'n' (left)
 sed -i 's/\xc3\xb1/n/g' $LNG'_filtered.po'
 #replace '�' with 'a' (left)
 sed -i 's/\xc3\xa2/a/g' $LNG'_filtered.po'
 #replace '�' with 'A' (left)
 sed -i 's/\xc3\x85/A/g' $LNG'_filtered.po'
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
