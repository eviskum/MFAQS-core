#ifndef _MFAQSCFG_H_
#define _MFAQSCFG_H_


#include <afx.h>


class JobDef {
public:
	JobDef();

public:
	CString Jobcode;
	CString Comment;
	CString Type;
	CString Pattern;
	CString Jobmanager;
	CString Followjobs;
	CString Parameters;

public:
	CString cJobcode;
	CString cComment;
	int cType;
	CString cPattern;
	CString cManager;
	CString cFollowjob1;
	CString cFollowjob2;
	CString cFollowjob3;
	CString cFollowjob4;
	CString cFollowjob5;
	CString cFollowjob6;
	CString cFollowjob7;
	CString cFollowjob8;
	CString cParm1;
	CString cParm2;
	CString cParm3;
	CString cParm4;
	CString cParm5;

public:
	JobDef *next;
};


extern JobDef *root, *current;

int ReadConfig();
int CheckConf();

#endif