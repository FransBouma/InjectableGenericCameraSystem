using System;
using System.Drawing;
using System.Windows.Forms;

namespace IGCSClient.Controls
{
	public partial class ApplicationOutput
	{
		private System.Windows.Forms.RichTextBox _outputTextBox;
		private CheckBox _verboseCheckBox;
		private System.Windows.Forms.ToolTip _mainToolTip;
		private System.ComponentModel.IContainer components;

		
		#region Windows Form Designer generated code
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this._outputTextBox = new System.Windows.Forms.RichTextBox();
			this._verboseCheckBox = new System.Windows.Forms.CheckBox();
			this._mainToolTip = new System.Windows.Forms.ToolTip(this.components);
			this.panelControl2 = new System.Windows.Forms.Panel();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this._clearButton = new System.Windows.Forms.Button();
			this.panelControl2.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// _outputTextBox
			// 
			this._outputTextBox.BackColor = System.Drawing.SystemColors.Window;
			this._outputTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this._outputTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this._outputTextBox.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this._outputTextBox.HideSelection = false;
			this._outputTextBox.Location = new System.Drawing.Point(3, 16);
			this._outputTextBox.Name = "_outputTextBox";
			this._outputTextBox.ReadOnly = true;
			this._outputTextBox.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
			this._outputTextBox.Size = new System.Drawing.Size(655, 327);
			this._outputTextBox.TabIndex = 2;
			this._outputTextBox.Text = "";
			// 
			// _verboseCheckBox
			// 
			this._verboseCheckBox.Checked = true;
			this._verboseCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
			this._verboseCheckBox.Location = new System.Drawing.Point(84, 7);
			this._verboseCheckBox.Name = "_verboseCheckBox";
			this._verboseCheckBox.Size = new System.Drawing.Size(104, 19);
			this._verboseCheckBox.TabIndex = 1;
			this._verboseCheckBox.Text = "Verbose output";
			this._mainToolTip.SetToolTip(this._verboseCheckBox, "Check this option to show all messages.");
			// 
			// panelControl2
			// 
			this.panelControl2.Controls.Add(this._clearButton);
			this.panelControl2.Controls.Add(this._verboseCheckBox);
			this.panelControl2.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelControl2.Location = new System.Drawing.Point(0, 0);
			this.panelControl2.Name = "panelControl2";
			this.panelControl2.Size = new System.Drawing.Size(661, 32);
			this.panelControl2.TabIndex = 3;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this._outputTextBox);
			this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox1.Location = new System.Drawing.Point(0, 32);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(661, 346);
			this.groupBox1.TabIndex = 4;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Application log";
			// 
			// _clearButton
			// 
			this._clearButton.Location = new System.Drawing.Point(5, 4);
			this._clearButton.Name = "_clearButton";
			this._clearButton.Size = new System.Drawing.Size(54, 22);
			this._clearButton.TabIndex = 6;
			this._clearButton.Text = "Clear";
			this._clearButton.UseVisualStyleBackColor = true;
			this._clearButton.Click += new System.EventHandler(this.btnClear_Click);
			// 
			// ApplicationOutput
			// 
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.panelControl2);
			this.Name = "ApplicationOutput";
			this.Size = new System.Drawing.Size(661, 378);
			this.panelControl2.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion
		private Panel panelControl2;
		private GroupBox groupBox1;
		private Button _clearButton;
	}
}

