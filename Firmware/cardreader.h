#ifndef CARDREADER_H
#define CARDREADER_H

#ifdef SDSUPPORT

#define MAX_DIR_DEPTH 10

#include "SdFile.h"
enum LsAction {LS_SerialPrint,LS_Count,LS_GetFilename};

class CardReader
{
public:
  CardReader();
  
  void initsd();
  void write_command(char *buf);
  void write_command_no_newline(char *buf);
  //files auto[0-9].g on the sd card are performed in a row
  //this is to delay autostart and hence the initialisaiton of the sd card to some seconds after the normal init, so the device is available quick after a reset

  void checkautostart(bool x); 
  void openFile(char* name,bool read,bool replace_current=true);
  void openLogFile(char* name);
  void removeFile(char* name);
  void closefile(bool store_location=false);
  void release();
  void startFileprint();
  void pauseSDPrint();
  uint32_t getFileSize();
  void getStatus();
  void printingHasFinished();

  void getfilename(uint16_t nr, const char* const match=NULL);
  void getfilename_simple(uint32_t position, const char * const match=NULL);
  uint16_t getnrfilenames();
  
  void getAbsFilename(char *t);
  

  void ls();
  void chdir(const char * relpath);
  void updir();
  void setroot();

  #ifdef SDCARD_SORT_ALPHA
	void presort();
  #ifdef SDSORT_QUICKSORT
	void swap(uint8_t left, uint8_t right);
	void quicksort(uint8_t left, uint8_t right);
  #endif //SDSORT_QUICKSORT
	void getfilename_sorted(const uint16_t nr);
	#if SDSORT_GCODE
	  FORCE_INLINE void setSortOn(bool b) { sort_alpha = b; presort(); }
	  FORCE_INLINE void setSortFolders(int i) { sort_folders = i; presort(); }
	  //FORCE_INLINE void setSortReverse(bool b) { sort_reverse = b; }
	#endif
  #endif

  FORCE_INLINE bool isFileOpen() { return file.isOpen(); }
  FORCE_INLINE bool eof() { return sdpos>=filesize ;};
  FORCE_INLINE int16_t get() {  sdpos = file.curPosition();return (int16_t)file.read();};
  FORCE_INLINE void setIndex(long index) {sdpos = index;file.seekSet(index);};
  FORCE_INLINE uint8_t percentDone(){if(!isFileOpen()) return 0; if(filesize) return sdpos/((filesize+99)/100); else return 0;};
  FORCE_INLINE char* getWorkDirName(){workDir.getFilename(filename);return filename;};

  bool ToshibaFlashAir_isEnabled() const { return card.getFlashAirCompatible(); }
  void ToshibaFlashAir_enable(bool enable) { card.setFlashAirCompatible(enable); }
  bool ToshibaFlashAir_GetIP(uint8_t *ip);

public:
  bool saving;
  bool logging;
  bool sdprinting ;  
  bool cardOK ;
  char filename[13];
  uint16_t creationTime, creationDate;
  uint32_t cluster, position;
  char longFilename[LONG_FILENAME_LENGTH];
  bool filenameIsDir;
  int lastnr; //last number of the autostart;
private:
  SdFile root,*curDir,workDir,workDirParents[MAX_DIR_DEPTH];
  uint16_t workDirDepth;

  // Sort files and folders alphabetically.
#ifdef SDCARD_SORT_ALPHA
  uint16_t sort_count;        // Count of sorted items in the current directory
  #if SDSORT_GCODE
    bool sort_alpha;          // Flag to enable / disable the feature
    int sort_folders;         // Flag to enable / disable folder sorting
							//bool sort_reverse;      // Flag to enable / disable reverse sorting
  #endif

							// By default the sort index is static
  #if SDSORT_DYNAMIC_RAM
    uint8_t *sort_order;
  #else
    uint8_t sort_order[SDSORT_LIMIT];
  #endif
  // Cache filenames to speed up SD menus.
  #if SDSORT_USES_RAM

    // If using dynamic ram for names, allocate on the heap.
    #if SDSORT_CACHE_NAMES
      #if SDSORT_DYNAMIC_RAM
        char **sortshort, **sortnames;
      #else
        char sortshort[SDSORT_LIMIT][FILENAME_LENGTH];
        char sortnames[SDSORT_LIMIT][FILENAME_LENGTH];
      #endif
    #elif !SDSORT_USES_STACK
      char sortnames[SDSORT_LIMIT][FILENAME_LENGTH];
	  uint16_t creation_time[SDSORT_LIMIT];
	  uint16_t creation_date[SDSORT_LIMIT];
    #endif

    // Folder sorting uses an isDir array when caching items.
    #if HAS_FOLDER_SORTING
      #if SDSORT_DYNAMIC_RAM
        uint8_t *isDir;
      #elif (SDSORT_CACHE_NAMES) || !(SDSORT_USES_STACK)
        uint8_t isDir[(SDSORT_LIMIT + 7) >> 3];
      #endif
    #endif

  #endif // SDSORT_USES_RAM

#endif // SDCARD_SORT_ALPHA


  Sd2Card card;
  SdVolume volume;
  SdFile file;
  #define SD_PROCEDURE_DEPTH 1
  #define MAXPATHNAMELENGTH (13*MAX_DIR_DEPTH+MAX_DIR_DEPTH+1)
  uint8_t file_subcall_ctr;
  uint32_t filespos[SD_PROCEDURE_DEPTH];
  char filenames[SD_PROCEDURE_DEPTH][MAXPATHNAMELENGTH];
  uint32_t filesize;
  //int16_t n;
  unsigned long autostart_atmillis;
  uint32_t sdpos ;

  bool autostart_stilltocheck; //the sd start is delayed, because otherwise the serial cannot answer fast enought to make contact with the hostsoftware.
  
  LsAction lsAction; //stored for recursion.
  int16_t nrFiles; //counter for the files in the current directory and recycled as position counter for getting the nrFiles'th name in the directory.
  char* diveDirName;
  void lsDive(const char *prepend, SdFile parent, const char * const match=NULL);
  #ifdef SDCARD_SORT_ALPHA
    void flush_presort();
  #endif
};
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
