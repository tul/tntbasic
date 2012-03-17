// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CClassicWindowLayer.cpp
// TNT Library
// Mark Tully
// 18/2/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

// Invisible windows quirks
// ------------------------
// There is a problem with the macs window lists. Send behind doesn't send window's behind invisible windows proper. This
// screws things up.
// It means that invisible windows sink to the bottom of the macs window list over time which is bad for when a floater has
// been hidden. The only work around I can see is to main my own window list and whenever I call select window etc to modify
// it.

#include		"MR Debug.h"
#include		"CClassicWindowLayer.h"
#include		"CWindowLayerManager.h"
#include		"CListIndexerT.h"
#include		"CWindow.h"


CWindow* CClassicWindowLayer::GetSelectedWindow()
{
	if (!layerActive)
		return 0L;
		
	CListIndexerT<CWindow>		indexer(&windowsList);
	
	while (CWindow* winPtr=indexer.GetNextData())
	{
		if (winPtr->IsWindowActive())
			return winPtr;
	}
	
	return 0L;
}

// Move the passed window to be in front of the front most window and activate it
// We should also deactivate the current window
void CClassicWindowLayer::SelectWindow(CWindow* windowPtr)
{
	if (windowPtr->IsWindowActive())
		return;

	CWindow*				behindWindow=CWindowLayerManager::GetWindowAboveLayer(this);
		
	// Deactivate the current front window
	DeactivateWindow(GetSelectedWindow());

	SendBehind(windowPtr,behindWindow,true);
	if (!windowPtr->IsWindowVisible())
	{
		//::ShowHide(windowPtr->GetWindowPtr(),true);
		windowPtr->ShowWindow();
		windowPtr->UpdateWindow();
	}
	else
	{
		//ActivateWindow(windowPtr);
		//windowPtr->UpdateWindow();

		ActivateFrontMostWindow();
		windowPtr->UpdateWindow();
	}
}

// This can be used to move a window in the MacOS window list and keep it updated in the window layers menu list
// If behind window is 0L then it's moved IN FRONT OF all other windows. NOTE: This differs from the MacOS send behind
void CClassicWindowLayer::SendBehind(CWindow* window,CWindow* behindWindow,Boolean reorderLayerList)
{
	if (behindWindow)
	{
		::SendBehind(window->GetWindowPtr(),behindWindow->GetWindowPtr());
		
		if (reorderLayerList)
		{
			if (IsWindowInLayer(behindWindow))
				windowsList.MoveElement(windowsList.FindNthElementByData(window,0),windowsList.FindNthElementByData(behindWindow,0),true);
			else
				windowsList.MoveElement(windowsList.FindNthElementByData(window,0),(CListElementT<CWindow>*)NULL,false);
		}
	}
	else
	{
		::BringToFront(window->GetWindowPtr());
		if (reorderLayerList)
			windowsList.MoveElement(windowsList.FindNthElementByData(window,0),(CListElementT<CWindow>*)NULL,false);
	}
}

// By Mark 3 and a 1/2 years later:
//  I think the purpose of this routine was to allow a window to be dragged with a grey rgn without changing it's z ordering. The toolbox drag window
//  changes the z ordering.
//  Now carbon has an official api which does, as far as i can see, everything that CClassicWindowLayer accomplished. At some point I should migrate to this but
//  the docs on it are pretty thin at the mo.
void CClassicWindowLayer::DragWindow(EventRecord &eventRecord,CWindow* windowPtr)
{
	if (!(eventRecord.modifiers&cmdKey))
		windowPtr->SelectWindow();
	
	GrafPtr			savePort;
	Point			thePoint;
	long			newLoc;
#if TARGET_API_MAC_CARBON
	CGrafPtr		wPort;
	WindowPtr		theWindow;
#else
	GrafPort		wPort;
	WindowPeek		theWindow;
#endif
	Rect			dragRect;
	RgnHandle		dragRgn;
	
	if (StillDown())
	{
		GetPort(&savePort);
		SetPort(GetWindowPort(windowPtr->GetWindowPtr()));

		// get the portrect for the window
		Rect			windowPortRect;

#if TARGET_API_MAC_CARBON
		::GetWindowPortBounds(windowPtr->GetWindowPtr(),&windowPortRect);
#else
		windowPortRect=windowPtr->GetWindowPtr()->portRect.left;
#endif
		thePoint.h =windowPortRect.left;
		thePoint.v = windowPortRect.top;
		LocalToGlobal(&thePoint);
		dragRgn = NewRgn();

#if TARGET_API_MAC_CARBON
		::GetWindowRegion(windowPtr->GetWindowPtr(),kWindowStructureRgn,dragRgn);
#else
		CopyRgn(((WindowPeek)windowPtr->GetWindowPtr())->strucRgn, dragRgn);
#endif
		
#if TARGET_API_MAC_CARBON
		wPort=::CreateNewPort();
		RgnHandle		wPortVisRgn=::NewRgn();
		::CopyRgn(GetGrayRgn(),wPortVisRgn);
#else
		OpenPort(&wPort);
		CopyRgn(GetGrayRgn(), wPort.visRgn);
#endif
		theWindow = FrontWindow();
		while (theWindow != windowPtr->GetWindowPtr())
		{
#if TARGET_API_MAC_CARBON
			RgnHandle		winStructRgn=::NewRgn();
			
			::GetWindowRegion(theWindow,kWindowStructureRgn,winStructRgn);
			::DiffRgn(wPortVisRgn,winStructRgn,wPortVisRgn);
			::DisposeRgn(winStructRgn);
			theWindow=::GetNextWindow(theWindow);
#else
			::DiffRgn(wPort.visRgn, theWindow->strucRgn, wPort.visRgn);
			theWindow = theWindow->nextWindow;
#endif
		}

#if TARGET_API_MAC_CARBON
		// install our calculated rgn
		::SetPortVisibleRegion(wPort,wPortVisRgn);		
		::GetRegionBounds(::GetGrayRgn(),&dragRect);
#else	
		dragRect = (*GetGrayRgn())->rgnBBox;
#endif
		InsetRect(&dragRect, 4, 4);
		
		// Here is the heart of the drag action. nil at the end is a space for a DragGrayRgnUPP
		newLoc = DragGrayRgn(dragRgn, eventRecord.where, &dragRect, &dragRect, noConstraint, nil);
		if ((newLoc != 0x80008000) && (newLoc != 0))
			MoveWindow(windowPtr->GetWindowPtr(),thePoint.h+LoWord(newLoc),thePoint.v+HiWord(newLoc),false);
	
		DisposeRgn(dragRgn);

#if TARGET_API_MAC_CARBON
		::DisposePort(wPort);
		::DisposeRgn(wPortVisRgn);
#else
		ClosePort(&wPort);
#endif
		SetPort(savePort);
	}
	else
		windowPtr->SelectWindow();
}

CWindow *CClassicWindowLayer::GetVisibleFrontWindow()
{
	CListIndexerT<CWindow>		indexer(&windowsList);
	CWindow						*windowPtr;
	
	while (windowPtr=indexer.GetNextData())
	{
		if (windowPtr->IsWindowVisible())
			return windowPtr;
	}
	
	return 0L;
}

CWindow *CClassicWindowLayer::GetVisibleBackWindow()
{
	CListIndexerT<CWindow>		indexer(&windowsList);
	CWindow				*windowPtr;
	CWindow				*lastVisibleWindow=0L;
	
	while (windowPtr=indexer.GetNextData())
	{
		if (windowPtr->IsWindowVisible())
			lastVisibleWindow=windowPtr;
	}
	
	return lastVisibleWindow;
}

// These two are like the standard GetFront/BackWindow procs except these return the front most and backmost windows regardless
// of their visiblity, whereas the standard ones return the frontmost/backmost visisibles
// they return 0L if there's no windows
CWindow *CClassicWindowLayer::GetAbsoluteBackWindow()
{
	TListIndex		max=CountWindows();

	if (max)
		return GetNthWindow(max-1);
	else
		return 0L;
}

/*e*/
CWindow *CClassicWindowLayer::GetAbsoluteFrontWindow()
{
	return GetNthWindow(0);
}

// This proc returns the first visible window which is in front of the window you pass it. It uses the windowsList linked list
// to find it.
CWindow *CClassicWindowLayer::GetVisibleWindowInFrontOf(CWindow *window)
{
	CListElementT<CWindow>		*ele=windowsList.FindNthElementByData(window,0);
	
	if (ele)
	{
		while (ele=ele->GetPrevElement())
		{
			if (ele->GetData()->IsWindowVisible())
				return ele->GetData();
		}
	}
	
	// Not found a window in this layer which is in front
	return CWindowLayerManager::GetWindowAboveLayer(this);
}

void CClassicWindowLayer::ShowWindow(CWindow* windowPtr)
{	
	// If the window was hidden then it should be moved into position behind a visible window before being shown
	CWindow*		behindWin=GetVisibleWindowInFrontOf(windowPtr);

	SendBehind(windowPtr,behindWin,false);
	::ShowHide(windowPtr->GetWindowPtr(),true);
	
	ActivateFrontMostWindow();
}
