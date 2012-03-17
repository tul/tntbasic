// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Marks Routines.h
// Mark Tully
// 12/2/96
// This file contains prototypes, "Useful Defines.h" contains the good stuff
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

#pragma once // include only once

#if __MWERKS__
#include	<MacHeaders.c>
#endif
#include	"Useful Defines.h"
#if __MWERKS__
#include	<EPPC.h>				// needed for the apple events module
#endif

// Necessary to call procs in .c files from a .cpp file
#ifdef __cplusplus
extern "C" {
#endif

// Drag.c
// Must have included Drag.h before this (Drag.h is the uni header not a Marks Routine)

#ifdef __DRAG__

#pragma mark Drag.c

// some useful defines...
#define		FlavourType			FlavorType
#define		FlavourFlags		FlavorFlags
#define		SetItemFlavourData	SetItemFlavorData
#define		CountDragItemFlavours	CountDragItemFlavors
#define		GetFlavourType		GetFlavorType
#define		GetFlavourFlags		GetFlavorFlags
#define		GetFlavourDataSize	GetFlavorDataSize
#define		GetFlavourData		GetFlavorData
#define		badDragFlavourErr	badDragFlavorErr
#define		AddDragItemFlavour	AddDragItemFlavor

Boolean IsFlavourAvailable(DragReference theDrag,ItemReference theItem,FlavourType type);
OSErr FetchFlavorData(DragReference theDrag,ItemReference theItem,FlavorType theType,Handle *theData);
OSErr FetchSenderOnlyFlavorData(DragReference theDrag,ItemReference theItem,FlavorType theType,Handle *theData);
Boolean InsideSenderWindow(DragReference theDrag);
Boolean DragAndDropPresent();

#endif

// Gestalt Bits.c
#pragma mark Gestalt Bits.c
Boolean System7Up();

// PowerPC.c
#pragma mark PowerPC.c
Boolean PowerPCPresent(void);

// LinkedList.c
#pragma mark LinkedLists.c
typedef struct
{
	Ptr			data;
	Ptr			next;
} ListElement, *ElementPtr, *LinkedList, **ElementHandle;

typedef Boolean (*ElementProc)(Ptr dataPtr,long *refCon,unsigned long count);
// The compare proc should return -1 if dataA<dataB, +1 is dataA>dataB and 0 if dataA=dataB
typedef signed char (*CompareProc)(Ptr dataA,Ptr dataB,void *refCon);

void *GetNextElement(LinkedList *list);
unsigned long FindCustomElement(LinkedList *listPtr,void *matchData,void *refCon,CompareProc theProc);
unsigned long FindElement(LinkedList *listPtr,void *findData,unsigned long dataOffset,Size dataSize);
signed char ShortCompareProc(Ptr dataA,Ptr dataB,void *refCon);
signed char LongCompareProc(Ptr dataA,Ptr dataB,void *refCon);
Boolean LSSortList(CompareProc theProc,LinkedList *listPtr,void *refCon);
Boolean AddElement(void *dataPtr,Size dataSize,ElementPtr *listPtr);
Boolean RemoveElement(LinkedList *listPtr,unsigned long num);
Boolean ForEachElementDo(LinkedList *listPtr,ElementProc routine,long *refCon);
Boolean ForNthElementDo(LinkedList *listPtr,unsigned long n,ElementProc routine,long *refCon);
ElementHandle GetElement(LinkedList *listPtr,unsigned long n);
Boolean CopyData(LinkedList *listPtr,Ptr dataDest,Size dataSize,unsigned long num);
Boolean AddressData(LinkedList *listPtr,Ptr *data,unsigned long num);
Boolean RemoveLastElement(LinkedList *listPtr);
unsigned long CountElements(LinkedList *listPtr);
void DestroyList(LinkedList *listPtr);
Boolean InsertElement(LinkedList *listPtr,unsigned long pos,Ptr dataPtr,Size dataSize);
Boolean BetterAddElement(Ptr dataPtr,Size dataSize,ElementHandle moo);
unsigned long AddSortedElement(void *dataPtr,Size dataSize,CompareProc sortProc,void *refCon,LinkedList *theList);
signed char PStringCompareProc(Ptr dataA,Ptr dataB,void *refCon);
Boolean DuplicateLinkedList(LinkedList *frontList,LinkedList *backList);
Boolean DupeElementProc(Ptr dataPtr,long *refCon,unsigned long count);
Size GetTotalDataSize(LinkedList *theList);
Boolean SizeElementProc(Ptr dataPtr,long *refCon,unsigned long count);
Size GetDataSize(LinkedList *theList,unsigned long element);
Size DumpLinkedList(LinkedList *theList,unsigned char *destPtr,Size bufferSize);

// Lists.c
#pragma mark Lists.c
#define	kScrollBarWidth	15
typedef Boolean (*CellProc)(ListHandle theList,Cell theCell,short cellCount,void *refCon);
// constants for GetVisibleCellsRgn
enum
{
	lMustBeSelected,
	lMustntBeSelected,
	lDontCare
};

Boolean IsCellVisible(ListHandle list,Cell cell);
void ScrollToCell(ListHandle list,Cell cell);
short UpDownList(ListHandle theList,Boolean up,Boolean loopAround);
Boolean PtInCells(Point thePoint,ListHandle theList,signed char selectState);
ListHandle CreateListView(DialogPtr theWindow,short listNum,short width,short height);
void UpdateList(DialogPtr theWindow,ListHandle theList);
ListHandle EckerCreateListView(DialogPtr theWindow,short listNum,short procID,short cellHeight,short width,short height);
Rect GetFullListRect(ListHandle theList);
Boolean ForEachSelectedCellDo(ListHandle theList,Boolean mustBeVisible,CellProc routine,void *refCon);
void SelectAllInList(ListHandle theList,Boolean select);
void EraseEmptyCells(ListHandle theList);
Boolean CellExists(Cell theCell,ListHandle theList);
Boolean AnyCellsSelected(ListHandle theList);
Boolean CountProc(ListHandle theList,Cell theCell,short cellCount,void *refCon);
short CountSelectedCells(ListHandle theList);
RgnHandle GetVisibleCellsRgn(ListHandle theList,signed char selectState);
void BetterSelectCell(ListHandle theList,Cell theCell);

// Regions.c
#pragma mark Regions.c
void BetterInvertRgn(RgnHandle rgn);
void OutlineRgn(RgnHandle theRgn);
void InverseRgn(RgnHandle theRgn);
void AddRectToRgn(RgnHandle theRgn,Rect *theRect);
void GlobalToLocalRgn(RgnHandle theRgn);
void LocalToGlobalRgn(RgnHandle theRgn);

// Low level.c
#pragma mark Low level.c
void SetNoForceQuit();
typedef struct
{
	long	DRegs[8];
	long	ARegs[7];
} S68KRegs;
#define		upp68KRegsProcInfo	kCStackBased | RESULT_SIZE(kNoByteCode) | STACK_ROUTINE_PARAMETER(1,kFourByteCode)
void Get68KRegs(S68KRegs *regs);
void Set68KRegs(S68KRegs *regs);
short CPUType();

// Password.c
#pragma mark Password.c
Boolean GetPassword(StringPtr password,StringPtr promt);
void PasswordKey(TEHandle teHndl, char theKey, StringPtr password);

// Text files.c
#pragma mark Text files.c
void ReadUntil(short file,char stopChar);
void ReadPString(short file,StringPtr theString,char stopChar);
void ReadIntoBuffer(short file,Ptr here,char stopChar,short *length);
char ReadChar(short file);
OSErr GetTextFileErr();
void SetTextFileErr(OSErr err);
void WritePString(short file,StringPtr theString);
void WriteChar(short file,char theChar);

// File Routines.c
#pragma mark File Routines.c
//OSErr FSpGetFileLocation(short refNum,FSSpec *spec);;
OSErr UpdateRelativeAliasFile(FSSpec *theAliasFile, FSSpec *targetFile, Boolean createIfNecessary);
Boolean IdentifyPackage(FSSpec *target, FSSpec *mainPackageFile);
SInt32 CountOpenResourceFiles();
OSErr SetCreatorAndFileType(
	FSSpec		*inSpec,
	OSType		inCreator,
	OSType		inFileType);
OSErr CreateFileLongName(
	FSSpec		*inParentDir,
	Str255		inFileName,
	OSType		inCreator,
	OSType		inFileType,
	FSSpec		*outFSSpec);
OSErr SetFInfoFlags(
	FSSpec			*inSpec,
	unsigned short	inAddFlags,
	unsigned short	inRemoveFlags);
OSErr SetDInfoFlags(
	FSSpec			*inSpec,
	unsigned short	inAddFlags,
	unsigned short	inRemoveFlags);
Boolean IsOS9OrGreater();
OSErr GetAppCreatorCode(OSType *outCreator);
OSErr ConvertPStringToHFSUniStr(Str255 inStr, HFSUniStr255 *outUniStr);
OSErr FSRefToFSSpec(
	FSRef		*inRef,
	FSSpec		*outSpec);
OSErr MakeFSSpecFromFileSpecLongName(
	FSSpec *ioSpec,
	Str255 inLongName);
Boolean CompareFSSpecs(
	FSSpec		*inFile1,
	FSSpec		*inFile2);
OSErr ForksExist(FSSpec *theFile, Boolean *outDataForkExists, Boolean *outResForkExists);
OSErr GetDirFSSpec(
	short	inVRefNum,
	long	inDirID,
	FSSpec	*outSpec);
OSErr FSpSpecExists(
	FSSpec		*inSpec,
	Boolean		*outExists);
OSErr GetLongFileName(const FSSpec *inSpec,Str255 outName);
OSErr GetCreatorAndType(const FSSpec *file,OSType *creator,OSType *fileType);
OSErr IsSystemFile(FSSpec *theSpec,Boolean *res);
unsigned long BothForksSize(FSSpec *theFile,OSErr *err);
OSErr GetAppSpec(FSSpec *spec);
OSErr OpenFileInFolder(StringPtr fileName,short *fRef,Boolean resFork);
typedef OSErr (*ScanProc)(CInfoPBPtr cipbp, long refCon);
OSErr ScanDirectoryCInfo(CInfoPBPtr cipbp,short vRefNum,long dirID,ScanProc sp,ScanProc folderProc,long refCon);
OSErr ScanDirectoryFSSpec(CInfoPBPtr cipbp,FSSpec *theFolder,ScanProc sp,ScanProc folderProc,long refCon);
OSErr IsFolder(FSSpec *theFolder,Boolean *theResult);
OSErr CInfoToFSSpec(CInfoPBRec *cipbp,FSSpec *theFile);
void FSSpecToCInfo(FSSpec *theFile,CInfoPBRec *cipbp);
//OSErr FlushFile(short fileRef);
OSErr GetVersionInfo(short resFile,short *major,short *sub,short *subSub);
OSErr GetVersionInfoWithStrs(
	short		inResFile,
	short		*outMajor,
	short		*outSub,
	short		*outSubSub,
	StringPtr	outVerShortText,
	StringPtr	outVerLongText);
OSErr BetterCreateFile(FSSpec *theSpec,Boolean resFork,short mode,OSType creator,OSType type);
OSErr IsFileReadOnly(short refNum,Boolean *fileIsReadOnly);
OSErr MakeFSSpecFromFileSpec(FSSpec *spec,Str32 nameString);
#define			kOverwriteFile	0		// constant for above kBottleIt is the other (resources.c)
OSErr GetModDate(
	short				inVRefNum,
	long				inDirID,
	ConstStr255Param	inName,
	unsigned long		*outModDate);
OSErr SetModDate(
	short				inVRefNum,
	long				inDirID,
	ConstStr255Param	inName,
	unsigned long		inModDate);

// Processes.c
#pragma mark Processes.c
Boolean SwitchToProcess(long process,Boolean wait);
long GetProcessCreator(ProcessSerialNumber *process);
Boolean WaitForFrontProcess(long process,unsigned long waitFor);
Boolean	ProcessExists(ProcessSerialNumber *process);
Boolean WaitResumeEvent();
Boolean IsFrontProcess();

// HidemenuBar.c
#pragma mark HideMenuBar.c
// Stop a naming conflict with MacOS 8.5's Universal Headers 3.2
#ifndef __MENUS__
void HideMenuBar();
void ShowMenuBar();
#endif
void PowerHideMenuBar(Boolean callPaintOne);

// Control Strip.c
#pragma mark Control Strip.c
Boolean IsControlStripInstalled();
void ShowHideControlStrip(Boolean state);
Boolean IsControlStripVisible();
void RestoreControlStrip();
void HideControlStrip();

// InitMac.c
#pragma mark InitMac.c
#if !TARGET_API_MAC_CARBON
void InitMac();
#endif

// File Dialogs.c
#pragma mark File Dialogs.c
// Call these
OSErr ChooseAndOpenFile(short *theRef,FSSpec *theSpec,Boolean resFork);
#if !TARGET_API_MAC_CARBON
void GetFileWithPreviews(short dLogID,short picID,short stringID,StandardFileReply *reply,SFTypeList typeList,short listCount,FileFilterUPP fileFilter,void (*)(WindowPtr));
#endif
// Don't call these
pascal short DHookProc(short item,DialogPtr inDialog,void *dataPtr);
pascal Boolean FileProc(ParmBlkPtr a,void *data);
pascal Boolean MR_EventHandler(DialogPtr theDialog,EventRecord *theEvent,short *itemHit,void *dataPtr);
void ShowPreview(DialogPtr inDialog);
void UpdatePreviewItem(DialogPtr inDialog,PicHandle thePic);

// New Strings.cpp
#pragma mark New Strings.cpp
typedef unsigned char String[];

SInt16 PPCompareString(
	const void*		inLeft,
	const void*		inRight,
	UInt8			inLeftLength,
	UInt8			inRightLength);

Boolean CmpStringNoCase(StringPtr a,StringPtr b);
Boolean CmpString(StringPtr a,StringPtr b);
void BetterNumberToString(unsigned long number,Boolean numberIsSigned,StringPtr string);
unsigned long BetterStringToNumber(StringPtr string,unsigned long *number,Boolean *wasNegative);
void DoubleToString(double theNum,StringPtr theString,short dps);
void NumToHexString(long number,StringPtr str,unsigned char minLength);
void CharToHexPair(unsigned char in,StringPtr out);
void ConcatPStringNum(StringPtr string,long num);
Boolean IsVowel(char theChar);
short FindAndReplacePStr(StringPtr string,StringPtr replaceMe,StringPtr withMe,Boolean caseSensitive);
short FindAndReplace(Ptr startBuff,Size *buffLength,Size maxBuffSize,Ptr replaceMe,Size replaceMeSize,Ptr withMe,Size withMeSize,Boolean caseSensitive);
short CountWords(StringPtr theString);
Boolean BetterGetFNum(StringPtr theString,short *theNum);
SInt32 SearchForChars(Ptr source,Ptr find,SInt32 sourceLen,SInt32 findLen,Boolean caseSensitive);
char ToggleCase(char theChar);
SInt32 SearchMemForChar(Ptr searchMe,char forMe,SInt32 buffLen,Boolean caseSensitive);
Boolean CmpBuffer(const char *a,const char *b,Size len,Boolean caseSensitive);
short ReplaceWord(StringPtr string,StringPtr replace,StringPtr with,Boolean caseSensitive);
#define CmpPString(a,b) CmpPStr(a,b)
Boolean CmpPStr(ConstStringPtr sourceA, ConstStringPtr sourceB);
SInt32 CmpPStrNoCaseDifference(
	ConstStringPtr inA,
	ConstStringPtr inB);
void BuffLen(Ptr theString);
short StrLen(StringPtr theString);
#define PadNum(a,b,c) PadPString(a,b,c)
void PadPString(StringPtr iString,short len,char padChar);
void PostPadPString(StringPtr iString,short len,char padChar);
#define CopyString(source,dest) CopyStr(source,dest)
void CopyStr(StringPtr source, StringPtr dest);
#define CopyPString(a,b) CopyPStr(a,b)
void CopyPStr(const unsigned char *source,unsigned char *dest);
void ConcatStrWD(StringPtr front, StringPtr back);
void ConcatStr(StringPtr front, StringPtr back);
#define ConcatPString(a,b) ConcatPStr(a,b)
void ConcatPStr(StringPtr ioFront, ConstStringPtr inBack);
void PStrToCStr(StringPtr pString);
void Num2Str(long num,StringPtr str);
void CStr2Num(StringPtr theString,long *num);
void RemoveChunk(Ptr a,short pos1, short pos2, short bufferLen);
void CStrToPStr(StringPtr cString);
Boolean CmpPStrNoCase(ConstStringPtr sourceA, ConstStringPtr sourceB);
void CopyPBuffer(Ptr start,StringPtr dest,char stopChar,short buffLen);
#define	CopyCAsPString(a,b) CopyCAsPStr(a,b)
void CopyCAsPStr(const StringPtr source, StringPtr dest);
void CopyPAsCStr(const StringPtr source, StringPtr dest);
char LowerCase(char theChar);
char UpperCase(char theChar);
inline char ToUpper(char a) { return UpperCase(a); }
inline char ToLower(char a) { return LowerCase(a); }
void ExtractNumber(StringPtr theString,StringPtr preFix,long *theNum);
char *BCDToString(
	UInt32	inBCD,
	char	outStr[10]);

// Cursor.c
#pragma mark Cursor.c
Boolean InitAnimatedCursors(short acurID, short interval);
void ReleaseAnimatedCursors(void);
void SpinCursor(void );

// Messages.c
#pragma mark Messages.c
void OpenMessBox();
void DoMessage(StringPtr mess);
void CloseMessBox();

// Menu Bits.c
#pragma mark Menu Bits.c
Boolean FetchTheBar(short theBar);
void ClearMenu(MenuHandle theMenu);
MenuHandle InsertSubMenu(short resID);
void SetMenuItem(short resID,short pos,StringPtr newName,Boolean enabled);
Boolean GetMenuItemState(MenuHandle handle,short itemId);
void BetterAppendMenu(MenuHandle handle,StringPtr string);
void Mark1MenuItem(MenuHandle menu,short item);
void BetterInsertMenuItem(MenuHandle menu,StringPtr text,short afterItem);

// Apple Events.cpp
#pragma mark Apple Events.cpp
// A quick typedef to replace AEEventHandlerProcPtr
typedef	pascal short (*MRAEInstallProc)(const struct AEDesc *,struct AEDesc *,long);

typedef struct
{
	AEEventClass			theEventClass;
	AEEventID				theEventID;
	MRAEInstallProc			theHandler;
} AEInstallStruct, *AEInstallStructPtr;

#define		MRAENumElements(x)	sizeof(x)/sizeof(AEInstallStruct)

#ifdef __cplusplus
Handle /*e*/ GetAEDataHandle(
	const AEDesc	&inDesc);
void /*e*/ CoerceWithReply(const AEDesc &inDesc,AEDesc &outDesc,DescType inDestType,AppleEvent &outReply);
void /*e*/ CoerceInPlaceDesc(AEDesc &inDesc,const AEDesc &inRequestedType);
void /*e*/ CoerceInPlaceType(AEDesc &inDesc,DescType toType);
Boolean /*e*/ GetPropertyFromAppleEvent(const AppleEvent &inAppleEvent,AEDesc &property,DescType propertyCode,DescType desiredType);
#endif

OSErr FetchAEErr(AppleEvent *appIn);
long FetchNum(AppleEvent *appIn,AEKeyword theKey);
OSErr FetchParam(AEKeyword plop,AppleEvent *appIn,Ptr dest,DescType theType,Size *len);
#if !TARGET_API_MAC_CARBON
OSErr GetSendersAddress(AppleEvent *event,AEAddressDesc *targ);
OSErr SelectTarget(StringPtr prompt,StringPtr appsList,TargetID *targ,PPCFilterUPP theProc);
OSErr SendAppleEvent(TargetID *targ,AEEventClass aeClass,AEEventID aeID);
OSErr BigSendAppleEvent(TargetID *targ,AEEventClass aeClass,AEEventID aeID,Ptr *data,short dataLen,DescType theDataType,Boolean returnData,Boolean checkForHandlerErr);
#endif
Boolean InitializeAE(Boolean inppc);
OSErr FetchParamAnySize(AEKeyword plop,AppleEvent *appIn,Ptr *dest,DescType theType);
typedef OSErr (*AEFileProc)(FSSpec *theFile); // AEFileProc type
OSErr ForEachFileDo(const AppleEvent *message,AEFileProc theirProc);
OSErr MRInstallAEHandlers(AEInstallStructPtr installUs,short numToInstall);
#define		InstallAEHandlerSet(x)	MRInstallAEHandlers(x,MRAENumElements(x))
short CountFiles(AppleEvent *message);

// Scrap.c
#pragma mark Scrap.c
void StoreScrap();
Boolean ScrapChanged();
OSErr ClipCutResource(short resFile,ResType theType,short resID);
OSErr ClipCopyResource(short resFile,ResType theType,short resID);
Boolean PasteProducesNumber(WindowPtr window,TEHandle te,long min,long max);
Boolean ScrapTextIsNumberInRange(long min,long max);
Boolean ScrapTextIsNumbersOnly();


// Resources.cpp
#pragma mark Resources.cpp

#define		IsResourceHandle(x)		((HGetState(x))&0x0020)

OSErr BetterGetString(short resFile,short resId,StringPtr string);
OSErr BetterGetResErr(void);
long BetterSizeResource(short resFile,ResType resType,short resID);
Boolean ResourceExists(short resFile,ResType resType,short resID);
Boolean IndResourceExists(short resFile,ResType resType,short inIndex);
Boolean DeleteResource(short resFile,ResType resType,short resID);
OSErr BetterCopyResource(short sourceFile,ResType sourceType,short sourceID,Boolean indexing,short destFile,short resMode);
Handle BetterGetIndResource(short resFile,ResType theType,short resID);
short BetterUniqueID(short resFile,ResType theType);
Handle BetterGetResource(short resFile,ResType theType,short resID);
Boolean DeleteAllResources(short resFile,ResType theType);
void BetterDisposeHandle(Handle theHan);
short BetterGetResAttrs(short resFile,ResType theType,short resID);
void BetterReleaseResource(Handle theRes);
OSErr BetterAddResource(short resFile,Handle theData,ResType type,short *id,StringPtr name,short mode);
#ifdef __cplusplus
bool GetNamedResourceId(ResType inType,Str255 inString,ResID &outId);
#endif

// constants for above function
enum
{
	kDontSave,
	kOverwriteResource,
	kUniqueID,
	kBottleIt
};

Handle BetterGetResourceByName(short resFile,ResType type,StringPtr name,unsigned char mode,Boolean caseSens);
// constants for above
enum
{
	rnIs,
	rnStartsWith,
	rnEndsWith,
	rnContains
};

void GetResName(Handle handle,Str255 name);

// Dialogs.cpp
#pragma mark Dialogs.c
// Resource ID's of the ALRTs from Misc Errors.rsrc
enum
{
	kNoteErrorID=1128,
	kNoteErrorFID,
	kWarning,
	kWarningF,
	kStop,
	kStopF
};

// Used in the progress bar jobber
#define	cBlack		{ 0x0000, 0x0000, 0x0000 }
#define cWhite		{ 0xFFFF, 0xFFFF, 0xFFFF }
#define cGrey		{ 0x4444, 0x4444, 0x4444 }
#define cGray		cGrey // incase of yanks
#define cBlue		{ 0xCCCC, 0xCCCC, 0xFFFF }
#define	cBackGrey	{ 56799,56799,56799 }
#define cBackGray	cBackGrey

#define		BetterPopUpMenuSelect		BetterPopupMenuSelect
#define		kSpecialPasteChar			0

Boolean IsItemEnabled(DialogPtr dlg,short item);
void FrameTE(DialogPtr dialog,short item);
void BetterTEKey(char keyPress,TEHandle handle);
short CycleTEItems(DialogPtr dialog,Boolean forwardsCycle);
Boolean ForceDialogSelect(EventRecord *event,DialogPtr dialog,short *itemHit);
short ControlHandleToDialogId(DialogPtr theDialog,ControlHandle theControl);
Boolean CheckForNullNumber(DialogPtr theWindow,short replacement);
long BetterPopupMenuSelect(short fontNum,short fontSize,MenuHandle menu,short top,short left,short value);
void InverseShadowBox(Rect *theRect);
void ShadowBox(Rect *theRect);
Boolean SimulateKeyPressInWindow(WindowPtr theWindow,TEHandle theTE,TEHandle *newTE,char keyPress,Ptr buffer,Size *bufferSize);
Boolean SimulateKeyPressInDialog(DialogPtr theDialog,StringPtr theString,char theChar);
void ResizeControl(ControlHandle theControl,Rect *newSize);
short BetterFindDItem(DialogPtr theDialog,Point thePoint,Boolean highest,Boolean ignoreDisabled);
Point GetLocalMouse(WindowPtr theWindow);
Boolean IsDoubleClick(EventRecord *theEvent);
void ClearDoubleClick();
void NumberToTE(DialogPtr theDialog,short theItem,long theNum);
short BetterMiscErrorStr(short resFile,short resID,short messID,short nameID,OSErr otherErrID,OSErr errID,Boolean fatal);
Boolean IsNumber(DialogPtr theWindow,char theChar,Boolean allowSigns);
void FlashTE(DialogPtr theWindow);
void SetEditItemFont(DialogPtr theDialog,short theItem);
Rect GetGWindowContentRect(WindowPtr theWindow);
Rect GetGWindowBoundingRect(WindowPtr theWindow);
short GetWinSideWidth(WindowPtr theWindow);
short GetWinTopHeight(WindowPtr theWindow);
void StaggerWindow(WindowPtr theWindow,short staggerDist);
void DrawGrowIconNoBars(WindowPtr theWindow);
void ErrorToString(OSErr theErr,StringPtr theString);
long TEToNumber(DialogPtr theDialog,short theItem,long *theRes);
void FrameButton(DialogPtr inDialog,short item);
void FlashButton(DialogPtr inDialog,short item);
void FlashControl(
	WindowPtr 	inWindow,
	ControlRef 	inControl);
void GetItemText(DialogPtr theDLog,short item,Ptr destination);
short MiscError(short resID,StringPtr message,StringPtr appName,OSErr errID,Boolean fatal);
short MiscErrorStr(short resFile,short resID,short messID,short nameID,OSErr errID,Boolean fatal);
short GetItemTextLength(DialogPtr theDlog,short theItem);
Rect ItemRect(DialogPtr theDLog,short item);
short ItemType(DialogPtr theDLog,short item);
ControlHandle ItemHandle(DialogPtr theDLog,short item);
void ToggleCBox(DialogPtr theDLog,short item,Boolean *boo);
void DoRadioGroup(DialogPtr dlg, short lo, short hi, short on);
void SetItemValue(DialogPtr theDLog,short item,short state);
short GetRadioFromGroup(DialogPtr dlg, short lo, short hi);
void MR_EnableControl(DialogPtr dlg, short controlNumber);
void MR_DisableControl(DialogPtr dlg, short controlNumber);
short ItemValue(DialogPtr theDLog,short item);
void DrawProgressBar(DialogPtr theDialog,short item,unsigned long min,unsigned long max,unsigned long val);
void BetterInvertRect(Rect *rect);
void SetItemRect(DialogPtr theDLog,short item,Rect *rect);
void MoveItem(DialogPtr theDLog,short item,short xShift,short yShift);

// Preferences.c
#pragma mark Preferences.c
OSErr GetPrefsFile(StringPtr prefFileName,short *fRefNum,OSType creator,OSType fileType,Boolean *init,Boolean getResFork);
OSErr SetPrefsFileVisibility(StringPtr name,Boolean visible);
OSErr SetFilesVisibility(short frefnum,Boolean visible);
#define	MakeFileVisible(x) SetFilesVisibility(x,true)
#define	MakeFileInvisible(x) SetFilesVisibility(x,false)
OSErr GetPrefsResource(Handle *data,ResType type,short iD,Size size,Boolean *init);
OSErr SetFileType(short frefnum,OSType type,OSType creator);

// Flood Fill.c
#pragma mark Flood Fill.c
#ifdef __QDOFFSCREEN__
void SetUpFill(GWorldPtr sourceWorld,unsigned char fillColour);
Boolean InitFill();
void KillFill();
void DoFloodFill(Point coords);
static void FillPixel(void);
static unsigned char PixColour(void);
static void MarkPixel(void);
void ShutDownFill();
#endif

// Keys.c
#pragma mark Keys.c
short IsPressed(unsigned short k );
void KeyToString (long keyMessage,char string[4],short resId);
void TypeString(StringPtr theString);
Boolean CommandPeriod();

// PixelDoublePPC.c
#pragma mark PixelDoublePPC.c
asm void PixelDouble(
   register  char *dstbase,
   register  UInt32 dstrowbytes,
   register  char *srcbase,
   register  UInt32 srcrowbytes,
   register  UInt32 rows,
   register  UInt32 words);

// Rects.c
#pragma mark Rects.c
void CopyRect(Rect sourceRect,Rect *destRect);
void CentreRectInRect(Rect *rectA,const Rect *rectB);
#define CenterRectInRect CentreRectInRect

void LocalToGlobalRect(Rect *theRect);
void GlobalToLocalRect(Rect *theRect);
float ScaleRect(
	Rect		*ioRectA,
	const Rect	*inRectB,
	Boolean		inGoBigger);
Boolean ColourRect(Rect);
signed char NewClipRect(Rect *rect,const Rect *clipRect);
// clip states returned from above
enum
{
	csClippedOff=-1,
	csNoChange=0,
	csClipped=1
};

// Memory.c
#pragma mark Memory.c

#define		IsHandleLocked(x)		((HGetState(x))&0x0080)

void FilterChars(
	char			inChar,
	Handle			ioHandle);
OSErr ResizeHandleWithInsert(Handle handle,unsigned long offset,Size wasSize,Size newSize);
Ptr FindHex(Ptr match,long matchLen,Ptr start,Ptr end);
void StringToHex(Ptr theString,Ptr destMem,short *length);
Boolean CharToHex(char *temp);
void FillMem(Ptr start,Ptr finish,char theFiller);
Boolean IsVMOn();
void FillMemString(Ptr start,Ptr finish,StringPtr theFiller);
OSErr BetterSetHandleSize(Handle theHandle,Size newSize,char fillByte);
OSErr BetterMemError();
void BinToHex(void *binaryData,void *hexBuffer,Size lengthToDo);

// Pictures.c
#pragma mark Pictures.c
void ScalePic(PicHandle thePic,Rect *theRect,Boolean goBigger);
Boolean DrawScaledPic(short pic, Rect *theRect);
PicHandle PixMapToPict(PixMapHandle inMap,Rect *sourceRect,Rect *destRect);
short SavePict(PicHandle thePic,short resID,StringPtr name,Boolean overWrite);
OSErr GetPICTCLUT(PicHandle thePic,CTabHandle *theTab);
CTabHandle FastGetPICTCLUT(PicHandle thePic,CGrafPtr thePort);
pascal void BL_CustomStdBitsProc(const BitMap *srcMap,const Rect *srcRect,const Rect *dstRect,short tMode, RgnHandle maskRgn);

// Random.c
#pragma mark Random.c
unsigned short Rnd( unsigned short min, unsigned short max );
void Randomize(long rnd);
void RandomizeByTime();

// ANSI Bits.c
#pragma mark ANSI Bits.c
#define		PrintPStr(x)		PrintPString(x)
void InputString(StringPtr theString);
void PrintPString(StringPtr string);
void PrintStr(short res,short index);
void InputNumber(long *theNum);

// Zooms.c
#pragma mark Zooms.c
void SetZoom(Boolean z);
void ZoomRect (Rect *smallrect, Rect *bigrect, Boolean zoomup) ;
void LToG (Rect *r) ;
int Blend (int i1, int i2) ;

// About box bits.c
#pragma mark About Box Bits.c

#ifdef __QDOFFSCREEN__

typedef struct
{
	GWorldPtr	buffWorld,backWorld,textWorld;
	Boolean		atBottom,atTop,transText;
	Rect		picRect;
	Rect		visRect;
	Rect		sourceRect;
	CTabHandle	theClut;
	WindowPtr	theWindow;
	short		textHeight;
	short		everyVBL;
	short		forceToBitDepth;				// set to a desired bit depth or to 0 for Lesser(pic depth,screenDepth)
	Boolean		blackText;						// if true, text is black, else it's white (defualt)
} ScrollyRec;

#define emptyScrollyRec	{0L,0L,0L,false,true,true,{0,0,0,0},{0,0,0,0},{0,0,0,0},0L,0L,0,1,0,false}

Boolean DrawTextIntoNewGWorld(short resID,Rect *ioBounds,GWorldPtr *outGWorldP,short theJust,
															StringPtr *replaceMe,StringPtr *withMe,short numToReplace,Boolean blackOnWhite);
Boolean StartupScrollyCredits(ScrollyRec *theRec,PicHandle thePic,short textID,short theJust,
														StringPtr *replaceMe,StringPtr *withMe,short numReplaces);
void ShutDownScrollyCredits(ScrollyRec *theRec);
void ScrollCredits(ScrollyRec *theRec);

#endif

// QuickDraw Routines.cpp
#pragma mark QuickDraw Routines.cpp
Boolean DrawStringTrunced(StringPtr string,short x,short y,short *width,TruncCode truncCode,Boolean condenseFirst);
unsigned short CalcLines(const unsigned char *text,Size length,short wrapWidth);
unsigned short GetLineHeight();

#ifdef __cplusplus
}
#endif
