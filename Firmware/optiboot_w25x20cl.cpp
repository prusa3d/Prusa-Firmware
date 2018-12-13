// Based on the OptiBoot project
// https://github.com/Optiboot/optiboot
// Licence GLP 2 or later.

#include "Marlin.h"
#include "w25x20cl.h"
#include "stk500.h"
#include "bootapp.h"

#define OPTIBOOT_MAJVER 6
#define OPTIBOOT_CUSTOMVER 0
#define OPTIBOOT_MINVER 2
static unsigned const int __attribute__((section(".version"))) 
  optiboot_version = 256*(OPTIBOOT_MAJVER + OPTIBOOT_CUSTOMVER) + OPTIBOOT_MINVER;

/* Watchdog settings */
#define WATCHDOG_OFF    (0)
#define WATCHDOG_16MS   (_BV(WDE))
#define WATCHDOG_32MS   (_BV(WDP0) | _BV(WDE))
#define WATCHDOG_64MS   (_BV(WDP1) | _BV(WDE))
#define WATCHDOG_125MS  (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_250MS  (_BV(WDP2) | _BV(WDE))
#define WATCHDOG_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_1S     (_BV(WDP2) | _BV(WDP1) | _BV(WDE))
#define WATCHDOG_2S     (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_4S     (_BV(WDP3) | _BV(WDE))
#define WATCHDOG_8S     (_BV(WDP3) | _BV(WDP0) | _BV(WDE))

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

static void watchdogConfig(uint8_t x) {
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = x;
}

static void watchdogReset() {
  __asm__ __volatile__ (
    "wdr\n"
  );
}

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
    watchdogReset();
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
    watchdogConfig(WATCHDOG_16MS);    // shorten WD timeout
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

void optiboot_w25x20cl_enter()
{
  if (boot_app_flags & BOOT_APP_FLG_USER0) return;
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
    watchdogReset();
    unsigned long  boot_timeout = 2000000;
    unsigned long  boot_timer = 0;
    const char    *ptr = entry_magic_send;
    const char    *end = strlen_P(entry_magic_send) + ptr;
    // Initialize the serial line.
    UCSR0A |= (1 << U2X0);
    UBRR0L = (((float)(F_CPU))/(((float)(115200))*8.0)-1.0+0.5);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Flush the serial line.
    while (RECV_READY) {
      watchdogReset();
      // Dummy register read (discard)
      (void)(*(char *)UDR0);
    }
    // Send the initial magic string.
    while (ptr != end)
      putch(pgm_read_byte(ptr ++));
    watchdogReset();
    // Wait for one second until a magic string (constant entry_magic) is received
    // from the serial line.
    ptr = entry_magic_receive;
    end = strlen_P(entry_magic_receive) + ptr;
    while (ptr != end) {
      while (! RECV_READY) {
        watchdogReset();
        delayMicroseconds(1);
        if (++ boot_timer > boot_timeout)
          // Timeout expired, continue with the application.
          return;
      }
      ch = UDR0;
      if (pgm_read_byte(ptr ++) != ch)
          // Magic was not received correctly, continue with the application
          return;
      watchdogReset();
    }
    // Send the cfm magic string.
    ptr = entry_magic_cfm;
    while (ptr != end)
      putch(pgm_read_byte(ptr ++));
  }

  spi_init();
  w25x20cl_init();
  watchdogConfig(WATCHDOG_OFF);

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
        if (address == 0 && (pages_erased & (1 << addr)) == 0) {
          w25x20cl_wait_busy();
          w25x20cl_enable_wr();
          w25x20cl_block64_erase(addr);
          pages_erased |= (1 << addr);
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
      watchdogConfig(WATCHDOG_16MS);
      verifySpace();
    }
    else {
      // This covers the response to commands like STK_ENTER_PROGMODE
      verifySpace();
    }
    putch(STK_OK);
  }
}
