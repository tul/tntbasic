// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CWindowLayer.h
// Mark Tully
// 18/2/98
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

#pragma once

#include						"TLinkedList.h"
#include						"TBufferedBoolean.h"
#include						"TBroadcaster.h"

class CWindow;

#define							BALANCE_ACTIVATIONS		0

class CWindowLayer
{
	friend class CWindowLayerManager;
	friend class CWindow;
	
	protected:
		CListElementT<CWindowLayer>		link;
#if BALANCE_ACTIVATIONS
		TBufferedBoolean				layerActive;
#else
		bool							layerActive;
#endif
		bool							batchHideShow;
		bool							mCheckWindowFrames;
	
		CLinkedListT<CWindow>			windowsList;
		TBroadcaster					mBroadcaster;

		virtual void					ActivateWindow(CWindow *window);
		virtual void					DeactivateWindow(CWindow *window);
		virtual CListElementT<CWindow>	*UnlinkWindow(CWindow *window);
		virtual void					ActivateFrontMostWindow();
		virtual TListIndex				GetWindowIndex(CWindow *window);
		
	public:
		typedef unsigned short			TWindowLayerFlags;
		// Layer flags
		enum
		{
			kHilightAllWindowsInLayer=1,
		};
		
		// messages for broadcasting
		enum
		{
			kWindowLayerActivated,
			kWindowLayerDeactivated
		};

	protected:
		TWindowLayerFlags				flags;

	public:	
		// don't call these directly, instead use CWindow-> routines
		virtual void					SelectWindow(CWindow *windowPtr) =0;
		virtual void					ShowWindow(CWindow *windowPtr) =0;
		virtual void					HideWindow(CWindow *windowPtr);
		

										CWindowLayer(TWindowLayerFlags flags=0);
		virtual							~CWindowLayer() {}
		
		virtual bool					IsWindowInLayer(WindowPtr window);
		virtual bool					IsWindowInLayer(CWindow *window);
		virtual CWindow					*GetWindowObject(WindowPtr windowPtr);
		virtual TListIndex				CountWindows();
		virtual CWindow /*e*/			*GetNthWindow(TListIndex windowIndex);
		TWindowLayerFlags				GetFlags()		{ return flags; }
		CListElementT<CWindowLayer>		*GetLink()		{ return &link; }

		virtual void /*e*/				AddWindow(CWindow *window);
		virtual void					AddWindow(CListElementT<CWindow> *listEle);
		virtual void					RemoveWindow(CWindow *window);
		virtual void					PrepareForBatchHideShow();
		virtual void					EndBatchHideShow();
		
		CLinkedListT<CWindow>			&GetWindowsList() { return windowsList; }
		
		virtual CWindow					*GetVisibleFrontWindow() = 0;
		virtual CWindow					*GetVisibleBackWindow() = 0;
		virtual CWindow					*GetAbsoluteFrontWindow() = 0;
		virtual CWindow					*GetAbsoluteBackWindow() = 0;
		virtual CWindow					*GetSelectedWindow() = 0;
		
		virtual void					RestoreWindowPositions(
											Rect		&inScreenSize);
		virtual void					SaveWindowPositions(
											Rect		&inScreenSize);

		virtual void					DragWindow(EventRecord &eventRecord,CWindow *windowPtr) =0;

		virtual void					DeactivateLayer();
		virtual void					ActivateLayer();

		virtual void					AddListener(
											TListener		*inList);
		
		virtual void					CheckWindowFrames();
};