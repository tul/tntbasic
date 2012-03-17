// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UKaboom.h
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

#pragma once

#include	<sound.h>

#include	"ResourceTypes.h"

class CResourceContainer;

class UKaboom
{
	private:
		static pascal void 			CallBackProc0(
										SndChannelPtr	inChannel,
										SndCommand		*inCommand);

		static pascal void 			CallBackProc1(
										SndChannelPtr	inChannel,
										SndCommand		*inCommand);

		static pascal void 			CallBackProc2(
										SndChannelPtr	inChannel,
										SndCommand		*inCommand);

		static pascal void 			CallBackProc3(
										SndChannelPtr	inChannel,
										SndCommand		*inCommand);

	protected:
		static const short			kChannels=4;
		static const short			kMaxSounds=500;
		
		static TResourceReference	sSoundResources[kMaxSounds];
		static UInt32				sSounds;
		
		static SInt32				sChannelSound[kChannels];
		static SInt32				sWasChannelSound[kChannels];
		
		static SInt32				sPriority[kChannels];
		static SInt32				sWasPriority[kChannels];
		
		static SInt32				sSoundResult;
		
		static SInt16				sVolume;
		
		static bool					sLooping[kChannels];
		static bool					sWasLooping[kChannels];
		
		static SndChannelPtr		sChannel[kChannels];
		static SndCallBackUPP		sCallBack[kChannels];

		static char					GetLowestPriority();
		static void					ClearSounds();
		
		static SndListHandle /*e*/	GetSound(
										int			inIndex);
	
	public:
		static const ResType		kSoundResType='snd ';
		static const ResType		kSoundBankResType='SnBk';
		static const SInt16			kSoundBankVersion=0;

		static void /*e*/			Initialise();
		static void					ShutDown();
		
		static void /*e*/			LoadBank(
										CResourceContainer	*inResC,
										Handle				inBank);
		static void 				UnloadAllSounds()	{ ClearSounds(); }
		
		static void /*e*/			PlaySound(
										SInt16			inSound,
										char			inBalance,
										unsigned char	inVolume,
										SInt32			inPriority);
	
		static void /*e*/			LoopSound(
										SInt16			inSound,
										char			inBalance,
										unsigned char	inVolume);

		static void /*e*/			StopSound(
										SInt16			inSound);
										
		static void /*e*/			StopChannel(
										SInt16			inChannel);
										
		static bool					SoundPlaying(
										SInt16			inSound);
										
		static bool					ChannelPlaying(
										SInt16			inChannel);
										
		static void					MoveSound(
										SInt16			inSound,
										char 			inBalance,
										unsigned char 	inVolume);

		static void					MoveChannel(
										SInt16			inChannel,
										char 			inBalance,
										unsigned char 	inVolume);
										
		static SInt16				GetVolume()							{ return sVolume; }
		static void					SetVolume(
										SInt16			inVolume)
									{
										if (inVolume>100)
											inVolume=100;
										else if (inVolume<0)
											inVolume=0;
											
										sVolume=inVolume;
									}
									
		static SInt32				SoundResult()						{ return sSoundResult; }
										
		static void /*e*/			StopAllSounds();
		static void /*e*/			Pause();
		static void /*e*/			Resume();
		static void /*e*/			Reset();
};