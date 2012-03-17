// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CWindowLayerManager.cpp
// TNT Library
// Mark Tully
// 19/5/02
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

// Manages window layers which map to window groups on OS X and window lists on classic

#include "CWindowLayerManager.h"
#include "CWindowLayer.h"
#include "CWindow.h"
#include "CListIndexerT.h"

CLinkedListT<CWindowLayer>		CWindowLayerManager::sLayersList;

// Use these two when you need to disable all windows for example when you app goes into the background.

// ---- Static
void CWindowLayerManager::ActivateAllLayers()
{
	CListIndexerT<CWindowLayer>		indexer(&sLayersList);
	
	while (CWindowLayer *layerPtr=indexer.GetNextData())
		layerPtr->ActivateLayer();
}

// ---- Static
void CWindowLayerManager::DeactivateAllLayers()
{
	CListIndexerT<CWindowLayer>		indexer(&sLayersList);

	while (CWindowLayer *layerPtr=indexer.GetNextData())
		layerPtr->DeactivateLayer();
}

void CWindowLayerManager::SaveAllWindowPositions(
	Rect		&inScreenSize)
{
	CListIndexerT<CWindowLayer>		indexer(&sLayersList);
	
	while (CWindowLayer *layerPtr=indexer.GetNextData())
		layerPtr->SaveWindowPositions(inScreenSize);	
}

void CWindowLayerManager::RestoreAllWindowPositions(
	Rect		&inScreenSize)
{
	CListIndexerT<CWindowLayer>		indexer(&sLayersList);
	
	while (CWindowLayer *layerPtr=indexer.GetNextData())
		layerPtr->RestoreWindowPositions(inScreenSize);	
}

// ---- Static
// This proc makes sure all the windows are in the correct order. You only need to call this if you have reason to believe
// that some OS call has altered the order of the windows thus screwing up the layering.
void CWindowLayerManager::NormalizeWindowOrder()
{
	CWindow			*lastFrontWindow=0L;
	CListIndexerT<CWindowLayer>	indexer(&sLayersList);

	while (CWindowLayer * layerPtr=indexer.GetNextData())
	{
		{
			CListIndexerT<CWindow>	windowIndexer(&layerPtr->GetWindowsList());
			Boolean					firstInLayer=true;
			
			while (CWindow *windowPtr=windowIndexer.GetNextData())
			{
				if (lastFrontWindow)
					::SendBehind(windowPtr->GetWindowPtr(),lastFrontWindow->GetWindowPtr());					
				else
					::BringToFront(windowPtr->GetWindowPtr());
				lastFrontWindow=windowPtr;

				// Set window hilight
				if (firstInLayer || layerPtr->GetFlags()&CWindowLayer::kHilightAllWindowsInLayer)
				{
					firstInLayer=false;
					::HiliteWindow(windowPtr->GetWindowPtr(),true);
				}
				else
					::HiliteWindow(windowPtr->GetWindowPtr(),false);
			}
		}
	}	
}

void CWindowLayerManager::AddLayer(
	CWindowLayer	*inLayer)
{
	sLayersList.LinkElement(inLayer->GetLink());
}

// This proc returns the window which all windows in this layer should be placed behind. If it returns 0L then the windows
// should be placed in front of all other windows.
// See note at top of file regarding invisible windows
CWindow *CWindowLayerManager::GetWindowAboveLayer(
	CWindowLayer		*inLayer)
{
	CListElementT<CWindowLayer>		*elePtr=inLayer->GetLink()->GetPrevElement();
	
	if (!elePtr)
		return 0L;
		
	CWindowLayer *		layerPtr=elePtr->GetData();
	
	if (layerPtr->GetVisibleBackWindow())
		return layerPtr->GetVisibleBackWindow();
	else
		return GetWindowAboveLayer(layerPtr);
}

// ---- Static
// This routine transfers a window from one layer to another
void CWindowLayerManager::TransferWindow(
	CWindowLayer		&inSourceLayer,
	CWindowLayer		&inDestLayer,
	CWindow				*inWindow)
{
	// Find the window in the source layer and unlink it
	CListElementT<CWindow>	*listEle=inSourceLayer.UnlinkWindow(inWindow);

	if (listEle)
		inDestLayer.AddWindow(listEle);
}
