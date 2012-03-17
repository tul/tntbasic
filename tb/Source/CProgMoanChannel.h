// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CProgMoanChannel.h
// © Mark Tully 2002
// 10/3/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

#pragma once

#include "TNTBasic_Types.h"
#include "CBLMoanChannel.h"
#include "CCString.h"
#include "CVMVirtualMachine.h"
#include "CListElementT.h"

class CTBSprite;
class CProgram;
class CViewport;
class CProgram;

class CProgMoanChannel : public CVMVirtualMachine, public CBLMoanChannel
{
	private:
		typedef CVMVirtualMachine	inheritedChannel;
		
	public:
		enum EProgState
		{
			kRunning,
			kDone,
			kBlockedByMove,
			kBlockedByAnim,
			kBlockedByScale,
			kBlockedByRotate
		};

	protected:	
		TTBInteger			mId;
		EProgState			mState;
		CLinkedListT<CTBSprite>	mSprites;		// list of aliases to sprites
		CLinkedListT<CViewport>	mViewports;		// list of aliases to viewports
		TTBInteger			mLocalRegs[10];		// R0-R9
		static TTBInteger	sSharedRegs[26];	// RA-RZ
		CProgram			*mProgram;

		virtual void		MovementFinished();
		virtual void		AnimationFinished();
		virtual void		ScaleFinished();
		virtual void		RotateFinished();
		
		virtual void		ThrowRuntimeError(
								OSErr			inErr);
		virtual void		ThrowUndefinedLabel(
								char			inLabel);
	
	public:
		CListElementT<CProgMoanChannel>		mLink;
		
		static void			ResetSharedRegs();
		
							CProgMoanChannel(
								CProgram		*inProg,
								TTBInteger		inId);
		virtual				~CProgMoanChannel();
								
		void /*e*/			SetProgram(
								const CCString	&inString);
								
		void /*e*/			StepProgram();
		void				RunMoanSlice();
		
		SInt32				GetId()				{ return mId; }

		void				LoadCode(
								CVMCodeChunk	*inCode);

		void				SetState(
								EProgState		inState) { mState=inState; }
								
		void				AddSpriteToChannel(
								CTBSprite		*inSprite);
		void				RemoveSpriteFromChannel(
								CTBSprite		*inSprite);
		void				AddViewportToChannel(
								CViewport		*inVP);
		void				RemoveViewportFromChannel(
								CViewport		*inVP);
		virtual void		RestartProgram();

		virtual void /*e*/	SetReg(
								char			inRegId,
								SInt32			inValue);
		virtual SInt32 /*e*/GetReg(
								char			inRegId);
		static void /*e*/	SetSharedReg(
								char			inRegId,
								SInt32			inValue);
		static SInt32 /*e*/	GetSharedReg(
								char			inRegId);
		
		bool				ChannelPlaying()	{ return mState!=kDone; }
};