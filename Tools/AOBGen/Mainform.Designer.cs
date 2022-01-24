namespace AOBGen
{
	partial class _mainForm
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(_mainForm));
			this._aobResultTextBox = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this._closeButton = new System.Windows.Forms.Button();
			this._copyAOBToClipboardButton = new System.Windows.Forms.Button();
			this._copyAOBAndCodeAsMarkdownButton = new System.Windows.Forms.Button();
			this._codeTextBox = new System.Windows.Forms.RichTextBox();
			this._alsoWildcardOffsetsCheckBox = new System.Windows.Forms.CheckBox();
			this._statusBar = new System.Windows.Forms.StatusStrip();
			this._resultSBPanel = new System.Windows.Forms.ToolStripStatusLabel();
			this._copyrightSBLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this._linkSBPanel = new System.Windows.Forms.ToolStripStatusLabel();
			this._markAOBLinesInAsmCheckbox = new System.Windows.Forms.CheckBox();
			this._statusBar.SuspendLayout();
			this.SuspendLayout();
			// 
			// _aobResultTextBox
			// 
			this._aobResultTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._aobResultTextBox.Font = new System.Drawing.Font("Consolas", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this._aobResultTextBox.Location = new System.Drawing.Point(12, 503);
			this._aobResultTextBox.Multiline = true;
			this._aobResultTextBox.Name = "_aobResultTextBox";
			this._aobResultTextBox.Size = new System.Drawing.Size(840, 71);
			this._aobResultTextBox.TabIndex = 2;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(9, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(535, 13);
			this.label1.TabIndex = 2;
			this.label1.Text = "Assembly (Paste your assembly here, then click one of the copy buttons below to c" +
    "opy the AOB to the clipboard)";
			// 
			// label2
			// 
			this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(9, 487);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(82, 13);
			this.label2.TabIndex = 3;
			this.label2.Text = "Generated AOB";
			// 
			// _closeButton
			// 
			this._closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._closeButton.Location = new System.Drawing.Point(777, 585);
			this._closeButton.Name = "_closeButton";
			this._closeButton.Size = new System.Drawing.Size(75, 23);
			this._closeButton.TabIndex = 6;
			this._closeButton.Text = "&Close";
			this._closeButton.UseVisualStyleBackColor = true;
			this._closeButton.Click += new System.EventHandler(this._closeButton_Click);
			// 
			// _copyAOBToClipboardButton
			// 
			this._copyAOBToClipboardButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this._copyAOBToClipboardButton.Location = new System.Drawing.Point(12, 585);
			this._copyAOBToClipboardButton.Name = "_copyAOBToClipboardButton";
			this._copyAOBToClipboardButton.Size = new System.Drawing.Size(94, 23);
			this._copyAOBToClipboardButton.TabIndex = 3;
			this._copyAOBToClipboardButton.Text = "Copy &AOB";
			this._copyAOBToClipboardButton.UseVisualStyleBackColor = true;
			this._copyAOBToClipboardButton.Click += new System.EventHandler(this._copyAOBToClipboardButton_Click);
			// 
			// _copyAOBAndCodeAsMarkdownButton
			// 
			this._copyAOBAndCodeAsMarkdownButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this._copyAOBAndCodeAsMarkdownButton.Location = new System.Drawing.Point(112, 585);
			this._copyAOBAndCodeAsMarkdownButton.Name = "_copyAOBAndCodeAsMarkdownButton";
			this._copyAOBAndCodeAsMarkdownButton.Size = new System.Drawing.Size(212, 23);
			this._copyAOBAndCodeAsMarkdownButton.TabIndex = 4;
			this._copyAOBAndCodeAsMarkdownButton.Text = "Copy AOB and asm as &Markdown";
			this._copyAOBAndCodeAsMarkdownButton.UseVisualStyleBackColor = true;
			this._copyAOBAndCodeAsMarkdownButton.Click += new System.EventHandler(this._copyAOBAndCodeAsMarkdownButton_Click);
			// 
			// _codeTextBox
			// 
			this._codeTextBox.AcceptsTab = true;
			this._codeTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._codeTextBox.Font = new System.Drawing.Font("Consolas", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this._codeTextBox.HideSelection = false;
			this._codeTextBox.Location = new System.Drawing.Point(12, 26);
			this._codeTextBox.Name = "_codeTextBox";
			this._codeTextBox.ShowSelectionMargin = true;
			this._codeTextBox.Size = new System.Drawing.Size(840, 430);
			this._codeTextBox.TabIndex = 0;
			this._codeTextBox.Text = "";
			this._codeTextBox.WordWrap = false;
			this._codeTextBox.SelectionChanged += new System.EventHandler(this._codeTextBox_SelectionChanged);
			this._codeTextBox.TextChanged += new System.EventHandler(this._codeTextBox_TextChanged);
			// 
			// _alsoWildcardOffsetsCheckBox
			// 
			this._alsoWildcardOffsetsCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this._alsoWildcardOffsetsCheckBox.AutoSize = true;
			this._alsoWildcardOffsetsCheckBox.Location = new System.Drawing.Point(12, 462);
			this._alsoWildcardOffsetsCheckBox.Name = "_alsoWildcardOffsetsCheckBox";
			this._alsoWildcardOffsetsCheckBox.Size = new System.Drawing.Size(122, 17);
			this._alsoWildcardOffsetsCheckBox.TabIndex = 1;
			this._alsoWildcardOffsetsCheckBox.Text = "Also wildcard offsets";
			this._alsoWildcardOffsetsCheckBox.UseVisualStyleBackColor = true;
			this._alsoWildcardOffsetsCheckBox.CheckedChanged += new System.EventHandler(this._alsoWildcardOffsetsCheckBox_CheckedChanged);
			// 
			// _statusBar
			// 
			this._statusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this._resultSBPanel,
            this._copyrightSBLabel,
            this._linkSBPanel});
			this._statusBar.Location = new System.Drawing.Point(0, 616);
			this._statusBar.Name = "_statusBar";
			this._statusBar.Size = new System.Drawing.Size(864, 24);
			this._statusBar.TabIndex = 6;
			this._statusBar.Text = "statusStrip1";
			// 
			// _resultSBPanel
			// 
			this._resultSBPanel.Name = "_resultSBPanel";
			this._resultSBPanel.Size = new System.Drawing.Size(598, 19);
			this._resultSBPanel.Spring = true;
			this._resultSBPanel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// _copyrightSBLabel
			// 
			this._copyrightSBLabel.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Left;
			this._copyrightSBLabel.Name = "_copyrightSBLabel";
			this._copyrightSBLabel.Size = new System.Drawing.Size(67, 19);
			this._copyrightSBLabel.Text = "By Otis_Inf";
			// 
			// _linkSBPanel
			// 
			this._linkSBPanel.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Left;
			this._linkSBPanel.IsLink = true;
			this._linkSBPanel.Name = "_linkSBPanel";
			this._linkSBPanel.Size = new System.Drawing.Size(184, 19);
			this._linkSBPanel.Text = "https://github.com/FransBouma";
			this._linkSBPanel.Click += new System.EventHandler(this._linkSBPanel_Click);
			// 
			// _markAOBLinesInAsmCheckbox
			// 
			this._markAOBLinesInAsmCheckbox.AutoSize = true;
			this._markAOBLinesInAsmCheckbox.Checked = true;
			this._markAOBLinesInAsmCheckbox.CheckState = System.Windows.Forms.CheckState.Checked;
			this._markAOBLinesInAsmCheckbox.Location = new System.Drawing.Point(330, 589);
			this._markAOBLinesInAsmCheckbox.Name = "_markAOBLinesInAsmCheckbox";
			this._markAOBLinesInAsmCheckbox.Size = new System.Drawing.Size(128, 17);
			this._markAOBLinesInAsmCheckbox.TabIndex = 5;
			this._markAOBLinesInAsmCheckbox.Text = "Mark aob lines in asm";
			this._markAOBLinesInAsmCheckbox.UseVisualStyleBackColor = true;
			// 
			// _mainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(864, 640);
			this.Controls.Add(this._markAOBLinesInAsmCheckbox);
			this.Controls.Add(this._alsoWildcardOffsetsCheckBox);
			this.Controls.Add(this._codeTextBox);
			this.Controls.Add(this._copyAOBAndCodeAsMarkdownButton);
			this.Controls.Add(this._copyAOBToClipboardButton);
			this.Controls.Add(this._closeButton);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this._aobResultTextBox);
			this.Controls.Add(this._statusBar);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "_mainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Generate AOB from assembly.";
			this._statusBar.ResumeLayout(false);
			this._statusBar.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.TextBox _aobResultTextBox;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button _closeButton;
		private System.Windows.Forms.Button _copyAOBToClipboardButton;
		private System.Windows.Forms.Button _copyAOBAndCodeAsMarkdownButton;
		private System.Windows.Forms.RichTextBox _codeTextBox;
		private System.Windows.Forms.CheckBox _alsoWildcardOffsetsCheckBox;
		private System.Windows.Forms.StatusStrip _statusBar;
		private System.Windows.Forms.ToolStripStatusLabel _resultSBPanel;
		private System.Windows.Forms.ToolStripStatusLabel _copyrightSBLabel;
		private System.Windows.Forms.ToolStripStatusLabel _linkSBPanel;
		private System.Windows.Forms.CheckBox _markAOBLinesInAsmCheckbox;
	}
}

