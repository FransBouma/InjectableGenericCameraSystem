namespace IGCSClient.GameSpecific.Controls
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
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label7 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.label8 = new System.Windows.Forms.Label();
			this.groupBox4 = new System.Windows.Forms.GroupBox();
			this._horizontalPanoramaControlsPanel = new System.Windows.Forms.Panel();
			this.label12 = new System.Windows.Forms.Label();
			this.label13 = new System.Windows.Forms.Label();
			this._lightfieldControlsPanel = new System.Windows.Forms.Panel();
			this.label14 = new System.Windows.Forms.Label();
			this.label15 = new System.Windows.Forms.Label();
			this.label16 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this._panoFovInput = new IGCSClient.Controls.FloatInput();
			this._panoOverlapInput = new IGCSClient.Controls.FloatInput();
			this._lightfieldDistanceInput = new IGCSClient.Controls.FloatInput();
			this._lightfieldShotCountInput = new IGCSClient.Controls.IntInput();
			this._shotFileTypeInput = new IGCSClient.Controls.DropDownInput();
			this._shotTypeInput = new IGCSClient.Controls.DropDownInput();
			this._frameWaitInput = new IGCSClient.Controls.IntInput();
			this._shotOutputFolderInput = new IGCSClient.Controls.FolderInput();
			this._fovSpeedInput = new IGCSClient.Controls.FloatInput();
			this._invertYInput = new IGCSClient.Controls.BoolInput();
			this._rotationSpeedInput = new IGCSClient.Controls.FloatInput();
			this._cameraControlDeviceInput = new IGCSClient.Controls.DropDownInput();
			this._movementSpeedInput = new IGCSClient.Controls.FloatInput();
			this._upMovementInput = new IGCSClient.Controls.FloatInput();
			this._slowMovementInput = new IGCSClient.Controls.FloatInput();
			this._fastMovementInput = new IGCSClient.Controls.FloatInput();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox4.SuspendLayout();
			this._horizontalPanoramaControlsPanel.SuspendLayout();
			this._lightfieldControlsPanel.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this._cameraControlDeviceInput);
			this.groupBox1.Controls.Add(this._movementSpeedInput);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this._upMovementInput);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this._slowMovementInput);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this._fastMovementInput);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(4, 4);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(282, 156);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Camera movement options";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(12, 128);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(113, 13);
			this.label4.TabIndex = 0;
			this.label4.Text = "Camera control device";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(12, 101);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(89, 13);
			this.label5.TabIndex = 0;
			this.label5.Text = "Movement speed";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(12, 74);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(116, 13);
			this.label3.TabIndex = 0;
			this.label3.Text = "Up movement multiplier";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(12, 48);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(125, 13);
			this.label2.TabIndex = 0;
			this.label2.Text = "Slow movement multiplier";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 22);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(122, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Fast movement multiplier";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this._invertYInput);
			this.groupBox2.Controls.Add(this._rotationSpeedInput);
			this.groupBox2.Controls.Add(this.label7);
			this.groupBox2.Controls.Add(this.label6);
			this.groupBox2.Location = new System.Drawing.Point(292, 4);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(301, 75);
			this.groupBox2.TabIndex = 1;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Camera rotation options";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(12, 48);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(110, 13);
			this.label7.TabIndex = 0;
			this.label7.Text = "Invert Y look direction";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(12, 22);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(79, 13);
			this.label6.TabIndex = 0;
			this.label6.Text = "Rotation speed";
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this._fovSpeedInput);
			this.groupBox3.Controls.Add(this.label8);
			this.groupBox3.Location = new System.Drawing.Point(292, 85);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(301, 75);
			this.groupBox3.TabIndex = 2;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Miscellaneous camera options";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(12, 24);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(155, 13);
			this.label8.TabIndex = 0;
			this.label8.Text = "Field of View (FoV) zoom speed";
			// 
			// groupBox4
			// 
			this.groupBox4.Controls.Add(this._horizontalPanoramaControlsPanel);
			this.groupBox4.Controls.Add(this._lightfieldControlsPanel);
			this.groupBox4.Controls.Add(this._shotFileTypeInput);
			this.groupBox4.Controls.Add(this._shotTypeInput);
			this.groupBox4.Controls.Add(this.label16);
			this.groupBox4.Controls.Add(this._frameWaitInput);
			this.groupBox4.Controls.Add(this.label11);
			this.groupBox4.Controls.Add(this._shotOutputFolderInput);
			this.groupBox4.Controls.Add(this.label10);
			this.groupBox4.Controls.Add(this.label9);
			this.groupBox4.Location = new System.Drawing.Point(4, 167);
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new System.Drawing.Size(589, 322);
			this.groupBox4.TabIndex = 3;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "Screenshot options";
			// 
			// _horizontalPanoramaControlsPanel
			// 
			this._horizontalPanoramaControlsPanel.Controls.Add(this._panoFovInput);
			this._horizontalPanoramaControlsPanel.Controls.Add(this.label12);
			this._horizontalPanoramaControlsPanel.Controls.Add(this._panoOverlapInput);
			this._horizontalPanoramaControlsPanel.Controls.Add(this.label13);
			this._horizontalPanoramaControlsPanel.Location = new System.Drawing.Point(2, 191);
			this._horizontalPanoramaControlsPanel.Name = "_horizontalPanoramaControlsPanel";
			this._horizontalPanoramaControlsPanel.Size = new System.Drawing.Size(581, 54);
			this._horizontalPanoramaControlsPanel.TabIndex = 4;
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.Location = new System.Drawing.Point(10, 7);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(209, 13);
			this.label12.TabIndex = 0;
			this.label12.Text = "Total field of view in panorama (in degrees)";
			// 
			// label13
			// 
			this.label13.AutoSize = true;
			this.label13.Location = new System.Drawing.Point(10, 33);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(184, 13);
			this.label13.TabIndex = 0;
			this.label13.Text = "Percentage of overlap between shots";
			// 
			// _lightfieldControlsPanel
			// 
			this._lightfieldControlsPanel.Controls.Add(this._lightfieldDistanceInput);
			this._lightfieldControlsPanel.Controls.Add(this.label14);
			this._lightfieldControlsPanel.Controls.Add(this._lightfieldShotCountInput);
			this._lightfieldControlsPanel.Controls.Add(this.label15);
			this._lightfieldControlsPanel.Location = new System.Drawing.Point(2, 121);
			this._lightfieldControlsPanel.Name = "_lightfieldControlsPanel";
			this._lightfieldControlsPanel.Size = new System.Drawing.Size(581, 54);
			this._lightfieldControlsPanel.TabIndex = 4;
			// 
			// label14
			// 
			this.label14.AutoSize = true;
			this.label14.Location = new System.Drawing.Point(10, 7);
			this.label14.Name = "label14";
			this.label14.Size = new System.Drawing.Size(162, 13);
			this.label14.TabIndex = 0;
			this.label14.Text = "Distance between lightfield shots";
			// 
			// label15
			// 
			this.label15.AutoSize = true;
			this.label15.Location = new System.Drawing.Point(10, 33);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(120, 13);
			this.label15.TabIndex = 0;
			this.label15.Text = "Number of shots to take";
			// 
			// label16
			// 
			this.label16.AutoSize = true;
			this.label16.Location = new System.Drawing.Point(12, 102);
			this.label16.Name = "label16";
			this.label16.Size = new System.Drawing.Size(100, 13);
			this.label16.TabIndex = 0;
			this.label16.Text = "Screenshot file type";
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Location = new System.Drawing.Point(12, 75);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(107, 13);
			this.label11.TabIndex = 0;
			this.label11.Text = "Multi-screenshot type";
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(12, 49);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(196, 13);
			this.label10.TabIndex = 0;
			this.label10.Text = "Number of frames to wait between steps";
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(12, 25);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(137, 13);
			this.label9.TabIndex = 0;
			this.label9.Text = "Screenshot output directory";
			// 
			// _panoFovInput
			// 
			this._panoFovInput.Location = new System.Drawing.Point(226, 5);
			this._panoFovInput.Name = "_panoFovInput";
			this._panoFovInput.Size = new System.Drawing.Size(109, 20);
			this._panoFovInput.TabIndex = 2;
			this._panoFovInput.Value = 0F;
			// 
			// _panoOverlapInput
			// 
			this._panoOverlapInput.Location = new System.Drawing.Point(226, 31);
			this._panoOverlapInput.Name = "_panoOverlapInput";
			this._panoOverlapInput.Size = new System.Drawing.Size(65, 20);
			this._panoOverlapInput.TabIndex = 3;
			this._panoOverlapInput.Value = 0F;
			// 
			// _lightfieldDistanceInput
			// 
			this._lightfieldDistanceInput.Location = new System.Drawing.Point(226, 5);
			this._lightfieldDistanceInput.Name = "_lightfieldDistanceInput";
			this._lightfieldDistanceInput.Size = new System.Drawing.Size(109, 20);
			this._lightfieldDistanceInput.TabIndex = 1;
			this._lightfieldDistanceInput.Value = 0F;
			// 
			// _lightfieldShotCountInput
			// 
			this._lightfieldShotCountInput.Location = new System.Drawing.Point(226, 31);
			this._lightfieldShotCountInput.Name = "_lightfieldShotCountInput";
			this._lightfieldShotCountInput.Size = new System.Drawing.Size(65, 20);
			this._lightfieldShotCountInput.TabIndex = 2;
			this._lightfieldShotCountInput.Value = 0;
			// 
			// _shotFileTypeInput
			// 
			this._shotFileTypeInput.Location = new System.Drawing.Point(228, 99);
			this._shotFileTypeInput.Name = "_shotFileTypeInput";
			this._shotFileTypeInput.Size = new System.Drawing.Size(170, 21);
			this._shotFileTypeInput.TabIndex = 3;
			this._shotFileTypeInput.Value = -1;
			// 
			// _shotTypeInput
			// 
			this._shotTypeInput.Location = new System.Drawing.Point(228, 73);
			this._shotTypeInput.Name = "_shotTypeInput";
			this._shotTypeInput.Size = new System.Drawing.Size(170, 21);
			this._shotTypeInput.TabIndex = 2;
			this._shotTypeInput.Value = -1;
			this._shotTypeInput.ValueChanged += new System.EventHandler(this._shotTypeInput_ValueChanged);
			// 
			// _frameWaitInput
			// 
			this._frameWaitInput.Location = new System.Drawing.Point(228, 47);
			this._frameWaitInput.Name = "_frameWaitInput";
			this._frameWaitInput.Size = new System.Drawing.Size(65, 20);
			this._frameWaitInput.TabIndex = 1;
			this._frameWaitInput.Value = 0;
			// 
			// _shotOutputFolderInput
			// 
			this._shotOutputFolderInput.Location = new System.Drawing.Point(228, 22);
			this._shotOutputFolderInput.Name = "_shotOutputFolderInput";
			this._shotOutputFolderInput.Size = new System.Drawing.Size(349, 20);
			this._shotOutputFolderInput.TabIndex = 0;
			this._shotOutputFolderInput.Value = "";
			// 
			// _fovSpeedInput
			// 
			this._fovSpeedInput.Location = new System.Drawing.Point(180, 22);
			this._fovSpeedInput.Name = "_fovSpeedInput";
			this._fovSpeedInput.Size = new System.Drawing.Size(109, 20);
			this._fovSpeedInput.TabIndex = 0;
			this._fovSpeedInput.Value = 0F;
			// 
			// _invertYInput
			// 
			this._invertYInput.Location = new System.Drawing.Point(180, 46);
			this._invertYInput.Name = "_invertYInput";
			this._invertYInput.Size = new System.Drawing.Size(20, 20);
			this._invertYInput.TabIndex = 1;
			this._invertYInput.Value = false;
			// 
			// _rotationSpeedInput
			// 
			this._rotationSpeedInput.Location = new System.Drawing.Point(180, 20);
			this._rotationSpeedInput.Name = "_rotationSpeedInput";
			this._rotationSpeedInput.Size = new System.Drawing.Size(109, 20);
			this._rotationSpeedInput.TabIndex = 0;
			this._rotationSpeedInput.Value = 0F;
			// 
			// _cameraControlDeviceInput
			// 
			this._cameraControlDeviceInput.Location = new System.Drawing.Point(161, 124);
			this._cameraControlDeviceInput.Name = "_cameraControlDeviceInput";
			this._cameraControlDeviceInput.Size = new System.Drawing.Size(109, 21);
			this._cameraControlDeviceInput.TabIndex = 4;
			this._cameraControlDeviceInput.Value = -1;
			// 
			// _movementSpeedInput
			// 
			this._movementSpeedInput.Location = new System.Drawing.Point(161, 98);
			this._movementSpeedInput.Name = "_movementSpeedInput";
			this._movementSpeedInput.Size = new System.Drawing.Size(109, 20);
			this._movementSpeedInput.TabIndex = 3;
			this._movementSpeedInput.Value = 0F;
			// 
			// _upMovementInput
			// 
			this._upMovementInput.Location = new System.Drawing.Point(161, 72);
			this._upMovementInput.Name = "_upMovementInput";
			this._upMovementInput.Size = new System.Drawing.Size(109, 20);
			this._upMovementInput.TabIndex = 2;
			this._upMovementInput.Value = 0F;
			// 
			// _slowMovementInput
			// 
			this._slowMovementInput.Location = new System.Drawing.Point(161, 46);
			this._slowMovementInput.Name = "_slowMovementInput";
			this._slowMovementInput.Size = new System.Drawing.Size(109, 20);
			this._slowMovementInput.TabIndex = 1;
			this._slowMovementInput.Value = 0F;
			// 
			// _fastMovementInput
			// 
			this._fastMovementInput.Location = new System.Drawing.Point(161, 20);
			this._fastMovementInput.Name = "_fastMovementInput";
			this._fastMovementInput.Size = new System.Drawing.Size(109, 20);
			this._fastMovementInput.TabIndex = 0;
			this._fastMovementInput.Value = 0F;
			// 
			// SettingEditor
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.groupBox4);
			this.Controls.Add(this.groupBox3);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Name = "SettingEditor";
			this.Size = new System.Drawing.Size(616, 517);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			this.groupBox4.ResumeLayout(false);
			this.groupBox4.PerformLayout();
			this._horizontalPanoramaControlsPanel.ResumeLayout(false);
			this._horizontalPanoramaControlsPanel.PerformLayout();
			this._lightfieldControlsPanel.ResumeLayout(false);
			this._lightfieldControlsPanel.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBox1;
		private IGCSClient.Controls.FloatInput _movementSpeedInput;
		private System.Windows.Forms.Label label4;
		private IGCSClient.Controls.FloatInput _upMovementInput;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label3;
		private IGCSClient.Controls.FloatInput _slowMovementInput;
		private System.Windows.Forms.Label label2;
		private IGCSClient.Controls.FloatInput _fastMovementInput;
		private System.Windows.Forms.Label label1;
		private IGCSClient.Controls.DropDownInput _cameraControlDeviceInput;
		private System.Windows.Forms.GroupBox groupBox2;
		private IGCSClient.Controls.FloatInput _rotationSpeedInput;
		private System.Windows.Forms.Label label6;
		private IGCSClient.Controls.BoolInput _invertYInput;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.GroupBox groupBox3;
		private IGCSClient.Controls.FloatInput _fovSpeedInput;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.GroupBox groupBox4;
		private IGCSClient.Controls.FolderInput _shotOutputFolderInput;
		private System.Windows.Forms.Label label9;
		private IGCSClient.Controls.IntInput _frameWaitInput;
		private System.Windows.Forms.Label label10;
		private IGCSClient.Controls.DropDownInput _shotTypeInput;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.Panel _horizontalPanoramaControlsPanel;
		private System.Windows.Forms.Panel _lightfieldControlsPanel;
		private IGCSClient.Controls.FloatInput _lightfieldDistanceInput;
		private System.Windows.Forms.Label label14;
		private IGCSClient.Controls.IntInput _lightfieldShotCountInput;
		private System.Windows.Forms.Label label15;
		private IGCSClient.Controls.FloatInput _panoFovInput;
		private System.Windows.Forms.Label label12;
		private IGCSClient.Controls.FloatInput _panoOverlapInput;
		private System.Windows.Forms.Label label13;
		private IGCSClient.Controls.DropDownInput _shotFileTypeInput;
		private System.Windows.Forms.Label label16;
	}
}
