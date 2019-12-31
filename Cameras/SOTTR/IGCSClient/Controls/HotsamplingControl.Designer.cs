namespace IGCSClient.Controls
{
	partial class HotsamplingControl
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(HotsamplingControl));
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this._currentResolutionGroupBox = new System.Windows.Forms.GroupBox();
			this._currentHeightTextBox = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this._currentARTextBox = new System.Windows.Forms.TextBox();
			this._currentWidthTextBox = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this._newResolutionGroupBox = new System.Windows.Forms.GroupBox();
			this.label1 = new System.Windows.Forms.Label();
			this._aspectRatioTextBox = new System.Windows.Forms.TextBox();
			this._useWindowBordersCheckBox = new System.Windows.Forms.CheckBox();
			this.label6 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this._setResolutionButton = new System.Windows.Forms.Button();
			this._newHeightUpDown = new System.Windows.Forms.NumericUpDown();
			this._newWidthUpDown = new System.Windows.Forms.NumericUpDown();
			this._warningLabel = new System.Windows.Forms.Button();
			this._refreshButton = new System.Windows.Forms.Button();
			this._availableResolutionsTreeView = new System.Windows.Forms.TreeView();
			this._treeIcons = new System.Windows.Forms.ImageList(this.components);
			this._resolutionRefreshTimer = new System.Windows.Forms.Timer(this.components);
			this.groupBox1.SuspendLayout();
			this._currentResolutionGroupBox.SuspendLayout();
			this._newResolutionGroupBox.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this._newHeightUpDown)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this._newWidthUpDown)).BeginInit();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this._currentResolutionGroupBox);
			this.groupBox1.Controls.Add(this._newResolutionGroupBox);
			this.groupBox1.Controls.Add(this._warningLabel);
			this.groupBox1.Controls.Add(this._refreshButton);
			this.groupBox1.Controls.Add(this._availableResolutionsTreeView);
			this.groupBox1.Location = new System.Drawing.Point(4, 4);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(776, 382);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Game window resolution control";
			// 
			// _currentResolutionGroupBox
			// 
			this._currentResolutionGroupBox.Controls.Add(this._currentHeightTextBox);
			this._currentResolutionGroupBox.Controls.Add(this.label3);
			this._currentResolutionGroupBox.Controls.Add(this.label4);
			this._currentResolutionGroupBox.Controls.Add(this._currentARTextBox);
			this._currentResolutionGroupBox.Controls.Add(this._currentWidthTextBox);
			this._currentResolutionGroupBox.Controls.Add(this.label2);
			this._currentResolutionGroupBox.Location = new System.Drawing.Point(241, 123);
			this._currentResolutionGroupBox.Name = "_currentResolutionGroupBox";
			this._currentResolutionGroupBox.Size = new System.Drawing.Size(315, 79);
			this._currentResolutionGroupBox.TabIndex = 1;
			this._currentResolutionGroupBox.TabStop = false;
			this._currentResolutionGroupBox.Text = "Active window resolution";
			// 
			// _currentHeightTextBox
			// 
			this._currentHeightTextBox.Location = new System.Drawing.Point(56, 47);
			this._currentHeightTextBox.Name = "_currentHeightTextBox";
			this._currentHeightTextBox.ReadOnly = true;
			this._currentHeightTextBox.Size = new System.Drawing.Size(58, 20);
			this._currentHeightTextBox.TabIndex = 1;
			this._currentHeightTextBox.TabStop = false;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(8, 50);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(38, 13);
			this.label3.TabIndex = 0;
			this.label3.Text = "Height";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(137, 24);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(63, 13);
			this.label4.TabIndex = 0;
			this.label4.Text = "Aspect ratio";
			// 
			// _currentARTextBox
			// 
			this._currentARTextBox.Location = new System.Drawing.Point(206, 21);
			this._currentARTextBox.Name = "_currentARTextBox";
			this._currentARTextBox.ReadOnly = true;
			this._currentARTextBox.Size = new System.Drawing.Size(58, 20);
			this._currentARTextBox.TabIndex = 1;
			this._currentARTextBox.TabStop = false;
			// 
			// _currentWidthTextBox
			// 
			this._currentWidthTextBox.Location = new System.Drawing.Point(56, 21);
			this._currentWidthTextBox.Name = "_currentWidthTextBox";
			this._currentWidthTextBox.ReadOnly = true;
			this._currentWidthTextBox.Size = new System.Drawing.Size(58, 20);
			this._currentWidthTextBox.TabIndex = 1;
			this._currentWidthTextBox.TabStop = false;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(8, 24);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(35, 13);
			this.label2.TabIndex = 0;
			this.label2.Text = "Width";
			// 
			// _newResolutionGroupBox
			// 
			this._newResolutionGroupBox.Controls.Add(this.label1);
			this._newResolutionGroupBox.Controls.Add(this._aspectRatioTextBox);
			this._newResolutionGroupBox.Controls.Add(this._useWindowBordersCheckBox);
			this._newResolutionGroupBox.Controls.Add(this.label6);
			this._newResolutionGroupBox.Controls.Add(this.label5);
			this._newResolutionGroupBox.Controls.Add(this._setResolutionButton);
			this._newResolutionGroupBox.Controls.Add(this._newHeightUpDown);
			this._newResolutionGroupBox.Controls.Add(this._newWidthUpDown);
			this._newResolutionGroupBox.Location = new System.Drawing.Point(240, 14);
			this._newResolutionGroupBox.Name = "_newResolutionGroupBox";
			this._newResolutionGroupBox.Size = new System.Drawing.Size(315, 103);
			this._newResolutionGroupBox.TabIndex = 0;
			this._newResolutionGroupBox.TabStop = false;
			this._newResolutionGroupBox.Text = "New resolution to set";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(8, 46);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(38, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Height";
			// 
			// _aspectRatioTextBox
			// 
			this._aspectRatioTextBox.Location = new System.Drawing.Point(207, 17);
			this._aspectRatioTextBox.Name = "_aspectRatioTextBox";
			this._aspectRatioTextBox.ReadOnly = true;
			this._aspectRatioTextBox.Size = new System.Drawing.Size(58, 20);
			this._aspectRatioTextBox.TabIndex = 1;
			this._aspectRatioTextBox.TabStop = false;
			// 
			// _useWindowBordersCheckBox
			// 
			this._useWindowBordersCheckBox.AutoSize = true;
			this._useWindowBordersCheckBox.Location = new System.Drawing.Point(141, 45);
			this._useWindowBordersCheckBox.Name = "_useWindowBordersCheckBox";
			this._useWindowBordersCheckBox.Size = new System.Drawing.Size(103, 17);
			this._useWindowBordersCheckBox.TabIndex = 3;
			this._useWindowBordersCheckBox.Text = "Window borders";
			this._useWindowBordersCheckBox.UseVisualStyleBackColor = true;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(138, 20);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(63, 13);
			this.label6.TabIndex = 0;
			this.label6.Text = "Aspect ratio";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(8, 20);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(35, 13);
			this.label5.TabIndex = 0;
			this.label5.Text = "Width";
			// 
			// _setResolutionButton
			// 
			this._setResolutionButton.Enabled = false;
			this._setResolutionButton.Location = new System.Drawing.Point(56, 69);
			this._setResolutionButton.Name = "_setResolutionButton";
			this._setResolutionButton.Size = new System.Drawing.Size(60, 23);
			this._setResolutionButton.TabIndex = 2;
			this._setResolutionButton.Text = "Set";
			this._setResolutionButton.UseVisualStyleBackColor = true;
			this._setResolutionButton.Click += new System.EventHandler(this._setResolutionButton_Click);
			// 
			// _newHeightUpDown
			// 
			this._newHeightUpDown.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this._newHeightUpDown.Location = new System.Drawing.Point(57, 44);
			this._newHeightUpDown.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
			this._newHeightUpDown.Name = "_newHeightUpDown";
			this._newHeightUpDown.Size = new System.Drawing.Size(58, 20);
			this._newHeightUpDown.TabIndex = 1;
			this._newHeightUpDown.ValueChanged += new System.EventHandler(this._newHeightUpDown_ValueChanged);
			// 
			// _newWidthUpDown
			// 
			this._newWidthUpDown.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this._newWidthUpDown.Location = new System.Drawing.Point(57, 18);
			this._newWidthUpDown.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
			this._newWidthUpDown.Name = "_newWidthUpDown";
			this._newWidthUpDown.Size = new System.Drawing.Size(58, 20);
			this._newWidthUpDown.TabIndex = 0;
			this._newWidthUpDown.ValueChanged += new System.EventHandler(this._newWidthUpDown_ValueChanged);
			// 
			// _warningLabel
			// 
			this._warningLabel.AutoEllipsis = true;
			this._warningLabel.FlatAppearance.BorderSize = 0;
			this._warningLabel.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
			this._warningLabel.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
			this._warningLabel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this._warningLabel.Image = ((System.Drawing.Image)(resources.GetObject("_warningLabel.Image")));
			this._warningLabel.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this._warningLabel.Location = new System.Drawing.Point(241, 208);
			this._warningLabel.Name = "_warningLabel";
			this._warningLabel.Size = new System.Drawing.Size(285, 36);
			this._warningLabel.TabIndex = 5;
			this._warningLabel.TabStop = false;
			this._warningLabel.Text = "Be aware that choosing a high resolution could make the game crash due to insuffi" +
    "cient memory available. ";
			this._warningLabel.TextAlign = System.Drawing.ContentAlignment.TopLeft;
			this._warningLabel.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this._warningLabel.UseVisualStyleBackColor = true;
			// 
			// _refreshButton
			// 
			this._refreshButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this._refreshButton.Location = new System.Drawing.Point(240, 354);
			this._refreshButton.Name = "_refreshButton";
			this._refreshButton.Size = new System.Drawing.Size(75, 23);
			this._refreshButton.TabIndex = 2;
			this._refreshButton.Text = "Refresh";
			this._refreshButton.UseVisualStyleBackColor = true;
			this._refreshButton.Click += new System.EventHandler(this._refreshButton_Click);
			// 
			// _availableResolutionsTreeView
			// 
			this._availableResolutionsTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this._availableResolutionsTreeView.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._availableResolutionsTreeView.HideSelection = false;
			this._availableResolutionsTreeView.ImageIndex = 0;
			this._availableResolutionsTreeView.ImageList = this._treeIcons;
			this._availableResolutionsTreeView.Location = new System.Drawing.Point(7, 20);
			this._availableResolutionsTreeView.Name = "_availableResolutionsTreeView";
			this._availableResolutionsTreeView.SelectedImageIndex = 0;
			this._availableResolutionsTreeView.Size = new System.Drawing.Size(227, 356);
			this._availableResolutionsTreeView.TabIndex = 0;
			this._availableResolutionsTreeView.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this._availableResolutionsTreeView_BeforeCollapse);
			this._availableResolutionsTreeView.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this._availableResolutionsTreeView_BeforeExpand);
			this._availableResolutionsTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this._availableResolutionsTreeView_AfterSelect);
			// 
			// _treeIcons
			// 
			this._treeIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("_treeIcons.ImageStream")));
			this._treeIcons.TransparentColor = System.Drawing.Color.Transparent;
			this._treeIcons.Images.SetKeyName(0, "Monitor_16x.png");
			this._treeIcons.Images.SetKeyName(1, "Folder_16x.png");
			this._treeIcons.Images.SetKeyName(2, "FolderOpen_16x.png");
			// 
			// _resolutionRefreshTimer
			// 
			this._resolutionRefreshTimer.Interval = 1000;
			this._resolutionRefreshTimer.Tick += new System.EventHandler(this._resolutionRefreshTimer_Tick);
			// 
			// HotsamplingControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.groupBox1);
			this.Name = "HotsamplingControl";
			this.Size = new System.Drawing.Size(783, 389);
			this.groupBox1.ResumeLayout(false);
			this._currentResolutionGroupBox.ResumeLayout(false);
			this._currentResolutionGroupBox.PerformLayout();
			this._newResolutionGroupBox.ResumeLayout(false);
			this._newResolutionGroupBox.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this._newHeightUpDown)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this._newWidthUpDown)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TreeView _availableResolutionsTreeView;
		private System.Windows.Forms.Button _refreshButton;
		private System.Windows.Forms.ImageList _treeIcons;
		private System.Windows.Forms.GroupBox _newResolutionGroupBox;
		private System.Windows.Forms.Button _setResolutionButton;
		private System.Windows.Forms.NumericUpDown _newHeightUpDown;
		private System.Windows.Forms.NumericUpDown _newWidthUpDown;
		private System.Windows.Forms.CheckBox _useWindowBordersCheckBox;
		private System.Windows.Forms.Button _warningLabel;
		private System.Windows.Forms.GroupBox _currentResolutionGroupBox;
		private System.Windows.Forms.TextBox _currentHeightTextBox;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox _currentWidthTextBox;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox _aspectRatioTextBox;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox _currentARTextBox;
		private System.Windows.Forms.Timer _resolutionRefreshTimer;
	}
}
