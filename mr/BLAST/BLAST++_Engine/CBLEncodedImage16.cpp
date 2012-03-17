// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLEncodedImage16.cpp
// © Mark Tully 2000
// 7/1/00
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
	Implements a RLE encoded image encoded and decoder.
	Supports 16-bit colour and 13 transparency levels obtained by fast bit manipulation rather than any table lookup
	business. It's a lot faster than table look up but the transparency levels are fixed at
	100, 98.4375, 96.875, 93.75, 87.5, 75, 50, 25, 12.5, 6.25, 3.125, 1.5625
	The largest gaps are around half way as you can see, if this is unacceptable a lookup table approach could be used
	which is slower but more flexible.
*/

#include		"CBLEncodedImage16.h"
#include		"UBLEncodedImage16.h"
#include		"CBLCanvas.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The constructor takes a flattened encoding produced by GetFlattenedEncoding. The handle can be a resource handle or not,
// the handle can be adopted or not.
CBLEncodedImage16::CBLEncodedImage16(
	TBLFlattenedImage16H	inEncodedData,
	bool					inAdopt) :
	mEncodedData(inEncodedData),
	mOwnsData(inAdopt)
{
	mColMask=0L;
	mTolerance=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBLEncodedImage16::~CBLEncodedImage16()
{
	delete mColMask;

	if (mEncodedData && mOwnsData)
		::BetterDisposeHandle((Handle)mEncodedData);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetImageOffset
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::GetImageOffset(
	TBLImageVector			&outX,
	TBLImageVector			&outY)
{
	outX=(**mEncodedData).xoff;
	outY=(**mEncodedData).yoff;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetImageOffset
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::SetImageOffset(
	TBLImageVector			inX,
	TBLImageVector			inY)
{
	(**mEncodedData).xoff=inX;
	(**mEncodedData).yoff=inY;
	mChangeIndex++;		// very important if a sprite is currently displaying the image
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetImageSize
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::GetImageSize(
	TBLImageVector			&outWidth,
	TBLImageVector			&outHeight)
{
	outWidth=(**mEncodedData).width;
	outHeight=(**mEncodedData).height;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawNoClip
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::DrawNoClip(
	TBLImageVector			inX,
	TBLImageVector			inY,
	CBLCanvas					&inCanvas)
{
	//DecodeImageNoClip16(*mEncodedData,&inCanvas.GetABCRec(),inX,inY);
	UBLEncodedImage16::DecodeImageNoClip16(*mEncodedData,&inCanvas.GetABCRec(),inX,inY);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawWithClip
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::DrawWithClip(
	TBLImageVector			inX,
	TBLImageVector			inY,
	CBLCanvas					&inCanvas,
	const Rect				&inClipRect)
{	
	//DecodeImageClip16(*mEncodedData,&inCanvas.GetABCRec(),inX,inY,inClipRect);
	UBLEncodedImage16::DecodeImageClip16(*mEncodedData,&inCanvas.GetABCRec(),inX,inY,inClipRect);
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateCollisionMask
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::CreateCollisionMask(
	SInt16			inTolerance)
{
	DeleteCollisionMask();
	
	if (inTolerance>255)
		inTolerance=255;
	else if (inTolerance<0)
		inTolerance=0;
	
	mTolerance=inTolerance;
	
	// Create a new collision mask
	TBLImageVector		width,height,charWidth;

	GetImageSize(width,height);
	
	if (width&0x07)
		charWidth=width/8+3;
	else
		charWidth=width/8+2;
	
	UInt8*		mask[8];
	
	for (char n=0; n<8; n++)
	{
		mask[n]=new UInt8[charWidth*height];
		
		if (!mask[n])
		{
			for (;n>=0; --n)
				delete [] mask[n];
			
			Throw_(memFullErr);
		}
		
		std::memset(mask[n],0,charWidth*height);	// set to zeros initially
		
	}

	BuildBitMask(mask,*mEncodedData,inTolerance,charWidth);

	mColMask=new CBLCollisionMask(mask,width,height);
	if (!mColMask)
	{
		for (char n=0; n<8; n++)
			delete [] mask[n];

		ThrowIfMemFull_(mColMask);	
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteCollisionMask
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::DeleteCollisionMask()
{	
	mTolerance=0;
	
	// Delete the old collision mask
	if (mColMask)
		delete mColMask;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SpriteCollision
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CBLEncodedImage16::SpriteCollision(
	CSpriteImage		&inOther,
	Rect				&rectA,
	Rect				&rectB)
{
	return mColMask->SpriteCollision(*((CBLSpriteImage*)&inOther)->GetCollisionMask(),rectA,rectB);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RectCollision
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CBLEncodedImage16::RectCollision(
	Rect		&inRect)
{
	return mColMask->RectCollision(inRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BuildBitMask
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::BuildBitMask(
	UInt8*				outMask[8],
	SFlattenedImage16	*inData,
	UInt8				inTolerance,
	UInt8				inCharWidth)
{
	if (inData->version!=0)
		return;

	TBLImageVector	x=0,y=0;
	UInt32			opCode,opCount;
	TBLColour16		*dataPtr=(TBLColour16*)(((unsigned char*)inData)+sizeof(SFlattenedImage16));
		
	do
	{
		opCount=opCode=*(UInt32*)dataPtr;
		dataPtr=(TBLColour16*)(((unsigned char*)dataPtr)+sizeof(UInt32));
		opCount&=0x00FFFFFF;
		opCode>>=24;
						
		// switch on opCode, try most likely first
		switch (opCode)
		{
			// 100% opaque
			case kTransp0:
				opCount++;
				while (--opCount)
				{
					SetMaskBit(outMask[0],x+8,y,inCharWidth);
					SetMaskBit(outMask[1],x+7,y,inCharWidth);
					SetMaskBit(outMask[2],x+6,y,inCharWidth);
					SetMaskBit(outMask[3],x+5,y,inCharWidth);
					SetMaskBit(outMask[4],x+4,y,inCharWidth);
					SetMaskBit(outMask[5],x+3,y,inCharWidth);
					SetMaskBit(outMask[6],x+2,y,inCharWidth);
					SetMaskBit(outMask[7],x+1,y,inCharWidth);
					
					x++;
					((short*)dataPtr)++;
				}
				break;

			// totally transparent, if another token is mapped to the transparent option then the data ptr will need to be
			// altered as well, hence the dataPtr+=opCount statement which is guarded by the compile time switch
			case kTransp100:
				x+=opCount;
				break;

			case kSkipRows:
				y+=opCount;
				x=0;
				break;

			case kTransp93:
				if (inTolerance>237)
				{
					opCount++;
					while (--opCount)
					{
						SetMaskBit(outMask[0],x+8,y,inCharWidth);
						SetMaskBit(outMask[1],x+7,y,inCharWidth);
						SetMaskBit(outMask[2],x+6,y,inCharWidth);
						SetMaskBit(outMask[3],x+5,y,inCharWidth);
						SetMaskBit(outMask[4],x+4,y,inCharWidth);
						SetMaskBit(outMask[5],x+3,y,inCharWidth);
						SetMaskBit(outMask[6],x+2,y,inCharWidth);
						SetMaskBit(outMask[7],x+1,y,inCharWidth);
						
						x++;
						((short*)dataPtr)++;
					}
				}
				else
				{
					x+=opCount;
					((short*)dataPtr)+=opCount;
				}
				break;

			case kTransp87:
				if (inTolerance>221)
				{
					opCount++;
					while (--opCount)
					{
						SetMaskBit(outMask[0],x+8,y,inCharWidth);
						SetMaskBit(outMask[1],x+7,y,inCharWidth);
						SetMaskBit(outMask[2],x+6,y,inCharWidth);
						SetMaskBit(outMask[3],x+5,y,inCharWidth);
						SetMaskBit(outMask[4],x+4,y,inCharWidth);
						SetMaskBit(outMask[5],x+3,y,inCharWidth);
						SetMaskBit(outMask[6],x+2,y,inCharWidth);
						SetMaskBit(outMask[7],x+1,y,inCharWidth);
						
						x++;
						((short*)dataPtr)++;
					}
				}
				else
				{
					x+=opCount;
					((short*)dataPtr)+=opCount;
				}
				break;

			case kTransp75:
				if (inTolerance>191)
				{
					opCount++;
					while (--opCount)
					{
						SetMaskBit(outMask[0],x+8,y,inCharWidth);
						SetMaskBit(outMask[1],x+7,y,inCharWidth);
						SetMaskBit(outMask[2],x+6,y,inCharWidth);
						SetMaskBit(outMask[3],x+5,y,inCharWidth);
						SetMaskBit(outMask[4],x+4,y,inCharWidth);
						SetMaskBit(outMask[5],x+3,y,inCharWidth);
						SetMaskBit(outMask[6],x+2,y,inCharWidth);
						SetMaskBit(outMask[7],x+1,y,inCharWidth);
						
						x++;
						((short*)dataPtr)++;
					}
				}
				else
				{
					x+=opCount;
					((short*)dataPtr)+=opCount;
				}
				break;

			case kTransp50:
				if (inTolerance>127)
				{
					opCount++;
					while (--opCount)
					{
						SetMaskBit(outMask[0],x+8,y,inCharWidth);
						SetMaskBit(outMask[1],x+7,y,inCharWidth);
						SetMaskBit(outMask[2],x+6,y,inCharWidth);
						SetMaskBit(outMask[3],x+5,y,inCharWidth);
						SetMaskBit(outMask[4],x+4,y,inCharWidth);
						SetMaskBit(outMask[5],x+3,y,inCharWidth);
						SetMaskBit(outMask[6],x+2,y,inCharWidth);
						SetMaskBit(outMask[7],x+1,y,inCharWidth);
						
						x++;
						((short*)dataPtr)++;
					}
				}
				else
				{
					x+=opCount;
					((short*)dataPtr)+=opCount;
				}
				break;

			case kTransp25:
				if (inTolerance>63)
				{
					opCount++;
					while (--opCount)
					{
						SetMaskBit(outMask[0],x+8,y,inCharWidth);
						SetMaskBit(outMask[1],x+7,y,inCharWidth);
						SetMaskBit(outMask[2],x+6,y,inCharWidth);
						SetMaskBit(outMask[3],x+5,y,inCharWidth);
						SetMaskBit(outMask[4],x+4,y,inCharWidth);
						SetMaskBit(outMask[5],x+3,y,inCharWidth);
						SetMaskBit(outMask[6],x+2,y,inCharWidth);
						SetMaskBit(outMask[7],x+1,y,inCharWidth);
						
						x++;
						((short*)dataPtr)++;
					}
				}
				else
				{
					x+=opCount;
					((short*)dataPtr)+=opCount;
				}
				break;

			case kTransp12:
				if (inTolerance>31)
				{
					opCount++;
					while (--opCount)
					{
						SetMaskBit(outMask[0],x+8,y,inCharWidth);
						SetMaskBit(outMask[1],x+7,y,inCharWidth);
						SetMaskBit(outMask[2],x+6,y,inCharWidth);
						SetMaskBit(outMask[3],x+5,y,inCharWidth);
						SetMaskBit(outMask[4],x+4,y,inCharWidth);
						SetMaskBit(outMask[5],x+3,y,inCharWidth);
						SetMaskBit(outMask[6],x+2,y,inCharWidth);
						SetMaskBit(outMask[7],x+1,y,inCharWidth);
						
						x++;
						((short*)dataPtr)++;
					}
				}
				else
				{
					x+=opCount;
					((short*)dataPtr)+=opCount;
				}
				break;
				
			case kTransp6:
				if (inTolerance>15)
				{
					opCount++;
					while (--opCount)
					{
						SetMaskBit(outMask[0],x+8,y,inCharWidth);
						SetMaskBit(outMask[1],x+7,y,inCharWidth);
						SetMaskBit(outMask[2],x+6,y,inCharWidth);
						SetMaskBit(outMask[3],x+5,y,inCharWidth);
						SetMaskBit(outMask[4],x+4,y,inCharWidth);
						SetMaskBit(outMask[5],x+3,y,inCharWidth);
						SetMaskBit(outMask[6],x+2,y,inCharWidth);
						SetMaskBit(outMask[7],x+1,y,inCharWidth);
						
						x++;
						((short*)dataPtr)++;
					}
				}
				else
				{
					x+=opCount;
					((short*)dataPtr)+=opCount;
				}
				break;

			case kEndImage:
				return;
				break;
				
			default:
				return;			// unrecognised token
				break;
		}
	} while (true);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetBitMask
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLEncodedImage16::SetMaskBit(UInt8 *inPtr,TBLImageVector inX,TBLImageVector inY,UInt8 inCharWidth)
{
	inPtr+=inCharWidth*inY+inX/8;
	
	*inPtr|=(0x80 >> (inX&7));
}