// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CViewport.h
// © John Treece-Birch 2000
// 20/7/00
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

#include						"CCanvasManager.h"
#include						"CListElementT.h"
#include						"CProgram.h"
#include						"CBLMoanChannel.h"

class CTBSprite;
class CTBSpriteGL;

class CViewport : public CListElementT<CViewport>
{
	protected:
		CProgram					*mProgram;
		TTBInteger					mCanvas,mXOffset,mYOffset;
		Rect						mViewportRect;
		bool						mUpdateAll;
		
		CLinkedListT<CTBSpriteGL> mSprites;
		
		virtual void /*e*/		VerifyNewOffsets(
									TTBInteger		inNewX,
									TTBInteger		inNewY);

	public:
		Rect						mBounds;
		CBLMoanChannel				mLocalMoanChannel;		// used for quick an easy move commands
		CSharedListElementT<CViewport>	mPublicLink;
		
								CViewport(
									CProgram		&inProgram,
									TTBInteger		inCanvas,
									const Rect		&inRect) : 
									CListElementT<CViewport>(this),
									mPublicLink(this)
								{
									mProgram=&inProgram;
									mCanvas=inCanvas;
									mBounds=inRect;
									
									mXOffset=0;
									mYOffset=0;
									
									mUpdateAll=true;
								}
		
		void /*e*/				ViewportOffset(
									TTBInteger		inX,
									TTBInteger		inY)
								{								
									if ((mXOffset!=inX) || (mYOffset!=inY))
									{
										VerifyNewOffsets(inX,inY);
										
										mXOffset=inX;
										mYOffset=inY;
										mUpdateAll=true;
									}
								}
		
		virtual void			Render();
		void					RenderSprites();
		
		TTBInteger				GetLeft()						{ return mBounds.left; }
		TTBInteger				GetTop()						{ return mBounds.top; }
		TTBInteger				GetWidth()						{ return FRectWidth(mBounds); }
		TTBInteger				GetHeight()						{ return FRectHeight(mBounds); }
		
		TTBInteger				GetXOffset()					{ return mXOffset; }
		TTBInteger				GetYOffset()					{ return mYOffset; }
		
		inline const Rect&		GetViewportRect()
								{									
									mViewportRect.top=mYOffset;
									mViewportRect.left=mXOffset;
									mViewportRect.bottom=mYOffset+FRectHeight(mBounds);
									mViewportRect.right=mXOffset+FRectWidth(mBounds);
									
									return mViewportRect;
								}
		
		TTBInteger				GetCanvas()						{ return mCanvas; }
		
		void					AddSprite(
									CTBSprite	*inSprite);
		
		void					RemoveSprite(
									CTBSprite	*inSprite);
									
		void					UpdateSpritePriority(
									CTBSprite	*inSprite);
									
		void					MoanViewport();
};