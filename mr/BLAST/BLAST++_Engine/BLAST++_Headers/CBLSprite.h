/***************************************************************************************************

CBLSprite.h
Header file for the BLAST++ª engine
© TNT Software 1997
All right reserved
Mark Tully
27/8/97

***************************************************************************************************/

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1997, Mark Tully and John Treece-Birch
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

#include		"BLAST_Types.h"
#include		"BLAST.h"
#include		"Linked Lists Lib.h"
#include		"CBLCollisionMask.h"

class			CBLSpriteLayer;
class			CBLCanvas;

// ********* Blast Sprite *********
// This sprite is a flexible class which you can use to manage any sprite imaginable. You can
// use the sprite with or without a sprite layer.
class CBLSprite
{
	private:
		void						*mRefCon;
		
	protected:
		Boolean						mNeedsRecalc;
		bool						mSpriteVisible;
		short						mX,mY;
		Rect						mSpriteRect;
		CBLSpriteLayer				*mLayer;

		// ********* RefCon access *********
		void						SetRefCon(void *refCon);
		void						*GetRefCon();

	public:
		// ********* Misc *********
		TListElement				mLink;
		signed char					mNeedsClip;

		// ********* Constructor/Destructor *********
									CBLSprite();
		virtual						~CBLSprite();
		virtual void				ResetSprite();

		// ********* Visibility access *********
		inline bool					IsVisible()								{ return mSpriteVisible; }
		virtual void				SetVisibility(
										bool	inState);
		
		// ********* Position setters *********
		virtual void				SetX(signed long x);
		virtual void				SetY(signed long y);
		virtual void				RelSetX(signed long x);
		virtual void				RelSetY(signed long y);
		virtual void				SetPos(signed long x,signed long y);
		virtual void				RelPos(signed long x,signed long y);
		
		void						SetLayer(CBLSpriteLayer *inLayer)		{ mLayer=inLayer; }
		CBLSpriteLayer				*GetLayer()								{ return mLayer; }
		
		// ********* Position getters *********
		virtual signed long			GetX();
		virtual signed long			GetY();
		virtual void				GetPos(signed long &x,signed long&y);		
		virtual void				GetSpriteRect(Rect &spriteRect);

		// ********* Sprite routines *********
		virtual void				RenderSprite(
										TBLImageVector		inXOff,
										TBLImageVector		inYOff,
										CBLCanvas			&inCanvas);
		virtual void				RenderSpriteClipped(
										TBLImageVector		inXOff,
										TBLImageVector		inYOff,
										CBLCanvas			&inCanvas,
										const Rect			&inClipRect);
										
		virtual void				RenderSprite(long xOff,long yOff,ABCPtr rec);
		virtual void				RenderSpriteClipped(long xOff,long yOff,ABCPtr rec,Rect &clipRect);
		virtual void				RecalcSprite(long xOff,long yOff,Rect &clipRect,Boolean masterRecalc);
		virtual void				MoanSprite();

};

typedef CBLSprite BlastSprite, *BlastSpritePtr, **BlastSpriteHandle;

// ********* Blast scalable sprite ingredient *********
// Derive both from CBLSprite and from this to give your sprite scaling abilities

typedef Fixed BlastFixed;

class CBLSpriteScalableIngredient
{
	protected:
		BlastFixed				mXScale,mYScale;
	
	public:
		// ********* Scale readers *********
		void					GetScale(BlastFixed &xScale,BlastFixed &yScale);
		BlastFixed				GetXScale();
		BlastFixed				GetYScale();
		
		// ********* Scale setters *********
		void					SetScale(BlastFixed xScale,BlastFixed yScale);
		void					SetXScale(BlastFixed xScale);
		void					SetYScale(BlastFixed yScale);
		void					SetScale(BlastFixed scale);
		
		// ********* Scale calculations *********
		// Include BLAST.h and use the scale calculation macros in that
		
};

typedef CBLSpriteScalableIngredient BlastSpriteScalableIngredient;

// ********* Blast sprite tint ingredient *********
// Add this to the derived mix to get tintable qualities. The static tint is there as you often
// don't need multiple tints on a scene. Of couse if you do, there's nothing to stop you either
// writing your own tint ingredient or simply creating an array of these ingredients. Both
// would be about the smae speed and take the same ram.

class CBLSpriteTintIngredient
{
	protected:
		TintHandle					mLocalTint;
		static TintHandle			mGlobalTint;

	public:
		// ********* Constructor *********
									CBLSpriteTintIngredient();	
									
		// ********* Tint accessors *********
		virtual void				SetLocalTint(TintHandle tint);
		virtual void				SetGlobalTint(TintHandle tint);
		virtual TintHandle			GetGlobalTint();
		virtual TintHandle			GetLocalTint();
		virtual TintHandle			GetTint();

};

typedef CBLSpriteTintIngredient BlastSpriteTintIngredient;

class CBLSpriteOpacityIngredient
{
	protected:
		float						mOpacity;
	
	public:
									CBLSpriteOpacityIngredient()			{ mOpacity=100; }
		virtual						~CBLSpriteOpacityIngredient()			{}
									
		virtual void				SetOpacity(float inOpacity)				{ mOpacity=inOpacity; }
		virtual float				GetOpacity()							{ return mOpacity; }
};

typedef CBLSpriteOpacityIngredient CBlastSpriteOpacityIngredient;

class CBLSpriteColourIngredient
{
	protected:
		bool						mValidColour;
		BlastColour16				mColour;
	
	public:
									CBLSpriteColourIngredient()
									{
										mValidColour=false;
										mColour=0;
									}
		virtual						~CBLSpriteColourIngredient()				{}
		
		virtual void				SetColour(RGBColour &inColour)				{ mColour=RGBColourToBlastColour16(&inColour); mValidColour=true; }
		virtual void				DisableColour()								{ mValidColour=false; }
};

typedef CBLSpriteColourIngredient CBlastSpriteColourIngredient;