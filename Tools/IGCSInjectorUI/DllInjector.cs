using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace IGCSInjectorUI
{
    internal class DllInjector
    {
		/// <summary>
		/// Injects the dll with the path specified into the process with the id specified. 
		/// </summary>
		/// <param name="fullPathOfDllToInject">the full path + filename of the dll to inject</param>
		/// <param name="processId">the PID of the process to inject the dll into.</param>
		/// <returns>true if succeeded, false otherwise. If false is returned, <see cref="LastError"/> is set with the error code.</returns>
		public bool PerformInjection(string fullPathOfDllToInject, int processId)
		{
			this.LastError = 0;

			uint dllLengthToPassInBytes = (uint)((fullPathOfDllToInject.Length + 1) * Marshal.SizeOf(typeof(char)));

			this.LastActionPerformed = "Opening the host process";
			IntPtr processHandle = Win32Wrapper.OpenProcess(ProcessAccessFlags.CreateThread | ProcessAccessFlags.QueryInformation | ProcessAccessFlags.VirtualMemoryOperation |
															ProcessAccessFlags.VirtualMemoryWrite | ProcessAccessFlags.VirtualMemoryRead, 
															false, processId);
			if(processHandle==IntPtr.Zero)
			{
				// failed, so set the error code and return
				this.LastError = Marshal.GetLastWin32Error();
				return false;
			}

			this.LastActionPerformed = "Obtaining the address of LoadLibraryA";
			IntPtr loadLibraryAddress = Win32Wrapper.GetProcAddress(Win32Wrapper.GetModuleHandle("kernel32.dll"), "LoadLibraryA");
			if(loadLibraryAddress==IntPtr.Zero)
			{
				this.LastError = Marshal.GetLastWin32Error();
				return false;
			}

			this.LastActionPerformed = "Allocating memory in the host process for the dll filename";
			IntPtr memoryInTargetProcess = Win32Wrapper.VirtualAllocEx(processHandle, IntPtr.Zero, dllLengthToPassInBytes, AllocationType.Commit | AllocationType.Reserve, 
																	   MemoryProtection.ReadWrite);
			if(memoryInTargetProcess==IntPtr.Zero)
			{
				this.LastError = Marshal.GetLastWin32Error();
				return false;
			}

			this.LastActionPerformed = "Writing dll filename into memory allocated in host process";
			IntPtr bytesWritten;
			bool result = Win32Wrapper.WriteProcessMemory(processHandle, memoryInTargetProcess, Encoding.Default.GetBytes(fullPathOfDllToInject), dllLengthToPassInBytes, out bytesWritten);
			if(!result)
			{
				this.LastError = Marshal.GetLastWin32Error();
				return false;
			}

			this.LastActionPerformed = "Creating a thread in the host process to load the dll";
			IntPtr remoteThreadHandle = Win32Wrapper.CreateRemoteThread(processHandle, IntPtr.Zero, 0, loadLibraryAddress, memoryInTargetProcess, 0, IntPtr.Zero);
			if(remoteThreadHandle==IntPtr.Zero)
			{
				this.LastError = Marshal.GetLastWin32Error();
				return false;
			}

			// cleaning up. No need for error checking. If this fails, it's not really a problem.
			this.LastActionPerformed = "Cleaning up...";
			Win32Wrapper.VirtualFreeEx(processHandle, memoryInTargetProcess, dllLengthToPassInBytes, FreeType.Release);

			this.LastActionPerformed = "Done";
			return true;
		}

		/// <summary>
		/// If PerformInjection returns false, this property contains the last error code returned by GetLastError()
		/// </summary>
		public int LastError {get;set;}
		/// <summary>
		/// The last action performed by the injector. Needed for error reporting.
		/// </summary>
		public string LastActionPerformed {get;set;}
    }
}
