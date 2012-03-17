// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CTMAPViewport.cpp
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

/*
	A wrapping viewport that automatically performs renders tmaps as it goes, resulting in a very effecient tile scrolling
	implementation.
*/

#include "CTMAPViewport.h"
#include "CBLCanvas.h"
#include "CGraphicsContext16.h"
#include "CTBSpriteGL.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Note the constructor is not public, instead use CreateNewTMAPViewport
CTMAPViewport::CTMAPViewport(
	CProgram		&inProgram,
	TTBInteger		inCanvasId,
	const Rect		&inDestRect,
	TTBInteger		inLayer,
	TTBInteger		inWidth,
	TTBInteger		inHeight) :
	CWrappingViewport(inProgram,inCanvasId,inDestRect,inWidth,inHeight),
	mRedrawEntireMap(false)
{	
	CCanvas			*canvas=inProgram.CheckGraphicsMode()->mCanvasManager.GetCanvas(inCanvasId);
	Rect			canvasBounds=canvas->GetBoundsRect();
	
	mTileWidth=UTNTMapManager::GetTileWidth() ? UTNTMapManager::GetTileWidth() : 1;
	mTileHeight=UTNTMapManager::GetTileHeight() ? UTNTMapManager::GetTileHeight() : 1;
	mViewableTilesX=FRectWidth(canvasBounds)/mTileWidth;
	mViewableTilesY=FRectHeight(canvasBounds)/mTileHeight;
	mLastRenderedMapOriginX=mLastRenderedMapOriginY=0;
	mLayerIndex=inLayer;
	
	RenderTileStrip(0,0,mViewableTilesX,mViewableTilesY);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CTMAPViewport::~CTMAPViewport()
{
	try
	{
		mProgram->CheckGraphicsMode()->mCanvasManager.CloseCanvas(GetCanvas());
	}
	catch(...)
	{
		// never throw from a destructor or bad things happen
	}
}

inline TTBInteger RoundUpToMultiple(
	TTBInteger		inNum,
	TTBInteger		inMult);
	
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RoundUpToMultiple
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Simple inline to round up to the nearest multiple
inline TTBInteger RoundUpToMultiple(
	TTBInteger		inNum,
	TTBInteger		inMult)
{
	if ((inNum%inMult)==0)	// exact multiple
		return inNum;
	else
		return inNum+(inMult-(inNum%inMult));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateNewTMAPViewport											Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CTMAPViewport *CTMAPViewport::CreateNewTMAPViewport(
	CProgram		&inProgram,
	TTBInteger		inCanvasId,
	TTBInteger		inLayer,
	const Rect		&inDestRect)
{
	UTNTMapManager::ThrowIfNoCurrentMap();

	TTBInteger		viewportWidth=FRectWidth(inDestRect),viewportHeight=FRectHeight(inDestRect);	
	TTBInteger		srcCanvasWidth,srcCanvasHeight;
	TTBInteger		tileWidth=UTNTMapManager::GetTileWidth() ? UTNTMapManager::GetTileWidth() : 1;
	TTBInteger		tileHeight=UTNTMapManager::GetTileHeight() ? UTNTMapManager::GetTileHeight() : 1;

	// open a canvas as big as the viewport with a 1 tile thick buffer around the edges	
	srcCanvasWidth=RoundUpToMultiple(viewportWidth,tileWidth)+1*tileWidth;
	srcCanvasHeight=RoundUpToMultiple(viewportHeight,tileHeight)+1*tileHeight;
	
	inProgram.CheckGraphicsMode()->mCanvasManager.OpenCanvas(inCanvasId,srcCanvasWidth,srcCanvasHeight);
	
	CTMAPViewport	*vp=new CTMAPViewport(inProgram,inCanvasId,inDestRect,inLayer,UTNTMapManager::GetMapWidth()*tileWidth,UTNTMapManager::GetMapHeight()*tileHeight);
	ThrowIfMemFull_(vp);
	
	return vp;
}
	
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawGriddedMap
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// All coords in tile coordinates (tile indexes rather than pixels). Pass x,y and w,h of map rect to render, then x,y of place
// in canvas to render it (again in tile coords)
inline void CTMAPViewport::DrawGriddedMap(
	TTBInteger		inMapX,
	TTBInteger		inMapY,
	TTBInteger		inMapW,
	TTBInteger		inMapH,
	TTBInteger		inDstGridX,		// tile coords
	TTBInteger		inDstGridY,
	CCanvas			*inCanvas)
{
	UTNTMapManager::DrawSection(inMapX,inMapY,inMapX+inMapW,inMapY+inMapH,mLayerIndex,inDstGridX*mTileWidth,inDstGridY*mTileHeight,inCanvas);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderTileStrip
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// renders a block of tiles onto the canvas. Tiles are always rendered at the same location on the canvas due to the way the
// wrapping works, therefore there is no need to specify a destination x and y coord in the canvas.
// This routine is needed because the strip being rendered could spill over the edge of the canvas and may therefore need
// breaking up into several draw map calls.
void CTMAPViewport::RenderTileStrip(
	TTBInteger		inMapX,
	TTBInteger		inMapY,
	TTBInteger		inWidth,
	TTBInteger		inHeight)
{
	TTBInteger		dstTop=inMapY,dstLeft=inMapX,dstBottom=dstTop+inHeight,dstRight=dstLeft+inWidth;
	CCanvas			*canvasP=mProgram->CheckGraphicsMode()->mCanvasManager.GetCanvas(GetCanvas());
	
	if (dstTop>=mViewableTilesY)
	{
		dstTop%=mViewableTilesY;
		dstBottom=dstTop+inHeight;
	}
	if (dstLeft>=mViewableTilesX)
	{
		dstLeft%=mViewableTilesX;
		dstRight=dstLeft+inWidth;
	}
	
	if (dstRight<=mViewableTilesX)		// horiz split?
	{
		// no horiz split
		if (dstBottom<=mViewableTilesY)	// vert split?
		{
			// no vert split
			DrawGriddedMap(inMapX,inMapY,inWidth,inHeight,dstLeft,dstTop,canvasP);
		}
		else
		{
			// vert split
			TTBInteger	wrappedHeight=dstBottom-mViewableTilesY;		// over lap height
			TTBInteger	nonWrappedHeight=inHeight-wrappedHeight;
			
			// draw overflow at the top of the canvas
			DrawGriddedMap(inMapX,inMapY+nonWrappedHeight,inWidth,wrappedHeight,dstLeft,0,canvasP);
			// draw non overflowed at the bottom of the canvas
			DrawGriddedMap(inMapX,inMapY,inWidth,nonWrappedHeight,dstLeft,dstTop,canvasP);			
		}
	}
	else
	{
		// horiz split
		TTBInteger	wrappedWidth=dstRight-mViewableTilesX;		// over lap height
		TTBInteger	nonWrappedWidth=inWidth-wrappedWidth;

		if (dstBottom<mViewableTilesY)	// vert split?
		{
			// no vert split, just horiz
			
			// draw over flow on the left of canvas
			DrawGriddedMap(inMapX+nonWrappedWidth,inMapY,wrappedWidth,inHeight,0,dstTop,canvasP);			
			// draw non overflow on the right of the canavs
			DrawGriddedMap(inMapX,inMapY,nonWrappedWidth,inHeight,dstLeft,dstTop,canvasP);			
		}
		else
		{
			TTBInteger	wrappedHeight=dstBottom-mViewableTilesY;		// over lap height
			TTBInteger	nonWrappedHeight=inHeight-wrappedHeight;

			// Four renders are needed now as the map has wrapped off the right and bottom edges creating 4 areas
			
			// top left, composed of wrapped tiles from right and bottom edges
			DrawGriddedMap(inMapX+nonWrappedWidth,inMapY+nonWrappedHeight,wrappedWidth,wrappedHeight,0,0,canvasP);
			
			// top right, wrapped tiles from the bottom, but not those that have gone off the right edge
			DrawGriddedMap(inMapX,inMapY+nonWrappedHeight,nonWrappedWidth,wrappedHeight,dstLeft,0,canvasP);
			
			// bottom left, wrapped tile from the right edge, but not those that have wrapped off the bottom
			DrawGriddedMap(inMapX+nonWrappedWidth,inMapY,wrappedWidth,nonWrappedHeight,0,dstTop,canvasP);
			
			// bottom right, the area of tiles that aren't going off the edge of the canvas
			DrawGriddedMap(inMapX,inMapY,nonWrappedWidth,nonWrappedHeight,dstLeft,dstTop,canvasP);
		}	
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderRevealedAreas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Looks at what the current offset should be according to the viewport offsets and renders the tiles accordingly.
// You don't need to think about wrapping here, just think about a large map being scrolled and figure which tiles would be
// revealed, all the wrapping is dont in the RenderTileStrip routine.
void CTMAPViewport::RenderRevealedAreas()
{
	TTBInteger		mapX=mXOffset/mTileWidth;
	TTBInteger		mapY=mYOffset/mTileHeight;
	
	if (mRedrawEntireMap)
	{		
		RenderTileStrip(mapX,mapY,mViewableTilesX,mViewableTilesY);	// render all rather than just revealed, useful for debugging
		mLastRenderedMapOriginX=mapX;
		mLastRenderedMapOriginY=mapY;
		mRedrawEntireMap=false;
		return;
	}

	if (mapX==mLastRenderedMapOriginX && mapY==mLastRenderedMapOriginY)
		return;
	
	// calculate the width of revealed tiles on each of the borders and redraw them
	
	TTBInteger	rightWidth=mapX-mLastRenderedMapOriginX;
	TTBInteger	bottomWidth=mapY-mLastRenderedMapOriginY;
	TTBInteger	leftWidth=-rightWidth;
	TTBInteger	topWidth=-bottomWidth;
	
	if (rightWidth<0) rightWidth=0;
	if (topWidth<0) topWidth=0;
	if (leftWidth<0) leftWidth=0;
	if (bottomWidth<0) bottomWidth=0;

	// if you're scrolling in both axis at once, then there is an overlapped area of tiles in the corner, where tiles are
	// both revealed because of an x scroll and a y scroll and are thus in both two refresh zones.
	// we don't want to draw this twice. To avoid this we always draw these corners as part of the top/bottom rendering.
	// If the top/bottom aren't being rendered (ie top width and bottom width are zero) then the corners are rendered
	// by the side renders.
	
	// render revealed top
	RenderTileStrip(mapX,mapY,mViewableTilesX,topWidth);
	// render left side
	RenderTileStrip(mapX,mapY+topWidth,leftWidth,mViewableTilesY-topWidth-bottomWidth);
	// render right side
	RenderTileStrip(mapX+mViewableTilesX-rightWidth,mapY+topWidth,rightWidth,mViewableTilesY-topWidth-bottomWidth);
	// render bottom side
	RenderTileStrip(mapX,mapY+mViewableTilesY-bottomWidth,mViewableTilesX,bottomWidth);
		
	// remember what we just rendered
	mLastRenderedMapOriginX=mapX;
	mLastRenderedMapOriginY=mapY;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Render
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CTMAPViewport::Render()
{
	RenderRevealedAreas();
	inheritedViewport::Render();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReactToChangedTile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// if the tile is on screen then refresh it, can't be bothered doing slick code here, just redraw all.
void CTMAPViewport::ReactToChangedTile(
	TTBInteger		inX,
	TTBInteger		inY,
	TTBInteger		inZ)
{
	if (inZ==mLayerIndex)
	{
		mRedrawEntireMap=true;
		mUpdateAll=true;
	}
}
