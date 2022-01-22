// AOBGen. (c) Frans Bouma
// License: MIT
// https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Tools/AOBGen
//////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AOBGen
{
	internal static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			Application.Run(new _mainForm());
		}
	}
}
