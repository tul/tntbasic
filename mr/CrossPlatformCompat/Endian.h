// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Endian.h
// TNT Library
// Mark Tully
// 16/8/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#pragma once

#include <MacTypes.h>

#if TARGET_RT_LITTLE_ENDIAN==1

#define FOUR_CHAR_CODE(x)		(((unsigned long) ((x) & 0x000000FF)) << 24) \
							| (((unsigned long) ((x) & 0x0000FF00)) << 8) \
							| (((unsigned long) ((x) & 0x00FF0000)) >> 8) \
							| (((unsigned long) ((x) & 0xFF000000)) >> 24)

inline void BEToH16(UInt16 &x)
{
    x= ((((short)( x ))<<8) | (((short)( x ))>>8)) ;
}

inline void BEToH16(SInt16 &x)
{
    x= ((((short)( x ))<<8) | (((short)( x ))>>8)) ;
}

inline void BEToH32(UInt32 &x)
{
    x= ((((long)( x )) << 24) | ((((long)( x )) << 8) & 0x00FF0000) | ((((long)( x )) >> 8) & 0x0000FF00) | ((((long)( x )) >> 24) & 0x000000FF)) ;
}

inline void BEToH32(SInt32 &x)
{
    x= ((((long)( x )) << 24) | ((((long)( x )) << 8) & 0x00FF0000) | ((((long)( x )) >> 8) & 0x0000FF00) | ((((long)( x )) >> 24) & 0x000000FF)) ;
}

inline void HToBE16(UInt16 &x)
{
    x= ((((short)( x ))<<8) | (((short)( x ))>>8)) ;
}

inline void HToBE16(SInt16 &x)
{
    x= ((((short)( x ))<<8) | (((short)( x ))>>8)) ;
}

inline void HToBE32(SInt32 &x)
{
    x= ((((long)( x )) << 24) | ((((long)( x )) << 8) & 0x00FF0000) | ((((long)( x )) >> 8) & 0x0000FF00) | ((((long)( x )) >> 24) & 0x000000FF)) ;
}

inline void HToBE32(UInt32 &x)
{
    x= ((((long)( x )) << 24) | ((((long)( x )) << 8) & 0x00FF0000) | ((((long)( x )) >> 8) & 0x0000FF00) | ((((long)( x )) >> 24) & 0x000000FF)) ;
}

#elif TARGET_RT_BIG_ENDIAN==1

#ifndef FOUR_CHAR_CODE
	#define FOUR_CHAR_CODE(x) x
#endif

inline void BEToH16(UInt16 &x)
{
}

inline void BEToH16(SInt16 &x)
{
}

inline void BEToH32(UInt32 &x)
{
}

inline void BEToH32(SInt32 &x)
{
}

inline void HToBE16(UInt16 &x)
{
}

inline void HToBE16(SInt16 &x)
{
}

inline void HToBE32(SInt32 &x)
{
}

inline void HToBE32(UInt32 &x)
{
}

#else

//#warning Endian.h can't figure out the Endian-ness of this machine!

#endif