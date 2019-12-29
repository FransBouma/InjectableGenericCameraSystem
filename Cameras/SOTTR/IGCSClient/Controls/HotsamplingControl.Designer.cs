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
			System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("1920x1200 (1x)");
			System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("2400x1500 (1.25x)");
			System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode("2880x1800 (1.5x)");
			System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode("16:10 (Monitor aspect ratio)", new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3});
			System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode("21:9");
			System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode("1:1");
			System.Windows.Forms.TreeNode treeNode7 = new System.Windows.Forms.TreeNode("9:16");
			System.Windows.Forms.TreeNode treeNode8 = new System.Windows.Forms.TreeNode("4:5");
			System.Windows.Forms.TreeNode treeNode9 = new System.Windows.Forms.TreeNode("Available resolutions", new System.Windows.Forms.TreeNode[] {
            treeNode4,
            treeNode5,
            treeNode6,
            treeNode7,
            treeNode8});
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this._availableResolutionsTreeView = new System.Windows.Forms.TreeView();
			this._refreshButton = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.groupBox1.Controls.Add(this._refreshButton);
			this.groupBox1.Controls.Add(this._availableResolutionsTreeView);
			this.groupBox1.Location = new System.Drawing.Point(4, 4);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(562, 382);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Game window resolution control";
			// 
			// _availableResolutionsTreeView
			// 
			this._availableResolutionsTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this._availableResolutionsTreeView.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._availableResolutionsTreeView.HideSelection = false;
			this._availableResolutionsTreeView.Location = new System.Drawing.Point(7, 20);
			this._availableResolutionsTreeView.Name = "_availableResolutionsTreeView";
			treeNode1.Name = "Node6";
			treeNode1.Text = "1920x1200 (1x)";
			treeNode2.Name = "Node7";
			treeNode2.Text = "2400x1500 (1.25x)";
			treeNode3.Name = "Node8";
			treeNode3.Text = "2880x1800 (1.5x)";
			treeNode4.Name = "Node1";
			treeNode4.Text = "16:10 (Monitor aspect ratio)";
			treeNode5.Name = "Node2";
			treeNode5.Text = "21:9";
			treeNode6.Name = "Node3";
			treeNode6.Text = "1:1";
			treeNode7.Name = "Node4";
			treeNode7.Text = "9:16";
			treeNode8.Name = "Node5";
			treeNode8.Text = "4:5";
			treeNode9.Name = "Node0";
			treeNode9.Text = "Available resolutions";
			this._availableResolutionsTreeView.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode9});
			this._availableResolutionsTreeView.Size = new System.Drawing.Size(202, 356);
			this._availableResolutionsTreeView.TabIndex = 0;
			// 
			// _refreshButton
			// 
			this._refreshButton.Location = new System.Drawing.Point(215, 353);
			this._refreshButton.Name = "_refreshButton";
			this._refreshButton.Size = new System.Drawing.Size(75, 23);
			this._refreshButton.TabIndex = 1;
			this._refreshButton.Text = "Refresh";
			this._refreshButton.UseVisualStyleBackColor = true;
			// 
			// HotsamplingControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.groupBox1);
			this.Name = "HotsamplingControl";
			this.Size = new System.Drawing.Size(569, 389);
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TreeView _availableResolutionsTreeView;
		private System.Windows.Forms.Button _refreshButton;
	}
}
