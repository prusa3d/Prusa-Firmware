#!/bin/bash
#
# Version 1.0.1 Build 46
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
# 14 Jan. 2022, 3d-gussner, Replace German UTF-8 'äöÿÿ' to HD44780 A00 ROM 'äöÿÿ'
# 28 Jan. 2022, 3d-gussner, Run lang-check and output `xx-output.txt` file to review
#                           translations
#                           new argruments `--information` `--import-check`
# 11 Jan. 2022, ingbrzy,    Add Slovak letters
# 11 Feb. 2022, 3d-gussner, Change to python3
# 14 Feb. 2022, 3d-gussner, Replace non-block space with space
#                           Fix single language run without config.sh OK
# 12 Mar. 2022, 3d-gussner, Update Norwegian replace umlaut and diacritics
#                           Update Swedish umlaut and diacritics
#############################################################################

echo "$(tput setaf 2)lang-import.sh started$(tput sgr 0)" >&2

LNG=$1
# if no arguments, 'all' is selected (all po and also pot will be generated)
if [ -z "$LNG" ]; then
  LNG=all;
# Config:
  if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
  if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr 0)" >&2; exit 1; fi
fi

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
 #replace 'Á' with 'A'
 sed -i 's/\xc3\x81/A/g' $LNG'_filtered.po'
 #replace 'á' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'Č' with 'C'
 sed -i 's/\xc4\x8c/C/g' $LNG'_filtered.po'
 #replace 'č' with 'c'
 sed -i 's/\xc4\x8d/c/g' $LNG'_filtered.po'
 #replace 'Ď' with 'D'
 sed -i 's/\xc4\x8e/D/g' $LNG'_filtered.po'
 #replace 'ď' with 'd'
 sed -i 's/\xc4\x8f/d/g' $LNG'_filtered.po'
 #replace 'É' with 'E'
 sed -i 's/\xc3\x89/E/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'Ě' with 'E'
 sed -i 's/\xc4\x9a/E/g' $LNG'_filtered.po'
 #replace 'ě' with 'e'
 sed -i 's/\xc4\x9b/e/g' $LNG'_filtered.po'
 #replace 'Í' with 'I'
 sed -i 's/\xc3\x8d/I/g' $LNG'_filtered.po'
 #replace 'í' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace 'Ň' with 'N'
 sed -i 's/\xc5\x87/N/g' $LNG'_filtered.po'
 #replace 'ň' with 'n'
 sed -i 's/\xc5\x88/n/g' $LNG'_filtered.po'
 #replace 'Ó' with 'O'
 sed -i 's/\xc3\x93/O/g' $LNG'_filtered.po'
 #replace 'ó' with 'o'
 sed -i 's/\xc3\xb3/o/g' $LNG'_filtered.po'
 #replace 'Ř' with 'R'
 sed -i 's/\xc5\x98/R/g' $LNG'_filtered.po'
 #replace 'ř' with 'r'
 sed -i 's/\xc5\x99/r/g' $LNG'_filtered.po'
 #replace 'Š' with 'S'
 sed -i 's/\xc5\xa0/S/g' $LNG'_filtered.po'
 #replace 'š' with 's'
 sed -i 's/\xc5\xa1/s/g' $LNG'_filtered.po'
 #replace 'Ť' with 'T'
 sed -i 's/\xc5\xa4/T/g' $LNG'_filtered.po'
 #replace 'ť' with 't'
 sed -i 's/\xc5\xa5/t/g' $LNG'_filtered.po'
 #replace 'Ú' with 'U'
 sed -i 's/\xc3\x9a/U/g' $LNG'_filtered.po'
 #replace 'ú' with 'u'
 sed -i 's/\xc3\xba/u/g' $LNG'_filtered.po'
 #replace 'Ů' with 'U'
 sed -i 's/\xc5\xae/U/g' $LNG'_filtered.po'
 #replace 'ů' with 'u'
 sed -i 's/\xc5\xaf/u/g' $LNG'_filtered.po'
 #replace 'Ý' with 'Y'
 sed -i 's/\xc3\x9d/Y/g' $LNG'_filtered.po'
 #replace 'ý' with 'y'
 sed -i 's/\xc3\xbd/y/g' $LNG'_filtered.po'
 #replace 'Ž' with 'Z'
 sed -i 's/\xc5\xbd/Z/g' $LNG'_filtered.po'
 #replace 'ž' with 'z'
 sed -i 's/\xc5\xbe/z/g' $LNG'_filtered.po'
fi

#replace in German translation https://en.wikipedia.org/wiki/German_orthography
#replace in Swedish as well
if [[ "$LNG" = "de" || "$LNG" = "sv" ]]; then
#replace UTF-8 'äöüß' to HD44780 A00 'äöüß'
 #replace 'ä' with 'A00 ROM ä'
 sed -i 's/\xc3\xa4/\\xe1/g' $LNG'_filtered.po'
 #replace 'Ä' with 'A00 ROM ä'
 sed -i 's/\xc3\x84/\\xe1/g' $LNG'_filtered.po'
 #replace 'ü' with 'A00 ROM ü'
 sed -i 's/\xc3\xbc/\\xf5/g' $LNG'_filtered.po'
 #replace 'Ü' with 'A00 ROM ü'
 sed -i 's/\xc3\x9c/\\xf5/g' $LNG'_filtered.po'
 #replace 'ö' with 'A00 ROM ö'
 sed -i 's/\xc3\xb6/\\xef/g' $LNG'_filtered.po'
 #replace 'Ö' with 'A00 ROM ö'
 sed -i 's/\xc3\x96/\\xef/g' $LNG'_filtered.po'
 #replace 'ß' with 'A00 ROM ß'
 sed -i 's/\xc3\x9f/\\xe2/g' $LNG'_filtered.po'
fi

#replace in spain translation
if [ "$LNG" = "es" ]; then
 #replace 'á' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace '¿' with '?'
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

#replace in french translation https://en.wikipedia.org/wiki/French_orthography
if [ "$LNG" = "fr" ]; then
 #replace 'á' with 'a' (right)
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'Á' with 'A' (right)
 sed -i 's/\xc3\x81/A/g' $LNG'_filtered.po'
 #replace 'à' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace 'À' with 'A' (left)
 sed -i 's/\xc3\x80/A/g' $LNG'_filtered.po'
 #replace 'é' with 'e' (right)
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'É' with 'E' (right)
 sed -i 's/\xc3\x89/E/g' $LNG'_filtered.po'
 #replace 'è' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'È' with 'E' (left)
 sed -i 's/\xc3\x88/E/g' $LNG'_filtered.po'
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

#replace in dutch translation according to https://nl.wikipedia.org/wiki/Accenttekens_in_de_Nederlandse_spelling
if [ "$LNG" = "nl" ]; then
 #replace 'ë' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ï' with 'i'
 sed -i 's/\xc3\xaf/i/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'è' with 'e' (left)
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'ö' with 'o' (left)
 sed -i 's/\xc3\xb6/o/g' $LNG'_filtered.po'
 #replace 'ê' with 'e' (left)
 sed -i 's/\xc3\xaa/e/g' $LNG'_filtered.po'
 #replace 'ü' with 'u' (left)
 sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
 #replace 'ç' with 'c' (left)
 sed -i 's/\xc3\xa7/c/g' $LNG'_filtered.po'
 #replace 'á' with 'a' (left)
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'à' with 'a' (left)
 sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
 #replace 'ä' with 'a' (left)
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'û' with 'u' (left)
 sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
 #replace 'î' with 'i' (left)
 sed -i 's/\xc3\xae/i/g' $LNG'_filtered.po'
 #replace 'í' with 'i' (left)
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace 'ô' with 'o' (left)
 sed -i 's/\xc3\xb4/o/g' $LNG'_filtered.po'
 #replace 'ú' with 'u' (left)
 sed -i 's/\xc3\xba/u/g' $LNG'_filtered.po'
 #replace 'ñ' with 'n' (left)
 sed -i 's/\xc3\xb1/n/g' $LNG'_filtered.po'
 #replace 'â' with 'a' (left)
 sed -i 's/\xc3\xa2/a/g' $LNG'_filtered.po'
 #replace 'Å' with 'A' (left)
 sed -i 's/\xc3\x85/A/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "sv" ]; then
#repace 'Å' with 'A'
sed -i 's/\xc3\x85/A/g' $LNG'_filtered.po'
#repace 'å' with 'a'
sed -i 's/\xc3\xA5/a/g' $LNG'_filtered.po'
fi

#https://en.wikipedia.org/wiki/Norwegian_orthography éèêóòôù ÅåÆæØø
if [ "$LNG" = "no" ]; then
 #replace UTF-8 'æÆøØ' to HD44780 A00 'äö'
 #repace 'Æ' with 'Ä'
 sed -i 's/\xc3\x86/\\xe1/g' $LNG'_filtered.po'
 #repace 'æ' with 'ä'
 sed -i 's/\xc3\xa6/\\xe1/g' $LNG'_filtered.po'
 #repace 'Ø' with 'Ö'
 sed -i 's/\xc3\x98/\\xef/g' $LNG'_filtered.po'
 #repace 'ø' with 'ö'
 sed -i 's/\xc3\xb8/\\xef/g' $LNG'_filtered.po'
 #replace diacritics
 #repace 'Å' with 'A'
 sed -i 's/\xc3\x85/A/g' $LNG'_filtered.po'
 #repace 'å' with 'a'
 sed -i 's/\xc3\xa5/a/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'è' with 'e'
 sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
 #replace 'ê' with 'e'
 sed -i 's/\xc3\xaa/e/g' $LNG'_filtered.po'
 #replace 'ó' with 'o'
 sed -i 's/\xc3\xb3/o/g' $LNG'_filtered.po'
 #replace 'ò' with 'o'
 sed -i 's/\xc3\xb2/o/g' $LNG'_filtered.po'
 #replace 'ô' with 'o'
 sed -i 's/\xc3\xb4/o/g' $LNG'_filtered.po'
 #replace 'ù' with 'u'
 sed -i 's/\xc3\xb9/u/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "da" ]; then
#repace 'Å' with 'Aa'
sed -i 's/\xc3\x85/Aa/g' $LNG'_filtered.po'
#repace 'å' with 'aa'
sed -i 's/\xc3\xA5/aa/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "sl" ]; then
 #replace 'ë' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ä' with 'a' (left)
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "hu" ]; then # See https://www.fileformat.info/info/charset/UTF-8/list.htm
 #replace 'Á' with 'A'(acute)
 sed -i 's/\xc3\x81/A/g' $LNG'_filtered.po'
 #replace 'á' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'É' with 'E' (acute)
 sed -i 's/\xc3\x89/E/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'Í' with 'I' (acute)
 sed -i 's/\xc3\x8d/I/g' $LNG'_filtered.po'
 #replace 'i̇́' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace 'Ó' with 'O' (acute)
 sed -i 's/\xc3\x93/O/g' $LNG'_filtered.po'
 #replace 'ó' with 'o'
 sed -i 's/\xc3\xb3/o/g' $LNG'_filtered.po'
 #replace 'Ö' with 'O' (diaresis)
 sed -i 's/\xc3\x96/O/g' $LNG'_filtered.po'
 #replace 'ö' with 'o'
 sed -i 's/\xc3\xb6/o/g' $LNG'_filtered.po'
 #replace 'Ő' with 'O' (double acute)
 sed -i 's/\xc5\x90/O/g' $LNG'_filtered.po'
 #replace 'ő' with 'o'
 sed -i 's/\xc5\x91/o/g' $LNG'_filtered.po'
 #replace 'Ú' with 'U' (acute) 
 sed -i 's/\xc3\x9a/U/g' $LNG'_filtered.po'
 #replace 'ú' with 'u'
 sed -i 's/\xc3\xba/u/g' $LNG'_filtered.po'
 #replace 'Ü' with 'U' (diaersis)
 sed -i 's/\xc3\x9c/U/g' $LNG'_filtered.po'
 #replace 'ü' with 'u'
 sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
 #replace 'Ű' with 'U' (double acute)
 sed -i 's/\xc5\xb0/U/g' $LNG'_filtered.po'
 #replace 'ű' with 'u'
 sed -i 's/\xc5\xb1/u/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "lb" ]; then
 #replace 'ë' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ä' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "hr" ]; then
 #replace 'ë' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ä' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi

if [ "$LNG" = "lt" ]; then
 #replace 'ë' with 'e'
 sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
 #replace 'ä' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
fi
#replace in polish translation
#if [ "$LNG" = "pl" ]; then
#fi

#replace in slovak translation
if [ "$LNG" = "sk" ]; then
 #replace 'Á' with 'A'
 sed -i 's/\xc3\x81/A/g' $LNG'_filtered.po'
 #replace 'á' with 'a'
 sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
 #replace 'ä' with 'a'
 sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
 #replace 'Č' with 'C'
 sed -i 's/\xc4\x8c/C/g' $LNG'_filtered.po'
 #replace 'č' with 'c'
 sed -i 's/\xc4\x8d/c/g' $LNG'_filtered.po'
 #replace 'Ď' with 'D'
 sed -i 's/\xc4\x8e/D/g' $LNG'_filtered.po'
 #replace 'ď' with 'd'
 sed -i 's/\xc4\x8f/d/g' $LNG'_filtered.po'
 #replace 'É' with 'E'
 sed -i 's/\xc3\x89/E/g' $LNG'_filtered.po'
 #replace 'é' with 'e'
 sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
 #replace 'Í' with 'I'
 sed -i 's/\xc3\x8d/I/g' $LNG'_filtered.po'
 #replace 'í' with 'i'
 sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
 #replace 'ľ' with 'l'
 sed -i 's/\xc4\xbe/l/g' $LNG'_filtered.po'
 #replace 'Ľ' with 'L'
 sed -i 's/\xc4\xbd/L/g' $LNG'_filtered.po'
 #replace 'Ň' with 'N'
 sed -i 's/\xc5\x87/N/g' $LNG'_filtered.po'
 #replace 'ň' with 'n'
 sed -i 's/\xc5\x88/n/g' $LNG'_filtered.po'
 #replace 'Ó' with 'O'
 sed -i 's/\xc3\x93/O/g' $LNG'_filtered.po'
 #replace 'ó' with 'o'
 sed -i 's/\xc3\xb3/o/g' $LNG'_filtered.po'
 #replace 'ô' with 'o'
 sed -i 's/\xc3\xb4/o/g' $LNG'_filtered.po'
 #replace 'Ô' with 'O'
 sed -i 's/\xc3\x94/O/g' $LNG'_filtered.po'
 #replace 'ŕ' with 'r'
 sed -i 's/\xc5\x95/r/g' $LNG'_filtered.po'
 #replace 'Ŕ' with 'R'
 sed -i 's/\xc5\x94/R/g' $LNG'_filtered.po'
 #replace 'Š' with 'S'
 sed -i 's/\xc5\xa0/S/g' $LNG'_filtered.po'
 #replace 'š' with 's'
 sed -i 's/\xc5\xa1/s/g' $LNG'_filtered.po'
 #replace 'Ť' with 'T'
 sed -i 's/\xc5\xa4/T/g' $LNG'_filtered.po'
 #replace 'ť' with 't'
 sed -i 's/\xc5\xa5/t/g' $LNG'_filtered.po'
 #replace 'Ú' with 'U'
 sed -i 's/\xc3\x9a/U/g' $LNG'_filtered.po'
 #replace 'ú' with 'u'
 sed -i 's/\xc3\xba/u/g' $LNG'_filtered.po'
 #replace 'Ý' with 'Y'
 sed -i 's/\xc3\x9d/Y/g' $LNG'_filtered.po'
 #replace 'ý' with 'y'
 sed -i 's/\xc3\xbd/y/g' $LNG'_filtered.po'
 #replace 'Ž' with 'Z'
 sed -i 's/\xc5\xbd/Z/g' $LNG'_filtered.po'
 #replace 'ž' with 'z'
 sed -i 's/\xc5\xbe/z/g' $LNG'_filtered.po'
fi

#replace UTF-8 'μ' to HD44780 A00 'μ'
 #replace 'μ' with 'A00 ROM μ'
 sed -i 's/\xce\xbc/\\xe4/g' $LNG'_filtered.po'
#replace non-break space with space
 sed -i 's/\xc2\xa0/ /g' $LNG'_filtered.po'

#check for nonasci characters except HD44780 ROM A00 'äöüß'
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
python3 ../../lang-check.py $LNG --warn-empty
#gerenate some output
python3 ../../lang-check.py $LNG --information >output-layout-$LNG.txt
grep "msgstr" $LNGISO.po | cut -d '"' -f2 | sort >output-sorted-$LNG.txt
echo >&2
echo "$(tput setaf 2)lang-import.sh finished$(tput sgr 0)">&2

