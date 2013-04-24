#if !defined(AFX_CERTPAGE_H__BA3DE6AC_0660_49FF_9937_D50BDA157C2E__INCLUDED_)
#define AFX_CERTPAGE_H__BA3DE6AC_0660_49FF_9937_D50BDA157C2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CertExtPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCertExtPage dialog
#include ".\Control\FileEditCtr\FileEditCtrl.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

class CCertExtPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCertExtPage)
private:
	void AddMsg(CString info,DWORD type = M_OK);
	void SetCheck();
	CString m_Path;
// Construction
public:
	void TranslateCT();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CCertExtPage();
	~CCertExtPage();
	CString m_PriKey;
	CString m_p12;
	CString m_cert;
	CString m_key;
	CString m_p7;
	CString m_crl;
	int m_CertFormat;

// Dialog Data
	//{{AFX_DATA(CCertExtPage)
	enum { IDD = IDD_PROPPAGE_EXT };
//	CXPEdit	m_XpEditPwd4;
//	CXPEdit	m_XpEditPwd3;
//	CXPEdit	m_XpEditPwd2;
//	CXPEdit	m_XpEditPwd;
	CFileEditCtrl	m_XpEditPath;
	CFileEditCtrl	m_XpEditP12;
	CFileEditCtrl	m_XpEditKey;
	CFileEditCtrl	m_XpEditInfile;
	CFileEditCtrl	m_XpEditOutfile;
//	CXPStyleButtonST	m_Bsout;
//	CXPStyleButtonST	m_Bsin;
//	CXPStyleButtonST	m_Bpub;
//	CXPStyleButtonST	m_Bpath;
	CXPStyleButtonST	m_Bok2;
	CXPStyleButtonST	m_Bok;
//	CXPStyleButtonST	m_Bkey;
	int		m_TypeP12;
	int		m_TypeCert;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCertExtPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCertExtPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBOk();
	afx_msg void OnRParse();
	afx_msg void OnRCreate();
	afx_msg void OnRChangep();
	afx_msg void OnRadioPemtoder();
	afx_msg void OnRadioDertopem();
	afx_msg void OnRadioRootch();
	afx_msg void OnRadioCrlch();
	afx_msg void OnRadioTiemch();
	afx_msg void OnRadioKeypairch();
	afx_msg void OnBOk2();
	afx_msg void OnRParse2();
	afx_msg void OnRCreate2();
	afx_msg void OnCheck1();
	//}}AFX_MSG
	BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CERTPAGE_H__BA3DE6AC_0660_49FF_9937_D50BDA157C2E__INCLUDED_)
