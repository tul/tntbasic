// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TWindow.h
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

#pragma once

#include		"TSharedListElement.h"

class CWindowLayer;

class CSavedPosition : public CListElementT<CSavedPosition>
{
	public:

		Rect					mScreenRes;
		Rect					mSavedPosition;
								CSavedPosition(
									Rect		&inScreenRect,
									Rect		&inWindowRect) :
									mScreenRes(inScreenRect),
									mSavedPosition(inWindowRect),
									CListElementT(this)
									{}
};

class CWindow
{
	friend class CWindowLayer;
	friend class CClassicWindowLayer;
	friend class CWindowLayerManager;
	friend class CCarbonWindowLayer;

	private:
		WindowPtr				windowPtr;
		CSharedListElementT<CWindow>	link;	
		CWindowLayer			*windowLayer;
		CDeletingLinkedListT<CSavedPosition>	mSavedPositions;
		void					*refCon;
		Boolean					active;
		EventHandlerRef			mHandlerRef;		// only used for carbon windows
		
	protected:
		static EventTypeSpec	sTypesList[];
		virtual void /*e*/		InstallCarbonWindowCallBacks();
		static pascal OSStatus	CarbonWindowHandler(
									EventHandlerCallRef inHandlerCallRef,
									EventRef			inEvent,
									void				*inUserData);

	public:
		/*e*/					CWindow(WindowPtr theWindow,CWindowLayer *windowLayer);
		virtual					~CWindow();
		
		virtual CListElementT<CWindow> *MakeAlias();
		virtual void /*e*/		LinkWindowToList(CLinkedListT<CWindow> *list);
		virtual void			DeleteAliasInList(CLinkedListT<CWindow> *list)	{ link.DeleteAliasInList(list); }
		virtual WindowPtr		GetWindowPtr()						{ return windowPtr; }
		virtual void			WindowActivated()					{ }
		virtual void			WindowDeactivated()					{ }
		virtual Boolean			IsWindowVisible()					{ return /*((WindowPeek)windowPtr)->visible;*/ ::IsWindowVisible(windowPtr); }
		virtual Boolean			IsWindowActive()					{ return active; }
		virtual CWindowLayer	*GetWindowLayer()					{ return windowLayer; }

		virtual void			HideWindow();
		virtual void			ShowWindow();
		virtual void			SelectWindow();
		virtual void			DragWindow(EventRecord &eventRecord);
		virtual void			UpdateWindow();

		virtual void			*GetRefCon()						{ return refCon; }
		virtual void			SetRefCon(void *newRefCon)			{ refCon=newRefCon; }

		virtual void			SaveWindowPosition(
									Rect		&inScreenSize);
		virtual void			RestoreWindowPosition(
									Rect		&inScreenSize);
};
