// NamedPipeTester.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <fileapi.h>
#include <handleapi.h>
#include <namedpipeapi.h>
#include <winbase.h>

using namespace std;

#define PIPENAME_DLL_TO_CLIENT	"\\\\.\\pipe\\IgcsDllToClient"
#define PIPENAME_CLIENT_TO_DLL 	"\\\\.\\pipe\\IgcsClientToDll"

static HANDLE _dllToClientPipe = nullptr;
static HANDLE _clientToDllPipe = nullptr;
static bool _dllToClientPipeConnected = false;
static bool _clientToDllPipeConnected = false;

void connectDllToClient();
void connectClientToDll();
void writeToClient();
void createReadThread();

int main()
{
	cout << "Named Pipe Tester, starting..." << endl;

	connectDllToClient();
	connectClientToDll();

	// now create a read thread which will read from the client to dll pipe
	createReadThread();

	// write indefinitely to the dll to client pipe.
	writeToClient();
}


void connectDllToClient()
{
	_dllToClientPipe = CreateFile(TEXT(PIPENAME_DLL_TO_CLIENT), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	_dllToClientPipeConnected = (_dllToClientPipe != INVALID_HANDLE_VALUE);
	cout << (!_dllToClientPipeConnected ? "DLL To Client pipe connect failed" : "DLL to Client pipe connect succeeded") << endl;
}

void connectClientToDll()
{
	// create named pipe
	_clientToDllPipe = CreateNamedPipe(TEXT(PIPENAME_CLIENT_TO_DLL), PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 0, 1024 * 4, 
									   NMPWAIT_WAIT_FOREVER, nullptr);
	_clientToDllPipeConnected = (_clientToDllPipe != INVALID_HANDLE_VALUE);
	cout << (!_clientToDllPipeConnected ? "Creation of Client to DLL named pipe failed" : "Creation of Client to DLL named pipe succeeded") << endl;
}

void writeToClient()
{
	cout << "Writing to client..." << endl;
	for(;;)
	{
		// write to the dll to client named pipe.
		DWORD numberOfBytesWritten;
		string messageToSend = "Message from dll!";
		auto result = WriteFile(_dllToClientPipe, messageToSend.c_str(), messageToSend.length() + 1, &numberOfBytesWritten, nullptr);
		if(!result)
		{
			cout << "WriteFile failed. Error: " << hex << GetLastError() << endl;
		}
		Sleep(2000);
	}
	cout << "Writing to client exit...";
}

DWORD WINAPI readThread(LPVOID lpParam)
{
	cout << "readThread started... " << endl;
	while(_clientToDllPipe!=INVALID_HANDLE_VALUE)
	{
		if(ConnectNamedPipe(_clientToDllPipe, nullptr))
		{
			cout << "Client connected to Client to DLL pipe..." << endl;
			BYTE buffer[1024];
			DWORD bytesRead;
			while(ReadFile(_clientToDllPipe, buffer, sizeof(buffer), &bytesRead, nullptr))
			{
				cout << "Message received from client of type: " << int(buffer[0]) << ". MessageId: " << int(buffer[1]) << ". Length: " << bytesRead << endl;
			}
		}
	}
	cout << "readThread exit..." << endl;
	return 1;
}

void createReadThread()
{
	DWORD threadId;
	HANDLE threadHandle = CreateThread(nullptr, 0, readThread, nullptr, 0, &threadId);
	cout << "Read thread created." << endl;
}

