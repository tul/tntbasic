// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CTBSprite.h
// © Mark Tully 2000
// 1/1/00
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

#pragma once

#include	"CGraphicsContext.h"
#include	"CImageBank.h"
#include	"CBLImageSprite.h"

class CTBSprite : public CBLImageSprite
{
	protected:
		typedef CBLImageSprite		inheritedSprite;
		
		TTBInteger					mCanvas;
		TTBInteger					mPriority;
		CImageBank					*mBank;
		
		static CGraphicsContext		*sContext;

	public:
		CBLMoanChannel				mLocalMoanChannel;		// used for quick an easy move/animate commands
		CSharedListElementT<CTBSprite>	mPublicLink;
		
								CTBSprite() : mPublicLink(this)
								{
									mCanvas=0;
									mPriority=0;
									mBank=sContext->mSpriteManager.GetDefaultSpriteBank();
								}

//		virtual void			SetX(float inX)=0;
//		virtual void			SetY(float inY)=0;
		
		bool					SetCanvas(TTBInteger inCanvas)
								{
									if (mCanvas!=inCanvas)
									{									
										sContext->RemoveSpriteFromCanvas(this,mCanvas);
										
										mCanvas=inCanvas;
										
										sContext->AddSpriteToCanvas(this,inCanvas);
										
										return true;
									}
									
									return false;
								}
		
		TTBInteger				GetCanvas()						{ return mCanvas; }

		void					SetBank(TTBInteger inBankId)
								{
									mBank=sContext->mSpriteManager.GetBank(inBankId);
								}
								
		void					SetBank(CImageBank *inBank)
								{
									mBank=inBank;
								}
								
		CImageBank				*GetBank()						{ return mBank; }
		TTBInteger				GetImageIndex();
		void					SetImageByIndex(
									CProgram		&ioState,
									TTBInteger		inIndex);

		TTBInteger				GetPriority()					{ return mPriority; }
		
		bool					SetPriority(
									TTBInteger	inIndex)
								{
									if (mPriority!=inIndex)
									{
										mPriority=inIndex;
										return true;
									}
										
									return false;
								}

		static void				SetContext(CGraphicsContext* inContext)		{ sContext=inContext; }

		virtual void			SetSpriteImage(
									CSpriteImage	*inImage)=0;		
		virtual CSpriteImage	*GetSpriteImage()=0;
		
		
		// Opacity									
		virtual void			SetOpacity(float inOpacity)=0;
		virtual float			GetOpacity()=0;
		
		virtual void			DisableColour()=0;
		virtual void			SetColour(
			   						RGBColour		&inColour)=0;
									
		virtual void			SetVisibility(
									bool			inValue)=0;
									
		// Collision
		virtual bool			RectCollision(
									Rect			&inRect)=0;						
		virtual bool			SpriteCollision(
									CTBSprite		*inSprite)=0;
									
		// Angle
		virtual inline float	GetAngle()									{ return 0; }
		virtual void			SetAngle(float inAngle)						{ }
		
		// Scaling
		virtual inline float	GetXScale()									{ return 1; }
		virtual inline float	GetYScale()									{ return 1; }
		virtual void			SetXScale(float inScale)					{ }
		virtual void			SetYScale(float inScale)					{ }
		
		// Flipping
		virtual inline bool		GetXFlip()									{ return false; }
		virtual inline bool		GetYFlip()									{ return false; }
		virtual void			SetXFlip(bool inValue)						{ }
		virtual void			SetYFlip(bool inValue)						{ }
		
		// Cropping
		virtual void			Crop(Rect &inRect)							{ }
		virtual void			CropOff()									{ }
		
		virtual void			MoanSprite();	
};