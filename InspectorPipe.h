// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the INSPECTORPIPE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// INSPECTORPIPE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef INSPECTORPIPE_EXPORTS
#define INSPECTORPIPE_API __declspec(dllexport)
#else
#define INSPECTORPIPE_API __declspec(dllimport)
#endif



#define DETOUR_ATTACH( FNC )													\
	p ## FNC = (t_ ## FNC)GetProcAddress(hTdeapi, #FNC);						\
	DetourTransactionBegin();													\
	DetourUpdateThread(GetCurrentThread());										\
	DetourAttach(&(PVOID&)p ##FNC, My ## FNC);					 				\
	if(DetourTransactionCommit() == NO_ERROR)									\
	OutputDebugString(L#FNC L"() detoured successfully\r\n");

#define DETOUR_DETACH( FNC )													\
	DetourTransactionBegin();												\
	DetourUpdateThread(GetCurrentThread());									\
	DetourDetach(&(PVOID&)p ##FNC, My ## FNC);								\
	DetourTransactionCommit();

#define TAB_PROTOTYPE( FNC, ... )							\
	typedef int (* t_ ##FNC )(  __VA_ARGS__ );				\
	t_ ##FNC p ##FNC;										\
	int My ## FNC( __VA_ARGS__ ); 

#define TAB_LAZY_IMPLEMENT_FUNCTION( FNC, ... )						\
	int My ## FNC ## ( __VA_ARGS__ )								\
	{																\
		int ret;

#define TAB_LAZY_IMPLEMENT_BODY( FNC, ... )						\
	fprintf(pLogFile,"\r\n>>> Entering " #FNC "\r\n");		\
	ret = p ## FNC( __VA_ARGS__ );							\
	fprintf(pLogFile,"\r\n<<< Leaving " #FNC "\r\n");		\
	return ret;												\
	};


TAB_PROTOTYPE(TDE_ServerProcessGetErrorInfo,void *a1, int a2);
TAB_PROTOTYPE(TDE_GetErrorInfo,void *a1, int a2);
TAB_PROTOTYPE(TDE_ErrorInfoGetCount,int a1, int a2);
TAB_PROTOTYPE(TDE_ServerProcessOpen,int a1);
TAB_PROTOTYPE(TDE_ServerProcessStop,void *a1);
TAB_PROTOTYPE(TDE_ServerProcessIsAlive,void *a1, int a2);
TAB_PROTOTYPE(TDE_ServerProcessGetProcessId,void *a1, int a2);
TAB_PROTOTYPE(TDE_ServerProcessClose,void *); // idb
TAB_PROTOTYPE(TDE_ConnectionOpen,int a1);
TAB_PROTOTYPE(TDE_ConnectionGetProperties,void *a1, int a2);
TAB_PROTOTYPE(TDE_ConnectionDisconnect,void *a1);
TAB_PROTOTYPE(TDE_ConnectionClose,void *); // idb
TAB_PROTOTYPE(TDE_PropertiesGetCount,int a1, int a2, int a3);
TAB_PROTOTYPE(TDE_PropertiesGetProperty,int a1, int a2, unsigned int a3, int a4, int a5);
TAB_PROTOTYPE(TDE_PropertiesClose,int, void *); // idb
TAB_PROTOTYPE(TDE_QueryExecute,void *a1, wchar_t * Src, int a3);
TAB_PROTOTYPE(TDE_StatementOpen,void *a1, int a2);
TAB_PROTOTYPE(TDE_StatementPrepare,void *a1, int a2, int a3);
TAB_PROTOTYPE(TDE_StatementExecute,void *a1, int a2, int a3);
TAB_PROTOTYPE(TDE_StatementCancel,void *a1, int a2);
TAB_PROTOTYPE(TDE_StatementEstimateProgress,void *a1, int a2, int a3, int a4, int a5, int a6);
TAB_PROTOTYPE(TDE_StatementClose,void *a1, void *a2);
TAB_PROTOTYPE(TDE_RowsetGetColumnCount,int a1, int a2, int a3);
TAB_PROTOTYPE(TDE_RowsetGetColumnProperties,int a1, int a2, unsigned int a3, int a4);
TAB_PROTOTYPE(TDE_RowsetHasMoreValues,void *a1, int a2, int a3);
TAB_PROTOTYPE(TDE_RowsetGetValues,void *a1, int a2, unsigned int a3, int a4, int a5);
TAB_PROTOTYPE(TDE_RowsetGetValueArray,void *a1, int a2, int a3);
TAB_PROTOTYPE(TDE_RowsetClose,void *a1, int a2);
TAB_PROTOTYPE(TDE_ValueArrayClear,int a1, int a2);
TAB_PROTOTYPE(TDE_ValueArrayGetSizeBytes,int a1, int a2, int a3);
TAB_PROTOTYPE(TDE_ValueArrayGetNumValues,int a1, int a2, int a3);
TAB_PROTOTYPE(TDE_ValueArrayReadValues,int a1, int a2, int a3);
TAB_PROTOTYPE(TDE_ValueArrayWriteValues,int a1, void *a2,  int a3, int a4);
TAB_PROTOTYPE(TDE_InsertOpen,void *a1, int a2, int a3, int a4, int a5);
TAB_PROTOTYPE(TDE_InsertGetColumnCount,int a1, int a2, int a3);
TAB_PROTOTYPE(TDE_InsertGetColumnProperties,void *a1, int a2, int Src, int a4);
TAB_PROTOTYPE(TDE_InsertPutValues,void *a1, int a2, int Src, int a4);
TAB_PROTOTYPE(TDE_InsertValueArrayOpen,void *a1, int a2, int a3);
TAB_PROTOTYPE(TDE_InsertPutValueArray,void *a1, int a2, int a3);
TAB_PROTOTYPE(TDE_InsertCommit,void *a1, int a2);
TAB_PROTOTYPE(TDE_InsertClose,void *a1, void *a2);
TAB_PROTOTYPE(TDE_DatabaseUploadOpen,void *a1, int Src, int a3);
TAB_PROTOTYPE(TDE_UploadSetSize,void *a1, int a2, char Src, int a4);
TAB_PROTOTYPE(TDE_UploadSend,void *a1, int a2, int Src, int a4);
TAB_PROTOTYPE(TDE_UploadCommit,void *a1, int a2);
TAB_PROTOTYPE(TDE_UploadClose,void *a1, void *a2);
TAB_PROTOTYPE(TDE_DatabaseDownloadOpen,void *a1, int Src, int a3);
TAB_PROTOTYPE(TDE_DownloadGetSize,int a1, int a2, int a3);
TAB_PROTOTYPE(TDE_DownloadRecv,void *a1, int a2, int Src, void *a4, int a5);
TAB_PROTOTYPE(TDE_DownloadClose,void *a1, void *a2);
TAB_PROTOTYPE(TDE_ErrorInfoGetError,int a1, unsigned int a2, int a3);
TAB_PROTOTYPE(TDE_ServerProcessGetConnectionDescriptor,void *a1, char *Dest, size_t Count);
TAB_PROTOTYPE(TDE_ServerProcessStart,void *a1, int a2, char *Src, int a4, int a5);
TAB_PROTOTYPE(TDE_ConnectionConnect,void *a1, char *Src);
TAB_PROTOTYPE(TDE_ConnectionDescriptorTcp,char *Src, unsigned __int16 a2, char *Dest, size_t Count);
TAB_PROTOTYPE(TDE_ConnectionDescriptorNamedPipe,void *Src, char *a2, char *a3, size_t Count);
TAB_PROTOTYPE(TDE_ValueArrayClose,int, void *); // idb
TAB_PROTOTYPE(TDE_ErrorInfoClose,void *); // idb



