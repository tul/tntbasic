/*
 *  PPPrivate.h
 *  PlayerPROCore
 *
 *  Created by C.W. Betts on 6/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __PLAYERPROCORE_PPPRIVATE__
#define __PLAYERPROCORE_PPPRIVATE__

#include "RDriver.h"

#pragma mark Core Audio Functions
OSErr initCoreAudio( MADDriverRec *inMADDriver, long init);
OSErr closeCoreAudio( MADDriverRec *inMADDriver);
void StopChannelCA(MADDriverRec *inMADDriver);
void PlayChannelCA(MADDriverRec *inMADDriver);

CFMutableArrayRef GetDefaultPluginFolderLocations();

void GetPStrFromCFString(const CFStringRef source, StringPtr pStrOut);
void **GetCOMPlugInterface(CFPlugInRef plugToTest, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID);


extern const CFStringRef kMadPlugMenuNameKey;
extern const CFStringRef kMadPlugAuthorNameKey;
extern const CFStringRef kMadPlugUTITypesKey;
extern const CFStringRef kMadPlugModeKey;
extern const CFStringRef kMadPlugTypeKey;

#endif
