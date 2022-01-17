#!/bin/bash
#
# lang-export.sh - multi-language support script
#  for generating lang_xx.po
#
# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi

if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi
echo "lang-export languages:$LANGUAGES" >&2

# relative path to source folder
SRCDIR="../Firmware"

# selected language is 1st argument (cz, de, ...)
LNG=$1

# if no arguments, 'all' is selected (all po and also pot will be generated)
if [ -z "$LNG" ]; then LNG=all; fi

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
#Danish
   *da*) echo "Danish" ;;
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
  echo "Invalid argument '$LNG'."
  exit 1
 fi
 INFILE=lang_en_$LNG.txt
 OUTFILE=po/Firmware_$LNGISO.po
fi

# remove output file if exists
if [ -e $OUTFILE ]; then rm -f -v $OUTFILE; fi

echo "lang-export.sh started"

#total strings
CNTTXT=$(grep '^#' -c $INFILE)
#not translated strings
CNTNT=$(grep '^\"\\x00\"' -c $INFILE)
echo " $CNTTXT texts, $CNTNT not translated"

# list .cpp, .c and .h files from source folder
SRCFILES=$(ls "$SRCDIR"/*.cpp "$SRCDIR"/*.c "$SRCDIR"/*.h)

echo " selected language=$LNGNAME"

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
   echo "  processing $num of $CNTTXT" >&2
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

echo "lang-export.sh finished"
exit 0
