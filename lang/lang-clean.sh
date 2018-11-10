#!/bin/sh
#
# clean.sh - multi-language support script
#  Remove all language output files from lang folder.
#

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

echo "lang-clean.sh started" >&2

clean_lang en
clean_lang cz
clean_lang de
clean_lang es
clean_lang fr
clean_lang it
clean_lang pl

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
