#!/bin/bash
#
# Version 1.0.1 Build 13
#
# clean.sh - multi-language support script
#  Remove all language output files from lang folder.
#
#############################################################################
# Change log:
#  1 Nov. 2018, XPila,      Initial
# 18 Oct. 2018, XPila,      New lang, arduino 1.8.5 - fw-clean.sh and lang-clean.sh fix
# 25 Oct. 2018, XPila,      New lang - fixed french langcode and comparsion in lang-clean script
# 10 Dec. 2018, jhoblitt,   make all shell scripts executable
# 26 Jul. 2019, leptun,     Fix shifted languages. Use \n and \x0a
# 14 Sep. 2019, 3d-gussner, Prepare adding new language
# 01 Mar. 2021, 3d-gussner, Move `Dutch` language parts
# 22 Mar. 2021, 3d-gussner, Move Dutch removing part to correct loaction
# 21 Dec. 2021, 3d-gussner, Use one config file for all languages
# 03 Jan. 2022, 3d-gussner, Cleanup outdated code
# 11 Jan. 2022, 3d-gussner, Also remove temporally files which have been
#                           generated for message and size count comparison
#                           Added version and Change log
#                           colored output
#                           Add Community language support
#                           Use `git rev-list --count HEAD lang-clean.sh`
#                           to get Build Nr
# 25 Jan. 2022, 3d-gussner, clean up lang-import.sh temproray files
# 14 Feb. 2022, 3d-gussner, Fix single language run without config.sh OK
# 16 Mar. 2022, 3d-gussner, Cleanup `output-layout/sorted-??.txt` 
#############################################################################
result=0

rm_if_exists()
{
 if [ -e $1 ]; then
  echo -n "$(tput sgr0) removing $(tput setaf 3)'$1'$(tput sgr0)..." >&2
  if rm $1; then
   echo "$(tput setaf 2)OK$(tput sgr0)" >&2
  else
   echo "$(tput setaf 1)NG!$(tput sgr0)" >&2
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
  rm_if_exists po/new/$1_new.po
  rm_if_exists po/new/$1.mo
  rm_if_exists po/new/$1_filtered.po
  rm_if_exists po/new/lang_en_$1.txt
  rm_if_exists po/new/output-layout-$1.txt
  rm_if_exists po/new/output-sorted-$1.txt
 fi
 rm_if_exists lang_$1_check.dif
 rm_if_exists lang_$1.bin
 rm_if_exists lang_$1.dat
 rm_if_exists lang_$1_1.tmp
 rm_if_exists lang_$1_2.tmp
 rm_if_exists po/new/nonascii.txt

}

echo "$(tput setaf 2)lang-clean.sh started$(tput sgr0)" >&2
#Clean English
 clean_lang en

#Clean languages
echo "lang-clean languages:$(tput setaf 2)$LANGUAGES$(tput sgr0)" >&2
if [ -e $1 ]; then
# Config:
  if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
  if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr0)" >&2; exit 1; fi

  if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
    LANGUAGES+=" $COMMUNITY_LANGUAGES"
  fi

 for lang in $LANGUAGES; do
  clean_lang $lang
 done
else
  clean_lang $1
fi

if [ $result -eq 0 ]; then
 echo "$(tput setaf 2) lang-clean.sh with success$(tput sgr0)" >&2
else
 echo "$(tput setaf 1) lang-clean.sh with errors!$(tput sgr0)" >&2
fi

case "$-" in
 *i*) echo "press enter key" >&2; read ;;
esac

exit $result
