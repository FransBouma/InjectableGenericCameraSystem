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
			this.components = new System.ComponentModel.Container();
			this._notificationLabel = new System.Windows.Forms.Label();
			this._notificationPurgeTimer = new System.Windows.Forms.Timer(this.components);
			this.SuspendLayout();
			// 
			// _notificationLabel
			// 
			this._notificationLabel.AutoSize = true;
			this._notificationLabel.Dock = System.Windows.Forms.DockStyle.Fill;
			this._notificationLabel.Location = new System.Drawing.Point(0, 0);
			this._notificationLabel.Name = "_notificationLabel";
			this._notificationLabel.Padding = new System.Windows.Forms.Padding(10);
			this._notificationLabel.Size = new System.Drawing.Size(121, 33);
			this._notificationLabel.TabIndex = 0;
			this._notificationLabel.Text = "This is the initial text";
			// 
			// _notificationPurgeTimer
			// 
			this._notificationPurgeTimer.Interval = 200;
			this._notificationPurgeTimer.Tick += new System.EventHandler(this._notificationPurgeTimer_Tick);
			// 
			// NotificationWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSize = true;
			this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.ClientSize = new System.Drawing.Size(315, 129);
			this.Controls.Add(this._notificationLabel);
			this.DoubleBuffered = true;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Location = new System.Drawing.Point(4, 4);
			this.MinimumSize = new System.Drawing.Size(10, 10);
			this.Name = "NotificationWindow";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "NotificationWindow";
			this.TopMost = true;
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.Label _notificationLabel;
		private System.Windows.Forms.Timer _notificationPurgeTimer;
	}
}