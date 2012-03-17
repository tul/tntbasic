// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Carbon_Settings.h
// © Mark Tully 2002
// 20/1/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

// Note
// RNE tends to impact performance on OS 9 quite a bit, but that's the differences between monopolising the CPU
// and not doing. Performance seems to degrade the more apps you run.
// Under OS X it gives better performance.

// set to 1 to use carbon timers and receivenextevent loop to do main tb processing under 9 and X
// set to 0 for classic style monopolise the cpu stuff
// set to UEnvironment::IsRunningOSX() to only do so under OS X
#define				USE_CARBON_TIMERS			UEnvironment::IsRunningOSX()

// set to 1 to use RecieveNextEvent to block your process when waiting for for frame draw
// time under 9 and X
// set to 0 to use spin locks
// set to UEnvironment::IsRunningOSX() to only do so under OS X
// NB: Only set this if using carbon timers
#define				USE_RNE_FOR_FRAME_WAIT		UEnvironment::IsRunningOSX()

// set to 1 to use RecieveNextEvent for input calls that block the tnt basic program, such
// as wait button click, wait mouse down etc under 9 and X
// set to 0 to use spin locks and low level readers like Button()
// set to UEnvironment::IsRunningOSX() to only do so under OS X
// NB: Only set this if using carbon timers
#define				USE_RNE_FOR_SYNC_INPUT		UEnvironment::IsRunningOSX()

// when doing a vbl sync on osx, use semaphores to communicate with the mp task
// set to 1 use semaphores under OS X
// set to 0 to to a spin lock in the main task to wait for the mp task to signal
// NB: always spin locks on os 9
#define				USE_SEMAPHORE_FOR_VBL		1

// set to 1 to draw directly to the window and use QDFlushPortBuffer to blit
// set to 0 to draw to a back buffer and then blit using copybits or blastcopy
// set to UEnvironment::IsRunningOSX() to only do so under OS X
// NB: when using window mode (as opposed to full screen mode) we can't rely on the window being 16 bit, so
// we can't draw direct to it. this incurs an extra buffer copy on Mac OS X - full screen will always give a
// higher framerate as that allows us to draw direct to the window back buffer
#define				DRAW_DIRECT_TO_WINDOW		(mGoFullScreen && UEnvironment::IsRunningOSX())

// set to 1 to read the keyboard state using carbon events. this includes cmd-. and escape.
// set to 0 to read it using GetKeys. GetKeys is buggy under os x on certain machines.
// set to UEnvironment::IsRunningOSX() to only do so under OS X
#define				USE_RNE_FOR_KEYBOARD_INPUT	UEnvironment::IsRunningOSX()