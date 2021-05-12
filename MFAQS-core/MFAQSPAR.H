// MiniFAQS partion


// #ifndef MFAQSPAR
// #define MFAQSPAR


#ifndef _MFAQSPAR_H_
#define _MFAQSPAR_H_


// #include <afx.h>
#include "mfaqsjob.h"
#include "mfaqslog.h"


#define DLLSUCCES	1
#define DLLERROR	2

typedef int (CALLBACK* LPFNDLLFUNC1)(int, char**);
typedef int (CALLBACK* LPFNDLLFUNC2)(HANDLE, HANDLE);
typedef int (CALLBACK* LPFNDLLFUNC3)(LPCTSTR, int*, int*);


class DllWorker
{
private:
	CString DllFile;
	HINSTANCE hDll;
	LPFNDLLFUNC1 WorkerFunc;
	LPFNDLLFUNC2 DllLogReg;
	CString service_name;
	CWinThread *hDllLogHandling;
public:
	DllWorker();
	~DllWorker();
	TCHAR temp_str[500];
	int mode, job, partion;
	HANDLE hLogEvent, hLogReady, hDllEndHandling;
	LPFNDLLFUNC3 DllGetLog;
	DllWorker(LPCTSTR filename, int part, LPCTSTR set_name);
	int LoadInit();
	void Release();
	int Run( int argc, char *argv[] );
	CString& GetServiceName();
	CString& GetDllName();
	void Serialize( CArchive& ar );
};

typedef struct {
	int no_workers;
	CString filename [100];
	CString servicename[100];
} DLLWORKERS;

class PartionInfo : public CObject
{
private:
	int no_dllworkers;
	DllWorker *workers[100];
public:
	PartionInfo();
	~PartionInfo();
	PartionInfo(JOBQUE *set_que, MFAQSJOB **set_job_ptr, int set_partion);
	void Serialize( CArchive& ar );
	DLLWORKERS *GetWorkerList();
	int InsertWorker(LPCTSTR filename, LPCTSTR servicename, int part);
	int DeleteWorker(LPCTSTR servicename);
	int LoadDlls();
	void ReleaseDlls();
	DllWorker& GetWorker(LPCTSTR servicename);

	JOBQUE *que;
	MFAQSJOB **job_ptr;
	HANDLE hReady, hInitError, hEndPartion, hStopUnloadDlls, hPartionStopped, hStartLoadDlls;
	CWinThread *hPartThread;
	int partion;
};


extern MFAQSJOB *runningjob[5];
extern PartionInfo *partions[5];



void StopPartion(int part);
int RestartPartion(int part);
int StartPartion(int part);
void EndPartion(int part);



#endif