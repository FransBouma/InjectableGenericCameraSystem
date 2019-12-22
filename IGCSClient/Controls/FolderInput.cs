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
using IGCSClient.Interfaces;
using SD.Tools.Algorithmia.GeneralDataStructures.EventArguments;
using SD.Tools.BCLExtensions.SystemRelated;

namespace IGCSClient.Controls
{
	public partial class FolderInput : UserControl, IInputControl<string>
	{
		#region Members
		private string _description;
		#endregion


		public event EventHandler ValueChanged;

		public FolderInput()
		{
			InitializeComponent();
			_description = string.Empty;
		}


		public void Setup(string initialFolder, string description)
		{
			_inputControl.Text = initialFolder;
			_description = description;
		}

		
		private void _inputControl_TextChanged(object sender, EventArgs e)
		{
			this.ValueChanged.RaiseEvent(this);
		}


		private void _browseButton_Click(object sender, EventArgs e)
		{
			if(!string.IsNullOrWhiteSpace(_inputControl.Text))
			{
				_folderBrowser.SelectedPath = _inputControl.Text;
			}

			if(!string.IsNullOrWhiteSpace(_description))
			{
				_folderBrowser.Description = _description;
			}

			var result = _folderBrowser.ShowDialog();
			if(result == DialogResult.OK)
			{
				_inputControl.Text = _folderBrowser.SelectedPath;
			}
		}


		#region Properties
		/// <inheritdoc/>
		public string Value
		{
			get { return _inputControl.Text; }
			set { _inputControl.Text = value; }
		}
		#endregion
	}
}
