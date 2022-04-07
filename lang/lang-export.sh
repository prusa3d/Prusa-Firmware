#!/bin/bash
#
# Version 1.0.1 Build 32
#
# lang-export.sh - multi-language support script
#  for generating /lang/po/Firmware_xx.po
#
#############################################################################
# Change log:
#  9 Nov. 2018, XPila,      Initial
# 10 Dec. 2018, jhoblitt,   make all shell scripts executable
# 14 Sep. 2019, 3d-gussner, Prepare adding new language
#  6 Sep. 2019, DRracer,    change to bash
#  1 Mar. 2019, 3d-gussner, Move `Dutch` language parts
#                           Add templates for future community languages
# 17 Dec. 2021, 3d-gussner, Use one config file for all languages
#                           Fix missing last translation
# 21 Dec. 2021, 3d-gussner, Add Swedish, Danish, Slovanian, Hungarian,
#                           Luxembourgish, Croatian
#  3 Jan. 2022, 3d-gussner, Add Lithuanian
#                           Cleanup outaded code
# 11 Jan. 2022, 3d-gussner, Added version and Change log
#                           colored output
#                           Add Community language support
#                           Use `git rev-list --count HEAD lang-export.sh`
#                           to get Build Nr
# 25 Jan. 2022, 3d-gussner, Replace German HD44780 A00 ROM 'äöüß' to UTF-8 'äöüß'
# 14 Feb. 2022, 3d-gussner, Fix single language run without config.sh OK
# 12 Mar. 2022, 3d-gussner, Update Norwegian replace umlaut and diacritics
#                           Fix find community languages
#                           Update Swedish replace umlaut and diacritics
#                           Replace '.!? äöü' with '.!? ÄÖÜ' in German and Swedish
#                           Replace '"äöü' with '"ÄÖÜ' in German and Swedish
# 18 Mar. 2022, 3d-gussner, Add Swedish ` pa ` to ` på ` conversion
#############################################################################

echo "$(tput setaf 2)lang-export.sh started$(tput sgr 0)" >&2

# relative path to source folder
if [ -z "$SRCDIR" ]; then
   SRCDIR=".."
fi

# selected language is 1st argument (cz, de, ...)
LNG=$1

# if no arguments, 'all' is selected (all po and also pot will be generated)
if [ -z "$LNG" ]; then
  LNG="all";
# Config:
  if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
  if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr 0)" >&2; exit 1; fi
  if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
    LANGUAGES+=" $COMMUNITY_LANGUAGES"
    echo $LANGUAGES>&2
  fi
  echo "$(tput setaf 2)lang-export languages:$LANGUAGES$(tput sgr 0)" >&2
fi

# if 'all' is selected, script will generate all po files and also pot file
if [ "$LNG" = "all" ]; then
 ./lang-export.sh en
  for lang in $LANGUAGES; do
   ./lang-export.sh $lang
  done
 exit 0
fi

# language code (iso639-1) is equal to LNG
LNGISO=$LNG
# exception for 'cz' (code='cs')
if [ "$LNG" = "cz" ]; then LNGISO=cs; fi

# po/pot creation/revision date
DATE=$(date)

# if 'en' is selected, generate pot instead of po
if [ "$LNG" = "en" ]; then
 INFILE=lang_en.txt
 OUTFILE=po/Firmware.pot
 LNGNAME="English"
else
 # language name in english
 LNGNAME=$(\
  case "$LNG" in
   *cz*) echo "Czech" ;;
   *de*) echo "German" ;;
   *es*) echo "Spanish" ;;
   *fr*) echo "French" ;;
   *it*) echo "Italian" ;;
   *pl*) echo "Polish" ;;
#Community language support
#Dutch
   *nl*) echo "Dutch" ;;
#Swedish
   *sv*) echo "Swedish" ;;
#Norwegian
   *no*) echo "Norwegian" ;;
#Danish
   *da*) echo "Danish" ;;
#Slovak
   *sk*) echo "Slovak" ;;   
#Slovanian
   *sl*) echo "Slovanian" ;;
#Hugarian
   *hu*) echo "Hugarian" ;;
#Luxembourgish
   *lb*) echo "Luxembourgish" ;;
#Croatian
   *hr*) echo "Croatian" ;;
#Lithuanian
   *lt*) echo "Lithuanian" ;;
#Romanian
   *ro*) echo "Romanian" ;;
#Use the 2 lines below as a template and replace 'qr' and 'New language'
##New language
#   *qr*) echo "New language" ;;
  esac)
 # unknown language - error
 if [ -z "LNGNAME" ]; then
  echo "Invalid argument $(tput setaf 1)'$LNG'$(tput sgr 0).">&2
  exit 1
 fi
 INFILE=lang_en_$LNG.txt
 OUTFILE=po/Firmware_$LNGISO.po
fi

# remove output file if exists
if [ -e $OUTFILE ]; then rm -f -v $OUTFILE; fi

#total strings
CNTTXT=$(grep '^#' -c $INFILE)
#not translated strings
CNTNT=$(grep '^\"\\x00\"' -c $INFILE)
echo " $(tput setaf 2)$CNTTXT$(tput sgr 0) texts, $(tput setaf 3)$CNTNT$(tput sgr 0) not translated" >&2

# list .cpp, .c and .h files from source folder
SRCFILES=$(ls "$SRCDIR/Firmware"/*.cpp "$SRCDIR/Firmware"/*.c "$SRCDIR/Firmware"/*.h)

echo " selected language=$(tput setaf 2)$LNGNAME$(tput sgr 0)" >&2

# write po/pot header
(
 echo "# Translation of Prusa-Firmware into $LNGNAME."
 echo "#"
 echo 'msgid ""'
 echo 'msgstr ""'
 echo '"MIME-Version: 1.0\n"'
 echo '"Content-Type: text/plain; charset=UTF-8\n"'
 echo '"Content-Transfer-Encoding: 8bit\n"'
 echo '"Language: '$LNGISO'\n"'
 echo '"Project-Id-Version: Prusa-Firmware\n"'
 echo '"POT-Creation-Date: '$DATE'\n"'
 echo '"PO-Revision-Date: '$DATE'\n"'
 echo '"Language-Team: \n"'
 echo '"X-Generator: Poedit 2.0.7\n"'
 echo '"X-Poedit-SourceCharset: UTF-8\n"'
 echo '"Last-Translator: \n"'
 echo '"Plural-Forms: nplurals=3; plural=(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;\n"'
 echo
) >$OUTFILE

#loop over all messages
s0=''
s1=''
s2=''
num=1
(cat $INFILE | sed "s/\\\\/\\\\\\\\/g" | while read -r s; do
 #start debug
 #if [ "${s:0:1}" = "\"" ]; then
 # echo  >&2
 # echo "s = $s ." >&2
 # echo "s0 = $s0 ." >&2
 # echo "s1 = $s1 ." >&2
 #fi
 #end debug
 if [ "${s:0:1}" = "\"" ]; then
  if [[ "${s0:0:1}" = "\"" || "$LNG" = "en" ]]; then
   echo -ne "  processing $num of $CNTTXT\033[0K\r" >&2
   # write po/pot item
   (
   if [ "$LNG" = "en" ]; then s1=$s0; s0=$s; fi
    search=$(/bin/echo -e "$s0")
    found=$(grep -m1 -n -F "$search" $SRCFILES | head -n1 | cut -f1-2 -d':' | sed "s/^.*\///")
    echo "$s1" | sed 's/ c=0//;s/ r=0//;s/^#/# /'
    #echo "$s1" | sed 's/ c=0//;s/ r=0//;s/^#/# /' >&2
    echo "#: $found"
    #echo "#: $found" >&2
    /bin/echo -e "msgid $s0"
    if [[ "$s" = "\"\\\\x00\"" || "$LNG" = "en" ]]; then
     echo 'msgstr ""'
    else
     /bin/echo -e "msgstr $s"
    fi
    echo
  )
  num=$((num+1))
  fi
 fi
 s1=$s0
 s0=$s
done >>$OUTFILE) 2>&1

#replace LF with CRLF
sync
sed -i 's/$/\r/' $OUTFILE

#replace HD44780 A00 'äöüß' to UTF-8 'äöüß'
if [[ "$LNG" = "de" || "$LNG" = "sv" ]]; then
  #replace 'A00 ROM '"ä' with '"Ä' 
  sed -i 's/"\\xe1/"\xc3\x84/g' $OUTFILE
  #replace 'A00 ROM '"ü' with '"Ü'
  sed -i 's/"\\xf5/"\xc3\x9c/g' $OUTFILE
  #replace 'A00 ROM '"ö' with '"Ö'
  sed -i 's/"\\xef/"\xc3\x96/g' $OUTFILE
  #replace 'A00 ROM '. ä' with '. Ä' 
  sed -i 's/\. \\xe1/. \xc3\x84/g' $OUTFILE
  #replace 'A00 ROM '. ü' with '. Ü'
  sed -i 's/\. \\xf5/. \xc3\x9c/g' $OUTFILE
  #replace 'A00 ROM '. ö' with '. Ö'
  sed -i 's/\. \\xef/. \xc3\x96/g' $OUTFILE
  #replace 'A00 ROM '! ä' with '! Ä' 
  sed -i 's/! \\xe1/! \xc3\x84/g' $OUTFILE
  #replace 'A00 ROM '! ü' with '! Ü'
  sed -i 's/! \\xf5/! \xc3\x9c/g' $OUTFILE
  #replace 'A00 ROM '! ö' with '! Ö'
  sed -i 's/! \\xef/! \xc3\x96/g' $OUTFILE
  #replace 'A00 ROM '? ä' with '? Ä' 
  sed -i 's/? \\xe1/? \xc3\x84/g' $OUTFILE
  #replace 'A00 ROM '? ü' with '? Ü'
  sed -i 's/? \\xf5/? \xc3\x9c/g' $OUTFILE
  #replace 'A00 ROM '? ö' with '? Ö'
  sed -i 's/? \\xef/? \xc3\x96/g' $OUTFILE
  #replace 'A00 ROM 'ä' with 'ä' 
  sed -i 's/\\xe1/\xc3\xa4/g' $OUTFILE
  #replace 'A00 ROM 'ü' with 'ü'
  sed -i 's/\\xf5/\xc3\xbc/g' $OUTFILE
  #replace 'A00 ROM 'ö' with 'ö'
  sed -i 's/\\xef/\xc3\xb6/g' $OUTFILE
  #replace 'A00 ROM 'ß'' with 'ß'
  sed -i 's/\\xe2/\xc3\x9f/g' $OUTFILE
fi

if [ "$LNG" = "no" ]; then
  #replace often used words
  #replace ' pa ' with ' på ' 
  sed -i 's/\ pa / p\xc3\xa5 /g' $OUTFILE
  #replace ' na ' with ' nå ' 
  sed -i 's/\ na / n\xc3\xa5 /g' $OUTFILE
  #replace '"Na ' with '"Nå ' 
  sed -i 's/\"Na /"N\xc3\xa5 /g' $OUTFILE
  #replace ' stal' with ' stål' 
  sed -i 's/\ stal/ st\xc3\xa5l/g' $OUTFILE
  #replace HD44780 A00 'äö' to UTF-8 'æø'
  #replace 'A00 ROM ä' with 'æ' 
  sed -i 's/\\xe1/\xc3\xa6/g' $OUTFILE
  #replace 'A00 ROM ö' with 'ø'
  sed -i 's/\\xef/\xc3\xb8/g' $OUTFILE
fi

if [ "$LNG" = "sv" ]; then
  #replace often used words
  #replace ' pa ' with ' på ' 
  sed -i 's/\ pa / p\xc3\xa5 /g' $OUTFILE
fi


#replace HD44780 A00 'μ' to UTF-8 'μ'
#replace 'A00 ROMμ' with ' μ'
sed -i 's/\\xe4/\xce\xbc/g' $OUTFILE


echo >&2
echo "$(tput setaf 2)lang-export.sh finished$(tput sgr 0)">&2
exit 0
