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
	__int64 _cameraWriteInterceptionContinue1 = 0;
	// the continue address for continuing execution after interception of the second block of code which writes to the camera matrix. 
	__int64 _cameraWriteInterceptionContinue2 = 0;
	// the continue address for continuing execution after interception of the third block of code which writes to the camera matrix. 
	__int64 _cameraWriteInterceptionContinue3 = 0;
	
	byte	_cameraEnabled = 0;
	byte	_fovEnabled = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------
// local data 
// The base address of the host image in its own address space. Used to calculate real addresses with using an offset.
static __int64 _hostImageAddress = 0;
static __int64 _cameraStructInterceptionStart = 0;
static __int64 _cameraWriteInterceptionStart1 = 0;
static __int64 _cameraWriteInterceptionStart2 = 0;
static __int64 _cameraWriteInterceptionStart3 = 0;
static float* _gameMatrix=NULL;
static Camera* _camera=NULL;
static float	_originalLookData[4];
static float	_originalCoordsData[3];
static Console* _consoleWrapper=NULL;

//--------------------------------------------------------------------------------------------------------------------------------
// Local function definitions
void SetCameraStructInterceptorHook();
void SetCameraWriteInterceptorHooks();
void DisableFoVWrite();
void WaitForCameraStruct();
void MainLoop();
void UpdateFrame();
void HandleUserInput();
void InitCamera();
void WriteCameraToCameraStructOfGame();
void RestoreOriginalCameraValues();
void CacheOriginalCameraValues();
void DisplayHelp();
void ResetFoV();
void ChangeFoV(float amount);

//--------------------------------------------------------------------------------------------------------------------------------
// Implementations
void SystemStart(HMODULE hostBaseAddress, Console c)
{
	g_systemActive = true;
	_consoleWrapper = &c;
	_hostImageAddress = (__int64)hostBaseAddress;
	SetCameraStructInterceptorHook();
	WaitForCameraStruct();
	// camera struct found, we can proceed.
	InitCamera();
	Keyboard::init(hostBaseAddress);
	Mouse::init(hostBaseAddress);
	SetCameraWriteInterceptorHooks();
	DisableFoVWrite();
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
#pragma message(">>>>>>>>>> UPDATE: MAKE KEYBOARD HANDLING WORK WITHOUT NEEDING THE CAMERA, SO HELP IS DISPLAYED AT THE START")

	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();
	bool altPressed = keyboard.keyDown(Keyboard::KEY_LALT) || keyboard.keyDown(Keyboard::KEY_RALT);
	bool ctrlPressed = keyboard.keyDown(Keyboard::KEY_LCONTROL) || keyboard.keyDown(Keyboard::KEY_RCONTROL);

	if (keyboard.keyDown(IGCS_KEY_HELP) && altPressed)
	{
		DisplayHelp();
		// wait for 250ms to avoid fast keyboard hammering displaying multiple times the help!
		Sleep(250);
	}
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
#pragma message (">>>>>>>>> FIXME: RESET ANGLES IN CAMERA")
		}
		_cameraEnabled = _cameraEnabled == 0 ? (byte)1 : (byte)0;
		// wait for 150ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(150);
	}
	//////////////////////////////////////////////////// FOV
	if (keyboard.keyDown(IGCS_KEY_FOV_RESET))
	{
		ResetFoV();
	}
	if (keyboard.keyDown(IGCS_KEY_FOV_DECREASE))
	{
		ChangeFoV(-DEFAULT_FOV_SPEED);
	}
	if (keyboard.keyDown(IGCS_KEY_FOV_INCREASE))
	{
		ChangeFoV(DEFAULT_FOV_SPEED);
	}

	//////////////////////////////////////////////////// CAMERA
	if(!_cameraEnabled)
	{
		// camera is disabled. We simply disable all input to the camera movement. 
		return;
	}
	_camera->ResetMovement();
	float multiplier = altPressed ? ALT_MULTIPLIER : ctrlPressed ? CTRL_MULTIPLIER : 1.0f;

	if (keyboard.keyDown(IGCS_KEY_CAMERA_ENABLE))
	{
#pragma message (">>>>>>>>>>>>>>>>>IMPLEMENT CAMERA LOCK")
	}
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
	XMVECTOR q = _camera->CalculateLookQuaternion();
	XMFLOAT4 qAsFloat4;
	XMStoreFloat4(&qAsFloat4, q);

	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);

	lookInMemory[0] = qAsFloat4.x;
	lookInMemory[1] = qAsFloat4.y;
	lookInMemory[2] = qAsFloat4.z;
	lookInMemory[3] = qAsFloat4.w;

	// calculate new coords.
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	XMFLOAT3 currentCoords = XMFLOAT3(coordsInMemory);
	XMFLOAT3 newCoords = _camera->CalculateNewCoords(currentCoords, q);
	coordsInMemory[0] = newCoords.x;
	coordsInMemory[1] = newCoords.y;
	coordsInMemory[2] = newCoords.z;
}


void ResetFoV()
{
	if (NULL == _cameraStructAddress)
	{
		return;
	}
	float* fovInMemory = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_CAMERA_STRUCT_OFFSET);
	*fovInMemory = DEFAULT_FOV_RADIANS;
}


void ChangeFoV(float amount)
{
	if (NULL == _cameraStructAddress)
	{
		return;
	}
	float* fovInMemory = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_CAMERA_STRUCT_OFFSET);
	*fovInMemory += amount;
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


void SetCameraStructInterceptorHook()
{
	SetHook(_hostImageAddress, (__int64)CAMERA_ADDRESS_INTERCEPT_START_OFFSET, (__int64)CAMERA_ADDRESS_INTERCEPT_CONTINUE_OFFSET, 
			&_cameraStructInterceptionContinue, &cameraAddressInterceptor);
}


void SetCameraWriteInterceptorHooks()
{
	// for each block of code that writes to the camera values we're manipulating we need an interception to block it. For the example game there are 3. 
	SetHook(_hostImageAddress, (__int64)CAMERA_WRITE_INTERCEPT1_START_OFFSET, (__int64)CAMERA_WRITE_INTERCEPT1_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue1, &cameraWriteInterceptor1);
	SetHook(_hostImageAddress, (__int64)CAMERA_WRITE_INTERCEPT2_START_OFFSET, (__int64)CAMERA_WRITE_INTERCEPT2_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue2, &cameraWriteInterceptor2);
	SetHook(_hostImageAddress, (__int64)CAMERA_WRITE_INTERCEPT3_START_OFFSET, (__int64)CAMERA_WRITE_INTERCEPT3_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue3, &cameraWriteInterceptor3);
}


// The FoV write is disabled with NOPs, as the code block contains jumps out of the block so it's not easy to intercept with a silent method. It's OK though
// as the FoV changes simply change a value, so instead of the game keeping it at a value we overwrite it. We reset it to a default value when the FoV is disabled by the user 
void DisableFoVWrite()
{
	NopRange(_hostImageAddress + FOV_WRITE_INTERCEPT1_START_OFFSET, 2);
	NopRange(_hostImageAddress + FOV_WRITE_INTERCEPT2_START_OFFSET, 8);
}


void InitCamera()
{
	_camera = new Camera();
	// World has Z up and Y out of the screen, so rotate around X (pitch) -90 degrees.
	_camera->SetPitch((-90.0f * XM_PI) / 180.f);
}


void RestoreOriginalCameraValues()
{
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	for(int i=0;i<4;i++)
	{
		lookInMemory[i] = _originalLookData[i];
	}
	for (int i = 0; i < 3; i++)
	{
		coordsInMemory[i] = _originalCoordsData[i];
	}
}


void CacheOriginalCameraValues()
{
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	for (int i = 0; i<4; i++)
	{
		_originalLookData[i] = lookInMemory[i];
	}
	for (int i = 0; i < 3; i++)
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
	_consoleWrapper->WriteLine("HOME              : Lock/unlock camera position");
	_consoleWrapper->WriteLine("ALT+rotate/move   : Faster rotate / move");
	_consoleWrapper->WriteLine("CTLR+rotate/move  : Slower rotate / move");
	_consoleWrapper->WriteLine("Arrow up/down     : Rotate camera up/down");
	_consoleWrapper->WriteLine("Arrow left/right  : Rotate camera left/right");
	_consoleWrapper->WriteLine("Numpad 8/Numpad 5 : Move camera forward/backward");
	_consoleWrapper->WriteLine("Numpad 4/Numpad 6 : Move camera left / right");
	_consoleWrapper->WriteLine("Numpad 7/Numpad 9 : Move camera up / down");
	_consoleWrapper->WriteLine("Numpad 1/Numpad 3 : Tilt camera left / right");
	_consoleWrapper->WriteLine("Numpad +/Numpad - : Increase / decrease FoV");
	_consoleWrapper->WriteLine("Numpad *          : Reset FoV");
	_consoleWrapper->WriteLine("ALT+H             : This help");
	_consoleWrapper->WriteLine("");
	_consoleWrapper->WriteLine("Mouse:", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("--------------------------------------------------------", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("Mouse movement    : Rotate camera up/down/left/right");
	_consoleWrapper->WriteLine("--------------------------------------------------------", CONSOLE_WHITE);
}
