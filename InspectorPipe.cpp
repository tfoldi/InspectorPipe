//#pragma comment(lib, "detoured.lib")
#pragma comment(lib, "detours.lib")

#include "stdafx.h"

#undef UNICODE
#include <cstdio>
#include <detours.h> //*IMPORTANT: Look at path if compiler error
#include <io.h>

//Prototypes
int (WINAPI *pWriteFile)(HANDLE hFile, LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped) = WriteFile;
int WINAPI MyWriteFile(HANDLE hFile, LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped);
int (WINAPI *pReadFile)(HANDLE hFile, LPVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped) = ReadFile;
int WINAPI MyReadFile(HANDLE hFile, LPVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped);

//Log files
FILE* pLogFile;

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch(Reason)
	{
	case DLL_PROCESS_ATTACH:	//Do standard detouring
		fopen_s(&pLogFile, "C:\\PipeLog.txt", "a+");
		DisableThreadLibraryCalls(hDLL);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)pWriteFile, MyWriteFile);
		if(DetourTransactionCommit() == NO_ERROR)
			OutputDebugString(L"WriteFile() detoured successfully");
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)pReadFile, MyReadFile);
		if(DetourTransactionCommit() == NO_ERROR)
			OutputDebugString(L"ReadFile() detoured successfully");
		break;
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();	//Detach
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)pWriteFile, MyWriteFile);
		DetourTransactionCommit();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)pReadFile, MyReadFile);
		DetourTransactionCommit();
		fclose(pLogFile);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

//Open file, write contents, close it
int WINAPI MyWriteFile(HANDLE hFile, LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	if ( (HANDLE)_get_osfhandle( _fileno(pLogFile) ) != hFile ) {
		fprintf(pLogFile,"\r\n>>> Write %lu bytes to HANDLE 0x%lx\r\n", nNumberOfBytesToWrite, hFile);
		fwrite(lpBuffer, 1, nNumberOfBytesToWrite, pLogFile);
		fputs("\r\n---\r\n",pLogFile);

	}
	return pWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

int WINAPI MyReadFile(HANDLE hFile, LPVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	int ret = pReadFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	fprintf(pLogFile,"\r\n<<< Read %lu bytes from HANDLE 0x%lx\r\n", nNumberOfBytesToWrite, hFile);
	fwrite(lpBuffer, 1, nNumberOfBytesToWrite, pLogFile);
	fputs("\r\n---\r\n",pLogFile);

	return ret;
}