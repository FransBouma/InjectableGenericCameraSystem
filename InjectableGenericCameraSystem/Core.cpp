#include "stdafx.h"
#include "InputHandler.h"
#include "FPCamera.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraAddressInterceptor();
	void cameraWriteInterceptor1();		// create as much interceptors for write interception as needed. In the example game, there are 4.
	void cameraWriteInterceptor2();
	void cameraWriteInterceptor3();
	void cameraWriteInterceptor4();
}

//--------------------------------------------------------------------------------------------------------------------------------
// data shared with asm functions. This is allocated here, 'C' style and not in some datastructure as passing that to 
// MASM is rather tedious. 
extern "C" {
	// The address of the camera matrix in memory, intercepted by the interceptor. 
	__int64 _cameraStructAddress=0;
	// The continue address for continuing execution after matrix address interception. 
	__int64 _cameraStructInterceptionContinue = 0;
	// the continue address for continuing execution after interception of the first block of code which writes to the camera matrix. 
	__int64 _matrixWriteInterceptionContinue1 = 0;
	// the continue address for continuing execution after interception of the second block of code which writes to the camera matrix. 
	__int64 _matrixWriteInterceptionContinue2 = 0;
	// the continue address for continuing execution after interception of the third block of code which writes to the camera matrix. 
	__int64 _matrixWriteInterceptionContinue3 = 0;
	// the continue address for continuing execution after interception of the fourth block of code which writes to the camera matrix. 
	__int64 _matrixWriteInterceptionContinue4 = 0;
	byte	_cameraEnabled = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------
// local data 
// The base address of the host image in its own address space. Used to calculate real addresses with using an offset.
__int64 _hostImageAddress = 0;
__int64 _cameraStructInterceptionStart = 0;
__int64 _matrixWriteInterceptionStart1 = 0;
__int64 _matrixWriteInterceptionStart2 = 0;
__int64 _matrixWriteInterceptionStart3 = 0;
__int64 _matrixWriteInterceptionStart4 = 0;
float* _gameMatrix;
InputHandler _inputHandler;
CFPCamera* _camera;

//--------------------------------------------------------------------------------------------------------------------------------
// Local function definitions
void SetcameraStructInterceptorHook();
void SetMatrixWriteInterceptorHooks();
void WaitForCameraStruct();
void MainLoop();
void HandleInput();
void WriteCameraToGameMatrix();
void SetHook(__int64 startOffset, __int64 continueOffset, __int64* interceptionContinue, void* asmFunction);

//--------------------------------------------------------------------------------------------------------------------------------
// Implementations
void SystemStart(HMODULE hostBaseAddress)
{
	cout << "System start" << endl;
	cout << showbase << hex;
	_hostImageAddress = (__int64)hostBaseAddress;
	SetcameraStructInterceptorHook();
	WaitForCameraStruct();
	// matrix found, we can do our business.
	_inputHandler.Initialize(hostBaseAddress, GetConsoleWindow());
	SetMatrixWriteInterceptorHooks();
	MainLoop();
	_inputHandler.Shutdown();
	cout << "System end" << endl;
}


void MainLoop()
{
	if (0 == _cameraStructAddress)
	{
		cout << "Matrix address not found. Can't continue with main loop" << endl;
		return;
	}
	_camera = new CFPCamera(D3DXVECTOR3(_gameMatrix[12], _gameMatrix[13], _gameMatrix[14]));
	while (true)
	{
		Sleep(25);
		HandleInput();
		WriteCameraToGameMatrix();
	}
}


void WriteCameraToGameMatrix()
{
	if (!_cameraEnabled)
	{
		return;
	}
	D3DXMATRIX viewMatrix;
	_camera->CalculateViewMatrix(&viewMatrix);
	_gameMatrix[0] = viewMatrix._11;
	_gameMatrix[1] = viewMatrix._12;
	_gameMatrix[2] = viewMatrix._13;
	_gameMatrix[3] = viewMatrix._14;
	_gameMatrix[4] = viewMatrix._21;
	_gameMatrix[5] = viewMatrix._22;
	_gameMatrix[6] = viewMatrix._23;
	_gameMatrix[7] = viewMatrix._24;
	_gameMatrix[8] = viewMatrix._31;
	_gameMatrix[9] = viewMatrix._32;
	_gameMatrix[10] = viewMatrix._33;
	_gameMatrix[11] = viewMatrix._34;
	_gameMatrix[12] = viewMatrix._41;
	_gameMatrix[13] = viewMatrix._42;
	_gameMatrix[14] = viewMatrix._43;
	_gameMatrix[15] = viewMatrix._44;
}


void HandleInput()
{
	// first keyboard / mouse input.
	_inputHandler.Frame();

	if (_inputHandler.IsKeyPressed(IGCS_KEY_CAMERA_ENABLE))
	{
#pragma message ("RE INITIALIZE CAMERA WITH COORDS FROM ORIGINAL MATRIX HERE, BEFORE ENABLING IT, IF IT'S DISABLED")
		_cameraEnabled = _cameraEnabled == 0 ? 1 : 0;
		cout << "Camera enabled: " << (_cameraEnabled==0 ? "False" : "True") << endl;
		// wait for 150ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(150);
	}
	if (!_cameraEnabled)
	{
		// camera is disabled. We simply disable all input to the camera movement. 
		return;
	}
	if (_inputHandler.IsKeyPressed(IGCS_KEY_MOVE_FORWARD))
	{
		cout << "Move forward pressed" << endl;
		_camera->MoveForward(0.1);
	}
	if (_inputHandler.IsKeyPressed(IGCS_KEY_MOVE_BACKWARD))
	{
		cout << "Move backward pressed" << endl;
		_camera->MoveForward(-0.1);
	}
	if (_inputHandler.IsKeyPressed(IGCS_KEY_MOVE_RIGHT))
	{
		cout << "Move right pressed" << endl;
		_camera->MoveRight(0.1);
	}

	// XInput for controller update
}


void WaitForCameraStruct()
{
	while(0 == _cameraStructAddress)
	{
		Sleep(100);
	}
	cout << "Address found: " << _cameraStructAddress << endl;
	_gameMatrix = reinterpret_cast<float*>(_cameraStructAddress + MATRIX_START_IN_CAMERA_STRUCT_OFFSET);
}


void SetcameraStructInterceptorHook()
{
	SetHook((__int64)MATRIX_ADDRESS_INTERCEPT_START_OFFSET, (__int64)MATRIX_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
}


void SetMatrixWriteInterceptorHooks()
{
	// for each block of code that writes to the camera matrix we need an interception to block it. For the example game there are 4. 
	SetHook((__int64)MATRIX_WRITE_INTERCEPT1_START_OFFSET, (__int64)MATRIX_WRITE_INTERCEPT1_CONTINUE_OFFSET, &_matrixWriteInterceptionContinue1, &cameraWriteInterceptor1);
	SetHook((__int64)MATRIX_WRITE_INTERCEPT2_START_OFFSET, (__int64)MATRIX_WRITE_INTERCEPT2_CONTINUE_OFFSET, &_matrixWriteInterceptionContinue2, &cameraWriteInterceptor2);
	SetHook((__int64)MATRIX_WRITE_INTERCEPT3_START_OFFSET, (__int64)MATRIX_WRITE_INTERCEPT3_CONTINUE_OFFSET, &_matrixWriteInterceptionContinue3, &cameraWriteInterceptor3);
	SetHook((__int64)MATRIX_WRITE_INTERCEPT4_START_OFFSET, (__int64)MATRIX_WRITE_INTERCEPT4_CONTINUE_OFFSET, &_matrixWriteInterceptionContinue4, &cameraWriteInterceptor4);
}


void SetHook(__int64 startOffset, __int64 continueOffset, __int64* interceptionContinue, void* asmFunction)
{
	BYTE* startOfHookAddress = reinterpret_cast<BYTE*>(_hostImageAddress + startOffset);
	*interceptionContinue = _hostImageAddress + (__int64)continueOffset;
	// now write bytes of jmp qword ptr [address], which is jmp qword ptr 0 offset.
	memcpy(startOfHookAddress, jmpFarInstructionBytes, sizeof(jmpFarInstructionBytes));
	// now write the address. Do this with a recast of the pointer to an __int64 pointer to avoid endianmess.
	__int64* interceptorAddressDestination = (__int64*)(startOfHookAddress + 6);
	interceptorAddressDestination[0] = (__int64)asmFunction;
}


/* Camera info
 >>>>>>>>>>> USE DirectXMath, which is the successor of D3DX. See: https://blogs.msdn.microsoft.com/chuckw/2015/08/05/where-is-the-directx-sdk-2015-edition/
 >>>>>>>>>> USE Camera and files from toymaker.info.
 >>>>>>>>>> FOV is always handled by the game, we can't embed this in the camera we'll be using as that creates a view matrix. 
			So the system needs additional key handling and target overwriting for that. This can't be done with the viewmatrix
            as it's done by the projection set for the window, which is done separately. this isn't a big deal though. 
            We already need additional key handling for timestop too, as that's a nice thing to add to the camera system as well (if timestop is available). 
 			FOV should always be usable, with or without the camera enabled. 
			The view matrix might have camera pos differently defined than the game wants, the code should anticipate on that. 
*/
