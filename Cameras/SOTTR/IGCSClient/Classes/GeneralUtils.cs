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
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Diagnostics;
using System.Threading.Tasks;

namespace IGCSClient.Classes
{
	public static class GeneralUtils
	{
		/// <summary>
		/// Converts the passed in payload value to a byte array which is ready to be send over the pipe
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="payload"></param>
		/// <returns>byte array to be send over the pipe which represents the payload.</returns>
		internal static byte[] ConvertToByteArray<T>(T payload)
		{
			if(!(payload is object))
			{
				return null;
			}

			PayloadType typeOfPayload = PayloadType.Undefined;
			byte[] payloadInBytes;
			switch(Type.GetTypeCode(typeof(T)))
			{
				case TypeCode.Boolean:
					payloadInBytes = BitConverter.GetBytes((bool)Convert.ChangeType(payload, typeof(bool)));
					typeOfPayload = PayloadType.Bool;
					break;
				case TypeCode.Byte:
					payloadInBytes = new byte[1] { (byte)Convert.ChangeType(payload, typeof(byte)) };
					typeOfPayload = PayloadType.Byte;
					break;
				case TypeCode.Int16:
					payloadInBytes = BitConverter.GetBytes((short)Convert.ChangeType(payload, typeof(short)));
					typeOfPayload = PayloadType.Int16;
					break;
				case TypeCode.Int32:
					payloadInBytes = BitConverter.GetBytes((int)Convert.ChangeType(payload, typeof(int)));
					typeOfPayload = PayloadType.Int32;
					break;
				case TypeCode.Int64:
					payloadInBytes = BitConverter.GetBytes((long)Convert.ChangeType(payload, typeof(long)));
					typeOfPayload = PayloadType.Int64;
					break;
				case TypeCode.Single:
					payloadInBytes = BitConverter.GetBytes((float)Convert.ChangeType(payload, typeof(float)));
					typeOfPayload = PayloadType.Float;
					break;
				case TypeCode.Double:
					payloadInBytes = BitConverter.GetBytes((double)Convert.ChangeType(payload, typeof(double)));
					typeOfPayload = PayloadType.Double;
					break;
				case TypeCode.String:
					// exception for string, as we can't marshal that to unmanaged memory
					payloadInBytes = Encoding.ASCII.GetBytes(payload as string);
					typeOfPayload = PayloadType.AsciiString;
					break;
				case TypeCode.Object:
					// if it's a random object we serialize it to bytes. Not going to use it in IGCS, but just in case. 
					using(var ms = new MemoryStream())
					{
						var bf = new BinaryFormatter();
						bf.Serialize(ms, payload);
						payloadInBytes = ms.ToArray();
					}
					break;
				default:
					// default primitive type. 
					// based on: https://stackoverflow.com/a/19468007
					var size = Marshal.SizeOf(payload);
					// Both managed and unmanaged buffers required.
					payloadInBytes = new byte[size];
					var ptr = Marshal.AllocHGlobal(size);
					// Copy object byte-to-byte to unmanaged memory.
					Marshal.StructureToPtr(payload, ptr, false);
					// Copy data from unmanaged memory to managed buffer.
					Marshal.Copy(ptr, payloadInBytes, 0, size);
					// Release unmanaged memory.
					Marshal.FreeHGlobal(ptr);
					break;
			}

			return payloadInBytes;
		}


		public static List<IntPtr> GetProcessWindowHandles(Process process)
		{
			List<IntPtr> rootWindows = GetChildWindows(IntPtr.Zero);
			List<IntPtr> dsProcRootWindows = new List<IntPtr>();
			foreach (IntPtr hWnd in rootWindows)
			{
				Win32Wrapper.GetWindowThreadProcessId(hWnd, out var lpdwProcessId);
				if(lpdwProcessId == process.Id)
				{
					dsProcRootWindows.Add(hWnd);
				}
			}
			return dsProcRootWindows;
		}


		public static List<IntPtr> GetChildWindows(IntPtr parent)
		{
			List<IntPtr> result = new List<IntPtr>();
			GCHandle listHandle = GCHandle.Alloc(result);
			try
			{
				Win32Wrapper.Win32Callback childProc = new Win32Wrapper.Win32Callback(EnumWindow);
				Win32Wrapper.EnumChildWindows(parent, childProc, GCHandle.ToIntPtr(listHandle));
			}
			finally
			{
				if(listHandle.IsAllocated)
				{
					listHandle.Free();
				}
			}
			return result;
		}


		private static bool EnumWindow(IntPtr handle, IntPtr pointer)
		{
			GCHandle gch = GCHandle.FromIntPtr(pointer);
			List<IntPtr> list = gch.Target as List<IntPtr>;
			if (list == null)
			{
				throw new InvalidCastException("GCHandle Target could not be cast as List<IntPtr>");
			}
			list.Add(handle);
			return true;
		}
	}
}
