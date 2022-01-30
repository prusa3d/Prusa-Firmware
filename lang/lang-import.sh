#!/bin/bash
#
# Version 1.0.1 Build 24
#
# lang-import.sh - multi-language support script
#  for importing translated xx.po
#
#############################################################################
# Change log:
#  9 Nov. 2018, XPila,      Initial
# 21 Nov. 2018, XPila,      fix - replace '\n' with space in all languages
# 10 Dec. 2018, jhoblitt,   make all shell scripts executable
# 21 Aug. 2019, 3d-gussner, Added "All" argument and it is default in nothing is chosen
#                           Added few German/French diacritical characters
#  6 Sep. 2019, DRracer,    change to bash
# 14 Sep. 2019, 3d-gussner, Prepare adding new language
#  1 Mar. 2019, 3d-gussner, Move `Dutch` language parts
#                           Add templates for future community languages
# 17 Dec. 2021, 3d-gussner, Use one config file for all languages
#                           Fix missing last translation
#                           Add counter
#                           replace two double quotes with `\x00`
# 21 Dec. 2021, 3d-gussner, Add Swedish, Danish, Slovanian, Hungarian,
#                           Luxembourgish, Croatian
#  3 Jan. 2022, 3d-gussner, Add Lithuanian
#                           Cleanup outaded code
# 11 Jan. 2022, 3d-gussner, Added version and Change log
#                           colored output
#                           Add Community language support
#                           Use `git rev-list --count HEAD lang-import.sh`
#                           to get Build Nr
# 14 Jan. 2022, 3d-gussner, Replace German UTF-8 'הצüß' to HD44780 A00 ROM 'הצüß'
# 28 Jan. 2022, 3d-gussner, Run lang-check and output `xx-output.txt` file to review
#                           translations
#                           new argruments `--information` `--import-check`
#############################################################################
# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr 0)" >&2; exit 1; fi

echo "$(tput setaf 2)lang-import.sh started$(tput sgr 0)" >&2

LNG=$1
# if no arguments, 'all' is selected (all po and also pot will be generated)
if [ -z "$LNG" ]; then LNG=all; fi

if [[ ! -z "$COMMUNITY_LANGUAGES" && "$LNG" = "all" ]]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
else
  LANGUAGES="$LNG"
fi
echo "$(tput setaf 2)lang-import languages:$LANGUAGES$(tput sgr 0)" >&2

# if 'all' is selected, script will generate all po files and also pot file
if [ "$LNG" = "all" ]; then
  for lang in $LANGUAGES; do
   ./lang-import.sh $lang
  done
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
 #replace '' with 'z'
 sed -i 's/\xc5\xbe/z/g' $LNG'_filtered.po'
 #replace 'ל' with 'e'
 sed -i 's/\xc4\x9b/e/g' $LNG'_filtered.po'
 #replace 'ם' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace 'ר' with 'r'
 sed -i 's/\xc5\x99/r/g' $LNG'_filtered.po'
 #replace 'ט' with 'c'
 sed -i 's/\xc4\x8d/c/g' $LNG'_filtered.po'
 #replace 'ב' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

#replace in german translation https://en.wikipedia.org/wiki/German_orthography
if [ "$LNG" = "de" ]; then
#replace UTF-8 'הצüß' to HD44780 A00 'הצüß'
 #replace 'ה' with 'A00 ROM ה'
 sed -i 's/\xc3\xa4/\\xe1/g' $LNG'_filtered.po'
 #replace 'ִ' with 'A00 ROM ה'
 sed -i 's/\xc3\x84/\\xe1/g' $LNG'_filtered.po'
 #replace 'ü' with 'A00 ROM ü'
 sed -i 's/\xc3\xbc/\\xf5/g' $LNG'_filtered.po'
 #replace 'Ü' with 'A00 ROM ü'
 sed -i 's/\xc3\x9c/\\xf5/g' $LNG'_filtered.po'
 #replace 'צ' with 'A00 ROM צ'
 sed -i 's/\xc3\xb6/\\xef/g' $LNG'_filtered.po'
 #replace 'ײ' with 'A00 ROM צ'
 sed -i 's/\xc3\x96/\\xef/g' $LNG'_filtered.po'
 #replace 'ß' with 'A00 ROM ß'
 sed -i 's/\xc3\x9f/\\xe2/g' $LNG'_filtered.po'
fi

#replace in spain translation
if [ "$LNG" = "es" ]; then
 #replace 'ב' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace '¿' with '?'
 sed -i 's/\xc2\xbf/?/g' $LNG'_filtered.po'
 #replace 'ף' with 'o'
 sed -i 's/\xc3\xb3/o/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'ם' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace '!' with '!'
 sed -i 's/\xc2\xa1/!/g' $LNG'_filtered.po'
 #replace 'n~' with 'n'
 sed -i 's/\xc3\xb1/n/g' $LNG'_filtered.po'
fi

#replace in french translation https://en.wikipedia.org/wiki/French_orthography
if [ "$LNG" = "fr" ]; then
 #replace 'ב' with 'a' (right)
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'ֱ' with 'A' (right)
 sed -i 's/\xc3\x81/A/g' $LNG'_filtered.po'
 #replace 'א' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace 'ְ' with 'A' (left)
 sed -i 's/\xc3\x80/A/g' $LNG'_filtered.po'
 #replace 'י' with 'e' (right)
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'ֹ' with 'E' (right)
 sed -i 's/\xc3\x89/E/g' $LNG'_filtered.po'
 #replace 'ט' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'ָ' with 'E' (left)
 sed -i 's/\xc3\x88/E/g' $LNG'_filtered.po'
fi

#replace in italian translation
if [ "$LNG" = "it" ]; then
 #replace 'י' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'ב' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace 'ף' with 'o' (left)
 sed -i 's/\xc3\xb2/o/g' $LNG'_filtered.po'
 #replace 'ת' with 'u' (left)
 sed -i 's/\xc3\xb9/u/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'ֹ' with 'E' (left)
 sed -i 's/\xc3\x88/E/g' $LNG'_filtered.po'
fi

#replace in dutch translation according to https://nl.wikipedia.org/wiki/Accenttekens_in_de_Nederlandse_spelling
if [ "$LNG" = "nl" ]; then
 #replace 'כ' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ן' with 'i'
 sed -i 's/\xc3\xaf/i/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'ט' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'צ' with 'o' (left)
 sed -i 's/\xc3\xb6/o/g' $LNG'_filtered.po'
 #replace 'ך' with 'e' (left)
 sed -i 's/\xc3\xaa/e/g' $LNG'_filtered.po'
 #replace 'ü' with 'u' (left)
 sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
 #replace 'ח' with 'c' (left)
 sed -i 's/\xc3\xa7/c/g' $LNG'_filtered.po'
 #replace 'ב' with 'a' (left)
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'א' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace 'ה' with 'a' (left)
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'û' with 'u' (left)
 sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
 #replace 'מ' with 'i' (left)
 sed -i 's/\xc3\xae/i/g' $LNG'_filtered.po'
 #replace 'ם' with 'i' (left)
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace 'פ' with 'o' (left)
 sed -i 's/\xc3\xb4/o/g' $LNG'_filtered.po'
 #replace 'ת' with 'u' (left)
 sed -i 's/\xc3\xba/u/g' $LNG'_filtered.po'
 #replace 'ס' with 'n' (left)
 sed -i 's/\xc3\xb1/n/g' $LNG'_filtered.po'
 #replace 'ג' with 'a' (left)
 sed -i 's/\xc3\xa2/a/g' $LNG'_filtered.po'
 #replace 'ֵ' with 'A' (left)
 sed -i 's/\xc3\x85/A/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "sv" ]; then
#repace 'ֵ' with 'Aa'
sed -i 's/\xc3\x85/Aa/g' $LNG'_filtered.po'
#repace 'ו' with 'aa'
sed -i 's/\xc3\xA5/aa/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "da" ]; then
#repace 'ֵ' with 'Aa'
sed -i 's/\xc3\x85/Aa/g' $LNG'_filtered.po'
#repace 'ו' with 'aa'
sed -i 's/\xc3\xA5/aa/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "sl" ]; then
 #replace 'כ' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ה' with 'a' (left)
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "hu" ]; then
 #replace 'כ' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ה' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "lb" ]; then
 #replace 'כ' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ה' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "hr" ]; then
 #replace 'כ' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ה' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "lt" ]; then
 #replace 'כ' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ה' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'י' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi
#replace in polish translation
#if [ "$LNG" = "pl" ]; then
#fi

#check for nonasci characters excpet HD44780 ROM A00 'הצüß'
if grep --color='auto' -P -n '[^\x00-\x7F]' $LNG'_filtered.po' >nonascii.txt; then
 exit
fi

#join lines with multi-line string constants
cat $LNG'_filtered.po' | sed ':a;N;$!ba;s/\x22\n\x22//g' > $LNG'_new.po'

#Get counter from po files

CNTTXT=$(grep '^# MSG' -c $LNGISO.po)
num=1
echo " selected language=$(tput setaf 2)$LNGISO$(tput sgr 0)" >&2
#generate new dictionary
cat ../../lang_en.txt | sed 's/\\/\\\\/g' | while read -r s; do
 /bin/echo -e "$s"
 #echo "s = $s ." >&2
 if [ "${s:0:1}" = "\"" ]; then
 
#  /bin/echo -e "$s"
  s=$(/bin/echo -e "$s")
  s2=$(grep -F -A1 -B0  "msgid $s" "$LNG"_new.po | tail -n1 | sed 's/^msgstr //')
  if [ -z "$s2" ]; then
   echo -ne "  processing $num of $CNTTXT\033[0K\r" >&2
   echo '"\x00"'
   num=$((num+1))
  else
   echo -ne "  processing $num of $CNTTXT\033[0K\r" >&2
   echo "$s2"
   num=$((num+1))
  fi
#  echo
 fi

done > lang_en_$LNG.txt
echo >&2
echo "$(tput setaf 2)Finished with $LNGISO$(tput sgr 0)" >&2
#replace two double quotes to "\x00"
sed -i 's/""/"\\x00"/g' lang_en_$LNG.txt
#remove CR
sed -i "s/\r//g" lang_en_$LNG.txt
#check new lang
./../../lang-check.py $LNG --warn-empty
./../../lang-check.py $LNG --information >$LNG-output.txt
echo >&2
echo "$(tput setaf 2)lang-import.sh finished$(tput sgr 0)">&2

