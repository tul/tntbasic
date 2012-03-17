// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// TNTBasic_Resources.h
// © Mark Tully 2000
// 8/6/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

#pragma once

#include	<PP_Types.h>

static const ResType		kTNTBasic_CodeResType='Basc';
static const ResIDT			kTNTBasic_CodeResId=128;

static const ResType		kTNTBasic_DataFilePathResType='DFIL';
static const ResIDT			kTNTBasic_DataFilePathResId=128;

static const ResType		kTNTBasic_EncryptedCodeResType='EncB';
static const ResIDT			kTNTBasic_EncryptedCodeResId=128;

static const ResType		kTNTBasic_ProgramInfoResType='Prog';
static const ResIDT			kTNTBasic_ProgramInfoResId=128;

static const ResIDT			kSTR_DefaultProgName=128;
static const ResIDT			kSTR_BuildFailedUseResAbove128=129;

static const ResIDT			kFREF_DataFile=130;
static const ResIDT			kBNDL_MainResource=128;

static const ResIDT			kTEXT_PlstTemplate=129;

static const ResIDT			kSnd_SnapShot=127;

static const ResIDT			kPICT_FPSFont=126;
static const ResIDT			kPICT_AboutBox=127;

static const ResIDT			kText_About=128;

static const ResIDT			kALRT_Error=136;
static const ResIDT			kALRT_WelcomeToTBOSX=137;
static const ResIDT			kALRT_FriendlyReminder=138;
static const ResIDT			kALRT_WelcomeToTB=139;
static const ResIDT			kALRT_ThanksForRegistering=140;
static const ResIDT			kALRT_WrongCode=141;

static const OSType			kTBCreatorCode='TNTb';

static const ResIDT			kPPob_RegBox=135;

static const ResIDT			kSTRx_RegStrings=129;
enum
{
	kStr_RegTo1=1,			// String registered to <name> <code> is made from
	kStr_RegTo2,			// kStr_RegTo1 + <name> + kStr_RegTo2 + <code> + kStr_RegTo3
	kStr_RegTo3,
	kStr_Unregistered
};

// see uhelp for the defs of these, in uhelp so that they can be used easily by tbb
/*static const ResIDT			kSTRx_HelpStrings=150;
enum
{
	kStr_TBHelpCentreFolderName=1,
	kStr_TBUserGuideFolderName
};*/