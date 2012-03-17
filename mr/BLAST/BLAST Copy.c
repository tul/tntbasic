/***************************************************************************************************

BLAST Copy.c
Extension of BLASTª engine
Mark Tully

3/10/96	:	Too many Blast copy and blast copy related routines about, decided to collect them
			to one file.
17/10/96:	Changed MakeBCRec to take into account the pixmaps bounds. The top left of this sez
			how far into the pixmap we should start drawing. If it sez -50 then it means skip in
			50 pixels. If you don't then mad things happen. So MakeBCRec now adjusts the baseAddr
			to go past this. It only really showed up when copying to screen as with gworlds you
			tend to have their pixmap bounds zero cornered. However, if you're window wasn't at
			top left corner of the screen and you blast copied, the image would miss the window,
			stragely without crashing or anything, just plastering it over you desktop.

2/1/00	:	Made BCToABCRec fill out the height and width of the ABCRec.

This provides procedures for copying bitmap data from and to 8 bit bitmaps. It's as fast as I
can do and has low over head. BCRecs are special records prepped by MakeBCRec and co.

SECTION 1 - BlastCopy

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
#include	"Blast_Debug.h"

#ifdef powerc
static 		Boolean			g601Chip=false; // stops us blitting doubles on non 601s
#endif

// checks if the mac can handle doubles or not
void Check601()
{
	#ifdef powerc
	g601Chip=(CPUType()==gestaltCPU601);	// do we have a 601 to blit doubles at a decent speed
	Set601Presence(g601Chip);
	#endif
}

// uses copy bits
void BlastCopyCB(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect)
{
	CopyBits((BitMap*)*source->pixMap,(BitMap *)*dest->pixMap,sourceRect,destRect,srcCopy,0L);
}

// copies from sourcerect to destrect, but at the dest, maskRect is not violated. This is a little
// like rgn clipping but is very fast.
void BlastCopyMaskRect(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect,Rect *maskRect)
{
	unsigned char	*srcPtr,*destPtr,*srcRowStart,*destRowStart;
	unsigned long	miscCount;
	unsigned long	widthToCopy=sourceRect->right-sourceRect->left;
	unsigned long	srcRB,destRB;
	signed long		mrdl=maskRect->right-destRect->left;
	signed long		mrdr=maskRect->right-destRect->right;
	signed long		mldl=maskRect->left-destRect->left;
	signed long		mldr=maskRect->left-destRect->right;
	short			yCount,maskTop=maskRect->top,maskBottom=maskRect->bottom;
	signed long		st=sourceRect->top,dt=sourceRect->top;
	signed long		drmr,mrml,mbmt;	// only used for split masks
	unsigned char	mode;
		
	if (mrdl>0 && mrdr<0 && mldl<=0)
		mode=1;
	else if (mldl>0 && mldr<0 && mrdr>=0)
		mode=2;
	else if (mldl>0 && mrdr<0)
	{
		// mode 3 is a split masking, we need some more constants
		mrml=maskRect->right-maskRect->left;
		drmr=destRect->right-maskRect->right;
		mode=3;
	}
	else if (mldl<=0 && mrdr>=0)
	{
		mode=6;
		if (maskTop>dt)
			mbmt=maskBottom-maskTop;
		else
		{
			if (maskBottom>dt)
			{
				mbmt=maskBottom-dt;//-1;
				st+=mbmt;
				dt+=mbmt;
			}
			mode=0;			// the skips been done
		}
	}
	else
		mode=0;
	
	srcRB=source->rowBytes;
	destRB=dest->rowBytes;
	srcRowStart=source->baseAddr+srcRB*st+sourceRect->left;
	destRowStart=dest->baseAddr+destRB*dt+destRect->left;
	
	for (yCount=dt; yCount<destRect->bottom; yCount++)
	{
		srcPtr=srcRowStart;
		srcRowStart+=srcRB;
		destPtr=destRowStart;
		destRowStart+=destRB;
		
		miscCount=widthToCopy;
		
		if (mode!=0 && yCount>=maskTop && yCount<maskBottom)
		{
			switch (mode)
			{
				case 1:
					srcPtr+=mrdl;
					destPtr+=mrdl;
					miscCount-=mrdl;
					break;
				
				case 2:
					miscCount=mldl;
					break;
					
				case 3:
					miscCount=mldl;
					mode=9;
					break;
					
				case 6:
					// skip mbmr rows
					yCount+=mbmt;
					srcRowStart+=srcRB*mbmt;
					destRowStart+=destRB*mbmt;
					continue;
					break;
			}
		}
		
		blit:		// evil goto, necessary this time
		
		// Move the data in the biggest chunks possible
		#ifdef powerc
		// raa PowerPC 601, 8 bytes in one move command
		if (g601Chip)
		{	
			while (miscCount>=sizeof(double))
			{
				*((double *)destPtr)++=*((double *)srcPtr)++;
				miscCount-=sizeof(double);
			}
		}
		#endif
		
		while(miscCount>=sizeof(unsigned long))
		{
			*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
			miscCount-=sizeof(unsigned long);				
		}
		
		while(miscCount>=sizeof(unsigned short))
		{
			*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
			miscCount-=sizeof(unsigned short);				
		}
		
		while(miscCount>=sizeof(unsigned char))
		{
			*((unsigned char *)destPtr)++=*((unsigned char *)srcPtr)++;
			miscCount-=sizeof(unsigned char);				
		}
	
		// watch for split masks
		if (mode==9)
		{
			mode=3;
			
			srcPtr+=mrml;
			destPtr+=mrml;
			miscCount=drmr;
			goto blit;
		}
	}
}

// BLASTCopy beats copybits at a gworld to gworld copy by ~25%. RAAAA! Also copies to screen,
// how compatable it is at doing this I don't know.
void BlastCopy(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect)
{
	unsigned char	*srcPtr,*destPtr,*srcRowStart,*destRowStart;
	unsigned long	miscCount;
	unsigned long	yCount;
	unsigned long	heightToCopy=sourceRect->bottom-sourceRect->top;
	unsigned long	widthToCopy=sourceRect->right-sourceRect->left;
	unsigned long	srcRB,destRB;
	
	srcRB=source->rowBytes;
	destRB=dest->rowBytes;
	srcRowStart=source->baseAddr+srcRB*sourceRect->top+sourceRect->left;
	destRowStart=dest->baseAddr+destRB*destRect->top+destRect->left;
	
	for (yCount=0; yCount<heightToCopy; yCount++)
	{
		srcPtr=srcRowStart;
		srcRowStart+=srcRB;
		destPtr=destRowStart;
		destRowStart+=destRB;
		
		miscCount=widthToCopy;
	
		// Move the data in the biggest chunks possible
		#ifdef powerc
		// raa PowerPC 601, 8 bytes in one move command
		if (g601Chip)
		{	
			while (miscCount>=sizeof(double))
			{
				*((double *)destPtr)++=*((double *)srcPtr)++;
				miscCount-=sizeof(double);
			}
		}
		#endif
		
		while(miscCount>=sizeof(unsigned long))
		{
			*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
			miscCount-=sizeof(unsigned long);				
		}
		
		while(miscCount>=sizeof(unsigned short))
		{
			*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
			miscCount-=sizeof(unsigned short);				
		}
		
		while(miscCount>=sizeof(unsigned char))
		{
			*((unsigned char *)destPtr)++=*((unsigned char *)srcPtr)++;
			miscCount-=sizeof(unsigned char);				
		}
	}
}

// Speed issues for BlastCopy
// --------------------------
// If the source/dest are aligned on the same 8 bit boundary then they are double aligned, and we can blit doubles without fear of misalignment
// If they're not then we shouldn't blit doubles as it will cost - big time

void BlastCopyDoubleAligned(
	unsigned char		*inSource,
	unsigned char		*inDest,
	unsigned long		inSourceRB,
	unsigned long		inDestRB,
	unsigned long		inLength, // inLength in bytes
	unsigned long		inRows);

inline Boolean IsDoubleAligned(
	unsigned char		*inSourcePtr,
	unsigned char		*inDestPtr,
	unsigned long		inSourceRB,
	unsigned long		inDestRB);	

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsDoubleAligned
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
inline Boolean IsDoubleAligned(
	unsigned char		*inSourcePtr,
	unsigned char		*inDestPtr,
	unsigned long		inSourceRB,
	unsigned long		inDestRB)
{
	return ( ((UInt32)inSourcePtr&7)==((UInt32)inDestPtr&7) ) &&	// same lower 3 bits means same relative spacing in memory
	(inSourceRB&7)==0 && (inDestRB&7)==0;		// rowbytes must be a multiple of 8, thus ensuring that relative alignment is constant
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BlastCopyDoubleAligned
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Blits sections of copy that are 8 byte aligned using doubles, blits stragglers either ends using longs/shorts/bytes
void BlastCopyDoubleAligned(
	unsigned char		*inSource,
	unsigned char		*inDest,
	unsigned long		inSourceRB,
	unsigned long		inDestRB,
	unsigned long		inLength, // inLength in bytes
	unsigned long		inRows)
{
	unsigned char		*srcPtr=inSource,*destPtr=inDest;
	unsigned long		leftSectionLength,midSectionLength,rightSectionLength,miscCount;		// all in bytes
	unsigned char		*destRowEnd=inDest+inLength;

	// Verify
	BlastAssert_(IsDoubleAligned(inSource,inDest,inSourceRB,inDestRB));
		
	// copy is split into 3 for each row
	// copy until 8 byte aligned
	// copy 8 byte aligned data
	// copy until end of row

	// Examples:
		
	// 012345670123456701234567012345670
	// |*******|*******|*******|*******|
	//      ++++++++						left=3, mid=0, right=5
	//   +++								left=3, mid=0, right=0
	//    +++++++++++++++					left=5, mid=8, right=2
	//       ++++							left=2, mid=0, right=2

	if (inLength>=8)
	{	
		// work out length of left side
		if ((UInt32)srcPtr&7)
			leftSectionLength=8-(UInt32)srcPtr&7;
		else
			leftSectionLength=0;
		
		// work out length of right section
		rightSectionLength=(UInt32)destRowEnd&7;

		// if both left and right are in the same 8 byte block then we do all copying in the left side
		if ((leftSectionLength+rightSectionLength)>inLength)
		{
			midSectionLength=0;
			rightSectionLength=0;
		}
		else
			midSectionLength=inLength-(leftSectionLength+rightSectionLength);
	}
	else
	{
		leftSectionLength=inLength;
		midSectionLength=0;
		rightSectionLength=0;
	}

	// double check our maths if in debug mode
	BlastAssert_(inLength==leftSectionLength+rightSectionLength+midSectionLength);	// check total data length
	BlastAssert_((midSectionLength&7)==0);	// multiple of 8

#if BLAST_DEBUG
	// make sure we've not hosed our mid section 8 byte aligned stuff
	unsigned char	*midSource=srcPtr+leftSectionLength,*midDest=destPtr+leftSectionLength;
	if (midSectionLength)
	{
		BlastAssert_(((UInt32)midSource&7)==0);
		BlastAssert_(((UInt32)midDest&7)==0);
	}
#endif
	
	// copy across rows and then down	
	inRows++;
	
	while (--inRows)
	{
		// copy left side
		if (leftSectionLength)
		{
			miscCount=leftSectionLength;
			while(miscCount>=sizeof(unsigned long))
			{
				*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
				miscCount-=sizeof(unsigned long);				
			}		
			while(miscCount>=sizeof(unsigned short))
			{
				*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
				miscCount-=sizeof(unsigned short);
			}
			if (miscCount)
				*destPtr++=*srcPtr++;
		}
		
		// copy mid section using doubles
		miscCount=midSectionLength;
		while (miscCount>=sizeof(double))
		{
			*((double *)destPtr)++=*((double *)srcPtr)++;
			miscCount-=sizeof(double);
		}
				
		// copy right side
		if (rightSectionLength)
		{
			miscCount=rightSectionLength;
			while(miscCount>=sizeof(unsigned long))
			{
				*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
				miscCount-=sizeof(unsigned long);				
			}		
			while(miscCount>=sizeof(unsigned short))
			{
				*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
				miscCount-=sizeof(unsigned short);				
			}
			if (miscCount)
				*destPtr++=*srcPtr++;
		}
				
		// next row
		inSource+=inSourceRB;
		srcPtr=inSource;
		inDest+=inDestRB;
		destPtr=inDest;
	}
}
	

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BlastCopy16
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BlastCopy16 is used in the same way as BlastCopy except it does 16 bit deep copies
void BlastCopy16(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect)
{
	unsigned char	*srcPtr,*destPtr,*srcRowStart,*destRowStart;
	unsigned long	miscCount;
	unsigned long	yCount;
	unsigned long	heightToCopy=sourceRect->bottom-sourceRect->top;
	unsigned long	widthToCopy=sourceRect->right-sourceRect->left;
	unsigned long	srcRB,destRB;
	
	srcRB=source->rowBytes;
	destRB=dest->rowBytes;
	srcRowStart=source->baseAddr+srcRB*sourceRect->top+sourceRect->left*sizeof(unsigned short);
	destRowStart=dest->baseAddr+destRB*destRect->top+destRect->left*sizeof(unsigned short);
	
	if (IsDoubleAligned(srcRowStart,destRowStart,srcRB,destRB))
	{
		BlastCopyDoubleAligned(srcRowStart,destRowStart,srcRB,destRB,widthToCopy*2,heightToCopy);
		return;
	}
	
	for (yCount=0; yCount<heightToCopy; yCount++)
	{
		srcPtr=srcRowStart;
		srcRowStart+=srcRB;
		destPtr=destRowStart;
		destRowStart+=destRB;
		
		miscCount=widthToCopy;
	
		// Move the data in the biggest chunks possible
		#ifdef powerc
		// raa PowerPC 601, 8 bytes in one move command
		if (g601Chip)
		{	
			while (miscCount>=sizeof(double)/2)
			{
				*((double *)destPtr)++=*((double *)srcPtr)++;
				miscCount-=sizeof(double)/2;
			}
		}
		#endif
		
		while(miscCount>=sizeof(unsigned long)/2)
		{
			*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
			miscCount-=sizeof(unsigned long)/2;				
		}
		
		while(miscCount>=sizeof(unsigned short)/2)
		{
			*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
			miscCount-=sizeof(unsigned short)/2;				
		}
	}
}

// Richard Mckeon
void BlastCopy16Mask(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect,unsigned short maskColour)
{
	unsigned char	*srcPtr,*destPtr,*srcRowStart,*destRowStart;
	unsigned long	miscCount;
	unsigned long	yCount;
	unsigned long	heightToCopy=sourceRect->bottom-sourceRect->top;
	unsigned long	widthToCopy=sourceRect->right-sourceRect->left;
	unsigned long	srcRB,destRB;

	srcRB=source->rowBytes;
	destRB=dest->rowBytes;
	srcRowStart=source->baseAddr+srcRB*sourceRect->top+sourceRect->left*2;
	destRowStart=dest->baseAddr+destRB*destRect->top+destRect->left*2;
	
	for (yCount=0; yCount<heightToCopy; yCount++)
	{
		srcPtr=srcRowStart;
		srcRowStart+=srcRB;
		destPtr=destRowStart;
		destRowStart+=destRB;
		
		miscCount=widthToCopy;
	
/*		// Move the data in the biggest chunks possible
		#ifdef powerc
		// raa PowerPC 601, 8 bytes in one move command
	//	if (g601Chip)
	//	{	
			while (miscCount>=sizeof(double)/2)
			{
//				*((double *)destPtr)++=*((double *)srcPtr)++;
//				miscCount-=sizeof(double)/2;
				if((short *)srcPtr==maskColour)
					(short *)(&copyDouble)==*(short *)
			}
	//	}
		#endif
		
		while(miscCount>=sizeof(unsigned long)/2)
		{
			*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
			miscCount-=sizeof(unsigned long)/2;				
		}*/
		
		while(miscCount>=sizeof(unsigned short)/2)
		{
			if(*((short *)srcPtr)!=maskColour)
				*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
			else
			{
				*((unsigned short *)destPtr)++;
				*((unsigned short *)srcPtr)++;
			}
			miscCount-=sizeof(unsigned short)/2;				
		}
		
	}
}

// Raa BlastCopyScaled routines, with optional clipping to clipRect
// scale < 1024 means scale up
// scale > 1024 means scale down
// scale = 1024 means no scale
void BlastCopyScaledClip(BCPtr sourceRec,BCPtr destRec,Rect *origScaleRect,Rect *clipRect,
											short destX,short destY,Fixed xScale,Fixed yScale)
{
	Rect			scaleRect=*origScaleRect;
	unsigned long	rowsDone=0L;
	Fixed			xCounter,yCounter,width,height;
	unsigned char	*rowStart,*destPtr,*origRowStart,*origDestPtr;
	
	// calc the destination width and height
	width=(scaleRect.right-scaleRect.left)<<kBlastFP;
	width-=(width%xScale);
	height=(scaleRect.bottom-scaleRect.top)<<kBlastFP;
	height-=(height%yScale);
	
	if (destY<clipRect->top)
	{
		// the top is too high
		scaleRect.top+=((clipRect->top-destY)*yScale)>>kBlastFP;
		height-=((clipRect->top-destY)*yScale);
		destY=clipRect->top;
	}
	
	if (destX<clipRect->left)
	{
		// the left is too far left
		scaleRect.left+=((clipRect->left-destX)*xScale)>>kBlastFP;
		width-=((clipRect->left-destX)*xScale);
		destX=clipRect->left;
	}
	
	// right too far right?
	if (destX+(width/xScale)>clipRect->right)
		width-=(destX+(width/xScale)-clipRect->right)*xScale;
	
	// bottom too low?
	if (destY+(height/yScale)>clipRect->bottom)
		height-=(destY+(height/yScale)-clipRect->bottom)*yScale;
	
	// if we have completely clipped the shape then bail
	if (height<=0 || width<=0)
		return;
	
	yCounter=0;
	origRowStart=sourceRec->baseAddr+(scaleRect.top*sourceRec->rowBytes)+scaleRect.left;
	origDestPtr=destRec->baseAddr+(destY*destRec->rowBytes)+destX;
	
	while (yCounter<height)
	{
		xCounter=0;
		
		rowStart=origRowStart+((yCounter>>kBlastFP)*sourceRec->rowBytes);
		yCounter+=yScale;

		destPtr=origDestPtr+(rowsDone*destRec->rowBytes);
		rowsDone++;

		while (xCounter<width)
		{
			*destPtr=*(rowStart+(xCounter>>kBlastFP));
			destPtr++;
			xCounter+=xScale;
		}
	}
}


void BlastCopyScaledNoClip(BCPtr sourceRec,BCPtr destRec,Rect *scaleRect,short destX,short destY,
																	Fixed xScale,Fixed yScale)
{
	unsigned long	rowsDone=0L;
	Fixed			xCounter,yCounter,width,height;
	unsigned char	*rowStart,*destPtr,*origRowStart,*origDestPtr;
	
	width=(scaleRect->right-scaleRect->left)<<kBlastFP;
	width-=(width%xScale);
	height=(scaleRect->bottom-scaleRect->top)<<kBlastFP;
	height-=(height%yScale);
	
	yCounter=0;
	origRowStart=sourceRec->baseAddr+(scaleRect->top*sourceRec->rowBytes)+scaleRect->left;
	origDestPtr=destRec->baseAddr+(destY*destRec->rowBytes)+destX;
	
	while (yCounter<height)
	{
		xCounter=0;
		
		rowStart=origRowStart+((yCounter>>kBlastFP)*sourceRec->rowBytes);
		yCounter+=yScale;

		destPtr=origDestPtr+(rowsDone*destRec->rowBytes);
		rowsDone++;

		while (xCounter<width)
		{
			*destPtr=*(rowStart+(xCounter>>kBlastFP));
			destPtr++;
			xCounter+=xScale;
		}
	}
}

void BlastCopyScaledNoClip16(BCPtr sourceRec,BCPtr destRec,Rect *scaleRect,short destX,short destY,
																	Fixed xScale,Fixed yScale)
{
	unsigned long	rowsDone=0L;
	Fixed			xCounter,yCounter,width,height;
	unsigned char	*rowStart,*destPtr,*origRowStart,*origDestPtr;
	
	width=(scaleRect->right-scaleRect->left)<<kBlastFP;
	width-=(width%xScale);
	height=(scaleRect->bottom-scaleRect->top)<<kBlastFP;
	height-=(height%yScale);
	
	yCounter=0;
	origRowStart=sourceRec->baseAddr+(scaleRect->top*sourceRec->rowBytes)+(scaleRect->left*sizeof(short));
	origDestPtr=destRec->baseAddr+(destY*destRec->rowBytes)+(destX*sizeof(short));
	
	while (yCounter<height)
	{
		xCounter=0;
		
		rowStart=origRowStart+((yCounter>>kBlastFP)*sourceRec->rowBytes);
		yCounter+=yScale;

		destPtr=origDestPtr+(rowsDone*destRec->rowBytes);
		rowsDone++;

		while (xCounter<width)
		{
			*(short*)destPtr=*(short*)(((unsigned long)(rowStart+(xCounter>>(kBlastFP-1))))&0xFFFFFFFE);	// & masks out the lowest bit which should be cleared
			((short*)destPtr)++;
			xCounter+=xScale;
		}
	}
}

// this proc performs the same as going bcRec=emptyBCRec
void InitBCRec(BCPtr rec)
{
	rec->bcRecVersion=kCurrentBCRecVersion;	// always kCurrentBCRecVersion
	rec->world=0L;
	rec->pixMap=0L;
	rec->baseAddr=0L;
	rec->rowBytes=0L;
}

// this proc performs the same as going abcRec=emptyABCRec
void InitABCRec(ABCPtr rec)
{
	InitBCRec(&rec->rec);
	rec->abcRecVersion=kCurrentABCRecVersion;	// always kCurrentABCRecVersion
	rec->customBitMap=0L;
	rec->height=0;
	rec->width=0;
	rec->rowsTable=0L;
}

// use this instead of allocating a gworld and then passing to MakeABC rec
// this allocates the memory for an 8 bit pixmap and sets it all to zero. Each scanline is aligned
// to an 8 byte boundry so you can blit doubles if you are careful.
/*Boolean AllocateABCRec(short width,short height,ABCPtr theRec)
{
	Ptr			bitMap,baseAddr;
	Size		pixmapSize;
	
	// first round the width UP to be a multiple of 8
	width=width+(8-width&7);
	
	// work out the size of the pixmap, adding 7 to allow us to make the first scanline (thus all
	// the others as width/rowbytes is a mulitple of 8)
	pixmapSize=width*height+7;
	
	bitMap=NewPtrClear(pixmapSize);
	if (!bitMap)
		return false;
	
	// round base addr up to a multiple of 8
	baseAddr=bitMap+(8-((unsigned long)bitMap)&7);
	
	theRec->rec.world=0L;
	theRec->rec.pixMap=0L;
	theRec->rec.baseAddr=(unsigned char*)baseAddr;
	theRec->rec.rowBytes=width;
	theRec->customBitMap=bitMap;
	theRec->width=width;
	theRec->height=height;
	
	return BCToABCRec(theRec);
}*/

Boolean BCToABCRec(ABCPtr theRec)
{
	short			scanLines;
	short			counter;
	unsigned char	*addr;

	theRec->abcRecVersion=kCurrentABCRecVersion;
	theRec->customBitMap=0L;

	// if we have a gworld/window ptr, we can use the port bounds rather than the pixmap
	// bounds. the port bounds are more reliable under os x, as windows port bounds != pixmap
	// bounds, and pixmap/port orgins are not aligned.
	BlastAssert_(theRec->rec.world);
	
	if (theRec->rec.world)
	{
		Rect		r;
		GetPortBounds(theRec->rec.world,&r);
		theRec->width=FRectWidth(r);
		scanLines=theRec->height=FRectHeight(r);
	}
	else
	{
		theRec->width=FRectWidth((*theRec->rec.pixMap)->bounds);
		scanLines=theRec->height=FRectHeight((*theRec->rec.pixMap)->bounds);
	}
	
	// allocate memory for a table and fill it
	if (!(theRec->rowsTable=(unsigned char**)NewPtr(sizeof(unsigned char*)*scanLines)))
		return false;
	
	addr=theRec->rec.baseAddr;
	
	for (counter=0; counter<scanLines; counter++)
	{
		theRec->rowsTable[counter]=addr;
		addr+=theRec->rec.rowBytes;
	}
	
	return true;
}

// call this whenever you want to dispose of an abc rec.
void DestroyABCRec(ABCPtr theRec)
{
	if (theRec->customBitMap)
	{
		DisposePtr(theRec->customBitMap);
		theRec->customBitMap=0L;
	}
	if (theRec->rowsTable)
	{
		DisposePtr((Ptr)theRec->rowsTable);
		theRec->rowsTable=0L;
	}
}

// used to make an advanced bcrec
Boolean MakeABCRecFromGWorld(GWorldPtr theWorld,ABCPtr theRec)
{
	InitABCRec(theRec);
	MakeBCRecFromGWorld(theWorld,(BCPtr)theRec);
	
	#if TARGET_API_MAC_CARBON
	Rect	rect;
	GetPortBounds(theWorld,&rect);
	
	theRec->width=FRectWidth(rect);
	theRec->height=FRectHeight(rect);
	#else
	theRec->width=FRectWidth(theWorld->portRect);
	theRec->height=FRectHeight(theWorld->portRect);
	#endif
	
	return BCToABCRec(theRec);	
}

// use this to make a blast copy rect which you will be using for direct screen access.
// The window you pass must be opened on the device passed in the deviceH.
// This is usually gPlayDevice, so just pass that. Basically it is the device which was active
// when the window was opened.
// **** NOTE: Under mac os x, you will probably want to call LockPortBits before making a bcrec
// from the window. This ensures the BCRec represents the window's back buffer. If you don't then
// it represents the window's screen surface instead.
void MakeBCRecFromWindow(CWindowPtr theWindow,BCPtr theRec,GDHandle deviceH)
{
	#if TARGET_API_MAC_CARBON
	MakeBCRecFromGWorld(GetWindowPort(theWindow),theRec);
	#else
	MakeBCRecFromGWorld(theWindow,theRec);
	#endif
	
	// take into account where the devices bit map is and use that to adjust the bitmap pointer
	// so our data hits our window
	theRec->baseAddr-=(**deviceH).gdRect.left;
	theRec->baseAddr-=(**deviceH).gdRect.top*theRec->rowBytes;
//	UnlockPortBits(GetWindowPort(theWindow));
}

// use this to make a BCRec for blast copy from a GWorldPtr
void MakeBCRecFromGWorld(GWorldPtr theWorld,BCPtr theRec)
{
	MakeBCRecFromPM(GetGWorldPixMap(theWorld),theRec);

	theRec->world=theWorld;
}

// use this to make a BCRec for blast copy from a pixmap handle
void MakeBCRecFromPM(PixMapHandle thePixMap,BCPtr theRec)
{
	theRec->bcRecVersion=kCurrentBCRecVersion;
	theRec->world=0L;
	theRec->pixMap=thePixMap;
	HLockHi((Handle)theRec->pixMap);
	LockPixels(theRec->pixMap);
	theRec->baseAddr=(unsigned char *)GetPixBaseAddr(theRec->pixMap);

//	theRec->rowBytes=(**theRec->pixMap).rowBytes&0x3fff;
	theRec->rowBytes=GetPixRowBytes(theRec->pixMap);
	
	theRec->baseAddr-=theRec->rowBytes*(**theRec->pixMap).bounds.top;
	theRec->baseAddr-=(**theRec->pixMap).bounds.left*((**theRec->pixMap).pixelSize/8);
}

// This varient of BlastCopy gets an 8 bit source and expands it to a 16 bit source using the passed blast colour table.
void BlastCopy8To16(BCPtr source,BCPtr dest,Rect *sourceRect,Rect *destRect,BlastColourTable16 blastColourTable)
{
	unsigned char	*srcPtr,*destPtr,*srcRowStart,*destRowStart;
	unsigned long	miscCount;
	unsigned long	yCount;
	unsigned long	heightToCopy=sourceRect->bottom-sourceRect->top;
	unsigned long	widthToCopy=sourceRect->right-sourceRect->left;
	unsigned long	srcRB,destRB;
	
	srcRB=source->rowBytes;
	destRB=dest->rowBytes;
	srcRowStart=source->baseAddr+srcRB*sourceRect->top+sourceRect->left*sizeof(unsigned char);	// 8 bit source
	destRowStart=dest->baseAddr+destRB*destRect->top+destRect->left*sizeof(unsigned short);		// 16 bit dest
	
	for (yCount=0; yCount<heightToCopy; yCount++)
	{
		srcPtr=srcRowStart;
		srcRowStart+=srcRB;
		destPtr=destRowStart;
		destRowStart+=destRB;
		
		miscCount=widthToCopy;
		
		while(miscCount>=sizeof(unsigned char))
		{
			*((unsigned short *)destPtr)++=blastColourTable[*((unsigned char *)srcPtr)++];
			miscCount-=sizeof(unsigned char);				
		}
	}
}

// This proc translates a MacOS colour table, into a BlastColourTable. It does not allocate the memory for the blast colour
// table.
void ClutToBlastColourTable16(CTabHandle ctab,BlastColourTable16 blastColourTable)
{
	short		counter;

	for (counter=0; counter<=(**ctab).ctSize; counter++)
		blastColourTable[counter]=RGBColourToBlastColour16(&(AccessColour(ctab,counter)));
	
	for (; counter<=255; counter++)
		blastColourTable[counter]=0L;
}

// Converts an RGBColour into a BlastColour16
BlastColour16 RGBColourToBlastColour16(
	const RGBColour *rgb)
{
	unsigned short		tempCol;

	// shift down by 11 to get in the range 0-31
	// then shift up to the position and or it into the colour

	tempCol=(rgb->red>>11)<<kRedShift16;
	tempCol|=(rgb->green>>11)<<kGreenShift16;
	tempCol|=(rgb->blue>>11)<<kBlueShift16;

	return tempCol;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BlastColour16ToRGBColour
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Converts a BlastColour16 into an RGBColour
// Note on accuracy: This does a bit manipulation to change components from being in the range of 0-31 to 0-65535. The lower
// 11 bits of the RGBColour are always left empty by this conversion meaning the highest attainable number is 63488.
// To fill in these bits as well use BlastColour16ToRGBColourAccurate
void BlastColour16ToRGBColour(BlastColour16 colour,RGBColour *rgb)
{
	rgb->red=((colour>>kRedShift16)&31)<<11;
	rgb->green=((colour>>kGreenShift16)&31)<<11;
	rgb->blue=((colour>>kBlueShift16)&31)<<11;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BlastColour16ToRGBColourAccurate
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Attempts to fill in the 11 empty buts during the conversion. Can convert 31 -> 65535 so your whites stay white for longer.
void BlastColour16ToRGBColourAccurate(
	BlastColour16	inColour,
	RGBColour		*outColour)
{
	outColour->red=(((UInt32)((inColour>>kRedShift16)&31)) * 65535)/31;
	outColour->green=(((UInt32)((inColour>>kGreenShift16)&31)) * 65535)/31;
	outColour->blue=(((UInt32)((inColour>>kBlueShift16)&31)) * 65535)/31;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BlastColour24ToRGBColour
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void BlastColour24ToRGBColour(BlastColour24 colour,RGBColour *rgb)
{
	rgb->red=((colour >> 16) & 0xFF)*256;
	rgb->green=((colour >> 8) & 0xFF)*256;
	rgb->blue=((colour >> 0) & 0xFF)*256;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RGBColourToBlastColour24
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
BlastColour24 RGBColourToBlastColour24(
	const RGBColour *rgb)
{
	return ((rgb->red/256) << 16) | ((rgb->green/256) << 8 | (rgb->blue/256));
}