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
using IGCSClient.Properties;

namespace IGCSClient.Forms
{
	/// <summary>
	/// Generic Exception viewer.
	/// </summary>
	internal class ExceptionViewer : System.Windows.Forms.Form
	{
		#region Class Member Declaration
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Button _closeButton;
		private Exception _exception;
		private System.Windows.Forms.RichTextBox _stackTraceTextBox;
		private System.Windows.Forms.RichTextBox _messageTextBox;
		private System.ComponentModel.Container components = null;
		#endregion

		public ExceptionViewer()
		{
			InitializeComponent();
		}

		public ExceptionViewer(Exception exceptionToView)
		{
			InitializeComponent();
			_exception = exceptionToView;
			ViewExceptionInWindow();
		}


		/// <summary>
		/// Raises the <see cref="E:System.Windows.Forms.Form.Load"/> event.
		/// </summary>
		/// <param name="e">An <see cref="T:System.EventArgs"/> that contains the event data.</param>
		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);
			this.MinimumSize = this.Size;
		}


		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		
		/// <summary>
		/// Purpose: will load the private membervariable m_kexException's data
		/// into the windows' controls and thus view it. Set m_kexException with the
		/// property kexException.
		/// </summary>
		public void ViewExceptionInWindow()
		{
			if(_exception==null)
			{
				// no exception loaded
				_messageTextBox.Text = "No exception loaded into window: can't visualize exception";
				return;
			}
			// visualize the exception.
			_stackTraceTextBox.SelectionBullet=true;
			_messageTextBox.SelectionBullet=true;
			_messageTextBox.Text += _exception.Message + Environment.NewLine;
			_stackTraceTextBox.Text += "-----[Core exception]--------------------" + Environment.NewLine;
			_stackTraceTextBox.Text += _exception.StackTrace + Environment.NewLine;
			// go into the inner exceptions.
			for(Exception innerException = _exception.InnerException;
				innerException!=null;innerException = innerException.InnerException)
			{
				_messageTextBox.Text += innerException.Message + Environment.NewLine;
				_stackTraceTextBox.Text += "-----[InnerException]--------------------" + Environment.NewLine;
				_stackTraceTextBox.Text += innerException.StackTrace + Environment.NewLine;
			}
			// strip extra newline from boxes
			_messageTextBox.Text = _messageTextBox.Text.Substring(0,_messageTextBox.Text.Length-1);
			_stackTraceTextBox.Text = _stackTraceTextBox.Text.Substring(0,_stackTraceTextBox.Text.Length-1);
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ExceptionViewer));
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this._messageTextBox = new System.Windows.Forms.RichTextBox();
			this._stackTraceTextBox = new System.Windows.Forms.RichTextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this._closeButton = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Anchor = System.Windows.Forms.AnchorStyles.Top;
			this.label1.BackColor = System.Drawing.Color.White;
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(2, 4);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(520, 34);
			this.label1.TabIndex = 0;
			this.label1.Text = "An exception occured. Below is the general information about this exception.";
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this._messageTextBox);
			this.groupBox1.Controls.Add(this._stackTraceTextBox);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Location = new System.Drawing.Point(4, 43);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(515, 437);
			this.groupBox1.TabIndex = 1;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = ".NET Exception information";
			// 
			// _messageTextBox
			// 
			this._messageTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._messageTextBox.BulletIndent = 10;
			this._messageTextBox.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this._messageTextBox.Location = new System.Drawing.Point(8, 36);
			this._messageTextBox.Name = "_messageTextBox";
			this._messageTextBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
			this._messageTextBox.Size = new System.Drawing.Size(501, 88);
			this._messageTextBox.TabIndex = 2;
			this._messageTextBox.Text = "";
			// 
			// _stackTraceTextBox
			// 
			this._stackTraceTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._stackTraceTextBox.BackColor = System.Drawing.SystemColors.Window;
			this._stackTraceTextBox.BulletIndent = 10;
			this._stackTraceTextBox.CausesValidation = false;
			this._stackTraceTextBox.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this._stackTraceTextBox.Location = new System.Drawing.Point(8, 148);
			this._stackTraceTextBox.Name = "_stackTraceTextBox";
			this._stackTraceTextBox.ReadOnly = true;
			this._stackTraceTextBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
			this._stackTraceTextBox.Size = new System.Drawing.Size(501, 283);
			this._stackTraceTextBox.TabIndex = 3;
			this._stackTraceTextBox.Text = "";
			// 
			// label5
			// 
			this.label5.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.label5.Location = new System.Drawing.Point(8, 132);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(188, 16);
			this.label5.TabIndex = 2;
			this.label5.Text = "Stack trace";
			// 
			// label4
			// 
			this.label4.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.label4.Location = new System.Drawing.Point(8, 20);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(64, 16);
			this.label4.TabIndex = 0;
			this.label4.Text = "Message";
			// 
			// label3
			// 
			this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.label3.BackColor = System.Drawing.Color.White;
			this.label3.Location = new System.Drawing.Point(0, 0);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(525, 40);
			this.label3.TabIndex = 3;
			// 
			// _closeButton
			// 
			this._closeButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this._closeButton.Location = new System.Drawing.Point(227, 486);
			this._closeButton.Name = "_closeButton";
			this._closeButton.Size = new System.Drawing.Size(72, 24);
			this._closeButton.TabIndex = 5;
			this._closeButton.Text = "Close";
			this._closeButton.Click += new System.EventHandler(this._closeButton_Click);
			// 
			// ExceptionViewer
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(525, 516);
			this.Controls.Add(this._closeButton);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ExceptionViewer";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Exception Viewer";
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void _closeButton_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}
		
		/// <summary>
		/// Purpose: sets the membervariable _exception which is visualized
		/// in the windows controls using ViewExceptionInWindow()
		/// </summary>
		public Exception ExceptionToView
		{
			set
			{
				_exception = value;
			}
		}
	}
}
