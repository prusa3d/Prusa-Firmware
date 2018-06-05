#!/bin/sh
# fix_source_1.sh - replace in code all usage of localized message constant from messages.h as _T(MSG_xxx)

if [ -e ./source ]; then
 echo 'folder ./source already exists.'
 read
 exit
fi

#create target folder
echo 'creating target folder...'
mkdir ./source
#list all source files except *language* and *messages*
files=$(ls ../Firmware/*.c* | grep -v 'language' | grep -v 'messages'; ls ../Firmware/*.h | grep -v 'language' | grep -v 'messages' )
echo 'copying files...'
cp $files ./source/
#list all source files in target folder
files=$(ls ./source/*.c*; ls ./source/*.h )

#replace source - internatinalized messages used once with _i("TEXT")
echo 'processing msgs_en_used_once.txt'
cat msgs_en_used_more.txt | sed "s/\\\\/\\\\\\\\/g;s/\//\\\\\\\\\//g" | while read name cols rows text; do
 if sed -i -E "s/\b$name\b/_T\($name\)/g" $files; then
  echo "$name OK"
 else
  echo "$name NG!"
 fi
done | tee fix_1.out

echo "fix1 finished... press key"

read
exit
