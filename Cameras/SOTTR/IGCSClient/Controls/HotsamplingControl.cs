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

namespace IGCSClient.Controls
{
	public partial class HotsamplingControl : UserControl
	{
		public HotsamplingControl()
		{
			InitializeComponent();

			// Use: Screen screen = Screen.FromHandle(hWnd of main window);
			// to obtain the screen the game window is located on. 
			// then calculate the rest from that object.
		}
	}


	/// <summary>
	/// Simple struct to use for a resolution.
	/// </summary>
	public struct Resolution
	{
		public decimal AspectRatio { get; set; }
		public int HorizontalResolution { get; set; }
		public int VerticalResolution { get; set; }


		// these are generated
		public override bool Equals(object obj)
		{
			return obj is Resolution resolution &&
				   AspectRatio == resolution.AspectRatio &&
				   HorizontalResolution == resolution.HorizontalResolution &&
				   VerticalResolution == resolution.VerticalResolution;
		}

		// these are generated
		public override int GetHashCode()
		{
			var hashCode = 1445329852;
			hashCode = hashCode * -1521134295 + AspectRatio.GetHashCode();
			hashCode = hashCode * -1521134295 + HorizontalResolution.GetHashCode();
			hashCode = hashCode * -1521134295 + VerticalResolution.GetHashCode();
			return hashCode;
		}
	}
}
