// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CMapObjects.h
// John Treece-Birch
// 9/6/99
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

#include			"CMarkableDataStore.h"
#include			"CHandleDataStore.h"
#include			"CTaggedDataSection.h"
#include			"CTaggedDataGroup.h"
#include			"CDataStoreStream.h"
#include			"TNT_Debugging.h"
#include			"Utility Objects.h"
#include			"TListIndexer.h"
#include			"Marks Routines.h"

#include			"CTNTMapTypes.h"

class CTNTMap;

class CMapObject
{
	private:
		TListElement			link;
		TMapTileIndex			x,y;
		TMapLayerIndex			z;
		TMapObjectTypeIndex		type;
		bool					selected;
	
	public:
								CMapObject(TMapTileIndex inX,TMapTileIndex inY,TMapLayerIndex inZ,TMapObjectTypeIndex inType);
								CMapObject(CDataStoreStream &inStream);
								CMapObject(CMapObject &inObject);
						
		void					SaveContent(CDataStoreStream &inStream);
		
		void					Scale(long inNewWidth,long inNewHeight,long inLeft,long inTop);
		void					Move(SInt32 inX,SInt32 inY);
		
		TMapTileIndex			GetX()									{ return x; };
		TMapTileIndex			GetY()									{ return y; };
		TMapLayerIndex			GetZ()									{ return z; };
		TMapObjectTypeIndex		GetType()								{ return type; };
		
		void					SetZ(TMapLayerIndex inZ)				{ z=inZ; };
		void					SetType(TMapObjectTypeIndex inType)		{ type=inType; };
		
		bool					Selected()								{ return selected; };
		void					ToggleSelect()							{ selected=!selected; };
		void					Select()								{ selected=true; };
		void					Deselect()								{ selected=false; }
		
		TListElement*			GetLink()								{ return &link; };
};

class CMapObjectType
{
	private:
		TListElement		link;
		RGBColour			colour;
		Str32				name;
		
	public:
							CMapObjectType(const Str32 inName,const RGBColour inColour);
							CMapObjectType(CDataStoreStream &inStream);
							CMapObjectType(CMapObjectType &inType);

		void				SaveInfo(CDataStoreStream &inStream);

		void				SetName(Str32 inName)			{ CopyPStr(inName,name); };
		void				SetColour(const RGBColour &inColour)	{ colour=inColour; };

		void				GetName(Str32 &outName)					{ CopyPStr(name,outName); };
		void				GetColour(RGBColour &outColour)			{ outColour=colour; };
		
		TListElement*		GetLink()								{ return &link;};
};

class CMapObjectsManager
{
	private:
		TLinkedList			objects;		
		TLinkedList			types;
		CTNTMap				*tmap;

		CMapObjectType*		GetObjectType(TMapObjectTypeIndex inIndex)			{ return (CMapObjectType*)types.GetNthElementData(inIndex); };

	public:
		static const SInt16	kInvalidObject=-1;
	
							CMapObjectsManager(CTNTMap *inTmap);
		virtual				~CMapObjectsManager();
	
		void				LoadContent(CDataStoreStream &inStream);
		void				LoadInfo(CDataStoreStream &inStream);
		
		void				SaveContent(CDataStoreStream &inStream);
		void				SaveInfo(CDataStoreStream &inStream);
		
		bool				Copy(CMapObjectsManager *copyThis);
				
		void				MoveSelection(SInt32 inX,SInt32 inY);
		
		void				ClipboardCopy();
		void 				ClipboardCut();
		void				ClipboardPaste(TMapTileIndex inX,TMapTileIndex inY);
		
		void				MoveLayer(SInt32 inMoveThis,SInt32 inToHere);
		void				RemoveLayer(TMapLayerIndex inLayer);
		
		void				AddObject(long x,long y,TMapLayerIndex z,TMapObjectTypeIndex inObject,bool inSelected);
		void				RemoveObject(long x,long y,TMapLayerIndex z);
		TMapObjectTypeIndex	GetObjectType(long x,long y,TMapLayerIndex z);
		
		void				AddObjectType(Str32 inName,const RGBColour &inColour);
		void				RemoveObjectType(TMapObjectTypeIndex inObject);
		
		void				SelectAllObjects();
		void				DeselectAllObjects();
		void				SelectObjectsRect(TMapTileIndex top,TMapTileIndex left,TMapTileIndex bottom,TMapTileIndex right);
		void				ToggleSelectObjectsRect(TMapTileIndex top,TMapTileIndex left,TMapTileIndex bottom,TMapTileIndex right);
		void 				DeleteSelectedObjects();
		
		bool				ValidSelection();

		SInt32				GetSelectionLeft();
		SInt32				GetSelectionTop();
		SInt32				GetSelectionBottom();
		SInt32				GetSelectionRight();

		void				SetObjectTypeName(TMapObjectTypeIndex inIndex,Str32 inName)					{ GetObjectType(inIndex)->SetName(inName); };
		void				SetObjectTypeColour(TMapObjectTypeIndex inIndex,const RGBColour &inColour)	{ GetObjectType(inIndex)->SetColour(inColour); };
		
		void				GetObjectTypeName(TMapObjectTypeIndex inIndex,Str32 &outName)				{ GetObjectType(inIndex)->GetName(outName); };
		void				GetObjectTypeColour(TMapObjectTypeIndex inIndex,RGBColour &outColour)		{ GetObjectType(inIndex)->GetColour(outColour); };
		
		TMapObjectIndex		CountObjects()																{ return objects.CountElements(); };	
		TMapObjectIndex		CountSelectedObjects();
		TMapObjectTypeIndex	CountTypes()																{ return types.CountElements(); }

		CLinkedListT<CMapObject>	*GetObjects()														{ return (CLinkedListT<CMapObject>*)&objects; }
		CMapObject*					GetObject(TMapObjectIndex inIndex)									{ return (CMapObject*)objects.GetNthElementData(inIndex); };

		SInt32				GetNthObjectType(TMapObjectIndex inIndex);
		SInt32				GetNthObjectX(TMapObjectIndex inIndex);
		SInt32				GetNthObjectY(TMapObjectIndex inIndex);
		SInt32				GetNthObjectZ(TMapObjectIndex inIndex);
};