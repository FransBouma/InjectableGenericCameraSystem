/////////////////////////////////////////////////////////////////////////////////
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
	void cameraAddressInterceptor2();
	void cameraWriteInterceptor1();		// create as much interceptors for write interception as needed. In the example game, there are 4.
	void cameraWriteInterceptor2();
	void cameraWriteInterceptor3();
	void gamespeedAddressInterceptor();
}

extern "C" {
	// The continue address for continuing execution after camera values address interception. 
	LPBYTE _cameraStructInterceptionContinue = 0;
	LPBYTE _cameraStructInterceptionContinue2 = 0;
	// the continue address for continuing execution after interception of the first block of code which writes to the camera values. 
	LPBYTE _cameraWriteInterceptionContinue1 = 0;
	// the continue address for continuing execution after interception of the second block of code which writes to the camera values. 
	LPBYTE _cameraWriteInterceptionContinue2 = 0;
	// the continue address for continuing execution after interception of the third block of code which writes to the camera values. 
	LPBYTE _cameraWriteInterceptionContinue3 = 0;
	// the continue address for the continuing execution after interception of the gamespeed block of code. 
	LPBYTE _gamespeedInterceptionContinue = 0;
}

static LPBYTE _cameraStructInterceptionStart = 0;
static LPBYTE _cameraWriteInterceptionStart1 = 0;
static LPBYTE _cameraWriteInterceptionStart2 = 0;
static LPBYTE _cameraWriteInterceptionStart3 = 0;
static LPBYTE _gamespeedInterceptionStart = 0;


void SetCameraStructInterceptorHook(LPBYTE hostImageAddress)
{
	SetHook(hostImageAddress, CAMERA_ADDRESS_INTERCEPT_START_OFFSET, CAMERA_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
	SetHook(hostImageAddress, CAMERA2_ADDRESS_INTERCEPT_START_OFFSET, CAMERA2_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_cameraStructInterceptionContinue2, &cameraAddressInterceptor2);
}


void SetCameraWriteInterceptorHooks(LPBYTE hostImageAddress)
{
	// for each block of code that writes to the camera values we're manipulating we need an interception to block it. For the example game there are 3. 
	SetHook(hostImageAddress, CAMERA_WRITE_INTERCEPT1_START_OFFSET, CAMERA_WRITE_INTERCEPT1_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue1, &cameraWriteInterceptor1);
	SetHook(hostImageAddress, CAMERA_WRITE_INTERCEPT2_START_OFFSET, CAMERA_WRITE_INTERCEPT2_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue2, &cameraWriteInterceptor2);
	SetHook(hostImageAddress, CAMERA_WRITE_INTERCEPT3_START_OFFSET, CAMERA_WRITE_INTERCEPT3_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue3, &cameraWriteInterceptor3);
}

void SetTimestopInterceptorHook(LPBYTE hostImageAddress)
{
	SetHook(hostImageAddress, GAMESPEED_ADDRESS_INTERCEPT_START_OFFSET, GAMESPEED_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_gamespeedInterceptionContinue, &gamespeedAddressInterceptor);
}


// The FoV write is disabled with NOPs, as the code block contains jumps out of the block so it's not easy to intercept with a silent method. It's OK though
// as the FoV changes simply change a value, so instead of the game keeping it at a value we overwrite it. We reset it to a default value when the FoV is disabled by the user 
void DisableFoVWrite(LPBYTE hostImageAddress)
{
	NopRange(hostImageAddress + FOV_WRITE_INTERCEPT1_START_OFFSET, 2);
	NopRange(hostImageAddress + FOV_WRITE_INTERCEPT2_START_OFFSET, 8);
}

