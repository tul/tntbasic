/***************************************************************************************************

BLAST Main.c
Central hub of the BLASTª engine
Mark Tully
12/3/96

This file needs to be linked to your project for BLASTª to funtion. StartUpBlast and ShutDownBlast
must be called at the beginning and end of your game if you are to use BLASTª functions.

SECTION 1 - Start up / Shut down of BLASTª
SECTION 2 - General routines

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

#include		"BLAST.h"

// part of the extendable architecture of BLASTª
#define			numProcs		2
typedef			Boolean (*StartUpProc)();
typedef			void (*ShutDownProc)();

// The name of a startup procedure / shut down procedure goes in these arrays
StartUpProc		gStartUp[numProcs]={StartUpFadeModule,0L};
ShutDownProc	gShutDown[numProcs]={ShutDownFadeModule,ShutDownSpriteModule};
short			gProcsInited=0;

// ********* SECTION 1 - Start up / Shut down of BLASTª *********

// Begin detonation...
// Initialises variable and the like
Boolean StartUpBlast()
{	
	// Set up some of the BLAST globals
	gPlayDevice=GetMainDevice(); // BLAST defaults to this screen
	Check601();					// check for 601
	
	// Init all modules
	for (gProcsInited=0; gProcsInited<numProcs; gProcsInited++)
	{
		if (gStartUp[gProcsInited]) // if it's not a nil pointer
		{
			if (!gStartUp[gProcsInited]()) // if it messed up
			{
				ShutDownBlast();
				return false;
			}
		}
	}
	return true;
}

// Make sure that this is called before ANY ExitToShell(). It can be called from anywhere!
// If you don't the player will not be impressed if you leave their Gamma levels at 0!
void ShutDownBlast()
{
	for (gProcsInited--;gProcsInited>-1;gProcsInited--) // shut down all started up modules
	{
		if (gShutDown[gProcsInited])
			gShutDown[gProcsInited]();
	}
	
	ShutDownBlastDraw();
	RestoreDepth(); // no effect if called already
	RestoreControlStrip(); // no effect if called already
//	ShowMenuBar(); // no effect if called already
	UnBlankMonitors(); // no effect if called already
	InitCursor();
	FlushEvents(everyEvent,0L); // make sure who ever follows us doesn't get any stray events
}

// ********* SECTION 2 - General routines *********

// A VERY useful routine which waits for a mouse click. It also calls WNE and handles window
// updates and has a time out and can return the event record of the final event and takes a
// custom update routine to keep non-BLAST windows updated as needed. (RAA!!)
// This may not return as soon as the mouse is clicked if it's the first WaitNextEvent() you
// have called since a depth change. The system uses the time to fix all the colours.
// returns true if the user clicked to get past the wait. Pass 0L for timeOut for click only
// pass 0L for the return rec if you don't want the event record
// pass 0L for the update routine of you don't care about any of your non-BLAST windows getting
// updated
Boolean NiceWaitMouseClick(UpdateProc theirUpdateRoutine,long timeOut,EventRecord *returnRec)
{
	EventRecord		theEvent;
	long			waitUntil=0L;
	long			wait;
	
	if (timeOut)
		waitUntil=LMGetTicks()+timeOut;
		
	do
	{
		if (timeOut)
		{
			wait=waitUntil-LMGetTicks();
			if (wait<0L)
				wait=0L;
		}
		else
			wait=0xFFFFFFFF; // Max wait
			
		// mDownMask|updateMask
		// The reason I'm using everyEvent instead of above is that I found that when the system
		// wanted to send you an event which wasn't on your list it would keep on sending it
		// and you would keep getting null events as your mask would kill them and so no events
		// would get through, I think that the event that was trying to get through was a
		// activateEvt but I'm not sure. It happenned when you switched to another app and back
		// again. Don't want to take any chances with any others though so I'll use the full
		// mask and just ignore the other events, OK?
		WaitNextEvent(everyEvent,&theEvent,wait,0L);
				
		if (theEvent.what==updateEvt)
			BlastUpdate((WindowPtr)theEvent.message,theirUpdateRoutine);
		
		if (waitUntil && waitUntil<=LMGetTicks())
			break;
			
	} while (theEvent.what!=mouseDown);
				
	if (returnRec)
		*returnRec=theEvent;
		
	if (theEvent.what==mouseDown) // true for mouse down, false for timeout
		return true;
	else
		return false;
}

// Give it the window that needs updating and the pointer to your custom proc for updating
// non-blast windows and the update will be handled
void BlastUpdate(WindowPtr updateWindow,UpdateProc theirUpdateRoutine)
{
	long		refcon;
	
	refcon=GetWRefCon(updateWindow); // Gets the 4 byte code I put in the refcon when opening
	
	if (refcon==wBlackType) // update the black out window
	{
		GrafPtr		savePort;
		
		GetPort(&savePort);
		SetPort(updateWindow);
				
		BeginUpdate(updateWindow);
		EraseRect(&updateWindow->portRect);
		EndUpdate(updateWindow);
		
		SetPort(savePort);
	}	
	else // if it's not a blackout window then call the users update routine
	{
		if (theirUpdateRoutine) // ...if they gave us one
			theirUpdateRoutine(updateWindow);
		else
		{
			BeginUpdate(updateWindow);
			EndUpdate(updateWindow);
		}
	}
}
