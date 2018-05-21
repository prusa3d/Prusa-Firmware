#!/bin/sh
#

#rm lang_en_cz_0.txt
#rm lang_en_cz_1.txt
#(cat msgs_cz.txt  | sed -E "s/([^ ]*) (.*)/\1% \2/g"; cat msgs_en.txt | sed -E "s/([^ ]*) ([^ ]*) ([^ ]*) (.*)/\1 \4/g") | sort > lang_en_cz_0.txt
#cat lang_en_cz_0.txt | sed -E "s/([^ %]*)[%]+ (.*)/\1 \2/g" > lang_en_cz_1.txt

name0=""
cat lang_en_cz_1.txt | while read name text; do
 if [ -z "$name0" ]; then
  name0=$name
 else
  if [ $name==$name0 ]; then
   echo $name0" OK"
  else
   echo $name0" NG! "$name
  fi
  name0=''
 fi
done

read