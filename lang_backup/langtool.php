<?php 

$langs = array("en","cz","it","es","pl");

function parselang($a) {
	$out=array();
	foreach ($a as $line){
		if (!strpos($line,"MSG")) continue;
		//echo $line;
		preg_match('/\#define\s*(\S*)\s*(.*)/is',$line,$a);
		//print_r($a);die();
		if (!$a[1]) continue;
		$v = trim($a[2]);

		//$v = str_replace('MACHINE_NAME "','"Prusa i3',$v);
		$v = str_replace('" FIRMWARE_URL "','https://github.com/prusa3d/Prusa-i3-Plus/',$v);
		$v = str_replace('" PROTOCOL_VERSION "','1.0',$v);
		$v = str_replace('" STRINGIFY(EXTRUDERS) "','1',$v);
		$v = str_replace('" MACHINE_UUID "','00000000-0000-0000-0000-000000000000',$v);

		$out[trim($a[1])]=$v;
	}
	return $out;
}

foreach ($langs as $lang) {
  $f = file("language_".$lang.".h");
  $$lang = parselang($f);
}
/*
$f = file("language_cz.h");
$cz = parselang($f);

$f = file("language_en.h");
$en = parselang($f);
*/
//print_r($cz);

$out="#ifndef LANGUAGE_ALL_H\n#define LANGUAGE_ALL_H\n\nextern unsigned char lang_selected;\nextern const char** MSG_ALL[];\n";

$counter=0;
$langname = 0;
$langsel = 0;
foreach ($cz as $k=>$v) {

	$out .= "#define $k MSG_ALL[lang_selected][$counter]\n";
	if ($k=="MSG_LANGUAGE_NAME") $langname = $counter;
	if ($k=="MSG_LANGUAGE_SELECT") $langsel = $counter;
	//$out2 .= "\t".''.$v.','."\n";
	//$out3 .= "\t".''.$en[$k].','."\n";

	$counter++;
}
$out.="#define LANGUAGE_NAME $langname \n";
$out.="#define LANGUAGE_SELECT $langsel \n";
$out.="#define LANG_NUM ".count($langs)." \n";
$out.="char* CAT2(const char *s1,const char *s2);\n";
$out.="char* CAT4(const char *s1,const char *s2,const char *s3,const char *s4);\n";
$out.="\n\n\n#endif //LANGUAGE_ALL.H";

file_put_contents("language_all.h",$out);
echo ".h created\n";


$out="#include <avr/pgmspace.h>\n#include \"configuration_prusa.h\"\n#define LCD_WIDTH 20\nextern unsigned char lang_selected;\n";
foreach ($langs as $lang) {
	$outa[$lang]="const char* MSG".strtoupper($lang)."[]  = {";
}
//$out2="const char* MSGCZ[]  = {";
//$out3="const char* MSGEN[]  = {";
$counter=0;
foreach ($cz as $k=>$v) {
	foreach ($langs as $lang) {
		$out .= "const char MSG".strtoupper($lang)."${counter}[] PROGMEM = { ".${$lang}[$k]." }; //$k\n";
		$outa[$lang].="MSG".strtoupper($lang)."$counter, //$k\n";
		if (${$lang}[$k] == "") echo "ERR: In language: ".strtoupper($lang)." string \"".$k."\" NOT FOUND !\n";
	}
	//$out .= "const char MSGCZ${counter}[] PROGMEM = { $v }; //$k\n";
	//$out .= "const char MSGEN${counter}[] PROGMEM = {".$en[$k]."}; //$k\n";

	//$out2 .= "MSGCZ$counter, //$k\n";
	//$out3 .= "MSGEN$counter, //$k\n";
	$counter++;
}

$langlist=array();
foreach ($langs as $lang) {
	$out.=	$outa[$lang] . "};\n";
	$langlist[]="MSG".strtoupper($lang);
}
/*
$out2.="};\n";
$out3.="};\n";

$out .= $out2;
$out .= $out3;
*/


$out .= "\n\nconst char** MSG_ALL[] = {".join(",",$langlist)."};";

$tools = <<<EOF

char langbuffer[LCD_WIDTH+1];
char* CAT2(const char *s1,const char *s2) {
  unsigned char len=0;
  strncpy_P(langbuffer+len,s1,LCD_WIDTH-len);
  len+=strlen_P(s1);
  strncpy_P(langbuffer+len,s2,LCD_WIDTH-len);
  return langbuffer;
}
char* CAT4(const char *s1,const char *s2,const char *s3,const char *s4) {
  unsigned char len=0;
  strncpy_P(langbuffer+len,s1,LCD_WIDTH-len);
  len+=strlen_P(s1);
  strncpy_P(langbuffer+len,s2,LCD_WIDTH-len);
  len+=strlen_P(s2);
  strncpy_P(langbuffer+len,s3,LCD_WIDTH-len);
  len+=strlen_P(s3);
  strncpy_P(langbuffer+len,s4,LCD_WIDTH-len);
  return langbuffer;
}
EOF;

$out.=$tools;

file_put_contents("language_all.cpp",$out);
echo ".cpp created.\nDone!\n";
//const char MSG1[] PROGMEM =  MSG_SD_INSERTED;
//const char* MSGCZ[] PROGMEM = {MSG_SD_INSERTED,MSG_SD_REMOVED};
