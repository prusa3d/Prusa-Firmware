#!/bin/sh
#
# lang-community.sh - Community language support script
# Check community languages are defined in `config.h`
#

# Root path
if [ -z "$ROOT_PATH" ]; then
    export ROOT_PATH=".."
fi

# Check community language NL = Dutch
COMMUNITY_LANG_NL=$(grep --max-count=1 "^#define COMMUNITY_LANG_NL" $ROOT_PATH/Firmware/config.h| cut -d '_' -f3 |cut -d ' ' -f1)
export NL=$COMMUNITY_LANG_NL

# Use the lines below as a template and replace 'QR' and 'new language'
# Check comminity language QR = new language
#COMMUNITY_LANG_QR=$(grep --max-count=1 "^#define COMMUNITY_LANG_QR" $ROOT_PATH/Firmware/config.h| cut -d '_' -f3 |cut -d ' ' -f1)
#export QR=$COMMUNITY_LANG_QR

#startup message
echo "lang-community.sh started" >&2
echo -n " Source code path: " >&2
if [ -e $ROOT_PATH ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=1; fi

echo " Found: " >&2
if [ "$COMMUNITY_LANG_NL" = "NL" ]; then
    echo "  $COMMUNITY_LANG_NL" >&2
    echo
    ./lang-build.sh nl
fi

# Use the 5 lines below as a template and replace 'QR' and 'qr'
#if [ "$COMMUNITY_LANG_QR" = "QR" ]; then
#    echo "  $COMMUNITY_LANG_QR" >&2
#    echo
#    ./lang-build.sh qr
#fi


#exiting function
finish()
{
 if [ $1 -eq 0 ]; then
  echo "lang-community.sh finished with success" >&2
 else
  echo "lang-community.sh finished with errors!" >&2
 fi
 echo 
 exit $1
}

finish 0
