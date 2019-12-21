using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using IGCSClient.NamedPipeSubSystem;
using SD.Tools.Algorithmia.GeneralDataStructures.EventArguments;

namespace IGCSClient
{
	public partial class MainForm : Form
	{
		private NamedPipeServer _pipeServer;

		public MainForm()
		{
			InitializeComponent();
		}


		protected override void OnLoad(EventArgs e)
		{
			base.OnLoad(e);
			_pipeServer = new NamedPipeServer(ConstantsEnums.NamedPipeName);
			_pipeServer.MessageReceived += _pipeServer_MessageReceived;
			LogLine("Named pipe enabled.");
		}


		private void LogLine(string line, params object[] args)
		{
			_logTextBox.Text += string.Format(line, args);
			_logTextBox.Text += Environment.NewLine;
		}


		private void HandleMessageReceived(ContainerEventArgs<byte[]> e)
		{
			byte[] value = e.Value;

			LogLine("Message received. Length of value: {0}", value.Length);

			// for now, assume a string
			string valueAsString = new ASCIIEncoding().GetString(value);
			LogLine("Value: {0}", valueAsString);
		}


		private void _pipeServer_MessageReceived(object sender, ContainerEventArgs<byte[]> e)
		{
			HandleMessageReceived(e);
		}


		private void _clearLogButton_Click(object sender, EventArgs e)
		{
			_logTextBox.Clear();
		}
	}
}
