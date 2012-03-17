// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLMoanChannel.cpp
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

/*
	MOAN
	Moan is MOvement, ANimation for sprites. A channel handles all the movement and animation tracking
	for a sprite (or group of sprites). When the channel is stepped, it returns a new x,y and image
	for the sprites on the channel. A moan should be stepped once every tick through the game.
	
	They support call backs for when animation or movement is finished. A channel could in theory be
	used to animate other game elements besides sprites, for example viewports.

	A moan channel allows tracking of movement between a start and an end location, along with
	animation using a (frame,delay) list.
	
	Moan channels support callbacks for when the animation finishes or when the movement ends.
*/

#include "CBLMoanChannel.h"
#include "Blast.h"
#include "Root.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBLMoanChannel::CBLMoanChannel() :
	mChannelPaused(false),
	mXOff(0),mYOff(0),mXVel(0),mYVel(0),mMovementStepsRemaining(0), mTotalX(0), mTotalY(0),
	mFrameCounter(0),mFrameDelay(0),mAnimRepeatCount(0), mMaxFrames(0), mFramesList(0),
	mXScaleVel(0.0f), mYScaleVel(0.0f), mScaleStepsRemaining(0), mRotateVel(0), mRotateStepsRemaining(0),
	mRotateForever(false)
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBLMoanChannel::~CBLMoanChannel()
{
	if (mFramesList)
		::DisposeHandle((Handle)mFramesList);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StepMoan
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the difference that should be applied to the x,y of the sprites on this channel, and the
// new image (-1 for no change)
void CBLMoanChannel::StepMoan(
	TBLImageVector			&outXDelta,
	TBLImageVector			&outYDelta,
	TBLImageVector			&outNewImage,
	float					&outXScaleDelta,
	float					&outYScaleDelta,
	float					&outRotateDelta)
{
	if (mChannelPaused)
	{
		outXDelta=outYDelta=0;
		outNewImage=-1;
		outXScaleDelta=outYScaleDelta=outRotateDelta=0.0f;
		return;
	}

	bool					callMoveCallback=false;
	
	// are we moving?
	if (mMovementStepsRemaining)
	{
		mXOff+=mXVel;
		mYOff+=mYVel;
		
		if (mXOff>=1.0f || mXOff<=-1.0f)
		{
			outXDelta=(SInt32)mXOff;
			mXOff-=outXDelta;
		}
		else
			outXDelta=0;
				
		if (mYOff>=1.0f || mYOff<=-1.0f)
		{
			outYDelta=(SInt32)mYOff;
			mYOff-=outYDelta;
		}
		else
			outYDelta=0;
		
		// update the total movement counters and make sure we've not moved too far (due to rounding errors)
		if (outXDelta)
		{
			if ( (mTotalX>0 && ((mTotalX-=outXDelta)<0)) || (mTotalX<0 && ((mTotalX-=outXDelta)>0)) )
			{
				// too far...
				outXDelta-=mTotalX;				// adjust speed
				mXVel=0;						// x movement complete
			}
		}		
		if (outYDelta)
		{
			if ( (mTotalY>0 && ((mTotalY-=outYDelta)<0)) || (mTotalY<0 && ((mTotalY-=outYDelta)>0)) )
			{
				// too far...
				outYDelta-=mTotalY;				// adjust speed
				mYVel=0;						// x movement complete
			}
		}
		
		if (!--mMovementStepsRemaining)		// end of movement?
		{
			// check x and y totals to ensure we've done them all (might not have due to rounding errors)
			if (mTotalX)
				outXDelta+=mTotalX;
			if (mTotalY)
				outYDelta+=mTotalY;
			callMoveCallback=true;			// We should call the movement call back after the animations have been processed. This is because it could change the anim settings and they might have another frame left to play yet.
		}
	}
	else
		outXDelta=outYDelta=0;
	
	if (mScaleStepsRemaining)
	{
		outXScaleDelta=mXScaleVel;
		outYScaleDelta=mYScaleVel;
		if (!--mScaleStepsRemaining)
			ScaleFinished();
	}
	else
		outXScaleDelta=outYScaleDelta=0.0f;
	
	if (mRotateForever)
		outRotateDelta=mRotateVel;
	else
	{
		if (mRotateStepsRemaining)
		{
			outRotateDelta=mRotateVel;
			if (!--mRotateStepsRemaining)
				RotateFinished();
		}
		else
			outRotateDelta=0.0f;
	}
		
	outNewImage=-1;		

	// are we animating
	if (mAnimRepeatCount)
	{
		if (mFrameDelay)
			mFrameDelay--;
		else
		{
			// frame delay has reached 0, time for next frame
			mFrameCounter++;
			if (mFrameCounter>=mMaxFrames)	// reached end of frames list
			{
				mFrameCounter=0;
				if (mAnimRepeatCount!=-1)	// -1 means loop forever
				{
					mAnimRepeatCount--;
					if (!mAnimRepeatCount)	// end of anim
						AnimationFinished();
					else
					{
						mFrameDelay=(*mFramesList)[0].delay;
						outNewImage=(*mFramesList)[0].image;
					}
				}
				else
				{
					mFrameDelay=(*mFramesList)[0].delay;
					outNewImage=(*mFramesList)[0].image;
				}
			}
			else
			{
				mFrameDelay=(*mFramesList)[mFrameCounter].delay;
				outNewImage=(*mFramesList)[mFrameCounter].image;
			}
		}
	}
	
	if (callMoveCallback)
		MovementFinished();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsAnimating
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns true if the next call to StepMoan will animate (may not change the frame, a delay is part of the anim too)
bool CBLMoanChannel::IsAnimating()
{
	return mAnimRepeatCount!=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsMoving
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns true if the next call to StepMoan will move (may not move, if speed is < 1 pix per step)
bool CBLMoanChannel::IsMoving()
{
	return mMovementStepsRemaining;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsRotating
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns true if the next call to StepMoan will rotate (may not rotate, if speed is < 1 pix per step)
bool CBLMoanChannel::IsRotating()
{
	return mRotateStepsRemaining || mRotateForever;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsScaling
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns true if the next call to StepMoan will scale
bool CBLMoanChannel::IsScaling()
{
	return mScaleStepsRemaining;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallScale
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the channel scaling by the delta at the specified units/sec
void CBLMoanChannel::InstallScale(
	float				inXDelta,
	float				inYDelta,
	float				inSpeed)
{
	float				dist;
	
	ResetScale();
	
	if (!inSpeed || (!inXDelta && !inYDelta))
		return;
	
	// to work the distance out, the only way I can see of doing so is to do a sqrt. You can
	// probably avoid doing this if you change the movement to work like a breesham line
	// algorithm, but then you might not be able to determine how many steps there would be.
	// Anyway, you can bypass the sqrt for some situations, and you can probably use a sqrt
	// approximation function too, but I don't have one to hand.
	if (inXDelta==0.0f)
		dist=FAbs(inYDelta);
	else if (inYDelta==0.0f)
		dist=FAbs(inXDelta);
	else if (inXDelta==inYDelta)
	{
		// take one delta and multiply by 1/cos(45) = 1.41421
		dist=FAbs(inXDelta)*1.41421f;
	}
	else
		dist=::sqrtf(inXDelta*inXDelta+inYDelta*inYDelta);
	
	mScaleStepsRemaining=dist/inSpeed;
	if (!mScaleStepsRemaining)
		mScaleStepsRemaining=1;

	mXScaleVel=inXDelta/mScaleStepsRemaining;
	mYScaleVel=inYDelta/mScaleStepsRemaining;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallRotateBySteps
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Rotates by inRDelta in the steps specified
void CBLMoanChannel::InstallRotateBySteps(
	float				inRDelta,
	float				inSteps)
{
	ResetRotate();
	
	if (!inSteps || !inRDelta)
		return;

	mRotateStepsRemaining=inSteps;
	mRotateVel=inRDelta/inSteps;
	mRotateForever=false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallRotate
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the channel rotating by the delta at the specified units/sec
// inForever == true, inSpeed can be negative and represents the dir of movement
// inForever == false, inSpeed is absolute and will move in the dir of inRDelta
void CBLMoanChannel::InstallRotate(
	float				inRDelta,
	float				inSpeed,
	bool				inForever)
{
	ResetRotate();
	
	if (!inSpeed || (!inForever && !inRDelta))
		return;

	if (inForever)
	{
		mRotateForever=true;
		mRotateStepsRemaining=0;
		mRotateVel=inSpeed;
	}
	else
	{
		mRotateForever=false;		
		mRotateStepsRemaining=FAbs(inRDelta)/inSpeed;
		if (!mRotateStepsRemaining)
			mRotateStepsRemaining=1;

		mRotateVel=inRDelta/mRotateStepsRemaining;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallScaleBySteps
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Scales by inXDelta and inYDelta steps in the steps specified
void CBLMoanChannel::InstallScaleBySteps(
	float				inXDelta,
	float				inYDelta,
	float				inSteps)
{
	ResetScale();
	
	if (!inSteps || (!inXDelta && !inYDelta))
		return;

	mScaleStepsRemaining=inSteps;
	mXScaleVel=inXDelta / inSteps;
	mYScaleVel=inYDelta / inSteps;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallMovement
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the channel moving by the delta at the specified pix/sec
// If you want to have finer control over the x,y speeds individually, i suggest using multiple channels
void CBLMoanChannel::InstallMovement(
	float			inXDelta,
	float			inYDelta,
	float			inSpeed)
{
	ResetMovement();

	if (!inSpeed || (!SInt32(inXDelta) && !SInt32(inYDelta)))
		return;

	float		dist;
	
	// to work the distance out, the only way I can see of doing so is to do a sqrt. You can
	// probably avoid doing this if you change the movement to work like a breesham line
	// algorithm, but then you might not be able to determine how many steps there would be.
	// Anyway, you can bypass the sqrt for some situations, and you can probably use a sqrt
	// approximation function too, but I don't have one to hand.
	if (inXDelta==0)
		dist=FAbs(inYDelta);
	else if (inYDelta==0)
		dist=FAbs(inXDelta);
	else if (inXDelta==inYDelta)
	{
		// take one delta and multiply by 1/cos(45) = 1.41421
		dist=((float)FAbs(inXDelta))*1.41421f;
	}
	else
		dist=::sqrtf(inXDelta*inXDelta+inYDelta*inYDelta);
	
	mMovementStepsRemaining=dist/inSpeed;
	if (!mMovementStepsRemaining)
		mMovementStepsRemaining=1;
	
	mXOff=mYOff=0;
	mTotalX=inXDelta;
	mTotalY=inYDelta;	
	
	mXVel=inXDelta/mMovementStepsRemaining;
	mYVel=inYDelta/mMovementStepsRemaining;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallMovementBySteps
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Moves to dest point in the specified number of steps
void CBLMoanChannel::InstallMovementBySteps(
	float			inXDelta,
	float			inYDelta,
	float			inSteps)
{
	ResetMovement();
	
	if (!inSteps || (!inXDelta && !inYDelta))
		return;

	mMovementStepsRemaining=inSteps;
	mXOff=mYOff=0;
	mTotalX=inXDelta;
	mTotalY=inYDelta;	
	mXVel=inXDelta / mMovementStepsRemaining;
	mYVel=inYDelta / mMovementStepsRemaining;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PrepareToInstallAnim						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Prepares to install an animation of the number of entries specified. This sets the max number
// of entries and resizes the storage as needed. The storage is then returned as AN UNLOCKED HANDLE
// to be filled out by the caller.
// When complete call AnimInstalled and pass the repeat count to start the anim
SBLMoanFrame **CBLMoanChannel::PrepareToInstallAnim(
	TBLImageVector		inNumEntries)
{
	ResetAnimation();					// turn off any playing anim

	if (inNumEntries!=mMaxFrames)		// gotta resize
	{
		if (!mFramesList)
		{
			mFramesList=(SBLMoanFrame**)::NewHandle(inNumEntries*sizeof(SBLMoanFrame));
			ThrowIfMemFull_(mFramesList);
		}
		else
			ThrowIfOSErr_(::BetterSetHandleSize((Handle)mFramesList,inNumEntries*sizeof(SBLMoanFrame),0));
			
		mMaxFrames=inNumEntries;
	}
	
	return mFramesList;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AnimInstalled
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Call this after you've called PrepareToInstallAnim and filled out the resultant handle
// this starts the animation. Use -1 for infinite loop and other for fixed amount
void CBLMoanChannel::AnimInstalled(
	TBLImageVector			inRepeatCount)
{
	if (inRepeatCount<-1)
		inRepeatCount=-1;

	if (mMaxFrames)	
		mAnimRepeatCount=inRepeatCount;
	else
		mAnimRepeatCount=0;		// don't start anim if we have no frames
	
	// this will cause the anim to be loaded on the next moan
	mFrameCounter=-1;
	mFrameDelay=0;
}