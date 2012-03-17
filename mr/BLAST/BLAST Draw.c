/***************************************************************************************************

BLAST Draw.c
Extension of BLASTª engine
Mark Tully
8/10/96	:	Opened the file with BlastDrawRect which is faster than BlastEraseRect by a fair
			bit. Also blits doubles on 603 at speed.

This provides procedures for drawing to 8 bit pixmaps faster than quick draw can

SECTION 1 - Misc
SECTION 2 - Blast polygons

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

#include	<FP.h>
#include	"BLAST.h"

short		gClipTop=0;
short		gClipBottom=480;
short		gClipLeft=0;
short		gClipRight=640;

//#define	useQuickDraw

void BlastTintRect(BCPtr theRec,Rect *tintMe,TintPtr tintPtr)
{
	unsigned char		*rowStart,*ptr;
	short				len=tintMe->right-tintMe->left,wasLen=len,yCount;
	
	ptr=rowStart=theRec->baseAddr+tintMe->top*theRec->rowBytes+tintMe->left;
	
	for (yCount=tintMe->top; yCount<tintMe->bottom; yCount++)
	{
		while (len>0)
		{
			*ptr=tintPtr[*ptr];
			ptr++;
			len--;
		}
		len=wasLen;
		rowStart+=theRec->rowBytes;
		ptr=rowStart;
	}
}

void BlastTintRect16(BCPtr inTintRec,Rect *inTintRect,BlastColour16 inTintColour,double inPercent)
{
	unsigned char		*rowStart,*ptr;
	short				len=inTintRect->right-inTintRect->left,wasLen=len,yCount;
	BlastColour16		sourceColour;
	RGBColour			tintColour,colour;
	
	ptr=rowStart=inTintRec->baseAddr+inTintRect->top*inTintRec->rowBytes+inTintRect->left*sizeof(unsigned short);
	
	for (yCount=inTintRect->top; yCount<inTintRect->bottom; yCount++)
	{
		while (len>0)
		{
			sourceColour=*(unsigned short*)ptr;
			
			BlastColour16ToRGBColour(inTintColour,&tintColour);
			BlastColour16ToRGBColour(sourceColour,&colour);
			
			colour.red=tintColour.red*inPercent+colour.red*(1-inPercent);
			colour.green=tintColour.green*inPercent+colour.green*(1-inPercent);
			colour.blue=tintColour.blue*inPercent+colour.blue*(1-inPercent);
			
			*(unsigned short*)ptr=RGBColourToBlastColour16(&colour);
			
			ptr+=sizeof(unsigned short);
			len--;
		}
		len=wasLen;
		rowStart+=inTintRec->rowBytes;
		ptr=rowStart;
	}
}

void BlastTintRect1650(BCPtr inTintRec,Rect *inTintRect,BlastColour16 inTintColour)
{
	unsigned char		*rowStart,*ptr;
	short				len=inTintRect->right-inTintRect->left,wasLen=len,yCount;
	BlastColour16		sourceColour;
	
	ptr=rowStart=inTintRec->baseAddr+inTintRect->top*inTintRec->rowBytes+inTintRect->left*sizeof(unsigned short);
	
	for (yCount=inTintRect->top; yCount<inTintRect->bottom; yCount++)
	{
		while (len>0)
		{
			sourceColour=*(unsigned short*)ptr;
			*(unsigned short*)ptr=BlastBlend1650(sourceColour,inTintColour);
			ptr+=sizeof(unsigned short);
			len--;
		}
		len=wasLen;
		rowStart+=inTintRec->rowBytes;
		ptr=rowStart;
	}
}

// New technique
// The idea here to to write the specified byte to the pixmap as fast as is possible.
// To do this I take the byte and double it up into shorts, longs, and doubles.
// Now, ppc has to do hardware correction of a write if the data you're writing isn't on it's
// natural boundies. This only caused a slight delay on the 601 but a software interuppt on
// 603. So what I do is make the address I'm writing to a multiple of 8 (double size) and then
// write doubles. The slight overhead from doing this is generally made up for in the speed up
// of writes on 601. However, on 603 it speeds it up tremendously as there are no interuppts
// thrown.
void BlastDrawRect(BCPtr theRec,Rect *eraseMe,unsigned char colIndex)
{
	unsigned char		*rowStart,*pointer;
	unsigned short		numDone=0,numToDo,eraseCount,numToErase;
	unsigned short		colIndexShort;
	unsigned long		colIndexLong;
	double				colIndexDouble;
	
	// these are faster than bit operations ("shift" with "or")
	((unsigned char*)&colIndexShort)[0]=colIndex;
	((unsigned char*)&colIndexShort)[1]=colIndex;

	((unsigned short*)&colIndexLong)[0]=colIndexShort;
	((unsigned short*)&colIndexLong)[1]=colIndexShort;

	((unsigned long*)&colIndexDouble)[0]=colIndexLong;
	((unsigned long*)&colIndexDouble)[1]=colIndexLong;
	
	rowStart=theRec->baseAddr+eraseMe->top*theRec->rowBytes+eraseMe->left;
	eraseCount=eraseMe->right-eraseMe->left;
	numToDo=eraseMe->bottom-eraseMe->top;
	
	while (numDone<numToDo)
	{
		pointer=rowStart;
		rowStart+=theRec->rowBytes;
		numToErase=eraseCount;
		numDone++;
		
		// is address a multiple of 8
		switch (((unsigned long)pointer)&3)
		{
			case 7:
				// byte, short, long
				*((unsigned char*)pointer)++=colIndex;
				*((unsigned short*)pointer)++=colIndexShort;
				*((unsigned long*)pointer)++=colIndexLong;
				numToErase-=7;
				break;
				
			case 6:
				// short, long
				*((unsigned short*)pointer)++=colIndexShort;
				*((unsigned long*)pointer)++=colIndexLong;
				numToErase-=6;
				break;
				
			case 5:
				// byte, long
				*((unsigned char*)pointer)++=colIndex;
				*((unsigned long*)pointer)++=colIndexLong;
				numToErase-=5;
				break;

			case 4:
				// long
				*((unsigned long*)pointer)++=colIndexLong;
				numToErase-=4;
				break;
				
			case 3:
				// byte, short
				*((unsigned char*)pointer)++=colIndex;
				numToErase-=3;
				break;
				
			case 2:
				// short
				*((unsigned short*)pointer)++=colIndexShort;
				numToErase-=2;
				break;
				
			case 1:
				// byte
				*((unsigned char*)pointer)++=colIndex;
				numToErase-=1;
				break;
		}
		
		// now we can blit doubles
		while (numToErase>=sizeof(double))
		{
			*((double *)pointer)++=colIndexDouble;
			numToErase-=sizeof(double);
		}	
		
		// it can be out here by a max of 7 bytes as the line length may not be a multiple of 8
		switch (numToErase)
		{
			case 7:
				// byte, short, long
				*((unsigned char*)pointer)++=colIndex;
				*((unsigned short*)pointer)++=colIndexShort;
				*((unsigned long*)pointer)++=colIndexLong;
				break;
				
			case 6:
				// short, long
				*((unsigned short*)pointer)++=colIndexShort;
				*((unsigned long*)pointer)++=colIndexLong;
				break;
				
			case 5:
				// byte, long
				*((unsigned char*)pointer)++=colIndex;
				*((unsigned long*)pointer)++=colIndexLong;
				break;

			case 4:
				// long
				*((unsigned long*)pointer)++=colIndexLong;
				break;
				
			case 3:
				// byte, short
				*((unsigned char*)pointer)++=colIndex;
				break;
				
			case 2:
				// short
				*((unsigned short*)pointer)++=colIndexShort;
				break;
				
			case 1:
				// byte
				*((unsigned char*)pointer)++=colIndex;
				break;
		}	
	}
}

// Finished tests
// for full screen clock effect (in ms)
// Toolbox				~660
// BlastLine			~473
// BlastLineBresenham	~159
// RAA!!
// Fundamentals of interactive computer graphics pg 434
// BlastLineBresenham
void BlastLine(BCPtr theRec,short sourceX,short sourceY,short destX,short destY,unsigned char col)
{
	#ifdef useQuickDraw
	ForeColor(col);
	MoveTo(sourceX,sourceY);
	LineTo(destX,destY);
	ForeColor(blackColor);
	#else
	
	short 				dx,dy,d,incr1,incr2,x,y,xEnd,yEnd,temp;
	unsigned char		*thePtr=theRec->baseAddr;
	short				rowBytes=theRec->rowBytes;
	
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
		
		thePtr+=x;
		thePtr+=(y*rowBytes);
		
		//BlastSetPixel(theRec,x,y,col);
		(*thePtr)=col;
		
		if (destY>=sourceY)
		{
			while (x<xEnd)
			{
				x++;
				thePtr++;
				
				if (d<0)
					d+=incr1;
				else
				{
					//y++;
					d+=incr2;
					thePtr+=rowBytes;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=col;
			}
		}
		else
		{
			while (x<xEnd)
			{
				x++;
				thePtr++;
				
				if (d<0)
					d+=incr1;
				else
				{
					//y--;
					d+=incr2;
					thePtr-=rowBytes;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=col;
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
		
		thePtr+=x;
		thePtr+=(y*rowBytes);
		
		//BlastSetPixel(theRec,x,y,col);
		(*thePtr)=col;
		
		if (destX>=sourceX)
		{
			while (y<yEnd)
			{
				y++;
				thePtr+=rowBytes;
				
				if (d<0)
					d+=incr1;
				else
				{
					//x++;
					d+=incr2;
					thePtr++;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=col;
			}
		}
		else
		{
			while (y<yEnd)
			{
				y++;
				thePtr+=rowBytes;
				
				if (d<0)
					d+=incr1;
				else
				{
					//x--;
					d+=incr2;
					thePtr--;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=col;
			}
		}
	}
		
	#endif
}

// same as above but draws 2 pixel thick lines, it is equivalent to calling this with x1,y1 to
// x2,y2 then calling it again with x1+1,y1 to x2+1,y1
void BlastLineX2(BCPtr theRec,short sourceX,short sourceY,short destX,short destY,unsigned char col)
{
	#ifdef useQuickDraw
	ForeColor(col);
	MoveTo(sourceX,sourceY);
	LineTo(destX,destY);
	ForeColor(blackColor);
	#else
	
	short 				dx,dy,d,incr1,incr2,x,y,xEnd,yEnd,temp;
	unsigned char		*thePtr=theRec->baseAddr,*otherPtr;
	short				rowBytes=theRec->rowBytes;
	short				blit;
	
	blit=col<<8|col;
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
		
		thePtr+=x;
		thePtr+=(y*rowBytes);
		otherPtr=thePtr+rowBytes;
		
		//BlastSetPixel(theRec,x,y,col);
		(*(short*)thePtr)=blit;
		(*(short*)otherPtr)=blit;
		
		if (destY>=sourceY)
		{
			while (x<xEnd)
			{
				x++;
				thePtr++;
				otherPtr++;
				
				if (d<0)
					d+=incr1;
				else
				{
					//y++;
					d+=incr2;
					thePtr+=rowBytes;
					otherPtr+=rowBytes;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*(short*)thePtr)=blit;
				(*(short*)otherPtr)=blit;
			}
		}
		else
		{
			while (x<xEnd)
			{
				x++;
				thePtr++;
				otherPtr++;
				
				if (d<0)
					d+=incr1;
				else
				{
					//y--;
					d+=incr2;
					thePtr-=rowBytes;
					otherPtr-=rowBytes;
				}
				(*(short*)thePtr)=blit;
				(*(short*)otherPtr)=blit;
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
		
		thePtr+=x;
		thePtr+=(y*rowBytes);
		otherPtr=thePtr+rowBytes;
		
		//BlastSetPixel(theRec,x,y,col);
		(*(short*)thePtr)=blit;
		(*(short*)otherPtr)=blit;
		
		if (destX>=sourceX)
		{
			while (y<yEnd)
			{
				y++;
				thePtr+=rowBytes;
				otherPtr+=rowBytes;
				
				if (d<0)
					d+=incr1;
				else
				{
					//x++;
					d+=incr2;
					thePtr++;
					otherPtr++;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*(short*)thePtr)=blit;
				(*(short*)otherPtr)=blit;
			}
		}
		else
		{
			while (y<yEnd)
			{
				y++;
				thePtr+=rowBytes;
				otherPtr+=rowBytes;
				
				if (d<0)
					d+=incr1;
				else
				{
					//x--;
					d+=incr2;
					thePtr--;
					otherPtr--;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*(short*)thePtr)=blit;
				(*(short*)otherPtr)=blit;
			}
		}
	}
		
	#endif
}

// same as normal blast line except it tints it
void BlastTintLine(BCPtr theRec,TintPtr tintMap,short sourceX,short sourceY,short destX,short destY)
{
	#ifdef useQuickDraw
	
	#else
	
	short 				dx,dy,d,incr1,incr2,x,y,xEnd,yEnd,temp;
	unsigned char		*thePtr=theRec->baseAddr;
	short				rowBytes=theRec->rowBytes;
	
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
		
		thePtr+=x;
		thePtr+=(y*rowBytes);
		
		//BlastSetPixel(theRec,x,y,col);
		(*thePtr)=tintMap[(*thePtr)];
		
		if (destY>=sourceY)
		{
			while (x<xEnd)
			{
				x++;
				thePtr++;
				
				if (d<0)
					d+=incr1;
				else
				{
					//y++;
					d+=incr2;
					thePtr+=rowBytes;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=tintMap[(*thePtr)];
			}
		}
		else
		{
			while (x<xEnd)
			{
				x++;
				thePtr++;
				
				if (d<0)
					d+=incr1;
				else
				{
					//y--;
					d+=incr2;
					thePtr-=rowBytes;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=tintMap[(*thePtr)];
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
		
		thePtr+=x;
		thePtr+=(y*rowBytes);
		
		//BlastSetPixel(theRec,x,y,col);
		(*thePtr)=tintMap[(*thePtr)];
		
		if (destX>=sourceX)
		{
			while (y<yEnd)
			{
				y++;
				thePtr+=rowBytes;
				
				if (d<0)
					d+=incr1;
				else
				{
					//x++;
					d+=incr2;
					thePtr++;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=tintMap[(*thePtr)];
			}
		}
		else
		{
			while (y<yEnd)
			{
				y++;
				thePtr+=rowBytes;
				
				if (d<0)
					d+=incr1;
				else
				{
					//x--;
					d+=incr2;
					thePtr--;
				}
		//		BlastSetPixel(theRec,x,y,col);
				(*thePtr)=tintMap[(*thePtr)];
			}
		}
	}
		
	#endif
}


/*
BlastLine(BCPtr theRec,short sourceX,short sourceY,short destX,short destY,unsigned char col)
{
	#ifndef useQuickDraw

	float			gradient;
	short			counter;
	float			xDist=destX-sourceX,absXDist;
	float			yDist=destY-sourceY,absYDist;
	unsigned char	*temp;
	unsigned long	rowBytes=theRec->rowBytes;

	absXDist=FAbs(xDist);
	absYDist=FAbs(yDist);
	
	temp=theRec->baseAddr+rowBytes*sourceY+sourceX;

	if (absXDist>=absYDist)
	{
		if (xDist>0)
		{
			gradient=yDist/xDist;
			
			for (counter=0; counter<absXDist; counter++)
			{
				//SetCPixel(counter+sourceX,counter*gradient+sourceY,&red);
				*(temp+counter+rowBytes*(long)(counter*gradient))=col;
			}
		}
		else
		{
			gradient=-yDist/xDist;
			
			for (counter=0; counter<absXDist; counter++)
			{
				//SetCPixel(sourceX-counter,counter*gradient+sourceY,&blue);
				//*(temp-counter+theRec->rowBytes*(long)(counter*gradient))=col;
				*(temp-counter+rowBytes*(long)(counter*gradient))=col;
			}
		}
	}
	else
	{
		if (yDist>0)
		{
			gradient=xDist/yDist;
			
			for (counter=0; counter<absYDist; counter++)
			{
				//SetCPixel(counter*gradient+sourceX,sourceY+counter,&white);
				*(temp+(long)(counter*gradient)+rowBytes*counter)=col;
			}
		}
		else
		{	
			gradient=-xDist/yDist;
			
			for (counter=(absYDist-1); counter>=0; counter--)
			{
			//	SetCPixel(counter*gradient+sourceX,sourceY-counter,&green);
				*(temp+(long)(counter*gradient)-rowBytes*counter)=col;
			}
		}
	}

	#else
	
	ForeColor(col);
	MoveTo(sourceX,sourceY);
	LineTo(destX,destY);
	ForeColor(blackColor);
	
	#endif
}*/

void ShutDownBlastDraw()
{
	DisposeBlastPolygon();
}

// ********* SECTION 2 - BLAST Polygon ********

short		*gLeftBoundry=0L,*gRightBoundry=0L;

// call this at the beginnging of your program. Give it the max y value that a pixel could take
// like 480. If you get this wrong you'll trash random memory.
Boolean InitBlastPolygon(short pixmapHeight)
{
	DisposeBlastPolygon();
	
	gLeftBoundry=(short*)NewPtrClear(pixmapHeight*sizeof(short));
	gRightBoundry=(short*)NewPtrClear(pixmapHeight*sizeof(short));

	return (gLeftBoundry && gRightBoundry);
}

// frees mem allocated above
void DisposeBlastPolygon()
{
	if (gLeftBoundry)
	{
		DisposePtr((Ptr)gLeftBoundry);
		gLeftBoundry=0L;
	}
	
	if (gRightBoundry)
	{
		DisposePtr((Ptr)gRightBoundry);
		gRightBoundry=0L;
	}
}

void SetBlastDrawClip(short top,short left,short bottom,short right)
{
	gClipTop=top;
	gClipLeft=left;
	gClipBottom=bottom;
	gClipRight=right;
}

/*BlastDrawCVPolygon(BCPtr theRec,CoordPtr theCoords,short coordCount,unsigned char colIndex)
{
	short		counter;
	Coord		triangle[4];
		
	triangle[0]=theCoords[0];
	triangle[3]=theCoords[0];
	
	// split the polygon up into triangles and draw them instead
	for (counter=0; counter<(coordCount-2); counter++)
	{
		triangle[1]=theCoords[counter+1];
		triangle[2]=theCoords[counter+2];
		BlastDrawTriangle(theRec,triangle,colIndex);
	}
}*/

// polygon must be closed and defined in a clockwise direction (unless you specify acw)
void BlastDrawCVPolygon(BCPtr theRec,CoordPtr theCoords,short coordCount,unsigned char colIndex,Boolean acw)
{
	#ifdef useQuickDraw

	PolyHandle		thePoly;
	short			counter;
	
	thePoly=OpenPoly();
	MoveTo(theCoords[0].x,theCoords[0].y);
	for (counter=1; counter<coordCount; counter++)
		LineTo(theCoords[counter].x,theCoords[counter].y);
	ClosePoly();

	RGBForeColor(&(**(**theRec->pixMap).pmTable).ctTable[colIndex].rgb);
	PaintPoly(thePoly);
	KillPoly(thePoly);
	ForeColor(blackColor);

	#else

	float			gradient;				// gradient between this and the next point
	float			x;						// current x
	short			highestY,lowestY;		// the greatest and least value of y acheived
	short			counter,smallCount;		// counters used in loops
	short			upTo,numToDo;		// loop terminators
	unsigned char	*temp,*rowStart;		// pixmap ptrs
	unsigned short	colIndexShort;			// the colour we're using doubled,quadrupled and octupled (?!)
	unsigned long	colIndexLong;
	double			colIndexDouble;
	short			*leftBoundry=gLeftBoundry,*rightBoundry=gRightBoundry,*tempShortPtr;
	
	/// init to max
	highestY=-32768;
	lowestY=32767;
	
	for (counter=0; counter<coordCount; counter++)
	{
		if (theCoords[counter].y>highestY)
			highestY=theCoords[counter].y;
		if (theCoords[counter].y<lowestY)
			lowestY=theCoords[counter].y;
	}
	
	highestY=Limit(highestY,gClipTop,gClipBottom);
	lowestY=Limit(lowestY,gClipTop,gClipBottom);
	
	// for each point
	for (counter=0; counter<(coordCount-1); counter++)
	{
		// find the gradient between this and the next point
		gradient=((float)(theCoords[counter+1].x-theCoords[counter].x))/(float)(theCoords[counter+1].y-theCoords[counter].y);
		if (theCoords[counter].y<theCoords[counter+1].y)
		{
			// if the next point is lower on screen then make this a right edge
			upTo=Lesser(theCoords[counter+1].y,gClipBottom-1);	// -1 as don't want to access element 480 of a 480 array
			smallCount=gClipTop-theCoords[counter].y;
			if (smallCount>0)
			{
				x=theCoords[counter].x+(gradient*smallCount);
				smallCount=gClipTop;
			}
			else
			{
				smallCount=theCoords[counter].y;
				x=theCoords[counter].x;
			}

			for (; smallCount<=upTo; smallCount++)
			{
				rightBoundry[smallCount]=(short)x;
				rightBoundry[smallCount]=Limit(rightBoundry[smallCount],gClipLeft,gClipRight);
				x+=gradient;
			}
		}
		else if (theCoords[counter+1].y<theCoords[counter].y)
		{
			// if the next point is higher on screen then make this a left edge
			upTo=Lesser(theCoords[counter].y,gClipBottom-1);	// -1 as don't want to access element 480 of a 480 array
			smallCount=gClipTop-theCoords[counter+1].y;
			if (smallCount>0)
			{
				x=theCoords[counter+1].x+(gradient*smallCount);
				smallCount=gClipTop;
			}
			else
			{
				smallCount=theCoords[counter+1].y;
				x=theCoords[counter+1].x;
			}
			
			for (; smallCount<=upTo; smallCount++)
			{
				leftBoundry[smallCount]=(short)x;
				leftBoundry[smallCount]=Limit(leftBoundry[smallCount],gClipLeft,gClipRight);
				x+=gradient;
			}
		}
	}
	
	// these are faster than bit operations ("shift" with "or")
	((unsigned char*)&colIndexShort)[0]=colIndex;
	((unsigned char*)&colIndexShort)[1]=colIndex;

	((unsigned short*)&colIndexLong)[0]=colIndexShort;
	((unsigned short*)&colIndexLong)[1]=colIndexShort;

	((unsigned long*)&colIndexDouble)[0]=colIndexLong;
	((unsigned long*)&colIndexDouble)[1]=colIndexLong;
	
	if (acw)
	{		
		tempShortPtr=leftBoundry;
		leftBoundry=rightBoundry;
		rightBoundry=tempShortPtr;
	}
	
	rowStart=theRec->baseAddr+lowestY*theRec->rowBytes;
	
	for (counter=lowestY; counter<highestY; counter++)
	{
		temp=rowStart;
		rowStart+=theRec->rowBytes;
		temp+=leftBoundry[counter];
		numToDo=rightBoundry[counter]-leftBoundry[counter];
		
		// fill the scan line 'temp' between leftBoundry[counter] and rightBoundry[counter]+1
		while (numToDo>=sizeof(double))
		{
			*((double *)temp)++=colIndexDouble;
			numToDo-=sizeof(double);
		}
		
		while (numToDo>=sizeof(unsigned long))
		{
			*((unsigned long *)temp)++=colIndexLong;
			numToDo-=sizeof(unsigned long);
		}
		
		while (numToDo>=sizeof(unsigned short))
		{
			*((unsigned short *)temp)++=colIndexShort;
			numToDo-=sizeof(unsigned short);
		}
		
		while (numToDo>=sizeof(unsigned char))
		{
			*((unsigned char *)temp)++=colIndex;
			numToDo-=sizeof(unsigned char);
		}
	}
	#endif
}

// draws a triangle as specified. Coords is an array of 3 coords
void BlastDrawTriangle(BCPtr theRec,CoordPtr theCoords,unsigned char colIndex)
{
	#ifdef useQuickDraw

	PolyHandle		thePoly;
	
	thePoly=OpenPoly();
	MoveTo(theCoords[0].x,theCoords[0].y);
	LineTo(theCoords[1].x,theCoords[1].y);
	LineTo(theCoords[2].x,theCoords[2].y);
	LineTo(theCoords[0].x,theCoords[0].y);
	ClosePoly();

	RGBForeColor(&(**(**theRec->pixMap).pmTable).ctTable[colIndex].rgb);
	PaintPoly(thePoly);
	KillPoly(thePoly);
	ForeColor(blackColor);

	#else

	Coord				h1,h2,h3;
	short				numToDo,counter,*leftBoundry=gLeftBoundry,*rightBoundry=gRightBoundry;
	short				*tempBoundry,rowsToDo;
	unsigned char		*temp,*rowStart;
	unsigned short		colIndexShort;
	unsigned long		colIndexLong;
	double				colIndexDouble;
	
	// these are faster than bit operations ("shift" with "or")
	((unsigned char*)&colIndexShort)[0]=colIndex;
	((unsigned char*)&colIndexShort)[1]=colIndex;

	((unsigned short*)&colIndexLong)[0]=colIndexShort;
	((unsigned short*)&colIndexLong)[1]=colIndexShort;

	((unsigned long*)&colIndexDouble)[0]=colIndexLong;
	((unsigned long*)&colIndexDouble)[1]=colIndexLong;

	// check that the coords are not all level in either axis
	if ((theCoords[0].x==theCoords[1].x) && (theCoords[1].x==theCoords[2].x))
		return;
	if ((theCoords[0].y==theCoords[1].y) && (theCoords[1].y==theCoords[2].y))
		return;
	
	// now sort the coords into y order, highest up the screen goes in h1, next in h2 last h3.
	if(theCoords[0].y<theCoords[1].y)
	{
		// 0 is higher than 1
		if (theCoords[0].y<theCoords[2].y)
		{
			// 0 is higher than 1 and 2
			if (theCoords[1].y<theCoords[2].y)
			{
				h1=theCoords[0];
				h2=theCoords[1];
				h3=theCoords[2];
			}
			else
			{
				h1=theCoords[0];
				h2=theCoords[2];
				h3=theCoords[1];
			}
		}
		else
		{
			// 2 is higher than 0, 0 is higher than 1
			h1=theCoords[2];
			h2=theCoords[0];
			h3=theCoords[1];
		}
	}
	else
	{
		// 1 is higher than 0
		if (theCoords[1].y<theCoords[2].y)
		{
			// 1 is higher than 2 and higher than 0
			if (theCoords[2].y<theCoords[0].y)
			{
				h1=theCoords[1];
				h2=theCoords[2];
				h3=theCoords[0];
			}
			else
			{
				h1=theCoords[1];
				h2=theCoords[0];
				h3=theCoords[2];
			}
		}
		else
		{
			// 1 is higher than 0 and 2 is higher than 1
			h1=theCoords[2];
			h2=theCoords[1];
			h3=theCoords[0];
		}
	}
	
	// figure out the boundries of the triangle
	if (h1.x<=h2.x)
	{
		// create right side boundry between h1 and h2
		CreateBoundry(rightBoundry,h1.x,h1.y,h2.x,h2.y);
		// append the to the boundry the line between h2 and h3
		CreateBoundry(rightBoundry,h2.x,h2.y,h3.x,h3.y);
		// create the left side of the boundry between h1 and h3
		CreateBoundry(leftBoundry,h1.x,h1.y,h3.x,h3.y);
		
		// sometimes it is necessary to swap the boundries around if h2 is on the wrong side
		// gClipBottom-1 as we never access a boundry by gClipBottom
		if (h2.x<leftBoundry[Greater(Lesser(h2.y,gClipBottom-1),gClipTop)])
		{
			tempBoundry=leftBoundry;
			leftBoundry=rightBoundry;
			rightBoundry=tempBoundry;
		}
	}
	else //if (h1.x>=h2.x)
	{
		// create left side boundry between h1 and h2
		CreateBoundry(leftBoundry,h1.x,h1.y,h2.x,h2.y);
		// append the to the boundry the line between h2 and h3
		CreateBoundry(leftBoundry,h2.x,h2.y,h3.x,h3.y);
		// create the left side of the boundry between h1 and h3
		CreateBoundry(rightBoundry,h1.x,h1.y,h3.x,h3.y);
		
		// sometimes it is necessary to swap the boundries around if h2 is on the wrong side
		// gClipBottom-1 as we never access a boundry by gClipBottom
		if (h2.x>rightBoundry[Greater(Lesser(h2.y,gClipBottom-1),gClipTop)])
		{
			tempBoundry=leftBoundry;
			leftBoundry=rightBoundry;
			rightBoundry=tempBoundry;
		}
	}
	
	// fill the boundries
	counter=Greater(h1.y,gClipTop);
	rowStart=theRec->baseAddr+counter*theRec->rowBytes;
	rowsToDo=Lesser(h3.y,gClipBottom);
	for (; counter<rowsToDo; counter++)
	{
		temp=rowStart;
		rowStart+=theRec->rowBytes;
		numToDo=Greater(leftBoundry[counter],gClipLeft);
		temp+=numToDo;
		numToDo=Lesser(rightBoundry[counter]+1,gClipRight)-numToDo;
		
		// fill the scan line 'temp' between leftBoundry[counter] and rightBoundry[counter]+1
		while (numToDo>=sizeof(double))
		{
			*((double *)temp)++=colIndexDouble;
			numToDo-=sizeof(double);
		}
		
		while (numToDo>=sizeof(unsigned long))
		{
			*((unsigned long *)temp)++=colIndexLong;
			numToDo-=sizeof(unsigned long);
		}
		
		while (numToDo>=sizeof(unsigned short))
		{
			*((unsigned short *)temp)++=colIndexShort;
			numToDo-=sizeof(unsigned short);
		}
		
		while (numToDo>=sizeof(unsigned char))
		{
			*((unsigned char *)temp)++=colIndex;
			numToDo-=sizeof(unsigned char);
		}	
	}
	
	#endif
}

/*CreateBoundry(short *boundry,short sourceX,short sourceY,short destX,short destY)
{
	if (destY!=sourceY)
	{
		short			counter,upTo;
		float			gradient;
		
		gradient=((float)destX-(float)sourceX)/((float)destY-(float)sourceY);
		
		upTo=Lesser(destY,(gClipBottom-1));	// -1 as don't want to access element 480 of a 480 array
		counter=Greater(sourceY,gClipTop);
		
		// for each scanline between the two points
		for (; counter<=upTo; counter++)
		{
			boundry[counter]=sourceX+(counter-sourceY)*gradient;		// should be divide by gradient, but gradient is x/y ie the reciprocal of the actual gradient
		}
	}
}*/

// the new create boundry routine.
// NB: destY>=sourceY
void CreateBoundry(short *boundry,short sourceX,short sourceY,short destX,short destY)
{
	if (destY!=sourceY)
	{
		float	gradient,x;
		short	counter,upTo;
		
		gradient=((float)(destX-sourceX))/(float)(destY-sourceY);

		upTo=Lesser(destY,(gClipBottom-1));	// -1 as don't want to access element 480 of a 480 array
		//counter=Greater(sourceY,gClipTop);
		counter=gClipTop-sourceY;
		if (counter>0)
		{
			x=sourceX+(gradient*counter);
			counter=gClipTop;
		}
		else
		{
			counter=sourceY;
			x=sourceX;
		}
		
		while (counter<=upTo)
		{
			boundry[counter++]=(short)x;
			x+=gradient;
		}
	}
}