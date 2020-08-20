/*
  twi.h - Stripped-down TWI/I2C library
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
*/

#pragma once

#include <inttypes.h>
#include <compat/twi.h>

#ifndef TWI_FREQ
#define TWI_FREQ 400000L
#endif

/*
 * Function twi_init
 * Desc     readys twi pins and sets twi bitrate
 * Input    none
 * Output   none
 */
void twi_init(void);

/*
 * Function twi_disable
 * Desc     disables twi pins
 * Input    none
 * Output   none
 */
void twi_disable(void);

/*
 * Function twi_rw8
 * Desc     read/write a single byte from a device
 * Input    address: 7bit i2c device address
 *          mode: TW_READ or TW_WRITE
 *          data: pointer to byte
 * Output   0 on success
 */
uint8_t twi_rw8(uint8_t address, uint8_t mode, uint8_t* data);
