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

//size=100B
bool SdFile::openFilteredGcode(SdBaseFile* dirFile, const char* path){
    if( open(dirFile, path, O_READ) ){
        gfReset(0,0);
        // compute the block to start with
        if( ! gfComputeNextFileBlock() )
            return false;
        return true;
    } else {
        return false;
    }
}

//size=90B
bool SdFile::seekSetFilteredGcode(uint32_t pos){
    bool rv = seekSet(pos);
    gfComputeNextFileBlock();
    return rv;
}

//size=50B
void SdFile::gfReset(uint32_t blk, uint16_t ofs){
    // @@TODO clean up
    gfBlock = blk;
    gfOffset = ofs;
    gfCachePBegin = vol_->cache()->data;
    // reset cache read ptr to its begin
    gfCacheP = gfCachePBegin;
}

//FORCE_INLINE const uint8_t * find_endl(const uint8_t *p){
//    while( *(++p) != '\n' ); // skip until a newline is found
//    return p;
//}

// think twice before allowing this to inline - manipulating 4B longs is costly
// moreover - this function has its parameters in registers only, so no heavy stack usage besides the call/ret
void __attribute__((noinline)) SdFile::gfUpdateCurrentPosition(uint16_t inc){
    curPosition_ += inc;
}

#define find_endl(resultP, startP) \
__asm__ __volatile__ (  \
"adiw r30, 1     \n" /* workaround the ++gfCacheP into post increment Z+ */ \
"cycle:          \n" \
"ld  r22, Z+     \n" \
"cpi r22, 0x0A   \n" \
"brne cycle      \n" \
"sbiw r30, 1     \n" /* workaround the ++gfCacheP into post increment Z+ */ \
: "=z" (resultP) /* result of the ASM code - in our case the Z register (R30:R31) */ \
: "z" (startP)   /* input of the ASM code - in our case the Z register as well (R30:R31) */ \
: "r22"          /* modifying register R22 - so that the compiler knows */ \
)

//size=400B
// avoid calling the default heavy-weight read() for just one byte
int16_t SdFile::readFilteredGcode(){
    gfEnsureBlock(); // this is unfortunate :( ... other calls are using the cache and we can loose the data block of our gcode file

    // assume, we have the 512B block cache filled and terminated with a '\n'
//    SERIAL_PROTOCOLPGM("read_byte enter:");
//    for(uint8_t i = 0; i < 16; ++i){
//        SERIAL_PROTOCOL( cacheP[i] );
//    }
    
    const uint8_t *start = gfCacheP;
    uint8_t consecutiveCommentLines = 0;
    while( *gfCacheP == ';' ){
        for(;;){

            //while( *(++gfCacheP) != '\n' ); // skip until a newline is found - suboptimal code!
            // Wondering, why this "nice while cycle" is done in such a weird way using a separate find_endl() function?
            // Have a look at the ASM code GCC produced!
            
            // At first - a separate find_endl() makes the compiler understand, 
            // that I don't need to store gfCacheP every time, I'm only interested in the final address where the '\n' was found
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
            find_endl(gfCacheP, gfCacheP);

            // found a newline, prepare the next block if block cache end reached
            if( gfCacheP - gfCachePBegin >= 512 ){
                // at the end of block cache, fill new data in
                gfUpdateCurrentPosition( gfCacheP - start );
                if( ! gfComputeNextFileBlock() )goto fail;
                gfEnsureBlock(); // fetch it into RAM
                gfCacheP = start = gfCachePBegin;
            } else {
                if(++consecutiveCommentLines == 255){
                    // SERIAL_PROTOCOLLN(sd->curPosition_);
                    goto forceExit;
                }
                // peek the next byte - we are inside the block at least at 511th index - still safe
                if( *(gfCacheP+1) == ';' ){
                    // consecutive comment
                    ++gfCacheP;
                    ++consecutiveCommentLines;
                }
                break; // found the real end of the line even across many blocks
            }
        }
    }
forceExit:
    {
        gfUpdateCurrentPosition( gfCacheP - start + 1 );
        int16_t rv = *gfCacheP++;
        
        // prepare next block if needed
        if( gfCacheP - gfCachePBegin >= 512 ){
            if( ! gfComputeNextFileBlock() )goto fail;
            // don't need to force fetch the block here, it will get loaded on the next call
            gfCacheP = gfCachePBegin;
        }    
        return rv;
    }
fail:
//    SERIAL_PROTOCOLLNPGM("CacheFAIL");
    return -1;
}

//size=100B
bool SdFile::gfEnsureBlock(){
    if ( vol_->cacheRawBlock(gfBlock, SdVolume::CACHE_FOR_READ)){
        // terminate with a '\n'
        const uint16_t terminateOfs = (fileSize_ - gfOffset) < 512 ? (fileSize_ - gfOffset) : 512U;
        vol_->cache()->data[ terminateOfs ] = '\n';
        return true;
    } else {
        return false;
    }
}

//size=350B
bool SdFile::gfComputeNextFileBlock() {
    // error if not open or write only
    if (!isOpen() || !(flags_ & O_READ)) return false;

    gfOffset = curPosition_ & 0X1FF;  // offset in block
    if (type_ == FAT_FILE_TYPE_ROOT_FIXED) {
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
