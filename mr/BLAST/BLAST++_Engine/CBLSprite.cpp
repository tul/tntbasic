/***************************************************************************************************

CBLSprite.cpp
Extension of BLAST++ª engine
© TNT Software 1997
All right reserved
Mark Tully

27/8/97		:	Started it

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


#include	"CBLSprite.h"
#include	"Useful Defines.h"
#include	"CBLCanvas.h"

// ********* CBLSprite *********

CBLSprite::CBLSprite() :
	mLink(this)
{
	mRefCon=0L;
	ResetSprite();
	mLayer=0L;
}

void CBLSprite::ResetSprite()
{
	mNeedsRecalc=false;
	mX=mY=0;
	FSetRect(mSpriteRect,0,0,0,0);
	mSpriteVisible=true;
	mNeedsClip=csClippedOff;
}

CBLSprite::~CBLSprite()
{

}

void CBLSprite::SetVisibility(
	bool		inState)
{
	if (inState!=mSpriteVisible)
		mSpriteVisible=inState;
}

void CBLSprite::SetX(signed long inX)
{
	if (mX!=inX)
	{
		mX=inX;
		mNeedsRecalc=true;
	}
}

void CBLSprite::SetY(signed long inY)
{
	if (mY!=inY)
	{
		mY=inY;
		mNeedsRecalc=true;
	}
}

void CBLSprite::RelSetX(signed long inX)
{
	if (inX)
	{
		mX+=inX;		
		mNeedsRecalc=true;
	}
}

void CBLSprite::RelSetY(signed long inY)
{
	if (inY)
	{
		mY+=inY;		
		mNeedsRecalc=true;
	}
}

void CBLSprite::SetPos(signed long inX,signed long inY)
{
	if (mX!=inX || mY!=inY)
	{
		mX=inX;
		mY=inY;
		mNeedsRecalc=true;
	}
}

void CBLSprite::RelPos(signed long inX,signed long inY)
{
	if (inX || inY)
	{
		mX+=inX;
		mY+=inY;
		mNeedsRecalc=true;
	}
}

signed long CBLSprite::GetX()
{
	return mX;
}

signed long CBLSprite::GetY()
{
	return mY;
}

void CBLSprite::GetPos(signed long &outX,signed long &outY)
{
	outX=mX;
	outY=mY;
}

void CBLSprite::GetSpriteRect(Rect &outSpriteRect)
{
	outSpriteRect=mSpriteRect;
}

void CBLSprite::RenderSprite(
	TBLImageVector		inXOff,
	TBLImageVector		inYOff,
	CBLCanvas				&inCanvas)
{
	RenderSprite(inXOff,inYOff,&inCanvas.GetABCRec());
}

void  CBLSprite::RenderSpriteClipped(
	TBLImageVector		inXOff,
	TBLImageVector		inYOff,
	CBLCanvas				&inCanvas,
	const Rect			&inClipRect)
{
	RenderSpriteClipped(inXOff,inYOff,&inCanvas.GetABCRec(),(Rect&)inClipRect);
}

void CBLSprite::RenderSprite(long xOff,long yOff,ABCPtr rec)
{

}

void CBLSprite::RenderSpriteClipped(long xOff,long yOff,ABCPtr rec,Rect &clipRect)
{

}

void CBLSprite::RecalcSprite(long xOff,long yOff,Rect &clipRect,Boolean masterRecalc)
{

}

void CBLSprite::MoanSprite()
{

}

void CBLSprite::SetRefCon(void *inRefCon)
{
	mRefCon=inRefCon;
}

void *CBLSprite::GetRefCon()
{
	return mRefCon;
}

// ********* Blast Sprite Scalable Ingredient *********
		
void CBLSpriteScalableIngredient::GetScale(BlastFixed &outXScale,BlastFixed &outYScale)
{
	outXScale=mXScale;
	outYScale=mYScale;
}

BlastFixed CBLSpriteScalableIngredient::GetXScale()
{
	return mXScale;
}

BlastFixed CBLSpriteScalableIngredient::GetYScale()
{
	return mYScale;
}

void CBLSpriteScalableIngredient::SetScale(BlastFixed inScale)
{
	mXScale=mYScale=inScale;
}

void CBLSpriteScalableIngredient::SetScale(BlastFixed inXScale,BlastFixed inYScale)
{
	mXScale=inXScale;
	mYScale=inYScale;
}

void CBLSpriteScalableIngredient::SetXScale(BlastFixed inXScale)
{
	mXScale=inXScale;
}

void CBLSpriteScalableIngredient::SetYScale(BlastFixed inYScale)
{
	mYScale=inYScale;
}

// ********* Blast sprite tint ingredient *********

TintHandle	CBLSpriteTintIngredient::mGlobalTint=0L;

CBLSpriteTintIngredient::CBLSpriteTintIngredient()
{
	mLocalTint=0L;
}

void CBLSpriteTintIngredient::SetLocalTint(TintHandle inTint)
{
	mLocalTint=inTint;
}

void CBLSpriteTintIngredient::SetGlobalTint(TintHandle inTint)
{
	mGlobalTint=inTint;
}

TintHandle CBLSpriteTintIngredient::GetGlobalTint()
{
	return mGlobalTint;
}

TintHandle CBLSpriteTintIngredient::GetLocalTint()
{
	return mLocalTint;
}

TintHandle CBLSpriteTintIngredient::GetTint()
{
	return mLocalTint?mLocalTint:mGlobalTint;
}
