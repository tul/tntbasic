// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TWindow.cpp
// Mark Tully
// 21/2/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1998, Mark Tully and John Treece-Birch
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

#include	"MR Debug.h"
#include	"CWindow.h"
#include	"CWindowLayer.h"
#include	"Useful Defines.h"
#include	"Root.h"
#include	"CCarbonWindowLayer.h"

/*e*/
CWindow::CWindow(WindowPtr windowPtr,CWindowLayer *windowLayer) :
	link(this), mHandlerRef(NULL)
{
	active=false;
	this->windowPtr=windowPtr;
	this->windowLayer=windowLayer;
	windowLayer->AddWindow(this);
	
	if (UEnvironment::IsRunningOSX())
		InstallCarbonWindowCallBacks();
}

CWindow::~CWindow()
{
	if (mHandlerRef)
	{
		RemoveEventHandler(mHandlerRef);
		mHandlerRef=NULL;
	}
	
	windowLayer->RemoveWindow(this);
	windowLayer=0L;
}

EventTypeSpec	CWindow::sTypesList[] = {	{ kEventClassWindow, kEventWindowActivated },
											{ kEventClassWindow, kEventWindowDeactivated } };
void CWindow::InstallCarbonWindowCallBacks()
{
	ThrowIfOSErr_(::InstallWindowEventHandler(windowPtr,NewEventHandlerUPP(CarbonWindowHandler),sizeof(sTypesList)/sizeof(EventTypeSpec),sTypesList,this,&mHandlerRef));
}

pascal OSStatus CWindow::CarbonWindowHandler(
	EventHandlerCallRef inHandlerCallRef,
	EventRef			inEvent,
	void				*inUserData)
{
	CWindow				*win=(CWindow*)inUserData;
	
	Try_
	{	
		switch (::GetEventKind(inEvent))
		{
			case kEventWindowActivated:
				{
					win->GetWindowLayer()->ActivateWindow(win);
					
					/*
					CCarbonWindowLayer		*carb=dynamic_cast<CCarbonWindowLayer*>(win->GetWindowLayer());
					
					AssertThrow_(carb);
					
					if (carb->CanWindowBeActivated(win))
					{
						win->active=true;
						win->WindowActivated();
					}*/
				}
				break;
				
			case kEventWindowDeactivated:
				win->GetWindowLayer()->DeactivateWindow(win);
				ECHO("Deactivate event received for window");
				
				/*
				if (win->IsWindowActive())
				{
					win->active=false;
					win->WindowDeactivated();				
				}*/
				break;
				
			default:
				ECHO("ERROR: Window handler received an event it doesn't know what to do with!");
				break;
		}
	}
	Catch_(err)
	{
		return ErrCode_(err);
	}
	
	return noErr;
}


void CWindow::UpdateWindow()
{
	BeginUpdate(GetWindowPtr());
	EndUpdate(GetWindowPtr());
}

CListElementT<CWindow> *CWindow::MakeAlias()
{
	return link.MakeAlias();
}

void CWindow::HideWindow()
{
	windowLayer->HideWindow(this);
}

void CWindow::ShowWindow()
{
	windowLayer->ShowWindow(this);
}

void CWindow::SelectWindow()
{
	windowLayer->SelectWindow(this);
}

void CWindow::DragWindow(EventRecord &eventRecord)
{
	windowLayer->DragWindow(eventRecord,this);
}

/*e*/
void CWindow::LinkWindowToList(CLinkedListT<CWindow> *list)
{
	CListElementT<CWindow>		*ele=MakeAlias();
	
	ThrowIfMemFull_(ele);
	list->LinkElement(ele);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SaveWindowPosition
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// saves the current position of the window under the screen rect passed, can be restored later with RestoreWindowPositions
// passing the same screen size.
void CWindow::SaveWindowPosition(
	Rect		&inScreenSize)
{
	CListIndexerT<CSavedPosition>		indexer(&mSavedPositions);
	Rect								position;
	
	::GetWindowBounds(windowPtr,kWindowStructureRgn,&position);	
	while (CSavedPosition *pos=indexer.GetNextData())
	{
		if (SameRect(pos->mScreenRes,inScreenSize))
		{
			pos->mSavedPosition=position;
			return;
		}
	}
	
	// not one existing, create a new one
	CSavedPosition		*p=new CSavedPosition(inScreenSize,position);
	if (p)
	{
		mSavedPositions.LinkElement(p);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RestoreWindowPosition
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CWindow::RestoreWindowPosition(
	Rect		&inScreenSize)
{
	CListIndexerT<CSavedPosition>		indexer(&mSavedPositions);
	
	while (CSavedPosition *pos=indexer.GetNextData())
	{
		if (SameRect(pos->mScreenRes,inScreenSize))
		{
			::SetWindowBounds(windowPtr,kWindowStructureRgn,&pos->mSavedPosition);
			return;
		}
	}	
}
