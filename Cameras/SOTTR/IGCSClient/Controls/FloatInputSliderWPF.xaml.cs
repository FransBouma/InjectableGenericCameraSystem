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
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using IGCSClient.Interfaces;
using SD.Tools.BCLExtensions.SystemRelated;

namespace IGCSClient.Controls
{
	/// <summary>
	/// Interaction logic for FloatInputWPF.xaml
	/// </summary>
	public partial class FloatInputSliderWPF : UserControl, IInputControl<float>, IFloatSettingControl
	{
		private bool _suppressEvents = false;

		public event EventHandler ValueChanged;

		public FloatInputSliderWPF()
		{
			InitializeComponent();
		}
		

		public void Setup(double minValue, double maxValue, int scale, double increment, double defaultValue)
		{
			// Necessary to suppress events as setting the minimum sets the value too.
			_suppressEvents = true;
			_sliderControl.Maximum = maxValue;
			_sliderControl.Minimum = minValue;
			_sliderControl.AutoToolTipPrecision = scale;
			_sliderControl.SmallChange = increment;
			_sliderControl.LargeChange = increment;
			_suppressEvents = false;
		}


		public void SetValueFromString(string valueAsString, float defaultValue)
		{
			if(!Single.TryParse(valueAsString, NumberStyles.Float, CultureInfo.InvariantCulture.NumberFormat, out var valueToSet))
			{
				valueToSet = defaultValue;
			}
			this.Value = valueToSet;
		}


		private void _sliderControl_OnValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			if(_suppressEvents)
			{
				return;
			}
			this.ValueChanged.RaiseEvent(this);
		}

		

		#region Properties
		/// <inheritdoc/>
		public float Value
		{
			get { return Convert.ToSingle(_sliderControl.Value); }
			set { _sliderControl.Value = Convert.ToDouble(value); }
		}


		public string Header
		{
			get { return _headerControl.Header?.ToString() ?? string.Empty; }
			set { _headerControl.Header = value; }
		}
		#endregion
	}
}
