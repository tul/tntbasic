
//
//	RLCocoaDisplay.h
//
//  © Ian R Ollmann (iano@cco.caltech.edu)	2000
//		All Rights Reserved
//
//	Please see accompanying documentation for duplication, distribution and licensing terms.
//
//

// Modified by Mark Tully 11/6/06 to work directly with Mach rather than doing so indirectly via function ptrs.

#include "RLDisplayMachO.h"

#ifndef __CGDIRECTDISPLAY__
	#include "CGDirectDisplay.h"
#endif
#include <Multiprocessing.h>

Rect *GetCocoaDisplayList( unsigned long *listEntryCount );
void DisposeCocoaDisplayList( Rect *list );

const int kDefaultVBLPriority = 20;


//These are private child classes of the RLDisplay virtual class. These define the actual
//environment dependent implementation under the Cocao and Classic programming environments.
class CocoaDisplay : public RLDisplay
{
	public:
		virtual	void			DisposeResolutionList( RLInfo *list ) const;
		virtual OSStatus		GetCurrentDrawingInfo( RLDrawInfo *info ) const;	
		virtual OSStatus		GetCurrentScreenSetting( RLInfo *info ) const;
		virtual UInt32			GetColorDepthsAtThisResolution( void ) const;
		virtual OSStatus		GetGamma( RLGammaTable *gamma ) const;
		virtual RLInfo			*GetResolutionList( unsigned long *listCount ) const;
		virtual OSStatus		SaveIconPositions( Handle *desktopInfo ) const;

		virtual OSStatus		AcquireDesktop( WindowRef *window, const RGBColor *background );
		virtual OSStatus		ReleaseDesktop( void );
		virtual OSStatus		SetColorDepth( UInt32 newColorDepth );
		virtual OSStatus		SetGamma( const RLGammaTable *gamma );
		virtual OSStatus		SetResolution( long opaqueModeID );
		virtual OSStatus		SetResolutionAndColorDepth( long opaqueModeID, UInt32 newColorDepth ); 
		virtual OSStatus		RestoreIconPositions( Handle desktopInfo ) const;

		virtual OSStatus		WaitForVerticalRetrace( void );
		virtual RetraceProc 	*InstallVerticalRetraceProcedure( VerticalRetraceProcPtr proc, long userData );
		virtual RetraceProc 	*EmulateVBL( UInt32 refresh, VerticalRetraceProcPtr proc, long userData );
		virtual OSStatus		DisposeVerticalRetraceProcedure( RetraceProc *proc );

		virtual GDHandle		GetDisplay( OSStatus *error ) const;
		
								CocoaDisplay( const Rect *where ) throw (OSStatus );
		virtual 				~CocoaDisplay();

	protected:
		CGDirectDisplayID		display;
		WindowRef				blankingWindow;
		Ptr						desktopRestoreContext;
		
		virtual OSStatus		SwitchMode( const CFDictionaryRef newMode );
};

class CocoaRetraceProc  : public RetraceProc
{
	public:
		
							CocoaRetraceProc(  RLDisplay *the_owner, UInt32 refresh, CGDirectDisplayID	display, VerticalRetraceProcPtr proc, long userData ) throw (OSStatus);
							CocoaRetraceProc(  RLDisplay *the_owner, CGDirectDisplayID	display, VerticalRetraceProcPtr proc, long userData ) throw (OSStatus);
		virtual				~CocoaRetraceProc();
									

	protected:
		CGDirectDisplayID	display;
		OSStatus			error;
		MPTaskID			task;
		UInt32				emulatedRefreshRate;
	
		static OSStatus		MPTaskProcedure( void *data );
		static OSStatus		MPFixedTimeTaskProcedure( void *data );
};

// The initilization / cleanup functions for classic and cocoa API's
OSStatus 	LoadDirectDisplayAPI( const FSSpec &ourLib );
void 	CleanUpDirectDisplayAPI( void );
