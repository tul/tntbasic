// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CMapObjects.cpp
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

#include	"CMapObjects.h"
#include	"CTNTMap.h"

CMapObjectsManager::CMapObjectsManager(CTNTMap *inTmap)
{
	tmap=inTmap;
}

CMapObjectsManager::~CMapObjectsManager()
{

}

void CMapObjectsManager::LoadContent(CDataStoreStream &inStream)
{
	TVersion	version;
	
	inStream >> version;
	
	if (version==CTNTMap::kObjectContentVersion)
	{
		TMapObjectIndex	objectCount;
		inStream >> objectCount;
		
		for (TMapObjectIndex n=0; n<objectCount; n++)
		{
			CMapObject		*object=new CMapObject(inStream);
			
			objects.AppendElement(object->GetLink());
		}
	}
	else
		Throw_(kUnknownFileFormat);
}

void CMapObjectsManager::LoadInfo(CDataStoreStream &inStream)
{
	TVersion	version;
	
	inStream >> version;

	if (version==CTNTMap::kObjectInfoVersion)
	{
		TMapObjectTypeIndex	typeCount;
		inStream >> typeCount;
		
		for (TMapObjectTypeIndex n=0; n<typeCount; n++)
		{
			CMapObjectType		*type=new CMapObjectType(inStream);
			
			types.AppendElement(type->GetLink());
		}
	}
	else
		Throw_(kUnknownFileFormat);
}

void CMapObjectsManager::SaveContent(CDataStoreStream &inStream)
{
	inStream << CTNTMap::kObjectContentVersion;

	// Count the number of objects inside the map
	TMapObjectIndex	count=0;
	TListIndexer	indexerA(&objects);
	while (CMapObject *theObject=(CMapObject*)indexerA.GetNextData())
	{
		if ((theObject->GetX()>=0) || (theObject->GetX()<tmap->GetMapWidth()) ||
			(theObject->GetY()>=0) || (theObject->GetY()<tmap->GetMapHeight()))
		{
			count++;
		}
	}

	inStream << count;
	
	// Save the objects inside the map
	TListIndexer	indexerB(&objects);
	while (CMapObject *theObject=(CMapObject*)indexerB.GetNextData())
	{
		if ((theObject->GetX()>=0) || (theObject->GetX()<tmap->GetMapWidth()) ||
			(theObject->GetY()>=0) || (theObject->GetY()<tmap->GetMapHeight()))
		{
			theObject->SaveContent(inStream);
		}
	}
}

void CMapObjectsManager::SaveInfo(CDataStoreStream &inStream)
{
	inStream << CTNTMap::kObjectInfoVersion;

	inStream << CountTypes();
	
	TListIndexer	indexer(&types);
	while (CMapObjectType *theType=(CMapObjectType*)indexer.GetNextData())
		theType->SaveInfo(inStream);
}

#pragma mark -

void CMapObjectsManager::MoveLayer(SInt32 inMoveThis,SInt32 inToHere)
{
	TListIndexer	indexer(&objects);
	
	if (inMoveThis<inToHere)
	{
		// Move layer upwards
		while (CMapObject *theObject=(CMapObject*)indexer.GetNextData())
		{
			if (theObject->GetZ()==inMoveThis)
				theObject->SetZ(inToHere);
			else if ((theObject->GetZ()>inMoveThis) && (theObject->GetZ()<=inToHere))
				theObject->SetZ(theObject->GetZ()-1);
		}
	}
	else
	{
		// Move layer downwards
		while (CMapObject *theObject=(CMapObject*)indexer.GetNextData())
		{
			if (theObject->GetZ()==inMoveThis)
				theObject->SetZ(inToHere+1);
			else if ((theObject->GetZ()>inToHere) && (theObject->GetZ()<inMoveThis))
				theObject->SetZ(theObject->GetZ()+1);
		}
	}
}

void CMapObjectsManager::RemoveLayer(TMapLayerIndex inLayer)
{
	TListIndexer	indexer(&objects);
	
	while (CMapObject *theObject=(CMapObject*)indexer.GetNextData())
	{
		if (theObject->GetZ()==inLayer)
			delete theObject;
		else if (theObject->GetZ()>inLayer)
			theObject->SetZ(theObject->GetZ()-1);
	}
}

bool CMapObjectsManager::Copy(CMapObjectsManager *copyThis)
{
	// Remove the old stuff
	TListIndexer	objectsIndexer(&objects);
	while (CMapObject *theObject=(CMapObject*)objectsIndexer.GetNextData())
		delete theObject;
		
	TListIndexer	typesIndexer(&types);
	while (CMapObjectType *theType=(CMapObjectType*)typesIndexer.GetNextData())
		delete theType;

	// Copy the new stuff
	TListIndexer	copyObjects(&copyThis->objects);
	while(CMapObject *theObject=(CMapObject*)copyObjects.GetNextData())
	{
		CMapObject		*newObject=new CMapObject(*theObject);
		
		if (!newObject)
			return false;
		
		objects.AppendElement(newObject->GetLink());
	}

	TListIndexer	copyTypes(&copyThis->types);
	while(CMapObjectType *theType=(CMapObjectType*)copyTypes.GetNextData())
	{
		CMapObjectType		*newObjectType=new CMapObjectType(*theType);
		
		if (!newObjectType)
			return false;
		
		types.AppendElement(newObjectType->GetLink());
	}

	return true;
}

#pragma mark -

void CMapObjectsManager::SelectAllObjects()
{
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
		object->Select();
}

void CMapObjectsManager::DeselectAllObjects()
{
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
			object->Deselect();
	}
}

void CMapObjectsManager::SelectObjectsRect(TMapTileIndex top,TMapTileIndex left,TMapTileIndex bottom,TMapTileIndex right)
{
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if ((object->GetX()>=left) && (object->GetX()<right) &&
			(object->GetY()>=top) && (object->GetY()<bottom))
		{
			object->Select();
		}
	}
}

void CMapObjectsManager::ToggleSelectObjectsRect(TMapTileIndex top,TMapTileIndex left,TMapTileIndex bottom,TMapTileIndex right)
{
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if ((object->GetX()>=left) && (object->GetX()<right) &&
			(object->GetY()>=top) && (object->GetY()<bottom))
		{
			object->ToggleSelect();
		}
	}
}

void CMapObjectsManager::DeleteSelectedObjects()
{
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
			delete object;
	}
}

SInt32 CMapObjectsManager::GetSelectionLeft()
{
	SInt32			amount=0x0FFFFFFF;
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
		{
			if (object->GetX()<amount)
				amount=object->GetX();
		}
	}
	
	return amount;
}

SInt32 CMapObjectsManager::GetSelectionTop()
{
	SInt32			amount=0x0FFFFFFF;
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
		{
			if (object->GetY()<amount)
				amount=object->GetY();
		}
	}
	
	return amount;
}

SInt32 CMapObjectsManager::GetSelectionRight()
{
	SInt32			amount=0;
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
		{
			if (object->GetX()>amount)
				amount=object->GetX();
		}
	}
	
	return amount;
}

SInt32 CMapObjectsManager::GetSelectionBottom()
{
	SInt32			amount=0;
	TListIndexer	indexer(&objects);

	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
		{
			if (object->GetY()>amount)
				amount=object->GetY();
		}
	}
	
	return amount;
}

TMapObjectIndex CMapObjectsManager::CountSelectedObjects()
{
	TMapObjectIndex	amount=0;
	TListIndexer	indexer(&objects);
	
	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
			amount++;
	}
	
	return amount;
}

bool CMapObjectsManager::ValidSelection()
{
	TListIndexer	indexer(&objects);
	
	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
			return true;
	}
	
	return false;
}

void CMapObjectsManager::MoveSelection(SInt32 inX,SInt32 inY)
{
	TListIndexer	indexer(&objects);
	
	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->Selected())
			object->Move(inX,inY);
	}
}

#pragma mark -

void CMapObjectsManager::AddObject(long x,long y,TMapLayerIndex z,TMapObjectTypeIndex inObject,bool inSelected)
{
	TListIndexer		indexer(&objects);
	
	// If a matching object is already there then don't put in another
	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if ((object->GetX()==x) && (object->GetY()==y) && (object->GetZ()==z) && (object->GetType()==inObject))
			return;
	}
	
	// Put in the new object
	CMapObject		*object=new CMapObject(x,y,z,inObject);
	
	if (object)
		objects.AppendElement(object->GetLink());
		
	if (inSelected)
		object->Select();
}

void CMapObjectsManager::RemoveObject(long x,long y,TMapLayerIndex z)
{
	TListIndexer		indexer(&objects);
	
	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if ((object->GetX()==x) && (object->GetY()==y) && (object->GetZ()==z))
			delete object;
	}
}

TMapObjectTypeIndex CMapObjectsManager::GetObjectType(long x,long y,TMapLayerIndex z)
{
	TListIndexer		indexer(&objects);
	
	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if ((object->GetX()==x) && (object->GetY()==y) && (object->GetZ()==z))
			return object->GetType();
	}
	
	return kInvalidObject;
}

void CMapObjectsManager::AddObjectType(Str32 inName,const RGBColour &inColour)
{
	CMapObjectType	*type=new CMapObjectType(inName,inColour);

	if (type)
		types.AppendElement(type->GetLink());
}

void CMapObjectsManager::RemoveObjectType(TMapObjectTypeIndex inObject)
{
	delete GetObjectType(inObject);
	
	TListIndexer		indexer(&objects);
	
	while (CMapObject *object=(CMapObject*)indexer.GetNextData())
	{
		if (object->GetType()==inObject)
			delete object;
		else if (object->GetType()>inObject)
			object->SetType(object->GetType()-1);
	}
}

SInt32 CMapObjectsManager::GetNthObjectType(TMapObjectIndex inIndex)
{
	CMapObject	*object=GetObject(inIndex);

	if (object)
		return object->GetType(); 

	return 0L;
}

SInt32 CMapObjectsManager::GetNthObjectX(TMapObjectIndex inIndex)
{
	CMapObject	*object=GetObject(inIndex);

	if (object)
		return object->GetX(); 

	return 0L;
}

SInt32 CMapObjectsManager::GetNthObjectY(TMapObjectIndex inIndex)
{
	CMapObject	*object=GetObject(inIndex);

	if (object)
		return object->GetY(); 

	return 0L;
}

SInt32 CMapObjectsManager::GetNthObjectZ(TMapObjectIndex inIndex)
{
	CMapObject	*object=GetObject(inIndex);

	if (object)
		return object->GetZ(); 

	return 0L;
}

#pragma mark -

CMapObjectType::CMapObjectType(const Str32 inName,const RGBColour inColour) : link(this)
{
	CopyPStr(inName,name);
	colour=inColour;
}

CMapObjectType::CMapObjectType(CMapObjectType &inType) : link(this)
{
	CopyPStr(inType.name,name);
	inType.GetColour(colour);
}

CMapObjectType::CMapObjectType(CDataStoreStream &inStream) : link(this)
{
	inStream >> colour.red >> colour.green >> colour.blue;
	inStream >> name;
}

void CMapObjectType::SaveInfo(CDataStoreStream &inStream)
{
	inStream << colour.red << colour.green << colour.blue;
	inStream << name;
}

#pragma mark -

CMapObject::CMapObject(TMapTileIndex inX,TMapTileIndex inY,TMapLayerIndex inZ,TMapObjectTypeIndex inType) : link(this)
{
	x=inX;
	y=inY;
	z=inZ;
	type=inType;
	selected=false;
}

CMapObject::CMapObject(CMapObject &inObject) : link(this)
{
	x=inObject.x;
	y=inObject.y;
	z=inObject.z;
	type=inObject.type;
	selected=inObject.selected;
}

CMapObject::CMapObject(CDataStoreStream &inStream) : link(this)
{
	inStream >> x >> y >> z;
	inStream >> type;
	selected=false;
}

void CMapObject::SaveContent(CDataStoreStream &inStream)
{
	inStream << x << y << z;
	inStream << type;
}

void CMapObject::Scale(long inNewWidth,long inNewHeight,long inLeft,long inTop)
{
	x+=inLeft;
	y+=inTop;
}

void CMapObject::Move(SInt32 inX,SInt32 inY)
{
	x+=inX;
	y+=inY;
}