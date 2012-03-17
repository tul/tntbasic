/***************************************************************************************************

BLAST Sprite Layer.h
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

#include		"BLAST.h"
#include		"Linked Lists Lib.h"
#include		"BLAST Sprite Base.h"
#include		"BLAST Draw Buffer Base.h"


// ********* Blast Sprite layer *********
class CBLSpriteLayer
{
	private:
		void						*mRefCon;

	protected:
		TLinkedList					mSpritesList;
		Boolean						mLayerHidden,mClipRectUsesOrigin,mMasterRecalc;
		Rect						mSpriteClipRect;
		long						mXOrigin,mYOrigin;

		// ********* RefCon access *********
		void						SetRefCon(void *refCon);
		void						*GetRefCon();

	public:
		// ********* Contructor/Destructor *********
									CBLSpriteLayer();
		virtual						~CBLSpriteLayer();
		
		// ********* Layer management routines *********
		virtual void				SetLayerVisability(Boolean on);
		virtual Boolean				GetLayerVisability();
		virtual void				SetLayerClipRect(const Rect &rect);
		virtual void				GetLayerClipRect(Rect &rect);
		
		// ********* Layer origin routines *********
		virtual void				SetClipRectUsesOrigin(Boolean on);
		virtual Boolean				GetClipRectUsesOrigin();
		virtual long				GetXOrigin();
		virtual long				GetYOrigin();
		virtual void				SetXOrigin(long x);
		virtual void				SetYOrigin(long y);
		virtual void				GetOrigin(long &x,long &y);
		virtual void				SetOrigin(long x,long y);
		virtual void				RelSetXOrigin(long xChange);
		virtual void				RelSetYOrigin(long yChange);
		virtual void				RelSetOrigin(long xChange,long yChange);
		
		TLinkedList					&GetSpriteList()				{ return mSpritesList; }
		
		bool						GetMasterRecalc()				{ return mMasterRecalc; }
		
		// ********* Sprite management routines *********
		virtual short				CountSprites();
		virtual Boolean				AddSpriteToLayer(
										BlastSpritePtr	addMe,
										TListIndex		inPosition=-1);
		virtual Boolean				AddSpriteToLayer(
										BlastSpritePtr	inAddMe,
										BlastSpritePtr	inRelativeSprite,
										bool			inOnTopOfRelativeSprite);
		virtual void				RemoveSpriteFromLayer(
										BlastSpritePtr	removeMe);

		// ********* Sprite useage routines *********
		virtual void				CalcAllSprites(BlastDrawBufferPtr invalBuffer);
		virtual void				RenderAllSprites(BlastDrawBufferPtr drawBuffer,Boolean invalAsRender);
		virtual void				MoanAllSprites();
		virtual bool				WillRender();

};

// for compatibility
typedef CBLSpriteLayer BlastSpriteLayer,*BlastSpriteLayerPtr,**BlastSpriteLayerHandle;
