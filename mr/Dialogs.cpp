// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Dialogs.cpp
// Mark Tully
// Dark ages
// 30/5/96
// 10/7/96 : IsNumber routine added
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

#include	"Marks Routines.h"
#include	"Utility Objects.h"

static unsigned long	gLastClick=0L;

// Private procs used by ForceDialogSelect
Boolean ForceDialogSelectMouseDown(EventRecord *event,DialogPtr dialog,short *itemHit);
Boolean ForceDialogSelectKeyDown(EventRecord *event,DialogPtr dialog,short *itemHit);

// checks if the keypress will produce a valid number checking the for signs (if allowed)
// also allows arrows. Does not allow tab, or any copy cut paste etc quick keys. Handle these 
// yourself before calling IsNumber.
Boolean IsNumber(DialogPtr theWindow,char theChar,Boolean allowSigns)
{
	if (theChar>='0' && theChar<='9')
		return true;
	else if (IsArrow(theChar))
		return true;
	
	if (allowSigns)
	{
		// now allow inputting of a sign (+/-) only under certain conditions. These are if the char will
		// be the first and that there is not already a +/- ath te front
		if ((theChar=='-' || theChar=='+') && CaretPos(DialogTE(theWindow))==0)
		{
			// now check that there's not already a +/- there or if there is it's going to get replaced
			if ((**DialogTE(theWindow)).selEnd>0 || (**DialogTE(theWindow)).teLength==0 ||
			( (*(**DialogTE(theWindow)).hText)[0]!='-' && (*(**DialogTE(theWindow)).hText)[0]!='+' ))
				return true;
		}
	}
	
	return false;
}

// Copies text from a dialogs edit text item
void GetItemText(DialogPtr theDLog,short item,Ptr destination)
{	
	short len=GetItemTextLength(theDLog,item);
	
	BlockMove(*((**(GetDialogTextEditHandle(theDLog))).hText),destination,(Size)len);
	((StringPtr)destination)[len]=0;
}

// Get the text length of a dialogs edit text item
short GetItemTextLength(DialogPtr theDlog,short theItem)
{
	SelIText(theDlog,theItem,0,0);
	return (**GetDialogTextEditHandle(theDlog)).teLength;
}

Rect ItemRect(DialogPtr theDLog,short item)
{
	DItemStuff;	
	
	GetDItem(theDLog,item,&iType,&iHandle,&iRect);
	
	return iRect;
}

// Moves a dialog item. If it's a control, it moves the control too.
void SetItemRect(DialogPtr theDLog,short item,Rect *rect)
{
	DItemStuff;

	GetDItem(theDLog,item,&iType,&iHandle,&iRect);
	SetDItem(theDLog,item,iType,iHandle,rect);
	
	if (iType&kControlDialogItem)
	{
		MoveControl((ControlHandle)iHandle,rect->left,rect->top);
		SizeControl((ControlHandle)iHandle,FRectWidth(*rect),FRectHeight(*rect));
	}
}

void MoveItem(DialogPtr theDLog,short item,short xShift,short yShift)
{
	Rect		iRect=ItemRect(theDLog,item);
	FOffset(iRect,xShift,yShift);
	SetItemRect(theDLog,item,&iRect);
}

void ToggleCBox(DialogPtr theDLog,short item,Boolean *boo)
{
	short			val;
	ControlHandle	iHandle=ItemHandle(theDLog,item);
	
	if (GetControlValue(iHandle))
		val=false;
	else
		val=true;
		
	SetControlValue(iHandle,val);
	
	if (boo)
		*boo=!*boo;
}

// Turns off gadgets between hi & lo and turns ON back on
void DoRadioGroup(DialogPtr dlg, short lo, short hi, short on)
{
	short i;
	
	for (i=lo;i<=hi;++i)
		SetItemValue(dlg,i, 0);
	SetItemValue(dlg,on, 1);
}

// Returns the radio button in the group which is hilited
short GetRadioFromGroup(DialogPtr dlg, short lo, short hi)
{
	short i;
	
	for (i=lo;i<=hi;++i) {
		if (ItemValue(dlg,i))
			return i;
	}
	return 0;
}

void MR_EnableControl(DialogPtr dlg, short controlNumber)
{
	HiliteControl(ItemHandle(dlg,controlNumber), 0);
}

void FrameButton(DialogPtr inDialog,short item)
{
	GrafPtr	savePort;
	Rect	frame;
	
	GetPort(&savePort);
	SetPortDialogPort(inDialog);
	frame=ItemRect(inDialog,item);
	InsetRect(&frame,-4,-4);
	PenSize(3,3);
	FrameRoundRect(&frame,16,16);
	PenSize(1,1);
	SetPort(savePort);
}

void FlashControl(
	WindowPtr 	inWindow,
	ControlRef 	inControl)
{
	unsigned long delay;
	ControlHandle	iHandle;
	GrafPtr		savePort;
	
	GetPort(&savePort);
	SetPortWindowPort(inWindow);
	HiliteControl(inControl,kControlButtonPart);
	QDFlushPortBuffer(GetWindowPort(inWindow),0);
	Delay(8L,&delay);
	HiliteControl(inControl,kControlNoPart);
	QDFlushPortBuffer(GetWindowPort(inWindow),0);
	SetPort(savePort);
}

void FlashButton(DialogPtr inDialog,short item)
{
	unsigned long delay;
	ControlHandle	iHandle;
	GrafPtr		savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(inDialog);
	iHandle=ItemHandle(inDialog,item);	
	HiliteControl(iHandle,1);
	QDFlushPortBuffer(GetDialogPort(inDialog),0);
	Delay(8L,&delay);
	HiliteControl(iHandle,0);
	QDFlushPortBuffer(GetDialogPort(inDialog),0);
	SetPort(savePort);
}

// gets the content rect of a window in global coords
Rect GetGWindowContentRect(WindowPtr theWindow)
{
	GrafPtr		savePort;
	Rect		theRect;
	
	GetPort(&savePort);
	SetPortWindowPort(theWindow);
	
#if TARGET_API_MAC_CARBON
	::GetWindowPortBounds(theWindow,&theRect);
#else
	theRect=theWindow->portRect;
#endif
	LocalToGlobalRect(&theRect);
	
	SetPort(savePort);
	
	return theRect;
}

// failed
/*
// finds the height of a windows title bar
short GetWinTopHeight(WindowPtr theWindow)
{
	Rect		contentRect,boundingRect;
	
	// gets the two rects in global coords
	contentRect=GetGWindowContentRect(theWindow);
	boundingRect=GetGWindowBoundingRect(theWindow);

	return contentRect.top-boundingRect.top;
}

// finds the width of a windows left side
short GetWinSideWidth(WindowPtr theWindow)
{
	Rect		contentRect,boundingRect;
	
	// gets the two rects in global coords
	contentRect=GetGWindowContentRect(theWindow);
	boundingRect=GetGWindowBoundingRect(theWindow);

	return contentRect.left-boundingRect.left;
}*/

// failed
/*
// gets the bouding rect of a window in global coords. This rect includes the frame and title bar
// of the rect etc. it only works for windows that are vis
Rect GetGWindowBoundingRect(WindowPtr theWindow)
{
	if (((WindowPeek)theWindow)->spareFlag) // if it can zoom
		return (**(WStateDataHandle)(**((WindowPeek)theWindow)->dataHandle)).userState;
	else
		return theWindow->portRect;
}
*/

// used to stagger windows around the screen by offsetting it from the frontmost window
void StaggerWindow(WindowPtr theWindow,short staggerDist)
{
	WindowPtr	frontWindow=FrontWindow();
	Rect		frontRect,newRect;

#if TARGET_API_MAC_CARBON	
	Rect		theWindowPortRect;
	::GetWindowPortBounds(theWindow,&theWindowPortRect);
	short		width=theWindowPortRect.right-theWindowPortRect.left;
	short		height=theWindowPortRect.bottom-theWindowPortRect.top;
#else	
	short		width=theWindow->portRect.right-theWindow->portRect.left;
	short		height=theWindow->portRect.bottom-theWindow->portRect.top;
#endif
	GrafPtr		savePort;
	
	// no windows open, slap the new in the top left
	if (!frontWindow)
	{		
		MoveWindow(theWindow,staggerDist+3,
					staggerDist+::GetMBarHeight()+18,false);
		return;
	}

	// find the current windows rect in global coords
	GetPort(&savePort);
	SetPortWindowPort(frontWindow);
#if TARGET_API_MAC_CARBON
	::GetWindowPortBounds(frontWindow,&frontRect);
#else
	frontRect=frontWindow->portRect;
#endif
	LocalToGlobalRect(&frontRect);
	
	// Now off set the rect
	newRect.top=frontRect.top;	// level
	newRect.left=frontRect.right+staggerDist;
	newRect.bottom=newRect.top+height;
	newRect.right=newRect.left+width;
	
	Rect		screenBounds;
	
#if TARGET_API_MAC_CARBON
	BitMap		screenBits;
	
	::GetQDGlobalsScreenBits(&screenBits);
	screenBounds=screenBits.bounds;
#else
	screenBounds=qd.screenBits.bounds;
#endif
	
	// check it's not legged it off screen to the right. If it has, move it to the far left of the
	// screen and indent it a little and move it down from it's current pos
	if (newRect.right>screenBounds.right)
	{
		newRect.left=staggerDist+3; // a little indent from the edge
		newRect.right=newRect.left+width;
		newRect.top+=staggerDist+::GetMBarHeight()+18; // 18 is about the height of the title bar
		newRect.bottom=newRect.top+height;
	}
	
	// if it's gone to low or is still too far right, then shove it in the top left corner
	if (newRect.bottom>screenBounds.bottom || newRect.right>screenBounds.right)
	{
		ZeroRectCorner(newRect);
		FOffset(newRect,staggerDist+3,staggerDist+::GetMBarHeight()+18);
	}
	
	// Move the window taking into account the current menu bar height and the windows frame
	MoveWindow(theWindow,newRect.left,newRect.top,false);
	
	// reset the port
	SetPort(savePort);
}

void DrawProgressBar(DialogPtr theDialog,short item,unsigned long min,unsigned long max,unsigned long val)
{
	DItemStuff;
	Rect		barRect,blankRect;
	float		onePart,width,range;
	GrafPtr		savePort;
	PenState	savePen;
	RGBColor	greyColor=cGrey,blueColor=cBlue,oldBack,oldFore;
	
	// Switch ports
	GetPort(&savePort);
	SetPortDialogPort(theDialog);
	
	// Save pen and color data
	GetPenState(&savePen);
	GetBackColor(&oldBack);
	GetForeColor(&oldFore);
	
	// clip values
	if (val<min)
		val=min;
	else if (val>max)
		val=max;
	
	// Get the bar's dimensions
	GetDItem(theDialog,item,&iType,&iHandle,&iRect);
	width=iRect.right-iRect.left;
	range=max-min;
	onePart=width/range;
	barRect=iRect;
	InsetRect(&barRect,1,1);
	blankRect=barRect;

	// the bar is currently filling entire prog bar (barRect = blankRect)
	// Note that if val=max then the entire bar must be filled
	if (val!=max)			// if it isn't supposed to be...
		barRect.right=barRect.left+onePart*(val-min);
	
	// barRect.right must be at least 1 more than barRect.left for us to deduct 1 from barRect.right
	// and not get a negative rect
	if ((barRect.right>barRect.left) && (val!=max))
		barRect.right--;

	blankRect.left=barRect.right;
	
	// Frame the entire bar
	PenNormal();
	FrameRect(&iRect);

	// It looks better to paint the bar first
	
	// Paint the filled region (theBar)
	RGBForeColor(&greyColor);
	PaintRect(&barRect);
	
	// Paint the empty region
	RGBForeColor(&blueColor);
	PaintRect(&blankRect);

	// Restore all
	RGBForeColor(&oldFore);
	RGBBackColor(&oldBack);
	SetPenState(&savePen);	
	SetPort(savePort);
}

void MR_DisableControl(DialogPtr dlg, short controlNumber)
{
	HiliteControl(ItemHandle(dlg,controlNumber), 255);
}

Boolean IsItemEnabled(DialogPtr dlg,short item)
{
	short		type=ItemType(dlg,item);
	
	if (type&kControlDialogItem)
	{
		// Is a control, read from control record
		return GetControlHilite(ItemHandle(dlg,item))!=255;
	}
	
	// Is an item, read from dlog manager
	return !(type&kItemDisableBit);
}

short ItemValue(DialogPtr theDLog,short item)
{
	return GetControlValue(ItemHandle(theDLog,item));
}

void SetItemValue(DialogPtr theDLog,short item,short state)
{
	SetControlValue(ItemHandle(theDLog,item),state);
}

// A quick routine which takes an error ID and converts it to a string and puts up a dialog
// with the appropiate message. Returns the button hit
short MiscError(short resID,StringPtr message,StringPtr appName,OSErr errID,Boolean fatal)
{
	Str255	errStr="\p";
	short	res;

	if (errID!=0)
		NumToString((long)errID,errStr);
	else // if 0 the put a ?
	{
		errStr[1]='?';
		errStr[0]=1;
	}

	ParamText(message,errStr,appName,0L);
	
	res=Alert(resID,0L);

	if (fatal)
		ExitToShell();

	return res;
}

// either get the return or use the ptr
long TEToNumber(DialogPtr theDialog,short theItem,long *theRes)
{
	Str255	temp;
	long	tempLong;
	
	GetIText((Handle)ItemHandle(theDialog,theItem),temp);
	StringToNum(temp,&tempLong);

	if (theRes)
		*theRes=tempLong;
	return tempLong;
}

void NumberToTE(DialogPtr theDialog,short theItem,long theNum)
{
	Str15	temp;
	
	NumToString(theNum,temp);
	SetIText((Handle)ItemHandle(theDialog,theItem),temp);
}

// you've met FindDItem. It's a git. It returns the arithmetically lowest item and also returns
// disabled items. We don't care about them. Here's a better one.
// if highest is true it returns the arithmetically highest instead of the lowest
// it returns the dlog item number (unlike FindDItem which returns number -1) and returns 0 for
// none
short BetterFindDItem(DialogPtr theDialog,Point thePoint,Boolean highest,Boolean ignoreDisabled)
{
	short		numItems=CountDITL(theDialog);
	Rect		itemRect;
	short		found=0,counter;
	
	for (counter=1; counter<=numItems; counter++)
	{
		// for each item see if the point's in the rect
		itemRect=ItemRect(theDialog,counter);
		if (FPointInRect(thePoint,itemRect))
		{
			// does this item have to be enabled
			if (ignoreDisabled && ItemType(theDialog,counter)==itemDisable)
			{
				// disabled item
			}
			else
			{
				found=counter;
			
				// found one if we're looking for the arithmetically lowest return now
				if (!highest)
					return found;
			}
		}
	}
	
	return found;
}

// this takes a Str255, no less. See SimulateKeyPressInWindow
Boolean SimulateKeyPressInDialog(DialogPtr theDialog,StringPtr theString,char theChar)
{
	Size		buffSize=255;
	TEHandle	theTE;

	// first, get the current text edit item (where the text would go)
	theTE=DialogTE(theDialog);
	
	if (SimulateKeyPressInWindow(GetDialogWindow(theDialog),theTE,0L,theChar,(Ptr)&theString[1],&buffSize))
	{
		theString[0]=buffSize;
		return true;
	}
	else
		return false;
}

// a very useful fuction which returns a string containing what the contents of a text edit box would
// be if the char passed was typed using BetterTEKey. It handles forward delete and paste from clip board.
// the idea is to create another text edit item with the same charactoristics and then do the key
// press in that
// returns false if it ran out of mem (very unlikely)
// if you're using dialogs then see SimulateKeyPressInDialog
// theWindow is the window to create the temp te in
// theTE is the te to simulate the key press in
// char is the key pressed
// then....
// ptr is a buffer to get the new text
// buffsize is the size of the buffer, it comes out with the length
// or....
// pass the address of an tehandle (set to 0L) in newTE  which you want the temp te returned in then
// get the data yourself. Then dispose of the TE yourself unless you like leaking memory.
// if the char is the constant kSpecialPasteChar (defined in Marks Routines.h" then it pastes the scrap
Boolean SimulateKeyPressInWindow(WindowPtr theWindow,TEHandle theTE,TEHandle *newTE,char keyPress,Ptr buffer,Size *bufferSize)
{
	TEHandle		tempTE;
	Rect			newRect;
	Size			copySize;
	Boolean			disposeIt=(newTE==0L);
	
	if (!newTE)
		newTE=&tempTE;
	
	// now create a new te off screen
	newRect=(**theTE).viewRect;
#if TARGET_API_MAC_CARBON
	Rect			pr;
	::GetWindowPortBounds(theWindow,&pr);
	FOffset(newRect,pr.right+5,pr.bottom+5);
#else
	FOffset(newRect,theWindow->portRect.right+5,theWindow->portRect.bottom+5);
#endif
	*newTE=TENew(&newRect,&newRect);
	if (!*newTE)
		return false;
	
	// Get the text from the current te and put it in the new one
	TESetText(*(**theTE).hText,(**theTE).teLength,*newTE);
	
	// make the selection the same aswell
	TESetSelect((**theTE).selStart,(**theTE).selEnd,*newTE);

	BetterTEKey(keyPress,*newTE);

	// get the new string
	if (bufferSize && buffer)
	{
		*bufferSize=Lesser(*bufferSize,(***newTE).teLength);
		BlockMove(*(***newTE).hText,buffer,*bufferSize);
	}
	
	// destroy the hidden te rec
	if (disposeIt)
		TEDispose(*newTE);
	
	return true;
}

// Like the standard TEKey except handles paste and forward delete
void BetterTEKey(char keyPress,TEHandle handle)
{
	// process the key press
	if (keyPress==kSpecialPasteChar)
		TEPaste(handle);
	else if (keyPress==kForDelChar) // don't let these go into TEKey doesn't do a delete, it just put in a char	
	{
		// if the selection is non-null then simply map the for del char onto a del char
		if ((**handle).selStart!=(**handle).selEnd)
			TEKey(kDeleteChar,handle);
		else
		{
			// if the caret is not at the end of the te then move it right and delete
			if ((**handle).selStart<(**handle).teLength)
			{
				TEKey(kRightChar,handle);
				TEKey(kDeleteChar,handle);
			}
		}
	}
	else
		TEKey(keyPress,handle);
}

// takes a control handle and finds it's id in the dialog
// if it can't find it it returns 0
short ControlHandleToDialogId(DialogPtr theDialog,ControlHandle theControl)
{
	short	max,counter;
	
	max=CountDITL(theDialog);
	
	for (counter=1; counter<=max; counter++)
	{
		if (theControl==ItemHandle(theDialog,counter))
			return counter;
	}
	
	return 0;
}

void ErrorToString(OSErr theErr,StringPtr errStr)
{
	if (theErr!=0)
		NumToString((long)theErr,errStr);
	else // if 0 the put a ?
	{
		errStr[1]='?';
		errStr[0]=1;
	}
}

void FrameTE(DialogPtr dialog,short item)
{
	UPortSaver				safe2(dialog);
	Rect					rect=ItemRect(dialog,item);
	UForeColourSaver		safe;
		
	FInsetRect(rect,-1,-1);
	RGBForeColour(&TColourPresets::kWhite);
	FrameRect(&rect);
	FInsetRect(rect,-1,-1);
	FrameRect(&rect);
	FInsetRect(rect,-2,-2);
	InverseShadowBox(&rect);
}

// changes the font of an edit item to that of the dialog
void SetEditItemFont(DialogPtr theDialog,short theItem)
{
	FontInfo	fInfo;
	GrafPtr		savePort;
	
	GetPort(&savePort);
	SetPortDialogPort(theDialog);
  
  	// sel it to make it's TEHandle go in textH field
  	SelIText(theDialog,theItem,0,32767);
   	
   	GetFontInfo(&fInfo);

    (*GetDialogTextEditHandle(theDialog))->fontAscent = fInfo.ascent;
    (*GetDialogTextEditHandle(theDialog))->lineHeight = fInfo.ascent + fInfo.descent + fInfo.leading;
#if TARGET_API_MAC_CARBON
    (*GetDialogTextEditHandle(theDialog))->txFont = GetPortTextFont(GetDialogPort(theDialog));
    (*GetDialogTextEditHandle(theDialog))->txFace = GetPortTextFace(GetDialogPort(theDialog));
    (*GetDialogTextEditHandle(theDialog))->txMode = GetPortTextMode(GetDialogPort(theDialog));
    (*GetDialogTextEditHandle(theDialog))->txSize = GetPortTextSize(GetDialogPort(theDialog));
#else
    (*GetDialogTextEditHandle(theDialog))->txFont = theDialog->txFont;
    (*GetDialogTextEditHandle(theDialog))->txFace = theDialog->txFace;
    (*GetDialogTextEditHandle(theDialog))->txMode = theDialog->txMode;
    (*GetDialogTextEditHandle(theDialog))->txSize = theDialog->txSize;
#endif
    
    SetPort(savePort);
}

// Problem is that calling DrawGrowIcon puts lines horizontally and vewrtically accross your window
// to frame the scroll bars for you. How nice. Except there's no way of stopping it.
// Until now... (orchestra hit)
// (crowd cheers as the curtains fall)
// (DrawGrowIconNoBars is centre stage and lit by rapidlly moving spot lights and dry ice swirls
// around it)
// (Mark gets commited to a mental institute)
// (to visit John)
// (who's locking up McKeon)
// (who's there because the entire world's mad)
// sorry...
// It works by setting the windows clip rgn to the bottom left corner thus clipping the crappy line
// which would other wise appear.
void DrawGrowIconNoBars(WindowPtr theWindow)
{
	Rect			winPortRect;

#if TARGET_API_MAC_CARBON
	GetWindowPortBounds(theWindow,&winPortRect);
	RgnHandle		oldClip=::NewRgn();
	GetPortClipRegion(GetWindowPort(theWindow),oldClip);
#else
	winPortrect=theWindow->portRect;
	RgnHandle		oldClip=theWindow->clipRgn;
#endif
	RgnHandle		tempRgn;
	Rect			growRect;
	
	// Work out where the icon is going
	growRect.bottom=winPortRect.bottom;
	growRect.right=winPortRect.right;
	growRect.top=growRect.bottom-15;
	growRect.left=growRect.right-15;

	// create a rgn and set it as the clip rgn
	tempRgn=NewRgn();
	RectRgn(tempRgn,&growRect);
#if TARGET_API_MAC_CARBON
	SetPortClipRegion(GetWindowPort(theWindow),tempRgn);
#else
	theWindow->clipRgn=tempRgn;
#endif
	
	// now we can draw the icon
	DrawGrowIcon(theWindow);
	
	// reset the rgn
#if TARGET_API_MAC_CARBON
	SetPortClipRegion(GetWindowPort(theWindow),oldClip);
	::DisposeRgn(oldClip);
#else
	theWindow->clipRgn=oldClip;
#endif
	DisposeRgn(tempRgn);
}

// flashes the caret blinker line in a particular dialog
void FlashTE(DialogPtr theWindow)
{
	if (GetDialogTextEditHandle(theWindow))
		TEIdle(GetDialogTextEditHandle(theWindow));
}

short MiscErrorStr(short resFile,short dialogId,short messID,short nameID,OSErr errID,Boolean fatal)
{
	return BetterMiscErrorStr(resFile,dialogId,messID,nameID,errID,0,fatal);
}

Boolean IsDoubleClick(EventRecord *theEvent)
{
	Boolean		well;

	if ((GetDblTime()+gLastClick)>=theEvent->when)
		well=true;
	else
		well=false;
		
	gLastClick=theEvent->when;
	
	return well;
}

// use this to discard the last call to is double click
void ClearDoubleClick()
{
	gLastClick=0L;
}

// gets the mouse relative to a window. It bascially switches windows for you then switches them
// back
Point GetLocalMouse(WindowPtr theWindow)
{
	GrafPtr	savePort;
	Point	localPoint;

	GetPort(&savePort);
	SetPortWindowPort(theWindow);

	GetMouse(&localPoint);
	SetPort(savePort);
	
	return localPoint;
}

// This now takes a res file parameter. Pass -1 as it for it to act like the old version otherwise
// give it the fRef of an open resfile.
short BetterMiscErrorStr(short resFile,short resID,short messID,short nameID,OSErr errID,OSErr otherErrID,Boolean fatal)
{
	Str15	errStr="\p",errStr2;
	short	res,saveRes=CurResFile();
	StringHandle	appNameString=0L,messageString=0L;

	if (resFile!=-1)
		UseResFile(resFile);

	if (errID!=0)
		NumToString((long)errID,errStr);
	else // if 0 the put a ?
	{
		errStr[1]='?';
		errStr[0]=1;
	}
	
	NumToString((long)otherErrID,errStr2);

	// Get the strings
	appNameString=(StringHandle)Get1Resource('STR ',nameID);
	messageString=(StringHandle)Get1Resource('STR ',messID);

	// Stop the moving (I don't think paramText makes a copy of them)
	if (appNameString)
		HLock((Handle)appNameString);
	if (messageString)
		HLock((Handle)messageString);

	ParamText(messageString ? *messageString:"\pUnknown error",errStr,appNameString ? *appNameString:"\pUnknown",errStr2);
	
	res=Alert(resID,0L);

	// Let the resources go again
	if (appNameString)
	{
		HUnlock((Handle)appNameString);
		ReleaseResource((Handle)appNameString);
	}
	if (messageString)
	{
		HUnlock((Handle)messageString);
		ReleaseResource((Handle)messageString);
	}

	UseResFile(saveRes);

	if (fatal)
		ExitToShell();

	return res;
}

ControlHandle ItemHandle(DialogPtr theDLog,short item)
{
	DItemStuff;	
	
	GetDialogItem(theDLog,item,&iType,&iHandle,&iRect);
	
	return (ControlHandle)iHandle;
}

short ItemType(DialogPtr theDLog,short item)
{
	DItemStuff;	
	
	GetDItem(theDLog,item,&iType,&iHandle,&iRect);
	
	return iType;
}

void BetterInvertRect(Rect *rect)
{
	unsigned char	hMode;

	hMode=LMGetHiliteMode();
	BitClr((Ptr)(&hMode),(long)pHiliteBit);
	LMSetHiliteMode(hMode);
	InvertRect(rect);
}

void ResizeControl(ControlHandle theControl,Rect *newSize)
{
	MoveControl(theControl,newSize->left,newSize->top);
	SizeControl(theControl,FRectWidth(*newSize),FRectHeight(*newSize));
}

// Draws a highlight on the left and top of the box and a shadow on the right and bottom
// for best effect frame this rect in black and have the rest of the dialog in cBackGrey
void ShadowBox(Rect *theRect)
{
	RGBColor	darkGrey={44563,44563,44563},black=cBlack,white=cWhite;

	RGBForeColor(&white);
	MoveTo(theRect->right-1,theRect->top);
	LineTo(theRect->left,theRect->top);
	LineTo(theRect->left,theRect->bottom-1);
	
	RGBForeColor(&darkGrey);
	LineTo(theRect->right-1,theRect->bottom-1);
	LineTo(theRect->right-1,theRect->top);
	
	RGBForeColor(&black);
}

// check if a te item is a null number is so change it
Boolean CheckForNullNumber(DialogPtr theWindow,short replacement)
{
	Boolean		needsChanging=false;
	
	if ((**DialogTE(theWindow)).teLength>1)
		return false; // OK
	else if ((**DialogTE(theWindow)).teLength==0)
		needsChanging=true; // blank
	// must be 1 char long here, what is that char?
	else if ((*(**DialogTE(theWindow)).hText)[0]=='+' || (*(**DialogTE(theWindow)).hText)[0]=='-')
		needsChanging=true; // +/-
	// must be a num
	else
		return false;

	// null, change it
	if (needsChanging)
	{
		NumberToTE(theWindow,CurrentTEItem(theWindow),(long)replacement);
		return true;
	}

	return false;
}

// this is the same as the regular popup menu select except you can change fonts. If you want
// geneva, don't pass applFont and 9 as it wont work. You'll get geneva 10. After many hours I figured out why this was so.
// If you pass the constant geneva,9 then that works. Note the font num geneva (3) is not applFont (1). I reckon that
// applFont is on ROM and there is only one size, 10. I don't even think the computer knows that it's geneva. Anyway,
// don't pass applFont, use geneva.
long BetterPopupMenuSelect(short fontNum,short fontSize,MenuHandle menu,short top,short left,short value)
{
	short			saveFont=LMGetSysFontFam();
	short			saveSize=LMGetSysFontSize();
	long			choice;
	
	LMSetSysFontFam(fontNum);
	LMSetSysFontSize(fontSize);
	LMSetLastSPExtra(-1L);

	choice=PopUpMenuSelect(menu,top,left,value);
			
	LMSetSysFontFam(saveFont);
	LMSetSysFontSize(saveSize);
	LMSetLastSPExtra(-1L);
	
	return choice;
}

// same as above except the light come the other way
void InverseShadowBox(Rect *theRect)
{
	RGBColor	darkGrey={44563,44563,44563},black=cBlack,white=cWhite;

	RGBForeColor(&darkGrey);
	MoveTo(theRect->right-1,theRect->top);
	LineTo(theRect->left,theRect->top);
	LineTo(theRect->left,theRect->bottom-1);
	
	RGBForeColor(&white);
	LineTo(theRect->right-1,theRect->bottom-1);
	LineTo(theRect->right-1,theRect->top);
	
	RGBForeColor(&black);
}

// DialogSelect only works on the front most window
// This proc works on any dialog, allowing buttons to be clicked when another window is in front. You might need this if
// you have for instance a floating palette in front of a modeless dialog.
// You have to pass a valid dialogptr for it to force the dialog select in. It works by implementing a custom DialogSelect
// and doesn't call the toolboxes' one at any point.
Boolean ForceDialogSelect(EventRecord *event,DialogPtr dialog,short *itemHit)
{
	short		tempItemHit;

	if (!dialog)
		return false;
	if (!itemHit)
		itemHit=&tempItemHit;

	*itemHit=0;

	switch (event->what)
	{
		case mouseDown:
			return ForceDialogSelectMouseDown(event,dialog,itemHit);
			break;
			
		case keyDown:
		case autoKey:
			return ForceDialogSelectKeyDown(event,dialog,itemHit);
			break;
	}
	
	return false;
}

Boolean ForceDialogSelectKeyDown(EventRecord *event,DialogPtr dialog,short *itemHit)
{
	char		charHit=event->message&charCodeMask;
	
	if (event->what!=keyDown && event->what!=autoKey)
		return false;

	if (!DialogTE(dialog))
		return false;

	// Do special processing of (shift) tab
	switch (charHit)
	{
		case kTabChar:
			*itemHit=CycleTEItems(dialog,(event->modifiers&shiftKey)==0);
			return true;
			break;
	}

	BetterTEKey(charHit,DialogTE(dialog));
	
	*itemHit=CurrentTEItem(dialog);
	
	return true;
}

Boolean ForceDialogSelectMouseDown(EventRecord *event,DialogPtr origDialog,short *itemHit)
{
	Point		localPoint=event->where;
	Boolean		result=true;
	DialogPtr	dialog;
	WindowPtr	foundWindow;

	if (event->what!=mouseDown)
		return false;
	
	if (FindWindow(event->where,&foundWindow)==inContent)
	{
		dialog=GetDialogFromWindow(foundWindow);
	
		GrafPtr		wasPort;
		short			type;
	
		if (origDialog!=dialog)
			return false;
	
		GetPort(&wasPort);
		SetPortDialogPort(dialog);
	
		GlobalToLocal(&localPoint);
	
		// Find the item clicked on
		*itemHit=FindDialogItem(dialog,localPoint)+1;
		
		if (*itemHit)
		{
			// If it's a control then track it
			type=ItemType(dialog,*itemHit);
			if (type&itemDisable)
				result=false;
			else
			{			
				// Item is enabled
				if (type&kControlDialogItem)
				{
					if (!TrackControl(ItemHandle(dialog,*itemHit),localPoint,(ControlActionUPP)-1L))
						result=false;
				}
				else if (type==editText)		
				{
					// Click in an active te item, if it's the current te item then call te click.
					// If it's not the active te item then deactivate the active te item and activate the new one
					if (*itemHit!=CurrentTEItem(dialog))
					{
						TEDeactivate(DialogTE(dialog));
						SelIText(dialog,*itemHit,0,0);
					}
					TEClick(localPoint,(event->modifiers&shiftKey)!=0,DialogTE(dialog));
				}
			}
		}
		else
			result=false;
		
		SetPort(wasPort);
	}
	
	return result;
}

// This returns the item that was tabbed onto. Pass a dialog to operate on and forwardsCycle is true to cycle forwards
short CycleTEItems(DialogPtr dialog,Boolean forwardsCycle)
{	
	short		maxIndex=CountDITL(dialog);
	short		index;
	short		previousTEItem=-1;
	short		direction=forwardsCycle ? 1 : -1;
	
	if (!maxIndex)
		return 0;
	
	// Cycle onto the next/previous te item
	index=CurrentTEItem(dialog);
	
	// Cycle in the given direction
	do
	{
		index+=direction;
		
		// Cycle the index
		if (index>maxIndex)
			index=1;
		else if (index==0)
			index=maxIndex;
		
		if (ItemType(dialog,index)==editText)
		{
			SelIText(dialog,index,0,32767);
			break;
		}
		
	} while (index!=CurrentTEItem(dialog));

	return index;
}







