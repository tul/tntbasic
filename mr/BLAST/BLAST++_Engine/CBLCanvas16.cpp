// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLCanvas16.cpp
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
	An accelerated canvas for 16-bit graphics
*/

#include		"TNT_Debugging.h"
#include		<cmath>
#include		"CBLCanvas16.h"
#include		"Utility Objects.h"
#include		"CBLMask16.h"
#include		"BLCalcTransp16.h"

const float		kPi=3.141592654;

void CheckGWorld(GWorldPtr inWorld)
{
	// we should always use native endian pixmaps 
	PixMapHandle		pm=::GetGWorldPixMap(inWorld);
	OSType				pf=(**pm).pixelFormat;
	if (pf!=k16LE555PixelFormat)
	{
		printf("bad pixmap endian\n");		// NO COMMIT
	}
	AssertThrow_(pf==k16LE555PixelFormat);
}

/*e*/
CBLCanvas16::CBLCanvas16(
	WindowPtr		inWindow,			// window's base addrs are calculated differently to gworlds so we need 2 constructors
	bool			inAdopt) :
	CBLCanvas(inWindow,inAdopt)
{
	CheckGWorld(GetWindowPort(inWindow));
}

/*e*/
CBLCanvas16::CBLCanvas16(
	GWorldPtr		inWorld,
	bool			inAdopt) :
	CBLCanvas(inWorld,inAdopt)
{
	CheckGWorld(inWorld);
}

void CBLCanvas16::SetGWorld(
	GWorldPtr		inWorld,
	bool			inAdopt)
{
	inherited::SetGWorld(inWorld,inAdopt);
	CheckGWorld(inWorld);
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This variation of the constructor allocates the gworld directly
CBLCanvas16::CBLCanvas16(
	SInt16			inWidth,
	SInt16			inHeight,
	GDHandle		inDevice)
{
	GWorldPtr		world;
	
	FSetRect(mBoundsRect,0,0,inHeight,inWidth);

	ThrowIfOSErr_(::NewGWorld(&world,16,&mBoundsRect,0L,inDevice,kNativeEndianPixMap | (inDevice ? noNewDevice : 0)));
	
	SetGWorld(world,true);
	
	mWidth=inWidth;
	mHeight=inHeight;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPixel16
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to read outside of bounds
// Accelerated via direct access, results if x,y are off canvas is 0
TBLColour16 CBLCanvas16::GetPixel16(
	SInt16			inX,
	SInt16			inY)
{
	if (inX<0 || inY<0 || inX>=GetWidth() || inY>=GetHeight())
		return 0;
	return ((TBLColour16*)mABCRec.rowsTable[inY])[inX];
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetPixel16
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Accelerated via direct access, clips to bounds
// Note : The reason there isn't a non clip version is because any speed increase you get from missing out 1 IF statement is
// going to be cancelled out by the fact that you've just dereferenced a couple of vtables. If you want it to be fast I'm
// afraid you'll just have to operate on the ABCRec itself.
void CBLCanvas16::SetPixel16(
	SInt16			inX,
	SInt16			inY,
	TBLColour16		inCol)
{
	if (inX<mABCRec.width && inY<mABCRec.height && inY>=0 && inX>=0)
	{
		if (mOpacity==1)
			((TBLColour16*)mABCRec.rowsTable[inY])[inX]=inCol;
		else
		{
			RGBColour		destRGB,sourceRGB;
			
			BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[inY])[inX],&destRGB);
			BlastColour16ToRGBColour(inCol,&sourceRGB);
		
			destRGB.red=sourceRGB.red*mOpacity+destRGB.red*(1-mOpacity);
			destRGB.green=sourceRGB.green*mOpacity+destRGB.green*(1-mOpacity);
			destRGB.blue=sourceRGB.blue*mOpacity+destRGB.blue*(1-mOpacity);

			((TBLColour16*)mABCRec.rowsTable[inY])[inX]=RGBColourToBlastColour16(&destRGB);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPixelRGB
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to read outside of bounds
void CBLCanvas16::GetPixelRGB(
	SInt16			inX,
	SInt16			inY,
	RGBColour		&outColour)
{
	if (inX<0 || inY<0 || inX>=GetWidth() || inY>=GetHeight())
		return;

	BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[inY])[inX],&outColour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetPixelRGB
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to set outside of bounds
void CBLCanvas16::SetPixelRGB(
	SInt16			inX,
	SInt16			inY,
	const RGBColour	&inColour)
{
	SetPixel16(inX,inY,RGBColourToBlastColour16(&inColour));
}
									
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::FillRect(
	const Rect		&inTempRect)
{
	if (mOpacity==1)
	{
		UWorldSaver		safe(mWorld);
		::RGBForeColor(&mForeColour);
		
		#if TARGET_API_MAC_CARBON
		Pattern		pat;
		::GetQDGlobalsBlack(&pat);
		::FillRect(&inTempRect,&pat);
		#else
		::FillRect(&inTempRect,&qd.black);
		#endif
	}
	else if (mOpacity>0)
	{
		Rect	inRect;
	
		// Clip the rect
		inRect.top=Greater(inTempRect.top,0);
		inRect.left=Greater(inTempRect.left,0);
		inRect.bottom=Lesser(inTempRect.bottom,GetHeight());
		inRect.right=Lesser(inTempRect.right,GetWidth());
	
		RGBColour		destRGB,sourceRGB;

		sourceRGB.red=mForeColour.red*mOpacity;
		sourceRGB.green=mForeColour.green*mOpacity;
		sourceRGB.blue=mForeColour.blue*mOpacity;
		
		for (SInt16 y=inRect.top; y<inRect.bottom; y++)
		{
			for (SInt16 x=inRect.left; x<inRect.right; x++)
			{
				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);

				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
			
				((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
			}	
		}
	}
	
	// Inval the rect
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(inTempRect);
}
									
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FrameRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::FrameRect(
	const Rect		&inRect)
{
	if (mOpacity==1)
	{
		UWorldSaver		safe(mWorld);
		::RGBForeColor(&mForeColour);
		::FrameRect(&inRect);
	}
	else if (mOpacity>0)
	{
		RGBColour			destRGB,sourceRGB;
		
		sourceRGB.red=mForeColour.red*mOpacity;
		sourceRGB.green=mForeColour.green*mOpacity;
		sourceRGB.blue=mForeColour.blue*mOpacity;
		
		// Draw the top and bottom lines
		for (SInt16 x=inRect.left; x<inRect.right; x++)
		{
			if (inRect.top>=0)
			{
				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[inRect.top])[x],&destRGB);

				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
			
				((TBLColour16*)mABCRec.rowsTable[inRect.top])[x]=RGBColourToBlastColour16(&destRGB);
			}
			
			if (inRect.bottom-1<GetHeight())
			{
				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[inRect.bottom-1])[x],&destRGB);

				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
			
				((TBLColour16*)mABCRec.rowsTable[inRect.bottom-1])[x]=RGBColourToBlastColour16(&destRGB);
			}
		}		
		
		// Draw the middle part
		Rect	tempRect=inRect;
		
		tempRect.top=Greater(tempRect.top+1,0);
		tempRect.bottom=Lesser(tempRect.bottom-1,GetHeight());
		
		for (SInt16 y=tempRect.top; y<tempRect.bottom; y++)
		{
			if (tempRect.left>=0)
			{
				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[tempRect.left],&destRGB);
				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				((TBLColour16*)mABCRec.rowsTable[y])[tempRect.left]=RGBColourToBlastColour16(&destRGB);
			}

			if (tempRect.right-1<GetWidth())
			{			
				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[tempRect.right-1],&destRGB);
				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				((TBLColour16*)mABCRec.rowsTable[y])[tempRect.right-1]=RGBColourToBlastColour16(&destRGB);
			}
		}
	}
	
	// Inval the rect
	if (mInvalCanvasAsDraw)
		InvalCanvasRect(inRect);
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawLine
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::DrawLine(
	SInt16			inX1,
	SInt16			inY1,
	SInt16			inX2,
	SInt16			inY2)
{
	if (mOpacity==1)
	{
		UWorldSaver		safe(mWorld);

		::RGBForeColour(&mForeColour);
		::MoveTo(inX1,inY1);
		::LineTo(inX2,inY2);
	}
	else
	{
		if (inX1<0 || inY1<0 || inX1>=GetWidth() || inY1>=GetHeight() ||
			inX2<0 || inY2<0 || inX2>=GetWidth() || inY2>=GetHeight())
		{
			Rect		rect={0,0,GetHeight(),GetWidth()};
		
			DrawLine16Clip(inX1,inY1,inX2,inY2,rect);
		}
		else
		{
			DrawLine16(inX1,inY1,inX2,inY2);
		}
	}
	
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
//		¥ DrawLine16
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::DrawLine16(
	SInt16			sourceX,
	SInt16			sourceY,
	SInt16			destX,
	SInt16 			destY)
{
	short 				dx,dy,d,incr1,incr2,x,y,xEnd,yEnd,temp;
	RGBColour			destRGB,sourceRGB;
	
	sourceRGB.red=mForeColour.red*mOpacity;
	sourceRGB.green=mForeColour.green*mOpacity;
	sourceRGB.blue=mForeColour.blue*mOpacity;
	
	dx=FAbs(destX-sourceX);
	dy=FAbs(destY-sourceY);
	
	if (dx>=dy)
	{
		d=2*dy-dx;
		incr1=2*dy;
		incr2=2*(dy-dx);

		// swap the coords around if necessary as this routine only works if sourceX<=destX
		if (sourceX>destX)
		{
			temp=sourceX;
			sourceX=destX;
			destX=temp;
			temp=sourceY;
			sourceY=destY;
			destY=temp;
		}
		
		x=sourceX;
		y=sourceY;
		xEnd=destX;
		
		BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
				
		destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
		destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
		destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
		
		((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
		
		if (destY>=sourceY)
		{
			while (x<xEnd)
			{
				x++;
				
				if (d<0)
					d+=incr1;
				else
				{
					d+=incr2;
					y++;
				}
				
				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
						
				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				
				((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
			}
		}
		else
		{
			while (x<xEnd)
			{
				x++;
				
				if (d<0)
					d+=incr1;
				else
				{
					d+=incr2;
					y--;
				}

				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
						
				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				
				((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
			}
		}
	}
	else
	{
		d=2*dx-dy;
		incr1=2*dx;
		incr2=2*(dx-dy);

		// swap the coords around if necessary as this routine only works if sourceY<=destY
		if (sourceY>destY)
		{
			temp=sourceX;
			sourceX=destX;
			destX=temp;
			temp=sourceY;
			sourceY=destY;
			destY=temp;
		}
		
		x=sourceX;
		y=sourceY;
		yEnd=destY;
		
		BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
				
		destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
		destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
		destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
		
		((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
		
		if (destX>=sourceX)
		{
			while (y<yEnd)
			{
				y++;
				
				if (d<0)
					d+=incr1;
				else
				{
					d+=incr2;
					x++;
				}

				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
				
				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				
				((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
			}
		}
		else
		{
			while (y<yEnd)
			{
				y++;
				
				if (d<0)
					d+=incr1;
				else
				{
					d+=incr2;
					x--;
				}
				
				BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
				
				destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
				destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
				destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				
				((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawLine16Clip
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::DrawLine16Clip(
	SInt16			sourceX,
	SInt16			sourceY,
	SInt16			destX,
	SInt16 			destY,
	const Rect		&inRect)
{
	short 				dx,dy,d,incr1,incr2,x,y,xEnd,yEnd,temp;
	RGBColour			destRGB,sourceRGB;
	
	sourceRGB.red=mForeColour.red*mOpacity;
	sourceRGB.green=mForeColour.green*mOpacity;
	sourceRGB.blue=mForeColour.blue*mOpacity;
	
	dx=FAbs(destX-sourceX);
	dy=FAbs(destY-sourceY);
	
	if (dx>=dy)
	{
		d=2*dy-dx;
		incr1=2*dy;
		incr2=2*(dy-dx);

		// swap the coords around if necessary as this routine only works if sourceX<=destX
		if (sourceX>destX)
		{
			temp=sourceX;
			sourceX=destX;
			destX=temp;
			temp=sourceY;
			sourceY=destY;
			destY=temp;
		}
		
		x=sourceX;
		y=sourceY;
		xEnd=destX;
				
		if (InRect(inRect,x,y))
		{
			BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
				
			destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
			destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
			destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				
			((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
		}
		
		if (destY>=sourceY)
		{
			while (x<xEnd)
			{
				x++;
				
				if (d<0)
					d+=incr1;
				else
				{
					y++;
					d+=incr2;
				}
				
				if (InRect(inRect,x,y))
				{
					BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
					
					destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
					destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
					destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
					
					((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
				}
			}
		}
		else
		{
			while (x<xEnd)
			{
				x++;
				
				if (d<0)
					d+=incr1;
				else
				{
					y--;
					d+=incr2;
				}

				if (InRect(inRect,x,y))
				{
					BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
					
					destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
					destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
					destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
					
					((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
				}
			}
		}
	}
	else
	{
		d=2*dx-dy;
		incr1=2*dx;
		incr2=2*(dx-dy);

		// swap the coords around if necessary as this routine only works if sourceY<=destY
		if (sourceY>destY)
		{
			temp=sourceX;
			sourceX=destX;
			destX=temp;
			temp=sourceY;
			sourceY=destY;
			destY=temp;
		}
		
		x=sourceX;
		y=sourceY;
		yEnd=destY;

		if (InRect(inRect,x,y))
		{		
			BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
				
			destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
			destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
			destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
				
			((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
		}
		
		if (destX>=sourceX)
		{
			while (y<yEnd)
			{
				y++;
				
				if (d<0)
					d+=incr1;
				else
				{
					x++;
					d+=incr2;
				}

				if (InRect(inRect,x,y))
				{
					BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
					
					destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
					destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
					destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
					
					((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
				}
			}
		}
		else
		{
			while (y<yEnd)
			{
				y++;
				
				if (d<0)
					d+=incr1;
				else
				{
					x--;
					d+=incr2;
				}
				
				if (InRect(inRect,x,y))
				{
					BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[x],&destRGB);
					
					destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
					destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
					destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
					
					((TBLColour16*)mABCRec.rowsTable[y])[x]=RGBColourToBlastColour16(&destRGB);
				}
			}
		}
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillPoly
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::FillPoly(
	SInt32				*inArray,
	SInt32				inNumCoOrdPairs)
{
	if (mOpacity==1)
		CBLCanvas::FillPoly(inArray,inNumCoOrdPairs);
	else
	{	
		UWorldSaver		safe(mWorld);
		RGBColour		destRGB,sourceRGB;
	
		sourceRGB.red=mForeColour.red*mOpacity;
		sourceRGB.green=mForeColour.green*mOpacity;
		sourceRGB.blue=mForeColour.blue*mOpacity;

		if (RgnHandle poly=::NewRgn())
		{
			::MoveTo(inArray[0],inArray[1]);
			
			::OpenRgn();
		
			for (SInt32 counter=2; counter<inNumCoOrdPairs*2; counter+=2)
				::LineTo(inArray[counter],inArray[counter+1]);
			
			::LineTo(inArray[0],inArray[1]);
			
			::CloseRgn(poly);
			
			#if TARGET_API_MAC_CARBON
			Rect	rect;
			GetRegionBounds(poly,&rect);
			#else
			Rect	rect=(*poly)->rgnBBox;
			#endif
			
			// Clip the rect
			rect.top=Greater(rect.top,0);
			rect.left=Greater(rect.left,0);
			rect.bottom=Lesser(rect.bottom,GetHeight());
			rect.right=Lesser(rect.right,GetWidth());
			
			// Fill the polygon
			Point	point;
			
			for (point.v=rect.top; point.v<rect.bottom; point.v++)
			{
				for (point.h=rect.left; point.h<rect.right; point.h++)
				{
					if (::PtInRgn(point,poly))
					{
						BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[point.v])[point.h],&destRGB);
				
						destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
						destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
						destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
						
						((TBLColour16*)mABCRec.rowsTable[point.v])[point.h]=RGBColourToBlastColour16(&destRGB);
					}
				}
			}
			
			// Inval the rect
			if (mInvalCanvasAsDraw)
				InvalCanvasRect(rect);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FramePoly
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::FramePoly(
	SInt32				*inArray,
	SInt32				inNumCoOrdPairs)
{
	if (mOpacity==1)
		CBLCanvas::FramePoly(inArray,inNumCoOrdPairs);
	else
	{	
		for (SInt32 count=0; count<(inNumCoOrdPairs-1)*2; count+=2)
			DrawLine(inArray[count],inArray[count+1],inArray[count+2],inArray[count+3]);
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillOval
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::FillOval(
	const Rect		&inRect)
{
	if (mOpacity==1)
		CBLCanvas::FillOval(inRect);
	else
	{	
		long			centreX=(inRect.left+inRect.right)/2,centreY=(inRect.top+inRect.bottom)/2;
		long			height=FRectHeight(inRect),width=FRectWidth(inRect);
		long			sqHeight=(height/2)*(height/2),sqWidth=(width/2)*(width/2);
		RGBColour		sourceRGB;
	
		sourceRGB.red=mForeColour.red*mOpacity;
		sourceRGB.green=mForeColour.green*mOpacity;
		sourceRGB.blue=mForeColour.blue*mOpacity;
	
		for (long y=inRect.top; y<inRect.bottom; y++)
		{
			if ((y>=0) && (y<mABCRec.height))
			{				
				float	tempY=(centreY-y)-0.5;
				float	x=std::sqrt((float)sqWidth*(1-((float)(tempY*tempY)/(float)sqHeight)));	
				float	startX=centreX-x,endX=centreX+x;
				
				if (startX-0.5>(long)startX)
					startX++;
				
				if (endX-0.5>(long)endX)
					endX++;

				FillRun(y,startX,endX,sourceRGB);
			}
		}
		
		if (mInvalCanvasAsDraw)
			InvalCanvasRect(inRect);
	}
}
									
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FrameOval
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::FrameOval(
	const Rect		&inRect)
{
	if (mOpacity==1)
		CBLCanvas::FrameOval(inRect);
	else
	{	
		long			centreX=(inRect.left+inRect.right)/2,centreY=(inRect.top+inRect.bottom)/2;
		long			height=FRectHeight(inRect),width=FRectWidth(inRect);
		long			sqHeight=(height/2)*(height/2),sqWidth=(width/2)*(width/2);
		RGBColour		destRGB,sourceRGB;
		SInt32			clipWidth=GetWidth(),clipHeight=GetHeight();
	
		sourceRGB.red=mForeColour.red*mOpacity;
		sourceRGB.green=mForeColour.green*mOpacity;
		sourceRGB.blue=mForeColour.blue*mOpacity;
	
		for (long y=inRect.top; y<inRect.bottom; y++)
		{
			if ((y>=0) && (y<mABCRec.height))
			{				
				float	tempY=(centreY-y)-0.5;
				float	x=std::sqrt((float)sqWidth*(1-((float)(tempY*tempY)/(float)sqHeight)));	
				float	startX=centreX-x,endX=centreX+x;
				
				if (startX-0.5>(long)startX)
					startX++;
				
				if (endX-0.5>(long)endX)
					endX++;
					
				if (startX>=0 && startX<clipWidth)
				{
					// Write the left pixel			
					BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[(long)startX],&destRGB);
					destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
					destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
					destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
					((TBLColour16*)mABCRec.rowsTable[y])[(long)startX]=RGBColourToBlastColour16(&destRGB);
				}

				if (endX>0 && endX<=clipWidth)
				{
					// Write the right pixel			
					BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[y])[(long)endX-1],&destRGB);
					destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
					destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
					destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
					((TBLColour16*)mABCRec.rowsTable[y])[(long)endX-1]=RGBColourToBlastColour16(&destRGB);
				}
			}
		}
		
		for (long x=inRect.left; x<=centreX; x++)
		{
			long	lastY;
			
			if ((x>=0) && (x<mABCRec.width))
			{				
				float	tempX=(centreX-x)-0.5;
				float	y=std::sqrt((float)sqHeight*(1-((float)(tempX*tempX)/(float)sqWidth)));	
				float	startY=centreY-y,endY=centreY+y;
				
				if (startY-0.5>(long)startY)
					startY++;
				
				if (endY-0.5>(long)endY)
					endY++;

				if ((long)startY==lastY)
				{
					if (startY>=0 && startY<clipHeight)
					{
						// Write the top pixel
						BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[(long)startY])[x],&destRGB);
						destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
						destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
						destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
						((TBLColour16*)mABCRec.rowsTable[(long)startY])[x]=RGBColourToBlastColour16(&destRGB);
					}
				
					if (endY>0 && endY<=clipHeight)
					{
						// Write the bottom pixel			
						BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[(long)endY-1])[x],&destRGB);
						destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
						destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
						destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
						((TBLColour16*)mABCRec.rowsTable[(long)endY-1])[x]=RGBColourToBlastColour16(&destRGB);
					}
				}
				else
					lastY=startY;
			}
		}
		
		for (long x=inRect.right-1; x>centreX; x--)
		{
			long	lastY;
			
			if ((x>=0) && (x<mABCRec.width))
			{				
				float	tempX=(centreX-x)-0.5;
				float	y=std::sqrt((float)sqHeight*(1-((float)(tempX*tempX)/(float)sqWidth)));	
				float	startY=centreY-y,endY=centreY+y;
				
				if (startY-0.5>(long)startY)
					startY++;
				
				if (endY-0.5>(long)endY)
					endY++;

				if ((long)startY==lastY)
				{
					if (startY>=0 && startY<clipHeight)
					{
						// Write the top pixel
						BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[(long)startY])[x],&destRGB);
						destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
						destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
						destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
						((TBLColour16*)mABCRec.rowsTable[(long)startY])[x]=RGBColourToBlastColour16(&destRGB);
					}
				
					if (endY>0 && endY<=clipHeight)
					{
						// Write the bottom pixel			
						BlastColour16ToRGBColour(((TBLColour16*)mABCRec.rowsTable[(long)endY-1])[x],&destRGB);
						destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
						destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
						destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
						((TBLColour16*)mABCRec.rowsTable[(long)endY-1])[x]=RGBColourToBlastColour16(&destRGB);
					}
				}
				else
					lastY=startY;
			}
		}
		
		if (mInvalCanvasAsDraw)
			InvalCanvasRect(inRect);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillRun
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLCanvas16::FillRun(
	long				inY,
	long				x,
	long				endX,
	const RGBColour		&sourceRGB)
{
	RGBColour		destRGB;

	x=Greater(x,0);
	endX=Lesser(endX,mABCRec.width);
	
	TBLColour16		*ptr=((TBLColour16*)mABCRec.rowsTable[inY]);
	
	// Draw the run from start to finish
	while (x<endX)
	{
		BlastColour16ToRGBColour(ptr[x],&destRGB);
				
		destRGB.red=sourceRGB.red+destRGB.red*(1-mOpacity);
		destRGB.green=sourceRGB.green+destRGB.green*(1-mOpacity);
		destRGB.blue=sourceRGB.blue+destRGB.blue*(1-mOpacity);
		
		ptr[x]=RGBColourToBlastColour16(&destRGB);
	
		x++;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MaskCopy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Copies the source data onto this canvas using the mask
// Assumes inSourceRect is inside inSource.bounds, clips the dest so it fits on
void CBLCanvas16::MaskCopy(
	CBLCanvas16		&inSource,
	const Rect		&inSourceRect,
	CBLMask16		*inMask,
	SInt16			inDestX,
	SInt16			inDestY)
{
	SInt16			x,y;
	SInt16			destX,destY;
	SInt16			endX,endY;
	TBLColour16		col;
	Rect			srcRect=inSourceRect;
	
	destY=inDestY;

	endX=inDestX+FRectWidth(inSourceRect);
	endY=inDestY+FRectHeight(inSourceRect);
	
	// Clip?
	// We know the top left of the dest rect is on the canvas, but the bottom right?
	if (endX>GetWidth())
	{
		endX-=endX-GetWidth();
		srcRect.right-=endX-GetWidth();
	}

	if (endY>GetHeight())
	{
		endY-=endY-GetHeight();
		srcRect.bottom-=endX-GetWidth();
	}
	
	for (y=srcRect.top; y<srcRect.bottom; y++)
	{
		destX=inDestX;

		for (x=srcRect.left; x<srcRect.right; x++)
		{
			col=inSource.GetPixel16(x,y);
		
			switch (inMask->ApplyMask(col,x,y))
			{
				case CBLMask16::kTransp100:
					SetPixel16(destX,destY,GetPixel16(x,y));
					break;
					
				case CBLMask16::kTransp93:
					SetPixel16(destX,destY,BLCalcTransp616(GetPixel16(x,y),col));
					break;
					
				case CBLMask16::kTransp87:
					SetPixel16(destX,destY,BLCalcTransp1216(GetPixel16(x,y),col));
					break;
					
				case CBLMask16::kTransp75:
					SetPixel16(destX,destY,BLCalcTransp2516(GetPixel16(x,y),col));
					break;
					
				case CBLMask16::kTransp50:
					SetPixel16(destX,destY,BLCalcTransp5016(GetPixel16(x,y),col));
					break;
					
				case CBLMask16::kTransp25:
					SetPixel16(destX,destY,BLCalcTransp7516(GetPixel16(x,y),col));
					break;
					
				case CBLMask16::kTransp12:
					SetPixel16(destX,destY,BLCalcTransp8716(GetPixel16(x,y),col));
					break;
					
				case CBLMask16::kTransp6:
					SetPixel16(destX,destY,BLCalcTransp9316(GetPixel16(x,y),col));
					break;
					
				case CBLMask16::kTransp0:
					SetPixel16(destX,destY,col);
					break;
			}
			
			destX++;
		}
		
		destY++;
	}
}
