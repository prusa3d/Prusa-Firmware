#!/bin/bash
#
# Version 1.0.1 Build 11
#
# fw-clean.sh - multi-language support script
#  Remove all firmware output files from lang folder.
#
#############################################################################
# Change log:
# 21 June 2018, XPila,      Initial
# 11 Sep. 2018, XPila,      Lang update, french translation
#                           resized reserved space
# 18 Oct. 2018, XPila,      New lang, arduino 1.8.5 - fw-clean.sh and lang-clean.sh fix
# 10 Dec. 2018, jhoblitt,   make all shell scripts executable
# 26 Jul. 2019, leptun,     Fix shifted languages. Use \n and \x0a
# 14 Sep. 2019, 3d-gussner, Prepare adding new language
# 01 Mar. 2021, 3d-gussner, Move `Dutch` language parts
# 22 Mar. 2021, 3d-gussner, Move Dutch removing part to correct loaction
# 17 Dec. 2021, 3d-gussner, Use one config file for all languages
# 11 Jan. 2022, 3d-gussner, Added version and Change log
#                           colored output
#                           Use `git rev-list --count HEAD fw-clean.sh`
#                           to get Build Nr
# 25 Jan. 2022, 3d-gussner, Update documentation
#############################################################################
# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr0)" >&2; exit 1; fi

if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi
echo "$(tput setaf 2)fw-clean.sh started$(tput sgr0)" >&2
echo "fw-clean languages:$(tput setaf 2)$LANGUAGES$(tput sgr0)" >&2

result=0

rm_if_exists()
{
 if [ -e $1 ]; then
  echo -n "$(tput sgr0) removing $(tput sgr0) '$1'..." >&2
  if rm $1; then
   echo "$(tput setaf 2)OK$(tput sgr0)" >&2
  else
   echo "$(tput setaf 1)NG!$(tput sgr0)" >&2
   result=1
  fi
 fi
}


rm_if_exists text.sym
rm_if_exists progmem1.sym
rm_if_exists progmem1.lss
rm_if_exists progmem1.hex
rm_if_exists progmem1.chr
rm_if_exists progmem1.var
rm_if_exists progmem1.txt
rm_if_exists textaddr.txt
rm_if_exists firmware.bin
rm_if_exists firmware.hex
rm_if_exists progmem.out
rm_if_exists textaddr.out
rm_if_exists update_lang.out
rm_if_exists lang.bin
rm_if_exists lang.hex


for lang in $LANGUAGES; do
 rm_if_exists firmware_$lang.hex
 rm_if_exists update_lang_$lang.out
done

if [ $result -eq 0 ]; then
 echo "$(tput setaf 2)fw-clean.sh finished with success$(tput sgr0)" >&2
else
 echo "$(tput setaf 1)fw-clean.sh finished with errors!$(tput sgr0)" >&2
fi

case "$-" in
 *i*) echo "press enter key"; read ;;
esac

exit $result
