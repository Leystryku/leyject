

#include "stdafx.h"
#include <Windows.h>
#include <string>
#include <strsafe.h>
#include <iostream>
#include <fstream>

using namespace std;

BOOL InjectRemoteThread(DWORD);

string dllname = "";

#include  <io.h>

inline bool exists_test2(const std::string& name) { //ty stackoverflow
	return (access(name.c_str(), 0) != -1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		
		cout << "Waiting for Game Window..." << endl;

		DWORD processID;
		HWND hwnd = NULL;
		char windowtitle[255];
		

		

		while( hwnd == NULL )
		{
			hwnd = FindWindowA("Valve001", NULL);
			if(!hwnd)
				hwnd = FindWindowA(0, "Counter-Strike: Global Offensive");

			Sleep(10);
		}

		GetWindowTextA(hwnd, windowtitle, 255);
		CreateDirectoryA("C:\\Nikyuria", NULL);
		CreateDirectoryA("C:\\Nikyuria\\dll", NULL);
		
		std::string swindowtitle;

		int len = strlen(windowtitle);
		for (int i = 0; i < len; i++)
		{
			if (i > len)
				break;

			if (windowtitle[i] == ' ')
				continue;

			if (windowtitle[i] == ':')
			{
				swindowtitle.push_back('_');
				continue;
			}

			char lowerchar = tolower(windowtitle[i]);

			swindowtitle.push_back(lowerchar);

		}
		cout << "Found game window - " << swindowtitle << endl;

		dllname = "C:\\Nikyuria\\dll\\";
		dllname.append(swindowtitle);
		dllname.append(".dll");

		if (!exists_test2(dllname.c_str()))
		{
			
			cout << "didnt find " << dllname.c_str() << ", using generic dll!" << std::endl;


			if (!exists_test2("C:\\Nikyuria\\dll\\generic.dll"))
			{
				cout << "didnt find generic DLL!" << std::endl;
				cin.get();
				return 0;
			}
				

			dllname = "C:\\Nikyuria\\dll\\generic.dll";
		}

		
		GetWindowThreadProcessId(hwnd, &processID);
		cout << "Injecting " << dllname.c_str() << " ..." << endl;
		InjectRemoteThread(processID);

	}
	catch (exception ex)
	{
		MessageBox( NULL, "#001", "Error while injecting into game.", MB_OK );
	
		return 1;
	}

	return 0;
}



BOOL InjectRemoteThread(DWORD ProcessID)
{
    HANDLE RemoteProc;
    LPVOID MemAlloc;
    LPVOID LoadLibAddress;

    if ( !ProcessID )
    {
        MessageBox(NULL, (LPCSTR)GetLastError(), "An error occured !", NULL);
        return 0;
    }
    RemoteProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessID);
    if ( !RemoteProc )
    {
        MessageBox(NULL, (LPCSTR)GetLastError(), "An error occured opening the target process !", NULL);
        return 0;
    }
    LoadLibAddress      =   (LPVOID)LoadLibraryA;
    MemAlloc            =   (LPVOID)VirtualAllocEx(RemoteProc, NULL, strlen(dllname.c_str())+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(RemoteProc, (LPVOID)MemAlloc, dllname.c_str(), strlen(dllname.c_str()) + 1, NULL);
    HANDLE t = CreateRemoteThread(RemoteProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddress, (LPVOID)MemAlloc, CREATE_SUSPENDED, NULL);
	Sleep(50);
	ResumeThread(t);

	try
	{
		VirtualFreeEx(RemoteProc, (LPVOID)MemAlloc, 0, MEM_RELEASE | MEM_DECOMMIT);
		CloseHandle(RemoteProc);
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
		return 1;
	}

	cout << "Injected !" << endl;
	Sleep(10000);

    return 1;
}