#pragma once

#include "stdafx.h"

#pragma pack(push, 1)
struct InterceptAddressPair
{
	// The address to start the overwrite operation. This is the address which will receive the jmp qword ptr [address] instruction
	__int64 Start;
	// The address to jump back to after interception. 
	__int64 Continue;
};
#pragma pack(pop)

