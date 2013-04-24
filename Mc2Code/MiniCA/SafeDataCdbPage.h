#if !defined(AFX_SAFEDATACDBPAGE_H__A92EFE02_F03C_4F42_82E1_0263E0ACD50F__INCLUDED_)
#define AFX_SAFEDATACDBPAGE_H__A92EFE02_F03C_4F42_82E1_0263E0ACD50F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SafeDataCdbPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CSafeDataCdbPage dialog
#include ".\Control\FileEditCtr\FileEditCtrl.h"

class CSafeDataCdbPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSafeDataCdbPage)

// Construction
public:
	enum EncType
	{
		NONE = 0,
		CRYPT = 1001,
		DIGEST = 2001,
		SIGN = 3001,
		SEAL = 4001,
		PubEnc = 5001,
		PrivEnc = 6001
	};
	void TranslateCT();
	BOOL GetCertPair(EncType eType,  BOOL bKeyType, char * sCert, 
		DWORD & dLen, CString & sPwd, CColorListBox * pList = NULL);
	CSafeDataCdbPage();
	~CSafeDataCdbPage();

// Dialog Data
	//{{AFX_DATA(CSafeDataCdbPage)
	enum { IDD = IDD_PROPPAGE_SAFEDS };
//	CXPEdit	m_XpEdit9;
//	CXPEdit	m_XpEdit6;
//	CXPEdit	m_XpEdit3;
	CFileEditCtrl	m_Edit8;
	CFileEditCtrl	m_Edit7;
	CFileEditCtrl	m_Edit5;
	CFileEditCtrl	m_Edit4;
	CFileEditCtrl	m_Edit2;
	CFileEditCtrl	m_Edit1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSafeDataCdbPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSafeDataCdbPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAFEDATACDBPAGE_H__A92EFE02_F03C_4F42_82E1_0263E0ACD50F__INCLUDED_)
