#include "stdafx.h"

// Sets a jmp qword ptr [address] statement at hostImageAddress + startOffset.
void SetHook(__int64 hostImageAddress, __int64 startOffset, __int64 continueOffset, __int64* interceptionContinue, void* asmFunction)
{
	byte* startOfHookAddress = reinterpret_cast<byte*>(hostImageAddress + startOffset);
	*interceptionContinue = hostImageAddress + (__int64)continueOffset;
	// now write bytes of jmp qword ptr [address], which is jmp qword ptr 0 offset.
	memcpy(startOfHookAddress, jmpFarInstructionBytes, sizeof(jmpFarInstructionBytes));
	// now write the address. Do this with a recast of the pointer to an __int64 pointer to avoid endianmess.
	__int64* interceptorAddressDestination = (__int64*)(startOfHookAddress + 6);
	interceptorAddressDestination[0] = (__int64)asmFunction;
}

// Writes NOP opcodes to a range of memory.
void NopRange(__int64 startAddress, int length)
{
	byte* pWriter = reinterpret_cast<byte*>(startAddress);
	for (int i = 0; i < length; i++)
	{
		pWriter[i] = 0x90;
	}
}
