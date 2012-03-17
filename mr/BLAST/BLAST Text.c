/***************************************************************************************************

BLAST Copy.c
Extension of BLASTª engine
Mark Tully

5/12/96	:	Took the code out of the Spy Game engine and generalised it.

SECTION 1 - General

***************************************************************************************************/

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

#include	"BLAST.h"

OSErr LoadBlastText(BTIPtr theInfo,short theFile,short theResId)
{
	GWorldPtr		theWorld;
	OSErr			theErr;
	BTEHandle		theBits;
	
	theInfo->lCoords=0L;
	theInfo->textRec.world=0L;
	
	theBits=(BTEHandle)BetterGetResource(theFile,kBlastTextExtrasResType,theResId);
	if (!theBits)
		return BetterGetResErr();
	
	theInfo->spaceWidth=(**theBits).spaceWidth;
	theInfo->remapMe=(**theBits).remapCol;
	theInfo->transp=(**theBits).transpCol;
	
	BetterDisposeHandle((Handle)theBits);
	
	if (theErr=NewGWorldWithPic(&theWorld,theResId,8))
		return theErr;
	MakeBCRecFromGWorld(theWorld,&theInfo->textRec);
	
	theInfo->lCoords=(Rect**)BetterGetResource(theFile,kLetterCoordResType,theResId);
	if (!theInfo->lCoords)
	{
		DisposeGWorld(theWorld);
		return BetterGetResErr();
	}
	
	HLock((Handle)theInfo->lCoords);
	
	return noErr;
}

void DisposeBlastText(BTIPtr theInfo)
{
	DisposeGWorld(theInfo->textRec.world);
	BetterDisposeHandle((Handle)theInfo->lCoords);
}

// all this routine does is write out the specified string at the specified coords
// a colour can be stated to be remapped to another
void BlastDrawString(BCPtr theRec,BTIPtr theInfo,StringPtr theString,short x,short y,unsigned char col)
{
	Byte					loopCounter,letterID;
	unsigned char			*destPtr,*sourcePtr;
	short					sourceRB,destRB;
	short					width,height,rowsDone=0;
	Rect					letterRect;
	short					pixWidth=theRec->world->portRect.right-theRec->world->portRect.left;
	short					pixHeight=theRec->world->portRect.bottom-theRec->world->portRect.top;
	
	destRB=theRec->rowBytes;
	sourceRB=theInfo->textRec.rowBytes;
	
	for (loopCounter=1; loopCounter<=theString[0]; loopCounter++)
	{
		// draw the specified char at the current location
		// move the source & dest ptr to the correct pos
		if (theString[loopCounter]==' ' || theString[loopCounter]=='\r')
		{
			x+=theInfo->spaceWidth;
			continue;
		}
		
		letterID=BlastGetLetterId(theString[loopCounter]);
		
		letterRect=(*theInfo->lCoords)[letterID];
		height=letterRect.bottom-letterRect.top;
		rowsDone=0;
		
		if (y+height>=pixHeight)
			return;
		
		// copy the char, 1 byte at a time
		while (height>0)
		{
			destPtr=theRec->baseAddr+(y * destRB)+x;
			width=letterRect.right-letterRect.left;
			sourcePtr=theInfo->textRec.baseAddr + (letterRect.top+rowsDone)*sourceRB + letterRect.left;
			
			if (x+width>=pixWidth)
				return;
			
			while (width>0)
			{
				if (*sourcePtr==theInfo->remapMe)
					*destPtr=col;
				else if (*sourcePtr!=theInfo->transp)
					*destPtr=*sourcePtr;
				sourcePtr++;
				destPtr++;
				width--;
			}
			
			y++;
			height--;
			rowsDone++;
		}

		// move the pen coords
		x+=(letterRect.right-letterRect.left)-1;	// -1 to condense the text by a pixel
		y-=letterRect.bottom-letterRect.top;	// move the y counter back up
	}
}

short BlastGetLetterId(char theChar)
{
	short		letterID=theChar-'!';
	
	if (((unsigned char)theChar)>'z')	// must be either ª © £ or ¥
	{	
		switch (theChar)
		{
			case '£':
				letterID=90;
				break;
				
			case 'ª':
				letterID=91;
				break;
				
			case '©':
				letterID=92;
				break;
				
			case '¥':
				letterID=93;
				break;
		}
	}
	
	return letterID;
}

short BlastTextWidth(BTIPtr theInfo,StringPtr theString,short startPos,short length)
{
	short	loopCounter;
	short	totalCount=0;
	Byte	letterID;
	
	for (loopCounter=startPos; loopCounter<=(length+startPos); loopCounter++)
	{
		if (theString[loopCounter]==' ' || theString[loopCounter]=='\r')
		{
			totalCount+=theInfo->spaceWidth;
			continue;
		}
		
		letterID=BlastGetLetterId(theString[loopCounter]);
		
		totalCount+=((*theInfo->lCoords)[letterID].right-(*theInfo->lCoords)[letterID].left)-1;	// -1 is the condensing dist
	}

	return totalCount;
}