////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2020, Frans Bouma
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
using System.Linq;
using System.Windows;
using System.Windows.Forms;
using IGCSClient.Classes;
using IGCSClient.GameSpecific.Classes;
using ModernWpf.Controls;
using DataObject = System.Windows.DataObject;
using UserControl = System.Windows.Controls.UserControl;

namespace IGCSClient.Controls
{
	/// <summary>
	/// Interaction logic for HotsamplingPage.xaml
	/// </summary>
	public partial class HotsamplingPage : UserControl
	{
		#region Members
		private WINDOWINFO _gameWindowInfo;
		private IntPtr _gameWindowHwnd;
		private Timer _resolutionRefreshTimer;
		#endregion


		public HotsamplingPage()
		{
			InitializeComponent();
			_resolutionRefreshTimer = new Timer() { Interval = 1000};
			_resolutionRefreshTimer.Tick += _resolutionRefreshTimer_Tick;
			DataObject.AddCopyingHandler(_newHeightBox, (s, e) => { if (e.IsDragDrop) e.CancelCommand(); });
			DataObject.AddCopyingHandler(_newWidthBox, (s, e) => { if (e.IsDragDrop) e.CancelCommand(); });
		}


		public void BindData()
		{
			GetActiveGameWindowInfo();
			// game window hwnd is set in getactivegamewindowinfo. 
			// Use Windows Forms. This app is opt-in on high dpi awareness, so the values should be correct. This high-dpi awareness will fuck up winforms controls but we're
			// not using these so it's ok. It's otherwise a PITA to get the monitor resolution of the monitor the window is on!
			var screenWithGameWindow = Screen.FromHandle(_gameWindowHwnd);
			BuildResolutionTree(screenWithGameWindow.Bounds);
			_resolutionRefreshTimer.Enabled = true;
		}
		

		private void BuildResolutionTree(System.Drawing.Rectangle screenBounds)
		{
			var arOfScreen = CalculateAspectRatio(screenBounds.Width, screenBounds.Height);
			arOfScreen.Description = "Monitor aspect ratio";

			// default ARs
			var defaultARs = new List<AspectRatio>()
							 {
								 new AspectRatio(16, 9), new AspectRatio(16, 10), new AspectRatio(21, 9), new AspectRatio(1, 1),
								 new AspectRatio(9, 16), new AspectRatio(2, 3), new AspectRatio(3, 2), new AspectRatio(2, 1), new AspectRatio(3, 4), new AspectRatio(4, 5)
							 };
			// remove the one we already have determined from the monitor (if present)
			defaultARs.Remove(arOfScreen);
			var resolutions = new List<Resolution>();
			// first the resolutions based on the screen resolution
			AddResolutionsForAspectRatio(resolutions, screenBounds.Width, screenBounds.Height, arOfScreen);
			// then the resolutions calculated from the aspect ratios in the list. 
			foreach(var ar in defaultARs)
			{
				if(ar.Horizontal <= ar.Vertical)
				{
					// use height instead of width so the full shot can be setup on the lowest res
					AddResolutionsForAspectRatio(resolutions, (int)(screenBounds.Height * ar.ARFactorWidth), screenBounds.Height, ar);
				}
				else
				{
					AddResolutionsForAspectRatio(resolutions, screenBounds.Width, (int)(screenBounds.Width * ar.ARFactorHeight), ar);
				}
			}

			_resolutionTreeView.ItemsSource = resolutions.GroupBy(r => r.AR.ToString());
		}


		private void AddResolutionsForAspectRatio(List<Resolution> resolutions, int initialWidth, int initialHeight, AspectRatio ar)
		{
			for(decimal i = 1; i < 6; i++)
			{
				resolutions.Add(GetResolutionForAR(initialWidth, initialHeight, ar, i));
				resolutions.Add(GetResolutionForAR(initialWidth, initialHeight, ar, i + 0.25m));
				resolutions.Add(GetResolutionForAR(initialWidth, initialHeight, ar, i + 0.5m));
				resolutions.Add(GetResolutionForAR(initialWidth, initialHeight, ar, i + 0.75m));
			}
		}


		private Resolution GetResolutionForAR(int width, int height, AspectRatio ar, decimal factor)
		{
			int newWidth = (int)(width * factor);
			int newHeight = (int)(height * factor);
			return new Resolution(ar, newWidth, newHeight, string.Format("{0} x {1} ({2:##.##}x)", newWidth, newHeight, factor));
		}


		private AspectRatio CalculateAspectRatio(int width, int height)
		{
			if(width <= 0 || height <= 0)
			{
				return new AspectRatio(0, 0);
			}

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

				

		private void PerformHotSampling()
		{
			if(_newWidthBox.Value < 100 || _newHeightBox.Value < 100)
			{
				return;
			}
			if((Win32Wrapper.IsIconic(_gameWindowHwnd) || Win32Wrapper.IsZoomed(_gameWindowHwnd)))
			{
				Win32Wrapper.ShowWindow(_gameWindowHwnd, Win32Wrapper.SW_SHOWNOACTIVATE);
			}

			int newHorizontalResolution = (int)_newWidthBox.Value;
			int newVerticalResolution = (int)_newHeightBox.Value;
			// always set the window at (0,0), if width is >= screen width.
			var screenBounds = Screen.FromHandle(_gameWindowHwnd).Bounds;
			uint uFlags = Win32Wrapper.SWP_NOZORDER | Win32Wrapper.SWP_NOACTIVATE | Win32Wrapper.SWP_NOOWNERZORDER | Win32Wrapper.SWP_NOSENDCHANGING;
			if(newHorizontalResolution < screenBounds.Width)
			{
				// let the window be where it is.
				uFlags |= Win32Wrapper.SWP_NOMOVE;
			}

			Win32Wrapper.SetWindowPos(_gameWindowHwnd, 0, 0, 0, newHorizontalResolution, newVerticalResolution, uFlags);
			if(GameSpecificConstants.HotsamplingRequiresEXITSIZEMOVE)
			{
				// A warning of unreachable code will be raised here, that's ok. this code is only used when the constant is true
				Win32Wrapper.SendMessage(_gameWindowHwnd, Win32Wrapper.WM_EXITSIZEMOVE, 0, 0);
			}

			// remove / add borders
			uint nStyle = (uint)Win32Wrapper.GetWindowLong(_gameWindowHwnd, Win32Wrapper.GWL_STYLE);
			nStyle = (nStyle | (Win32Wrapper.WS_THICKFRAME + Win32Wrapper.WS_DLGFRAME + Win32Wrapper.WS_BORDER + Win32Wrapper.WS_MAXIMIZEBOX + Win32Wrapper.WS_MINIMIZEBOX));
			if(_useWindowBordersCheckBox.IsChecked==false)
			{
				nStyle^=(Win32Wrapper.WS_THICKFRAME + Win32Wrapper.WS_DLGFRAME + Win32Wrapper.WS_BORDER + Win32Wrapper.WS_MAXIMIZEBOX + Win32Wrapper.WS_MINIMIZEBOX);
			}
			Win32Wrapper.SetWindowLong(_gameWindowHwnd, Win32Wrapper.GWL_STYLE, nStyle);
			uFlags = Win32Wrapper.SWP_NOSIZE | Win32Wrapper.SWP_NOMOVE | Win32Wrapper.SWP_NOZORDER | Win32Wrapper.SWP_NOACTIVATE | Win32Wrapper.SWP_NOOWNERZORDER | Win32Wrapper.SWP_NOSENDCHANGING | Win32Wrapper.SWP_FRAMECHANGED;
			Win32Wrapper.SetWindowPos(_gameWindowHwnd, 0, 0, 0, 0, 0, uFlags);

			if(_switchAfterResizingCheckBox.IsChecked==true)
			{
				// focus the attached application's window
				Win32Wrapper.SetForegroundWindow(_gameWindowHwnd);
			}
		}
		

		private void GetActiveGameWindowInfo()
		{
			_gameWindowHwnd = AppStateSingleton.Instance().GetMainWindowHandleOfAttachedProcess();
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
			_currentARTextBox.Text = CalculateAspectRatio(_gameWindowInfo.rcWindow.Width, _gameWindowInfo.rcWindow.Height).ToString(appendDescription:false);
			if(_newHeightBox.Value <= 0 || _newWidthBox.Value <= 0)
			{
				//reset with current window
				_newHeightBox.Value = _gameWindowInfo.rcWindow.Height;
				_newWidthBox.Value = _gameWindowInfo.rcWindow.Width;
				_aspectRatioTextBox.Text = _currentARTextBox.Text;
			}
		}
		

		private void HandleResolutionValueChanged()
		{
			int horizontalResolution = (int)_newWidthBox.Value;
			int verticalResolution = (int)_newHeightBox.Value;
			_setResolutionButton.IsEnabled = (horizontalResolution >= 640 && verticalResolution >= 480);
			_aspectRatioTextBox.Text = CalculateAspectRatio(horizontalResolution, verticalResolution).ToString(appendDescription:false);
		}


		private void _resolutionTreeView_OnSelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
		{
			if(!(e.NewValue is Resolution))
			{
				// set to disabled. 
				return;
			}
			var selectedResolution = (Resolution)e.NewValue;
			_newHeightBox.Value = selectedResolution.VerticalResolution;
			_newWidthBox.Value = selectedResolution.HorizontalResolution;
			_aspectRatioTextBox.Text = selectedResolution.AR.ToString(appendDescription:false);
		}


		private void _setResolutionButton_OnClick(object sender, RoutedEventArgs e)
		{
			PerformHotSampling();
		}


		private void _newWidthBox_OnValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
		{
			_newWidthBox.Value = GeneralUtils.ClampNaN(args.OldValue, args.NewValue);
			HandleResolutionValueChanged();
		}


		private void _newHeightBox_OnValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
		{
			_newHeightBox.Value = GeneralUtils.ClampNaN(args.OldValue, args.NewValue);
			HandleResolutionValueChanged();
		}

		
		private void _resolutionRefreshTimer_Tick(object sender, EventArgs e)
		{
			GetActiveGameWindowInfo();
		}


		private void _switchToGameWindowButton_OnClick(object sender, RoutedEventArgs e)
		{
			// focus the attached application's window
			Win32Wrapper.SetForegroundWindow(_gameWindowHwnd);
		}


		private void _refreshButton_OnClick(object sender, RoutedEventArgs e)
		{
			GetActiveGameWindowInfo();
			var screenWithGameWindow = Screen.FromHandle(_gameWindowHwnd);
			BuildResolutionTree(screenWithGameWindow.Bounds);
		}
	}
}
