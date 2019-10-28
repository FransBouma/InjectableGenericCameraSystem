using System;

namespace IGCSInjectorUI
{
	internal class DllCacheData
	{
		internal DllCacheData(string dllName, DateTime lastUsedDate)
		{
			this.DllName = dllName;
			this.LastUsedDate = lastUsedDate;
		}

		internal string DllName {get;set;}
		internal DateTime LastUsedDate {get;set;}
	}
}