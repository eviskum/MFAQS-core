// MFAQS (MiniFAQS til NT)
// Esben Viskum, AO, maj 1999
// Modellering af et MFAQS-job/kø.

#ifndef _MFAQSJOB_H_
#define _MFAQSJOB_H_


#include <afx.h>
#include <afxtempl.h>

#define TIMEJOB			1
#define EVENTJOB		2
#define TIMEEVENTJOB	3
#define MANUALJOB		4

#define	RUNONCE			1
#define RUNDAILY		2
#define RUNMONTHLY		3

#define JOB_READY				1
#define JOB_ACTIVE_WAIT			2
#define JOB_ACTIVE_WAIT_WARNING	3
#define JOB_ACTIVE				4
#define JOB_ACTIVE_WARNING		5
#define JOB_RUNNING				6
#define JOB_RUNNING_WARNING		7
#define JOB_TIMEOUT				8
#define JOB_ERROR				9
#define JOB_SUCCES				10
#define JOB_FOLLOW_ACTIVE		11

#define QUE_ACTIVE			1
#define QUE_RUNNING			2
#define QUE_ERROR			3
#define QUE_SUCCES			4
#define QUE_FOLLOW_ACTIVE	5
#define QUE_SERVICE_HANDLE	-1
#define QUE_EMPTY			-2
#define QUE_BUSY			-3

#define SERVICE_NORMAL			1
#define SERVICE_BREAK_INSERTED	2
#define SERVICE_STOP_INSERTED	3


class FREQUENCE
{
private:
	int freq_type;
	unsigned char days[31];
public:
	FREQUENCE();
	FREQUENCE(int init_freq_type);
	void Serialize( CArchive& ar );
	void Display();
	void SetDay(int day, int val);
	int GetDay(int day);
	int SetFreqType( int init_freq_type );
	int GetFreqType();
};

class DEPENDENCY
{
private:
	TCHAR dependency_job[10][9];
	void *job_schedule;
public:
	DEPENDENCY();
	DEPENDENCY(void *set_schedule);
	void Serialize( CArchive& ar );
	void Display();
	void SetDependency_job(int job_index);
	void SetDependency_job(int job_index, LPCTSTR job_code);
	LPCTSTR GetDependency_job(int job_index);
	int check_dependency();
	int check_dependency(int job_index);
};

class FOLLOWJOBS
{
private:
	TCHAR follow_job[10][9];
	void *job_schedule;
public:
	FOLLOWJOBS();
	FOLLOWJOBS(void *set_schedule);
	void Serialize( /* CArchive& ar */ );
	void Display();
	void SetFollow_job(int job_index);
	void SetFollow_job(int job_index, LPCTSTR job_code);
	void ActivateJobs();
};

class JOBPARAMETER
{
private:
	TCHAR parameter[5][100];
	TCHAR temp_str[100];
public:
	JOBPARAMETER();
	void Serialize( /* CArchive& ar */ );
	void Display();
	void SetParm(int parm_index, LPCTSTR parmval);
	LPTSTR GetParm(int parm_index);
	LPTSTR GetParm(int parm_index, LPCTSTR parmval);
};

class JobLogInfo
{
public:
	CArray<CString,CString> loginfo;
};

class JobHistLogInfo
{
private:
	int no_jobs;
	JobLogInfo *job_array[10];
public:
	JobHistLogInfo();
	~JobHistLogInfo();
	void InsertJobLog(JobLogInfo *loginfo);
	int no_Jobs();
	JobLogInfo *GetJobLog(int index);
};

class MFAQSJOB
{
private:
	TCHAR job_code[9];
	TCHAR comment[256];
	int job_type;
	CTime start_time;
	CTimeSpan warning_time;
	CTimeSpan timeout_time;
	CTimeSpan reset_time;
	FREQUENCE frequence;
	CTimeSpan max_interval;
	TCHAR file_pattern[100];
	DEPENDENCY dependency;
	FOLLOWJOBS followjobs;
	TCHAR job_manager[100];
	JOBPARAMETER parameter;

	CTime last_run;
	CTime active_time;
	int job_status;
	int job_runs;
	CString current_file;


	void *job_schedule;

	JobLogInfo *currentjoblog;
	JobHistLogInfo histlog;

	HANDLE jobmutex;

public:
	MFAQSJOB();
	MFAQSJOB(void *set_job_schedule);
	MFAQSJOB(void *set_job_schedule, LPCTSTR set_job_code, LPCTSTR set_comment);
	~MFAQSJOB();
	void Serialize( /* CArchive& ar */ );
	void LockJob();
	void UnLockJob();
	void DisplayJobShort();
	void DisplayJobLong();
	void SetTimeInfo(int start_year, int start_month, int start_day, int start_hour, int start_min,
					  int warning_hour = 3, int warning_min = 0, int timeout_hour = 6, int timeout_min = 0,
					  int reset_hour = 12, int reset_min = 0);
	void SetJobType(int set_job_type, LPCTSTR set_job_manager, LPCTSTR parm1 = NULL,
			LPCTSTR parm2 = NULL, LPCTSTR parm3 = NULL, LPCTSTR parm4 = NULL, LPCTSTR parm5 = NULL);
	void SetDependency(LPCTSTR job1 = NULL, LPCTSTR job2 = NULL, LPCTSTR job3 = NULL,
							 LPCTSTR job4 = NULL, LPCTSTR job5 = NULL, LPCTSTR job6 = NULL,
							 LPCTSTR job7 = NULL, LPCTSTR job8 = NULL, LPCTSTR job9 = NULL,
							 LPCTSTR job10 = NULL);
	void SetFollowjob(LPCTSTR job1 = NULL, LPCTSTR job2 = NULL, LPCTSTR job3 = NULL,
							 LPCTSTR job4 = NULL, LPCTSTR job5 = NULL, LPCTSTR job6 = NULL,
							 LPCTSTR job7 = NULL, LPCTSTR job8 = NULL, LPCTSTR job9 = NULL,
							 LPCTSTR job10 = NULL);
	void SetFrequence(int freq_type, int no_days, int days[]);
	void SetFilePattern(LPCTSTR set_pattern, int max_days = 1, int max_hour = 0, int max_min = 0);
	int UpdateJobStatus(int mode);
	void SetJobStatus(int que_status);
	int GetJobStatus();
	LPTSTR GetJobCode();
	LPTSTR GetJobCode(LPTSTR get_comment);
	LPTSTR GetManager();
	LPTSTR GetJobParameter(int parmidx);
	void LogWriteNewJob(int partion, int mode, LPCTSTR text);
	void LogWriteNewJob(int partion, int mode, CString& text);
	void LogWrite(int partion, int mode, LPCTSTR text);
	void LogWrite(int partion, int mode, CString& text);
	void LogFollowActive();
};


typedef struct {
	int handle[1000];
	int status[1000];
	int no_jobs;
} QUE_JOBS;

class JOBQUE
{
private:
	HANDLE mutexhandle;
	struct {
		int handle;
		MFAQSJOB *job_ptr;
		int status;
	} jobs[1000];
	int no_jobs;
public:
	JOBQUE();
	~JOBQUE();
	void LockQue();
	void UnLockQue();
	void InsertJob(int insert_handle, MFAQSJOB *insert_job);
	void DeleteJob(int delete_handle);
	void HandleAdjust(int delete_handle);
	void InsertServiceBreak();
	void RemoveServiceBreak();
	MFAQSJOB *StartJob();
	void SetReturnCode(int retcode);
	int GetJobStatus();
	int JobServiceStatus();
	QUE_JOBS *GetQueStatus();
	int QueEmptyStatus();
};


class JOB_MANAGER
{
private:
	TCHAR manager[100];
	int que[5];
	int last_que;
public:
	JOB_MANAGER();
	JOB_MANAGER(LPCTSTR set_manager, int que1, int que2, int que3, int que4, int que5);
	void Serialize( CArchive& ar );
	LPTSTR GetManagerName();
	int GetQue();
	void AddQue(int newque);
	void RemoveQue(int removeque);
};


class JOB_MANAGERS
{
private:
	JOB_MANAGER *managers[100];
	int no_manager;
public:
	JOB_MANAGERS();
	~JOB_MANAGERS();
	void Serialize( CArchive& ar );
	void InsertManager(LPCTSTR set_manager, int que1, int que2, int que3, int que4, int que5);
	void InsertManager(LPCTSTR set_manager, int que);
	void RemoveManager(LPCTSTR set_manager, int que);
	int GetQue(LPCTSTR get_manager);
};


class JOB_SCHEDULE : public JOB_MANAGERS
{
private:
	MFAQSJOB *job_ptr[10000];
	int no_jobs;
	JOBQUE *que[5];
	CTimeSpan daily_log_time;
	CString notesmail_list;
	CTimeSpan error_mail_span;
public:
	JOB_SCHEDULE();
	~JOB_SCHEDULE();
	void LockJobs();
	void UnlockJobs();
	void Serialize( CArchive& ar );
	void SetOptions(CTimeSpan& dailyspan, CString& notesmail, CTimeSpan& logerrorspan);
	void InsertJob(MFAQSJOB *newjob);
	void DeleteJob(int jobhandle);
	int DisplayJobs();
	void DisplayJob(int handle);
	JOBQUE *GetJobQue(int que_no);
	int FindJob(LPCTSTR findjob);
	int GetJobStatus(int handle);
	int StatusUpdate(int mode);
	MFAQSJOB *GetJob(int handle);
	void InsertServiceBreak();
	void RemoveServiceBreak();
};


#endif