/*
  twi.c - Stripped-down TWI/I2C library
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
*/

#include <math.h>
#include "Arduino.h" // for digitalWrite

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "twi.h"


void twi_init(void)
{
  // activate internal pullups for twi.
  digitalWrite(SDA, 1);
  digitalWrite(SCL, 1);

  // initialize twi prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

  /* twi bit rate formula from atmega128 manual pg 204
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  note: TWBR should be 10 or higher for master mode
  It is 72 for a 16mhz Wiring board with 100kHz TWI */
}

void twi_disable(void)
{
  // deactivate internal pullups for twi.
  digitalWrite(SDA, 0);
  digitalWrite(SCL, 0);
}

static void twi_wait()
{
    while(!(TWCR & _BV(TWINT)));
}

uint8_t twi_rw8(uint8_t address, uint8_t mode, uint8_t* data)
{
  // send start condition
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
  twi_wait();
  if(TW_STATUS != TW_START)
      return 1;

  // send address
  TWDR = mode;
  TWDR |= (address << 1);
  TWCR = _BV(TWEN) | _BV(TWINT);
  twi_wait();

  if(mode == TW_WRITE)
  {
      if(TW_STATUS != TW_MT_SLA_ACK)
          return 2;

      // send data
      TWDR = *data;
      TWCR = _BV(TWEN) | _BV(TWINT);
      twi_wait();
      if(TW_STATUS != TW_MT_DATA_ACK)
          return 3;
  }
  else
  {
      if(TW_STATUS != TW_MR_SLA_ACK)
          return 2;

      // receive data
      TWCR = _BV(TWEN) | _BV(TWINT);
      twi_wait();

      // accept ACK or NACK (since only 1 byte is read)
      if(!(TW_STATUS & TW_MR_DATA_ACK))
          return 3;

      *data = TWDR;
  }

  // send stop
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);

  return 0;
}
