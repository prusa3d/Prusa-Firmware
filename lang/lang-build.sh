#!/bin/sh
#
# lang-build.sh - multi-language support script
#  generate lang_xx.bin (language binary file)
#
# Input files:
#  lang_en.txt
#  lang_en_xx.txt
#
# Output files:
#
#
# Selected language:
LNG=$1
if [ -z "$LNG" ]; then LNG='cz'; fi
#

#awk code to format ui16 variables for dd
awk_ui16='{ h=int($1/256); printf("\\x%02x\\x%02x\n", int($1-256*h), h); }'

#exiting function
finish()
{
 if [ $1 -eq 0 ]; then
  if [ -e lang_en.tmp ]; then rm lang_en.tmp; fi
  if [ -e lang_en_$LNG.tmp ]; then rm lang_en_$LNG.tmp; fi
  if [ -e lang_en_$LNG.dif ]; then rm lang_en_$LNG.dif; fi
 fi
# echo >&2
 if [ $1 -eq 0 ]; then
  echo "make_lang.sh finished with success" >&2
 else
  echo "make_lang.sh finished with errors!" >&2
 fi
 exit $1
}

#returns hexadecial data for lang code
lang_code()
# $1 - language code ('en', 'cz'...)
{
 case "$1" in
  *en*) echo '\x6e\x65' ;;
  *cz*) echo '\x73\x63' ;;
  *de*) echo '\x65\x64' ;;
  *es*) echo '\x73\x65' ;;
  *fr*) echo '\x72\x66' ;;
  *it*) echo '\x74\x69' ;;
  *pl*) echo '\x6c\x70' ;;
 esac
 echo '??'
}

#
write_header()
# $1 - lang
# $2 - size
# $3 - count
# $4 - checksum
# $5 - signature
{
 /bin/echo -n -e "\xa5\x5a\xb4\x4b" |\
  dd of=lang_$1.bin bs=1 count=4 seek=0 conv=notrunc 2>/dev/null
 /bin/echo -n -e $(echo -n "$(($2))" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=4 conv=notrunc 2>/dev/null
 /bin/echo -n -e $(echo -n "$(($3))" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=6 conv=notrunc 2>/dev/null
 /bin/echo -n -e $(echo -n "$(($4))" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=8 conv=notrunc 2>/dev/null
 /bin/echo -n -e "$(lang_code $1)" |\
  dd of=lang_$1.bin bs=1 count=2 seek=10 conv=notrunc 2>/dev/null
 sig_h=$(($5 / 65536))
 /bin/echo -n -e $(echo -n "$sig_h" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=14 conv=notrunc 2>/dev/null
 sig_l=$(($5 - $sig_h * 65536))
 /bin/echo -n -e $(echo -n "$sig_l" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=12 conv=notrunc 2>/dev/null
}

make_lang2()
# $1 - lang ('en', 'cz'...)
{
 rm -f lang_$LNG.tmp
 rm -f lang_$LNG.dat
 rm -f lang_$LNG.bin
 LNG=$1
 #create lang_xx.tmp - different processing for 'en' language
 if [ "$LNG" = "en" ]; then
  #remove comments and empty lines
  cat lang_en.txt | sed '/^$/d;/^#/d'
 else
  #remove comments and empty lines, print lines with translated text only
  cat lang_en_$LNG.txt | sed '/^$/d;/^#/d' | sed -n 'n;p'
 fi | sed 's/^\"\\x00\"$/\"\"/' > lang_$LNG.tmp
 #create lang_xx.dat (binary text data file)
 cat lang_$LNG.tmp | sed 's/^\"/printf \"/;s/"$/\\x00\"/' | sh >lang_$LNG.dat
 #calculate number of strings
 count=$(grep -c '^"' lang_$LNG.tmp)
 echo "count="$count
 #calculate text data offset
 offs=$((16 + 2 * $count))
 echo "offs="$offs
 #calculate text data size
 size=$(($offs + $(wc -c lang_$LNG.dat | cut -f1 -d' ')))
 echo "size="$size
 #write header with empty signature and checksum
 write_header $LNG $size $count 0x0000 0x00000000
 #write offset table
 cat lang_$LNG.tmp | sed 's/^\"//;s/\"$//' |\
  sed 's/\\x[0-9a-f][0-9a-f]/\./g;s/\\[0-7][0-7][0-7]/\./g;s/\ /\./g' |\
  awk 'BEGIN { o='$offs';} { h=int(o/256); printf("%c%c",int(o-256*h), h); o+=(length($0)+1); }' |\
  dd of=./lang_$LNG.bin bs=1 seek=16 conv=notrunc 2>/dev/null
 #write binary text data
 dd if=./lang_$LNG.dat of=./lang_$LNG.bin bs=1 seek=$offs conv=notrunc 2>/dev/null
 #calculate and update checksum
 chsum=$(cat lang_$LNG.bin | xxd | cut -c11-49 | tr ' ' "\n" | sed '/^$/d' | awk 'BEGIN { sum = 0; } { sum += strtonum("0x"$1); if (sum > 0xffff) sum -= 0x10000; } END { printf("%x\n", sum); }')
 /bin/echo -n -e $(echo -n $((0x$chsum)) | awk "$awk_ui16") |\
  dd of=lang_$LNG.bin bs=1 count=2 seek=8 conv=notrunc 2>/dev/null
 #remove temporary files
 rm -f lang_$LNG.tmp
 rm -f lang_$LNG.dat
}

make_lang2 $1
exit

make_lang()
{
LNG=$1

echo "make_lang.sh started" >&2
echo "selected language=$LNG" >&2

#check if input files exists
echo -n " checking input files..." >&2
if [ ! -e lang_en.txt ]; then echo "NG!  file lang_en.txt not found!" >&2; exit 1; fi

if [ ! -e lang_en_$LNG.txt ]; then echo "NG!  file lang_en_$LNG.txt not found!" >&2; exit 1; fi
echo "OK" >&2

#filter comment and empty lines from key and dictionary files, create temporary files
echo -n " creating tmp files..." >&2
cat lang_en.txt | sed "/^$/d;/^#/d" > lang_en.tmp
cat lang_en_$LNG.txt | sed "/^$/d;/^#/d" > lang_en_$LNG.tmp
echo "OK" >&2
#cat lang_en_$LNG.tmp | sed 'n;d' >test1.txt

#compare files using diff and check for differences
echo -n " comparing tmp files..." >&2
if ! cat lang_en_$LNG.tmp | sed 'n;d' | diff lang_en.tmp - > lang_en_$LNG.dif; then
 echo "NG!" >&2
 echo "Entries in lang_en_$LNG.txt are different from lang_en.txt!" >&2
 echo "please check lang_en_$LNG.dif" >&2
 finish 1
fi
echo "OK" >&2

#generate lang_xx.txt (secondary language text data sorted by ids)
echo -n " generating lang_$LNG.txt..." >&2
cat lang_en_$LNG.tmp | sed '1~2d' | sed "s/^\"\\\\x00/\"/" > lang_$LNG.txt
echo "OK" >&2

#generate lang_xx.dat (secondary language text data in binary form)
echo -n " generating lang_$LNG.dat..." >&2
cat lang_$LNG.txt | sed "s/\\\\/\\\\\\\\/g" | while read s; do
 s=${s#\"}
 s=${s%\"}
 /bin/echo -e -n "$s\x00"
done >lang_$LNG.dat
echo "OK" >&2

#calculate variables
lt_magic='\xa5\x5a\xb4\x4b'
lt_count=$(grep -c '^' lang_$LNG.txt)
lt_data_size=$(wc -c lang_$LNG.dat | cut -f1 -d' ')
lt_offs_size=$((2 * $lt_count))
lt_size=$((16 + $lt_offs_size + $lt_data_size))
lt_chsum=0
lt_code='\xff\xff'
lt_resv1='\xff\xff\xff\xff'

case "$LNG" in
 *en*) lt_code='\x6e\x65' ;;
 *cz*) lt_code='\x73\x63' ;;
 *de*) lt_code='\x65\x64' ;;
 *es*) lt_code='\x73\x65' ;;
 *it*) lt_code='\x74\x69' ;;
 *pl*) lt_code='\x6c\x70' ;;
esac

#generate lang_xx.ofs (secondary language text data offset table)
echo -n " generating lang_$LNG.ofs..." >&2
cat lang_$LNG.txt | sed "s/\\\\x[0-9a-f][0-9a-f]/\./g;s/\\\\[0-7][0-7][0-7]/\./g" |\
 awk 'BEGIN { o='$((16 + $lt_offs_size))';} { printf("%d\n",o); o+=(length($0)-1); }' > lang_$LNG.ofs
echo "OK" >&2

#generate lang_xx.bin (secondary language result binary file)
echo " generating lang_$LNG.bin:" >&2
#create empty file
dd if=/dev/zero of=lang_$LNG.bin bs=1 count=$lt_size 2>/dev/null

#write data to binary file with dd

echo -n "  writing header (16 bytes)..." >&2
/bin/echo -n -e "$lt_magic" |\
 dd of=lang_$LNG.bin bs=1 count=4 seek=0 conv=notrunc 2>/dev/null
/bin/echo -n -e $(echo -n "$lt_size" | awk "$awk_ui16") |\
 dd of=lang_$LNG.bin bs=1 count=2 seek=4 conv=notrunc 2>/dev/null
/bin/echo -n -e $(echo -n "$lt_count" | awk "$awk_ui16") |\
 dd of=lang_$LNG.bin bs=1 count=2 seek=6 conv=notrunc 2>/dev/null
/bin/echo -n -e $(echo -n "$lt_chsum" | awk "$awk_ui16") |\
 dd of=lang_$LNG.bin bs=1 count=2 seek=8 conv=notrunc 2>/dev/null
/bin/echo -n -e "$lt_code" |\
 dd of=lang_$LNG.bin bs=1 count=2 seek=10 conv=notrunc 2>/dev/null
/bin/echo -n -e "$lt_resv1" |\
 dd of=lang_$LNG.bin bs=1 count=4 seek=12 conv=notrunc 2>/dev/null
echo "OK" >&2

echo -n "  writing offset table ($lt_offs_size bytes)..." >&2
/bin/echo -n -e $(cat lang_$LNG.ofs | awk "$awk_ui16" | tr -d '\n'; echo) |\
 dd of=./lang_$LNG.bin bs=1 count=$lt_offs_size seek=16 conv=notrunc 2>/dev/null
echo "OK" >&2

echo -n "  writing text data ($lt_data_size bytes)..." >&2
dd if=./lang_$LNG.dat of=./lang_$LNG.bin bs=1 count=$lt_data_size seek=$((16 + $lt_offs_size)) conv=notrunc 2>/dev/null
echo "OK" >&2

#calculate and update checksum
lt_chsum=$(cat lang_$LNG.bin | xxd | cut -c11-49 | tr ' ' "\n" | sed '/^$/d' | awk 'BEGIN { sum = 0; } { sum += strtonum("0x"$1); if (sum > 0xffff) sum -= 0x10000; } END { printf("%x\n", sum); }')
/bin/echo -n -e $(echo -n $((0x$lt_chsum)) | awk "$awk_ui16") |\
 dd of=lang_$LNG.bin bs=1 count=2 seek=8 conv=notrunc 2>/dev/null

echo " lang_table details:" >&2
echo "  lt_count = $lt_count" >&2
echo "  lt_size  = $lt_size" >&2
echo "  lt_chsum = $lt_chsum" >&2
}

echo $LNG

if [ "$LNG" = "all" ]; then
 make_lang cz
 make_lang de
 make_lang es
 make_lang it
 make_lang pl
 exit 0
else
 make_lang $LNG
fi

finish 0
