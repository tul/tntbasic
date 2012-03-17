/***************************************************************************************************

BLAST Fades.c
Extension of BLASTª engine
Mark Tully
12/3/96

This provides functions for producing fades on any monitor (graphic devices) attached to the Mac.
NB: Procedures preceeded by BL_ are internal and should not be called from your game.
This uses Gamma.c in case you're getting some strange Linker errors

SECTION 1 - Init routines
SECTION 2 - Routines for fading to and from black

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

#include	"Gamma.h"
#include	"BLAST.h"
#include	<PICTUtils.h>
#include	<Palettes.h>

// Globals - Used by fade pic
PicHandle	gCurrentFadePic=0L;
Rect		gCurrentFadeRect;

// ********* SECTION 1 - Init routines *********

// Returns true if it started up OK
Boolean StartUpFadeModule()
{
	if (!IsGammaAvailable()) 
		return false;
		
	SetupGammaTools();
	
	return true;
}

void ShutDownFadeModule()
{
	DoGammaFade(100); // make sure everythings turned up proper
	DisposeGammaTools(); // and only then dispose of them
}

// ********* SECTION 2 - Routines for fading to and from black *********

// There are two types, the normal fade, and the EckerFade. The EckerFade provides more power
// and can be by passed with a click of the mouse if wanted. It also takes starting and finishing
// values for half fades etc.

void FadeUp(Byte fadeSpeed,Boolean allMonitors)
{
	register short	i;
	
	if (fadeSpeed==0)
		fadeSpeed=1;
		
	for(i=0; i <= 100; i+=fadeSpeed)
	{
		if (allMonitors)
			DoGammaFade(i);
		else
			DoOneGammaFade(gPlayDevice, i);
	}

	if (allMonitors)
		DoGammaFade(100);
	else
		DoOneGammaFade(gPlayDevice, 100);
}

void EckerFadeUp(unsigned char *startAt,Byte finishAt,Boolean *skipFade,Byte speed,Boolean allMonitors)
{
	Boolean		skipped=false;
	
	if (speed==0)
		speed=1;
		
	for (*startAt; *startAt<finishAt; *startAt+=speed)
	{
		if (allMonitors)
			DoGammaFade(*startAt);
		else
			DoOneGammaFade(gPlayDevice, *startAt);
		
		if (*skipFade && Button())
		{
			skipped=true;
			break;
		}	
	}
	
	if (!skipped)
	{
		if (allMonitors)
			DoGammaFade(finishAt);
		else
			DoOneGammaFade(gPlayDevice, finishAt);
		*startAt=finishAt;
	}
	
	*skipFade=skipped;			
}

void FadeDown(Byte fadeSpeed,Boolean allMonitors)
{
	register short	i;

	if (fadeSpeed==0)
		fadeSpeed=1;
	
	for(i=100; i >= 0; i-=fadeSpeed)
	{
		if (allMonitors)
			DoGammaFade(i);
		else
			DoOneGammaFade(gPlayDevice, i);
	}
	
	if (allMonitors)
		DoGammaFade(0);
	else
		DoOneGammaFade(gPlayDevice, 0);
}

void EckerFadeDown(unsigned char *startAt,Byte finishAt,Boolean *skipFade,Byte speed,Boolean allMonitors)
{
	Boolean		skipped=false;
	
	if (speed==0)
		speed=1;
	
	for (*startAt; *startAt>finishAt; *startAt-=speed)
	{
		if (allMonitors)
			DoGammaFade(*startAt);
		else
			DoOneGammaFade(gPlayDevice, *startAt);
		
		if (*skipFade && Button())
		{
			skipped=true;
			break;
		}	
	}
	
	if (!skipped)
	{
		if (allMonitors)
			DoGammaFade(finishAt);
		else
			DoOneGammaFade(gPlayDevice, finishAt);
		*startAt=finishAt;
	}
	
	*skipFade=skipped;			
}

// ********* SECTION 3 - Routines which use the above routines *********

// FadePic
// This routine takes a PicID and expects a CLUT with the same resID as
// it. If the CLUT is not found it get's the palette from the PICT which means calling 
// GetPictInfo which is currently a lot slower than necessary.
// The fade will take place on gPlayDevice on which gPlayWindow better reside or it will look 
// insane. This fade routine presumes a 256 colour indexed PICT. The device depth is presumed to
//  be >=256 colours.
// Screen should have gamma level set to 0 when this routine is called
// Note, this will destroy the current palette of the window
// If skip is true, the player can cut the fade short. This means that when this returns, the 
// gamma level is not always guaranteed to be 0.

// 14/3/96
// Latest parameter, didTheySkip. Give it the address of a boolean and it will return whether the
// player skipped the fade by pressing the mouse button. If you pass 0L for this, it won't tell
// you and it also won't crash
// Checks if the player has tries to skip the fade before starting the fade
// Uses 'pltt' resources instead of CLUTs as it's less hassle and they don't need converting
// Returns noErr if the fade went OK

// 21/7/97
// Updated to use both indexed colour and direct colour
// Guaranteed to exit on a gamma level of zero for the play device
// The window MUST NOT have been enabled for dts else this routine will bosh, big time.
// It now takes a structure describing the fades
// No longer needs palettes or cluts

OSErr FadePic(TFadeParamsPtr params)
{
	OSErr				err=noErr;
	unsigned char		i=0; 			// used for fading
	EventRecord			theEvent;
	Boolean				using8Bit=false;
	short				depth;
	
	if (params->version!=kCurrentFadePicParamsVersion)
		return paramErr;
	
	params->didTheySkip=false;
	
	// Before we do anything, check if the player has just clicked the mouse before this.
	// It is upto the programmer to flush any mouse events which they think may be lingering.
	if (GetOSEvent(mDownMask,&theEvent))
	{
		params->didTheySkip=true;
		return noErr;
	}
	
	// figure out the depth of the window
	{
		GDHandle	maxDev=GetMaxDevice(&(GetGWindowContentRect(gPlayWindow)));
		
		depth=(**(**maxDev).gdPMap).pixelSize;
	}
	
	switch (depth)
	{
		case 32:
		case 24:
			gCurrentFadePic=(PicHandle)BetterGetResource(params->resFile,'PICT',params->fadePic+k32BitPicOffset);
			if (!gCurrentFadePic && BetterGetResErr()!=resNotFound)
				break;	// error
			else if (gCurrentFadePic)
				break;
		
		case 16:
			gCurrentFadePic=(PicHandle)BetterGetResource(params->resFile,'PICT',params->fadePic+k16BitPicOffset);
			if (!gCurrentFadePic && BetterGetResErr()!=resNotFound)
				break;	// error
			else if (gCurrentFadePic)
				break;
				
		default:
			using8Bit=true;
			gCurrentFadePic=(PicHandle)BetterGetResource(params->resFile,'PICT',params->fadePic+k8BitPicOffset);
	}
	
	if (!gCurrentFadePic)
	{
		// error, cannot get the pic to display
		return BetterGetResErr();
	}
	
	// at this stage we have either a 16/32 bit pic or an 8 bit pic with the using8Bit flag set
	// if 8-bit set the palette to the right one
	if (using8Bit)
	{
		CTabHandle		tempClut;

		tempClut=FastGetPICTCLUT(gCurrentFadePic,(CWindowPtr)gPlayWindow);
		SetWindPaletteFromClut(gPlayWindow,tempClut);
		DisposeCTable(tempClut);
	}

	// Load our logo into the middle of the window
 	gCurrentFadeRect=gPlayWindow->portRect; // Rect that pic will appear centered in
 	{
 		RGBColour		black=cBlack,white=cWhite;
 		RGBBackColour(&black);
 	
 		EraseRect(&gPlayWindow->portRect);
 		
 		RGBForeColour(&black);
 		RGBBackColour(&white);
 	}
 	ScalePic(gCurrentFadePic,&gCurrentFadeRect,false);
	DrawPicture(gCurrentFadePic,&gCurrentFadeRect);
	
	// clear any pending update events as this window has just been redrawn
	BeginUpdate(gPlayWindow);
	EndUpdate(gPlayWindow);
		
	// Fade the main screen up to show our pic
	params->didTheySkip=params->canSkip;
	EckerFadeUp(&i,100,&params->didTheySkip,params->fadeSpeed,false); // fade up main monitor
	if (!params->didTheySkip) // if they didn't skip the fade
		params->didTheySkip=NiceWaitMouseClick(BL_UpdateFadeWindow,params->timeOut,0L); // Update savvy WNE mouse wait with timeout
		
	{
		Boolean temp=false;		// cannot skip this fade
		EckerFadeDown(&i,0,&temp,params->fadeSpeed,false); // fade down main monitor
	}
	
	ReleaseResource((Handle)gCurrentFadePic);
	
	RestoreDeviceClut(gPlayDevice);
	
	return noErr;
}

void BL_UpdateFadeWindow(WindowPtr theWindow)
{
	if (theWindow==gPlayWindow)
	{
		RGBColour	black=cBlack,white=cWhite;
		GrafPtr		savePort;
		
		GetPort(&savePort);
		SetPort(theWindow);
	
		BeginUpdate(gPlayWindow);
		RGBBackColour(&black);
		EraseRect(&gPlayWindow->portRect);
		RGBBackColour(&white);
		DrawPicture(gCurrentFadePic,&gCurrentFadeRect);
		EndUpdate(gPlayWindow);
	
		SetPort(savePort);
	}
}

OSErr DoBlastSlideShow(TFadeParamsPtr params,short numPics,Boolean loop)
{
	OSErr		err;
	short		picsShown=0;
	
	do
	{
		err=FadePic(params);
		if (err)
			return err;
		picsShown++;
		params->fadePic+=kPicsPerFrame;
		if (loop)
		{
			if (picsShown==numPics)
			{
				params->fadePic-=kPicsPerFrame*picsShown;
				picsShown=0;
			}
		}
	} while (picsShown<numPics && !params->didTheySkip);

	return noErr;
}