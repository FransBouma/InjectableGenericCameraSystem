#pragma once
#include "stdafx.h"
#include "OverlayConsole.h"

// Thanks to @HattiWatt1 for this.
class EngineClient
{
public:
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void Function7();
	virtual void Function8();
	virtual void Function9();
	virtual void Function10();
	virtual void Function11();
	virtual void Function12();
	virtual void Function13();
	virtual void Function14();
	virtual void Function15();
	virtual void Function16();
	virtual void Function17();
	virtual void Function18();
	virtual void Function19();
	virtual void Function20();
	virtual void Function21();
	virtual void Function22();
	virtual void Function23();
	virtual void Function24();
	virtual void Function25();
	virtual void ClientCmd_Unrestricted(const char*);
	virtual void ServerCmd(const char*);
	virtual void ClientCmd(const char*);
	virtual void Function29();
	virtual void Function30();
	virtual void Function31();
	virtual void Function32();
	virtual void Function33();
	virtual void Function34();
	virtual void Function35();
	virtual void Function36();
	virtual void Function37();
	virtual void Function38();
	virtual void Function39();
	virtual void Function40();
	virtual void Function41();
	virtual void Function42();
	virtual void Function43();
	virtual void Function44();
	virtual void Function45();
	virtual void Function46();
	virtual void Function47();
	virtual void Function48();
	virtual void Function49();
	virtual void Function50();
	virtual void Function51();
	virtual void Function52();

public:
	static EngineClient* GetInstance(LPBYTE engineClientLocation) 
	{ 
		//00007FFED44B5CEF | 48 8B 0D 4A 7C 88 00             | mov rcx,qword ptr ds:[7FFED4D3D940]                   | Arg1
		//00007FFED44B5CF6 | 48 8D 15 AB 43 58 00             | lea rdx,qword ptr ds:[7FFED4A3A0A8]                   | Arg2 = "setpause nomsg"
		//00007FFED44B5CFD | 48 8B 01                         | mov rax,qword ptr ds:[rcx]                            |
		//00007FFED44B5D00 | FF 90 D8 00 00 00                | call qword ptr ds:[rax+D8]                            | Call RunFunction. 
		// engineClientLocation is the '7FFED4D3D940' address given in the above assembler. So we first read the value stored at that address which is the 
		// EngineClient object address. We then cast that to the EngineClient interface we defined ourselves. The C++ compiler then properly makes sure it 
		// looks up the VTable pointer and calls the right offsets. 
		return (EngineClient*)*((__int64*)engineClientLocation);
	}
};