// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLImageSprite.cpp
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
	Uses CBLSpriteImages to draw to the CBLCanvas. Doesn't implement drawing to ABCRecs, only canvases.
*/

#include		"CBLImageSprite.h"
#include		"CBLSpriteImage.h"

#include		"CBLCanvas.h"
#include		"CBLEncodedImage16.h"
#include		"UBLEncodedImage16.h"
#include		"UBLEncodedImageFX16.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetBLSpriteImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Changes the sprite image to the one specified. Can be nil.
void CBLImageSprite::SetBLSpriteImage(
	CBLSpriteImage			*inImage)
{
	if (inImage!=mSpriteImage)
	{
		mSpriteImage=inImage;
		mNeedsRecalc=true;
		if (mSpriteImage)
			mImageChangeIndex=mSpriteImage->GetChangeIndex();
		else
			mImageChangeIndex=0;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RecalcSprite
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// work out the sprites rect
// if too big, clip then set the clip flag
void CBLImageSprite::RecalcSprite(
	TBLImageVector	inXOff,
	TBLImageVector	inYOff,
	Rect			&inClipRect,
	Boolean			inMasterRecalc)
{
	if (mNeedsRecalc || inMasterRecalc || (mSpriteImage && (mSpriteImage->GetChangeIndex()!=mImageChangeIndex)))
	{
		mNeedsRecalc=false;
		if (mSpriteImage)
		{
			mImageChangeIndex=mSpriteImage->GetChangeIndex();
			mSpriteImage->GetImageFrame(mX-inXOff,mY-inYOff,mSpriteRect);
			
			// does it need clipping?
			mNeedsClip=NewClipRect(&mSpriteRect,&inClipRect);
		}
		else
		{
			FSetRect(mSpriteRect,0,0,0,0);
			mNeedsClip=csClippedOff;
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderSprite
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLImageSprite::RenderSprite(
	TBLImageVector 	inXOff,
	TBLImageVector	inYOff,
	CBLCanvas		&inRec)
{
	if (mSpriteImage)
	{
		if (mValidColour)
		{
			// Draw the sprite as one solid colour
			gSpriteColour=mColour;
			
			if (mOpacity>97)
				UBLEncodedImageFX16::DecodeColourNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>90)
				UBLEncodedImageFX16::DecodeColour93NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>82)
				UBLEncodedImageFX16::DecodeColour87NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>69)
				UBLEncodedImageFX16::DecodeColour75NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>57)
				UBLEncodedImageFX16::DecodeColour63NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>44)
				UBLEncodedImageFX16::DecodeColour50NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>32)
				UBLEncodedImageFX16::DecodeColour38NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>19)
				UBLEncodedImageFX16::DecodeColour25NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>9)
				UBLEncodedImageFX16::DecodeColour12NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>3)
				UBLEncodedImageFX16::DecodeColour6NoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
		}
		else
		{
			// Draw the sprite normally
			if (mOpacity>97)
				UBLEncodedImage16::DecodeImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>90)
				UBLEncodedImageFX16::Decode93ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>82)
				UBLEncodedImageFX16::Decode87ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>69)
				UBLEncodedImageFX16::Decode75ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>57)
				UBLEncodedImageFX16::Decode63ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>44)
				UBLEncodedImageFX16::Decode50ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>32)
				UBLEncodedImageFX16::Decode38ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>19)
				UBLEncodedImageFX16::Decode25ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>9)
				UBLEncodedImageFX16::Decode12ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
			else if (mOpacity>3)
				UBLEncodedImageFX16::Decode6ImageNoClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderSpriteClipped
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBLImageSprite::RenderSpriteClipped(
	TBLImageVector	inXOff,
	TBLImageVector	inYOff,
	CBLCanvas		&inRec,
	const Rect		&inClipRect)
{
	if (mSpriteImage)
	{
		if (mValidColour)
		{
			// Draw the sprite as one solid colour
			gSpriteColour=mColour;
			
			if (mOpacity>97)
				UBLEncodedImageFX16::DecodeColourClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>90)
				UBLEncodedImageFX16::DecodeColour93Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>82)
				UBLEncodedImageFX16::DecodeColour87Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>69)
				UBLEncodedImageFX16::DecodeColour75Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>57)
				UBLEncodedImageFX16::DecodeColour63Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>44)
				UBLEncodedImageFX16::DecodeColour50Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>32)
				UBLEncodedImageFX16::DecodeColour38Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>19)
				UBLEncodedImageFX16::DecodeColour25Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>9)
				UBLEncodedImageFX16::DecodeColour12Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>3)
				UBLEncodedImageFX16::DecodeColour6Clip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
		}
		else
		{
			if (mOpacity>97)
				UBLEncodedImage16::DecodeImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>90)
				UBLEncodedImageFX16::Decode93ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>82)
				UBLEncodedImageFX16::Decode87ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>69)
				UBLEncodedImageFX16::Decode75ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>57)
				UBLEncodedImageFX16::Decode63ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>44)
				UBLEncodedImageFX16::Decode50ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>32)
				UBLEncodedImageFX16::Decode38ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>19)
				UBLEncodedImageFX16::Decode25ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>9)
				UBLEncodedImageFX16::Decode12ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
			else if (mOpacity>3)
				UBLEncodedImageFX16::Decode6ImageClip16(*((CBLEncodedImage16*)mSpriteImage)->GetEncodedData(),&inRec.GetABCRec(),mX-inXOff,mY-inYOff,inClipRect);
		}
	}
}

bool CBLImageSprite::RectCollision(Rect &inRect)
{
	Rect				rect,newRect,clipRect;

	// Calculate the sprite rect
	if (!mLayer)
		return false;
	
	mLayer->GetLayerClipRect(clipRect);
	if (mLayer->GetClipRectUsesOrigin())
		FOffset(clipRect,mLayer->GetXOrigin(),mLayer->GetYOrigin());
	
	RecalcSprite(mLayer->GetXOrigin(),mLayer->GetYOrigin(),clipRect,mLayer->GetMasterRecalc());
	if (mNeedsClip==csClippedOff)
		return false;
	
	if (mSpriteImage)
		mSpriteImage->GetImageFrame(mX,mY,rect);
	
	// If the rects do not touch then return
	if ((rect.top>=inRect.bottom) || (rect.bottom<=inRect.top) || (rect.left>=inRect.right) || (rect.right<=inRect.left))
		return false;

	if (!BitmapCollisions())
		return true;
	
	// Make the rect as small as possible to save time later
	newRect.top=Greater(rect.top,inRect.top);
	newRect.left=Greater(rect.left,inRect.left);
	newRect.bottom=Lesser(rect.bottom,inRect.bottom);
	newRect.right=Lesser(rect.right,inRect.right);
	
	// Move the rect to be part of the collision mask
	newRect.top-=rect.top;
	newRect.left-=rect.left;
	newRect.bottom-=rect.top;
	newRect.right-=rect.left;
	
	return mSpriteImage ? mSpriteImage->RectCollision(newRect) : false;
}

bool CBLImageSprite::SpriteCollision(CBLImageSprite &inOther)
{
	Rect		newRect,rectA,rectB,newRectA,newRectB,clipRect;

	// Calculate the sprite rects
	if (!mLayer)
		return false;
	
	mLayer->GetLayerClipRect(clipRect);
	if (mLayer->GetClipRectUsesOrigin())
		FOffset(clipRect,mLayer->GetXOrigin(),mLayer->GetYOrigin());
	RecalcSprite(mLayer->GetXOrigin(),mLayer->GetYOrigin(),clipRect,mLayer->GetMasterRecalc());
	if (mNeedsClip==csClippedOff)
		return false;
	
	if (!inOther.GetLayer())
		return false;
	
	inOther.GetLayer()->GetLayerClipRect(clipRect);
	if (inOther.GetLayer()->GetClipRectUsesOrigin())
		FOffset(clipRect,inOther.GetLayer()->GetXOrigin(),inOther.GetLayer()->GetYOrigin());
	RecalcSprite(inOther.GetLayer()->GetXOrigin(),inOther.GetLayer()->GetYOrigin(),clipRect,inOther.GetLayer()->GetMasterRecalc());
	if (inOther.mNeedsClip==csClippedOff)
		return false;
	
	if (mSpriteImage)
		mSpriteImage->GetImageFrame(mX,mY,rectA);
	
	if (inOther.GetBLSpriteImage())
		inOther.GetBLSpriteImage()->GetImageFrame(inOther.GetX(),inOther.GetY(),rectB);
		
	//GetSpriteRect(rectA);
	//inOther.GetSpriteRect(rectB);
	
	// If the rects do not touch then return
	// NB: if rectA.top==rectB.bottom then the rects are not overlapping, same for left==right
	if ((rectA.top>=rectB.bottom) || (rectA.bottom<=rectB.top) || (rectA.left>=rectB.right) || (rectA.right<=rectB.left))
		return false;

	if ((!BitmapCollisions()) && (!inOther.BitmapCollisions()))
		return true;
	
	if ((BitmapCollisions()) && (!inOther.BitmapCollisions()))
	{	
		// Make the rect as small as possible to save time later
		newRect.top=Greater(rectA.top,rectB.top);
		newRect.left=Greater(rectA.left,rectB.left);
		newRect.bottom=Lesser(rectA.bottom,rectB.bottom);
		newRect.right=Lesser(rectA.right,rectB.right);
		
		// Move the rect to be part of the collision mask
		newRect.top-=rectA.top;
		newRect.left-=rectA.left;
		newRect.bottom-=rectA.top;
		newRect.right-=rectA.left;
	
		return mSpriteImage ? mSpriteImage->RectCollision(newRect) : false;
	}
	
	if ((!BitmapCollisions()) && (inOther.BitmapCollisions()))
	{
		// Make the rect as small as possible to save time later
		newRect.top=Greater(rectA.top,rectB.top);
		newRect.left=Greater(rectA.left,rectB.left);
		newRect.bottom=Lesser(rectA.bottom,rectB.bottom);
		newRect.right=Lesser(rectA.right,rectB.right);
		
		// Move the rect to be part of the collision mask
		newRect.top-=rectB.top;
		newRect.left-=rectB.left;
		newRect.bottom-=rectB.top;
		newRect.right-=rectB.left;
	
		return inOther.GetBLSpriteImage() ? inOther.GetBLSpriteImage()->RectCollision(newRect) : false;
	}
	
	// Make the rect as small as possible to save time later
	newRect.top=Greater(rectA.top,rectB.top);
	newRect.left=Greater(rectA.left,rectB.left);
	newRect.bottom=Lesser(rectA.bottom,rectB.bottom);
	newRect.right=Lesser(rectA.right,rectB.right);
	
	// Move the rects to be part of the collision mask
	newRectA.top=newRect.top-rectA.top;
	newRectA.left=newRect.left-rectA.left;
	newRectA.bottom=newRect.bottom-rectA.top;
	newRectA.right=newRect.right-rectA.left;
	
	newRectB.top=newRect.top-rectB.top;
	newRectB.left=newRect.left-rectB.left;
	newRectB.bottom=newRect.bottom-rectB.top;
	newRectB.right=newRect.right-rectB.left;
	
	return (mSpriteImage && inOther.GetBLSpriteImage()) ? mSpriteImage->SpriteCollision(*inOther.GetBLSpriteImage(),newRectA,newRectB) : false;
}