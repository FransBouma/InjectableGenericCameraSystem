#include "stdafx.h"
#include "DataStructures.h"

using namespace std;

// external asm functions
extern "C" {
	void interceptor();
}

// data shared with asm functions. This is allocated here, 'C' style and not in some datastructure as passing that to 
// MASM is rather tedious. 
extern "C" {
	// The address of the camera matrix in memory, intercepted by the interceptor. 
	__int64 _matrixAddress=0;
	// The continue address for continuing execution after matrix address interception. 
	__int64 _matrixAddressInterceptionContinue = 0;
	// the continue address for continuing execution after interception of code which writes to the camera matrix. 
	__int64 _matrixWriteInterceptionContinue = 0;
}

// local data 
// The base address of the host image in its own address space. Used to calculate real addresses with using an offset.
__int64 _hostImageAddress = 0;
__int64 _matrixAddressInterceptionStart = 0;
__int64 _matrixWriteInterceptionStart = 0;


// Local functions
void SetMatrixAddressInterceptorHook();

void MainLoop(HMODULE hostBaseAddress)
{
	cout << "System start" << endl;
	cout << showbase << hex;
	_hostImageAddress = (__int64)hostBaseAddress;
	SetMatrixAddressInterceptorHook();
	while(0==_matrixAddress)
	{
		Sleep(100);
	}
	cout << "Address found: " << _matrixAddress << endl;
	cout << "System end" << endl;
}


void SetMatrixAddressInterceptorHook()
{
	BYTE* startOfHookAddress = reinterpret_cast<BYTE*>(_hostImageAddress + (__int64)MATRIX_ADDRESS_INTERCEPT_START_OFFSET);
	// now write bytes of jmp qword ptr [address], which is jmp qword ptr 0 offset.
	startOfHookAddress[0] = 0xff;
	startOfHookAddress[1] = 0x25;
	startOfHookAddress[2] = 0;
	startOfHookAddress[3] = 0;
	startOfHookAddress[4] = 0;
	startOfHookAddress[5] = 0;
	// now write the address. Do this with a recast of the pointer to an __int64 pointer to avoid endianmess.
	__int64* interceptorAddressDestination = (__int64*)(startOfHookAddress+6);
	void* interceptorAddress = &interceptor;
	interceptorAddressDestination[0] = (__int64)interceptorAddress;
	_matrixAddressInterceptionContinue = _hostImageAddress + (__int64)MATRIX_ADDRESS_INTERCEPT_CONTINUE_OFFSET;
}


/* Camera info
// >>>>>>>>>>> USE DirectXMath, which is the successor of D3DX. See: https://blogs.msdn.microsoft.com/chuckw/2015/08/05/where-is-the-directx-sdk-2015-edition/
// >>>>>>>>>> USE Camera and files from toymaker.info.
// >>>>>>>>>> For FOV: Use:

void camera::setPerspectiveProjection(float FOV, float aspectRatio, float zNear, float zFar)
{
//convert FOV from degrees to radians
FOV = FOV * (float) DEG_TO_RAD;

D3DXMatrixPerspectiveFovLH( &projectionMatrix, FOV, aspectRatio, zNear, zFar );
}

where aspectRatio is the aspect ratio of the window of the main process. So obtain that ratio from a method, then feed it to this method.
Intercept window resize messages in a message pump? Check if this is necessary. Might not be. If interception is needed, this is done through a global hook:
https://www.codeproject.com/Articles/1037/Hooks-and-DLLs

But first create the camera, then we'll add the FoV and see if AR resizing is needed.
Also try to obtain the viewport of the d3d device, this might be a better approach than the window as the window might be 1920x1200 but the rendered view is 1920x1080
like in AC Unity/syndicate.

*/
