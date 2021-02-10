/* Arduino SdFat Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * \file
 * \brief SdFile class
 */
#include "Marlin.h"

#ifdef SDSUPPORT
#include "SdBaseFile.h"
//#include <Print.h>
#ifndef SdFile_h
#define SdFile_h
//------------------------------------------------------------------------------
/**
 * \class SdFile
 * \brief SdBaseFile with Print.
 */
class SdFile : public SdBaseFile/*, public Print*/ {
  // GCode filtering vars and methods - due to optimization reasons not wrapped in a separate class
  
  // beware - this read ptr is manipulated inside just 2 methods - readFilteredGcode and gfReset
  // If you even want to call gfReset from readFilteredGcode, you must make sure
  // to update gfReadPtr inside readFilteredGcode from a local copy (see explanation of this trick in readFilteredGcode)
  const uint8_t *gfReadPtr;
  
  uint32_t gfBlock; // remember the current file block to be kept in cache - due to reuse of the memory, the block may fall out a must be read back
  uint16_t gfOffset;

  const uint8_t *gfBlockBuffBegin()const;
  
  void gfReset();
  
  bool gfEnsureBlock();
  bool gfComputeNextFileBlock();
  void gfUpdateCurrentPosition(uint16_t inc);
public:
  SdFile() {}
  SdFile(const char* name, uint8_t oflag);
  #if ARDUINO >= 100
      size_t write(uint8_t b);
  #else
   void write(uint8_t b);
  #endif
  
  bool openFilteredGcode(SdBaseFile* dirFile, const char* path);
  int16_t readFilteredGcode();
  bool seekSetFilteredGcode(uint32_t pos);
  int16_t write(const void* buf, uint16_t nbyte);
  void write(const char* str);
  void write_P(PGM_P str);
  void writeln_P(PGM_P str);
};
#endif  // SdFile_h


#endif
