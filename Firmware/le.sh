# line ending management script
# CRLF - windows default ('\r\n')
# LF   - unix default ('\n')
# arguments:
# ?crlf - print all .cpp and .h files with CRLF line endings
# ?lf   - print all .cpp and .h files with LF line endings
# crlf - replace line endings in all .cpp and .h files to CRLF
# lf   - replace line endings in all .cpp and .h files to LF

if [ "$1" == "?crlf" ] || [ $# -eq 0 ]; then
 echo 'cpp and h files with CRLF line endings:'
 find {*.cpp,*.h} -not -type d -exec file "{}" ";" | grep CRLF | sed 's/:.*//g'
elif [ "$1" == "?lf" ]; then
 echo 'cpp and h files with LF line endings:'
 find {*.cpp,*.h} -not -type d -exec file "{}" ";" | grep -v CRLF | sed 's/:.*//g'
fi
if [ "$1" == "crlf" ]; then
 echo 'replacing LF with CRLF in all cpp and h files:'
 find {*.cpp,*.h} -not -type d -exec file "{}" ";" | grep -v CRLF | sed 's/:.*//g' | while read fn; do
  echo "$fn"
  sed -i 's/$/\r/g' $fn
 done
elif [ "$1" == "lf" ]; then
 echo 'replacing CRLF with LF in all cpp and h files:'
 find {*.cpp,*.h} -not -type d -exec file "{}" ";" | grep CRLF | sed 's/:.*//g' | while read fn; do
  echo "$fn"
  sed -i 's/\r\n/\n/g' $fn
 done
fi
