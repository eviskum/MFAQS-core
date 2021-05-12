#ifndef _MFAQSLOG_H_
#define _MFAQSLOG_H_


#include <afx.h>

#define LOGNORMAL	1
#define LOGERROR	2

#define LNORM( txt ) logg->WriteLog(LOGNORMAL, txt);
#define LERR( txt ) logg->WriteLog(LOGERROR, txt);
#define LPNORM( part, txt ) logg->WriteLogPart(part+1, LOGNORMAL, txt);
#define LPERR( part, txt ) logg->WriteLogPart(part+1, LOGERROR, txt);

class MFAQSLog
{
private:
	CTime lastlog;
	int isOpen;
	CString masterfile;
	CString errorfile;
	CStdioFile masterlog;
	CStdioFile errorlog;
	HANDLE logmutex;
	void OpenFiles();
	int console;
public:
	MFAQSLog();
	~MFAQSLog();
	void WriteLog(int infotype, LPCTSTR logtext);
	void WriteLog(int infotype, CString& logtext);
	void WriteLogPart(int partion, int infotype, LPCTSTR logtext);
	void WriteLogPart(int partion, int infotype, CString& logtext);
	void EnableConsole();
	void DisableConsole();
};


extern MFAQSLog *logg;


#endif