#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <sstream>
#include <Windows.h>
#include <TlHelp32.h>

#include "inject_func.h"

using namespace std;

bool isValidFile(const string& file)
{
	struct stat buffer;
	return (stat(file.c_str(), &buffer) == 0);
}

int main(int argc, int** argv)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::stringstream cmd;
	cmd << "StarskyPC.exe /HWND=" << (int)GetConsoleHwnd() << " /SpawnerRespawn" << endl;

	cout << "Ready to execute " << cmd.str() << endl;

	system("pause");

	CreateProcess(NULL, LPSTR(cmd.str().c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	//Sleep(100);

	if (isValidFile("addon.dll"))
	{
		cout << "Injecting.." << endl;
		Inject(pi.dwProcessId, "addon.dll");
	}

	//SuspendThread(pi.hThread);

	//system("pause");

	ResumeThread(pi.hThread);

	WaitForSingleObject(pi.hProcess, INFINITE);
	return 0;
}