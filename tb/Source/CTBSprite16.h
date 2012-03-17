// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CTBSprite16.h
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

#include	"CTBSprite.h"

class CTBSprite16 : public CTBSprite
{
	
	
	public:
//		virtual void			SetX(float inX)				{ inheritedSprite::SetX(inX); }
//		virtual void			SetY(float inY)				{ inheritedSprite::SetY(inY); }
		
		virtual inline void		SetOpacity(float inOpacity)	{ inheritedSprite::SetOpacity(inOpacity); }
		virtual inline float	GetOpacity()				{ return ((float)inheritedSprite::GetOpacity())/100; }
		
		virtual void			DisableColour()				{ inheritedSprite::DisableColour(); }
		virtual void			SetColour(
			   						RGBColour		&inColour)
			   					{
			   						inheritedSprite::SetColour(inColour);
			   					}

		virtual CSpriteImage	*GetSpriteImage()				{ return inheritedSprite::GetBLSpriteImage(); }
		
		inline virtual void		SetSpriteImage(
									CSpriteImage	*inImage)
								{
									inheritedSprite::SetBLSpriteImage((CBLSpriteImage*)inImage); 
								}
								
		virtual bool			RectCollision(
									Rect			&inRect)
								{
									return inheritedSprite::RectCollision(inRect);
								}
								
		virtual bool			SpriteCollision(
									CTBSprite		*inSprite)
								{
									return inheritedSprite::SpriteCollision(*(CBLImageSprite*)inSprite);	
								}
		
		virtual void			SetVisibility(bool inValue)		{ inheritedSprite::SetVisibility(inValue); }
};