#ifndef _MFAQSWRK_H_
#define _MFAQSWRK_H_


// #include <afx.h>
#include "mfaqsjob.h"
#include "mfaqslog.h"

int StartWorker(int part, int argc, TCHAR *argv[]);
int StopWorker(int part);

#endif