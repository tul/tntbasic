// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CTBSpriteGL.h
// © John Treece-Birch 2002
// 21/2/02
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

#include	"CTBSprite.h"
#include	"CGLImageBank.h"

class CViewport;

class CTBSpriteGL : public CTBSprite
{
	private:
		float						mOpacity;
		
		bool						mValidColour;
		float						mRed,mGreen,mBlue;
		
		float						mAngle;
		float						mXScale,mYScale;
		bool						mXFlipped,mYFlipped;
		
		bool						mCropped;
		Rect						mCropRect;
		
		CViewport					*mViewport;
		
		CGLSpriteImage				*mSpriteImage;

		CListElementT<CTBSpriteGL>	mGLSpriteLink;

	public:
								CTBSpriteGL() :
									mGLSpriteLink(this)
								{
									mOpacity=1;
									
									mValidColour=false;
									mRed=0;
									mGreen=0;
									mBlue=0;
									
									mAngle=0.0f;
									mXScale=1.0f;
									mYScale=1.0f;
									
									mXFlipped=false;
									mYFlipped=false;
									
									mCropped=false;
									
									mSpriteImage=NULL;
									
									mViewport=NULL;
								}
		
		CListElementT<CTBSpriteGL>*		GetGLLink()					{ return &mGLSpriteLink; }
		
		void					Render(
									int			inScreenWidth,
									int			inScreenHeight,
									Rect		&inBounds,
									float		inOffsetX,
									float		inOffsetY);
		
		inline float			GetWidthPercent()					{ return mSpriteImage->GetWidthPercent(); }
		inline float			GetHeightPercent()					{ return mSpriteImage->GetHeightPercent(); }
		
		virtual inline void		SetSpriteImage(
									CSpriteImage	*inImage)
								{
									mSpriteImage=(CGLSpriteImage*)inImage;
								}
		virtual CSpriteImage	*GetSpriteImage()						{ return mSpriteImage; }
		
		inline bool				Valid()										{ return (mSpriteImage==0) ? false : true; }
		
		// Hotspot
		inline SInt16			GetXOffset()								{ return mSpriteImage->GetXOffset(); }
		inline SInt16			GetYOffset()								{ return mSpriteImage->GetYOffset(); }
		
		// Opacity									
		inline void				SetOpacity(float inOpacity)					{ mOpacity=inOpacity/100.0f; }
		inline float			GetOpacity()								{ return mOpacity; }
		
		// Colour
		inline bool				IsColoured()								{ return mValidColour; }
		inline void				DisableColour()								{ mValidColour=false; }
		inline float			GetRed()									{ return mRed; }
		inline float			GetGreen()									{ return mGreen; }
		inline float			GetBlue()									{ return mBlue; }
		virtual void			SetColour(
			   						RGBColour &inColour)
			   					{
			   						mRed=(float)inColour.red/65536;
			   						mGreen=(float)inColour.green/65536;
			   						mBlue=(float)inColour.blue/65536;
			   						mValidColour=true;
			   					}
		
		// Angle
		virtual inline float	GetAngle()									{ return mAngle; }
		virtual void			SetAngle(float inAngle)						{ mAngle=inAngle; }
		
		// Flipping
		virtual inline bool		GetXFlip()									{ return mXFlipped; }
		virtual inline bool		GetYFlip()									{ return mYFlipped; }
		virtual void			SetXFlip(bool inValue)						{ mXFlipped=inValue; }
		virtual void			SetYFlip(bool inValue)						{ mYFlipped=inValue; }
		
		// Scaling
		virtual inline float	GetXScale()									{ return mXScale; }
		virtual inline float	GetYScale()									{ return mYScale; }
		virtual void			SetXScale(float inScale)					{ mXScale=inScale; }
		virtual void			SetYScale(float inScale)					{ mYScale=inScale; }
		
		SInt32					GetTextureNumber()							{ return mSpriteImage->GetTextureNumber(); }
		
		virtual void			SetVisibility(
									bool			inValue)				{ }
									
		virtual bool			RectCollision(
									Rect			&inRect)
								{
									if (Valid())
									{
										Rect	spriteRect;
										
										CalculateCollisionRect(spriteRect);
										
										if ((spriteRect.top<inRect.bottom) &&
											(spriteRect.bottom>inRect.top) &&
											(spriteRect.left<inRect.right) &&
											(spriteRect.right>inRect.left))
										{
											return true;
										}
									}
									
									return false;
								}
								
		virtual bool			SpriteCollision(
									CTBSprite		*inSprite);
		
		virtual void			CropOff()					{ mCropped=false; }
		
		virtual void			Crop(
									Rect	&inCropRect)
								{
									mCropped=true;
									mCropRect=inCropRect;
								}
		
		void					CalculateCollisionRect(
									Rect	&outRect);
		
		void					CalculateRect(
									Rect	&outRect)
								{
									outRect.top=GetY()-mSpriteImage->GetYOffset();
									outRect.left=GetX()-mSpriteImage->GetXOffset();
									outRect.bottom=outRect.top+mSpriteImage->GetHeight();
									outRect.right=outRect.left+mSpriteImage->GetWidth();
								}
		
		void					CalculateRenderCoords(
									float	&outTop,
									float	&outLeft,
									float	&outBottom,
									float	&outRight)
								{
									if (mXFlipped)
										outLeft=mSpriteImage->GetXOffset()-mSpriteImage->GetWidth();
									else
										outLeft=-mSpriteImage->GetXOffset();
									
									if (mYFlipped)
										outTop=mSpriteImage->GetYOffset()+mSpriteImage->GetHeight();
									else
										outTop=-mSpriteImage->GetYOffset();
										
									outBottom=outTop+mSpriteImage->GetHeight();
									outRight=outLeft+mSpriteImage->GetWidth();
								}
								
		void					SetViewport(CViewport *inViewport)		{ mViewport=inViewport; }
};