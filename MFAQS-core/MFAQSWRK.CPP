#include "StdAfx.h"

#include <fstream>
#include "mfaqslog.h"
#include "mfaqswrk.h"
using namespace std;

typedef int (CALLBACK* LPFNDLLFUNC10)(int, int, char**);
typedef int (CALLBACK* LPFNDLLFUNC11)(void);
typedef int (CALLBACK* LPFNDLLFUNC12)(void *, void *);



extern "C" void PASCAL EXPORT LogNorm(int part, LPCTSTR txt) {
	LPNORM( part+10, txt )
}

extern "C" void PASCAL EXPORT LogErr(int part, LPCTSTR txt) {
	LPERR( part+10, txt )
}

class DllWorkerPart
{
private:
	int workpart;
	CString DllFile;
	HINSTANCE hDll;
	LPFNDLLFUNC10 StartWork;
	LPFNDLLFUNC12 DllLogReg;
	TCHAR parm1[100];
	TCHAR parm2[100];
	TCHAR parm3[100];
	TCHAR parm4[100];
	TCHAR parm5[100];
	TCHAR *argv[5];
	int argc;
public:
	DllWorkerPart(int part);
	~DllWorkerPart();
	LPFNDLLFUNC11 StopWork;
	int LoadInit(int pargc, TCHAR *pargv[]);
	void Release();
	int Run();
};

DllWorkerPart *workpart[5];


DllWorkerPart::DllWorkerPart(int part)
{
	workpart = part;
}

DllWorkerPart::~DllWorkerPart()
{
	if (hDll != NULL) FreeLibrary(hDll);
}


int DllWorkerPart::LoadInit(int pargc, TCHAR *pargv[])
{
	LPNORM(workpart+10, CString("Loader worker modul"))
	for (int i = 0; i < pargc; i++) {
		if (i == 0) {
			strcpy(parm1, pargv[i]);
			argv[0] = parm1;
		} else if (i == 1) {
			strcpy(parm2, pargv[i]);
			argv[1] = parm2;
		} else if (i == 2) {
			strcpy(parm3, pargv[i]);
			argv[2] = parm3;
		} else if (i == 3) {
			strcpy(parm4, pargv[i]);
			argv[3] = parm4;
		} else {
			strcpy(parm5, pargv[i]);
			argv[4] = parm5;
		}
	}
	argc = pargc;
	if (argc > 0) {
		DllFile = argv[0];
		if ((hDll = AfxLoadLibrary(DllFile)) == NULL) {
			LPERR(workpart+10, CString("ERROR: kunne ikke loade worker modul \"") + DllFile + "\"")
			return(FALSE);
		}
		if (hDll != NULL) {
			LPNORM(workpart+10, CString("initialiserer worker modul \"") + DllFile + "\"")
			StartWork = (LPFNDLLFUNC10)GetProcAddress(hDll, "StartWork");
			StopWork = (LPFNDLLFUNC11)GetProcAddress(hDll, "StopWork");
			DllLogReg = (LPFNDLLFUNC12)GetProcAddress(hDll, "DllLogReg");
		}
		if (hDll == NULL || StartWork == NULL || StopWork == NULL || DllLogReg == NULL) {
			LPERR(workpart+10, CString("ERROR: kunne ikke loade/initialisere work modul \"") + DllFile + "\"")
			return(FALSE);
		} else {
			DllLogReg(&LogNorm, &LogErr);
		}
	}
	return(TRUE);
}

void DllWorkerPart::Release()
{
	if (hDll != NULL) AfxFreeLibrary(hDll);
	hDll = NULL;
}

int DllWorkerPart::Run()
{
	int retval;
	LPNORM(workpart+10, CString("Work modul \"") + DllFile + "\" aktiveres")
	retval = StartWork(workpart, argc, argv);
	return(retval);
}

int StartWorker(int part, int argc, TCHAR *argv[]) {
	TCHAR temp_str[100];

	sprintf(temp_str, "..starter work partion W%i", part+1);
	LNORM(temp_str)

	workpart[part] = new DllWorkerPart(part);
	workpart[part]->LoadInit(argc, argv);
	return (workpart[part]->Run());
}

int StopWorker(int part) {
	workpart[part]->StopWork();
	workpart[part]->Release();
	delete workpart[part];

	return(TRUE);
}
