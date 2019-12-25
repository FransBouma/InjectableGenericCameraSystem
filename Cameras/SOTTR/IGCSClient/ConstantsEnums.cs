////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2019, Frans Bouma
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IGCSClient
{
	internal class ConstantsEnums
	{
		internal static readonly string NamedPipeName = "IgcsIPCPipe";
		internal static readonly int BufferLength = 1*1024*1024;	// 1MB buffer should be more than enough. We expect to be actively reading whenever things arrive.
		internal static readonly string IniFilename = "IGCSClientSettings.ini";
	}


	/// <summary>
	/// Enum to define the payload type. We support a limited number of value types to send over the pipe. We keep things simple for now.
	/// </summary>
	public enum PayloadType : byte
	{
		Undefined = 0,
		Byte,			// 1 byte length
		Int16,			// 2 byte length
		Int32,			// 4 byte length
		Int64,			// 8 byte length
		AsciiString,	// string length + 1 byte length
		Float,			// 4 byte length, IEEE encoded
		Double,			// 8 byte length, IEEE encoded
		Bool,			// 1 byte length, 1 == true, 0 == false.
	}

	public class CameraDeviceType
	{
		public const byte KeyboardMouse = 0;
		public const byte Gamepad = 1;
		public const byte Both = 2;
	}


	public class ScreenshotType
	{
		public const byte HorizontalPanorama = 0;
		public const byte Lightfield = 1;
	}


	public class SettingType
	{
		public const byte FastMovementMultiplier = 0;
		public const byte SlowMovementMultiplier = 1;
		public const byte UpMovementMultiplier = 2;
		public const byte MovementSpeed = 3;
		public const byte CameraControlDevice = 4;
		public const byte RotationSpeed = 5;
		public const byte InvertYLookDirection = 6;
		public const byte FoVZoomSpeed = 7;
		public const byte ShotOutputFolder = 8;
		public const byte ShotFramesToWait = 9;
		public const byte ShotType = 10;
		public const byte PanoTotalFoV = 11;
		public const byte PanoOverlapPercentage = 12;
		public const byte LightfieldDistance = 13;
		public const byte LightfieldShotCount = 14;
		// to add more, derived a type of this class and define the next value one higher than the last one in this class.
	}


	public class KeyBindingType
	{
		public const byte BlockInputToGame = 0;
		public const byte EnableDisableCamera = 1;
		public const byte DecreaseFoV=2;
		public const byte IncreaseFoV=3;
		public const byte ResetFoV=4;
		public const byte LockUnlockCameraMovement=5;
		public const byte MoveCameraLeft=6;
		public const byte MoveCameraRight=7;
		public const byte MoveCameraForward=8;
		public const byte MoveCameraBackward=9;
		public const byte MoveCameraUp=10;
		public const byte MoveCameraDown=11;
		public const byte RotateCameraUp=12;
		public const byte RotateCameraDown=13;
		public const byte RotateCameraLeft=14;
		public const byte RotateCameraRight=15;
		public const byte TiltCameraLeft=16;
		public const byte TiltCameraRight=17;
		public const byte TestMultiShotSetup=18;
		public const byte TakeShot=19;
		public const byte TakeMultiShot=20;
		public const byte ToggleHUD=21;
		public const byte PauseUnpauseGame = 22;
		// to add more, derived a type of this class and define the next value one higher than the last one in this class.
	}


	public class SettingKind
	{
		public const byte NormalSetting = 1;
		public const byte KeyBinding = 2;
	}
}
