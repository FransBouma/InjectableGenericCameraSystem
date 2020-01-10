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
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using IGCSClient.Classes;
using IGCSClient.GameSpecific.Classes;

namespace IGCSClient.Forms
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		public MainWindow()
		{
			InitializeComponent();
			LogHandlerSingleton.Instance().Setup(_logControl);
		}


		protected override void OnClosing(CancelEventArgs e)
		{
			AppStateSingleton.Instance().SaveSettingsToIni();
			AppStateSingleton.Instance().SaveRecentProcessList();
			base.OnClosing(e);
		}


		private void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
		{
			this.Title = string.Format("{0} (v{1}). By {2}", GameSpecificConstants.ClientWindowTitle, GameSpecificConstants.CameraVersion, GameSpecificConstants.CameraCredits);
			//FillAboutTab();

			// First setup the controls 
			//_settingsEditor.Setup();
			//_keyBindingsEditor.Setup();

			// then load the values from the ini file (if any) so the controls are already there.
			AppStateSingleton.Instance().LoadFromIni();
			AppStateSingleton.Instance().LoadRecentProcessList();
			MessageHandlerSingleton.Instance().ConnectedToNamedPipeFunc = () => HandleConnectedToPipe();
			MessageHandlerSingleton.Instance().ClientConnectionReceivedFunc = () => HandleConnectionReceived();
			_generalTabControl.UpdateSelectedRenderAPI();

			// Disable all tabs, except general, log and about.
			//_hotsamplingTab.Enabled = false;
			//_settingsTab.Enabled = false;
			//_keyBindingsTab.Enabled = false;

			//var notificationWindow = new NotificationWindow();
			//MessageHandlerSingleton.Instance().NotificationLogFunc = s => notificationWindow.AddNotification(s);
			//notificationWindow.Show(this);

		}
		

		private void HandleDllInjected()
		{
			// enable all tabs
			//_hotsamplingTab.Enabled = true;
			//_settingsTab.Enabled = true;
			//_keyBindingsTab.Enabled = true;
			//// show the resolutions on the hotsampling tab
			//_hotsamplingControl.BindData();

			//// Send preferred rendering API. 
			AppStateSingleton.Instance().PreferredRenderApiKind = _generalTabControl.SelectedRenderAPI;
			MessageHandlerSingleton.Instance().SendDXGIHookActionForPreferredRenderAPI(_generalTabControl.SelectedRenderAPI);
		}


		private void HandleConnectionReceived()
		{
			LogHandlerSingleton.Instance().LogLine("DLL Connected to our named pipe.", "System", true);
			SetTextOnTextBlock(_clientToDllConnectedSBLabel, "Client->Camera dll active");
		}

		
		private void HandleConnectedToPipe()
		{
			LogHandlerSingleton.Instance().LogLine("Connected to the DLL's named pipe", "System", true);
#warning >>>>>>>>>>>> IMPLEMENT
			//AppStateSingleton.Instance().SendSettings();
			//AppStateSingleton.Instance().SendKeyBindings();
			LogHandlerSingleton.Instance().LogLine("Initial settings sent", "System", true);
			SetTextOnTextBlock(_dllToClientConnectedSBLabel, "Camera dll->client active");
		}


		private void SetTextOnTextBlock(TextBlock toSet, string toPass)
		{
			if(toSet == null)
			{
				return;
			}
			if(toSet.CheckAccess())
			{
				toSet.Text = toPass;
			}
			else
			{
				toSet.Dispatcher?.Invoke(()=>toSet.Text = toPass);
			}
		}

		private void Hyperlink_OnRequestNavigate(object sender, RequestNavigateEventArgs e)
		{
			Process.Start(e.Uri.ToString());
		}


		private void _generalTabControl_OnDllInjected(object sender, EventArgs e)
		{
			HandleDllInjected();
		}


		private void _generalTabControl_OnAttachedProcessExited(object sender, EventArgs e)
		{
			// Attached process died, we should too
			this.Close();
		}
	}
}
