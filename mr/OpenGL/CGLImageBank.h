// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CGLImageBank.h
// © John Treece-Birch 2002
// 5/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

#pragma once

#include		"CImageBank.h"
#include		"ResourceTypes.h"

class CGLCanvas;

class CGLSpriteImage : public CSpriteImage
{
	private:
		SInt32			mTextureId;
		SInt32			mWidth,mHeight;
		float			mWidthPercent,mHeightPercent;
		SInt32			mHotspotX,mHotspotY;
	
	public:
						CGLSpriteImage(
							SInt32	inTextureId,
							SInt32	inWidth,
							SInt32	inHeight,
							float	inWidthPercent,
							float	inHeightPercent,
							SInt32	inHotspotX,
							SInt32	inHotspotY)
						{
							mTextureId=inTextureId;
							
							mWidth=inWidth;
							mHeight=inHeight;
							
							mWidthPercent=inWidthPercent;
							mHeightPercent=inHeightPercent;
							
							mHotspotX=inHotspotX;
							mHotspotY=inHotspotY;
						}
						
						~CGLSpriteImage();
	
		virtual void	GetImageSize(
							SInt32		&outWidth,
							SInt32		&outHeight)
						{
							outWidth=mWidth;
							outHeight=mHeight;
						}
		
		virtual void	GetImageOffset(
							SInt32		&outX,
							SInt32		&outY)
						{
							outX=mHotspotX;
							outY=mHotspotY;
						}
									
		SInt32			GetWidth()								{ return mWidth; }
		SInt32			GetHeight()								{ return mHeight; }
	
		float			GetWidthPercent()						{ return mWidthPercent; }
		float			GetHeightPercent()						{ return mHeightPercent; }
		
		SInt32			GetXOffset()							{ return mHotspotX; }
		SInt32			GetYOffset()							{ return mHotspotY; }
						
		void			SetImageOffset(
							SInt32		inHotspotX,
							SInt32		inHotspotY)
						{
							mHotspotX=inHotspotX;
							mHotspotY=inHotspotY;
						}
						
		SInt16			GetCollisionTolerance()					{ return 0; }
		bool			GetCollisionType()						{ return false; }
		void			CreateCollisionMask(
							SInt16		inTolerance)			{ }
		void			DeleteCollisionMask()					{ }
		
		SInt32			GetTextureNumber()						{ return mTextureId; }
};

class CGLImageBank : public CImageBank
{
	protected:
		static void					ShrinkToContents(
											CGLCanvas		*inCanvas,
											CGLCanvas		*inMaskCanvas,
											Rect			&ioRect,
											RGBColor		&inMaskColour,
											SInt16			&ioXOff,
											SInt16			&ioYOff);

	public:
		virtual							~CGLImageBank()					{ ClearBank(); }
		
		virtual void					ClearBank();

		virtual void					DeleteImage(
											ArrayIndexT		inImageIndex);
		
		virtual void /*e*/				Load(
											CResourceContainer	*inResC,
											TResId				inResId,
											CResourceContainer	*outCacheContainer=NULL);
											
		virtual void /*e*/				LoadData(
											Handle			inHandle);
											
		static CGLSpriteImage*			EncodeImage(
											CGLCanvas		*inCanvas,
											CGLCanvas		*inMaskCanvas,
											Rect			&inRect,
											Rect			&inMaskRect,
											RGBColor		&inMaskColour,
											SInt16			inXOff,
											SInt16			inYOff,
											SImageFlags		inFlags);
};
