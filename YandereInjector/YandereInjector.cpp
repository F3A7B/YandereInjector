#include <iostream>
#include <Windows.h>
#include <string>
#include <stdio.h>
#include <TlHelp32.h>

using namespace std;

void GetProcessID(const char* Process, DWORD& ProcessID) {
	HANDLE ProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(ProcessSnapshot, &ProcessEntry);
	do {
		if (!strcmp(ProcessEntry.szExeFile, Process)) {
			ProcessID = ProcessEntry.th32ProcessID;
		} 
	} while (Process32Next(ProcessSnapshot, &ProcessEntry));
	CloseHandle(ProcessSnapshot);

	if (!ProcessID) {
		printf("[!] Failed to find target process!");
		Sleep(4000);
		exit(0);
	}
}

int main() {
	string DllName;
	string TargetName;
	DWORD TargetPID;
	char InjectorPath[MAX_PATH];
	char DllPath[MAX_PATH];
	char OriginalBytes[5];

	SetConsoleTitleA("YandereInjector by Nexo");

	cout << "YandereInjector by Nexo\n\n";
	cout << "Provide DLL name (with .dll): "; cin >> DllName;
	cout << "Provide process name (with .exe): "; cin >> TargetName;

	DllName.insert(0, "\\");

	GetCurrentDirectoryA(sizeof(InjectorPath), InjectorPath);
	strncpy(DllPath, InjectorPath, sizeof(DllPath));
	strncat(DllPath, DllName.c_str(), sizeof(DllPath));
	
	GetProcessID(TargetName.c_str(), TargetPID);

	HANDLE TargetHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, TargetPID);
	LPVOID OpenFile = GetProcAddress(LoadLibraryW(L"ntdll"), "NtOpenFile");

	memcpy(OriginalBytes, OpenFile, 5);
	WriteProcessMemory(TargetHandle, OpenFile, OriginalBytes, 5, 0);

	LPVOID LoadLib			= (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	LPVOID Memory			= (LPVOID)VirtualAllocEx(TargetHandle, 0, strlen(DllPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(TargetHandle, Memory, DllPath, strlen(DllPath), 0);
	CreateRemoteThread(TargetHandle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLib, (LPVOID)Memory, 0, 0);
	CloseHandle(TargetHandle);
}