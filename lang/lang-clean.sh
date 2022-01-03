#!/bin/bash
#
# clean.sh - multi-language support script
#  Remove all language output files from lang folder.
#

# Config:
echo "CONFIG: $CONFIG_OK"
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi

if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi

result=0

rm_if_exists()
{
 if [ -e $1 ]; then
  echo -n " removing '$1'..." >&2
  if rm $1; then
   echo "OK" >&2
  else
   echo "NG!" >&2
   result=1
  fi
 fi
}

clean_lang()
{
 if [ "$1" = "en" ]; then
  rm_if_exists lang_$1.tmp
 else
  rm_if_exists lang_$1.tmp
  rm_if_exists lang_en_$1.tmp
  rm_if_exists lang_en_$1.dif
  rm_if_exists lang_$1.ofs
  rm_if_exists lang_$1.txt
 fi
 rm_if_exists lang_$1_check.dif
 rm_if_exists lang_$1.bin
 rm_if_exists lang_$1.dat
 rm_if_exists lang_$1_1.tmp
 rm_if_exists lang_$1_2.tmp
}

#Clean English
 clean_lang en

#Clean languages
echo "lang-clean.sh started" >&2
echo "lang-clean languages:$LANGUAGES" >&2
 for lang in $LANGUAGES; do
  clean_lang $lang
 done

echo -n "lang-clean.sh finished" >&2
if [ $result -eq 0 ]; then
 echo " with success" >&2
else
 echo " with errors!" >&2
fi

case "$-" in
 *i*) echo "press enter key" >&2; read ;;
esac

exit $result
