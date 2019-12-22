namespace IGCSClient.Controls
{
	partial class FloatInput
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
			this._inputControl = new System.Windows.Forms.NumericUpDown();
			((System.ComponentModel.ISupportInitialize)(this._inputControl)).BeginInit();
			this.SuspendLayout();
			// 
			// _inputControl
			// 
			this._inputControl.DecimalPlaces = 2;
			this._inputControl.Dock = System.Windows.Forms.DockStyle.Fill;
			this._inputControl.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
			this._inputControl.Location = new System.Drawing.Point(0, 0);
			this._inputControl.Maximum = new decimal(new int[] {
            15,
            0,
            0,
            131072});
			this._inputControl.Name = "_inputControl";
			this._inputControl.Size = new System.Drawing.Size(530, 20);
			this._inputControl.TabIndex = 1;
			this._inputControl.ValueChanged += new System.EventHandler(this._inputControl_ValueChanged);
			// 
			// FloatInput
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this._inputControl);
			this.Name = "FloatInput";
			this.Size = new System.Drawing.Size(530, 20);
			((System.ComponentModel.ISupportInitialize)(this._inputControl)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion
		private System.Windows.Forms.NumericUpDown _inputControl;
	}
}
