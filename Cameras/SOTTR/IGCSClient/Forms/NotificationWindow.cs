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
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using SD.Tools.Algorithmia.GeneralDataStructures;

namespace IGCSClient.Forms
{
	public partial class NotificationWindow : Form
	{
		#region Members
		private List<Pair<string, DateTime>> _notifications;
		private object _notificationsSemaphore;
		#endregion


		public NotificationWindow()
		{
			InitializeComponent();
			this.Opacity = ConstantsEnums.NotificationWindowOpacity;
			_notifications = new List<Pair<string, DateTime>>();
			_notificationsSemaphore = new object();
			DisplayNotifications();
			_notificationPurgeTimer.Start();
		}


		public void AddNotification(string toAdd)
		{
			lock(_notificationsSemaphore)
			{
				_notifications.Add(new Pair<string, DateTime>(toAdd, DateTime.Now));
			}
		}


		private void DisplayNotifications()
		{
			string displayText = string.Empty;
			lock(_notificationsSemaphore)
			{
				if(_notifications.Count > 0)
				{
					displayText = string.Join(Environment.NewLine, _notifications.Select(n => n.Value1).ToArray());
				}
			}
			this.Opacity = string.IsNullOrWhiteSpace(displayText) ? 0.0 : ConstantsEnums.NotificationWindowOpacity;
			_notificationLabel.Text = displayText;
		}


		private void PurgeOutdatedNotifications()
		{
			lock(_notificationsSemaphore)
			{
				var minimalTime = DateTime.Now.AddSeconds(-ConstantsEnums.NotificationDisplayTimeInSeconds);
				var toKeep = _notifications.Where(n => n.Value2 >= minimalTime).ToList();
				_notifications.Clear();
				_notifications.AddRange(toKeep);
			}
			DisplayNotifications();
		}


		private void _notificationPurgeTimer_Tick(object sender, EventArgs e)
		{
			PurgeOutdatedNotifications();
		}
	}
}
