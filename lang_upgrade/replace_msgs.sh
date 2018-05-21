#!/bin/sh
# replace_msgs.sh - step3 - replace source and generate messages.h and messages.c files

#mkdir ./source
#files=$(ls ../Firmware/*.c* | grep -v 'language'; ls ../Firmware/*.h | grep -v 'language'; )
#cp $files ./source/
#exit

files=$(ls ./source/*.c* | grep -v 'language'; ls ./source/*.h | grep -v 'language'; )
#echo "$files"

#list messages used only once
 #msgs=$(cat msgs_usage.txt | grep " 1$" | cut -f1 -d' ')
#make regular expression from the list - replace spaces with '\b|\b'
 #msgs=$(echo $msgs | sed "s/ /\\\b\\\|\\\b/g")
#filter this messages from msgs_en.txt to msgs_en_1.txt
 #cat msgs_en.txt | grep "$msgs" > msgs_en_1.txt



cat msgs_en_X.txt | sed "s/\\\\/\\\\\\\\/g;s/\//\\\\\\\\\//g" | while read name cols rows text; do
 comment="$name $cols $rows"
 if sed -i -E "s/(.*)(\b$name\b)(.*)$/\1_i\($text\)\3\/\/\/\/$comment/g" $files; then
  echo "$name OK"
 else
  echo "$name NG!"
 fi
done | tee replace2.out

read

#grep "$msgs" $files > msg_.txt

exit


cat msgs_en.txt | grep

echo "$msgs" | while read name; do
# sed -i "s/\b$name\b/_i\(\"$text\"\)/g" ./source/ultralcd.cpp
done
read
exit

#
name=MSG_INFO_NOZZLE_FAN
text="Nozzle FAN:"
#grep  "\b$name\b" $files
sed -i "s/\b$name\b/_i\(\"$text\"\)/g" ./source/ultralcd.cpp
read
exit


#unused messages will be listed in msgs_unused.txt.
cat msgs_usage.txt | grep " 0$" | cut -f1 -d' ' >msgs_unused.txt

#remove generated source files
if [ -e messages.h ]; then rm messages.h; fi
if [ -e messages.c ]; then rm messages.c; fi

#messages used twice or more will be listed in messages.h and messages.cpp.
msgs=$(cat msgs_usage.txt | grep -v " 0$" | cut -f1 -d' ')
echo '//messages.h' > messages.h
echo '#include <avr/pgmspace.h>' >> messages.h
echo '//messages.c' >> messages.c
echo '#include "messages.h"' >> messages.c
echo -n '#define bool int
' >> messages.c
echo -n '#define true 1
' >> messages.c
echo -n '#define false 0
' >> messages.c
echo '#include "Configuration_prusa.h"' >> messages.c

sync -f messages.c


echo '//internationalized messages' | tee -a messages.h >> messages.c

cat msgs_en.txt | sed 's/\\/\\\\/g' | while read msg cols rows text; do
 if echo "$msgs" | grep "^$msg" >/dev/null; then
  echo "extern const char $msg[] PROGMEM;" >> messages.h
  echo "const char $msg[] PROGMEM = "$text";" >> messages.c
  echo "$msg"
 fi
done

echo '//not internationalized messages' | tee -a messages.h >> messages.c

cat msgs_common.txt | sed 's/\\/\\\\/g' | while read msg text; do
 if echo "$msgs" | grep "^$msg" >/dev/null; then
  echo "extern const char $msg[] PROGMEM;" >> messages.h
  echo "const char $msg[] PROGMEM = "$text";" >> messages.c
  echo "$msg"
 fi
done

read
exit












#messages used twice or more will be listed in messages.h and messages.cpp.
#msgs=$(cat msgs_usage.txt | grep -v " 0$" | grep -v " 1$" | cut -f1 -d' ')
msgs=$(cat msgs_usage.txt | grep -v " 0$" | cut -f1 -d' ')
echo '//messages.h' > messages.h
echo '//messages.c' > messages.c
echo '#include <avr/pgmspace.h>' >> messages.h
echo '#include <avr/pgmspace.h>' >> messages.c
#internationalized messages
echo '//internationalized messages' >> messages.h
echo '//internationalized messages' >> messages.c
msgs2=$(cat msgs_en.txt | while read msg cols rows text; do
 if echo "$msgs" | grep "^$msg" >/dev/null; then
  echo "extern const char $msg[] PROGMEM;" >> messages.h
  echo "const char $msg[] PROGMEM = $text;" >> messages.c
  echo $msg
 fi
done)
#not internationalized messages
echo '//not internationalized messages' >> messages.h
echo '//not internationalized messages' >> messages.c
cat msgs_common.txt | while read msg text; do
 if echo "$msgs" | grep "^$msg" >/dev/null; then
  if echo "$msgs2" | grep "^$msg" >/dev/null; then
  else
   echo "extern const char $msg[] PROGMEM;" >> messages.h
   echo "const char $msg[] PROGMEM = $text;" >> messages.c
   echo $msg
  fi
 fi
done

echo "finished..."
read
exit

cat msgs_en.txt | while read msg cols rows text; do
 if echo "$msgs" | grep "^$msg" >/dev/null; then
  echo "$msg  $text"
 fi
done

#; then

#  echo "" $msg
# fi
#done

#echo "$msgs" | while read msg; do
# echo "" $msg
#done

#msg=MSG_WIZARD_FILAMENT_LOADED
#txt="PSTR(\"Is filament loaded?\")"
#fn=../ultralcd.cpp
#sed -i "s/$msg/$txt/g" $fn

read
