/***************************************************************************************************

BLAST Sprites Layer.cpp
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

#include	"BLAST Sprite Base.h"
#include	"BLAST Sprite Layer.h"
#include	"Useful Defines.h"

// ********* CBLSpriteLayer *********

CBLSpriteLayer::CBLSpriteLayer()
{
	mLayerHidden=false;
	FSetRect(mSpriteClipRect,0,0,0,0);
	mXOrigin=mYOrigin=0;
	mClipRectUsesOrigin=false;
	mMasterRecalc=true;
}

CBLSpriteLayer::~CBLSpriteLayer()
{
	TListIndexer		indexer(&mSpritesList);
		
	while (BlastSpritePtr temp=(BlastSpritePtr)indexer.GetNextData())
		RemoveSpriteFromLayer(temp);
}

short CBLSpriteLayer::CountSprites()
{
	return mSpritesList.CountElements();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddSpriteToLayer
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds a sprite to the layer at the index specified. Pass -1 for end of list and therefore topmost sprite
Boolean CBLSpriteLayer::AddSpriteToLayer(
	BlastSpritePtr	inAddMe,
	TListIndex		inIndex)
{
	if (inIndex==-1)
		mSpritesList.AppendElement(&inAddMe->mLink);
	else
		mSpritesList.InsertElement(&inAddMe->mLink,inIndex,false);
	inAddMe->SetLayer(this);
	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddSpriteToLayer
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds a sprite to the layer next to the sprite specified. If you pass a nil sprite and then pass true for ontop of it
// will be on top of all sprites. Pass false and nil sprite for behind all sprites
Boolean CBLSpriteLayer::AddSpriteToLayer(
	BlastSpritePtr	inAddMe,
	BlastSpritePtr	inRelativeSprite,
	bool			inOnTopOfRelativeSprite)
{
	mSpritesList.InsertElement(&inAddMe->mLink,inRelativeSprite ? &inRelativeSprite->mLink : 0,inOnTopOfRelativeSprite);
	inAddMe->SetLayer(this);
	return true;
}

void CBLSpriteLayer::RemoveSpriteFromLayer(BlastSpritePtr removeMe)
{
	removeMe->SetLayer(0L);
	mSpritesList.UnlinkElement(&removeMe->mLink);
}

// returns true if this layer will render something if render is called
bool CBLSpriteLayer::WillRender()
{
	if (mLayerHidden)
		return false;

	TListIndexer		listIndexer(&mSpritesList);
	BlastSpritePtr		temp;
		
	while (temp=(BlastSpritePtr)listIndexer.GetNextData())
	{
		if (temp->IsVisible())
			return true;
	}
	
	return false;
}

void CBLSpriteLayer::CalcAllSprites(BlastDrawBufferPtr invalBuffer)
{
	if (mLayerHidden)
		return;

	BlastSpritePtr		temp;
	Rect				spriteRect;
	TListIndexer		listIndexer(&mSpritesList);
		
	// if the clip rect is measured from the layer origin then move it
	if (mClipRectUsesOrigin)
		FOffset(mSpriteClipRect,mXOrigin,mYOrigin);
	
	while (temp=(BlastSpritePtr)listIndexer.GetNextData())
	{
		if (temp->IsVisible())
		{
			temp->RecalcSprite(mXOrigin,mYOrigin,mSpriteClipRect,mMasterRecalc);
			
			// only mark it as needing copying to screen if it's a) visable and b) we have an inval buffer
			if (invalBuffer && temp->mNeedsClip!=csClippedOff)
			{
				temp->GetSpriteRect(spriteRect);
				invalBuffer->InvalRect(spriteRect);
			}
		}
	}
	
	if (mClipRectUsesOrigin)
		FOffset(mSpriteClipRect,-mXOrigin,-mYOrigin);
	mMasterRecalc=false;
}

void CBLSpriteLayer::RenderAllSprites(BlastDrawBufferPtr drawBuffer,Boolean invalAsDraw)
{
	if (mLayerHidden)
		return;

	// if the clip rect is measured from the layer origin then move it
	if (mClipRectUsesOrigin)
		FOffset(mSpriteClipRect,mXOrigin,mYOrigin);

	BlastSpritePtr		temp;
	ABCPtr				rec=drawBuffer->GetSourceBuffer();
	CBLCanvas			*canvas=drawBuffer->GetSourceCanvas();
	Rect				spriteRect;
	TListIndexer		indexer(&mSpritesList);
		
	while (temp=(BlastSpritePtr)indexer.GetNextData())
	{
		if (!temp->IsVisible())
			continue;
	
		switch (temp->mNeedsClip)
		{
			case csClippedOff:
				break;
			
			case csClipped:
				if (canvas)
					temp->RenderSpriteClipped(mXOrigin,mYOrigin,*canvas,mSpriteClipRect);
				else
					temp->RenderSpriteClipped(mXOrigin,mYOrigin,rec,mSpriteClipRect);
				if (invalAsDraw)
				{
					temp->GetSpriteRect(spriteRect);
					drawBuffer->InvalRect(spriteRect);
				}
				break;
			
			case csNoChange:
				if (canvas)
					temp->RenderSprite(mXOrigin,mYOrigin,*canvas);
				else
					temp->RenderSprite(mXOrigin,mYOrigin,rec);
				if (invalAsDraw)
				{
					temp->GetSpriteRect(spriteRect);
					drawBuffer->InvalRect(spriteRect);
				}
				break;
		}
	}
	
	// restore the rect
	if (mClipRectUsesOrigin)
		FOffset(mSpriteClipRect,-mXOrigin,-mYOrigin);
}

void CBLSpriteLayer::MoanAllSprites()
{
	BlastSpritePtr		temp;
	TListIndexer		indexer(&mSpritesList);
		
	while (temp=(BlastSpritePtr)indexer.GetNextData())
		temp->MoanSprite();
}

void CBLSpriteLayer::SetLayerVisability(Boolean on)
{
	mLayerHidden=!on;
}

Boolean CBLSpriteLayer::GetLayerVisability()
{
	return !mLayerHidden;
}

void CBLSpriteLayer::SetLayerClipRect(const Rect &clipRect)
{
	mMasterRecalc=true;
	mSpriteClipRect=clipRect;
}

void CBLSpriteLayer::GetLayerClipRect(Rect &rect)
{
	rect=mSpriteClipRect;
}

void CBLSpriteLayer::SetClipRectUsesOrigin(Boolean on)
{
	mMasterRecalc=true;
	mClipRectUsesOrigin=on;
}

Boolean CBLSpriteLayer::GetClipRectUsesOrigin()
{
	return mClipRectUsesOrigin;
}

long CBLSpriteLayer::GetXOrigin()
{
	return mXOrigin;
}

long CBLSpriteLayer::GetYOrigin()
{
	return mYOrigin;
}

void CBLSpriteLayer::SetXOrigin(long x)
{
	mMasterRecalc=true;
	mXOrigin=x;
}

void CBLSpriteLayer::SetYOrigin(long y)
{
	mMasterRecalc=true;
	mYOrigin=y;
}

void CBLSpriteLayer::GetOrigin(long &x,long &y)
{
	x=mXOrigin;
	y=mYOrigin;
}

void CBLSpriteLayer::SetOrigin(long x,long y)
{
	mMasterRecalc=true;
	mXOrigin=x;
	mYOrigin=y;
}

void CBLSpriteLayer::RelSetXOrigin(long xChange)
{
	mMasterRecalc=true;
	mXOrigin+=xChange;
}

void CBLSpriteLayer::RelSetYOrigin(long yChange)
{
	mMasterRecalc=true;
	mYOrigin+=yChange;
}

void CBLSpriteLayer::RelSetOrigin(long xChange,long yChange)
{
	mMasterRecalc=true;
	mXOrigin+=xChange;
	mYOrigin+=yChange;
}

void CBLSpriteLayer::SetRefCon(void *refCon)
{
	mRefCon=refCon;
}

void *CBLSpriteLayer::GetRefCon()
{
	return mRefCon;
}
