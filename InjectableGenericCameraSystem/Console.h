#pragma once
#include "stdafx.h"

using namespace std;

#define CONSOLE_WHITE	15
#define CONSOLE_NORMAL  7

class Console
{
public:
	Console();
	~Console();

	void Init();
	void Release();
	void WriteHeader();
	void WriteLine(const string& toWrite);
	void WriteLine(const string& toWrite, int color);
	void WriteError(const string& error);
	void SetColor(int color);

private:
};

