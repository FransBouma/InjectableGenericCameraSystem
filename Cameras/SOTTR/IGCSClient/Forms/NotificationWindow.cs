using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IGCSClient.Forms
{
	public partial class NotificationWindow : Form
	{
		public NotificationWindow()
		{
			InitializeComponent();
			this.Opacity = 0.7;
		}


		public void AddNotification(string toAdd)
		{
			_notificationLabel.Text += toAdd + Environment.NewLine;
		}
	}
}
