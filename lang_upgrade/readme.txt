lang_upgrade - scripts for migration to new multilanguage support design

upgrade.sh - entire process:
Run scripts: clean.sh, make_msgs.sh, find_msgs.sh, make_source.sh.
Backup (move) all language*.h and language*.cpp files from source to folder '../lang_backup'.
Copy folder ./source/*.* to ../Firmware, new files will be messages.h, messages.c, language.h, language.c and other source will be replaced.
After this step should be source compilable in english version, LANG_MODE in config.h is set to 0 (primary language only)


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
Generate filtered msgs_en.txt and msgs_common.txt files. Each file is sorted to three output files - unused, used once and used more.
Output files will be:
 msgs_common_unused.txt, msgs_common_used_more.txt, msgs_common_used_once.txt
 msgs_en_unused.txt, msgs_en_used_more.txt, msgs_en_used_once.txt

3. make_source.sh
Copy all source files to folder ./source
Replace all messages used once in all ./source/*.c* files directly with the english version string constant and comment at end of line.
Generate messages.h and messages.c source files with messages used twice and more.
Replace line '' in Marlin_main.cpp with comment.
