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
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using IGCSClient.Classes;
using IGCSClient.Forms;
using IGCSClient.GameSpecific.Classes;

namespace IGCSClient
{
	static class Program
	{
		private static MainForm _mainWindow;

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			Application.SetUnhandledExceptionMode(UnhandledExceptionMode.Automatic, true);
			// wire event handler for unhandled exceptions, so these will be shown in our own exception viewer.
			Application.ThreadException += new ThreadExceptionEventHandler(Application_ThreadException);
			AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CurrentDomain_UnhandledException);

			_mainWindow = new MainForm();
			Init();
			Application.Run(_mainWindow);
		}


		private static void Init()
		{
			SettingFactory.InitializeSettings();
			SettingFactory.InitializeKeyBindings();
		}


		/// <summary>
		/// Handles the ThreadException event of the Application control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="System.Threading.ThreadExceptionEventArgs"/> instance containing the event data.</param>
		private static void Application_ThreadException(object sender, ThreadExceptionEventArgs e)
		{
			if(_mainWindow == null)
			{
				SimpleExceptionViewer.Show(null, e.Exception, "An Unhandled exception occurred.");
			}
			else
			{
				_mainWindow.Invoke(new Action<Exception>((a) => SimpleExceptionViewer.Show(null, a, "An Unhandled exception occurred.")), new object[] { e.Exception });
			}
		}


		/// <summary>
		/// Handles the UnhandledException event of the CurrentDomain control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="System.UnhandledExceptionEventArgs"/> instance containing the event data.</param>
		private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
		{
			if(_mainWindow == null)
			{
				SimpleExceptionViewer.Show(null, (Exception)e.ExceptionObject, "An unhandled exception occurred.");
			}
			else
			{
				_mainWindow.Invoke(new Action<Exception>((a) => SimpleExceptionViewer.Show(null, a, "An Unhandled exception occurred.")), 
								   new object[] { (Exception)e.ExceptionObject });
			}
		}
	}
}
