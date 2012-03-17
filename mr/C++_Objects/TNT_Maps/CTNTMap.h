// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CTNTMap.h
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

#include			"CTNTMapTypes.h"

#include			"CMarkableDataStore.h"
#include			"CHandleDataStore.h"
#include			"CTaggedDataSection.h"
#include			"CTaggedDataGroup.h"
#include			"CDataStoreStream.h"
#include			"TNT_Debugging.h"
#include			"TListIndexer.h"

#include			"CMapLayers.h"
#include			"CMapObjects.h"
#include			"CMapPolygons.h"

#include			"CCString.h"
#include			<string.h>

enum
{
	kUnknownFileFormat=-1,
};

enum EPictureReferenceType
{
	kNoPicture=0,
	kPictureId,
	kPictureName,
};

class CMapPolygon;

class CGenesisDataStore : public CMarkableDataStore
{
	protected:
		static const short		kTaggedDataVersion=0;
	
		CHandleDataStore		mHandle;
		
	public:
								CGenesisDataStore(Handle inHandle);
								
		Handle					GetHandle(bool inStrip)							{return mHandle.GetDataHandle(inStrip);};
};

class CTNTMap
{
	public:
		static const short	kMaxTileSize=32;
		
		static const CTaggedDataSection::TTagId			kDimensions='DIME';
		static const TVersion							kDimensionsVersion=2;
		
		static const CTaggedDataSection::TTagId			kGSUPReference='GSRF';
		static const TVersion							kGSUPReferenceVersion=1;
		
		static const CTaggedDataSection::TTagId			kLayerContent='LACT';
		static const TVersion							kLayerContentVersion=1;
		
		static const CTaggedDataSection::TTagId			kLayerInfo='LAIN';
		static const TVersion							kLayerInfoVersion=1;
		
		static const CTaggedDataSection::TTagId			kPolygonContent='POCT';
		static const TVersion							kPolygonContentVersion=1;
		
		static const CTaggedDataSection::TTagId			kPolygonInfo='POIN';
		static const TVersion							kPolygonInfoVersion=1;
		
		static const CTaggedDataSection::TTagId			kObjectContent='OBCT';
		static const TVersion							kObjectContentVersion=1;
		
		static const CTaggedDataSection::TTagId			kObjectInfo='OBIN';
		static const TVersion							kObjectInfoVersion=1;
		
		static const CTaggedDataSection::TTagId			kMapInfo='MAIN';
		static const TVersion							kMapInfoVersion=1;
	
	protected:
		// Map data
		unsigned short		mMapWidth,mMapHeight,mTileWidth,mTileHeight;
		unsigned short		mPolygons,mLayers;
		unsigned short		mTilesUsed;
		
		EPictureReferenceType			mPicReference;
		short							mPicId;
		char							mPicName[256];
		
		// Map content
		TLinkedList			mLayersList;
		CMapObjectsManager	*mObjectsManager;
		TLinkedList			mPolygonsList;
		
		void				ChangeTilesUsed();

		void				ScaleLayers(long inNewWidth,long inNewHeight,Size inSize,long inLeft,long inTop);
		void				ScalePolygons(long inNewWidth,long inNewHeight,long inLeft,long inTop);
	
	public:
		static const UInt16		kTransparentTile=0;
		static const UInt16		kTransparentClass=0;
	
							CTNTMap();
		virtual				~CTNTMap();
		
		// Maps
		virtual void		LoadDataFile(Handle inDataHandle);
		virtual Handle		SaveDataFile();
		
		virtual void		LoadResourceFile(Handle inResourceHandle);
		virtual Handle		SaveResourceFile();
				
		unsigned short		GetMapWidth()									{ return mMapWidth; }
		unsigned short		GetMapHeight()									{ return mMapHeight; }
		void				SetMapWidth(unsigned short newWidth)			{ mMapWidth=newWidth; }
		void				SetMapHeight(unsigned short newHeight)			{ mMapHeight=newHeight; }
		
		bool				InMap(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z);
				
		// Tiles
		unsigned short		GetTileWidth()									{ return mTileWidth; }
		unsigned short		GetTileHeight()									{ return mTileHeight; }
		unsigned short		GetTilesUsed()									{ return mTilesUsed; }
	
		void				SetTile(long x,long y,TMapLayerIndex z, unsigned short tile);
		long				GetTile(long x,long y,TMapLayerIndex z);
		
		void				SetTileWidth(unsigned short newWidth)			{ mTileWidth=newWidth; }
		void				SetTileHeight(unsigned short newHeight)			{ mTileHeight=newHeight; }
		void				SetTilesUsed(unsigned short newTiles)			{ mTilesUsed=newTiles; }
		
		Size				GetTileDataSize();
		
		// Pictures
		short					GetPicId()										{ return mPicId; }
		EPictureReferenceType	GetPicReferenceType()							{ return mPicReference; }
		char*					GetPicName()									{ return mPicName; }
		
		void				SetPicId(short inPicId)								{ mPicId=inPicId; }
		void				SetPicName(char* inPicName)							{ /*std::*/strcpy(mPicName,inPicName); }
		void				SetPicReferenceType(EPictureReferenceType inType)	{ mPicReference=inType; }
				
		// Layers
		void				AddLayer(Str32 inName);
		void				RemoveLayer(TMapLayerIndex inLayer);
		void				MoveLayer(SInt32 inMoveThis,SInt32 inToHere);
		
		void				SetLayerOpacity(TMapLayerIndex inLayer,TMapLayerOpacity inOpacity);
		TMapLayerOpacity	GetLayerOpacity(TMapLayerIndex inLayer);
		
		void				SetLayerName(TMapLayerIndex inLayer,Str32 inName);
		void				GetLayerName(TMapLayerIndex inLayer,Str32 inName);
		
		CMapLayer*			GetLayer(TMapLayerIndex inIndex)				{ return (CMapLayer*)mLayersList.GetNthElementData(inIndex); };
		TMapLayerIndex		CountLayers()									{ return mLayers; };

		// Objects
		TMapObjectIndex		CountObjects()									{ return mObjectsManager->CountObjects(); };
		
		void				AddObject(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z,TMapObjectTypeIndex inObject,bool inSelected)			{ mObjectsManager->AddObject(x,y,z,inObject,inSelected); };
		void				RemoveObject(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z)														{ mObjectsManager->RemoveObject(x,y,z); };
		TMapObjectTypeIndex	GetObjectType(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z)														{ return mObjectsManager->GetObjectType(x,y,z); };
		
		void				SelectAllObjects()																								{ mObjectsManager->SelectAllObjects(); };
		void				DeselectAllObjects()																							{ mObjectsManager->DeselectAllObjects(); };
		
		void				SelectObjectsRect(TMapTileIndex top,TMapTileIndex left,TMapTileIndex bottom,TMapTileIndex right)				{ mObjectsManager->SelectObjectsRect(top,left,bottom,right); };
		void				ToggleSelectObjectsRect(TMapTileIndex top,TMapTileIndex left,TMapTileIndex bottom,TMapTileIndex right)			{ mObjectsManager->ToggleSelectObjectsRect(top,left,bottom,right); };

		void				DeleteSelectedObjects()																				{ mObjectsManager->DeleteSelectedObjects(); };
		SInt32				GetSelectedObjectsLeft()																			{ return mObjectsManager->GetSelectionLeft(); };
		SInt32				GetSelectedObjectsTop()																				{ return mObjectsManager->GetSelectionTop(); };

		void				MoveSelectedObjects(SInt32 inX,SInt32 inY)															{ mObjectsManager->MoveSelection(inX,inY); };

		bool				ObjectsSelected()																					{ return mObjectsManager->ValidSelection(); };

		SInt32				GetNthObjectType(TMapObjectIndex inObjectIndex)								{ return mObjectsManager->GetNthObjectType(inObjectIndex); };
		SInt32				GetNthObjectX(TMapObjectIndex inObjectIndex)								{ return mObjectsManager->GetNthObjectX(inObjectIndex); };
		SInt32				GetNthObjectY(TMapObjectIndex inObjectIndex)								{ return mObjectsManager->GetNthObjectY(inObjectIndex); };
		SInt32				GetNthObjectZ(TMapObjectIndex inObjectIndex)								{ return mObjectsManager->GetNthObjectZ(inObjectIndex); };

		// Object Types
		TMapObjectTypeIndex	CountObjectTypes()																		{ return mObjectsManager->CountTypes(); }
		
		void				AddObjectType(Str32 inName,const RGBColour &inColour)									{ mObjectsManager->AddObjectType(inName,inColour); };
		void				RemoveObjectType(TMapObjectTypeIndex inObject)											{ mObjectsManager->RemoveObjectType(inObject); };
		
		void				SetObjectTypeColour(TMapObjectTypeIndex inObject,const RGBColour &inColour)				{ mObjectsManager->SetObjectTypeColour(inObject,inColour); };
		void				GetObjectTypeColour(TMapObjectTypeIndex inObject,RGBColour &outColour)					{ mObjectsManager->GetObjectTypeColour(inObject,outColour); };
		
		void				SetObjectTypeName(TMapObjectTypeIndex inObject,Str32 inName)							{ mObjectsManager->SetObjectTypeName(inObject,inName); };
		void				GetObjectTypeName(TMapObjectTypeIndex inObject,Str32 &outName)							{ mObjectsManager->GetObjectTypeName(inObject,outName); };
		
		// Polygons
		void				AddPolygon(Str32 inName,RGBColour inColour);
		void				RemovePolygon(TMapPolygonIndex inPolygon);
		
		void				SetPolygonColour(TMapPolygonIndex inPolygon,RGBColour inColour);
		RGBColour			GetPolygonColour(TMapPolygonIndex inPolygon);
		
		void				GetPolygonName(TMapPolygonIndex inPolygon,Str32 outName);
		void				SetPolygonName(TMapPolygonIndex inPolygon,Str32 inName);
		
		CMapPolygon*		GetPolygon(TMapPolygonIndex inIndex)				{ return (CMapPolygon*)mPolygonsList.GetNthElementData(inIndex);};
		TMapPolygonIndex	CountPolygons()										{ return mPolygons;};
		
		TMapPolygonIndex	GetPolygonAt(unsigned short x,unsigned short y,TMapLayerIndex z);
		
		bool				InPolygon(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z,TMapPolygonIndex p);
};