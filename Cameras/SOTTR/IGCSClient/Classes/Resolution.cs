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
 
namespace IGCSClient.Classes
{
	/// <summary>
	/// Simple struct to use for a resolution.
	/// </summary>
	public struct Resolution
	{
		public Resolution(AspectRatio ar, int horizontalResolution, int verticalResolution, string description)
		{
			this.AR = ar;
			this.HorizontalResolution = horizontalResolution;
			this.VerticalResolution = verticalResolution;
			this.Description = description;
		}

		// these are generated
		public override bool Equals(object obj)
		{
			return obj is Resolution resolution &&
				   this.AR.Equals(resolution.AR) &&
				   this.HorizontalResolution == resolution.HorizontalResolution &&
				   this.VerticalResolution == resolution.VerticalResolution;
		}

		// these are generated
		public override int GetHashCode()
		{
			var hashCode = 1445329852;
			hashCode = hashCode * -1521134295 + this.AR.GetHashCode();
			hashCode = hashCode * -1521134295 + this.HorizontalResolution.GetHashCode();
			hashCode = hashCode * -1521134295 + this.VerticalResolution.GetHashCode();
			return hashCode;
		}

		public AspectRatio AR { get;  }
		public int HorizontalResolution { get; }
		public int VerticalResolution { get; }
		public string Description { get; }
	}
}