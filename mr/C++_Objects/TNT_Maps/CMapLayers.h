// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CMapLayers.h
// John Treece-Birch
// 24/2/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1998, Mark Tully and John Treece-Birch
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

#include				"CTNTMap.h"
#include				"Marks Routines.h"
#include				"TNewHandleStream.h"

class CTNTMap;

class CMapLayer
{
	private:
		TListElement			link;
		Handle					tilesData;
		Str32					name;
		TMapLayerOpacity		opacity;
		
		CTNTMap					*tmap;
	
	public:
		static const short	kMinOpacity=0;
		static const short	kMaxOpacity=100;
	
							CMapLayer(CTNTMap *inTmap,Str32 inName);
							CMapLayer(CTNTMap *inMap,CDataStoreStream &inStream);
							CMapLayer(CTNTMap *inTmap,TNewHandleStream &inStream);
							CMapLayer(CTNTMap *inTmap,CMapLayer *copyThis);
		virtual				~CMapLayer();
		
		void				SaveContent(CNewHandleStream &inStream);
		void				SaveContent(CDataStoreStream &inStream);
		
		void				LoadInfo(CDataStoreStream &inStream);
		void				SaveInfo(CDataStoreStream &inStream);
		
		void				SetName(Str32 inName)					{CopyPStr(inName,name);};
		void				GetName(Str32 outName)					{CopyPStr(name,outName);};
		
		void				SetOpacity(TMapLayerOpacity inOpacity)			{ opacity=inOpacity; };
		TMapLayerOpacity	GetOpacity()									{ return opacity; };
		
		void				Scale(long inNewWidth,long inNewHeight,Size inNewSize,long inLeft,long inTop);
		
		TMapTileIndex		GetTile(long x, long y);
		void				SetTile(long x, long y,TMapTileIndex tile);
		
		TListElement*		GetLink()								{return &link;};
		Handle				GetTilesData()							{return tilesData;};
};