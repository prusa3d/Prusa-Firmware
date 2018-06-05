#!/bin/sh
#
echo 'make_msgs.sh'
if [ -e make_msgs.out ]; then rm make_msgs.out; fi
CUSTOM_MENDEL_NAME='Prusa i3 MK3'

calc_charcount()
{
 #extract texts for charcount calculation
 cat $1 | cut -f$2- -d' ' | sed -E "s/\" \"//g" >_txt.txt_0
 #replace printer name
 cat _txt.txt_0 | sed -E "s/CUSTOM_MENDEL_NAME/\"$CUSTOM_MENDEL_NAME\"/g" >_txt.txt_1
 #replace oct chars with space
 cat _txt.txt_1 | sed -E "s/\\\\[0-7]{3}/ /g" >_txt.txt_2
 #replace hex chars with space
 cat _txt.txt_2 | sed -E "s/\\\\x[0-9A-Fa-f]{2}/ /g" >_txt.txt_3
 #replace \" with '
 cat _txt.txt_3 | sed -E "s/\\\\\"/'/g" >_txt.txt_4
 #replace "_white_space_" with empty sequence - strigs
 cat _txt.txt_4 | sed -E "s/\"[ \t]*\"//g" >_txt.txt_5
 #replace " and white space at end of line
 cat _txt.txt_5 | sed -E "s/\"[ \t]*$/\"/g" >_txt.txt_6
 #replace all " with empty sequence
 cat _txt.txt_6 | sed -E "s/\"//g" >_txt.txt_7
 #calculate charcount
 stat -c'%s' _txt.txt_7
 rm _txt.txt_*
}

process_language_common()
{
 echo -n ' processing language_common.h ...' | tee -a make_msgs.out
 #list all defines without '+' prefix from language_common.h
 #cat ../Firmware/language_common.h | grep -E "^[+]*define" | sed "s/lenght/length/g" >msgs_common.txt_0
 cat ../Firmware/language_common.h | grep -E "^define" | sed "s/lenght/length/g" >msgs_common.txt_0
  #replace define and +define
 cat msgs_common.txt_0 | sed -E "s/^[+]*define[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\1 c=0 r=0 \2/g" | sort >msgs_common.txt
 #calculate msgcount
 msgcount=$(grep -c '' msgs_common.txt)
 #calculate charcount
 charcount=$(calc_charcount msgs_common.txt 4)
 #remove tmp files
 rm msgs_common.txt_*
 echo "ok ($msgcount messages, $charcount characters)" | tee -a make_msgs.out
}


process_language_en()
{
 echo -n ' processing language_en.h ...' | tee -a make_msgs.out
 #list all defines from language_en.h
 cat ../Firmware/language_en.h | grep "^#define" | sed "s/lenght/length/g" >msgs_en.txt_0
 #replace #define(length=xx,lines=xx)
 cat msgs_en.txt_0 | sed -E "s/^#define\(length=([0-9]*),[ \t]*lines=([0-9]*)\)[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\3 c=\1 r=\2 \4/g" >msgs_en.txt_1
 #replace #define(length=xx)
 cat msgs_en.txt_1 | sed -E "s/^#define\(length=([0-9]*)\)[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\2 c=\1 r=0 \3/g" >msgs_en.txt_2
 #replace #define
 cat msgs_en.txt_2 | sed -E "s/^#define[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\1 c=0 r=0 \2/g" | sort >msgs_en.txt
 #calculate msgcount
 msgcount=$(grep -c '' msgs_en.txt)
 #calculate charcount
 charcount=$(calc_charcount msgs_en.txt 4)
 #remove tmp files
 rm msgs_en.txt_*
 echo "ok ($msgcount messages, $charcount characters)" | tee -a make_msgs.out
}

process_language_xx()
{
 echo -n " processing language_$1.h ..." | tee -a make_msgs.out
 #list all defines from language_cz.h
 cat "../lang_backup/language_$1.h" | sed "s/[ \t]*\+//;s/^\+ #/#/;s/^\+#/#/" | grep -E "^#define" >"msgs_$1.txt_0"
 cat "msgs_$1.txt_0" | sed "s/(length = [0-9]*)//" | sed "s/(length = [0-9]*, lines = [0-9]*)//" > "msgs_$1.txt_1"
 cat "msgs_$1.txt_1" | sed -E "s/^#define[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\1 \2/g" | sort >"msgs_$1.txt"
 #calculate msgcount
 msgcount=$(grep -c '' "msgs_$1.txt")
 #calculate charcount
 charcount=$(calc_charcount "msgs_$1.txt" 2)
 #remove tmp files
 rm "msgs_$1.txt_0"
 rm "msgs_$1.txt_1"
 echo "ok ($msgcount messages, $charcount characters)" | tee -a make_msgs.out
}

#process_language_common
#process_language_en
#process_language_xx cz
process_language_xx de
#process_language_xx it
#process_language_xx pl
#process_language_xx es


echo "make_msgs.sh finished... press key"
read
