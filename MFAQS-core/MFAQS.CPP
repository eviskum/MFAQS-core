// DB2 (VM/VSE) --> MS SQL (NT)
// Esben Viskum, AO, april 1999


#include "StdAfx.h"
#include "MFAQS.h"
#include <fstream>
#include <conio.h>
#include "mfaqsjob.h"
#include "mfaqslog.h"
#include "mfaqspar.h"
#include "mfaqswrk.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

// using namespace std;

void CommandParser(LPCTSTR command);

HANDLE end_scheduler_event;
HANDLE insert_scheduler_break;
HANDLE stop_scheduler_event;
HANDLE scheduler_stopped;
HANDLE scheduler_break;
HANDLE hSchedule;

JOB_SCHEDULE *scheduler;
MFAQSLog *logg;
int partion_lock[5];
int conffile = FALSE;
TCHAR configfilename[100];

CFile configfile;


void initPartions()
{
	TCHAR temp_str[100];
	int i;

	LNORM("..opretter partions-information")
	for (i = 0; i < 5; i++) {
		sprintf_s(temp_str, "..opretter partionsdata for partion %i", i+1);
		LNORM(temp_str)
		partions[i] = new PartionInfo(scheduler->GetJobQue(i), &runningjob[i], i);
	}
	LNORM("..partionsdata oprettet")
}

int StartupPartions()
{
	int i, retval = TRUE;

	for (i = 0; i < 5; i++) partion_lock[i] = FALSE;
	for (i = 0; i < 5; i++) {
		if (StartPartion(i) == FALSE) {
			retval = FALSE;
			break;
		}
	}
	if (retval == FALSE) {
		for (int j = 0; j <= i; j++) {
			EndPartion(j);
		}
	} else {
/*		TCHAR var10[] = "DLL\\WORKRUN\\DEBUG\\WORKRUN.DLL";
		TCHAR var11[] = "0";
		TCHAR var12[] = "-DUDVRFC";
//		TCHAR var10[] = "DLL\\SAPINTF1.DLL";
//		TCHAR var11[] = "-DUDVRFC";
		TCHAR var20[] = "DLL\\SAPINTF2.DLL";
		TCHAR var21[] = "-DTPLRFC";
		TCHAR var30[] = "DLL\\SAPINTF3.DLL";
		TCHAR var31[] = "-DPRDRFC";
		for (i = 0; i < 1; i++) {
			TCHAR *argv[2];
			switch (i) {
			case 0:
				argv[0] = var10;
				argv[1] = var11;
				argv[2] = var12;
				break;
			case 1:
				argv[0] = var20;
				argv[1] = var21;
				break;
			case 2:
				argv[0] = var30;
				argv[1] = var31;
				break;
			}
			if (StartWorker(i, 2, argv) == FALSE) {
				retval = FALSE;
				break;
			}
		}
		if (retval == FALSE) {
			for (int j = 0; j <= i; j++) {
				StopWorker(j);
			}
		}
*/	}
	return(retval);
}


JOB_SCHEDULE *initMFAQS( int argc, char *argv[] )
{
	LNORM("..starter job scheduler");
	scheduler = new JOB_SCHEDULE;
	initPartions();
	if (argc == 2) {
		LNORM("..loader jobs");
		configfile.Open(argv[1], CFile::modeRead);
		CArchive ar(&configfile, CArchive::load);
		scheduler->Serialize( ar );
		for (int i = 0; i < 5; i++) partions[i]->Serialize( ar );
		ar.Close();
		configfile.Close();
		strcpy_s(configfilename, argv[1]);
		conffile = TRUE;
	}
	return (scheduler);
}




void CreateSchedulerEvents()
{
	SECURITY_ATTRIBUTES signalatt;
	signalatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	signalatt.lpSecurityDescriptor = NULL;
	signalatt.bInheritHandle = TRUE;
	end_scheduler_event = CreateEvent(&signalatt, TRUE, FALSE, NULL);
	insert_scheduler_break = CreateEvent(&signalatt, TRUE, FALSE, NULL);
	stop_scheduler_event = CreateEvent(&signalatt, TRUE, FALSE, NULL);
	scheduler_stopped = CreateEvent(&signalatt, TRUE, FALSE, NULL);
	scheduler_break = CreateEvent(&signalatt, TRUE, FALSE, NULL);
	hSchedule = CreateMutex(&signalatt, FALSE, NULL);
}

UINT ScheduleLoop( LPVOID )
{
	int break_inserted = FALSE;
	int schedulercount = 0;

	LNORM("..schedulering aktiveret")

	do {
		WaitForSingleObject(hSchedule, INFINITE);
		if (WaitForSingleObject(insert_scheduler_break, 0) == WAIT_OBJECT_0) {
			if (break_inserted == FALSE) {
				break_inserted = TRUE;
				LNORM("..schedule break indsat -- afvikler igangvaerende jobs")
				scheduler->InsertServiceBreak();
			}
		} else if (break_inserted == TRUE) break_inserted = FALSE;
		if (break_inserted) {
			LNORM("..schedule update -- afvikler igangvaerende jobs");
			if (scheduler->StatusUpdate(SERVICE_BREAK_INSERTED) == TRUE) {
				LNORM("..alle jobs foer break er afviklet")
				SetEvent(scheduler_break);
			}
		} else if (WaitForSingleObject(stop_scheduler_event, 0) == WAIT_OBJECT_0) {
			LNORM("..schedule update -- afvikler alle jobs i jobkoerne")
			if (scheduler->StatusUpdate(SERVICE_STOP_INSERTED) == TRUE) {
				LNORM("..alle jobs i koerne afviklet")
				SetEvent(scheduler_stopped);
			}
		} else {
			if (schedulercount++ == 60) {
				LNORM("..scheduler koerer")
				schedulercount = 0;
			}
			scheduler->StatusUpdate(SERVICE_NORMAL);
		}
		ReleaseMutex(hSchedule);
	} while (WaitForSingleObject(end_scheduler_event, 10000) == WAIT_TIMEOUT);

	LNORM("..schedulering stoppet")
	return(1);
}


int mfaqsmain( int argc, char *argv[] )
{
	int cont = TRUE;
	TCHAR command[500];

	logg = new MFAQSLog;

	LNORM("MiniFAQS starter.....");

	CreateSchedulerEvents();
	scheduler = initMFAQS(argc, argv);
	if (StartupPartions() == TRUE) {
		CWinThread *hSchedule = AfxBeginThread(&ScheduleLoop, NULL);
		hSchedule->m_bAutoDelete = FALSE;

		do {
			if (_kbhit()) {
				if (_getch() == 'S') {
					LNORM("MiniFAQS i service mode");
					int servicemode = TRUE;
					logg->DisableConsole();
					cout << endl << "Velkommen til MFAQS service mode......" << endl;
					do {
						cout << " MFAQS:>" << flush;
						gets_s(command);
						if (strcmp(command, "RUN") == 0) {
							int locked = FALSE;
							for (int i = 0; i < 5; i++) if (partion_lock[i]) {
								cout << " FEJL: partionen (" << i+1 << ") er stadig locked" << endl << flush;
								locked = TRUE;
							}
							if (locked == FALSE) servicemode = FALSE;
						} else if (strcmp(command, "QUIT") == 0) {
							cout << "nedlukker MiniFAQS - vent !!!!" << endl << flush;
							logg->EnableConsole();
							SetEvent(insert_scheduler_break);
							WaitForSingleObject(scheduler_break, INFINITE);
							SetEvent(end_scheduler_event);
							DWORD ThreadStatus;
							do {
								GetExitCodeThread(hSchedule->m_hThread, &ThreadStatus);
								Sleep(500);
							} while (ThreadStatus == STILL_ACTIVE);
							delete hSchedule;
							for (int i = 0; i < 5; i++) {
								EndPartion(i);
								delete partions[i];
							}
							delete scheduler;
							LNORM("..scheduler nedlukket");
							servicemode = FALSE;
							cont = FALSE;
						} else {
							CommandParser(command);
						}
					} while (servicemode);
					logg->EnableConsole();
					LNORM("MiniFAQS slutter service mode")
				}
			}
			Sleep(1000);
		} while (cont == TRUE);
	}

	delete logg;

	return(0);
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		mfaqsmain(argc, argv);
	}

	return nRetCode;
}
