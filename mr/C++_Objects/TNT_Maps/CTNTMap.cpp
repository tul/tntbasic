// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CTNTMap.cpp
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

#include	"CTNTMap.h"
#include	"TNewHandleStream.h"

CTNTMap::CTNTMap()
{
	mPicId=0;
	std::strcpy(mPicName,"");
	mPicReference=kNoPicture;

	mMapWidth=10;
	mMapHeight=10;
	mTileWidth=16;
	mTileHeight=16;
	mTilesUsed=255;
	
	mPolygons=0;
	mLayers=0;
	
	mObjectsManager=new CMapObjectsManager(this);
}

CTNTMap::~CTNTMap()
{
	// delete all polygons
	TListIndexer	polygonIndexer(&mPolygonsList);
	while (CMapPolygon *thePolygon=(CMapPolygon*)polygonIndexer.GetNextData())
		delete thePolygon;
	
	// delete all layers
	TListIndexer	layerIndexer(&mLayersList);
	while (CMapLayer *theLayer=(CMapLayer*)layerIndexer.GetNextData())
		delete theLayer;

	delete mObjectsManager;
}

#pragma mark -

void CTNTMap::LoadDataFile(Handle inDataHandle)
{
	TNewHandleStream	stream(inDataHandle,false);
	
	SInt32				n,version;
	
	stream >> version;
	
	if (version==0)
	{
		// Header
		SInt32				mapWidth,mapHeight,layers,tileWidth,tileHeight,tilesUsed;
		
		stream >> mapWidth >> mapHeight >> layers;
		stream >> tileWidth >> tileHeight >> tilesUsed;
		
		mMapWidth=mapWidth;
		mMapHeight=mapHeight;
		mLayers=layers;
		mTileWidth=tileWidth;
		mTileHeight=tileHeight;
		mTilesUsed=tilesUsed;
		
		SInt32				stringLength;
		
		stream >> stringLength;
		
		for (n=0; n<stringLength; n++)
			stream >> mPicName[n];
			
		mPicName[n]=0;
		
		// Tile content
		for (n=0; n<mLayers; n++)
		{
			CMapLayer	*layer=new CMapLayer(this,stream);
				
			mLayersList.AppendElement(layer->GetLink());
		}
			
		// Objects
		
		// Polygons
		SInt32			polygons;
		
		stream >> polygons;
	}
}

Handle CTNTMap::SaveDataFile()
{
	CNewHandleStream			stream;
	SInt32						n,version=0;
	
	stream << version;
	
	// Header
	SInt32				mapWidth,mapHeight,layers,tileWidth,tileHeight,tilesUsed;
	
	mapWidth=mMapWidth;
	mapHeight=mMapHeight;
	layers=mLayers;
	tileWidth=mTileWidth;
	tileHeight=mTileHeight;
	tilesUsed=mTilesUsed;
	
	stream << mapWidth << mapHeight << layers;
	stream << tileWidth << tileHeight << tilesUsed;
	
	SInt32		stringLength=std::strlen(mPicName);
	
	stream << stringLength;
	
	for (n=0; n<stringLength; n++)
		stream << mPicName[n];
		
	// Tile content
	TListIndexer	layersIndexer(&mLayersList);
	while (CMapLayer *theLayer=(CMapLayer*)layersIndexer.GetNextData())
		theLayer->SaveContent(stream);
		
	// Objects
	
	// Polygons
	SInt32			polygons=mPolygons;
	
	stream << polygons;
	
	stream.SetDisposeHandleFlag(false);
	
	return stream.GetDataHandle();
}

void CTNTMap::LoadResourceFile(Handle resourceHandle)
{
	CGenesisDataStore			dataStore(resourceHandle);
	CTaggedDataGroup			dataGroup(&dataStore,sizeof(short),dataStore.GetDataStore()->GetLength()-sizeof(short));

	dataGroup.SetLock(true);

	Try_
	{
		// Load the map dimensions
		CDataStoreStream			dimStream(dataGroup.GetTag(kDimensions));
		TVersion					dimVersion;
		
		dimStream >> dimVersion;
		
		if (dimVersion==1)
		{
			dimStream >> mMapWidth >> mMapHeight;
			dimStream >> mTileWidth >> mTileHeight;
			dimStream >> mTilesUsed;
			
			bool picIdValid;
			dimStream >> mPicId >> picIdValid;
			
			if (picIdValid)
				mPicReference=kPictureId;
			else
				mPicReference=kNoPicture;
		}
		else if (dimVersion==2)
		{
			dimStream >> mMapWidth >> mMapHeight;
			dimStream >> mTileWidth >> mTileHeight;
			dimStream >> mTilesUsed;
			
			SInt8	picReference;
			
			dimStream >> picReference >> mPicId >> mPicName;
			
			mPicReference=(EPictureReferenceType)picReference;
		}
		else
			Throw_(kUnknownFileFormat);
		
		// Load the layer content
		CDataStoreStream			layerContentStream(dataGroup.GetTag(kLayerContent));
		TVersion					layerContentVersion;

		layerContentStream >> layerContentVersion;
		
		if (layerContentVersion==kLayerContentVersion)
		{
			layerContentStream >> mLayers;
			
			for (UInt16 n=0; n<mLayers; n++)
			{
				CMapLayer	*layer=new CMapLayer(this,layerContentStream);
				ThrowIfMemFull_(layer);
				
				mLayersList.AppendElement(layer->GetLink());
			}
		}
		else
			Throw_(kUnknownFileFormat);
			
		// Load the polygon content
		CDataStoreStream			polygonContentStream(dataGroup.GetTag(kPolygonContent));
		TVersion					polygonContentVersion;

		polygonContentStream >> polygonContentVersion;
		
		if (polygonContentVersion==kPolygonContentVersion)
		{
			polygonContentStream >> mPolygons;
			
			for (UInt16 n=0; n<mPolygons; n++)
			{
				CMapPolygon	*polygon=new CMapPolygon(polygonContentStream);

				ThrowIfMemFull_(polygon);			
				mPolygonsList.AppendElement(polygon->GetLink());
			}
		}
		else
			Throw_(kUnknownFileFormat);
		
		// Load the object content
		CDataStoreStream			objectContentStream(dataGroup.GetTag(kObjectContent));
		
		mObjectsManager->LoadContent(objectContentStream);
	}
	
	Catch_(err)
	{
		Throw_(kUnknownFileFormat);
	}
	
	// Attempt to load the non-essential items
	Try_
	{
		// Load the layer info
		CDataStoreStream			layerInfoStream(dataGroup.GetTag(kLayerInfo));
		TVersion					layerInfoVersion;

		layerInfoStream >> layerInfoVersion;
		
		if (layerInfoVersion==kLayerInfoVersion)
		{
			layerInfoStream << CountLayers();
			
			TListIndexer	layersIndexerB(&mLayersList);
			while (CMapLayer *theLayer=(CMapLayer*)layersIndexerB.GetNextData())
				theLayer->LoadInfo(layerInfoStream);
		}
		else
			Throw_(kUnknownFileFormat);
	}
	
	Catch_(err)
	{
		if (ErrCode_(err)!=kTNTErr_TagNotFound)
			Throw_(kUnknownFileFormat);
	}
	
	Try_
	{
		// Load the polygon info
		CDataStoreStream			polygonInfoStream(dataGroup.GetTag(kPolygonInfo));
		TVersion					polygonInfoVersion;

		polygonInfoStream >> polygonInfoVersion;
		
		if (polygonInfoVersion==kPolygonInfoVersion)
		{
			polygonInfoStream << CountPolygons();
			
			TListIndexer	polygonIndexerB(&mPolygonsList);
			while (CMapPolygon *thePolygon=(CMapPolygon*)polygonIndexerB.GetNextData())
				thePolygon->LoadInfo(polygonInfoStream);
		}
		else
			Throw_(kUnknownFileFormat);
	}

	Catch_(err)
	{
		if (ErrCode_(err)!=kTNTErr_TagNotFound)
			Throw_(kUnknownFileFormat);
	}
	
	Try_
	{
		// Load the object info
		CDataStoreStream			objectInfoStream(dataGroup.GetTag(kObjectInfo));
		
		mObjectsManager->LoadInfo(objectInfoStream);
	}

	Catch_(err)
	{
		if (ErrCode_(err)!=kTNTErr_TagNotFound)
			Throw_(kUnknownFileFormat);
	}
}

Handle CTNTMap::SaveResourceFile()
{
	// Create data store
	CGenesisDataStore			dataStore(0L);
	CTaggedDataGroup			dataGroup(&dataStore,sizeof(short),0);
	
	dataGroup.SetLock(true);
	
	// Save the map dimensions
	dataGroup.InsertTag(kDimensions);
	CDataStoreStream			dimStream(dataGroup.GetTag(kDimensions));
	
	dimStream << kDimensionsVersion;
	
	dimStream << GetMapWidth();
	dimStream << GetMapHeight();
	dimStream << GetTileWidth();
	dimStream << GetTileHeight();
	dimStream << GetTilesUsed();
	
	dimStream << (SInt8)mPicReference;
	dimStream << mPicId;
	dimStream << mPicName;
	
	// Save the layer content
	dataGroup.InsertTag(kLayerContent);
	CDataStoreStream			layerContentStream(dataGroup.GetTag(kLayerContent));

	layerContentStream << kLayerContentVersion;
	layerContentStream << CountLayers();
	
	TListIndexer	layersIndexerA(&mLayersList);
	while (CMapLayer *theLayer=(CMapLayer*)layersIndexerA.GetNextData())
		theLayer->SaveContent(layerContentStream);

	// Save the object content
	dataGroup.InsertTag(kObjectContent);
	CDataStoreStream			objectContentStream(dataGroup.GetTag(kObjectContent));
	
	mObjectsManager->SaveContent(objectContentStream);

	// Save the polygon content
	dataGroup.InsertTag(kPolygonContent);
	CDataStoreStream			polygonContentStream(dataGroup.GetTag(kPolygonContent));
	
	polygonContentStream << kPolygonContentVersion;
	polygonContentStream << CountPolygons();
	
	TListIndexer	polygonIndexerA(&mPolygonsList);
	while (CMapPolygon *thePolygon=(CMapPolygon*)polygonIndexerA.GetNextData())
		thePolygon->SaveContent(polygonContentStream);
	
	// Save the polygon info
	dataGroup.InsertTag(kPolygonInfo);
	CDataStoreStream			polygonInfoStream(dataGroup.GetTag(kPolygonInfo));
	
	polygonInfoStream << kPolygonInfoVersion;
	polygonInfoStream << CountPolygons();
	
	TListIndexer	polygonIndexerB(&mPolygonsList);
	while (CMapPolygon *thePolygon=(CMapPolygon*)polygonIndexerB.GetNextData())
		thePolygon->SaveInfo(polygonInfoStream);
	
	// Save the object info
	dataGroup.InsertTag(kObjectInfo);
	CDataStoreStream			objectInfoStream(dataGroup.GetTag(kObjectInfo));
	
	mObjectsManager->SaveInfo(objectInfoStream);
	
	// Save the layer info
	dataGroup.InsertTag(kLayerInfo);
	CDataStoreStream			layerInfoStream(dataGroup.GetTag(kLayerInfo));
	
	layerInfoStream << kLayerInfoVersion;
	layerInfoStream << CountLayers();
	
	TListIndexer	layersIndexerB(&mLayersList);
	while (CMapLayer *theLayer=(CMapLayer*)layersIndexerB.GetNextData())
		theLayer->SaveInfo(layerInfoStream);
	
	return dataStore.GetHandle(true);
}

#pragma mark -

bool CTNTMap::InMap(long x,long y,TMapLayerIndex z)
{
	if ((x>=0) && (x<GetMapWidth()) && (y>=0) && (y<GetMapHeight()) && (z>=0) && (z<CountLayers()))
		return true;
	else
		return false;
}

long CTNTMap::GetTile(long x,long y,TMapLayerIndex z)
{
	long	tile=-1;
	
	if (InMap(x,y,z))
		tile=GetLayer(z)->GetTile(x,y);

	return tile;
}

void CTNTMap::SetTile(long x,long y,TMapLayerIndex z,unsigned short tile)
{
	if (InMap(x,y,z))
		GetLayer(z)->SetTile(x,y,tile);
}

Size CTNTMap::GetTileDataSize()
{
	if (GetTilesUsed()>255)
		return sizeof(short);
	else
		return sizeof(char);
}

#pragma mark -

void CTNTMap::AddLayer(Str32 inName)
{
	CMapLayer		*layer=new CMapLayer(this,inName);
	
	mLayersList.AppendElement(layer->GetLink());
	
	mLayers++;
}

void CTNTMap::RemoveLayer(TMapLayerIndex inLayer)
{
	delete GetLayer(inLayer);
	mLayers--;
	
	mObjectsManager->RemoveLayer(inLayer);
	
	// Resort all the polygons
	TListIndexer	indexer(&mPolygonsList);
	while (CMapPolygon *polygon=(CMapPolygon*)indexer.GetNextData())
		polygon->DeleteLayer(inLayer);
}

void CTNTMap::MoveLayer(SInt32 inMoveThis,SInt32 inToHere)
{
	if ((inMoveThis==inToHere) || (inMoveThis-1==inToHere))
		return;

	// Move the layer
	CMapLayer	*moveLayer=(CMapLayer*)mLayersList.GetNthElementData(inMoveThis);
	mLayersList.MoveElement(moveLayer->GetLink(),inToHere+1,false);

	// Move the objects
	mObjectsManager->MoveLayer(inMoveThis,inToHere);
	
	// Move the layers
	TListIndexer	indexer(&mPolygonsList);
	
	while (CMapPolygon *thePolygon=(CMapPolygon*)indexer.GetNextData())
		thePolygon->MoveLayer(inMoveThis,inToHere);
}

void CTNTMap::SetLayerOpacity(TMapLayerIndex inLayer,TMapLayerOpacity inOpacity)
{
	GetLayer(inLayer)->SetOpacity(inOpacity);
}

TMapLayerOpacity CTNTMap::GetLayerOpacity(TMapLayerIndex inLayer)
{
	return GetLayer(inLayer)->GetOpacity();
}

void CTNTMap::GetLayerName(TMapLayerIndex inLayer,Str32 inName)
{
	GetLayer(inLayer)->GetName(inName);
}

void CTNTMap::SetLayerName(TMapLayerIndex inLayer,Str32 inName)
{
	GetLayer(inLayer)->SetName(inName);
}

#pragma mark -

void CTNTMap::AddPolygon(Str32 inName,RGBColour inColour)
{
	CMapPolygon		*polygon=new CMapPolygon(inName,inColour);
	
	mPolygonsList.AppendElement(polygon->GetLink());
	
	mPolygons++;
}

void CTNTMap::RemovePolygon(TMapPolygonIndex inPolygon)
{
	CMapPolygon	*p=GetPolygon(inPolygon);
	
	if (p)
	{
		delete p;	
		mPolygons--;
	}
}

void CTNTMap::SetPolygonColour(TMapPolygonIndex inPolygon,RGBColour inColour)
{
	CMapPolygon	*p=GetPolygon(inPolygon);
	
	if (p)
		p->SetColour(inColour);
}

RGBColour CTNTMap::GetPolygonColour(TMapPolygonIndex inPolygon)
{
	CMapPolygon	*p=GetPolygon(inPolygon);
	
	if (p)
		return p->GetColour();
	else
	{
		RGBColour	col={0,0,0};
		return col;
	}
}

void CTNTMap::GetPolygonName(TMapPolygonIndex inPolygon,Str32 outName)
{
	CMapPolygon	*p=GetPolygon(inPolygon);
	
	if (p)
		p->GetName(outName);
	else
		outName[0]=0;
}

bool CTNTMap::InPolygon(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z,TMapPolygonIndex inP)
{
	CMapPolygon	*p=GetPolygon(inP);
	
	if (p)
		return p->In(x,y,z);
	else
		return false;
}

TMapPolygonIndex CTNTMap::GetPolygonAt(
	unsigned short		inX,
	unsigned short		inY,
	TMapLayerIndex 		inZ)
{
	TMapPolygonIndex	polygons=CountPolygons();

	for (TMapPolygonIndex n=0; n<polygons; n++)
	{
		if (InPolygon(inX,inY,inZ,n))
			return n;
	}
	
	return -1;
}

#pragma mark -

CGenesisDataStore::CGenesisDataStore(Handle inHandle) : mHandle(),CMarkableDataStore(&mHandle)
{
	// If no handle was passed in then create one
	if (!inHandle)
	{
		inHandle=NewHandle(sizeof(short));
		**(short**)inHandle=kTaggedDataVersion;
	}
	
	// Is the handle passed in the tagged version
	if ((**(short**)inHandle)==kTaggedDataVersion)
	{
		mHandle.SetDataHandle(inHandle,false);
	}
	else
	{
		Throw_(kUnknownFileFormat);
	}
}