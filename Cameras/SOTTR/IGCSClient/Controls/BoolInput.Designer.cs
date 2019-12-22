namespace IGCSClient.Controls
{
	partial class BoolInput
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
			this._inputControl = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// _inputControl
			// 
			this._inputControl.Dock = System.Windows.Forms.DockStyle.Fill;
			this._inputControl.Location = new System.Drawing.Point(0, 0);
			this._inputControl.Name = "_inputControl";
			this._inputControl.Size = new System.Drawing.Size(356, 20);
			this._inputControl.TabIndex = 0;
			this._inputControl.UseVisualStyleBackColor = true;
			this._inputControl.CheckedChanged += new System.EventHandler(this._inputControl_CheckedChanged);
			// 
			// BoolInput
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this._inputControl);
			this.Name = "BoolInput";
			this.Size = new System.Drawing.Size(356, 20);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.CheckBox _inputControl;
	}
}
