// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UKaboom.cpp
// © John Treece-Birch 2000
// 24/1/00
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

#include	"UKaboom.h"
#include	"TNT_Debugging.h"
#include	"TNT_Errors.h"
#include	"Utility Objects.h"
#include	"Root.h"
#include	"CResource.h"
#include	"CResourceContainer.h"
#include	"CResourceManager.h"

TResourceReference	UKaboom::sSoundResources[kMaxSounds];
//SndListHandle		UKaboom::sSound[kMaxSounds];
UInt32				UKaboom::sSounds=0;

SInt32				UKaboom::sChannelSound[kChannels];
SInt32				UKaboom::sWasChannelSound[kChannels];

SInt32				UKaboom::sPriority[kChannels];
SInt32				UKaboom::sWasPriority[kChannels];

SInt16				UKaboom::sVolume;

bool				UKaboom::sLooping[kChannels];
bool				UKaboom::sWasLooping[kChannels];

SInt32				UKaboom::sSoundResult;

SndChannelPtr		UKaboom::sChannel[kChannels];
SndCallBackUPP		UKaboom::sCallBack[kChannels];

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Initialise						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Open sound channels and load sounds
void UKaboom::Initialise()
{
	// Initialise the variables
	for (char n=0; n<kChannels; n++)
	{
		sChannelSound[n]=-1;
		sWasChannelSound[n]=-1;
		
		sPriority[n]=-9999999;
		sWasPriority[n]=-9999999;
		
		sLooping[n]=false;
		sWasLooping[n]=false;
		
		sChannel[n]=0L;
		sCallBack[n]=0L;
	}
	
	for (short n=0; n<kMaxSounds; n++)
		sSoundResources[n]=0L;

	sSounds=0;

	sVolume=100;

	// Initialise the call back procs
	sCallBack[0]=NewSndCallBackUPP(UKaboom::CallBackProc0);
	sCallBack[1]=NewSndCallBackUPP(UKaboom::CallBackProc1);
	sCallBack[2]=NewSndCallBackUPP(UKaboom::CallBackProc2);
	sCallBack[3]=NewSndCallBackUPP(UKaboom::CallBackProc3);

	// Open the channels
	for (char n=0; n<kChannels; n++)
	{
		if (!sCallBack[n])
		{
			ECHO("UKaboom::Init - Failed to allocate callback for channel " << int(n));
			Throw_(memFullErr);
		}
		
		OSErr	err=::SndNewChannel(&sChannel[n],0,0,sCallBack[n]);
		
		if (err)
		{
			ECHO("UKaboom::Init - Error initing channel " << int(n));
			Throw_(err);
		}
	}
	
	sSoundResult=-1;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShutDown
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UKaboom::ShutDown()
{
	Try_
	{
		UnloadAllSounds();

		// Dispose the call back procs
		// Dispose the sound channels
		for (char n=0; n<kChannels; n++)
		{
			DisposeSndCallBackUPP(sCallBack[n]);
			sCallBack[n]=NULL;
			if (sChannel[n])
				::SndDisposeChannel(sChannel[n],true);
		}
	}
	Catch_(err)
	{
		ECHO("ERROR Shutting down Kaboom, err code " << ErrCode_(err));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClearSounds
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Releases all the sounds
void UKaboom::ClearSounds()
{
	Try_
	{
		StopAllSounds();		// ensure they're not playing
	}
	Catch_(err)
	{
		ECHO("UKaboom::StopAllSounds error " << ErrCode_(err));
	}
	
	CResourceManager		*rman=CResourceManager::GetManager();
	for (short n=0; n<kMaxSounds; n++)
	{
		if (sSoundResources[n])
		{
			CResource	*res=rman->Resolve(sSoundResources[n]);
			if (!res)
			{
				ECHO("UKaboom::ClearSounds - stale resource reference");
			}
			else
			{
				res->Release();
			}		
			sSoundResources[n]=0L;
		}
	}
	
	sSounds=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadBank							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Open sound channels and load sounds
void UKaboom::LoadBank(
	CResourceContainer	*inResContainer,
	Handle				inHandle)
{
	AssertThrow_(inHandle && IsHandleValid(inHandle));
	
	UHandleLocker	lock(inHandle,true);
		
	Try_
	{
		ClearSounds();
	
		AssertThrow_(IsHandleValid(inHandle));		// we did have a bug where ClearSounds was unloading the SnBk handle, so check it's still valid

		// Load the new sounds
		Ptr		data=*inHandle;
		short	version=CFSwapInt16BigToHost(*(short*)data);
			
		if (version!=0)
			Throw_(kTNTErr_UnsupportedDataFormat);
		
		data+=sizeof(short);

		short	numSounds=CFSwapInt16BigToHost(*(short*)data);
		data+=sizeof(short);
		
		// check sound bank size adds up
		AssertThrow_(::GetHandleSize(inHandle)==(numSounds*sizeof(short)+sizeof(short)+sizeof(short)));

		for (short a=0; a<numSounds; a++)
		{
//			sSound[a]=(SndListHandle)UTBResourceManager::GetResource(*(short*)data,kSoundResType);
			sSoundResources[a]=inResContainer->GetResource(kSoundResType,CFSwapInt16BigToHost(*(short*)data));
			data+=sizeof(short);
							
			sSounds++;
		}			
	}	
	Catch_(err)
	{
		ClearSounds();		
		throw;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PlaySound							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Open sound channels and load sounds
void UKaboom::PlaySound(
	SInt16			inSound,
	char 			inBalance,
	unsigned char 	inVolume,
	SInt32 			inPriority)
{
	sSoundResult=-1;
	
	// Check the sound number is valid
	if ((inSound<0) || (inSound>=sSounds))
		return;
	
	// Check the sound does exist
	if (!sSoundResources[inSound])
		return;
	
	char	n=GetLowestPriority();
	
	inPriority++;
	
	if (inPriority>sPriority[n])
	{	
		// Set values
		sPriority[n]=inPriority;
		sLooping[n]=false;
		sChannelSound[n]=inSound;
	
		// Sort out balance
		if (inBalance<0)
			inBalance=0;
		else if (inBalance>100)
			inBalance=100;
	
		if (inVolume>255)
			inVolume=255;
		else if (inVolume<0)
			inVolume=0;
	
		// Stop the sound if there is one playing
		SndCommand		silence={quietCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&silence));
		
		// Flush any other events
		SndCommand		flush={flushCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&flush));

		// Set the volume of both speakers
		float			volume=((float)sVolume)/100.0f*inVolume;
		
		unsigned char	rightVolume=volume*(double)inBalance/100;
		unsigned char	leftVolume=volume*(100-(double)inBalance)/100;
		
		SndCommand		volumeCommand={volumeCmd,0,0};
		volumeCommand.param2=(long)((long)rightVolume<<16L | leftVolume);
		ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&volumeCommand));
		
		// Play the sound
		ThrowIfOSErr_(::SndPlay(sChannel[n],GetSound(inSound),true));
		
		// Call back when the sound is finished
		SndCommand		callBack={callBackCmd,0,0};
		ThrowIfOSErr_(::SndDoCommand(sChannel[n],&callBack,true));
	
		sSoundResult=n;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoopSound						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Loop the specified sound
void UKaboom::LoopSound(
	SInt16			inSound,
	char			inBalance,
	unsigned char	inVolume)
{
	sSoundResult=-1;
	
	// Check the sound number is valid
	if ((inSound<0) || (inSound>=sSounds))
		return;
	
	// Check the sound does exist
	if (!sSoundResources[inSound])
		return;
	
	char	n=GetLowestPriority();
	
	// Set values
	sPriority[n]=65535;
	sLooping[n]=true;
	sChannelSound[n]=inSound;

	// Sort out balance
	if (inBalance<0)
		inBalance=0;
	else if (inBalance>100)
		inBalance=100;

	if (inVolume>255)
		inVolume=255;
	else if (inVolume<0)
		inVolume=0;
				
	// Flush any other events
	SndCommand		flush={flushCmd,0,0};
	ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&flush));

	// Stop the sound if there is one playing
	SndCommand		silence={quietCmd,0,0};
	ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&silence));

	// Set the volume of both speakers
	float			volume=((float)sVolume)/100.0f*inVolume;
	
	unsigned char	rightVolume=volume*(double)inBalance/100;
	unsigned char	leftVolume=volume*(100-(double)inBalance)/100;
	
	SndCommand		volumeCommand={volumeCmd,0,0};
	volumeCommand.param2=(long)((long)rightVolume<<16L | leftVolume);
	ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&volumeCommand));
	
	// Play the sound
	ThrowIfOSErr_(::SndPlay(sChannel[n],GetSound(inSound),true));
	
	// Call back when the sound is finished
	SndCommand		callBack={callBackCmd,0,0};
	ThrowIfOSErr_(::SndDoCommand(sChannel[n],&callBack,true));
	
	sSoundResult=-1;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StopSound						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UKaboom::StopSound(
	SInt16			inSound)
{
	for (char n=0; n<kChannels; n++)
	{
		if (sChannelSound[n]==inSound)
		{
			sLooping[n]=false;
			sPriority[n]=0;
			sChannelSound[n]=-1;
		
			// Flush any other events
			SndCommand		flush={flushCmd,0,0};
			ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&flush));
		
			// Stop the sound if there is one playing
			SndCommand		silence={quietCmd,0,0};
			ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&silence));
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StopChannel						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UKaboom::StopChannel(
	SInt16			inChannel)
{
	if ((inChannel<0) || (inChannel>kChannels-1))
		return;

	sLooping[inChannel]=false;
	sPriority[inChannel]=0;
	sChannelSound[inChannel]=-1;

	// Flush any other events
	SndCommand		flush={flushCmd,0,0};
	ThrowIfOSErr_(::SndDoImmediate(sChannel[inChannel],&flush));

	// Stop the sound if there is one playing
	SndCommand		silence={quietCmd,0,0};
	ThrowIfOSErr_(::SndDoImmediate(sChannel[inChannel],&silence));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SoundPlaying
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UKaboom::SoundPlaying(
	SInt16			inSound)
{
	for (char n=0; n<kChannels; n++)
	{
		if (sChannelSound[n]==inSound)
			return true;
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ChannelPlaying
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UKaboom::ChannelPlaying(
	SInt16			inChannel)
{
	if ((inChannel<0) || (inChannel>kChannels-1))
		return false;

	if (sChannelSound[inChannel]!=-1)
		return true;
		
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MoveSound
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UKaboom::MoveSound(
	SInt16			inSound,
	char 			inBalance,
	unsigned char 	inVolume)
{
	for (char n=0; n<kChannels; n++)
	{
		if (sChannelSound[n]==inSound)
		{
			if (inBalance>100)
				inBalance=100;
			else if (inBalance<0)
				inBalance=0;
				
			if (inVolume>255)
				inVolume=255;
			else if (inVolume<0)
				inVolume=0;
		
			// Set the volume of both speakers
			unsigned char	rightVolume=inVolume*(double)inBalance/100;
			unsigned char	leftVolume=inVolume*(100-(double)inBalance)/100;
			
			SndCommand		volume={volumeCmd,0,0};
			volume.param2=(long)((long)rightVolume<<16L | leftVolume);
			ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&volume));
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MoveChannel
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UKaboom::MoveChannel(
	SInt16			inChannel,
	char 			inBalance,
	unsigned char 	inVolume)
{
	if ((inChannel<0) || (inChannel>kChannels-1))
		return;

	// Set the volume of both speakers
	if (inBalance>100)
		inBalance=100;
	else if (inBalance<0)
		inBalance=0;
		
	if (inVolume>255)
		inVolume=255;
	else if (inVolume<0)
		inVolume=0;
				
	unsigned char	rightVolume=inVolume*(double)inBalance/100;
	unsigned char	leftVolume=inVolume*(100-(double)inBalance)/100;
	
	SndCommand		volume={volumeCmd,0,0};
	volume.param2=(long)((long)rightVolume<<16L | leftVolume);
	ThrowIfOSErr_(::SndDoImmediate(sChannel[inChannel],&volume));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Reset							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Stops all sounds and unloads all resources
void UKaboom::Reset()
{
	ClearSounds();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Pause							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pauses all sounds
void UKaboom::Pause()
{
	for (char n=0; n<kChannels; n++)
	{
		sWasLooping[n]=sLooping[n];
		sWasPriority[n]=sPriority[n];
		sWasChannelSound[n]=sChannelSound[n];
	}
	
	StopAllSounds();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Resume						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resumes all sounds paused with the pause command
void UKaboom::Resume()
{
	for (char n=0; n<kChannels; n++)
	{
		sLooping[n]=sWasLooping[n];
		sPriority[n]=sWasPriority[n];
		sChannelSound[n]=sWasChannelSound[n];
	
		// Call back all the sounds
		SndCommand		callBack={callBackCmd,0,0};
		ThrowIfOSErr_(::SndDoCommand(sChannel[n],&callBack,true));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StopAllSounds					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Open sound channels and load sounds
void UKaboom::StopAllSounds()
{
	for (char n=0; n<kChannels; n++)
	{
		sLooping[n]=false;
		sPriority[n]=0;
		sChannelSound[n]=-1;
	
		// Flush any other events
		SndCommand		flush={flushCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&flush));
	
		// Stop the sound if there is one playing
		SndCommand		silence={quietCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[n],&silence));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetLowestPriority
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Open sound channels and load sounds
char UKaboom::GetLowestPriority()
{
	char	current=0;

	for (char n=0; n<kChannels; n++)
	{
		if (sPriority[n]<sPriority[current])
			current=n;
	}
	
	return current;
}

#pragma mark -

pascal void UKaboom::CallBackProc0(
	SndChannelPtr 	inChannel,
	SndCommand 		*inCommand)
{
	if (sLooping[0])
	{
		// Flush any other events
		SndCommand		flush={flushCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[0],&flush));
		
		::SndPlay(sChannel[0],GetSound(sChannelSound[0]),true);
		
		// Call back when the sound is finished
		SndCommand		callBack={callBackCmd,0,0};
		ThrowIfOSErr_(::SndDoCommand(sChannel[0],&callBack,true));
	}
	else
	{
		sPriority[0]=0;
		sChannelSound[0]=-1;
	}
}

pascal void UKaboom::CallBackProc1(
	SndChannelPtr 	inChannel,
	SndCommand 		*inCommand)
{
	if (sLooping[1])
	{
		// Flush any other events
		SndCommand		flush={flushCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[1],&flush));
	
		::SndPlay(sChannel[1],GetSound(sChannelSound[1]),true);
		
		// Call back when the sound is finished
		SndCommand		callBack={callBackCmd,0,0};
		ThrowIfOSErr_(::SndDoCommand(sChannel[1],&callBack,true));
	}
	else
	{
		sPriority[1]=0;
		sChannelSound[1]=-1;
	}
}

pascal void UKaboom::CallBackProc2(
	SndChannelPtr 	inChannel,
	SndCommand 		*inCommand)
{
	if (sLooping[2])
	{
		// Flush any other events
		SndCommand		flush={flushCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[2],&flush));
		
		::SndPlay(sChannel[2],GetSound(sChannelSound[2]),true);
		
		// Call back when the sound is finished
		SndCommand		callBack={callBackCmd,0,0};
		ThrowIfOSErr_(::SndDoCommand(sChannel[2],&callBack,true));
	}
	else
	{
		sPriority[2]=0;
		sChannelSound[2]=-1;
	}
}

pascal void UKaboom::CallBackProc3(
	SndChannelPtr 	inChannel,
	SndCommand 		*inCommand)
{
	if (sLooping[3])
	{
		// Flush any other events
		SndCommand		flush={flushCmd,0,0};
		ThrowIfOSErr_(::SndDoImmediate(sChannel[3],&flush));
		
		::SndPlay(sChannel[3],GetSound(sChannelSound[3]),true);
		
		// Call back when the sound is finished
		SndCommand		callBack={callBackCmd,0,0};
		ThrowIfOSErr_(::SndDoCommand(sChannel[3],&callBack,true));
	}
	else
	{
		sPriority[3]=0;
		sChannelSound[3]=-1;
	}
}

// returns the sound handle from the array entry passed
SndListHandle UKaboom::GetSound(
	int			inIndex)
{
	AssertThrow_(inIndex>=0 && inIndex<sSounds);
	CResource	*res=CResourceManager::GetManager()->Resolve(sSoundResources[inIndex]);
	AssertThrow_(res);
	return (SndListHandle)res->GetReadOnlyDataHandle();
}
