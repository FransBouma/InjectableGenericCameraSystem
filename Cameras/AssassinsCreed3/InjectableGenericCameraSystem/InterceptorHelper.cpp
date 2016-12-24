////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2016, Frans Bouma
// All rights reserved.
// https://github.com/FransBouma/InjectableGenericCameraSystem
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
//  * Redistributions of source code must retain the above copyright notice, this
//	  list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Game specific code. The interface is defined in InterceptorHelper.h, these methods have to be implemented, that's it.
// Use asm functions to inject the hooks
/////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "InterceptorHelper.h"

using namespace std;
//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions

extern "C" {
	void cameraAddressInterceptor();
	void cameraWriteInterceptor1();		// create as much interceptors for write interception as needed. In the example game, there are 4.
	void timestopStructInterceptor();
}

extern "C" {
	// The continue address for continuing execution after camera values address interception. 
	LPBYTE _cameraStructInterceptionContinue = 0;
	// the continue address for continuing execution after interception of the first block of code which writes to the camera values. 
	LPBYTE _cameraWriteInterceptionContinue1 = 0;
	// the continue address for the continuing execution after interception of the timestop block of code. 
	LPBYTE _timestopStructInterceptionContinue = 0;
}

static LPBYTE _cameraStructInterceptionStart = 0;
static LPBYTE _cameraWriteInterceptionStart1 = 0;
static LPBYTE _timestopStructInterceptionStart = 0;


void SetCameraStructInterceptorHook(LPBYTE hostImageAddress)
{
	SetHook(hostImageAddress, CAMERA_ADDRESS_INTERCEPT_START_OFFSET, CAMERA_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
}


void SetCameraWriteInterceptorHooks(LPBYTE hostImageAddress)
{
	// for each block of code that writes to the camera values we're manipulating we need an interception to block it. For the example game there are 3. 
	SetHook(hostImageAddress, CAMERA_WRITE_INTERCEPT1_START_OFFSET, CAMERA_WRITE_INTERCEPT1_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue1, &cameraWriteInterceptor1);
}

void SetTimestopInterceptorHook(LPBYTE hostImageAddress)
{
	SetHook(hostImageAddress, TIMESTOP_ADDRESS_INTERCEPT_START_OFFSET, TIMESTOP_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_timestopStructInterceptionContinue, &timestopStructInterceptor);
}

