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
using IGCSClient.Interfaces;

namespace IGCSClient.Classes
{
	/// <summary>
	/// Singleton class containing the single appstate instance which contains the state of this application at runtime. 
	/// </summary>
	public static class AppStateSingleton
	{
		private static AppState _instance = new AppState();

		/// <summary>Dummy static constructor to make sure threadsafe initialization is performed.</summary>
		static AppStateSingleton() {}

		/// <summary>
		/// Gets the single instance in use in this application
		/// </summary>
		/// <returns></returns>
		public static AppState Instance() => _instance;
	}

	/// <summary>
	/// The class containing the actual application state at runtime.
	/// </summary>
	public class AppState
	{
		#region Members
		private readonly List<ISetting> _settings;
		private readonly List<KeyBindingSetting> _keyBindings;
		#endregion

		internal AppState()
		{
			_settings = new List<ISetting>();
			_keyBindings = new List<KeyBindingSetting>();
		}


		public void AddSetting(ISetting settingToAdd)
		{
			_settings.Add(settingToAdd);
		}


		public void AddKeyBinding(KeyBindingSetting bindingToAdd)
		{
			_keyBindings.Add(bindingToAdd);
		}


		public void WriteToIni()
		{
			var iniFile = new IniFileHandler(ConstantsEnums.IniFilename);
			foreach(var setting in _settings)
			{
				iniFile.Write(setting.Name, setting.GetValueAsString(), "CameraSettings");
			}

			foreach(ISetting binding in _keyBindings)
			{
				iniFile.Write(binding.Name, binding.GetValueAsString(), "KeyBindings");
			}
		}


		public void LoadFromIni()
		{
			var iniFile = new IniFileHandler(ConstantsEnums.IniFilename);
			if(iniFile.FileExists())
			{
				foreach(var setting in _settings)
				{
					setting.SetValueFromString(iniFile.Read(setting.Name, "CameraSettings"));
				}

				foreach(var binding in _keyBindings)
				{
					binding.SetValueFromString(iniFile.Read(binding.Name, "KeyBindings"));
				}
			}
		}


		#region Properties
		public ISetting[] Settings
		{
			get { return _settings.ToArray(); }
		}

		public ISetting[] KeyBindings
		{
			get { return _keyBindings.ToArray(); }
		}
		#endregion
	}
}
