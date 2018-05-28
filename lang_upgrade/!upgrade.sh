#!/bin/sh
# upgrade.sh
#

if [ -e ../lang_backup ]; then
 echo 'folder ../lang_backup already exist!'
else
 ./clean.sh
 ./make_msgs.sh
 ./find_msgs.sh
 ./make_source.sh
 echo 'backup old files...'
 mkdir ../lang_backup
 mv ../Firmware/langtool.* ../lang_backup/
 mv ../Firmware/language*.* ../lang_backup/
 echo 'copying new files...'
 cp ./source/* ../Firmware/
 echo 'finished'
fi
read