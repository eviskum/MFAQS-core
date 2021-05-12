// MFAQS partions styring

#include "StdAfx.h"

#include <fstream>
#include "mfaqspar.h"
#include "mfaqslog.h"
using namespace std;



MFAQSJOB *runningjob[5];
PartionInfo *partions[5];


UINT DllLogHandling( LPVOID dll_ptr)
{
	DllWorker *dll_info = (DllWorker *) dll_ptr;
	int cont = TRUE;
	do {
		if (WaitForSingleObject(dll_info->hLogEvent, 3000) == WAIT_TIMEOUT) {
			if (WaitForSingleObject(dll_info->hDllEndHandling, 0) == WAIT_OBJECT_0) cont = FALSE;
		} else {
			dll_info->DllGetLog(dll_info->temp_str, &dll_info->mode, &dll_info->job);
			if (dll_info->job == TRUE) {
				runningjob[dll_info->partion]->LogWrite(dll_info->partion, dll_info->mode, dll_info->temp_str);
			} else {
				if (dll_info->mode == LOGNORMAL) LPNORM(dll_info->partion, dll_info->temp_str)
				else LPERR(dll_info->partion, dll_info->temp_str)
			}
			SetEvent(dll_info->hLogReady);
		}
	} while (cont);
	return(0);
}

DllWorker::DllWorker()
{
	hDll = NULL;
	SECURITY_ATTRIBUTES signalatt;
	signalatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	signalatt.lpSecurityDescriptor = NULL;
	signalatt.bInheritHandle = TRUE;
	hLogEvent = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hLogReady = CreateEvent(&signalatt, FALSE, TRUE, NULL);
	hDllEndHandling = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hDllLogHandling = AfxBeginThread(&DllLogHandling, (LPVOID) this);
}

DllWorker::DllWorker(LPCTSTR filename, int part, LPCTSTR set_name)
{
	DllFile = filename;
	partion = part;
	service_name = set_name;
	hDll = NULL;
	SECURITY_ATTRIBUTES signalatt;
	signalatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	signalatt.lpSecurityDescriptor = NULL;
	signalatt.bInheritHandle = TRUE;
	hLogEvent = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hLogReady = CreateEvent(&signalatt, FALSE, TRUE, NULL);
	hDllEndHandling = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hDllLogHandling = AfxBeginThread(&DllLogHandling, (LPVOID) this);
}

DllWorker::~DllWorker()
{
	SetEvent(hDllEndHandling);
	DWORD ThreadStatus;
	do {
		GetExitCodeThread(hDllLogHandling, &ThreadStatus);
		Sleep(500);
	} while (ThreadStatus == STILL_ACTIVE);
	if (hDll != NULL) FreeLibrary(hDll);
}

int DllWorker::LoadInit()
{
	LPNORM(partion, CString("Loader service modul \"") + service_name + "\"")
	hDll = AfxLoadLibrary(DllFile);
	if (hDll != NULL) {
		LPNORM(partion, CString("initialiserer service modul \"") + service_name + "\"")
		WorkerFunc = (LPFNDLLFUNC1)GetProcAddress(hDll, "DllWorker");
		DllLogReg = (LPFNDLLFUNC2)GetProcAddress(hDll, "DllLogReg");
		DllGetLog = (LPFNDLLFUNC3)GetProcAddress(hDll, "DllGetLog");
	}
	if (hDll == NULL || WorkerFunc == NULL || DllLogReg == NULL) {
		LPERR(partion, CString("ERROR: kunne ikke loade/initialisere service modul \"") + service_name + "\"")
		return(FALSE);
	} else {
		DllLogReg(hLogEvent, hLogReady);
	}
	return(TRUE);
}

void DllWorker::Release()
{
	if (hDll != NULL) AfxFreeLibrary(hDll);
	hDll = NULL;
}

int DllWorker::Run( int argc, char *argv[] )
{
	int retval;
	LPNORM(partion, CString("Service modul \"") + service_name + "\" aktiveres")
	retval = WorkerFunc(argc, argv);
	return(retval);
}

CString& DllWorker::GetServiceName()
{
	return(service_name);
}

CString& DllWorker::GetDllName()
{
	return(DllFile);
}

void DllWorker::Serialize( CArchive& ar )
{
	if ( ar.IsStoring() ) {
		ar << DllFile;
		ar << partion;
		ar << service_name;
    } else {
		ar >> DllFile;
		ar >> partion;
		ar >> service_name;
	}
}




PartionInfo::PartionInfo()
{
	no_dllworkers = 0;
}

PartionInfo::~PartionInfo()
{
	for (int i = 0; i < no_dllworkers; i++) delete workers[i];
}

PartionInfo::PartionInfo(JOBQUE *set_que, MFAQSJOB **set_job_ptr, int set_partion)
{
	que = set_que;
	job_ptr = set_job_ptr;
	partion = set_partion;
	no_dllworkers = 0;
	SECURITY_ATTRIBUTES signalatt;
	signalatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	signalatt.lpSecurityDescriptor = NULL;
	signalatt.bInheritHandle = TRUE;
	hReady = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hInitError = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hEndPartion = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hStopUnloadDlls = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hPartionStopped = CreateEvent(&signalatt, FALSE, FALSE, NULL);
	hStartLoadDlls = CreateEvent(&signalatt, FALSE, FALSE, NULL);
}

void PartionInfo::Serialize( CArchive& ar )
{
	if ( ar.IsStoring() ) {
		ar << no_dllworkers;
		for (int i = 0; i < no_dllworkers; i++) workers[i]->Serialize( ar );

    } else {
		ar >> no_dllworkers;
		for (int i = 0; i < no_dllworkers; i++) {
			workers[i] = new DllWorker;
			workers[i]->Serialize( ar );
		}
	}
}

DLLWORKERS *PartionInfo::GetWorkerList()
{
	DLLWORKERS *workerlist;
	workerlist = new DLLWORKERS;
	workerlist->no_workers = no_dllworkers;
	for (int i = 0; i < no_dllworkers; i++) {
		workerlist->filename[i] = workers[i]->GetDllName();
		workerlist->servicename[i] = workers[i]->GetServiceName();
	}
	return(workerlist);
}

int PartionInfo::InsertWorker(LPCTSTR filename, LPCTSTR servicename, int part)
{
	workers[no_dllworkers++] = new DllWorker(filename, part, servicename);
	return(TRUE);
}

int PartionInfo::DeleteWorker(LPCTSTR servicename)
{
	int found = FALSE;

	for (int i = 0; i < no_dllworkers; i++) {
		if (found == TRUE) {
			workers[i-1] = workers[i];
		} else {
			if (workers[i]->GetServiceName() == servicename) {
				delete workers[i];
				found = TRUE;
			}
		}
	}
	if (found) {
		no_dllworkers--;
		return(TRUE);
	} else return (FALSE);
}

int PartionInfo::LoadDlls()
{
	int retval = TRUE;
	for (int i = 0; i < no_dllworkers; i++) {
		if (workers[i]->LoadInit() == FALSE) retval = FALSE;
	}
	return(retval);
}

void PartionInfo::ReleaseDlls()
{
	for (int i = 0; i < no_dllworkers; i++) {
		workers[i]->Release();
	}
}

DllWorker& PartionInfo::GetWorker(LPCTSTR servicename)
{
	int idx = -1;
	for (int i = 0; i < no_dllworkers; i++) {
		if (workers[i]->GetServiceName() == servicename) idx = i;
	}
	return(*workers[idx]);
}



UINT JobPartion( LPVOID partinfo)
{
	MFAQSJOB *currentjob;
	int succes = TRUE;

	LPNORM(((PartionInfo *) partinfo)->partion, "..loader service moduler (DLL'er)")
	if (((PartionInfo *) partinfo)->LoadDlls() == TRUE) {
		SetEvent(((PartionInfo *) partinfo)->hReady);
		LPNORM(((PartionInfo *) partinfo)->partion, "..service moduler loadet og initialiseret")
	} else {
		SetEvent(((PartionInfo *) partinfo)->hInitError);
		LPERR(((PartionInfo *) partinfo)->partion, "ERROR:..kunne ikke loade og/eller initialisere service moduler")
		succes = FALSE;
	}
	
	do {
		if (succes) {
			if (WaitForSingleObject(((PartionInfo *) partinfo)->hStopUnloadDlls, 0) == WAIT_OBJECT_0) {
				((PartionInfo *) partinfo)->ReleaseDlls();
				SetEvent(((PartionInfo *) partinfo)->hPartionStopped);
				int cont;
				do {
					cont = FALSE;
					WaitForSingleObject(((PartionInfo *) partinfo)->hStartLoadDlls, INFINITE);
					if (((PartionInfo *) partinfo)->LoadDlls() == TRUE) {
						SetEvent(((PartionInfo *) partinfo)->hReady);
					} else {
						cont = TRUE;
						((PartionInfo *) partinfo)->ReleaseDlls();
						SetEvent(((PartionInfo *) partinfo)->hInitError);
					}
				} while (cont);
			}
			if ((currentjob = ((PartionInfo *) partinfo)->que->StartJob()) != NULL) {
				runningjob[((PartionInfo *) partinfo)->partion] = currentjob;
				char c_argument[5][100], *parm, *argument[5];
				int j = 0, retcode;
				for (int i = 0; i < 5; i++) {
					if ((parm = currentjob->GetJobParameter(i)) != NULL) {
						strcpy(c_argument[j++], currentjob->GetJobParameter(i));
						argument[i] = c_argument[i];
					}
				}
				DllWorker& worker = ((PartionInfo *) partinfo)->
					GetWorker((*((PartionInfo *) partinfo)->job_ptr)->GetManager());
				LPNORM(((PartionInfo *) partinfo)->partion, CString("start jobbet \"") + (*((PartionInfo *) partinfo)->job_ptr)->GetJobCode() + "\" med service modul \"" + worker.GetServiceName() + "\"")
				retcode = worker.Run(j, argument);
				if (retcode == DLLSUCCES) {
					LPNORM(((PartionInfo *) partinfo)->partion, CString("jobbet \"") + (*((PartionInfo *) partinfo)->job_ptr)->GetJobCode() + "\" er afsluttet med succes")
					((PartionInfo *) partinfo)->que->SetReturnCode(QUE_SUCCES);
				} else {
					LPERR(((PartionInfo *) partinfo)->partion, CString("ERROR: jobbet \"") + (*((PartionInfo *) partinfo)->job_ptr)->GetJobCode() + "\" er afsluttet med fejl")
					((PartionInfo *) partinfo)->que->SetReturnCode(QUE_ERROR);
				}
			}
		}
	} while (WaitForSingleObject(((PartionInfo *) partinfo)->hEndPartion, 3000) == WAIT_TIMEOUT);
	((PartionInfo *) partinfo)->ReleaseDlls();
	return(TRUE);
}

void StopPartion(int part)
{
	TCHAR temp_str[100];

	sprintf(temp_str, "..stopper partion %i", part+1);
	LNORM(temp_str)
	SetEvent(partions[part]->hStopUnloadDlls);
	WaitForSingleObject(partions[part]->hPartionStopped, INFINITE);
	sprintf(temp_str, "..partion %i stoppet", part+1);
	LNORM(temp_str)
}

int RestartPartion(int part)
{
	TCHAR temp_str[100];

	SetEvent(partions[part]->hStartLoadDlls);
	HANDLE handlearray[2];
	handlearray[0] = partions[part]->hReady; handlearray[1] = partions[part]->hInitError;
	int retval = WaitForMultipleObjects(2, handlearray, FALSE, INFINITE);
	if (retval-WAIT_OBJECT_0 == 0) {
		sprintf(temp_str, "..partion %i er genstartet og initialiseret", part+1);
		LNORM(temp_str)
		return(TRUE);
	} else {
		sprintf(temp_str, "ERROR: kunne ikke initialisere partion %i", part+1);
		LERR(temp_str)
		return(FALSE);
	}
}

int StartPartion(int part)
{
	TCHAR temp_str[100];

	sprintf(temp_str, "..starter partion %i", part+1);
	LNORM(temp_str)
	if ((partions[part]->hPartThread = AfxBeginThread(&JobPartion, (LPVOID) partions[part])) == NULL) {
		sprintf(temp_str, "ERROR: kunne ikke starte partion %i", part+1);
		LERR(temp_str)
		return(FALSE);
	}
	partions[part]->hPartThread->m_bAutoDelete = FALSE;
	int cont = TRUE, retval;
	HANDLE handlearray[2];
	handlearray[0] = partions[part]->hReady; handlearray[1] = partions[part]->hInitError;
	retval = WaitForMultipleObjects(2, handlearray, FALSE, INFINITE);
	if (retval-WAIT_OBJECT_0 == 0) {
		sprintf(temp_str, "..partion %i er startet og initialiseret", part+1);
		LNORM(temp_str)
		return(TRUE);
	} else {
		sprintf(temp_str, "ERROR: kunne ikke initialisere partion %i", part+1);
		LERR(temp_str)
		return(FALSE);
	}
}

void EndPartion(int part)
{
	TCHAR temp_str[100];
	DWORD ThreadStatus;

	sprintf(temp_str, "..nedlukker partion %i", part+1);
	LNORM(temp_str)
	SetEvent(partions[part]->hEndPartion);
	do {
		GetExitCodeThread(partions[part]->hPartThread->m_hThread, &ThreadStatus);
		Sleep(500);
	} while (ThreadStatus == STILL_ACTIVE);
	delete partions[part]->hPartThread;
	sprintf(temp_str, "..partion %i afsluttet og lukket", part+1);
	LNORM(temp_str)
}