// MFAQS (MiniFAQS til NT)
// Esben Viskum, AO, maj 1999
// Indlæsning af konfiguration.

#include "StdAfx.h"
#include <afxtempl.h>
#include "mfaqscfg.h"
#include <fstream>
#include "mfaqslog.h"
using namespace std;

CArray<CString, CString> confdata;
CTime modTime;


JobDef::JobDef() {
	next = NULL;
}


JobDef *root, *current;


int ReadConfig() {
	CString tmp;
	confdata.RemoveAll();
	CStdioFile conffile;

	if (conffile.Open("MFAQS.INI", CFile::modeRead | CFile::shareDenyRead) == 0) {
		return(0);
	}
	while (conffile.ReadString(tmp)) if (tmp > " ") confdata.Add(tmp);
	conffile.Close();
	CFileFind filefind;
	if (filefind.FindFile("MFAQS.INI")) {
		filefind.FindNextFile();
		filefind.GetLastWriteTime( modTime );
	} else modTime = CTime::GetCurrentTime();
	filefind.Close();

	root = current = NULL;
	BOOL inMFAQSjob = FALSE;
	int idx;

	for (int i = 0; i < confdata.GetSize(); i++) {
		if (confdata[i].Left(1) == "[") {
			if (confdata[i].Find("[MFAQSJOB]") >= 0) {
				if (current == NULL) current = new JobDef();
				else { 
					current->next = new JobDef();
					current = current->next;
				}
				if (root == NULL) root = current;
				inMFAQSjob = TRUE;
			} else inMFAQSjob = FALSE;
		} else {
			if (inMFAQSjob) {
				if ((idx = confdata[i].Find("Jobcode=")) >= 0) {
					if (confdata[i].GetLength() > 8) current->Jobcode = confdata[i].Mid(8);
					current->Jobcode.TrimLeft(); current->Jobcode.TrimRight();
				} else if ((idx = confdata[i].Find("Comment=")) >= 0) {
					if (confdata[i].GetLength() > 8) current->Comment = confdata[i].Mid(8);
					current->Comment.TrimLeft(); current->Comment.TrimRight();
				} else if ((idx = confdata[i].Find("Type=")) >= 0) {
					if (confdata[i].GetLength() > 5) current->Type = confdata[i].Mid(5);
					current->Type.TrimLeft(); current->Type.TrimRight();
				} else if ((idx = confdata[i].Find("Pattern=")) >= 0) {
					if (confdata[i].GetLength() > 8) current->Pattern = confdata[i].Mid(8);
					current->Pattern.TrimLeft(); current->Pattern.TrimRight();
				} else if ((idx = confdata[i].Find("Jobmanager=")) >= 0) {
					if (confdata[i].GetLength() > 11) current->Jobmanager = confdata[i].Mid(11);
					current->Jobmanager.TrimLeft(); current->Jobmanager.TrimRight();
				} else if ((idx = confdata[i].Find("Followjobs=")) >= 0) {
					if (confdata[i].GetLength() > 11) current->Followjobs = confdata[i].Mid(11);
					current->Followjobs.TrimLeft(); current->Followjobs.TrimRight();
				} else if ((idx = confdata[i].Find("Parameters=")) >= 0) {
					if (confdata[i].GetLength() > 11) current->Parameters = confdata[i].Mid(11);
					current->Parameters.TrimLeft(); current->Parameters.TrimRight();
				}
			}
		}
	}

	CArray<CString,CString> tmpParm;
	CArray<CString,CString> tmpFollow;

	int no_jobs = 0;
	current = root;
	while (current != NULL) {
		current->cJobcode = current->Jobcode;
		current->cComment = current->Comment;
		current->cManager = current->Jobmanager;

		if (current->Type == "EVENT") current->cType = 0;
		else current->cType = 1;
		current->cPattern = current->Pattern;
		int sidx = 0, idx;
		tmpParm.RemoveAll();
		while ((current->Parameters.GetLength()-1 > sidx) &&
			((idx = current->Parameters.Find(",", sidx)) >= 0)) {
			tmpParm.Add(current->Parameters.Mid(sidx, idx-sidx));
			sidx = idx+1;
		}
		if (current->Parameters.GetLength()-1 >= sidx) tmpParm.Add(current->Parameters.Mid(sidx));
		sidx = 0;
		tmpFollow.RemoveAll();
		while ((current->Followjobs.GetLength()-1 > sidx) &&
			((idx = current->Followjobs.Find(",", sidx)) >= 0)) {
			tmpFollow.Add(current->Followjobs.Mid(sidx, idx-sidx));
			sidx = idx+1;
		}
		if (current->Followjobs.GetLength()-1 >= sidx)
			tmpFollow.Add(current->Followjobs.Mid(sidx));
		if (tmpParm.GetSize() > 0) current->cParm1 = tmpParm[0];
		if (tmpParm.GetSize() > 1) current->cParm2 = tmpParm[1];
		if (tmpParm.GetSize() > 2) current->cParm3 = tmpParm[2];
		if (tmpParm.GetSize() > 3) current->cParm4 = tmpParm[3];
		if (tmpParm.GetSize() > 4) current->cParm5 = tmpParm[4];
		if (tmpFollow.GetSize() > 0) current->cFollowjob1 = tmpFollow[0];
		if (tmpFollow.GetSize() > 1) current->cFollowjob2 = tmpFollow[1];
		if (tmpFollow.GetSize() > 2) current->cFollowjob3 = tmpFollow[2];
		if (tmpFollow.GetSize() > 3) current->cFollowjob4 = tmpFollow[3];
		if (tmpFollow.GetSize() > 4) current->cFollowjob5 = tmpFollow[4];
		if (tmpFollow.GetSize() > 5) current->cFollowjob6 = tmpFollow[5];
		if (tmpFollow.GetSize() > 6) current->cFollowjob7 = tmpFollow[6];
		if (tmpFollow.GetSize() > 7) current->cFollowjob8 = tmpFollow[7];
		current = current->next;
		no_jobs++;
	}
	TCHAR tmpstr[256];
	sprintf_s(tmpstr, "%d jobs indlaest fra MFAQS.INI", no_jobs);
	LNORM(tmpstr)
	return(1);
}

int CheckConf() {
	CTime fTime;
	CFileFind filefind;
	if (filefind.FindFile("MFAQS.INI")) {
		filefind.FindNextFile();
		filefind.GetLastWriteTime( fTime ) ;
		filefind.Close();
		if (fTime > modTime) return(1);
	} else filefind.Close();
	return(0);
}
