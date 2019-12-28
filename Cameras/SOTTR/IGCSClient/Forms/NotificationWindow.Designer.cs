namespace IGCSClient.Forms
{
	partial class NotificationWindow
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
			this._notificationGroupBox = new System.Windows.Forms.GroupBox();
			this._notificationLabel = new System.Windows.Forms.Label();
			this._notificationGroupBox.SuspendLayout();
			this.SuspendLayout();
			// 
			// _notificationGroupBox
			// 
			this._notificationGroupBox.AutoSize = true;
			this._notificationGroupBox.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this._notificationGroupBox.BackColor = System.Drawing.Color.Transparent;
			this._notificationGroupBox.Controls.Add(this._notificationLabel);
			this._notificationGroupBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this._notificationGroupBox.Location = new System.Drawing.Point(0, 0);
			this._notificationGroupBox.Name = "_notificationGroupBox";
			this._notificationGroupBox.Size = new System.Drawing.Size(800, 450);
			this._notificationGroupBox.TabIndex = 0;
			this._notificationGroupBox.TabStop = false;
			this._notificationGroupBox.Text = "Notifications";
			// 
			// _notificationLabel
			// 
			this._notificationLabel.AutoSize = true;
			this._notificationLabel.Dock = System.Windows.Forms.DockStyle.Fill;
			this._notificationLabel.Location = new System.Drawing.Point(3, 16);
			this._notificationLabel.Name = "_notificationLabel";
			this._notificationLabel.Size = new System.Drawing.Size(101, 13);
			this._notificationLabel.TabIndex = 0;
			this._notificationLabel.Text = "This is the initial text";
			// 
			// NotificationWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSize = true;
			this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.ClientSize = new System.Drawing.Size(800, 450);
			this.Controls.Add(this._notificationGroupBox);
			this.DoubleBuffered = true;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Location = new System.Drawing.Point(4, 4);
			this.MinimumSize = new System.Drawing.Size(10, 10);
			this.Name = "NotificationWindow";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "NotificationWindow";
			this.TopMost = true;
			this._notificationGroupBox.ResumeLayout(false);
			this._notificationGroupBox.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.GroupBox _notificationGroupBox;
		private System.Windows.Forms.Label _notificationLabel;
	}
}