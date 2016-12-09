#include "stdafx.h"
#include "Console.h"

using namespace std;

Console::Console()
{
}


Console::~Console()
{
}


void Console::Release()
{
	FreeConsole();
}


void Console::WriteHeader()
{
	SetColor(CONSOLE_WHITE);
	cout << "Injectable camera tools for " << GAME_NAME << endl;
	WriteLine("Powered by Injectable Generic Camera System by Otis_Inf");
	WriteLine("Get your copy at: https://github.com/FransBouma/InjectableGenericCameraSystem");
	cout << "Camera credits: " << CAMERA_CREDITS << endl;
	SetColor(9);
	WriteLine("-----------------------------------------------------------------------------");
	SetColor(CONSOLE_NORMAL);
}

void Console::WriteLine(const string& toWrite, int color)
{
	SetColor(color);
	WriteLine(toWrite);
	SetColor(CONSOLE_NORMAL);
}


void Console::WriteLine(const string& toWrite)
{
	cout << toWrite << endl;
}


void Console::WriteError(const string& error)
{
	cerr << error << endl;
}


void Console::Init()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	// Redirect the CRT standard input, output, and error handles to the console
	FILE *fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	//Clear the error state for each of the C++ standard stream objects. 
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();

	SetColor(15);
	SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), 12);
}


void Console::SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}