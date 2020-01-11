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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using IGCSClient.Classes;
using IGCSClient.Interfaces;

namespace IGCSClient.GameSpecific.Controls
{
	public partial class SettingEditor : UserControl
	{
		public SettingEditor()
		{
			InitializeComponent();
		}


		internal void Setup()
		{
			var settings = AppStateSingleton.Instance().Settings;
			foreach(var setting in settings)
			{
				switch(setting.ID)
				{
					//case SettingType.FastMovementMultiplier:
					//	setting.Setup(_fastMovementInput);
					//	break;
					//case SettingType.SlowMovementMultiplier:
					//	setting.Setup(_slowMovementInput);
					//	break;
					//case SettingType.UpMovementMultiplier:
					//	setting.Setup(_upMovementInput);
					//	break;
					//case SettingType.MovementSpeed:
					//	setting.Setup(_movementSpeedInput);
					//	break;
					//case SettingType.CameraControlDevice:
					//	setting.Setup(_cameraControlDeviceInput);
					//	break;
					//case SettingType.RotationSpeed:
					//	setting.Setup(_rotationSpeedInput);
					//	break;
					//case SettingType.InvertYLookDirection:
					//	setting.Setup(_invertYInput);
					//	break;
					//case SettingType.FoVZoomSpeed:
					//	setting.Setup(_fovSpeedInput);
					//	break;
					//case SettingType.ShotOutputFolder:
					//	setting.Setup(_shotOutputFolderInput);
					//	break;
					//case SettingType.ShotFramesToWait:
					//	setting.Setup(_frameWaitInput);
					//	break;
					//case SettingType.ShotType:
					//	setting.Setup(_shotTypeInput);
					//	break;
					//case SettingType.PanoTotalFoV:
					//	setting.Setup(_panoFovInput);
					//	break;
					//case SettingType.PanoOverlapPercentage:
					//	setting.Setup(_panoOverlapInput);
					//	break;
					//case SettingType.LightfieldDistance:
					//	setting.Setup(_lightfieldDistanceInput);
					//	break;
					//case SettingType.LightfieldShotCount:
					//	setting.Setup(_lightfieldShotCountInput);
					//	break;
					//case SettingType.ShotFileType:
					//	setting.Setup(_shotFileTypeInput);
					//	break;
				}
			}
		}


		private void _shotTypeInput_ValueChanged(object sender, EventArgs e)
		{
			// display the right panel.
			var control = (IInputControl<int>)sender;
			_horizontalPanoramaControlsPanel.Visible = (control.Value == ScreenshotType.HorizontalPanorama);
			_lightfieldControlsPanel.Visible = (control.Value == ScreenshotType.Lightfield);
		}
	}
}
