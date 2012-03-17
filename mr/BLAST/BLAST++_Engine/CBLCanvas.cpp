// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLCanvas.cpp
// © Mark Tully 2000
// 5/1/00
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

/*
	A class which represents a canvas which can be drawn on
	
	Canvases use an ABCRec to draw to. This must hold a CGrafPort which quickdraw can use
	(eg a window or gworld). The draw buffer is optional, if set whenever the canvas is invalled the request is passed onto
	the buffer allowing it to be blitted on the next blit call.
	
	The draw buffer is NOT owned by this object.
*/

#include	"Blast_Debug.h"
#include	"CBLCanvas.h"
#include	"Utility Objects.h"
#include	"Blast Draw buffer base.h"
#include	"CBLCanvas16.h"
#include	"CBLMatrix.h"
#include	"CCString.h"
//#include	"Drawing_Statements.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Default constructor, use SetGWorld to specify the gworld later
CBLCanvas::CBLCanvas() :
	mWorld(0),
	mOwnsWorld(true),
	mBuffer(0),
	mBackColour(TColourPresets::kBlack),
	mForeColour(TColourPresets::kWhite),
	mInvalCanvasAsDraw(false),
	mIsWindow(false)
{
	::InitABCRec(&mABCRec);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Can either take ownership of the gworld or not. Doens't dispose of the gworld on fail of construction.
CBLCanvas::CBLCanvas(
	GWorldPtr		inWorld,
	bool			inAdopt) :
	mWorld(inWorld),
	mOwnsWorld(inAdopt),
	mBuffer(0),
	mBackColour(TColourPresets::kBlack),
	mForeColour(TColourPresets::kWhite),
	mInvalCanvasAsDraw(false),
	mIsWindow(false)
{
	ThrowIfNil_(inWorld);

	if (!MakeABCRecFromGWorld(inWorld,&mABCRec))
		Throw_(memFullErr);
		
	#if TARGET_API_MAC_CARBON
	::GetPortBounds(inWorld,&mBoundsRect);
	#else
	mBoundsRect=inWorld->portRect;
	#endif
	
	mWidth=FRectWidth(mBoundsRect);
	mHeight=FRectHeight(mBoundsRect);
	
	ZeroRectCorner(mBoundsRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Can either take ownership of the window or not. Doens't dispose of the window on fail of construction.
CBLCanvas::CBLCanvas(
	WindowPtr		inWindow,
	bool			inAdopt) :
	mWorld(GetWindowPort(inWindow)),
	mOwnsWorld(inAdopt),
	mBuffer(0),
	mBackColour(TColourPresets::kBlack),
	mForeColour(TColourPresets::kWhite),
	mInvalCanvasAsDraw(false),
	mIsWindow(true)
{
	ThrowIfNil_(inWindow);
	MakeBCRecFromWindow(inWindow,&mABCRec.rec,::GetMainDevice());
	if (!BCToABCRec(&mABCRec))
		Throw_(memFullErr);	
		
	#if TARGET_API_MAC_CARBON
	::GetPortBounds(mWorld,&mBoundsRect);
	#else
	mBoundsRect=inWorld->portRect;
	#endif
	
	mWidth=FRectWidth(mBoundsRect);
	mHeight=FRectHeight(mBoundsRect);
	
	ZeroRectCorner(mBoundsRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Deletes the world if it owns it
CBLCanvas::~CBLCanvas()
{
	if (mWorld && mOwnsWorld)
	{
		if (mIsWindow)
		{
			WindowPtr		window=GetWindowFromPort(mWorld);

			BlastAssert_(IsValidWindowPtr(window));
			::DisposeWindow(window);
		}
		else
			::DisposeGWorld(mWorld);
	}
	::DestroyABCRec(&mABCRec);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetGWorld												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Specifies a new gworld for this canvas
void CBLCanvas::SetGWorld(
	GWorldPtr		inWorld,
	bool			inAdopt)
{
	if (mWorld && mOwnsWorld)
		::DisposeGWorld(mWorld);
	mWorld=inWorld;
	mOwnsWorld=inAdopt;
	mIsWindow=false;
	::DestroyABCRec(&mABCRec);
	if (!MakeABCRecFromGWorld(inWorld,&mABCRec))
		Throw_(memFullErr);
	
	#if TARGET_API_MAC_CARBON
	::GetPortBounds(inWorld,&mBoundsRect);
	#else
	mBoundsRect=inWorld->portRect;
	#endif
	
	ZeroRectCorner(mBoundsRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawPict
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws a pict into the world
void CBLCanvas::DrawPict(
	PicHandle		inHandle,
	const Rect		&inRect)
{
	UWorldSaver		safe(mWorld);

	::DrawPicture(inHandle,&inRect);
	InvalCanvasRect(inRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EraseAll
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Paints the entire canvas with the back colour
void CBLCanvas::EraseAll()
{
	UWorldSaver			safe(mWorld);
	Rect				rect;
	
	#if TARGET_API_MAC_CARBON
	::GetPortBounds(mWorld,&rect);
	#else
	rect=mWorld->portRect;
	#endif

	::RGBBackColour(&mBackColour);
	
	ZeroRectCorner(rect);

	::EraseRect(&rect);
	
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(rect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PaintAll
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Paints the entire canvas with the fore colour
void CBLCanvas::PaintAll()
{
	UWorldSaver			safe(mWorld);
	Rect				rect;
	
	#if TARGET_API_MAC_CARBON
	::GetPortBounds(mWorld,&rect);
	#else
	rect=mWorld->portRect;
	#endif
	
	::RGBForeColour(&mForeColour);
	
	ZeroRectCorner(rect);

	#if TARGET_API_MAC_CARBON
	Pattern		pat;
	::GetQDGlobalsBlack(&pat);
	::FillRect(&rect,&pat);
	#else
	::FillRect(&rect,&qd.black);
	#endif
	
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(rect);
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InvalCanvasRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// If a draw buffer is set it records the rectange as invalid meaning it will be blitted next call to the drawbuffer. This
// function is called by the canvas drawing procs if InvalCanvasAsDraw() is true.
void CBLCanvas::InvalCanvasRect(
	const Rect		&inRect)
{
	if (mBuffer)
	{
		Rect	rect=inRect;	// the rect is returned with the actual clipped rect
		mBuffer->InvalRectWithClip(rect);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InvalCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// If a draw buffer is set it records the whole canvas as invalid meaning it will be blitted next call to the drawbuffer.
void CBLCanvas::InvalCanvas()
{
	if (mBuffer)
	{
		Rect	rect={0,0,GetHeight(),GetWidth()};
		
		mBuffer->InvalRectWithClip(rect);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPixel16
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to read outside of bounds
TBLColour16 CBLCanvas::GetPixel16(
	SInt16			inX,
	SInt16			inY)
{
	RGBColour		colour;
	
	GetPixelRGB(inX,inY,colour);
	return RGBColourToBlastColour16(&colour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPixelRGB
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to read outside of bounds
void CBLCanvas::GetPixelRGB(
	SInt16			inX,
	SInt16			inY,
	RGBColour		&outColour)
{
	if (inX<0 || inY<0 || inX>=GetWidth() || inY>=GetHeight())
		return;
		
	UWorldSaver		safe(mWorld);

	::GetCPixel(inX,inY,&outColour);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetPixelRGB
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to set outside of bounds
void CBLCanvas::SetPixelRGB(
	SInt16			inX,
	SInt16			inY,
	const RGBColour	&inColour)
{
	if (inX<0 || inY<0 || inX>=GetWidth() || inY>=GetHeight())
		return;

	UWorldSaver		safe(mWorld);
	
	::SetCPixel(inX,inY,&inColour);
}
									
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetPixel16
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to read outside of bounds
void CBLCanvas::SetPixel16(
	SInt16			inX,
	SInt16			inY,
	TBLColour16		inCol)
{
	RGBColour		colour;

	BlastColour16ToRGBColour(inCol,&colour);
	
	SetPixelRGB(inX,inY,colour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ NewCanvas															/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Creates a canvas based on the gworld passed. By examining the depth it can create a canvas accelerated for that depth.
CBLCanvas *CBLCanvas::NewCanvas(
	GWorldPtr		inWorld,
	bool			inAdopt)
{
	CBLCanvas			*canvas;

	ThrowIfNil_(inWorld);
	
	switch ((**::GetGWorldPixMap(inWorld)).pixelSize)
	{
		case 16:
			canvas=new CBLCanvas16(inWorld,inAdopt);
			break;
		
		default:
			canvas=new CBLCanvas(inWorld,inAdopt);
			break;
	}
	
	return canvas;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ NewCanvasWithPicture												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Creates a new canvas with the picture handle specified.
CBLCanvas *CBLCanvas::NewCanvasWithPicture(
	PicHandle		inPic)
{
	GWorldPtr		gworld;
	CBLCanvas		*canvas;
	
	ThrowIfNil_(inPic);
	ThrowIfOSErr_(::NewGWorldWithPicHandle(&gworld,inPic,0));

	Try_
	{
		canvas=NewCanvas(gworld,true);
	}
	Catch_(err)
	{
		::DisposeGWorld(gworld);
		throw;
	}
	
	return canvas;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ApplyMatrix
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Applies an arbitary image matrix to the rectangle specified.
void CBLCanvas::ApplyMatrix(
	const CBLMatrix	&inMatrix,				// the matrix
	CBLCanvas		&inSourceCanvas,		// the canvas containing the source image
	const Rect		&inSourceRect,			// the source rectangle
	SInt16			inDestX,				// x dest
	SInt16			inDestY)				// y dest
{
	SInt16			x,y,spreadLeft,spreadRight,spreadUp,spreadDown,xm,ym,sourceX,sourceY;
	RGBColour		workCol;
	SInt32			r,g,b,element,scale;

	// Calculate the # of pix in each direction we should take into account
	spreadRight=spreadLeft=(inMatrix.GetWidth()-1)/2;
	if ((inMatrix.GetWidth()&1)==0)
		spreadRight++;
	spreadUp=spreadDown=(inMatrix.GetHeight()-1)/2;
	if ((inMatrix.GetHeight()&1)==0)
		spreadDown++;
	
	// Calc dest rect
	Rect			destRect=inSourceRect;	

	// Indent the dest rect so that we don't go outside of it for our pixels
	// (should do proper clipping really so the effect can be applied right up to the edge of the rect)
	destRect.top+=spreadUp;
	destRect.bottom-=spreadDown;
	destRect.left+=spreadLeft;
	destRect.right-=spreadRight;
	inDestX+=spreadLeft;			// adjust dest to leep it all aligned
	inDestY+=spreadUp;

	inDestX-=destRect.left;
	inDestY-=destRect.top;
	FOffset(destRect,inDestX,inDestY);
				
	// For each pixel on the screen	
	for (y=destRect.top; y<destRect.bottom; y++)
	{
		for (x=destRect.left; x<destRect.right; x++)
		{
			r=g=b=0;						// clear the working colour
			sourceX=x-inDestX-spreadLeft;	// top left calc source coords
			sourceY=y-inDestY-spreadUp;
		
			for (ym=0; ym<inMatrix.GetHeight(); ym++)
			{			
				for (xm=0; xm<inMatrix.GetWidth(); xm++)
				{
					element=inMatrix.GetElement(xm,ym);
					
					if (element)
					{
						inSourceCanvas.GetPixelRGB(sourceX+xm,sourceY+ym,workCol);
						r+=((SInt32)workCol.red)*element;
						g+=((SInt32)workCol.green)*element;
						b+=((SInt32)workCol.blue)*element;
					}					
				}
			}

			r/=inMatrix.GetScale();
			g/=inMatrix.GetScale();
			b/=inMatrix.GetScale();
			
			workCol.red=Limit(r,0,0xFFFF);
			workCol.green=Limit(g,0,0xFFFF);
			workCol.blue=Limit(b,0,0xFFFF);
			
			SetPixelRGB(x,y,workCol);
		}
	}
	
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(destRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Copy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas::Copy(
	GWorldPtr		inSourceWorld,
	const Rect		&inCopyRect,
	SInt16			inDestX,
	SInt16			inDestY)
{
	// Quick check of the params for null copies...
	if (inDestX>=mABCRec.width || inDestY>=mABCRec.height)
		return;
	if (FRectWidth(inCopyRect)==0 || FRectHeight(inCopyRect)==0)
		return;
		
	// Calc the dest rect
	Rect		destRect=inCopyRect,srcRect=inCopyRect;
	
	inDestX-=destRect.left;
	inDestY-=destRect.top;
	FOffset(destRect,inDestX,inDestY);
		
	// And invoke the mighty CopyBits
	UWorldSaver	safe(mWorld);
	::RGBForeColour(&TColourPresets::kBlack);
	::RGBBackColour(&TColourPresets::kWhite);
	#if TARGET_API_MAC_CARBON
	::CopyBits((BitMap*)*(GetPortPixMap(inSourceWorld)),(BitMap*)*mABCRec.rec.pixMap,&srcRect,&destRect,srcCopy,0L);
	#else
	::CopyBits((BitMap*)*(inSourceWorld->portPixMap),(BitMap*)*mABCRec.rec.pixMap,&srcRect,&destRect,srcCopy,0L);
	#endif
	
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(destRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Copy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Does an generic image transfer from one canvas to another
// Assumes the source rect is all on the source canvas and is valid (left<=right && top<=bottom)
// Clips the other rect and dest
void CBLCanvas::Copy(
	CCanvas			*inSourceCanvas,
	const Rect		&inCopyRect,
	SInt16			inDestX,
	SInt16			inDestY)
{
	// Quick check of the params for null copies...
	if (inDestX>=mABCRec.width || inDestY>=mABCRec.height)
		return;
	if (FRectWidth(inCopyRect)==0 || FRectHeight(inCopyRect)==0)
		return;
		
	// Calc the dest rect
	Rect			destRect=inCopyRect;
	Rect			srcRect=inCopyRect;
	
	inDestX-=destRect.left;
	inDestY-=destRect.top;
	FOffset(destRect,inDestX,inDestY);
	
	// Clip?
	// We know the top left of the dest rect is on the canvas, but the bottom right?
	if (destRect.right>GetWidth())
	{
		srcRect.right-=destRect.right-GetWidth();
		destRect.right-=destRect.right-GetWidth();
	}
	if (destRect.bottom>GetHeight())
	{
		srcRect.bottom-=destRect.bottom-GetHeight();
		destRect.bottom-=destRect.bottom-GetHeight();
	}
		
	// And invoke the mighty CopyBits - or BlastCopy16
	if (GetDepth()==16 && ((CBLCanvas*)inSourceCanvas)->GetDepth()==16)
		::BlastCopy16(&((CBLCanvas*)inSourceCanvas)->mABCRec.rec,&mABCRec.rec,&srcRect,&destRect);
	else
	{
		UWorldSaver	safe(mWorld);
		::RGBForeColour(&TColourPresets::kBlack);
		::RGBBackColour(&TColourPresets::kWhite);
		::CopyBits((BitMap*)*((CBLCanvas*)inSourceCanvas)->mABCRec.rec.pixMap,(BitMap*)*mABCRec.rec.pixMap,&srcRect,&destRect,srcCopy,0L);
	}
	
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(destRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalcTextRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the rectangle occupied by a text string in the current settings
void CBLCanvas::CalcTextRect(
	SInt16			inX,
	SInt16			inY,
	const CCString	&inStr,
	Rect			&outRect)
{
	UWorldSaver		safe(mWorld);
	
	::TextFace(mTextFace);
	::TextFont(mFontId);
	::TextSize(mTextSize);

	SInt16			width=::TextWidth(inStr.mString,0,inStr.GetLength());
	FontInfo		fontInfo;
	
	::GetFontInfo(&fontInfo);		// could cache this?
	
	outRect.left=inX;
	outRect.right=outRect.left+width;
	outRect.top=inY-fontInfo.ascent;
	outRect.bottom=inY+fontInfo.descent;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawText
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws the text in the current face, font, size, col etc
void CBLCanvas::DrawText(
	SInt16			inX,
	SInt16			inY,
	const CCString	&inStr)
{
	UWorldSaver		safe(mWorld);
	
	::RGBForeColour(&mForeColour);
	::RGBBackColour(&mBackColour);
	::TextFace(mTextFace);
	::TextFont(mFontId);
	::TextSize(mTextSize);
	::MoveTo(inX,inY);
	::DrawText(inStr.mString,0,inStr.GetLength());
	
	// Inval the rect
	if (mInvalCanvasAsDraw)
	{
		Rect	rect;
		// figure the text rectangle
		CalcTextRect(inX,inY,inStr,rect);
		InvalCanvasRect(rect);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawLine
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws a line x across and y down
void CBLCanvas::DrawLine(
	SInt16			inX1,
	SInt16			inY1,
	SInt16			inX2,
	SInt16			inY2)
{
	UWorldSaver		safe(mWorld);

	::RGBForeColour(&mForeColour);
	::MoveTo(inX1,inY1);
	::LineTo(inX2,inY2);
	
	// Inval the rect
	if (mInvalCanvasAsDraw)
	{
		Rect	rect;
		
		rect.top=inY1;
		rect.left=inX1;
		rect.bottom=inY2;
		rect.right=inX2;
		
		// Validate the rect (swap left/right top/bottom so it's in order)
		ValidateRect(rect);
		
		// Extend the rect to capture right and lower pixels
		rect.bottom++;
		rect.right++;
		
		// Mark it for blitting
		InvalCanvasRect(rect);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws a filled in rectangle
void CBLCanvas::FillRect(
	const Rect		&inRect)
{
	UWorldSaver		safe(mWorld);
	
	::RGBForeColour(&mForeColour);
	::RGBBackColour(&mBackColour);
	
	#if TARGET_API_MAC_CARBON
	Pattern		pat;
	::GetQDGlobalsBlack(&pat);
	::FillRect(&inRect,&pat);
	#else
	::FillRect(&inRect,&qd.black);
	#endif
		
	// Inval the rect
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(inRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FrameRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws a hollow rectangle
void CBLCanvas::FrameRect(
	const Rect		&inRect)
{
	UWorldSaver		safe(mWorld);
	
	::RGBForeColour(&mForeColour);
	::RGBBackColour(&mBackColour);
		
	::FrameRect(&inRect);
		
	// Inval the rect
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(inRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillOval
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws a filled in oval
void CBLCanvas::FillOval(
	const Rect		&inRect)
{
	UWorldSaver		safe(mWorld);
	
	::RGBForeColour(&mForeColour);
	::RGBBackColour(&mBackColour);
	
	#if TARGET_API_MAC_CARBON
	Pattern		pat;
	::GetQDGlobalsBlack(&pat);
	::FillOval(&inRect,&pat);
	#else
	::FillOval(&inRect,&qd.black);
	#endif

	// Inval the rect
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(inRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FrameOval
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws a hollow oval
void CBLCanvas::FrameOval(
	const Rect		&inRect)
{
	UWorldSaver		safe(mWorld);
	
	::RGBForeColour(&mForeColour);
	::RGBBackColour(&mBackColour);
	
	::FrameOval(&inRect);
		
	// Inval the rect
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(inRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillPoly
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas::FillPoly(
	SInt32				*inArray,
	SInt32				inNumCoOrdPairs)
{
	UWorldSaver		safe(mWorld);
	
	::RGBForeColour(&mForeColour);
	::RGBBackColour(&mBackColour);
	::MoveTo(inArray[0],inArray[1]);

	if (PolyHandle poly=::OpenPoly())
	{
		for (SInt32 counter=2; counter<inNumCoOrdPairs*2; counter+=2)
			::LineTo(inArray[counter],inArray[counter+1]);
			
		::ClosePoly();
		
		#if TARGET_API_MAC_CARBON
		Pattern		pat;
		::GetQDGlobalsBlack(&pat);
		::FillPoly(poly,&pat);
		#else
		::FillPoly(poly,&qd.black);
		#endif
		
		// Inval the rect
		if (mInvalCanvasAsDraw)
		{
			Rect	rect=(*poly)->polyBBox;
		
			InvalCanvasRect(rect);
		}

		::KillPoly(poly);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FramePoly
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas::FramePoly(
	SInt32				*inArray,
	SInt32				inNumCoOrdPairs)
{
	UWorldSaver		safe(mWorld);
	
	::RGBForeColour(&mForeColour);
	::RGBBackColour(&mBackColour);
	::MoveTo(inArray[0],inArray[1]);
	
	if (PolyHandle poly=::OpenPoly())
	{
		for (SInt32 counter=2; counter<inNumCoOrdPairs*2; counter+=2)
			::LineTo(inArray[counter],inArray[counter+1]);
			
		::ClosePoly();
		::FramePoly(poly);
		
		// Inval the rect
		if (mInvalCanvasAsDraw)
		{
			Rect	rect=(*poly)->polyBBox;
			
			rect.bottom++;
			rect.right++;
			
			InvalCanvasRect(rect);
		}

		::KillPoly(poly);
	}
}
