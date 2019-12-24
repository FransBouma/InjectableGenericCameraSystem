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
using SD.Tools.BCLExtensions.SystemRelated;

namespace IGCSClient.Controls
{
	/// <summary>
	/// Simple editor which can retrieve a key combination pressed by the user. 
	/// </summary>
	public partial class KeyCombinationEditor : UserControl, IInputControl<KeyCombination>
	{
		#region Members
		private KeyCombination _tmpCombination, _toEdit, _activeCombination;
		private bool _editing;
		#endregion

		public event EventHandler ValueChanged;

		private void _keyInputTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if(!_editing)
			{
				return;
			}

			if(e.KeyCode == Keys.Escape)
			{
				EndEditing();
				return;
			}

			if(e.KeyValue > 255 || e.KeyCode == Keys.ControlKey || e.KeyCode == Keys.Menu || e.KeyCode == Keys.ShiftKey)
			{
				// ignore
				return;
			}
			_tmpCombination.SetNewValues(e.Alt, e.Control, e.Shift, e.KeyValue);
			ReflectEditingState();
			e.Handled = true;
		}

		public KeyCombinationEditor()
		{
			InitializeComponent();
			_tmpCombination = new KeyCombination();
			_toEdit = new KeyCombination();
			_okButton.Visible = false;
			_cancelButton.Visible = false;
			_activeCombination = _toEdit;

#warning DEBUG CODE
			_toEdit.SetNewValues(true, false, false, 67);       //Alt-C
#warning END DEBUG CODE

			ReflectEditingState();
		}


		public void SetValueFromString(string valueAsString, KeyCombination defaultValue)
		{
#warning >>>>>>>>>>> IMPLEMENT
		}


		private void SwitchToEditModeIfNeeded()
		{
			if(_editing)
			{
				return;
			}

			StartEditing();
		}


		private void StartEditing()
		{
			_editing = true;
			_tmpCombination.Clear();
			_activeCombination = _tmpCombination;
			_cancelButton.Visible = true;
			_okButton.Visible = true;
			ReflectEditingState();
		}


		private void EndEditing()
		{
			if(!_editing)
			{
				return;
			}
			_activeCombination = _toEdit;
			_cancelButton.Visible = false;
			_okButton.Visible = false;
			_editing = false;
			ReflectEditingState();
			this.ValueChanged.RaiseEvent(this);
		}


		private void ReflectEditingState()
		{
			if(_activeCombination == null)
			{
				return;
			}
			_keyInputTextBox.Text = _activeCombination.ToString();
		}

		private void _cancelButton_Click(object sender, EventArgs e)
		{
			EndEditing();
		}

		private void KeyCombinationEditor_Leave(object sender, EventArgs e)
		{
			EndEditing();
		}

		private void _okButton_Click(object sender, EventArgs e)
		{
			_toEdit.CopyValues(_tmpCombination);
			EndEditing();
		}


		private void _keyInputTextBox_Click(object sender, EventArgs e)
		{
			SwitchToEditModeIfNeeded();
		}


		#region Properties
		public KeyCombination Value
		{
			get { return _toEdit;}
			set
			{
				_toEdit = value;
				ReflectEditingState();
			}
		}
		#endregion
	}
}
