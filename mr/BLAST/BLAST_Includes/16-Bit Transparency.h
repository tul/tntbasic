// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ'
//	16-Bit Transparency.h
//	By Richard Mckeon
//	28/10/97
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ'

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1997, Mark Tully and John Treece-Birch
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

#ifdef __cplusplus
extern "C" {
#endif

//	Global
extern UInt8 gAlphaColourTable[256][32][32];

//	Prototypes
void AlphaColour_Init();
UInt16 AlphaColour(UInt16 pAlpha,UInt16 pColor1,UInt16 pColor2);
void TransCopy(BCPtr source,BCPtr dest,Rect *sourceRect,Rect *destRect,UInt16 pAlpha);
void TransCopyMask(BCPtr source,BCPtr dest,Rect *sourceRect,Rect *destRect,UInt16 pAlpha,unsigned short maskColour);
UInt16 FastAlphaColour128(UInt16 pColour1,UInt16 pColour2);

#ifdef __cplusplus
}
#endif

/*	How it works :  A 16-Bit pixel is made up of 3 5-bit channels, 1 for red, 1 for blue
and 1 for green, and 1 bit that doesn't do anything.  When doing a transparency between 2
pixels, each channel from one pixel is compared with the same channel from the other pixel i.e.
red and red.  However, there are only 32 different values for each channel, and the same
transformation is done on all channels.  So for example, a 50% transparency between 25 red and
16 red would give the same number as between 25 green and 16 green.  If you have a finite
number of alpha (transparency) levels, you can just calculate a 3d look up table, and for each
channel access it to get the required result.  Then just bit shift and or the results together.
*/