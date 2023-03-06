#ifndef CARDREADER_H
#define CARDREADER_H

#define SDSUPPORT

#ifdef SDSUPPORT

#define MAX_DIR_DEPTH 6

#include "SdFile.h"
class CardReader
{
public:
  CardReader();
  
  enum LsAction : uint8_t
  {
    LS_SerialPrint,
    LS_Count,
    LS_GetFilename,
  };
  struct ls_param
  {
    bool LFN : 1;
    bool timestamp : 1;
    inline ls_param():LFN(0), timestamp(0) { }
    inline ls_param(bool LFN, bool timestamp):LFN(LFN), timestamp(timestamp) { }
  } __attribute__((packed));
  
  void initsd(bool doPresort = true);
  void write_command(char *buf);
  void write_command_no_newline(char *buf);
  //files auto[0-9].g on the sd card are performed in a row
  //this is to delay autostart and hence the initialisaiton of the sd card to some seconds after the normal init, so the device is available quick after a reset

  void checkautostart(bool x); 
  void openFileWrite(const char* name);
  void openFileReadFilteredGcode(const char* name, bool replace_current = false);
  void openLogFile(const char* name);
  void removeFile(const char* name);
  void closefile(bool store_location=false);
  void release();
  void startFileprint();
  uint32_t getFileSize();
  void getStatus(bool arg_P);
  void printingHasFinished();

  void getfilename(uint16_t nr, const char* const match=NULL);
  void getfilename_simple(uint16_t entry, const char * const match = NULL);
  void getfilename_next(uint32_t position, const char * const match = NULL);
  uint16_t getnrfilenames();
  
  void getAbsFilename(char *t);
  void printAbsFilenameFast();
  void getDirName(char* name, uint8_t level);
  uint8_t getWorkDirDepth();
  

  void ls(ls_param params);
  bool chdir(const char * relpath, bool doPresort);
  void updir();
  void setroot(bool doPresort);

  #ifdef SDCARD_SORT_ALPHA
     void presort();
     void getfilename_sorted(const uint16_t nr, uint8_t sdSort);
     void getfilename_afterMaxSorting(uint16_t entry, const char * const match = NULL);
  #endif

  FORCE_INLINE bool isFileOpen() { return file.isOpen(); }
  bool eof() { return sdpos>=filesize; }
  FORCE_INLINE int16_t getFilteredGcodeChar()
  {
      int16_t c = (int16_t)file.readFilteredGcode();
      sdpos = file.curPosition();
      return c;
  };
  void setIndex(long index) {sdpos = index;file.seekSetFilteredGcode(index);};
  FORCE_INLINE uint8_t percentDone(){if(!isFileOpen()) return 0; if(filesize) return sdpos/((filesize+99)/100); else return 0;};
  FORCE_INLINE char* getWorkDirName(){workDir.getFilename(filename);return filename;};
  FORCE_INLINE uint32_t get_sdpos() { if (!isFileOpen()) return 0; else return(sdpos); };

  bool ToshibaFlashAir_isEnabled() const { return card.getFlashAirCompatible(); }
  void ToshibaFlashAir_enable(bool enable) { card.setFlashAirCompatible(enable); }
  bool ToshibaFlashAir_GetIP(uint8_t *ip);

public:
  bool saving;
  bool logging;
  bool sdprinting ;  
  bool cardOK ;
  char filename[FILENAME_LENGTH];
  // There are scenarios when simple modification time is not enough (on MS Windows)
  // Therefore these timestamps hold the most recent one of creation/modification date/times
  uint16_t crmodTime, crmodDate;
  uint32_t /* cluster, */ position;
  char longFilename[LONG_FILENAME_LENGTH];
  bool filenameIsDir;
  int lastnr; //last number of the autostart;
#ifdef SDCARD_SORT_ALPHA
  bool presort_flag;
#endif // SDCARD_SORT_ALPHA
  char dir_names[MAX_DIR_DEPTH][9];
private:
  SdFile root,*curDir,workDir,workDirParents[MAX_DIR_DEPTH];
  uint8_t workDirDepth;

  // Sort files and folders alphabetically.
#ifdef SDCARD_SORT_ALPHA
  uint16_t sort_count;        // Count of sorted items in the current directory
  uint16_t sort_entries[SDSORT_LIMIT];
  uint16_t lastSortedFilePosition;

#endif // SDCARD_SORT_ALPHA

#ifdef DEBUG_SD_SPEED_TEST
public:
#endif //DEBUG_SD_SPEED_TEST
  Sd2Card card;

private:
  SdVolume volume;
  SdFile file;
  #define SD_PROCEDURE_DEPTH 1
  #define MAXPATHNAMELENGTH (13*MAX_DIR_DEPTH+MAX_DIR_DEPTH+1)
  uint8_t file_subcall_ctr;
  uint32_t filespos[SD_PROCEDURE_DEPTH];
  char filenames[SD_PROCEDURE_DEPTH][MAXPATHNAMELENGTH];
  uint32_t filesize;
  //int16_t n;
  ShortTimer autostart_atmillis;
  uint32_t sdpos ;

  uint16_t nrFiles; //counter for the files in the current directory and recycled as position counter for getting the nrFiles'th name in the directory.

  bool diveSubfolder (const char *&fileName);
  void lsDive(const char *prepend, SdFile parent, const char * const match=NULL, LsAction lsAction = LS_GetFilename, ls_param lsParams = ls_param());
#ifdef SDCARD_SORT_ALPHA
  void flush_presort();
#endif
};
extern bool Stopped;
extern CardReader card;
#define IS_SD_PRINTING (card.sdprinting)

#if (SDCARDDETECT > -1)
# ifdef SDCARDDETECTINVERTED 
#  define IS_SD_INSERTED (READ(SDCARDDETECT)!=0)
# else
#  define IS_SD_INSERTED (READ(SDCARDDETECT)==0)
# endif //SDCARDTETECTINVERTED
#else
//If we don't have a card detect line, aways asume the card is inserted
# define IS_SD_INSERTED true
#endif

#else

#define IS_SD_PRINTING (false)

#endif //SDSUPPORT
#endif
