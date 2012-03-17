// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CTMAPViewport.h
// © Mark Tully 2002
// 6/2/02
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

#include "CWrappingViewport.h"

class CTMAPViewport : public CWrappingViewport
{
	private:
		typedef CWrappingViewport	inheritedViewport;

	protected:
		TTBInteger					mLastRenderedMapOriginX,mLastRenderedMapOriginY;
		TTBInteger					mViewableTilesX,mViewableTilesY;
		TTBInteger					mTileWidth,mTileHeight;			// cached, set to 1 if map returns 0 for either of them as 0 would feck up many calculations
		TTBInteger					mLayerIndex;
		bool						mRedrawEntireMap;
	
									CTMAPViewport(
										CProgram		&inProgram,
										TTBInteger		inCanvasId,
										const Rect		&inDestRect,
										TTBInteger		inLayer,
										TTBInteger		inWidth,
										TTBInteger		inHeight);

		void						RenderTileStrip(
										TTBInteger		inMapX,
										TTBInteger		inMapY,
										TTBInteger		inWidth,
										TTBInteger		inHeight);
		void						RenderRevealedAreas();
		inline void					DrawGriddedMap(
										TTBInteger		inMapX,
										TTBInteger		inMapY,
										TTBInteger		inMapW,
										TTBInteger		inMapH,
										TTBInteger		inDstGridX,		// tile coords
										TTBInteger		inDstGridY,
										CCanvas			*inCanvas);
	
	public:
		virtual						~CTMAPViewport();
	
		static CTMAPViewport /*e*/	*CreateNewTMAPViewport(
										CProgram		&inProgram,
										TTBInteger		inCanvasId,
										TTBInteger		inLayer,
										const Rect		&inDestRect);										

		virtual void				Render();
		
		void						ReactToChangedTile(
										TTBInteger		inX,
										TTBInteger		inY,
										TTBInteger		inZ);
										
		TTBInteger					GetLayerIndex()		{ return mLayerIndex; }
};