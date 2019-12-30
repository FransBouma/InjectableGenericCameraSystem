namespace IGCSClient.Forms
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
			this._statusBar = new System.Windows.Forms.StatusStrip();
			this._dllToClientConnectedSBLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this._clientToDllConnectedSBLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this._mainTabControl = new System.Windows.Forms.TabControl();
			this._generalTab = new System.Windows.Forms.TabPage();
			this._generalTabControl = new IGCSClient.Controls.GeneralTabControl();
			this._settingsTab = new System.Windows.Forms.TabPage();
			this._settingsEditor = new IGCSClient.GameSpecific.Controls.SettingEditor();
			this._keyBindingsTab = new System.Windows.Forms.TabPage();
			this._keyBindingsEditor = new IGCSClient.GameSpecific.Controls.KeyBindingEditor();
			this._hotsamplingTab = new System.Windows.Forms.TabPage();
			this._hotsamplingControl = new IGCSClient.Controls.HotsamplingControl();
			this._logTab = new System.Windows.Forms.TabPage();
			this._logControl = new IGCSClient.Controls.ApplicationOutput();
			this._aboutTab = new System.Windows.Forms.TabPage();
			this._statusBar.SuspendLayout();
			this._mainTabControl.SuspendLayout();
			this._generalTab.SuspendLayout();
			this._settingsTab.SuspendLayout();
			this._keyBindingsTab.SuspendLayout();
			this._hotsamplingTab.SuspendLayout();
			this._logTab.SuspendLayout();
			this.SuspendLayout();
			// 
			// _statusBar
			// 
			this._statusBar.Location = new System.Drawing.Point(0, 435);
			this._statusBar.Name = "_statusBar";
			this._statusBar.Size = new System.Drawing.Size(666, 24);
			this._statusBar.TabIndex = 0;
			// 
			// _dllToClientConnectedSBLabel
			// 
			this._dllToClientConnectedSBLabel.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
			this._dllToClientConnectedSBLabel.Name = "_dllToClientConnectedSBLabel";
			this._dllToClientConnectedSBLabel.Size = new System.Drawing.Size(90, 19);
			this._dllToClientConnectedSBLabel.Text = "Not connected";
			// 
			// _clientToDllConnectedSBLabel
			// 
			this._clientToDllConnectedSBLabel.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
			this._clientToDllConnectedSBLabel.Name = "_clientToDllConnectedSBLabel";
			this._clientToDllConnectedSBLabel.Size = new System.Drawing.Size(90, 19);
			this._clientToDllConnectedSBLabel.Text = "Not connected";
			// 
			// _mainTabControl
			// 
			this._mainTabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._mainTabControl.Controls.Add(this._generalTab);
			this._mainTabControl.Controls.Add(this._settingsTab);
			this._mainTabControl.Controls.Add(this._keyBindingsTab);
			this._mainTabControl.Controls.Add(this._hotsamplingTab);
			this._mainTabControl.Controls.Add(this._logTab);
			this._mainTabControl.Controls.Add(this._aboutTab);
			this._mainTabControl.Location = new System.Drawing.Point(3, 3);
			this._mainTabControl.Name = "_mainTabControl";
			this._mainTabControl.SelectedIndex = 0;
			this._mainTabControl.Size = new System.Drawing.Size(660, 431);
			this._mainTabControl.TabIndex = 1;
			// 
			// _generalTab
			// 
			this._generalTab.BackColor = System.Drawing.Color.White;
			this._generalTab.Controls.Add(this._generalTabControl);
			this._generalTab.Location = new System.Drawing.Point(4, 22);
			this._generalTab.Name = "_generalTab";
			this._generalTab.Padding = new System.Windows.Forms.Padding(3);
			this._generalTab.Size = new System.Drawing.Size(641, 390);
			this._generalTab.TabIndex = 0;
			this._generalTab.Text = "General";
			// 
			// _generalTabControl
			// 
			this._generalTabControl.Dock = System.Windows.Forms.DockStyle.Fill;
			this._generalTabControl.Location = new System.Drawing.Point(3, 3);
			this._generalTabControl.Name = "_generalTabControl";
			this._generalTabControl.Size = new System.Drawing.Size(635, 384);
			this._generalTabControl.TabIndex = 0;
			this._generalTabControl.DllInjected += new System.EventHandler(this._generalTabControl_DllInjected);
			// 
			// _settingsTab
			// 
			this._settingsTab.BackColor = System.Drawing.SystemColors.Window;
			this._settingsTab.Controls.Add(this._settingsEditor);
			this._settingsTab.Location = new System.Drawing.Point(4, 22);
			this._settingsTab.Name = "_settingsTab";
			this._settingsTab.Padding = new System.Windows.Forms.Padding(3);
			this._settingsTab.Size = new System.Drawing.Size(641, 390);
			this._settingsTab.TabIndex = 1;
			this._settingsTab.Text = "Settings";
			// 
			// _settingsEditor
			// 
			this._settingsEditor.Dock = System.Windows.Forms.DockStyle.Fill;
			this._settingsEditor.Location = new System.Drawing.Point(3, 3);
			this._settingsEditor.Name = "_settingsEditor";
			this._settingsEditor.Size = new System.Drawing.Size(635, 384);
			this._settingsEditor.TabIndex = 4;
			// 
			// _keyBindingsTab
			// 
			this._keyBindingsTab.AutoScroll = true;
			this._keyBindingsTab.BackColor = System.Drawing.SystemColors.Window;
			this._keyBindingsTab.Controls.Add(this._keyBindingsEditor);
			this._keyBindingsTab.Location = new System.Drawing.Point(4, 22);
			this._keyBindingsTab.Name = "_keyBindingsTab";
			this._keyBindingsTab.Padding = new System.Windows.Forms.Padding(3);
			this._keyBindingsTab.Size = new System.Drawing.Size(641, 390);
			this._keyBindingsTab.TabIndex = 3;
			this._keyBindingsTab.Text = "Key bindings";
			// 
			// _keyBindingsEditor
			// 
			this._keyBindingsEditor.BackColor = System.Drawing.SystemColors.Window;
			this._keyBindingsEditor.Dock = System.Windows.Forms.DockStyle.Top;
			this._keyBindingsEditor.Location = new System.Drawing.Point(3, 3);
			this._keyBindingsEditor.Name = "_keyBindingsEditor";
			this._keyBindingsEditor.Size = new System.Drawing.Size(618, 587);
			this._keyBindingsEditor.TabIndex = 4;
			// 
			// _hotsamplingTab
			// 
			this._hotsamplingTab.Controls.Add(this._hotsamplingControl);
			this._hotsamplingTab.Location = new System.Drawing.Point(4, 22);
			this._hotsamplingTab.Name = "_hotsamplingTab";
			this._hotsamplingTab.Padding = new System.Windows.Forms.Padding(3);
			this._hotsamplingTab.Size = new System.Drawing.Size(652, 405);
			this._hotsamplingTab.TabIndex = 4;
			this._hotsamplingTab.Text = "Hotsampling";
			this._hotsamplingTab.UseVisualStyleBackColor = true;
			// 
			// _hotsamplingControl
			// 
			this._hotsamplingControl.Dock = System.Windows.Forms.DockStyle.Fill;
			this._hotsamplingControl.Location = new System.Drawing.Point(3, 3);
			this._hotsamplingControl.Name = "_hotsamplingControl";
			this._hotsamplingControl.Size = new System.Drawing.Size(646, 399);
			this._hotsamplingControl.TabIndex = 0;
			// 
			// _logTab
			// 
			this._logTab.Controls.Add(this._logControl);
			this._logTab.Location = new System.Drawing.Point(4, 22);
			this._logTab.Name = "_logTab";
			this._logTab.Padding = new System.Windows.Forms.Padding(3);
			this._logTab.Size = new System.Drawing.Size(641, 390);
			this._logTab.TabIndex = 2;
			this._logTab.Text = "Log";
			this._logTab.UseVisualStyleBackColor = true;
			// 
			// _logControl
			// 
			this._logControl.BackColor = System.Drawing.SystemColors.Window;
			this._logControl.Dock = System.Windows.Forms.DockStyle.Fill;
			this._logControl.Location = new System.Drawing.Point(3, 3);
			this._logControl.Name = "_logControl";
			this._logControl.Size = new System.Drawing.Size(635, 384);
			this._logControl.TabIndex = 4;
			// 
			// _aboutTab
			// 
			this._aboutTab.Location = new System.Drawing.Point(4, 22);
			this._aboutTab.Name = "_aboutTab";
			this._aboutTab.Padding = new System.Windows.Forms.Padding(3);
			this._aboutTab.Size = new System.Drawing.Size(641, 390);
			this._aboutTab.TabIndex = 5;
			this._aboutTab.Text = "About";
			this._aboutTab.UseVisualStyleBackColor = true;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(666, 459);
			this.Controls.Add(this._mainTabControl);
			this.Controls.Add(this._statusBar);
			this.DoubleBuffered = true;
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Form1";
			this._statusBar.ResumeLayout(false);
			this._statusBar.PerformLayout();
			this._mainTabControl.ResumeLayout(false);
			this._generalTab.ResumeLayout(false);
			this._settingsTab.ResumeLayout(false);
			this._keyBindingsTab.ResumeLayout(false);
			this._hotsamplingTab.ResumeLayout(false);
			this._logTab.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.StatusStrip _statusBar;
		private System.Windows.Forms.TabControl _mainTabControl;
		private System.Windows.Forms.TabPage _generalTab;
		private System.Windows.Forms.TabPage _settingsTab;
		private System.Windows.Forms.TabPage _logTab;
		private System.Windows.Forms.TabPage _keyBindingsTab;
		private System.Windows.Forms.TabPage _hotsamplingTab;
		private GameSpecific.Controls.SettingEditor _settingsEditor;
		private GameSpecific.Controls.KeyBindingEditor _keyBindingsEditor;
		private Controls.ApplicationOutput _logControl;
		private System.Windows.Forms.ToolStripStatusLabel _dllToClientConnectedSBLabel;
		private System.Windows.Forms.ToolStripStatusLabel _clientToDllConnectedSBLabel;
		private System.Windows.Forms.TabPage _aboutTab;
		private Controls.GeneralTabControl _generalTabControl;
		private Controls.HotsamplingControl _hotsamplingControl;
	}
}

