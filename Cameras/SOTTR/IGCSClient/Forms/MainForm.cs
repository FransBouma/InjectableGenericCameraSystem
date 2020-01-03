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
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using IGCSClient.Classes;
using IGCSClient.GameSpecific.Classes;
using IGCSClient.Properties;

namespace IGCSClient.Forms
{
	public partial class MainForm : Form
	{
		public MainForm()
		{
			InitializeComponent();
			LogHandlerSingleton.Instance().Setup(_logControl);
			this.MinimumSize = this.Size;
		}


		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);
			this.Text = string.Format("{0} (v{1}). By {2}", GameSpecificConstants.ClientWindowTitle, GameSpecificConstants.CameraVersion, GameSpecificConstants.CameraCredits);
			FillAboutTab();

			// First setup the controls 
			_settingsEditor.Setup();
			_keyBindingsEditor.Setup();

			// then load the values from the ini file (if any) so the controls are already there.
			AppStateSingleton.Instance().LoadFromIni();
			AppStateSingleton.Instance().LoadRecentProcessList();
			MessageHandlerSingleton.Instance().ConnectedToNamedPipeFunc = () => HandleConnectedToPipe();
			MessageHandlerSingleton.Instance().ClientConnectionReceivedFunc = () => HandleConnectionReceived();
			_generalTabControl.UpdateSelectedRenderAPI();

			// Disable all tabs, except general, log and about.
			_hotsamplingTab.Enabled = false;
			_settingsTab.Enabled = false;
			_keyBindingsTab.Enabled = false;

			var notificationWindow = new NotificationWindow();
			MessageHandlerSingleton.Instance().NotificationLogFunc = s => notificationWindow.AddNotification(s);
			notificationWindow.Show(this);
		}


		protected override void OnClosing(CancelEventArgs e)
		{
			AppStateSingleton.Instance().SaveSettingsToIni();
			AppStateSingleton.Instance().SaveRecentProcessList();
			base.OnClosing(e);
		}

		

		private void HandleDllInjected()
		{
			// enable all tabs
			_hotsamplingTab.Enabled = true;
			_settingsTab.Enabled = true;
			_keyBindingsTab.Enabled = true;
			// show the resolutions on the hotsampling tab
			_hotsamplingControl.BindData();

			// Send preferred rendering API. 
			AppStateSingleton.Instance().PreferredRenderApiKind = _generalTabControl.SelectedRenderAPI;
			MessageHandlerSingleton.Instance().SendDXGIHookActionForPreferredRenderAPI(_generalTabControl.SelectedRenderAPI);
		}


		private void HandleConnectionReceived()
		{
			LogHandlerSingleton.Instance().LogLine("DLL Connected to our named pipe.", "System", true, true);

			_dllToClientConnectedSBLabel.Text = "Client->Camera dll active";
		}

		
		private void HandleConnectedToPipe()
		{
			LogHandlerSingleton.Instance().LogLine("Connected to the DLL's named pipe", "System", true, true);
			AppStateSingleton.Instance().SendSettings();
			AppStateSingleton.Instance().SendKeyBindings();
			LogHandlerSingleton.Instance().LogLine("Initial settings sent", "System", true, true);
			_clientToDllConnectedSBLabel.Text = "Camera dll->client active";
		}
		

		private void FillAboutTab()
		{
			_cameraAboutGroupBox.Text += GameSpecificConstants.ClientWindowTitle;
			_cameraAboutLabel.Text += GameSpecificConstants.CameraCredits;

			_aboutIGCSTextBox.Text+=@"This camera is powered by Injectable Generic Camera System (IGCS).
Copyright (c) 2020 Otis_Inf (Frans Bouma). All rights reserved.
https://github.com/FransBouma/InjectableGenericCameraSystem

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)";

			_aboutIGCSTextBox.Text += Environment.NewLine + Environment.NewLine + @"IGCS uses MinHook, copyright (c) 2009-2017 Tsuda Kageyu. All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)";

			_aboutIGCSTextBox.Text += Environment.NewLine + Environment.NewLine + @"Special thanks to: 
 1. Crosire, for creating and opening the Reshade sourcecode. It was a great help with grabbing screenshots on DX12!
 2. The users of my camera's, you all are a great inspiration to make the best camera tools possible!)";
		}

		
		private void _generalTabControl_DllInjected(object sender, EventArgs e)
		{
			HandleDllInjected();
		}


		private void _generalTabControl_AttachedProcessExited(object sender, EventArgs e)
		{
			// Attached process died, we should too
			this.Close();
		}


		private void _igcsSBLink_Click(object sender, EventArgs e)
		{
			Process.Start(ConstantsEnums.IGCSRootURL);
		}


		private void _aboutIGCSTextBox_LinkClicked(object sender, LinkClickedEventArgs e)
		{
			Process.Start(e.LinkText);
		}
	}
}
