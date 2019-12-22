namespace IGCSClient.Controls
{
	partial class DropDownInput
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
			this._inputControl = new System.Windows.Forms.ComboBox();
			this.SuspendLayout();
			// 
			// _inputControl
			// 
			this._inputControl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._inputControl.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this._inputControl.FormattingEnabled = true;
			this._inputControl.Location = new System.Drawing.Point(0, 0);
			this._inputControl.Name = "_inputControl";
			this._inputControl.Size = new System.Drawing.Size(360, 21);
			this._inputControl.TabIndex = 0;
			this._inputControl.SelectedIndexChanged += new System.EventHandler(this._inputControl_SelectedIndexChanged);
			// 
			// DropDownInput
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this._inputControl);
			this.Name = "DropDownInput";
			this.Size = new System.Drawing.Size(360, 21);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ComboBox _inputControl;
	}
}
