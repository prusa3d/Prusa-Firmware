#ifndef SOUND_H
#define SOUND_H


#define MSG_SOUND_MODE_LOUD "Sound      [loud]"
#define MSG_SOUND_MODE_ONCE "Sound      [once]"
#define MSG_SOUND_MODE_SILENT "Sound    [silent]"
#define MSG_SOUND_MODE_MUTE "Sound      [mute]"


#define e_SOUND_MODE_NULL 0xFF
typedef enum
     {e_SOUND_MODE_LOUD,e_SOUND_MODE_ONCE,e_SOUND_MODE_SILENT,e_SOUND_MODE_MUTE} eSOUND_MODE;
#define e_SOUND_MODE_DEFAULT e_SOUND_MODE_LOUD

typedef enum
     {e_SOUND_TYPE_ButtonEcho,e_SOUND_TYPE_EncoderEcho,e_SOUND_TYPE_StandardPrompt,e_SOUND_TYPE_StandardConfirm,e_SOUND_TYPE_StandardWarning,e_SOUND_TYPE_StandardAlert} eSOUND_TYPE;
typedef enum
     {e_SOUND_CLASS_Echo,e_SOUND_CLASS_Prompt,e_SOUND_CLASS_Confirm,e_SOUND_CLASS_Warning,e_SOUND_CLASS_Alert} eSOUND_CLASS;


extern eSOUND_MODE eSoundMode;


extern void Sound_Init(void);
extern void Sound_Default(void);
extern void Sound_Save(void);
extern void Sound_CycleState(void);
extern void Sound_MakeSound(eSOUND_TYPE eSoundType);

//static void Sound_DoSound_Echo(void);
//static void Sound_DoSound_Prompt(void);

#endif // SOUND_H
