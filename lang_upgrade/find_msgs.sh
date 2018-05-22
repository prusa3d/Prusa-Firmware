#!/bin/sh
#
echo 'find_msgs.sh'

#list all source files from source folder except *language* files
files=$(ls ../Firmware/*.c* | grep -v 'language'; ls ../Firmware/*.h | grep -v 'language'; )

echo ' processing msgs_en.txt and msgs_common.txt...'
#msgs=$(cat msgs_en.txt | cut -f 1 -d' ')
msgs=$(cat msgs_en.txt msgs_common.txt | cut -f 1 -d' ')
#msgs=$(cat msgs_common.txt | cut -f 1 -d' ')
echo "$msgs" | while read msg; do
 echo -n "$msg "
 found=$(grep -c -E "\b$msg\b" $files | sed "/:0$/d;s/.*:/+/g")
 echo $(("0"$found))
done | tee msgs_usage.txt_0
cat msgs_usage.txt_0 | sort -k2 -n >msgs_usage.txt
rm msgs_usage.txt_0

#list messages that are not used
msgs=$(cat msgs_usage.txt | grep " 0$" | cut -f1 -d' ')
#make regular expression from the list - replace spaces with '\b|\b' (match whole words)
msgs='\b'$(echo $msgs | sed "s/ /\\\b\\\|\\\b/g")'\b'
#grep unused messages
cat msgs_en.txt | grep "$msgs" > msgs_en_unused.txt
cat msgs_common.txt | grep "$msgs" > msgs_common_unused.txt

#list messages used once
msgs=$(cat msgs_usage.txt | grep " 1$" | cut -f1 -d' ')
#make regular expression from the list - replace spaces with '\b|\b' (match whole words)
msgs='\b'$(echo $msgs | sed "s/ /\\\b\\\|\\\b/g")'\b'
#grep unused messages
cat msgs_en.txt | grep "$msgs" > msgs_en_used_once.txt
cat msgs_common.txt | grep "$msgs" > msgs_common_used_once.txt

#list messages used once more (exclude unused and used once)
msgs=$(cat msgs_usage.txt | grep -v " 0$" | grep -v " 1$" | cut -f1 -d' ')
#make regular expression from the list - replace spaces with '\b|\b' (match whole words)
msgs='\b'$(echo $msgs | sed "s/ /\\\b\\\|\\\b/g")'\b'
#grep unused messages
cat msgs_en.txt | grep "$msgs" > msgs_en_used_more.txt
cat msgs_common.txt | grep "$msgs" > msgs_common_used_more.txt

echo "step2 finished... press key"

read
exit
