Nova podpora vice jazyku ve firmware


Zmeny oproti stavajicimu frameworku:
1. Deklarace lokalizovanych textu primo v kodu, neni nutne udrzovat tabulky.
2. Zatim dvoj jazycna verze (en_cz, en_de atd). Moznost rozsirit na vicejazycnou (en_cz_de - pro MK2).
3. Moznost vyberu druheho jazyka ulozeneho v SPI flash (nebude zabirat misto v interni flash, pouze MK3).
5. Bash postbuild proces namisto perloveho skriptu + nastroje na spravu slovniku.

Popis:
Novy framework je trochu podobny jako znamy i18n20, ale sity na miru pro AVR atmega s ohledem na maximalni jednoduchost a usporu interni flashe.
Stringy ktere maji byt prelozene se deklaruji pomoci specialnich maker, zbytek obstara postbuild.
Vsechny lokalizovane texty se nachazi ve specialni sekci, v pripade AVR musi byt stringy umisteny v dolnich 64kB flash - tzv 'progmem'.
Po zbuildovani arduinem bude fungovat pouze anglictina, je treba spustit postbuild ktery na zaklade slovniku doplni data sekundarniho jazka a vytvori modifikovany hexfile.
Jedina data ktera je treba udrzovat jsou slovniky pro preklad. Jsou to textove soubory kde je vzdy sparovan anglicky text s prelozenym textem.
Kazdy text ve slovniku je jeden radek, muze obsahovat specialni znaky v hexadecimalni podobe (e.g. '\x0a'). Nasledujici radek je vzdy prelozeny text.
Tento jednoduchy format je zvolen proto aby bylo mozno slovniky a proces prekladu spravovat jen pomoci gitu a nekolika skriptu.

Pokud pridame nebo zmenime nejaky text v kodu, zmeni se po zbuildovani a spusteni nastroje 'update.sh' soubor lang_en_code.txt.
To je generovany soubor ktery obsahuje vsechny lokalizovane texty pouzite v kodu setridene podle abecedy.
V gitu uvidime zmenu kterou rucne preneseme do slovniku lang_en_xx.txt, zaroven vytvorime pozadavek na preklad ci korekturu pozadovaneho textu.
Pokud pridame nebo zmenime nejaky text ve slovnikach, zmeni se po spusteni nastroje 'update.sh' soubor lang_en_dict.txt.
Ten obsahuje vsechny lokalizovane texty ze slovniku (v anglictine), respektive mnozinu jejich sjednoceni.
V idealnim pripade by soubory lang_en_code.txt a lang_en_dict.txt mely byt totozne.
Pokud se zmeni slovnik, je treba znovu vygenerovat binarni soubory lang_en_xx.bin.


Pouziti v kodu, priklady:

1. deklarace lokalizovaneho textu v kodu - makro '_i':
puts_P(_i("Kill all humans!")); //v cz vypise "Zabit vsechny lidi!"

2. deklarace lokalizovaneho textu jako globalni konstanty - makro 'PROGMEM_I1' a 'ISTR':
const char MSG_PREHEAT[] PROGMEM_I1 = ISTR("Preheat"); //deklarace
puts_P(get_translation(MSG_PREHEAT)); //v cz vypise "Predehrev"

3. fukce get_translation - zkratka makro '_T':
puts_P(_T(MSG_PREHEAT)); //v cz vypise "Predehrev"

4. deklarace lokalizovaneho textu jako lokalni promenne - makro '_I':
const char* text = preheat?_I("Preheat"):_I("Cooldown");
puts_P(_T(text)); //v cz vypise "Predehrev" nebo "Zchlazeni"

5. deklarace nelokalizovaneho textu - makro 'PROGMEM_N1' a '_n' nebo '_N':
const char MSG_MK3[] PROGMEM_N1 = "MK3"; //deklarace
const char* text = _n("MK3");
 nebo
const char* text = _N("MK3");
