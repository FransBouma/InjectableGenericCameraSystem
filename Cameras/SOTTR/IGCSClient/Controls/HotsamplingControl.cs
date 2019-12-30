////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2019, Frans Bouma
// All rights reserved.
// https://github.com/FransBouma/InjectableGenericCameraSystem
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
//  * Redistributions of source code must retain the above copyright notice, this
//	  list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using IGCSClient.Classes;
using IGCSClient.GameSpecific.Classes;

namespace IGCSClient.Controls
{
	public partial class HotsamplingControl : UserControl
	{
		#region Members
		private WINDOWINFO _gameWindowInfo;
		private IntPtr _gameWindowHwnd;

		private enum IconIndex
		{
			Monitor = 0,
			FolderNormal = 1,
			FolderOpen = 2
		}
		#endregion


		public HotsamplingControl()
		{
			InitializeComponent();
			_gameWindowHwnd = IntPtr.Zero;
		}


		public void BindData()
		{
			GetActiveGameWindowInfo();
			var screenWithGameWindow = Screen.FromHandle(_gameWindowHwnd);
			BuildResolutionTree(screenWithGameWindow.Bounds);
			_resolutionRefreshTimer.Enabled = true;
		}


		private void BuildResolutionTree(Rectangle screenBounds)
		{
			_availableResolutionsTreeView.Nodes.Clear();
			var arOfScreen = CalculateAspectRatio(screenBounds.Width, screenBounds.Height);
			// default ARs
			var defaultARs = new List<AspectRatio>()
							 {
								 new AspectRatio(16, 9), new AspectRatio(16, 10), new AspectRatio(21, 9), new AspectRatio(1, 1),
								 new AspectRatio(9, 16), new AspectRatio(2, 3), new AspectRatio(3, 2), new AspectRatio(2, 1), new AspectRatio(3, 4), new AspectRatio(4, 5)
							 };
			// remove the one we already have determined from the monitor (if present)
			defaultARs.Remove(arOfScreen);
			var rootNode = _availableResolutionsTreeView.Nodes.Add(Guid.NewGuid().ToString(), "Available resolutions");
			rootNode.ImageIndex = (int)IconIndex.FolderNormal;
			rootNode.SelectedImageIndex = (int)IconIndex.FolderNormal;
			// first the resolutions based on the screen resolution
			var resolutionNode = rootNode.Nodes.Add(Guid.NewGuid().ToString(), string.Format("{0} (Monitor aspect ratio)", arOfScreen));
			resolutionNode.ImageIndex = (int)IconIndex.FolderNormal;
			resolutionNode.SelectedImageIndex = (int)IconIndex.FolderNormal;
			AddResolutionNodes(resolutionNode, screenBounds.Width, screenBounds.Height, arOfScreen);
			resolutionNode.EnsureVisible();
			resolutionNode.Expand();
			// then the resolutions calculated from the aspect ratios in the list. 
			foreach(var ar in defaultARs)
			{
				resolutionNode = rootNode.Nodes.Add(Guid.NewGuid().ToString(), ar.ToString());
				resolutionNode.ImageIndex = (int)IconIndex.FolderNormal;
				resolutionNode.SelectedImageIndex = (int)IconIndex.FolderNormal;
				if(ar.Horizontal <= ar.Vertical)
				{
					// use height instead of width so the full shot can be setup on the lowest res
					AddResolutionNodes(resolutionNode, (int)(screenBounds.Height * ar.ARFactorWidth), screenBounds.Height, ar);
				}
				else
				{
					AddResolutionNodes(resolutionNode, screenBounds.Width, (int)(screenBounds.Width * ar.ARFactorHeight), ar);
				}
			}
		}


		private void AddResolutionNodes(TreeNode parentNode, int initialWidth, int initialHeight, AspectRatio ar)
		{
			for(decimal i = 1; i < 6; i++)
			{
				AddResolutionNode(parentNode, initialWidth, initialHeight, ar, i); 
				AddResolutionNode(parentNode, initialWidth, initialHeight, ar, i+0.25m);
				AddResolutionNode(parentNode, initialWidth, initialHeight, ar, i+0.5m);
				AddResolutionNode(parentNode, initialWidth, initialHeight, ar, i+0.75m);
			}
		}


		private void AddResolutionNode(TreeNode parentNode, int width, int height, AspectRatio ar, decimal factor)
		{
			int newWidth = (int)(width * factor);
			int newHeight = (int)(height * factor);
			var node = parentNode.Nodes.Add(Guid.NewGuid().ToString(), string.Format("{0} x {1} ({2:##.##}x)", newWidth, newHeight, factor));
			node.Tag = new Resolution(ar, newWidth, newHeight);
			node.ImageIndex = (int)IconIndex.Monitor;
			node.SelectedImageIndex = (int)(int)IconIndex.Monitor;
		}


		private AspectRatio CalculateAspectRatio(int width, int height)
		{
			// calculate biggest common divisor using Euclides' algorithm: https://en.wikipedia.org/wiki/Euclidean_algorithm
			// then divide both width and height with that number to get the values for the AR.
			int biggest, smallest;
			if(width > height)
			{
				biggest = width;
				smallest = height;
			}
			else
			{
				biggest = height;
				smallest = width;
			}

			int gcd = -1;
			while(true)
			{
				var rest = biggest % smallest;
				if(rest == 0)
				{
					// done
					gcd = smallest;
					break;
				}
				// not done yet
				biggest = smallest;
				smallest = rest;
			}

			// If not found, pick 16:9
			var toReturn = (gcd < 0) ? new AspectRatio(16, 9) : new AspectRatio(width/gcd, height/gcd);
			// we have one exception: 16:10. This will resolve to 8:5, but it's used as '16:10'. So we'll do a check on that and then bump it up.
			if(toReturn.Equals(new AspectRatio(8, 5)))
			{
				toReturn = new AspectRatio(16, 10);
			}
			// lots of monitors are 21.5:9 instead of 21:9, we'll leave these as-is.
			return toReturn;
		}


		private void GetActiveGameWindowInfo()
		{
			_gameWindowHwnd = AppStateSingleton.Instance().GetMainWindowHandleOfAttachedProcess();
			_currentResolutionGroupBox.Enabled = _gameWindowHwnd != IntPtr.Zero;
			if(_gameWindowHwnd == IntPtr.Zero)
			{
				_currentWidthTextBox.Text = string.Empty;
				_currentHeightTextBox.Text = string.Empty;
				_currentARTextBox.Text = string.Empty;
				return;
			}
			Win32Wrapper.GetWindowInfo(_gameWindowHwnd, ref _gameWindowInfo);
			_currentWidthTextBox.Text = _gameWindowInfo.rcWindow.Width.ToString();
			_currentHeightTextBox.Text = _gameWindowInfo.rcWindow.Height.ToString();
			_currentARTextBox.Text = CalculateAspectRatio(_gameWindowInfo.rcWindow.Width, _gameWindowInfo.rcWindow.Height).ToString();
		}
		

		private void PerformHotSampling()
		{
			var selectedNode = _availableResolutionsTreeView.SelectedNode;
			if(selectedNode == null || selectedNode.Tag == null)
			{
				return;
			}

			var resolution = (Resolution)selectedNode.Tag;
			if((Win32Wrapper.IsIconic(_gameWindowHwnd) || Win32Wrapper.IsZoomed(_gameWindowHwnd)))
			{
				Win32Wrapper.ShowWindow(_gameWindowHwnd, Win32Wrapper.SW_SHOWNOACTIVATE);
			}
			// always set the window at (0,0), if width is >= screen width.
			var screenBounds = Screen.FromHandle(_gameWindowHwnd).Bounds;
			int x = resolution.HorizontalResolution >= screenBounds.Width ? 0 : _gameWindowInfo.rcWindow.left;
			int y = resolution.HorizontalResolution >= screenBounds.Height ? 0 : _gameWindowInfo.rcWindow.top;
			uint uFlags = Win32Wrapper.SWP_NOZORDER | Win32Wrapper.SWP_NOACTIVATE | Win32Wrapper.SWP_NOOWNERZORDER | Win32Wrapper.SWP_NOSENDCHANGING;
			if(resolution.HorizontalResolution < screenBounds.Width)
			{
				// let the window be where it is.
				uFlags |= Win32Wrapper.SWP_NOMOVE;
			}

			Win32Wrapper.SetWindowPos(_gameWindowHwnd, 0, x, y, x+resolution.HorizontalResolution, y+resolution.VerticalResolution, uFlags);
			if(GameSpecificConstants.HotsamplingRequiresEXITSIZEMOVE)
			{
				Win32Wrapper.SendMessage(_gameWindowHwnd, Win32Wrapper.WM_EXITSIZEMOVE, 0, 0);
			}

			//if(_removeBordersCheckBox.Checked)
			{
				// remove the borders
				uint nStyle = (uint)Win32Wrapper.GetWindowLong(_gameWindowHwnd, Win32Wrapper.GWL_STYLE);
				nStyle = (nStyle | (Win32Wrapper.WS_THICKFRAME + Win32Wrapper.WS_DLGFRAME + Win32Wrapper.WS_BORDER)) ^
						 (Win32Wrapper.WS_THICKFRAME + Win32Wrapper.WS_DLGFRAME + Win32Wrapper.WS_BORDER);
				Win32Wrapper.SetWindowLong(_gameWindowHwnd, Win32Wrapper.GWL_STYLE, nStyle);

				nStyle = (uint)Win32Wrapper.GetWindowLong(_gameWindowHwnd, Win32Wrapper.GWL_EXSTYLE);
				nStyle = (nStyle | (Win32Wrapper.WS_EX_DLGMODALFRAME + Win32Wrapper.WS_EX_WINDOWEDGE + Win32Wrapper.WS_EX_CLIENTEDGE + Win32Wrapper.WS_EX_STATICEDGE)) ^
						 (Win32Wrapper.WS_EX_DLGMODALFRAME + Win32Wrapper.WS_EX_WINDOWEDGE + Win32Wrapper.WS_EX_CLIENTEDGE + Win32Wrapper.WS_EX_STATICEDGE);
				Win32Wrapper.SetWindowLong(_gameWindowHwnd, Win32Wrapper.GWL_EXSTYLE, nStyle);

				uFlags = Win32Wrapper.SWP_NOSIZE | Win32Wrapper.SWP_NOMOVE | Win32Wrapper.SWP_NOZORDER | Win32Wrapper.SWP_NOACTIVATE | Win32Wrapper.SWP_NOOWNERZORDER | Win32Wrapper.SWP_NOSENDCHANGING | Win32Wrapper.SWP_FRAMECHANGED;
				Win32Wrapper.SetWindowPos(_gameWindowHwnd, 0, 0, 0, 0, 0, uFlags);
			}
		}
		

		private void _availableResolutionsTreeView_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
		{
			if(e.Node.Tag != null)
			{
				return;
			}

			e.Node.ImageIndex = (int)IconIndex.FolderNormal;
			e.Node.SelectedImageIndex = (int)IconIndex.FolderNormal;
		}


		private void _availableResolutionsTreeView_BeforeExpand(object sender, TreeViewCancelEventArgs e)
		{
			if(e.Node.Tag != null)
			{
				return;
			}

			e.Node.ImageIndex = (int)IconIndex.FolderOpen;
			e.Node.SelectedImageIndex = (int)IconIndex.FolderOpen;
		}


		private void _availableResolutionsTreeView_AfterSelect(object sender, TreeViewEventArgs e)
		{
			if(e.Node.Tag == null)
			{
				_newResolutionGroupBox.Enabled = false;
				return;
			}

			_newResolutionGroupBox.Enabled = true;
			var resolution = (Resolution)e.Node.Tag;
			_newHeightUpDown.Value = resolution.VerticalResolution;
			_newWidthUpDown.Value = resolution.HorizontalResolution;
			_aspectRatioTextBox.Text = resolution.AR.ToString();
		}


		private void _resolutionRefreshTimer_Tick(object sender, EventArgs e)
		{
			GetActiveGameWindowInfo();
		}


		private void _refreshButton_Click(object sender, EventArgs e)
		{
			GetActiveGameWindowInfo();
			var screenWithGameWindow = Screen.FromHandle(_gameWindowHwnd);
			BuildResolutionTree(screenWithGameWindow.Bounds);
		}


		private void _setResolutionButton_Click(object sender, EventArgs e)
		{
			PerformHotSampling();
		}
	}


	public struct AspectRatio
	{
		public AspectRatio(int horizontal, int vertical)
		{
			this.Horizontal = horizontal;
			this.Vertical = vertical;
		}

		/// <summary>
		/// Factor used for calculating the height using a width
		/// </summary>
		public decimal ARFactorHeight
		{
			get { return (decimal)Vertical / (decimal)Horizontal; }
		}
		/// <summary>
		/// Factor used for calculating the width using a height
		/// </summary>
		public decimal ARFactorWidth
		{
			get { return (decimal)Horizontal / (decimal)Vertical; }
		}
		public int Horizontal { get; }
		public int Vertical { get; }

		public override string ToString()
		{
			return string.Format("{0}:{1}", Horizontal, Vertical);
		}


		// these are generated
		public override bool Equals(object obj)
		{
			return obj is AspectRatio ratio &&
				   Horizontal == ratio.Horizontal &&
				   Vertical == ratio.Vertical;
		}

		// these are generated
		public override int GetHashCode()
		{
			var hashCode = 1211898032;
			hashCode = hashCode * -1521134295 + Horizontal.GetHashCode();
			hashCode = hashCode * -1521134295 + Vertical.GetHashCode();
			return hashCode;
		}
	}


	/// <summary>
	/// Simple struct to use for a resolution.
	/// </summary>
	public struct Resolution
	{
		public Resolution(AspectRatio ar, int horizontalResolution, int verticalResolution)
		{
			AR = ar;
			HorizontalResolution = horizontalResolution;
			VerticalResolution = verticalResolution;
		}

		public AspectRatio AR { get;  }
		public int HorizontalResolution { get; }
		public int VerticalResolution { get; }


		// these are generated
		public override bool Equals(object obj)
		{
			return obj is Resolution resolution &&
				   AR.Equals(resolution.AR) &&
				   HorizontalResolution == resolution.HorizontalResolution &&
				   VerticalResolution == resolution.VerticalResolution;
		}

		// these are generated
		public override int GetHashCode()
		{
			var hashCode = 1445329852;
			hashCode = hashCode * -1521134295 + AR.GetHashCode();
			hashCode = hashCode * -1521134295 + HorizontalResolution.GetHashCode();
			hashCode = hashCode * -1521134295 + VerticalResolution.GetHashCode();
			return hashCode;
		}
	}
}
