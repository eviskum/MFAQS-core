#include "StdAfx.h"


// #include <afx.h>
// #include <afxtempl.h>
#include <fstream>
#include "mfaqspar.h"
#include "mfaqsjob.h"
using namespace std;


CArray<CString,CString> cmd_lex;
extern int partion_lock[5];
extern JOB_SCHEDULE *scheduler;
extern HANDLE hSchedule;
extern int conffile;
extern TCHAR configfilename[100];
extern CFile configfile;
int partionslocked = 0;


void CommandLexer(LPCTSTR command)
{
	CString cmd(command);
	cmd_lex.RemoveAll();
	int start_idx = 0, end_idx = 0, length = cmd.GetLength();
	while (end_idx < length) {
		while (start_idx < length && cmd[start_idx] == ' ') start_idx++;
		end_idx = start_idx;
		while (end_idx < length && cmd[end_idx] != ' ') end_idx++;
		if (start_idx < length) cmd_lex.Add(cmd.Mid(start_idx, end_idx-start_idx));
		start_idx = end_idx;
	}
}

void NewModule(int part)
{
	TCHAR str_modulename[100], str_filename[100];
	cout << endl << " Ny service modul i partion (" << part << ")" << endl <<
					" ----------------------------------" << endl;
	if (partion_lock[part-1] == FALSE) {
		cout << " FEJL: partionen er ikke laast" << endl << flush;
	} else {
		cout << "  Indtast modulnavn (8 karakterer) >" << flush;
		gets_s(str_modulename);
		CString modulename(str_modulename);
		if (modulename.GetLength() == 8) {
			cout << "  Indtast modul-filnavn (dll) >" << flush;
			gets_s(str_filename);
			CString filename(str_filename);
			DLLWORKERS *workerlist = partions[part-1]->GetWorkerList();
			int found = FALSE;
			for (int i = 0; i < workerlist->no_workers; i++) {
				if (workerlist->servicename[i] == modulename) found = TRUE;
			}
			delete workerlist;
			if (found) {
				cout << " FEJL: modulnavnet findes allerede i denne partion" << endl << flush;
			} else {
				scheduler->InsertManager(modulename, part-1);
				partions[part-1]->InsertWorker(filename, modulename, part-1);
			}
		} else {
			cout << " FEJL: modulnavnet skal være 8 karakterer langt" << endl << flush;
		}
	}
}

void RemoveModule(int part, CString& module)
{
	cout << endl << " Fjernelse af service modul i partion (" << part << ")" << endl <<
					"---------------------------------------------" << endl;
	cout <<			"  Service modulet \"" << module << "\" fjernes" << endl << flush;
	if (partion_lock[part-1] == FALSE) {
		cout << " FEJL: partionen er ikke laast" << endl << flush;
	} else {
		DLLWORKERS *workerlist = partions[part-1]->GetWorkerList();
		int found = FALSE;
		for (int i = 0; i < workerlist->no_workers; i++) {
			if (workerlist->servicename[i] == module) found = TRUE;
		}
		delete workerlist;
		if (found) {
			TCHAR ans[100];
			cout << "  er det ok, at modulet fjernes (J/N) >" << flush;
			gets_s(ans);
			CString answer(ans);
			if (answer == "J" || answer == "JA") {
				partions[part-1]->DeleteWorker(module);
				scheduler->RemoveManager(module, part-1);
			} else {
				cout << "  Modulet slettes ikke !!!! " << endl << flush;
			}
		} else {
			cout << " FEJL: modulnavnet findes ikke i denne partion" << endl << flush;
		}
	}
}

int val_eventjob(CString& pattern)
{
	if (pattern.GetLength() > 0) return(TRUE);
	else return(FALSE);
}

int val_timeeventjob(CString& pattern)
{
	if (pattern.GetLength() > 0) return(TRUE);
	else return(FALSE);
}

int val_time(CString& starttime, CString& warningtime, CString& timeouttime, CString& resettime,
			 CString& timetype, CString& weekdays, CString& monthdays, CString& dependencyjobs)
{
	CommandLexer(starttime);
	if (cmd_lex.GetSize() == 5) {
		if (atoi(cmd_lex[0]) < 1999 || atoi(cmd_lex[0]) > 2050 ||
			atoi(cmd_lex[1]) < 1 || atoi(cmd_lex[1]) > 12 ||
			atoi(cmd_lex[2]) < 1 || atoi(cmd_lex[2]) > 31 ||
			atoi(cmd_lex[3]) < 0 || atoi(cmd_lex[3]) > 23 ||
			atoi(cmd_lex[4]) < 0 || atoi(cmd_lex[4]) > 59) {
			cout << " FEJL: forkert starttidspunkt " << endl << flush;
			return(FALSE);
		} else {
			switch (atoi(cmd_lex[1])) {
			case 2:
				if ((atoi(cmd_lex[0]) % 4 == 0 && atoi(cmd_lex[2]) > 29) ||
					(atoi(cmd_lex[0]) % 4 != 0 && atoi(cmd_lex[2]) > 28)) {
					cout << " FEJL: forkert starttidspunkt " << endl << flush;
					return(FALSE);
				}
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				if (atoi(cmd_lex[2]) > 30) {
					cout << " FEJL: forkert starttidspunkt " << endl << flush;
					return(FALSE);
				}
				break;
			default:
				break;
			}
		}
	} else {
		cout << " FEJL: forkert starttidspunkt " << endl << flush;
		return(FALSE);
	}
	CommandLexer(warningtime);
	if (cmd_lex.GetSize() == 2) {
		if (atoi(cmd_lex[0]) < 0 || atoi(cmd_lex[0]) > 23 || atoi(cmd_lex[1]) < 0 || atoi(cmd_lex[1]) > 59) {
			cout << " FEJL: forkert warning-tidspunkt " << endl << flush;
			return(FALSE);
		}
	} else {
		cout << " FEJL: forkert warning-tidspunkt " << endl << flush;
		return(FALSE);
	}
	CTimeSpan warningspan(0, atoi(cmd_lex[0]), atoi(cmd_lex[1]), 0);
	CommandLexer(timeouttime);
	if (cmd_lex.GetSize() == 2) {
		if (atoi(cmd_lex[0]) < 0 || atoi(cmd_lex[0]) > 23 || atoi(cmd_lex[1]) < 0 || atoi(cmd_lex[1]) > 59) {
			cout << " FEJL: forkert timeout-tidspunkt " << endl << flush;
			return(FALSE);
		}
	} else {
		cout << " FEJL: forkert timeout-tidspunkt " << endl << flush;
		return(FALSE);
	}
	CTimeSpan timeoutspan(0, atoi(cmd_lex[0]), atoi(cmd_lex[1]), 0);
	CommandLexer(resettime);
	if (cmd_lex.GetSize() == 2) {
		if (atoi(cmd_lex[0]) < 0 || atoi(cmd_lex[0]) > 23 || atoi(cmd_lex[1]) < 0 || atoi(cmd_lex[1]) > 59) {
			cout << " FEJL: forkert reset-tidspunkt " << endl << flush;
			return(FALSE);
		}
	} else {
		cout << " FEJL: forkert reset-tidspunkt " << endl << flush;
		return(FALSE);
	}
	CTimeSpan resetspan(0, atoi(cmd_lex[0]), atoi(cmd_lex[1]), 0);
	if (warningspan >= timeoutspan || timeoutspan >= resetspan) {
		cout << " FEJL: der skal gaelde foelgende: warning-tid < timeout-tid < reset-tid !!!" << endl << flush;
		return(FALSE);
	}
	if (timetype == "ONCE") {
	} else if (timetype == "DAILY") {
		CommandLexer(weekdays);
		int check_ok = TRUE;
		if (cmd_lex.GetSize() < 1 || cmd_lex.GetSize() > 7) check_ok = FALSE;
		for (int i = 0; i < cmd_lex.GetSize(); i++) {
			if (atoi(cmd_lex[0]) < 1 || atoi(cmd_lex[0]) > 7) check_ok = FALSE;
			if (i > 0 && atoi(cmd_lex[i]) <= atoi(cmd_lex[i-1])) check_ok = FALSE;
		}
		if (check_ok == FALSE) {
			cout << " FEJL: forkert ugedage-indtastning !!!!" << endl << flush;
			return(FALSE);
		}
	} else if (timetype == "MONTHLY") {
		CommandLexer(monthdays);
		int check_ok = TRUE;
		if (cmd_lex.GetSize() < 1 || cmd_lex.GetSize() > 31) check_ok = FALSE;
		for (int i = 0; i < cmd_lex.GetSize(); i++) {
			if (atoi(cmd_lex[0]) < 1 || atoi(cmd_lex[0]) > 31) check_ok = FALSE;
			if (i > 0 && atoi(cmd_lex[i]) <= atoi(cmd_lex[i-1])) check_ok = FALSE;
		}
		if (check_ok == FALSE) {
			cout << " FEJL: forkert maanedsdage-indtastning !!!!" << endl << flush;
			return(FALSE);
		}
	} else {
		cout << " FEJL: forkert tids-type indtastning !!!!" << endl << flush;
		return(FALSE);
	}
	CommandLexer(dependencyjobs);
	if (cmd_lex.GetSize() > 10) {
		cout << " FEJL: for mange afhaengige jobs (max 10) !!!!" << endl << flush;
		return(FALSE);
	}
	int check_ok = TRUE;
	for (int i = 0; i < cmd_lex.GetSize(); i++) {
		if (cmd_lex[i].GetLength() != 8) check_ok = FALSE;
	}
	if (check_ok == FALSE) {
		cout << " FEJL: job-koderne skal vaere 8 karaktere lange !!!!" << endl << flush;
		return(FALSE);
	}
	return(TRUE);
}

int val_follow(CString& followjobs)
{
	CommandLexer(followjobs);
	if (cmd_lex.GetSize() > 10) {
		cout << " FEJL: for mange efterfoelger-jobs (max 10) !!!!" << endl << flush;
		return(FALSE);
	}
	int check_ok = TRUE;
	for (int i = 0; i < cmd_lex.GetSize(); i++) {
		if (cmd_lex[i].GetLength() != 8) check_ok = FALSE;
	}
	if (check_ok == FALSE) {
		cout << " FEJL: job-koderne skal vaere 8 karaktere lange !!!!" << endl << flush;
		return(FALSE);
	}
	return(TRUE);
}

void CreateManuelJob(CString& job_name, CString& comment, CString& manager, CString& parametre, CString& followjobs)
{
	scheduler->LockJobs();
	if (scheduler->FindJob(job_name) < 0) {
		MFAQSJOB *newjob = new MFAQSJOB(scheduler, job_name, comment);
		LPCTSTR parm1 = NULL, parm2 = NULL, parm3 = NULL, parm4 = NULL, parm5 = NULL;
		CommandLexer(parametre);
		if (cmd_lex.GetSize() > 0) parm1 = cmd_lex[0];
		if (cmd_lex.GetSize() > 1) parm2 = cmd_lex[1];
		if (cmd_lex.GetSize() > 2) parm3 = cmd_lex[2];
		if (cmd_lex.GetSize() > 3) parm4 = cmd_lex[3];
		if (cmd_lex.GetSize() > 4) parm5 = cmd_lex[4];
		newjob->SetJobType(MANUALJOB, manager, parm1, parm2, parm3, parm4, parm5);
		{
			LPCTSTR job1 = NULL, job2 = NULL, job3 = NULL, job4 = NULL, job5 = NULL,
				job6 = NULL, job7 = NULL, job8 = NULL, job9 = NULL, job10 = NULL;
			CommandLexer(followjobs);
			if (cmd_lex.GetSize() > 0) job1 = cmd_lex[0];
			if (cmd_lex.GetSize() > 1) job2 = cmd_lex[1];
			if (cmd_lex.GetSize() > 2) job3 = cmd_lex[2];
			if (cmd_lex.GetSize() > 3) job4 = cmd_lex[3];
			if (cmd_lex.GetSize() > 4) job5 = cmd_lex[4];
			if (cmd_lex.GetSize() > 5) job6 = cmd_lex[5];
			if (cmd_lex.GetSize() > 6) job7 = cmd_lex[6];
			if (cmd_lex.GetSize() > 7) job8 = cmd_lex[7];
			if (cmd_lex.GetSize() > 8) job9 = cmd_lex[8];
			if (cmd_lex.GetSize() > 9) job10 = cmd_lex[9];
			newjob->SetFollowjob(job1, job2, job3, job4, job5, job6, job7, job8, job9, job10);
		}
		scheduler->InsertJob(newjob);
		LNORM(CString("Nyt job \"") + job_name + "\" er oprettet")
	} else {
		cout << " FEJL: jobbet findes i forvejen !!!!" << endl << flush;
	}
	scheduler->UnlockJobs();
}

void CreateEventJob(CString& job_name, CString& comment, CString& manager, CString& parametre,
					CString& pattern, CString& maxtime, CString& followjobs)
{
	scheduler->LockJobs();
	if (scheduler->FindJob(job_name) < 0) {
		MFAQSJOB *newjob = new MFAQSJOB(scheduler, job_name, comment);
		LPCTSTR parm1 = NULL, parm2 = NULL, parm3 = NULL, parm4 = NULL, parm5 = NULL;
		CommandLexer(parametre);
		if (cmd_lex.GetSize() > 0) parm1 = cmd_lex[0];
		if (cmd_lex.GetSize() > 1) parm2 = cmd_lex[1];
		if (cmd_lex.GetSize() > 2) parm3 = cmd_lex[2];
		if (cmd_lex.GetSize() > 3) parm4 = cmd_lex[3];
		if (cmd_lex.GetSize() > 4) parm5 = cmd_lex[4];
		newjob->SetJobType(EVENTJOB, manager, parm1, parm2, parm3, parm4, parm5);
		CommandLexer(maxtime);
		newjob->SetFilePattern(pattern, atoi(cmd_lex[0]), atoi(cmd_lex[1]), atoi(cmd_lex[2]));
		{
			LPCTSTR job1 = NULL, job2 = NULL, job3 = NULL, job4 = NULL, job5 = NULL,
				job6 = NULL, job7 = NULL, job8 = NULL, job9 = NULL, job10 = NULL;
			CommandLexer(followjobs);
			if (cmd_lex.GetSize() > 0) job1 = cmd_lex[0];
			if (cmd_lex.GetSize() > 1) job2 = cmd_lex[1];
			if (cmd_lex.GetSize() > 2) job3 = cmd_lex[2];
			if (cmd_lex.GetSize() > 3) job4 = cmd_lex[3];
			if (cmd_lex.GetSize() > 4) job5 = cmd_lex[4];
			if (cmd_lex.GetSize() > 5) job6 = cmd_lex[5];
			if (cmd_lex.GetSize() > 6) job7 = cmd_lex[6];
			if (cmd_lex.GetSize() > 7) job8 = cmd_lex[7];
			if (cmd_lex.GetSize() > 8) job9 = cmd_lex[8];
			if (cmd_lex.GetSize() > 9) job10 = cmd_lex[9];
			newjob->SetFollowjob(job1, job2, job3, job4, job5, job6, job7, job8, job9, job10);
		}
		scheduler->InsertJob(newjob);
		LNORM(CString("Nyt job \"") + job_name + "\" er oprettet")
	} else {
		cout << " FEJL: jobbet findes i forvejen !!!!" << endl << flush;
	}
	scheduler->UnlockJobs();
}

void CreateTimeEventJob(CString& job_name, CString& comment, CString& manager, CString& parametre,
						CString& starttime, CString& warningtime, CString& timeouttime,
						CString& resettime, CString& timetype, CString& weekdays, CString& monthdays,
						CString& dependencyjobs, CString& pattern, CString& followjobs)
{
	scheduler->LockJobs();
	if (scheduler->FindJob(job_name) < 0) {
		MFAQSJOB *newjob = new MFAQSJOB(scheduler, job_name, comment);
		LPCTSTR parm1 = NULL, parm2 = NULL, parm3 = NULL, parm4 = NULL, parm5 = NULL;
		CommandLexer(parametre);
		if (cmd_lex.GetSize() > 0) parm1 = cmd_lex[0];
		if (cmd_lex.GetSize() > 1) parm2 = cmd_lex[1];
		if (cmd_lex.GetSize() > 2) parm3 = cmd_lex[2];
		if (cmd_lex.GetSize() > 3) parm4 = cmd_lex[3];
		if (cmd_lex.GetSize() > 4) parm5 = cmd_lex[4];
		newjob->SetJobType(TIMEEVENTJOB, manager, parm1, parm2, parm3, parm4, parm5);
		newjob->SetFilePattern(pattern);
		{
			CommandLexer(starttime);
			int start_y = atoi(cmd_lex[0]), start_m = atoi(cmd_lex[1]), start_d = atoi(cmd_lex[2]),
				start_h = atoi(cmd_lex[3]), start_min = atoi(cmd_lex[4]);
			CommandLexer(warningtime);
			int warning_h = atoi(cmd_lex[0]), warning_m = atoi(cmd_lex[1]);
			CommandLexer(timeouttime);
			int timeout_h = atoi(cmd_lex[0]), timeout_m = atoi(cmd_lex[1]);
			CommandLexer(resettime);
			int reset_h = atoi(cmd_lex[0]), reset_m = atoi(cmd_lex[1]);
			newjob->SetTimeInfo(start_y, start_m, start_d, start_h, start_min, warning_h, warning_m,
				timeout_h, timeout_m, reset_h, reset_m);
			LPCTSTR job1 = NULL, job2 = NULL, job3 = NULL, job4 = NULL, job5 = NULL,
				job6 = NULL, job7 = NULL, job8 = NULL, job9 = NULL, job10 = NULL;
			CommandLexer(dependencyjobs);
			if (cmd_lex.GetSize() > 0) job1 = cmd_lex[0];
			if (cmd_lex.GetSize() > 1) job2 = cmd_lex[1];
			if (cmd_lex.GetSize() > 2) job3 = cmd_lex[2];
			if (cmd_lex.GetSize() > 3) job4 = cmd_lex[3];
			if (cmd_lex.GetSize() > 4) job5 = cmd_lex[4];
			if (cmd_lex.GetSize() > 5) job6 = cmd_lex[5];
			if (cmd_lex.GetSize() > 6) job7 = cmd_lex[6];
			if (cmd_lex.GetSize() > 7) job8 = cmd_lex[7];
			if (cmd_lex.GetSize() > 8) job9 = cmd_lex[8];
			if (cmd_lex.GetSize() > 9) job10 = cmd_lex[9];
			newjob->SetDependency(job1, job2, job3, job4, job5, job6, job7, job8, job9, job10);
			{
				LPCTSTR job1 = NULL, job2 = NULL, job3 = NULL, job4 = NULL, job5 = NULL,
					job6 = NULL, job7 = NULL, job8 = NULL, job9 = NULL, job10 = NULL;
				CommandLexer(followjobs);
				if (cmd_lex.GetSize() > 0) job1 = cmd_lex[0];
				if (cmd_lex.GetSize() > 1) job2 = cmd_lex[1];
				if (cmd_lex.GetSize() > 2) job3 = cmd_lex[2];
				if (cmd_lex.GetSize() > 3) job4 = cmd_lex[3];
				if (cmd_lex.GetSize() > 4) job5 = cmd_lex[4];
				if (cmd_lex.GetSize() > 5) job6 = cmd_lex[5];
				if (cmd_lex.GetSize() > 6) job7 = cmd_lex[6];
				if (cmd_lex.GetSize() > 7) job8 = cmd_lex[7];
				if (cmd_lex.GetSize() > 8) job9 = cmd_lex[8];
				if (cmd_lex.GetSize() > 9) job10 = cmd_lex[9];
				newjob->SetFollowjob(job1, job2, job3, job4, job5, job6, job7, job8, job9, job10);
			}
			int freqtype = (timetype == "ONCE" ? RUNONCE : (timetype == "DAILY" ? RUNDAILY : RUNMONTHLY));
			int no_days = 0, days[366];
			if (freqtype == RUNDAILY) {
				CommandLexer(weekdays);
				for (int i = 0; i < cmd_lex.GetSize(); i++) days[i] = atoi(cmd_lex[i]);
				no_days = cmd_lex.GetSize();
			}
			if (freqtype == RUNMONTHLY) {
				CommandLexer(monthdays);
				for (int i = 0; i < cmd_lex.GetSize(); i++) days[i] = atoi(cmd_lex[i]);
				no_days = cmd_lex.GetSize();
			}
			newjob->SetFrequence(freqtype, no_days, days);
		}
		scheduler->InsertJob(newjob);
		LNORM(CString("Nyt job \"") + job_name + "\" er oprettet")
	} else {
		cout << " FEJL: jobbet findes i forvejen !!!!" << endl << flush;
	}
	scheduler->UnlockJobs();
}

void CreateTimeJob(CString& job_name, CString& comment, CString& manager, CString& parametre,
						CString& starttime, CString& warningtime, CString& timeouttime,
						CString& resettime, CString& timetype, CString& weekdays, CString& monthdays,
						CString& dependencyjobs, CString& followjobs)
{
	scheduler->LockJobs();
	if (scheduler->FindJob(job_name) < 0) {
		MFAQSJOB *newjob = new MFAQSJOB(scheduler, job_name, comment);
		LPCTSTR parm1 = NULL, parm2 = NULL, parm3 = NULL, parm4 = NULL, parm5 = NULL;
		CommandLexer(parametre);
		if (cmd_lex.GetSize() > 0) parm1 = cmd_lex[0];
		if (cmd_lex.GetSize() > 1) parm2 = cmd_lex[1];
		if (cmd_lex.GetSize() > 2) parm3 = cmd_lex[2];
		if (cmd_lex.GetSize() > 3) parm4 = cmd_lex[3];
		if (cmd_lex.GetSize() > 4) parm5 = cmd_lex[4];
		newjob->SetJobType(TIMEJOB, manager, parm1, parm2, parm3, parm4, parm5);
		{
			CommandLexer(starttime);
			int start_y = atoi(cmd_lex[0]), start_m = atoi(cmd_lex[1]), start_d = atoi(cmd_lex[2]),
				start_h = atoi(cmd_lex[3]), start_min = atoi(cmd_lex[4]);
			CommandLexer(warningtime);
			int warning_h = atoi(cmd_lex[0]), warning_m = atoi(cmd_lex[1]);
			CommandLexer(timeouttime);
			int timeout_h = atoi(cmd_lex[0]), timeout_m = atoi(cmd_lex[1]);
			CommandLexer(resettime);
			int reset_h = atoi(cmd_lex[0]), reset_m = atoi(cmd_lex[1]);
			newjob->SetTimeInfo(start_y, start_m, start_d, start_h, start_min, warning_h, warning_m,
				timeout_h, timeout_m, reset_h, reset_m);
			LPCTSTR job1 = NULL, job2 = NULL, job3 = NULL, job4 = NULL, job5 = NULL,
				job6 = NULL, job7 = NULL, job8 = NULL, job9 = NULL, job10 = NULL;
			CommandLexer(dependencyjobs);
			if (cmd_lex.GetSize() > 0) job1 = cmd_lex[0];
			if (cmd_lex.GetSize() > 1) job2 = cmd_lex[1];
			if (cmd_lex.GetSize() > 2) job3 = cmd_lex[2];
			if (cmd_lex.GetSize() > 3) job4 = cmd_lex[3];
			if (cmd_lex.GetSize() > 4) job5 = cmd_lex[4];
			if (cmd_lex.GetSize() > 5) job6 = cmd_lex[5];
			if (cmd_lex.GetSize() > 6) job7 = cmd_lex[6];
			if (cmd_lex.GetSize() > 7) job8 = cmd_lex[7];
			if (cmd_lex.GetSize() > 8) job9 = cmd_lex[8];
			if (cmd_lex.GetSize() > 9) job10 = cmd_lex[9];
			newjob->SetDependency(job1, job2, job3, job4, job5, job6, job7, job8, job9, job10);
			{
				LPCTSTR job1 = NULL, job2 = NULL, job3 = NULL, job4 = NULL, job5 = NULL,
					job6 = NULL, job7 = NULL, job8 = NULL, job9 = NULL, job10 = NULL;
				CommandLexer(followjobs);
				if (cmd_lex.GetSize() > 0) job1 = cmd_lex[0];
				if (cmd_lex.GetSize() > 1) job2 = cmd_lex[1];
				if (cmd_lex.GetSize() > 2) job3 = cmd_lex[2];
				if (cmd_lex.GetSize() > 3) job4 = cmd_lex[3];
				if (cmd_lex.GetSize() > 4) job5 = cmd_lex[4];
				if (cmd_lex.GetSize() > 5) job6 = cmd_lex[5];
				if (cmd_lex.GetSize() > 6) job7 = cmd_lex[6];
				if (cmd_lex.GetSize() > 7) job8 = cmd_lex[7];
				if (cmd_lex.GetSize() > 8) job9 = cmd_lex[8];
				if (cmd_lex.GetSize() > 9) job10 = cmd_lex[9];
				newjob->SetFollowjob(job1, job2, job3, job4, job5, job6, job7, job8, job9, job10);
			}
			int freqtype = (timetype == "ONCE" ? RUNONCE : (timetype == "DAILY" ? RUNDAILY : RUNMONTHLY));
			int no_days = 0, days[366];
			if (freqtype == RUNDAILY) {
				CommandLexer(weekdays);
				for (int i = 0; i < cmd_lex.GetSize(); i++) days[i] = atoi(cmd_lex[i]);
				no_days = cmd_lex.GetSize();
			}
			if (freqtype == RUNMONTHLY) {
				CommandLexer(monthdays);
				for (int i = 0; i < cmd_lex.GetSize(); i++) days[i] = atoi(cmd_lex[i]);
				no_days = cmd_lex.GetSize();
			}
			newjob->SetFrequence(freqtype, no_days, days);
		}
		scheduler->InsertJob(newjob);
		LNORM(CString("Nyt job \"") + job_name + "\" er oprettet")
	} else {
		cout << " FEJL: jobbet findes i forvejen !!!!" << endl << flush;
	}
	scheduler->UnlockJobs();
}

void NewJob()
{
	TCHAR temp_str[500];
	CString job_name, comment, manager, parms, job_type;
	cout << endl << " Nyt MFAQS-job" << endl <<
					" ----------------------------------" << endl;
	cout << "  Indtast job-navn (8 karakterer) >" << flush;
	gets_s(temp_str); job_name = temp_str;
	if (job_name.GetLength() == 8) {
		cout << "  Indtast job-kommentar (max 50 tegn) >" << flush;
		gets_s(temp_str); comment = temp_str;
		cout << "  Indtast service-modul >" << flush;
		gets_s(temp_str); manager = temp_str;
		if (manager.GetLength() == 8) {
			cout << "  Indtast parametre >" << flush;
			gets_s(temp_str); parms = temp_str;
			cout << "  Indtast job-type (MANUEL/TIME/EVENT/TIMEEVENT) >" << flush;
			gets_s(temp_str); job_type = temp_str;
			cout << "  Indtast efterfoelger jobs >" << flush;
			gets_s(temp_str); CString followjobs(temp_str);
			if (val_follow(followjobs)) {
				if (job_type == "MANUEL") {
					CreateManuelJob(job_name, comment, manager, parms, followjobs);
				} else if (job_type == "EVENT" || job_type == "TIME" || job_type == "TIMEEVENT") {
					CString pattern, starttime, warningtime, timeouttime, resettime, timetype,
						weekdays, monthdays, dependencyjobs, maxtime;
					if (job_type == "EVENT" || job_type == "TIMEEVENT") {
						cout << "  Indtast trigger fil-pattern >" << flush;
						gets_s(temp_str); pattern = temp_str;
					}
					if (job_type == "TIMEEVENT" || job_type == "TIME") {
						cout << "  Indtast start tidspunkt (tid/dato \"YYYY MM DD hh mm\") >" << flush;
						gets_s(temp_str); starttime = temp_str;
						cout << "  Indtast warning tid (\"hh mm\") >" << flush;
						gets_s(temp_str); warningtime = temp_str;
						cout << "  Indtast timeout tid (\"hh mm\") >" << flush;
						gets_s(temp_str); timeouttime = temp_str;
						cout << "  Indtast reset tid   (\"hh mm\") >" << flush;
						gets_s(temp_str); resettime = temp_str;
						cout << "  Indtast tids-type (ONCE/DAILY/MONTHLY) >" << flush;
						gets_s(temp_str); timetype = temp_str;
						if (timetype == "DAILY") {
							cout << "  Indtast ugedagene (adskildt af mellemrum, mandag = 1) >" << flush;
							gets_s(temp_str); weekdays = temp_str;
						} else if (timetype == "MONTHLY") {
							cout << "  Indtast maanedsdatoer (adskildt af mellemrum) >" << flush;
							gets_s(temp_str); monthdays = temp_str;
						}
					}
					if (job_type == "EVENT") {
						cout << "  Indtast max. tid mellem koersler (\"ddd hh mm\") >" << flush;
						gets_s(temp_str); maxtime = temp_str;
					}
					if (job_type == "TIMEEVENT" || job_type == "TIME") {
						cout << "  Indtast jobafhaengigheder >" << flush;
						gets_s(temp_str); dependencyjobs = temp_str;
					}
					if (job_type == "EVENT") {
						if (val_eventjob(pattern)) CreateEventJob(job_name, comment, manager, parms, pattern, maxtime, followjobs);
					} else if (job_type == "TIMEEVENT") {
						if (val_time(starttime, warningtime, timeouttime, resettime,
							timetype, weekdays, monthdays, dependencyjobs) && val_timeeventjob(pattern))
							CreateTimeEventJob(job_name, comment, manager, parms,
							starttime, warningtime, timeouttime, resettime, timetype,
							weekdays, monthdays, dependencyjobs, pattern, followjobs);
					} else {
						if (val_time(starttime, warningtime, timeouttime, resettime,
							timetype, weekdays, monthdays, dependencyjobs))
							CreateTimeJob(job_name, comment, manager, parms,
							starttime, warningtime, timeouttime, resettime, timetype,
							weekdays, monthdays, dependencyjobs, followjobs);
					}
				} else {
					cout << " FEJL: forkert indtastning" << endl << flush;
				}
			} else {
				cout << " FEJL: efterfoelger jobs forkert indtastet" << endl << flush;
			}
		} else {
			cout << " FEJL: modul-navnet skal vaere 8 karakterer langt" << endl << flush;
		}
	} else {
		cout << " FEJL: job-navnet skal vaere 8 karakterer langt" << endl << flush;
	}
}

void RemoveJob(CString& job)
{
	cout << "Sletter jobs: " << job << " ..." << endl;
	int jobidx;
	if ((jobidx = scheduler->FindJob(job)) < 0) {
		cout << "FEJL: jobbet findes ikke" << endl << flush;
	} else {
		scheduler->DeleteJob(jobidx);
	}
}

void ChangeModule(int part, CString& module)
{
	cout << "ChangeModule " << part << " " << module << flush;
}

void ChangeJob(CString& job)
{
	cout << "ChangeJob " << job << flush;
}

void CreateSQLpVSEJobs(CString& tabelnavn)
{
	if (tabelnavn.GetLength() != 6) {
		cout << "FEJL: tabelnavnet skal være 6 karakterer langt" << endl << flush;
	} else {
		scheduler->LockJobs();
		if (scheduler->FindJob(tabelnavn + "UD") < 0 &&
			scheduler->FindJob(tabelnavn + "IN") < 0 &&
			scheduler->FindJob(tabelnavn + "FR") < 0) {
			scheduler->UnlockJobs();
			CString udtabel = tabelnavn + "UD";
			CString udcomment = tabelnavn + " DB2-UDTRAEK";
			CString udmodul("DB2OUT00");
			CString udparms = CString("SQLPVSE\\") + tabelnavn +
				"_INSERT.SQL E:\\DATA\\SQLPVSE\\" + tabelnavn + ".DAT %";
			CString udevent = CString("E:\\FTPROOT\\WAREHOUSE\\") + tabelnavn;
			CString udmaxtime("100 00 00");
			CString udfollow = tabelnavn + "IN " + tabelnavn + "FR";
			CString intabel = tabelnavn + "IN";
			CString incomment = tabelnavn + " SQL-INDLAESNING";
			CString inmodul("SQLINP00");
			CString inparms = CString("SQLPVSE\\") + tabelnavn +
				"_INSERT.SQL E:\\DATA\\SQLPVSE\\" + tabelnavn + ".DAT " + tabelnavn;
			CString infollow("");
			CString frtabel = tabelnavn + "FR";
			CString frcomment = tabelnavn + " FAQS SVAR";
			CString frmodul("FAQSRTN0");
			CString frparms = tabelnavn;
			CString frfollow("");
			CreateEventJob(udtabel, udcomment, udmodul, udparms, udevent, udmaxtime, udfollow);
			CreateManuelJob(intabel, incomment, inmodul, inparms, infollow);
			CreateManuelJob(frtabel, frcomment, frmodul, frparms, frfollow);
		} else {
			scheduler->UnlockJobs();
			cout << "FEJL: jobs findes i forvejen" << endl << flush;
		}
	}
}

void CreateSQLPROJobs(CString& jobnavn, CString& tabelnavn)
{
	if (jobnavn.GetLength() != 6) {
		cout << "FEJL: tabelnavnet skal vaere 6 karakterer langt" << endl << flush;
	} else {
		scheduler->LockJobs();
		if (scheduler->FindJob(jobnavn + "UD") < 0 &&
			scheduler->FindJob(jobnavn + "IN") < 0 &&
			scheduler->FindJob(jobnavn + "FR") < 0) {
			scheduler->UnlockJobs();
			CString udtabel = jobnavn + "UD";
			CString udcomment = tabelnavn + " VSAM-KONVERTERING";
			CString udmodul("VSAM2SQL");
			CString udparms = CString("SQLPRO\\") + tabelnavn + ".VSAM WIN.XLT" +
				" % E:\\DATA\\SQLPRO\\" + tabelnavn + ".DAT";
			CString udevent = CString("E:\\FTPROOT\\WAREHOUSE\\ic1lock") + tabelnavn + ".*";
			CString udmaxtime("100 00 00");
			CString udfollow = jobnavn + "IN";
			CString intabel = jobnavn + "IN";
			CString incomment = tabelnavn + " SQL-INDLAESNING";
			CString inmodul("SQLINP00");
			CString inparms = CString("SQLPRO\\") + tabelnavn + ".VSAM" +
				" E:\\DATA\\SQLPRO\\" + tabelnavn + ".DAT VSAM " + tabelnavn;
			CString infollow("");
			CreateEventJob(udtabel, udcomment, udmodul, udparms, udevent, udmaxtime, udfollow);
			CreateManuelJob(intabel, incomment, inmodul, inparms, infollow);
		} else {
			scheduler->UnlockJobs();
			cout << "FEJL: jobs findes i forvejen" << endl << flush;
		}
	}
}

void CreateVSAMJobs(CString& jobnavn, CString& tabelnavn)
{
	if (jobnavn.GetLength() != 6) {
		cout << "FEJL: tabelnavnet skal vaere 6 karakterer langt" << endl << flush;
	} else {
		scheduler->LockJobs();
		if (scheduler->FindJob(jobnavn + "UD") < 0 &&
			scheduler->FindJob(jobnavn + "IN") < 0 &&
			scheduler->FindJob(jobnavn + "FR") < 0) {
			scheduler->UnlockJobs();
			CString udtabel = jobnavn + "UD";
			CString udcomment = tabelnavn + " VSAM-KONVERTERING";
			CString udmodul("VSAM2SQL");
			CString udparms = CString("VSAM\\") + tabelnavn + ".VSAM WIN.XLT" +
				" % E:\\DATA\\VSAM\\" + tabelnavn + ".DAT";
			CString udevent = CString("E:\\FTPROOT\\WAREHOUSE\\ic1lock") + tabelnavn + ".*";
			CString udmaxtime("100 00 00");
			CString udfollow = jobnavn + "IN";
			CString intabel = jobnavn + "IN";
			CString incomment = tabelnavn + " SQL-INDLAESNING";
			CString inmodul("SQLINP00");
			CString inparms = CString("VSAM\\") + tabelnavn + ".VSAM" +
				" E:\\DATA\\VSAM\\" + tabelnavn + ".DAT VSAM " + tabelnavn;
			CString infollow("");
			CreateEventJob(udtabel, udcomment, udmodul, udparms, udevent, udmaxtime, udfollow);
			CreateManuelJob(intabel, incomment, inmodul, inparms, infollow);
		} else {
			scheduler->UnlockJobs();
			cout << "FEJL: jobs findes i forvejen" << endl << flush;
		}
	}
}

void CreateKONVVSAMJobs(CString& jobnavn, CString& tabelnavn)
{
	if (jobnavn.GetLength() != 6) {
		cout << "FEJL: tabelnavnet skal vaere 6 karakterer langt" << endl << flush;
	} else {
		scheduler->LockJobs();
		if (scheduler->FindJob(jobnavn + "UD") < 0 &&
			scheduler->FindJob(jobnavn + "IN") < 0 &&
			scheduler->FindJob(jobnavn + "FR") < 0) {
			scheduler->UnlockJobs();
			CString udtabel = jobnavn + "UD";
			CString udcomment = tabelnavn + " VSAM-KONVERTERING";
			CString udmodul("VSAM2SQL");
			CString udparms = CString("KONVERTER\\VSAM\\") + tabelnavn + ".VSAM WIN.XLT" +
				" % E:\\DATA\\KONVERTER\\VSAM\\" + tabelnavn + ".DAT";
			CString udevent = CString("E:\\FTPROOT\\WAREHOUSE\\ic1lock") + tabelnavn + ".*";
			CString udmaxtime("100 00 00");
			CString udfollow = jobnavn + "IN";
			CString intabel = jobnavn + "IN";
			CString incomment = tabelnavn + " SQL-INDLAESNING";
			CString inmodul("SQLINP00");
			CString inparms = CString("KONVERTER\\VSAM\\") + tabelnavn + ".VSAM" +
				" E:\\DATA\\KONVERTER\\VSAM\\" + tabelnavn + ".DAT VSAM " + tabelnavn;
			CString infollow("");
			CreateEventJob(udtabel, udcomment, udmodul, udparms, udevent, udmaxtime, udfollow);
			CreateManuelJob(intabel, incomment, inmodul, inparms, infollow);
		} else {
			scheduler->UnlockJobs();
			cout << "FEJL: jobs findes i forvejen" << endl << flush;
		}
	}
}

void DisplayJobAll()
{
	scheduler->LockJobs();
	cout << endl << " Oversigt over alle MFAQS jobs" << endl <<
					" ----------------------------------" << endl;
	if (scheduler->DisplayJobs() == 0) cout << "  (ingen jobs i MFAQS)" << endl << flush;
	scheduler->UnlockJobs();
}

void DisplayJobStatus(CString& job)
{
	scheduler->LockJobs();
	cout << endl << " Status af \"" << job << "\"" << endl <<
					" ----------------------------------" << endl;
	int idx = scheduler->FindJob(job);
	if (idx < 0) cout << "  (intet med denne kode)" << endl;
	else scheduler->DisplayJob(idx);
	scheduler->UnlockJobs();
	cout << endl << flush;
}

void DisplayJobLastRun(CString& job)
{
	cout << "DisplayJobLastRun " << job << flush;
}

void DisplayJobRun(CString& job, int jobrun)
{
	cout << "DisplayJobRun " << job << " " << jobrun << flush;
}

void DiplayQue(int part)
{
	cout << endl << " Koe-aktivitet i partion (" << part << ")" << endl <<
					"----------------------------------" << endl;
	scheduler->GetJobQue(part-1)->LockQue();
	QUE_JOBS *quelist = scheduler->GetJobQue(part-1)->GetQueStatus();
	if (quelist->no_jobs == 0) {
		cout << "  (ingen jobs i koeen)" << endl;
	} else {
		for (int i = quelist->no_jobs-1; i >= 0; i--) {
			if (quelist->handle[i] == QUE_SERVICE_HANDLE) {
				cout << "  SERVICE BREAK !!!!" << endl;
			} else {
				TCHAR comment[100];
				LPCTSTR jobcode = scheduler->GetJob(quelist->handle[i])->GetJobCode(comment);
				if (quelist->status[i] == QUE_ACTIVE) {
					cout << "  ACTIVE   ";
				} else if (quelist->status[i] == QUE_RUNNING) {
					cout << "  RUNNING  ";
				} else if (quelist->status[i] == QUE_ERROR) {
					cout << "  ERROR    ";
				} else {
					cout << "  SUCCES   ";
				}
				cout << jobcode << " " << comment << endl;
			}
		}
	}
	delete quelist;
	scheduler->GetJobQue(part-1)->UnLockQue();
	cout << flush;
}

void DisplayModuleAll(int part)
{
	cout << endl << " Service moduler i partion (" << part << ")" << endl <<
					" ---------------------------------" << endl;
	DLLWORKERS *workerlist = partions[part-1]->GetWorkerList();
	if (workerlist->no_workers == 0) {
		cout << "  (ingen service moduler)" << endl;
	} else {
		for (int i = 0; i < workerlist->no_workers; i++) {
			cout << " \"" << workerlist->servicename[i] << "\" " << workerlist->filename[i] << endl;
		}
	}
	delete workerlist;
	cout << flush;
}

void DisplayModule(int part, CString& module)
{
	cout << endl << " Service modul i partion (" << part << ")" << endl <<
					" ---------------------------------" << endl;
	DLLWORKERS *workerlist = partions[part-1] -> GetWorkerList();
	if (workerlist->no_workers == 0) {
		cout << "  (ingen service moduler i partion)" << endl;
	} else {
		int found = FALSE;
		for (int i = 0; i < workerlist->no_workers; i++) {
			if (workerlist->servicename[i] == module) {
				found = TRUE;
				cout << " \"" << workerlist->servicename[i] << "\" " << workerlist->filename[i] << endl;
			}
			if (found == FALSE) {
				cout << "  (intet service modul med det navn)" << endl;
			}
		}
	}
	delete workerlist;
	cout << flush;
}

void LockPartion(int part)
{
	TCHAR ans[100];

	cout << " Laasning af partion (" << part << ")" << endl <<
			" --------------------------" << endl;
	if (partion_lock[part-1]) {
		cout << "  Partionen er allerede laast !!!!!" << endl;
	} else {
		cout << " Partionen laases til service efter evt. koerende job er afsluttet." << endl <<
				" Er dette OK (J/N) ???? >" << flush;
		gets_s(ans);
		CString answer(ans);
		if (answer == "J" || answer == "JA") {
			cout << "  partionen laases - vent !!!!" << endl << flush;
			if (partionslocked == 0) WaitForSingleObject(hSchedule, INFINITE);
			partionslocked++;
			StopPartion(part-1);
			cout << "  partionen er nu laast !!!!!" << endl << flush;
			partion_lock[part-1] = TRUE;
		} else {
			cout << "  partionen laases ikke....." << endl;
		}
	}
	cout << flush;
}

void UnlockPartion(int part)
{
	cout << " Genstart af partion (" << part << ")" << endl <<
			" --------------------------" << endl;
	if (partion_lock[part-1] == FALSE) {
		cout << "  partionen er ikke laast !!!!!" << endl;
	} else {
		cout << "  partionen genstartes...." << endl << flush;
		if (RestartPartion(part-1)) {
			partionslocked--;
			if (partionslocked == 0) ReleaseMutex(hSchedule);
			cout << "  partionen er genstartet" << endl;
			partion_lock[part-1] = FALSE;
		} else {
			cout << " partionen kunne ikke genstartes og er stadig laast !!!!!" << endl;
		}
	}
	cout << flush;
}

int ValConfigOptions(CString& dailylog, CString& notesmail, CString& logerrortime)
{
	CommandLexer(dailylog);
	if (cmd_lex.GetSize() != 2) {
		cout << " FEJL: forkert tidspunkt for logudlaesning !!!!" << endl;
		return(FALSE);
	}
	if (atoi(cmd_lex[0]) < 0 || atoi(cmd_lex[0]) > 23 || atoi(cmd_lex[1]) < 0 || atoi(cmd_lex[1]) > 59) {
		cout << " FEJL: forkert tidspunkt for logudlaesning !!!!" << endl;
		return(FALSE);
	}
	CommandLexer(logerrortime);
	if (cmd_lex.GetSize() != 1) {
		cout << " FEJL: forkert tidsrum for fejl-udsendelse !!!!" << endl;
		return(FALSE);
	}
	if (atoi(cmd_lex[0]) < 1 || atoi(cmd_lex[0]) > 59) {
		cout << " FEJL: forkert tidsrum for fejl-udsendelse !!!!" << endl;
		return(FALSE);
	}
	return(TRUE);
}

void SetConfigOptions(CString& dailylog, CString& notesmail, CString& logerrortime)
{
	CommandLexer(dailylog);
	CTimeSpan dailyspan(0, atoi(cmd_lex[0]), atoi(cmd_lex[1]), 0);
	CTimeSpan logerrorspan(0, 0, atoi(cmd_lex[0]), 0);
	scheduler->SetOptions(dailyspan, notesmail, logerrorspan);
}

void ConfigMFAQS()
{
	TCHAR temp_str[100];
	cout << " Configuration af MFAQS" << endl <<
			" --------------------------" << endl;
	cout << "  Indtast tidspunkt for daglig log-udlaesning (\"hh mm\") >";
	gets_s(temp_str); CString dailylog(temp_str);
	cout << "  Indtast Notes mail'ing liste >";
	gets_s(temp_str); CString notesmail(temp_str);
	cout << "  Indtast tidsrum for fejl-udsendelse (\"mm\") >";
	gets_s(temp_str); CString logerrortime(temp_str);
	if (ValConfigOptions(dailylog, notesmail, logerrortime)) {
		SetConfigOptions(dailylog, notesmail, logerrortime);
	}
}

void SaveMFAQSConfig(LPCTSTR configfilename)
{
	LNORM("..gemmer konfiguration");
	configfile.Open(configfilename, CFile::modeWrite | CFile::modeCreate);
	CArchive ar(&configfile, CArchive::store);
	scheduler->Serialize( ar );
	for (int i = 0; i < 5; i++) partions[i]->Serialize( ar );
	ar.Close();
	configfile.Close();
}

void CommandParser(LPCTSTR command)
{
	CommandLexer(command);
	if (cmd_lex.GetSize() > 0) {
		if (cmd_lex[0] == "NEW") {				// new job, service module
			if (cmd_lex.GetSize() == 1) {
				cout << " syntax: NEW JOB" << endl << "         NEW MODULE <partion 1-5>" << endl << flush;
			} else {
				if (cmd_lex[1] == "MODULE") {
					if (cmd_lex.GetSize() == 2) {
						cout << " syntax: NEW MODULE <partion 1-5>" << endl << flush;
					} else {
						int part = atoi(cmd_lex[2]);
						if (1 <= part && part <= 5 && cmd_lex.GetSize() == 3) {
							NewModule(part);
						} else {
							cout << " FEJL: korrekt syntax: NEW MODULE <partion 1-5>" << endl << flush;
						}
					}
				} else if (cmd_lex[1] == "JOB") {
					if (cmd_lex.GetSize() == 2) {
						NewJob();
					} else {
						cout << " FEJL: korrekt syntax: NEW JOB" << endl << flush;
					}
				} else if (cmd_lex[1] == "SQLPVSE") {
					if (cmd_lex.GetSize() == 3) {
						CreateSQLpVSEJobs(cmd_lex[2]);
					} else {
						cout << " FEJL: korrekt syntax: NEW SQLPVSE <tabelnavn>" << endl << flush;
					}
				} else if (cmd_lex[1] == "SQLPRO") {
					if (cmd_lex.GetSize() == 4) {
						CreateSQLPROJobs(cmd_lex[2], cmd_lex[3]);
					} else {
						cout << " FEJL: korrekt syntax: NEW SQLPRO <jobnavn> <tabelnavn>" << endl << flush;
					}
				} else if (cmd_lex[1] == "VSAM") {
					if (cmd_lex.GetSize() == 4) {
						CreateVSAMJobs(cmd_lex[2], cmd_lex[3]);
					} else {
						cout << " FEJL: korrekt syntax: NEW VSAM <jobnavn> <tabelnavn>" << endl << flush;
					}
				} else if (cmd_lex[1] == "KVSAM") {
					if (cmd_lex.GetSize() == 4) {
						CreateKONVVSAMJobs(cmd_lex[2], cmd_lex[3]);
					} else {
						cout << " FEJL: korrekt syntax: NEW VSAM <jobnavn> <tabelnavn>" << endl << flush;
					}
				} else {
					cout << " FEJL: korrekt syntax: NEW JOB" << endl <<
							"                       NEW MODULE <partion 1-5>" << endl << flush;
				}
			}
		} else if (cmd_lex[0] == "REMOVE") {	// remove job, service module
			if (cmd_lex.GetSize() == 1) {
				cout << " syntax: REMOVE JOB <job code>" << endl <<
						"         REMOVE MODULE <partion 1-5> <module name>" << endl << flush;
			} else {
				if (cmd_lex[1] == "MODULE") {
					if (cmd_lex.GetSize() == 2) {
						cout << " syntax: REMOVE MODULE <partion 1-5> <module name>" << endl << flush;
					} else {
						int part = atoi(cmd_lex[2]);
						if (1 <= part && part <= 5) {
							if (cmd_lex.GetSize() == 3) {
								cout << " syntax: REMOVE MODULE <partion 1-5> <module name>" << endl << flush;
							} else if (cmd_lex.GetSize() == 4) {
								RemoveModule(part, cmd_lex[3]);
							} else {
								cout << " FEJL: korrekt syntax: REMOVE MODULE <partion 1-5> <module name>" << endl << flush;
							}
						} else {
							cout << " FEJL: korrekt syntax: REMOVE MODULE <partion 1-5> <module name>" << endl << flush;
						}
					}
				} else if (cmd_lex[1] == "JOB") {
					if (cmd_lex.GetSize() == 2) {
						cout << " syntax: REMOVE JOB <job code>" << endl << flush;
					} else {
						if (cmd_lex.GetSize() == 3) {
							RemoveJob(cmd_lex[2]);
						} else {
							cout << " FEJL: korrekt syntax: REMOVE JOB <job code>" << endl << flush;
						}
					}
				} else {
					cout << " FEJL: korrekt syntax: REMOVE JOB <job code>" << endl <<
							"                       REMOVE MODULE <partion 1-5> <module name>" << endl << flush;
				}
			}
		} else if (cmd_lex[0] == "CHANGE") {	// change job, service module
			if (cmd_lex.GetSize() == 1) {
				cout << " syntax: CHANGE JOB <job code>" << endl <<
						"         CHANGE MODULE <partion 1-5> <module name>" << endl << flush;
			} else {
				if (cmd_lex[1] == "MODULE") {
					if (cmd_lex.GetSize() == 2) {
						cout << " syntax: CHANGE MODULE <partion 1-5> <module name>" << endl << flush;
					} else {
						int part = atoi(cmd_lex[2]);
						if (1 <= part && part <= 5) {
							if (cmd_lex.GetSize() == 3) {
								cout << " syntax: CHANGE MODULE <partion 1-5> <module name>" << endl << flush;
							} else if (cmd_lex.GetSize() == 4) {
								ChangeModule(part, cmd_lex[3]);
							} else {
								cout << " FEJL: korrekt syntax: CHANGE MODULE <partion 1-5> <module name>" << endl << flush;
							}
						} else {
							cout << " FEJL: korrekt syntax: CHANGE MODULE <partion 1-5> <module name>" << endl << flush;
						}
					}
				} else if (cmd_lex[1] == "JOB") {
					if (cmd_lex.GetSize() == 2) {
						cout << " syntax: CHANGE JOB <job code>" << endl << flush;
					} else {
						if (cmd_lex.GetSize() == 3) {
							ChangeJob(cmd_lex[2]);
						} else {
							cout << " FEJL: korrekt syntax: CHANGE JOB <job code>" << endl << flush;
						}
					}
				} else {
					cout << " FEJL: korrekt syntax: CHANGE JOB <job code>" << endl <<
							"                       CHANGE MODULE <partion 1-5> <module name>" << endl << flush;
				}
			}
		} else if (cmd_lex[0] == "DISPLAY") {	// jobs, ques, service module
			if (cmd_lex.GetSize() == 1) {
				cout << " syntax: DISPLAY JOB ..." << endl <<
						"         DISPLAY QUE <partion 1-5>" << endl <<
						"         DISPLAY MODULES <partions 1-5> ..." << endl << flush;
			} else {
				if (cmd_lex[1] == "JOB") {
					if (cmd_lex.GetSize() == 2) {
						cout << " syntax: DISPLAY JOB ALL" << endl <<
								"         DISPLAY JOB CODE <job code> STATUS" << endl <<
								"         DISPLAY JOB CODE <job code> RUN <x 1-10>" << endl <<
								"         DISPLAY JOB CODE <job code> LASTRUNS" << endl << flush;
					} else {
						if (cmd_lex[2] == "ALL") {
							if (cmd_lex.GetSize() == 3) {
								DisplayJobAll();
							} else {
								cout << " FEJL: korrekt syntax: DISPLAY JOB ALL" << endl << flush;
							}
						} else if (cmd_lex[2] == "CODE") {
							if (cmd_lex.GetSize() == 3 || cmd_lex.GetSize() == 4) {
								cout << " syntax: DISPLAY JOB CODE <job code> STATUS" << endl <<
										"         DISPLAY JOB CODE <job code> RUN <x 1-10>" << endl <<
										"         DISPLAY JOB CODE <job code> LASTRUNS" << endl << flush;
							} else {
								if (cmd_lex[4] == "STATUS") {
									if (cmd_lex.GetSize() == 5) {
										DisplayJobStatus(cmd_lex[3]);
									} else {
										cout << " FEJL: korrekt syntax: DISPLAY JOB CODE <job code> STATUS" <<
											endl << flush;
									}
								} else if (cmd_lex[4] == "RUN") {
									if (cmd_lex.GetSize() == 5) {
										cout << " syntax: DISPLAY JOB CODE <job code> RUN <x 1-10>" << endl << flush;
									} else {
										int jobrun = atoi(cmd_lex[5]);
										if (cmd_lex.GetSize() == 6 && jobrun >= 1 && jobrun <= 10) {
											DisplayJobRun(cmd_lex[3], jobrun);
										} else {
											cout << " FEJL: korrekt syntax: DISPLAY JOB CODE <job code> RUN <x 1-10>" <<
												endl << flush;
										}
									}
								} else if (cmd_lex[4] == "LASTRUNS") {
									if (cmd_lex.GetSize() == 5) {
										DisplayJobLastRun(cmd_lex[3]);
									} else {
										cout << " FEJL: korrekt syntax: DISPLAY JOB CODE <job code> LASTRUN" <<
											endl << flush;
									}
								} else {
									cout << " FEJL: korrekt syntax: DISPLAY JOB CODE <job code> STATUS" << endl <<
											"                       DISPLAY JOB CODE <job code> RUN <x 1-10>" << endl <<
											"                       DISPLAY JOB CODE <job code> LASTRUNS" << endl << flush;
								}
							}
						} else {
							cout << " FEJL: korrekt syntax: DISPLAY JOB ALL" << endl <<
									"                       DISPLAY JOB CODE <job code> STATUS" << endl <<
									"                       DISPLAY JOB CODE <job code> RUN <x 1-10>" << endl <<
									"                       DISPLAY JOB CODE <job code> LASTRUNS" << endl << flush;
						}
					}
				} else if (cmd_lex[1] == "QUE") {
					if (cmd_lex.GetSize() == 3) {
						int part = atoi(cmd_lex[2]);
						if (1 <= part && part <= 5) {
							DiplayQue(part);
						} else {
							cout << " FEJL: korrekt syntax: DISPLAY QUE <partion 1-5>" << endl << flush;
						}
					} else {
						cout << " syntax: DISPLAY QUE <partion 1-5>" << endl << flush;
					}
				} else if (cmd_lex[1] == "MODULE") {
					if (cmd_lex.GetSize() == 2) {
						cout << " syntax: DISPLAY MODULE <partions 1-5> ALL" << endl <<
								"         DISPLAY MODULE <partions 1-5> NAME <module name>" << endl << flush;
					} else {
						int part = atoi(cmd_lex[2]);
						if (cmd_lex.GetSize() == 3) {
							if (1 <= part && part <= 5) {
								cout << " syntax: DISPLAY MODULE <partions 1-5> ALL" << endl <<
										"         DISPLAY MODULE <partions 1-5> NAME <module name>" << endl << flush;
							} else {
								cout << " FEJL: korrekt syntax: DISPLAY MODULE <partions 1-5> ALL" << endl <<
										"                       DISPLAY MODULE <partions 1-5> NAME <module name>" << endl << flush;
							}
						} else {
							if (cmd_lex[3] == "ALL") {
								if (cmd_lex.GetSize() == 4 && 1 <= part && part <= 5) {
									DisplayModuleAll(part);
								} else {
									cout << " FEJL: korrekt syntax: DISPLAY MODULE <partions 1-5> ALL" << endl << flush;
								}
							} else if (cmd_lex[3] == "NAME") {
								if (cmd_lex.GetSize() == 5 && 1 <= part && part <= 5) {
									DisplayModule(part, cmd_lex[4]);
								} else {
									cout << " FEJL: korrekt syntax: DISPLAY MODULE <partions 1-5> NAME <module name>" << endl << flush;
								}
							} else {
								cout << " FEJL: korrekt syntax: DISPLAY MODULE <partions 1-5> ALL" << endl <<
										"                       DISPLAY MODULE <partions 1-5> NAME <module name>" << endl << flush;
							}
						}
					}
				} else {
					cout << " syntax: DISPLAY JOB <job code> ..." << endl <<
							"         DISPLAY QUE <partion 1-5>" << endl <<
							"         DISPLAY MODULE <partions 1-5> ..." << endl << flush;
				}
			}
		} else if (cmd_lex[0] == "LOCK") {	// lock partion
			if (cmd_lex.GetSize() == 1) {
				cout << " syntax: LOCK <partion 1-5>" << endl << flush;
			} else {
				int part = atoi(cmd_lex[1]);
				if (cmd_lex.GetSize() == 2 && 1 <= part && part <= 5) {
					LockPartion(part);
				} else {
					cout << " FEJL: korrekt syntax: LOCK <partion 1-5>" << endl << flush;
				}
			}
		} else if (cmd_lex[0] == "UNLOCK") {	// unlock partion
			if (cmd_lex.GetSize() == 1) {
				cout << " syntax: UNLOCK <partion 1-5>" << endl << flush;
			} else {
				int part = atoi(cmd_lex[1]);
				if (cmd_lex.GetSize() == 2 && 1 <= part && part <= 5) {
					UnlockPartion(part);
				} else {
					cout << " FEJL: korrekt syntax: UNLOCK <partion 1-5>" << endl << flush;
				}
			}
		} else if (cmd_lex[0] == "SAVE") {		// save configuration
			if (cmd_lex.GetSize() == 1) {
				if (conffile) SaveMFAQSConfig(configfilename);
				else {
					cout << " FEJL: ingen default konfigurationsfil" << endl << flush;
				}
			} else if (cmd_lex.GetSize() == 2) {
				SaveMFAQSConfig(cmd_lex[1]);
				conffile = TRUE;
				strcpy_s(configfilename, cmd_lex[1]);
			} else {
				cout << " FEJL: forkert syntax: SAVE <configfile>" << endl << flush;
			}
		} else if (cmd_lex[0] == "CONFIG") {	// config MFAQS
			if (cmd_lex.GetSize() != 1) {
				cout << " FEJL: korrekt syntax: CONFIG" << endl << flush;
			} else {
				ConfigMFAQS();
			}
		} else {
			cout << " syntax: NEW ..." << endl <<
					"         REMOVE ..." << endl <<
					"         CHANGE ..." << endl <<
					"         DISPLAY ..." << endl <<
					"         LOCK ..." << endl <<
					"         UNLOCK ..." << endl <<
					"         EXEC ..." << endl <<
					"         SAVE ..." << endl <<
					"         CONFIG" << endl <<
					"         QUIT" << endl <<
					"         RUN" << endl << flush;
		}
	}
}
