// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// PixelDoublePPC.c
// Mark Tully
// ??/??/95
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1995, Mark Tully and John Treece-Birch
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or other
*   materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
* ***** END LICENSE BLOCK ***** */

// Pixel doubling routine
// Not by Mark Tully at all. Nope.

#include	"Marks Routines.h"

// rows = scan lines to double
// words is the number of pixels to double/4

asm void PixelDouble(
    register char *dstbase,
    register UInt32 dstrowbytes,
    register char *srcbase,
    register UInt32 srcrowbytes,
    register UInt32 rows,
    register UInt32 words)
{
#define srcptr  r9
#define dstptr1 r10
#define dstptr2 r11
#define src     r12
#define dst1    r30
#define dst2    r31
#define dst     fp0

// check for null rects on paranoid principles
    cmpwi   rows,0
    beqlr
    cmpwi   words,0
    beqlr
// we need two nonvolatile registers, so we save them here
    stw     r30,-8(sp)
    stw     r31,-4(sp)
// adjust the pointers for preincrement addressing
    addi    dstbase,dstbase,-8
    addi    srcbase,srcbase,-4
rowloop:
// set up the source and destination pointers
    mr      dstptr1,dstbase
    add     dstptr2,dstbase,dstrowbytes
    mr      srcptr,srcbase
    add     srcbase,srcbase,srcrowbytes
    add     dstbase,dstptr2,dstrowbytes
// we use the counter register for the inner loop
    mtctr   words
wordloop:
// get four source pixels
    lwzu    src,4(srcptr)
// do the horizontal expansion
    rlwinm  dst1,src,0,0,7
    rlwinm  dst2,src,0,24,31
    rlwimi  dst1,src,24,8,23
    rlwimi  dst2,src,8,8,23
    rlwimi  dst1,src,16,24,31
    rlwimi  dst2,src,16,0,7
// move the expanded pixels into a fp register by way of (cached) memory
    stw     dst1,-16(sp)
    stw     dst2,-12(sp)
    lfd     dst,-16(sp)
// put sixteen destination pixels
    stfdu   dst,8(dstptr1)
    stfdu   dst,8(dstptr2)
// end of inner loop
    bdnz    wordloop
// end of outer loop
    cmpwi   rows,1
    addi    rows,rows,-1
    bne     rowloop
// we're done.  restore the saved registers and return
    lwz     r30,-8(sp)
    lwz     r31,-4(sp)
    blr
#undef srcptr
#undef dstptr1
#undef dstptr2
#undef src
#undef dst1
#undef dst
}
