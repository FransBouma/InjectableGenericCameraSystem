namespace IGCSClient.Forms
{
	partial class SimpleExceptionViewer
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if(disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this._exceptionMessageTextBox = new System.Windows.Forms.TextBox();
			this._okButton = new System.Windows.Forms.Button();
			this._detailsButton = new System.Windows.Forms.Button();
			this._copyToClipBoardButton = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this._exceptionMessageTextBox);
			this.groupBox1.Location = new System.Drawing.Point(3, 4);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(537, 191);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "General exception information";
			// 
			// _exceptionMessageTextBox
			// 
			this._exceptionMessageTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this._exceptionMessageTextBox.Location = new System.Drawing.Point(3, 16);
			this._exceptionMessageTextBox.Multiline = true;
			this._exceptionMessageTextBox.Name = "_exceptionMessageTextBox";
			this._exceptionMessageTextBox.ReadOnly = true;
			this._exceptionMessageTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this._exceptionMessageTextBox.Size = new System.Drawing.Size(531, 172);
			this._exceptionMessageTextBox.TabIndex = 0;
			this._exceptionMessageTextBox.TabStop = false;
			// 
			// _okButton
			// 
			this._okButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this._okButton.Location = new System.Drawing.Point(234, 201);
			this._okButton.Name = "_okButton";
			this._okButton.Size = new System.Drawing.Size(75, 23);
			this._okButton.TabIndex = 0;
			this._okButton.Text = "OK";
			this._okButton.UseVisualStyleBackColor = true;
			this._okButton.Click += new System.EventHandler(this._okButton_Click);
			// 
			// _detailsButton
			// 
			this._detailsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._detailsButton.Location = new System.Drawing.Point(340, 201);
			this._detailsButton.Name = "_detailsButton";
			this._detailsButton.Size = new System.Drawing.Size(75, 23);
			this._detailsButton.TabIndex = 1;
			this._detailsButton.Text = "Details...";
			this._detailsButton.UseVisualStyleBackColor = true;
			this._detailsButton.Click += new System.EventHandler(this._detailsButton_Click);
			// 
			// _copyToClipBoardButton
			// 
			this._copyToClipBoardButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._copyToClipBoardButton.Location = new System.Drawing.Point(421, 201);
			this._copyToClipBoardButton.Name = "_copyToClipBoardButton";
			this._copyToClipBoardButton.Size = new System.Drawing.Size(116, 23);
			this._copyToClipBoardButton.TabIndex = 2;
			this._copyToClipBoardButton.Text = "Copy to Clipboard";
			this._copyToClipBoardButton.UseVisualStyleBackColor = true;
			this._copyToClipBoardButton.Click += new System.EventHandler(this._copyToClipBoardButton_Click);
			// 
			// SimpleExceptionViewer
			// 
			this.AcceptButton = this._okButton;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(542, 229);
			this.Controls.Add(this._copyToClipBoardButton);
			this.Controls.Add(this._detailsButton);
			this.Controls.Add(this._okButton);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
			this.Name = "SimpleExceptionViewer";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Exception Viewer";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox _exceptionMessageTextBox;
		//private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button _okButton;
		private System.Windows.Forms.Button _detailsButton;
		private System.Windows.Forms.Button _copyToClipBoardButton;
	}
}