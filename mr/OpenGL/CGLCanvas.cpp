// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CGLCanvas.cpp
// © John Treece-Birch 2002
// 22/1/00
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
	A canvas for Open GL graphics
*/

#include		"TNT_Debugging.h"
#include		<cmath>
#include		"CGLCanvas.h"
#include		"Utility Objects.h"
#include		"CGraphicsContextGL.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGLCanvas::CGLCanvas(
	SInt16				inWidth,
	SInt16				inHeight) :
	CCanvas(inWidth,inHeight)
{
	ThrowIfMemFull_(mPixels=new GLubyte[mWidth*mHeight*3]);
	
	for (SInt32 n=0; n<mWidth*mHeight*3; n++)
		mPixels[n]=0;
	
	mBackRed=0;
	mBackGreen=0;
	mBackBlue=0;
	
	mForeRed=255;
	mForeGreen=255;
	mForeBlue=255;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGLCanvas::~CGLCanvas()
{
	delete [] mPixels;
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPixel24
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TBLColour24 CGLCanvas::GetPixel24(
	SInt16			inX,
	SInt16			inY)
{
	if (inX<0 || inY<0 || inX>=GetWidth() || inY>=GetHeight())
		return 0;
	
	unsigned char	*pixel=&mPixels[((mHeight-1-inY)*mWidth+inX)*3];
	
	return ((*pixel) << 16) + ((*(pixel+1)) << 8) + (*(pixel+2));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetPixel24
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::SetPixel24(
	SInt16			inX,
	SInt16			inY,
	TBLColour24		inCol)
{
	if (inX<mWidth && inY<mHeight && inY>=0 && inX>=0)
	{
		unsigned char	*pixel=&mPixels[((mHeight-1-inY)*mWidth+inX)*3];
		
		if (mOpacity==1)
		{
			(*pixel++)=(inCol>>16)&255;
			(*pixel++)=(inCol>>8)&255;
			*pixel=(inCol)&255;
		}
		else
		{
			*pixel=((inCol>>16)&255)*mOpacity+(*pixel)*(1-mOpacity);
			pixel++;
			
			*pixel=((inCol>>8)&255)*mOpacity+(*pixel)*(1-mOpacity);
			pixel++;
			
			*pixel=((inCol)&255)*mOpacity+(*pixel)*(1-mOpacity);
		}
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPixelRGB
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to read outside of bounds
void CGLCanvas::GetPixelRGB(
	SInt16			inX,
	SInt16			inY,
	RGBColour		&outColour)
{
	if ((inX>=0) && (inY>=0) && (inX<mWidth) && (inY<mHeight))
	{
		unsigned char	*pixel=&mPixels[((mHeight-1-inY)*mWidth+inX)*3];
	
		outColour.red=(*pixel++)*256;
		outColour.green=(*pixel++)*256;
		outColour.blue=(*pixel++)*256;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetPixelRGB
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Ignores attempts to set outside of bounds
void CGLCanvas::SetPixelRGB(
	SInt16			inX,
	SInt16			inY,
	const RGBColour	&inColour)
{
	if ((inX>=0) && (inY>=0) && (inX<mWidth) && (inY<mHeight))
	{
		unsigned char	*pixel=&mPixels[((mHeight-1-inY)*mWidth+inX)*3];
		
		(*pixel++)=inColour.red/256;
		(*pixel++)=inColour.green/256;
		(*pixel++)=inColour.blue/256;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PaintPixel
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::PaintPixel(
	SInt16		inX,
	SInt16		inY)
{
	unsigned char	*pixel=&mPixels[(inY*mWidth+inX)*3];
	
	if (mOpacity==1)
	{
		(*pixel++)=mForeRed;
		(*pixel++)=mForeGreen;
		(*pixel++)=mForeBlue;
	}
	else if (mOpacity>0)
	{
		(*pixel)=mOpacity*mForeRed+(1-mOpacity)*(*pixel); pixel++;
		(*pixel)=mOpacity*mForeGreen+(1-mOpacity)*(*pixel); pixel++;
		(*pixel)=mOpacity*mForeBlue+(1-mOpacity)*(*pixel); pixel++;
	}
}

#pragma mark -
					
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::FillRect(
	const Rect		&inConstRect)
{
	Rect	rect=inConstRect;
	
	ClipRect(rect);

	rect.top=(mHeight-1)-rect.top;
	rect.bottom=(mHeight-1)-rect.bottom;

	if (mOpacity==1)
	{
		unsigned char	*pixel;
		
		for (int y=rect.top; y>rect.bottom; y--)
		{
			pixel=&mPixels[(y*mWidth+rect.left)*3];
			
			for (int x=rect.left; x<rect.right; x++)
			{
				(*pixel++)=mForeRed;
				(*pixel++)=mForeGreen;
				(*pixel++)=mForeBlue;
			}
		}
	}
	else if (mOpacity>0)
	{
		unsigned char	*pixel;
		
		for (int y=rect.top; y>rect.bottom; y--)
		{
			pixel=&mPixels[(y*mWidth+rect.left)*3];
			
			for (int x=rect.left; x<rect.right; x++)
			{
				(*pixel)=mOpacity*mForeRed+(1-mOpacity)*(*pixel); pixel++;
				(*pixel)=mOpacity*mForeGreen+(1-mOpacity)*(*pixel); pixel++;
				(*pixel)=mOpacity*mForeBlue+(1-mOpacity)*(*pixel); pixel++;
			}
		}
	}
}
									
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FrameRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::FrameRect(
	const Rect		&inConstRect)
{
	Rect	rect=inConstRect;
	
	Rect	clippedRect=rect;
	ClipRect(clippedRect);
	
	rect.top=(mHeight-1)-rect.top;
	rect.bottom=(mHeight-1)-rect.bottom;
	
	// Draw the top and bottom lines
	for (SInt16 x=clippedRect.left; x<clippedRect.right; x++)
	{
		if (rect.top<mHeight)
			PaintPixel(x,rect.top);
		
		if (rect.bottom+1>=0)
			PaintPixel(x,rect.bottom+1);
	}		
	
	// Draw the middle part
	rect.top=Lesser(rect.top+1,mHeight-1);
	rect.bottom=Greater(rect.bottom+1,0);
	
	for (SInt16 y=rect.bottom; y<rect.top; y++)
	{
		if (rect.left>=0)
			PaintPixel(rect.left,y);

		if (rect.right-1<mWidth)
			PaintPixel(rect.right-1,y);
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawLine
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::DrawLine(
	SInt16			inX1,
	SInt16			inY1,
	SInt16			inX2,
	SInt16			inY2)
{
	SInt16		y1=(mHeight-1)-inY1;
	SInt16		y2=(mHeight-1)-inY2;

	if (inX1<0 || y1<0 || inX1>=GetWidth() || y1>=GetHeight() ||
		inX2<0 || y2<0 || inX2>=GetWidth() || y2>=GetHeight())
	{
		Rect		rect={0,0,GetHeight(),GetWidth()};
	
		DrawLineGLClip(inX1,y1,inX2,y2,rect);
	}
	else
	{
		DrawLineGL(inX1,y1,inX2,y2);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawLineGL
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::DrawLineGL(
	SInt16			sourceX,
	SInt16			sourceY,
	SInt16			destX,
	SInt16 			destY)
{
	SInt16 				dx,dy,d,incr1,incr2,x,y,xEnd,yEnd,temp;
	
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
		
		PaintPixel(x,y);
		
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
				
				PaintPixel(x,y);
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

				PaintPixel(x,y);
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
		
		PaintPixel(x,y);
		
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

				PaintPixel(x,y);
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
				
				PaintPixel(x,y);
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawLineGLClip
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::DrawLineGLClip(
	SInt16			sourceX,
	SInt16			sourceY,
	SInt16			destX,
	SInt16 			destY,
	const Rect		&inRect)
{
	SInt16 				dx,dy,d,incr1,incr2,x,y,xEnd,yEnd,temp;

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
			PaintPixel(x,y);
		
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
					PaintPixel(x,y);
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
					PaintPixel(x,y);
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
			PaintPixel(x,y);
		
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
					PaintPixel(x,y);
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
					PaintPixel(x,y);
			}
		}
	}
}
#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillPoly
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::FillPoly(
	SInt32				*inArray,
	SInt32				inNumCoOrdPairs)
{
	if (RgnHandle poly=::NewRgn())
	{
		::MoveTo(inArray[0],(mHeight-1)-inArray[1]);
		
		::OpenRgn();
	
		for (SInt32 counter=2; counter<inNumCoOrdPairs*2; counter+=2)
			::LineTo(inArray[counter],(mHeight-1)-inArray[counter+1]);
		
		::LineTo(inArray[0],(mHeight-1)-inArray[1]);
		
		::CloseRgn(poly);

		Rect	rect;
		
		GetRegionBounds(poly,&rect);
		
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
					PaintPixel(point.h,point.v);
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FramePoly
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::FramePoly(
	SInt32				*inArray,
	SInt32				inNumCoOrdPairs)
{
	for (SInt32 count=0; count<(inNumCoOrdPairs-1)*2; count+=2)
		DrawLine(inArray[count],inArray[count+1],inArray[count+2],inArray[count+3]);
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillOval
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::FillOval(
	const Rect		&inRect)
{
	Rect			rect=inRect;
	long			centreX=(inRect.left+inRect.right)/2,centreY=(inRect.top+inRect.bottom)/2;
	long			height=FRectHeight(inRect),width=FRectWidth(inRect);
	long			sqHeight=(height/2)*(height/2),sqWidth=(width/2)*(width/2);

	for (long y=rect.top; y<rect.bottom; y++)
	{
		if ((y>=0) && (y<GetHeight()))
		{				
			float	tempY=(centreY-y)-0.5;
			float	x=std::sqrt((float)sqWidth*(1-((float)(tempY*tempY)/(float)sqHeight)));	
			float	startX=centreX-x,endX=centreX+x;
			
			if (startX-0.5>(long)startX)
				startX++;
			
			if (endX-0.5>(long)endX)
				endX++;

			for (SInt16 x=startX; x<endX; x++)
				PaintPixel(x,(mHeight-1)-y);
		}
	}
}
									
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FrameOval
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::FrameOval(
	const Rect		&inRect)
{	
	long			centreX=(inRect.left+inRect.right)/2,centreY=(inRect.top+inRect.bottom)/2;
	long			height=FRectHeight(inRect),width=FRectWidth(inRect);
	long			sqHeight=(height/2)*(height/2),sqWidth=(width/2)*(width/2);

	for (long y=inRect.top; y<inRect.bottom; y++)
	{
		if ((y>=0) && (y<mHeight))
		{				
			float	tempY=(centreY-y)-0.5;
			float	x=std::sqrt((float)sqWidth*(1-((float)(tempY*tempY)/(float)sqHeight)));	
			float	startX=centreX-x,endX=centreX+x;
			
			if (startX-0.5>(long)startX)
				startX++;
			
			if (endX-0.5>(long)endX)
				endX++;

			// Write the left pixel
			PaintPixel(startX,(mHeight-1)-y);

			// Write the right pixel
			PaintPixel(endX-1,(mHeight-1)-y);
		}
	}
	
	for (long x=inRect.left; x<=centreX; x++)
	{
		long	lastY;
		
		if ((x>=0) && (x<mWidth))
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
				// Write the top pixel
				PaintPixel(x,(mHeight-1)-(SInt16)startY);
			
				// Write the bottom pixel
				PaintPixel(x,(mHeight-1)-((SInt16)endY-1));
			}
			else
				lastY=startY;
		}
	}
	
	for (long x=inRect.right-1; x>centreX; x--)
	{
		long	lastY;
		
		if ((x>=0) && (x<mWidth))
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
				// Write the top pixel
				PaintPixel(x,(mHeight-1)-(SInt16)startY);
			
				// Write the bottom pixel
				PaintPixel(x,(mHeight-1)-((SInt16)endY-1));
			}
			else
				lastY=startY;
		}
	}
}

#pragma mark -

void CGLCanvas::DrawText(
	SInt16			inX,
	SInt16			inY,
	const CCString	&inStr)
{
	GWorldPtr		world;
	Rect			canvasRect,worldRect;
	SInt16			x,y;
	
	CalcTextRect(inX,inY,inStr,canvasRect);
	worldRect=canvasRect;
	
	ThrowIfOSErr_(::NewGWorld(&world,32,&worldRect,0,NULL,kNativeEndianPixMap));
	
	{
		UWorldSaver			safe(world);
		
		// Copy the pixel data to the gworld
		for (y=canvasRect.top; y<canvasRect.bottom; y++)
		{
			if ((y>=0) && (y<mHeight))
			{
				for (x=canvasRect.left; x<canvasRect.right; x++)
				{
					if ((x>=0) && (x<mWidth))
					{
						RGBColour	colour;
						
						colour.red=mPixels[(((mHeight-1)-y)*mWidth+x)*3+0]*256;
						colour.green=mPixels[(((mHeight-1)-y)*mWidth+x)*3+1]*256;
						colour.blue=mPixels[(((mHeight-1)-y)*mWidth+x)*3+2]*256;
						
						::SetCPixel(x,y,&colour);
					}
				}
			}
		}
		
		// Draw the text
		{
			RGBColour			foreColour={mForeRed*256,mForeGreen*256,mForeBlue*256};
			RGBColour			backColour={mBackRed*256,mBackGreen*256,mBackBlue*256};
			
			UForeColourSaver	foreSaver(foreColour);
			UBackColourSaver	backSaver(backColour);
		
			::TextFace(mTextFace);
			::TextFont(mFontId);
			::TextSize(mTextSize);
			::MoveTo(inX,inY);
			
			::DrawText(inStr.mString,0,inStr.GetLength());
		}
		
		// Copy the gworld to the pixel data
		for (y=canvasRect.top; y<canvasRect.bottom; y++)
		{
			if ((y>=0) && (y<mHeight))
			{
				for (x=canvasRect.left; x<canvasRect.right; x++)
				{
					if ((x>=0) && (x<mWidth))
					{
						RGBColour	colour;
						
						::GetCPixel(x,y,&colour);
					
						mPixels[(((mHeight-1)-y)*mWidth+x)*3+0]=colour.red/256;
						mPixels[(((mHeight-1)-y)*mWidth+x)*3+1]=colour.green/256;
						mPixels[(((mHeight-1)-y)*mWidth+x)*3+2]=colour.blue/256;
					}
				}
			}
		}
	}
	
	::DisposeGWorld(world);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalcTextRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::CalcTextRect(
	SInt16			inX,
	SInt16			inY,
	const CCString	&inStr,
	Rect			&outRect)
{
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

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawPict
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::DrawPict(
	PicHandle		inHandle,
	const Rect		&inRect)
{
	GWorldPtr	world=NULL;
	
	Try_
	{
		Rect		rect={0,0,FRectHeight(inRect),FRectWidth(inRect)};
	
		ThrowIfOSErr_(::NewGWorldWithPicHandle(&world,inHandle,32));
		
		CGrafPtr	oldPort;
			
		::GetPort(&oldPort);
		::SetPort(world);

		Rect	clippedRect;
		
		clippedRect.top=Greater(0,inRect.top);
		clippedRect.left=Greater(0,inRect.left);
		clippedRect.bottom=Lesser(mHeight,inRect.bottom);
		clippedRect.right=Lesser(mWidth,inRect.right);

		SInt16		imageWidth=FRectWidth(clippedRect);
		SInt16 		imageHeight=FRectHeight(clippedRect);
		
		for (int y=(clippedRect.top-inRect.top); y<imageHeight; y++)
		{
			SInt16		canvasY=(mHeight-1)-(y+inRect.top);
			
			for (int x=(clippedRect.left-inRect.left); x<imageWidth; x++)
			{
				SInt16		canvasX=x+inRect.left;
				RGBColor	colour;
			
				::GetCPixel(x,y,&colour);
			
				mPixels[(canvasY*mWidth+canvasX)*3+0]=colour.red/256;
				mPixels[(canvasY*mWidth+canvasX)*3+1]=colour.green/256;
				mPixels[(canvasY*mWidth+canvasX)*3+2]=colour.blue/256;
			}
		}
		
		// Cleanup
		::SetPort(oldPort);
		::DisposeGWorld(world);
	}
	
	Catch_(err)
	{
		if (world)
			::DisposeGWorld(world);
			
		throw;
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Copy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::Copy(
	CCanvas			*inSourceCanvas,
	const Rect		&inCopyRect,
	SInt16			inDestX,
	SInt16			inDestY)
{
	CGLCanvas	*glCanvas=(CGLCanvas*)inSourceCanvas;

	// Quick check of the params for null copies...
	if (FRectWidth(inCopyRect)==0 || FRectHeight(inCopyRect)==0)
		return;
		
	// Calc the dest rect
	Rect			destRect=inCopyRect;
	Rect			srcRect=inCopyRect;
	
	inDestX-=destRect.left;
	inDestY-=destRect.top;
	FOffset(destRect,inDestX,inDestY);
	
	// Clip
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
	
	SInt16		height=FRectHeight(destRect);
	SInt16		width=FRectWidth(destRect);

	for (SInt16	y=0; y<height; y++)
	{
		SInt16			sourceY=(glCanvas->GetHeight()-1)-(y+srcRect.top);
		SInt16			destY=(GetHeight()-1)-(y+destRect.top);
		
		SInt16			blitWidth=width*3;
		
		unsigned char	*destPtr=&mPixels[(destY*mWidth+destRect.left)*3];
		unsigned char	*srcPtr=&glCanvas->mPixels[(sourceY*glCanvas->mWidth+srcRect.left)*3];
		
		while (blitWidth>=sizeof(unsigned long))
		{
			*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
			blitWidth-=sizeof(unsigned long);
		}
		
		while (blitWidth>=sizeof(unsigned short))
		{
			*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
			blitWidth-=sizeof(unsigned short);
		}
		
		while (blitWidth>=sizeof(unsigned char))
		{
			*((unsigned long *)destPtr)++=*((unsigned char *)srcPtr)++;
			blitWidth-=sizeof(unsigned char);
		}
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PaintAll
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::PaintAll()
{
	unsigned char	*pixel=mPixels;
	
	for (int n=0; n<mHeight*mWidth; n++)
	{
		(*pixel++)=mForeRed;
		(*pixel++)=mForeGreen;
		(*pixel++)=mForeBlue;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EraseAll
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::EraseAll()
{	
	unsigned char	*pixel=mPixels;
	
	for (int n=0; n<mHeight*mWidth; n++)
	{
		(*pixel++)=mBackRed;
		(*pixel++)=mBackGreen;
		(*pixel++)=mBackBlue;
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClipRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLCanvas::ClipRect(
	Rect	 &ioRect)
{
	if (ioRect.top<0)
		ioRect.top=0;
		
	if (ioRect.left<0)
		ioRect.left=0;
		
	if (ioRect.bottom>mHeight)
		ioRect.bottom=mHeight;
		
	if (ioRect.right>mWidth)
		ioRect.right=mWidth;
}