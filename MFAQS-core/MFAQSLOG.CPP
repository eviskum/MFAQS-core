// MFAQS Logning


#include "StdAfx.h"

// #include <afx.h>
#include <fstream>
#include "mfaqslog.h"
using namespace std;


MFAQSLog::MFAQSLog()
{
	isOpen = FALSE;
	console = TRUE;
	SECURITY_ATTRIBUTES mutexatt;
	mutexatt.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutexatt.lpSecurityDescriptor = NULL;
	mutexatt.bInheritHandle = TRUE;
	logmutex = CreateMutex(&mutexatt, FALSE, NULL);
}

MFAQSLog::~MFAQSLog()
{
	if (isOpen) {
		masterlog.Close();
		errorlog.Close();
	}
	CloseHandle(logmutex);
}

void MFAQSLog::OpenFiles()
{
	if (isOpen == FALSE) {
		lastlog = CTime::GetCurrentTime();
		masterfile = CString("LogFiles\\logfile.") + lastlog.Format("%Y%m%d");
		errorfile = CString("ErrorLog\\errorlog.") + lastlog.Format("%Y%m%d");
		masterlog.Open(masterfile, CFile::modeCreate | CFile::modeNoTruncate |
			CFile::modeReadWrite | CFile::shareDenyWrite);
		errorlog.Open(errorfile, CFile::modeCreate | CFile::modeNoTruncate |
			CFile::modeReadWrite | CFile::shareDenyWrite);
		masterlog.SeekToEnd(); errorlog.SeekToEnd();
		isOpen = TRUE;
	} else {
		lastlog = CTime::GetCurrentTime();
		if (masterfile != (CString("LogFiles\\logfile.") + lastlog.Format("%Y%m%d"))) {
			masterlog.Close(); errorlog.Close();
			masterfile = CString("LogFiles\\logfile.") + lastlog.Format("%Y%m%d");
			errorfile = CString("ErrorLog\\errorlog.") + lastlog.Format("%Y%m%d");
			masterlog.Open(masterfile, CFile::modeCreate | CFile::modeNoTruncate |
				CFile::modeReadWrite | CFile::shareDenyWrite);
			errorlog.Open(errorfile, CFile::modeCreate | CFile::modeNoTruncate |
				CFile::modeReadWrite | CFile::shareDenyWrite);
			masterlog.SeekToEnd(); errorlog.SeekToEnd();
			isOpen = TRUE;
		}
	}
}

void MFAQSLog::WriteLog(int infotype, LPCTSTR logtext)
{
	WaitForSingleObject(logmutex, INFINITE);
	OpenFiles();
	CString logtxt = (CTime::GetCurrentTime()).Format("%H:%M:%S> ") + logtext + "\n";
	if (console || infotype == LOGERROR) cout << logtxt << flush;
	masterlog.WriteString((LPCTSTR) logtxt);
	if (infotype == LOGERROR) errorlog.WriteString((LPCTSTR) logtxt);
	masterlog.Flush(); errorlog.Flush();
	ReleaseMutex(logmutex);
}

void MFAQSLog::WriteLog(int infotype, CString& logtext)
{
	WaitForSingleObject(logmutex, INFINITE);
	OpenFiles();
	CString logtxt = (CTime::GetCurrentTime()).Format("%H:%M:%S> ") + logtext + "\n";
	if (console || infotype == LOGERROR) cout << logtxt << flush;
	masterlog.WriteString((LPCTSTR) logtxt);
	if (infotype == LOGERROR) errorlog.WriteString((LPCTSTR) logtxt);
	masterlog.Flush(); errorlog.Flush();
	ReleaseMutex(logmutex);
}

void MFAQSLog::WriteLogPart(int partion, int infotype, LPCTSTR logtext)
{
	TCHAR temp_str[10];

	WaitForSingleObject(logmutex, INFINITE);
	if (partion > 10) sprintf(temp_str, "W%i", partion-10);
	else sprintf(temp_str, "%i", partion);
	OpenFiles();
	CString logtxt = (CTime::GetCurrentTime()).Format("%H:%M:%S (") + temp_str + ")> " + logtext + "\n";
	if (console || infotype == LOGERROR) cout << logtxt << flush;
	masterlog.WriteString((LPCTSTR) logtxt);
	if (infotype == LOGERROR) errorlog.WriteString((LPCTSTR) logtxt);
	masterlog.Flush(); errorlog.Flush();
	ReleaseMutex(logmutex);
}

void MFAQSLog::WriteLogPart(int partion, int infotype, CString& logtext)
{
	TCHAR temp_str[10];

	WaitForSingleObject(logmutex, INFINITE);
	if (partion > 10) sprintf(temp_str, "W%i", partion-10);
	else sprintf(temp_str, "%i", partion);
	OpenFiles();
	CString logtxt = (CTime::GetCurrentTime()).Format("%H:%M:%S (") + temp_str + ")> " + logtext + "\n";
	if (console || infotype == LOGERROR) cout << logtxt << flush;
	masterlog.WriteString((LPCTSTR) logtxt);
	if (infotype == LOGERROR) errorlog.WriteString((LPCTSTR) logtxt);
	masterlog.Flush(); errorlog.Flush();
	ReleaseMutex(logmutex);
}

void MFAQSLog::EnableConsole()
{
	WaitForSingleObject(logmutex, INFINITE);
	console = TRUE;
	ReleaseMutex(logmutex);
}

void MFAQSLog::DisableConsole()
{
	WaitForSingleObject(logmutex, INFINITE);
	console = FALSE;
	ReleaseMutex(logmutex);
}