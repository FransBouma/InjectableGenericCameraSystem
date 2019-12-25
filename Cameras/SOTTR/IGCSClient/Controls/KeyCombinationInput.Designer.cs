namespace IGCSClient.Controls
{
	partial class KeyCombinationInput
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
			this._keyInputTextBox = new System.Windows.Forms.TextBox();
			this._cancelButton = new System.Windows.Forms.Button();
			this._okButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// _keyInputTextBox
			// 
			this._keyInputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._keyInputTextBox.Location = new System.Drawing.Point(0, 1);
			this._keyInputTextBox.Name = "_keyInputTextBox";
			this._keyInputTextBox.ReadOnly = true;
			this._keyInputTextBox.Size = new System.Drawing.Size(171, 20);
			this._keyInputTextBox.TabIndex = 0;
			this._keyInputTextBox.Click += new System.EventHandler(this._keyInputTextBox_Click);
			this._keyInputTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this._keyInputTextBox_KeyDown);
			// 
			// _cancelButton
			// 
			this._cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._cancelButton.Location = new System.Drawing.Point(220, 0);
			this._cancelButton.Name = "_cancelButton";
			this._cancelButton.Size = new System.Drawing.Size(49, 22);
			this._cancelButton.TabIndex = 2;
			this._cancelButton.Text = "Cancel";
			this._cancelButton.UseVisualStyleBackColor = true;
			this._cancelButton.Click += new System.EventHandler(this._cancelButton_Click);
			// 
			// _okButton
			// 
			this._okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this._okButton.Location = new System.Drawing.Point(171, 0);
			this._okButton.Name = "_okButton";
			this._okButton.Size = new System.Drawing.Size(49, 22);
			this._okButton.TabIndex = 1;
			this._okButton.Text = "OK";
			this._okButton.UseVisualStyleBackColor = true;
			this._okButton.Click += new System.EventHandler(this._okButton_Click);
			// 
			// KeyCombinationEditor
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this._okButton);
			this.Controls.Add(this._cancelButton);
			this.Controls.Add(this._keyInputTextBox);
			this.Name = "KeyCombinationInput";
			this.Size = new System.Drawing.Size(270, 23);
			this.Leave += new System.EventHandler(this.KeyCombinationEditor_Leave);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox _keyInputTextBox;
		private System.Windows.Forms.Button _cancelButton;
		private System.Windows.Forms.Button _okButton;
	}
}
