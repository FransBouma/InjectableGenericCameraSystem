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
using System.Windows.Forms;
using IGCSClient.Interfaces;
using SD.Tools.Algorithmia.UtilityClasses;

namespace IGCSClient.Classes
{
	/// <summary>
	/// Defines a setting which is edited in the client and used in the system on the other side of the named pipe.
	/// </summary>
	public class Setting<T> : ISetting
	{
		#region Members
		public IInputControl<T> _inputControl;
		#endregion


		public void Setup(IInputControl<T> controlToUse)
		{
			ArgumentVerifier.CantBeNull(controlToUse, nameof(controlToUse));
			_inputControl = controlToUse;
			_inputControl.ValueChanged += _inputControl_ValueChanged;
		}


		private void _inputControl_ValueChanged(object sender, EventArgs e)
		{
#warning TODO: Implement message passing of new value to the named pipe.
			LogHandlerSingleton.Instance().LogLine("Value of {0} changed to: {1}", this.Label, false, true, this.ID, this.Value);
		}


		#region Properties
		/// <summary>
		/// The ID of the setting which is the same for both the client and the system on the other side of the named pipe.
		/// </summary>
		public byte ID { get; set; }
		/// <summary>
		/// The label of the group this setting belongs to. Used to group controls in the GUI.
		/// </summary>
		public string GroupLabel { get; set; }
		/// <summary>
		/// The label to use for editing this setting.
		/// </summary>
		public string Label { get; set; }
		/// <summary>
		/// The input control to use for this setting. 
		/// </summary>
		public Control InputControl
		{
			get { return _inputControl as Control; }
		}
		/// <summary>
		/// The current value of the setting as known by this client.
		/// </summary>
		public T Value
		{
			get { return _inputControl.Value; }
		}
		#endregion
	}
}
