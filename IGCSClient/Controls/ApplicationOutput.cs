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
using System.Drawing;
using System.Windows.Forms;

namespace IGCSClient.Controls
{
	/// <summary>
	/// General output window for application feedback.
	/// </summary>
	public partial class ApplicationOutput : UserControl
	{
		/// <summary>
		/// Initializes a new instance of the <see cref="ApplicationOutput"/> class.
		/// </summary>
		public ApplicationOutput()
		{
			InitializeComponent();
		}


		/// <summary>
		/// Logs the line in LineToLog in the output window, no verbose specification, which means that the line is logged
		/// even when the verbose checkbox is disabled. Always appends a newline.
		/// </summary>
		/// <param name="lineToLog">Line to log which can contain format characters</param>
		/// <param name="source">Source description of the line</param>
		/// <param name="args">The args to pass to the string formatter.</param>
		public void LogLine(string lineToLog, string source, params object[] args)
		{
			LogLine(lineToLog, source, false, true, false, args);
		}


		/// <summary>
		/// Logs the line in LineToLog in the output window, based on the verbose setting isVerboseMessage, which means that the line is not logged
		/// when the verbose checkbox is disabled if isVerboseMessage is set to true. Always appends a newline.
		/// </summary>
		/// <param name="lineToLog">Line to log which can contain format characters</param>
		/// <param name="source">Source description of the line</param>
		/// <param name="isVerboseMessage">Flag to signal if the line is a VerboseMessage, which means it is only logged when the Verbose checkbox is set</param>
		/// <param name="args">The args to pass to the string formatter.</param>
		public void LogLine(string lineToLog, string source, bool isVerboseMessage, params object[] args)
		{
			LogLine(lineToLog, source, isVerboseMessage, true, false, args);
		}



		/// <summary>
		/// Logs the given line to the output window. Based on the verbose checkbox and the VerboseMessage flag the message is logged or not.
		/// If the verbose checkbox is set, also lines with VerboseMessage=true will be logged, otherwise these messages will be suppressed.
		/// Appends a newline if AppendNewLine is set to true.
		/// </summary>
		/// <param name="lineToLog">Line to log which can contain format characters</param>
		/// <param name="source">Source description of the line</param>
		/// <param name="isVerboseMessage">Flag to signal if the line is a VerboseMessage, which means it is only logged when the Verbose checkbox is set</param>
		/// <param name="appendNewLine">Flag to signal if a Newline should be appended to the line.</param>
		/// <param name="args">The args to pass to the string formatter.</param>
		public void LogLine(string lineToLog, string source, bool isVerboseMessage, bool appendNewLine, params object[] args)
		{
			LogLine(lineToLog, source, isVerboseMessage, appendNewLine, false, args);
		}


		/// <summary>
		/// Logs the given line to the output window. Based on the verbose checkbox and the VerboseMessage flag the message is logged or not.
		/// If the verbose checkbox is set, also lines with VerboseMessage=true will be logged, otherwise these messages will be suppressed.
		/// Appends a newline if AppendNewLine is set to true.
		/// </summary>
		/// <param name="lineToLog">Line to log which can contain format characters</param>
		/// <param name="source">Source description of the line</param>
		/// <param name="isVerboseMessage">Flag to signal if the line is a VerboseMessage, which means it is only logged when the Verbose checkbox is set</param>
		/// <param name="appendNewLine">Flag to signal if a Newline should be appended to the line.</param>
		/// <param name="isError">if set to <c>true</c> [is error].</param>
		/// <param name="args">The args to pass to the string formatter.</param>
		public void LogLine(string lineToLog, string source, bool isVerboseMessage, bool appendNewLine, bool isError, params object[] args)
		{
			if( (!_verboseCheckBox.Checked) && isVerboseMessage)
			{
				// should not be logged
				return;
			}

			string crlf = string.Empty;
			if(appendNewLine)
			{
				crlf = Environment.NewLine;
			}
			Font currentFont = _outputTextBox.Font;
			// display Source
			if(!string.IsNullOrEmpty(source))
			{
				_outputTextBox.SelectionFont = new Font(currentFont, FontStyle.Bold);
				_outputTextBox.AppendText(string.Format("{0}::", source));
				_outputTextBox.SelectionFont = currentFont;
			}

			var currentColor = _outputTextBox.SelectionColor;
			if(isError)
			{
				_outputTextBox.SelectionColor = Color.Red;
			}
			string stringToLog = lineToLog;
			if(args.Length>0)
			{
				stringToLog = string.Format(lineToLog, args);
			}
			_outputTextBox.AppendText(string.Format("{0}{1}", stringToLog, crlf));
			_outputTextBox.SelectionColor = currentColor;
		}


		/// <summary>
		/// Handles the Click event of the btnClear control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
		private void btnClear_Click(object sender, EventArgs e)
		{
			_outputTextBox.Clear();
			Application.DoEvents();
		}
	}
}
