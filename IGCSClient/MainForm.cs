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
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using IGCSClient.Classes;
using IGCSClient.Controls;
using IGCSClient.Interfaces;
using IGCSClient.NamedPipeSubSystem;
using SD.Tools.Algorithmia.GeneralDataStructures.EventArguments;

namespace IGCSClient
{
	public partial class MainForm : Form
	{
		private NamedPipeServer _pipeServer;

		public MainForm()
		{
			InitializeComponent();
			LogHandlerSingleton.Instance().Setup(_logControl);
		}


		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);
			_pipeServer = new NamedPipeServer(ConstantsEnums.NamedPipeName);
			_pipeServer.MessageReceived += _pipeServer_MessageReceived;
			LogHandlerSingleton.Instance().LogLine("Named pipe enabled.", "System");
			AddDummySettings();
		}

// TEST CODE BELOW THIS LINE


		private void AddDummySettings()
		{
			var settings = new List<ISetting>();
			settings.Add(CreateSetting("General camera settings", 1, "Rotation speed", CreateFloatInput(0.0M, 3.0M, 2, 0.1M, 0.1f)));
			settings.Add(CreateSetting("General camera settings", 2, "Movement speed", CreateFloatInput(0.0M, 15.0M, 3, 0.01M, 0.001f)));
			settings.Add(CreateSetting("General camera settings", 3, "Slowness factor", CreateFloatInput(0.0M, 3.0M, 2, 0.1M, 0.5f)));

			settings.Add(CreateSetting("Complex settings", 4, "Move camera when this menu is up", CreateBoolInput(true)));
			_settingsEditor.BuildUI(settings);
		}


		private IInputControl<bool> CreateBoolInput(bool value)
		{
			var toReturn = new BoolInput() {Value = value};
			return toReturn;
		}


		private IInputControl<float> CreateFloatInput(decimal minValue, decimal maxValue, int scale, decimal increment, float value)
		{
			var toReturn = new FloatInput();
			toReturn.Setup(minValue, maxValue, scale, increment);
			toReturn.Value = value;
			return toReturn;
		}

		private Setting<T> CreateSetting<T>(string groupLabel, byte id, string label, IInputControl<T> inputControl)
		{
			var toReturn = new Setting<T>() {GroupLabel = groupLabel, ID = id, Label = label};
			toReturn.Setup(inputControl);
			return toReturn;
		}


		private void HandleMessageReceived(ContainerEventArgs<byte[]> e)
		{
			byte[] value = e.Value;

			LogHandlerSingleton.Instance().LogLine("Message received. Length of value: {0}", "Named pipe", value.Length);

			// for now, assume a string
			string valueAsString = new ASCIIEncoding().GetString(value);
			LogHandlerSingleton.Instance().LogLine("Value: {0}", "Named pipe", valueAsString);
		}


		private void _pipeServer_MessageReceived(object sender, ContainerEventArgs<byte[]> e)
		{
			HandleMessageReceived(e);
		}
	}
}
