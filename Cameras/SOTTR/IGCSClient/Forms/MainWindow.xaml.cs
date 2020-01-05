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
			//_generalTabControl.UpdateSelectedRenderAPI();

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
			//AppStateSingleton.Instance().PreferredRenderApiKind = _generalTabControl.SelectedRenderAPI;
			//MessageHandlerSingleton.Instance().SendDXGIHookActionForPreferredRenderAPI(_generalTabControl.SelectedRenderAPI);
		}


		private void HandleConnectionReceived()
		{
			LogHandlerSingleton.Instance().LogLine("DLL Connected to our named pipe.", "System", true, true);

			//_dllToClientConnectedSBLabel.Text = "Client->Camera dll active";
		}

		
		private void HandleConnectedToPipe()
		{
			LogHandlerSingleton.Instance().LogLine("Connected to the DLL's named pipe", "System", true, true);
			AppStateSingleton.Instance().SendSettings();
			AppStateSingleton.Instance().SendKeyBindings();
			LogHandlerSingleton.Instance().LogLine("Initial settings sent", "System", true, true);
			//_clientToDllConnectedSBLabel.Text = "Camera dll->client active";
		}


		private void Hyperlink_OnRequestNavigate(object sender, RequestNavigateEventArgs e)
		{
			Process.Start(e.Uri.ToString());
		}
	}
}
