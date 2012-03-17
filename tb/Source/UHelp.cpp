// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UHelp.cpp
// © Mark Tully 2001
// 28/11/01
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2001, Mark Tully and John Treece-Birch
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

/*
	Code adding an alias to help to the system and managing the help functions
*/

#include "TNT_Debugging.h"
#include "UHelp.h"
#include "Root.h"
#include "Marks Routines.h"

#ifdef WritersBlock
	#include "Hieroglyph Menus.h"
	#include "TBlockProperties.h"
#endif

Boolean	UHelp::sUserGuideInstalled=false,UHelp::sHelpCentreInstalled=false;
MenuItemIndex	UHelp::sFirstHelpCommand=0;

void MakeAliasFile(
	const FSSpec		&inTargetFile,
	const FSSpec		&inNewAliasFile);
bool DoesAliasPointToFolder(
	FSSpec		&inAliasSpec,
	FSSpec		&inDestSpec);

#define USE_BUNDLED_HELP		1

enum
{
#if USE_BUNDLED_HELP
	kHelpMenu_TNTBasicHelp=-2,		// we don't manage this menu item when using bundled help
	kHelpMenu_UserGuide=-1,
	kHelpMenu_EmbeddedUserGuide=0,
#else
	kHelpMenu_EmbeddedUserGuide=-1,
	kHelpMenu_TNTBasicHelp=0,
	kHelpMenu_UserGuide=1
#endif
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ Init								Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UHelp::Init()
{
	Try_
	{
#if !USE_BUNDLED_HELP
		// old way of doing things, has the help external from the app
		InstallHelp();
#else
		// new way - embed it in the app bundle and just enable the menu items
		sHelpCentreInstalled=true;
		sUserGuideInstalled=true;
#endif
	}
	Catch_(err)
	{
		ECHO("*** ERROR: Error installing help " << ErrCode_(err));
	}	

	Try_
	{
		InstallHelpMenuItems();
	}
	Catch_(err)
	{
		ECHO("*** ERROR: Error installing help menu items " << ErrCode_(err));
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ HandleHelpHit						Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Launches the help view app, retrieves an url path from the resources and then looks up the page
void UHelp::HandleHelpHit(
	MenuItemIndex		inItem)
{
	inItem-=sFirstHelpCommand;		// knock off the first item, caters for extra help items such as balloon help under 9
	
	ECHO("TB Help item hit, item: " << inItem);
	
	SInt16		bookIndex=-1,urlIndex=-1,prefixIndex=-1;
	
	switch (inItem)
	{
		case kHelpMenu_TNTBasicHelp:
			bookIndex=kStr_TBHelpCentreFolderName;
			urlIndex=kStr_TBHelpCentreIndexURL;
			break;
			
		case kHelpMenu_UserGuide:
			bookIndex=kStr_TBUserGuideFolderName;
			urlIndex=kStr_TBUserGuideIndexURL;
			break;

		case kHelpMenu_EmbeddedUserGuide:
			bookIndex=kStr_TBHelpCentreFolderName;
			prefixIndex=kStr_TBUserGuideFolderName;
			urlIndex=kStr_TBUserGuideIndexURL;
			break;
	}
	
	if (AHGotoPage && urlIndex!=-1)
	{
		// open help centre
		LStr255		book(kSTRx_HelpStrings,bookIndex);
		LStr255		url(kSTRx_HelpStrings,urlIndex);
		LStr255		prefix;

		if (prefixIndex!=-1)
		{
			prefix.Assign(kSTRx_HelpStrings,prefixIndex);
			prefix+="/";
			prefix+=url;
			url=prefix;
		}

		ECHO("Help request, open book '"<<StringPtr(book)<<"' url '"<<StringPtr(url)<<"'");
		
		// convert to cstr
		CFStringRef bookStr=CFStringCreateWithPascalString(0,book,kCFStringEncodingMacRoman);
		CFStringRef urlStr=CFStringCreateWithPascalString(0,url,kCFStringEncodingMacRoman);
		
		OSErr	err=::AHGotoPage(bookStr,urlStr,0);	// NB can't use the CFSTR macro on variable so have to do the conversion above
		if (err)
		{
			ECHO("Error going to help page : " << err);
		}
		
		CFRelease(bookStr);
		CFRelease(urlStr);
	}
	else
		ECHO("*** ERROR : AHGotoPage not available or help item undefined");
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ InstallHelpMenuItems				Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UHelp::InstallHelpMenuItems()
{
	// Now add the items
	LStr255		helpCentreName(kSTRx_HelpStrings,kStr_TBHelpCentreFolderName);
	LStr255		userGuideName(kSTRx_HelpStrings,kStr_TBUserGuideFolderName);

#ifdef WritersBlock
	THrMenuReference		helpMenu=hr_GetMenuReference(hr_GetHieroBlock(),kHMHelpMenuID);
	THrMenuItemReference	centre,guide;
	hr_ShowHideMenuItem(centre=hr_InsertMenuItem(helpMenu,TBlockProperties::GetBlockReference(),0,0,true,helpCentreName),true);
	hr_ShowHideMenuItem(guide=hr_InsertMenuItem(helpMenu,TBlockProperties::GetBlockReference(),1,0,true,userGuideName),true);
	if (!sHelpCentreInstalled)
		::hr_EnableDisableMenuItem(centre,false);
	if (!sUserGuideInstalled)
		::hr_EnableDisableMenuItem(guide,false);
#else
	if (!HMGetHelpMenu)
	{
		ECHO("HMGetHelpMenu unavailable");
		return;
	}
	
	MenuRef			m;

	ThrowIfOSErr_(::HMGetHelpMenu(&m,&sFirstHelpCommand));

#if !USE_BUNDLED_HELP		// with bundled help - we automatically get the 'TNT Basic Help' menu item so we only need the User Guide one
	AppendMenu(m,helpCentreName);
	if (!sHelpCentreInstalled)
		::DisableMenuItem(m,CountMenuItems(m));
#endif
	AppendMenu(m,userGuideName);
	if (!sUserGuideInstalled)
		::DisableMenuItem(m,CountMenuItems(m));
#endif
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ InstallHelp						Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Makes aliases of the TNT Basic Help and TNT Basic User Guide folders and places them in the system's help folder
void UHelp::InstallHelp()
{
	FSSpec		helpFolder;
	FSSpec		tbHelpCentre;
	FSSpec		tbUserGuide;
	FSSpec		tbHelpCentreAlias;
	FSSpec		tbUserGuideAlias;
	OSErr		err;
	
	ECHOINDENT("Installing help and user guide aliases...");
	
	err=::FindFolder(kOnAppropriateDisk,kHelpFolderType,kCreateFolder,&helpFolder.vRefNum,&helpFolder.parID);
	if (err)
	{
		// think kOnAppropriateDisk fails on 8.6?
		err=::FindFolder(kOnSystemDisk,kHelpFolderType,kCreateFolder,&helpFolder.vRefNum,&helpFolder.parID);
		if (err)
		{
			ECHO("*** ERROR: Can't find help folder error " << err);
			Throw_(err);
		}
	}
	
	LStr255		helpCentreName(kSTRx_HelpStrings,kStr_TBHelpCentreFolderName);
	LStr255		userGuideName(kSTRx_HelpStrings,kStr_TBUserGuideFolderName);
	Boolean		tbHelpCentreExists;
	Boolean		tbUserGuideExists;

	err=::GetAppSpec(&tbHelpCentre);
	if (err)
	{
		ECHO("*** ERROR : Can't get app spec " << err);
		Throw_(err);
	}

	// make specs for help centre and user guide and their aliases
	tbUserGuide=tbHelpCentre;
	LString::CopyPStr(helpCentreName,tbHelpCentre.name);
	LString::CopyPStr(userGuideName,tbUserGuide.name);

	tbUserGuideAlias=tbHelpCentreAlias=helpFolder;	
	LString::CopyPStr(userGuideName,tbUserGuideAlias.name);	
	LString::CopyPStr(helpCentreName,tbHelpCentreAlias.name);	

	// see what exists
	Try_
	{
		ThrowIfOSErr_(FSpSpecExists(&tbUserGuide,&tbUserGuideExists));
		ThrowIfOSErr_(FSpSpecExists(&tbHelpCentre,&tbHelpCentreExists));
		ThrowIfOSErr_(FSpSpecExists(&tbUserGuideAlias,&sUserGuideInstalled));
		ThrowIfOSErr_(FSpSpecExists(&tbHelpCentreAlias,&sHelpCentreInstalled));
	}
	Catch_(err)
	{
		ECHO("*** ERROR : Can't figure out which files exist " << ErrCode_(err));
		throw;
	}

	// if it's installed, verify the alias points to the help in this folder
	if (tbUserGuideExists)
	{
		if (sUserGuideInstalled)
		{
			if (DoesAliasPointToFolder(tbUserGuideAlias,tbUserGuide))
				ECHO("User guide already installed and it checks out");
			else
			{
				ECHO("Wrong user guide installed, removing...");
				
				if (err=::FSpDelete(&tbUserGuideAlias))
					ECHO("Error deleting out of date user guide alias... " << err);
				else
					sUserGuideInstalled=false;
			}
		}
	}

	if (tbHelpCentreExists)
	{
		if (sHelpCentreInstalled)
		{
			if (DoesAliasPointToFolder(tbHelpCentreAlias,tbHelpCentre))
				ECHO("Help centre already installed and it checks out");
			else
			{
				ECHO("Wrong help centre installed, removing...");
				
				if (err=::FSpDelete(&tbHelpCentreAlias))
					ECHO("Error deleting out of date help centre alias... " << err);
				else
					sHelpCentreInstalled=false;
			}
		}
	}
	
	if (!sHelpCentreInstalled)		// is there an alias to the help centre in the folder?
	{
		ECHO("Help centre not installed, installing...");
		
		if (tbHelpCentreExists)
		{			
			// get a ref to TNT Basic Help centre		
			Try_
			{
				tbHelpCentreAlias.name[0]=0;
				MakeAliasFile(tbHelpCentre,tbHelpCentreAlias);
			}
			Catch_(err)
			{
				ECHO("*** ERROR : Can't make alias to help centre " << ErrCode_(err));
				throw;
			}

			sHelpCentreInstalled=true;			
			ECHO("TNT Basic Help centre installed");
		}
		else
			ECHO("Help centre missing, can't install");
	}

	if (!sUserGuideInstalled)		// is there an alias to the help centre in the folder?
	{
		ECHO("User guide not installed, installing...");
		
		if (tbUserGuideExists)
		{			
			// get a ref to TNT Basic Help centre		
			Try_
			{
				tbUserGuideAlias.name[0]=0;
				MakeAliasFile(tbUserGuide,tbUserGuideAlias);
			}
			Catch_(err)
			{
				ECHO("*** ERROR : Can't make alias to user guide " << ErrCode_(err));
				throw;
			}

			sUserGuideInstalled=true;			
			ECHO("TNT Basic User Guide installed");
		}
		else
			ECHO("User Guide missing, can't install");
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ DoesAliasPointToFolder				Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns true if a given alias points to a given folder, defaults to false if it can't resolve the alias
bool DoesAliasPointToFolder(
	FSSpec		&inAliasSpec,
	FSSpec		&inDestSpec)
{
	bool		replaceAlias=false;
	FSSpec		resolvedSpec=inAliasSpec;
	Boolean		wasAliased,wasFolder;
	
	OSErr		err=::ResolveAliasFile(&resolvedSpec,false,&wasFolder,&wasAliased);
	
	if (err)	// didn't work, replace it
		replaceAlias=true;
	else
	{
		if (wasAliased)
		{
			if (!wasFolder)
				replaceAlias=true;
			else
			{
				// is an alias to a folder, is it to our folder?
				if (resolvedSpec.parID==inDestSpec.parID &&
					resolvedSpec.vRefNum==inDestSpec.vRefNum &&
					CmpPStr(resolvedSpec.name,inDestSpec.name))
				{
					// same, already installed - everything is alright
				}
				else
				{
					// different, replace it
					replaceAlias=true;
				}
			}
		}
		else
			;		// if it's not an alias leave it, maybe the use has copied the folder to the alias location directly
	}
	
	return !replaceAlias;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//	¥ MakeAliasFile					Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// this should probably migrate to mr
// makes an alias of target file and places in at the (non existing) spec of newAliasFile. Uses the finder via apple events
void MakeAliasFile(
	const FSSpec		&inTargetFile,
	const FSSpec		&inNewAliasFile)
{
	// make an apple event and send it to the finder telling it to make an alias
	StAEDescriptor		targetSpec(inTargetFile);
	StAEDescriptor		aliasSpec(inNewAliasFile);
	
	OSType				finder='MACS';
	StAEDescriptor		finderAddr(typeApplSignature,&finder,sizeof(OSType));
	
	StAEDescriptor		ae;
	
	// make ae to finder
	ThrowIfOSErr_(::AECreateAppleEvent(kAECoreSuite, kAECreateElement,
										finderAddr,
										kAutoGenerateReturnID,
										kAnyTransactionID,
										ae));

	// add appropriate parameters
	OSType				type=typeAlias;
	StAEDescriptor		typeToMake(typeType,&type,sizeof(typeAlias));
	
	// make new alias at <x> to <x>
	ThrowIfOSErr_(::AEPutParamDesc(ae,keyAEObjectClass,typeToMake));
	ThrowIfOSErr_(::AEPutParamDesc(ae,keyASPrepositionTo,targetSpec));
	ThrowIfOSErr_(::AEPutParamDesc(ae,keyAEInsertHere,aliasSpec));

	// send to the finder
	StAEDescriptor	reply;
	ThrowIfOSErr_(::AESend(ae,reply,kAEWaitReply,kAENormalPriority, kAEDefaultTimeout, nil, nil));
	
	// check the reply for errors
	StAEDescriptor	err(reply,keyErrorNumber,typeSInt32);
	SInt16			errNum=noErr;
	
	if (err.IsNotNull())
		UExtractFromAEDesc::TheSInt16(err,errNum);
	if (errNum)
	{
		StAEDescriptor	str(reply,keyErrorString,typeText);
		Str255			strs;
		UExtractFromAEDesc::ThePString(str,strs);
		ECHO("Error from finder, code " << errNum << " string: " << strs);
		Throw_(errNum);
	}
}
