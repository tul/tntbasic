// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// UTNTMapManager.h
// John Treece-Birch
// 18/5/00
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

#include	"CTNTMap.h"
#include	"UTBException.h"

class CCanvas;
class CProgram;

class UTNTMapManager
{	
	private:
		static TArray<CTNTMap*>		sMaps;
		static TArray<SInt32>		sMapIds;
		static TArray<CCanvas*>		sTileCanvas;
		
		static UInt16				sCurrentMapNumber;
		
		static CTNTMap*				GetMap(ResIDT inResId);
		static ArrayIndexT			GetMapNumber(ResIDT inResId);
		static ArrayIndexT			GetFreeSpace();
		
	public:
		static void				Initialise();
		static void				ShutDown();

		static void				SetCurrentMap(
									CProgram			&ioProgram,
									ResIDT				inResId);
		static void				LoadTNTMap(
									CProgram			&ioProgram,
									ResIDT				inResId);
		static void				UnloadTNTMap(
									CProgram			&ioProgram,
									ResIDT				inResId);

		static TTBInteger		GetTile(
									TTBInteger			inX,
									TTBInteger 			inY,
									TTBInteger 			inZ);
		static void				SetTile(
									CProgram			&ioProgram,
									TTBInteger			inX,
									TTBInteger			inY,
									TTBInteger			inZ,
									TTBInteger			inTile);
		static TTBInteger		GetMapWidth();
		static TTBInteger		GetMapHeight();
		static TTBInteger		GetTileWidth();
		static TTBInteger		GetTileHeight();
		static TTBInteger		CountLayers();

		static TTBInteger		CountObjects();
		static TTBInteger		GetNthObjectType(TTBInteger inObjectIndex);
		static TTBInteger		GetNthObjectX(TTBInteger inObjectIndex);
		static TTBInteger		GetNthObjectY(TTBInteger inObjectIndex);
		static TTBInteger		GetNthObjectZ(TTBInteger inObjectIndex);
		static CCString			GetNthObjectName(TTBInteger inObjectIndex);

		static void				GetPolygonBounds(
									TTBInteger	inPolyIndex,
									TTBInteger	&outTop,
									TTBInteger	&outLeft,
									TTBInteger	&outBottom,
									TTBInteger	&outRight);
		static void				GetPolygonName(
									TTBInteger	inPolyIndex,
									CCString	&outString);
		static TTBInteger		CountPolygons();
		static bool				InPolygon(
									TTBInteger inPolygonIndex,
									TTBInteger inX,
									TTBInteger inY,
									TTBInteger inZ);

		static void				DrawSection(
									TTBInteger			inLeft,
									TTBInteger			inTop,
									TTBInteger			inRight,
									TTBInteger			inBottom,
									TTBInteger			inLayer,
									TTBInteger			inX,
									TTBInteger			inY,
									CCanvas				*inCanvas);
		
		static bool				LineCol(
									TTBInteger			inStartX,
									TTBInteger			inStartY,
									TTBInteger			inEndX,
									TTBInteger			inEndY,
									TTBInteger			inLayer,
									TTBInteger			inStartTile,
									TTBInteger			inEndTile);
	
		static void /*e*/		ThrowIfNoCurrentMap();
};
