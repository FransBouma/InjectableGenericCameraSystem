namespace IGCSClient.Controls
{
	partial class SettingEditor
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
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.floatInput4 = new IGCSClient.Controls.FloatInput();
			this.label4 = new System.Windows.Forms.Label();
			this.floatInput3 = new IGCSClient.Controls.FloatInput();
			this.label5 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.floatInput2 = new IGCSClient.Controls.FloatInput();
			this.label2 = new System.Windows.Forms.Label();
			this.floatInput1 = new IGCSClient.Controls.FloatInput();
			this.label1 = new System.Windows.Forms.Label();
			this.dropDownInput1 = new IGCSClient.Controls.DropDownInput();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.dropDownInput1);
			this.groupBox1.Controls.Add(this.floatInput4);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.floatInput3);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.floatInput2);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.floatInput1);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(4, 4);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(648, 161);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Camera movement options";
			// 
			// floatInput4
			// 
			this.floatInput4.Location = new System.Drawing.Point(164, 101);
			this.floatInput4.Name = "floatInput4";
			this.floatInput4.Size = new System.Drawing.Size(109, 20);
			this.floatInput4.TabIndex = 1;
			this.floatInput4.Value = 0F;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(11, 132);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(113, 13);
			this.label4.TabIndex = 0;
			this.label4.Text = "Camera control device";
			// 
			// floatInput3
			// 
			this.floatInput3.Location = new System.Drawing.Point(164, 75);
			this.floatInput3.Name = "floatInput3";
			this.floatInput3.Size = new System.Drawing.Size(109, 20);
			this.floatInput3.TabIndex = 1;
			this.floatInput3.Value = 0F;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(11, 104);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(89, 13);
			this.label5.TabIndex = 0;
			this.label5.Text = "Movement speed";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(11, 76);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(116, 13);
			this.label3.TabIndex = 0;
			this.label3.Text = "Up movement multiplier";
			// 
			// floatInput2
			// 
			this.floatInput2.Location = new System.Drawing.Point(164, 49);
			this.floatInput2.Name = "floatInput2";
			this.floatInput2.Size = new System.Drawing.Size(109, 20);
			this.floatInput2.TabIndex = 1;
			this.floatInput2.Value = 0F;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(11, 50);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(125, 13);
			this.label2.TabIndex = 0;
			this.label2.Text = "Slow movement multiplier";
			// 
			// floatInput1
			// 
			this.floatInput1.Location = new System.Drawing.Point(164, 23);
			this.floatInput1.Name = "floatInput1";
			this.floatInput1.Size = new System.Drawing.Size(109, 20);
			this.floatInput1.TabIndex = 1;
			this.floatInput1.Value = 0F;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(11, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(122, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Fast movement multiplier";
			// 
			// dropDownInput1
			// 
			this.dropDownInput1.Location = new System.Drawing.Point(164, 127);
			this.dropDownInput1.Name = "dropDownInput1";
			this.dropDownInput1.Size = new System.Drawing.Size(109, 21);
			this.dropDownInput1.TabIndex = 2;
			this.dropDownInput1.Value = -1;
			// 
			// SettingEditor
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.groupBox1);
			this.Name = "SettingEditor";
			this.Size = new System.Drawing.Size(655, 421);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBox1;
		private FloatInput floatInput4;
		private System.Windows.Forms.Label label4;
		private FloatInput floatInput3;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label3;
		private FloatInput floatInput2;
		private System.Windows.Forms.Label label2;
		private FloatInput floatInput1;
		private System.Windows.Forms.Label label1;
		private DropDownInput dropDownInput1;
	}
}
