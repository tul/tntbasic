// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// TNT Basic
// CApplication.cpp
// © Mark Tully 2000
// 4/2/00
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#include		"TNT_Debugging.h"
#include		"FileCopy.h"
#include		"MoreFilesExtras.h"

#include		"Carbon_Settings.h"

#include		<new>
//#include		<FSp_fopen.h>
#include		<UProcess.h>

#include		<UNavServicesDialogs.h>
#include		<LScrollerView.h>
#include		<LScrollBar.h>
#include		<LAMTrackActionImp.h>
#include		<LGAFocusBorder.h>
#include		<LSeparatorLine.h>
#include		<LGASeparatorImp.h>
#include		<LPushButton.h>
#include		<LAMPushButtonImp.h>
#include		<LPopupButton.h>
#include		<LAMPopupButtonImp.h>
#include		<LStaticText.h>
#include		<LAMStaticTextImp.h>
#include		<LTextGroupBox.h>
#include		<LAMTextGroupBoxImp.h>
#include		<LProgressBar.h>
#include		<LCheckBox.h>
#include		<LEditText.h>
#include		<LAMEditTextImp.h>
#include		<LImageWell.h>
#include		<LAMImageWellImp.h>
#include		<LTextColumn.h>

#include		"CProgramInfo.h"
#include		"Utility Objects.h"
#include		"CApplication.h"
#include		"CStdErrorBroker.h"
#include		"UErrors.h"
#include		"CError.h"
#include		"TNTBasic_Resources.h"
#include		"Useful Defines.h"

#include		"UFileManager.h"
#include		"UTNTMusicManager.h"
#include		"UKaboom.h"
#include		"UFuse.h"

#include		"CResourceContainerCreator.h"
#include		"CResourceContainer.h"
#include		"UResources.h"
#include		"CResource.h"
#include		"CResourceManager.h"
#include		"CMacResFileCreator.h"
#include		"CBundleResFileCompatCreator.h"
#include		"CTBResClassifier.h"

#include		"UEncryption.h"
#include		"CFPrefsHelpers.h"

#include		"CGraphicsContext.h"
#include		"CProgram.h"
#include		"CStatement.h"
#include		"UTimingUtils.h"
#include		"OutputOperators.h"
#include		"Version.h"
#include		"TNTBasic_Commands.h"
#include		"TNTBasic_AppleEvents.h"

#include		"TNewHandleStream.h"
#include		"UHelp.h"
#include		"MoreFiles.h"
#include		"UCursorManager.h"
#include		"UEditServer.h"

// Background console debugging support
// Use ECHO() to output things to a file in the "Console" folder in the preferences directory. Run
// the BkgConsole application to view the output.
//#include		"UBkgConsole.h"
//#include		<PDebug.h>
//#include		<PConsoleSink.h>	// required here for DEBUG_STARTUP() macro
#include		"Root.h"

// Turn on the 'Profiler Information' in the PPC processor control panel to get this
#if __profile__
#include		<Profiler.h>
#endif

extern int gLineno;

// tntbasicprivatecode	with the option key held
#define			kCodeEncryptionKey	"†~†∫åß^çπ®^√å†´çø∂´"

#define			kShowFPSPrefsStr	CFSTR("Show FPS")

#define			STD_EVENT_LIST		{ kEventClassMouse, kEventMouseMoved }, { kEventClassAppleEvent, kEventAppleEvent }, { kEventClassApplication, kEventAppDeactivated }, { kEventClassWindow, kEventWindowDeactivated }, { kEventClassMouse, kEventMouseDown }, { kEventClassKeyboard, kEventRawKeyDown }, { kEventClassKeyboard, kEventRawKeyUp }, { kEventClassKeyboard, kEventRawKeyRepeat }, { kEventClassKeyboard, kEventRawKeyModifiersChanged }, { kEventClassWindow, kEventWindowUpdate}

CApplication			*CApplication::sApplication=0;
const EventTypeSpec		CApplication::kStdEventsList[]				={ STD_EVENT_LIST };
const EventTypeSpec		CApplication::kStdEventsListPlusAllMouse[]	={ STD_EVENT_LIST , { kEventClassMouse, kEventMouseUp }, { kEventClassMouse, kEventMouseDragged } };
const UInt16			CApplication::kStdEventsListCount=sizeof(kStdEventsList)/sizeof(kStdEventsList[0]);
const UInt16			CApplication::kStdEventsListPlusAllMouseCount=sizeof(kStdEventsListPlusAllMouse)/sizeof(kStdEventsListPlusAllMouse[0]);
static int				mainFunc();

#define RUN_PROGRAM_AS_CARBON_TIMER		0

extern int basicdebug;
int		g_argc;
char	**g_argv;

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• main
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
int main(int argc, char *argv[])
{
	g_argc=argc;
	g_argv=argv;
	return mainFunc();
}

class CAppEventHandler : public LEventHandler
{
	public:
							CAppEventHandler()
							{
								EventTargetRef		target=::GetApplicationEventTarget();
								
								Install(target,kEventClassCommand,kEventCommandProcess);
							}
							
		OSStatus 			DoEvent(
								EventHandlerCallRef	inCallRef,
								EventRef			inEventRef)
							{
								OSStatus			err=eventNotHandledErr;
								CApplication		*app=CApplication::GetApplication();
								if (app)
								{
									err=app->DoEvent(inCallRef,inEventRef);
								}
								return err;
							}
};

static int mainFunc()
{
	SetDebugThrow_(PP_PowerPlant::debugAction_Nothing);	// Set Debugging options
	SetDebugSignal_(PP_PowerPlant::debugAction_Alert);

	// to enable debugging - set YYDEBUG in basic.yacc to 1 and set this var here to true
//	basicdebug=true;

#if __MWERKS__		// this is how you stop MSL throwing excpetions when out of memory
	std::__throws_bad_alloc=false;						// Stop exceptions under low memory situations
#elif __GNUC__
	// under gcc this is done in the prefix file by stating that we don't want to use the version of new that throws
	// TODO - verify this works!
#endif

	PP_PowerPlant::InitializeHeap(5);					// Initialize Memory Manager
														// Parameter is number of Master Pointer
														// blocks to allocate
	
	PP_PowerPlant::UQDGlobals::InitializeToolbox();		// Initialize standard Toolbox managers
	
	{
		UInt32	time;
		
		::GetDateTime(&time);
		std::srand(time);
	}

	UBkgConsole::OpenDefaultLogFile();
	// echo time and date of run to the debug file
	UBkgConsole::DebugHeader();

// It should be possible to get the help book added to the help centre without having to
// copy it into any help/system folders. This doesn't work tho.
/*Try_
{
	FSSpec		spec;
	FSRef		ref;
	
	ThrowIfOSErr_(GetAppSpec(&spec));	
	CopyPStr("\pTNT Basic Help",spec.name);
//	ThrowIfOSErr_(MakeFSSpecFromFileSpec(&spec,"\pTNT Basic Help.htm"));
	ThrowIfOSErr_(::FSpMakeFSRef(&spec,&ref));
	ThrowIfOSErr_(::AHRegisterHelpBook(&ref));
}
Catch_(err)
{
	ECHO("Error registering help: " << ErrCode_(err));
}*/

	
	new PP_PowerPlant::LGrowZone(20000);				// Install a GrowZone function to catch
														// low memory situations.

	CStdErrorBroker	brok(CurResFile());					// error broker to map err code to strings

	Try_	
	{
		CApplication		theApp;								// create instance of TNT Basic application
		CAppEventHandler	appE;

#if __profile__
		ECHO("*** Starting profiler... ***");
		::ProfilerInit(collectDetailed,bestTimeBase,400,100);
#endif
		
		theApp.Run();

#if __profile__
		ECHO("*** Stopping profiler... ***");
		::ProfilerTerm();
#endif
		
		FlushEvents(everyEvent,0L);
	}
	Catch_(err)
	{
		if (ErrCode_(err))	// errcode 0 means it has already been reported
		{
			CCString		str("Unhandled exception caught - this should NOT happen. Please email bugs@tntbasic.com. The error code of the exception was ");
			str.Append(ErrCode_(err));
			str+=".\rTNT Basic will now exit.";
			SignalString_(str);
		}
		
		// this will output noerr if the err has already been reported
		ECHO("Unhandled exception caught in CApplication.cpp, will exit now, err code " << ErrCode_(err));
	}
		
	// we don't need to call LCleanUpTask::CleanUpAtExit() here as it's done by the cfm terminator routine


	ECHO("TNT Basic terminated normally");

	return 0;
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• Constructor
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Register the used powerplant classes
CApplication::CApplication() :
	mCurrentProgram(0),
	mProgramPaused(false),
	mShowFPS(false),
	mAllowFullScreen(true),
	mMacResFileCreator(NULL),
	mBundleResFileCreator(NULL)
{
	Try_
	{
		mAppletMode=false;
		mReturnToApp=0;
		sApplication=this;
		mAppResumed=UProcess::AmIFront();
		mAppResFile=CurResFile();
		ClearKeyboard();
		
		// System load
		if (PP_PowerPlant::UEnvironment::HasFeature(PP_PowerPlant::env_HasAppearance))
			::RegisterAppearanceClient();
			
		RegisterClass_(LWindow);
		RegisterClass_(LTextEditView);
		RegisterClass_(LScrollerView);
		RegisterClass_(LScrollBar);
		RegisterClassID_(LAMTrackActionImp, LScrollBar::imp_class_ID);	
		
		UNavServicesDialogs::Load();
		CGraphicsContext::sMsTimer.StartTask();
		
		ThrowIfMemFull_(new CResourceManager);
		CBundleResFileCompatClassifier	*classi=new CTBResClassifier;
		ThrowIfMemFull_(classi);
		ThrowIfMemFull_(mBundleResFileCreator=new CBundleResFileCompatCreator(classi));
		ThrowIfMemFull_(mMacResFileCreator=new CMacResFileCreator);
		
		Try_
		{
			UKaboom::Initialise();
		}
		Catch_(err)
		{
			Str255		str;
			CCString	errstr("Error initialising sound engine.\r");
			ErrStr_(err,str);
			errstr+=str;
			SignalString_(errstr);
			Throw_(0);	// 0 means don't report error
		}
		UTNTMusicManager::Initialise(USE_CARBON_TIMERS);
		CInputManager::Startup();
		CGraphicsContext::Initialise();
		
		// Register some classes
		RegisterClass_(LScrollerView);
		RegisterClass_(LScrollBar);
		RegisterClassID_(LAMTrackActionImp, LScrollBar::imp_class_ID);
		
		RegisterClass_(LGAFocusBorder);
		
		RegisterClass_(LColorEraseAttachment);

		RegisterClass_(LTabGroup);

		RegisterClass_(LControl);

		RegisterClass_(LWindowThemeAttachment);

		RegisterClass_(LDialogBox);
		RegisterClass_(LPicture);
		
		RegisterClass_(LTextColumn);
		
		RegisterClass_(LSeparatorLine);
		RegisterClassID_(LGASeparatorImp,LSeparatorLine::imp_class_ID);
		
		RegisterClass_(LPushButton);
		RegisterClassID_(LAMPushButtonImp,LPushButton::imp_class_ID);
		
		RegisterClass_(LPopupButton);
		RegisterClassID_(LAMPopupButtonImp,LPopupButton::imp_class_ID);
		
		RegisterClass_(LStaticText);
		RegisterClassID_(LAMStaticTextImp,LStaticText::imp_class_ID);
		
		RegisterClass_(LTextGroupBox);
		RegisterClassID_(LAMTextGroupBoxImp,LTextGroupBox::imp_class_ID);
		
		RegisterClass_(LProgressBar);
		RegisterClassID_(LAMTrackActionImp,LProgressBar::imp_class_ID);
		
		RegisterClass_(LCheckBox);
		RegisterClassID_(LAMControlImp,LCheckBox::imp_class_ID);
		
		RegisterClass_(LEditText);
		RegisterClassID_(LAMEditTextImp,LEditText::imp_class_ID);
		
		RegisterClass_(LImageWell);
		RegisterClassID_(LAMImageWellImp,LImageWell::imp_class_ID);
		
		RegisterClass_(LCicnButton);
		
		if (::ResourceExists(-1,kTNTBasic_EncryptedCodeResType,kTNTBasic_EncryptedCodeResId))	// are we an applet
			mAppletMode=true;

		if (mAppletMode==false)
		{
			UHelp::Init();
			UEditServer::Initialise();
			
			// process other prefs
			if (!CFPrefsGetBool(kShowFPSPrefsStr,mShowFPS))
				mShowFPS=false;
		}

		ParseCmdLine();
			
	//	mBeepHandler.Install(GetApplicationEventTarget(),kEventClassKeyboard,kEventRawKeyDown,this,&HandleCarbonKey);
	}
	Catch_(err)
	{
		if (ErrCode_(err))
		{
			Str255		str;
			CCString	errstr("Fatal error starting up TNT Basic engine.\r");
			ErrStr_(err,str);
			errstr+=str;
			SignalString_(errstr);
		}
		Throw_(0);	// don't report error
	}
}


//OSStatus
//CApplication::HandleCarbonKey(
//	EventHandlerCallRef	/* inCallRef */,
//	EventRef			/* inEventRef */)
//{
//	SysBeep(0);
//
//	return noErr;
//}


// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• Destructor
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
CApplication::~CApplication()
{
	EndProgram();		// terminate the currently running program

	// System shutdown
	UNavServicesDialogs::Unload();
	CGraphicsContext::sMsTimer.StopTask();
	
	UKaboom::ShutDown();
//	CMADDriver::ShutDown();
	CInputManager::Shutdown();
	CGraphicsContext::Shutdown();
//	UMusicManager::ShutDown();
	UTNTMusicManager::ShutDown();
	UEditServer::Shutdown();

	UBkgConsole::CloseLogFile();
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ParseCmdLine
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Read cmd line parameters - useful for debugging
void CApplication::ParseCmdLine()
{
	for (int i=1; i<g_argc; i++)
	{
		if (strcmp(g_argv[i],"--nofullscreen")==0)
		{
			mAllowFullScreen=false;
		}
		else
		{
			printf("Unknown cmd line argument '%s'\n",g_argv[i]);
		}
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• MakeMenuBar
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Overridden to delete the quit item on x and to set the quick key for take screen shot to f11
void CApplication::MakeMenuBar()
{
	inheritedApp::MakeMenuBar();

	// If we're running on OS X remove the Quit menu item and separator
	LMenuBar		*menuBar=LMenuBar::GetCurrentMenuBar();
	MenuHandle		menu=0;
	SInt16			item=0;
	SInt16			menuId=0;

	long	result;
	OSErr	err=::Gestalt(gestaltMenuMgrAttr,&result);
    
	if (!err && (result & gestaltMenuMgrAquaLayoutMask))
	{
		menuBar->FindMenuItem(cmd_Quit,menuId,menu,item);

		if (menu)
		{
			LMenu		*fileMenu=menuBar->FetchMenu(menuId);
			
			if (fileMenu)
			{
				fileMenu->RemoveItem(item-1);	// remove quit and sep bar
				fileMenu->RemoveItem(item-1);	// note quit moves up one
			}
		}
	}

	
	if (menuBar)
	{
		menuBar->FindMenuItem(cmd_TBTakeScreenShot,menuId,menu,item);
			
		if (menu)
			::SetMenuItemKeyGlyph(menu,item,kMenuF11Glyph);
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• MakeDataFilePathForResContainer						/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// creates a data file path handle which records where the passed res container fsspec is so that when run in applet mode the app
// can find the game data
Handle CApplication::MakeDataFilePathForResContainer(
	OSType				inType,
	FSSpec				*inResContainerSpec)
{
	CNewHandleStream	stream;
	SInt16				version=0;
	
	stream << version;
	stream << inType;
	
	switch (inType)
	{
		case kDataType_AppResFork:		// data is embedded in app resource
			break;
		case kDataType_DataBundle:
			stream << inResContainerSpec->name;
			break;
		default:
			Throw_(kTNTErr_UnsupportedDataFormat);
			break;
	}
	
	return stream.DetachDataHandle();
}


// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• GetResourceContainerForDataFilePath					/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// creates a CResourceContainer* for the specified datafilepath - this will either be a container to get resources from the
// tb app itself, or to get it from some data file in the same dir.
CResourceContainer *CApplication::GetResourceContainerForDataFilePath(
	Handle			inFilePathHandle)
{
	CNewHandleStream	stream(inFilePathHandle,false);
	SInt16				version;
	OSType				dataType;
	CResourceContainer	*container=NULL;
	
	stream >> version;
	if (version!=0)
	{
		Throw_(kTNTErr_UnsupportedDataFormat);
	}
	stream >> dataType;
	
	switch (dataType)
	{
		case kDataType_AppResFork:		// data is embedded in app resource
			{
				ECHO("Data stored in app res fork\n");
				FSSpec		spec;
				ThrowIfOSErr_(::GetAppSpec(&spec));
				container=mMacResFileCreator->OpenFile(&spec,true);
			}
			break;
		case kDataType_DataBundle:
			{
				ECHO("Data stored in separate bundle\n");

				Str255		dataName;
				FSSpec		appSpec;
				FSSpec		spec;
				stream >> dataName;
				ThrowIfOSErr_(::GetAppSpec(&appSpec));

				ThrowIfOSErr_(::FSMakeFSSpec(appSpec.vRefNum,appSpec.parID,dataName,&spec));
				container=mBundleResFileCreator->OpenFile(&spec,true);
			}
			break;
		default:
			Throw_(kTNTErr_UnsupportedDataFormat);
			break;
	}
	
	ThrowIfMemFull_(container);

	return container;	
}


// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• StartUp												/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Called when we're run with no documents
void CApplication::StartUp()
{
	Handle			dataFile=::BetterGetResource(-1,kTNTBasic_DataFilePathResType,kTNTBasic_DataFilePathResId);
	
	if (dataFile)		// if no data file - this isn't a built game
	{
		mAppletMode=true;
		
		ECHO("Applet mode...");
				
		CResourceContainer	*rconc=NULL;
		
		{
			UHandleReleaser		rel(dataFile);
			rconc=GetResourceContainerForDataFilePath(dataFile);
			dataFile=NULL;
		}
		
		CProgramInfo		*progInfo=NULL;				
		StDeleter<CResourceContainer>	delR(rconc);
		StDeleter<CProgramInfo>			delP;
		Handle				codeHandle=NULL;
		
		// get the code - we own this handle
		codeHandle=rconc->CopyResourceContent(kTNTBasic_EncryptedCodeResType,kTNTBasic_EncryptedCodeResId);
		UHandleReleaser		delC(codeHandle);
				
		Try_
		{
			TResName		progName;

			rconc->GetNameForResId(kTNTBasic_EncryptedCodeResType,kTNTBasic_EncryptedCodeResId,progName);

			progInfo=new CProgramInfo(rconc,progName);
			ThrowIfMemFull_(progInfo);
			delP.Adopt(progInfo);
			
			UEncryption::SimpleEncryptDecrypt(codeHandle,kCodeEncryptionKey);
		}
		Catch_(err)
		{
			std::cout << "Error: " << err << "\n";
			return;
		}

		if (!ParseHandle(delC.Release(),delP.Release(),delR.Release(),true,0))
			Throw_(kTBErr_SyntaxError);
			
		// all now owned by program
		codeHandle=NULL;
		progInfo=NULL;
		rconc=NULL;

		// Now run it
		Try_
		{
			LCommander::SetUpdateCommandStatus(true);	// enabled commands for program execution

			FSSpec		spec;
	
			ThrowIfOSErr_(::GetAppSpec(&spec));
			
			mCurrentProgram->BeginExecution();
			mProgramPaused=false;
		}
		Catch_(err)
		{
			EndProgram();
			
			std::cout << "Runtime exception caught : " << err << "\n";
			throw;
		}	
	}
	else
	{
		ECHO("Server mode...");
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ShowAboutBox
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// This about box needs serious help...
void CApplication::ShowAboutBox()
{
	UPortSaver	safe;
	
	if (!DoRaaAboutBox())
	{
		::ParamText(verShortTextP,"\p","\p","\p");
		UModalAlerts::NoteAlert(128);
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• FindCommandStatus
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
void CApplication::FindCommandStatus(
	CommandT			inCommand,
	Boolean&			outEnabled,
	Boolean&			outUsesMark,
	UInt16&				outMark,
	Str255				outName)
{	
	switch (inCommand)
	{
		case cmd_TBRegister:
			outEnabled=false;
			break;
	
		case cmd_TBToggleFPS:
			outUsesMark=true;		// need to set this to true always if your menu item may or maynot have a mark
			outEnabled=true;
			if (mShowFPS)
				outMark=checkMark;
			else
				outMark=0;				
			break;
	
		case cmd_TBPauseResume:
			outMark=0;
			outUsesMark=true;
			if (mCurrentProgram)
			{
				outEnabled=true;
				if (mProgramPaused)
					outMark=checkMark;
			}
			break;
	
		case cmd_TBRestart:
		case cmd_TBTerminate:
			if (mCurrentProgram)
				outEnabled=true;
			break;
	
		case cmd_TBConfigureControls:
			if (mCurrentProgram)
			{
				if (mCurrentProgram->HasValidInput())
					outEnabled=true;
			}
			break;
			
		case cmd_Open:
			outEnabled=true;
			break;

		case cmd_TBRun:
			// opens a TNT basic file
			outEnabled=(mCurrentProgram==0);
			break;
			
		case cmd_TBTakeScreenShot:
			// enabled if the program has a graphics context
			outEnabled=(mCurrentProgram!=0) && (mCurrentProgram->GetGraphicsContext()!=0);
			break;
	
		default:
			inheritedApp::FindCommandStatus(inCommand,outEnabled,outUsesMark,outMark,outName);
			break;
	}
}


// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ObeyCommand											/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Implements application level commands such as file open, creation etc.
Boolean CApplication::ObeyCommand(
	CommandT			inCommand,
	void*				ioParam)
{
	bool				handled=true;
	SInt16				item;
	
	// see if it's a powerplant synthetic command from the help menu
	if (LCommander::IsHelpMenuCommand(inCommand,item))
	{
		UHelp::HandleHelpHit(item);
		return true;
	}

	switch (inCommand)
	{
		case cmd_TBRegister:
			// nothing - it's all free!
			break;
	
		case cmd_TBRestart:
			RestartProgram();
			break;
	
		case cmd_TBTerminate:
			EndProgram();
			break;
			
		case cmd_TBTakeScreenShot:
			TakeScreenShot(false);
			break;
		
		case cmd_TBToggleFPS:
			mShowFPS=!mShowFPS;
			LCommander::SetUpdateCommandStatus(true);
			if (mCurrentProgram && mCurrentProgram->GetGraphicsContext())
				mCurrentProgram->GetGraphicsContext()->ShowFPS(mShowFPS);
			if (!mAppletMode)		// IMPORTANT: don't want to save/load prefs if in applet mode
			{
				CFPrefsSetBool(kShowFPSPrefsStr,mShowFPS);
				if (!CFPrefsFlush())
					ECHO("Error saving show fps preference\n");
			}
			break;
			
		case cmd_TBPauseResume:
			if (mCurrentProgram)
			{
				LCommander::SetUpdateCommandStatus(true);
				if (mProgramPaused)
					ResumeProgram();
				else
					PauseProgram();
			}
			break;

		case cmd_TBConfigureControls:
			if (mCurrentProgram)
			{
				if (mCurrentProgram->HasValidInput())
					mCurrentProgram->ConfigureControls();
			}
			break;

		case cmd_Open:
			{
				bool										askOK;
				FSSpec										spec;
				
				{
					UNavServicesDialogs::LFileChooser		chooser;

					// filter for the types we can open
					OSType									types[]={'rsrc','RBun'};
					LFileTypeList							fileTypes(fileTypes_All);			// now that we can open resources from the data fork, there's no sensible way of checking file types, so allow everything
					NavDialogOptionFlags					flags=(kNavDefaultNavDlogOptions|kNavSupportPackages|kNavNoTypePopup)&~(kNavAllowPreviews);
					NavDialogCreationOptions				*options = chooser.GetDialogOptions();

					flags &= ~kNavAllowMultipleFiles;		// Can't select multiple files
					options->optionFlags = flags;
		
					askOK = chooser.AskOpenFile(fileTypes);
		
					if (askOK)
					{
						chooser.GetFileSpec(1,spec);
					}
				}
				
				if (askOK)
				{
					Try_
					{
						UEditServer::OpenForEdit(&spec);
					}
					Catch_(err)
					{
						// error reported
						//std::cout << "Error opening file : " << err << endl;
					}
				}
			}
			break;

		case cmd_TBRun:
			// opens a TNT basic file
			{
				FSSpec										spec;
				bool										askOK;
				
				{
					UNavServicesDialogs::LFileChooser		chooser;

					// filter for the types we can open
					OSType									types[]={'rsrc','RBun'};
					LFileTypeList							fileTypes(fileTypes_All);			// now that we can open resources from the data fork, there's no sensible way of checking file types, so allow everything
					NavDialogOptionFlags					flags=(kNavDefaultNavDlogOptions|kNavSupportPackages|kNavNoTypePopup)&~(kNavAllowPreviews);
					NavDialogCreationOptions				*options = chooser.GetDialogOptions();

					flags &= ~kNavAllowMultipleFiles;		// Can't select multiple files
					options->optionFlags = flags;
					options->windowTitle=CFSTR("Run");
		
					askOK = chooser.AskOpenFile(fileTypes);
		
					if (askOK)
						chooser.GetFileSpec(1,spec);
				}
				
				if (askOK)
				{
					Try_
					{
						OpenDocument(&spec,0L,true,0,true);
					}
					Catch_(err)
					{
						// error reported
						//std::cout << "Error opening file : " << err << endl;
					}
				}
			}
			break;

		default:
			handled=inheritedApp::ObeyCommand(inCommand,ioParam);
			break;
	}
	
	return handled;
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• StoreCompileErrorsInAEDesc
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Places a list of the compile errors in the ae desc passed. Will allocate the desc as an aedesc list if it's of typenull
// Adds an error string field for each error which is displayable text for the user
void CApplication::StoreCompileErrorsInAEDesc(
	AEDesc			&outDesc)
{
	Try_
	{
		AssertThrow_(mCurrentProgram);

		// make it into a list if it isn't one already
		if (outDesc.descriptorType!=typeAEList)
			ThrowIfOSErr_(::AECreateList(0,0,false,&outDesc));

		CListIndexerT<CError>		errIndexer(&mCurrentProgram->GetErrors());
		Str255						errStr;
		SInt32						index=1;
				
		while (CError *err=errIndexer.GetNextData())
		{
			StAEDescriptor	errStrDesc(err->mException.mContextRecord,keyErrorString,typeText);
		
			if (errStrDesc.IsNull())		// no err str available, generate one and add it to the ae before we copy it
			{
				err->mException.CreateErrorString(errStr);
				
				errStrDesc.Assign(errStr);
				
				ThrowIfOSErr_(::AEPutKeyDesc(err->mException.mContextRecord,keyErrorString,errStrDesc));
			}
			
			ThrowIfOSErr_(::AEPutDesc(&outDesc,index++,err->mException.mContextRecord));
		}		
	}
	Catch_(err)
	{
		// doh...
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• DumpCompileErrorsToStream
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Call this when you know there has been some compile errors and you wish to output them to a stream. If the errors
// list is empty then a message indicating the error recording has failed is output instead.
void CApplication::DumpCompileErrorsToStream(
	Handle			inCodeHandle,
	std::ostream	&outStream)
{
	if (!inCodeHandle || !mCurrentProgram)
		return;

	CListIndexerT<CError>		errIndexer(&mCurrentProgram->GetErrors());
	SInt32						index=1;
	
	outStream << (SInt32)mCurrentProgram->GetErrors().CountElements() << " compile error(s) detected\n";
	
	while (CError *err=errIndexer.GetNextData())
	{
		StAEDescriptor	lineNumDesc(err->mException.mContextRecord,keyTBSourceLineNum,typeSInt32);
		StAEDescriptor	startDesc(err->mException.mContextRecord,keyTBSourceRangeStart,typeSInt32);
		StAEDescriptor	endDesc(err->mException.mContextRecord,keyTBSourceRangeEnd,typeSInt32);
		SInt32			lineNum=0;
		SInt32			startRange=0;
		SInt32			endRange=0;
		
		if (lineNumDesc.IsNotNull())
			lineNum=**(SInt32**)lineNumDesc.mDesc.dataHandle;
		if (startDesc.IsNotNull())
			startRange=**(SInt32**)startDesc.mDesc.dataHandle;
		if (endDesc.IsNotNull())
			endRange=**(SInt32**)endDesc.mDesc.dataHandle;
		
		outStream << "Compile time error #" << index++ << "\n    Line# : ";
		if (lineNum)
			outStream << lineNum;
		else
			outStream << "????";
		outStream << "\n    Error : " <<  err->mException << "\n    Code  : " ;
		
		if (lineNum)
		{
			// print the offending line with the text underlined
			// find line "lineNum"
			SInt32			lineStartOffset=0,max=::GetHandleSize(inCodeHandle),lineIndex=1,lineEndOffset,index;

			for (index=0; index<max; index++)
			{
				if (lineIndex==lineNum)
					break;
					
				if ((*inCodeHandle)[index]=='\r')	// found an endline?
				{
					lineStartOffset=index+1;
					lineIndex++;
				}
			}
			
			// skip white space at beginning of line
			for ( ; lineStartOffset<max; lineStartOffset++)
			{
				if ((*inCodeHandle)[lineStartOffset]=='\r')	// whoops, we appear to have skipped all the white space until a cr
					break;
				if (((*inCodeHandle)[lineStartOffset]!=' ') && ((*inCodeHandle)[lineStartOffset]!='\t'))
					break; // found a non white space char
			}
			
			// range from lineStartOffset to max or the next endline is the line length, print it out
			for (lineEndOffset=lineStartOffset; lineEndOffset<max; lineEndOffset++)
			{
				if ((*inCodeHandle)[lineEndOffset]=='\r')
					break;
			}
			
			for (index=lineStartOffset; index<lineEndOffset; index++)
				outStream << (*inCodeHandle)[index];
			outStream << "\n";
			
			// now underline the code
			outStream << "            ";
			
			// print spaces for a bit
			for (index=0; index<(startRange-lineStartOffset); index++)
				outStream << " ";
			// print chevrons for a bit
			for (index=0; index<(endRange-startRange); index++)
				outStream << "^";
			outStream << "\n";
		}
		else
		{
			SignalPStr_("\pError reporting error\rError source location missing");
			outStream << "????\n";
		}
	}

	if (mCurrentProgram->GetErrors().IsEmpty())
	{
		outStream << "*** Program contains parse errors which are not being correctly processed.\nCannot run program.\n";
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ParseHandle
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Parses the passed handle, releasing it when done. Result is stored in the mCurrentProgram parameter. Errors are reported,
// if inOutputErrors is true. Returns true for good parse, false for errors. Errors are stored in outErrors record if it's
// non null.
// Returns true for OK parse, false for contained errors.
// The pass CResourceContainer and CProgramInfo are also adoped
bool CApplication::ParseHandle(
	Handle					inCodeHandle,
	CProgramInfo			*inProgInfo,			// can be NULL if no resource file is being used
	CResourceContainer		*inResContainer,		// can be NULL if no resource file is being used
	bool					inOutputErrors,
	AEDesc					*outErrors)
{
	StHandleReleaser				rel(inCodeHandle);	// works for either resource handles or mem handles
	StDeleter<CProgramInfo>			delP(inProgInfo);
	StDeleter<CResourceContainer>	delR(inResContainer);

	if (mCurrentProgram)
	{
		SignalString_("CApplication::ParseHandle() - A program is already loaded");
		return false;
	}

	UInt32					startParse,endParse;
	bool					hasErrors;

	startParse=CGraphicsContext::sMsTimer.GetTime();
	Try_
	{
		mCurrentProgram=CProgram::ParseAndLink(inCodeHandle,hasErrors,inProgInfo->GetProgName());
		
		mCurrentProgram->SetProgramInfo(delP.Release());
		mCurrentProgram->SetProgramResContainer(delR.Release());
		
		if (hasErrors)
		{
			if (inOutputErrors)
			{
				UProcess::MakeMeFront();
				DumpCompileErrorsToStream(inCodeHandle,std::cout);
			}
			if (outErrors)
				StoreCompileErrorsInAEDesc(*outErrors);
			
			// delete the fecked up program
			delete mCurrentProgram;
			mCurrentProgram=NULL;
		}
	}
	Catch_(err)
	{
		if (ErrCode_(err)==-1)	// parse error
		{
			std::cout << "*** UNHANDLED : Parse error at or before line " << gLineno << "\n";
		}
		else
		{
			if (dynamic_cast<const CTBLinkerException*>(&err)) // link err
				std::cout << "*** UNHANDLED : Linker error\n    Error : " << err << "\n";
			else	// compile err
				std::cout << "*** UNHANDLED : Compile time error\n    Line# : At or before " << gLineno << "\n    Error : " <<  err << "\n";
		}
		hasErrors=true;
		SignalString_("Unhandled errors in CApplication::ParseHandle()");
	}
	endParse=CGraphicsContext::sMsTimer.GetTime();
	
	return !hasErrors;		// returns true for OK, false for errors
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• OpenDocument											/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Opens a basic file and runs it.
// Optionally reports compile errors and will return them in outCompErrors if it's non null.
// Returns true/false depending on whether it compiled sucessfully or not, if a run time error occurs then an excpetion
// is thrown instead.
bool CApplication::OpenDocument(
	FSSpec				*inFile,
	OSType				inReturnToApp,
	bool				inReportErrors,
	AEDesc				*outErrors,
	bool				inRunProgram)
{
	// Can currently only handle running one program at once
	if (mCurrentProgram)
		UTBException::ThrowProgramAlreadyRunning();

	Handle							codeHandle=0;
	OSErr							err=noErr;
	StDeleter<CProgramInfo>			delP;
	StDeleter<CResourceContainer>	delR;


	{
		// Open the program resource container and get the program info
		CResourceContainerCreator		*rcc=CResourceContainerCreator::GetCreatorForFile(inFile);
		TResName						codeName;		
		if (!rcc)
		{
			Throw_(kTBErr_UnrecognisedResFileFormat);
		}
		delR.Adopt(rcc->OpenFile(inFile,true));
		delR->GetNameForResId(kTNTBasic_CodeResType,kTNTBasic_CodeResId,codeName);
		delP.Adopt(new CProgramInfo(delR.Get(),codeName));
		ThrowIfMemFull_(delP.Get());

		StTNTResource		code(delR.Get(),kTNTBasic_CodeResType,kTNTBasic_CodeResId);
		codeHandle=code->GetReadOnlyDataHandle();
		ThrowIfOSErr_(::HandToHand(&codeHandle));		// parser currently releases the handle so we need to dupe it
	}
		
	Try_
	{
		StExpectException	expect(kTBErr_SyntaxError);	// suppress reporting of routine exceptions
		
		if (!ParseHandle(codeHandle,delP.Release(),delR.Release(),inReportErrors,outErrors))
			Throw_(kTBErr_SyntaxError);

	}
	Catch_(err)
	{
		if (ErrCode_(err)==-1)
		{
			if (inReportErrors==false)	// if the errors weren't being stuck up on screen and we got some errors, go back to the prev app
			{
				mReturnToApp=inReturnToApp;
				ReturnToSender();
			}
		
			return false;	// bad parse, return false for that
		}
		
		throw;
	}
	
	mReturnToApp=inReturnToApp;		// assign return to after parsing is complete then we don't switch back on parse error
	
	if (!inRunProgram)
	{
		EndProgram();
		return true;
	}

	// The makemefront command can issue us an idle event which will attempt to run the program
	// the program isn't ready yet so set program paused to true so it won't run
	mProgramPaused=true;	

	UProcess::MakeMeFront();		// Bring us to the front in case we weren't already there
	
	// Set the home directory to this
	FSSpec		homeDirectory;
	
	homeDirectory=*inFile;
	LString::CopyPStr("\p",homeDirectory.name);
	
	UFileManager::SetGameDirectory(homeDirectory);
		
	// Now run it
	Try_
	{
		LCommander::SetUpdateCommandStatus(true);	// enabled commands for program execution

		mCurrentProgram->BeginExecution();
		mProgramPaused=false;
	}
	Catch_(err)
	{
		EndProgram();
		
		std::cout << "Runtime exception caught : " << err << "\n";
		throw;
	}
	
	return true;	
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• RestartProgram										/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Restarts the currently running program
void CApplication::RestartProgram()
{
	if (mCurrentProgram)
	{
		LCommander::SetUpdateCommandStatus(true);
		mProgramPaused=false;
		mCurrentProgram->EndExecution();
		mCurrentProgram->BeginExecution();
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ReturnToSender
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Brings the mReturnToApp process to the front
void CApplication::ReturnToSender()
{
	if (mReturnToApp)
	{
		try
		{
			// if the console window is open then don't bother switching app as they probably want to read it
			if (mOutputWindow.GetWindowNoOpen()==0)
			{
				ProcessSerialNumber		num=UProcess::GetPSN(mReturnToApp);
				::SetFrontProcess(&num);
			}

			mReturnToApp=0;
		}
		catch(...)
		{
			mReturnToApp=0;
		}
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• EndProgram
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Terminates the currently running program and cleans up after it
void CApplication::EndProgram()
{
	if (mCurrentProgram)
	{
#if __profile__
		Str255		str;
		CopyPString(mCurrentProgram->GetProgramName(),str);
		::ProfilerDump(str);
		str[str[0]+1]=0;
		ECHO("Profile dumped to file \'" << (char*)str[1] << "\'...");
		::ProfilerClear();
#endif

		LCommander::SetUpdateCommandStatus(true);

		delete mCurrentProgram;
		mCurrentProgram=0;

		ReturnToSender();		

		if (mAppletMode)
		{
			// The executable quits after terminating program zero unless there's some errors on the screen
			if (mOutputWindow.GetWindowNoOpen()==0)
			{
				// we must self terminate....
				Try_
				{
					SendAEQuit();
				}
				catch(...)
				{
				}
			}
		}	
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• PauseProgram												/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Pauses the currently running program
void CApplication::PauseProgram()
{
	ClearKeyboard();			// don't want any keys to look like they've been held down if using carbon events we could lose the keyup event

	if (mCurrentProgram)
	{
		if (mCurrentProgram->CanBreak())
		{
			mProgramPaused=true;
			mCurrentProgram->Pause();
			LCommander::SetUpdateCommandStatus(true);
			::MacDrawMenuBar();
		}
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ResumeProgram												/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Resumes the currently running program
void CApplication::ResumeProgram()
{
	ClearKeyboard();			// don't want any keys to look like they've been held down if using carbon events we could lose the keyup event

	if (USE_RNE_FOR_KEYBOARD_INPUT)
	{
		UInt32	keys=::GetCurrentKeyModifiers();	// need to ensure the caps lock comes through and other modifiers that could be held down come through
		KeyMapModsChanged(keys);					// pretend they've all just been pressed. this will result in same behaviour as mac os 9 one (new keys will generate key presses)
	}

	if (mCurrentProgram)
	{
		if (mCurrentProgram->CanBreak())
		{
			LCommander::SetUpdateCommandStatus(true);
			mCurrentProgram->Resume();
			mProgramPaused=false;
		}
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• DoTask
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
void CApplication::DoTask()
{
	if (!mCurrentProgram)
		return;

#define	CHECK_EVENTS_FREQUENCY	20	// ms - changed to 20 to cater for john's 'cat like' reflexes...

	Try_
	{		
		UInt32		startTime=CGraphicsContext::sMsTimer.GetTime();
		UInt32		nextTime=startTime+CHECK_EVENTS_FREQUENCY;
		bool		breaked=false,paused=false,running=true,screenShotHeld=false;

		while (running)
		{
			if (mCurrentProgram->PopForceEventProcessFlag() || CGraphicsContext::sMsTimer.GetTime()>=nextTime)
			{
				EventRef		event;

				if (!USE_RNE_FOR_KEYBOARD_INPUT)
					UpdateKeyboardGetKeys();

				while (running && mCurrentProgram->TBReceiveNextEvent(EventListWithCount(kStdEventsList),kEventDurationNoWait,true,&event)==noErr)
				{
					UInt32		eventKind=0,eventClass=0;
					
					switch (eventClass=::GetEventClass(event))
					{												
						case kEventClassMouse:
							switch (eventKind=::GetEventKind(event))
							{
								case kEventMouseDown:
									HandleRunningProgramMouseDown(event);
									break;
									
								case kEventMouseMoved:
									UCursorManager::MouseMoved();
									break;
							}
							break;

						case kEventClassKeyboard:
							switch (eventKind=::GetEventKind(event))
							{
								case kEventRawKeyModifiersChanged:
									{
										UInt32		modifiers=0;
										kEventParamKeyModifiers;

										OSStatus stat=::GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,0,sizeof(UInt32),0,&modifiers);
										if (stat)
											ECHO("GetEventParameter failed to get keymods for rawkeymodschanged event - stat : " << stat << "\n");
										else
										{
											if (USE_RNE_FOR_KEYBOARD_INPUT)
												KeyMapModsChanged(modifiers);
										}
									}
									break;
								
								case kEventRawKeyDown:
								case kEventRawKeyRepeat:
								case kEventRawKeyUp:
									{
										UInt32	modifiers=0,scanCode=0;
										char	charCode;
										char	progress=0;
										
										// On MacOS 10.1.3, ReceiveNextEvent stops returning the char code for key presses
										// after the first one. This breaks our cmd-. detection and escape detection which
										// uses the char code.
										// So use the scan code if the char code is coming back null, this isn't as good as
										// the same char doesn't have the same keycode on different keyboard layouts, but
										// what else can we do?
										// NB: This bug would break any keyboard reading commands under OS X too, the bug
										// is reset on exit from RunApplicationEventLoop. Doesn't make any difference if
										// carbon timer returns each time.
										
										Try_
										{
											ThrowIfOSErr_(::GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,0,sizeof(UInt32),0,&modifiers));
											progress++;
											ThrowIfOSErr_(::GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,0,sizeof(char),0,&charCode));
											progress++;
											ThrowIfOSErr_(::GetEventParameter(event,kEventParamKeyCode,typeUInt32,0,sizeof(UInt32),0,&scanCode));
										}
										Catch_(err)
										{
											ECHO("The " << ((SInt32)progress) << "GetEventParameter failed, error " << ErrCode_(err) << "\n");
										}
										
										if (USE_RNE_FOR_KEYBOARD_INPUT)
										{
											// process keyboard input
											CKeyQueue::STBRawKeycode	raw;
											raw.b.tbMods=0;
											raw.b.macMods=(modifiers>>8)&0xFF;
											raw.b.scancode=scanCode;
											raw.b.charcode=charCode;
											
											if (!raw.b.charcode)		// try and work around crap OS X 10.1.3 bug by generating the char code
												raw.b.charcode=CKeyQueue::ScancodeAndModsToCharcode(raw.b.scancode,raw.b.macMods);

											switch (eventKind)
											{
												case kEventRawKeyDown:
													KeyMapPress(raw.rawcode,false);
													break;
												case kEventRawKeyUp:
													KeyMapRelease(raw.rawcode);
													break;
												case kEventRawKeyRepeat:
													KeyMapPress(raw.rawcode,true);
													break;
												default:
													Debug_("unknown key event");
													break;
											}
										}

										if (eventKind==kEventRawKeyDown)
										{
											// process menu short cuts if the program wants them
											if (mCurrentProgram->ShouldProcessMenuShortCuts())
											{
												EventRecord		oldEvent;
												
												if (::ConvertEventRefToEventRecord(event,&oldEvent))
												{
													EventKeyDown(oldEvent);
												}
												else
												{
													ECHO("Failed to make raw key down into old event");
												}
												if (!mCurrentProgram || mState==programState_Quitting)
												{
													breaked=true;
													running=false;
													continue;
												}
											}
																				
	//										ECHO("Key pressed, char code " << ((SInt32)charCode) << " key code " << scanCode << " modifiers " << modifiers);
											if ((scanCode==0x67) && (modifiers&(1<<cmdKeyBit)))		// cmd-F11 to you and me
												TakeScreenShot(true);
											else
											{
												if (mCurrentProgram->CanBreak())
												{									
													switch (charCode)
													{
														case kEscChar:		// escape hit
															paused=true;
															ECHO("Esc received, pausing...");
															break;
															
														case '.':
															if (modifiers&(1<<cmdKeyBit))
															{
																breaked=true;
																ECHO("Runtime cmd-. received, breaking...");
															}
															break;
															
														case '\0':			// see 10.1.3 bug note above
															// use key code, this is bad as key codes for characters vary depending on keyboard layout from
															// country to country
															switch (scanCode)
															{
																case 47:		// . on US/UK keyboard
																	if (modifiers&(1<<cmdKeyBit))
																	{
																		breaked=true;
																		ECHO("Runtime cmd-. received through Mac OS X 10.1.3 workaround, breaking...");
																	}
																	break;
																
																case kEscSC:
																	ECHO("Esc received through Mac OS X 10.1.3 workaround, pausing...");
																	paused=true;
																	break;															
															}
													}
												}
											}
										}
									}
									break;
							}
							break;
							
						case kEventClassWindow:
							switch (eventKind=GetEventKind(event))
							{
								case kEventWindowUpdate:
									{
										WindowRef		w;
										EventRecord		r;
										
										::GetEventParameter(event,kEventParamDirectObject,typeWindowRef,0,sizeof(w),0,&w);
										::ConvertEventRefToEventRecord(event,&r);
										EventUpdate(r);
									}
									break;
									
								case kEventWindowDeactivated:
									// pause program
									if (mCurrentProgram->ShouldPauseProgramWhenFocusLost())
									{
										WindowRef		w;
										
										::GetEventParameter(event,kEventParamDirectObject,typeWindowRef,0,sizeof(w),0,&w);
										
										CGraphicsContext	*gc=mCurrentProgram->GetGraphicsContext();
										if (gc && gc->GetGameWindow()==w)
										{
											ECHO("game window deactivated - pausing...");
											paused=true;
										}
									}
									break;
									
#if 0 // this doesn't appear to be received by the main event loop
								case kEventWindowCursorChange:
									{
									/*
									    Parameters for window cursor change events:
									    
									    kEventWindowCursorChange
									        -->     kEventParamDirectObject     typeWindowRef
									        -->     kEventParamMouseLocation    typeQDPoint
									        -->     kEventParamKeyModifiers     typeUInt32
									*/
										CGraphicsContext	*g=mCurrentProgram->GetGraphicsContext();
										if (g)
										{
											WindowRef		w=NULL;
											::GetEventParameter(event,kEventParamDirectObject,typeWindowRef,0,sizeof(w),0,&w);
											if (w)//==mWindow)
											{
												ECHO("in window");
											}
											else
											{
												ECHO("not in window");
											}
										}
									}
									break;
#endif
							}
							break;
							
						case kEventClassApplication:
							switch (eventKind=GetEventKind(event))
							{
								case kEventAppDeactivated:
									// pause program
									if (mCurrentProgram->ShouldPauseProgramWhenFocusLost())
									{
										ECHO("game deactivated - pausing...");
										paused=true;
									}
									break;
							}
							break;
							
						case kEventClassAppleEvent:
							switch (eventKind=GetEventKind(event))
							{
								case kEventAppleEvent:									
									{
										EventRecord	oldEvent;
										
										if (::ConvertEventRefToEventRecord(event,&oldEvent))
										{
											::AEProcessAppleEvent(&oldEvent);
										}
									}
									break;
							}							
							break;
					}
					
					ReleaseEvent(event);

					if (!mCurrentProgram || mState==programState_Quitting)
					{
						breaked=true;
						running=false;
						continue;
					}
				}
				
				nextTime=CGraphicsContext::sMsTimer.GetTime()+CHECK_EVENTS_FREQUENCY;

				if (paused || breaked)
				{
					running=false;					
					continue;	// skip the Step() command below
				}
				
#if RUN_PROGRAM_AS_CARBON_TIMER
				SetNextFireTime(0.0f);
				return;
#endif
			}

			if (!mCurrentProgram->Step())
				running=false;		
		}

		if (paused)
			PauseProgram();
		else
			EndProgram();
	}
	Catch_(err)
	{
		HandleRuntimeException(err);
	}

	ECHO("Quitting event loop " << AbsTimeStr());
	::QuitEventLoop(GetCurrentEventLoop());		// allows us to exit the RunCurrentEventLoop call
	ECHO("Quitting event loop done " << AbsTimeStr());
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• HandleRunningProgramMouseDown
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// does not adopt the event passed
bool CApplication::HandleRunningProgramMouseDown(
	EventRef		inRef)
{
	bool				handled=false;

	if (mCurrentProgram)
	{
		CGraphicsContext	*gc=mCurrentProgram->GetGraphicsContext();
		
		if (gc && gc->UsingWindowMode())
		{
			handled=true;
			
			Point			point;

			::GetEventParameter(inRef,kEventParamMouseLocation,typeQDPoint,0,sizeof(point),0,&point);

			WindowRef		window=NULL;		
			WindowPartCode	part=::MacFindWindow(point,&window);
			
			if (window)
			{
				// dispatch to windows event handler
				EventTargetRef		target=::GetWindowEventTarget(window);

				OSStatus			err=::SendEventToEventTarget(inRef,target);

				ECHO("mouse down sent to window event target : result " << err);

				// the std drag handler will leave the mouse up in the queue, ready to be consumed by us
				// but we don't pick them off by default so flush it now lest it linger and cause problems for
				// subsequent window drags by making them think the mouse has been released as soon as its clicked				
				{
					EventTypeSpec	eventsList[]={ { kEventClassMouse, kEventMouseUp } };
					::FlushEventsMatchingListFromQueue(GetCurrentEventQueue(),1,eventsList);
				}
	  
	  			handled=(err==noErr);		// will return eventNotHandledErr
			}
			else
			{
				switch (part)
				{
					case inMenuBar:
						{
							ECHO("menu click");
							EventRecord		er;
							::ConvertEventRefToEventRecord(inRef,&er);
							ClickMenuBar(er);
							handled=true;
						}
						break;

					default:
						// not handled
						ECHO("mouse down not handled");
						break;
				}
			}
		}
	}
	
	return handled;
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• RunProgram												/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Runs the currently loaded program until it terminates, is forced terminated or until the user pauses with the esc key
void CApplication::RunProgram()
{
	static bool		canEnter=true;
	
	if (canEnter==false)		// program already running - must have got here by reentry from one of the commands
		return;					// in the program using our null event handler

	StValueChanger<bool>	dontReenter(canEnter,false);

	if (USE_CARBON_TIMERS && (mCurrentProgram && (!mProgramPaused)))
	{
		// install the timer
		Try_
		{
			// We only need the timer to fire once, then it's locked in.
			// If pass time other than 1 shot you can get a delay when the event loop quits,
			// presumably as the os trys to track how many times your timer should have fired
			// but hasn't due to it never returning from the first tick.
			#define kEventDuration1Shot		0.0
			Install(GetCurrentEventLoop(),kEventDurationNoWait,kEventDuration1Shot);
		}
		Catch_(err)
		{
			ECHO("Install of carbon timer failed: " << ErrCode_(err));
			throw;
		}
		
		OSErr	err;
		
#if RUN_PROGRAM_AS_CARBON_TIMER
		///////////////////////// test code
		if (0)
		{
			Rect			bounds={50,50,150,150};
			WindowRef		w=::NewCWindow(0L,&bounds,"\ptest",true,kWindowDocumentProc,(WindowPtr)-1,true,0L);
			//ThrowIfOSErr_(::InstallStandardEventHandler(::GetApplicationEventTarget()));		// according to apple docs, this doesn't do anything for app targets
			ThrowIfOSErr_(::InstallStandardEventHandler(::GetWindowEventTarget(w)));
		}
//		::RunApplicationEventLoop();		// this causes the std event handlers to be called, but it seems we can only call this once per run, meaning we
											// can only run the first program
//		err=noErr;
		err=RunCurrentEventLoop(kEventDurationForever);
#else
		// call run event loop to get the lad up and running
		// it doesn't matter how long we state the event loop will run for, because it only needs to run long enough to invoke
		// our first timer once. The natuaral value to pass is simply kEventDurationForver, ie run until the program ends, but
		// on mac os 9, this causes the TNT Basic to delay exiting RunCurrentEventLoop for a good 10 seconds. If you use a shorter
		// duration it works out much better and doesn't have any affect on OS X, as long as the timer fires you're ok.

		err=RunCurrentEventLoop(kEventDurationMillisecond);
#endif
		
		if (err==eventLoopQuitErr || err==eventLoopTimedOutErr)
			err=noErr;
		
		if (err)
			ECHO("RunCurrentEventLoop error: " << err);
		else
			ECHO("Event loop completed at " << AbsTimeStr());

		Remove();		// remove timer
	}
	else if (mCurrentProgram && (!mProgramPaused))
	{
		bool				breaked=false,paused=false,running=true,screenShotHeld=false;
		UInt32				nextKeyCheck=0;			// use this to stop us checking the key too often

		while (running)
		{
			if (!mCurrentProgram->Step())
				running=false;
		
			if (CGraphicsContext::sMsTimer.GetTime()>nextKeyCheck)
			{
				UpdateKeyboardGetKeys();

				if (mCurrentProgram->CanBreak())
				{
					if (FastIsPressed(mKeyMap,kCommandKeySC) && FastIsPressed(mKeyMap,0x2f))	// cmd-.
						breaked=true;
					if (FastIsPressed(mKeyMap,kEscSC))
						paused=true;
					if (FastIsPressed(mKeyMap,0x67) && FastIsPressed(mKeyMap,kCommandKeySC))			// screenshot on cmd-F11
					{
						if (!screenShotHeld)
						{
							screenShotHeld=true;
							TakeScreenShot(true);
						}
					}
					else
						screenShotHeld=false;
										
					if (paused || breaked)
						running=false;
				}
				
				nextKeyCheck=CGraphicsContext::sMsTimer.GetTime()+CHECK_EVENTS_FREQUENCY;
			}
			
			UTNTMusicManager::TickEngine();
		}
		
		if (paused)
		{
			PauseProgram();
			while (IsPressed(kEscSC))	// wait for esc key up
				;
		}
		else
			EndProgram();

		::FlushEvents(kUserInputEventMask,0L);
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• UseIdleTime
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// When the idle time is hit the current program is ran until the user pauses it or it finishes
void CApplication::UseIdleTime(
	const EventRecord&	inMacEvent)
{
	inheritedApp::UseIdleTime(inMacEvent);

	UEditServer::Tick();
	
	Try_
	{
		if ((mAppResumed) && (!mProgramPaused))		// wait until we've processed the resume event before running the program
			RunProgram();
	}
	Catch_(err)
	{
		HandleRuntimeException(err);
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• HandleRuntimeException
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Reports runtime exceptions to the console and terminates the program
void CApplication::HandleRuntimeException(
	const LException &inErr)
{
	// Report the runtime exception
	const CTBRuntimeException		*rte=dynamic_cast<const CTBRuntimeException*>(&inErr);
	
	if (rte)
	{
		std::cout << "*** Runtime exception caught *** \n    Line# : " << rte->GetStatement()->GetSourceLineNo() << "\n    Code  : ";

		Handle		source=mCurrentProgram->GetSourceCode();
		
		if (source)
		{
			// got the source, locate the correct line and output it
			UHandleLocker	lock(source,true);
			UInt32			max=::GetHandleSize(source);
			UInt32			linesEncountered=1;
			UInt32			destLine=rte->GetStatement()->GetSourceLineNo();
			bool			skippingWhiteSpace=true;	// used to skip white space at the beginning of a source code line
			
			for (UInt32 temp=0; temp<max; temp++)
			{
				if ((*source)[temp]=='\n')	// new line
					linesEncountered++;
				else if (linesEncountered==destLine)
				{
					if (skippingWhiteSpace && ((*source)[temp]==' ' || (*source)[temp]=='\t'))
						; // white space
					else
					{
						skippingWhiteSpace=false;
						std::cout << (*source)[temp];
					}
				}
				else if (linesEncountered>destLine)
					break;
			}
		}
		else
			std::cout << "Source unavailable";
			
		// if it's a moan compile error say so
		{
			StAEDescriptor		moanProg(rte->mContextRecord,keyTBMoanProg,typeText);
			StAEDescriptor		moanChan(rte->mContextRecord,keyTBMoanChannel,typeSInt32);
			StAEDescriptor		moanErrorLoc(rte->mContextRecord,keyTBMoanProgError,typeSInt32);
		
			if (moanProg.IsNotNull() && moanChan.IsNotNull() && moanErrorLoc.IsNotNull())
			{
				SInt32			chanId;
				CCString		progStr="<< Moan prog unavailable >>";
				Size			size=::AEGetDescDataSize(moanProg);				
				
				::AEGetDescData(moanProg,progStr.mString,progStr.GetMaxLength()-1);	// failiure will leave empty string
				progStr.mString[size]=0;	// null terminate to make a cstr
				
				UExtractFromAEDesc::TheSInt32(moanChan,chanId);
				
				std::cout << "\n    Error : Moan compile error on channel " << chanId;
				std::cout << "\n            " << inErr;
				std::cout << "\n    Prog  : " << progStr;
				
				// now mark the error location with a ^
				std::cout << "\n            ";
				SInt32			errLoc;

				UExtractFromAEDesc::TheSInt32(moanErrorLoc,errLoc);
				
				while (errLoc--)
					std::cout << " ";
				std::cout << "^\n";
			}
			else if (moanChan.IsNotNull())		// moan runtime error
			{
				SInt32			chanId;
				UExtractFromAEDesc::TheSInt32(moanChan,chanId);
				std::cout << "\n    Error : Moan runtime error on channel " << chanId;
				std::cout << "\n            " << inErr << "\n";				
			}
			else	// else just print the error message
				std::cout << "\n    Error : " << inErr << "\n";
		}
	}
	else
		std::cout << "*** Unsourced runtime exception caught : " << inErr << " ***\n";

	mReturnToApp=0L;		// If an error has occurred then don't switch out of TNT basic, let them read the error
	EndProgram();
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• HandleKeyPress											/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
Boolean CApplication::HandleKeyPress(
	const EventRecord& inKeyEvent)
{
	if ((inKeyEvent.message&charCodeMask)==kEscChar)
	{
		// resume
		if (mProgramPaused)
		{
			// wait until they release the esc key
			while (IsPressed(kEscSC))
				;
			ResumeProgram();
		}
		return true;
	}
	else
		return inheritedApp::HandleKeyPress(inKeyEvent);
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• EventUpdate												/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
void CApplication::EventUpdate(
	const EventRecord& inMacEvent)
{
	LWindow		*theWindow = LWindow::FetchWindowObject((WindowPtr)inMacEvent.message);
	bool		handled=false;

	if (theWindow != nil)
	{
		theWindow->UpdatePort();
		handled=true;
	}	
	else
	{
		if (mCurrentProgram)
		{
			if (mCurrentProgram->EventUpdate(inMacEvent))
				handled=true;
		}
	}
	
	
	// i remmed this out cos it was stopping our about box being updated
	// 		- mark (30/10/01)
/*	if (!handled)
	{
		WindowPtr			wp=(WindowPtr)inMacEvent.message;
		UWindowUpdater		safe(wp);
	}*/
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• EventResume
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
void CApplication::EventResume(
	const EventRecord& inMacEvent)
{
	inheritedApp::EventResume(inMacEvent);
	mAppResumed=true;
	
	if (mCurrentProgram)
		mCurrentProgram->EventResume(inMacEvent);
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• EventSuspend
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
void CApplication::EventSuspend(
	const EventRecord& inMacEvent)
{
	inheritedApp::EventSuspend(inMacEvent);
	mAppResumed=false;
	
	if (mCurrentProgram)
		mCurrentProgram->EventSuspend(inMacEvent);
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• HandleAppleEvent
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Supports the open AE
void CApplication::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult,
	long				inAENumber)
{
	Try_
	{
		switch (inAENumber)
		{
			case ae_OpenDoc:
			case ae_TBTest:
				Try_
				{
					DoAEOpen(inAppleEvent,outResult,inAENumber==ae_OpenDoc);
				}
				Catch_(err)
				{
					// ensure the outresult is stored before throwing
					if ( (outResult.descriptorType!=typeNull) && (outAEReply.descriptorType != typeNull) )
						ThrowIfOSErr_(::AEPutParamDesc(&outAEReply, keyAEResult, &outResult));
					throw;
				}
				break;

			case ae_TBBuild:
				DoAEBuild(inAppleEvent);
				break;
				
			case ae_TBQueryRegistration:
				// return whether or not tb is registered
				{
					Boolean			regged=true;
					ThrowIfOSErr_(::AECreateDesc(typeBoolean,&regged,sizeof(regged),&outResult));
				}
				break;

			default:
				inheritedApp::HandleAppleEvent(inAppleEvent,outAEReply,outResult,inAENumber);
				break;
		}
	}
	Catch_(err)
	{
		// put the error string on the reply event
		Str255		errStr="\p";
		ErrStr_(err,errStr);
		
		if (errStr[0])	// error string?
		{
			StAEDescriptor		desc(errStr);
			::AEPutParamDesc(&outAEReply,keyErrorString,desc);
		}
	
		throw;
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• GetAppletTemplateSpec										/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Gets the fsspec of the template for the applet
void CApplication::GetAppTemplateSpec(
	FSSpec		&outSpec)
{
	ThrowIfOSErr_(::GetAppSpec(&outSpec));
	
	CopyPString("\pTNT Basic",outSpec.name);
}

#if 0
////////////////////////////// OLD RES MERGE CODE
// used prior to tb 1.3 to merge project resources into dest app
void CApplication::OldResMerge()
{
		// Now merge the resources from source into dest to create the final application
		{
			LFile		sourceFile(source),destFile(dest);			
			SInt16		destFileRef=destFile.OpenResourceFork(fsRdWrPerm);
			SInt16		sourceFileRef=sourceFile.OpenResourceFork(fsRdPerm);
			
			// Get the creator code of the file
			Handle		infoHandle=::BetterGetResource(sourceFileRef,kTNTBasic_ProgramInfoResType,kTNTBasic_ProgramInfoResId);
												   
			if (infoHandle)
			{
				TNewHandleStream		stream(infoHandle,true);
		
				short		version;
			
				stream >> version;
			
				if (version==0)
					stream >> creatorCode;
			}
			
			SInt16		maxTypes,maxResources;
			ResType		thisType;
			
			maxTypes=::Count1Types();
			
			for (SInt16 typeIndex=1; typeIndex<=maxTypes; typeIndex++)
			{
				GetIndType(&thisType,typeIndex);
							
				maxResources=::Count1Resources(thisType);
				
				for (SInt16 resIndex=1; resIndex<=maxResources; resIndex++)
				{
					Handle				theRes=::Get1IndResource(thisType,resIndex);
					ThrowIfResFail_(theRes);
					UHandleReleaser		rel(theRes);

					Str255				resName;
					SInt16				resId;

					::GetResInfo(theRes,&resId,&thisType,resName);
					ThrowIfResError_();
					::DetachResource(theRes);
					ThrowIfResError_();

					OSErr		err;

					// encrypt basic resources on the way through and change them from
					// 'Basc' to 'EncB'
					if (thisType==kTNTBasic_CodeResType)
					{
						UEncryption::SimpleEncryptDecrypt(theRes,kCodeEncryptionKey);
						err=::BetterAddResource(destFileRef,theRes,kTNTBasic_EncryptedCodeResType,&resId,resName,kBottleIt);
					}
					else
						err=::BetterAddResource(destFileRef,theRes,thisType,&resId,resName,kBottleIt);
						
					ThrowIfResError_();
					
					if (err==userCanceledErr)
					{
						// error writing resource as it already exists....
						char		temp[5]; // convert type to cstr
						
						*((ResType*)&temp[0])=thisType;
						temp[4]=0;
						
						std::cout << "Error merging resource " << temp << " " << (SInt32)resId << ". Resource already exists in Applet Template.\n";
					}
					else
						ThrowIfOSErr_(err);						
				}
			}
			
			::UpdateResFile(destFileRef);
		}
}
////////////////////////
#endif

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• CreateDataBuild														/*e*/
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// copies all project data from the source container to the dest container
// does any conversion needed for the runtime applet from the source project
void CApplication::CreateDataBuild(
	CResourceContainer		*inSourceContainer,
	CResourceContainer		*inDestContainer,
	OSType					*outCreatorCode)
{
	Handle			allTypes=NULL;

	allTypes=inSourceContainer->GetAllResTypes();

	CNewHandleStream	allTypesStream(allTypes,true);
	SInt32				length=allTypesStream.GetLength();

	while (allTypesStream.GetMarker()<length)
	{
		ResType		type;
		Handle		typeHandle=NULL;
		SInt32		count=0;
		
		allTypesStream >> type;

		typeHandle=inSourceContainer->GetAllResourceIdsAndNames(type,count);
		CNewHandleStream	typeStream(typeHandle,true);

		for (SInt32	ri=0; ri<count; ri++)
		{
			TResId				id;
			TResName			name;

			typeStream >> id;
			typeStream >> name;

			StTNTResource		res(inSourceContainer,type,id);

			// encrypt basic resources on the way through and change them from
			// 'Basc' to 'EncB'
			if (type==kTNTBasic_CodeResType)
			{
				Handle		copy=res->GetReadOnlyDataHandle();
				ThrowIfOSErr_(::HandToHand(&copy));
				UEncryption::SimpleEncryptDecrypt(copy,kCodeEncryptionKey);
				inDestContainer->AddResource(kTNTBasic_EncryptedCodeResType,id,name,copy,true);				
			}
			else
			{
				Handle				resData=res->GetReadOnlyDataHandle();
				
				// read the creator code as we copy it across - the caller will need this to set the creator code of the resulting app
				if (type==kTNTBasic_ProgramInfoResType && id==kTNTBasic_ProgramInfoResId)
				{
					OSType		result=ReadCreatorCodeFromProgInfo(res.Resolve());
					if (result)
						*outCreatorCode=result;
				}

				Try_
				{				
					inDestContainer->AddResource(type,id,name,resData,false);
				}
				Catch_(err)
				{
					// append an extra message to say builds often fail due to using resource ids <128
					if (ErrCode_(err)==kTNTErr_ResourceAlreadyExists)
					{
						Str255		newErrStr,errS;
						BetterGetString(mAppResFile,kSTR_BuildFailedUseResAbove128,newErrStr);

						ErrStr_(err,errS);			// get existing error message
						ConcatPStr(newErrStr,"\p\n");
						ConcatPStr(newErrStr,errS);
						ThrowStr_(ErrCode_(err),newErrStr);						
					}
					throw;
				}
			}
		}
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ReadCreatorCodeFromProgInfo
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// returns the creator code from a prog info resource, returns 0 if error
OSType CApplication::ReadCreatorCodeFromProgInfo(
	CResource		*inRes)
{
	Handle				resData=inRes->GetReadOnlyDataHandle();	
	TNewHandleStream	stream(resData,false);
	OSType				result=0;

	short		version;

	stream >> version;

	if (version==0)
		stream >> result;

	return result;
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• DoAEBuild
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Creates an applet to run the resource
void CApplication::DoAEBuild(
	const AppleEvent	&inAE)
{
	Try_
	{
		FSSpec				source,dest,templateSpec;		
		StAEDescriptor		sourceDesc(inAE,keyDirectObject,typeFSS);
		StAEDescriptor		destDesc(inAE,keyTBDest,typeFSS);
		StAEDescriptor		returnTo(inAE,keyTBReturnToProcess,typeApplSignature);
		OSType				returnToApp=0L;

		if (returnTo.IsNotNull())
		{
			if (returnTo.mDesc.descriptorType==typeApplSignature)
				::BlockMoveData(*(void**)returnTo.mDesc.dataHandle,&returnToApp,sizeof(returnToApp));
		}

		if (sourceDesc.mDesc.descriptorType==typeFSS)	// not null
			::BlockMoveData(*(void**)sourceDesc.mDesc.dataHandle,&source,sizeof(FSSpec));
		else
			Throw_(paramErr);

		if (destDesc.mDesc.descriptorType==typeFSS)	// not null
			::BlockMoveData(*(void**)destDesc.mDesc.dataHandle,&dest,sizeof(FSSpec));
		else
			Throw_(paramErr);

		FSSpec		destContainerSpec=dest;
		
		// The destSpec should be that of a directory not a file, we need to get the FSSpec of the parent directory
		ThrowIfOSErr_(::GetDirFSSpec(dest.vRefNum,dest.parID,&destContainerSpec));

		GetAppTemplateSpec(templateSpec);
		
		// delete the dest file if it exists
		OSErr err=::FSpDelete(&dest);
		if (err!=fnfErr)
			ThrowIfOSErr_(err);

		ThrowIfOSErr_(::FSpFileCopy(&templateSpec,&destContainerSpec,dest.name,0,0,true));
		
		OSType		creatorCode='TNTP';
		
		// open the passed resource file and determine whether it should be merged into the app or into a separate data bundle
		CResourceContainerCreator		*creator=CResourceContainerCreator::GetCreatorForFile(&source);
		if (!creator)
			Throw_(kTBErr_UnrecognisedResFileFormat);

		{		
			StDeleter<CResourceContainer>	sourceResFile(creator->OpenFile(&source,true));
			StDeleter<CResourceContainer>	destApp(mMacResFileCreator->OpenFile(&dest,false));
			bool							isPackageDataFile=false;
			
			
			// TODO: decide in what format to build the app
			// if the game is small we can merge it into the app (rsrc map max size is 16Mb)
			// otherwise, use data folder
			// FUTURE: produce a mac os x .app with the data internalised in the bundle
			// FOR NOW: use the same format as the file passed
			if (dynamic_cast<CMacResFileCreator*>(creator))
			{
				// came from a mac res file - merge into the app to create an all in one
				
				CreateDataBuild(sourceResFile,destApp,&creatorCode);

				// write resource saying load from app
				destApp->AddResource(kTNTBasic_DataFilePathResType,kTNTBasic_DataFilePathResId,"\p",CApplication::MakeDataFilePathForResContainer(kDataType_AppResFork,&dest));
			}
			else
			{
				// came from other resource file
				
				// need to read the creator code here and now
				Try_
				{
					StTNTResource			res(sourceResFile,kTNTBasic_ProgramInfoResType,kTNTBasic_ProgramInfoResId);
					OSType					creator=ReadCreatorCodeFromProgInfo(res.Resolve());
					
					if (creator)
						creatorCode=creator;
				}
				Catch_(err)
				{
					ECHO("Failed to get creator code from prog info of data file during build " << ErrCode_(err));
				}

				// make the '.creatorcode' str
				Str15			creatorCodeStr;
				creatorCodeStr[1]='.';
				::BlockMoveData(&creatorCode,&creatorCodeStr[2],sizeof(creatorCode));
				for (creatorCodeStr[0]=0; creatorCodeStr[0]<sizeof(creatorCode); creatorCodeStr[0]++)
				{
					SInt8	chr=creatorCodeStr[creatorCodeStr[0]+2];
					
					if (chr==' ' || chr==0)
					{
						break;
					}
				}
				creatorCodeStr[0]++;
				
				// create fsspec for data file
				FSSpec				destDataFile;
				destDataFile.vRefNum=dest.vRefNum;
				destDataFile.parID=dest.parID;
				CopyPStr(dest.name,destDataFile.name);
				ConcatPStr(destDataFile.name,"\p Data");
				ConcatPStr(destDataFile.name,creatorCodeStr);
				
				// create data file
				{
					mBundleResFileCreator->CreateFile(&destDataFile,creatorCodeStr,creatorCode,creatorCode);
					
					StDeleter<CResourceContainer>	dataFile(mBundleResFileCreator->OpenFile(&destDataFile,false));
					
					CreateDataBuild(sourceResFile,dataFile,&creatorCode);
				}

#if 0				
				// now we have the creator code, use it to rename the data bundle - this will allow us to make the bundle owned
				// by the built app, rather than hieroglyph
				Str63			newName;
				Str15			creatorCodeStr;
				::BlockMoveData(&creatorCode,&creatorCodeStr[1],sizeof(creatorCode));
				for (creatorCodeStr[0]=0; creatorCodeStr[0]<sizeof(creatorCode); creatorCodeStr[0]++)
				{
					SInt8	chr=creatorCodeStr[creatorCodeStr[0]+1];
					
					if (chr==' ' || chr==0)
					{
						break;
					}
				}
				CopyPStr(destDataFile.name,newName);
				ConcatPStr(newName,"\p.");
				ConcatPStr(newName,creatorCodeStr);
				
				ThrowIfOSErr_(::HMoveRenameCompat(destDataFile.vRefNum,destDataFile.parID,destDataFile.name,destDataFile.parID,NULL,newName));
				
				CopyPStr(newName,destDataFile.name);
#endif
				
				// write resource into app saying load from data file
				destApp->AddResource(kTNTBasic_DataFilePathResType,kTNTBasic_DataFilePathResId,"\p",CApplication::MakeDataFilePathForResContainer(kDataType_DataBundle,&destDataFile));
				
				isPackageDataFile=true;
			}
			
			// edit the plst of the newly created app to add the data folder as a package
			{
				Handle				plstHandle=::BetterGetResource(mAppResFile,'TEXT',kTEXT_PlstTemplate);
				
				ThrowIfOSErr_(::BetterGetResErr());
				::DetachResource(plstHandle);
				ThrowIfResError_();
				
				Size				len=::GetHandleSize(plstHandle);
				Size				maxLen=len+512;	// add a bit of space for substituting in strings
				
				ThrowIfOSErr_(::BetterSetHandleSize(plstHandle,maxLen,0));

				{
					StHandleLocker		locked(plstHandle);
					short				numReplaces;
					const StringPtr		kCreatorCodeSub="\p$CREATORCODE";
					const StringPtr		kGameName="\p$GAMENAME";
					const StringPtr		kIsPackageBool="\p$ISPACKAGEBOOL";
					
					numReplaces=::FindAndReplace(*plstHandle,&len,maxLen,(Ptr)kCreatorCodeSub+1,kCreatorCodeSub[0],(Ptr)&creatorCode,sizeof(creatorCode),true);
					Assert_(numReplaces>0);

					numReplaces=::FindAndReplace(*plstHandle,&len,maxLen,(Ptr)kGameName+1,kGameName[0],(Ptr)&dest.name[1],dest.name[0],true);
					Assert_(numReplaces>0);

					const unsigned char		*isPackageStr=isPackageDataFile ? "\ptrue" : "\pfalse";
					numReplaces=::FindAndReplace(*plstHandle,&len,maxLen,(Ptr)kIsPackageBool+1,kIsPackageBool[0],(Ptr)isPackageStr+1,isPackageStr[0],true);
					Assert_(numReplaces>0);
				}
				
				// for mac os 9 file types - change the fref resource for our data file to be the data file type				
				StTNTResource		res(destApp,'FREF',kFREF_DataFile);
				StTNTResource		res2(destApp,'BNDL',kBNDL_MainResource);
				UInt32				size=sizeof(creatorCode);
				
				ThrowIfOSErr_(res->PutResourceData(0,&creatorCode,size));		// file type is in the first 4 bytes of the resource
				ThrowIfOSErr_(res2->PutResourceData(0,&creatorCode,size));		// file type is in the first 4 bytes of the resource
				
				
				destApp->ChangeResourceInfo(kTBCreatorCode,0,creatorCode,0,"\p");
				
				ThrowIfOSErr_(::BetterSetHandleSize(plstHandle,len,0));
				
				destApp->DeleteResource('plst',0);
				destApp->AddResource('plst',0,"\p",plstHandle,true);
				
				destApp->Flush();
			}
							
			if (returnToApp)
			{
				try
				{
					// if the console window is open then don't bother switching app as they probably want to read it
					if (mOutputWindow.GetWindowNoOpen()==0)
					{
						ProcessSerialNumber		num=UProcess::GetPSN(returnToApp);
						::SetFrontProcess(&num);
					}
				}
				catch(...)
				{
				}
			}
		}
		
		// Set the creator code
		::FSpChangeCreatorType(&dest,creatorCode,'APPL');
	}
	Catch_(err)
	{
		std::cout << "Error building applet: " << err << "\n";
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• DoAEOpen
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Opens a list of documents and extracts the return to app sig
void CApplication::DoAEOpen(
	const AppleEvent	&inAppleEvent,
	AEDesc				&outResult,
	bool				inRunIt)
{	
	if (mCurrentProgram)		// program already running
		Throw_(kTBErr_ProgramAlreadyRunning);		

	StartUp();	// map onto the version without the documents
	
	OSType				returnToApp=0L;
	Boolean				supErrs=false;

	{
		StAEDescriptor		returnTo(inAppleEvent,keyTBReturnToProcess,typeApplSignature);
		
		if (returnTo.IsNotNull())
		{
			if (returnTo.mDesc.descriptorType==typeApplSignature)
				::BlockMoveData(*(void**)returnTo.mDesc.dataHandle,&returnToApp,sizeof(returnToApp));
		}
	}
	
	{
		StAEDescriptor		supErr(inAppleEvent,keyTBSurpressErrors,typeBoolean);

		if (supErr.IsNotNull())
			UExtractFromAEDesc::TheBoolean(supErr,supErrs);
	}
	
	// Extract the first document only
	StAEDescriptor	docList;
	OSErr			err = ::AEGetParamDesc(&inAppleEvent,keyDirectObject,typeAEList, docList);
	ThrowIfOSErr_(err);

	AEKeyword	theKey;
	DescType	theType;
	FSSpec		theFileSpec;
	Size		theSize;
	err = ::AEGetNthPtr(docList,1,typeFSS,&theKey,&theType,(Ptr)&theFileSpec,sizeof(FSSpec),&theSize);
	ThrowIfOSErr_(err);
	
	if (!OpenDocument(&theFileSpec,returnToApp,!supErrs,&outResult,inRunIt))
		Throw_(-1);	// misc error
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• TakeScreenShot
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Takes a screen shot of the main canvas and saves it out to a file
// can gab from the window or from the backbuffer. Depending on the program state either one could be either partially drawn
// or empty.
void CApplication::TakeScreenShot(
	bool			inGrabFromWindow)
{
	Try_
	{
		if (mCurrentProgram && mCurrentProgram->GetGraphicsContext())
		{
			CGraphicsContext		*context=mCurrentProgram->GetGraphicsContext();
			SInt16					vref;
			SInt32					dirId;
			
			OSErr	err=::FindFolder(kOnAppropriateDisk,kPictureDocumentsFolderType,false,&vref,&dirId);
			
			if (err)
				err=::FindFolder(kOnAppropriateDisk,kClassicDesktopFolderType,false,&vref,&dirId);
			
			if (err)
			{
				ECHO("Can't take screen shot, can't find pictures folder or OS 9 desktop!");
				Throw_(err);		// can't find anywhere to save the file
			}
						
			PicHandle				pic=context->TakeScreenShot(inGrabFromWindow);
			
			Try_
			{
				ThrowIfNil_(pic);
			
				LStr255					progName=mCurrentProgram->GetProgramName();
				LStr255					fileName;
				unsigned char			suffix[]="\p.pict";
				Str15					uniqueNumStr="\p";
				SInt16					uniqueNum=0;
				FSSpec					fsSpec;
				
				// make a unique name
				do
				{
					fileName=progName;
					if ((fileName[0]+suffix[0]+uniqueNumStr[0])>kHFSMaxFileNameChars)
						fileName[0]=kHFSMaxFileNameChars-suffix[0]-uniqueNumStr[0];		// truncate it				
					
					fileName+=uniqueNumStr;
					fileName+=suffix;
					
					err=::FSMakeFSSpec(vref,dirId,fileName,&fsSpec);
					
					if (err==fnfErr)
						break;			// file does not exist so this will do for a file name
					
					ThrowIfOSErr_(err);	// any other error is bad
					
					// no error, file existed, move to unique next file name and retry
					uniqueNum++;
					::NumToString(uniqueNum,uniqueNumStr);
					
					// safety constaint
					if (uniqueNum>999)		// how many pictures?!
						Throw_(-1);
						
				} while (true);	
				
				// save pict info FSSpec
				LFileStream			file(fsSpec);
				
				file.CreateNewDataFile(UEnvironment::IsRunningOSX() ? 'prvw' : 'ogle','PICT',smSystemScript);	// pict for pictureviewer or pict for preview under os x
				file.OpenDataFork(fsRdWrPerm);

				// pict file has 512 bytes of empty ness then the pict handle
				{
					char			stuff[512];			
					file.WriteBlock(stuff,512);
				}
				{
					UHandleLocker		lock((Handle)pic,true);
					
					file.WriteBlock(*pic,::GetHandleSize((Handle)pic));
				}
			}
			Catch_(err)
			{
				::KillPicture(pic);
				throw;
			}
			::KillPicture(pic);
			
			// Success, play snap shot sound
			Try_
			{
				UResFetcher				res(mAppResFile,'snd ',kSnd_SnapShot);
				UHandleLocker			lock(res);
				
				::SndPlay(0L,(SndListHandle)res.GetResourceHandle(),(false));
			}
			Catch_(err)
			{
				// ignore
			}
		}
	}
	Catch_(err)
	{
		SysBeep(0);
		ECHO("Error taking screen shot: " << ErrCode_(err));
	}
}


// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• DoRaaAboutBox
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Attempts to do a rather fancy TNT scrolly text about box, returns false for fail, true for ok
bool CApplication::DoRaaAboutBox()
{
	UResFileSaver	safeResFile(mAppResFile);
	WindowPtr		theWindow=0;
	PicHandle		backPic=0L;
	Rect			winRect;
	ScrollyRec		theRec=emptyScrollyRec;
	Boolean			ret=true;
	
	backPic=GetPicture(kPICT_AboutBox);
	if (backPic)
	{
		winRect=(**backPic).picFrame;
		
		BigEndianRectToNative(winRect);
		
		BitMap		screenBits;
				
		CenterRectInRect(&winRect,&GetQDGlobalsScreenBits(&screenBits)->bounds);
		theWindow=NewWindow(0L,&winRect,"\p",false,plainDBox,(WindowPtr)-1,false,0L);
		if (theWindow)
		{
			LStr255			regStr;
			
			/*
			if (URegistration::IsRegistered())
			{
				regStr+=LStr255(kSTRx_RegStrings,kStr_RegTo1);
				regStr+=URegistration::GetName();
				regStr+=LStr255(kSTRx_RegStrings,kStr_RegTo2);
				regStr+=URegistration::GetCode();
				regStr+=LStr255(kSTRx_RegStrings,kStr_RegTo3);
			}
			else
				regStr.Assign(kSTRx_RegStrings,kStr_Unregistered);
			*/
		
			StringPtr		findStrings[]={"\p∆ver","\p∆reg","\p2000-06"};
			StringPtr		replaceStrings[]={verShortTextP,regStr,"\p2000-12"};
					
			SetPort(GetWindowPort(theWindow));
			ZeroRectCorner(winRect);
			
			// Fill out a scrolly rec and then prepare the credits
			theRec.theWindow=theWindow;
			theRec.visRect=winRect;
			theRec.picRect=winRect;
			theRec.visRect.top+=10;
			theRec.visRect.bottom-=10;
			theRec.visRect.left+=220;
			theRec.visRect.right-=20;
			theRec.forceToBitDepth=16;
			theRec.blackText=true;
			theRec.everyVBL=2;		// 30hz
			
			if (StartupScrollyCredits(&theRec,backPic,kText_About,teJustCenter,findStrings,replaceStrings,3))
			{
				ShowWindow(theWindow);
				DrawPicture(backPic,&winRect);

/*#if TARGET_API_MAC_CARBON
	// can't link to this through carbon - doh
	if (QDDisplayWaitCursor)
		::QDDisplayWaitCursor(false);		// stop spinning wheel cursor which will start cos we're not calling wne
#endif	*/			
				while (!Button())
					ScrollCredits(&theRec);

/*#if TARGET_API_MAC_CARBON
	if (QDDisplayWaitCursor)
		::QDDisplayWaitCursor(true);
#endif*/
			}
			else
				ret=false;
			
			ShutDownScrollyCredits(&theRec);
			DisposeWindow(theWindow);
		}
		ReleaseResource((Handle)backPic);
	}
	
	FlushEvents(mDownMask | keyDownMask | autoKeyMask,0L);
	
	return ret;
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• IsFPSVisible
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
bool CApplication::IsFPSVisible()
{
	return mShowFPS;
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• AbsTimeStr					Static
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// returns a time stamp in seconds, useful for timing operations in conjunction with ECHO calls
double CApplication::AbsTimeStr()
{
	AbsoluteTime	time=::UpTime();
	
	return AbsoluteToSeconds(time);
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• AbsoluteToSeconds				Static
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
double CApplication::AbsoluteToSeconds(
	AbsoluteTime inTime )
{
    union
    {
        UInt64	i;
        Nanoseconds ns;
    }temp;
    temp.ns = AbsoluteToNanoseconds( inTime );
    return 0.000000001 * (double) temp.i;
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• KeyMapXOR					Static
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// does an XOR operation on two keymap listings
void CApplication::KeyMapXOR(
	KeyMap			outResult,
	const KeyMap	inA,
	const KeyMap	inB)
{
	for (UInt16 i=0; i<kNumKeyMapLongs; i++)
	{
#if TARGET_RT_LITTLE_ENDIAN
		outResult[i].bigEndianValue=inA[i].bigEndianValue^inB[i].bigEndianValue;
#else
		outResult[i]=inA[i]^inB[i];
#endif
	}
}

#if TARGET_RT_LITTLE_ENDIAN
#define KeyMapEq(inA,inB)		(inA[0].bigEndianValue==inB[0].bigEndianValue && inA[1].bigEndianValue==inB[1].bigEndianValue && inA[2].bigEndianValue==inB[2].bigEndianValue && inA[3].bigEndianValue==inB[3].bigEndianValue)
#define KeyMapAssign(outA,inB)	do { outA[0].bigEndianValue=inB[0].bigEndianValue; outA[1].bigEndianValue=inB[1].bigEndianValue; outA[2].bigEndianValue=inB[2].bigEndianValue; outA[3].bigEndianValue=inB[3].bigEndianValue; } while (false)
#else
#define KeyMapEq(inA,inB)		(inA[0]==inB[0] && inA[1]==inB[1] && inA[2]==inB[2] && inA[3]==inB[3])
#define KeyMapAssign(outA,inB)	do { outA[0]=inB[0]; outA[1]=inB[1]; outA[2]=inB[2]; outA[3]=inB[3]; } while (false)
#endif

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• KeyCanAutoRepeat
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// returns true if the key with the given scan code is allowed to generate auto repeats
// only used from the GetKeys keyboard reader
bool CApplication::KeyCanAutoRepeat(
	UInt32			inScanCode)
{
	return (inScanCode!=kCommandKeySC && inScanCode!=kOptionKeySC && inScanCode!=kShiftKeySC && inScanCode!=kControlKeySC && inScanCode!=kCapsLockSC && inScanCode!=kFunctionKeySC);
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• UpdateKeyboardGetKeys
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// updates the applications keymap, and posts any new keypresses to the currently running program
// either uses getkeys
void CApplication::UpdateKeyboardGetKeys()
{
#if TARGET_RT_LITTLE_ENDIAN
	printf("UpdateKeyboardGetKeys needs endian flipping code adding\n");
	// it would be sensible to wrap the keymap structure and possibly to apply the endian flipping when it is filled out?
#else
	UInt32			newUpdateTime=CGraphicsContext::sMsTimer.GetTime();
	KeyMap			newkeys;
	
	if (newUpdateTime==mKeyMapLastUpdateTime)
		return;		// can't update twice in the same tick as that would mess up our key repeating code

	mKeyMapLastUpdateTime=newUpdateTime;
	
	::GetKeys(newkeys);
	if (!KeyMapEq(newkeys,mKeyMap))
	{
		KeyMap	difference;
		
		KeyMapXOR(difference,mKeyMap,newkeys);

		// first update the held keys list
		for (UInt16 keyInt=0; keyInt<kNumKeyMapLongs; keyInt++)
		{
			if (difference[keyInt])			// check 4 bytes at a time for differences
			{
				UInt8	*keyBytes=(UInt8*)&difference[keyInt];
				
				for (UInt8 byteIndex=0; byteIndex<4; byteIndex++)		// now at a byte level
				{
					if (!keyBytes[byteIndex])
						continue;
						
					for (UInt8 bit=0; bit<8; bit++)						// finally at a bit level
					{						
						if (keyBytes[byteIndex]&(1<<bit))
						{
							UInt32		scanCode=8*byteIndex+keyInt*32+bit;
															
							if (((UInt8*)&newkeys[keyInt])[byteIndex]&(1<<bit))
							{
								// key scanCode has just been pressed
								if (mHeldKeysCount<kMaxHeldKeysToTrack)
								{
									mHeldKeys[mHeldKeysCount].scancode=scanCode;
									mHeldKeys[mHeldKeysCount].lastPressTime=mKeyMapLastUpdateTime;
									mHeldKeys[mHeldKeysCount].autoRepeating=false;
									mHeldKeysCount++;
								}
								else
									ECHO("Tracking too many held keys, key with scancode " << scanCode << " being ignored\n");
							}
							else
							{
								bool	found=false;

								// key scanCode has just been released, expect to find it in the held keys list
								for (UInt16 heldKey=0; heldKey<mHeldKeysCount; heldKey++)
								{
									if (mHeldKeys[heldKey].scancode==scanCode)
									{
										mHeldKeys[heldKey]=mHeldKeys[--mHeldKeysCount];
										found=true;
										break;
									}
								}
								
								if (!found)
									ECHO("Failed find newly released key with scancode " << scanCode << " in held keys list\n");
							}
						}
					}
				}
			}
		}
		
		// update the keymap
		KeyMapAssign(mKeyMap,newkeys);
	}

	// now use the held keys list to post keys to the program
	if (mCurrentProgram)
	{
		UInt32		beginRepeatAge=(LMGetKeyThresh()*1000)/60;
		UInt32		repeatAge=(LMGetKeyRepThresh()*1000)/60;
		UInt32		currentModifiers=0;
		
		if (FastIsPressed(mKeyMap,kCommandKeySC))
			currentModifiers|=cmdKey;
		if (FastIsPressed(mKeyMap,kOptionKeySC))
			currentModifiers|=optionKey;
		if (FastIsPressed(mKeyMap,kShiftKeySC))
			currentModifiers|=shiftKey;
		if (FastIsPressed(mKeyMap,kControlKeySC))
			currentModifiers|=controlKey;
		if (FastIsPressed(mKeyMap,kCapsLockSC))
			currentModifiers|=alphaLock;		// alpha lock is another name for caps lock for some reason
		
		for (UInt16 heldKey=0; heldKey<mHeldKeysCount; heldKey++)
		{
			UInt32		age=mKeyMapLastUpdateTime-mHeldKeys[heldKey].lastPressTime;
			
			if (age==0 && !mHeldKeys[heldKey].autoRepeating)
				mCurrentProgram->HandleKeyPressByScancode(mHeldKeys[heldKey].scancode,currentModifiers,false);
			else if (((!mHeldKeys[heldKey].autoRepeating && age>beginRepeatAge) || (mHeldKeys[heldKey].autoRepeating && age>repeatAge)) && KeyCanAutoRepeat(mHeldKeys[heldKey].scancode))
			{
				mHeldKeys[heldKey].autoRepeating=true;
				mHeldKeys[heldKey].lastPressTime=mKeyMapLastUpdateTime;
				mCurrentProgram->HandleKeyPressByScancode(mHeldKeys[heldKey].scancode,currentModifiers,true);
			}
		}
	}
#endif
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• ClearKeyboard
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// clears the keymap and the list of held keys for key press auto repeat generation
// (nb called from constructor)
void CApplication::ClearKeyboard()
{
	mKeyMapLastUpdateTime=0;
	mHeldKeysCount=0;
	for (SInt16 i=0; i<kNumKeyMapLongs; i++)
	{
#if TARGET_RT_LITTLE_ENDIAN
		mKeyMap[i].bigEndianValue=0;	
#else
		mKeyMap[i]=0;	
#endif
	}
}

//#define FastSetPressed(km,k)		(( ((unsigned char *)(km))[(k)>>3] >> ((k) & 7) ) & 1)
#define FastSetPressed(km,k)		((unsigned char *)(km))[(k)>>3] |= 1 << ((k) & 7)
#define FastClearPressed(km,k)		((unsigned char *)(km))[(k)>>3] &= ~((unsigned char)(1 << ((k) & 7)))

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• KeyMapPress
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// updates the key map when a key is pressed and sends an keypress event to the current program
void CApplication::KeyMapPress(
	UInt32		inRaw,
	bool		inIsRepeat)
{
	CKeyQueue::STBRawKeycode		raw;
	raw.rawcode=inRaw;

	FastSetPressed(mKeyMap,raw.b.scancode);
	
	if (mCurrentProgram)
		mCurrentProgram->HandleKeyPressByRawcode(inRaw,inIsRepeat);
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• KeyMapRelease
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// updates the keymap when a key is released
void CApplication::KeyMapRelease(
	UInt32		inRaw)
{
	CKeyQueue::STBRawKeycode		raw;
	raw.rawcode=inRaw;

	FastClearPressed(mKeyMap,raw.b.scancode);
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• KeyMapModsChanged
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// updates the keymap modifier keys
void CApplication::KeyMapModsChanged(
	UInt32			inMods)
{
	struct
	{
		UInt32		mask,scan;
	} modArray[] = { { cmdKey,							kCommandKeySC },
					{ shiftKey | rightShiftKey,			kShiftKeySC },
					{ alphaLock, 						kCapsLockSC },
					{ optionKey | rightOptionKey,		kOptionKeySC },
					{ controlKey | rightControlKey,		kControlKeySC },
				};
				
	for (UInt16 i=0; i<sizeof(modArray)/sizeof(modArray[0]); i++)
	{
		CKeyQueue::STBRawKeycode	raw;
		raw.b.tbMods=0;
		raw.b.macMods=(inMods>>8)&0xFF;
		raw.b.scancode=modArray[i].scan;
		raw.b.charcode=0;
		
		if (inMods&modArray[i].mask && !FastIsPressed(mKeyMap,modArray[i].scan))
			KeyMapPress(raw.rawcode,false);
		else if (!(inMods&modArray[i].mask) && FastIsPressed(mKeyMap,modArray[i].scan))
			KeyMapRelease(raw.rawcode);
	}
}

// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//		• DoEvent
// ——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// app event handler
OSStatus CApplication::DoEvent(
	EventHandlerCallRef	inCallRef,
	EventRef			inEventRef)
{
	OSStatus		err=eventNotHandledErr;
	
	Try_
	{	
		UInt32		eventKind=0,eventClass=0;

		switch (eventClass=::GetEventClass(inEventRef))
		{
#if 0		// this was an attempt to capture the quit event from a cmd-q reqieved whilst a program is executing - it doesn't get called and so has been commented out
			case kEventClassCommand:
				ECHO("class command");
				switch (eventKind=GetEventKind(inEventRef))
				{
					case kEventCommandProcess:
						{
							HICommand	cm;
					 
						  	if (::GetEventParameter(inEventRef,kEventParamDirectObject,typeHICommand,0,sizeof(cm),0,&cm)==noErr)
						  	{
						  		switch (cm.commandID)
						  		{
						  			case kHICommandQuit:
						  				err=noErr;
						  				if (!mCurrentProgram || mCurrentProgram->CanBreak())
						  				{
						  					DoQuit(cmd_Quit);
						  				}
						  				break;
						  		}
						  	}
						}
						break;
				}
				break;
#endif
		}
	}
	Catch_(errc)
	{
		err=ErrCode_(errc);
	}
	
	return err;
}
