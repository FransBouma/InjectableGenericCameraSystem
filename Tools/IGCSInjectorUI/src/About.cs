using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing.Imaging;

namespace IGCSInjectorUI
{
	internal class About : Form
	{
		#region Class Member Declarations
		private readonly int _logoWidth;
		private readonly int _logoHeigth;
		private readonly int _creditsScrollWidth;
		private readonly int _creditsScrollHeight;

		private readonly Bitmap	_logoImage;
		private readonly Bitmap _creditsScrollImage;
		private bool	_zoomIn, _doScroll;
		private int     _zoomCounter;
		private int _creditScrollStartLine;
		private	double  _gamma;
		private readonly double  _xZoomDelta;
		private readonly double  _yZoomDelta;

		// coordinate source.
		private readonly double[]	_xsCoords={-128,128,-128};
		private readonly double[]	_ysCoords={-64,-64,64};

		// coordinate destination. (for drawing).
		private readonly double[]	_xdCoords={-128,128,-128};
		private readonly double[]	_ydCoords={-64,-64,64};

		private readonly uint[]	_logoSrcPixels;
		private readonly uint[] _creditsScrollSrcPixels;

		private const double _dgamma = 0.15;
		private const int _layerFadeAlpha = 230;
		#endregion

		#region Controls

		private System.Windows.Forms.Button _okButton;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.LinkLabel linkLabel1;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Timer _animTimer;
		private PictureBox _logoPictureBox;
		private PictureBox _creditsPictureBox;
		private System.ComponentModel.IContainer components;
		#endregion


		/// <summary>
		/// Initializes a new instance of the <see cref="About"/> class.
		/// </summary>
		internal About()
		{
			InitializeComponent();
			this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer, true);

			// load logo image
			_logoImage = (Bitmap)_logoPictureBox.Image;
			_logoWidth = _logoImage.Width;
			_logoHeigth = _logoImage.Height;
			_logoSrcPixels = new uint[_logoWidth * _logoHeigth];

			// load credit scroller image
			_creditsScrollImage = (Bitmap)_creditsPictureBox.Image;
			_creditsScrollWidth = _creditsScrollImage.Width;
			_creditsScrollHeight = _creditsScrollImage.Height;
			_creditsScrollSrcPixels = new uint[_creditsScrollWidth * _creditsScrollHeight];

			// set layer fade delta. This is the fade delta increased per pixel from the left edge to the middle of the buffer, where it reaches the
			// destination level. 

			// read the initial pixels into the srcpixel array. This makes it possible to perform an in-place rendering to avoid memory trashing
			GrabPixels(_logoImage, _logoWidth, _logoHeigth, _logoSrcPixels);
			GrabPixels(_creditsScrollImage, _creditsScrollWidth, _creditsScrollHeight, _creditsScrollSrcPixels);

			// init coord arrays
			double halfWidth = _logoWidth / 2.0;
			_xsCoords[0] = -halfWidth;
			_xsCoords[1] = halfWidth;
			_xsCoords[2] = -halfWidth;
			_xdCoords[0] = -halfWidth;
			_xdCoords[1] = halfWidth;
			_xdCoords[2] = -halfWidth;

			double halfHeight = _logoHeigth / 2.0;
			_ysCoords[0] = -halfHeight;
			_ysCoords[1] = -halfHeight;
			_ysCoords[2] = halfHeight;
			_ydCoords[0] = -halfHeight;
			_ydCoords[1] = -halfHeight;
			_ydCoords[2] = halfHeight;

			_gamma=0;
			_creditScrollStartLine = 0;
			// calculate zoomdelta's.
			_xZoomDelta=7.0; // 2.0;
			_yZoomDelta = 3.5;// 0.5;
		}


		/// <summary>
		/// Grabs the pixels.
		/// </summary>
		/// <param name="toGrab">To grab.</param>
		/// <param name="width">The width.</param>
		/// <param name="heigth">The heigth.</param>
		/// <param name="destinationArray">The destination array.</param>
		private static void GrabPixels(Bitmap toGrab, int width, int heigth, uint[] destinationArray)
		{
			BitmapData srcData = toGrab.LockBits(new Rectangle(0, 0, width, heigth), ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
			unsafe
			{
				uint *pSrc = (uint *)(void *)srcData.Scan0;

				for(int i=0;i<heigth;i++)
				{
					for(int j=0;j<width;j++)
					{
						destinationArray[(i * width) + j] = pSrc[(i * width) + j];
					}
				}
			}
			toGrab.UnlockBits(srcData);
		}


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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(About));
			this._okButton = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.linkLabel1 = new System.Windows.Forms.LinkLabel();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this._animTimer = new System.Windows.Forms.Timer(this.components);
			this._creditsPictureBox = new System.Windows.Forms.PictureBox();
			this._logoPictureBox = new System.Windows.Forms.PictureBox();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this._creditsPictureBox)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this._logoPictureBox)).BeginInit();
			this.SuspendLayout();
			// 
			// _okButton
			// 
			this._okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._okButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this._okButton.Location = new System.Drawing.Point(360, 365);
			this._okButton.Name = "_okButton";
			this._okButton.Size = new System.Drawing.Size(72, 23);
			this._okButton.TabIndex = 0;
			this._okButton.Text = "OK";
			this._okButton.Click += new System.EventHandler(this._okButton_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this.linkLabel1);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Location = new System.Drawing.Point(11, 227);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(427, 83);
			this.groupBox1.TabIndex = 2;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Application information";
			// 
			// linkLabel1
			// 
			this.linkLabel1.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.linkLabel1.Location = new System.Drawing.Point(54, 57);
			this.linkLabel1.Name = "linkLabel1";
			this.linkLabel1.Size = new System.Drawing.Size(318, 19);
			this.linkLabel1.TabIndex = 4;
			this.linkLabel1.TabStop = true;
			this.linkLabel1.Text = "https://github.com/FransBouma/InjectableGenericCameraSystem";
			this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(136, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(155, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "IGCS Injector is ©2020 Otis_Inf";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(166, 33);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(95, 18);
			this.label2.TabIndex = 0;
			this.label2.Text = "All rights reserved.";
			// 
			// label4
			// 
			this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label4.Location = new System.Drawing.Point(9, 323);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(303, 66);
			this.label4.TabIndex = 3;
			this.label4.Text = resources.GetString("label4.Text");
			// 
			// _animTimer
			// 
			this._animTimer.Enabled = true;
			this._animTimer.Interval = 20;
			this._animTimer.Tick += new System.EventHandler(this._animTimer_Tick);
			// 
			// _creditsPictureBox
			// 
			this._creditsPictureBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this._creditsPictureBox.Image = ((System.Drawing.Image)(resources.GetObject("_creditsPictureBox.Image")));
			this._creditsPictureBox.InitialImage = null;
			this._creditsPictureBox.Location = new System.Drawing.Point(345, 324);
			this._creditsPictureBox.Name = "_creditsPictureBox";
			this._creditsPictureBox.Size = new System.Drawing.Size(74, 35);
			this._creditsPictureBox.TabIndex = 4;
			this._creditsPictureBox.TabStop = false;
			this._creditsPictureBox.Visible = false;
			// 
			// _logoPictureBox
			// 
			this._logoPictureBox.BackColor = System.Drawing.Color.White;
			this._logoPictureBox.Dock = System.Windows.Forms.DockStyle.Top;
			this._logoPictureBox.Image = global::IGCSInjectorUI.MainResources.IGCS_SplashScreen;
			this._logoPictureBox.InitialImage = null;
			this._logoPictureBox.Location = new System.Drawing.Point(0, 0);
			this._logoPictureBox.Name = "_logoPictureBox";
			this._logoPictureBox.Size = new System.Drawing.Size(450, 217);
			this._logoPictureBox.TabIndex = 0;
			this._logoPictureBox.TabStop = false;
			// 
			// About
			// 
			this.AcceptButton = this._okButton;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(450, 398);
			this.Controls.Add(this._creditsPictureBox);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this._okButton);
			this.Controls.Add(this._logoPictureBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "About";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "About IGCS Injector";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.About_Closing);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this._creditsPictureBox)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this._logoPictureBox)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// Handles the Tick event of the _animTimer control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The event arguments instance containing the event data.</param>
		private void _animTimer_Tick(object sender, EventArgs e)
		{
			Zoom();
			Rotate();
			MoveCreditsScrollStartLine();
			RenderFrame();
		}


		/// <summary>
		/// Moves the credits scroll start line.
		/// </summary>
		private void MoveCreditsScrollStartLine()
		{
			if(_doScroll)
			{
				_creditScrollStartLine++;
				if(_creditScrollStartLine >= _creditsScrollHeight)
				{
					_creditScrollStartLine = 0;
				}
				_doScroll=false;
			}
			else
			{
				_doScroll=true;
			}
		}


		/// <summary>
		/// Zooms the rectangle used as a viewport on the sourceimage
		/// </summary>
		private void Zoom()
		{
			if(_zoomIn)
			{
				for(int i=0;i<3;i++)
				{
					if(_xsCoords[i]<0)
					{
						_xsCoords[i]+=_xZoomDelta;
					}
					else
					{
						_xsCoords[i]-=_xZoomDelta;
					}
					if(_ysCoords[i]<0)
					{
						_ysCoords[i]+=_yZoomDelta;
					}
					else
					{
						_ysCoords[i]-=_yZoomDelta;
					}
				}
				_zoomCounter++;
				if(_zoomCounter>=10)
				{
					_zoomIn=false;
				}
			}
			else
			{
				for(int i=0;i<3;i++)
				{
					if(_xsCoords[i]<0)
					{
						_xsCoords[i]-=_xZoomDelta;
					}
					else
					{
						_xsCoords[i]+=_xZoomDelta;
					}
					if(_ysCoords[i]<0)
					{
						_ysCoords[i]-=_yZoomDelta;
					}
					else
					{
						_ysCoords[i]+=_yZoomDelta;
					}
				}
				_zoomCounter--;
				if(_zoomCounter<-200)
				{
					_zoomIn=true;
				}
			}
		}


		/// <summary>
		/// Renders the rotating logo.
		/// </summary>
		private void RenderFrame()
		{
			BitmapData imageDst = _logoImage.LockBits(new Rectangle(0, 0, _logoWidth, _logoHeigth), ImageLockMode.ReadWrite, PixelFormat.Format32bppRgb);
			IntPtr scan0 = imageDst.Scan0;

			DoRotozoomer(scan0);
			RenderCreditsScroll(imageDst);
			_logoImage.UnlockBits(imageDst);
			_logoPictureBox.Image = _logoImage;
		}


		/// <summary>
		/// Renders the rotozoomer effect
		/// </summary>
		/// <param name="scan0">pointer to the first byte in the buffer</param>
		private void DoRotozoomer(IntPtr scan0)
		{
			unsafe
			{
				double xa = _xdCoords[0];
				double xb = _xdCoords[1];
				double xc = _xdCoords[2];
				double ya = _ydCoords[0];
				double yb = _ydCoords[1];
				double yc = _ydCoords[2];
				double xab_delta = (xb - xa) / _logoWidth;
				double yab_delta = (yb - ya) / _logoWidth;
				double xac_delta = (xc - xa) / _logoHeigth;
				double yac_delta = (yc - ya) / _logoHeigth;

				// transpose the rotating centre to the middle of the picture.
				double x_off = xa + (_logoWidth * 0.5);
				double y_off = ya + (_logoHeigth * 0.5);

				uint *pDest = (uint *)(void *)scan0;

				for(int i=0;i<_logoHeigth;i++)
				{
					for(int j=0;j<_logoWidth;j++)
					{
						int readX=((int)x_off);
						int readY=((int)y_off);
						if(readX<0)
						{
							readX = (_logoWidth-1) - (-readX % (_logoWidth-1));
						}
						if(readY<0)
						{
							readY = (_logoHeigth-1) - (-readY % (_logoHeigth-1));
						}
						if(readX>=_logoWidth)
						{
							readX = 0 + (readX % (_logoWidth-1));
						}
						if(readY>=_logoHeigth)
						{
							readY = 0 + (readY % (_logoHeigth-1));
						}
						uint pixelBits = _logoSrcPixels[(readY * _logoWidth) + readX];
						pDest[0] = BlendPixels(0, pixelBits, 230);
						x_off+=xab_delta;
						y_off+=yab_delta;
						pDest++;
					}
					double iD = i;
					x_off=xa + (iD * (xac_delta - (0.004 * iD))) + (_logoWidth * 0.5);
					y_off=ya + (iD * (yac_delta + (0.003 * iD))) + (_logoHeigth * 0.5);
				}
			}
		}


		/// <summary>
		/// Renders the credits scroll on top of the framebuffer using blending.
		/// </summary>
		/// <param name="frameBuffer">The frame buffer.</param>
		private void RenderCreditsScroll(BitmapData frameBuffer)
		{
			const int destinationStartX = 0;
			IntPtr scan0 = frameBuffer.Scan0;

			unsafe
			{
				uint* pDest = (uint*)(void*)scan0;
				for(int i = 0; i < _logoHeigth; i++)
				{
					int lineIndex = _creditScrollStartLine+i;
					if(lineIndex>=_creditsScrollHeight)
					{
						lineIndex = (_creditScrollStartLine + i) - _creditsScrollHeight;
					}
					for(int j = 0; j < _creditsScrollWidth; j++)
					{
						uint creditsScrollPixel = _creditsScrollSrcPixels[(lineIndex*_creditsScrollWidth) + j];
						int destinationIndex = (i * _logoWidth) + j + destinationStartX;
						uint framebufferPixel = pDest[destinationIndex];
						uint destinationPixel = BlendPixels(framebufferPixel, creditsScrollPixel, 255);
						pDest[destinationIndex]=destinationPixel;
					}
				}
			}
		}


		/// <summary>
		/// Blends the pixels pixelBits1 and pixelBits2 together using the mask specified.
		/// </summary>
		/// <param name="pixelBits1">The pixel bits1.</param>
		/// <param name="pixelBits2">The pixel bits2.</param>
		/// <param name="mask">The mask.</param>
		/// <returns></returns>
		private static uint BlendPixels(uint pixelBits1, uint pixelBits2, int mask)
		{
			int alpha = (int)((pixelBits2 & 0xff000000) >> 24);
			int maskToUse = (int)(mask * (alpha / 255.0));
			int invertMask = 255 - maskToUse;
			uint red1 = (pixelBits1 & 0x00ff0000);
			uint green1 = (pixelBits1 & 0x0000ff00);
			uint blue1 = (pixelBits1 & 0x000000ff);
			uint red2 = (pixelBits2 & 0x00ff0000);
			uint green2 = (pixelBits2 & 0x0000ff00);
			uint blue2 = (pixelBits2 & 0x000000ff);

			uint destinationRed = ((uint)((red1 * invertMask) + (red2 * maskToUse)) >> 8) & 0x00ff0000;
			uint destinationGreen = ((uint)((green1 * invertMask) + (green2 * maskToUse))>>8) & 0x0000ff00;
			uint destinationBlue = ((uint)((blue1 * invertMask) + (blue2 * maskToUse)) >> 8) & 0x000000ff;

			return 0xff000000 | destinationRed | destinationGreen | destinationBlue;
		}


		/// <summary>
		/// Rotates the rectangle which is used as a viewport on the source image data
		/// </summary>
		public void Rotate()
		{
			// first update the angle. only gamma is interesting...
			_gamma=(_gamma + _dgamma)%360;
			double gammaRad=(_gamma/360) * 2 * Math.PI;
		
			// get the sin and cos of the angles
			double sinG= Math.Sin(gammaRad);		
			double cosG = Math.Cos(gammaRad);		

			// now rotate the coords...
			for(int i=0;i<3;i++)
			{
				double yn = _ysCoords[i];
				double xn = _xsCoords[i];

				_xdCoords[i] = (xn * cosG) - (yn * sinG);
				_ydCoords[i] = (yn * cosG) + (xn * sinG);
			}
		}



		/// <summary>
		/// Handles the Closing event of the About control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The event arguments instance containing the event data.</param>
		private void About_Closing(object sender, CancelEventArgs e)
		{
			_animTimer.Tick-= new EventHandler(this._animTimer_Tick);
			_animTimer.Stop();
			_animTimer.Enabled=false;
		}


		/// <summary>
		/// Handles the Click event of the _okButton control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The event arguments instance containing the event data.</param>
		private void _okButton_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		/// <summary>
		/// Handles the LinkClicked event of the linkLabel1 control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The event arguments instance containing the event data.</param>
		private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
		{
			System.Diagnostics.Process.Start("https://github.com/FransBouma/InjectableGenericCameraSystem");
		}
	}
}
