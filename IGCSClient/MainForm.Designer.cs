namespace IGCSClient
{
	partial class MainForm
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
			this._clearLogButton = new System.Windows.Forms.Button();
			this._logTextBox = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// _clearLogButton
			// 
			this._clearLogButton.Location = new System.Drawing.Point(12, 12);
			this._clearLogButton.Name = "_clearLogButton";
			this._clearLogButton.Size = new System.Drawing.Size(75, 23);
			this._clearLogButton.TabIndex = 0;
			this._clearLogButton.Text = "Clear";
			this._clearLogButton.UseVisualStyleBackColor = true;
			this._clearLogButton.Click += new System.EventHandler(this._clearLogButton_Click);
			// 
			// _logTextBox
			// 
			this._logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._logTextBox.Location = new System.Drawing.Point(13, 42);
			this._logTextBox.Multiline = true;
			this._logTextBox.Name = "_logTextBox";
			this._logTextBox.Size = new System.Drawing.Size(775, 396);
			this._logTextBox.TabIndex = 1;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(800, 450);
			this.Controls.Add(this._logTextBox);
			this.Controls.Add(this._clearLogButton);
			this.Name = "MainForm";
			this.Text = "Form1";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button _clearLogButton;
		private System.Windows.Forms.TextBox _logTextBox;
	}
}

