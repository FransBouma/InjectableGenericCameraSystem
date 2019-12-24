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
			this.statusStrip1 = new System.Windows.Forms.StatusStrip();
			this._mainTabControl = new System.Windows.Forms.TabControl();
			this._generalTab = new System.Windows.Forms.TabPage();
			this._settingsTab = new System.Windows.Forms.TabPage();
			this._settingsEditor = new IGCSClient.GameSpecific.Controls.SettingEditor();
			this._keyBindingsTab = new System.Windows.Forms.TabPage();
			this.keyBindingEditor1 = new IGCSClient.GameSpecific.Controls.KeyBindingEditor();
			this._hotsamplingTab = new System.Windows.Forms.TabPage();
			this._logTab = new System.Windows.Forms.TabPage();
			this._logControl = new IGCSClient.Controls.ApplicationOutput();
			this._mainTabControl.SuspendLayout();
			this._settingsTab.SuspendLayout();
			this._keyBindingsTab.SuspendLayout();
			this._logTab.SuspendLayout();
			this.SuspendLayout();
			// 
			// statusStrip1
			// 
			this.statusStrip1.Location = new System.Drawing.Point(0, 428);
			this.statusStrip1.Name = "statusStrip1";
			this.statusStrip1.Size = new System.Drawing.Size(668, 22);
			this.statusStrip1.TabIndex = 0;
			this.statusStrip1.Text = "statusStrip1";
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
			this._mainTabControl.Location = new System.Drawing.Point(3, 3);
			this._mainTabControl.Name = "_mainTabControl";
			this._mainTabControl.SelectedIndex = 0;
			this._mainTabControl.Size = new System.Drawing.Size(662, 422);
			this._mainTabControl.TabIndex = 1;
			this._mainTabControl.Selecting += new System.Windows.Forms.TabControlCancelEventHandler(this._mainTabControl_Selecting);
			// 
			// _generalTab
			// 
			this._generalTab.BackColor = System.Drawing.Color.White;
			this._generalTab.Location = new System.Drawing.Point(4, 22);
			this._generalTab.Name = "_generalTab";
			this._generalTab.Padding = new System.Windows.Forms.Padding(3);
			this._generalTab.Size = new System.Drawing.Size(878, 637);
			this._generalTab.TabIndex = 0;
			this._generalTab.Text = "General";
			// 
			// _settingsTab
			// 
			this._settingsTab.BackColor = System.Drawing.SystemColors.Window;
			this._settingsTab.Controls.Add(this._settingsEditor);
			this._settingsTab.Location = new System.Drawing.Point(4, 22);
			this._settingsTab.Name = "_settingsTab";
			this._settingsTab.Padding = new System.Windows.Forms.Padding(3);
			this._settingsTab.Size = new System.Drawing.Size(654, 396);
			this._settingsTab.TabIndex = 1;
			this._settingsTab.Text = "Settings";
			// 
			// _settingsEditor
			// 
			this._settingsEditor.Dock = System.Windows.Forms.DockStyle.Fill;
			this._settingsEditor.Location = new System.Drawing.Point(3, 3);
			this._settingsEditor.Name = "_settingsEditor";
			this._settingsEditor.Size = new System.Drawing.Size(648, 390);
			this._settingsEditor.TabIndex = 4;
			// 
			// _keyBindingsTab
			// 
			this._keyBindingsTab.AutoScroll = true;
			this._keyBindingsTab.BackColor = System.Drawing.SystemColors.Window;
			this._keyBindingsTab.Controls.Add(this.keyBindingEditor1);
			this._keyBindingsTab.Location = new System.Drawing.Point(4, 22);
			this._keyBindingsTab.Name = "_keyBindingsTab";
			this._keyBindingsTab.Padding = new System.Windows.Forms.Padding(3);
			this._keyBindingsTab.Size = new System.Drawing.Size(654, 396);
			this._keyBindingsTab.TabIndex = 3;
			this._keyBindingsTab.Text = "Key bindings";
			// 
			// keyBindingEditor1
			// 
			this.keyBindingEditor1.BackColor = System.Drawing.SystemColors.Window;
			this.keyBindingEditor1.Dock = System.Windows.Forms.DockStyle.Top;
			this.keyBindingEditor1.Location = new System.Drawing.Point(3, 3);
			this.keyBindingEditor1.Name = "keyBindingEditor1";
			this.keyBindingEditor1.Size = new System.Drawing.Size(631, 587);
			this.keyBindingEditor1.TabIndex = 4;
			// 
			// _hotsamplingTab
			// 
			this._hotsamplingTab.Location = new System.Drawing.Point(4, 22);
			this._hotsamplingTab.Name = "_hotsamplingTab";
			this._hotsamplingTab.Padding = new System.Windows.Forms.Padding(3);
			this._hotsamplingTab.Size = new System.Drawing.Size(878, 637);
			this._hotsamplingTab.TabIndex = 4;
			this._hotsamplingTab.Text = "Hotsampling";
			this._hotsamplingTab.UseVisualStyleBackColor = true;
			// 
			// _logTab
			// 
			this._logTab.Controls.Add(this._logControl);
			this._logTab.Location = new System.Drawing.Point(4, 22);
			this._logTab.Name = "_logTab";
			this._logTab.Padding = new System.Windows.Forms.Padding(3);
			this._logTab.Size = new System.Drawing.Size(878, 637);
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
			this._logControl.Size = new System.Drawing.Size(872, 631);
			this._logControl.TabIndex = 4;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(668, 450);
			this.Controls.Add(this._mainTabControl);
			this.Controls.Add(this.statusStrip1);
			this.DoubleBuffered = true;
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Form1";
			this._mainTabControl.ResumeLayout(false);
			this._settingsTab.ResumeLayout(false);
			this._keyBindingsTab.ResumeLayout(false);
			this._logTab.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.StatusStrip statusStrip1;
		private System.Windows.Forms.TabControl _mainTabControl;
		private System.Windows.Forms.TabPage _generalTab;
		private System.Windows.Forms.TabPage _settingsTab;
		private System.Windows.Forms.TabPage _logTab;
		private System.Windows.Forms.TabPage _keyBindingsTab;
		private System.Windows.Forms.TabPage _hotsamplingTab;
		private GameSpecific.Controls.SettingEditor _settingsEditor;
		private GameSpecific.Controls.KeyBindingEditor keyBindingEditor1;
		private Controls.ApplicationOutput _logControl;
	}
}

