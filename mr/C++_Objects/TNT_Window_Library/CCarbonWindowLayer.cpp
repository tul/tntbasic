// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CCarbonWindowLayer.cpp
// TNT Library
// Mark Tully
// 22/5/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

#include "TNT_Debugging.h"
#include "CCarbonWindowLayer.h"
#include "CWindow.h"
#include "TListIndexer.h"
#include "Root.h"

SInt32		CCarbonWindowLayer::sLevel=0;		// This is perhaps a bit dodgey, should probably be using CGWindowLevelForKey
WindowGroupRef	CCarbonWindowLayer::sRootGroup=NULL;

CCarbonWindowLayer::CCarbonWindowLayer(
	CWindowLayer::TWindowLayerFlags	inFlags) :
	CWindowLayer(flags),
	mWindowGroup(NULL)
{	
	WindowGroupAttributes		groupFlags=0;
	
	if (inFlags&kHilightAllWindowsInLayer)
		groupFlags|=kWindowGroupAttrSharedActivation;	
		
	if (!sRootGroup)
		ThrowIfOSErr_(::CreateWindowGroup(0,&sRootGroup));
		
	ThrowIfOSErr_(::CreateWindowGroup(groupFlags,&mWindowGroup));
	ThrowIfOSErr_(::SetWindowGroupLevel(mWindowGroup,sLevel++));
	ThrowIfOSErr_(::SetWindowGroupParent(mWindowGroup,sRootGroup));
}

CCarbonWindowLayer::~CCarbonWindowLayer()
{
	::ReleaseWindowGroup(mWindowGroup);
	mWindowGroup=NULL;
}

void CCarbonWindowLayer::AddWindow(
	CListElementT<CWindow> *listEle)
{
	inheritedLayer::AddWindow(listEle);

	CWindow			*windowPtr=listEle->GetData();

	OSErr		err=::SetWindowGroup(windowPtr->GetWindowPtr(),mWindowGroup);

	if (err)
		ECHO("CCarbonWindowLayer::AddWindow - error code " << err);
}

void CCarbonWindowLayer::SelectWindow(CWindow *windowPtr)
{
	if (windowPtr->IsWindowActive())
		return;

	// Deactivate the current front window
	DeactivateWindow(GetSelectedWindow());
	
	if (!windowPtr->IsWindowVisible())
		windowPtr->ShowWindow();

	::SelectWindow(windowPtr->GetWindowPtr());

	windowPtr->UpdateWindow();
	ActivateFrontMostWindow();


/*
	if (windowPtr->IsWindowActive())
		return;

	::ShowHide(windowPtr->GetWindowPtr(),true);
	
	if (layerActive)
	{
		ECHO("Selecting window");
		::SelectWindow(windowPtr->GetWindowPtr());
	}
	else
	{
		::BringToFront(windowPtr->GetWindowPtr());
		ECHO("Bringing windwo to front");
	}
	
	// quick process of events so that they happen in place as the block expects
	ActivateFrontMostWindow();
	windowPtr->UpdateWindow();*/
}

CWindow *CCarbonWindowLayer::GetVisibleFrontWindow()
{
	WindowPtr			w=NULL;	
	OSErr				err=::GetIndexedWindow(mWindowGroup,1,kWindowGroupContentsVisible,&w);
	
	if (err)
	{
		if (err!=errWindowNotFound)
			ECHO("Error in CCarbonWindowLayer::GetVisibleFrontWindow(), code " << err);
		return NULL;
	}
		
	CWindow				*cw=GetWindowObject(w);
	
	if (!cw)
		ECHO("Error in CCarbonWindowLayer::GetVisibleFrontWindow(), unreigstered window!");
	
	return cw;
}

CWindow *CCarbonWindowLayer::GetVisibleBackWindow()
{
	WindowPtr			w=NULL;	
	OSErr				err=::GetIndexedWindow(mWindowGroup,CountWindowGroupContents(mWindowGroup,kWindowGroupContentsReturnWindows | kWindowGroupContentsVisible),kWindowGroupContentsVisible,&w);
	
	if (err)
	{
		if (err!=errWindowNotFound)
			ECHO("Error in CCarbonWindowLayer::GetVisibleBackWindow(), code " << err);
		return NULL;
	}
		
	CWindow				*cw=GetWindowObject(w);
	
	if (!cw)
		ECHO("Error in CCarbonWindowLayer::GetVisibleBackWindow(), unreigstered window!");
	
	return cw;
}

CWindow *CCarbonWindowLayer::GetAbsoluteFrontWindow()
{
	WindowPtr			w=NULL;	
	OSErr				err=::GetIndexedWindow(mWindowGroup,1,0,&w);
	
	if (err)
	{
		if (err!=errWindowNotFound)
			ECHO("Error in CCarbonWindowLayer::GetAbsoluteFrontWindow(), code " << err);
		return NULL;
	}
		
	CWindow				*cw=GetWindowObject(w);
	
	if (!cw)
		ECHO("Error in CCarbonWindowLayer::GetAbsoluteFrontWindow(), unreigstered window!");
	
	return cw;
}

CWindow *CCarbonWindowLayer::GetAbsoluteBackWindow()
{
	WindowPtr			w=NULL;	
	OSErr				err=::GetIndexedWindow(mWindowGroup,CountWindowGroupContents(mWindowGroup,kWindowGroupContentsReturnWindows),kWindowGroupContentsVisible,&w);
	
	if (err)
	{
		if (err!=errWindowNotFound)
			ECHO("Error in CCarbonWindowLayer::GetAbsoluteBackWindow(), code " << err);
		return NULL;
	}
		
	CWindow				*cw=GetWindowObject(w);
	
	if (!cw)
		ECHO("Error in CCarbonWindowLayer::GetAbsoluteBackWindow(), unreigstered window!");
	
	return cw;
}

CWindow *CCarbonWindowLayer::GetSelectedWindow()
{
	if (!layerActive)
		return 0L;
		
	WindowPtr			w=NULL;
	CWindow				*found=NULL;
	ItemCount			maxIndex=::CountWindowGroupContents(mWindowGroup,kWindowGroupContentsReturnWindows|kWindowGroupContentsVisible);
	
	for (ItemCount index=1; index<=maxIndex; index++)
	{
		OSErr				err=::GetIndexedWindow(mWindowGroup,1,kWindowGroupContentsVisible,&w);
	
		if (err)
		{
			ECHO("Error in CCarbonWindowLayer::GetSelectedWindow(), code " << err);
			return NULL;
		}
		
		found=GetWindowObject(w);
	
		if (!found)
			ECHO("Error in CCarbonWindowLayer::GetSelectedWindow(), unreigstered window!");
		else
			return found;
	};
	
	return found;
}

void CCarbonWindowLayer::DragWindow(EventRecord &eventRecord,CWindow *windowPtr)
{
	::DragWindow(windowPtr->GetWindowPtr(),eventRecord.where,NULL);
}

void CCarbonWindowLayer::ShowWindow(CWindow* windowPtr)
{	
	::ShowHide(windowPtr->GetWindowPtr(),true);
	
	ActivateFrontMostWindow();
}
