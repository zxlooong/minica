#if !defined(AFX_CAPAGE_H__5AD36C39_E124_4AE2_8D7A_2A7D11A740F4__INCLUDED_)
#define AFX_CAPAGE_H__5AD36C39_E124_4AE2_8D7A_2A7D11A740F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCenterPage.h : header file
//
#include ".\Control\FileEditCtr\FileEditCtrl.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CCaCenterPage dialog

class CCaCenterPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCenterPage)

// Construction
public:
	void SetNamePath(const CString & strPath);
	void TranslateCT();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CCaCenterPage();
	~CCaCenterPage();
	CString m_Path;

// Dialog Data
	//{{AFX_DATA(CCaCenterPage)
	enum { IDD = IDD_PROPPAGE_CA };
	CFileEditCtrl	m_DirEdit;
//	CXPEdit	m_XpEditReq;
//	CXPEdit	m_XpEditCn;
//	CXPEdit	m_XpEditEm;
//	CXPEdit	m_XpEditC;
//	CXPEdit	m_XpEditO;
//	CXPEdit	m_XpEditOu;
//	CXPEdit	m_XpEditSt;
//	CXPEdit	m_XpEditL;
//	CXPEdit	m_XpEditT;
//	CXPEdit	m_XpEditG;
//	CXPEdit	m_XpEditS;
//	CXPEdit	m_XpEditDay;
//	CXPEdit	m_XpEditSn;
	CFileEditCtrl	m_XpEdit4;
	CStatic	m_StaticCN;
	CXPStyleButtonST	m_Bcrl2;
	CXPStyleButtonST	m_Bview;
//	CXPStyleButtonST	m_Bpath4;
	CXPStyleButtonST	m_Bmkreq;
	CXPStyleButtonST	m_Bmkcert2;
	CXPStyleButtonST	m_Bmkcert;
	CXPStyleButtonST	m_Bcrl;
	CXPStyleButtonST	m_Bmkroot;
	CString	m_C;
	CString	m_ST;
	CString	m_L;
	CString	m_O;
	CString	m_OU;
	CString	m_CN;
	CString	m_E;
	CString	m_S;
	CString	m_G;
	CString	m_T;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCenterPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCaCenterPage)
	virtual BOOL OnInitDialog();
//	afx_msg void OnBPath4();
	afx_msg void OnBCrl();
	afx_msg void OnBMkroot();
	afx_msg void OnBMkreq();
	afx_msg void OnBMkcert();
	afx_msg void OnCheck();
	afx_msg void OnBMkcert2();
	afx_msg void OnCheckP12();
	afx_msg void OnBCrl2();
	//}}AFX_MSG
	BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()
private:
	void AddMsg(CString info,DWORD type = M_OK);
	int m_CertFormat;
	BOOL m_MadePfx;
	UINT m_NameType;
	CString m_ExtName, m_IniPathName;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPAGE_H__5AD36C39_E124_4AE2_8D7A_2A7D11A740F4__INCLUDED_)
