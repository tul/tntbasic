// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// About box bits.c
// Mark Tully
// 19/6/96
// 20/6/96
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

#include	<QDOffscreen.h>
#include	"BLAST.h"
#include	"Marks Routines.h"

#define		fadeLevels		12

Boolean			gPaused=false;
unsigned long	gWaitUntil=0L;

// Renders a text and stly resource into a gworld with multiple replacements. Each find and replace pair must be the
// same length as the length of the scrolly text cannot be changed
Boolean DrawTextIntoNewGWorld(
	short resID,
	Rect *ioBounds,
	GWorldPtr *outGWorldP,
	short theJust,
	StringPtr *replaceMe,
	StringPtr *withMe,
	short numToReplace,
	Boolean	blackOnWhite)		// if true, black text on white bg, if false, white text on black bg
{
	Handle			theText;
	TEHandle		theTE;
	StScrpHandle	theStyle;
	GWorldPtr		saveWorld;
	GDHandle		saveDevice;
	OSErr			err;
	Boolean			res=false;
	short			counter;
	
	// Allocate TE rec and get resources
	TextFont(applFont);	
	TextSize(9);
	theTE=TEStyleNew(ioBounds, ioBounds);
	theText=GetResource('TEXT',resID);
	theStyle=(StScrpHandle) GetResource('styl',resID);
	
	if (!(theText&&theStyle&&theTE))
	{
		if (theTE)
			TEDispose(theTE);
		if (theText)
			ReleaseResource(theText);
		if (theStyle)
			ReleaseResource((Handle)theStyle);
		return false;
	}
	
	// Once got them enter it all into the te rec and justify it
	HidePen();
	HLock(theText);
	TEStyleInsert(*theText,GetHandleSize(theText),theStyle,theTE);
	ShowPen();
	ReleaseResource(theText);
	ReleaseResource((Handle)theStyle);
	theText=0;
	theStyle=0;


	// Firstly, perform a find and replace operation on the text. This is used for replacing
	// a constant in your text with the version number supplied.
	
	for (counter=0; counter<numToReplace; counter++)
	{
//		short pos=SearchForChars(*theText,(Ptr)&replaceMe[counter][1],GetHandleSize(theText),replaceMe[counter][0],false);
		short pos=SearchForChars(*(**theTE).hText,(Ptr)&replaceMe[counter][1],(**theTE).teLength,replaceMe[counter][0],false);
	
		if (pos!=-1)
		{
//			if (ResizeHandleWithInsert(theText,pos,replaceMe[counter][0],withMe[counter][0]))
			::TESetSelect(pos,pos+replaceMe[counter][0],theTE);
			::TEDelete(theTE);
			::TEInsert(&withMe[counter][1],withMe[counter][0],theTE);
/*			{
				if (theTE)
					TEDispose(theTE);
				if (theText)
					ReleaseResource(theText);
				if (theStyle)
					ReleaseResource((Handle)theStyle);
				return false;
			}
			
			BlockMove(&withMe[counter][1],(*theText)+pos,withMe[counter][0]);*/
		}
	}
	
	
	TESetAlignment(theJust, theTE);
	TECalText(theTE);
	
	// Determine height of the Text
	ioBounds->right=ioBounds->right-ioBounds->left;
	ioBounds->left=ioBounds->top = 0;
	ioBounds->bottom=TEGetHeight((**theTE).nLines,0,theTE);
	
	// Create new GWorld that is the height of the Text
	err=NewGWorld(outGWorldP,1,ioBounds,0L,0L,0);
	
	if (err==noErr)
	{
		GetGWorld(&saveWorld, &saveDevice);
		SetGWorld(*outGWorldP,0L);
		HLockHi((Handle)GetGWorldPixMap(*outGWorldP));
		LockPixels(GetGWorldPixMap(*outGWorldP));
	
		EraseRect(ioBounds);

		// Draw Text inside GWorld
		(**theTE).viewRect = *ioBounds;
		(**theTE).destRect = *ioBounds;
		(**theTE).inPort = (GrafPtr) *outGWorldP;
		TEUpdate(ioBounds, theTE);
	
		if (!blackOnWhite)
			InvertRect(ioBounds); // White letters on black background

		SetGWorld(saveWorld, saveDevice);
		
		res=true;	
	}
	
	TEDispose(theTE);
	
	return res;
}

// Set the port to your window, slam in the value of visRect,theWindow and picRect then set all
// the others to 0 then call this. Always call ShutDownScrollyCredits, even if this returns
// false.
// the two strings can be used for passing version numbers. Have a version resource as a rez
// doc that is compiled and added to your project at link time. In your resource file, have
// the text resource say something like
// Version  .  
// Then give startup scrolly credits  .   as the replaceMe string and the current version string
// as the with me string. note that if your're using version.r (Marks Routines) then include
// your Version.h and use the constant verShortTextP
// You can acutally pass arrays of find and replace strings. Each pair of find/replace strings must
// be the same length still.
Boolean StartupScrollyCredits(ScrollyRec *theRec,PicHandle thePic,short textID,short theJust,
														StringPtr *replaceMe,StringPtr *withMe,short numReplaces)
{	
	short		picDepth,clutDepth,clutCols;
	Rect		globalRect=theRec->visRect;
	Rect		theRect=theRec->visRect;

	gPaused=false;
	gWaitUntil=0L;
	
	// adjust the picrect
	{
		short	xoff,yoff;
		
		xoff=theRec->visRect.left-theRec->picRect.left;
		yoff=theRec->visRect.top-theRec->picRect.top;
		
		FOffset(theRec->picRect,-xoff,-yoff);
	}
	
	clutDepth=theRec->forceToBitDepth;
	
	if (!clutDepth)
	{
		// calculate the depth that the back world will be in
		// it will be the lesser of the screen depth and the 
		theRec->theClut=FastGetPICTCLUT(thePic,0L);
		if (!theRec->theClut)
		{
			// can't get it's clut, mustn't be indexed
			clutDepth=16;
		}
		else
		{
			clutCols=(**theRec->theClut).ctSize+1;
			
			if (clutCols<=2)
				clutDepth=1;
			else if (clutCols<=4)
				clutDepth=2;
			else if (clutCols<=16)
				clutDepth=4;
			else if (clutCols<=256)
				clutDepth=8;
			else
				clutDepth=16;
		}
	}
	
	LocalToGlobal((Point *)&globalRect.top);
	LocalToGlobal((Point *)&globalRect.bottom);
	
	picDepth=Lesser(clutDepth,(**(** (GetMaxDevice(&globalRect)) ).gdPMap).pixelSize);
	
	ZeroRectCorner(theRect);
	
	// Buff world for merging images
	if (NewGWorld(&theRec->buffWorld,picDepth,&theRect,0L,0L,0))
		return false;
	HLockHi((Handle)GetGWorldPixMap(theRec->buffWorld));
	LockPixels(GetGWorldPixMap(theRec->buffWorld));

	//	Make GWorld for background.
	if (NewGWorld(&theRec->backWorld,picDepth,&theRect,theRec->theClut,0L,0L))
		return false;
	HLockHi((Handle)GetGWorldPixMap(theRec->backWorld));
	LockPixels(GetGWorldPixMap(theRec->backWorld));
	
	// Draw the Pic into the backworld
	{
		CGrafPtr	saveWorld;
		GDHandle	saveGD;
		
		GetGWorld(&saveWorld,&saveGD);
		SetGWorld(theRec->backWorld,0L);
		DrawPicture(thePic,&theRec->picRect);
		SetGWorld(saveWorld,saveGD);
	}
	
	// Make GWorld for credits.
	{
		Rect		creditsBox=theRec->visRect;

		if (!DrawTextIntoNewGWorld(textID,&creditsBox,&theRec->textWorld,theJust,replaceMe,withMe,numReplaces,theRec->blackText))
			return false;
			
		theRec->sourceRect.left=0;
		theRec->sourceRect.top=0;
		theRec->sourceRect.right=theRec->visRect.right-theRec->visRect.left;
		theRec->sourceRect.bottom=1;
		theRec->textHeight=creditsBox.bottom;
	}
	
	return true;
}

void ShutDownScrollyCredits(ScrollyRec *theRec)
{
	if (theRec->theClut)
		DisposeCTable(theRec->theClut);
		
	DisposeGWorld(theRec->backWorld);
	DisposeGWorld(theRec->buffWorld);
	DisposeGWorld(theRec->textWorld);
}

// Will scroll the credits 1 unit
void ScrollCredits(ScrollyRec *theRec)
{
	unsigned short greyLevels[fadeLevels]={
		4369, 8738, 17476, 21845, 30583, 34952, 43690, 48059, 52428, 56979,
		61166, 65535
	};

	Rect		buffWorldPortRect;
	CGrafPtr	winPort=GetWindowPort(theRec->theWindow);
	const BitMap		*winBMap=GetPortBitMapForCopyBits(winPort);

	#if TARGET_API_MAC_CARBON
		::GetPortBounds(theRec->buffWorld,&buffWorldPortRect);
	#else
		buffWorldPortRect=theRec->buffWorld->portRect;
	#endif

	
	// check for pause
	if (IsPressed(49))
	{
		gPaused=!gPaused;
		while(IsPressed(49));
	}

	if (!gPaused)
	{
		CGrafPtr	saveWorld;
		GDHandle	saveGD;
		short		count;
		RGBColor	fadeColor;
		
		GetGWorld(&saveWorld,&saveGD);
	
		// render to buff world
		// first copy the text into the buffworld
		{
			Rect	destRect;
			
			
			SetGWorld(theRec->buffWorld,0L);

			if (theRec->atBottom)
			{
				Rect	eraseRect;
				
				destRect=theRec->sourceRect;
				ZeroRectCorner(destRect);
				FastSetRect(eraseRect,destRect.bottom,0,destRect.bottom+1,destRect.right);
				
				if (theRec->blackText)
				{
					BackColor(whiteColor);
					EraseRect(&eraseRect);
				}
				else
				{
					BackColor(blackColor);
					EraseRect(&eraseRect);
					BackColor(whiteColor);
				}
			}
			else if (theRec->atTop)
			{
				if (theRec->sourceRect.bottom==1) // first frame
				{
					if (theRec->blackText)
					{
						BackColor(whiteColor);
						EraseRect(&buffWorldPortRect);
					}
					else
					{
						BackColor(blackColor);
						EraseRect(&buffWorldPortRect);
						BackColor(whiteColor);
					}
				}
				
				FastSetRect(destRect,buffWorldPortRect.bottom-theRec->sourceRect.bottom,0,buffWorldPortRect.bottom,buffWorldPortRect.right);
			}
			else
				destRect=buffWorldPortRect;
			
			CopyBits((BitMap*)*GetGWorldPixMap(theRec->textWorld),
					(BitMap*)*GetGWorldPixMap(theRec->buffWorld),&theRec->sourceRect,
					&destRect,srcCopy,0L);
		}
			
		// Now fade in at the bottom and out at the top
		if (theRec->blackText)
			PenMode(addMax);
		else
			PenMode(adMin);
			
		for (count=0;count<fadeLevels-1;count++)
		{
			if (theRec->blackText)
				fadeColor.red=fadeColor.blue=fadeColor.green=greyLevels[(fadeLevels-1)-count];
			else
				fadeColor.red=fadeColor.blue=fadeColor.green=greyLevels[count];
			RGBForeColor(&fadeColor);
			MoveTo(0,count);
			Line(buffWorldPortRect.right,0);
			MoveTo(0,buffWorldPortRect.bottom-count-1);
			Line(buffWorldPortRect.right,0);
		}
		ForeColor(blackColor);
		PenMode(patCopy);
	
		// Add the backdrop over the text so that it appears to be fading into the backdrop
		// but only do this step if transparent backdrop is set, otherwise it copies white text
		// on a black backdrop
		if (theRec->transText)
		{
			short		mode;
			
			if (theRec->blackText)
				mode=adMin;
			else
				mode=addMax;
		
			CopyBits((BitMap *)*GetGWorldPixMap(theRec->backWorld),
					(BitMap *)*GetGWorldPixMap(theRec->buffWorld),
					&buffWorldPortRect,&buffWorldPortRect,mode,0L);		
		}
		
		// reset the draw world to the main screen
		SetGWorld(saveWorld,saveGD);
	
		// wait draw time
		while (gWaitUntil>TickCount());

		// copy to screen
		#if TARGET_API_MAC_CARBON
		::CopyBits((BitMap *)*GetGWorldPixMap(theRec->buffWorld),winBMap,
				 &buffWorldPortRect,&theRec->visRect,srcCopy,0L);
		
		// flush graphics
		::QDFlushPortBuffer(winPort,0);
		
		#else
		CopyBits((BitMap *)*GetGWorldPixMap(theRec->buffWorld),(BitMap *)&theRec->theWindow->portBits,
				 &buffWorldPortRect,&theRec->visRect,srcCopy,0L);
		#endif
					 
		gWaitUntil=TickCount()+theRec->everyVBL;

		// Move the copy rect down and wrap around if necessary
		if (!theRec->atTop)
			theRec->sourceRect.top++;
		if (!theRec->atBottom)
			theRec->sourceRect.bottom++;
		
		// reached the bottom of the text. Stop inscreasing the bottom and finish moving the
		// text which is on screen all the way up to the top.
		if (!theRec->atBottom && theRec->sourceRect.bottom>=theRec->textHeight)
		{
			theRec->sourceRect.bottom=theRec->textHeight;
			theRec->atBottom=true;
		}
		else if (theRec->sourceRect.top>=theRec->textHeight)
		{
			// the top has reached the bottom of the text so
			// wrap around
			theRec->sourceRect.top=0;
			theRec->sourceRect.bottom=1;
			theRec->atTop=true;
			theRec->atBottom=false;
		}
		else if (theRec->atTop)
		{
			// at the top, do we start scrolling the top yet?
			if (theRec->sourceRect.bottom>=(theRec->visRect.bottom-theRec->visRect.top))
			{
				// resume normal scrolling
				theRec->atTop=false;
			}
		}
	}
}