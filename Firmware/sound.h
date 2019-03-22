#ifndef SOUND_H
#define SOUND_H


#define MSG_SOUND_MODE_ON  "Menu beep    [on]"
#define MSG_SOUND_MODE_OFF "Menu beep   [off]"

#define MSG_ALERT_MODE_REPEAT "Alert [repeating]"
#define MSG_ALERT_MODE_ONCE   "Alert      [once]"
#define MSG_ALERT_MODE_OFF    "Alert       [off]"


#define e_SOUND_MODE_NULL 0xFF
typedef enum
     {e_SOUND_MODE_ON,e_SOUND_MODE_OFF} eSOUND_MODE;
#define e_SOUND_MODE_DEFAULT e_SOUND_MODE_ON

#define e_ALERT_MODE_NULL 0xFF
typedef enum
     {e_ALERT_MODE_REPEAT,e_ALERT_MODE_ONCE,e_ALERT_MODE_OFF} eALERT_MODE;
#define e_ALERT_MODE_DEFAULT e_ALERT_MODE_REPEAT

typedef enum
     {e_SOUND_TYPE_ButtonEcho,e_SOUND_TYPE_EncoderEcho,e_SOUND_TYPE_StandardPrompt,e_SOUND_TYPE_StandardConfirm,e_SOUND_TYPE_StandardWarning,e_SOUND_TYPE_StandardAlert} eSOUND_TYPE;
typedef enum
     {e_SOUND_CLASS_Echo,e_SOUND_CLASS_Prompt,e_SOUND_CLASS_Confirm,e_SOUND_CLASS_Warning,e_SOUND_CLASS_Alert} eSOUND_CLASS;


extern eSOUND_MODE eSoundMode;
extern eALERT_MODE eAlertMode;


extern void Sound_Init(void);
extern void Sound_Default(void);
extern void Sound_Save(void);
extern void Sound_CycleState(void);
extern void Alert_Default(void);
extern void Alert_Save(void);
extern void Alert_CycleState(void);
extern void Sound_MakeSound(eSOUND_TYPE eSoundType);

//static void Sound_DoSound_Echo(void);
//static void Sound_DoSound_Prompt(void);

#endif // SOUND_H
