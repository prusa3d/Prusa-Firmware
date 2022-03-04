//! @file
// Based on the OptiBoot project
// https://github.com/Optiboot/optiboot
// Licence GLP 2 or later.

#include "Marlin.h"
#include "xflash.h"
#include "stk500.h"
#include "bootapp.h"
#include <avr/wdt.h>

#define OPTIBOOT_MAJVER 6
#define OPTIBOOT_CUSTOMVER 0
#define OPTIBOOT_MINVER 2
static unsigned const int __attribute__((section(".version"))) 
  optiboot_version = 256*(OPTIBOOT_MAJVER + OPTIBOOT_CUSTOMVER) + OPTIBOOT_MINVER;

#if 0
#define XFLASH_SIGNATURE_0 9
#define XFLASH_SIGNATURE_1 8
#define XFLASH_SIGNATURE_2 7
#else
//FIXME this is a signature of ATmega2560!
#define XFLASH_SIGNATURE_0 0x1E
#define XFLASH_SIGNATURE_1 0x98
#define XFLASH_SIGNATURE_2 0x01
#endif

static uint8_t getch(void) {
  while (!MYSERIAL.available()) {};
  wdt_reset();
  return MYSERIAL.read();
}

static void putch(uint8_t ch) {
  MYSERIAL.write(ch);
}

static void verifySpace() {
  if (getch() != CRC_EOP) {
    putch(STK_FAILED);
    wdt_enable(WDTO_15MS); // shorten WD timeout
    while (1)           // and busy-loop so that WD causes
      ;             //  a reset and app start.
  }
  putch(STK_INSYNC);
}

static void getNch(uint8_t count) {
  do getch(); while (--count);
  verifySpace();
}

typedef uint16_t pagelen_t;

//Thou shalt not change these messages, else the avrdude-slicer xflash implementation will no longer work and the language upload will fail.
//Right now we support 2 xflash chips - the original w25x20cl and a new one GD25Q20C
static const char entry_magic_send   [] PROGMEM = "start";
static const char entry_magic_receive[] PROGMEM = "w25x20cl_enter\n";
static const char entry_magic_cfm    [] PROGMEM = "w25x20cl_cfm";

struct block_t;
extern struct block_t *block_buffer;

//! @brief Enter an STK500 compatible Optiboot boot loader waiting for flashing the languages to an external flash memory.
//! @return 1 if "start\n" was not sent. Optiboot was skipped
//! @return 0 if "start\n" was sent. Optiboot ran normally. No need to send "start\n" in setup()
uint8_t optiboot_xflash_enter()
{
// Make sure to check boot_app_magic as well. Since these bootapp flags are located right in the middle of the stack,
// they can be unintentionally changed. As a workaround to the language upload problem, do not only check for one bit if it's set,
// but rather test 33 bits for the correct value before exiting optiboot early.
  if ((boot_app_magic == BOOT_APP_MAGIC) && (boot_app_flags & BOOT_APP_FLG_USER0)) return 1;
  uint8_t ch;
  uint8_t rampz = 0;
  register uint16_t address = 0;
  register pagelen_t length;
  // Use the planner's queue for the receive / transmit buffers.
//  uint8_t *buff = (uint8_t*)block_buffer;
  uint8_t buff[260];
  // bitmap of pages to be written. Bit is set to 1 if the page has already been erased.
  uint8_t pages_erased = 0;

  // Handshake sequence: Initialize the serial line, flush serial line, send magic, receive magic.
  // If the magic is not received on time, or it is not received correctly, continue to the application.
  {
    wdt_reset();
    const uint8_t selectedSerialPort_bak = selectedSerialPort;
    if (GPIOR0 == 0x01) {
        selectedSerialPort = 1;
        MYSERIAL.begin(BAUDRATE);
        _delay_ms(500); //artifical delay. Postpone "start" message because avrdude-slicer is not ready quick enough when the board is not reset between bootloaders
    }
    else {
        selectedSerialPort = 0;
    }
    MYSERIAL.flush(); //clear RX buffer
    
    // Send the initial magic string.
    MYSERIAL.printlnPGM(entry_magic_send);
    
    // Wait for two seconds until a magic string (constant entry_magic) is received
    // from the serial line.
    const char *ptr = entry_magic_receive;
    uint8_t charsConsumed = 0;
    while ((ch = pgm_read_byte(ptr++))) {
      unsigned long  boot_timer = 2000000;
      while (!MYSERIAL.available()) {
        wdt_reset();
        if ( --boot_timer == 0) {
          // Timeout expired, continue with the application.
          if (selectedSerialPort != selectedSerialPort_bak) {
            selectedSerialPort = selectedSerialPort_bak; //revert Serial setting
            return 1; //start was sent on the wrong port. Send it again later on the correct one.
          }
          else {
            MYSERIAL.rewind(charsConsumed);
            return 0; //start was sent on the correct port. Do not send a second start later.
          }
        }
      }
      uint8_t newChar = MYSERIAL.read();
      charsConsumed++;
      if (ch != newChar)
      {
          // Magic was not received correctly, continue with the application
          if (selectedSerialPort != selectedSerialPort_bak) {
            selectedSerialPort = selectedSerialPort_bak; //revert Serial setting
            return 1; //start was sent on the wrong port. Send it again later on the correct one.
          }
          else {
            MYSERIAL.rewind(charsConsumed);
            return 0; //start was sent on the correct port. Do not send a second start later.
          }
      }
      wdt_reset();
    }
    
    // Send the cfm magic string.
    MYSERIAL.printlnPGM(entry_magic_cfm);
  }

  spi_init();
  xflash_init();
  wdt_disable();

  /* Forever loop: exits by causing WDT reset */
  for (;;) {
    /* get character from UART */
    ch = getch();

    if(ch == STK_GET_PARAMETER) {
      unsigned char which = getch();
      verifySpace();
      /*
       * Send optiboot version as "SW version"
       * Note that the references to memory are optimized away.
       */
      if (which == STK_SW_MINOR) {
        putch(optiboot_version & 0xFF);
      } else if (which == STK_SW_MAJOR) {
        putch(optiboot_version >> 8);
      } else {
        /*
         * GET PARAMETER returns a generic 0x03 reply for
               * other parameters - enough to keep Avrdude happy
         */
        putch(0x03);
      }
    }
    else if(ch == STK_SET_DEVICE) {
      // SET DEVICE is ignored
      getNch(20);
    }
    else if(ch == STK_SET_DEVICE_EXT) {
      // SET DEVICE EXT is ignored
      getNch(5);
    }
    else if(ch == STK_LOAD_ADDRESS) {
      // LOAD ADDRESS
      uint16_t newAddress;
      // Workaround for the infamous ';' bug in the Prusa3D usb to serial converter.
      // Send the binary data by nibbles to avoid transmitting the ';' character.
      newAddress  = getch();
      newAddress |= getch();
      newAddress |= (((uint16_t)getch()) << 8);
      newAddress |= (((uint16_t)getch()) << 8);
      // Transfer top bit to LSB in rampz
      if (newAddress & 0x8000)
        rampz |= 0x01;
      else
        rampz &= 0xFE;
      newAddress += newAddress; // Convert from word address to byte address
      address = newAddress;
      verifySpace();
    }
    else if(ch == STK_UNIVERSAL) {
      // LOAD_EXTENDED_ADDRESS is needed in STK_UNIVERSAL for addressing more than 128kB
      if ( AVR_OP_LOAD_EXT_ADDR == getch() ) {
        // get address
        getch();  // get '0'
        rampz = (rampz & 0x01) | ((getch() << 1) & 0xff);  // get address and put it in rampz
        getNch(1); // get last '0'
        // response
        putch(0x00);
      }
      else {
        // everything else is ignored
        getNch(3);
        putch(0x00);
      }
    }
    /* Write memory, length is big endian and is in bytes */
    else if(ch == STK_PROG_PAGE) {
      // PROGRAM PAGE - we support flash programming only, not EEPROM
      uint8_t desttype;
      uint8_t *bufPtr;
      pagelen_t savelength;
      // Read the page length, with the length transferred each nibble separately to work around
      // the Prusa's USB to serial infamous semicolon issue.
      length  = ((pagelen_t)getch()) << 8;
      length |= ((pagelen_t)getch()) << 8;
      length |= getch();
      length |= getch();

      savelength = length;
      // Read the destination type. It should always be 'F' as flash.
      desttype = getch();

      // read a page worth of contents
      bufPtr = buff;
      do *bufPtr++ = getch();
      while (--length);

      // Read command terminator, start reply
      verifySpace();
      if (desttype == 'E') {
        while (1) ; // Error: wait for WDT
      } else {
        uint32_t addr = (((uint32_t)rampz) << 16) | address;
        // During a single bootloader run, only erase a 64kB block once.
        // An 8bit bitmask 'pages_erased' covers 512kB of FLASH memory.
        if ((address == 0) && (pages_erased & (1 << (addr >> 16))) == 0) {
          xflash_wait_busy();
          xflash_enable_wr();
          xflash_block64_erase(addr);
          pages_erased |= (1 << (addr >> 16));
        }
        xflash_wait_busy();
        xflash_enable_wr();
        xflash_page_program(addr, buff, savelength);
        xflash_wait_busy();
        xflash_disable_wr();
      }
    }
    /* Read memory block mode, length is big endian.  */
    else if(ch == STK_READ_PAGE) {
      uint32_t addr = (((uint32_t)rampz) << 16) | address;
      register pagelen_t i;
      // Read the page length, with the length transferred each nibble separately to work around
      // the Prusa's USB to serial infamous semicolon issue.
      length  = ((pagelen_t)getch()) << 8;
      length |= ((pagelen_t)getch()) << 8;
      length |= getch();
      length |= getch();
      // Read the destination type. It should always be 'F' as flash. It is not checked.
      (void)getch();
      verifySpace();
      xflash_wait_busy();
      xflash_rd_data(addr, buff, length);
      for (i = 0; i < length; ++ i)
        putch(buff[i]);
    }
    /* Get device signature bytes  */
    else if(ch == STK_READ_SIGN) {
      // READ SIGN - return what Avrdude wants to hear
      verifySpace();
      putch(XFLASH_SIGNATURE_0);
      putch(XFLASH_SIGNATURE_1);
      putch(XFLASH_SIGNATURE_2);
    }
    else if (ch == STK_LEAVE_PROGMODE) { /* 'Q' */
      // Adaboot no-wait mod
      wdt_enable(WDTO_15MS);
      verifySpace();
    }
    else {
      // This covers the response to commands like STK_ENTER_PROGMODE
      verifySpace();
    }
    putch(STK_OK);
  }
}
