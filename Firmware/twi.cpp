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
#include <util/delay.h>
#include "config.h"
#include "fastio.h"
#include "twi.h"
#include "Timer.h"


void twi_init(void)
{
  // activate internal pullups for SDA
  SET_INPUT(SDA_PIN);
  WRITE(SDA_PIN, 1);
  
  // start with the SDA pulled low
  WRITE(SCL_PIN, 0);
  SET_OUTPUT(SCL_PIN);
  
  // clock 10 cycles to make sure that the sensor is not stuck in a register read.
  for (uint8_t i = 0; i < 10; i++) {
    WRITE(SCL_PIN, 1);
    _delay_us((1000000 / TWI_FREQ) / 2);
    WRITE(SCL_PIN, 0);
    _delay_us((1000000 / TWI_FREQ) / 2);
  }

  // activate internal pullups for SCL
  SET_INPUT(SCL_PIN);
  WRITE(SCL_PIN, 1);

  // initialize twi prescaler and bit rate
  TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

  /* twi bit rate formula from atmega128 manual pg 204
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  note: TWBR should be 10 or higher for master mode
  It is 72 for a 16mhz Wiring board with 100kHz TWI */
}

void twi_disable(void)
{
  // disable TWI hardware.
  TWCR = 0;
  
  // deactivate internal pullups for twi.
  WRITE(SDA_PIN, 0);
  WRITE(SCL_PIN, 0);
}


static void twi_stop()
{
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
}


static uint8_t twi_wait(uint8_t status)
{
  ShortTimer timmy;
  timmy.start();
  while(!(TWCR & _BV(TWINT))) {
    if (timmy.expired(TWI_TIMEOUT_MS)) {
      return 2;
    }
  }
  if(TW_STATUS != status)
  {
      twi_stop();
      return 1;
  }
  return 0;
}


static uint8_t twi_start(uint8_t address, uint8_t reg)
{
  // send start condition
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
  if(twi_wait(TW_START))
      return 1;

  // send address
  TWDR = TW_WRITE | (address << 1);
  TWCR = _BV(TWEN) | _BV(TWINT);
  if(twi_wait(TW_MT_SLA_ACK))
      return 2;

  // send register
  TWDR = reg;
  TWCR = _BV(TWEN) | _BV(TWINT);
  if(twi_wait(TW_MT_DATA_ACK))
      return 3;

  return 0;
}


uint8_t twi_check(uint8_t address)
{
    // send start condition
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
    if(twi_wait(TW_START))
        return 1;
    
      // send address
    TWDR = TW_WRITE | (address << 1);
    TWCR = _BV(TWEN) | _BV(TWINT);
    if(twi_wait(TW_MT_SLA_ACK))
        return 2;
    
    // send stop
    twi_stop();
    return 0;
}


uint8_t twi_r8(uint8_t address, uint8_t reg, uint8_t* data)
{
  if(twi_start(address, reg))
      return 1;

  // repeat start
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
  if(twi_wait(TW_REP_START))
      return 2;

  // start receiving
  TWDR = TW_READ | (address << 1);
  TWCR = _BV(TWEN) | _BV(TWINT);
  if(twi_wait(TW_MR_SLA_ACK))
      return 3;

  // receive data
  TWCR = _BV(TWEN) | _BV(TWINT);
  if(twi_wait(TW_MR_DATA_NACK))
      return 4;

  *data = TWDR;

  // send stop
  twi_stop();
  return 0;
}


uint8_t twi_w8(uint8_t address, uint8_t reg, uint8_t data)
{
  if(twi_start(address, reg))
      return 1;

  // send data
  TWDR = data;
  TWCR = _BV(TWEN) | _BV(TWINT);
  if(twi_wait(TW_MT_DATA_ACK))
      return 2;

  // send stop
  twi_stop();
  return 0;
}
