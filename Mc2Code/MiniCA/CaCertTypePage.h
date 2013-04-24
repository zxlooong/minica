#if !defined(AFX_CACERTTYPEPAGE_H__AF475516_1CAA_43D5_B192_7E8F82B1D532__INCLUDED_)
#define AFX_CACERTTYPEPAGE_H__AF475516_1CAA_43D5_B192_7E8F82B1D532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCertTypePage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CCaCertTypePage dialog
#include ".\Control\StaticTree\StaticTreeCtrl.h"

class CCaCertTypePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCertTypePage)

// Construction
public:
	void TranslateCT();
	void GetCert(CString & kusage, CString & ekusage, int & type);
	void GetCertTypeInfo(CStaticTreeCtrl * p_WizardTree,
						 HTREENODE &  hTypeType, 
						 HTREENODE &  hTypeKusage, 
						 HTREENODE &  hTypeEkusage);
	CCaCertTypePage();
	~CCaCertTypePage();

// Dialog Data
	//{{AFX_DATA(CCaCertTypePage)
	enum { IDD = IDD_PROPPAGE_CATYPE };
	int		m_CertType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCertTypePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCaCertTypePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	afx_msg void OnRadio9();
	afx_msg void OnRadio10();
	afx_msg void OnRadio11();
	//}}AFX_MSG
	BOOL m_bTestCheck;	// A variable used to show DDX_Check feature

	DECLARE_MESSAGE_MAP()
	void SetState(BOOL bEnable,CString  strState);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACERTTYPEPAGE_H__AF475516_1CAA_43D5_B192_7E8F82B1D532__INCLUDED_)
