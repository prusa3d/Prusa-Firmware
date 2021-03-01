# How-to add a new language to Prusa Firmware

We will use Dutch as an example here.

## Prepare Prusa Firmware

QR = palceholder for language in upper case

qr = placehodler for language in lower case

AB = palceholder for hexadecial

Files needs to be modified
- `../Firmware/language.h` 

  In section `/** @name Language codes (ISO639-1)*/` add the new `#define LANG_CODE_QR 0xABAB //!<'qr'`following ISO639-1 convention for QR.
https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes

  Example:
  `#define LANG_CODE_NL 0x6e6c //!<'nl'` where the hex value `0x6e6c` is in ascii `nl`

- `../Firmware/language.c`

  In section `const char* lang_get_name_by_code(uint16_t code)` add `case LANG_CODE_NL: return _n("Language");`

  Example:

  `case LANG_CODE_NL: return _n("Nederlands");` Where `Language` is native spoken version, here `Nederlands` (Netherlands) or `Vlaams` (Belgium). This will be displayed on the LCD menu.

- `../lang/lang-add.sh`

  In section `cat lang_add.txt | sed 's/^/"/;s/$/"/' | while read new_s; do` add `insert_qr "$new_s" 'qr'`where qr 
  
  Example:
  `insert_qr "$new_s" 'nl'` with qr value `nl`for Dutch

- `../lang/lang-build.sh`

  In section `#returns hexadecial data for lang code` add a case `*qr*) echo '0x71\0x72'`
  
  Example:
  `*nl*) echo '\x6c\x6e' ;;` !!! IMPORTANT that the hex values are switched so 'nl' is here in 'ln' !!!
  
  In generate "all" section add `generate_binary 'qr'
  
  Example:
  `generate_binary 'nl'`
  
- `../lang/lang-check.py`

  Add in `help` the new language `qr`
  
  Example:
  From `help="Check lang file (en|cs|de|es|fr|it|pl)")` to `help="Check lang file (en|cs|de|es|fr|nl|it|pl)")`
  
- In `../lang/lang-clean.sh`

  In section echo `"lang-clean.sh started" >&2` add `clean_lang qr`
  
  Example:
  `clean_lang nl`
  

- `../lang/lang-export.sh`

  In section `# if 'all' is selected, script will generate all po files and also pot file` add `./lang-export.sh qr`
  
  Example:
  
  `./lang-export.sh nl`
  
  In section ` # language name in english` add `*qr*) echo "Language-in-English" ;;`
  
  Example:
  `*nl*) echo "Dutch" ;;`
  
  

  
- `../lang/lang-import.sh`

  In section `#replace in languages translation` add new rule set for the language.
  As the LCD screen doesn't not support äöüßéè and other special characters, it makes sense to "normalize" these.
  
  Example:
  ```
  #replace in dutch translation according to https://nl.wikipedia.org/wiki/Accenttekens_in_de_Nederlandse_spelling
  if [ "$LNG" = "nl" ]; then
   #replace 'ë' with 'e'
   sed -i 's/\xc3\xab/e/g' $LNG'_filtered.po'
   #replace 'ï' with 'i'
   sed -i 's/\xc3\xaf/i/g' $LNG'_filtered.po'
   #replace 'é' with 'e'
   sed -i 's/\xc3\xa9/e/g' $LNG'_filtered.po'
   #replace 'è' with 'e' (left)
   sed -i 's/\xc3\xa8/e/g' $LNG'_filtered.po'
   #replace 'ö' with 'o' (left)
   sed -i 's/\xc3\xb6/o/g' $LNG'_filtered.po'
   #replace 'ê' with 'e' (left)
   sed -i 's/\xc3\xaa/e/g' $LNG'_filtered.po'
   #replace 'ü' with 'u' (left)
   sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
   #replace 'ç' with 'c' (left)
   sed -i 's/\xc3\xa7/c/g' $LNG'_filtered.po'
   #replace 'á' with 'a' (left)
   sed -i 's/\xc3\xa1/a/g' $LNG'_filtered.po'
   #replace 'à' with 'a' (left)
   sed -i 's/\xc3\xa0/a/g' $LNG'_filtered.po'
   #replace 'ä' with 'a' (left)
   sed -i 's/\xc3\xa4/a/g' $LNG'_filtered.po'
   #replace 'û' with 'u' (left)
   sed -i 's/\xc3\xbc/u/g' $LNG'_filtered.po'
   #replace 'î' with 'i' (left)
   sed -i 's/\xc3\xae/i/g' $LNG'_filtered.po'
   #replace 'í' with 'i' (left)
   sed -i 's/\xc3\xad/i/g' $LNG'_filtered.po'
   #replace 'ô' with 'o' (left)
   sed -i 's/\xc3\xb4/o/g' $LNG'_filtered.po'
   #replace 'ú' with 'u' (left)
   sed -i 's/\xc3\xba/u/g' $LNG'_filtered.po'
   #replace 'ñ' with 'n' (left)
   sed -i 's/\xc3\xb1/n/g' $LNG'_filtered.po'
   #replace 'â' with 'a' (left)
   sed -i 's/\xc3\xa2/a/g' $LNG'_filtered.po'
   #replace 'Å' with 'A' (left)
   sed -i 's/\xc3\x85/A/g' $LNG'_filtered.po'
  fi
  ```

- `../lang/fw-build.sh`

  In section `#update _SEC_LANG in binary file if language is selected` add
  ```
  if [ -e lang_qr.bin ]; then
   echo -n " Language-in-English  : " >&2
   ./update_lang.sh qr 2>./update_lang_qr.out 1>/dev/null
   if [ $? -eq 0 ]; then echo 'OK' >&2; else echo 'NG!' >&2; fi
  fi
  ```
 
  Example:
  ```
  if [ -e lang_nl.bin ]; then
   echo -n " Dutch  : " >&2
   ./update_lang.sh nl 2>./update_lang_nl.out 1>/dev/null
  if [ $? -eq 0 ]; then echo 'OK' >&2; else echo 'NG!' >&2; fi
  fi
  ```
  
  In section `#create binary file with all languages` add `if [ -e lang_qr.bin ]; then cat lang_qr.bin >> lang.bin; fi`
  
  Example:
  
  `if [ -e lang_nl.bin ]; then cat lang_nl.bin >> lang.bin; fi`

- `../lang/fw-clean.sh`

  In section `echo "fw-clean.sh started" >&2` add
  
  ```
  rm_if_exists firmware_qr.hex
  ...
  ...
  rm_if_exists update_lang_qr.out
  ```
  
  Example:
  `rm_if_exists firmware_nl.hex`
  
  and
  
  `rm_if_exists update_lang_nl.out`


## Prepare language part

To prepare the acutal language translation files we need create the `lang_en_qr.txt` file.
1. Copy and `lang_en.txt` as `lang_en_qr.txt`
2. run `../lang/lang-export.sh`
3. copy `../lang/po/Firmware_qr.po` file to `../lang/po/new/qr.po`
4. translate all messages using POEdit or other tools.
5. use `lang/lang-import.sh qr` to generate `lang_en_qr.txt` from translated po files
6. move `../lang/po/new/lang_en_qr.txt` to `../lang/lang_en_qr.txt`
7. cleanup `../lang/po/new` folder by deleting
   ```
   qr_filtered.po
   qr_new.po
   noasci.txt
   ```




##