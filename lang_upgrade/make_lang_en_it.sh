#!/bin/sh
# make_lang_en_it.sh - create en_it dictionary from msgs
echo "make_lang_en_it.sh"
echo "please wait..."

cat msgs_en_used_once.txt msgs_en_used_more.txt |\
 sed "s/\bCUSTOM_MENDEL_NAME\b/\"Prusa i3 MK3\"/g" |\
 sed "s/\" \"//g" |\
 sed 's/\\"/\\x22/g' |\
 sed 's/\\xF8/\\xf8/g' > msgs_en_.txt

cat msgs_it.txt |\
 sed "s/\bCUSTOM_MENDEL_NAME\b/\"Prusa i3 MK3\"/g" |\
 sed "s/\" \"//g" |\
 sed 's/\\"/\\x22/g' |\
 sed 's/\\xF8/\\xf8/g' > msgs_it_.txt
 
cat lang_en.txt | sed "/^$/d;/^#/d" | sed "s/\\\/\\\\\\\/g" |\
 while read text; do
 msg=''
 msg=$(grep -a -m 1 -h -w -F "$text" msgs_en_.txt)
 if [ -z "$msg" ]; then
  echo "#???"
  echo "$text"
  echo '"\x00"'
  echo
 else
  name=$(echo $msg | cut -f1 -d' ')
  cols=$(echo $msg | cut -f2 -d' ')
  rows=$(echo $msg | cut -f3 -d' ')
  echo "#$name $cols $rows"
  echo "$text"
  msg_it=$(grep -a -m 1 -h "^$name " msgs_it_.txt)
  if [ -z "$msg_it" ]; then
   echo '"\x00"'
   echo
  else
   text_it=$(echo $msg_it | cut -f2- -d' ')
   echo "$text_it"
   echo
  fi
 fi
done > lang_en_it.txt

rm msgs_en_.txt
rm msgs_it_.txt

echo "finished... press any key"
read
exit

#not found in msgs_it:
# MSG_EXTRUDER_CORRECTION_OFF "  [off"
# MSG_MEASURED_OFFSET "[0;0] point offset"
# MSG_EXTRUDER_CORRECTION "E-correct"
# MSG_PRUSA3D_FORUM "forum.prusa3d.com"
# MSG_PRUSA3D_HOWTO "howto.prusa3d.com"
# MSG_PRINTER_DISCONNECTED "Printer disconnected"
# MSG_PRUSA3D "prusa3d.com"
# MSG_TEMP_CAL_WARNING "Stable ambient temperature 21-26C is needed a rigid stand is required."
