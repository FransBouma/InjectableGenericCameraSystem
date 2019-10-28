namespace IGCSInjectorUI
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this._openDllToInjectDialog = new System.Windows.Forms.OpenFileDialog();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this._selectProcessButton = new System.Windows.Forms.Button();
			this._browseForDllButton = new System.Windows.Forms.Button();
			this._processNameTextBox = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this._dllFilenameTextBox = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this._injectButton = new System.Windows.Forms.Button();
			this._cancelButton = new System.Windows.Forms.Button();
			this._mainToolTip = new System.Windows.Forms.ToolTip(this.components);
			this._aboutButton = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// _openDllToInjectDialog
			// 
			this._openDllToInjectDialog.DefaultExt = "dll";
			this._openDllToInjectDialog.Filter = "Dll files|*.dll";
			this._openDllToInjectDialog.Title = "Please select the dll to inject";
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this._selectProcessButton);
			this.groupBox1.Controls.Add(this._browseForDllButton);
			this.groupBox1.Controls.Add(this._processNameTextBox);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this._dllFilenameTextBox);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(13, 13);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(578, 79);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Process and DLL selection";
			// 
			// _selectProcessButton
			// 
			this._selectProcessButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._selectProcessButton.Location = new System.Drawing.Point(497, 17);
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
			this._browseForDllButton.Location = new System.Drawing.Point(497, 43);
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
			this._processNameTextBox.Location = new System.Drawing.Point(117, 19);
			this._processNameTextBox.Name = "_processNameTextBox";
			this._processNameTextBox.ReadOnly = true;
			this._processNameTextBox.Size = new System.Drawing.Size(374, 20);
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
			this._dllFilenameTextBox.Location = new System.Drawing.Point(117, 45);
			this._dllFilenameTextBox.Name = "_dllFilenameTextBox";
			this._dllFilenameTextBox.Size = new System.Drawing.Size(374, 20);
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
			// _injectButton
			// 
			this._injectButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this._injectButton.Enabled = false;
			this._injectButton.Location = new System.Drawing.Point(190, 107);
			this._injectButton.Name = "_injectButton";
			this._injectButton.Size = new System.Drawing.Size(99, 23);
			this._injectButton.TabIndex = 1;
			this._injectButton.Text = "&Inject DLL";
			this._injectButton.UseVisualStyleBackColor = true;
			this._injectButton.Click += new System.EventHandler(this._injectButton_Click);
			// 
			// _cancelButton
			// 
			this._cancelButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
			this._cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this._cancelButton.Location = new System.Drawing.Point(313, 107);
			this._cancelButton.Name = "_cancelButton";
			this._cancelButton.Size = new System.Drawing.Size(99, 23);
			this._cancelButton.TabIndex = 2;
			this._cancelButton.Text = "&Cancel";
			this._cancelButton.UseVisualStyleBackColor = true;
			this._cancelButton.Click += new System.EventHandler(this._cancelButton_Click);
			// 
			// _mainToolTip
			// 
			this._mainToolTip.AutoPopDelay = 5000;
			this._mainToolTip.InitialDelay = 100;
			this._mainToolTip.ReshowDelay = 100;
			// 
			// _aboutButton
			// 
			this._aboutButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._aboutButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this._aboutButton.Location = new System.Drawing.Point(510, 107);
			this._aboutButton.Name = "_aboutButton";
			this._aboutButton.Size = new System.Drawing.Size(75, 23);
			this._aboutButton.TabIndex = 2;
			this._aboutButton.Text = "About...";
			this._aboutButton.UseVisualStyleBackColor = true;
			this._aboutButton.Click += new System.EventHandler(this._aboutButton_Click);
			// 
			// MainForm
			// 
			this.AcceptButton = this._injectButton;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this._cancelButton;
			this.ClientSize = new System.Drawing.Size(603, 140);
			this.Controls.Add(this._aboutButton);
			this.Controls.Add(this._cancelButton);
			this.Controls.Add(this._injectButton);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "IGCS Injector";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);

        }

		#endregion

		private System.Windows.Forms.OpenFileDialog _openDllToInjectDialog;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button _browseForDllButton;
		private System.Windows.Forms.TextBox _dllFilenameTextBox;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button _selectProcessButton;
		private System.Windows.Forms.TextBox _processNameTextBox;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button _injectButton;
		private System.Windows.Forms.Button _cancelButton;
		private System.Windows.Forms.ToolTip _mainToolTip;
		private System.Windows.Forms.Button _aboutButton;
	}
}

