// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Button_Statements.cpp
// © John Treece-Birch 2000
// 12/7/00
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

#include		"CApplication.h"
#include		"Carbon_Settings.h"
#include		"Button_Statements.h"
#include		"CTBSprite.h"
#include		"CProgram.h"
#include		"CSpriteManager.h"
#include		"CGraphicsContext16.h"

TTBInteger		CButtonTracker::sTrackThis=-1;
TTBInteger		CButtonTracker::sHiliteThis=-1;
bool			CButtonTracker::sWasPressed=false;

TTBInteger		CGetButtonClickFunction::sButton=-1;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CPollButtonClickFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CPollButtonClickFunction::EvaluateInt(
	CProgram			&ioState)
{	
	CButtonTracker::GetButtonClick(ioState);
	
	return CGetButtonClickFunction::sButton;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CWaitButtonClickHeadStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The head statement will flush events, the tail loops until a mouse event triggers it
CWaitButtonClickHeadStatement::CWaitButtonClickHeadStatement()
{
	CStatement		*c=new CWaitButtonClickTailStatement();
	ThrowIfMemFull_(c);
	Append(c);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CWaitButtonClickHeadStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Clears the button states and flushes mouse down events
void CWaitButtonClickHeadStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CGetButtonClickFunction::sButton=-1;

	if (USE_RNE_FOR_SYNC_INPUT)
	{
		EventTypeSpec	eventsList[]={ { kEventClassMouse, kEventMouseDown } };
		::FlushEventsMatchingListFromQueue(GetCurrentEventQueue(),1,eventsList);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CWaitButtonClickTailStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CWaitButtonClickTailStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CButtonTracker::GetButtonClick(ioState);

	// Redraw all the buttons
	ioState.CheckGraphicsMode()->RenderSpritesAndBlit();

	if ((CGetButtonClickFunction::sButton==-1) && (UButtonManager::ValidButtons()))
	{
		if (USE_RNE_FOR_SYNC_INPUT)
		{
			// wait for a standard event with an infinite time out, this includes esc and cmd-. and mouse moved
			// basically we yield to other processes until the mouse is moved/clicked or a key is hit
			EventRef	e;
			bool		processed=false;
						
			ioState.TBReceiveNextEvent(EventListWithCount(CApplication::kStdEventsListPlusAllMouse),kEventDurationForever,false,&e);
			
			// if it's a mouse click, flush it and exit, if it's not a mouse click, don't flush it, let it be handled then repeat
			if (::GetEventClass(e)==kEventClassMouse)
			{
				switch (::GetEventKind(e))
				{
					case kEventMouseDown:
						if (!ioState.MouseEventIsOverGameWindow(e))		// click in our window?
						{
							break;
						}
						// else fall through...

					case kEventMouseUp:
					case kEventMouseDragged:
						// remove the event from the queue
						::RemoveEventFromQueue(GetCurrentEventQueue(),e);		// we now own e
						::ReleaseEvent(e);
						processed=true;
						break;
				}
			}
			
			if (!processed)
				ioState.SetForceEventProcess();		// get the event out of our event queue immediately
		}

		// loop around
		ioState.SetPC(this);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CButtonTracker::GetButtonClick
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CButtonTracker::GetButtonClick(
	CProgram			&ioState)
{
	Point			thePoint;
	::GetMouse(&thePoint);

//	ioState.CheckGraphicsMode()->LocalToGlobal(thePoint);

	CGetButtonClickFunction::sButton=-1;

	if (!Button())
	{
		// Button is not pressed
		sHiliteThis=UButtonManager::IsPointIn(thePoint.h,thePoint.v);
	}
	else
	{
		// Button is pressed
		if (sTrackThis!=-1)
			sHiliteThis=UButtonManager::IsPointIn(thePoint.h,thePoint.v);
		else
			sHiliteThis=-1;
	}

	if ((Button()) && (!sWasPressed))
	{
		// Button has just been pressed
		sWasPressed=true;
		sHiliteThis=sTrackThis=UButtonManager::IsPointIn(thePoint.h,thePoint.v);
	}
	else if ((!Button()) && (sWasPressed))
	{
		// Button has just been released
		if ((UButtonManager::IsPointIn(thePoint.h,thePoint.v)==sTrackThis) && (sTrackThis!=-1))
		{
			CGetButtonClickFunction::sButton=sTrackThis;
		
			#if TB_FlushEventsAllTheFeckinTime
			::FlushEvents(kUserInputEventMask,0L);
			#endif
		}
		
		sWasPressed=false;
		sHiliteThis=-1;
		sTrackThis=-1;
	}

	UButtonManager::UpdateButtons(sHiliteThis,sTrackThis,ioState);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CNewButtonStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CNewButtonStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CImageBank		*bank;
	
	if (mBank.IsNull())
		bank=ioState.CheckGraphicsMode()->mSpriteManager.GetDefaultSpriteBank();
	else
		bank=ioState.CheckGraphicsMode()->mSpriteManager.GetBank(mBank(ioState));

	UButtonManager::NewButton(mIndex(ioState),mX(ioState),mY(ioState),mImage(ioState),mOverImage(ioState),mDownImage(ioState),bank,ioState);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CClearButtonsStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CClearButtonsStatement::ExecuteSelf(
	CProgram			&ioState)
{	
	UButtonManager::ResetButtons();
}