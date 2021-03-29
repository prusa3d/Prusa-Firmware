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
#include "Marlin.h"

#ifdef SDSUPPORT
#include "SdFile.h"
/**  Create a file object and open it in the current working directory.
 *
 * \param[in] path A path with a valid 8.3 DOS name for a file to be opened.
 *
 * \param[in] oflag Values for \a oflag are constructed by a bitwise-inclusive
 * OR of open flags. see SdBaseFile::open(SdBaseFile*, const char*, uint8_t).
 */
SdFile::SdFile(const char* path, uint8_t oflag) : SdBaseFile(path, oflag) {
}

bool SdFile::openFilteredGcode(SdBaseFile* dirFile, const char* path){
    if( open(dirFile, path, O_READ) ){
        // compute the block to start with
        if( ! gfComputeNextFileBlock() )
            return false;
        gfReset();
        return true;
    } else {
        return false;
    }
}

bool SdFile::seekSetFilteredGcode(uint32_t pos){
    if(! seekSet(pos) )return false;
    if(! gfComputeNextFileBlock() )return false;
    gfReset();
    return true;
}

const uint8_t *SdFile::gfBlockBuffBegin() const {
    return vol_->cache()->data; // this is constant for the whole time, so it should be fast and sleek
}

void SdFile::gfReset(){
    // reset cache read ptr to its begin
    gfReadPtr = gfBlockBuffBegin() + gfOffset;
}

// think twice before allowing this to inline - manipulating 4B longs is costly
// moreover - this function has its parameters in registers only, so no heavy stack usage besides the call/ret
void __attribute__((noinline)) SdFile::gfUpdateCurrentPosition(uint16_t inc){
    curPosition_ += inc;
}

#define find_endl(resultP, startP) \
__asm__ __volatile__ (  \
"cycle:          \n" \
"ld  r22, Z+     \n" \
"cpi r22, 0x0A   \n" \
"brne cycle      \n" \
: "=z" (resultP) /* result of the ASM code - in our case the Z register (R30:R31) */ \
: "z" (startP)   /* input of the ASM code - in our case the Z register as well (R30:R31) */ \
: "r22"          /* modifying register R22 - so that the compiler knows */ \
)

// avoid calling the default heavy-weight read() for just one byte
int16_t SdFile::readFilteredGcode(){
    if( ! gfEnsureBlock() ){
        goto eof_or_fail; // this is unfortunate :( ... other calls are using the cache and we can loose the data block of our gcode file
    }
    // assume, we have the 512B block cache filled and terminated with a '\n'
    {
    const uint8_t *start = gfReadPtr;

    // It may seem unreasonable to copy the variable into a local one and copy it back at the end of this method,
    // but there is an important point of view: the compiler is unsure whether it can optimize the reads/writes
    // to gfReadPtr within this method, because it is a class member variable. 
    // The compiler cannot see, if omitting read/write won't have any incorrect side-effects to the rest of the whole FW.
    // So this trick explicitly states, that rdPtr is a local variable limited to the scope of this method,
    // therefore the compiler can omit read/write to it (keep it in registers!) as it sees fit.
    // And it does! Codesize dropped by 68B!
    const uint8_t *rdPtr = gfReadPtr;

    // the same applies to gfXBegin, codesize dropped another 100B!
    const uint8_t *blockBuffBegin = gfBlockBuffBegin();
    
    uint8_t consecutiveCommentLines = 0;
    while( *rdPtr == ';' ){
        for(;;){

            //while( *(++gfReadPtr) != '\n' ); // skip until a newline is found - suboptimal code!
            // Wondering, why this "nice while cycle" is done in such a weird way using a separate find_endl() function?
            // Have a look at the ASM code GCC produced!
            
            // At first - a separate find_endl() makes the compiler understand, 
            // that I don't need to store gfReadPtr every time, I'm only interested in the final address where the '\n' was found
            // - the cycle can run on CPU registers only without touching memory besides reading the character being compared.
            // Not only makes the code run considerably faster, but is also 40B shorter!
            // This was the generated code:
            //FORCE_INLINE const uint8_t * find_endl(const uint8_t *p){
            //   while( *(++p) != '\n' ); // skip until a newline is found
            //   return p; }
            //   11c5e:	movw	r30, r18
            //   11c60:	subi	r18, 0xFF	; 255
            //   11c62:	sbci	r19, 0xFF	; 255
            //   11c64:	ld	r22, Z
            //   11c66:	cpi	r22, 0x0A	; 10
            //   11c68:	brne	.-12     	; 0x11c5e <get_command()+0x524>            

            // Still, even that was suboptimal as the compiler seems not to understand the usage of ld r22, Z+ (the plus is important)
            // aka automatic increment of the Z register (R30:R31 pair)
            // There is no other way than pure ASM!
            find_endl(rdPtr, rdPtr);

            // found a newline, prepare the next block if block cache end reached
            if( rdPtr - blockBuffBegin > 512 ){
                // at the end of block cache, fill new data in
                gfUpdateCurrentPosition( rdPtr - start - 1 );
                if( ! gfComputeNextFileBlock() )goto eof_or_fail;
                if( ! gfEnsureBlock() )goto eof_or_fail; // fetch it into RAM
                rdPtr = start = blockBuffBegin;
            } else {
                if(consecutiveCommentLines >= 250){
                    --rdPtr; // unget the already consumed newline
                    goto emit_char;
                }
                // peek the next byte - we are inside the block at least at 511th index - still safe
                if( *rdPtr == ';' ){
                    // consecutive comment
                    ++consecutiveCommentLines;
                } else {
                    --rdPtr; // unget the already consumed newline
                    goto emit_char;
                }
                break; // found the real end of the line even across many blocks
            }
        }
    }
emit_char:
    {
        gfUpdateCurrentPosition( rdPtr - start + 1 );
        int16_t rv = *rdPtr++;
        
        if( curPosition_ >= fileSize_ ){
            // past the end of file
            goto eof_or_fail;
        } else if( rdPtr - blockBuffBegin >= 512 ){
            // past the end of current bufferred block - prepare the next one...
            if( ! gfComputeNextFileBlock() )goto eof_or_fail;
            // don't need to force fetch the block here, it will get loaded on the next call
            rdPtr = blockBuffBegin;
        }

        // save the current read ptr for the next run
        gfReadPtr = rdPtr;
        return rv;
    }

}

eof_or_fail:
    // make the rdptr point to a safe location - end of file
    gfReadPtr = gfBlockBuffBegin() + 512;
    return -1;
}

bool SdFile::gfEnsureBlock(){
    // this comparison is heavy-weight, especially when there is another one inside cacheRawBlock
    // but it is necessary to avoid computing of terminateOfs if not needed
    if( gfBlock != vol_->cacheBlockNumber_ ){
        if ( ! vol_->cacheRawBlock(gfBlock, SdVolume::CACHE_FOR_READ)){
            return false;
        }
        // terminate with a '\n'
        const uint32_t terminateOfs = fileSize_ - gfOffset;
        vol_->cache()->data[ terminateOfs < 512 ? terminateOfs : 512 ] = '\n';
    }
    return true;
}

bool SdFile::gfComputeNextFileBlock() {
    // error if not open or write only
    if (!isOpen() || !(flags_ & O_READ)) return false;

    gfOffset = curPosition_ & 0X1FF;  // offset in block
    if (type_ == FAT_FILE_TYPE_ROOT_FIXED) {
        // SHR by 9 means skip the last byte and shift just 3 bytes by 1
        // -> should be 8 instructions... and not the horrible loop shifting 4 bytes at once
        // still need to get some work on this
        gfBlock = vol_->rootDirStart() + (curPosition_ >> 9); 
    } else {
        uint8_t blockOfCluster = vol_->blockOfCluster(curPosition_);
        if (gfOffset == 0 && blockOfCluster == 0) {
            // start of new cluster
            if (curPosition_ == 0) {
                // use first cluster in file
                curCluster_ = firstCluster_;
            } else {
                // get next cluster from FAT
                if (!vol_->fatGet(curCluster_, &curCluster_)) return false;
            }
        }
        gfBlock = vol_->clusterStartBlock(curCluster_) + blockOfCluster;
    }
    return true;
}

//------------------------------------------------------------------------------
/** Write data to an open file.
 *
 * \note Data is moved to the cache but may not be written to the
 * storage device until sync() is called.
 *
 * \param[in] buf Pointer to the location of the data to be written.
 *
 * \param[in] nbyte Number of bytes to write.
 *
 * \return For success write() returns the number of bytes written, always
 * \a nbyte.  If an error occurs, write() returns -1.  Possible errors
 * include write() is called before a file has been opened, write is called
 * for a read-only file, device is full, a corrupt file system or an I/O error.
 *
 */
int16_t SdFile::write(const void* buf, uint16_t nbyte) {
  return SdBaseFile::write(buf, nbyte);
}
//------------------------------------------------------------------------------
/** Write a byte to a file. Required by the Arduino Print class.
 * \param[in] b the byte to be written.
 * Use writeError to check for errors.
 */
#if ARDUINO >= 100
size_t SdFile::write(uint8_t b)
{
    return SdBaseFile::write(&b, 1);
}
#else
void SdFile::write(uint8_t b)
{
    SdBaseFile::write(&b, 1);
}
#endif
//------------------------------------------------------------------------------
/** Write a string to a file. Used by the Arduino Print class.
 * \param[in] str Pointer to the string.
 * Use writeError to check for errors.
 */
void SdFile::write(const char* str) {
  SdBaseFile::write(str, strlen(str));
}
//------------------------------------------------------------------------------
/** Write a PROGMEM string to a file.
 * \param[in] str Pointer to the PROGMEM string.
 * Use writeError to check for errors.
 */
void SdFile::write_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) write(c);
}
//------------------------------------------------------------------------------
/** Write a PROGMEM string followed by CR/LF to a file.
 * \param[in] str Pointer to the PROGMEM string.
 * Use writeError to check for errors.
 */
void SdFile::writeln_P(PGM_P str) {
  write_P(str);
  write_P(PSTR("\r\n"));
}


#endif
