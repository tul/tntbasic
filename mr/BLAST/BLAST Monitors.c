/***************************************************************************************************

BLAST Monitors.c
Extension of BLASTª engine
Mark Tully
12/3/96

This provides functions for manipulating all monitors (graphic devices) attached to the Mac.
NB: Procedures preceeded by BL_ are internal and should not be called from your game.

SECTION 1 - Routines for clearing all attached monitors
SECTION 2 - Routines for walking down the device list
SECTION 3 - Routines for choosing the monitor to play on
SECTION 4 - Routines for getting and setting the play device's depth

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
#include	<Palettes.h>

// Globals
LinkedList		gBlackList=0L;	// linked list of all the blackout monitors
short			gOldDepth=0;
PicHandle		gChooseMonPic=0L;

// ********* SECTION 1 - Routines for clearing all attached monitors *********

// Opens up a blacked out window on all monitors. The windows are stored in a linked list.
Boolean BlankMonitors()
{
	GrafPtr		savePort;
	Boolean		failed;
	
	GetPort(&savePort);
	
	failed=ForEachMonitorDo(BL_OpenBlackOutWindow,0L); // failed = out of ram	
	
	SetPort(savePort);
	
	return failed;
}

// recalcs the visible rgns of all the blackout windows, this is necessary after hiding or
// showing the menu bar
void RecalcVisRgns()
{
	LinkedList		temp=gBlackList;
	WindowPtr		*window;
	RgnHandle		tempRgn;
	GrafPtr			savePort;
	Boolean			done=false;

	tempRgn=NewRgn();	
	GetPort(&savePort);
	
	while (!done)
	{
		window=(WindowPtr*)GetNextElement(&temp);
		if (!window)
		{
			done=true;
			window=&gPlayWindow;
		}
	
		if (*window)
		{
			CopyRgn((**window).visRgn,tempRgn);
			
			CalcVis(*window);
		
			DiffRgn((**window).visRgn,tempRgn,tempRgn);
		
			SetPort(*window);
			InvalRgn(tempRgn);
		}
	}
	
	DisposeRgn(tempRgn);
	SetPort(savePort);
}

// Reverse of above
// 14/3/96
// Now resets monitor CLUTs avoiding flash when program exits to shell
void UnBlankMonitors()
{
	RestoreDeviceClut(0L); // resets the palette for each monitor. The closing of the windows
							// makes the system update it all
	ForEachElementDo(&gBlackList,BL_CloseBlackOutWindow,0L);
	DestroyList(&gBlackList); // dispose of the list
}

void ShowHideBlackOut(Boolean showIt)
{
	ForEachElementDo(&gBlackList,BL_ShowHideBlackOutWindow,(long*)showIt);
}

Boolean BL_ShowHideBlackOutWindow(Ptr window,long *ref,unsigned long count)
{
	WindowPtr		theWindow=*(WindowPtr *)window;
	
	ShowHide(theWindow,(Boolean)ref);
}

// Called by BlankMonitors repeatedly
Boolean BL_OpenBlackOutWindow(GDHandle gd,long *monList,unsigned long id)
{
	WindowPtr	thisMon=0L;
	
	thisMon=NewWindow(0L,&(**gd).gdRect,"\p",true,plainDBox,(WindowPtr)-1,false,wBlackType);
	if (!thisMon)
		return true;
	
	SetPort(thisMon);
	BackColor(blackColor);
	EraseRect(&thisMon->portRect);
		
	// Got the window, add to linked list
	if (!AddElement((Ptr)&thisMon,sizeof(WindowPtr),&gBlackList)) // add it to the linked list
		return false; // out of ram
		
	return true;
}

// Called by UnblankMonitors repeatedly
Boolean BL_CloseBlackOutWindow(Ptr window,long *ref,unsigned long count)
{	
	DisposeWindow((WindowPtr)*((WindowPtr *)window)); // looks nasty doesn't it...
	return true;
}

// ********* SECTION 2 - Routines for walking down the device list *********

// Executes the specified routine for each availible monitor
// Returns false if the monProc routine returns false.
Boolean ForEachMonitorDo(MonitorProc monProc,long *ref)
{
	GDHandle	hi;
	short 		count=0;
	
	hi=LMGetDeviceList();
	
	while (hi)
	{
		count++;
		
		if (!monProc(hi,ref,count))	
			return false;
			
		hi=(GDHandle)(**hi).gdNextGD;
	}
	return true;
}

// Same as above except with minimum requirements
Boolean ForEachSuitableMonitorDo(MonitorProc monProc,MonSpec *mon,long *ref)
{
	GDHandle	hi;
	short 		count=0;
	
	hi=LMGetDeviceList();
	
	while (hi)
	{
		count++;
		
		if (BL_IsMonOK(hi,mon))
		{
			if (!monProc(hi,ref,count))
				return false;
		}
			
		hi=(GDHandle)(**hi).gdNextGD;
	}
	return true;
}

// used to count up the number of suitable monitors
void CountMonitors(MonSpec *mon)
{
	mon->count=0;
	
	ForEachSuitableMonitorDo(BL_CountMonitorsProc,mon,(long *)mon);
}

Boolean BL_CountMonitorsProc(GDHandle gd,long *mon,unsigned long id)
{
	((MonSpec*)mon)->count++;
	
	return true;
}

// ********* SECTION 3 - Routines for choosing the monitor to play on *********

// Opens up a dialog on each monitor and waits for a mouse click in one then returns the no of
// monitor in numMon, only the monitors which match the requirements are given as a choice
Boolean ChooseMonitor(short picID,short *numMon,MonSpec *req)
{	
	LinkedList		monList=0L;
	EventRecord		theEvent;
	Boolean			done=false;
	WindowPtr		tempWin;
	short			part;
	long			refCon;
	
	gChooseMonPic=GetPicture(picID); // open the picture to be displayed on each monitor
	if (!gChooseMonPic)
		return false;
	
	ForEachSuitableMonitorDo(BL_OpenMonitorWindow,req,(long *)&monList); // open a window up on each mon with pic in
	
	while(!done)
	{
		NiceWaitMouseClick(BL_MonWinUpdateRoutine,0L,&theEvent);
		part = FindWindow(theEvent.where, &tempWin);
		if (part==inContent) // clicked in a window, which one?
		{	
			refCon=GetWRefCon(tempWin);
			if (refCon==wMonType) // clicked in a monitor window
			{
				ForEachElementDo(&monList,BL_WhichMonitor,(long *)&tempWin); // tempWin comes out as a monID code
				*numMon=(short)tempWin;
				done=true;
			}
		}
	}
	
	// Close all the windows in the linked list built by OpenMonitor window
	ForEachElementDo(&monList,BL_CloseMonitorWindow,0L);
	DestroyList(&monList); // dispose of the list
	
	ReleaseResource((Handle)gChooseMonPic); // get rid of the pic
	gChooseMonPic=0L;
}

// Returns the ID of the monitor that this window is on
Boolean BL_WhichMonitor(Ptr mon,long *ref,unsigned long count)
{
	if ((WindowPtr)(*ref)==((MonWinPtr)mon)->window)
	{
		*ref=((MonWinPtr)mon)->monID;
		return false; // dont check the others
	}
	return true;
}

void BL_MonWinUpdateRoutine(WindowPtr updateWindow)
{
	GrafPtr		savePort;
	
	GetPort(&savePort);
	SetPort(updateWindow);
	
	BeginUpdate(updateWindow);
	DrawPicture(gChooseMonPic,&updateWindow->portRect);
	EndUpdate(updateWindow);
	
	SetPort(savePort);
}

// This is called repeatedly by ChooseMonitor to open all the windows
Boolean BL_OpenMonitorWindow(GDHandle gd,long *monList,unsigned long id)
{
	Rect	wRect=(**gChooseMonPic).picFrame;
	MonWin	mon;
	
	CenterRectInRect(&wRect,&(**gd).gdRect);
	
	mon.monID=id;
	mon.window=NewWindow(0L,&wRect,"\p",true,plainDBox,(WindowPtr)-1,false,wMonType);
	
	if (!mon.window)
		return false;
		
	BL_MonWinUpdateRoutine(mon.window);
	
	AddElement((Ptr)&mon,sizeof(MonWin),(LinkedList *)monList); // add it to the linked list
	
	return true;	
}

// Called repeatedly by ChooseMonitor to close all the windows
Boolean BL_CloseMonitorWindow(Ptr mon,long *ref,unsigned long count)
{	
	DisposeWindow(((MonWinPtr)mon)->window);
	
	return true;
}

Boolean BL_IsMonOK(GDHandle gd,MonSpec *req)
{
	if (BiggerRect((**gd).gdRect,req->size) && (HasDepth(gd,req->depth,0,0)))
		return true;
	else
		return false;
}

// call this with the monitor id of the monitor to play on. At the moment it doesn't support
// switching of monitors (ie call this once only)
void SetPlayMonitor(short monitorID)
{
	GDHandle	meow=(GDHandle)monitorID;
	
	if (ForEachMonitorDo(BL_MatchMonIDtoGD,(long *)&meow))
		SysBeep(0); // can't find the monitor
	else
		gPlayDevice=meow;
}

// used above to find which monitor corresponds to an id
// theMon is an address of the gdhandle to recieve the gdhandle
Boolean BL_MatchMonIDtoGD(GDHandle gd,long *theMon,unsigned long id)
{
	short temp=(short)(*theMon);
	
	if (temp==id)
	{
		*theMon=(long)gd;
		return false;
	}
	return true;
}

// ********* SECTION 4 - Routines for getting and setting the play device's depth

void RestoreDepth()
{
	if (gOldDepth)
	{
		SetDepth(gPlayDevice,gOldDepth,0,0); // presumes that it can be set as it was this to start with
		gOldDepth=0;
	}
}

Boolean NiceSetDepth(short depth)
{	
	if (!HasDepth(gPlayDevice,depth,0,0)) // device does not support that number of colours
		return false;
	
	// Get previous depth
	gOldDepth=GetDeviceDepth(gPlayDevice);
	
	if (SetDepth(gPlayDevice,depth,0,0))
		return false;
		
	return true;
}