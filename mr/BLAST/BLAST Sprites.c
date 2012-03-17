/***************************************************************************************************

BLAST Sprites.c
Extension of BLASTª engine
Mark Tully

9/5/96	:	Sprite structure memory handling
13/5/96	:	Sprite draw code
19/5/96	:	BLAST Copy is born. First version is ~25% faster than copybits for GWorld to GWorld 
			copy + about a million other things done. ie wrote section 2
20/5/96	:	Multiple sprite support implemented (well made to work, it was there but was crap)
22/5/96	:	Another bug bites the dust...
			If blast copy got invalid rects (ie right < left or off the world etc) it would crash
			Not a bug in blast copy but one in the code to calculate the rects
 9/6/96 :	Fixed a pretty major bug in blast copy. It must have been by sheer chance that it was
 			working before. When doing a buff2back copy it copied from back2buff instead. (I got
 			a couple of vars mixed up).
19/7/96 :	Got back from holiday and added screen scrolling. It's very slow.
23/8/96 : 	Fixed a bug in add rect to blit list where the flags x touch and y touch were not 
			being reset to false for each test. This meant that one rect could touch on the x only  
			and the next rect would only have to touch on the y to qualify for x and y touch.
28/8/96	:	Added the ability to y prioritise the sprite draw list
26/9/96	:	Added the ability to have scaled sprites.
3/10/96	:	Added the ability to draw direct to screen. As BlastCopy is about 3 times faster than
			copybits this makes a big difference, especially on 603 where I suspect copybits uses
			doubles. (603 dies on a non aligned double blit  going into a software interuppt
			blowing the instruction cache and generally slowing things down, 601 corrects the
			blit in hardware). Moved blast copy and co to it's own file as this one is too big.
24/5/97	:	Changed BLAST Decode.c to take a BCPtr to decode to so had to change this file too

This provides procedures for manipulating sprite data for your sprites. Things like image number
position on screen etc. Image handling is done in BLAST Images.c

SECTION 1 - Sprite memory handling
SECTION 2 - Graphic Routines
SECTION 3 - Sprite control routines

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

#include	"BLAST.h"

//#define		useCopyBits

// Globals
OldBlastSpritePtr	*gSpriteList=0L;	// Pointer to the list of sprites
short				gMaxSprites=0,gNumSprites=0;
Rect				*gScreenRects=0L;	// a list of rects which contain the areas to be copied to screen this vbl
short				gNumRects=0;		// counter for above
static Rect			*gBlastRects=0L;	// a list of rects which will be copied between the gworlds this vbl
static short		gNumBlastRects=0;	// counter for above
static short		gNumUpdateRects=0;	// max num of times BlastInvalRect can be called per update
static Boolean		*gUsed=0L;			// used in the y prioritise functions
short				*gDrawList=0L;			// determines the order sprites are rendered in

// ********* SECTION 1 - Sprite memory handling *********

// call this to allow you to force a copy to the screen from the back world later on.
// it must be called before init sprites and you must check yourself that you don't exceed the
// number of rects here when calling blastinvalrect
void SetNumUpdateRects(short theNum)
{
	gNumUpdateRects=theNum;
}

// Allocates a buffer for requested number of sprites. Each sprite you specify here takes up 4
// bytes of RAM. Allocate them all at once to stop mem frag.
Boolean InitSprites(short numSprites)
{	
	if (gSpriteList || gScreenRects || gBlastRects || gDrawList)
		DisposeSpriteList();
		
	gMaxSprites=numSprites;
	gNumSprites=0;
	gSpriteList=(OldBlastSpritePtr *)NewPtrClear(numSprites*sizeof(OldBlastSpritePtr));
	gScreenRects=(Rect *)NewPtrClear(gMaxSprites*sizeof(Rect)*2+gNumUpdateRects);	// allocate space for the maximum number of rects that will be needed
																// *2 because a rect for where the sprite was and where it is now
	gBlastRects=(Rect *)NewPtrClear(gMaxSprites*sizeof(Rect)*4);
	if (gDrawList=(short *)NewPtr(gMaxSprites*sizeof(short)))
		ResetDrawList();
	if (!gScreenRects || !gBlastRects || !gSpriteList || !gDrawList)	// if we didn't get it then dispose of the sprite list
		DisposeSpriteList();
	return (gSpriteList!=0);
}

// Free up all individual sprites and then the buffer too
void DisposeSpriteList()
{
	if (gSpriteList)
	{
		short count;
	
		for (count=0; count<gNumSprites; count++) // for each sprite do
			DisposeOldBlastSprite(count);
			
		gNumSprites=0;
		gMaxSprites=0;
		DisposePtr((Ptr)gSpriteList);
		gSpriteList=0L;
	}
	
	if (gScreenRects)
	{
		DisposePtr((Ptr)gScreenRects);
		gScreenRects=0L;
	}
		
	if (gBlastRects)
	{
		DisposePtr((Ptr)gBlastRects);
		gBlastRects=0L;
	}
	
	if (gDrawList)
	{
		DisposePtr((Ptr)gDrawList);
		gDrawList=0L;
	}
	
	if (gUsed)
	{
		DisposePtr((Ptr)gUsed);
		gUsed=0L;
	}
}

// Free up the RAM for a particular sprite
void DisposeOldBlastSprite(short pos)
{
	if (gSpriteList[pos]) // don't release if it isn't there
	{
		DisposePtr((Ptr)gSpriteList[pos]); // Release the RAM
		gSpriteList[pos]=0L;		  // And mark it as gone
		ResetDrawList();
	}
}

// pos = -1 means next free slot other wise it must be a valid slot
// if the slot is already allocated then this function will do nothing
// return of -1 means fail, other wise it's the place where it was loaded
short AllocateSprite(short pos)
{
	if (!gSpriteList)		// InitSprites been called?
		return -1;
	
	if (gNumSprites>=gMaxSprites) // Full?
		return -1;
		
	if (pos>=gMaxSprites)	// Is pos valid?
		return -1;
		
	if (pos==-1)		// Next slot mode?
	{
		pos=NextFreeSprite();	
		if (pos==-1)
			return -1;
	}
		
	if (!gSpriteList[pos]) // If sprite already exists do nothing, otherwise...
	{
		// Now a new sprite must be allocated if possible
		gSpriteList[pos]=(OldBlastSpritePtr)NewPtrClear(sizeof(OldBlastSprite));
		if (!gSpriteList[pos])
			return -1; // LACK OF RAM!!!
		
		// got it
		gNumSprites++;
	}
	
	ResetDrawList();
	
	return pos;
}

// Searches through the sprite list for a free slot, returns -1 if none
short NextFreeSprite()
{
	short	count;
	
	// Full or non-existant?
	if (!gSpriteList)
		return -1;
	if (gNumSprites>=gMaxSprites)
		return -1;
		
	for (count=0; count<gMaxSprites; count++)
	{
		if (!gSpriteList[count])
			return count;
	}
	return -1;
}

// Resets the draw list to the defaults. That is the highest number sprite is drawn first down to the
// lowest
// Basically is scan the sprite list and makes a list of all the sprites in use
void ResetDrawList()
{
	short	count,spriteCounter=0;
	
	for (count=gMaxSprites-1; count>=0; count--)
	{
		if (gSpriteList[count])
		{
			// found a sprite in use
			gDrawList[spriteCounter++]=count;		
		}
	}
}

// call this after init sprites if you intend to be calling YPrioritise
Boolean SetUpYPrioritise()
{
	gUsed=(Boolean*)NewPtr(gMaxSprites*sizeof(Boolean));

	return (gUsed!=0L);
}

// Prioritises the draw list so that the sprites lower down are drawn last and so are on top of the
// others. Pass it a spriteNumber to start at. All sprites below this number will be left alone.
// This is useful if you don't want to prioritse the mouse sprite or something.
void YPrioritise(short fromMe)
{
	short		pos=fromMe,spriteID,count,spriteBase,height;
	OldBlastSpritePtr	theSprite;

	// first fill all the used array with false
	while (pos<gNumSprites)
		gUsed[pos++]=false;

	pos=0;
	
	while (pos<(gNumSprites-fromMe)) // for all the sprites after and including fromMe
	{
		height=32767;
		
		for (count=fromMe; count<gNumSprites; count++)
		{
			// check it's not used
			if (gUsed[count])
				continue;
			
			theSprite=gSpriteList[count];
			
			// if the sprite is not to be drawn then simply add it to the list anyway then exit
			if (!theSprite->drawMe)
			{
				spriteID=count;
				break;
			}	
			
			// figure out at what level the bottom of the sprite is
			if (theSprite->scaleMe)
				spriteBase=theSprite->y-ScaledLength((**theSprite->data).yoff,theSprite->yScale)+ScaledLength((**theSprite->data).height,theSprite->yScale);
			else
				spriteBase=theSprite->y-(**theSprite->data).yoff+(**theSprite->data).height;
			
			if (spriteBase<height)
			{
				height=spriteBase;
				spriteID=count;
			}
		}
		
		// found the highest sprite add it to the array
		gUsed[spriteID]=true;
		gDrawList[pos++]=spriteID;
	}
}

// ********* SECTION 2 - Graphic Routines *********

static BCRec			gBackRec=emptyBCRec,gBuffRec=emptyBCRec,gScreenRec=emptyBCRec;
static Rect				gBoundsRect;
static Boolean			gBuffWorldUpdated=false; // this is set if the buff world is fully updated
												 // with the contents of the back world. it can
												 // only be used if there is no scrolling on the
												 // screen. It cuts out a sprite sized copy
// variables used in scrolling
static short			gXShift=0,gYShift=0;	 // the dist the screen is going to move this frame
static short			gXOffset=0,gYOffset=0;	 // absolute screen offsets measured from top left
static Boolean			gDTS=false;				 // Direct to screen drawing

void SetBackWorld(GWorldPtr theWorld)
{	
	MakeBCRecFromGWorld(theWorld,&gBackRec);
}

void SetBuffWorld(GWorldPtr theWorld)
{
	if (gBuffRec.pixMap) // if it's already in use
		UnlockPixels(gBuffRec.pixMap);
	
	MakeBCRecFromGWorld(theWorld,&gBuffRec);
	gBoundsRect=theWorld->portRect;
	ZeroRectCorner(gBoundsRect);
	// FastSetRect(gBoundsRect,50,50,430,590); // test rect
}

// This can only be called if no scrolling is going to occur on the screen
// It copies the contents of BuffWorld into backworld and so the contents behind each sprite
// no longer need to be stored before a sprite draw
// RenderSprites comment which is marked with ****
void UpdateBuffWorld()
{
	#ifdef useCopyBits
	BlastCopyCB(kBack2Buff,&gBoundsRect,&gBoundsRect);
	#else
	BlastCopy(kBack2Buff,&gBoundsRect,&gBoundsRect);
	#endif
	gBuffWorldUpdated=true;
}

// Draw all sprites into work buffer
// For the screen copy operations (ie moving blocks of pixels) the tactic is to store the coords of
// the rectangle which is being copied. Everytime one is stored, if it overlapps another one, or
// touches it it is merged to one bigger rect.
void RenderSprites()
{	
	short		count,tempX,tempY;
	OldBlastSpritePtr	currentSprite;
	Rect		lastURect,tempRect;
	Boolean		scrolling=gXShift|gYShift; // are we scrolling this VBL
	
	// Clear the rect lists
	gNumBlastRects=0;
	
	// First redraw all the chewed up screen made by the last display of sprites
	// Copy from buffworld to back world
	// This loop builds up a list of the rectangle to be updated. They must be updated on the screen
	// and on the backworld
	// Was previously using the same list of updateRects built for the screen to update the backworld
	// this was bas as the merged rects would often capture more area than was necessary, some of
	// which was not stored and so was invalid.
	// This does not happen when it is copied to screen as the copy to screen is done off the
	// backmap which is ALWAYS valid. Buffmap is only valid if there is no scrolling
	for (count=0; count<gNumSprites; count++)
	{
		currentSprite=gSpriteList[count];
		if (currentSprite->updateBackDrop) // this one needs updating
		{
			// make a rect of where to copy from (update rect in global coords)
			tempRect=currentSprite->updateRect;
			FOffset(tempRect,-gXOffset,-gYOffset);
			
			#ifdef useCopyBits
			BlastCopyCB(kBuff2Back,&tempRect,&currentSprite->updateRect); // update the backworld from the buffworld
			#else
			BlastCopy(kBuff2Back,&tempRect,&currentSprite->updateRect); // update the backworld from the buffworld
			#endif
			
			if (!scrolling)
				AddRectToBlitList(&gNumRects,gScreenRects,&currentSprite->updateRect);
				
			currentSprite->updateBackDrop=false;
		}
		
		if (currentSprite->data==0L)
			currentSprite->drawMe=false; // null image
	}
	
	// scroll the screen if necessary
	if (scrolling)
	{
		gXOffset+=gXShift;
		gYOffset+=gYShift;
		FOffset(gBoundsRect,gXShift,gYShift);
	}
	
	// for each sprite back up the backdrop
	for (count=0; count<gNumSprites; count++)	
	{
		currentSprite=gSpriteList[count];
		if (currentSprite->drawMe) // does this sprite need drawing? See SpriteOff()
		{	
			// Make sure we record what backdrop got shifted where so it can be updated later when
			// the sprite is removed from backworld.
			// currentSprite->updateRect is the rect that this sprite is going to occupy.
			lastURect=currentSprite->updateRect;
			
			currentSprite->updateBackDrop=true;
			currentSprite->clipMe=false;
				
			if (currentSprite->scaleMe)
			{	
				currentSprite->updateRect.left=currentSprite->x - ScaledLength((**currentSprite->data).xoff,currentSprite->xScale);
				currentSprite->updateRect.top=currentSprite->y - ScaledLength((**currentSprite->data).yoff,currentSprite->yScale);
				currentSprite->updateRect.right=currentSprite->updateRect.left + ScaledLength((**currentSprite->data).width,currentSprite->xScale);
				currentSprite->updateRect.bottom=currentSprite->updateRect.top + ScaledLength((**currentSprite->data).height,currentSprite->yScale);
			}
			else
			{
				currentSprite->updateRect.left=currentSprite->x - (**currentSprite->data).xoff;
				currentSprite->updateRect.top=currentSprite->y - (**currentSprite->data).yoff;
				currentSprite->updateRect.right=currentSprite->updateRect.left + (**currentSprite->data).width;
				currentSprite->updateRect.bottom=currentSprite->updateRect.top + (**currentSprite->data).height;
			}
			
			// if this sprite is suppose to be displayed independantly of the screen pos then off
			// set it's position such
			if (currentSprite->floating)
				FOffset(currentSprite->updateRect,gXOffset,gYOffset);
			
			// Must clip this to the clip rect (gBoundsRect)
			// if any figures are altered then the sprite needs clipping or doesn't need drawing
			// at all
			if (currentSprite->updateRect.left<gBoundsRect.left)
			{
				currentSprite->updateRect.left=gBoundsRect.left;
				currentSprite->clipMe=true;
			}
			if (currentSprite->updateRect.right>gBoundsRect.right)
			{
				currentSprite->updateRect.right=gBoundsRect.right;
				currentSprite->clipMe=true;
			}
			if (currentSprite->updateRect.top<gBoundsRect.top)
			{
				currentSprite->updateRect.top=gBoundsRect.top;
				currentSprite->clipMe=true;
			}
			if (currentSprite->updateRect.bottom>gBoundsRect.bottom)
			{
				currentSprite->updateRect.bottom=gBoundsRect.bottom;
				currentSprite->clipMe=true;
			}
			
			// In clipping the rect, it may have become invalid. if this has happened then it should
			// NOT be drawn as BlastCopy doesn't cater for bad rects.
			// if the width and height are more than 0.
			if (((currentSprite->updateRect.right-currentSprite->updateRect.left)>0) && ((currentSprite->updateRect.bottom-currentSprite->updateRect.top)>0)) // valid rect?
			{
				// Now the update rect is set up we should copy the data from backworld into buffworld
				// again build a rect list of these.
				// **** No need to stash the backdrop if the buff world already has a perfect copy of it
				// this perfect copy can only be preserved if no scrolling is happening
				if (!gBuffWorldUpdated)
				{
					// A further optimising note that all the rects in gScreenRects are already updated and
					// so don't need to be copied again.
					// As it is, the rects in gScreenRects are the previous update rects for the sprites.
					// Therefore the only data which needs copying to the buff world is the difference
					// between the last update rect and this one.
					// If the rects are the same, no background is exposed and so none needs to be
					// redrawn.
					// This is all handled by the AddDiffenceToBlitList proc.
					
					//if (!RectIsInList(&currentSprite->updateRect,gNumRects,gScreenRects))
					//	AddRectToBlitList(&gNumBlastRects,gBlastRects,&currentSprite->updateRect);
					
					if (scrolling)
						AddRectToBlitList(&gNumBlastRects,gBlastRects,&currentSprite->updateRect);
					else
						AddDifferenceToBlitList(&gNumBlastRects,gBlastRects,&lastURect,&currentSprite->updateRect);
				}
			}
			else
			{
				// if the sprite was invalid then it ain't going to get a sprite stuck on it so
				// the backdrop won't need repairing.
				currentSprite->updateBackDrop=false;
				// updateRect is invalid. Nullify it so lastURect isn't invalid next draw
				((long*)&currentSprite->updateRect)[0]=0L;	// top and left = 0
				((long*)&currentSprite->updateRect)[1]=0L;	// bottom and right = 0
			}
		}
	}
	
	// **** if gBuffWorldUpdated then there is no need to stash the backdrop before overwriting as
	// it is already safe
	if (!gBuffWorldUpdated)
	{
		// A further optimising note is that all the rects in gScreenRects are already updated and
		// so don't need to be copied again.
		// These were filtered out while gRectsList was being built
				
		// copy the screen area which is going to be overwritten to the buffworld
		for (count=0; count<gNumBlastRects; count++)
		{
			// build a temp rect of where to copy to taking into account screen offsets
			tempRect=gBlastRects[count];
			FOffset(tempRect,-gXOffset,-gYOffset);
			
			#ifdef useCopyBits
			BlastCopyCB(kBack2Buff,&gBlastRects[count],&tempRect);
			#else
			BlastCopy(kBack2Buff,&gBlastRects[count],&tempRect);
			#endif
		}
	}
	
	// for each sprite, draw the sprites image, sprite 0 is displayed on top
	for (count=0; count<gNumSprites; count++)
	{
		currentSprite=gSpriteList[gDrawList[count]];
		if (currentSprite->drawMe) // does this sprite need drawing?
		{			
			// Once the data is safe we can overwrite the backdrop
			// if this image doesn't need clipping then don't enter the clipping routine which will
			// constantly check for clipping.
			// Note that if the update rect is set to all zeros then this sprite will be clipped off
			// the screen and we don't need to draw it at all
			// currentSprite->right cannot equal zero unless it has been specifically set to it
			// by the routine which calculates the rect.
			if (currentSprite->updateRect.right>0)
			{
				// if the sprite is floating then make the necessary adjustements to the blit
				// position
				tempX=currentSprite->x;
				tempY=currentSprite->y;
				if (currentSprite->floating)
				{
					tempX+=gXOffset;
					tempY+=gYOffset;
				}
				
				if (currentSprite->scaleMe)
				{
					if (currentSprite->clipMe) // this is set by the backdrop restoring routine
						DecodeImageScaledClip(&gBackRec,currentSprite->data,**(currentSprite->theTint),&gBoundsRect,tempX,tempY,currentSprite->xScale,currentSprite->yScale);
					else
						DecodeImageScaled(&gBackRec,currentSprite->data,**(currentSprite->theTint),tempX,tempY,currentSprite->xScale,currentSprite->yScale); // No clipping needed call the faster no clipping routine				
				}
				else
				{
					if (currentSprite->clipMe) // this is set by the backdrop restoring routine
						DecodeImageClip(&gBackRec,currentSprite->data,**(currentSprite->theTint),&gBoundsRect,tempX,tempY);
					else
						DecodeImageNoClip(&gBackRec,currentSprite->data,**(currentSprite->theTint),tempX,tempY); // No clipping needed call the faster no clipping routine
				}
				
				// Also add it to the list of rects which are to be copied to the screen. The rect
				// will be merged most likely with it's update rect (where it was last vbl) so this
				// is very effcient.
				if (!scrolling)
					AddRectToBlitList(&gNumRects,gScreenRects,&currentSprite->updateRect);
			}
		}
	}
	
	// if scrolling then copy the entire backworld to the screen
	if (scrolling)
	{
		AddRectToBlitList(&gNumRects,gScreenRects,&gBoundsRect);
		gXShift=0;
		gYShift=0;
	}
}

// enables direct to screen drawing. Drawn to the BCRec you pass to this proc.
// BCRecs for windows are created in the same way (use MakeBCRec and pass it a CWindowPtr)
void EnableDTSMode(BCRec *theRec)
{
	gDTS=true;
	gScreenRec=*theRec;
}

void DisableDTSMode()
{
	gDTS=false;
}

// the rects in gScreenRects are in global coords
void UpdateScreen(Boolean fullUpdate)
{
	short 	count;
	Rect	tempRect;
	BCRec	windowRec;
		
	if (fullUpdate)
	{
		if (gDTS)
			BlastCopy(&gBackRec,&gScreenRec,&gBoundsRect,&gPlayWindow->portRect);
		else
		{
			CopyBits((BitMap *)*gBackRec.pixMap,(BitMap *)&gPlayWindow->portBits,&gBoundsRect,&gPlayWindow->portRect,srcCopy,0L);
		}
	}
	else
	{
		// blit each rect in the blit list, NB: Must convert each one from global coords to local
		// coords by offsetting the screen position
		if (gDTS)
		{
			for (count=0; count<gNumRects; count++)
			{
				tempRect.left=gScreenRects[count].left-gXOffset;
				tempRect.right=gScreenRects[count].right-gXOffset;
				tempRect.bottom=gScreenRects[count].bottom-gYOffset;
				tempRect.top=gScreenRects[count].top-gYOffset;
				
				BlastCopy(&gBackRec,&gScreenRec,&gScreenRects[count],&tempRect);
			}
		}
		else
		{
			for (count=0; count<gNumRects; count++)
			{
				tempRect.left=gScreenRects[count].left-gXOffset;
				tempRect.right=gScreenRects[count].right-gXOffset;
				tempRect.bottom=gScreenRects[count].bottom-gYOffset;
				tempRect.top=gScreenRects[count].top-gYOffset;
				
				CopyBits((BitMap *)*gBackRec.pixMap,(BitMap *)&gPlayWindow->portBits,&gScreenRects[count],&tempRect,srcCopy,0L);
			}		
		}
	}
	
	// clear the rect list
	gNumRects=0;
}

// Decides whether a rect is already in a list. The rect must be encloses by one rect inside the list
static Boolean RectIsInList(Rect *theRect,short numRects,Rect *rectList)
{
	short count;
	
	// for each rect in the list
	for (count=0; count<numRects; count++)
	{
		// if "theRect" is completely contained by "rectList[count]"
		if (theRect->left>=rectList[count].left && theRect->right<=rectList[count].right && theRect->top>=rectList[count].top && theRect->bottom<=rectList[count].bottom)
			return true; // it's in the list
	}
	return false;
}

// Adds the diffence between two rects into the rect list. A max of 4 rects can be added by this
// proc per call. That is if the new rect has grown and so screen needs to be updated all around
// it. (I know it's backwards but it is right).
// Note that the rects are *NOT* merged as they are added to the list. As the idea of this proc
// is so that only the thin strips of pixels down each side of the sprite is copied back to the
// back world merging them would produce the rect we started with which would make this whole
// proc a little pointless wouldn't it?
// Another thing is that if the rects don't overlap then this proc just adds this rect and not the
// difference which could be huge if the sprites just leapt accross the screen.
static AddDifferenceToBlitList(short *numRects,Rect *rectList,Rect *lastRect,Rect *thisRect)
{
	Rect		tempRect;
	
	// is the last rect empty? Quickly check by comparing them as longs...
	if ( ((long*)lastRect)[0]==0L && ((long*)lastRect)[1]==0L)
	{
		rectList[(*numRects)++]=(*thisRect);
		return;
	}
		
	// no movement or last rect was bigger than new rect? no need to update any uncovered backdrop
	if (lastRect->left<=thisRect->left && lastRect->right>=thisRect->right && lastRect->top<=thisRect->top && lastRect->bottom>=thisRect->bottom)
		return;
	
	// rects don't touch?
	if ((thisRect->right<lastRect->left) || // too far left
	    (thisRect->left>lastRect->right) || // too far right
	    (thisRect->bottom<lastRect->top) || // too far up
	    (thisRect->top>lastRect->bottom)) // too far down
	{
		// the rects fail to touch so just add thisRect and don't bother with the differeces
		rectList[(*numRects)++]=(*thisRect);
		return;
	}
	
	// Add the top rect if there is one
	if ((lastRect->top-thisRect->top)>0) // top rect exists, add to the list
	{
		tempRect.top=thisRect->top;
		tempRect.left=thisRect->left;
		tempRect.right=thisRect->right;
		tempRect.bottom=lastRect->top;
		rectList[(*numRects)++]=tempRect;
	}
	
	// Add the bottom rect if there is one
	if ((thisRect->bottom-lastRect->bottom)>0) // bottom rect exists, add to the list
	{
		tempRect.bottom=thisRect->bottom;
		tempRect.left=thisRect->left;
		tempRect.right=thisRect->right;
		tempRect.top=lastRect->bottom;
		rectList[(*numRects)++]=tempRect;
	}
	
	// Add the left rect if there is one
	if ((lastRect->left-thisRect->left)>0) // bottom rect exists, add to the list
	{
		tempRect.bottom=lastRect->bottom;
		tempRect.top=lastRect->top;
		tempRect.left=thisRect->left;
		tempRect.right=lastRect->left;
		rectList[(*numRects)++]=tempRect;
	}
	
	// Add the right rect if there is one
	if ((thisRect->right-lastRect->right)>0) // bottom rect exists, add to the list
	{
		tempRect.bottom=lastRect->bottom;
		tempRect.top=lastRect->top;
		tempRect.left=lastRect->right;
		tempRect.right=thisRect->right;
		rectList[(*numRects)++]=tempRect;
	}
}

// forces a rect to be updated on the screen next screen update
void BlastInvalRect(Rect *theRect)
{
	AddRectToBlitList(&gNumRects,gScreenRects,theRect);
}

// Adds a rect to a list of rects to be copied to the screen
// It does so intelligently, merging close rects wherever possible
static AddRectToBlitList(short *rectCount,Rect *theRectList,Rect *theRect)
{
	short 	count;
	Boolean	xTouch,yTouch;
	
	for (count=0; count<*rectCount; count++)
	{
		xTouch=false;
		yTouch=false;
		
		// do the rect touch?
		// check horizontally
		if (theRect->left < theRectList[count].left)
		{
			if (theRect->right >= theRectList[count].left)
				xTouch=true;
		}
		else
		{
			if (theRectList[count].right >= theRect->left)
				xTouch=true;
		}	
	
		// check vertically
		if (theRect->top < theRectList[count].top)
		{
			if (theRect->bottom >= theRectList[count].top)
				yTouch=true;
		}
		else
		{
			if (theRectList[count].bottom >= theRect->top)
				yTouch=true;
		}
		
		if (xTouch && yTouch)
		{
			// merge the rects
			theRectList[count].left=Lesser(theRectList[count].left,theRect->left);
			theRectList[count].right=Greater(theRectList[count].right,theRect->right);
			theRectList[count].top=Lesser(theRectList[count].top,theRect->top);
			theRectList[count].bottom=Greater(theRectList[count].bottom,theRect->bottom);
			return;
		}
	}
	theRectList[(*rectCount)++]=*theRect;	
}

// never, ever,ever set the new clip rect to one which is bigger than the buffWorlds Portrect
// it's generally not a very clever thing to change anyway, it's real important to BLAST or
// more particularly to your Mac's memory...
void SetClipRect(Rect *theRect)
{
	gBoundsRect=*theRect;
}

void GetClipRect(Rect *theRect)
{
	*theRect=gBoundsRect;
}

void SetBoundsRect(short top,short left,short bottom,short right)
{
	gBoundsRect.top=top;
	gBoundsRect.left=left;
	gBoundsRect.right=right;
	gBoundsRect.bottom=bottom;
}


// ********* SECTION 3 - Sprite control procs ********
// Use these to move your sprites around

// changes a sprites anim frame
void SetSpriteImage(short spriteNo,short bankNo,short imageNo)
{
	gSpriteList[spriteNo]->data=gBankList[bankNo]->imageList[imageNo];
	gSpriteList[spriteNo]->theTint=gBankList[bankNo]->bankTint;
	gSpriteList[spriteNo]->drawMe=true;
}

void SetSpriteImageH(short spriteNo,ImageHandle newImage)
{
	gSpriteList[spriteNo]->data=newImage;
	gSpriteList[spriteNo]->drawMe=true;
}

void SetSpriteTintH(short spriteNo,TintHandle theTint)
{
	gSpriteList[spriteNo]->drawMe=true;
	gSpriteList[spriteNo]->theTint=theTint;
}

void SetSpriteFloat(short spriteNo,Boolean floatMe)
{
	gSpriteList[spriteNo]->floating=floatMe;
}

// changes a sprites x,y coords relative to top left of window
void SetSpritePos(short spriteNo,short x,short y)
{
	gSpriteList[spriteNo]->x=x;
	gSpriteList[spriteNo]->y=y;
	gSpriteList[spriteNo]->drawMe=true;
}

void GetSpritePos(short spriteNo,Point *thePos)
{
	thePos->h=gSpriteList[spriteNo]->x;
	thePos->v=gSpriteList[spriteNo]->y;
}

// returns the bounding rect of a sprite at it's current position with it's current image (taking
// into account the image hotspot)
void GetSpriteRect(short spriteNo,Rect *theRect)
{
	OldBlastSpritePtr		theSprite=gSpriteList[spriteNo];
	
	if (theSprite->scaleMe)
	{
		theRect->left=theSprite->x - ScaledLength((**theSprite->data).xoff,theSprite->xScale);
		theRect->top=theSprite->y - ScaledLength((**theSprite->data).yoff,theSprite->yScale);		
		theRect->right=theRect->left + ScaledLength((**theSprite->data).width,theSprite->xScale);
		theRect->bottom=theRect->top + ScaledLength((**theSprite->data).width,theSprite->xScale);
	}
	else
	{
		theRect->left=theSprite->x - (**theSprite->data).xoff;
		theRect->top=theSprite->y - (**theSprite->data).yoff;
		theRect->right=theRect->left + (**theSprite->data).width;
		theRect->bottom=theRect->top + (**theSprite->data).height;
	}
}

void SetSpriteScale(short spriteNo,Fixed xScale,Fixed yScale)
{
	OldBlastSpritePtr		theSprite=gSpriteList[spriteNo];
	
	theSprite->scaleMe=true;
	theSprite->xScale=xScale;
	theSprite->yScale=yScale;
}

void SetupPerspective(PerspecPtr thePersp,short topPos,Fixed topScale,short bottomPos,Fixed bottomScale)
{
	// fill out the perspective bit in the spyglobals struct
//	Fixed	topPos=topPoss<<kBlastFP;
//	Fixed	bottomPos=bottomPoss<<kBlastFP;

//	topScale<<=kBlastFP;
//	bottomScale<<=kBlastFP;
	
	thePersp->topPos=topPos;
	thePersp->bottomPos=bottomPos;
	thePersp->topScale=topScale;
	thePersp->bottomScale=bottomScale;
	thePersp->constant1=(topScale-bottomScale)/(topPos-bottomPos);
	thePersp->constant2=topScale-(thePersp->constant1*topPos);
}

void DoSpritePerspective(PerspecPtr thePersp,short theSprite)
{
	OldBlastSpritePtr		currentSprite=gSpriteList[theSprite];
	Fixed			scale;
	Fixed			pos=currentSprite->y;//<<kBlastFP;
	
	if (pos<thePersp->topPos)
		scale=thePersp->topScale;
	else if (pos>thePersp->bottomPos)
		scale=thePersp->bottomScale;
	else
		scale=thePersp->constant1*pos+thePersp->constant2;
	
	//scale>>=kBlastFP;
	currentSprite->scaleMe=true;
	currentSprite->xScale=scale;
	currentSprite->yScale=scale;
}

void DoMoanPerspective(PerspecPtr thePersp,MoanPtr theMoan)
{
	Fixed 			pos=theMoan->y>>kBlastFP;
	Fixed			scale;
	
	if (pos<thePersp->topPos)
		scale=thePersp->topScale;
	else if (pos>thePersp->bottomPos)
		scale=thePersp->bottomScale;
	else
	//	scale=thePersp->constant1*pos+thePersp->constant2;
	{
		Fixed		initalHeight,finalHeight;
		
		initalHeight=ScaledLength(188,thePersp->bottomScale);
		finalHeight=ScaledLength(188,thePersp->topScale);
		scale=(initalHeight-((initalHeight-finalHeight)/(thePersp->bottomPos-thePersp->topPos))*(thePersp->bottomPos-pos))/initalHeight;
	}
	
	SetSpriteScale(theMoan->spriteNum,scale,scale);
	
//	theMoan->xSpeed=ScaledLength(4096,scale);
//	theMoan->ySpeed=ScaledLength(2<<kBlastFP,scale);
}

void SpriteScaleOff(short spriteNo)
{
	OldBlastSpritePtr		theSprite=gSpriteList[spriteNo];
	
	theSprite->scaleMe=false;
}

void SpritesScaleOff()
{
	short count;
	
	for (count=0; count<gNumSprites; count++)
		SpriteScaleOff(count);
}

// stops sprite "num" being drawn. It will be erased if it's on the screen at the next call to
// RenderSprites. it will be shown automatically next time SetSpritePos is used on it.
void SpriteOff(short num)
{
	gSpriteList[num]->drawMe=false;
}

// Turns off all sprites. See above.
void SpritesOff()
{
	short count;
	
	for (count=0; count<gNumSprites; count++)
		SpriteOff(count);
}

// makes sprite "num" be drawn
void SpriteOn(short num)
{
	gSpriteList[num]->drawMe=true;
}

// Turns on all sprites. See above.
void SpritesOn()
{
	short count;
	
	for (count=0; count<gNumSprites; count++)
		SpriteOn(count);
}

// Offsets the screen to the coords specified
void ScreenOffset(short x,short y)
{
	short		buffWidth=gBoundsRect.right-gBoundsRect.left;
	short		buffHeight=gBoundsRect.bottom-gBoundsRect.top;

	gXShift=x;
	gYShift=y;

	// check for invalid scrolls
	// too far to the right?
	if (x+(gXOffset+buffWidth)>=(**gBackRec.pixMap).bounds.right)
	{
		// set it to scroll to the edge, but no further
		gXShift=(**gBackRec.pixMap).bounds.right-(gXOffset+buffWidth);
	}
	
	// too far left
	if (x+gXOffset<(**gBackRec.pixMap).bounds.left)
	{
		// set it to scroll to the edge, but no further
		gXShift=(**gBackRec.pixMap).bounds.left-gXOffset;
	}
	
	// too far up
	if (y+gYOffset<(**gBackRec.pixMap).bounds.top)
	{
		// set it to scroll to the edge, but no further
		gYShift=(**gBackRec.pixMap).bounds.top-gYOffset;
	}
	
	// too far down?
	if (y+(gYOffset+buffHeight)>=(**gBackRec.pixMap).bounds.bottom)
	{
		// set it to scroll to the edge, but no further
		gYShift=(**gBackRec.pixMap).bounds.bottom-(gYOffset+buffHeight);
	}
}