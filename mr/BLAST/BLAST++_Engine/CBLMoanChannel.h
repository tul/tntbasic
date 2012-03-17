// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLMoanChannel.h
// © Mark Tully 2002
// 17/2/02
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

#include "Blast_Types.h"

struct SBLMoanFrame
{
	SInt32			image;		// image index to be displayed
	SInt32			delay;		// number of passes to stay on screen for
};

class CBLMoanChannel
{
	protected:
		// if true, channel has no affect
		bool				mChannelPaused;
	
		// xoff and yoff range from 0 to +/- 1 (not inclusive). They add up until they go over 1 then
		// they are added to the sprites position and rounded down.
		// xvel and yvel are added to the xoff and yoff as long as there are movement steps remaining
		float				mXOff,mYOff,mXVel,mYVel;
		SInt32				mTotalX,mTotalY;
		SInt32				mMovementStepsRemaining;

		// the frame counter is the index into the frames list
		// the repeat count is how many times to play the frames list for (-1 for indefinitely)
		// the frame delay is the number of passes this frame will remain set for		
		UInt32				mFrameCounter,mFrameDelay,mMaxFrames;
		SInt32				mAnimRepeatCount;

		// scaling		
		float				mXScaleVel,mYScaleVel;
		SInt32				mScaleStepsRemaining;
		
		// rotating
		float				mRotateVel;
		SInt32				mRotateStepsRemaining;
		bool				mRotateForever;

		// arbitary max for max number of frame entries in an animation list, increase if not enough
		static const UInt16	kMaxFrames=50;
		
		// animation list
		SBLMoanFrame		**mFramesList;
		
		// call back for end of movement/animation/scale/rotate, override if needed (e.g. to load new settings)
		virtual void		MovementFinished()			{};
		virtual void		AnimationFinished()			{};
		virtual void		ScaleFinished()				{};
		virtual void		RotateFinished()			{};
		
	public:
							CBLMoanChannel();
		virtual				~CBLMoanChannel();
		
		virtual bool		IsAnimating();
		virtual bool		IsMoving();
		virtual bool		IsScaling();
		virtual bool		IsRotating();
		
		virtual void		StepMoan(
								TBLImageVector			&outXDelta,
								TBLImageVector			&outYDelta,
								TBLImageVector			&outNewImage,
								float					&outXScale,
								float					&outYScale,
								float					&outRDelta);
								
		void				SetChannelPause(
								bool	inState)		{ mChannelPaused=inState; }
		bool				IsChannelPaused()			{ return mChannelPaused; }

		// stop movement/anim without calling callback		
		virtual void		ResetMovement()				{ mMovementStepsRemaining=0; }
		virtual void		ResetAnimation()			{ mAnimRepeatCount=0; }
		virtual void		ResetScale()				{ mScaleStepsRemaining=0; }
		virtual void		ResetRotate()				{ mRotateStepsRemaining=0; mRotateForever=false; }
		
		virtual void		InstallMovement(
								float				inXDelta,
								float				inYDelta,
								float				inSpeed);
		virtual void		InstallMovementBySteps(
								float				inXDelta,
								float				inYDelta,
								float				inSteps);
		virtual void		InstallScale(
								float				inXDelta,
								float				inYDelta,
								float				inSpeed);
		virtual void		InstallScaleBySteps(
								float				inXDelta,
								float				inYDelta,
								float				inSteps);
		virtual void		InstallRotate(
								float				inRDelta,
								float				inSpeed,
								bool				inForever);
		virtual void		InstallRotateBySteps(
								float				inRDelta,
								float				inSteps);


		virtual SBLMoanFrame /*e*/	**PrepareToInstallAnim(
										TBLImageVector		inNumEntries);
		virtual void				AnimInstalled(
										TBLImageVector		inRepeatCount);

};