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

namespace IGCSClient.Forms
{
	/// <summary>
	/// Code based on process selector from SRWE (https://github.com/dtgDTGdtg/SRWE )
	/// </summary>
	public partial class ProcessSelectorWPF : Window
	{
		private DataTable _boundProcessList;
		private List<string> _recentProcesses;

		public ProcessSelectorWPF(List<string> recentProcesses)
		{
			InitializeComponent();
			_recentProcesses = recentProcesses ?? new List<string>();

			_boundProcessList = new DataTable();
			var keyColumns = new DataColumn[1];
			keyColumns[0] = _boundProcessList.Columns.Add("ProcessID", typeof(int));
			_boundProcessList.PrimaryKey = keyColumns;
			_boundProcessList.Columns.Add("ModuleIcon", typeof(Image));
			_boundProcessList.Columns.Add("ProcessName");
			_boundProcessList.Columns.Add("WindowTitle");
			_boundProcessList.Columns.Add("FileName");


			//_processGrid.AutoGenerateColumns = false;
			_processGrid.ItemsSource = _boundProcessList.DefaultView;
		}


		private void ProcessSelectorWPF_OnLoaded(object sender, RoutedEventArgs e)
		{
			RefreshProcessList();
		}

		
		private void RefreshProcessList()
		{
			_boundProcessList.Clear();
			DataRow row;
			Process currentProcess = Process.GetCurrentProcess();

			foreach (Process process in Process.GetProcesses())
			{
				try
				{
					// only show processes with a window. This filters out the phantom exe's that spawn the actual game process as they don't have a window with a window title. 
					// E.g. games usually are started with an exe that logs into steam, egs etc. and then starts the actual exe which can have the same name. 
					if (process.SessionId == currentProcess.SessionId && process.Id != currentProcess.Id && !string.IsNullOrWhiteSpace(process.MainWindowTitle))
					{
						row = _boundProcessList.NewRow();
						row[0] = process.Id;
						//row[1] = System.Drawing.Icon.ExtractAssociatedIcon(process.MainModule.FileName).ToBitmap();
						row[2] = process.MainModule.ModuleName;
						row[3] = process.MainWindowTitle;
						row[4] = process.MainModule.FileName;

						if (IsRecentProcess(process.MainModule.ModuleName))
						{
							_boundProcessList.Rows.InsertAt(row, 0);
						}
						else
						{
							_boundProcessList.Rows.Add(row);
						}
					}
				}
				catch (Win32Exception ex)
				{
					if ((uint)ex.ErrorCode != 0x80004005)
					{
						throw;
					}
				}
			}
			//if (_processGrid.RowCount > 0)
			//{
			//	_processGrid.ClearSelection();
			//	_processGrid.CurrentCell = _processGrid.Rows[0].Cells[1];
			//}
		}


		private bool IsRecentProcess(string processName)
		{
			return _recentProcesses.Any(p=>!string.IsNullOrEmpty(p) && p.Equals(processName, StringComparison.CurrentCultureIgnoreCase));
		}


		private void _refreshButton_Click(object sender, RoutedEventArgs e)
		{
			RefreshProcessList();
		}

		private void _selectButton_Click(object sender, RoutedEventArgs e)
		{
			this.SelectedProcess = null;
			//if (_processGrid.SelectedRows.Count > 0)
			//{
			//	var selectedRow = (DataRowView)_processGrid.SelectedRows[0].DataBoundItem;

			//	if (this.SelectedProcess == null || this.SelectedProcess.HasExited || this.SelectedProcess.Id != (int)selectedRow[0])
			//	{
			//		this.SelectedProcess = Process.GetProcessById((int)selectedRow[0]);
			//	}
			//}
			this.DialogResult = true;
			this.Close();
		}

		private void _cancelButton_Click(object sender, RoutedEventArgs e)
		{
			this.DialogResult = false;
			this.Close();
		}
		

		#region Properties
		public Process SelectedProcess { get; private set; }
		#endregion
	}
}
