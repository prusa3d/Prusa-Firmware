#!/bin/sh
#
# Version 1.0.2
#
# clean.sh - multi-language support script
#  Remove all language output files from lang folder.
#
#############################################################################
# Change log:
# 14 May 2020, 3d-gussner, Also remove temporally files which have been generated
#                          for message and size count comparison
# 14 May 2020, 3d-gussner, Added version and Change log
# 9 June 2020, 3d-gussner, colored output
#############################################################################
#############################################################################

result=0

rm_if_exists()
{
 if [ -e $1 ]; then
  echo -n "$(tput sgr0) removing '$1'...$(tput sgr0)" >&2
  if rm $1; then
   echo "$(tput setaf 2)OK" >&2
  else
   echo "$(tput setaf 1)NG!" >&2
   result=1
  fi
 fi
}

clean_lang()
{
 if [ "$1" = "en" ]; then
  rm_if_exists lang_$1.tmp
  rm_if_exists lang_$1.cnt
  rm_if_exists lang_$1.max
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

echo "$(tput setaf 2)lang-clean.sh started$(tput sgr0)" >&2

clean_lang en
clean_lang cz
clean_lang de
clean_lang es
clean_lang fr
clean_lang it
clean_lang pl
#Community language support
#Dutch
clean_lang nl

#Use the 2 lines below as a template and replace 'qr'
##New language
#clean_lang_qr

echo -n "lang-clean.sh finished" >&2
if [ $result -eq 0 ]; then
 echo " with success$(tput sgr0)" >&2
else
 echo " with errors!$(tput sgr0)" >&2
fi

case "$-" in
 *i*) echo "press enter key" >&2; read ;;
esac

exit $result
