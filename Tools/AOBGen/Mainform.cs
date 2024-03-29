﻿// AOBGen. (c) Frans Bouma
// License: MIT
// https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Tools/AOBGen
//////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AOBGen
{
	public partial class _mainForm : Form
	{
		private AOBGenerator _aobGen;


		public _mainForm()
		{
			InitializeComponent();

			_aobGen = new AOBGenerator();
			this.Text += string.Format(" v{0}", this.GetType().Assembly.GetName().Version.ToString(3));
		}


		private void CopyAOBToClipboard()
		{
			if(string.IsNullOrWhiteSpace(_aobResultTextBox.Text))
			{
				return;
			}
			Clipboard.SetDataObject(_aobResultTextBox.Text.Trim());
			_resultSBPanel.Text = "AOB string copied!";
		}


		private void CopyAOBAndCodeAsMarkdown()
		{
			var sb = new StringBuilder();
			string template = @"```
{0}
```

AOB String:
```
{1}
```
";
			sb.AppendFormat(template, GetCodeTextBlockForMarkdown(), _aobResultTextBox.Text);
			Clipboard.SetDataObject(sb.ToString());
			_resultSBPanel.Text = "AOB and asm copied a markdown!";
		}


		private string GetCodeTextBlockForMarkdown()
		{
			if(_codeTextBox.SelectionLength <= 0 || !_markAOBLinesInAsmCheckbox.Checked)
			{
				return StripOffLastEmptyLine(_codeTextBox.Text);
			}

			var sb = new StringBuilder();
			sb.Append(_codeTextBox.Text.Substring(0, _codeTextBox.SelectionStart));
			sb.Append("---------------[AOB Start]--------------------");
			sb.Append(Environment.NewLine);
			sb.Append(_codeTextBox.Text.Substring(_codeTextBox.SelectionStart, _codeTextBox.SelectionLength));
			sb.Append("---------------[AOB End]----------------------");
			sb.Append(Environment.NewLine);
			sb.Append(_codeTextBox.Text.Substring(_codeTextBox.SelectionStart + _codeTextBox.SelectionLength));

			return StripOffLastEmptyLine(sb.ToString());
		}


		private string StripOffLastEmptyLine(string toClean)
		{
			return toClean.TrimEnd('\r', '\n');
		}


		private void GenerateAOB()
		{
			_resultSBPanel.Text = string.Empty;

			var toParse = string.IsNullOrEmpty(_codeTextBox.SelectedText) ? _codeTextBox.Text : _codeTextBox.SelectedText;
			if(string.IsNullOrEmpty(toParse))
			{
				return;
			}
			_aobResultTextBox.Text = _aobGen.GenerateAOB(toParse, _alsoWildcardOffsetsCheckBox.Checked);
		}


		private void _closeButton_Click(object sender, EventArgs e)
		{
			this.Close();
		}


		private void _copyAOBToClipboardButton_Click(object sender, EventArgs e)
		{
			CopyAOBToClipboard();
		}


		private void _copyAOBAndCodeAsMarkdownButton_Click(object sender, EventArgs e)
		{
			CopyAOBAndCodeAsMarkdown();
		}


		private void _codeTextBox_TextChanged(object sender, EventArgs e)
		{
			// pad the text with a CRLF if it's not there
			if(!_codeTextBox.Text.EndsWith("\n") && !_codeTextBox.Text.EndsWith("\r\n"))
			{
				var selectionStart = _codeTextBox.SelectionStart;
				_codeTextBox.Text += Environment.NewLine;
				_codeTextBox.SelectionStart = selectionStart;
				_codeTextBox.SelectionLength = 0;
			}
			GenerateAOB();
		}


		private void _codeTextBox_SelectionChanged(object sender, EventArgs e)
		{
			if(_codeTextBox.SelectedText.Length > 0)
			{
				// make sure only full lines are selected
				var selectionStartLineNo = _codeTextBox.GetLineFromCharIndex(_codeTextBox.SelectionStart);
				var selectionEndLineNo = _codeTextBox.GetLineFromCharIndex(_codeTextBox.SelectionStart + _codeTextBox.SelectionLength);
				if(selectionEndLineNo == selectionStartLineNo)
				{
					selectionEndLineNo++;
				}

				var selectionStartIndex = _codeTextBox.GetFirstCharIndexFromLine(selectionStartLineNo);
				var selectionEndIndex = _codeTextBox.GetFirstCharIndexFromLine(selectionEndLineNo);
				if(selectionEndIndex < 0)
				{
					selectionEndIndex = _codeTextBox.TextLength-1;
				}
				if(selectionEndIndex < selectionStartIndex)
				{
					(selectionStartIndex, selectionEndIndex) = (selectionEndIndex, selectionStartIndex);
				}
				_codeTextBox.Select(selectionStartIndex, selectionEndIndex-selectionStartIndex);
			}

			GenerateAOB();
		}

		private void _alsoWildcardOffsetsCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			GenerateAOB();
		}

		private void _linkSBPanel_Click(object sender, EventArgs e)
		{
			Process.Start(_linkSBPanel.Text);
		}
	}
}
