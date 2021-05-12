// WORKRUN.h : main header file for the WORKRUN DLL
//

#if !defined(AFX_WORKRUN_H__3575CFE5_80FD_4A11_ABBC_BBDC6E45F073__INCLUDED_)
#define AFX_WORKRUN_H__3575CFE5_80FD_4A11_ABBC_BBDC6E45F073__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWORKRUNApp
// See WORKRUN.cpp for the implementation of this class
//

class CWORKRUNApp : public CWinApp
{
public:
	CWORKRUNApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWORKRUNApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWORKRUNApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKRUN_H__3575CFE5_80FD_4A11_ABBC_BBDC6E45F073__INCLUDED_)
