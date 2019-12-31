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
using System.Text;
using System.Windows.Forms;
using IGCSClient.Properties;

namespace IGCSClient.Forms
{
	/// <summary>
	/// Simple dialog which is used to display a less intrusive exception dialog to the user. 
	/// </summary>
	public partial class SimpleExceptionViewer : Form
	{
		#region Class Member Declarations
		private readonly Exception _toView;
		#endregion

		/// <summary>
		/// Initializes a new instance of the <see cref="SimpleExceptionViewer"/> class.
		/// </summary>
		/// <param name="toView">To view.</param>
		/// <param name="caption">The caption.</param>
		public SimpleExceptionViewer(Exception toView, string caption)
		{
			InitializeComponent();
			_toView = toView;
			if(!string.IsNullOrEmpty(caption))
			{
				this.Text = caption;
			}
		}


		/// <summary>
		/// Shows a new instance of SimpleExceptionViewer with the exception specified
		/// </summary>
		/// <param name="parent">The parent.</param>
		/// <param name="toView">To view.</param>
		/// <param name="caption">The caption.</param>
		public static void Show(IWin32Window parent, Exception toView, string caption)
		{
			using(SimpleExceptionViewer viewer = new SimpleExceptionViewer(toView, caption))
			{
				viewer.ShowDialog(parent);
			}
		}
		

		/// <summary>
		/// Raises the <see cref="E:System.Windows.Forms.Form.Load"/> event.
		/// </summary>
		/// <param name="e">An <see cref="T:System.EventArgs"/> that contains the event data.</param>
		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);
			_detailsButton.Enabled = (_toView != null);
			if(_toView == null)
			{
				return;
			}
			_exceptionMessageTextBox.Text += "Exception message:" + Environment.NewLine + "-------------------------------" + Environment.NewLine 
						+ "Exception type: " + _toView.GetType().Name + Environment.NewLine + _toView.Message + Environment.NewLine + Environment.NewLine;
			AddInnerExceptionMessage(_toView.InnerException);
			_exceptionMessageTextBox.SelectionLength = 0;
			_exceptionMessageTextBox.SelectedText = string.Empty;
		}


		/// <summary>
		/// Adds the inner exception message.
		/// </summary>
		/// <param name="innerException">The inner exception.</param>
		private void AddInnerExceptionMessage(Exception innerException)
		{
			if(innerException == null)
			{
				return;
			}
			_exceptionMessageTextBox.Text += "Inner Exception message:" + Environment.NewLine + "-------------------------------" + Environment.NewLine + 
									innerException.Message + Environment.NewLine + Environment.NewLine;
			AddInnerExceptionMessage(innerException.InnerException);
		}


		/// <summary>
		/// Handles the copy to clip board.
		/// </summary>
		private void HandleCopyToClipBoard()
		{
			var builder = new StringBuilder();
			builder.AppendFormat("Exception information.{0}", Environment.NewLine);
			builder.AppendFormat("============================================================={0}", Environment.NewLine);
			ExceptionToString(builder, _toView);
			Clipboard.SetText(builder.ToString());
			MessageBox.Show(this, "Exception details copied to clipboard", "Copy succeeded", MessageBoxButtons.OK, MessageBoxIcon.Information);
		}


		/// <summary>
		/// Exceptions to string.
		/// </summary>
		/// <param name="toAppendTo">To append to.</param>
		/// <param name="ex">The ex.</param>
		private static void ExceptionToString(StringBuilder toAppendTo, Exception ex)
		{
			toAppendTo.AppendFormat("Exception details:{0}", Environment.NewLine);
			toAppendTo.AppendFormat("====================={0}", Environment.NewLine);
			toAppendTo.AppendFormat("Message: {0}{1}", ex.Message, Environment.NewLine);
			toAppendTo.AppendFormat("Source: {0}{1}", ex.Source, Environment.NewLine);
			toAppendTo.AppendFormat("Stack trace: {1}{0}{1}{1}", ex.StackTrace, Environment.NewLine);
			if(ex.InnerException != null)
			{
				toAppendTo.AppendFormat("Inner exception:-----------------------{0}", Environment.NewLine);
				ExceptionToString(toAppendTo, ex.InnerException);
				toAppendTo.AppendFormat("---------------------------------------{0}", Environment.NewLine);
			}
			else
			{
				toAppendTo.AppendFormat("Inner exception: <null>{0}", Environment.NewLine);
			}
		}


		/// <summary>
		/// Handles the Click event of the _okButton control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
		private void _okButton_Click(object sender, EventArgs e)
		{
			this.Close();
		}


		/// <summary>
		/// Handles the Click event of the _detailsButton control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
		private void _detailsButton_Click(object sender, EventArgs e)
		{
			using(ExceptionViewer viewer = new ExceptionViewer(_toView))
			{
				viewer.ShowDialog(this);
			}
		}

		/// <summary>
		/// Handles the Click event of the _copyToClipBoardButton control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
		private void _copyToClipBoardButton_Click(object sender, EventArgs e)
		{
			HandleCopyToClipBoard();
		}
	}
}
