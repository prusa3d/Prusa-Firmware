lang_upgrade - scripts for migration to new multilanguage support design


0. clean.sh
delete all output files

1. make_msgs.sh
Process all language_xx.h files and extract informations to msgs_xx.txt files in simple format.
Every line in msgs_en.txt has following format: MSG_xx c=cc r=rr "text". 
Every line in other msgs_xx.txt has simpler format: MSG_xx "text".
MSG_xx is original message identifier, cc is column count (originaly length) and rr is row count (originaly lines).
Output files msgs_xx.txt are sorted by message identifier (ascending order).
make_msgs.sh also reports number of messages in each language_xx.h file and total number of characters in program memory.

2. find_msgs.sh
Find usage of each message and output listing in to file msgs_usage.txt in format: MSG_xx nn.
MSG_xx is identifier, nn is number of occurrences. Output is sorted by number of occurrences (ascending order).

3. replace_msgs.sh
List all unused messages to file msgs_unused.txt.
Copy all source files to folder ./source
Replace all single-used messages in all ./source/*.c* files directly with the english version string constant and comment at end of line.
Generate messages.h and messages.c source files with messages used twice and more.

4. upgrade.sh
Backup (move) all language*.h and language*.cpp files from source to folder '../backup'.
Copy folder ./source/*.* to ../Firmware, new files will be messages.h, messages.c, language.h and other source will be replaced.
After this step should be source compilable in english version, LANG_MODE in config.h is set to LANG_MODE_SINGLE.
