#include "Marlin.h"
#include "cardreader.h"
#include "ultralcd.h"
#include "stepper.h"
#include "temperature.h"
#include "language.h"

#ifdef SDSUPPORT

#define LONGEST_FILENAME (longFilename[0] ? longFilename : filename)

CardReader::CardReader()
{

   #ifdef SDCARD_SORT_ALPHA
     sort_count = 0;
     #if SDSORT_GCODE
       sort_alpha = true;
     sort_folders = FOLDER_SORTING;
     //sort_reverse = false;
     #endif
   #endif

   filesize = 0;
   sdpos = 0;
   sdprinting = false;
   cardOK = false;
   saving = false;
   logging = false;
   autostart_atmillis=0;
   workDirDepth = 0;
   file_subcall_ctr=0;
   memset(workDirParents, 0, sizeof(workDirParents));

   autostart_stilltocheck=true; //the SD start is delayed, because otherwise the serial cannot answer fast enough to make contact with the host software.
   lastnr=0;
  //power to SD reader
  #if SDPOWER > -1
    SET_OUTPUT(SDPOWER); 
    WRITE(SDPOWER,HIGH);
  #endif //SDPOWER
  
  autostart_atmillis=_millis()+5000;
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
+*   LS_Count       - Add +1 to nrFiles for every file within the parent
+*   LS_GetFilename - Get the filename of the file indexed by nrFiles
+*   LS_SerialPrint - Print the full path and size of each file to serial output
+*/

void CardReader::lsDive(const char *prepend, SdFile parent, const char * const match/*=NULL*/) {
	dir_t p;
	uint8_t cnt = 0;
		// Read the next entry from a directory
		while (parent.readDir(p, longFilename) > 0) {
			// If the entry is a directory and the action is LS_SerialPrint
			if (DIR_IS_SUBDIR(&p) && lsAction != LS_Count && lsAction != LS_GetFilename) {
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
				SdFile dir;
				if (!dir.open(parent, lfilename, O_READ)) {
					if (lsAction == LS_SerialPrint) {
						//SERIAL_ECHO_START();
						//SERIAL_ECHOPGM(_i("Cannot open subdir"));////MSG_SD_CANT_OPEN_SUBDIR
						//SERIAL_ECHOLN(lfilename);
					}
				}
				lsDive(path, dir);
				// close() is done automatically by destructor of SdFile
			}
			else {
				uint8_t pn0 = p.name[0];
				if (pn0 == DIR_NAME_FREE) break;
				if (pn0 == DIR_NAME_DELETED || pn0 == '.') continue;
				if (longFilename[0] == '.') continue;
				if (!DIR_IS_FILE_OR_SUBDIR(&p) || (p.attributes & DIR_ATT_HIDDEN)) continue;
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
						SERIAL_PROTOCOLLN(p.fileSize);
						break;
				
					case LS_GetFilename:
						//SERIAL_ECHOPGM("File: ");				
						createFilename(filename, p);
						cluster = parent.curCluster();
						position = parent.curPosition();
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
						modificationDate = p.lastWriteDate;
						modificationTime = p.lastWriteTime;
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

void CardReader::ls() 
{
  lsAction=LS_SerialPrint;
  //if(lsAction==LS_Count)
  //nrFiles=0;

  root.rewind();
  lsDive("",root);
}


void CardReader::initsd()
{
  cardOK = false;
  if(root.isOpen())
    root.close();
#ifdef SDSLOW
  if (!card.init(SPI_HALF_SPEED,SDSS)
  #if defined(LCD_SDSS) && (LCD_SDSS != SDSS)
    && !card.init(SPI_HALF_SPEED,LCD_SDSS)
  #endif
    )
#else
  if (!card.init(SPI_FULL_SPEED,SDSS)
  #if defined(LCD_SDSS) && (LCD_SDSS != SDSS)
    && !card.init(SPI_FULL_SPEED,LCD_SDSS)
  #endif
    )
#endif
  {
    //if (!card.init(SPI_HALF_SPEED,SDSS))
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
	presort();
  #endif

  /*
  if(!workDir.openRoot(&volume))
  {
    SERIAL_ECHOLNPGM(MSG_SD_WORKDIR_FAIL);
  }
  */
  
}

void CardReader::setroot()
{
  /*if(!workDir.openRoot(&volume))
  {
    SERIAL_ECHOLNPGM(MSG_SD_WORKDIR_FAIL);
  }*/
  workDir=root;
  
  curDir=&workDir;
  #ifdef SDCARD_SORT_ALPHA
	  presort();
  #endif
}
void CardReader::release()
{
  sdprinting = false;
  cardOK = false;
}

void CardReader::startFileprint()
{
  if(cardOK)
  {
    sdprinting = true;
    Stopped = false;
	#ifdef SDCARD_SORT_ALPHA
		//flush_presort();
	#endif
  }
}

void CardReader::openLogFile(const char* name)
{
  logging = true;
  openFile(name, false);
}

void CardReader::getDirName(char* name, uint8_t level)
{	
		workDirParents[level].getFilename(name);
}

uint16_t CardReader::getWorkDirDepth() {
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
  if(cnt<MAXPATHNAMELENGTH-13)
    file.getFilename(t);
  else
    t[0]=0;
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
 * @param[in,out] dir SdFile object to operate with,
 *  in case of absolute path, curDir is modified to point to dir,
 *  so it is not possible to create on stack inside this function,
 *  as curDir would point to destroyed object.
 */
void CardReader::diveSubfolder (const char *fileName, SdFile& dir)
{
    curDir=&root;
    if (!fileName) return;

    const char *dirname_start, *dirname_end;
    if (fileName[0] == '/') // absolute path
    {
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
                subdirname[maxLen] = 0;
                const size_t len = ((static_cast<size_t>(dirname_end-dirname_start))>maxLen) ? maxLen : (dirname_end-dirname_start);
                strncpy(subdirname, dirname_start, len);
                SERIAL_ECHOLN(subdirname);
                if (!dir.open(curDir, subdirname, O_READ))
                {
                    SERIAL_PROTOCOLRPGM(MSG_SD_OPEN_FILE_FAIL);
                    SERIAL_PROTOCOL(subdirname);
                    SERIAL_PROTOCOLLNPGM(".");
                    return;
                }
                else
                {
                    //SERIAL_ECHOLN("dive ok");
                }

                curDir = &dir;
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
}

void CardReader::openFile(const char* name,bool read, bool replace_current/*=true*/)
{
  if(!cardOK)
    return;
  if(file.isOpen())  //replacing current file by new file, or subfile call
  {
    if(!replace_current)
    {
     if((int)file_subcall_ctr>(int)SD_PROCEDURE_DEPTH-1)
     {
       // SERIAL_ERROR_START;
       // SERIAL_ERRORPGM("trying to call sub-gcode files with too many levels. MAX level is:");
       // SERIAL_ERRORLN(SD_PROCEDURE_DEPTH);
       kill(_n("trying to call sub-gcode files with too many levels."), 1);
       return;
     }
     
     SERIAL_ECHO_START;
     SERIAL_ECHOPGM("SUBROUTINE CALL target:\"");
     SERIAL_ECHO(name);
     SERIAL_ECHOPGM("\" parent:\"");
     
     //store current filename and position
     getAbsFilename(filenames[file_subcall_ctr]);
     
     SERIAL_ECHO(filenames[file_subcall_ctr]);
     SERIAL_ECHOPGM("\" pos");
     SERIAL_ECHOLN(sdpos);
     filespos[file_subcall_ctr]=sdpos;
     file_subcall_ctr++;
    }
    else
    {
     SERIAL_ECHO_START;
     SERIAL_ECHOPGM("Now doing file: ");
     SERIAL_ECHOLN(name);
    }
    file.close();
  }
  else //opening fresh file
  {
    file_subcall_ctr=0; //resetting procedure depth in case user cancels print while in procedure
    SERIAL_ECHO_START;
    SERIAL_ECHOPGM("Now fresh file: ");
    SERIAL_ECHOLN(name);
  }
  sdprinting = false;

  SdFile myDir;
  const char *fname=name;
  diveSubfolder(fname,myDir);

  if(read)
  {
    if (file.open(curDir, fname, O_READ)) 
    {
      filesize = file.fileSize();
      SERIAL_PROTOCOLRPGM(_N("File opened: "));////MSG_SD_FILE_OPENED
      SERIAL_PROTOCOL(fname);
      SERIAL_PROTOCOLRPGM(_n(" Size: "));////MSG_SD_SIZE
      SERIAL_PROTOCOLLN(filesize);
      sdpos = 0;
      
      SERIAL_PROTOCOLLNRPGM(_N("File selected"));////MSG_SD_FILE_SELECTED
      getfilename(0, fname);
      lcd_setstatus(longFilename[0] ? longFilename : fname);
      lcd_setstatus("SD-PRINTING         ");
    }
    else
    {
      SERIAL_PROTOCOLRPGM(MSG_SD_OPEN_FILE_FAIL);
      SERIAL_PROTOCOL(fname);
      SERIAL_PROTOCOLLNPGM(".");
    }
  }
  else 
  { //write
    if (!file.open(curDir, fname, O_CREAT | O_APPEND | O_WRITE | O_TRUNC))
    {
      SERIAL_PROTOCOLRPGM(MSG_SD_OPEN_FILE_FAIL);
      SERIAL_PROTOCOL(fname);
      SERIAL_PROTOCOLLNPGM(".");
    }
    else
    {
      saving = true;
      SERIAL_PROTOCOLRPGM(_N("Writing to file: "));////MSG_SD_WRITE_TO_FILE
      SERIAL_PROTOCOLLN(name);
      lcd_setstatus(fname);
    }
  }
  
}

void CardReader::removeFile(const char* name)
{
    if(!cardOK) return;
    file.close();
    sdprinting = false;

    SdFile myDir;
    const char *fname=name;
    diveSubfolder(fname,myDir);

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
      SERIAL_PROTOCOLLNPGM(".");
    }
  
}

uint32_t CardReader::getFileSize()
{
	return filesize;
}

void CardReader::getStatus()
{
  if(sdprinting)
  {
      if (isPrintPaused) {
          SERIAL_PROTOCOLLNPGM("SD print paused");
      }
      else if (saved_printing) {
          SERIAL_PROTOCOLLNPGM("Print saved");
      }
      else {
          SERIAL_PROTOCOL(longFilename);
          SERIAL_PROTOCOLPGM("\n");
          SERIAL_PROTOCOLRPGM(_N("SD printing byte "));////MSG_SD_PRINTING_BYTE
          SERIAL_PROTOCOL(sdpos);
          SERIAL_PROTOCOLPGM("/");
          SERIAL_PROTOCOLLN(filesize);
          uint16_t time = _millis()/60000 - starttime/60000;
          SERIAL_PROTOCOL(itostr2(time/60));
          SERIAL_PROTOCOL(':');
          SERIAL_PROTOCOL(itostr2(time%60));
          SERIAL_PROTOCOLPGM("\n");
      }
  }
  else {
    SERIAL_PROTOCOLLNPGM("Not SD printing");
  }
}
void CardReader::write_command(char *buf)
{
  char* begin = buf;
  char* npos = 0;
  char* end = buf + strlen(buf) - 1;

  file.writeError = false;
  if((npos = strchr(buf, 'N')) != NULL)
  {
    begin = strchr(npos, ' ') + 1;
    end = strchr(npos, '*') - 1;
  }
  end[1] = '\r';
  end[2] = '\n';
  end[3] = '\0';
  file.write(begin);
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
    MYSERIAL.println("An error while writing to the SD Card.");
  }
}


void CardReader::checkautostart(bool force)
{
  if(!force)
  {
    if(!autostart_stilltocheck)
      return;
    if(autostart_atmillis<_millis())
      return;
  }
  autostart_stilltocheck=false;
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
  lsAction=LS_GetFilename;
  nrFiles=nr;
  curDir->rewind();
  lsDive("",*curDir,match);
  
}

void CardReader::getfilename_simple(uint32_t position, const char * const match/*=NULL*/)
{
	curDir = &workDir;
	lsAction = LS_GetFilename;
	nrFiles = 0;
	curDir->seekSet(position);
	lsDive("", *curDir, match);
}

uint16_t CardReader::getnrfilenames()
{
  curDir=&workDir;
  lsAction=LS_Count;
  nrFiles=0;
  curDir->rewind();
  lsDive("",*curDir);
  //SERIAL_ECHOLN(nrFiles);
  return nrFiles;
}

void CardReader::chdir(const char * relpath)
{
  SdFile newfile;
  SdFile *parent=&root;
  
  if(workDir.isOpen())
    parent=&workDir;
  
  if(!newfile.open(*parent,relpath, O_READ))
  {
   SERIAL_ECHO_START;
   SERIAL_ECHORPGM(_n("Cannot enter subdir: "));////MSG_SD_CANT_ENTER_SUBDIR
   SERIAL_ECHOLN(relpath);
  }
  else
  {
    if (workDirDepth < MAX_DIR_DEPTH) {
      for (int d = ++workDirDepth; d--;)
        workDirParents[d+1] = workDirParents[d];
      workDirParents[0]=*parent;
    }
    workDir=newfile;
	#ifdef SDCARD_SORT_ALPHA
		presort();
	#endif
  }
}

void CardReader::updir()
{
  if(workDirDepth > 0)
  {
    --workDirDepth;
    workDir = workDirParents[0];
    for (unsigned int d = 0; d < workDirDepth; d++)
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
void CardReader::getfilename_sorted(const uint16_t nr) {
	getfilename(
	#if SDSORT_GCODE
		sort_alpha &&
	#endif
		(nr < sort_count) ? sort_order[nr] : nr
	);
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
	if (farm_mode || IS_SD_INSERTED == false) return; //sorting is not used in farm mode
	uint8_t sdSort = eeprom_read_byte((uint8_t*)EEPROM_SD_SORT);

	if (sdSort == SD_SORT_NONE) return; //sd sort is turned off

	#if SDSORT_GCODE
	if (!sort_alpha) return;
	#endif
	KEEPALIVE_STATE(IN_HANDLER);

	// Throw away old sort index
	flush_presort();

	// If there are files, sort up to the limit
	uint16_t fileCnt = getnrfilenames();
	if (fileCnt > 0) {

		// Never sort more than the max allowed
		// If you use folders to organize, 20 may be enough
		if (fileCnt > SDSORT_LIMIT) {
			lcd_show_fullscreen_message_and_wait_P(_i("Some files will not be sorted. Max. No. of files in 1 folder for sorting is 100."));////MSG_FILE_CNT c=20 r=4
			fileCnt = SDSORT_LIMIT;
		}
		lcd_clear();
		#if !SDSORT_USES_RAM
			lcd_set_progress();
		#endif
		lcd_puts_at_P(0, 1, _i("Sorting files"));////MSG_SORTING c=20 r=1

		// Sort order is always needed. May be static or dynamic.
		#if SDSORT_DYNAMIC_RAM
		sort_order = new uint8_t[fileCnt];
		#endif

		// Use RAM to store the entire directory during pre-sort.
		// SDSORT_LIMIT should be set to prevent over-allocation.
		#if SDSORT_USES_RAM

		// If using dynamic ram for names, allocate on the heap.
		#if SDSORT_CACHE_NAMES
		#if SDSORT_DYNAMIC_RAM
		sortshort = new char*[fileCnt];
		sortnames = new char*[fileCnt];
		#endif
		#elif SDSORT_USES_STACK
		char sortnames[fileCnt][LONG_FILENAME_LENGTH];
		uint16_t modification_time[fileCnt];
		uint16_t modification_date[fileCnt];
		#endif

		// Folder sorting needs 1 bit per entry for flags.
		#if HAS_FOLDER_SORTING
		#if SDSORT_DYNAMIC_RAM
		isDir = new uint8_t[(fileCnt + 7) >> 3];
		#elif SDSORT_USES_STACK
		uint8_t isDir[(fileCnt + 7) >> 3];
		#endif
		#endif

		#else // !SDSORT_USES_RAM

		uint32_t positions[fileCnt];

		// By default re-read the names from SD for every compare
		// retaining only two filenames at a time. This is very
		// slow but is safest and uses minimal RAM.
		char name1[LONG_FILENAME_LENGTH + 1];
		uint16_t modification_time_bckp;
		uint16_t modification_date_bckp;

		#endif
		position = 0;
		if (fileCnt > 1) {
			// Init sort order.
			for (uint16_t i = 0; i < fileCnt; i++) {
				if (!IS_SD_INSERTED) return;
				manage_heater();
				sort_order[i] = i;
				positions[i] = position;
				getfilename(i);
				// If using RAM then read all filenames now.
				#if SDSORT_USES_RAM
				getfilename(i);
				#if SDSORT_DYNAMIC_RAM
				// Use dynamic method to copy long filename
				sortnames[i] = strdup(LONGEST_FILENAME);
				#if SDSORT_CACHE_NAMES
				// When caching also store the short name, since
				// we're replacing the getfilename() behavior.
				sortshort[i] = strdup(filename);
				#endif
				#else
				// Copy filenames into the static array
				strcpy(sortnames[i], LONGEST_FILENAME);
				modification_time[i] = modificationTime;
				modification_date[i] = modificationDate;
				#if SDSORT_CACHE_NAMES
				strcpy(sortshort[i], filename);
				#endif
				#endif
				// char out[30];
				// sprintf_P(out, PSTR("---- %i %s %s"), i, filenameIsDir ? "D" : " ", sortnames[i]);
				// SERIAL_ECHOLN(out);
				#if HAS_FOLDER_SORTING
				const uint16_t bit = i & 0x07, ind = i >> 3;
				if (bit == 0) isDir[ind] = 0x00;
				if (filenameIsDir) isDir[ind] |= _BV(bit);
				#endif
				#endif
			}

#ifdef QUICKSORT
			quicksort(0, fileCnt - 1);
#else //Qicksort not defined, use Bubble Sort
			uint32_t counter = 0;
			uint16_t total = 0.5*(fileCnt - 1)*(fileCnt);

			// Compare names from the array or just the two buffered names
			#if SDSORT_USES_RAM
			#define _SORT_CMP_NODIR() (strcasecmp(sortnames[o1], sortnames[o2]) > 0)
			#define _SORT_CMP_TIME_NODIR() (((modification_date[o1] == modification_date[o2]) && (modification_time[o1] < modification_time[o2])) || \
																	(modification_date[o1] < modification_date [o2]))
			#else
			#define _SORT_CMP_NODIR() (strcasecmp(name1, name2) > 0) //true if lowercase(name1) > lowercase(name2)
			#define _SORT_CMP_TIME_NODIR() (((modification_date_bckp == modificationDate) && (modification_time_bckp > modificationTime)) || \
																	(modification_date_bckp > modificationDate))

			#endif

			#if HAS_FOLDER_SORTING
			#if SDSORT_USES_RAM
			// Folder sorting needs an index and bit to test for folder-ness.
			const uint8_t ind1 = o1 >> 3, bit1 = o1 & 0x07,
				ind2 = o2 >> 3, bit2 = o2 & 0x07;
			#define _SORT_CMP_DIR(fs) \
										  (((isDir[ind1] & _BV(bit1)) != 0) == ((isDir[ind2] & _BV(bit2)) != 0) \
											? _SORT_CMP_NODIR() \
											: (isDir[fs > 0 ? ind1 : ind2] & (fs > 0 ? _BV(bit1) : _BV(bit2))) != 0)
			#define _SORT_CMP_TIME_DIR(fs) \
										  (((isDir[ind1] & _BV(bit1)) != 0) == ((isDir[ind2] & _BV(bit2)) != 0) \
											? _SORT_CMP_TIME_NODIR() \
											: (isDir[fs > 0 ? ind1 : ind2] & (fs > 0 ? _BV(bit1) : _BV(bit2))) != 0)
			#else
			#define _SORT_CMP_DIR(fs) ((dir1 == filenameIsDir) ? _SORT_CMP_NODIR() : (fs > 0 ? dir1 : !dir1))
			#define _SORT_CMP_TIME_DIR(fs) ((dir1 == filenameIsDir) ? _SORT_CMP_TIME_NODIR() : (fs < 0 ? dir1 : !dir1))
			#endif
			#endif

			for (uint16_t i = fileCnt; --i;) {
				if (!IS_SD_INSERTED) return;
				bool didSwap = false;

				#if !SDSORT_USES_RAM //show progresss bar only if slow sorting method is used
				int8_t percent = (counter * 100) / total;//((counter * 100) / pow((fileCnt-1),2));
				for (int column = 0; column < 20; column++) {
					if (column < (percent / 5))
					{
						lcd_set_cursor(column, 2);
						lcd_print('\x01'); //simple progress bar
					}
				}
				counter++;
				#endif

				//MYSERIAL.println(int(i));
				for (uint16_t j = 0; j < i; ++j) {
					if (!IS_SD_INSERTED) return;
					manage_heater();
					const uint16_t o1 = sort_order[j], o2 = sort_order[j + 1];

					// The most economical method reads names as-needed
					// throughout the loop. Slow if there are many.
					#if !SDSORT_USES_RAM
					counter++;
					getfilename_simple(positions[o1]);
					strcpy(name1, LONGEST_FILENAME); // save (or getfilename below will trounce it)
					modification_date_bckp = modificationDate;
					modification_time_bckp = modificationTime;
					#if HAS_FOLDER_SORTING
					bool dir1 = filenameIsDir;
					#endif
					getfilename_simple(positions[o2]);
					char *name2 = LONGEST_FILENAME; // use the string in-place

					#endif // !SDSORT_USES_RAM

													// Sort the current pair according to settings.
					if (
					#if HAS_FOLDER_SORTING
					(sdSort == SD_SORT_TIME && _SORT_CMP_TIME_DIR(FOLDER_SORTING)) || (sdSort == SD_SORT_ALPHA && _SORT_CMP_DIR(FOLDER_SORTING))
					#else
						(sdSort == SD_SORT_TIME && _SORT_CMP_TIME_NODIR()) || (sdSort == SD_SORT_ALPHA && _SORT_CMP_NODIR())
					#endif
						)
					{
						sort_order[j] = o2;
						sort_order[j + 1] = o1;
						didSwap = true;
					}
				}
				if (!didSwap) break;
			} //end of bubble sort loop
#endif
			  // Using RAM but not keeping names around
			#if (SDSORT_USES_RAM && !SDSORT_CACHE_NAMES)
			#if SDSORT_DYNAMIC_RAM
			for (uint16_t i = 0; i < fileCnt; ++i) free(sortnames[i]);
			#if HAS_FOLDER_SORTING
			free(isDir);
			#endif
			#endif
			#endif
		}
		else {
			sort_order[0] = 0;
		#if (SDSORT_USES_RAM && SDSORT_CACHE_NAMES)
			getfilename(0);
			#if SDSORT_DYNAMIC_RAM
			sortnames = new char*[1];
			sortnames[0] = strdup(LONGEST_FILENAME); // malloc
			sortshort = new char*[1];
			sortshort[0] = strdup(filename);         // malloc
			isDir = new uint8_t[1];
			#else
			strcpy(sortnames[0], LONGEST_FILENAME);
			strcpy(sortshort[0], filename);
			#endif
			isDir[0] = filenameIsDir ? 0x01 : 0x00;
		#endif
		}

		sort_count = fileCnt;
	}
#if !SDSORT_USES_RAM //show progresss bar only if slow sorting method is used
	for (int column = 0; column <= 19; column++)
	{
		lcd_set_cursor(column, 2);
		lcd_print('\x01'); //simple progress bar
	}
	_delay(300);
	lcd_set_degree();
	lcd_clear();
#endif
	lcd_update(2);
	KEEPALIVE_STATE(NOT_BUSY);
	lcd_timeoutToStatus.start();
}

void CardReader::flush_presort() {
	if (sort_count > 0) {
		#if SDSORT_DYNAMIC_RAM
		delete sort_order;
		#if SDSORT_CACHE_NAMES
		for (uint8_t i = 0; i < sort_count; ++i) {
			free(sortshort[i]); // strdup
			free(sortnames[i]); // strdup
		}
		delete sortshort;
		delete sortnames;
		#endif
		#endif
		sort_count = 0;
	}
}

#endif // SDCARD_SORT_ALPHA



void CardReader::printingHasFinished()
{
    st_synchronize();
    if(file_subcall_ctr>0) //heading up to a parent file that called current as a procedure.
    {
      file.close();
      file_subcall_ctr--;
      openFile(filenames[file_subcall_ctr],true,true);
      setIndex(filespos[file_subcall_ctr]);
      startFileprint();
    }
    else
    {
      quickStop();
      file.close();
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
