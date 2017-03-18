////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2017, Frans Bouma
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
#include "stdafx.h"
#include "InterceptorHelper.h"
#include "GameConstants.h"
#include "GameImageHooker.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraAddressInterceptor();
	void gameSpeedInterceptor();
	void cameraWriteInterceptor();
	void hudToggleInterceptor();
	void timeStopInterceptor();
}

// external addresses used in asm.
extern "C" {
	// The continue address for continuing execution after camera values address interception. 
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWriteInterceptionContinue = nullptr;
	// the continue addresss for the continuing execution after interception of the game speed block of code.
	LPBYTE _gameSpeedInterceptorContinue = nullptr;
	// the continue addresss for the continuing execution after interception of the hud toggle block of code
	LPBYTE _hudToggleInterceptorContinue = nullptr;
	// the continue addresss for the continuing execution after interception of the timestop block of code
	LPBYTE _timeStopStructInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void setCameraStructInterceptorHook(LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(hostImageAddress, CAMERA_ADDRESS_INTERCEPT_START_OFFSET, CAMERA_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
	}

	void setCameraWriteInterceptorHook(LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(hostImageAddress, CAMERA_WRITE_INTERCEPT_START_OFFSET, CAMERA_WRITE_INTERCEPT_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue, &cameraWriteInterceptor);
	}

	void setTimestopInterceptorHook(LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(hostImageAddress, GAMESPEED_ADDRESS_INTERCEPT_START_OFFSET, GAMESPEED_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_gameSpeedInterceptorContinue, &gameSpeedInterceptor);
		GameImageHooker::setHook(hostImageAddress, TIMESTOP_ADDRESS_INTERCEPT_START_OFFSET, TIMESTOP_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_timeStopStructInterceptionContinue, &timeStopInterceptor);
	}

	void setHudToggleInterceptorHook(LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(hostImageAddress, HUD_TOGGLE_ADDRESS_INTERCEPT_START_OFFSET, HUD_TOGGLE_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_hudToggleInterceptorContinue, &hudToggleInterceptor);
	}
}
