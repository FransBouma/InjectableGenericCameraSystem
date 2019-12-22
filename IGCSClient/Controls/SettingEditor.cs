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

namespace IGCSClient.Controls
{
	public partial class SettingEditor : UserControl
	{
		public SettingEditor()
		{
			InitializeComponent();
		}


		public void BuildUI(List<ISetting> settingsToShow)
		{
			var settingsByGroup = settingsToShow.GroupBy(s => s.GroupLabel);
			foreach(var g in settingsByGroup)
			{
				// add a group control
				var groupControl = new GroupBox() {AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, Dock = DockStyle.Top, Text = g.Key};
				var table = new TableLayoutPanel() { AutoSizeMode = AutoSizeMode.GrowAndShrink, AutoSize = true, Dock = DockStyle.Fill};
				table.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
				table.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
				table.ColumnCount = 2;
				table.RowCount = 1;

				foreach(var s in g)
				{
					table.RowCount++;
					var label = new Label() { Text = s.Label, TextAlign = ContentAlignment.MiddleLeft, Dock = DockStyle.Top, AutoSize = true, Anchor = AnchorStyles.None};
					table.Controls.Add(label);
					table.Controls.Add(s.InputControl);
					table.SetCellPosition(label, new TableLayoutPanelCellPosition(0, table.RowCount-1));
					s.InputControl.Dock = DockStyle.Top;
					s.InputControl.Anchor = AnchorStyles.Left;
					table.SetCellPosition(s.InputControl, new TableLayoutPanelCellPosition(1, table.RowCount-1));
				}
				groupControl.Controls.Add(table);
				this.Controls.Add(groupControl);
			}
		}
	}

}
