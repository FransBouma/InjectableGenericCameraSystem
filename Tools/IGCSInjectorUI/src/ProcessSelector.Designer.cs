namespace IGCSInjectorUI
{
	partial class ProcessSelector
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
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
			this._processGrid = new System.Windows.Forms.DataGridView();
			this.COL_ICON = new System.Windows.Forms.DataGridViewImageColumn();
			this.COL_PROCESS_NAME = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.COL_WINDOW_TITLE = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.COL_LOCATION = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this._cancelButton = new System.Windows.Forms.Button();
			this._selectButton = new System.Windows.Forms.Button();
			this._refreshButton = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this._processGrid)).BeginInit();
			this.SuspendLayout();
			// 
			// _processGrid
			// 
			this._processGrid.AllowUserToAddRows = false;
			this._processGrid.AllowUserToDeleteRows = false;
			this._processGrid.AllowUserToResizeRows = false;
			this._processGrid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this._processGrid.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
			this._processGrid.BackgroundColor = System.Drawing.SystemColors.Window;
			this._processGrid.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
			dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
			dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
			dataGridViewCellStyle1.Padding = new System.Windows.Forms.Padding(0, 3, 0, 4);
			dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
			this._processGrid.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this._processGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this._processGrid.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.COL_ICON,
            this.COL_PROCESS_NAME,
            this.COL_WINDOW_TITLE,
            this.COL_LOCATION});
			this._processGrid.Location = new System.Drawing.Point(13, 13);
			this._processGrid.Margin = new System.Windows.Forms.Padding(4);
			this._processGrid.MultiSelect = false;
			this._processGrid.Name = "_processGrid";
			this._processGrid.ReadOnly = true;
			this._processGrid.RowHeadersVisible = false;
			this._processGrid.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
			this._processGrid.Size = new System.Drawing.Size(651, 331);
			this._processGrid.TabIndex = 4;
			this._processGrid.CellContentDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this._processGrid_CellContentDoubleClick);
			// 
			// COL_ICON
			// 
			this.COL_ICON.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.DisplayedCellsExceptHeader;
			this.COL_ICON.DataPropertyName = "ModuleIcon";
			this.COL_ICON.HeaderText = "";
			this.COL_ICON.Name = "COL_ICON";
			this.COL_ICON.ReadOnly = true;
			this.COL_ICON.Width = 5;
			// 
			// COL_PROCESS_NAME
			// 
			this.COL_PROCESS_NAME.DataPropertyName = "ProcessName";
			dataGridViewCellStyle2.Padding = new System.Windows.Forms.Padding(3, 0, 0, 0);
			this.COL_PROCESS_NAME.DefaultCellStyle = dataGridViewCellStyle2;
			this.COL_PROCESS_NAME.HeaderText = "Process Name";
			this.COL_PROCESS_NAME.Name = "COL_PROCESS_NAME";
			this.COL_PROCESS_NAME.ReadOnly = true;
			this.COL_PROCESS_NAME.Width = 160;
			// 
			// COL_WINDOW_TITLE
			// 
			this.COL_WINDOW_TITLE.DataPropertyName = "WindowTitle";
			this.COL_WINDOW_TITLE.HeaderText = "Window Title";
			this.COL_WINDOW_TITLE.Name = "COL_WINDOW_TITLE";
			this.COL_WINDOW_TITLE.ReadOnly = true;
			this.COL_WINDOW_TITLE.Width = 240;
			// 
			// COL_LOCATION
			// 
			this.COL_LOCATION.DataPropertyName = "FileName";
			this.COL_LOCATION.HeaderText = "Location";
			this.COL_LOCATION.Name = "COL_LOCATION";
			this.COL_LOCATION.ReadOnly = true;
			this.COL_LOCATION.Width = 240;
			// 
			// _cancelButton
			// 
			this._cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._cancelButton.Location = new System.Drawing.Point(584, 352);
			this._cancelButton.Margin = new System.Windows.Forms.Padding(4);
			this._cancelButton.Name = "_cancelButton";
			this._cancelButton.Size = new System.Drawing.Size(80, 23);
			this._cancelButton.TabIndex = 7;
			this._cancelButton.Text = "Cancel";
			this._cancelButton.UseVisualStyleBackColor = true;
			this._cancelButton.Click += new System.EventHandler(this._cancelButton_Click);
			// 
			// _selectButton
			// 
			this._selectButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._selectButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this._selectButton.Location = new System.Drawing.Point(496, 352);
			this._selectButton.Margin = new System.Windows.Forms.Padding(4);
			this._selectButton.Name = "_selectButton";
			this._selectButton.Size = new System.Drawing.Size(80, 23);
			this._selectButton.TabIndex = 5;
			this._selectButton.Text = "Select";
			this._selectButton.UseVisualStyleBackColor = true;
			this._selectButton.Click += new System.EventHandler(this._selectButton_Click);
			// 
			// _refreshButton
			// 
			this._refreshButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this._refreshButton.Location = new System.Drawing.Point(13, 352);
			this._refreshButton.Margin = new System.Windows.Forms.Padding(4);
			this._refreshButton.Name = "_refreshButton";
			this._refreshButton.Size = new System.Drawing.Size(80, 23);
			this._refreshButton.TabIndex = 6;
			this._refreshButton.Text = "Refresh";
			this._refreshButton.UseVisualStyleBackColor = true;
			this._refreshButton.Click += new System.EventHandler(this._refreshButton_Click);
			// 
			// ProcessSelector
			// 
			this.AcceptButton = this._selectButton;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this._selectButton;
			this.ClientSize = new System.Drawing.Size(677, 386);
			this.Controls.Add(this._processGrid);
			this.Controls.Add(this._cancelButton);
			this.Controls.Add(this._selectButton);
			this.Controls.Add(this._refreshButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
			this.Name = "ProcessSelector";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Please select the process to inject the dll into";
			((System.ComponentModel.ISupportInitialize)(this._processGrid)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		internal System.Windows.Forms.DataGridView _processGrid;
		private System.Windows.Forms.DataGridViewImageColumn COL_ICON;
		private System.Windows.Forms.DataGridViewTextBoxColumn COL_PROCESS_NAME;
		private System.Windows.Forms.DataGridViewTextBoxColumn COL_WINDOW_TITLE;
		private System.Windows.Forms.DataGridViewTextBoxColumn COL_LOCATION;
		internal System.Windows.Forms.Button _cancelButton;
		internal System.Windows.Forms.Button _selectButton;
		private System.Windows.Forms.Button _refreshButton;
	}
}