#include "sound.h"

#include "Marlin.h"

//#include <inttypes.h>
//#include <avr/eeprom.h>
//#include "eeprom.h"


//eSOUND_MODE eSoundMode=e_SOUND_MODE_LOUD;
// nema vyznam, pokud se bude volat Sound_Init (tzn. poc. hodnota je v EEPROM)
// !?! eSOUND_MODE eSoundMode; v ultraldc.cpp :: cd_settings_menu() se takto jevi jako lokalni promenna
eSOUND_MODE eSoundMode; //=e_SOUND_MODE_DEFAULT;


static void Sound_SaveMode(void);
static void Sound_DoSound_Echo(void);
static void Sound_DoSound_Prompt(void);
static void Sound_DoSound_Alert(bool bOnce);


void Sound_Init(void)
{
SET_OUTPUT(BEEPER);
eSoundMode=(eSOUND_MODE)eeprom_read_byte((uint8_t*)EEPROM_SOUND_MODE);
if(eSoundMode==e_SOUND_MODE_NULL)
     Sound_Default();                             // je potreba provest i ulozeni do EEPROM
}

void Sound_Default(void)
{
eSoundMode=e_SOUND_MODE_DEFAULT;
Sound_SaveMode();
}

void Sound_SaveMode(void)
{
eeprom_update_byte((uint8_t*)EEPROM_SOUND_MODE,(uint8_t)eSoundMode);
}

void Sound_CycleState(void)
{
switch(eSoundMode)
     {
     case e_SOUND_MODE_LOUD:
          eSoundMode=e_SOUND_MODE_ONCE;
          break;
     case e_SOUND_MODE_ONCE:
          eSoundMode=e_SOUND_MODE_SILENT;
          break;
     case e_SOUND_MODE_SILENT:
          eSoundMode=e_SOUND_MODE_MUTE;
          break;
     case e_SOUND_MODE_MUTE:
          eSoundMode=e_SOUND_MODE_LOUD;
          break;
     default:
          eSoundMode=e_SOUND_MODE_LOUD;
     }
Sound_SaveMode();
}

void Sound_MakeSound(eSOUND_TYPE eSoundType)
{
switch(eSoundMode)
     {
     case e_SOUND_MODE_LOUD:
          if(eSoundType==e_SOUND_TYPE_ButtonEcho)
               Sound_DoSound_Echo();
          if(eSoundType==e_SOUND_TYPE_StandardPrompt)
               Sound_DoSound_Prompt();
          if(eSoundType==e_SOUND_TYPE_StandardAlert)
               Sound_DoSound_Alert(false);
          break;
     case e_SOUND_MODE_ONCE:
          if(eSoundType==e_SOUND_TYPE_ButtonEcho)
              Sound_DoSound_Echo();
          if(eSoundType==e_SOUND_TYPE_StandardPrompt)
               Sound_DoSound_Prompt();
          if(eSoundType==e_SOUND_TYPE_StandardAlert)
               Sound_DoSound_Alert(true);
          break;
     case e_SOUND_MODE_SILENT:
          if(eSoundType==e_SOUND_TYPE_StandardAlert)
               Sound_DoSound_Alert(true);
          break;
     case e_SOUND_MODE_MUTE:
          break;
     default:
          ;
     }
}


static void Sound_DoSound_Echo(void)
{
uint8_t nI;

for(nI=0;nI<10;nI++)
     {
     WRITE(BEEPER,HIGH);
     delayMicroseconds(100);
     WRITE(BEEPER,LOW);
     delayMicroseconds(100);
     }
}

static void Sound_DoSound_Prompt(void)
{
WRITE(BEEPER,HIGH);
delay_keep_alive(500);
WRITE(BEEPER,LOW);
}

static void Sound_DoSound_Alert(bool bOnce)
{
uint8_t nI,nMax;

nMax=bOnce?1:3;
for(nI=0;nI<nMax;nI++)
     {
     WRITE(BEEPER,HIGH);
     delay_keep_alive(200);
     WRITE(BEEPER,LOW);
     delay_keep_alive(500);
     }
}
