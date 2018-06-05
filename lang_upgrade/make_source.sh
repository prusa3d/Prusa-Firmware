#!/bin/sh
# make_source.sh - step3 - replace old source files and generate new


if [ -e ./source ]; then
 echo 'folder ./source already exists.'
 read
 exit
fi

#create target folder
echo 'creating target folder...'
mkdir ./source
#list all source files except *language*
files=$(ls ../Firmware/*.c* | grep -v 'language'; ls ../Firmware/*.h | grep -v 'language' )
#copy original source files to target folder
echo 'copying files...'
cp $files ./source/
#list all source files in target folder
files=$(ls ./source/*.c*; ls ./source/*.h )

#replace source - internatinalized messages used once with _i("TEXT")
echo 'processing msgs_en_used_once.txt'
cat msgs_en_used_once.txt | sed "s/\\\\/\\\\\\\\/g;s/\//\\\\\\\\\//g" | while read name cols rows text; do
 comment="$name $cols $rows"
 if sed -i -E "s/(.*)(\b$name\b)(.*)$/\1_i\($text\)\3\/\/\/\/$comment/g" $files; then
  echo "$name OK"
 else
  echo "$name NG!"
 fi
done | tee replace_en.out

#replace source - not internatinalized messages used once with _n("TEXT")
echo 'processing msgs_common_used_once.txt'
cat msgs_common_used_once.txt | sed "s/\\\\/\\\\\\\\/g;s/\//\\\\\\\\\//g" | while read name cols rows text; do
 comment="$name $cols $rows"
 if sed -i -E "s/(.*)(\b$name\b)(.*)$/\1_n\($text\)\3\/\/\/\/$comment/g" $files; then
  echo "$name OK"
 else
  echo "$name NG!"
 fi
done | tee replace_common.out

#messages used twice or more will be listed in messages.h and messages.c.
cp ./src/messages.h ./source/
cp ./src/messages.c ./source/

echo '//internationalized messages' | tee -a ./source/messages.h >> ./source/messages.c
cat msgs_en_used_more.txt | sed 's/\\/\\\\/g' | while read msg cols rows text; do
 comment="$cols $rows"
 echo "extern const char $msg[];" >> ./source/messages.h
 echo "const char $msg[] PROGMEM_I1 = ISTR("$text"); ////$comment" >> ./source/messages.c
 echo "$msg"
done

echo '//not internationalized messages' | tee -a ./source/messages.h >> ./source/messages.c
cat msgs_common_used_more.txt | sed 's/\\/\\\\/g' | while read msg cols rows text; do
 comment="$cols $rows"
 echo "extern const char $msg[];" >> ./source/messages.h
 echo "const char $msg[] PROGMEM_N1 = "$text"; ////$comment" >> ./source/messages.c
 echo "$msg"
done


#copy new source files
cp ./src/language.h ./source/
cp ./src/language.c ./source/
cp ./src/config.h ./source/

#replace line with declaration in Marlin_main.cpp
sed -i -E "s/^(unsigned char lang_selected = 0;)/\/\/\1/" ./source/Marlin_main.cpp

echo "step3 finished... press key"
read
exit
