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
using IGCSClient.Classes;

namespace IGCSClient.GameSpecific.Classes
{
	/// <summary>
	/// Creates setting instances for the setting editor. It defines the setting defaults/min/max values for the various settings based
	/// on the game it will be used with.
	/// </summary>
	public static class SettingFactory
	{
		public static void InitializeSettings()
		{
			var appState = AppStateSingleton.Instance();
			appState.AddSetting(new FloatSetting(SettingType.FastMovementMultiplier, nameof(SettingType.FastMovementMultiplier), 0.1M, 100.0M, 2, 0.01M, 5.0f));
			appState.AddSetting(new FloatSetting(SettingType.SlowMovementMultiplier, nameof(SettingType.SlowMovementMultiplier), 0.001M, 1.0M, 3, 0.001M, 0.1f));
			appState.AddSetting(new FloatSetting(SettingType.UpMovementMultiplier, nameof(SettingType.UpMovementMultiplier), 0.1M, 10.0M, 2, 0.01M, 0.7f));
			appState.AddSetting(new FloatSetting(SettingType.MovementSpeed, nameof(SettingType.MovementSpeed), 0.1M, 3.0M, 2, 0.01M, 1.0f));
			appState.AddSetting(new DropDownSetting(SettingType.CameraControlDevice, nameof(SettingType.CameraControlDevice),
													new List<string>()
													{
														nameof(CameraDeviceType.KeyboardMouse), nameof(CameraDeviceType.Gamepad), nameof(CameraDeviceType.Both)
													}, 2));
			appState.AddSetting(new FloatSetting(SettingType.RotationSpeed, nameof(SettingType.RotationSpeed), 0.001M, 0.5M, 3, 0.001M, 0.01f));
			appState.AddSetting(new BoolSetting(SettingType.InvertYLookDirection, nameof(SettingType.InvertYLookDirection), false));
			appState.AddSetting(new FloatSetting(SettingType.FoVZoomSpeed, nameof(SettingType.FoVZoomSpeed), 0.0001M, 0.01M, 4, 0.0001M, 0.001f));
			appState.AddSetting(new FolderSetting(SettingType.ShotOutputFolder, nameof(SettingType.ShotOutputFolder), 
												  Environment.GetFolderPath(Environment.SpecialFolder.MyPictures), "Please select the screenshot output directory"));
			appState.AddSetting(new IntSetting(SettingType.ShotFramesToWait, nameof(SettingType.ShotFramesToWait), 1, 100, 1, 5));
			appState.AddSetting(new DropDownSetting(SettingType.ShotType, nameof(SettingType.ShotType), new List<string>()
																										{
																											nameof(ScreenshotType.HorizontalPanorama), 
																											nameof(ScreenshotType.Lightfield)
																										}, 0));
			appState.AddSetting(new FloatSetting(SettingType.PanoTotalFoV, nameof(SettingType.PanoTotalFoV), 30.0M, 359.0M, 1, 0.1M, 110.0f));
			appState.AddSetting(new FloatSetting(SettingType.PanoOverlapPercentage, nameof(SettingType.PanoOverlapPercentage), 0.1M, 99.9M, 1, 0.1M, 80.0f));
			appState.AddSetting(new FloatSetting(SettingType.LightfieldDistance, nameof(SettingType.LightfieldDistance), 0.001M, 5.0M, 3, 0.001M, 1.0f));
			appState.AddSetting(new IntSetting(SettingType.LightfieldShotCount, nameof(SettingType.LightfieldShotCount), 0, 60, 1, 45));
		}
	}
}
