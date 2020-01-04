#pragma once

#include <iostream>

using std::cout;
using std::endl;
using std::string;

//Code from wiki

bool Inject(DWORD pId, const char* dllName)
{
	//Apro il processo con tutti i permessi
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, false, pId);


	//Se sono riuscito ad aprire il processo...
	if (h) {
		cout << "Processo aperto correttamente" << endl;

		//Ottengo l'indirizzo della funzione LoadLibraryA
		LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

		//Alloco uno spazio nella memoria del processo
		//Leggibile e scrivibile
		LPVOID dereercomp = VirtualAllocEx(h, NULL, strlen(dllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		//Scrivo il path della DLL
		if (!WriteProcessMemory(h, dereercomp, dllName, strlen(dllName), NULL)) {
			cout << "Errore nello scrivere la memoria del processo!" << endl;
			return false;
		}
		else {
			cout << "Scrittura memoria del processo andata a buon fine" << endl;
		}

		//CreateRemoteThread() per creare un nuovo thread nel processo taget.
		//Questo nuovo thread chiama la funzione LoadLibraryA()
		//e per unico argomento un puntatore alla stringa allocata all'interno dello stack del nostro programma contenente il path alla nostra dll
		HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
		WaitForSingleObject(asdc, INFINITE);
		//Tolgo lo spazio allocato
		VirtualFreeEx(h, dereercomp, strlen(dllName), MEM_RELEASE);
		//Chiudo il thread
		CloseHandle(asdc);
		//Chiudo il processo
		CloseHandle(h);
		return true;
	}

	return false;
}

DWORD GetProcessByName(PCSTR name)
{
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			if (string(process.szExeFile) == string(name))
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	return pid;
}

HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.
	char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
										// WindowTitle.
	char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
										// WindowTitle.

	// Fetch current window title.

	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

	// Format a "unique" NewWindowTitle.

	wsprintf(pszNewWindowTitle, "%d/%d",
		GetTickCount(),
		GetCurrentProcessId());

	// Change current window title.

	SetConsoleTitle(pszNewWindowTitle);

	// Ensure window title has been updated.

	Sleep(40);

	// Look for NewWindowTitle.

	hwndFound = FindWindow(NULL, pszNewWindowTitle);

	// Restore original window title.

	SetConsoleTitle(pszOldWindowTitle);

	return(hwndFound);
}