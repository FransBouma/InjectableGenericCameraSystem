namespace IGCSClient.Controls
{
	partial class GeneralTabControl
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
			this.components = new System.ComponentModel.Container();
			this._injectDllGroupBox = new System.Windows.Forms.GroupBox();
			this._injectButton = new System.Windows.Forms.Button();
			this._selectProcessButton = new System.Windows.Forms.Button();
			this._browseForDllButton = new System.Windows.Forms.Button();
			this._processNameTextBox = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this._dllFilenameTextBox = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this._attachedProcessInfoGroupBox = new System.Windows.Forms.GroupBox();
			this._windowTitleLabel = new System.Windows.Forms.Label();
			this._executablePathLabel = new System.Windows.Forms.Label();
			this._executableIconPictureBox = new System.Windows.Forms.PictureBox();
			this._openDllToInjectDialog = new System.Windows.Forms.OpenFileDialog();
			this._mainToolTip = new System.Windows.Forms.ToolTip(this.components);
			this._executableTextBox = new System.Windows.Forms.TextBox();
			this._windowTitleTextBox = new System.Windows.Forms.TextBox();
			this._injectDllGroupBox.SuspendLayout();
			this._attachedProcessInfoGroupBox.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this._executableIconPictureBox)).BeginInit();
			this.SuspendLayout();
			// 
			// _injectDllGroupBox
			// 
			this._injectDllGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._injectDllGroupBox.Controls.Add(this._injectButton);
			this._injectDllGroupBox.Controls.Add(this._selectProcessButton);
			this._injectDllGroupBox.Controls.Add(this._browseForDllButton);
			this._injectDllGroupBox.Controls.Add(this._processNameTextBox);
			this._injectDllGroupBox.Controls.Add(this.label2);
			this._injectDllGroupBox.Controls.Add(this._dllFilenameTextBox);
			this._injectDllGroupBox.Controls.Add(this.label1);
			this._injectDllGroupBox.Location = new System.Drawing.Point(4, 4);
			this._injectDllGroupBox.Name = "_injectDllGroupBox";
			this._injectDllGroupBox.Size = new System.Drawing.Size(708, 113);
			this._injectDllGroupBox.TabIndex = 1;
			this._injectDllGroupBox.TabStop = false;
			this._injectDllGroupBox.Text = "Process and DLL selection";
			// 
			// _injectButton
			// 
			this._injectButton.Enabled = false;
			this._injectButton.Location = new System.Drawing.Point(122, 71);
			this._injectButton.Name = "_injectButton";
			this._injectButton.Size = new System.Drawing.Size(99, 23);
			this._injectButton.TabIndex = 2;
			this._injectButton.Text = "&Inject DLL";
			this._injectButton.UseVisualStyleBackColor = true;
			this._injectButton.Click += new System.EventHandler(this._injectButton_Click);
			// 
			// _selectProcessButton
			// 
			this._selectProcessButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._selectProcessButton.Location = new System.Drawing.Point(627, 17);
			this._selectProcessButton.Name = "_selectProcessButton";
			this._selectProcessButton.Size = new System.Drawing.Size(75, 23);
			this._selectProcessButton.TabIndex = 1;
			this._selectProcessButton.Text = "&Select...";
			this._selectProcessButton.UseVisualStyleBackColor = true;
			this._selectProcessButton.Click += new System.EventHandler(this._selectProcessButton_Click);
			// 
			// _browseForDllButton
			// 
			this._browseForDllButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._browseForDllButton.Location = new System.Drawing.Point(627, 43);
			this._browseForDllButton.Name = "_browseForDllButton";
			this._browseForDllButton.Size = new System.Drawing.Size(75, 23);
			this._browseForDllButton.TabIndex = 3;
			this._browseForDllButton.Text = "&Browse...";
			this._browseForDllButton.UseVisualStyleBackColor = true;
			this._browseForDllButton.Click += new System.EventHandler(this._browseForDllButton_Click);
			// 
			// _processNameTextBox
			// 
			this._processNameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._processNameTextBox.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
			this._processNameTextBox.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystem;
			this._processNameTextBox.Location = new System.Drawing.Point(122, 19);
			this._processNameTextBox.Name = "_processNameTextBox";
			this._processNameTextBox.ReadOnly = true;
			this._processNameTextBox.Size = new System.Drawing.Size(499, 20);
			this._processNameTextBox.TabIndex = 0;
			this._processNameTextBox.TabStop = false;
			this._processNameTextBox.TextChanged += new System.EventHandler(this._processNameTextBox_TextChanged);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(6, 22);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(105, 13);
			this.label2.TabIndex = 0;
			this.label2.Text = "Process to inject into";
			// 
			// _dllFilenameTextBox
			// 
			this._dllFilenameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._dllFilenameTextBox.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
			this._dllFilenameTextBox.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystem;
			this._dllFilenameTextBox.Location = new System.Drawing.Point(122, 45);
			this._dllFilenameTextBox.Name = "_dllFilenameTextBox";
			this._dllFilenameTextBox.Size = new System.Drawing.Size(499, 20);
			this._dllFilenameTextBox.TabIndex = 2;
			this._dllFilenameTextBox.TextChanged += new System.EventHandler(this._dllFilenameTextBox_TextChanged);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(6, 48);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(67, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "DLL to inject";
			// 
			// _attachedProcessInfoGroupBox
			// 
			this._attachedProcessInfoGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._attachedProcessInfoGroupBox.Controls.Add(this._windowTitleLabel);
			this._attachedProcessInfoGroupBox.Controls.Add(this._executablePathLabel);
			this._attachedProcessInfoGroupBox.Controls.Add(this._executableIconPictureBox);
			this._attachedProcessInfoGroupBox.Controls.Add(this._windowTitleTextBox);
			this._attachedProcessInfoGroupBox.Controls.Add(this._executableTextBox);
			this._attachedProcessInfoGroupBox.Location = new System.Drawing.Point(4, 4);
			this._attachedProcessInfoGroupBox.Name = "_attachedProcessInfoGroupBox";
			this._attachedProcessInfoGroupBox.Size = new System.Drawing.Size(708, 74);
			this._attachedProcessInfoGroupBox.TabIndex = 2;
			this._attachedProcessInfoGroupBox.TabStop = false;
			this._attachedProcessInfoGroupBox.Text = "Attached process info";
			// 
			// _windowTitleLabel
			// 
			this._windowTitleLabel.AutoSize = true;
			this._windowTitleLabel.Location = new System.Drawing.Point(55, 46);
			this._windowTitleLabel.Name = "_windowTitleLabel";
			this._windowTitleLabel.Size = new System.Drawing.Size(65, 13);
			this._windowTitleLabel.TabIndex = 1;
			this._windowTitleLabel.Text = "Window title";
			// 
			// _executablePathLabel
			// 
			this._executablePathLabel.AutoSize = true;
			this._executablePathLabel.Location = new System.Drawing.Point(55, 20);
			this._executablePathLabel.Name = "_executablePathLabel";
			this._executablePathLabel.Size = new System.Drawing.Size(60, 13);
			this._executablePathLabel.TabIndex = 1;
			this._executablePathLabel.Text = "Executable";
			// 
			// _executableIconPictureBox
			// 
			this._executableIconPictureBox.Location = new System.Drawing.Point(9, 20);
			this._executableIconPictureBox.Name = "_executableIconPictureBox";
			this._executableIconPictureBox.Size = new System.Drawing.Size(40, 40);
			this._executableIconPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this._executableIconPictureBox.TabIndex = 0;
			this._executableIconPictureBox.TabStop = false;
			// 
			// _openDllToInjectDialog
			// 
			this._openDllToInjectDialog.DefaultExt = "dll";
			this._openDllToInjectDialog.Filter = "Dll files|*.dll";
			this._openDllToInjectDialog.Title = "Please select the dll to inject";
			// 
			// _mainToolTip
			// 
			this._mainToolTip.AutoPopDelay = 5000;
			this._mainToolTip.InitialDelay = 100;
			this._mainToolTip.ReshowDelay = 100;
			// 
			// _executableTextBox
			// 
			this._executableTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._executableTextBox.Location = new System.Drawing.Point(122, 17);
			this._executableTextBox.Name = "_executableTextBox";
			this._executableTextBox.ReadOnly = true;
			this._executableTextBox.Size = new System.Drawing.Size(580, 20);
			this._executableTextBox.TabIndex = 0;
			this._executableTextBox.TabStop = false;
			this._executableTextBox.TextChanged += new System.EventHandler(this._processNameTextBox_TextChanged);
			// 
			// _windowTitleTextBox
			// 
			this._windowTitleTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._windowTitleTextBox.Location = new System.Drawing.Point(122, 43);
			this._windowTitleTextBox.Name = "_windowTitleTextBox";
			this._windowTitleTextBox.ReadOnly = true;
			this._windowTitleTextBox.Size = new System.Drawing.Size(580, 20);
			this._windowTitleTextBox.TabIndex = 0;
			this._windowTitleTextBox.TabStop = false;
			this._windowTitleTextBox.TextChanged += new System.EventHandler(this._processNameTextBox_TextChanged);
			// 
			// GeneralTabControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this._attachedProcessInfoGroupBox);
			this.Controls.Add(this._injectDllGroupBox);
			this.Name = "GeneralTabControl";
			this.Size = new System.Drawing.Size(719, 282);
			this._injectDllGroupBox.ResumeLayout(false);
			this._injectDllGroupBox.PerformLayout();
			this._attachedProcessInfoGroupBox.ResumeLayout(false);
			this._attachedProcessInfoGroupBox.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this._executableIconPictureBox)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox _injectDllGroupBox;
		private System.Windows.Forms.Button _injectButton;
		private System.Windows.Forms.Button _selectProcessButton;
		private System.Windows.Forms.Button _browseForDllButton;
		private System.Windows.Forms.TextBox _processNameTextBox;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox _dllFilenameTextBox;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.GroupBox _attachedProcessInfoGroupBox;
		private System.Windows.Forms.Label _windowTitleLabel;
		private System.Windows.Forms.Label _executablePathLabel;
		private System.Windows.Forms.PictureBox _executableIconPictureBox;
		private System.Windows.Forms.OpenFileDialog _openDllToInjectDialog;
		private System.Windows.Forms.ToolTip _mainToolTip;
		private System.Windows.Forms.TextBox _windowTitleTextBox;
		private System.Windows.Forms.TextBox _executableTextBox;
	}
}
