#if !defined(AFX_SAFEINFOPAGE_H__B65C8891_0B6C_4C45_94B8_0A38087CDF60__INCLUDED_)
#define AFX_SAFEINFOPAGE_H__B65C8891_0B6C_4C45_94B8_0A38087CDF60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SafeInfoPage.h : header file
//
#include ".\Control\ProgressCtrlST\ProgressCtrlST.h"
#include "SafeDataCdbPage.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSafeInfoPage dialog

class CSafeInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSafeInfoPage)

// Construction
public:
	void TranslateCT();
	CSafeInfoPage();
	~CSafeInfoPage();
// Dialog Data
	//{{AFX_DATA(CSafeInfoPage)
	enum { IDD = IDD_PROPPAGE_SAFEI };
//	CXPEdit	m_XpEditPwd;
//	CXPEdit	m_XpEditOutFile;
//	CXPEdit	m_XpEditInfile;
	CXPStyleButtonST	m_Bvsign;
	CXPStyleButtonST	m_Bsign;
	CXPStyleButtonST	m_Brsakey2;
	CXPStyleButtonST	m_Brsakey;
	CXPStyleButtonST	m_Brsacert2;
	CXPStyleButtonST	m_Brsacert;
	CXPStyleButtonST	m_Benc;
	CXPStyleButtonST	m_Bdigest;
	CXPStyleButtonST	m_Bdec;
	CProgressCtrlST	m_Progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSafeInfoPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CSafeInfoPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBenc();
	afx_msg void OnBdec();
	afx_msg void OnBdigest();
	afx_msg void OnBsign();
	afx_msg void OnBvsign();
	afx_msg void OnBrsacert();
	afx_msg void OnBrsakey();
	afx_msg void OnBrsakey2();
	afx_msg void OnBrsacert2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Disply(BOOL bDisply = FALSE);
	void AddMsg(CString info, DWORD type = M_OK);
	CString m_p12Pwd;
	char m_strKey[10240];//P12密码,证书路径,或内容
	DWORD m_lenKey;//长度
	CSafeDataCdbPage * m_pPageCdb;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAFEINFOPAGE_H__B65C8891_0B6C_4C45_94B8_0A38087CDF60__INCLUDED_)
