// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CMapPolygons.h
// John Treece-Birch
// 22/3/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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

#include			"CTNTMap.h"

class CMapPolygonComponent
{
	private:
		TListElement		link;
		Rect				area;
		TMapLayerIndex		layer;
	
	public:
							CMapPolygonComponent();
							CMapPolygonComponent(Rect &inRect,TMapLayerIndex inLayer);
							CMapPolygonComponent(CDataStoreStream &inStream);
									
		void				SaveContent(CDataStoreStream &inStream);

		void				Scale(TMapTileIndex inWidth,TMapTileIndex inHeight,TMapTileIndex inTop,TMapTileIndex inLeft);

		TMapLayerIndex		GetLayer()							{return layer;};
		void				SetLayer(TMapLayerIndex inLayer)	{layer=inLayer;};
		
		Rect				GetArea()							{return area;};
		void				SetArea(Rect inRect)				{area=inRect;};
		
		static Size			GetSize()							{return sizeof(Rect)+sizeof(unsigned short);};
		
		bool				In(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z);
		
		TListElement*		GetLink()							{return &link;};
};

class CMapPolygon
{
	private:
		TListElement					link;
		TMapPolygonComponentIndex		components;
		TLinkedList						componentsList;
		RGBColour						colour;
		Str32							name;
		
	public:
							CMapPolygon();
							CMapPolygon(Str32 inName,RGBColour inColour);
							CMapPolygon(CDataStoreStream &inStream);
							CMapPolygon(CMapPolygon *copyThis);
		virtual 			~CMapPolygon();
				
		void				SaveContent(CDataStoreStream &inStream);
		void				LoadInfo(CDataStoreStream &inStream);
		void				SaveInfo(CDataStoreStream &inStream);
		
		void				Scale(TMapTileIndex inWidth,TMapTileIndex inHeight,TMapTileIndex inTop,TMapTileIndex inLeft);
		void				MoveLayer(SInt32 inMoveThis,SInt32 inToHere);
		void				DeleteLayer(TMapLayerIndex inLayer);
		
		void				SetName(Str32 inName)					{LString::CopyPStr(inName,name);};
		void				GetName(Str32 outName)					{LString::CopyPStr(name,outName);};

		void				GetPolygonMaximumBounds(
								TMapTileIndex		&outTop,
								TMapTileIndex		&outLeft,
								TMapTileIndex		&outBottom,
								TMapTileIndex		&outRight);
		
		void				SetColour(RGBColour inColour)			{colour=inColour;};
		RGBColour			GetColour()								{return colour;};
		
		bool				In(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z);
		
		void				AddComponent(Rect &rect,TMapLayerIndex layer);
		void				DeleteComponent();
		void				RemoveComponent(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z);

		TMapPolygonComponentIndex		GetComponents()										{ return components;};
		CMapPolygonComponent*			GetComponent(TMapPolygonComponentIndex inIndex)		{ return (CMapPolygonComponent*)componentsList.GetNthElementData(inIndex);};
		
		TListElement*					GetLink()											{return &link;};
};