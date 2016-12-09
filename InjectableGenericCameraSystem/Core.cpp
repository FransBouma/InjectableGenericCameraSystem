#include "stdafx.h"
#include "input.h"
#include "Camera.h"
#include "InterceptorHelper.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// data shared with asm functions. This is allocated here, 'C' style and not in some datastructure as passing that to 
// MASM is rather tedious. 
extern "C" {
	LPBYTE _cameraStructAddress=NULL;
	byte _cameraEnabled = 0;
	LPBYTE _timeStopStructAddress = NULL;
	byte _timeStopped = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------
// local data 
static Camera* _camera=NULL;
static float _originalLookData[4];
static float _originalCoordsData[3];
static Console* _consoleWrapper=NULL;
static LPBYTE _hostImageAddress = NULL;
static bool _cameraMovementLocked = false;

//--------------------------------------------------------------------------------------------------------------------------------
// Local function definitions
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
	_hostImageAddress = (LPBYTE)hostBaseAddress;
	DisplayHelp();
	bool result = PerformAOBScans(_hostImageAddress);
	if (!result)
	{
		// can't continue
		return;
	}
	// initialization
	SetCameraStructInterceptorHook(_hostImageAddress);
	Keyboard::init(hostBaseAddress);
	Mouse::init(hostBaseAddress);
	WaitForCameraStruct();
	InitCamera();
	SetCameraWriteInterceptorHooks(_hostImageAddress);
	DisableFoVWrite(_hostImageAddress);
	// done initializing, start main loop of camera. We won't return from this
	MainLoop();
}


// Core loop of the system
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


// updates the data and camera for a frame 
void UpdateFrame()
{
	HandleUserInput();
	WriteCameraToCameraStructOfGame();
}


void HandleUserInput()
{
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();
	keyboard.update();
	mouse.update();
	bool altPressed = keyboard.keyDown(Keyboard::KEY_LALT) || keyboard.keyDown(Keyboard::KEY_RALT);
	bool ctrlPressed = keyboard.keyDown(Keyboard::KEY_LCONTROL) || keyboard.keyDown(Keyboard::KEY_RCONTROL);

	if (keyboard.keyDown(IGCS_KEY_HELP) && altPressed)
	{
		DisplayHelp();
		// wait for 250ms to avoid fast keyboard hammering displaying multiple times the help!
		Sleep(250);
	}
	if (0 == _cameraStructAddress)
	{
		// camera not found yet, can't proceed.
		return;
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
			_camera->ResetAngles();
		}
		_cameraEnabled = _cameraEnabled == 0 ? (byte)1 : (byte)0;
		// wait for 150ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(150);
	}
	if (keyboard.keyDown(IGCS_KEY_TIMESTOP))
	{
		if (_timeStopped)
		{
			_consoleWrapper->WriteLine("Game unpaused");
		}
		else
		{
			_consoleWrapper->WriteLine("Game paused");
		}
		_timeStopped = _timeStopped == 0 ? (byte)1 : (byte)0;
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

	if (keyboard.keyDown(IGCS_KEY_CAMERA_LOCK))
	{
		if (_cameraMovementLocked)
		{
			_consoleWrapper->WriteLine("Camera movement unlocked");
		}
		else
		{
			_consoleWrapper->WriteLine("Camera movement locked");
		}
		_cameraMovementLocked = !_cameraMovementLocked;
		// wait 150 ms to avoid fast keyboard hammering unlocking/locking the camera movement right away
		Sleep(150);
	}
	if (_cameraMovementLocked)
	{
		// no movement allowed, simply return
		return;
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


void InitCamera()
{
	_camera = new Camera();
	_camera->SetPitch(INITIAL_PITCH_RADIANS);
	_camera->SetRoll(INITIAL_ROLL_RADIANS);
	_camera->SetYaw(INITIAL_YAW_RADIANS);
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
	_consoleWrapper->WriteLine("HOME              : Lock/unlock camera movement");
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
	_consoleWrapper->WriteLine("Numpad 0          : Pause / Continue game");
	_consoleWrapper->WriteLine("ALT+H             : This help");
	_consoleWrapper->WriteLine("");
	_consoleWrapper->WriteLine("Mouse:", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("--------------------------------------------------------", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("Mouse movement    : Rotate camera up/down/left/right");
	_consoleWrapper->WriteLine("--------------------------------------------------------", CONSOLE_WHITE);
}
