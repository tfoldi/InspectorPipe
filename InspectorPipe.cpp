//#pragma comment(lib, "detoured.lib")
#pragma comment(lib, "detours.lib")

#include "stdafx.h"

#undef UNICODE
#include <cstdio>
#include <detours.h> //*IMPORTANT: Look at path if compiler error
#include <io.h>
#include "InspectorPipe.h"

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
HMODULE  hTdeapi;


TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ServerProcessGetErrorInfo,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_ServerProcessGetErrorInfo, a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_QueryExecute,void *a1, int Src, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_QueryExecute,a1, Src, a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_StatementOpen,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_StatementOpen,a1,a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_StatementPrepare,void *a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_StatementPrepare,a1,a2,a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_StatementExecute,void *a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_StatementExecute,a1,a2,a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_GetErrorInfo,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_GetErrorInfo,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ErrorInfoGetCount,int a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_ErrorInfoGetCount, a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ServerProcessOpen,int a1);
TAB_LAZY_IMPLEMENT_BODY(TDE_ServerProcessOpen, a1);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ServerProcessStop,void *a1);
TAB_LAZY_IMPLEMENT_BODY(TDE_ServerProcessStop,a1);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ServerProcessIsAlive,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_ServerProcessIsAlive,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ServerProcessGetProcessId,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_ServerProcessGetProcessId,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ServerProcessClose,void *a1); // idb
TAB_LAZY_IMPLEMENT_BODY(TDE_ServerProcessClose,a1); // idb

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ConnectionOpen,int a1);
TAB_LAZY_IMPLEMENT_BODY(TDE_ConnectionOpen, a1);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ConnectionGetProperties,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_ConnectionGetProperties,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ConnectionDisconnect,void *a1);
TAB_LAZY_IMPLEMENT_BODY(TDE_ConnectionDisconnect,a1);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ConnectionClose,void * a1); // idb
TAB_LAZY_IMPLEMENT_BODY(TDE_ConnectionClose,a1); // idb

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_PropertiesGetCount,int a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_PropertiesGetCount, a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_PropertiesGetProperty,int a1, int a2, unsigned int a3, int a4, int a5);
TAB_LAZY_IMPLEMENT_BODY(TDE_PropertiesGetProperty, a1,  a2, a3,  a4,  a5);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_PropertiesClose,int a1, void * a2); // idb
TAB_LAZY_IMPLEMENT_BODY(TDE_PropertiesClose,a1, a2); // idb

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_StatementCancel,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_StatementCancel,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_StatementEstimateProgress,void *a1, int a2, int a3, int a4, int a5, int a6);
TAB_LAZY_IMPLEMENT_BODY(TDE_StatementEstimateProgress,a1,  a2,  a3,  a4,  a5,  a6);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_StatementClose,void *a1, void *a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_StatementClose,a1, a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_RowsetGetColumnCount,int a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_RowsetGetColumnCount, a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_RowsetGetColumnProperties,int a1, int a2, unsigned int a3, int a4);
TAB_LAZY_IMPLEMENT_BODY(TDE_RowsetGetColumnProperties, a1,  a2,  a3,  a4);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_RowsetHasMoreValues,void *a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_RowsetHasMoreValues,a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_RowsetGetValues,void *a1, int a2, unsigned int a3, int a4, int a5);
TAB_LAZY_IMPLEMENT_BODY(TDE_RowsetGetValues,a1,  a2, a3,  a4,  a5);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_RowsetGetValueArray,void *a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_RowsetGetValueArray,a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_RowsetClose,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_RowsetClose,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ValueArrayClear,int a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_ValueArrayClear, a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ValueArrayGetSizeBytes,int a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_ValueArrayGetSizeBytes, a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ValueArrayGetNumValues,int a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_ValueArrayGetNumValues, a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ValueArrayReadValues,int a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_ValueArrayReadValues, a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ValueArrayWriteValues,int a1, void *a2,  int a3, int a4);
TAB_LAZY_IMPLEMENT_BODY(TDE_ValueArrayWriteValues, a1, a2,   a3,  a4);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertOpen,void *a1, int a2, int a3, int a4, int a5);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertOpen,a1,  a2,  a3,  a4,  a5);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertGetColumnCount,int a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertGetColumnCount, a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertGetColumnProperties,void *a1, int a2, int Src, int a4);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertGetColumnProperties,a1,  a2,  Src,  a4);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertPutValues,void *a1, int a2, int Src, int a4);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertPutValues,a1,  a2,  Src,  a4);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertValueArrayOpen,void *a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertValueArrayOpen,a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertPutValueArray,void *a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertPutValueArray,a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertCommit,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertCommit,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_InsertClose,void *a1, void *a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_InsertClose,a1, a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_DatabaseUploadOpen,void *a1, int Src, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_DatabaseUploadOpen,a1,  Src,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_UploadSetSize,void *a1, int a2, char Src, int a4);
TAB_LAZY_IMPLEMENT_BODY(TDE_UploadSetSize,a1,  a2,  Src,  a4);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_UploadSend,void *a1, int a2, int Src, int a4);
TAB_LAZY_IMPLEMENT_BODY(TDE_UploadSend,a1,  a2,  Src,  a4);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_UploadCommit,void *a1, int a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_UploadCommit,a1,  a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_UploadClose,void *a1, void *a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_UploadClose,a1, a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_DatabaseDownloadOpen,void *a1, int Src, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_DatabaseDownloadOpen,a1,  Src,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_DownloadGetSize,int a1, int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_DownloadGetSize, a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_DownloadRecv,void *a1, int a2, int Src, void *a4, int a5);
TAB_LAZY_IMPLEMENT_BODY(TDE_DownloadRecv,a1,  a2,  Src, a4,  a5);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_DownloadClose,void *a1, void *a2);
TAB_LAZY_IMPLEMENT_BODY(TDE_DownloadClose,a1, a2);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ErrorInfoGetError,int a1, unsigned int a2, int a3);
TAB_LAZY_IMPLEMENT_BODY(TDE_ErrorInfoGetError,a1,  a2,  a3);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ServerProcessGetConnectionDescriptor,void *a1, char *Dest, size_t Count);
TAB_LAZY_IMPLEMENT_BODY(TDE_ServerProcessGetConnectionDescriptor,a1, Dest,  Count);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ConnectionConnect,void *a1, char *Src);
TAB_LAZY_IMPLEMENT_BODY(TDE_ConnectionConnect,a1, Src);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ConnectionDescriptorTcp,char *Src, unsigned __int16 a2, char *Dest, size_t Count);
TAB_LAZY_IMPLEMENT_BODY(TDE_ConnectionDescriptorTcp,Src,  a2, Dest,  Count);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ConnectionDescriptorNamedPipe,void *Src, char *a2, char *a3, size_t Count);
TAB_LAZY_IMPLEMENT_BODY(TDE_ConnectionDescriptorNamedPipe, Src,  a2, a3,  Count);

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ValueArrayClose,int a1, void *a2); // idb
TAB_LAZY_IMPLEMENT_BODY(TDE_ValueArrayClose,a1,a2); // idb

TAB_LAZY_IMPLEMENT_FUNCTION(TDE_ErrorInfoClose,void * a1); // idb
TAB_LAZY_IMPLEMENT_BODY(TDE_ErrorInfoClose,a1); // idb

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch(Reason)
	{
	case DLL_PROCESS_ATTACH:	//Do standard detouring
		hTdeapi= LoadLibrary(L"tdeapi.dll");	
		fopen_s(&pLogFile, "PipeLog.txt", "a+");
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
		DETOUR_ATTACH(TDE_ServerProcessGetErrorInfo);
		DETOUR_ATTACH(TDE_GetErrorInfo);
		DETOUR_ATTACH(TDE_ErrorInfoGetCount);
		DETOUR_ATTACH(TDE_ServerProcessOpen);
		DETOUR_ATTACH(TDE_ServerProcessStop);
		DETOUR_ATTACH(TDE_ServerProcessIsAlive);
		DETOUR_ATTACH(TDE_ServerProcessGetProcessId);
		DETOUR_ATTACH(TDE_ServerProcessClose);
		DETOUR_ATTACH(TDE_ConnectionOpen);
		DETOUR_ATTACH(TDE_ConnectionGetProperties);
		DETOUR_ATTACH(TDE_ConnectionDisconnect);
		DETOUR_ATTACH(TDE_ConnectionClose);
		DETOUR_ATTACH(TDE_PropertiesGetCount);
		DETOUR_ATTACH(TDE_PropertiesGetProperty);
		DETOUR_ATTACH(TDE_PropertiesClose);
		DETOUR_ATTACH(TDE_QueryExecute);
		DETOUR_ATTACH(TDE_StatementOpen);
		DETOUR_ATTACH(TDE_StatementPrepare);
		DETOUR_ATTACH(TDE_StatementExecute);
		DETOUR_ATTACH(TDE_StatementCancel);
		DETOUR_ATTACH(TDE_StatementEstimateProgress);
		DETOUR_ATTACH(TDE_StatementClose);
		DETOUR_ATTACH(TDE_RowsetGetColumnCount);
		DETOUR_ATTACH(TDE_RowsetGetColumnProperties);
		DETOUR_ATTACH(TDE_RowsetHasMoreValues);
		DETOUR_ATTACH(TDE_RowsetGetValues);
		DETOUR_ATTACH(TDE_RowsetGetValueArray);
		DETOUR_ATTACH(TDE_RowsetClose);
		DETOUR_ATTACH(TDE_ValueArrayClear);
		DETOUR_ATTACH(TDE_ValueArrayGetSizeBytes);
		DETOUR_ATTACH(TDE_ValueArrayGetNumValues);
		DETOUR_ATTACH(TDE_ValueArrayReadValues);
		DETOUR_ATTACH(TDE_ValueArrayWriteValues);
		DETOUR_ATTACH(TDE_InsertOpen);
		DETOUR_ATTACH(TDE_InsertGetColumnCount);
		DETOUR_ATTACH(TDE_InsertGetColumnProperties);
		DETOUR_ATTACH(TDE_InsertPutValues);
		DETOUR_ATTACH(TDE_InsertValueArrayOpen);
		DETOUR_ATTACH(TDE_InsertPutValueArray);
		DETOUR_ATTACH(TDE_InsertCommit);
		DETOUR_ATTACH(TDE_InsertClose);
		DETOUR_ATTACH(TDE_DatabaseUploadOpen);
		DETOUR_ATTACH(TDE_UploadSetSize);
		DETOUR_ATTACH(TDE_UploadSend);
		DETOUR_ATTACH(TDE_UploadCommit);
		DETOUR_ATTACH(TDE_UploadClose);
		DETOUR_ATTACH(TDE_DatabaseDownloadOpen);
		DETOUR_ATTACH(TDE_DownloadGetSize);
		DETOUR_ATTACH(TDE_DownloadRecv);
		DETOUR_ATTACH(TDE_DownloadClose);
		DETOUR_ATTACH(TDE_ErrorInfoGetError);
		DETOUR_ATTACH(TDE_ServerProcessGetConnectionDescriptor);
		DETOUR_ATTACH(TDE_ServerProcessStart);
		DETOUR_ATTACH(TDE_ConnectionConnect);
		DETOUR_ATTACH(TDE_ConnectionDescriptorTcp);
		DETOUR_ATTACH(TDE_ConnectionDescriptorNamedPipe);
		DETOUR_ATTACH(TDE_ValueArrayClose);
		DETOUR_ATTACH(TDE_ErrorInfoClose);

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
		DETOUR_DETACH(TDE_ServerProcessGetErrorInfo);
		DETOUR_DETACH(TDE_GetErrorInfo);
		DETOUR_DETACH(TDE_ErrorInfoGetCount);
		DETOUR_DETACH(TDE_ServerProcessOpen);
		DETOUR_DETACH(TDE_ServerProcessStop);
		DETOUR_DETACH(TDE_ServerProcessIsAlive);
		DETOUR_DETACH(TDE_ServerProcessGetProcessId);
		DETOUR_DETACH(TDE_ServerProcessClose);
		DETOUR_DETACH(TDE_ConnectionOpen);
		DETOUR_DETACH(TDE_ConnectionGetProperties);
		DETOUR_DETACH(TDE_ConnectionDisconnect);
		DETOUR_DETACH(TDE_ConnectionClose);
		DETOUR_DETACH(TDE_PropertiesGetCount);
		DETOUR_DETACH(TDE_PropertiesGetProperty);
		DETOUR_DETACH(TDE_PropertiesClose);
		DETOUR_DETACH(TDE_QueryExecute);
		DETOUR_DETACH(TDE_StatementOpen);
		DETOUR_DETACH(TDE_StatementPrepare);
		DETOUR_DETACH(TDE_StatementExecute);
		DETOUR_DETACH(TDE_StatementCancel);
		DETOUR_DETACH(TDE_StatementEstimateProgress);
		DETOUR_DETACH(TDE_StatementClose);
		DETOUR_DETACH(TDE_RowsetGetColumnCount);
		DETOUR_DETACH(TDE_RowsetGetColumnProperties);
		DETOUR_DETACH(TDE_RowsetHasMoreValues);
		DETOUR_DETACH(TDE_RowsetGetValues);
		DETOUR_DETACH(TDE_RowsetGetValueArray);
		DETOUR_DETACH(TDE_RowsetClose);
		DETOUR_DETACH(TDE_ValueArrayClear);
		DETOUR_DETACH(TDE_ValueArrayGetSizeBytes);
		DETOUR_DETACH(TDE_ValueArrayGetNumValues);
		DETOUR_DETACH(TDE_ValueArrayReadValues);
		DETOUR_DETACH(TDE_ValueArrayWriteValues);
		DETOUR_DETACH(TDE_InsertOpen);
		DETOUR_DETACH(TDE_InsertGetColumnCount);
		DETOUR_DETACH(TDE_InsertGetColumnProperties);
		DETOUR_DETACH(TDE_InsertPutValues);
		DETOUR_DETACH(TDE_InsertValueArrayOpen);
		DETOUR_DETACH(TDE_InsertPutValueArray);
		DETOUR_DETACH(TDE_InsertCommit);
		DETOUR_DETACH(TDE_InsertClose);
		DETOUR_DETACH(TDE_DatabaseUploadOpen);
		DETOUR_DETACH(TDE_UploadSetSize);
		DETOUR_DETACH(TDE_UploadSend);
		DETOUR_DETACH(TDE_UploadCommit);
		DETOUR_DETACH(TDE_UploadClose);
		DETOUR_DETACH(TDE_DatabaseDownloadOpen);
		DETOUR_DETACH(TDE_DownloadGetSize);
		DETOUR_DETACH(TDE_DownloadRecv);
		DETOUR_DETACH(TDE_DownloadClose);
		DETOUR_DETACH(TDE_ErrorInfoGetError);
		DETOUR_DETACH(TDE_ServerProcessGetConnectionDescriptor);
		DETOUR_DETACH(TDE_ServerProcessStart);
		DETOUR_DETACH(TDE_ConnectionConnect);
		DETOUR_DETACH(TDE_ConnectionDescriptorTcp);
		DETOUR_DETACH(TDE_ConnectionDescriptorNamedPipe);
		DETOUR_DETACH(TDE_ValueArrayClose);
		DETOUR_DETACH(TDE_ErrorInfoClose);

		fclose(pLogFile);
		if ( hTdeapi )
			FreeLibrary(hTdeapi);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


int MyTDE_ServerProcessStart(void *a1, int a2, char *Src, int a4, int a5)
{
	fprintf(pLogFile,"\r\n--- Called myTDE_ServerProcessStart with %s\r\n", Src);
	return pTDE_ServerProcessStart(a1,a2,Src,a4,a5);
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