#include "Marlin.h"
#include "cmdqueue.h"
#include "cardreader.h"
#include "ultralcd.h"
#include "menu.h"
#include "stepper.h"
#include "temperature.h"
#include "language.h"
#include "Prusa_farm.h"

#ifdef SDSUPPORT

#define LONGEST_FILENAME (longFilename[0] ? longFilename : filename)

CardReader::CardReader()
{

   #ifdef SDCARD_SORT_ALPHA
     sort_count = 0;
   #endif

   filesize = 0;
   sdpos = 0;
   sdprinting = false;
   cardOK = false;
   saving = false;
   logging = false;
   workDirDepth = 0;
   file_subcall_ctr=0;
   memset(workDirParents, 0, sizeof(workDirParents));
   presort_flag = false;

   lastnr=0;
  //power to SD reader
  #if SDPOWER > -1
    SET_OUTPUT(SDPOWER); 
    WRITE(SDPOWER,HIGH);
  #endif //SDPOWER
  
  autostart_atmillis.start(); // reset timer
}

char *createFilename(char *buffer,const dir_t &p) //buffer>12characters
{
  char *pos=buffer;
  for (uint8_t i = 0; i < 11; i++) 
  {
    if (p.name[i] == ' ')continue;
    if (i == 8) 
    {
      *pos++='.';
    }
    *pos++=p.name[i];
  }
  *pos++=0;
  return buffer;
}

/**
+* Dive into a folder and recurse depth-first to perform a pre-set operation lsAction:
+*   LS_Count           - Add +1 to nrFiles for every file within the parent
+*   LS_GetFilename     - Get the filename of the file indexed by nrFiles
+*   LS_SerialPrint     - Print the full path and size of each file to serial output
+*/

void CardReader::lsDive(const char *prepend, SdFile parent, const char * const match/*=NULL*/, LsAction lsAction, ls_param lsParams) {
	static uint8_t recursionCnt = 0;
	// RAII incrementer for the recursionCnt
	class _incrementer
	{
		public:
		_incrementer() {recursionCnt++;}
		~_incrementer() {recursionCnt--;}
	} recursionCntIncrementer;
	
	dir_t p;
	uint8_t cnt = 0;
		// Read the next entry from a directory
		for (position = parent.curPosition(); parent.readDir(p, longFilename) > 0; position = parent.curPosition()) {
			if (recursionCnt > MAX_DIR_DEPTH)
				return;
			uint8_t pn0 = p.name[0];
			if (pn0 == DIR_NAME_FREE) break;
			if (pn0 == DIR_NAME_DELETED || pn0 == '.') continue;
			if (longFilename[0] == '.') continue;
			if (!DIR_IS_FILE_OR_SUBDIR(&p) || (p.attributes & DIR_ATT_HIDDEN)) continue;
			if (DIR_IS_SUBDIR(&p) && lsAction == LS_SerialPrint) { // If the entry is a directory and the action is LS_SerialPrint
				// Get the short name for the item, which we know is a folder
				char lfilename[FILENAME_LENGTH];
				createFilename(lfilename, p);
				// Allocate enough stack space for the full path to a folder, trailing slash, and nul
				bool prepend_is_empty = (prepend[0] == '\0');
				int len = (prepend_is_empty ? 1 : strlen(prepend)) + strlen(lfilename) + 1 + 1;
				char path[len];
				// Append the FOLDERNAME12/ to the passed string.
				// It contains the full path to the "parent" argument.
				// We now have the full path to the item in this folder.
				strcpy(path, prepend_is_empty ? "/" : prepend); // root slash if prepend is empty
				strcat(path, lfilename); // FILENAME_LENGTH-1 characters maximum
				strcat(path, "/");       // 1 character
				// Serial.print(path);
				// Get a new directory object using the full path
				// and dive recursively into it.
				
				if (lsParams.LFN)
					printf_P(PSTR("DIR_ENTER: %s \"%s\"\n"), path, longFilename[0] ? longFilename : lfilename);
				
				SdFile dir;
				if (!dir.open(parent, lfilename, O_READ)) {
					//SERIAL_ECHO_START();
					//SERIAL_ECHOPGM(_i("Cannot open subdir"));////MSG_SD_CANT_OPEN_SUBDIR
					//SERIAL_ECHOLN(lfilename);
				}
				lsDive(path, dir, NULL, lsAction, lsParams);
				// close() is done automatically by destructor of SdFile
				
				if (lsParams.LFN)
					puts_P(PSTR("DIR_EXIT"));
			}
			else {
				filenameIsDir = DIR_IS_SUBDIR(&p);
				if (!filenameIsDir && (p.name[8] != 'G' || p.name[9] == '~')) continue;
				switch (lsAction) {
					case LS_Count:
						nrFiles++;
						break;
					
					case LS_SerialPrint:
						createFilename(filename, p);
						SERIAL_PROTOCOL(prepend);
						SERIAL_PROTOCOL(filename);
						
						MYSERIAL.write(' ');
						SERIAL_PROTOCOL(p.fileSize);
						
						if (lsParams.timestamp)
						{
							crmodDate = p.lastWriteDate;
							crmodTime = p.lastWriteTime;
							if( crmodDate < p.creationDate || ( crmodDate == p.creationDate && crmodTime < p.creationTime ) ){
								crmodDate = p.creationDate;
								crmodTime = p.creationTime;
							}
							printf_P(PSTR(" %#lx"), ((uint32_t)crmodDate << 16) | crmodTime);
						}
						
						if (lsParams.LFN)
							printf_P(PSTR(" \"%s\""), LONGEST_FILENAME);
						
						SERIAL_PROTOCOLLN();
						manage_heater();
						break;
				
					case LS_GetFilename:
						//SERIAL_ECHOPGM("File: ");
						createFilename(filename, p);
						// cluster = parent.curCluster();
						// position = parent.curPosition();
						/*MYSERIAL.println(filename);
						SERIAL_ECHOPGM("Write date: ");
						writeDate = p.lastWriteDate;
						MYSERIAL.println(writeDate);
						writeTime = p.lastWriteTime;
						SERIAL_ECHOPGM("Creation date: ");
						MYSERIAL.println(p.creationDate);
						SERIAL_ECHOPGM("Access date: ");
						MYSERIAL.println(p.lastAccessDate);
						SERIAL_ECHOLNPGM("");*/
						crmodDate = p.lastWriteDate;
						crmodTime = p.lastWriteTime;
						// There are scenarios when simple modification time is not enough (on MS Windows)
						// For example - extract an old g-code from an archive onto the SD card.
						// In such case the creation time is current time (which is correct), but the modification time
						// stays the same - i.e. old.
						// Therefore let's pick the most recent timestamp from both creation and modification timestamps
						if( crmodDate < p.creationDate || ( crmodDate == p.creationDate && crmodTime < p.creationTime ) ){
							crmodDate = p.creationDate;
							crmodTime = p.creationTime;
						}
						//writeDate = p.lastAccessDate;
						if (match != NULL) {
							if (strcasecmp(match, filename) == 0) return;
						}
						else if (cnt == nrFiles) return;
						cnt++;
						break;
				}
			}
		} // while readDir
}

void CardReader::ls(ls_param params)
{
  root.rewind();
  lsDive("",root, NULL, LS_SerialPrint, params);
}


void CardReader::initsd(bool doPresort/* = true*/)
{
  cardOK = false;
  if(root.isOpen())
    root.close();
#ifdef SDSLOW
  if (!card.init(SPI_HALF_SPEED)
    )
#else
  if (!card.init(SPI_FULL_SPEED)
    )
#endif
  {
    SERIAL_ECHO_START;
    SERIAL_ECHOLNRPGM(_n("SD init fail"));////MSG_SD_INIT_FAIL
  }
  else if (!volume.init(&card))
  {
    SERIAL_ERROR_START;
    SERIAL_ERRORLNRPGM(_n("volume.init failed"));////MSG_SD_VOL_INIT_FAIL
  }
  else if (!root.openRoot(&volume)) 
  {
    SERIAL_ERROR_START;
    SERIAL_ERRORLNRPGM(_n("openRoot failed"));////MSG_SD_OPENROOT_FAIL
  }
  else 
  {
    cardOK = true;
    SERIAL_ECHO_START;
    SERIAL_ECHOLNRPGM(_n("SD card ok"));////MSG_SD_CARD_OK
  }
  workDir=root;
  curDir=&root;
  workDirDepth = 0;

  #ifdef SDCARD_SORT_ALPHA
  if (doPresort)
    presort();
  #endif

  /*
  if(!workDir.openRoot(&volume))
  {
    SERIAL_ECHOLNPGM(MSG_SD_WORKDIR_FAIL);
  }
  */
  
}

void CardReader::setroot(bool doPresort)
{
  workDir=root;
  workDirDepth = 0;
  
  curDir=&workDir;
#ifdef SDCARD_SORT_ALPHA
	if (doPresort)
		presort();
	else
		presort_flag = true;
#endif
}
void CardReader::release()
{
  sdprinting = false;
  cardOK = false;
  SERIAL_ECHO_START;
  SERIAL_ECHOLNRPGM(_n("SD card released"));////MSG_SD_CARD_RELEASED
}

void CardReader::startFileprint()
{
  if(cardOK)
  {
    sdprinting = true;
	#ifdef SDCARD_SORT_ALPHA
		//flush_presort();
	#endif
  }
}

void CardReader::openLogFile(const char* name)
{
  logging = true;
  openFileWrite(name);
}

void CardReader::getDirName(char* name, uint8_t level)
{	
		workDirParents[level].getFilename(name);
}

uint8_t CardReader::getWorkDirDepth() {
	return workDirDepth;
}

void CardReader::getAbsFilename(char *t)
{
  uint8_t cnt=0;
  *t='/';t++;cnt++;
  for(uint8_t i=0;i<workDirDepth;i++)
  {
    workDirParents[i].getFilename(t); //SDBaseFile.getfilename!
    while(*t!=0 && cnt< MAXPATHNAMELENGTH) 
    {t++;cnt++;}  //crawl counter forward.
  }
  if(cnt < MAXPATHNAMELENGTH - FILENAME_LENGTH)
    file.getFilename(t);
  else
    t[0]=0;
}

void CardReader::printAbsFilenameFast()
{
    SERIAL_PROTOCOL('/');
    for (uint8_t i = 0; i < getWorkDirDepth(); i++)
    {
        SERIAL_PROTOCOL(dir_names[i]);
        SERIAL_PROTOCOL('/');
    }
    SERIAL_PROTOCOL(LONGEST_FILENAME);
}

/**
 * @brief Dive into subfolder
 *
 * Method sets curDir to point to root, in case fileName is null.
 * Method sets curDir to point to workDir, in case fileName path is relative
 * (doesn't start with '/')
 * Method sets curDir to point to dir, which is specified by absolute path
 * specified by fileName. In such case fileName is updated so it points to
 * file name without the path.
 *
 * @param[in,out] fileName
 *  expects file name including path
 *  in case of absolute path, file name without path is returned
 */
bool CardReader::diveSubfolder (const char *&fileName)
{
    curDir=&root;
    if (!fileName)
        return 1;

    const char *dirname_start, *dirname_end;
    if (fileName[0] == '/') // absolute path
    {
        setroot(false);
        dirname_start = fileName + 1;
        while (*dirname_start)
        {
            dirname_end = strchr(dirname_start, '/');
            //SERIAL_ECHO("start:");SERIAL_ECHOLN((int)(dirname_start-name));
            //SERIAL_ECHO("end  :");SERIAL_ECHOLN((int)(dirname_end-name));
            if (dirname_end && dirname_end > dirname_start)
            {
                const size_t maxLen = 12;
                char subdirname[maxLen+1];
                const size_t len = ((static_cast<size_t>(dirname_end-dirname_start))>maxLen) ? maxLen : (dirname_end-dirname_start);
                strncpy(subdirname, dirname_start, len);
                subdirname[len] = 0;
                if (!chdir(subdirname, false))
                    return 0;

                curDir = &workDir;
                dirname_start = dirname_end + 1;
            }
            else // the reminder after all /fsa/fdsa/ is the filename
            {
                fileName = dirname_start;
                //SERIAL_ECHOLN("remaider");
                //SERIAL_ECHOLN(fname);
                break;
            }

        }
    }
    else //relative path
    {
        curDir = &workDir;
    }
    return 1;
}

static const char ofKill[] PROGMEM = "trying to call sub-gcode files with too many levels.";
static const char ofSubroutineCallTgt[] PROGMEM = "SUBROUTINE CALL target:\"";
static const char ofParent[] PROGMEM = "\" parent:\"";
static const char ofPos[] PROGMEM = "\" pos";
static const char ofNowDoingFile[] PROGMEM = "Now doing file: ";
static const char ofNowFreshFile[] PROGMEM = "Now fresh file: ";
static const char ofFileOpened[] PROGMEM = "File opened: ";
static const char ofSize[] PROGMEM = " Size: ";
static const char ofFileSelected[] PROGMEM = "File selected";
static const char ofSDPrinting[] PROGMEM = "SD-PRINTING";
static const char ofWritingToFile[] PROGMEM = "Writing to file: ";

void CardReader::openFileReadFilteredGcode(const char* name, bool replace_current/* = false*/){
    if(!cardOK)
        return;
    
    if(file.isOpen()){  //replacing current file by new file, or subfile call
        if(!replace_current){
            if((int)file_subcall_ctr>(int)SD_PROCEDURE_DEPTH-1){
                // SERIAL_ERROR_START;
                // SERIAL_ERRORPGM("trying to call sub-gcode files with too many levels. MAX level is:");
                // SERIAL_ERRORLN(SD_PROCEDURE_DEPTH);
                kill(ofKill, 1);
                return;
            }
            
            SERIAL_ECHO_START;
            SERIAL_ECHORPGM(ofSubroutineCallTgt);
            SERIAL_ECHO(name);
            SERIAL_ECHORPGM(ofParent);
            
            //store current filename and position
            getAbsFilename(filenames[file_subcall_ctr]);
            
            SERIAL_ECHO(filenames[file_subcall_ctr]);
            SERIAL_ECHORPGM(ofPos);
            SERIAL_ECHOLN(sdpos);
            filespos[file_subcall_ctr]=sdpos;
            file_subcall_ctr++;
        } else {
            SERIAL_ECHO_START;
            SERIAL_ECHORPGM(ofNowDoingFile);
            SERIAL_ECHOLN(name);
        }
        file.close();
    } else { //opening fresh file
        file_subcall_ctr=0; //resetting procedure depth in case user cancels print while in procedure
        SERIAL_ECHO_START;
        SERIAL_ECHORPGM(ofNowFreshFile);
        SERIAL_ECHOLN(name);
    }
    sdprinting = false;
  
    const char *fname=name;
    if (!diveSubfolder(fname))
      return;
  
    if (file.openFilteredGcode(curDir, fname)) {
        getfilename(0, fname);
        filesize = file.fileSize();
        SERIAL_PROTOCOLRPGM(ofFileOpened);////MSG_SD_FILE_OPENED
        printAbsFilenameFast();
        SERIAL_PROTOCOLRPGM(ofSize);////MSG_SD_SIZE
        SERIAL_PROTOCOLLN(filesize);
        sdpos = 0;
        
        SERIAL_PROTOCOLLNRPGM(ofFileSelected);////MSG_SD_FILE_SELECTED
        lcd_setstatuspgm(ofFileSelected);
        scrollstuff = 0;
      } else {
        SERIAL_PROTOCOLRPGM(MSG_SD_OPEN_FILE_FAIL);
        SERIAL_PROTOCOL(fname);
        SERIAL_PROTOCOLLN('.');
      }
}

void CardReader::openFileWrite(const char* name)
{
    if(!cardOK)
        return;
    if(file.isOpen()){  //replacing current file by new file, or subfile call
#if 0
        // I doubt chained files support is necessary for file saving:
        // Intentionally disabled because it takes a lot of code size while being not used

        if((int)file_subcall_ctr>(int)SD_PROCEDURE_DEPTH-1){
            // SERIAL_ERROR_START;
            // SERIAL_ERRORPGM("trying to call sub-gcode files with too many levels. MAX level is:");
            // SERIAL_ERRORLN(SD_PROCEDURE_DEPTH);
            kill(ofKill, 1);
            return;
        }
        
        SERIAL_ECHO_START;
        SERIAL_ECHORPGM(ofSubroutineCallTgt);
        SERIAL_ECHO(name);
        SERIAL_ECHORPGM(ofParent);
        
        //store current filename and position
        getAbsFilename(filenames[file_subcall_ctr]);
        
        SERIAL_ECHO(filenames[file_subcall_ctr]);
        SERIAL_ECHORPGM(ofPos);
        SERIAL_ECHOLN(sdpos);
        filespos[file_subcall_ctr]=sdpos;
        file_subcall_ctr++;
        file.close();
#else
        SERIAL_ECHOLNPGM("File already opened");
#endif
    } else { //opening fresh file
        file_subcall_ctr=0; //resetting procedure depth in case user cancels print while in procedure
        SERIAL_ECHO_START;
        SERIAL_ECHORPGM(ofNowFreshFile);
        SERIAL_ECHOLN(name);
    }
    sdprinting = false;
    
    const char *fname=name;
    if (!diveSubfolder(fname))
      return;
    
    //write
    if (!file.open(curDir, fname, O_CREAT | O_APPEND | O_WRITE | O_TRUNC)){
        SERIAL_PROTOCOLRPGM(MSG_SD_OPEN_FILE_FAIL);
        SERIAL_PROTOCOL(fname);
        SERIAL_PROTOCOLLN('.');
    } else {
        saving = true;
        getfilename(0, fname);
        SERIAL_PROTOCOLRPGM(ofWritingToFile);////MSG_SD_WRITE_TO_FILE
        printAbsFilenameFast();
        SERIAL_PROTOCOLLN();
        
        SERIAL_PROTOCOLLNRPGM(ofFileSelected);////MSG_SD_FILE_SELECTED
        lcd_setstatuspgm(ofFileSelected);
        scrollstuff = 0;
    }
}

void CardReader::removeFile(const char* name)
{
    if(!cardOK) return;
    file.close();
    sdprinting = false;

    const char *fname=name;
    if (!diveSubfolder(fname))
      return;

    if (file.remove(curDir, fname)) 
    {
      SERIAL_PROTOCOLPGM("File deleted:");
      SERIAL_PROTOCOLLN(fname);
      sdpos = 0;
	  #ifdef SDCARD_SORT_ALPHA
		  presort();
	  #endif
    }
    else
    {
      SERIAL_PROTOCOLPGM("Deletion failed, File: ");
      SERIAL_PROTOCOL(fname);
      SERIAL_PROTOCOLLN('.');
    }
  
}

uint32_t CardReader::getFileSize()
{
	return filesize;
}

void CardReader::getStatus(bool arg_P)
{
    if (isPrintPaused)
    {
        if (saved_printing && (saved_printing_type == PRINTING_TYPE_SD))
            SERIAL_PROTOCOLLNPGM("SD print paused");
        else
            SERIAL_PROTOCOLLNPGM("Print saved");
    }
    else if (sdprinting)
    {
        if (arg_P)
        {
            printAbsFilenameFast();
            SERIAL_PROTOCOLLN();
        }
        else
            SERIAL_PROTOCOLLN(LONGEST_FILENAME);
        
        SERIAL_PROTOCOLRPGM(_N("SD printing byte "));////MSG_SD_PRINTING_BYTE
        SERIAL_PROTOCOL(sdpos);
        SERIAL_PROTOCOL('/');
        SERIAL_PROTOCOLLN(filesize);
        uint16_t time = ( _millis() - starttime ) / 60000U;
        SERIAL_PROTOCOL((int)(time / 60));
        SERIAL_PROTOCOL(':');
        SERIAL_PROTOCOLLN((int)(time % 60));
    }
    else
        SERIAL_PROTOCOLLNPGM("Not SD printing");
}
void CardReader::write_command(char *buf)
{
  file.writeError = false;
  file.write(buf); //write command
  file.write("\r\n"); //write line termination
  if (file.writeError)
  {
    SERIAL_ERROR_START;
    SERIAL_ERRORLNRPGM(MSG_SD_ERR_WRITE_TO_FILE);
  }
}

#define CHUNK_SIZE 64

void CardReader::write_command_no_newline(char *buf)
{
  file.write(buf, CHUNK_SIZE);
  if (file.writeError)
  {
    SERIAL_ERROR_START;
    SERIAL_ERRORLNRPGM(MSG_SD_ERR_WRITE_TO_FILE);
    SERIAL_PROTOCOLLNPGM("An error while writing to the SD Card.");
  }
}


void CardReader::checkautostart(bool force)
{
  // The SD start is delayed because otherwise the serial cannot answer
  // fast enough to make contact with the host software.
  static bool autostart_stilltocheck = true; 
  if(!force)
  {
    if(!autostart_stilltocheck)
      return;
    if(autostart_atmillis.expired(5000))
      return;
  }
  autostart_stilltocheck = false;
  if(!cardOK)
  {
    initsd();
    if(!cardOK) //fail
      return;
  }
  
  char autoname[30];
  sprintf_P(autoname, PSTR("auto%i.g"), lastnr);
  for(int8_t i=0;i<(int8_t)strlen(autoname);i++)
    autoname[i]=tolower(autoname[i]);
  dir_t p;

  root.rewind();
  
  bool found=false;
  while (root.readDir(p, NULL) > 0) 
  {
    for(int8_t i=0;i<(int8_t)strlen((char*)p.name);i++)
    p.name[i]=tolower(p.name[i]);
    //Serial.print((char*)p.name);
    //Serial.print(" ");
    //Serial.println(autoname);
    if(p.name[9]!='~') //skip safety copies
    if(strncmp((char*)p.name,autoname,5)==0)
    {
      char cmd[30];
      // M23: Select SD file
      sprintf_P(cmd, PSTR("M23 %s"), autoname);
      enquecommand(cmd);
      // M24: Start/resume SD print
      enquecommand_P(PSTR("M24"));
      found=true;
    }
  }
  if(!found)
    lastnr=-1;
  else
    lastnr++;
}

void CardReader::closefile(bool store_location)
{
  file.sync();
  file.close();
  saving = false; 
  logging = false;
  
  if(store_location)
  {
    //future: store printer state, filename and position for continuing a stopped print
    // so one can unplug the printer and continue printing the next day.
    
  }

  
}

void CardReader::getfilename(uint16_t nr, const char * const match/*=NULL*/)
{
  curDir=&workDir;
  nrFiles=nr;
  curDir->rewind();
  lsDive("",*curDir,match, LS_GetFilename);
  
}

void CardReader::getfilename_simple(uint16_t entry, const char * const match/*=NULL*/)
{
	curDir = &workDir;
	nrFiles = 0;
	curDir->seekSet((uint32_t)entry << 5);
	lsDive("", *curDir, match, LS_GetFilename);
}

void CardReader::getfilename_next(uint32_t position, const char * const match/*=NULL*/)
{
	curDir = &workDir;
	nrFiles = 1;
	curDir->seekSet(position);
	lsDive("", *curDir, match, LS_GetFilename);
}

uint16_t CardReader::getnrfilenames()
{
  curDir=&workDir;
  nrFiles=0;
  curDir->rewind();
  lsDive("",*curDir, NULL, LS_Count);
  //SERIAL_ECHOLN(nrFiles);
  return nrFiles;
}

bool CardReader::chdir(const char * relpath, bool doPresort)
{
  SdFile newfile;
  SdFile *parent=&root;
  
  if(workDir.isOpen())
    parent=&workDir;
  
  if(!newfile.open(*parent,relpath, O_READ) || ((workDirDepth + 1) >= MAX_DIR_DEPTH))
  {
   SERIAL_ECHO_START;
   SERIAL_ECHORPGM(_n("Cannot enter subdir: "));////MSG_SD_CANT_ENTER_SUBDIR
   SERIAL_ECHOLN(relpath);
   return 0;
  }
  else
  {
    strcpy(dir_names[workDirDepth], relpath);
    puts(relpath);

    if (workDirDepth < MAX_DIR_DEPTH) {
      for (uint8_t d = ++workDirDepth; d--;)
        workDirParents[d+1] = workDirParents[d];
      workDirParents[0]=*parent;
    }
    workDir=newfile;

#ifdef SDCARD_SORT_ALPHA
	if (doPresort)
		presort();
	else
		presort_flag = true;
#endif
	return 1;
  }
}

void CardReader::updir()
{
  if(workDirDepth > 0)
  {
    --workDirDepth;
    workDir = workDirParents[0];
    for (uint8_t d = 0; d < workDirDepth; d++)
    {
        workDirParents[d] = workDirParents[d+1];
    }
	#ifdef SDCARD_SORT_ALPHA
    presort();
	#endif
  }
}

#ifdef SDCARD_SORT_ALPHA

/**
* Get the name of a file in the current directory by sort-index
*/
void CardReader::getfilename_sorted(const uint16_t nr, uint8_t sdSort) {
    if (nr < sort_count)
        getfilename_simple(sort_entries[(sdSort == SD_SORT_ALPHA) ? (sort_count - nr - 1) : nr]);
    else
        getfilename_afterMaxSorting(nr);
}

void CardReader::getfilename_afterMaxSorting(uint16_t entry, const char * const match/*=NULL*/)
{
	curDir = &workDir;
	nrFiles = entry - sort_count + 1;
	curDir->seekSet(lastSortedFilePosition << 5);
	lsDive("", *curDir, match, LS_GetFilename);
}

/**
* Read all the files and produce a sort key
*
* We can do this in 3 ways...
*  - Minimal RAM: Read two filenames at a time sorting along...
*  - Some RAM: Buffer the directory just for this sort
*  - Most RAM: Buffer the directory and return filenames from RAM
*/
void CardReader::presort() {
	// Throw away old sort index
	flush_presort();
	
	if (IS_SD_INSERTED == false) return; //sorting is not used in farm mode
	uint8_t sdSort = eeprom_read_byte((uint8_t*)EEPROM_SD_SORT);

	KEEPALIVE_STATE(IN_HANDLER);

	// If there are files, sort up to the limit
	uint16_t fileCnt = getnrfilenames();
	if (fileCnt > 0) {
		// Never sort more than the max allowed
		// If you use folders to organize, 20 may be enough
		if (fileCnt > SDSORT_LIMIT) {
			if ((sdSort != SD_SORT_NONE) && !farm_mode) {
				lcd_show_fullscreen_message_and_wait_P(_i("Some files will not be sorted. Max. No. of files in 1 folder for sorting is 100."));////MSG_FILE_CNT c=20 r=6
			}
			fileCnt = SDSORT_LIMIT;
		}

		sort_count = fileCnt;
		
		// Init sort order.
		for (uint16_t i = 0; i < fileCnt; i++) {
			if (!IS_SD_INSERTED) return;
			manage_heater();
			if (i == 0)
				getfilename(0);
			else
				getfilename_next(position);
			sort_entries[i] = position >> 5;
		}

		if ((fileCnt > 1) && (sdSort != SD_SORT_NONE) && !farm_mode) {

#ifdef SORTING_SPEEDTEST
			LongTimer sortingSpeedtestTimer;
			sortingSpeedtestTimer.start();
#endif //SORTING_SPEEDTEST
			lastSortedFilePosition = position >> 5;

			// By default re-read the names from SD for every compare
			// retaining only two filenames at a time. This is very
			// slow but is safest and uses minimal RAM.
			char name1[LONG_FILENAME_LENGTH];
			uint16_t crmod_time_bckp;
			uint16_t crmod_date_bckp;

#ifdef INSERTSORT

#define _SORT_CMP_NODIR() (strcasecmp(name1, name2) < 0) //true if lowercase(name1) < lowercase(name2)
#define _SORT_CMP_TIME_NODIR() (((crmod_date_bckp == crmodDate) && (crmod_time_bckp > crmodTime)) || (crmod_date_bckp > crmodDate))

#if HAS_FOLDER_SORTING
#define _SORT_CMP_DIR(fs) ((dir1 == filenameIsDir) ? _SORT_CMP_NODIR() : (fs < 0 ? dir1 : !dir1))
#define _SORT_CMP_TIME_DIR(fs) ((dir1 == filenameIsDir) ? _SORT_CMP_TIME_NODIR() : (fs < 0 ? dir1 : !dir1))
#endif

      uint16_t counter = 0;
      menu_progressbar_init(fileCnt * fileCnt / 2, _T(MSG_SORTING_FILES));

      for (uint16_t i = 1; i < fileCnt; ++i){
        // if (!IS_SD_INSERTED) return;
        menu_progressbar_update(counter);
        counter += i;

        /// pop the position
        const uint16_t o1 = sort_entries[i];
        getfilename_simple(o1);
        strcpy(name1, LONGEST_FILENAME); // save (or getfilename below will trounce it)
        crmod_date_bckp = crmodDate;
        crmod_time_bckp = crmodTime;
        #if HAS_FOLDER_SORTING
        bool dir1 = filenameIsDir;
        #endif

        /// find proper place
        uint16_t j = i;
        for (; j > 0; --j){
          if (!IS_SD_INSERTED) return;
          
          #ifdef SORTING_DUMP
          for (uint16_t z = 0; z < fileCnt; z++){
            printf_P(PSTR("%2u "), sort_entries[z]);
          }
          MYSERIAL.println();
          #endif
          
          manage_heater();
          const uint16_t o2 = sort_entries[j - 1];

          getfilename_simple(o2);
          char *name2 = LONGEST_FILENAME; // use the string in-place

          // Sort the current pair according to settings.
          if (
          #if HAS_FOLDER_SORTING
            (sdSort == SD_SORT_TIME && _SORT_CMP_TIME_DIR(FOLDER_SORTING)) || (sdSort == SD_SORT_ALPHA && !_SORT_CMP_DIR(FOLDER_SORTING))
          #else
            (sdSort == SD_SORT_TIME && _SORT_CMP_TIME_NODIR()) || (sdSort == SD_SORT_ALPHA && !_SORT_CMP_NODIR())
          #endif
            )
          {
            break;
          } else {
            #ifdef SORTING_DUMP
            puts_P(PSTR("shift"));
            #endif            
            sort_entries[j] = o2;
          }
        }
        /// place the position
        sort_entries[j] = o1;
      }

#else //Bubble Sort

#define _SORT_CMP_NODIR() (strcasecmp(name1, name2) < 0) //true if lowercase(name1) < lowercase(name2)
#define _SORT_CMP_TIME_NODIR() (((crmod_date_bckp == crmodDate) && (crmod_time_bckp > crmodTime)) || (crmod_date_bckp > crmodDate))

#if HAS_FOLDER_SORTING
#define _SORT_CMP_DIR(fs) ((dir1 == filenameIsDir) ? _SORT_CMP_NODIR() : (fs < 0 ? dir1 : !dir1))
#define _SORT_CMP_TIME_DIR(fs) ((dir1 == filenameIsDir) ? _SORT_CMP_TIME_NODIR() : (fs < 0 ? dir1 : !dir1))
#endif

			uint16_t counter = 0;
			menu_progressbar_init(0.5*(fileCnt - 1)*(fileCnt), _T(MSG_SORTING_FILES));

			for (uint16_t i = fileCnt; --i;) {
				if (!IS_SD_INSERTED) return;
				bool didSwap = false;

				menu_progressbar_update(counter);
				counter++;

				for (uint16_t j = 0; j < i; ++j) {
					if (!IS_SD_INSERTED) return;
					#ifdef SORTING_DUMP
					for (uint16_t z = 0; z < fileCnt; z++)
					{
						printf_P(PSTR("%2u "), sort_entries[z]);
					}
					MYSERIAL.println();
					#endif
					manage_heater();
					const uint16_t o1 = sort_entries[j], o2 = sort_entries[j + 1];

					counter++;
					getfilename_simple(o1);
					strcpy(name1, LONGEST_FILENAME); // save (or getfilename below will trounce it)
					crmod_date_bckp = crmodDate;
					crmod_time_bckp = crmodTime;
					#if HAS_FOLDER_SORTING
					bool dir1 = filenameIsDir;
					#endif
					getfilename_simple(o2);
					char *name2 = LONGEST_FILENAME; // use the string in-place

													// Sort the current pair according to settings.
					if (
					#if HAS_FOLDER_SORTING
						(sdSort == SD_SORT_TIME && _SORT_CMP_TIME_DIR(FOLDER_SORTING)) || (sdSort == SD_SORT_ALPHA && !_SORT_CMP_DIR(FOLDER_SORTING))
					#else
						(sdSort == SD_SORT_TIME && _SORT_CMP_TIME_NODIR()) || (sdSort == SD_SORT_ALPHA && !_SORT_CMP_NODIR())
					#endif
						)
					{
						#ifdef SORTING_DUMP
						puts_P(PSTR("swap"));
						#endif
						
						sort_entries[j] = o2;
						sort_entries[j + 1] = o1;
						didSwap = true;
					}
				}
				if (!didSwap) break;
			} //end of bubble sort loop
#endif
			
#ifdef SORTING_SPEEDTEST
			printf_P(PSTR("sortingSpeedtestTimer:%lu\n"), sortingSpeedtestTimer.elapsed());
#endif //SORTING_SPEEDTEST
			
			#ifdef SORTING_DUMP
			for (uint16_t z = 0; z < fileCnt; z++)
				printf_P(PSTR("%2u "), sort_entries[z]);
			SERIAL_PROTOCOLLN();
			#endif

			menu_progressbar_finish();
		}
	}

	KEEPALIVE_STATE(NOT_BUSY);
}

void CardReader::flush_presort() {
	sort_count = 0;
	lastSortedFilePosition = 0;
}

#endif // SDCARD_SORT_ALPHA



void CardReader::printingHasFinished()
{
    st_synchronize();
    file.close();

    if(file_subcall_ctr>0) //heading up to a parent file that called current as a procedure.
    {
      file_subcall_ctr--;
      openFileReadFilteredGcode(filenames[file_subcall_ctr],true);
      setIndex(filespos[file_subcall_ctr]);
      startFileprint();
    }
    else
    {
      sdprinting = false;
      if(SD_FINISHED_STEPPERRELEASE)
      {
          finishAndDisableSteppers();
          //enquecommand_P(PSTR(SD_FINISHED_RELEASECOMMAND));
      }
      autotempShutdown();
	  #ifdef SDCARD_SORT_ALPHA
		  //presort();
	  #endif
    }
}

bool CardReader::ToshibaFlashAir_GetIP(uint8_t *ip)
{
    memset(ip, 0, 4);
    return card.readExtMemory(1, 1, 0x400+0x150, 4, ip);
}

#endif //SDSUPPORT
