// MFAQS (MiniFAQS til NT)
// Esben Viskum, AO, maj 1999
// Modellering af et MFAQS-job/kø.

#include "StdAfx.h"

// #include <afx.h>
// #include <afxtempl.h>
#include <fstream>
#include "mfaqsjob.h"
#include "mfaqslog.h"
#include "mfaqscfg.h"
using namespace std;

FREQUENCE::FREQUENCE()
{
	for (int i = 0; i < 31; i++) days[i] = FALSE;
}

FREQUENCE::FREQUENCE(int init_freq_type)
{
	freq_type = init_freq_type;
	for (int i = 0; i < 31; i++) days[i] = FALSE;
}

void FREQUENCE::Display()
{
	cout << " Frekvens: ";
	if (freq_type == RUNONCE) {
		cout << "ONCE  ";
	} else if (freq_type == RUNDAILY) {
		cout << "DAILY, ugedage: ";
		for (int i = 0; i < 7; i++) if (days[i]) cout << i+1 << " ";
	} else if (freq_type == RUNMONTHLY) {
		cout << "MONTHLY, maanedsdage: ";
		for (int i = 0; i < 31; i++) if (days[i]) cout << i+1 << " ";
	}
	cout << endl;
}

void FREQUENCE::SetDay(int day, int val)
{
	days[day-1] = val;
}

int FREQUENCE::GetDay(int day)
{
	return days[day-1];
}

int FREQUENCE::SetFreqType( int set_freq_type )
{
	freq_type = set_freq_type;
	return freq_type;
}

int FREQUENCE::GetFreqType( void )
{
	return freq_type;
}

void FREQUENCE::Serialize( CArchive& ar )
{
	if ( ar.IsStoring() ) {
//		int temp, i;
//        ar << freq_type;
//		for (i = 0; i < 31; i++) {
//			temp = days[i];
//			ar << temp;
//		}
    } else {
//		int temp, i;
//		ar >> freq_type;
//		for (i = 0; i < 31; i++) {
//			ar >> temp;
//			days[i] = temp;
//		}
	}
}





DEPENDENCY::DEPENDENCY()
{
	for (int i = 0; i < 10; i++) dependency_job[i][0] = '\0';
}

DEPENDENCY::DEPENDENCY(void *set_schedule)
{
	for (int i = 0; i < 10; i++) dependency_job[i][0] = '\0';
	job_schedule = set_schedule;
}

void DEPENDENCY::Display()
{
	int found = FALSE, j = 0;
	for (int i = 0; i < 10; i++) {
		if (dependency_job[i][0] != '\0') {
			found = TRUE; j++;
			cout << " Job-afhaengighed " << j << ": " << dependency_job[i] << " afhaengighedstatus: ";
			if (check_dependency(i+1)) cout << "OK";
			else cout << "--";
			cout << endl;
		}
	}
	if (found == FALSE) cout << "  (ingen afhaengigheder)" << endl;
}

void DEPENDENCY::SetDependency_job(int job_index)
{
	dependency_job[job_index-1][0] = '\0';
}

void DEPENDENCY::SetDependency_job(int job_index, LPCTSTR job_code)
{
	strcpy(dependency_job[job_index-1], job_code);
}

LPCTSTR DEPENDENCY::GetDependency_job(int job_index)
{
	if (dependency_job[job_index-1][0] == '\0') return(NULL);
	return(dependency_job[job_index-1]);
}

int DEPENDENCY::check_dependency()
{
	int check = TRUE;
	for (int i = 0; i < 10; i++) if (check_dependency(i+1) == FALSE) check = FALSE;
	return(check);
}

int DEPENDENCY::check_dependency(int job_index)
{
	if (dependency_job[job_index-1][0] == '\0') return(TRUE);
	int job_handle = ((JOB_SCHEDULE *) job_schedule)->FindJob(dependency_job[job_index-1]);
	if (job_handle >= 0) {
		if (((JOB_SCHEDULE *) job_schedule)->GetJobStatus(job_handle) == JOB_SUCCES) return(TRUE);
		else return(FALSE);
	}
	return(FALSE);
}

void DEPENDENCY::Serialize( CArchive& ar )
{
	if ( ar.IsStoring() ) {
//		int i;
//		for (i = 0; i < 10; i++) {
//			ar << CString(dependency_job[i]);
//		}
    } else {
//		int i;
//		CString temp_str;
//		for (i = 0; i < 10; i++) {
//			ar >> temp_str;
//			strcpy(dependency_job[i], temp_str);
//		}
	}
}


FOLLOWJOBS::FOLLOWJOBS( void )
{
	for (int i = 0; i < 10; i++) follow_job[i][0] = '\0';
}

FOLLOWJOBS::FOLLOWJOBS(void *set_schedule)
{
	for (int i = 0; i < 10; i++) follow_job[i][0] = '\0';
	job_schedule = set_schedule;
}

void FOLLOWJOBS::Serialize( /* CArchive& ar */ )
{
//	if ( ar.IsStoring() ) {
//		int i;
//		for (i = 0; i < 10; i++) {
//			ar << CString(follow_job[i]);
//		}
//    } else {
//		int i;
//		CString temp_str;
//		for (i = 0; i < 10; i++) {
//			ar >> temp_str;
//			strcpy(follow_job[i], temp_str);
//		}
		if (current->cFollowjob1 > "        ") strcpy(follow_job[0], (LPCTSTR) current->cFollowjob1);
		else follow_job[0][0] = '\0';
		if (current->cFollowjob2 > "        ") strcpy(follow_job[1], (LPCTSTR) current->cFollowjob2);
		else follow_job[1][0] = '\0';
		if (current->cFollowjob3 > "        ") strcpy(follow_job[2], (LPCTSTR) current->cFollowjob3);
		else follow_job[2][0] = '\0';
		if (current->cFollowjob4 > "        ") strcpy(follow_job[3], (LPCTSTR) current->cFollowjob4);
		else follow_job[3][0] = '\0';
		if (current->cFollowjob5 > "        ") strcpy(follow_job[4], (LPCTSTR) current->cFollowjob5);
		else follow_job[4][0] = '\0';
		if (current->cFollowjob6 > "        ") strcpy(follow_job[5], (LPCTSTR) current->cFollowjob6);
		else follow_job[5][0] = '\0';
		if (current->cFollowjob7 > "        ") strcpy(follow_job[6], (LPCTSTR) current->cFollowjob7);
		else follow_job[6][0] = '\0';
		if (current->cFollowjob8 > "        ") strcpy(follow_job[7], (LPCTSTR) current->cFollowjob8);
		else follow_job[7][0] = '\0';
		follow_job[8][0] = '\0';
		follow_job[9][0] = '\0';
//	}
}

void FOLLOWJOBS::Display()
{
	int found = FALSE, j = 0;
	for (int i = 0; i < 10; i++) {
		if (follow_job[i][0] != '\0') {
			found = TRUE; j++;
			cout << " Efterfoelgende jobs " << j << ": " << follow_job[i] << endl;
		}
	}
	if (found == FALSE) cout << "  (ingen efterfoelgende jobs)" << endl;
}

void FOLLOWJOBS::SetFollow_job(int job_index)
{
	follow_job[job_index-1][0] = '\0';
}

void FOLLOWJOBS::SetFollow_job(int job_index, LPCTSTR job_code)
{
	strcpy(follow_job[job_index-1], job_code);
}

void FOLLOWJOBS::ActivateJobs()
{
	for (int i = 0; i < 10; i++) {
		if (follow_job[i][0] != '\0') {
			int job_handle = ((JOB_SCHEDULE *) job_schedule)->FindJob(follow_job[i]);
			if (job_handle >= 0) {
				if (((JOB_SCHEDULE *) job_schedule)->GetJobStatus(job_handle) == JOB_READY) {
					((JOB_SCHEDULE *) job_schedule)->GetJob(job_handle)->SetJobStatus(QUE_FOLLOW_ACTIVE);
				}
			}
		}
	}
}


JOBPARAMETER::JOBPARAMETER( void )
{
	for (int i = 0; i < 5; i++) parameter[i][0] = '\0';
}

void JOBPARAMETER::Display( void )
{
	int found = FALSE, j = 0;
	for (int i = 0; i < 5; i++) {
		if (parameter[i][0] != '\0') {
			found = TRUE; j++;
			cout << " Parameter " << j << ": " << parameter[i] << endl;
		}
	}
	if (found == FALSE) cout << "  (ingen parametre)" << endl;
}

void JOBPARAMETER::SetParm(int parm_index, LPCTSTR parmval)
{
	strcpy(parameter[parm_index-1], parmval);
}

LPTSTR JOBPARAMETER::GetParm(int parm_index)
{
	if (parameter[parm_index-1][0] == '\0') return(NULL);
	return(parameter[parm_index-1]);
}

LPTSTR JOBPARAMETER::GetParm(int parm_index,  LPCTSTR parmval)
{
	if (parameter[parm_index-1][0] == '%') {
		strcpy(temp_str, parmval);
		return(temp_str);
	} else {
		if (parameter[parm_index-1][0] == '\0') return(NULL);
		return(parameter[parm_index-1]);
	}
}

void JOBPARAMETER::Serialize( /* CArchive& ar */ )
{
//	if ( ar.IsStoring() ) {
//		int i;
//		for (i = 0; i < 5; i++) {
//			ar << CString(parameter[i]);
//		}
//    } else {
//		int i;
//		CString temp_str;
//		for (i = 0; i < 5; i++) {
//			ar >> temp_str;
//			strcpy(parameter[i], temp_str);
//		}
		if (current->cParm1 > " ") strcpy(parameter[0], (LPCTSTR) current->cParm1);
		else parameter[0][0] = '\0';
		if (current->cParm2 > " ") strcpy(parameter[1], (LPCTSTR) current->cParm2);
		else parameter[1][0] = '\0';
		if (current->cParm3 > " ") strcpy(parameter[2], (LPCTSTR) current->cParm3);
		else parameter[2][0] = '\0';
		if (current->cParm4 > " ") strcpy(parameter[3], (LPCTSTR) current->cParm4);
		else parameter[3][0] = '\0';
		if (current->cParm5 > " ") strcpy(parameter[4], (LPCTSTR) current->cParm5);
		else parameter[4][0] = '\0';
//	}
}


JobHistLogInfo::JobHistLogInfo()
{
	no_jobs = 0;
	for (int i = 0; i < 10; i++) job_array[i] = NULL;
}

JobHistLogInfo::~JobHistLogInfo()
{
	for (int i = 0; i < no_jobs; i++) delete job_array[i];
	no_jobs = 0;
}

void JobHistLogInfo::InsertJobLog(JobLogInfo *loginfo)
{
	int insert_pos = 9;
	if (no_jobs > 9) {
		delete job_array[0];
		for (int i = 1; i < 10; i++) job_array[i-1] = job_array[i];
	} else insert_pos = no_jobs++;
	job_array[insert_pos] = loginfo;
}

int JobHistLogInfo::no_Jobs()
{
	return (no_jobs);
}

JobLogInfo *JobHistLogInfo::GetJobLog(int index)
{
	return(job_array[no_jobs-index-1]);
}



MFAQSJOB::MFAQSJOB()
{
	job_status = JOB_READY;
	job_runs = 0;
	job_type = MANUALJOB;
	currentjoblog = NULL;
}


MFAQSJOB::MFAQSJOB(void *set_job_schedule) :
frequence(RUNONCE), last_run(CTime::GetCurrentTime()),
dependency(set_job_schedule), followjobs(set_job_schedule)
{
	job_status = JOB_READY;
	job_runs = 0;
	job_type = MANUALJOB;
	currentjoblog = NULL;

	job_schedule = set_job_schedule;

	SECURITY_ATTRIBUTES mutexatt;
	mutexatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutexatt.lpSecurityDescriptor = NULL;
	mutexatt.bInheritHandle = TRUE;
	
	jobmutex = CreateMutex(&mutexatt, FALSE, NULL);
}

MFAQSJOB::MFAQSJOB(void *set_job_schedule, LPCTSTR set_job_code, LPCTSTR set_comment) :
frequence(RUNONCE), last_run(CTime::GetCurrentTime()),
dependency(set_job_schedule), followjobs(set_job_schedule)
{
	job_status = JOB_READY;
	job_runs = 0;
	strcpy(job_code, set_job_code);
	strcpy(comment, set_comment);
	job_type = MANUALJOB;
	currentjoblog = NULL;

	job_schedule = set_job_schedule;

	SECURITY_ATTRIBUTES mutexatt;
	mutexatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutexatt.lpSecurityDescriptor = NULL;
	mutexatt.bInheritHandle = TRUE;
	
	jobmutex = CreateMutex(&mutexatt, FALSE, NULL);
}

MFAQSJOB::~MFAQSJOB()
{
	CloseHandle(jobmutex);
}

void MFAQSJOB::LockJob()
{
	WaitForSingleObject(jobmutex, 10000);
}

void MFAQSJOB::UnLockJob()
{
	ReleaseMutex(jobmutex);
}

void MFAQSJOB::DisplayJobShort()
{
	cout << " Job-kode: " << job_code << " Kommentar: " << comment << endl;
	cout << " Type: ";
	if (job_type == MANUALJOB) {
		cout << "MANUEL   ";
	} else if (job_type == TIMEJOB) {
		cout << "TIME     ";
	} else if (job_type == TIMEEVENTJOB) {
		cout << "TIMEEVENT";
	} else {
		cout << "EVENT    ";
	}
	cout << "             Status: ";
	if (job_status == JOB_READY) {
		cout << "READY";
	} else if (job_status == JOB_ACTIVE_WAIT) {
		cout << "ACTIVE, WAIT-MODE";
	} else if (job_status == JOB_ACTIVE_WAIT_WARNING) {
		cout << "ACTIVE, WAIT-MODE, WARNING";
	} else if (job_status == JOB_ACTIVE) {
		cout << "ACTIVE";
	} else if (job_status == JOB_ACTIVE_WARNING) {
		cout << "ACTIVE, WARNING";
	} else if (job_status == JOB_RUNNING) {
		cout << "RUNNING";
	} else if (job_status == JOB_RUNNING_WARNING) {
		cout << "RUNNING, WARNING";
	} else if (job_status == JOB_TIMEOUT) {
		cout << "TIMEOUT";
	} else if (job_status == JOB_ERROR) {
		cout << "ERROR";
	} else if (job_status == JOB_SUCCES) {
		cout << "SUCCES";
	}
	cout << endl;
}

void MFAQSJOB::DisplayJobLong()
{
	DisplayJobShort();
	cout << " Service modul: " << job_manager << endl;
	parameter.Display();
	followjobs.Display();
	if (job_type == TIMEJOB || job_type == TIMEEVENTJOB) {
		cout << " Starttidspunkt: " << start_time.Format("%Y-%m-%d %H:%M") << endl << " Warning-tid: " <<
			warning_time.Format("%H:%M") << "  Timeout-tid: " << timeout_time.Format("%H:%M") <<
			"  Reset-tid: " << reset_time.Format("%H:%M") << endl;
		frequence.Display();
		dependency.Display();
	}
	if (job_type == TIMEEVENTJOB || job_type == EVENTJOB) {
		cout << " Event fil-pattern: " << file_pattern << endl;
	}
	if (job_type == EVENTJOB) {
		cout << " Max-tid mellem koersler: " << max_interval.Format("dage: %D, %H:%M") << endl;
	}
	cout << " Antal koersler siden MFAQS opstart: " << job_runs << "  Sidste koersel: ";
	if (job_runs == 0) cout << "N/A" << endl;
	else cout << last_run.Format("%Y-%m-%d %H:%M") << endl;
}



void MFAQSJOB::SetTimeInfo(int start_year, int start_month, int start_day, int start_hour, int start_min,
					  int warning_hour, int warning_min, int timeout_hour, int timeout_min,
					  int reset_hour, int reset_min)
{
	start_time = CTime(start_year, start_month, start_day, start_hour, start_min, 0);
	warning_time = CTimeSpan(0, warning_hour, warning_min, 0);
	timeout_time = CTimeSpan(0, timeout_hour, timeout_min, 0);
	reset_time = CTimeSpan(0, reset_hour, reset_min, 0);
}

void MFAQSJOB::SetJobType(int set_job_type, LPCTSTR set_job_manager, LPCTSTR parm1,
			LPCTSTR parm2, LPCTSTR parm3, LPCTSTR parm4, LPCTSTR parm5)
{
	job_type = set_job_type;
	strcpy(job_manager, set_job_manager);
	if (parm1 != NULL) parameter.SetParm(1, parm1);
	if (parm2 != NULL) parameter.SetParm(2, parm2);
	if (parm3 != NULL) parameter.SetParm(3, parm3);
	if (parm4 != NULL) parameter.SetParm(4, parm4);
	if (parm5 != NULL) parameter.SetParm(5, parm5);
}

void MFAQSJOB::SetDependency(LPCTSTR job1, LPCTSTR job2, LPCTSTR job3,
							 LPCTSTR job4, LPCTSTR job5, LPCTSTR job6,
							 LPCTSTR job7, LPCTSTR job8, LPCTSTR job9,
							 LPCTSTR job10)
{
	if (job1 != NULL) dependency.SetDependency_job(1, job1);
	if (job2 != NULL) dependency.SetDependency_job(2, job2);
	if (job3 != NULL) dependency.SetDependency_job(3, job3);
	if (job4 != NULL) dependency.SetDependency_job(4, job4);
	if (job5 != NULL) dependency.SetDependency_job(5, job5);
	if (job6 != NULL) dependency.SetDependency_job(6, job6);
	if (job7 != NULL) dependency.SetDependency_job(7, job7);
	if (job8 != NULL) dependency.SetDependency_job(8, job8);
	if (job9 != NULL) dependency.SetDependency_job(9, job9);
	if (job10 != NULL) dependency.SetDependency_job(10, job10);
}

void MFAQSJOB::SetFollowjob(LPCTSTR job1, LPCTSTR job2, LPCTSTR job3,
							 LPCTSTR job4, LPCTSTR job5, LPCTSTR job6,
							 LPCTSTR job7, LPCTSTR job8, LPCTSTR job9,
							 LPCTSTR job10)
{
	if (job1 != NULL) followjobs.SetFollow_job(1, job1);
	if (job2 != NULL) followjobs.SetFollow_job(2, job2);
	if (job3 != NULL) followjobs.SetFollow_job(3, job3);
	if (job4 != NULL) followjobs.SetFollow_job(4, job4);
	if (job5 != NULL) followjobs.SetFollow_job(5, job5);
	if (job6 != NULL) followjobs.SetFollow_job(6, job6);
	if (job7 != NULL) followjobs.SetFollow_job(7, job7);
	if (job8 != NULL) followjobs.SetFollow_job(8, job8);
	if (job9 != NULL) followjobs.SetFollow_job(9, job9);
	if (job10 != NULL) followjobs.SetFollow_job(10, job10);
}

void MFAQSJOB::SetFrequence(int freq_type, int no_days, int days[])
{
	frequence.SetFreqType(freq_type);
	for (int i = 0; i < no_days; i++) frequence.SetDay(days[i], TRUE);
}

void MFAQSJOB::SetFilePattern(LPCTSTR set_pattern, int max_days, int max_hour, int max_min)
{
	strcpy(file_pattern, set_pattern);
	max_interval = CTimeSpan(max_days, max_hour, max_min, 0);
}

int MFAQSJOB::UpdateJobStatus(int mode)
{
	CTime currenttime = CTime::GetCurrentTime();
	if ((job_type == TIMEJOB) || (job_type == TIMEEVENTJOB)) {
		switch (job_status) {
		case JOB_READY:
			{
				int timerun = FALSE, runok = FALSE;
				CTime test_time(currenttime.GetYear(), currenttime.GetMonth(),
					currenttime.GetDay(), start_time.GetHour(), start_time.GetMinute(), 0);
				CTime test_start_time = test_time - CTimeSpan(1,0,0,0);
				CTime test_end_time = test_start_time + reset_time;
				if (test_start_time <= currenttime && currenttime < test_end_time &&
					test_start_time > last_run && start_time <= currenttime) {
					timerun = TRUE;
				} else {
					test_start_time = test_time;
					test_end_time = test_start_time + reset_time;
					if (test_start_time <= currenttime && currenttime < test_end_time &&
						test_start_time > last_run && start_time <= currenttime) {
						timerun = TRUE;
					}
				}
				if (timerun) {
					if (frequence.GetFreqType() == RUNONCE) {
						if (job_runs == 0 && test_start_time.GetYear() == start_time.GetYear() &&
							test_start_time.GetMonth() == start_time.GetMonth() &&
							test_start_time.GetDay() == start_time.GetDay()) runok = TRUE;
					} else if (frequence.GetFreqType() == RUNDAILY) {
						if (frequence.GetDay(test_start_time.GetDayOfWeek())) runok = TRUE;
					} else if (frequence.GetFreqType() == RUNMONTHLY) {
						if (frequence.GetDay(test_start_time.GetDay())) runok = TRUE;
					}
				}
				if (runok) {
					if (dependency.check_dependency()) {
						if (job_type == TIMEJOB) {
							if (mode != SERVICE_STOP_INSERTED) {
								job_status = JOB_ACTIVE;
								LogWriteNewJob(-1, LOGNORMAL,
									CString("Jobbet \"") + job_code + "\" er klar og bliver sat i koe");
							}
						} else if (job_type == TIMEEVENTJOB) {
							job_status = JOB_ACTIVE_WAIT;
							LogWriteNewJob(-1, LOGNORMAL,
								CString("Jobbet \"") + job_code + "\" er klar, men venter paa fil");
						}
					} else {
						job_status = JOB_ACTIVE_WAIT;
						LogWriteNewJob(-1, LOGNORMAL,
							CString("Jobbet \"") + job_code + "\" er klar, men venter paa afhaengige job");
					}
					active_time = test_start_time;
				}
			}
			break;
		case JOB_ACTIVE_WAIT:
			if (dependency.check_dependency()) {
				if (job_type == TIMEEVENTJOB) {
					CFileFind filefind;
					if (filefind.FindFile(file_pattern)) {
						filefind.FindNextFile();
						current_file = filefind.GetFilePath();
						if (mode != SERVICE_STOP_INSERTED) {
							job_status = JOB_ACTIVE;
							LogWrite(-1, LOGNORMAL,
								CString("Jobbet \"") + job_code + "\" er klar og bliver sat i koe");
						}
					}
					filefind.Close();
				} else if (mode != SERVICE_STOP_INSERTED) {
					job_status = JOB_ACTIVE;
					LogWrite(-1, LOGNORMAL,
						CString("Jobbet \"") + job_code + "\" er klar og bliver sat i koe");
				}
			}
			if (job_status != JOB_ACTIVE && ((active_time + warning_time) < currenttime)) {
				job_status = JOB_ACTIVE_WAIT_WARNING;
				LogWrite(-1, LOGERROR,
					CString("WARNING: \"") + job_code + "\" er klar, men venter stadig paa andre jobs eller filer");
			}
			break;
		case JOB_ACTIVE_WAIT_WARNING:
			if (dependency.check_dependency()) {
				if (job_type == TIMEEVENTJOB) {
					CFileFind filefind;
					if (filefind.FindFile(file_pattern)) {
						filefind.FindNextFile();
						current_file = filefind.GetFilePath();
						if (mode != SERVICE_STOP_INSERTED) {
							job_status = JOB_ACTIVE;
							LogWrite(-1, LOGNORMAL,
								CString("Jobbet \"") + job_code + "\" er klar og bliver sat i koe");
						}
					}
				} else if (mode != SERVICE_STOP_INSERTED) {
					job_status = JOB_ACTIVE;
					LogWrite(-1, LOGNORMAL,
						CString("Jobbet \"") + job_code + "\" er klar og bliver sat i koe");
				}
			}
			if (job_status != JOB_ACTIVE && ((active_time + timeout_time) < currenttime)) {
				job_status = JOB_TIMEOUT;
				LogWrite(-1, LOGERROR,
					CString("ERROR: \"") + job_code + "\" har overskredet tidsfristen og stoppes");
			}
			break;
		case JOB_ACTIVE:
			if ((active_time + warning_time) < currenttime) {
				job_status = JOB_ACTIVE_WARNING;
				LogWrite(-1, LOGERROR,
					CString("WARNING: \"") + job_code + "\" er klar, men er forsinket");
			}
			break;
		case JOB_ACTIVE_WARNING:
			if ((active_time + timeout_time) < currenttime) {
				job_status = JOB_TIMEOUT;
				LogWrite(-1, LOGERROR,
					CString("ERROR: \"") + job_code + "\" har overskredet tidsfristen og stoppes");
			}
			break;
		case JOB_RUNNING:
			// check for warning
			{
				CTime warning = active_time + warning_time;
				if (currenttime > warning) job_type = JOB_RUNNING_WARNING;
			}
			break;
		case JOB_RUNNING_WARNING:
			LogWrite(-1, LOGERROR,
				CString("WARNING: \"") + job_code + "\" koerer, men er forsinket");
			break;
		case JOB_TIMEOUT:
		case JOB_ERROR:
		case JOB_SUCCES:
			{
				if (job_status == JOB_SUCCES) followjobs.ActivateJobs();
				CTime reset = active_time + reset_time;
				if (currenttime > reset) {
					job_status = JOB_READY;
					LogWrite(-1, LOGNORMAL,
						CString("Jobbet \"") + job_code + "\" reset'es og er klar igen");
				}
			}
			break;
		default:
			break;
		}
	} else if (job_type == EVENTJOB) {
		switch (job_status) {
		case JOB_READY:
			{
				CFileFind filefind;
				if (filefind.FindFile(file_pattern)) {
					while (filefind.FindNextFile()) {
						if ((filefind.IsDots() == 0) && (filefind.IsDirectory() == 0)) break;
					}
					if ((filefind.IsDots() == 0) && (filefind.IsDirectory() == 0)) {
						current_file = filefind.GetFilePath();
						if (mode != SERVICE_STOP_INSERTED) {
							job_status = JOB_ACTIVE;
							LogWrite(-1, LOGNORMAL,
								CString("Jobbet \"") + job_code + "\" saettes i koe, da der er kommet en fil");
						}
					}
				}
				filefind.Close();
			}
			break;
		case JOB_ACTIVE_WAIT:
		case JOB_ACTIVE_WAIT_WARNING:
		case JOB_ACTIVE:
			break;
		case JOB_ACTIVE_WARNING:
		case JOB_RUNNING:
			break;
		case JOB_RUNNING_WARNING:
		case JOB_TIMEOUT:
		case JOB_ERROR:
		case JOB_SUCCES:
			if (job_status == JOB_SUCCES) followjobs.ActivateJobs();
			job_status = JOB_READY;
			LogWrite(-1, LOGNORMAL,
				CString("Jobbet \"") + job_code + "\" reset'es og er klar igen");
			break;
		default:
			break;
		}
	} else if (job_type == MANUALJOB) {
		switch (job_status) {
		case JOB_READY:
			break;
		case JOB_ACTIVE_WAIT:
		case JOB_ACTIVE_WAIT_WARNING:
		case JOB_ACTIVE:
			break;
		case JOB_ACTIVE_WARNING:
		case JOB_RUNNING:
			break;
		case JOB_RUNNING_WARNING:
		case JOB_TIMEOUT:
		case JOB_ERROR:
		case JOB_SUCCES:
			if (job_status == JOB_SUCCES) followjobs.ActivateJobs();
			job_status = JOB_READY;
			LogWrite(-1, LOGNORMAL,
				CString("Jobbet \"") + job_code + "\" reset'es og er klar igen");
			break;
		default:
			break;
		}
	}
	return(job_status);
}

void MFAQSJOB::SetJobStatus(int que_status)
{
	switch (que_status) {
	case QUE_FOLLOW_ACTIVE:
		job_status = JOB_FOLLOW_ACTIVE;
		break;
	case QUE_ACTIVE:
		job_status = JOB_ACTIVE;
		break;
	case QUE_RUNNING:
		job_status = JOB_RUNNING;
		last_run = CTime::GetCurrentTime();
		break;
	case QUE_ERROR:
		job_status = JOB_ERROR;
		job_runs++;
		break;
	case QUE_SUCCES:
		job_status = JOB_SUCCES;
		job_runs++;
		break;
	}
}

int MFAQSJOB::GetJobStatus()
{
	return(job_status);
}

LPTSTR MFAQSJOB::GetJobCode()
{
	return(job_code);
}

LPTSTR MFAQSJOB::GetJobCode(LPTSTR get_comment)
{
	strcpy(get_comment, comment);
	return(job_code);
}

LPTSTR MFAQSJOB::GetManager()
{
	return(job_manager);
}

LPTSTR MFAQSJOB::GetJobParameter(int parmidx)
{
	if (job_type == TIMEEVENTJOB || job_type == EVENTJOB) {
		return (parameter.GetParm(parmidx+1, current_file));
	} else {
		return (parameter.GetParm(parmidx+1));
	}
}




void MFAQSJOB::LogWriteNewJob(int partion, int mode, LPCTSTR text)
{
	currentjoblog = new JobLogInfo;
	histlog.InsertJobLog(currentjoblog);

	CString logstr = (CTime::GetCurrentTime()).Format("%H:%M:%S");

	if (partion >= 0) {
		TCHAR temp_str[10];
		sprintf(temp_str, " (%i) ", partion+1);
		logstr += temp_str;
		if (mode == LOGNORMAL) LPNORM(partion, text)
		else LPERR(partion, text)
	} else {
		logstr += " ";
		if (mode == LOGNORMAL) LNORM(text)
		else LERR(text)
	}
	logstr += text;
//	currentjoblog->loginfo.Add(logstr);
}

void MFAQSJOB::LogWriteNewJob(int partion, int mode, CString& text)
{
	currentjoblog = new JobLogInfo;
	histlog.InsertJobLog(currentjoblog);

	CString logstr = (CTime::GetCurrentTime()).Format("%H:%M:%S");

	if (partion >= 0) {
		TCHAR temp_str[10];
		sprintf(temp_str, " (%i) ", partion+1);
		logstr += temp_str;
		if (mode == LOGNORMAL) LPNORM(partion, text)
		else LPERR(partion, text)
	} else {
		logstr += " ";
		if (mode == LOGNORMAL) LNORM(text)
		else LERR(text)
	}
	logstr += text;
//	currentjoblog->loginfo.Add(logstr);
}

void MFAQSJOB::LogWrite(int partion, int mode, LPCTSTR text)
{
	CString logstr = (CTime::GetCurrentTime()).Format("%H:%M:%S");

	if (partion >= 0) {
		TCHAR temp_str[10];
		sprintf(temp_str, " (%i) ", partion+1);
		logstr += temp_str;
		if (mode == LOGNORMAL) LPNORM(partion, text)
		else LPERR(partion, text)
	} else {
		logstr += " ";
		if (mode == LOGNORMAL) LNORM(text)
		else LERR(text)
	}
	logstr += text;
//	currentjoblog->loginfo.Add(logstr);
}

void MFAQSJOB::LogWrite(int partion, int mode, CString& text)
{
	CString logstr = (CTime::GetCurrentTime()).Format("%H:%M:%S");

	if (partion >= 0) {
		TCHAR temp_str[10];
		sprintf(temp_str, " (%i) ", partion+1);
		logstr += temp_str;
		if (mode == LOGNORMAL) LPNORM(partion, text)
		else LPERR(partion, text)
	} else {
		logstr += " ";
		if (mode == LOGNORMAL) LNORM(text)
		else LERR(text)
	}
	logstr += text;
//	currentjoblog->loginfo.Add(logstr);
}

void MFAQSJOB::LogFollowActive()
{
	LogWrite(-1, LOGNORMAL,
		CString("Jobbet \"") + job_code + "\" saettes i koe - aktiveret af andet job");
}

void MFAQSJOB::Serialize( /* CArchive& ar */ )
{
//	frequence.Serialize( );
//	dependency.Serialize( );
	followjobs.Serialize( );
	parameter.Serialize( );

//	if ( ar.IsStoring() ) {
//		ar << CString(job_code);
//		ar << CString(comment);
//		ar << job_type;
//		ar << start_time;
//		ar << warning_time;
//		ar << timeout_time;
//		ar << reset_time;
//		ar << max_interval;
//		ar << CString(file_pattern);
//		ar << CString(job_manager);
//    } else {
		CString temp_str;
//		ar >> temp_str;
//		strcpy(job_code, temp_str);
		strcpy(job_code, (LPCTSTR) current->cJobcode);
//		ar >> temp_str;
//		strcpy(comment, temp_str);
		strcpy(comment, (LPCTSTR) current->cComment);
//		ar >> job_type;
		if (current->cType == 0) job_type = EVENTJOB;
		else job_type = MANUALJOB;
//		ar >> start_time;
//		ar >> warning_time;
//		ar >> timeout_time;
//		ar >> reset_time;
//		ar >> max_interval;
//		ar >> temp_str;
//		strcpy(file_pattern, temp_str);
		strcpy(file_pattern, (LPCTSTR) current->cPattern);
//		ar >> temp_str;
//		strcpy(job_manager, temp_str);
		strcpy(job_manager, (LPCTSTR) current->cManager);
//	}
//	LNORM(CString("Jobbet ") + job_code + " indlaest i scheduler");
}





JOB_SCHEDULE::JOB_SCHEDULE()
{
	for (int i = 0; i < 10000; i++) job_ptr[i] = NULL;
	no_jobs = 0;
	LNORM("..opretter og starter jobkoer");
	for (int i = 0; i < 5; i++) que[i] = new JOBQUE;
}

JOB_SCHEDULE::~JOB_SCHEDULE()
{
	for (int i = 0; i < 10000; i++) if (job_ptr[i] != NULL) delete job_ptr[i];
	for (int i = 0; i < 5; i++) delete que[i];
	LNORM("..job-koer stoppet");
}

void JOB_SCHEDULE::LockJobs()
{
	for (int i = 0; i < no_jobs; i++) job_ptr[i]->LockJob();
}

void JOB_SCHEDULE::UnlockJobs()
{
	for (int i = 0; i < no_jobs; i++) job_ptr[i]->UnLockJob();
}

void JOB_SCHEDULE::InsertJob(MFAQSJOB *newjob)
{
	job_ptr[no_jobs++] = newjob;
}

void JOB_SCHEDULE::DeleteJob(int jobhandle)
{
	for (int i = 0; i < 5; i++) que[i]->LockQue();
	LockJobs();
	if (job_ptr[jobhandle]->GetJobStatus() == JOB_RUNNING) {
		cout << "FEJL: jobbet koerer og kan derfor ikke slettes !!!" << endl << flush;
	} else {
		delete job_ptr[jobhandle];
		for (int i = jobhandle + 1; i < no_jobs; i++) job_ptr[i-1] = job_ptr[i];
		for (int i = 0; i < 5; i++) {
			que[i]->DeleteJob(jobhandle);
			que[i]->HandleAdjust(jobhandle);
		}
		job_ptr[--no_jobs] = NULL;
	}
	UnlockJobs();
	for (int i = 0; i < 5; i++) que[i]->UnLockQue();
}

int JOB_SCHEDULE::DisplayJobs()
{
	for (int i = 0; i < no_jobs; i++) {
		job_ptr[i]->DisplayJobShort();
		cout << endl;
	}
	cout << flush;
	return(no_jobs);
}

void JOB_SCHEDULE::DisplayJob(int handle)
{
	job_ptr[handle]->DisplayJobLong();
	cout << flush;
}

JOBQUE *JOB_SCHEDULE::GetJobQue(int que_no)
{
	return(que[que_no]);
}

int JOB_SCHEDULE::FindJob(LPCTSTR findjob)
{
	for (int i = 0; i < 10000; i++) if (job_ptr[i] != NULL)
		if (strcmp(job_ptr[i]->GetJobCode(), findjob) == 0) return (i);
	return(-1);
}

int JOB_SCHEDULE::GetJobStatus(int handle)
{
	return(job_ptr[handle]->GetJobStatus());
}

int JOB_SCHEDULE::StatusUpdate(int mode)
{
	int i, prev_status, new_status;
	int retval = TRUE;

	for (i = 0; i < 5; i++) que[i]->LockQue();
	for (i = 0; i < no_jobs; i++) job_ptr[i]->LockJob();

	for (i = 0; i < 5; i++) que[i]->GetJobStatus();
	for (i = 0; i < no_jobs; i++) {
		prev_status = job_ptr[i]->GetJobStatus();
		if (prev_status == JOB_FOLLOW_ACTIVE) {
			job_ptr[i]->LogFollowActive();
			int insert_que = GetQue(job_ptr[i]->GetManager());
			if (insert_que >= 0) {
				job_ptr[i]->SetJobStatus(QUE_ACTIVE);
				que[GetQue(job_ptr[i]->GetManager())]->InsertJob(i, job_ptr[i]);
			} else {
				LERR(CString("ERROR: service modulet \"") + job_ptr[i]->GetManager() + "\" findes ikke");
				job_ptr[i]->SetJobStatus(QUE_ERROR);
			}
		} else {
			new_status = job_ptr[i]->UpdateJobStatus(mode);
			switch(prev_status) {
			case JOB_READY:
			case JOB_ACTIVE_WAIT:
			case JOB_ACTIVE_WAIT_WARNING:
				if (new_status == JOB_ACTIVE || new_status == JOB_ACTIVE_WARNING) {
					int insert_que = GetQue(job_ptr[i]->GetManager());
					if (insert_que >= 0)
						que[GetQue(job_ptr[i]->GetManager())]->InsertJob(i, job_ptr[i]);
					else {
						LERR(CString("ERROR: service modulet \"") + job_ptr[i]->GetManager() + "\" findes ikke");
						job_ptr[i]->SetJobStatus(QUE_ERROR);
					}
				}

// hvis timeout fjern job fra køer

				break;
			case JOB_RUNNING:
			case JOB_RUNNING_WARNING:
			case JOB_TIMEOUT:
			case JOB_ERROR:
			case JOB_SUCCES:
				break;
			default:
				break;
			}
			if (prev_status != JOB_TIMEOUT && new_status == JOB_TIMEOUT)
				for (int j = 0; j < 5; j++) que[j]->DeleteJob(i);
		}
	}

	if (mode == SERVICE_BREAK_INSERTED) {
		for (i = 0; i < 5; i++) {
			if (que[i]->JobServiceStatus() == FALSE) retval = FALSE;
		}
	} else if (mode == SERVICE_STOP_INSERTED) {
		for (i = 0; i < 5; i++) {
			if (que[i]->QueEmptyStatus() == FALSE) retval = FALSE;
		}
	}

	int queues_empty = TRUE;
	for (i = 0; i < 5; i++) {
		if (que[i]->QueEmptyStatus()  == FALSE) queues_empty = FALSE;
	}
	if ( queues_empty == TRUE ) {
		if (CheckConf() == 1) {
			if (ReadConfig() == 1) {
				for (int i = 0; i < 10000; i++) if (job_ptr[i] != NULL) delete job_ptr[i];
				for (i = 0; i < 10000; i++) job_ptr[i] = NULL;
				no_jobs = 0;
				current = root;
				while (current != NULL) {
					job_ptr[no_jobs] = new MFAQSJOB( this );
					job_ptr[no_jobs]->Serialize( );
					no_jobs++;
					current = current->next;
				}
			}
		}
	}
	for (i = no_jobs - 1; i >= 0; i--) job_ptr[i]->UnLockJob();
	for (i = 4; i >= 0; i--) que[i]->UnLockQue();

	return(retval);
}

MFAQSJOB *JOB_SCHEDULE::GetJob(int handle)
{
	return(job_ptr[handle]);
}

void JOB_SCHEDULE::InsertServiceBreak()
{
	int i;
	for (i = 0; i < 5; i++) que[i]->LockQue();
	for (i = 0; i < no_jobs; i++) job_ptr[i]->LockJob();

	for (i = 0; i < 5; i++) que[i]->InsertServiceBreak();

	for (i = no_jobs - 1; i >= 0; i--) job_ptr[i]->UnLockJob();
	for (i = 4; i >= 0; i--) que[i]->UnLockQue();

}

void JOB_SCHEDULE::RemoveServiceBreak()
{
	int i;
	for (i = 0; i < 5; i++) que[i]->LockQue();
	for (i = 0; i < no_jobs; i++) job_ptr[i]->LockJob();

	for (i = 0; i < 5; i++) que[i]->RemoveServiceBreak();

	for (i = no_jobs - 1; i >= 0; i--) job_ptr[i]->UnLockJob();
	for (i = 4; i >= 0; i--) que[i]->UnLockQue();

}

void JOB_SCHEDULE::SetOptions(CTimeSpan& dailyspan, CString& notesmail, CTimeSpan& logerrorspan)
{
	daily_log_time = dailyspan;
	notesmail_list = notesmail;
	error_mail_span = logerrorspan;
}

void JOB_SCHEDULE::Serialize( CArchive& ar )
{
	JOB_MANAGERS::Serialize( ar );

	if ( ar.IsStoring() ) {
		ar << daily_log_time;
		ar << notesmail_list;
		ar << error_mail_span;
//		ar << no_jobs;
//		for (int i = 0; i < no_jobs; i++) job_ptr[i]->Serialize( ar );
    } else {
		ar >> daily_log_time;
		ar >> notesmail_list;
		ar >> error_mail_span;
//		ar >> no_jobs;
		if (ReadConfig() == 1) {
			LNORM("MFAQS.INI indlaest")
		} else {
			LERR("FEJL: kunne ikke laese MFAQS.INI")
		}
		current = root;
		int i = 0;
		while (current != NULL) {
//		for (int i = 0; i < no_jobs; i++) {
			job_ptr[i] = new MFAQSJOB( this );
			job_ptr[i]->Serialize(  );
			i++;
			current = current->next;
		}
		no_jobs = i;
	}
}


JOBQUE::JOBQUE()
{
	no_jobs = 0;

	SECURITY_ATTRIBUTES mutexatt;
	mutexatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutexatt.lpSecurityDescriptor = NULL;
	mutexatt.bInheritHandle = TRUE;
	mutexhandle = CreateMutex(&mutexatt, FALSE, NULL);
}

JOBQUE::~JOBQUE()
{
	CloseHandle(mutexhandle);
}

void JOBQUE::LockQue()
{
	WaitForSingleObject(mutexhandle, 10000);
}

void JOBQUE::UnLockQue()
{
	ReleaseMutex(mutexhandle);
}

void JOBQUE::InsertJob(int insert_handle, MFAQSJOB *insert_job)
{
	jobs[no_jobs].handle = insert_handle;
	jobs[no_jobs].job_ptr = insert_job;
	jobs[no_jobs++].status = QUE_ACTIVE;
}

void JOBQUE::DeleteJob(int delete_handle)
{
	int fundet = FALSE;
	for (int i = 0; i < no_jobs; i++) {
		if (fundet == TRUE) {
			jobs[i-1].handle = jobs[i].handle;
			jobs[i-1].job_ptr = jobs[i].job_ptr;
			jobs[i-1].status = jobs[i].status;
		} else {
			if (jobs[no_jobs].handle == delete_handle) fundet = TRUE;
		}
	}
	if (fundet == TRUE) no_jobs--;
}

void JOBQUE::HandleAdjust(int delete_handle)
{
	int fundet = FALSE;
	for (int i = 0; i < no_jobs; i++) {
		if (jobs[i].handle >= delete_handle) jobs[i].handle--;
	}
}

void JOBQUE::InsertServiceBreak()
{
	if (no_jobs > 0) {
		if (jobs[0].status != QUE_ACTIVE) {
			for (int i = no_jobs; i > 1; i--) {
				jobs[i].handle = jobs[i-1].handle;
				jobs[i].job_ptr = jobs[i-1].job_ptr;
				jobs[i].status = jobs[i-1].status;
			}
			no_jobs++;
			jobs[1].handle = QUE_SERVICE_HANDLE;
		} else {
			for (int i = no_jobs; i > 0; i--) {
				jobs[i].handle = jobs[i-1].handle;
				jobs[i].job_ptr = jobs[i-1].job_ptr;
				jobs[i].status = jobs[i-1].status;
			}
			no_jobs++;
			jobs[0].handle = QUE_SERVICE_HANDLE;
		}
	} else {
		jobs[no_jobs++].handle = QUE_SERVICE_HANDLE;
	}
}

void JOBQUE::RemoveServiceBreak()
{
	int found = FALSE, i;

	for (i = 0; i < no_jobs; i++) {
		if (found == FALSE) {
			if (jobs[0].handle == QUE_SERVICE_HANDLE) found = TRUE;
		} else {
			jobs[i-1].handle = jobs[i].handle;
			jobs[i-1].job_ptr = jobs[i].job_ptr;
			jobs[i-1].status = jobs[i].status;
		}
	}
	if (found == TRUE) no_jobs--;
}

MFAQSJOB *JOBQUE::StartJob()
{
	MFAQSJOB *retjob = NULL;
	if (no_jobs > 0) {
		if (jobs[0].handle >= 0 && jobs[0].status == QUE_ACTIVE) {
			jobs[0].status = QUE_RUNNING;
			jobs[0].job_ptr->SetJobStatus(QUE_RUNNING);
			retjob = jobs[0].job_ptr;
		}
	}
	return (retjob);
}

void JOBQUE::SetReturnCode(int retcode)
{
	jobs[0].status = retcode;
}

int JOBQUE::GetJobStatus()
{
	int rethandle;
	if (no_jobs > 0) {
		if (jobs[0].handle != QUE_SERVICE_HANDLE &&
			(jobs[0].status == QUE_ERROR || jobs[0].status == QUE_SUCCES)) {
			jobs[0].job_ptr->SetJobStatus(jobs[0].status);
			rethandle = jobs[0].handle;
			for (int i = 1; i < no_jobs; i++) {
				jobs[i-1].handle = jobs[i].handle;
				jobs[i-1].job_ptr = jobs[i].job_ptr;
				jobs[i-1].status = jobs[i].status;
			}
			no_jobs--;
		} else {
			if (jobs[0].handle == QUE_SERVICE_HANDLE) rethandle = QUE_SERVICE_HANDLE;
			else rethandle = QUE_BUSY;
		}
	} else {
		rethandle = QUE_EMPTY;
	}
	return(rethandle);
}

int JOBQUE::JobServiceStatus()
{
	if (no_jobs > 0 && jobs[0].handle == QUE_SERVICE_HANDLE) return(TRUE);
	else return (FALSE);
}

int JOBQUE::QueEmptyStatus()
{
	if (no_jobs == 0) return(TRUE);
	else return(FALSE);
}

QUE_JOBS *JOBQUE::GetQueStatus()
{
	QUE_JOBS *retque;
	retque = new QUE_JOBS;
	retque->no_jobs = no_jobs;
	for (int i = 0; i < no_jobs; i++) {
		retque->handle[i] = jobs[i].handle;
		retque->status[i] = jobs[i].status;
	}
	return(retque);
}


JOB_MANAGER::JOB_MANAGER()
{
}

JOB_MANAGER::JOB_MANAGER(LPCTSTR set_manager, int que1, int que2, int que3, int que4, int que5)
{
	strcpy(manager, set_manager);
	que[0] = que1; que[1] = que2; que[2] = que3; que[3] = que4; que[4] = que5;
	if (que[4]) last_que = 4;
	else if (que[3]) last_que = 3;
	else if (que[2]) last_que = 2;
	else if (que[1]) last_que = 1;
	else last_que = 0;
}


int JOB_MANAGER::GetQue()
{
	if (que[0] == FALSE && que[1] == FALSE && que[2] == FALSE &&
		que[3] == FALSE && que[4] == FALSE) return(-1);
	int found = FALSE;
	int index = last_que + 1;
	while (found == FALSE) {
		if (index > 4) index = 0;
		if (que[index]) found = TRUE;
		else index++;
	}
	return (index);
}

LPTSTR JOB_MANAGER::GetManagerName()
{
	return(manager);
}

void JOB_MANAGER::AddQue(int newque)
{
	que[newque] = TRUE;
}

void JOB_MANAGER::RemoveQue(int removeque)
{
	que[removeque] = FALSE;
}

void JOB_MANAGER::Serialize( CArchive& ar )
{
	int i;
	if ( ar.IsStoring() ) {
		ar << CString(manager);
		for (i = 0; i < 5; i++) {
			ar << que[i];
		}
		ar << last_que;
    } else {
		CString temp_str;
		ar >> temp_str;
		strcpy(manager, temp_str);
		for (i = 0; i < 5; i++) {
			ar >> que[i];
		}
		ar >> last_que;
	}
}

JOB_MANAGERS::JOB_MANAGERS()
{
	no_manager = 0;
}

JOB_MANAGERS::~JOB_MANAGERS()
{
	for (int i = 0; i < no_manager; i++) delete managers[i];
}

void JOB_MANAGERS::InsertManager(LPCTSTR set_manager, int que1, int que2, int que3, int que4, int que5)
{
	managers[no_manager++] = new JOB_MANAGER(set_manager, que1, que2, que3, que4, que5);
}

void JOB_MANAGERS::InsertManager(LPCTSTR set_manager, int que)
{
	int i, found = FALSE;
	for (i =  0; i < no_manager; i++) {
		if (strcmp(set_manager, managers[i]->GetManagerName()) == 0) {
			managers[i]->AddQue(que);
			found = TRUE;
		}
	}
	if (found == FALSE) {
		switch (que) {
		case 0:
			managers[no_manager++] = new JOB_MANAGER(set_manager, TRUE, FALSE, FALSE, FALSE, FALSE);
			break;
		case 1:
			managers[no_manager++] = new JOB_MANAGER(set_manager, FALSE, TRUE, FALSE, FALSE, FALSE);
			break;
		case 2:
			managers[no_manager++] = new JOB_MANAGER(set_manager, FALSE, FALSE, TRUE, FALSE, FALSE);
			break;
		case 3:
			managers[no_manager++] = new JOB_MANAGER(set_manager, FALSE, FALSE, FALSE, TRUE, FALSE);
			break;
		case 4:
			managers[no_manager++] = new JOB_MANAGER(set_manager, FALSE, FALSE, FALSE, FALSE, TRUE);
			break;
		}
	}
}

void JOB_MANAGERS::RemoveManager(LPCTSTR set_manager, int que)
{
	int found = FALSE, idx;
	for (int i =  0; i < no_manager; i++) {
		if (strcmp(set_manager, managers[i]->GetManagerName()) == 0) {
			managers[i]->RemoveQue(que);
			found = TRUE; idx = i;
		}
	}
	if (found && GetQue(set_manager) == -1) {
		for (int i = idx; i < no_manager-1; i++) {
			managers[i] = managers[i+1];
		}
		no_manager--;
	}
}

int JOB_MANAGERS::GetQue(LPCTSTR get_manager)
{
	int i, found = FALSE, retval = -1;

	for (i = 0; i < no_manager; i++)
		if (strcmp(managers[i]->GetManagerName(), get_manager) == 0)
			return (managers[i]->GetQue());
	return(-1);
}

void JOB_MANAGERS::Serialize( CArchive& ar )
{
	if ( ar.IsStoring() ) {
		ar << no_manager;
		for (int i = 0; i < no_manager; i++) managers[i]->Serialize( ar );
    } else {
		ar >> no_manager;
		for (int i = 0; i < no_manager; i++) {
			managers[i] = new JOB_MANAGER;
			managers[i]->Serialize( ar );
		}
	}
}
