// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// File Dialog routines.c
// Mark Tully
// 11/3/96
// 12/9/96	:	Added an update proc thingy
// Contains routines for quickly using the Standard File package
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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


#include		"Blast.h"

#define		kTextPreviewItem	11		// see below
#define		kPicPreviewItem		10		// Make sure the ID of the user item to display the PICT
										// preview in is this

Boolean				gPicPreviewOn=false;	// signals whether a preview is on
Str255				gStringPreview="\p"; 	// can afford to keep this in mem, easier to read pic from disk when needed
short				gPicID,gStringID;
StandardFileReply	*gTheReply;
UpdateProc			gUpdateProc;
FSSpec				gPrevSpec={0,0L,"\p"}; // used to store the last file previewed so we can
											// tell if it's changed

// This routine allows the user to pick a file and shows them a text preview and a pic preview
// of the file as they go.
// For just a quick call, do this
// GetFileWithPreviews(dLogID,picID,stringID,&reply,0L,-1,0L);
// Parameters explained:
// dLogID - id of a DLOG resource, must be numbered in a particuar order (see Custom Open.rsrc)
// picID - id of the PICT to be shown as a preview from each file
// stringID - id of the string to be shown from each file
// reply - address of a StandardFileReply to fill out
// typeList - list of all the types that can be displayed
// 			  eg	SFTypeList	typeList={'TEXT','PICT'};
// listCount - number of items in above list. -1 will show all files
// fileFilter - a custom filter which will be called for each file of the type above before it is
//              added to the display. Defined like this:
//				FileFilterYDUPP theFilter=NewFileFilterYDProc(myFilterProc);
//				
// Example FilterProc: 
//
// Modify this to choose what files should be shown. Only files of types specified in the typeList
// will get this far. Return false to show the file, true to repress the file. Create a UPP of it
// and give it to GetFileWithPreviews.
//
// pascal Boolean FileProc(ParmBlkPtr a,void *data)
// {	
//	  return false;
// }

void GetFileWithPreviews(short dLogID,short picID,short stringID,StandardFileReply *reply,SFTypeList typeList,short listCount,FileFilterUPP fileFilter,UpdateProc theUpdate)
{
	Point				place={-1,-1};
	DlgHookUPP			dLogHook=NewDlgHookYDProc(DHookProc);
	ModalFilterYDUPP	evFilter=NewModalFilterYDProc(MR_EventHandler);

	gUpdateProc=theUpdate;
	gPicID=picID;
	gStringID=stringID;
	gTheReply=reply;
	
	CustomGetFile(fileFilter,listCount,typeList,reply,dLogID,place,dLogHook,evFilter,0L,0L,0L);
}

// This update routine allows me to update my preview items when necessary
pascal Boolean MR_EventHandler(DialogPtr theDialog,EventRecord *theEvent,short *itemHit,void *dataPtr)
{
	if (theEvent->what==updateEvt)
	{
		if ((WindowPtr)theEvent->message==theDialog)
		{
			if (gPicPreviewOn) // if theres a pic preview on, update it
				ShowPreview(theDialog);
			else
				UpdatePreviewItem(theDialog,0L);
		}
		else
			BlastUpdate((WindowPtr)theEvent->message,gUpdateProc);
	}
	
	// returning false means the dialog manager will call beginupdate and endupdate
	return false; 
}

// This dialog hook allows me to check the currently selected file on null events and take
// action if it has changed
pascal short DHookProc(short item,DialogPtr inDialog,void *dataPtr)
{
	if (item==sfHookNullEvent && gTheReply->sfFile.name[0]!=0) // if periodic check and selected
	{
		// is it a different item
		// The following line checks to see if the file is the same. It checks the thing which is most likely
		// to have changed first (the name) and then the others. By checking them all like this, it means
		// that you can select a file with the same name as the current one, only on a different disk
		// and the pic will still work
		if ((!CmpPStr(gPrevSpec.name,gTheReply->sfFile.name)) || (gPrevSpec.parID!=gTheReply->sfFile.parID) || (gPrevSpec.vRefNum!=gTheReply->sfFile.vRefNum))
		{			
			// Right, it's a different item
			ShowPreview(inDialog);
		}
	}
	
	return item;
}

// shows a preview of the currently selected file
void ShowPreview(DialogPtr inDialog)
{
	if (!(gTheReply->sfIsFolder || gTheReply->sfIsVolume)) // if either of these are true then it's not a file
	{
		// get the pic from the file if one exists	
		PicHandle		thePic=0L;
		short			fRef=0,err=0;
		
		fRef=HOpenResFile(gTheReply->sfFile.vRefNum,gTheReply->sfFile.parID,gTheReply->sfFile.name,fsRdPerm); // only need read permission
		if (fRef<0) // failed if <0
			fRef=0;
		
		if (fRef) // opened
		{
			Handle		string=0L;
					
			// get pic
			thePic=(PicHandle)Get1Resource('PICT',gPicID);
			err=ResError(); // debug puposes only
		
			// get string
			string=Get1Resource('STR ',gStringID);
			if (string)
				CopyPString((StringPtr)*string,gStringPreview);
			else
				gStringPreview[0]=0;
			
			err=ResError(); // debug puposes only
		}
				
		UpdatePreviewItem(inDialog,thePic);
						
		if (fRef) // only close if open
		{
			CloseResFile(fRef);
			fRef=0;
		}
	}
	else // just clicked on a non-file, clear preview items
	{
		gStringPreview[0]=0; // clear the string
		UpdatePreviewItem(inDialog,0L);
	}
	gPrevSpec=gTheReply->sfFile;
}

// Redraws the preview items. Pass 0L for the pic if you want it clear.
void UpdatePreviewItem(DialogPtr inDialog,PicHandle thePic)
{	
	GrafPtr			savePort;
	Rect			picRect;
	
	GetPort(&savePort);
	SetPort(inDialog);
	
	// Frame the rect
	picRect=ItemRect(inDialog,kPicPreviewItem); // id of pic rect
	FrameRect(&picRect);
	InsetRect(&picRect,2,2);

	EraseRect(&picRect);
	if (thePic) // got a pic then draw it
	{
		gPicPreviewOn=true;
		ScalePic(thePic,&picRect,false);
		DrawPicture(thePic,&picRect);
	}
	else
		gPicPreviewOn=false;

	// And the text
	SetIText((Handle)ItemHandle(inDialog,kTextPreviewItem),gStringPreview);
	
	SetPort(savePort);
}


// *********************************************************************************************
// END OF CUSTOM FILE OPEN
// *********************************************************************************************

OSErr ChooseAndOpenFile(short *theRef,FSSpec *theSpec,Boolean resFork)
{
	StandardFileReply		reply;
	OSErr					err=noErr;
	
	*theRef=0;

	StandardGetFile(0L,-1,0L,&reply);
	
	if (!reply.sfGood)
		return userCanceledErr;

	*theSpec=reply.sfFile; // copy the spec
	
	if (resFork) // open the res fork
	{
		// was previously:
		// err=FSpOpenRF(theSpec,fsRdWrPerm,theRef);
		// however this opens the file without reading the resource map into memory. This trap
		// is intended to open the fork for copying or direct access, you don't get to access
		// the resources. Guide, any trap saying RF means resource fork (don't get access to
		// resources), any saying ResFile, you do. Hence now I use:
		*theRef = HOpenResFile(theSpec->vRefNum,theSpec->parID,theSpec->name,fsRdWrPerm);
		if (*theRef==-1)
			err=ResError();
	}
	else
		err=FSpOpenDF(theSpec,fsRdWrPerm,theRef);

	if (err)
		*theRef=0;

	return err;
}
