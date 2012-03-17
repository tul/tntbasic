// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Useful defines
// Mark Tully
// 12/2/96
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Keys
#define			FastIsPressed(km,k)		(( ((unsigned char *)(km))[(k)>>3] >> ((k) & 7) ) & 1)

#define			modKeys		0x1B00
#define			kControlKey	0x1000
#define			kCommandKey	0x0100
#define			kOptionKey	0x0800
#define			kShiftKey	0x0200
#define			kReturnChar	0x0D
#define			kEnterChar	0x03
#define			kHelpChar	0x05
#define			kDeleteChar	0x08
#define			kForDelChar	0x7F
#define			kTabChar	0x09
#define			kUpChar		0x1E
#define			kDownChar	0x1F
#define			kLeftChar	0x1C
#define			kRightChar	0x1D
#define			kEscChar	0x1B

#define			kPageDownKey	0x79
#define			kPageUpKey		0x74
#define			kHomeKey		0x73
#define			kEndKey			0x77

// Key scancodes
enum
{
	kCommandKeySC=55,
	kShiftKeySC,
	kCapsLockSC,
	kOptionKeySC,
	kControlKeySC,
	kLeftSC=0x7B,
	kRightSC,
	kDownSC,
	kUpSC,
	kEscSC=0x35,
	kSpaceSC=0x31,
	kFunctionKeySC=0x3F		// fn key on powerbooks
};

// Dialogs
#define 		DItemStuff short iType; Rect iRect; Handle iHandle
#define			GetCBox(theDLog,item) (ItemValue(theDLog,item)!=0)
#define			SetCBox(theDLog,item,value) SetItemValue(theDLog,item,((value)!=0))
#define			EraseWindow(win)	EraseRect(&(win)->portRect)

// gets the item num of the current editable text item
#if TARGET_API_MAC_CARBON
#define			CurrentTEItem(x)	GetDialogKeyboardFocusItem(x)
#else
#define			CurrentTEItem(x)	(((DialogPeek)(x))->editField+1)
#endif
#define			CurrentTE(x)		CurrentTEItem(x)
// gets the rec of above
#if TARGET_API_MAC_CARBON
#define			DialogTE(x)			GetDialogTextEditHandle(x)
#else
#define			DialogTE(x)			(((DialogPeek)x)->textH)
#endif
// gets the point where text would enter in the record if a key was pressed
#define			CaretPos(x)			((**(x)).selStart)
// returns true if the key is an arrow key
#define			IsArrow(x)			((x)>=kLeftChar && (x)<=kDownChar)
#define			IsAsciiNumber(x)	((x)>='0' && (x)<='9')

// Text files
#define			kTab	'\t'
#define			kCR		'\r'

// Strings
// This are used when accessings lists of pstrings in RAM. It skips past a single pstring.
#define		SkipPStr(ptr)			((ptr)+=(ptr)[0]+1)
#define		SkipPString(ptr)		SkipPStr(ptr)

// Clears the string by setting it to 0 in length, but first check the ptr is not 0L
#define		SafeClearPStr(ptr)		((ptr)?((ptr)[0]=0):0L)
#define		SafeClearPString(ptr)	SafeClearPStr(ptr)

// Checks if a string is either 0L or empty
#define		IsNullStr(ptr)		((!ptr) || (!(ptr)[0]))
#define		IsNullString(ptr)	IsNullStr(ptr)

// Rects
#define			FastSetRect(rect,a,b,c,d)	{(rect).top=(a);(rect).left=(b);(rect).bottom=(c);(rect).right=(d);}
#define			FSetRect(r,a,b,c,d)			FastSetRect(r,a,b,c,d)
#define			BiggerRect(a,b) 			(((a).right-(a).left)>=((b).right-(b).left))&&\
											(((a).bottom-(a).top)>=((b).bottom-(b).top))
#define			FOffset(a,x,y)				{(a).top+=(y);(a).bottom+=(y);(a).left+=(x);(a).right+=(x);}
#define			ZeroRectCorner(a)			{(a).right-=(a).left;(a).bottom-=(a).top;(a).left=0;(a).top=0;}
#define			BigEndianRectToNative(r)	{ \
												(r).top=CFSwapInt16BigToHost((r).top); \
												(r).left=CFSwapInt16BigToHost((r).left); \
												(r).bottom=CFSwapInt16BigToHost((r).bottom); \
												(r).right=CFSwapInt16BigToHost((r).right); \
											}
#define			AlignRect32(rect)			{rect.left&=0xfffc; rect.right=(rect.right+3)&0xfffc;}
#define			FRectWidth(rect)			((rect).right-(rect).left)
#define			FRectHeight(rect)			((rect).bottom-(rect).top)
#define			FInsetRect(rect,x,y)		{(rect).left+=(x);(rect).right-=(x);(rect).top+=(y);(rect).bottom-=(y);}
#define			FPointInRect(point,rect)	(point.h>=rect.left && point.h<rect.right && point.v>=rect.top && point.v<rect.bottom)
#define			SameRect(a,b)				(((a).top==(b).top) && ((a).bottom==(b).bottom) && ((a).left==(b).left) && ((a).right==(b).right))
#define			FSectRect(a,b)				(((a).left<(b).right) && ((a).right>(b).left) && ((a).top<(b).bottom) && ((a).bottom>(b).top))
#if !TARGET_API_MAC_CARBON
#define			WholeWindowRect(winP)		((**((WindowPeek)(winP))->strucRgn).rgnBBox)
#endif
// Swaps sides of rect so that it's valid (left<=right && top<=bottom)
#define 		ValidateRect(rect)			do												\
											{												\
												short	temp;								\
												if (rect.top>rect.bottom)					\
													temp=rect.top, rect.top=rect.bottom, rect.bottom=temp;	\
												if (rect.left>rect.right)					\
													temp=rect.left, rect.left=rect.right, rect.right=temp;	\
											} while (false)


// Americanisms
#define			RGBBackColour(x)			RGBBackColor(x)
#define			RGBForeColour(x)			RGBForeColor(x)
#define			RGBColour					RGBColor

// Events
#define suspendEvt				200
#define resumeEvt				201
#define mouseMovedEvt			202
#define deactivateEvt			203
#define App4Selector(event)		(*((unsigned char *) &(event)->message))
#define IsSuspendResume(event)	( App4Selector(event) == 1 )
#define IsResume(event)			( (IsSuspendResume(event)) && ((event)->message & 1) )
#define IsSuspend(event)		( (IsSuspendResume(event)) && (!IsResume(event)) )
#define IsMouseMoved(event)		( App4Selector(event) == 0xFA )
#define kUserInputEventMask		(mDownMask|mUpMask|keyDownMask|keyUpMask|autoKeyMask)		// all events generated by user input

// Other
#define IsCharDefinedInASCIITable(charr) (charr>=0x20 && charr<0xDA && charr!=0xCA)	// Some charactor values are not defined in the ASCII Matric (look it up in THINK) and so can't be printed on screen. This allows you to filter them out
#define	Greater(a,b)		((a)>(b)?(a):(b))	// returns the greater
#define	Lesser(a,b)			((a)>(b)?(b):(a))	// returns the lesser
#define	Limit(a,min,max)	((a)<(min) ? (min):((a)>(max)?(max):(a)))	// clips a to be between min and max (inclusive)
#define	FHiWord(x)			((short)((x)>>16))
#define	FLoWord(x)			((short)((x) & 0xFFFF))
#define	FAbs(x)				((x)>0 ? (x) : -(x))
#define	SwapValues(a,b,temp)	(temp)=(a), (a)=(b), (b)=(temp)
#define XORSwapValue(a,b)	(a)^=(b), (b)^=(a), (a)^=(b)
#define	InRange(x,min,max)	((x)>=(min) && (x)<(max))
#define	InRangeInclusive(x,min,max)	((x)>=(min) && (x)<=(max))

// Binary
#define FIsBitSet(bitId,number)	(((1<<(bitId))&(number))!=0)
#define	FSetBit(bitId,number)	((number)|=(1<<(bitId)))
#define	FClearBit(bitId,number)	((number)&=(0xFFFFFFFF-(1<<(bitId))))

// Menus
#if TARGET_API_MAC_CARBON
#define	EnableDisableItem(menuH,itemNo,state)	((state) ? EnableMenuItem(menuH,itemNo) : DisableMenuItem(menuH,itemNo))
#else
#define	EnableDisableItem(menuH,itemNo,state)	((state) ? EnableItem(menuH,itemNo) : DisableItem(menuH,itemNo))
#endif
#define	kMenuSepBarText			"\p(-"

// old routine names
#if OLDROUTINENAMES==0
#define SelIText(theDialog, itemNo, strtSel, endSel)  	SelectDialogItemText(theDialog, itemNo, strtSel, endSel)
#define GetIText(item, text) 							GetDialogItemText(item, text)
#define SetIText(item, text) 							SetDialogItemText(item, text)
#define GetDItem(theDialog, itemNo, itemType, item, box)  GetDialogItem(theDialog, itemNo, itemType, item, box)
#define SetDItem(theDialog, itemNo, itemType, item, box)  SetDialogItem(theDialog, itemNo, itemType, item, box)
#define SizeResource(theResource) 						GetResourceSizeOnDisk(theResource)
#endif

#ifdef __cplusplus
}
#endif
