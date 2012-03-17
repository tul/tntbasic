// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CWindowLayer.cpp
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

// Abstract class for managing a layer of windows

#include		"MR Debug.h"
#include		"CWindowLayer.h"
#include		"CWindow.h"
#include		"CListIndexerT.h"
#include		"CLinkedListT.h"
#include		"CWindowLayerManager.h"
#include		"Root.h"

void CWindowLayer::SaveWindowPositions(
	Rect		&inScreenSize)
{
	CListIndexerT<CWindow>	indexer(&windowsList);
	
	while (CWindow *window=indexer.GetNextData())
	{
		window->SaveWindowPosition(inScreenSize);
	}
}

void CWindowLayer::RestoreWindowPositions(
	Rect		&inScreenSize)
{
	CListIndexerT<CWindow>		indexer(&windowsList);
	
	while (CWindow *window=indexer.GetNextData())
	{
		window->RestoreWindowPosition(inScreenSize);
	}
}

bool CWindowLayer::IsWindowInLayer(WindowPtr theWindow)
{
	return (GetWindowObject(theWindow)!=0L);
}

bool CWindowLayer::IsWindowInLayer(CWindow *theWindow)
{
	return (theWindow->windowLayer==this);
}

CWindowLayer::CWindowLayer(TWindowLayerFlags flags) : layerActive(true), link(this)
{
	this->flags=flags;
	CWindowLayerManager::AddLayer(this);
	batchHideShow=false;
}

/*e*/
// When you add the window it should be hidden, then use select window to select it
void CWindowLayer::AddWindow(CWindow *windowPtr)
{
	CListElementT<CWindow>		*listEle=windowPtr->MakeAlias();

	if (!listEle)
		Throw_(memFullErr);

	AddWindow(listEle);
}

void CWindowLayer::AddWindow(CListElementT<CWindow> *listEle)
{
	CWindow			*windowPtr=listEle->GetData();
	
	windowsList.AppendElement(listEle);
	windowPtr->windowLayer=this;
	
	if (flags&kHilightAllWindowsInLayer)
		HiliteWindow(windowPtr->GetWindowPtr(),true);
}

CListElementT<CWindow> *CWindowLayer::UnlinkWindow(CWindow *windowPtr)
{
	Boolean				winWasActive=windowPtr->IsWindowActive();	
	CListElementT<CWindow>		*elePtr=windowsList.FindNthElementByData(windowPtr,0);

	windowsList.UnlinkElement(elePtr);
	windowPtr->windowLayer=0L;
	
	if (winWasActive)
		ActivateFrontMostWindow();
	
	return elePtr;
}

// You should hide the window before removing it using HideWindow
void CWindowLayer::RemoveWindow(CWindow *windowPtr)
{
	CListElementT<CWindow>		*elePtr=UnlinkWindow(windowPtr);
	
	if (elePtr)
		delete elePtr;
}

TListIndex CWindowLayer::GetWindowIndex(CWindow *window)
{
	CListElementT<CWindow>		*ele=windowsList.FindNthElementByData(window,0);
	
	if (!ele)
		return 0;
	
	return ele->GetCurrentIndex();
}

CWindow *CWindowLayer::GetWindowObject(WindowPtr windowPtr)
{
	CListIndexerT<CWindow>		indexer(&windowsList);

	while (CWindow *windowIndex=indexer.GetNextData())
	{
		if (windowIndex->GetWindowPtr()==windowPtr)
			return windowIndex;
	}
	return 0L;
}

TListIndex CWindowLayer::CountWindows()
{
	return windowsList.CountElements();
}

/*e*/
CWindow *CWindowLayer::GetNthWindow(TListIndex windowIndex)
{
	CWindow			*winPtr;
	
	winPtr=windowsList.GetNthElementData(windowIndex);
	if (!winPtr)
		Throw_(paramErr);
		
	return winPtr;
}

void CWindowLayer::PrepareForBatchHideShow()
{
	batchHideShow=true;
}

void CWindowLayer::EndBatchHideShow()
{
	batchHideShow=false;
	
	ActivateFrontMostWindow();
}

void CWindowLayer::AddListener(
	TListener		*inList)
{
	mBroadcaster.AddListener(inList);
}

// Use these two for activing and deactivating a layer

// Deactivates the selected window in a layer and stops any other windows from being activated. You can still add windows
// to the layer, they'll just remain in their deactive state. (NB : All windows are deactive when first added )
void CWindowLayer::DeactivateLayer()
{
	CWindow*		selectedWindow=GetSelectedWindow();	// Get the selected window before deactivating the layer, when the layers deactive, there is no selected window

#if BALANCE_ACTIVATIONS
	switch (layerActive=false)
	{
		case TBufferedBoolean::kBooleanCleared:
			// This disables the frontmost window in all layers
			DeactivateWindow(selectedWindow);
			break;
	}
#else
	if (layerActive)
	{
		layerActive=false;
		DeactivateWindow(selectedWindow);
	}
#endif
	mBroadcaster.BroadcastMessage(kWindowLayerDeactivated);
}

// Activates the specfied layer. It enables the frontmost window in the layer and allows windows to be selected again.
void CWindowLayer::ActivateLayer()
{
	// This enables the frontmost window in all layers
#if BALANCE_ACTIVATIONS
	switch (layerActive=true)
	{
		case TBufferedBoolean::kBooleanSet:
			ActivateWindow(GetVisibleFrontWindow());
			break;
	}
#else
	if (!layerActive)
	{
		layerActive=true;
		ActivateWindow(GetVisibleFrontWindow());
	}
#endif	
	mBroadcaster.BroadcastMessage(kWindowLayerActivated);
}

void CWindowLayer::ActivateWindow(CWindow* theWindow)
{
	if (theWindow && !theWindow->IsWindowActive() && theWindow->IsWindowVisible() && layerActive && !batchHideShow)
	{
		theWindow->active=true;
	
		HiliteWindow(theWindow->GetWindowPtr(),true);
		
		//LMSetCurActivate(theWindow->GetWindowPtr());	// don't understand this so rem it!
		theWindow->WindowActivated();
		theWindow->UpdateWindow();
	}
}

void CWindowLayer::DeactivateWindow(CWindow* theWindow)
{
	if (theWindow && theWindow->IsWindowActive() /* && layerActive */ )
	{
		theWindow->active=false;
	
		if (!(flags&kHilightAllWindowsInLayer))
			HiliteWindow(theWindow->GetWindowPtr(),false);
		
		//LMSetCurDeactive(theWindow->GetWindowPtr());
		theWindow->WindowDeactivated();
		theWindow->UpdateWindow();
	}
	
	// sanity check - ensure the window frame state is synced with the window content state
	if (::IsWindowActive && (/*!layerActive ||*/ (!(flags&kHilightAllWindowsInLayer))))
	{
		if (theWindow && ::IsWindowActive(theWindow->GetWindowPtr()))
		{
			mCheckWindowFrames=true;
//			ECHO("Window active state was wrong??? - Correcting");
//			::HiliteWindow(theWindow->GetWindowPtr(),false);
		}
	}
}

// This proc ensures that only the front window of the layer is selected. If it's not then it deactivates the active window
// and makes the one at the front active. It does for all layers.
void CWindowLayer::ActivateFrontMostWindow()
{
	if (batchHideShow)
		return;
		
	// If the front most visible window should be the selected window and no other windows in the layer should be selected
	// make it so
	// Go through the list backwards, deactivating any active windows until the first window is encountered which is
	// activated. The idea is to stop multiple window's being active simultaneously
	CListIndexerT<CWindow>	indexer(&windowsList,false);
	CWindow*				visibleFrontWindow=GetVisibleFrontWindow();

	while (CWindow* currentWindow=indexer.GetPrevData())
	{
		if (currentWindow!=visibleFrontWindow || !layerActive)
			DeactivateWindow(currentWindow);
		else
			ActivateWindow(currentWindow);
	}
	
	// Now group all the layers windows together. It could be that the layers are mixed together so we seperate them now.
//	OrderWindowsIntoListOrder();
}


void CWindowLayer::HideWindow(CWindow* windowPtr)
{
	::ShowHide(windowPtr->GetWindowPtr(),false);
	
	DeactivateWindow(windowPtr);
	
	ActivateFrontMostWindow();
}

// Sometimes the window frames could end up in a different state to the window content - this
// seems to have gone away as of OS X 10.1.5. If it comes back, I'll finish this code which
// was supposed to check the window frame state when Activate and Deactivate window had
// reason to be suspicious.
void CWindowLayer::CheckWindowFrames()
{

}