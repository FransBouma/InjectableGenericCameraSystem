// AOBGen. (c) Frans Bouma
// License: MIT
// https://github.com/FransBouma/InjectableGenericCameraSystem/tree/master/Tools/AOBGen
//////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AOBGen
{
	internal class AOBGenerator
	{
		public string GenerateAOB(string toParse, bool alsoWildcardOffsets)
		{
			// we can have two formats: Cheat Engine and x64dbg. 
			if(string.IsNullOrWhiteSpace(toParse))
			{
				return string.Empty;
			}
			
			string[] lines = toParse.Split(new string[] { "\r\n", "\r", "\n" }, StringSplitOptions.None);
			foreach(var l in lines)
			{
				if(string.IsNullOrWhiteSpace(l))
				{
					continue;
				}

				if(l.Contains('|'))
				{
					// x64dbg
					return HandleX64Dbg(lines, alsoWildcardOffsets);
				}

				if(l.Count(c => c == '-') > 1)
				{
					// cheat engine
					return HandleCheatEngine(lines, alsoWildcardOffsets);
				}
			}

			return "<Unknown format>";
		}
		

		private void HandleByteFragments(StringBuilder sb, string secondFragment, string thirdFragment, bool alsoWildcardOffsets)
		{
			var byteFragments = secondFragment.Split(' ');

			bool offsetWildcarded = false;
			bool avxOpcode = false;
			for(var i = 0; i < byteFragments.Length; i++)
			{
				var byteFragment = byteFragments[i];
				switch(byteFragment.Length)
				{
					case 2:
						if((alsoWildcardOffsets && i == byteFragments.Length - 1 && thirdFragment.Contains("+" + byteFragment) && !offsetWildcarded) || (avxOpcode && i == 1 && i!=byteFragment.Length-1))
						{
							sb.Append("?? ");
						}
						else
						{
							AOBGenerator.AppendByteFragment(sb, byteFragment);
						}
						break;
					case 4:
						AOBGenerator.AppendByteFragment(sb, byteFragment);
						break;
					case 6:
						if(avxOpcode)
						{
							// assume part of RIP
							sb.Append("?? ?? ?? ");
						}
						else
						{
							AOBGenerator.AppendByteFragment(sb, byteFragment);
						}
						break;
					case 8:
						// offset or RIP relative block
						// We convert the byteFragment to a hexadecimal number. If that number is present in third fragment, it's not a RIP relative address but an offset.
						if(i > 0 && CheckIfShouldBeWildcarded(byteFragment.ToLowerInvariant(), thirdFragment, alsoWildcardOffsets, out bool performedOffsetWildcard))
						{
							offsetWildcarded |= performedOffsetWildcard;
							sb.Append("?? ?? ?? ?? ");
						}
						else
						{
							if(i == 0)
							{
								avxOpcode = true;
							}
							AOBGenerator.AppendByteFragment(sb, byteFragment);
						}
						break;
					case 10:
						AOBGenerator.AppendByteFragment(sb, byteFragment);
						break;
					default:
						// unknown
						continue;
				}
			}
		}


		private static void AppendByteFragment(StringBuilder sb, string byteFragment)
		{
			for(int i = 0; i < byteFragment.Length; i += 2)
			{
				sb.Append(byteFragment[i]);
				sb.Append(byteFragment[i + 1]);
				sb.Append(' ');
			}
		}


		private string HandleCheatEngine(string[] lines, bool alsoWildcardOffsets)
		{
			var sb = new StringBuilder();
			foreach(var l in lines)
			{
				if(string.IsNullOrWhiteSpace(l))
				{
					continue;
				}

				var indexFirstHyphen = l.IndexOf(" - ", StringComparison.InvariantCulture);
				if(indexFirstHyphen < 0)
				{
					continue;
				}
				var indexOfSecondHyphen = l.IndexOf(" - ", indexFirstHyphen + 1, StringComparison.InvariantCulture);
				if(indexOfSecondHyphen < 0)
				{
					continue;
				}
				string secondFragment = l.Substring(indexFirstHyphen + 3, (indexOfSecondHyphen - indexFirstHyphen) - 2).Trim();
				string thirdFragment = l.Substring(indexOfSecondHyphen + 3).Trim().ToLowerInvariant();

				HandleByteFragments(sb, secondFragment, thirdFragment, alsoWildcardOffsets);
			}

			return sb.ToString();
		}


		private string HandleX64Dbg(string[] lines, bool alsoWildcardOffsets)
		{
			var sb = new StringBuilder();
			foreach(var l in lines)
			{
				if(string.IsNullOrWhiteSpace(l))
				{
					continue;
				}

				var fragments = l.Split('|');
				if(fragments.Length < 3)
				{
					// invalid line
					continue;
				}
				// replace ':' with a space in the bytefragment
				fragments[1] = fragments[1].Replace(':', ' ');
				HandleByteFragments(sb, fragments[1], fragments[2], alsoWildcardOffsets);
			}

			return sb.ToString();
		}


		/// <summary>
		/// We wildcard rip relative numbers and optionally offsets
		/// </summary>
		/// <param name="byteFragment"></param>
		/// <param name="thirdFragment"></param>
		/// <param name="alsoWildcardOffsets"></param>
		/// <param name="performedOffsetWildcard"></param>
		/// <returns></returns>
		private bool CheckIfShouldBeWildcarded(string byteFragment, string thirdFragment, bool alsoWildcardOffsets, out bool performedOffsetWildcard)
		{
			performedOffsetWildcard = false;
			// convert the byte fragment to a hexadecimal number. then check if that number is present in thirdfragment.
			// aabbccdd becomes ddccbbaa
			var numberAsLittleEndian = new string(new char[] { byteFragment[6], byteFragment[7], byteFragment[4], byteFragment[5], byteFragment[2], byteFragment[3], byteFragment[0], byteFragment[1] });
			// strip off all 0's at the front, so convert it to an int and then back to a hex string
			uint bytes = uint.Parse(numberAsLittleEndian, NumberStyles.HexNumber);
			string bytesAsHex = bytes.ToString("X");
			var fragmentIsRIPRelativeAddress = !thirdFragment.Contains(bytesAsHex) && !thirdFragment.Contains(numberAsLittleEndian);
			if(alsoWildcardOffsets)
			{
				performedOffsetWildcard = true;
				return fragmentIsRIPRelativeAddress || thirdFragment.Contains(bytesAsHex) || thirdFragment.Contains(numberAsLittleEndian);
			}
			return fragmentIsRIPRelativeAddress;
		}

	}
}
