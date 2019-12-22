namespace IGCSClient.Controls
{
	partial class FolderInput
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

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this._inputControl = new System.Windows.Forms.TextBox();
			this._browseButton = new System.Windows.Forms.Button();
			this._folderBrowser = new System.Windows.Forms.FolderBrowserDialog();
			this.SuspendLayout();
			// 
			// _inputControl
			// 
			this._inputControl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._inputControl.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
			this._inputControl.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystem;
			this._inputControl.Location = new System.Drawing.Point(0, 0);
			this._inputControl.Name = "_inputControl";
			this._inputControl.Size = new System.Drawing.Size(231, 20);
			this._inputControl.TabIndex = 0;
			this._inputControl.TextChanged += new System.EventHandler(this._inputControl_TextChanged);
			// 
			// _browseButton
			// 
			this._browseButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._browseButton.Image = global::IGCSClient.Properties.Resources.Folder_16x;
			this._browseButton.Location = new System.Drawing.Point(231, 0);
			this._browseButton.Name = "_browseButton";
			this._browseButton.Size = new System.Drawing.Size(22, 20);
			this._browseButton.TabIndex = 1;
			this._browseButton.UseVisualStyleBackColor = true;
			this._browseButton.Click += new System.EventHandler(this._browseButton_Click);
			// 
			// _folderBrowser
			// 
			this._folderBrowser.Description = "Please select a folder...";
			// 
			// FolderInput
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this._browseButton);
			this.Controls.Add(this._inputControl);
			this.Name = "FolderInput";
			this.Size = new System.Drawing.Size(253, 20);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox _inputControl;
		private System.Windows.Forms.Button _browseButton;
		private System.Windows.Forms.FolderBrowserDialog _folderBrowser;
	}
}
