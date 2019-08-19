# Translations

## Workflow

- Build firmware using build.sh. That creates necessary files in the `lang` dir.
- add messages that are new: investigate not-tran.txt, copy selected strings into `lang_add.txt` and run lang-add.sh
- export PO files for translation: `lang-export.sh`
- when translators finish their work use `lang-import.sh`

## Code / usage
There are 2 modes of operation. If `LANG_MODE==0`, only one language is being used (the default compilation approach from plain Arduino IDE).
The reset of this explanation is devoted to `LANG_MODE==1`:

`language.h`:
```C++
// section .loc_sec (originaly .progmem0) will be used for localized translated strings
#define PROGMEM_I2 __attribute__((section(".loc_sec")))
// section .loc_pri (originaly .progmem1) will be used for localized strings in english
#define PROGMEM_I1 __attribute__((section(".loc_pri")))
// section .noloc (originaly progmem2) will be used for not localized strings in english
#define PROGMEM_N1 __attribute__((section(".noloc")))
#define _I(s) (__extension__({static const char __c[] PROGMEM_I1 = "\xff\xff" s; &__c[0];}))
#define ISTR(s) "\xff\xff" s
#define _i(s) lang_get_translation(_I(s))
#define _T(s) lang_get_translation(s)
```
That explains the macros:
- `_i` expands into `lang_get_translation((__extension__({static const char __c[] PROGMEM_I1 = "\xff\xff" s; &__c[0];})))` . Note the two 0xff's in the beginning of the string. `_i` allows for declaring a string directly inplace of C++ code, no string table is used. The downside of this approach is obvious - the compiler is not able/willing to merge duplicit strings into one.
- `_T` expands into `lang_get_translation(s)` without the two 0xff's at the beginning. Must be used in conjunction with MSG tables in `messages.h`. Allows to declare a string only once and use many times.
- `_N` means not-translated. These strings reside in a different segment of memory.

The two 0xff's are somehow magically replaced by real string ID's where the translations are available (still don't know where).
```C++
const char* lang_get_translation(const char* s){
	if (lang_selected == 0) return s + 2; //primary language selected, return orig. str.
	if (lang_table == 0) return s + 2; //sec. lang table not found, return orig. str.
	uint16_t ui = pgm_read_word(((uint16_t*)s)); //read string id
	if (ui == 0xffff) return s + 2; //translation not found, return orig. str.
	ui = pgm_read_word(((uint16_t*)(((char*)lang_table + 16 + ui*2)))); //read relative offset
	if (pgm_read_byte(((uint8_t*)((char*)lang_table + ui))) == 0) //read first character
		return s + 2;//zero length string == not translated, return orig. str.
	return (const char*)((char*)lang_table + ui); //return calculated pointer
}
```

## Files

### `lang_en.txt`
```
#MSG_CRASH_DET_ONLY_IN_NORMAL c=20 r=4
"Crash detection can\x0abe turned on only in\x0aNormal mode"
```

### `lang_en_*.txt`
```
#MSG_CRASH_DET_ONLY_IN_NORMAL c=20 r=4
"Crash detection can\x0abe turned on only in\x0aNormal mode"
"Crash detekce muze\x0abyt zapnuta pouze v\x0aNormal modu"
```
1. a comment - usually a MSG define with number of characters (c) and rows (r)
2. English text
3. translated text

### `not_tran.txt`
A simple list of strings that are not translated yet.

### `not_used.txt`
A list os strings not currently used in the firmware. May be removed from `lang_en_*.txt` files by hand.

## Scripts

### `config.sh`
- Checks setup and sets auxiliary env vars used in many other scripts. 
- Looks for env var `ARDUINO`. If not found/empty, a default `C:/arduino-1.8.5` is used.
- Sets env var `CONFIG_OK=1` when all good, otherwise sets `CONFIG_OK=0`

### `fw-build.sh`
Joins firmware HEX and language binaries into one file.

### `fw-clean.sh`

### `lang-add.sh`
Adds new messages into the dictionary regardless of whether there have been any older versions.

### `lang-build.sh`
Generates lang_xx.bin (language binary files) for the whole firmware build.

Arguments:
- `$1` : language code (`en`, `cz`, `de`, `es`, `fr`, `it`, `pl`) or `all`
- empty/no arguments defaults to `all`

Input: `lang_en.txt` or `lang_en_xx.txt`

Output: `lang_xx.bin`

Temporary files: `lang_xx.tmp` and `lang_xx.dat`

Description of the process:
The script first runs `lang-check.py $1` and removes empty lines and comments (and non-translated texts) into `lang_$1.tmp`.
The tmp file now contains all translated texts (some of them empty, i.e. "").
The tmp file is then transformed into `lang_$1.dat`, which is a simple dump of all texts together, each terminated with a `\x00`.
Format of the `bin` file:
- 00-01: `A5 5A`
- 02-03: `B4 4B`
- 04-05: 2B size
- 06-07: 2B number of strings
- 08-09: 2B checksum
- 0A-0B: 2B lang code hex data: basically `en` converted into `ne`, i.e. characters swapped. Only `cz` is changed into `sc` (old `cs` ISO code).
- 0C-0D: 2B signature low
- 0E-0F: 2B signature high
- 10-(10 + 2*number of strings): table of string offsets from the beginning of this file
- after the table there are the strings themselves, each terminated with `\x00`

The signature is composed of 2B number of strings and 2B checksum in lang_en.bin. Signature in lang_en.bin is zero.

### `lang-check.sh` and `lang-check.py`
Both do the same, only lang-check.py is newer, i.e. lang-check.sh is not used anymore.
lang-check.py makes a binary comparison between what's in the dictionary and what's in the binary.

### `lang-clean.sh`
Removes all language output files from lang folder. That means deleting:
-  if [ "$1" = "en" ]; then
  rm_if_exists lang_$1.tmp
 else
  rm_if_exists lang_$1.tmp
  rm_if_exists lang_en_$1.tmp
  rm_if_exists lang_en_$1.dif
  rm_if_exists lang_$1.ofs
  rm_if_exists lang_$1.txt
 fi
 rm_if_exists lang_$1_check.dif
 rm_if_exists lang_$1.bin
 rm_if_exists lang_$1.dat
 rm_if_exists lang_$1_1.tmp
 rm_if_exists lang_$1_2.tmp

### `lang-export.sh`
Exports PO (gettext) for external translators.

### `lang-import.sh`
Import from PO

### `progmem.sh`

Examine content of progmem sections (default is progmem1).

Input:
- $OUTDIR/Firmware.ino.elf
- $OUTDIR/sketch/*.o (all object files)

Outputs:
- text.sym     - formated symbol listing of section '.text'
- $PROGMEM.sym - formated symbol listing of section '.progmemX'
- $PROGMEM.lss - disassembly listing file
- $PROGMEM.hex - variables - hex
- $PROGMEM.chr - variables - char escape
- $PROGMEM.var - variables - strings
- $PROGMEM.txt - text data only (not used)

Description of process:
- check input files
- remove output files
- list symbol table of section '.text' from output elf file to text.sym (sorted by address)
- calculate start and stop address of section '.$PROGMEM'
- dump $PROGMEM data in hex format, cut disassembly (keep hex data only) into $PROGMEM.lss
- convert $PROGMEM.lss to $PROGMEM.hex:
 - replace empty lines with '|' (variables separated by empty lines)
 - remove address from multiline variables (keep address at first variable line only)
 - remove '<' and '>:', remove whitespace at end of lines
 - remove line-endings, replace separator with '\n' (join hex data lines - each line will contain single variable)
- convert $PROGMEM.hex to $PROGMEM.chr (prepare string data for character check and conversion)
 - replace first space with tab
 - replace second and third space with tab and space
 - replace all remaining spaces with '\x'
 - replace all tabs with spaces
- convert $PROGMEM.chr to $PROGMEM.var (convert data to text) - a set of special characters is escaped here including `\x0a`


### `textaddr.sh`

Compiles `progmem1.var` and `lang_en.txt` files to `textaddr.txt` file (mapping of progmem addreses to text idenifiers).

Description of process:
- check if input files exists
- create sorted list of strings from progmem1.var and lang_en.txt
- lines from progmem1.var will contain addres (8 chars) and english text
- lines from lang_en.txt will contain linenumber and english text
- after sort this will generate pairs of lines (line from progmem1 first)
- result of sort is compiled with simple script and stored into file textaddr.txt

Input:
- progmem1.var
- lang_en.txt

Output:
- textaddr.txt



update_lang.sh
