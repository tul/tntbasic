/***************************************************************************************************
BLAST Moan.c
Extension of BLASTª engine
Mark Tully
28/7/96
***************************************************************************************************/

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

/***************************************************************************************************

BLAST Moan.c
Extension of BLASTª engine
Mark Tully
28/7/96
10/8/96	:	Changed to used more accurate fixed point math
28/8/96	:	Added some more procedures and the ability to use anim resources. Also documented the
			moan structure.

Moan is BLAST's sprite MOvement and ANimation system. It has facilities to automate sprite
movement and animation which is very handy. Just call MoanSprite(&moanptr) to do the
automatic movements

The fields of the moan structure explained in full

*/
#pragma mark Movement explained
/*

See also Flags explained

Many fields of the moan are in fixed point math. This means that they can handle very sensitive
and if neccessary slow movement and animation. To get an integer out of a fixed point number shift
the number right by the constant kBlastFP.
eg.
num = fixed >> kBlastFP;
To put a number into fixed shift it the other way. It is the same as muliplying/dividing by 1024.
This is not where the power of fixed point numbers lie.
Putting in 1024 will move that sprite 1 pixel every frame. Putting in 512 will move it 1 pixel every
2 frames. Not an easy thing to do. The slowest blast can move is 1 pixel every 1024 frames.
eg.
fixed = 512;	// 1 pixel every to frames
Note: No shifing as the 512 is a fixed number. In non-fixed it would be half (0.5 << kBlastFP).

The variables:

x,y					current sprite coords in fixed point

startX,startY		the starting coords in fixed notation. These are used when the moan is reset to
					move the sprite to the start of the sequence.

destX,destY			destination coords in fixed point.

xSpeed,ySpeed		the distance the sprite moves per pass. Fixed point.

*/
#pragma mark Animation explained
/*

See also Flags explained

The same can be done for animation as well as movement.
The variable curFrame holds the frame as a fixed point number. The variable frameRate will be added
on each frame and the frame will change when curFrame has gone up by 1024 (1 in fixed point) like
the movement. This way the anim can change frames every 10 frames on screen or whatever.
Frames for moans are always displayed from the same bank so you can't have frames in different
banks. You can swap the bank a frame is in using SwapImages (BLAST Images.c). The bank is the variable
bankNum.

What you can also do now is use ANIM resources. This is good as all you anim variables can be changed
without the need to recompile. An anim resource stores 2 pieces of information. A frame number,
relative to the beginning of the bank (not the resource number!) and a frame rate for that frame.
Note that it's a frame rate for the frame, not a delay.

So an animation could be such
ANIM	Frame:	Frame rate:
0:		1,		256
1:		2, 		512
2:		3, 		1024

This would display frame 1 for 4 passes then frame 2 for 2 passes then frame 3 for 1. Note that a
number bigger than 1024 will miss out frames and 0 will stop the animation.

The variable curFrame is used as an index into the animList. For example curFrame is 0 it would get
ANIM 0 from the animList (displaying frame 1 not 0). Then when curFrame hits 1024 ANIM 1 will be
displayed. The variable frameRate changes as each frame is used.

The curFrame counter is reset to the lowest valid number for the new frame. This is done as if the
last frame went up (for example) by 900 each time, it would pass 1024 on the second pass and the next
frame would be loaded. The next frames frame rate is 512 as it wants to be on for 2 frames. However 
the counter is already 776 into the new frames time as the last frame went up by more than it needed.
As a result the new frame is only on for 1 pass. To stop this foolishness the curFrame counter is
rounded down so that for each frame it's counter starts a 0.
curFrame is not reset in this way if you are not using animLists as the frameRate is a constant.

The variables:

startFrame 		This is the first frame which will be displayed. This number is in fixed point.

endFrame		This is the stopper frame. This frame will never be displayed. When reached the
				anim either loops or ends. This means that you anim resource must have a dummy frame
				at the end which is never displayed.

curFrame		the current frame in fixed notation.

frameRate		the frame rate in fixed notation

animHandle		a handle to the anim list

bankNum			the number of the bank from which the animations come from

*/
#pragma mark Moan Linking
/*

It is very possible to link moans together so that you can set a spite MOving and ANimating (MOANing)
at the start or your game and it will keep it up for as long as you call MoanSprite in you main loop.
You could have a moan of the sprite moving accros the screen to the right, then turning, then moving
left accross the screen, turing and then back to the first one.

How?!
The field theNextMoan is used for this purpose. Normally you would have it set to 0. However, putting
the address of another moan in here will cause that moan to be run after the current one has finished.
The next moan could well link back to the first moan to loop the moan sequence. When the next moan
is got, ResetMoan is called on it to init it's variables.

More power?
Decision making and evaluation can be added into the moan without a lot of hassel. When the moan
finshes, you can make it call a moan call back which would then alter the moan or change moans
completely. I used this in the spy game to feed in a new destination and animation every time the
spy stopped. This allowed me to have waypoints between the start and destination.
A moan call back proc looks like this

Boolean	MoanCallBack(MoanPtr *theMoan)

Note that you get a ptr to a moan ptr, ie a moan handle. Setting *theMoan to 0L will stop the moaning.
Setting *theMoan to the address of another moan will change the moan. Or you can double dereferece
the handle and change the moan contents directly. If the proc returns true it want's the standard
init to be performed on the moan (the procedure ResetMoan will be called). This sets the current
frame to the first frame. The current position to the start position and resets the flags. If you
don't want this to happen return false. But make sure that you set (**theMoan).finished to false
if you want the moan to carry on (this flag is normally reset in the ResetMoan proc).

*/
#pragma mark Flags
/*

movingReset			set to true if the moan should move. When the moan finishes the flag currently
					moving us set to false. When the moan is later reset the ResetMoan proc needs to
					know whether the moan should be moving and sets the currentlyMoving flag from
					this one. This flag does not change.
currentlyMoving		Set to true while the sprite is moving. Set to false when it get's to it's dest.
					If you want the sprite to move this must be true. It is set of the moving reset
					flag in the ResetMoan proc. If you are not calling that, set it directly.
					
animatingReset
currentlyAnimating	These serve the same purpose for animating as the above ones do for movement.

finshed				Set to true when the moan is finished and requires no more processing. This must
					be set to false if you want the moan to be processed. It is basically the on/off
					switch for the moan.
					
exitOnAnimEnd		if this is set the moan will exit (Exit : stop and link to the next moan if
					there) when the animation finishes. Note that the animation cannot finish if
					animLoop is true.

animLoop			When the last frame is reached, the frame counter is reset to the start frame.

exitOnMoveEnd		exits when the destination is reached.

moveLoop			When the destination is reached, it jumps back to the start coords and moves to
					the dest again.

*/
#pragma mark Misc fields
/*

refCon				the classic mac refcon. It's a long and you can put anything you want in it.
					An example use would be storing a pointer to a structure which gave you info on
					the type of sprite this is like if it was a bullet a baddy or sommat so you
					knew what to do with it on moanCall backs or whatever.
					
spriteNum			the sprite number this moan is linked to.

SECTION 1 - Routines for automatic movement and animation of sprites

***************************************************************************************************/

#pragma mark -

#include	"BLAST.h"

// ********* SECTION 1 - Routines for automatic movement and animation of sprites *********

// a sprites image is only changed if the sprite is being animated.
// This routine changes it to what it should be, using the anim list if necessary
void RedisplayMoanSprite(MoanPtr *theMoan)
{
	short		nowFrame;
	
	if (!*theMoan)
		return;
	
	nowFrame=(**theMoan).curFrame >> kBlastFP;

	// if we have an animation list then we must be prepared to change the frame rate if the
	// frame has just changed
	if ((**theMoan).animHandle)
		nowFrame=((AnimPtr)(*(**theMoan).animHandle+2))[nowFrame].frame;
	SetSpriteImage((**theMoan).spriteNum,(**theMoan).bankNum,nowFrame);
}

// The mighty moan sprite routine

void MoanSprite(MoanPtr *theMoan)
{	
	if (!*theMoan || (**theMoan).finished)
		return;

	if ((**theMoan).currentlyMoving)
	{
		// move in the x direction
		if ((**theMoan).x>(**theMoan).destX)
		{
			(**theMoan).x-=(**theMoan).xSpeed;
			if ((**theMoan).x<(**theMoan).destX)
				(**theMoan).x=(**theMoan).destX;
		}
		else if ((**theMoan).x<(**theMoan).destX)
		{
			(**theMoan).x+=(**theMoan).xSpeed;
			if ((**theMoan).x>(**theMoan).destX)
				(**theMoan).x=(**theMoan).destX;
		}
			
		// move in the y direction
		if ((**theMoan).y>(**theMoan).destY)
		{
			(**theMoan).y-=(**theMoan).ySpeed;
			if ((**theMoan).y<(**theMoan).destY)
				(**theMoan).y=(**theMoan).destY;
		}
		else if ((**theMoan).y<(**theMoan).destY)
		{
			(**theMoan).y+=(**theMoan).ySpeed;
			if ((**theMoan).y>(**theMoan).destY)
				(**theMoan).y=(**theMoan).destY;
		}
			
		// check if movement is finished
		if ((**theMoan).x==(**theMoan).destX && (**theMoan).y==(**theMoan).destY)
		{
			// loop movement if neccessary
			if ((**theMoan).moveLoop)
			{
				(**theMoan).destX=(**theMoan).startX;
				(**theMoan).destY=(**theMoan).startY;
			}
			else
				(**theMoan).currentlyMoving=false;
		}
	}
	
	// handle the animation
	if ((**theMoan).currentlyAnimating)
	{
		short		wasFrame=(**theMoan).curFrame >> kBlastFP;
		short		nowFrame;
		
		(**theMoan).curFrame+=(**theMoan).frameRate;
	
		if ((**theMoan).curFrame>=(**theMoan).endFrame)
		{			
			if ((**theMoan).animLoop)
				(**theMoan).curFrame=(**theMoan).startFrame;
			else
			{
				(**theMoan).currentlyAnimating=false;	// finished the anim
				(**theMoan).curFrame-=(**theMoan).frameRate; // can't display endframe
			}
		}			
	
		nowFrame=(**theMoan).curFrame >> kBlastFP;

		// if we have an animation list then we must be prepared to change the frame rate if the
		// frame has just changed
		if ((**theMoan).animHandle)
		{
			if (nowFrame!=wasFrame)
			{
				// frame has chaged
				(**theMoan).curFrame&=0xFFFFFC00;	// clear the fractional part so that the frame rate below is obeyed
				(**theMoan).frameRate=((AnimPtr)(*(**theMoan).animHandle+2))[nowFrame].frameRate;
			}
			nowFrame=((AnimPtr)(*(**theMoan).animHandle+2))[nowFrame].frame;
		}
		SetSpriteImage((**theMoan).spriteNum,(**theMoan).bankNum,nowFrame);
	}
	
	// do the sprite
	SetSpritePos((**theMoan).spriteNum,(**theMoan).x >> kBlastFP,(**theMoan).y >> kBlastFP);
	
	// check if this moan is finished
	if ((!(**theMoan).currentlyMoving && (**theMoan).exitOnMoveEnd) ||
										 (!(**theMoan).currentlyAnimating && (**theMoan).exitOnAnimEnd))
	{	
		Boolean			standardInit=true;
		
		(**theMoan).finished=true;
		
		// move onto the next moan if possible
		// check the next moan field
		if ((**theMoan).theNextMoan)
			*theMoan=(**theMoan).theNextMoan;
		
		// if no moan in the next moan field then call their call back proc (if there is one)
		else if ((**theMoan).moanCallBack)
			standardInit=(**theMoan).moanCallBack(theMoan);
		else
			return;		// no next moan
		
		// if we have the next moan
		if (*theMoan && standardInit)
			MoanReset(*theMoan);
	}
}

// returns the frame a moan is currently showing
short GetMoanFrame(MoanPtr theMoan)
{
	if (theMoan->animHandle)
		return ((AnimPtr)(*theMoan->animHandle+2))[theMoan->curFrame>>kBlastFP].frame;
	else
		return theMoan->curFrame>>kBlastFP;
}

// changes the frame of a moan to the one given
void SetMoanFrame(MoanPtr theMoan,short theFrame)
{
	theMoan->curFrame=theFrame<<kBlastFP;		// set the current frame for reference
	theMoan->startFrame=theFrame<<kBlastFP;		// set these in case animating is turned on. Don't want to animate with the old frames
	theMoan->endFrame=theFrame<<kBlastFP;		
	theMoan->currentlyAnimating=false;			// turn off animation
	theMoan->animHandle=0L;						// blank the animList so that the frame given is the one used and not an index into this list
	SetSpriteImage(theMoan->spriteNum,theMoan->bankNum,theFrame);	// finally set the image as it won't be set by the moan proc unless it's animating
}

// resets the moan
void MoanReset(MoanPtr theMoan)
{
	if (theMoan)
	{
		// init the moan
		theMoan->x=theMoan->startX;
		theMoan->y=theMoan->startY;
		theMoan->curFrame=theMoan->startFrame;
		theMoan->finished=false;
		theMoan->currentlyMoving=theMoan->movingReset;
		theMoan->currentlyAnimating=theMoan->animatingReset;
	}
}