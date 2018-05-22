#!/bin/sh
# make_lang_en.sh - create english dictionary from msgs
echo "make_lang_en.sh"
cat msgs_en_used_once.txt msgs_en_used_more.txt | cut -f4- -d' ' |\
 sed "s/\bCUSTOM_MENDEL_NAME\b/\"Prusa i3 MK3\"/g" |\
 sed "s/\" \"//g;s/\"//;s/\"$//" |\
 sed 's/\\"/\\x22/g' |\
 sed 's/\\xF8/\\xf8/g' |\
 sort > lang_en.txt
echo "finished... press any key"
read
exit