#include "stdafx.h"
#include "input.h"
#include "Camera.h"

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
Camera* _camera;
float	_originalLookData[4];
float	_originalCoordsData[3];
Console* _consoleWrapper;

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
void RestoreOriginalCameraValues();
void CacheOriginalCameraValues();
void DisplayHelp();
void SetHook(__int64 startOffset, __int64 continueOffset, __int64* interceptionContinue, void* asmFunction);

//--------------------------------------------------------------------------------------------------------------------------------
// Implementations
void SystemStart(HMODULE hostBaseAddress, Console c)
{
	g_systemActive = true;
	_consoleWrapper = &c;
	_hostImageAddress = (__int64)hostBaseAddress;
	SetcameraStructInterceptorHook();
	WaitForCameraStruct();
	// camera struct found, we can proceed.
	InitCamera();
	Keyboard::init(hostBaseAddress);
	Mouse::init(hostBaseAddress);
	SetMatrixWriteInterceptorHooks();
	MainLoop();
}


void MainLoop()
{
	if (0 == _cameraStructAddress)
	{
		cerr << "Matrix address not found. Can't continue with main loop" << endl;
		return;
	}
	while (g_systemActive)
	{
		Sleep(FRAME_SLEEP);
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
	bool altPressed = keyboard.keyDown(Keyboard::KEY_LALT) || keyboard.keyDown(Keyboard::KEY_RALT);
	bool ctrlPressed = keyboard.keyDown(Keyboard::KEY_LCONTROL) || keyboard.keyDown(Keyboard::KEY_RCONTROL);

	if(keyboard.keyPressed(IGCS_KEY_CAMERA_ENABLE))
	{
		if (_cameraEnabled)
		{
			RestoreOriginalCameraValues();
			_consoleWrapper->WriteLine("Camera disabled");
		}
		else
		{
			CacheOriginalCameraValues();
			_consoleWrapper->WriteLine("Camera enabled");
		}
		_cameraEnabled = _cameraEnabled == 0 ? 1 : 0;
		// wait for 150ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(150);
	}
	if (keyboard.keyDown(IGCS_KEY_HELP) && altPressed)
	{
		DisplayHelp();
		// wait for 250ms to avoid fast keyboard hammering displaying multiple times the help!
		Sleep(250);
	}
	if(!_cameraEnabled)
	{
		// camera is disabled. We simply disable all input to the camera movement. 
		return;
	}

	_camera->ResetMovement();
	float multiplier = altPressed ? ALT_MULTIPLIER : ctrlPressed ? CTRL_MULTIPLIER : 1.0f;

	if(keyboard.keyDown(IGCS_KEY_MOVE_FORWARD))
	{
		_camera->MoveForward(multiplier);
	}
	if(keyboard.keyDown(IGCS_KEY_MOVE_BACKWARD))
	{
		_camera->MoveForward(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_RIGHT))
	{
		_camera->MoveRight(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_LEFT))
	{
		_camera->MoveRight(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_UP))
	{
		_camera->MoveUp(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_DOWN))
	{
		_camera->MoveUp(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_DOWN))
	{
		_camera->Pitch(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_UP))
	{
		_camera->Pitch(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_RIGHT))
	{
		_camera->Yaw(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_LEFT))
	{
		_camera->Yaw(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_TILT_LEFT))
	{
		_camera->Roll(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_TILT_RIGHT))
	{
		_camera->Roll(-multiplier);
	}

	// mouse 
	_camera->Pitch(mouse.yPosRelative() * MOUSE_SPEED_CORRECTION * multiplier);
	_camera->Yaw(mouse.xPosRelative() * MOUSE_SPEED_CORRECTION * multiplier);

	// XInput for controller update
}


void WriteCameraToCameraStructOfGame()
{
	if (!_cameraEnabled)
	{
		return;
	}

	// calculate new look quaternion
	D3DXQUATERNION q;
	_camera->CalculateLookQuaternion(&q);
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	lookInMemory[0] = q.x;
	lookInMemory[1] = q.y;
	lookInMemory[2] = q.z;
	lookInMemory[3] = q.w;

	// calculate new coords.
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	D3DXVECTOR3 currentCoords = D3DXVECTOR3(coordsInMemory);
	D3DXVECTOR3 newCoords;
	_camera->CalculateNewCoords(&newCoords, currentCoords, q);
	coordsInMemory[0] = newCoords.x;
	coordsInMemory[1] = newCoords.y;
	coordsInMemory[2] = newCoords.z;
}


void WaitForCameraStruct()
{
	_consoleWrapper->WriteLine("Waiting for camera struct interception...");
	while(0 == _cameraStructAddress)
	{
		Sleep(100);
	}
	_consoleWrapper->WriteLine("Camera found.");
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
	_camera = new Camera();
	// World has Z up and Y out of the screen, so rotate around X (pitch) -90 degrees.
	_camera->SetPitch((-90.0f * D3DX_PI) / 180.f);
}


void RestoreOriginalCameraValues()
{
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	for(int i=0;i<sizeof(_originalLookData);i++)
	{
		lookInMemory[i] = _originalLookData[i];
	}
	for (int i = 0; i < sizeof(_originalCoordsData); i++)
	{
		coordsInMemory[i] = _originalCoordsData[i];
	}
}


void CacheOriginalCameraValues()
{
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	for (int i = 0; i<sizeof(_originalLookData); i++)
	{
		_originalLookData[i] = lookInMemory[i];
	}
	for (int i = 0; i < sizeof(_originalCoordsData); i++)
	{
		_originalCoordsData[i] = coordsInMemory[i];
	}
}


void DisplayHelp()
{
	_consoleWrapper->WriteLine("---[IGCS Help]------------------------------------------", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("Keyboard:", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("--------------------------------------------------------", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("INS               : Enable/Disable camera");
	_consoleWrapper->WriteLine("ALT+rotate/move   : Faster rotate / move");
	_consoleWrapper->WriteLine("CTLR+rotate/move  : Slower rotate / move");
	_consoleWrapper->WriteLine("Arrow up/down     : Rotate camera up/down");
	_consoleWrapper->WriteLine("Arrow left/right  : Rotate camera left/right");
	_consoleWrapper->WriteLine("Numpad 8/Numpad 5 : Move camera forward/backward");
	_consoleWrapper->WriteLine("Numpad 4/Numpad 6 : Move camera left / right");
	_consoleWrapper->WriteLine("Numpad 7/Numpad 9 : Move camera up / down");
	_consoleWrapper->WriteLine("Numpad 1/Numpad 3 : Tilt camera left / right");
	_consoleWrapper->WriteLine("HOME              : Enable / disable FoV override");
	_consoleWrapper->WriteLine("Numpad +/Numpad - : Increase / decrease FoV");
	_consoleWrapper->WriteLine("ALT+H             : This help");
	_consoleWrapper->WriteLine("");
	_consoleWrapper->WriteLine("Mouse:", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("--------------------------------------------------------", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("Mouse movement    : Rotate camera up/down/left/right");
	_consoleWrapper->WriteLine("--------------------------------------------------------", CONSOLE_WHITE);
}


/* Camera info
 >>>>>>>>>>> USE DirectXMath, which is the successor of D3DX. See: https://blogs.msdn.microsoft.com/chuckw/2015/08/05/where-is-the-directx-sdk-2015-edition/
*/
