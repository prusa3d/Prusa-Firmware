#include "cmdqueue.h"
#include "cardreader.h"
#include "ultralcd.h"

extern bool Stopped;

// Reserve BUFSIZE lines of length MAX_CMD_SIZE plus CMDBUFFER_RESERVE_FRONT.
char cmdbuffer[BUFSIZE * (MAX_CMD_SIZE + 1) + CMDBUFFER_RESERVE_FRONT];
// Head of the circular buffer, where to read.
size_t bufindr = 0;
// Tail of the buffer, where to write.
static size_t bufindw = 0;
// Number of lines in cmdbuffer.
int buflen = 0;
// Flag for processing the current command inside the main Arduino loop().
// If a new command was pushed to the front of a command buffer while
// processing another command, this replaces the command on the top.
// Therefore don't remove the command from the queue in the loop() function.
bool cmdbuffer_front_already_processed = false;

int serial_count = 0;  //index of character read from serial line
boolean comment_mode = false;
char *strchr_pointer; // just a pointer to find chars in the command string like X, Y, Z, E, etc

unsigned long TimeSent = _millis();
unsigned long TimeNow = _millis();

long gcode_N = 0;
long gcode_LastN = 0;
long Stopped_gcode_LastN = 0;

uint32_t sdpos_atomic = 0;


// Pop the currently processed command from the queue.
// It is expected, that there is at least one command in the queue.
bool cmdqueue_pop_front()
{
    if (buflen > 0) {
#ifdef CMDBUFFER_DEBUG
        SERIAL_ECHOPGM("Dequeing ");
        SERIAL_ECHO(cmdbuffer+bufindr+CMDHDRSIZE);
        SERIAL_ECHOLNPGM("");
        SERIAL_ECHOPGM("Old indices: buflen ");
        SERIAL_ECHO(buflen);
        SERIAL_ECHOPGM(", bufindr ");
        SERIAL_ECHO(bufindr);
        SERIAL_ECHOPGM(", bufindw ");
        SERIAL_ECHO(bufindw);
        SERIAL_ECHOPGM(", serial_count ");
        SERIAL_ECHO(serial_count);
        SERIAL_ECHOPGM(", bufsize ");
        SERIAL_ECHO(sizeof(cmdbuffer));
        SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
        if (-- buflen == 0) {
            // Empty buffer.
            if (serial_count == 0)
                // No serial communication is pending. Reset both pointers to zero.
                bufindw = 0;
            bufindr = bufindw;
        } else {
            // There is at least one ready line in the buffer.
            // First skip the current command ID and iterate up to the end of the string.
            for (bufindr += CMDHDRSIZE; cmdbuffer[bufindr] != 0; ++ bufindr) ;
            // Second, skip the end of string null character and iterate until a nonzero command ID is found.
            for (++ bufindr; bufindr < sizeof(cmdbuffer) && cmdbuffer[bufindr] == 0; ++ bufindr) ;
            // If the end of the buffer was empty,
            if (bufindr == sizeof(cmdbuffer)) {
                // skip to the start and find the nonzero command.
                for (bufindr = 0; cmdbuffer[bufindr] == 0; ++ bufindr) ;
            }
#ifdef CMDBUFFER_DEBUG
            SERIAL_ECHOPGM("New indices: buflen ");
            SERIAL_ECHO(buflen);
            SERIAL_ECHOPGM(", bufindr ");
            SERIAL_ECHO(bufindr);
            SERIAL_ECHOPGM(", bufindw ");
            SERIAL_ECHO(bufindw);
            SERIAL_ECHOPGM(", serial_count ");
            SERIAL_ECHO(serial_count);
            SERIAL_ECHOPGM(" new command on the top: ");
            SERIAL_ECHO(cmdbuffer+bufindr+CMDHDRSIZE);
            SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
        }
        return true;
    }
    return false;
}

void cmdqueue_reset()
{
    bufindr = 0;
    bufindw = 0;
    buflen = 0;

	//commands are removed from command queue after process_command() function is finished
	//reseting command queue and enqueing new commands during some (usually long running) command processing would cause that new commands are immediately removed from queue (or damaged)
	//this will ensure that all new commands which are enqueued after cmdqueue reset, will be always executed
    cmdbuffer_front_already_processed = true; 
}

// How long a string could be pushed to the front of the command queue?
// If yes, adjust bufindr to the new position, where the new command could be enqued.
// len_asked does not contain the zero terminator size.
static bool cmdqueue_could_enqueue_front(size_t len_asked)
{
    // MAX_CMD_SIZE has to accommodate the zero terminator.
    if (len_asked >= MAX_CMD_SIZE)
        return false;
    // Remove the currently processed command from the queue.
    if (! cmdbuffer_front_already_processed) {
        cmdqueue_pop_front();
        cmdbuffer_front_already_processed = true;
    }
    if (bufindr == bufindw && buflen > 0)
        // Full buffer.
        return false;
    // Adjust the end of the write buffer based on whether a partial line is in the receive buffer.
    int endw = (serial_count > 0) ? (bufindw + MAX_CMD_SIZE + 1) : bufindw;
    if (bufindw < bufindr) {
        int bufindr_new = bufindr - len_asked - (1 + CMDHDRSIZE);
        // Simple case. There is a contiguous space between the write buffer and the read buffer.
        if (endw <= bufindr_new) {
            bufindr = bufindr_new;
            return true;
        }
    } else {
        // Otherwise the free space is split between the start and end.
        if (len_asked + (1 + CMDHDRSIZE) <= bufindr) {
            // Could fit at the start.
            bufindr -= len_asked + (1 + CMDHDRSIZE);
            return true;
        }
        int bufindr_new = sizeof(cmdbuffer) - len_asked - (1 + CMDHDRSIZE);
        if (endw <= bufindr_new) {
            memset(cmdbuffer, 0, bufindr);
            bufindr = bufindr_new;
            return true;
        }
    }
    return false;
}

// Could one enqueue a command of length len_asked into the buffer,
// while leaving CMDBUFFER_RESERVE_FRONT at the start?
// If yes, adjust bufindw to the new position, where the new command could be enqued.
// len_asked does not contain the zero terminator size.
// This function may update bufindw, therefore for the power panic to work, this function must be called
// with the interrupts disabled!
static bool cmdqueue_could_enqueue_back(size_t len_asked, bool atomic_update = false)
{
    // MAX_CMD_SIZE has to accommodate the zero terminator.
    if (len_asked >= MAX_CMD_SIZE)
        return false;

    if (bufindr == bufindw && buflen > 0)
        // Full buffer.
        return false;

    if (serial_count > 0) {
        // If there is some data stored starting at bufindw, len_asked is certainly smaller than
        // the allocated data buffer. Try to reserve a new buffer and to move the already received
        // serial data.
        // How much memory to reserve for the commands pushed to the front?
        // End of the queue, when pushing to the end.
        size_t endw = bufindw + len_asked + (1 + CMDHDRSIZE);
        if (bufindw < bufindr)
            // Simple case. There is a contiguous space between the write buffer and the read buffer.
            return endw + CMDBUFFER_RESERVE_FRONT <= bufindr;
        // Otherwise the free space is split between the start and end.
        if (// Could one fit to the end, including the reserve?
            endw + CMDBUFFER_RESERVE_FRONT <= sizeof(cmdbuffer) ||
            // Could one fit to the end, and the reserve to the start?
            (endw <= sizeof(cmdbuffer) && CMDBUFFER_RESERVE_FRONT <= bufindr))
            return true;
        // Could one fit both to the start?
        if (len_asked + (1 + CMDHDRSIZE) + CMDBUFFER_RESERVE_FRONT <= bufindr) {
            // Mark the rest of the buffer as used.
            memset(cmdbuffer+bufindw, 0, sizeof(cmdbuffer)-bufindw);
            // and point to the start.
            // Be careful! The bufindw needs to be changed atomically for the power panic & filament panic to work.
            if (atomic_update)
                cli();
            bufindw = 0;
            if (atomic_update)
                sei();
            return true;
        }
    } else {
        // How much memory to reserve for the commands pushed to the front?
        // End of the queue, when pushing to the end.
        size_t endw = bufindw + len_asked + (1 + CMDHDRSIZE);
        if (bufindw < bufindr)
            // Simple case. There is a contiguous space between the write buffer and the read buffer.
            return endw + CMDBUFFER_RESERVE_FRONT <= bufindr;
        // Otherwise the free space is split between the start and end.
        if (// Could one fit to the end, including the reserve?
            endw + CMDBUFFER_RESERVE_FRONT <= sizeof(cmdbuffer) ||
            // Could one fit to the end, and the reserve to the start?
            (endw <= sizeof(cmdbuffer) && CMDBUFFER_RESERVE_FRONT <= bufindr))
            return true;
        // Could one fit both to the start?
        if (len_asked + (1 + CMDHDRSIZE) + CMDBUFFER_RESERVE_FRONT <= bufindr) {
            // Mark the rest of the buffer as used.
            memset(cmdbuffer+bufindw, 0, sizeof(cmdbuffer)-bufindw);
            // and point to the start.
            // Be careful! The bufindw needs to be changed atomically for the power panic & filament panic to work.
            if (atomic_update)
                cli();
            bufindw = 0;
            if (atomic_update)
                sei();
            return true;
        }
    }
    return false;
}

#ifdef CMDBUFFER_DEBUG
void cmdqueue_dump_to_serial_single_line(int nr, const char *p)
{
    SERIAL_ECHOPGM("Entry nr: ");
    SERIAL_ECHO(nr);
    SERIAL_ECHOPGM(", type: ");
    SERIAL_ECHO(int(*p));
    SERIAL_ECHOPGM(", cmd: ");
    SERIAL_ECHO(p+1);  
    SERIAL_ECHOLNPGM("");
}

void cmdqueue_dump_to_serial()
{
    if (buflen == 0) {
        SERIAL_ECHOLNPGM("The command buffer is empty.");
    } else {
        SERIAL_ECHOPGM("Content of the buffer: entries ");
        SERIAL_ECHO(buflen);
        SERIAL_ECHOPGM(", indr ");
        SERIAL_ECHO(bufindr);
        SERIAL_ECHOPGM(", indw ");
        SERIAL_ECHO(bufindw);
        SERIAL_ECHOLNPGM("");
        int nr = 0;
        if (bufindr < bufindw) {
            for (const char *p = cmdbuffer + bufindr; p < cmdbuffer + bufindw; ++ nr) {
                cmdqueue_dump_to_serial_single_line(nr, p);
                // Skip the command.
                for (++p; *p != 0; ++ p);
                // Skip the gaps.
                for (++p; p < cmdbuffer + bufindw && *p == 0; ++ p);
            }
        } else {
            for (const char *p = cmdbuffer + bufindr; p < cmdbuffer + sizeof(cmdbuffer); ++ nr) {
                cmdqueue_dump_to_serial_single_line(nr, p);
                // Skip the command.
                for (++p; *p != 0; ++ p);
                // Skip the gaps.
                for (++p; p < cmdbuffer + sizeof(cmdbuffer) && *p == 0; ++ p);
            }
            for (const char *p = cmdbuffer; p < cmdbuffer + bufindw; ++ nr) {
                cmdqueue_dump_to_serial_single_line(nr, p);
                // Skip the command.
                for (++p; *p != 0; ++ p);
                // Skip the gaps.
                for (++p; p < cmdbuffer + bufindw && *p == 0; ++ p);
            }
        }
        SERIAL_ECHOLNPGM("End of the buffer.");
    }
}
#endif /* CMDBUFFER_DEBUG */

//adds an command to the main command buffer
//thats really done in a non-safe way.
//needs overworking someday
// Currently the maximum length of a command piped through this function is around 20 characters
void enquecommand(const char *cmd, bool from_progmem)
{
    size_t len = from_progmem ? strlen_P(cmd) : strlen(cmd);
    // Does cmd fit the queue while leaving sufficient space at the front for the chained commands?
    // If it fits, it may move bufindw, so it points to a contiguous buffer, which fits cmd.
    if (cmdqueue_could_enqueue_back(len)) {
        // This is dangerous if a mixing of serial and this happens
        // This may easily be tested: If serial_count > 0, we have a problem.
        cmdbuffer[bufindw] = CMDBUFFER_CURRENT_TYPE_UI;
        if (from_progmem)
            strcpy_P(cmdbuffer + bufindw + CMDHDRSIZE, cmd);
        else
            strcpy(cmdbuffer + bufindw + CMDHDRSIZE, cmd);
        SERIAL_ECHO_START;
        SERIAL_ECHORPGM(MSG_Enqueing);
        SERIAL_ECHO(cmdbuffer + bufindw + CMDHDRSIZE);
        SERIAL_ECHOLNPGM("\"");
        bufindw += len + (CMDHDRSIZE + 1);
        if (bufindw == sizeof(cmdbuffer))
            bufindw = 0;
        ++ buflen;
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    } else {
        SERIAL_ERROR_START;
        SERIAL_ECHORPGM(MSG_Enqueing);
        if (from_progmem)
            SERIAL_PROTOCOLRPGM(cmd);
        else
            SERIAL_ECHO(cmd);
        SERIAL_ECHOLNPGM("\" failed: Buffer full!");
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    }
}

bool cmd_buffer_empty()
{
	return (buflen == 0);
}

void enquecommand_front(const char *cmd, bool from_progmem)
{
    size_t len = from_progmem ? strlen_P(cmd) : strlen(cmd);
    // Does cmd fit the queue? This call shall move bufindr, so the command may be copied.
    if (cmdqueue_could_enqueue_front(len)) {
        cmdbuffer[bufindr] = CMDBUFFER_CURRENT_TYPE_UI;
        if (from_progmem)
            strcpy_P(cmdbuffer + bufindr + CMDHDRSIZE, cmd);
        else
            strcpy(cmdbuffer + bufindr + CMDHDRSIZE, cmd);
        ++ buflen;
        SERIAL_ECHO_START;
        SERIAL_ECHOPGM("Enqueing to the front: \"");
        SERIAL_ECHO(cmdbuffer + bufindr + CMDHDRSIZE);
        SERIAL_ECHOLNPGM("\"");
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    } else {
        SERIAL_ERROR_START;
        SERIAL_ECHOPGM("Enqueing to the front: \"");
        if (from_progmem)
            SERIAL_PROTOCOLRPGM(cmd);
        else
            SERIAL_ECHO(cmd);
        SERIAL_ECHOLNPGM("\" failed: Buffer full!");
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    }
}

// Mark the command at the top of the command queue as new.
// Therefore it will not be removed from the queue.
void repeatcommand_front()
{
    cmdbuffer_front_already_processed = true;
} 

bool is_buffer_empty()
{
    if (buflen == 0) return true;
    else return false;
}

void proc_commands() {
	if (buflen)
	{
		process_commands();
		if (!cmdbuffer_front_already_processed)
			cmdqueue_pop_front();
		cmdbuffer_front_already_processed = false;
	}
}

void get_command()
{
    // Test and reserve space for the new command string.
    if (! cmdqueue_could_enqueue_back(MAX_CMD_SIZE - 1, true))
      return;

	if (MYSERIAL.available() == RX_BUFFER_SIZE - 1) { //compare number of chars buffered in rx buffer with rx buffer size
		MYSERIAL.flush();
		SERIAL_ECHOLNPGM("Full RX Buffer");   //if buffer was full, there is danger that reading of last gcode will not be completed
	}

  // start of serial line processing loop
  while ((MYSERIAL.available() > 0 && !saved_printing) || (MYSERIAL.available() > 0 && isPrintPaused)) {  //is print is saved (crash detection or filament detection), dont process data from serial line
	
    char serial_char = MYSERIAL.read();
/*    if (selectedSerialPort == 1)
    {
        selectedSerialPort = 0; 
        MYSERIAL.write(serial_char); // for debuging serial line 2 in farm_mode
        selectedSerialPort = 1; 
    } */ //RP - removed
      TimeSent = _millis();
      TimeNow = _millis();

    if (serial_char < 0)
        // Ignore extended ASCII characters. These characters have no meaning in the G-code apart from the file names
        // and Marlin does not support such file names anyway.
        // Serial characters with a highest bit set to 1 are generated when the USB cable is unplugged, leading
        // to a hang-up of the print process from an SD card.
        continue;
    if(serial_char == '\n' ||
       serial_char == '\r' ||
       serial_count >= (MAX_CMD_SIZE - 1) )
    {
      if(!serial_count) { //if empty line
        comment_mode = false; //for new command
        return;
      }
      cmdbuffer[bufindw+serial_count+CMDHDRSIZE] = 0; //terminate string
      if(!comment_mode){
		  
		  gcode_N = 0;

		  // Line numbers must be first in buffer

		  if ((strstr(cmdbuffer+bufindw+CMDHDRSIZE, "PRUSA") == NULL) &&
			  (cmdbuffer[bufindw+CMDHDRSIZE] == 'N')) {

			  // Line number met. When sending a G-code over a serial line, each line may be stamped with its index,
			  // and Marlin tests, whether the successive lines are stamped with an increasing line number ID
			  gcode_N = (strtol(cmdbuffer+bufindw+CMDHDRSIZE+1, NULL, 10));
			  if(gcode_N != gcode_LastN+1 && (strstr_P(cmdbuffer+bufindw+CMDHDRSIZE, PSTR("M110")) == NULL) ) {
				  // M110 - set current line number.
				  // Line numbers not sent in succession.
				  SERIAL_ERROR_START;
				  SERIAL_ERRORRPGM(_n("Line Number is not Last Line Number+1, Last Line: "));////MSG_ERR_LINE_NO c=0 r=0
				  SERIAL_ERRORLN(gcode_LastN);
				  //Serial.println(gcode_N);
				  FlushSerialRequestResend();
				  serial_count = 0;
				  return;
			  }

			  if((strchr_pointer = strchr(cmdbuffer+bufindw+CMDHDRSIZE, '*')) != NULL)
			  {
				  byte checksum = 0;
				  char *p = cmdbuffer+bufindw+CMDHDRSIZE;
				  while (p != strchr_pointer)
					  checksum = checksum^(*p++);
				  if (int(strtol(strchr_pointer+1, NULL, 10)) != int(checksum)) {
					  SERIAL_ERROR_START;
					  SERIAL_ERRORRPGM(_n("checksum mismatch, Last Line: "));////MSG_ERR_CHECKSUM_MISMATCH c=0 r=0
					  SERIAL_ERRORLN(gcode_LastN);
					  FlushSerialRequestResend();
					  serial_count = 0;
					  return;
				  }
				  // If no errors, remove the checksum and continue parsing.
				  *strchr_pointer = 0;
			  }
			  else
			  {
				  SERIAL_ERROR_START;
				  SERIAL_ERRORRPGM(_n("No Checksum with line number, Last Line: "));////MSG_ERR_NO_CHECKSUM c=0 r=0
				  SERIAL_ERRORLN(gcode_LastN);
				  FlushSerialRequestResend();
				  serial_count = 0;
				  return;
			  }

			  // Don't parse N again with code_seen('N')
			  cmdbuffer[bufindw + CMDHDRSIZE] = '$';
			  //if no errors, continue parsing
			  gcode_LastN = gcode_N;
		}
        // if we don't receive 'N' but still see '*'
        if ((cmdbuffer[bufindw + CMDHDRSIZE] != 'N') && (cmdbuffer[bufindw + CMDHDRSIZE] != '$') && (strchr(cmdbuffer+bufindw+CMDHDRSIZE, '*') != NULL))
        {

            SERIAL_ERROR_START;
            SERIAL_ERRORRPGM(_n("No Line Number with checksum, Last Line: "));////MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM c=0 r=0
            SERIAL_ERRORLN(gcode_LastN);
			FlushSerialRequestResend();
            serial_count = 0;
            return;
        }
        if ((strchr_pointer = strchr(cmdbuffer+bufindw+CMDHDRSIZE, 'G')) != NULL) {
              if (! IS_SD_PRINTING) {
                      usb_printing_counter = 10;
                      is_usb_printing = true;
              }
            if (Stopped == true) {
                int gcode = strtol(strchr_pointer+1, NULL, 10);
                if (gcode >= 0 && gcode <= 3) {
                    SERIAL_ERRORLNRPGM(MSG_ERR_STOPPED);
                    LCD_MESSAGERPGM(_T(MSG_STOPPED));
                }
            }
        } // end of 'G' command

        //If command was e-stop process now
        if(strcmp(cmdbuffer+bufindw+CMDHDRSIZE, "M112") == 0)
          kill("", 2);
        
        // Store the current line into buffer, move to the next line.
		// Store type of entry
        cmdbuffer[bufindw] = gcode_N ? CMDBUFFER_CURRENT_TYPE_USB_WITH_LINENR : CMDBUFFER_CURRENT_TYPE_USB;
#ifdef CMDBUFFER_DEBUG
        SERIAL_ECHO_START;
        SERIAL_ECHOPGM("Storing a command line to buffer: ");
        SERIAL_ECHO(cmdbuffer+bufindw+CMDHDRSIZE);
        SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
        bufindw += strlen(cmdbuffer+bufindw+CMDHDRSIZE) + (1 + CMDHDRSIZE);
        if (bufindw == sizeof(cmdbuffer))
            bufindw = 0;
        ++ buflen;
#ifdef CMDBUFFER_DEBUG
        SERIAL_ECHOPGM("Number of commands in the buffer: ");
        SERIAL_ECHO(buflen);
        SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
      } // end of 'not comment mode'
      serial_count = 0; //clear buffer
      // Don't call cmdqueue_could_enqueue_back if there are no characters waiting
      // in the queue, as this function will reserve the memory.
      if (MYSERIAL.available() == 0 || ! cmdqueue_could_enqueue_back(MAX_CMD_SIZE-1, true))
          return;
    } // end of "end of line" processing
    else {
      // Not an "end of line" symbol. Store the new character into a buffer.
      if(serial_char == ';') comment_mode = true;
      if(!comment_mode) cmdbuffer[bufindw+CMDHDRSIZE+serial_count++] = serial_char;
    }
  } // end of serial line processing loop

    if(farm_mode){
        TimeNow = _millis();
        if ( ((TimeNow - TimeSent) > 800) && (serial_count > 0) ) {
            cmdbuffer[bufindw+serial_count+CMDHDRSIZE] = 0;
            
            bufindw += strlen(cmdbuffer+bufindw+CMDHDRSIZE) + (1 + CMDHDRSIZE);
            if (bufindw == sizeof(cmdbuffer))
                bufindw = 0;
            ++ buflen;
            
            serial_count = 0;
            
            SERIAL_ECHOPGM("TIMEOUT:");
            //memset(cmdbuffer, 0 , sizeof(cmdbuffer));
            return;
        }
    }

  #ifdef SDSUPPORT
  if(!card.sdprinting || serial_count!=0){
    // If there is a half filled buffer from serial line, wait until return before
    // continuing with the serial line.
     return;
  }

  //'#' stops reading from SD to the buffer prematurely, so procedural macro calls are possible
  // if it occurs, stop_buffering is triggered and the buffer is ran dry.
  // this character _can_ occur in serial com, due to checksums. however, no checksums are used in SD printing

  static bool stop_buffering=false;
  if(buflen==0) stop_buffering=false;
  union {
    struct {
        char lo;
        char hi;
    } lohi;
    uint16_t value;
  } sd_count;
  sd_count.value = 0;
  // Reads whole lines from the SD card. Never leaves a half-filled line in the cmdbuffer.
  while( !card.eof() && !stop_buffering) {
    int16_t n=card.get();
    char serial_char = (char)n;
    if(serial_char == '\n' ||
       serial_char == '\r' ||
       ((serial_char == '#' || serial_char == ':') && comment_mode == false) ||
       serial_count >= (MAX_CMD_SIZE - 1) || n==-1)
    {
      if(card.eof()){
        SERIAL_PROTOCOLLNRPGM(_n("Done printing file"));////MSG_FILE_PRINTED c=0 r=0
        stoptime=_millis();
        char time[30];
        unsigned long t=(stoptime-starttime-pause_time)/1000;
        pause_time = 0;
        int hours, minutes;
        minutes=(t/60)%60;
        hours=t/60/60;
        save_statistics(total_filament_used, t);
        sprintf_P(time, PSTR("%i hours %i minutes"),hours, minutes);
        SERIAL_ECHO_START;
        SERIAL_ECHOLN(time);
        lcd_setstatus(time);
        card.printingHasFinished();
        card.checkautostart(true);

        if (farm_mode)
        {
            prusa_statistics(6);
            lcd_commands_type = LCD_COMMAND_FARM_MODE_CONFIRM;
        }

      }
      if(serial_char=='#')
        stop_buffering=true;

      if(!serial_count)
      {
        // This is either an empty line, or a line with just a comment.
        // Continue to the following line, and continue accumulating the number of bytes
        // read from the sdcard into sd_count, 
        // so that the lenght of the already read empty lines and comments will be added
        // to the following non-empty line. 
        comment_mode = false;
        continue; //if empty line
      }
      // The new command buffer could be updated non-atomically, because it is not yet considered
      // to be inside the active queue.
      sd_count.value = (card.get_sdpos()+1) - sdpos_atomic;
      cmdbuffer[bufindw] = CMDBUFFER_CURRENT_TYPE_SDCARD;
      cmdbuffer[bufindw+1] = sd_count.lohi.lo;
      cmdbuffer[bufindw+2] = sd_count.lohi.hi;
      cmdbuffer[bufindw+serial_count+CMDHDRSIZE] = 0; //terminate string
      // Calculate the length before disabling the interrupts.
      uint8_t len = strlen(cmdbuffer+bufindw+CMDHDRSIZE) + (1 + CMDHDRSIZE);

//      SERIAL_ECHOPGM("SD cmd(");
//      MYSERIAL.print(sd_count.value, DEC);
//      SERIAL_ECHOPGM(") ");
//      SERIAL_ECHOLN(cmdbuffer+bufindw+CMDHDRSIZE);
//    SERIAL_ECHOPGM("cmdbuffer:");
//    MYSERIAL.print(cmdbuffer);
//    SERIAL_ECHOPGM("buflen:");
//    MYSERIAL.print(buflen+1);
      sd_count.value = 0;

      cli();
      // This block locks the interrupts globally for 3.56 us,
      // which corresponds to a maximum repeat frequency of 280.70 kHz.
      // This blocking is safe in the context of a 10kHz stepper driver interrupt
      // or a 115200 Bd serial line receive interrupt, which will not trigger faster than 12kHz.
      ++ buflen;
      bufindw += len;
      sdpos_atomic = card.get_sdpos()+1;
      if (bufindw == sizeof(cmdbuffer))
          bufindw = 0;
      sei();

      comment_mode = false; //for new command
      serial_count = 0; //clear buffer
      // The following line will reserve buffer space if available.
      if (! cmdqueue_could_enqueue_back(MAX_CMD_SIZE-1, true))
          return;
    }
    else
    {
      if(serial_char == ';') comment_mode = true;
      else if(!comment_mode) cmdbuffer[bufindw+CMDHDRSIZE+serial_count++] = serial_char;
    }
  }

  #endif //SDSUPPORT
}

uint16_t cmdqueue_calc_sd_length()
{
    if (buflen == 0)
        return 0;
    union {
        struct {
            char lo;
            char hi;
        } lohi;
        uint16_t value;
    } sdlen_single;
    uint16_t sdlen = 0;
    for (size_t _buflen = buflen, _bufindr = bufindr;;) {
        if (cmdbuffer[_bufindr] == CMDBUFFER_CURRENT_TYPE_SDCARD) {
            sdlen_single.lohi.lo = cmdbuffer[_bufindr + 1];
            sdlen_single.lohi.hi = cmdbuffer[_bufindr + 2];
            sdlen += sdlen_single.value;
        }
        if (-- _buflen == 0)
            break;
        // First skip the current command ID and iterate up to the end of the string.
        for (_bufindr += CMDHDRSIZE; cmdbuffer[_bufindr] != 0; ++ _bufindr) ;
        // Second, skip the end of string null character and iterate until a nonzero command ID is found.
        for (++ _bufindr; _bufindr < sizeof(cmdbuffer) && cmdbuffer[_bufindr] == 0; ++ _bufindr) ;
        // If the end of the buffer was empty,
        if (_bufindr == sizeof(cmdbuffer)) {
            // skip to the start and find the nonzero command.
            for (_bufindr = 0; cmdbuffer[_bufindr] == 0; ++ _bufindr) ;
        }
    }
    return sdlen;
}
