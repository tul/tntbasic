// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST.h
// Header file containg all prototypes for the BLASTª engine
// Mark Tully
// 12/3/96
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

// NB: Don't call routines preceeded with BL_ as these are intended for internal use inside the
// BLASTª engine.

#pragma once

#include	"Marks Routines.h"
#include	<QDOffScreen.h>

#ifndef __GNUC__		// retrace.h is not available on os x
#include	<Retrace.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Window types
#define		wMonType		'MONN'
#define		wBlackType		'BLAK'
#define		wNonBlastType	'NBLT'

// ********* BLAST Moan.c *********

#pragma mark BLAST Moan.c

#define			kBlastFP			10
#define			kBlastFPLowerBits	1023

typedef struct
{
	short		frame;
	short		padder;
	Fixed		frameRate;
} Animation, *AnimPtr, **AnimHandle;

typedef struct Moan
{
	// the sprite
	short		spriteNum;					// BLAST num of the sprite

	// refcon
	void		*refCon;

	// movement info
	Fixed		startX,startY;				// starting x,y (fixed)
	Fixed		x,y;						// current x,y (fixed)
	Fixed		destX,destY;				// final x,y (fixed)
	
	Fixed		xSpeed,ySpeed;				// the pixels moved when the timer goes (fixed)
	
	// animation info
	Fixed		startFrame;					// starting frame (fixed)
	Fixed		curFrame;					// current frame (fixed)
	Fixed		endFrame;					// final frame (fixed)
	Fixed		frameRate;					// the frame rate (fixed)
	Handle		animHandle;					// handle to an anim resource
	short		bankNum;					// bank num

	// moan flags
	Boolean		currentlyMoving;			// true while the sprite is moving
	Boolean		currentlyAnimating;			// true while the sprite is animating
	Boolean		movingReset;				// set to true if this moan should move after moan reset
	Boolean		animatingReset;				// set to true if this moan should animate after reset
	Boolean		exitOnMoveEnd;				// set to true to go to the next moan on movement end
	Boolean		exitOnAnimEnd;				// set to true to go to the next moan on animation end
	Boolean		moveLoop;					// set to true if the move is going to loop
	Boolean		animLoop;					// set to true if the anim is going to loop

	// info on the next moan
	struct Moan	*theNextMoan;
	Boolean		(*moanCallBack)(struct Moan**);

	// have we finished
	Boolean		finished;

} Moan, *MoanPtr;

void MoanSprite(MoanPtr *theMoan);
short GetMoanFrame(MoanPtr theMoan);
void SetMoanFrame(MoanPtr theMoan,short theFrame);
void MoanReset(MoanPtr theMoan);
void RedisplayMoanSprite(MoanPtr *theMoan);

// ********* BLAST Monitors.c *********

#pragma mark BLAST Monitors.c

extern short	gOldDepth;

#define	UnblankMonitors() UnBlankMonitors()
#define GetDeviceDepth(dev) (**( **dev).gdPMap).pixelSize

typedef struct // used to hold info on a ChooseMonitor window
{
	WindowPtr		window;
	short			monID;
} MonWin, *MonWinPtr, **MonWinHandle;

typedef struct // used to hold the specs of the desired monitor
{
	Rect	size;	 // obvious really
	short	depth;   // hmmm
	short	count;   // this returns the number of monitors which fit the specified requirements
} MonSpec;

typedef Boolean (*MonitorProc)(GDHandle gd,long *refCon,unsigned long monID); // MonitorProc type

void RecalcVisRgns();
void ShowHideBlackOut(Boolean showIt);
Boolean BL_ShowHideBlackOutWindow(Ptr window,long *ref,unsigned long count);
Boolean BL_CountMonitorsProc(GDHandle gd,long *mon,unsigned long id);
void CountMonitors(MonSpec *mon);
Boolean BlankMonitors();
void UnBlankMonitors();
Boolean BL_IsMonOK(GDHandle gd,MonSpec *req);
void SetPlayMonitor(short monitorID);
Boolean BL_MatchMonIDtoGD(GDHandle gd,long *theMon,unsigned long id);
Boolean BL_OpenBlackOutWindow(GDHandle gd,long *monList,unsigned long id);
Boolean BL_CloseBlackOutWindow(Ptr window,long *ref,unsigned long count);
Boolean ForEachMonitorDo(MonitorProc monProc,long *ref);
Boolean ForEachSuitableMonitorDo(MonitorProc monProc,MonSpec *mon,long *ref);
Boolean ChooseMonitor(short picID,short *numMon,MonSpec *req);
Boolean BL_WhichMonitor(Ptr mon,long *ref,unsigned long count);
void BL_MonWinUpdateRoutine(WindowPtr updateWindow);
Boolean BL_OpenMonitorWindow(GDHandle gd,long *monList,unsigned long id);
Boolean BL_CloseMonitorWindow(Ptr mon,long *ref,unsigned long count);
void RestoreDepth();
Boolean NiceSetDepth(short depth);

// ********* BLAST Windows.c *********

#pragma mark BLAST Windows.c

#define		SetPlayWindow(window)	gPlayWindow=window
// Give this a PixMapHandle to get it's colour table seed. Making sure two pixmaps have the same
// seed stops copybits attempting colour mapping.
#define		GetGWorldClut(world)	GetPMClut(GetGWorldPixMap((GWorldPtr)world))
#define		GetPMClut(pixels)		((**((PixMapHandle)(pixels))).pmTable)
#define		GetPMSeed(pixels)		((**GetPMClut(pixels)).ctSeed)
#define		GetDeviceSeed(dev)		GetPMSeed((**((GDHandle)dev)).gdPMap)
#define		FixCTSeed(theWorld)		GetPMSeed(GetGWorldPixMap(theWorld))=GetDeviceSeed(gPlayDevice)
#define		AccessColour(ctab,index)	((**ctab).ctTable[index].rgb)

typedef 	void (*UpdateProc)(WindowPtr window); // UpdateProc type

WindowPtr NewNonBlastWindow(short width,short height,UpdateProc updateRoutine,Boolean goingDTS);
void SetWindPaletteFromClut(WindowPtr theWindow,CTabHandle theClut);
void HideCursorInPlayWindow();
WindowPtr ChangeWindowMode(WindowPtr wasWin,UpdateProc updateRoutine,Boolean dts);
void DisposeNonBlastWindow(WindowPtr theWindow);

// ********* BLAST Main.c *********

#pragma mark BLAST Main.c

#define		StartupBlast	StartUpBlast

Boolean StartUpBlast();
void ShutDownBlast();
Boolean NiceWaitMouseClick(UpdateProc theirUpdateRoutine,long timeOut,EventRecord *returnRec);
void BlastUpdate(WindowPtr updateWindow,UpdateProc theirUpdateRoutine);

// ********* BLAST Fades.c *********

#pragma mark BLAST Fades.c

enum
{
	k32BitPicOffset=0,
	k16BitPicOffset,
	k8BitPicOffset,
	kPicsPerFrame
};

#define			kCurrentFadePicParamsVersion		1

typedef struct
{
	unsigned long	version;			// set to kCurrentFadeParamsVersion
	short			resFile;			// res file to get the pictures from, -1 for current
	short			fadePic;			// pic to fade. id is direct col, id+1 is indexed col
	
	unsigned char	fadeSpeed;			// speed of the fades, lesser the faster
	unsigned long	timeOut;			// time each pic is displayed for in ticks
	Boolean			canSkip;			// should the routine allow skipping
	Boolean			didTheySkip;		// true if the user skipped, or false if it reached the
										// end uninterrupted
} TFadeParams, *TFadeParamsPtr;

Boolean StartUpFadeModule();
void ShutDownFadeModule();
void EckerFadeUp(unsigned char *startAt,Byte finishAt,Boolean *skipFade,Byte speed,Boolean allMonitors);
void EckerFadeDown(unsigned char *startAt,Byte finishAt,Boolean *skipFade,Byte speed,Boolean allMonitors);
void FadeDown(Byte fadeSpeed,Boolean allMonitors);
OSErr DoBlastSlideShow(TFadeParamsPtr params,short numPics,Boolean loop);
void FadeUp(Byte fadeSpeed,Boolean allMonitors);
OSErr FadePic(TFadeParamsPtr params);
void BL_UpdateFadeWindow(WindowPtr theWindow);

// ******** BLAST Tint.c *********

#pragma mark BLAST Tint.c

#define	tintSize		256
typedef unsigned char	TintPtr[256];
typedef	TintPtr			**TintHandle;

#define		kTintResType	'TINT'
void MakeTintMapShifted(CTabHandle theClut,TintPtr theTint,long rShift,long gShift,long bShift);
void ShiftClut(CTabHandle theClut,long rShift,long gShift,long bShift);
void ShiftColour(RGBColor *theCol,long rShift,long gShift,long bShift);
void MakeTintMapTinted(CTabHandle theClut,TintPtr theTint,long rPerCent,long gPerCent,long bPerCent);
void TintColour(RGBColor *theCol,long rPerCent,long gPerCent,long bPerCent);
void TintClut(CTabHandle theClut,long rShift,long gShift,long bShift);
unsigned char BlastRGB2Index(CTabHandle theTable,RGBColour *theCol);
short RGB2IndexGW(CTabHandle theTable,RGBColor *theCol);
QDErr OpenTintWorld(CTabHandle theTable);
void CloseTintWorld();
void ApplyOpacityClut(CTabHandle theClut,RGBColour *opacityOverlay,double opacity);
void ApplyOpacityColour(RGBColour *sourceColour,RGBColour *opacityOverlay,double opacity);
void MakeOpacityMap(CTabHandle theClut,TintPtr theTint,RGBColour *opacityCol,double opacity);

// ********* BLAST Images.c *********

#pragma mark BLAST Images.c

#define		kImageResType		'IMAG'
#define		kBankResType		'BANK'

#define		kLoadFragmented		0
#define		kLoadContinuous		0x0100

#define		AccessImage(bank,image)		(gBankList[(bank)]->imageList[(image)])

typedef struct
{
	short	xoff,yoff;		// offsets for this image
	short	width,height;	// dimensions of this image
} ImageData, *ImagePtr, **ImageHandle;

typedef struct
{
	short		maxImages;
	short		usedImages;
	TintHandle	bankTint;
	ImageHandle	*imageList;
} Bank, *BankPtr, **BankHandle;

typedef short **DiskBankHandle; // disk bank handle is a handle to an array of shorts

// Blast image globals
extern BankPtr			*gBankList;
extern short			gMaxBanks,gBanksUsed;

short ImagesInBank(short bankNum);
Boolean InitBanks(short num);
short AllocateBank(short bankNum,short images);
Boolean DisposeBank(short bankNum);
void DisposeImage(short bank,short image);
ImageHandle	LoadImage(short resID);
ImageHandle	GetImageFromBank(short bank,short image,Boolean detachIt);
Boolean LoadBankFromDisk(short resID,short pos,Boolean append);
Boolean ImageExists(short bank,short image);
void ShutDownSpriteModule();
void DisposeBankList();
void DisposeAllBanks();
short NextFreeBank();
short NextFreeImage(short bank);
Boolean SwapImages(short bankA,short imageA,short bankB,short imageB);
Boolean SwapBanks(short bankA,short bankB);
short LoadImageIntoBank(short resID,short bank,short pos);
short PutImageInBank(short bank,short pos,ImageHandle theImage);
TintHandle LoadTint(short resID);
short LoadTintIntoBank(short resID,short bank);

// ********* BLAST Copy.c *********

#pragma mark BLAST Copy.c

// blast copy rec (see MacBCRec)
typedef struct
{
	unsigned long	bcRecVersion;	// always kCurrentBCRecVersion
	GWorldPtr		world;
	PixMapHandle	pixMap;
	unsigned char	*baseAddr;
	unsigned long	rowBytes;
} BCRec, *BCPtr, BDRec, *BDPtr;
// BlastCopy record, AKA BlastDraw record

// advanced blast copy rec (see MakeABCRec)
typedef struct
{
	BCRec			rec;
	unsigned long	abcRecVersion;	// always kCurrentABCRecVersion
	Ptr				customBitMap;
	short			height;
	short			width;
	unsigned char	**rowsTable;
} ABCRec, *ABCPtr;

enum
{
	kCurrentBCRecVersion=1,
	kCurrentABCRecVersion=1
};

#define		emptyBCRec		{kCurrentBCRecVersion,0L,0L,0L,0L}
#define		emptyABCRec		{emptyBCRec,kCurrentABCRecVersion,0L,0,0,0L}

// Modes for blast copy
#define		kBuff2Back	&gBuffRec,&gBackRec
#define		kBack2Buff	&gBackRec,&gBuffRec

// Blast colour tables
typedef UInt32					BlastColour24;
typedef unsigned short			BlastColour16;
typedef BlastColour16			BlastColourTable16[256];
typedef	BlastColourTable16		**BlastColourTable16Handle;

// these are the number of bits to shift the short to access each of the colour components.
enum
{
	kRedShift16=10,
	kGreenShift16=5,
	kBlueShift16=0,
	kRedShift24=16,
	kGreenShift24=8,
	kBlueShift24=0
};

//				0011 1101 1110 1111			=		0x3DEF
#define			BlastBlend1650(inColA,inColB)		(((inColA>>1) & 0x3DEF) + ((inColB >>1) & 0x3DEF))

void InitBCRec(BCPtr rec);
void InitABCRec(ABCPtr rec);

void BlastCopyScaledClip(BCPtr sourceRec,BCPtr destRec,Rect *origScaleRect,Rect *clipRec,
											short destX,short destY,Fixed xScale,Fixed yScale);
void BlastCopyScaledNoClip(BCPtr sourceRec,BCPtr destRec,Rect *origScaleRect,short destX,short destY,
																	Fixed xScale,Fixed yScale);
void BlastCopyScaledNoClip16(BCPtr sourceRec,BCPtr destRec,Rect *scaleRect,short destX,short destY,
																	Fixed xScale,Fixed yScale);
void DestroyABCRec(ABCPtr theRec);
Boolean BCToABCRec(ABCPtr theRec);
Boolean AllocateABCRec(short width,short height,ABCPtr theRec);
Boolean MakeABCRecFromGWorld(GWorldPtr theWorld,ABCPtr theRec);
void BlastCopy16(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect);
void BlastCopy16Mask(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect,unsigned short maskColour);
#define	BLASTCopy(a,b,c,d)	BlastCopy(a,b,c,d)
#define	BLASTCopyCB(a,b,c,d)	BlastCopyCB(a,b,c,d)
void BlastCopy(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect);
void BlastCopyCB(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect);
void BlastCopyMaskRect(BCPtr source,BCPtr dest,const Rect *sourceRect,const Rect *destRect,Rect *maskRect);
void Check601();
void MakeBCRecFromPM(PixMapHandle thePixMap,BCPtr theRec);
void MakeBCRecFromGWorld(GWorldPtr theWorld,BCPtr theRec);
void MakeBCRecFromWindow(CWindowPtr theWindow,BCPtr theRec,GDHandle deviceH);

void BlastCopy8To16(BCPtr source,BCPtr dest,Rect *sourceRect,Rect *destRect,BlastColourTable16 blastColourTable);
void ClutToBlastColourTable16(CTabHandle ctab,BlastColourTable16 blastColourTable);
void BlastColour16ToRGBColour(BlastColour16 colour,RGBColour *rgb);
void BlastColour16ToRGBColourAccurate(
	BlastColour16	inColour,
	RGBColour		*outColour);
BlastColour16 RGBColourToBlastColour16(const RGBColour *rgb);

void BlastColour24ToRGBColour(BlastColour24 colour,RGBColour *rgb);
BlastColour24 RGBColourToBlastColour24(const RGBColour *rgb);

// ********* BLAST Pixel Double EPX.c *********
void BlastPixelDoubleNoClip16EPX(BCPtr inSourceRec,BCPtr outDestRec,Rect *inDoubleRect,short inDestX,short inDestY);
void BlastPixelDoubleNoClip8EPX(BCPtr inSourceRec,BCPtr outDestRec,Rect *inDoubleRect,short inDestX,short inDestY);
void BlastPixelDoubleNoClip8EPXNoWrapAround(BCPtr inSourceRec,BCPtr outDestRec,Rect *inDoubleRect,short inDestX,short inDestY);
void BlastPixelDoubleNoClip16EPXNoWrapAround(BCPtr inSourceRec,BCPtr outDestRec,Rect *inDoubleRect,short inDestX,short inDestY);

// ********* BLAST Sprites.c *********

#pragma mark BLAST Sprites.c

typedef struct
{
	short		x,y;			// coords of sprite
	Boolean		floating;		// set to true for the sprites pos to be measured in "hardware"
								// coords. (from the top left of the SCREEN instead of the back
								// drop)
	Boolean		drawMe;			// set to true if this sprite is to be displayed
	Boolean		updateBackDrop;	// set if the backdrop from this sprite needs to be redrawn
	Boolean		clipMe;			// set if the sprite was found to need clipping when it's updateRect was being calculated
	Boolean		scaleMe;		// set if the sprite is to be scaled by the values below
	Fixed		xScale,yScale;	// scale values for the sprite
	Rect		updateRect;		// the coords of the screen destroyed by this sprite
	ImageHandle	data;			// pointer to this sprites image data
	TintHandle	theTint;		// handle to the tint for this sprite
} OldBlastSprite, *OldBlastSpritePtr, **OldBlastSpriteHandle;

typedef struct
{
	Fixed	constant1;
	Fixed	constant2;
	Fixed	topPos,bottomPos;
	Fixed	topScale,bottomScale;
} Perspec, *PerspecPtr;

// Blast sprite globals
extern OldBlastSpritePtr	*gSpriteList;
extern short				gMaxSprites,gNumSprites,*gDrawList;

void SetNumUpdateRects(short theNum);
void DisableDTSMode();
void SpritesScaleOff();
void DoMoanPerspective(PerspecPtr thePersp,MoanPtr theMoan);
void DoSpritePerspective(PerspecPtr thePersp,short theSprite);
void SetupPerspective(PerspecPtr thePersp,short topPos,Fixed topScale,short bottomPos,Fixed bottomScale);
void GetClipRect(Rect *theRect);
void SetClipRect(Rect *theRect);
void SetSpriteTintH(short spriteNo,TintHandle theTint);
void SpriteScaleOff(short spriteNo);
void SetSpriteScale(short spriteNo,Fixed xScale,Fixed yScale);
void SetBoundsRect(short top,short left,short bottom,short right);
Boolean SetUpYPrioritise();
void YPrioritise(short fromMe);
void GetSpriteRect(short spriteNo,Rect *theRect);
void ResetDrawList();
void GetSpritePos(short spriteNo,Point *thePos);
void BlastInvalRect(Rect *theRect);
void SetSpriteImageH(short spriteNo,ImageHandle newImage);
void SetSpriteFloat(short spriteNo,Boolean floatMe);
Boolean InitSprites(short numSprites);
void DisposeSpriteList();
void DisposeOldBlastSprite(short pos);
short AllocateSprite(short pos);
short NextFreeSprite();
void RenderSprites();
void UpdateBuffWorld();
void SetBuffWorld(GWorldPtr theWorld);
void SetBackWorld(GWorldPtr theWorld);
void SetSpriteImage(short spriteNo,short bankNo,short imageNo);
void SetSpritePos(short spriteNo,short x,short y);
void SpriteOff(short num);
void SpritesOff();
void SpriteOn(short num);
void SpritesOn();
void UpdateScreen(Boolean fullUpdate);
static void AddRectToBlitList(short *rectCount,Rect *theRectList,Rect *theRect);
static Boolean RectIsInList(Rect *theRect,short numRects,Rect *rectList);
static void AddDifferenceToBlitList(short *numRects,Rect *rectList,Rect *lastRect,Rect *thisRect);
void ScreenOffset(short x,short y);
void EnableDTSMode(BCRec *theRec);

// ********* BLAST Draw.c *********

#pragma mark BLAST Draw.c

extern		short		*gLeftBoundry,*gRightBoundry;
extern		short		gClipTop;
extern		short		gClipBottom;
extern		short		gClipLeft;
extern		short		gClipRight;

typedef struct
{
	short			x,y;
} Coord, *CoordPtr;

// couple of macros for writing a pixel to a bit map. Efficient, but you could probabaly better
// them if you were doing a lot of pixels by taking the contents of the bcrec and placing it
// in variables instead of the structure. Variables will, most likely, be in registers.

// parameters : BlastSetPixel(BCPtr theRec,short x,short y,unsigned char col)
#define		BlastGetPixel(theRec,x,y) 			(*((theRec)->baseAddr+(y)*(theRec)->rowBytes+(x)))
#define		BlastSetPixel(theRec,x,y,col) 		(BlastGetPixel(theRec,x,y)=col)

#define		BlastGetPixelAdv(theRec,x,y)		((theRec)->rowsTable[y][x])
#define		BlastSetPixelAdv(theRec,x,y,col)	(BlastGetPixelAdv(theRec,x,y)=col)

void BlastTintRect16(BCPtr inTintRec,Rect *inTintRect,BlastColour16 inTintColour,double inPercent);
void BlastTintRect1650(BCPtr inTintRec,Rect *inTintRect,BlastColour16 inTintColour);
void BlastTintRect(BCPtr theRec,Rect *tintMe,TintPtr tintPtr);
void BlastDrawRect(BCPtr theRec,Rect *eraseMe,unsigned char colIndex);
void BlastLine(BCPtr theRec,short sourceX,short sourceY,short destX,short destY,unsigned char col);
void BlastDrawTriangle(BCPtr theRec,CoordPtr theCoords,unsigned char theCol);
void CreateBoundry(short *boundy,short sourceX,short sourceY,short destX,short destY);
void BlastDrawCVPolygon(BCPtr theRec,CoordPtr theCoords,short coordCount,unsigned char colIndex,Boolean acw);
Boolean InitBlastPolygon(short pixmapHeight);
void DisposeBlastPolygon();
void ShutDownBlastDraw();
void SetBlastDrawClip(short top,short left,short bottom,short right);
void BlastLineBresenham(BCPtr theRec,short sourceX,short sourceY,short destX,short destY,unsigned char col);
void NewCreateBoundry(short *boundry,short sourceX,short sourceY,short destX,short destY);
void BlastLineX2(BCPtr theRec,short sourceX,short sourceY,short destX,short destY,unsigned char col);
void BlastTintLine(BCPtr theRec,TintPtr tintMap,short sourceX,short sourceY,short destX,short destY);

// ********* BLAST GWorlds.c *********

#pragma mark BLAST GWorlds.c

OSErr SetGWPaletteFromPICT(GWorldPtr theWorld,PicHandle thePic);
OSErr DrawPicIntoWorld(GWorldPtr drawWorld,PicHandle thePic,short x,short y);
OSErr NewGWorldWithPic(GWorldPtr *theWorld,short picID,short depth);
OSErr NewNonBlastGWorld(GWorldPtr *theWorld,Rect *theRect,short depth,CTabHandle *theCLUT,short clutID,Boolean customClut);
void EraseGWorld(GWorldPtr theWorld);
OSErr NewGWorldWithPicHandle(GWorldPtr *theWorld,PicHandle thePicture,short depth);
void ChangeGWorldClut(GWorldPtr theWorld,CTabHandle theClut);
void BlastEraseRect(GWorldPtr theWorld,Rect *eraseMe,Byte colIndex);
Boolean PixelMarked1Bit(BCPtr theRec,Point thePoint);

// ********* BLAST Encode.c *********

#pragma mark BLAST Encode.c

typedef struct
{
	PixMapHandle	thePixMap;
	ImageHandle		theImage;
	BCRec			*theRec;
	Rect			*theRect;
	short			clearCol;
	short			shadowCol;
	Boolean			shrinkHandle;
	Boolean			shrinkRect;
} EncodeRec, *EncodePtr;

#define		emptyEncodeRec		{0L,0L,0L,0L,-1,-1,true,true}

// Runmode constants
enum
{
	rmNoRun,
	rmDrawRun,
	rmShadowRun,
	rmSkipRun,
	rmLineStart,
	rmEndShape
};

Size WorstCaseEncode(Rect *theRect);
Boolean EckerEncodeTheRect(EncodePtr theEncode);
short GPixelColour(PixMapHandle thePixMap,short x,short y);
ImageHandle EncodeTheRect(PixMapHandle thePixMap,Rect *theRect,short clearCol,short shadowCol);
short FindGWUnusedCol(GWorldPtr theWorld);

// ********* BLAST Decode.c *********

#pragma mark BLAST Decode.c

// give it a length in pixels and it tells you how many pixels it will be after the scale
// you really don't want to be calling this in extremely speed critical code. I don't know
// how well the compiler will optimise it but I see a division and 2 mod ops in there which
// aren't the fastest of ops you can do.

// unfixed in, unfixed out
#define		ScaledLength(length,scale)		((((length)<<kBlastFP) % (scale) ? 1 : 0) +( ((length)<<kBlastFP) -( ((length)<<kBlastFP) %(scale)))/(scale))
#define		ScaledLengthUU(length,scale)	ScaledLength(length,scale)

// fixed in, unfixed out
#define		ScaledLengthFU(length,scale)	(((length) % (scale) ? 1 : 0) +( (length) -( (length) %(scale)))/(scale))

// fixed in, fixed out
#define		ScaledLengthFF(length,scale)	(ScaledLengthFU(length,scale)<<kBlastFP)

// unfixed in, fixed out
#define		ScaledLengthUF(length,scale)	(ScaledLengthUU(length,scale)<<kBlastFP)

// These macros are used to find out how many pixels a length was before it was scaled. These
// results can be out by up to one scale factor (in the positive) due to rounding in the
// scaling down.

// unfixed in unfixed out
#define		UnscaledLength(length,scale)	(((length)*(scale))>>kBlastFP)
#define		UnscaledLengthUU(length,scale)	UnscaledLength(length,scale)

// unfixed in fixed out
#define		UnscaledLengthUF(length,scale)	((length)*(scale))

// fixed in fixed out
#define		UnscaledLengthFF(length,scale)	UnscaledLengthUF((length)>>kBlastFP,scale)

// fixed in unfixed out
#define		UnscaledLenghtFU(length,scale)	UnscaledLength((length)>>kBlastFP,scale)

void DecodeImageNoClip(BCPtr theRec,ImageHandle data,TintPtr tintMap,short xPos,short yPos);
void DecodeImageClip(BCPtr theRec,ImageHandle data,TintPtr tintMap,Rect *globalClipRect,short xPos,short yPos);
void Set601Presence(Boolean here);
void DecodeImageScaled(BCPtr theRec,ImageHandle data,TintPtr tintMap,short xPos,short yPos,Fixed xScale,Fixed yScale);
void DecodeImageScaledClip(BCPtr theRec,ImageHandle data,TintPtr tintMap,Rect *globalClipRect,short xPos,short yPos,Fixed xScale,Fixed yScale);
void ApplyTintToImage(ImageHandle theHandle,TintPtr theTint);

// ********* BLAST VBL.c *********

#pragma mark BLAST VBL.c

#ifndef __GNUC__		// os x doesn't support this old vbl stuff

typedef struct ExtendedVBLTask
{
	VBLTask		vblRec;
	void		(*customProc)(struct ExtendedVBLTask*);
	short		masterVblCount;
	void		*refCon;
	#ifndef powerc
	long		appA5;		// extra storage for the A5 value if doing it for 68K
	#endif
} ExtendedVBLTask, *ExVBLTaskPtr, ExVblRec, *ExVblPtr;

void Remove60HzTask(ExVBLTaskPtr *vblRec);
OSErr Install60HzTask(ExVBLTaskPtr *vblRec,void *refCon,short vblCount,ProcPtr theTask);
pascal void PreProcess60HzTask(ExVBLTaskPtr vblRec);

#endif

// ********* BLAST Text.c *********

#pragma mark BLAST Text.c

typedef struct
{
	BCRec			textRec;
	Rect			**lCoords;
	short			spaceWidth;
	unsigned char	transp,remapMe;
} BlastTextInfo, *BTIPtr, **BTIHandle;

typedef struct
{
	short			spaceWidth,remapCol,transpCol;
} BlastTextExtras, *BTEPtr, **BTEHandle;

#define			kLetterCoordResType			'LCRD'
#define			kBlastTextExtrasResType		'BTXT'

OSErr LoadBlastText(BTIPtr theInfo,short theFile,short theResId);
short BlastGetLetterId(char theChar);
void BlastDrawString(BCPtr theRec,BTIPtr theInfo,StringPtr theString,short x,short y,unsigned char col);
void DisposeBlastText(BTIPtr theInfo);
short BlastTextWidth(BTIPtr theInfo,StringPtr theString,short startPos,short length);

#ifdef __cplusplus
}
#endif

// Note, this has to be at the bottom

#include	"BLAST Globals.h"
