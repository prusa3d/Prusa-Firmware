# 1. Příprava vývojového prostředí

   1. nainstalujte vývojové prostředí `"Arduino Software IDE"` pro operační prostředí, které jste zvyklí používat  
`https://www.arduino.cc -> Software->Downloads`  
doporučujeme použít starší verzi `"1.6.9"`, kterou používáme na našem build serveru pro překlad oficiálních buildů 
_pozn.: v případě přetrvávajících potíží s překladem zkontrolujte verzi aktuálně použitého překladače jazyka C/C++ (GCC) - měla by být `4.8.1`; verzi ověříte zadáním příkazu  
`avr-gcc --version`  
pokud si nejste jisti umístěním souboru (závisí na způsobu, jakým bylo `"Arduino Software IDE"` nainstalováno), použijte funkci vyhledání v rámci systému souborů_  
_pozn.: konflikt názvů knihoven / modulů `"LiquidCrystal"` známý v předchozích verzích již není aktuální (původní knihovnu tudíž není nutné mazat ani přejmenovat)_

   2. do nabídky podporovaných cílových desek Arduino přidejte desku (`UltiMachine`) `RAMBo`  
`File->Preferences->Settings`  
do pole `"Additional Boards Manager URLs"`  
vložte adresu / text  
`"https://raw.githubusercontent.com/ultimachine/ArduinoAddons/master/package_ultimachine_index.json"`  
ev. je možno 'ručně upravit' položku  
`"boardsmanager.additional.urls=....."`  
v souboru `"preferences.txt"` (parametr umožňuje zápis seznamu adres oddělených čárkami)  
_pozn.: konkrétní umístění tohoto souboru na Vašem disku lze zjistit následujícím způsobem:  
`File->Preferences->Settings`  (`"More preferences can be edited in file ..."`)_  
následně proveďte  
`Tools->Board->BoardsManager`  
ze zobrazeného seznamu vyberte položku `"RAMBo"` (pravděpodobně bude označena jako `"RepRap Arduino-compatible Mother Board (RAMBo) by UltiMachine"`  
_pozn.: tuto položku zvolte pro všechny varianty desek použitých v tiskárnách `'Prusa i3 MKx'`, tzn. pro `RAMBo-mini x.y` i `EINSy x.y`_  
'kliknutím' na položku se zobrazí tlačítko pro instalaci; ve výběrovém seznamu zvolte verzi `"1.0.1"` (poslední známá verze k datu vydání tohoto dokumentu)  
_(po provedení instalace je položka označena poznámkou `"INSTALLED"` a lze ji následně použít při výběru cílové desky)_  

   3. modify platform.txt to enable float printf support:  
add "-Wl,-u,vfprintf -lprintf_flt -lm" to "compiler.c.elf.flags=" before existing flag "-Wl,--gc-sections"  
example:  
`"compiler.c.elf.flags=-w -Os -Wl,-u,vfprintf -lprintf_flt -lm -Wl,--gc-sections"`

# 2. Překlad zdrojoveho kódu

do zvoleného adresáře umístěte zdrojové kódy odpovídající modelu Vaší tiskárny získané z repozitáře  
`https://github.com/prusa3d/Prusa-Firmware/`  
v podadresáři `"Firmware/variants/"` vyberte konfigurační soubor (`.h`) odpovídající modelu Vaší tiskárny, vytvořte kopii s názvem `"Configuration_prusa.h"` (popř. proveďte prosté přejmenování) a překopírujte do adresáře `"Firmware/"`  

spusťte vývojové prostředí `"Arduino IDE"`; v adresáři, kam jste umístili zdrojové kódy, vyberte z podadresáře `"Firmware/"` soubor `"Firmware.ino"`  
`File->Open`  
proveďte požadované úpravy kódu; **veškeré změny ovšem provádíte na svou vlastní odpovědnost!**  

jako cílovou desku pro překlad vyberte  `"RAMBo"`  
`Tools->Board->RAMBo`  
_pozn.: nelze použít žádnou z variant `"Arduino Mega …"`, přestože se jedná o shodný MCU_  

spusťte překlad  
`Sketch->Verify/Compile`  

výsledný kód nahrajte do připojené tiskárny  
`Sketch->Upload`  

přeložený kód můžete také nechat uložit do souboru (v tzv. `HEX`-formátu) `"Firmware.ino.rambo.hex"`:  
`Sketch->ExportCompiledBinary`  
a do tiskárny ho následně nahrát pomocí programu `"FirmwareUpdater"`  
_pozn.: soubor je vytvořen v adresáři `"Firmware/"`_  
