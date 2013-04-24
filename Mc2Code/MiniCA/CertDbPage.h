#if !defined(AFX_SETUPPAGE_H__2C00EC73_D8C4_44FB_ABE6_7A860E2E1F66__INCLUDED_)
#define AFX_SETUPPAGE_H__2C00EC73_D8C4_44FB_ABE6_7A860E2E1F66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CertDbPage.h : header file
//
#include ".\Control\Tree\SetupTree.h"
#include ".\Control\SuperList\SuperGridCtrl.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CCertDbPage dialog

class CCertDbPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCertDbPage)

// Construction
public:
//	CString GetCertPath(int dIndex);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CCertDbPage();
	~CCertDbPage();
	void AddMsg(CString info,DWORD type = M_OK);
	void TranslateCT();
	UINT GetNameType();

// Dialog Data
	//{{AFX_DATA(CCertDbPage)
	enum { IDD = IDD_PROPPAGE_CERTSETUP };
	CXPStyleButtonST	m_Bsavecdb;
	CXPStyleButtonST	m_Bresertdb;
	CSetupTree	m_TreeSetup;
//	CXPEdit	m_CertSave;
	CXPStyleButtonST	m_Bselect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCertDbPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCertDbPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBResertdb();
	afx_msg void OnBSavecdb();
	afx_msg void OnRclickTreeSetup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void SetPathInfo(CString strPath, CString strIconFile, CString strIconIndex = "0",  CString strInfoTip = "");
	void SetCaPath(CString strPath);
	CString m_IniPathName;
	HTREEITEM m_hItem[32];	//保存树控件的项信息,用于更改语言
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPAGE_H__2C00EC73_D8C4_44FB_ABE6_7A860E2E1F66__INCLUDED_)
