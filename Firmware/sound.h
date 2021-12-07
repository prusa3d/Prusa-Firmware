#include <stdint.h>
#ifndef SOUND_H
#define SOUND_H


#define e_SOUND_MODE_NULL 0xFF
typedef enum
     {e_SOUND_MODE_LOUD,e_SOUND_MODE_ONCE,e_SOUND_MODE_SILENT,e_SOUND_MODE_BLIND} eSOUND_MODE;
#define e_SOUND_MODE_DEFAULT e_SOUND_MODE_LOUD

typedef enum
     {e_SOUND_TYPE_ButtonEcho,e_SOUND_TYPE_EncoderEcho,e_SOUND_TYPE_StandardPrompt,e_SOUND_TYPE_StandardConfirm,e_SOUND_TYPE_StandardWarning,e_SOUND_TYPE_StandardAlert,e_SOUND_TYPE_EncoderMove,e_SOUND_TYPE_BlindAlert} eSOUND_TYPE;
typedef enum
     {e_SOUND_CLASS_Echo,e_SOUND_CLASS_Prompt,e_SOUND_CLASS_Confirm,e_SOUND_CLASS_Warning,e_SOUND_CLASS_Alert} eSOUND_CLASS;

// The minimum number of milliseconds of sound duration, to use the keep-alive method instead of the default delay
// This is used to allow the printer to retain control in long-running sound output
#define SOUND_KEEPALIVE_MIN_MS 1100

extern eSOUND_MODE eSoundMode;


extern void Sound_Init(void);
extern void Sound_Default(void);
extern void Sound_CycleState(void);
extern void Sound_MakeSound(eSOUND_TYPE eSoundType);
extern void Sound_MakeCustom(uint16_t ms,uint16_t tone_ ,bool critical);

//static void Sound_DoSound_Echo(void);
//static void Sound_DoSound_Prompt(void);

#endif // SOUND_H
