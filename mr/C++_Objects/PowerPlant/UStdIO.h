// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UStdIO.h
// © Mark Tully 2000
// 8/2/00
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

#include		<LTextEditView.h>
#include		"CLinkedListT.h"

class CStdOutTarget
{
	protected:
		static CLinkedListT<CStdOutTarget>	sTargets;

		CListElementT<CStdOutTarget>		mStdOutLink;
		bool								mStdOutActive;
		
	public:		
											CStdOutTarget();
        virtual                             ~CStdOutTarget();
		
		bool								IsStdOutActive()			{ return mStdOutActive; }
		void								SetStdOutActive(
												bool		inState)	{ mStdOutActive=inState; }

		virtual long /*e*/					WriteCharsToConsole(
												char					*inBuffer,
												long					inSize) = 0;								
			
		static CStdOutTarget				*GetOutputTarget();
};

class CStdInSource
{
	protected:
		static CLinkedListT<CStdInSource>	sSources;

		CListElementT<CStdInSource>			mStdInLink;
		bool								mStdInActive;
		
	public:		
											CStdInSource();

		bool								IsStdInActive()				{ return mStdInActive; }
		void								SetStdInActive(
												bool		inState)	{ mStdInActive=inState; }

		virtual long /*e*/					ReadCharsFromConsole(
												char					*inBuffer,
												long					inSize) = 0;								
			
		static CStdInSource					*GetInputSource();
};

#define ALLOW_MSL_CONSOLE_HACK	!__MACH__

#if ALLOW_MSL_CONSOLE_HACK
void InstallReadConsoleHack();
#endif
