// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ'
//	16-Bit Transparency.c
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

//	Original code by some bloke on MAC-GAMES-DEV

#include "Blast.h"
#include "16-Bit Transparency.h"

UInt8 gAlphaColourTable[256][32][32];

void AlphaColour_Init()
{
     UInt16 alpha,not_alpha;

     for (alpha = 0;alpha < 256;alpha++)
     {
          UInt16 not_alpha = 255 - alpha;
          short c1,c2;

          for (c1 = 0;c1 < 32;c1++)
               for (c2 = 0;c2 < 32;c2++)
                    gAlphaColourTable[alpha][c1][c2] = ((alpha * c1) + (not_alpha * c2)) / 256;
     }
}

UInt16 AlphaColour(UInt16 pAlpha,UInt16 pColor1,UInt16 pColor2)
{
     UInt16 r1, r2, g1, g2, b1, b2, result;

     r1 = (pColor1 >> 10) & 0x1f;
     g1 = (pColor1 >>  5) & 0x1f;
     b1 = (pColor1      ) & 0x1f;

     r2 = (pColor2 >> 10) & 0x1f;
     g2 = (pColor2 >>  5) & 0x1f;
     b2 = (pColor2      ) & 0x1f;

     result  = gAlphaColourTable[pAlpha][r1][r2] << 10;
     result |= gAlphaColourTable[pAlpha][g1][g2] << 5;
     result |= gAlphaColourTable[pAlpha][b1][b2];

     return result;
}

void TransCopy(BCPtr source,BCPtr dest,Rect *sourceRect,Rect *destRect,UInt16 pAlpha)
{
	unsigned char	*srcPtr,*destPtr,*srcRowStart,*destRowStart;
	unsigned long	miscCount;
	unsigned long	yCount;
	unsigned long	heightToCopy=sourceRect->bottom-sourceRect->top;
	unsigned long	widthToCopy=sourceRect->right-sourceRect->left;
	unsigned long	srcRB,destRB;
	
	srcRB=source->rowBytes;
	destRB=dest->rowBytes;
	srcRowStart=source->baseAddr+srcRB*sourceRect->top+sourceRect->left*2;
	destRowStart=dest->baseAddr+destRB*destRect->top+destRect->left*2;
	
	for (yCount=0; yCount<heightToCopy; yCount++)
	{
		srcPtr=srcRowStart;
		srcRowStart+=srcRB;
		destPtr=destRowStart;
		destRowStart+=destRB;
		
		miscCount=widthToCopy;
	
	/*	// Move the data in the biggest chunks possible
		#ifdef powerc
		// raa PowerPC 601, 8 bytes in one move command
		if (g601Chip)
		{	
			while (miscCount>=sizeof(double))
			{
				*((double *)destPtr)++=*((double *)srcPtr)++;
				miscCount-=sizeof(double);
			}
		}
		#endif
		
		while(miscCount>=sizeof(unsigned long))
		{
			*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
			miscCount-=sizeof(unsigned long);				
		}*/
		
		while(miscCount>=sizeof(unsigned short)/2)
		{
			*((unsigned short *)destPtr)++=AlphaColour(pAlpha,*((unsigned short *)srcPtr)++,*((unsigned short *)destPtr));
			miscCount-=sizeof(unsigned short)/2;				
		}
	}
}

void TransCopyMask(BCPtr source,BCPtr dest,Rect *sourceRect,Rect *destRect,UInt16 pAlpha,unsigned short maskColour)
{
	unsigned char	*srcPtr,*destPtr,*srcRowStart,*destRowStart;
	unsigned long	miscCount;
	unsigned long	yCount;
	unsigned long	heightToCopy=sourceRect->bottom-sourceRect->top;
	unsigned long	widthToCopy=sourceRect->right-sourceRect->left;
	unsigned long	srcRB,destRB;
	
	srcRB=source->rowBytes;
	destRB=dest->rowBytes;
	srcRowStart=source->baseAddr+srcRB*sourceRect->top+sourceRect->left*2;
	destRowStart=dest->baseAddr+destRB*destRect->top+destRect->left*2;
	
	for (yCount=0; yCount<heightToCopy; yCount++)
	{
		srcPtr=srcRowStart;
		srcRowStart+=srcRB;
		destPtr=destRowStart;
		destRowStart+=destRB;
		
		miscCount=widthToCopy;
	
	/*	// Move the data in the biggest chunks possible
		#ifdef powerc
		// raa PowerPC 601, 8 bytes in one move command
		if (g601Chip)
		{	
			while (miscCount>=sizeof(double))
			{
				*((double *)destPtr)++=*((double *)srcPtr)++;
				miscCount-=sizeof(double);
			}
		}
		#endif
		
		while(miscCount>=sizeof(unsigned long))
		{
			*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
			miscCount-=sizeof(unsigned long);				
		}*/
		
		while(miscCount>=sizeof(unsigned short)/2)
		{
			if(*((short *)srcPtr)!=maskColour)
				*((unsigned short *)destPtr)++=AlphaColour(pAlpha,*((unsigned short *)srcPtr)++,*((unsigned short *)destPtr));
			else
			{
				*((unsigned short *)destPtr)++;
				*((unsigned short *)srcPtr)++;
			}
			miscCount-=sizeof(unsigned short)/2;				
		}
		
		/*while(miscCount>=sizeof(unsigned char))
		{
			
			*((unsigned char *)destPtr)++=AlphaColor(pAlpha,*((unsigned char *)srcPtr)++,*((unsigned char *)destPtr));
			miscCount-=sizeof(unsigned char);				
		}*/
	}
}

UInt16 FastAlphaColour128(UInt16 pColour1,UInt16 pColour2)
{
//	UInt16 result;
//	pColour1&=0x7FFF;
//	pColour2&=0x7FFF;
	
	pColour1>>=1;
	pColour2>>=1;
	
	pColour1&=0x3DEF;
	pColour2&=0x3DEF;
	
//	result=pColour1+pColour2;
	
//	result&=0x7DEF;
	
//	return result;
	return pColour1+pColour2;
}
