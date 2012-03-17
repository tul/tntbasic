// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Cursors.c
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

// this code is ripped from think reference under animated cursors except GetCursor calls are
// now GetCCursor

#include	"Marks Routines.h"

typedef struct
			/* The structure of an 'acur' resource */
{
	short numberOfFrames;		/* number of cursors to animate */
	short whichFrame;			/* current frame number */
	CCrsr** frame[];			/* Pointer to the first cursor */
} acur, *acurPtr, **acurHandle;

/* Redefine HiWord() as a macro to increase speed. */
#define HiWrd(aLong)	(((aLong) >> 16) & 0xFFFF)

/* Some module-local globals */
static short gTickInterval;	/* number of ticks between a frame switch */
static long gLastTick;	/* tick count of last call to SpinCursor */
static acurHandle gFrameList;		/* our cursor list */

/* Try to get the acur record and the cursor list for acurID, returning TRUE
   if everything goes as planned. */
Boolean
InitAnimatedCursors(short acurID, short interval)
{
	register short i=0;
	register short cursID;
	Boolean noErrFlag = FALSE;

	if((gFrameList = (acurHandle) GetResource('acur',acurID))) {
			/* got it! */
			noErrFlag = TRUE;
			while((i<(*gFrameList)->numberOfFrames) && noErrFlag) {
			/* The id of the cursor is stored in the high word of the frame handle */
					cursID = (short) HiWrd((long) (*gFrameList)->frame[i]);
					(*gFrameList)->frame[i] = GetCCursor(cursID);
					if((*gFrameList)->frame[i])
							i++;				/* get the next one */
					else
					{
						
							noErrFlag=FALSE;	/* foo! we couldn't find the cursor
														*/
					}
			}
	}
	if(noErrFlag) {
			/* We have the cursors, now initialize the other fields */
			gTickInterval = interval;
			gLastTick = TickCount();
			// (*gFrameList)->whichFrame = 0; start from frame the resource sez
	}
	return noErrFlag;
}

/* Free up the storage used by the current animated cursor and all
   of its frames */
void
ReleaseAnimatedCursors()
{
	short i;

	for(i=0;i<(*gFrameList)->numberOfFrames;i++)
			ReleaseResource((Handle) (*gFrameList)->frame[i]);
	ReleaseResource((Handle) gFrameList);
}

/* 	Display the next frame in the sequence, if it's time. If not, do nothing.
	This code should be pretty tight, but it might be possible to bum a few
	instructions; ideally it should be as fast as possible. I doubt it will
	be necessary to hand-hack it though.

	I chose this implementation over using a VBL task for the following reason:
	the whole point of using an animated cursor is to let the user know that
	your application is chugging away doing something. With this technique if
	the applications shoots off to never-never land, the cursor will probably
	stop spinning. A VBL task, on the other hand, would probably just sit and
	merrily go on spinning away, the user none the wiser. */
void
SpinCursor()
{
	register long  newTick;

	newTick = LMGetTicks();

	/* Is it time? */
	if(newTick < (gLastTick + gTickInterval))
			return;		/* nope */

	/* Grab the frame, increment (and reset, if necessary) the count, and
			display the new cursor */
	SetCCursor(((*gFrameList)->frame[(*gFrameList)->whichFrame++]));
	if((*gFrameList)->whichFrame == (*gFrameList)->numberOfFrames)
			(*gFrameList)->whichFrame = 0;
	gLastTick = newTick;
}

