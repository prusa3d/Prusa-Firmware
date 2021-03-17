//! @file
// Based on the OptiBoot project
// https://github.com/Optiboot/optiboot
// Licence GLP 2 or later.

#include "Marlin.h"
#include "w25x20cl.h"
#include "stk500.h"
#include "bootapp.h"
#include <avr/wdt.h>

#define OPTIBOOT_MAJVER 6
#define OPTIBOOT_CUSTOMVER 0
#define OPTIBOOT_MINVER 2
static unsigned const int __attribute__((section(".version"))) 
  optiboot_version = 256*(OPTIBOOT_MAJVER + OPTIBOOT_CUSTOMVER) + OPTIBOOT_MINVER;

#if 0
#define W25X20CL_SIGNATURE_0 9
#define W25X20CL_SIGNATURE_1 8
#define W25X20CL_SIGNATURE_2 7
#else
//FIXME this is a signature of ATmega2560!
#define W25X20CL_SIGNATURE_0 0x1E
#define W25X20CL_SIGNATURE_1 0x98
#define W25X20CL_SIGNATURE_2 0x01
#endif

#define RECV_READY ((UCSR0A & _BV(RXC0)) != 0)

static uint8_t getch(void) {
  uint8_t ch;
  while(! RECV_READY) ;
  if (!(UCSR0A & _BV(FE0))) {
      /*
       * A Framing Error indicates (probably) that something is talking
       * to us at the wrong bit rate.  Assume that this is because it
       * expects to be talking to the application, and DON'T reset the
       * watchdog.  This should cause the bootloader to abort and run
       * the application "soon", if it keeps happening.  (Note that we
       * don't care that an invalid char is returned...)
       */
    wdt_reset();
  }
  ch = UDR0;
  return ch;
}

static void putch(char ch) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = ch;
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

static const char entry_magic_send   [] PROGMEM = "start\n";
static const char entry_magic_receive[] PROGMEM = "w25x20cl_enter\n";
static const char entry_magic_cfm    [] PROGMEM = "w25x20cl_cfm\n";

struct block_t;
extern struct block_t *block_buffer;

//! @brief Enter an STK500 compatible Optiboot boot loader waiting for flashing the languages to an external flash memory.
//! @return 1 if "start\n" was not sent. Optiboot was skipped
//! @return 0 if "start\n" was sent. Optiboot ran normally. No need to send "start\n" in setup()
uint8_t optiboot_w25x20cl_enter()
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
    const char    *ptr = entry_magic_send;
    const char    *end = strlen_P(entry_magic_send) + ptr;
    const uint8_t selectedSerialPort_bak = selectedSerialPort;
    // Flush the serial line.
    while (RECV_READY) {
      wdt_reset();
      // Dummy register read (discard)
      (void)(*(char *)UDR0);
    }
    selectedSerialPort = 0; //switch to Serial0
    MYSERIAL.flush(); //clear RX buffer
    int SerialHead = rx_buffer.head;
    // Send the initial magic string.
    while (ptr != end)
      putch(pgm_read_byte(ptr ++));
    wdt_reset();
    // Wait for two seconds until a magic string (constant entry_magic) is received
    // from the serial line.
    ptr = entry_magic_receive;
    end = strlen_P(entry_magic_receive) + ptr;
    while (ptr != end) {
      unsigned long  boot_timer = 2000000;
      // Beware of this volatile pointer - it is important since the while-cycle below
      // doesn't contain any obvious references to rx_buffer.head
      // thus the compiler is allowed to remove the check from the cycle
      // i.e. rx_buffer.head == SerialHead would not be checked at all!
      // With the volatile keyword the compiler generates exactly the same code as without it with only one difference:
      // the last brne instruction jumps onto the (*rx_head == SerialHead) check and NOT onto the wdr instruction bypassing the check.
      volatile int *rx_head = &rx_buffer.head;
      while (*rx_head == SerialHead) {
        wdt_reset();
        if ( --boot_timer == 0) {
          // Timeout expired, continue with the application.
          selectedSerialPort = selectedSerialPort_bak; //revert Serial setting
          return 0;
        }
      }
      ch = rx_buffer.buffer[SerialHead];
      SerialHead = (unsigned int)(SerialHead + 1) % RX_BUFFER_SIZE;
      if (pgm_read_byte(ptr ++) != ch)
      {
          // Magic was not received correctly, continue with the application
          selectedSerialPort = selectedSerialPort_bak; //revert Serial setting
          return 0;
      }
      wdt_reset();
    }
    cbi(UCSR0B, RXCIE0); //disable the MarlinSerial0 interrupt
    // Send the cfm magic string.
    ptr = entry_magic_cfm;
    end = strlen_P(entry_magic_cfm) + ptr;
    while (ptr != end)
      putch(pgm_read_byte(ptr ++));
  }

  spi_init();
  w25x20cl_init();
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
          w25x20cl_wait_busy();
          w25x20cl_enable_wr();
          w25x20cl_block64_erase(addr);
          pages_erased |= (1 << (addr >> 16));
        }
        w25x20cl_wait_busy();
        w25x20cl_enable_wr();
        w25x20cl_page_program(addr, buff, savelength);
        w25x20cl_wait_busy();
        w25x20cl_disable_wr();
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
      w25x20cl_wait_busy();
      w25x20cl_rd_data(addr, buff, length);
      for (i = 0; i < length; ++ i)
        putch(buff[i]);
    }
    /* Get device signature bytes  */
    else if(ch == STK_READ_SIGN) {
      // READ SIGN - return what Avrdude wants to hear
      verifySpace();
      putch(W25X20CL_SIGNATURE_0);
      putch(W25X20CL_SIGNATURE_1);
      putch(W25X20CL_SIGNATURE_2);
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
