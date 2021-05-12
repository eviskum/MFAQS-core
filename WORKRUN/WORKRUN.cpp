// WORKRUN.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "WORKRUN.h"
#include "process.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CWORKRUNApp

BEGIN_MESSAGE_MAP(CWORKRUNApp, CWinApp)
	//{{AFX_MSG_MAP(CWORKRUNApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWORKRUNApp construction

CWORKRUNApp::CWORKRUNApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWORKRUNApp object

CWORKRUNApp theApp;

typedef void (CALLBACK* LPFNLOGFUNC1)(int, LPCTSTR);
typedef void (CALLBACK* LPFNLOGFUNC2)(int, LPCTSTR);

#define LJNORM( txt )	LogNorm(workpart, txt);
#define LJERR( txt )	LogErr(workpart, txt);

#define LOGERROR 1
#define LOGSUCCES 2

LPFNLOGFUNC1 LogNorm;
LPFNLOGFUNC2 LogErr;

int workpart;

typedef struct {
	int logtype;
	TCHAR logtxt[4000];
} ComBuf;

class WorkInfo {
public:
	WorkInfo(int part);
	~WorkInfo();
	HANDLE hLogEvent;
	HANDLE hEndLogHandling;
	HANDLE hLogReady;
	HANDLE hEndWorkProc;
	HANDLE hWorkProcEnded;
	int partion;
	HANDLE MapHandle;
	ComBuf *combuf;
	void GetLogTxt();
};

WorkInfo::WorkInfo(int part) {
	partion = part;
	TCHAR filemap[1024];
	sprintf(filemap, "MFAQSWORK%i", partion);
	MapHandle = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 4096,
								(LPCTSTR) filemap);
	combuf = (ComBuf *) MapViewOfFile(MapHandle, FILE_MAP_WRITE, 0, 0, 4096);

	SECURITY_ATTRIBUTES signalatt;
	signalatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	signalatt.lpSecurityDescriptor = NULL;
	signalatt.bInheritHandle = TRUE;
	TCHAR logevent[1024];
	sprintf(logevent, "MFAQSLOGWORK%i", partion);
	hLogEvent = CreateEvent(&signalatt, FALSE, TRUE, logevent);
	hEndLogHandling = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	TCHAR logready[1024];
	sprintf(logready, "MFAQSLOGREADY%i", partion);
	hLogReady = CreateEvent(&signalatt, FALSE, TRUE, logready);
	TCHAR endwork[1024];
	sprintf(endwork, "MFAQSENDWORK%i", partion);
	hEndWorkProc = CreateEvent(&signalatt, FALSE, FALSE, endwork);
	TCHAR workended[1024];
	sprintf(workended, "MFAQSWORKENDED%i", partion);
	hWorkProcEnded = CreateEvent(&signalatt, FALSE, FALSE, workended);
}

WorkInfo::~WorkInfo() {
	CloseHandle(hWorkProcEnded);
	CloseHandle(hEndWorkProc);
	CloseHandle(hLogReady);
	CloseHandle(hEndLogHandling);
	CloseHandle(hLogEvent);
	UnmapViewOfFile(combuf);
	CloseHandle(MapHandle);
}

void WorkInfo::GetLogTxt() {
	TCHAR logtxt[4096];
	strcpy(logtxt, combuf->logtxt);
	if (combuf->logtype == LOGERROR) LJERR(logtxt)
	else LJNORM(logtxt);
}

UINT LogHandling( LPVOID pWork)
{
	WorkInfo *pWorkInfo = (WorkInfo *) pWork;

	int cont = TRUE;
	do {
		if (WaitForSingleObject(pWorkInfo->hLogEvent, 3000) == WAIT_TIMEOUT) {
			if (WaitForSingleObject(pWorkInfo->hEndLogHandling, 0) == WAIT_OBJECT_0) cont = FALSE;
		} else {
			pWorkInfo->GetLogTxt();
			SetEvent(pWorkInfo->hLogReady);
		}
	} while (cont);
	return(0);
}

WorkInfo *ptrWorkInfo;
CWinThread *hWorkLoop;

extern "C" int PASCAL EXPORT DllLogReg(void *Func1, void *Func2) {
	LogNorm = (LPFNLOGFUNC1) Func1;
	LogErr = (LPFNLOGFUNC2) Func2;
	return(TRUE);
}


extern "C" int PASCAL EXPORT StopWork(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	return(TRUE);
}

extern "C" int PASCAL EXPORT StartWork(int part, int argc, TCHAR *argv[]) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	workpart = part;
	ptrWorkInfo = new WorkInfo(workpart);
	
	if ((hWorkLoop = AfxBeginThread(&LogHandling, (LPVOID) ptrWorkInfo)) == NULL) {
		LJERR("Kunne ikke starte LogHandling thread")
		delete ptrWorkInfo;
		return(FALSE);
	}
	LJNORM("Loader work process")

		CreateProcess


BOOL CreateProcess(argv[0], NULL, NULL, NULL, FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS,
				   NULL, NULL, 


  LPVOID lpEnvironment,  // pointer to new environment block
  LPCTSTR lpCurrentDirectory,   // pointer to current directory name
  LPSTARTUPINFO lpStartupInfo,  // pointer to STARTUPINFO
  LPPROCESS_INFORMATION lpProcessInformation  // pointer to PROCESS_INFORMATION
);





//	_spawnl(_P_DETACH, "WORK\\SAPINTF\\debug\\SAPINTF.EXE", "SAPINTF" "0", "-DUDVRFC", NULL);
	LJNORM("Work process spawned")

	return(TRUE);
}
