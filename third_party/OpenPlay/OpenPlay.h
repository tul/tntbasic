/*
 * Copyright (c) 1999-2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999-2002 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.1 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 * Modified: $Date: 2002/09/07 08:46:51 $
 * Revision: $Id: OpenPlay.h,v 1.1.1.1 2002/09/07 08:46:51 mark Exp $
 */

#ifndef __OPENPLAY__
#define __OPENPLAY__

/*-------------------------------------------------------------------------------------------
	Determine which platform we're compiling for: MacOS, Windows, Posix
*/

	#if defined(__MWERKS__)			/* Metrowerks CodeWarrior Compiler */

		#if defined(__INTEL__)		/* Compiling for x86, assume windows */
			#define windows_build  1
			#define little_endian  1
			#define has_byte_type  1
		#elif defined(__POWERPC__)	/* Compiling for PowerPC, assume MacOS */
			#define macintosh_build  1
			#define mac_cfm_build 1
			#define big_endian	1
			#undef  has_byte_type
		#else
		  #error "unsupported Metrowerks target"
		#endif
	
		#undef  has_unused_pragma	/* unused pragma can't be used in define 8-( */
	
	#elif defined(PROJECT_BUILDER_CARBON) 	/* project builder doing a carbon build as opposed */
		#define macintosh_build 1			/* to a posix build which comes up under POSIX_BUILD below*/
		#define big_endian 1
		
	#elif defined(_MSC_VER)			/* microsoft compiler */
		#define little_endian 1
		#define windows_build  1
		#undef  has_unused_pragma 
		#undef  has_byte_type
		#undef  has_mark_pragma
	
	#elif defined(__MRC__)			/* mpw mrc */
		#define macintosh_build  1
		#define big_endian  1

		#undef  has_unused_pragma 
		#undef  has_byte_type
	
	#elif defined(__WATCOMC__)		/* Watcom, set to fake Microsoft compiler */
		#define little_endian 1			
		#define MICROSOFT
		#define windows_build	1

		#define has_unused_pragma 1
		#define has_byte_type
	
	#elif defined (POSIX_BUILD)		/* Posix -- linux, unix, etc. */
		
		#define posix_build 1

		#ifdef linux
			#include <endian.h>
			#if __BYTE_ORDER == __BIG_ENDIAN
				#define big_endian 1
			#endif
			#if __BYTE_ORDER == __LITTLE_ENDIAN
				#define little_endian 1
			#endif		
		#else
			#ifdef __BIG_ENDIAN__
				#define big_endian 1
			#endif 
			#ifdef __LITTLE_ENDIAN__
				#define little_endian 1
			#endif
		#endif /* linux */
		
		#undef  has_unused_pragma
		#undef  has_byte_type			
	#elif defined(__GNUC__)				// FIXME : should probably upgrade to new openplay
		#define macintosh_build		1
		#if TARGET_RT_BIG_ENDIAN
			#define big_endian	1
		#elif TARGET_RT_LITTLE_ENDIAN
			#define little_endian	1
		#else
			#error "Doh"
		#endif
	#else
	  	#error "Unsupported compiler target"
	#endif
		
	/* make sure we've defined one and only one endian type */
	#if defined (big_endian) && defined (little_endian)
	 	#error "Somehow big and little endian got defined!!"
	#endif
	
	#ifndef big_endian
		#ifndef little_endian
			#error "need to define either big or little endian"
		#endif
	#endif
	
/*-------------------------------------------------------------------------------------------
	basic types
*/

		/* ecf 020104 do any of these vary across platforms?  when 64 bit comes around or whatnot... */
		typedef unsigned char	NMBoolean;

		typedef unsigned long	NMUInt32;
		typedef signed long		NMSInt32;

		typedef signed short	NMSInt16;
		typedef unsigned short	NMUInt16;

		typedef unsigned char	NMUInt8;
		typedef signed char		NMSInt8;

		typedef NMSInt32		NMType;
		typedef NMSInt32    	NMErr;
		typedef NMUInt32		NMFlags;

/*-------------------------------------------------------------------------------------------
	Specify and specific platfrom settings.
*/

	#if PRAGMA_ONCE
	#pragma once
	#endif
	
	#if defined(macintosh_build)

	
		#if TARGET_API_MAC_CARBON
			#define carbon_build 1
		#endif
		
		#if (!macho_build)
			#include <ConditionalMacros.h>
		#endif
		
		#ifndef __MACTYPES__
		#include <MacTypes.h>
		#endif
		#ifndef __EVENTS__
		#include <Events.h>
		#endif
		#ifndef __OPENTRANSPORT__
		#include <OpenTransport.h>
		#endif
		#ifndef __OPENTRANSPORTPROVIDERS__
		#include <OpenTransportProviders.h>
		#endif
	
		#define FATAL_EXIT            exit(-1)
	
		typedef  EventRecord  EVENT;
		typedef  Rect         RECT;
		typedef  DialogPtr    DIALOGPTR;

		#define OPENPLAY_CALLBACK 	void

		#define OP_CALLBACK_API CALLBACK_API
		#define OP_DEFINE_API_C DEFINE_API_C
		
	#elif windows_build
	
		#define WIN32_LEAN_AND_MEAN /* We limit the import of certain libs... */
	
		#include <Windows.h>
	
		#define OPENPLAY_CALLBACK 	void
		#define FATAL_EXIT        	FatalExit(0)
		typedef HWND				DIALOGPTR;

		typedef struct private_event
		{
			HWND        dialog;
			UINT        message;
			WPARAM      wParam;
			LPARAM      lParam;
		} EVENT;

		#ifdef OPENPLAY_DLLEXPORT
			#define OP_DEFINE_API_C(_type) __declspec(dllexport) _type __cdecl
			#define OP_CALLBACK_API(_type, _name)              _type (__cdecl *_name)
		#else
			#define OP_DEFINE_API_C(_type)                     _type __cdecl
			#define OP_CALLBACK_API(_type, _name)              _type (__cdecl *_name)
		#endif /* OPENPLAY_DLLEXPORT */

	#elif defined (posix_build)
	
		#define OPENPLAY_CALLBACK   void
		#define FATAL_EXIT          exit(EXIT_FAILURE)
		typedef  unsigned short  	word;

		#if (OP_POSIX_USE_CARBON_TYPES)
			typedef  EventRecord  EVENT;
			typedef  DialogPtr    DIALOGPTR;	
			typedef  Rect         RECT;			
		#else
			typedef  void*  			DIALOGPTR;
			typedef struct _event
			{
				NMSInt32 unknown;       
			} EVENT;
	
			typedef struct _rect
			{
				NMUInt32 top;
				NMUInt32 left;
				NMUInt32 bottom;
				NMUInt32 right;
			} RECT;
			
		#endif
		
		
		#define OP_DEFINE_API_C(_type) _type
		#define OP_CALLBACK_API(_type, _name) _type (*_name)
	
	#else
		#error unknown build type
	#endif 
	

	/* the basic mac types required if MacTypes.h is not available */
	#ifndef __MACTYPES__


		/* We should get rid of this part!!!!. BEGIN BEGIN BEGIN BEGIN BEGIN BEGIN BEGIN BEGIN BEGIN */

		typedef NMSInt32						OSStatus;
		typedef NMUInt16						InetPort;
		typedef unsigned char 					Str31[32];
		typedef const unsigned char *			ConstStr31Param;
												
		struct NumVersion {
												/* Numeric version part of 'vers' resource */
			NMUInt8 		majorRev;			/* 1st part of version number in BCD */
			NMUInt8 		minorAndBugRev;		/* 2nd & 3rd part of version number share a byte */
			NMUInt8 		stage;				/* stage code: dev, alpha, beta, final */
			NMUInt8 		nonRelRev;			/* revision level of non-released version */
		};

		typedef struct NumVersion	NumVersion;	

		/* We should get rid of this part!!!!. END END END END END END END END END END END END END */

	#endif
		
/*-------------------------------------------------------------------------------------------
	define parameters
*/

	#if defined(has_unused_pragma)
	  #define UNUSED_PARAMETER(x)  #pragma unused(x)
	#else
	  #define UNUSED_PARAMETER(x)  (void) (x);
	#endif
	
	#ifndef has_byte_type
	  #define  BYTE  unsigned char
	#endif
		
/*-------------------------------------------------------------------------------------------
	Define Openplay types and defines
*/
	
	typedef enum
	{
		kNMConnectRequest	= 1,
		kNMDatagramData		= 2,
		kNMStreamData		= 3,
		kNMFlowClear		= 4,
		kNMAcceptComplete	= 5,
		kNMHandoffComplete	= 6,
		kNMEndpointDied		= 7,
		kNMCloseComplete	= 8
	} NMCallbackCode;
	
	/* Special number for max players.
		kNMNoPassiveEndpoints means you can't host on a workstation (e.g. AOL module) */

	enum
	{
		kNMNoEndpointLimit	= 0xFFFFFFFF,
		kNMNoPassiveEndpoints	= 0
	};
	
	enum
	{
		kNMNameLength	= 32,
		kNMCopyrightLen	= 32
	};
	
	typedef NMUInt32 NMHostID;
	
	enum
	{
		kNMModuleHasStream			= 0x00000001,
		kNMModuleHasDatagram		= 0x00000002,
		kNMModuleHasExpedited		= 0x00000004,
		kNMModuleRequiresIdleTime	= 0x00000008
	};
	
	typedef struct NMModuleInfo
	{
		NMUInt32	size;
		NMType		type;
		char		name[kNMNameLength];
		char		copyright[kNMCopyrightLen];
		NMUInt32	maxPacketSize;
		NMUInt32	maxEndpoints;
		NMFlags		flags;
	} NMModuleInfo;
	
	typedef enum
	{
		kNMEnumAdd	= 1,
		kNMEnumDelete,
		kNMEnumClear
	} NMEnumerationCommand;
	
	typedef enum
	{
		kNMModeNone = 0,
		kNMDatagramMode = 1,
		kNMStreamMode = 2,
		kNMNormalMode = kNMStreamMode + kNMDatagramMode
	} NMEndpointMode;
	
	typedef struct NMEnumerationItem
	{
		NMHostID	id;
		char 		*name;
		NMUInt32	customEnumDataLen;
		void		*customEnumData;
	} NMEnumerationItem;
	
	typedef OPENPLAY_CALLBACK (*NMEnumerationCallbackPtr)(
									void 				*	inContext,
									NMEnumerationCommand	inCommand, 
									NMEnumerationItem		*item);
	
	
	/* Flags that a module has to obey */
	enum
	{
		kNMBlocking	= 0x00000001
	};
	
	/* OpenPlay Error Codes */
	enum
	{
		kNMOpenEndpointFailedErr	= -5000,
		kNMOutOfMemoryErr			= -4999,
		kNMParameterErr				= -4998,
		kNMTimeoutErr				= -4997,
		kNMInvalidConfigErr			= -4996,
		kNMNewerVersionErr			= -4995,
		kNMOpenFailedErr			= -4994,
		kNMAcceptFailedErr			= -4993,
		kNMConfigStringTooSmallErr	= -4992,
		kNMResourceErr				= -4991,
		kNMAlreadyEnumeratingErr	= -4990,
		kNMNotEnumeratingErr		= -4989,
		kNMEnumerationFailedErr		= -4988,
		kNMNoDataErr				= -4987,
		kNMProtocolInitFailedErr	= -4985,
		kNMInternalErr				= -4984,
		kNMMoreDataErr				= -4983,
		kNMUnknownPassThrough		= -4982,
		kNMAddressNotFound			= -4981,
		kNMAddressNotValidYet		= -4980,
		kNMWrongModeErr				= -4979,
		kNMBadStateErr				= -4978,
		kNMTooMuchDataErr			= -4977,
		kNMCantBlockErr				= -4976,
		kNMInitFailedErr			= -4975,
		kNMFlowErr					= -4974,
		kNMProtocolErr				= -4793,
		kNMModuleInfoTooSmall		= -4792,
		kNMClassicOnlyModuleErr		= -4791,	
		kNMNoError					= 0
	};
	
	enum
	{
		errBadPacketDefinition		= -1000,
		errBadShortAlignment		= -1001,
		errBadLongAlignment			= -1002,
		errBadPacketDefinitionSize	= -1003,
		errBadVersion				= -1004,
		errParamErr					= -1005,
		errNoMoreNetModules			= -1006,
		errModuleNotFound			= -1007,
		errBadConfig				= -1008,
		errFunctionNotBound			= -1009,
		errNoMemory					= -1010
	};
	
/*	------------------------------	Public Definitions */

	
	#define MAXIMUM_NETMODULES			(64)
	#define NET_MODULE_CREATOR			'OPLY'
	#define CURRENT_OPENPLAY_VERSION	(1)
	
/*	------------------------------	Public Types */

	
	typedef NMSInt16 _packet_data_size;
	
	enum
	{
		k2Byte	= -1,
		k4Byte	= -2
	};
	
	typedef struct protocol_identifier
	{
		NMSInt32	version;
		NMType		type;
		char		name[kNMNameLength];
	} NMProtocol;
	
	typedef struct Endpoint *PEndpointRef;
	
	typedef struct ProtocolConfig *PConfigRef;
	
	typedef OPENPLAY_CALLBACK (*PEndpointCallbackFunction)(
								PEndpointRef	 		inEndpoint, 
								void 					*inContext,
								NMCallbackCode			inCode, 
								NMErr		 			inError, 
								void 					*inCookie);
	
	typedef enum
	{
		kOpenNone	= 0x00,
		kOpenActive	= 0x01 /* open it active (dial for a connection, etc.) */
	} NMOpenFlags;

	/* Module types for OpenPlay Modules */
	enum
	{
		kATModuleType = 'Atlk',
		kIPModuleType = 'Inet'
	};
	
/*	------------------------------	Public Variables */

/*	------------------------------	Public Functions */	
	
/*-------------------------------------------------------------------------------------------
	Define Netsprocket types and defines
*/

	#define kNSpStr32Len 32
		
	enum {
		kNSpMaxPlayerNameLen		= 31,
		kNSpMaxGroupNameLen			= 31,
		kNSpMaxPasswordLen			= 31,
		kNSpMaxGameNameLen			= 31,
		kNSpMaxDefinitionStringLen	= 255
	};
	
	/* NetSprocket basic types */
	typedef struct OpaqueNSpGameReference* 			NSpGameReference;
	typedef struct OpaqueNSpProtocolReference*		NSpProtocolReference;
	typedef struct OpaqueNSpProtocolListReference*  NSpProtocolListReference;
	typedef struct OpaqueNSpAddressReference*		NSpAddressReference;

	typedef NMSInt32 						NSpEventCode;
	typedef NMSInt32 						NSpGameID;
	typedef NMSInt32 						NSpPlayerID;
	typedef NSpPlayerID 					NSpGroupID;
	typedef NMUInt32 						NSpPlayerType;
	typedef NMSInt32 						NSpFlags;
	typedef unsigned char					NSpPlayerName[kNSpStr32Len];

	/* Individual player info */	

	struct NSpPlayerInfo {
		NSpPlayerID 						id;
		NSpPlayerType 						type;
		unsigned char 						name[kNSpStr32Len];
		NMUInt32 							groupCount;
		NSpGroupID 							groups[1];
	};
	typedef struct NSpPlayerInfo NSpPlayerInfo;

	typedef NSpPlayerInfo *					NSpPlayerInfoPtr;

	/* list of all players */
	
	struct NSpPlayerEnumeration {
		NMUInt32 							count;
		NSpPlayerInfoPtr 					playerInfo[1];
	};
	typedef struct NSpPlayerEnumeration		NSpPlayerEnumeration;
	typedef NSpPlayerEnumeration *			NSpPlayerEnumerationPtr;

	/* Individual group info */
	
	struct NSpGroupInfo {
		NSpGroupID 							id;
		NMUInt32 							playerCount;
		NSpPlayerID 						players[1];
	};
	typedef struct NSpGroupInfo				NSpGroupInfo;
	typedef NSpGroupInfo *					NSpGroupInfoPtr;

	/* List of all groups */
	
	struct NSpGroupEnumeration {
		NMUInt32 							count;
		NSpGroupInfoPtr 					groups[1];
	};
	typedef struct NSpGroupEnumeration		NSpGroupEnumeration;
	typedef NSpGroupEnumeration *			NSpGroupEnumerationPtr;

	/* Topology types */
	
	typedef NMUInt32 						NSpTopology;
	enum {
		kNSpClientServer			= 0x00000001
	};
	
	/* Game information */

	struct NSpGameInfo {
		NMUInt32 							maxPlayers;
		NMUInt32 							currentPlayers;
		NMUInt32 							currentGroups;
		NSpTopology 						topology;
		NMUInt32 							reserved;
		unsigned char 						name[kNSpStr32Len];
		unsigned char 						password[kNSpStr32Len];
	};
	typedef struct NSpGameInfo				NSpGameInfo;
	
	/* Structure used for sending and receiving network messages */

	struct NSpMessageHeader {
		NMUInt32 							version;		/* Used by NetSprocket.  Don't touch this */
		NMSInt32 							what;			/* The kind of message (e.g. player joined) */
		NSpPlayerID 						from;			/* ID of the sender */
		NSpPlayerID 						to;				/* (player or group) id of the intended recipient */
		NMUInt32 							id;				/* Unique ID for this message & (from) player */
		NMUInt32 							when;			/* Timestamp for the message */
		NMUInt32 							messageLen;		/* Bytes of data in the entire message (including the header) */
	};
	typedef struct NSpMessageHeader NSpMessageHeader;

	/* NetSprocket-defined message structures */

	struct NSpErrorMessage {
		NSpMessageHeader 				header;
		OSStatus 						error;
	};

	typedef struct NSpErrorMessage		NSpErrorMessage;
	
	struct NSpJoinRequestMessage {
		NSpMessageHeader 				header;
		unsigned char 					name[kNSpStr32Len];
		unsigned char 					password[kNSpStr32Len];
		NMUInt32 						theType;
		NMUInt32 						customDataLen;
		NMUInt8 						customData[1];
	};

	typedef struct NSpJoinRequestMessage	NSpJoinRequestMessage;
	
	struct NSpJoinApprovedMessage {
		NSpMessageHeader 				header;
	};
	typedef struct NSpJoinApprovedMessage	NSpJoinApprovedMessage;
	
	struct NSpJoinDeniedMessage {
		NSpMessageHeader 				header;
		unsigned char 					reason[256];
	};
	typedef struct NSpJoinDeniedMessage		NSpJoinDeniedMessage;
	
	struct NSpPlayerJoinedMessage {
		NSpMessageHeader 				header;
		NMUInt32 						playerCount;
		NSpPlayerInfo 					playerInfo;
	};
	typedef struct NSpPlayerJoinedMessage	NSpPlayerJoinedMessage;
	
	struct NSpPlayerLeftMessage {
		NSpMessageHeader 				header;
		NMUInt32 						playerCount;
		NSpPlayerID 					playerID;
		NSpPlayerName 					playerName;
	};
	typedef struct NSpPlayerLeftMessage		NSpPlayerLeftMessage;
	
	struct NSpHostChangedMessage {
		NSpMessageHeader 				header;
		NSpPlayerID 					newHost;
	};
	typedef struct NSpHostChangedMessage	NSpHostChangedMessage;
	
	struct NSpGameTerminatedMessage {
		NSpMessageHeader 				header;
	};
	typedef struct NSpGameTerminatedMessage	NSpGameTerminatedMessage;
	
	struct NSpCreateGroupMessage {
		NSpMessageHeader 				header;
		NSpGroupID 						groupID;
		NSpPlayerID 					requestingPlayer;
	};
	typedef struct NSpCreateGroupMessage	NSpCreateGroupMessage;
	
	struct NSpDeleteGroupMessage {
		NSpMessageHeader 				header;
		NSpGroupID 						groupID;
		NSpPlayerID 					requestingPlayer;
	};
	typedef struct NSpDeleteGroupMessage	NSpDeleteGroupMessage;
	
	struct NSpAddPlayerToGroupMessage {
		NSpMessageHeader 				header;
		NSpGroupID 						group;
		NSpPlayerID 					player;
	};
	typedef struct NSpAddPlayerToGroupMessage NSpAddPlayerToGroupMessage;
	
	struct NSpRemovePlayerFromGroupMessage {
		NSpMessageHeader 				header;
		NSpGroupID 						group;
		NSpPlayerID 					player;
	};
	typedef struct NSpRemovePlayerFromGroupMessage NSpRemovePlayerFromGroupMessage;
	
	struct NSpPlayerTypeChangedMessage {
		NSpMessageHeader 				header;
		NSpPlayerID 					player;
		NSpPlayerType 					newType;
	};
	typedef struct NSpPlayerTypeChangedMessage NSpPlayerTypeChangedMessage;

	/* Different kinds of messages.  These can NOT be bitwise ORed together */

	enum {
		kNSpSendFlag_Junk			= 0x00100000,					/* will be sent (try once) when there is nothing else pending */
		kNSpSendFlag_Normal			= 0x00200000,					/* will be sent immediately (try once) */
		kNSpSendFlag_Registered		= 0x00400000					/* will be sent immediately (guaranteed, in order) */
	};
	
	
	/* Options for message delivery.  These can be bitwise ORed together with each other,
			as well as with ONE of the above */
	enum {
		kNSpSendFlag_FailIfPipeFull	= 0x00000001,
		kNSpSendFlag_SelfSend		= 0x00000002,
		kNSpSendFlag_Blocking		= 0x00000004
	};
	
	
	/* Options for Hosting Joining, and Deleting games */
	enum {
		kNSpGameFlag_DontAdvertise		= 0x00000001,
		kNSpGameFlag_ForceTerminateGame = 0x00000002
	};
	
	/* Message "what" types */
	/* Apple reserves all negative "what" values (anything with high bit set) */
	enum {
		kNSpSystemMessagePrefix		= (NMSInt32)0x80000000,
		kNSpError					= kNSpSystemMessagePrefix | 0x7FFFFFFF,
		kNSpJoinRequest				= kNSpSystemMessagePrefix | 0x00000001,
		kNSpJoinApproved			= kNSpSystemMessagePrefix | 0x00000002,
		kNSpJoinDenied				= kNSpSystemMessagePrefix | 0x00000003,
		kNSpPlayerJoined			= kNSpSystemMessagePrefix | 0x00000004,
		kNSpPlayerLeft				= kNSpSystemMessagePrefix | 0x00000005,
		kNSpHostChanged				= kNSpSystemMessagePrefix | 0x00000006,
		kNSpGameTerminated			= kNSpSystemMessagePrefix | 0x00000007,
		kNSpGroupCreated			= kNSpSystemMessagePrefix | 0x00000008,
		kNSpGroupDeleted			= kNSpSystemMessagePrefix | 0x00000009,
		kNSpPlayerAddedToGroup		= kNSpSystemMessagePrefix | 0x0000000A,
		kNSpPlayerRemovedFromGroup	= kNSpSystemMessagePrefix | 0x0000000B,
		kNSpPlayerTypeChanged		= kNSpSystemMessagePrefix | 0x0000000C
	};
	
	
	/* Special TPlayerIDs  for sending messages */
	enum {
		kNSpAllPlayers				= 0x00000000,
		kNSpHostOnly				= (NMSInt32)0xFFFFFFFF
	};
	
	
	#ifndef __MACTYPES__
	
		/* NetSprocket Error Codes */
		enum {
			kNSpInitializationFailedErr	= -30360,
			kNSpAlreadyInitializedErr	= -30361,
			kNSpTopologyNotSupportedErr	= -30362,
			kNSpPipeFullErr				= -30364,
			kNSpHostFailedErr			= -30365,
			kNSpProtocolNotAvailableErr	= -30366,
			kNSpInvalidGameRefErr		= -30367,
			kNSpInvalidParameterErr		= -30369,
			kNSpOTNotPresentErr			= -30370,
			kNSpOTVersionTooOldErr		= -30371,
			kNSpMemAllocationErr		= -30373,
			kNSpAlreadyAdvertisingErr	= -30374,
			kNSpNotAdvertisingErr		= -30376,
			kNSpInvalidAddressErr		= -30377,
			kNSpFreeQExhaustedErr		= -30378,
			kNSpRemovePlayerFailedErr	= -30379,
			kNSpAddressInUseErr			= -30380,
			kNSpFeatureNotImplementedErr = -30381,
			kNSpNameRequiredErr			= -30382,
			kNSpInvalidPlayerIDErr		= -30383,
			kNSpInvalidGroupIDErr		= -30384,
			kNSpNoPlayersErr			= -30385,
			kNSpNoGroupsErr				= -30386,
			kNSpNoHostVolunteersErr		= -30387,
			kNSpCreateGroupFailedErr	= -30388,
			kNSpAddPlayerFailedErr		= -30389,
			kNSpInvalidDefinitionErr	= -30390,
			kNSpInvalidProtocolRefErr	= -30391,
			kNSpInvalidProtocolListErr	= -30392,
			kNSpTimeoutErr				= -30393,
			kNSpGameTerminatedErr		= -30394,
			kNSpConnectFailedErr		= -30395,
			kNSpSendFailedErr			= -30396,
			kNSpMessageTooBigErr		= -30397,
			kNSpCantBlockErr			= -30398,
			kNSpJoinFailedErr			= -30399
		};
		
	#endif	/*	__MACTYPES__	*/
	
	
#ifdef __cplusplus
extern "C" {
#endif

	#if PRAGMA_IMPORT
	#pragma import on
	#endif

	#if PRAGMA_STRUCT_ALIGN
		#pragma options align=power
	#elif PRAGMA_STRUCT_PACKPUSH
		#pragma pack(push, 2)
	#elif PRAGMA_STRUCT_PACK
		#pragma pack(2)
	#endif

	
	
/*-------------------------------------------------------------------------------------------
	OPENPLAY API
*/
	/* ----------- Protocol Management Layer */
		
		OP_DEFINE_API_C(NMErr) 
		GetIndexedProtocol			(		NMSInt16 		index, 
											NMProtocol *	protocol);
		
	/* ----------- config functions */
	
		OP_DEFINE_API_C(NMErr) 
		ProtocolCreateConfig			(	NMType 			type, 
											NMType 			game_id, 
											const char *	game_name, 
											const void *	enum_data, 
											NMUInt32 		enum_data_len, 
											char *			configuration_string, 
											PConfigRef *	inConfig);
													
		OP_DEFINE_API_C(NMErr)
		ProtocolDisposeConfig			(	PConfigRef config);

		OP_DEFINE_API_C(NMType) 
		ProtocolGetConfigType			(	PConfigRef config );	//LR :why parse config string when we save the type?

		OP_DEFINE_API_C(NMErr) 
		ProtocolGetConfigString			(	PConfigRef config, 
											char *config_string, 
											NMSInt16 max_length);
									
		OP_DEFINE_API_C(NMErr) 
		ProtocolGetConfigStringLen		(	PConfigRef		config, 
											NMSInt16 *		length);
										
		OP_DEFINE_API_C(NMErr) 
		ProtocolBindEnumerationToConfig	(	PConfigRef 	config, 
											NMHostID 	inID);
											
		OP_DEFINE_API_C(NMErr) 
		ProtocolConfigPassThrough		(	PConfigRef 	config, 
											NMUInt32 inSelector, 
											void *inParamBlock);
	
	/* ----------- enumeration functions */
	
		OP_DEFINE_API_C(NMErr) 
		ProtocolStartEnumeration		(	PConfigRef 					config, 
											NMEnumerationCallbackPtr	inCallback,
											void *						inContext,
											NMBoolean 					inActive);
										
		OP_DEFINE_API_C(NMErr) 
		ProtocolIdleEnumeration			(	PConfigRef config);

		OP_DEFINE_API_C(NMErr)
		ProtocolEndEnumeration			(	PConfigRef config);
		
		OP_DEFINE_API_C(void) 
		ProtocolStartAdvertising		(	PEndpointRef endpoint);	// [Edmark/PBE] 11/8/99 changed parameter from PConfigRef to PEndpointRef

		OP_DEFINE_API_C(void) 
		ProtocolStopAdvertising			(	PEndpointRef endpoint);		// [Edmark/PBE] 11/8/99 changed parameter from PConfigRef to PEndpointRef
	
	/* ---------- opening/closing */
	
		OP_DEFINE_API_C(NMErr) 
		ProtocolOpenEndpoint			(	PConfigRef inConfig,
											PEndpointCallbackFunction inCallback,
											void *inContext, 
											PEndpointRef *outEndpoint, 
											NMOpenFlags flags);
									
		OP_DEFINE_API_C(NMErr) 
		ProtocolCloseEndpoint			(	PEndpointRef endpoint, 
											NMBoolean inOrderly);
	
	/* ----------- options */
	
		OP_DEFINE_API_C(NMErr) 
		ProtocolSetTimeout				(	PEndpointRef endpoint, 
											NMSInt32 timeout);
								
		OP_DEFINE_API_C(NMBoolean) 
		ProtocolIsAlive					(	PEndpointRef endpoint);
		
		OP_DEFINE_API_C(NMErr) 
		ProtocolIdle					(	PEndpointRef endpoint);
		
		OP_DEFINE_API_C(NMErr) 
		ProtocolFunctionPassThrough		(	PEndpointRef endpoint, 
											NMUInt32 inSelector,
											void *inParamBlock);
										
		OP_DEFINE_API_C(NMErr) 
		ProtocolSetEndpointContext		(	PEndpointRef endpoint, 
											void *newContext);	/* [Edmark/PBE] 11/10/99 added */
	
	/* ----------- connections */
	
		OP_DEFINE_API_C(NMErr) 
		ProtocolAcceptConnection		(	PEndpointRef endpoint, 
											void *inCookie, 
											PEndpointCallbackFunction inNewCallback,
											void *inNewContext);
		
		OP_DEFINE_API_C(NMErr) 
		ProtocolRejectConnection		(	PEndpointRef endpoint, 
											void *inCookie);
	
	/* ----------- sending/receiving */	
	
		OP_DEFINE_API_C(NMErr) 
		ProtocolSendPacket				(	PEndpointRef endpoint, 
											void *inData, 
											NMUInt32 inLength, 
											NMFlags inFlags);
										
		OP_DEFINE_API_C(NMErr) 
		ProtocolReceivePacket			(	PEndpointRef endpoint, 
											void *outData, 
											NMUInt32 *ioSize, 
											NMFlags *outFlags);
	
	/* ------------ entering/leaving notifiers */

		OP_DEFINE_API_C(NMErr) 
		ProtocolEnterNotifier			(	PEndpointRef endpoint,
											NMEndpointMode endpointMode);

		OP_DEFINE_API_C(NMErr)
		ProtocolLeaveNotifier			(	PEndpointRef endpoint,
											NMEndpointMode endpointMode);


	/* ----------- streaming */
	
		OP_DEFINE_API_C(NMSInt32) 
		ProtocolSend					(	PEndpointRef endpoint, 
											void *inData, 
											NMUInt32 inSize, 
											NMFlags inFlags);
												
		OP_DEFINE_API_C(NMErr) 
		ProtocolReceive					(	PEndpointRef endpoint, 
											void *outData, 
											NMUInt32 *ioSize, 
											NMFlags *outFlags);
	
	/* ----------- information functions */
	
		OP_DEFINE_API_C(NMErr) 
		ProtocolGetEndpointInfo			(	PEndpointRef endpoint, 
											NMModuleInfo *info);
	
	
	/* ----------- prototypes */
	
		OP_DEFINE_API_C(NMErr)
		ValidateCrossPlatformPacket		(	_packet_data_size *		packet_definition, 
											NMSInt16 				packet_definition_length, 
											NMSInt16 				packet_length);
		
		OP_DEFINE_API_C(NMErr)
		SwapCrossPlatformPacket			(	_packet_data_size *	packet_definition, 
											NMSInt16 			packet_definition_length, 
											void *				packet_data, 
											NMSInt16 			packet_length);
	
	
	/* ----------- dialog functions */
		OP_DEFINE_API_C(NMErr) 
		ProtocolSetupDialog				(	DIALOGPTR dialog, 
											NMSInt16 frame, 
											NMSInt16 inBaseItem, 
											PConfigRef config);
								
		OP_DEFINE_API_C(NMBoolean) 
		ProtocolHandleEvent				(	DIALOGPTR 	dialog,
											EVENT *		event,
											PConfigRef	config);
								
		OP_DEFINE_API_C(NMErr) 
		ProtocolHandleItemHit			(	DIALOGPTR 	dialog, 
											NMSInt16	inItemHit,
											PConfigRef	config);
									
		OP_DEFINE_API_C(NMBoolean) 
		ProtocolDialogTeardown			(	DIALOGPTR 	dialog, 
											NMBoolean 	update_config,
											PConfigRef 	config);
									
		OP_DEFINE_API_C(void) 
		ProtocolGetRequiredDialogFrame	(	RECT *		r, 
											PConfigRef	config);
	
	
/*-------------------------------------------------------------------------------------------
	NETSPROCKET API
*/
	
	/************************  Initialization  ************************/
	OP_DEFINE_API_C( OSStatus )
	NSpInitialize					(NMUInt32 				inStandardMessageSize,
									 NMUInt32 				inBufferSize,
									 NMUInt32 				inQElements,
									 NSpGameID 				inGameID,
									 NMUInt32 				inTimeout);
	
	
	/**************************  Protocols  **************************/
	/* Programmatic protocol routines */
	OP_DEFINE_API_C( OSStatus )
	NSpProtocol_New					(const char *			inDefinitionString,
									 NSpProtocolReference *	outReference);
	
	OP_DEFINE_API_C( void )
	NSpProtocol_Dispose				(NSpProtocolReference 	inProtocolRef);
	
	OP_DEFINE_API_C( OSStatus )
	NSpProtocol_ExtractDefinitionString (NSpProtocolReference  inProtocolRef,
										 char *					outDefinitionString);
	
	
	/* Protocol list routines */
	OP_DEFINE_API_C( OSStatus )
	NSpProtocolList_New				(NSpProtocolReference 	inProtocolRef,
									 NSpProtocolListReference * outList);
	
	OP_DEFINE_API_C( void )
	NSpProtocolList_Dispose			(NSpProtocolListReference  inProtocolList);
	
	OP_DEFINE_API_C( OSStatus )
	NSpProtocolList_Append			(NSpProtocolListReference  inProtocolList,
									 NSpProtocolReference 	inProtocolRef);
	
	OP_DEFINE_API_C( OSStatus )
	NSpProtocolList_Remove			(NSpProtocolListReference  inProtocolList,
									 NSpProtocolReference 	inProtocolRef);
	
	OP_DEFINE_API_C( OSStatus )
	NSpProtocolList_RemoveIndexed	(NSpProtocolListReference  inProtocolList,
									 NMUInt32 				inIndex);
	
	OP_DEFINE_API_C( NMUInt32 )
	NSpProtocolList_GetCount		(NSpProtocolListReference  inProtocolList);
	
	OP_DEFINE_API_C( NSpProtocolReference )
	NSpProtocolList_GetIndexedRef	(NSpProtocolListReference  inProtocolList,
									 NMUInt32 				inIndex);
	
	
	/* Helpers */
	OP_DEFINE_API_C( NSpProtocolReference )
	NSpProtocol_CreateAppleTalk		(const unsigned char 	inNBPName[kNSpStr32Len],
									 const unsigned char 	inNBPType[kNSpStr32Len],
									 NMUInt32 				inMaxRTT,
									 NMUInt32 				inMinThruput);
	
	OP_DEFINE_API_C( NSpProtocolReference )
	NSpProtocol_CreateIP			(NMUInt16 				inPort,
									 NMUInt32 				inMaxRTT,
									 NMUInt32 				inMinThruput);
	
	
	/***********************  Human Interface  ************************/
	
	#if (macintosh_build)
		typedef OP_CALLBACK_API( NMBoolean , NSpEventProcPtr )(EventRecord *inEvent);
	#else
		/*dummy function - pass NULL*/
		typedef OP_CALLBACK_API( NMBoolean , NSpEventProcPtr )(void *event);		
	#endif
	
	OP_DEFINE_API_C( NSpAddressReference )
	NSpDoModalJoinDialog			(const unsigned char 	inGameType[kNSpStr32Len],
									 const unsigned char 	inEntityListLabel[256],
									 unsigned char 			ioName[kNSpStr32Len],
									 unsigned char 			ioPassword[kNSpStr32Len],
									 NSpEventProcPtr 		inEventProcPtr);
	
	OP_DEFINE_API_C( NMBoolean )
	NSpDoModalHostDialog			(NSpProtocolListReference	ioProtocolList,
									 unsigned char 				ioGameName[kNSpStr32Len],
									 unsigned char 				ioPlayerName[kNSpStr32Len],
									 unsigned char 				ioPassword[kNSpStr32Len],
									 NSpEventProcPtr 			inEventProcPtr);
		
	/*********************  Hosting and Joining  **********************/
	OP_DEFINE_API_C( OSStatus )
	NSpGame_Host					(NSpGameReference *		outGame,
									 NSpProtocolListReference  inProtocolList,
									 NMUInt32 				inMaxPlayers,
									 const unsigned char 	inGameName[kNSpStr32Len],
									 const unsigned char 	inPassword[kNSpStr32Len],
									 const unsigned char 	inPlayerName[kNSpStr32Len],
									 NSpPlayerType 			inPlayerType,
									 NSpTopology 			inTopology,
									 NSpFlags 				inFlags);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGame_Join					(NSpGameReference *		outGame,
									 NSpAddressReference 	inAddress,
									 const unsigned char 	inName[kNSpStr32Len],
									 const unsigned char 	inPassword[kNSpStr32Len],
									 NSpPlayerType 			inType,
									 void *					inCustomData,
									 NMUInt32 				inCustomDataLen,
									 NSpFlags 				inFlags);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGame_EnableAdvertising		(NSpGameReference 		inGame,
									 NSpProtocolReference 	inProtocol,
									 NMBoolean 				inEnable);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGame_Dispose					(NSpGameReference 		inGame,
									 NSpFlags 				inFlags);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGame_GetInfo					(NSpGameReference 		inGame,
									 NSpGameInfo *			ioInfo);
	
	/**************************  Messaging  **************************/
	OP_DEFINE_API_C( OSStatus )
	NSpMessage_Send					(NSpGameReference 		inGame,
									 NSpMessageHeader *		inMessage,
									 NSpFlags 				inFlags);
	
	OP_DEFINE_API_C( NSpMessageHeader *)
	NSpMessage_Get					(NSpGameReference 		inGame);
	
	OP_DEFINE_API_C( void )
	NSpMessage_Release				(NSpGameReference 		inGame,
									 NSpMessageHeader *		inMessage);
	
	/* Helpers */
	OP_DEFINE_API_C( OSStatus )
	NSpMessage_SendTo				(NSpGameReference 		inGame,
									 NSpPlayerID 			inTo,
									 NMSInt32 				inWhat,
									 void *					inData,
									 NMUInt32 				inDataLen,
									 NSpFlags 				inFlags);
	
	
	/*********************  Player Information  **********************/
	OP_DEFINE_API_C( OSStatus )
	NSpPlayer_ChangeType			(NSpGameReference 		inGame,
									 NSpPlayerID 			inPlayerID,
									 NSpPlayerType 			inNewType);
	
	OP_DEFINE_API_C( OSStatus )
	NSpPlayer_Remove				(NSpGameReference 		inGame,
									 NSpPlayerID 			inPlayerID);
	
	
	OP_DEFINE_API_C( NSpPlayerID )
	NSpPlayer_GetMyID				(NSpGameReference 		inGame);
	
	OP_DEFINE_API_C( OSStatus )
	NSpPlayer_GetInfo				(NSpGameReference 		inGame,
									 NSpPlayerID 			inPlayerID,
									 NSpPlayerInfoPtr *		outInfo);
	
	OP_DEFINE_API_C( void )
	NSpPlayer_ReleaseInfo			(NSpGameReference 		inGame,
									 NSpPlayerInfoPtr 		inInfo);
	
	OP_DEFINE_API_C( OSStatus )
	NSpPlayer_GetEnumeration		(NSpGameReference 		inGame,
									 NSpPlayerEnumerationPtr * outPlayers);
	
	OP_DEFINE_API_C( void )
	NSpPlayer_ReleaseEnumeration	(NSpGameReference 		inGame,
									 NSpPlayerEnumerationPtr  inPlayers);
	
	OP_DEFINE_API_C( NMUInt32 )
	NSpPlayer_GetRoundTripTime		(NSpGameReference 		inGame,
									 NSpPlayerID 			inPlayer);
	
	OP_DEFINE_API_C( NMUInt32 )
	NSpPlayer_GetThruput			(NSpGameReference 		inGame,
									 NSpPlayerID 			inPlayer);
	
	
	/*********************  Group Management  **********************/
	OP_DEFINE_API_C( OSStatus )
	NSpGroup_New					(NSpGameReference 		inGame,
									 NSpGroupID *			outGroupID);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGroup_Dispose				(NSpGameReference 		inGame,
									 NSpGroupID 			inGroupID);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGroup_AddPlayer				(NSpGameReference 		inGame,
									 NSpGroupID 			inGroupID,
									 NSpPlayerID 			inPlayerID);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGroup_RemovePlayer			(NSpGameReference 		inGame,
									 NSpGroupID 			inGroupID,
									 NSpPlayerID 			inPlayerID);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGroup_GetInfo				(NSpGameReference 		inGame,
									 NSpGroupID 			inGroupID,
									 NSpGroupInfoPtr *		outInfo);
	
	OP_DEFINE_API_C( void )
	NSpGroup_ReleaseInfo			(NSpGameReference 		inGame,
									 NSpGroupInfoPtr 		inInfo);
	
	OP_DEFINE_API_C( OSStatus )
	NSpGroup_GetEnumeration			(NSpGameReference 		inGame,
									 NSpGroupEnumerationPtr * outGroups);
	
	OP_DEFINE_API_C( void )
	NSpGroup_ReleaseEnumeration		(NSpGameReference 		inGame,
									 NSpGroupEnumerationPtr  inGroups);
	
	
	/**************************  Utilities  ***************************/
	OP_DEFINE_API_C( NumVersion )
	NSpGetVersion					(void);
	
	OP_DEFINE_API_C( void )
	NSpSetConnectTimeout			(NMUInt32 				inSeconds);
	
	OP_DEFINE_API_C( void )
	NSpClearMessageHeader			(NSpMessageHeader *		inMessage);
	
	OP_DEFINE_API_C( NMUInt32 )
	NSpGetCurrentTimeStamp			(NSpGameReference 		inGame);
	
	OP_DEFINE_API_C( NSpAddressReference )
	NSpCreateATlkAddressReference	(	char *theName, 
										char *theType, 
										char *theZone);
	
	OP_DEFINE_API_C( NSpAddressReference )
	NSpCreateIPAddressReference		(	char *	inIPAddress, 
										char *	inIPPort);
	
	OP_DEFINE_API_C( void )
	NSpReleaseAddressReference		(NSpAddressReference 	inAddress);
	
	OP_DEFINE_API_C( OSStatus )
	NSpPlayer_GetIPAddress			(NSpGameReference 		inGame,
									 NSpPlayerID 			inPlayerID,
									 char *					outAddress);
	
	
	/************************ Mac-CFM-Versions-Only routines ****************/
	#if mac_cfm_build
		OP_DEFINE_API_C( OSStatus )
		NSpPlayer_GetAddress			(NSpGameReference 		inGame,
										 NSpPlayerID 			inPlayerID,
										 OTAddress **			outAddress);
		
		OP_DEFINE_API_C( NSpAddressReference )
		NSpConvertOTAddrToAddressReference (OTAddress *			inAddress);
		
		OP_DEFINE_API_C( OTAddress *)
		NSpConvertAddressReferenceToOTAddr (NSpAddressReference  inAddress);
		
		OP_DEFINE_API_C( void )
		NSpReleaseOTAddress	(OTAddress 		*inAddress);
		
	#endif	/*	mac_cfm_build	*/
	
	/************************ Advanced/Async routines ****************/
	typedef OP_CALLBACK_API( void , NSpCallbackProcPtr )(	NSpGameReference inGame, 
														void *inContext, 
														NSpEventCode inCode, 
														OSStatus inStatus, 
														void *inCookie);
	
	OP_DEFINE_API_C( OSStatus )
	NSpInstallCallbackHandler		(NSpCallbackProcPtr 	inHandler,
									 void *					inContext);
	
	
	typedef OP_CALLBACK_API( NMBoolean , NSpJoinRequestHandlerProcPtr )(	NSpGameReference 		inGame, 
																		NSpJoinRequestMessage *	inMessage, 
																		void *					inContext, 
																		unsigned char *			outReason);
	
	OP_DEFINE_API_C( OSStatus )
	NSpInstallJoinRequestHandler	(NSpJoinRequestHandlerProcPtr  inHandler,
									 void *							inContext);
	
	
	typedef OP_CALLBACK_API( NMBoolean , NSpMessageHandlerProcPtr )(	NSpGameReference 	inGame, 
																	NSpMessageHeader *	inMessage, 
																	void *				inContext);
	
	OP_DEFINE_API_C( OSStatus )
	NSpInstallAsyncMessageHandler	(NSpMessageHandlerProcPtr  inHandler,
									 void *						inContext);


	#if PRAGMA_STRUCT_ALIGN
		#pragma options align=reset
	#elif PRAGMA_STRUCT_PACKPUSH
		#pragma pack(pop)
	#elif PRAGMA_STRUCT_PACK
		#pragma pack()
	#endif

	#ifdef PRAGMA_IMPORT_OFF
		#pragma import off
	#elif PRAGMA_IMPORT
		#pragma import reset
	#endif

#ifdef __cplusplus
}
#endif

#endif /*	__OPENPLAY__	*/
