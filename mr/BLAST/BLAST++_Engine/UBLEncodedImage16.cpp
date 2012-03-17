// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// UBLEncodedImage16.cpp
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
	Supports 16-bit colour and 9 transparency levels obtained by fast bit manipulation rather than any table lookup
	business. It's a lot faster than table look up but the transparency levels are fixed at
	100, 93.75, 87.5, 75, 50, 25, 12.5, 6.25
	The largest gaps are around half way as you can see, if this is unacceptable a lookup table approach could be used
	which is slower but more flexible.
*/

#include		"UBLEncodedImage16.h"
#include		"CBLMask16.h"
#include		"CBLCanvas.h"
#include		"BLCalcTransp16.h"
#include		"Utility Objects.h"
#include		"CResourceContainer.h"
#include		"CResource.h"
#include		"UResources.h"

// Stream parser used by decoder and encoded to create the stream
#include		"TNewHandleStream.h"
#include		"StMarkerSaver.h"

class CBLEncodedImageStream16 : public TNewHandleStream
{
	protected:
		TEncodeToken		mLastToken;
		UInt16				mCount;
		UInt32				mLastTokenMarker;

	public:
							CBLEncodedImageStream16(
								UInt16			inWidth,
								UInt16			inHeight)
							{
								mNativeEndian=true;

								mLastToken=kNotInRun;
								mCount=0;
								
								SFlattenedImage16	header;
								
								header.version=0;
								header.width=inWidth;
								header.height=inHeight;
								header.xoff=0;
								header.yoff=0;
								
								WriteBlock(&header,sizeof(header));
								
								mLastTokenMarker=GetMarker();
								
								(*this) << 0L; // write a null op code to fill out later
							}
	
		void /*e*/			Add(
								TEncodeToken	inToken,
								UInt16			inColour=0)
							{
								if (inToken!=mLastToken)
								{
									// The token has changed to a new run, if the new token is an end line or a end shape
									// and the last token was a transp100 or a skip rows then we don't need to store the
									// last token
									if (((inToken==kSkipRows) || (inToken==kEndImage)) && (mLastToken==kTransp100))
										DiscardRun();
									else
										FinishRun();

									// if it's the end of the image then write the token out immediately rather than starting
									// a run									
									if (inToken==kEndImage)
									{
										mLastToken=kEndImage;
										FinishRun();
										return;
									}
									
									mLastToken=inToken;
								}
								mCount++;
								if (mLastToken!=kTransp100 && mLastToken!=kSkipRows && mLastToken!=kEndImage)
									(*this) << inColour;
							}

		// Throws away the current run, the next run will be stored where this one was, note that discarding a run with
		// data outputted is bad as the next token written will be before the data and it's data will be after THAT data.
		void				DiscardRun()
							{
								mLastToken=kNotInRun;
								mCount=0;
							}
		
		void				FinishRun()
							{
								if (mLastToken!=kNotInRun)
								{
									{
										StMarkerSaver	safe(*this,mLastTokenMarker);									
										(*this) << (UInt32)(mLastToken << 24 | mCount);
									}
									mLastTokenMarker=GetMarker();
									//SetMarker(4,streamFrom_Marker);
									if (mLastToken!=kEndImage)
										(*this) << 0L;	// write a null op code - fill out later, unless of course there are no more tokens
									mLastToken=kNotInRun;
									mCount=0;
								}
							}
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeImageClip16												Static
//		¥ DecodeImageNoClip16											Static
//		¥ DecodeImage													Static
//		¥ DecodeImageNoClip												Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Assumes the ABCRec is a 16bit
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(inColour)

#define			BL_TRANSP0CASE			case kTransp0:
#define			BL_TRANSP6CASE			case kTransp6:
#define			BL_TRANSP12CASE			case kTransp12:
#define			BL_TRANSP25CASE			case kTransp25:
#define			BL_TRANSP50CASE			case kTransp50:
#define			BL_TRANSP75CASE			case kTransp75:
#define			BL_TRANSP87CASE			case kTransp87:
#define			BL_TRANSP93CASE			case kTransp93:
#define			BL_TRANSP100CASE		case kTransp100:

#define			BL_DECODECLIP16			UBLEncodedImage16::DecodeImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImage16::DecodeImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImage16::DecodeImageClip
#define			BL_DECODENOCLIP			UBLEncodedImage16::DecodeImageNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL

#undef			BL_TRANSP0CASE
#undef			BL_TRANSP6CASE
#undef			BL_TRANSP12CASE
#undef			BL_TRANSP25CASE
#undef			BL_TRANSP50CASE
#undef			BL_TRANSP75CASE
#undef			BL_TRANSP87CASE
#undef			BL_TRANSP93CASE
#undef			BL_TRANSP100CASE

#undef			BL_DECODECLIP16
#undef			BL_DECODERNOCLIP16
#undef			BL_DECODERCLIP
#undef			BL_DECODERNOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EncodeImage													Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Encodes the data into an image
TBLFlattenedImage16H UBLEncodedImage16::EncodeImage(
	CBLCanvas			&inImage,
	Rect				&inImageRect,
	CBLMask16			*inMask,
	Boolean				inShrinkToContents,
	SInt16				&outXChange,
	SInt16				&outYChange)
{
	UInt16					sourceColour;
	
	outXChange=0;
	outYChange=0;
	
	// Shrink To Contents
	if (inShrinkToContents)
		ShrinkToContents(inImage,inImageRect,inMask,outXChange,outYChange);
	
	// Encode the picture
	CBLEncodedImageStream16	stream(FRectWidth(inImageRect),FRectHeight(inImageRect));
	
	stream.SetLength(stream.GetLength()+FRectWidth(inImageRect)*FRectHeight(inImageRect)*6);
	
	for (SInt16 y=inImageRect.top,my=outYChange; y<inImageRect.bottom; y++,my++)
	{
		for (SInt16 x=inImageRect.left,mx=outXChange; x<inImageRect.right; x++,mx++)
		{
			sourceColour=inImage.GetPixel16(x,y);
			stream.Add(inMask->ApplyMask(sourceColour,mx,my),sourceColour);
		}

		stream.Add(kSkipRows);		
	}
	
	stream.Add(kEndImage);
	
	stream.SetLength(stream.GetMarker());
	
	return (TBLFlattenedImage16H)stream.DetachDataHandle();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadImage													/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Loads the image from the specified resource id. Returns nil if res not found, throws on other error.
TBLFlattenedImage16H UBLEncodedImage16::LoadImage(
	CResourceContainer	*inResContainer,
	TResId				inResId)
{
	TBLFlattenedImage16H				imageH=0L;

	Try_
	{
		if (inResContainer->ResourceExists(kBLEncodedImage16ResType,inResId))
		{
			StTNTResource					res(inResContainer,kBLEncodedImage16ResType,inResId);
			Handle							dupe=res->GetReadOnlyDataHandle();

			ThrowIfOSErr_(::HandToHand(&dupe));
			imageH=(TBLFlattenedImage16H)dupe;
		}
	}
	Catch_(err)
	{
		if (ErrCode_(err)!=resNotFound)
		{
			throw;
		}
	}

	return imageH;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SaveImage													/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Saves the image to the specified resource id. This is used to cache encoded images so that they can be retrieved by
// the LoadImage function later. The handle is copied, so the caller is still responsible for disposing of it.
// If the resource exists it is overwritten.
void UBLEncodedImage16::SaveImage(
	CResourceContainer		*inResContainer,
	TBLFlattenedImage16H	inDataHandle,
	TResId					inResId)
{
	if (inResContainer->ResourceExists(kBLEncodedImage16ResType,inResId))
	{
		inResContainer->DeleteResource(kBLEncodedImage16ResType,inResId);
	}
	
	inResContainer->AddResource(kBLEncodedImage16ResType,inResId,"\p",(Handle)inDataHandle,false);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShrinkToContents													Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Shrinks the image to it's smallest possible size by removing the blank space, this makes it quicker to draw.
void UBLEncodedImage16::ShrinkToContents(
	CBLCanvas			&inImage,
	Rect				&ioImageRect,
	CBLMask16			*inMask,
	SInt16				&outXChange,
	SInt16				&outYChange)
{
	SInt16		topCut=0,leftCut=0,bottomCut=0,rightCut=0;

	outXChange=0;
	outYChange=0;

	if (inMask)
	{
		UInt16						sourceColour;
		SInt16						tempX,tempY;
		
		// Clip the top
		tempX=0; tempY=0; sourceColour=inImage.GetPixel16(tempX,tempY);
		
		while (inMask->ApplyMask(sourceColour,tempX,tempY)==CBLMask16::kTransp100)
		{
			tempX++;
		
			if (tempX==FRectWidth(ioImageRect))
			{
				tempX=0;
				tempY++;
				topCut++;
				
				if (tempY==FRectHeight(ioImageRect))
				{
					// Set the new rect
					ioImageRect.top+=topCut;
					ioImageRect.left+=leftCut;
					ioImageRect.bottom-=bottomCut;
					ioImageRect.right-=rightCut;
		
					outXChange=leftCut/2;
					outYChange=topCut/2;
		
					return;
				}
			}
			
			sourceColour=inImage.GetPixel16(tempX,tempY);
		}
		
		// Clip the left
		tempX=0; tempY=0; sourceColour=inImage.GetPixel16(tempX,tempY);
		
		while (inMask->ApplyMask(sourceColour,tempX,tempY)==CBLMask16::kTransp100)
		{
			tempY++;
			
			if (tempY==FRectHeight(ioImageRect))
			{
				tempY=0;
				tempX++;
				leftCut++;
			}
			
			sourceColour=inImage.GetPixel16(tempX,tempY);
		}
		
		// Clip the bottom
		tempX=0; tempY=FRectHeight(ioImageRect)-1; sourceColour=inImage.GetPixel16(tempX,tempY);
		
		while (inMask->ApplyMask(sourceColour,tempX,tempY)==CBLMask16::kTransp100)
		{
			tempX++;
			
			if (tempX==FRectWidth(ioImageRect))
			{
				tempX=0;
				tempY--;
				bottomCut++;
			}
			
			sourceColour=inImage.GetPixel16(tempX,tempY);
		}
		
		// Clip the right
		tempX=FRectWidth(ioImageRect)-1; tempY=0; sourceColour=inImage.GetPixel16(tempX,tempY);
		
		while (inMask->ApplyMask(sourceColour,tempX,tempY)==CBLMask16::kTransp100)
		{
			tempY++;
			
			if (tempY==FRectHeight(ioImageRect))
			{
				tempY=0;
				tempX--;
				rightCut++;
			}
			
			sourceColour=inImage.GetPixel16(tempX,tempY);
		}
		
		// Set the new rect
		ioImageRect.top+=topCut;
		ioImageRect.left+=leftCut;
		ioImageRect.bottom-=bottomCut;
		ioImageRect.right-=rightCut;
		
		// Change the hot-spot
		outXChange=leftCut;
		outYChange=topCut;
	}
}
