#include "stdafx.h"
#include "input.h"
#include "FPCamera.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraAddressInterceptor();
	void cameraWriteInterceptor1();		// create as much interceptors for write interception as needed. In the example game, there are 4.
	void cameraWriteInterceptor2();
	void cameraWriteInterceptor3();
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
float* _gameMatrix;
CFPCamera* _camera;

//--------------------------------------------------------------------------------------------------------------------------------
// Local function definitions
void SetcameraStructInterceptorHook();
void SetMatrixWriteInterceptorHooks();
void WaitForCameraStruct();
void MainLoop();
void UpdateFrame();
void HandleUserInput();
void InitCamera();
void WriteCameraToCameraStructOfGame();
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
	InitCamera();
	// matrix found, we can do our business.
	Keyboard::init(hostBaseAddress);
	Mouse::init(hostBaseAddress);
	SetMatrixWriteInterceptorHooks();
	MainLoop();
	
	cout << "System end" << endl;
}


void MainLoop()
{
	if (0 == _cameraStructAddress)
	{
		cout << "Matrix address not found. Can't continue with main loop" << endl;
		return;
	}
	while (true)
	{
		Sleep(8);
		UpdateFrame();
	}
}


void UpdateFrame()
{
	Keyboard::instance().update();
	Mouse::instance().update();
	HandleUserInput();
	WriteCameraToCameraStructOfGame();
}


void HandleUserInput()
{
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	if(keyboard.keyPressed(IGCS_KEY_CAMERA_ENABLE))
	{
#pragma message ("RE INITIALIZE CAMERA WITH COORDS FROM ORIGINAL MATRIX HERE, BEFORE ENABLING IT, IF IT'S DISABLED")
		_cameraEnabled = _cameraEnabled == 0 ? 1 : 0;
		cout << "Camera enabled: " << (_cameraEnabled==0 ? "False" : "True") << endl;
		// wait for 150ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(150);
	}
	if(!_cameraEnabled)
	{
		// camera is disabled. We simply disable all input to the camera movement. 
		return;
	}

	_camera->ResetMovement();
	bool altPressed = keyboard.keyDown(Keyboard::KEY_LALT) || keyboard.keyDown(Keyboard::KEY_RALT);
	bool ctrlPressed = keyboard.keyDown(Keyboard::KEY_LCONTROL) || keyboard.keyDown(Keyboard::KEY_RCONTROL);
	float multiplier = altPressed ? 3.0f : ctrlPressed ? 0.3f : 1.0f;

	if(keyboard.keyDown(IGCS_KEY_MOVE_FORWARD))
	{
		cout << "Move forward pressed" << endl;
		_camera->MoveForward(0.1f * multiplier);
	}
	if(keyboard.keyDown(IGCS_KEY_MOVE_BACKWARD))
	{
		cout << "Move backward pressed" << endl;
		_camera->MoveForward(-0.1f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_RIGHT))
	{
		cout << "Move right pressed" << endl;
		_camera->MoveRight(0.1f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_LEFT))
	{
		cout << "Move left pressed" << endl;
		_camera->MoveRight(-0.1f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_UP))
	{
		cout << "Move up pressed" << endl;
		_camera->MoveUp(0.1f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_DOWN))
	{
		cout << "Move down pressed" << endl;
		_camera->MoveUp(-0.1f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_DOWN))
	{
		cout << "Rotate down pressed" << endl;
		_camera->Pitch(0.01f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_UP))
	{
		cout << "Rotate up pressed" << endl;
		_camera->Pitch(-0.01f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_RIGHT))
	{
		cout << "Rotate right pressed" << endl;
		_camera->Yaw(0.01f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_LEFT))
	{
		cout << "Rotate left pressed" << endl;
		_camera->Yaw(-0.01f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_TILT_LEFT))
	{
		cout << "Tilt left pressed" << endl;
		_camera->Roll(0.01f * multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_TILT_RIGHT))
	{
		cout << "Tilt right pressed" << endl;
		_camera->Roll(-0.01f * multiplier);
	}

	// mouse 
	_camera->Pitch(mouse.yPosRelative() * 0.001f * multiplier);
	_camera->Yaw(mouse.xPosRelative() * 0.001f * multiplier);

	// XInput for controller update
}


void WriteCameraToCameraStructOfGame()
{
	if (!_cameraEnabled)
	{
		return;
	}

	D3DXQUATERNION q;
	_camera->CalculateLookQuaternion(&q);
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	lookInMemory[0] = q.x;
	lookInMemory[1] = q.y;
	lookInMemory[2] = q.z;
	lookInMemory[3] = q.w;

	// calculate new coords.
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	D3DXVECTOR3 currentCoords = D3DXVECTOR3(coordsInMemory[0], coordsInMemory[1], coordsInMemory[2]);
	D3DXVECTOR3 newCoords;
	_camera->CalculateNewCoords(&newCoords, currentCoords, q);
	coordsInMemory[0] = newCoords.x;
	coordsInMemory[1] = newCoords.y;
	coordsInMemory[2] = newCoords.z;
}


void WaitForCameraStruct()
{
	cout << "Waiting for camera struct interception..." << endl;
	while(0 == _cameraStructAddress)
	{
		Sleep(100);
	}
	cout << "Address found: " << _cameraStructAddress << endl;
}


void SetcameraStructInterceptorHook()
{
	SetHook((__int64)MATRIX_ADDRESS_INTERCEPT_START_OFFSET, (__int64)MATRIX_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
}


void SetMatrixWriteInterceptorHooks()
{
	// for each block of code that writes to the camera values we're manipulating we need an interception to block it. For the example game there are 3. 
	SetHook((__int64)MATRIX_WRITE_INTERCEPT1_START_OFFSET, (__int64)MATRIX_WRITE_INTERCEPT1_CONTINUE_OFFSET, &_matrixWriteInterceptionContinue1, &cameraWriteInterceptor1);
	SetHook((__int64)MATRIX_WRITE_INTERCEPT2_START_OFFSET, (__int64)MATRIX_WRITE_INTERCEPT2_CONTINUE_OFFSET, &_matrixWriteInterceptionContinue2, &cameraWriteInterceptor2);
	SetHook((__int64)MATRIX_WRITE_INTERCEPT3_START_OFFSET, (__int64)MATRIX_WRITE_INTERCEPT3_CONTINUE_OFFSET, &_matrixWriteInterceptionContinue3, &cameraWriteInterceptor3);
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


void InitCamera()
{
	_camera = new CFPCamera();
	// World has Z up and Y into the screen, so rotate around X (pitch) -90 degrees.
	_camera->Pitch((-90.0f * D3DX_PI) / 180.f);
}


/* Camera info
 >>>>>>>>>>> USE DirectXMath, which is the successor of D3DX. See: https://blogs.msdn.microsoft.com/chuckw/2015/08/05/where-is-the-directx-sdk-2015-edition/
*/
