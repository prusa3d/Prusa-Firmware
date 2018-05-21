#!/bin/sh
#
echo 'find_msgs.sh'

files=$(ls ../Firmware/*.c* | grep -v 'language'; ls ../Firmware/*.h | grep -v 'language'; )

echo -n ' processing msgs_en.txt ...'
#msgs=$(cat msgs_en.txt | cut -f 1 -d' ')
msgs=$(cat msgs_en.txt msgs_common.txt | cut -f 1 -d' ')
#msgs=$(cat msgs_common.txt | cut -f 1 -d' ')
echo "$msgs" | while read msg; do
 echo -n "$msg "
 found=$(grep -c "$msg" $files | sed "/:0$/d;s/.*:/+/g")
 echo $(("0"$found))
done  | tee msgs_usage.txt_0
cat msgs_usage.txt_0 | sort -k2 -n >msgs_usage.txt
rm msgs_usage.txt_0
echo "finished.."
read
exit

#replace #define(length=xx,lines=xx)
cat msgs_en.txt_0 | sed -E "s/^#define\(length=([0-9]*),[ \t]*lines=([0-9]*)\)[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\3 c=\1 r=\2 \4/g" >msgs_en.txt_1
#replace #define(length=xx)
cat msgs_en.txt_1 | sed -E "s/^#define\(length=([0-9]*)\)[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\2 c=\1 r=0 \3/g" >msgs_en.txt_2
#replace #define
cat msgs_en.txt_2 | sed -E "s/^#define[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\1 c=0 r=0 \2/g" >msgs_en.txt
#remove tmp files
rm msgs_en.txt_*
echo "ok"

echo -n ' processing language_cz.h ...'
#list all defines from language_cz.h
cat ../Firmware/language_cz.h | grep "^#define" >msgs_cz.txt_0
cat msgs_cz.txt_0 | sed -E "s/^#define[ \t]*([^ \t]*)[ \t]*([^ \t]*[ \t]*\"[^\"]*\"*)/\1 \2/g" >msgs_cz.txt
#remove tmp files
rm msgs_cz.txt_*
echo "ok"

echo "finished.."
read
exit






#cat msgs_en.txt_3 | grep "^XXX" >msgs_en.txt_4

#cat msgs_en.txt_0 | sed -E "s/^#define\(length=([0-9]{2}), lines=([0-9]{2})\)[ ]*([^ ]*)/\3 \2 \1/g" >msgs_en.txt_1
#cat msgs_en.txt_0 | sed -E "s/^#define\(length=([0-9]{2})\)[ ]*([^ ]*)/\2 \1/g" >msgs_en.txt_1
#cat msgs_en.txt_1 | sed -E "s/^#define[ ]*([^ ]*)/\1/g" >msgs_en.txt_2
#[0-9]{+2}
#cat ../Firmware/language_en.h | sed "s/^#define(length=[0-9]*)//g" >msgs_en.txt_0
#msgs=$(ls ../Firmware/*.c* | grep -v 'language'; ls ../Firmware/*.h | grep -v 'language'; )
#echo "$files" | while read fn; do
# if grep "MSG_SD_INSERTED" $fn >/dev/null; then
#  echo $fn
# fi
#done
echo "finished.."
read
