// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// COutputWindow.h
// © Mark Tully 2000
// 16/3/00
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

#pragma once

#include		<PP_Types.h>
#include		"UStdIO.h"

class			LWindow;
class			LCommander;
class			LTextEditView;

class COutputWindow : public CStdOutTarget
{
	protected:
		LWindow					*mWindow;
		PaneIDT					mWinId;
		bool					mRemapNewLines;
				
		static const PaneIDT	kPaneId_Text='Text';
		static const ResIDT		kPPob_OutputWindow=9999;

		static SInt16			sAppResFile;
		static LCommander		*sSuperCommander;
		
		virtual LTextEditView	*GetTextEditView();
		
		virtual void			CopyDataAndRemap(
									char		*outBuffer,
									const char	*inBuffer,
									Size		inSize);
		
	public:

		// if using multiple COutputWindows give each a different id otherwise they'll all use the same window
		// NB : These ids must be different from the PaneId of any of your apps windows else COutputWindow will get confused
								COutputWindow(
									PaneIDT		inWindowId='OutW') : mWindow(0), mWinId(inWindowId), mRemapNewLines(true) {}
		virtual					~COutputWindow();

		virtual long /*e*/		WriteCharsToConsole(
									char						*inBuffer,
									long						inSize);

		static void				SetSuperCommander(
									LCommander		*inComm) 	{ sSuperCommander=inComm; }

		virtual LWindow			*GetWindow();
		virtual LWindow			*GetWindowNoOpen();
};
