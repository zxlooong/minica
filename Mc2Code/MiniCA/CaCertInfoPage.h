#if !defined(AFX_CACERTINFOPAGE_H__9B8E259D_C2BD_4B1C_A42D_A41E78DDCFE0__INCLUDED_)
#define AFX_CACERTINFOPAGE_H__9B8E259D_C2BD_4B1C_A42D_A41E78DDCFE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCertInfoPage.h : header file
//
#include ".\Control\StaticTree\StaticTreeCtrl.h"
#include ".\Control\CheckList\ListCtrl.h"
#include ".\Control\FileEditCtr\FileEditCtrl.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCaCertInfoPage dialog

class CCaCertInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCertInfoPage)

// Construction
public:
	void TranslateCT();
	void GetIniInfo(CString strIniPath);
	void SetNamePath(const CString &strPath);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	struct stuCERTINFO
	{
		UINT uCertFormat;
		UINT uCertGenType;  // 0 evp; 1 evp->csp; 2 csp; 3 p11;
		char cCertPwd[18];	//pfx pin
		char cCertPwd2[18];  // p11 pin
		BOOL bCertP12;
		char cCertDir[256];
		UINT uCertDay;
		char cCertCsp[256];
		UINT uCertLen;
		char cCertCon[42];
	//	UINT uP11dirLen;
		char cP11dir[256];
		stuCERTINFO(){
			memset(this,0,sizeof(stuCERTINFO));
		}
	};

	void GetCert(stuSUBJECT *& pCERT,stuCERTINFO & certInfo);
	BOOL GetCertInfo(CStaticTreeCtrl * p_WizardTree,
					 HTREENODE & hInfoBase, 
					 HTREENODE &  hInfoHold, 
					 HTREENODE &  hInfoSave);
	CCaCertInfoPage();
	~CCaCertInfoPage();

// Dialog Data
	//{{AFX_DATA(CCaCertInfoPage)
	enum { IDD = IDD_PROPPAGE_CAINFO };
//	CXPEdit	m_XpEditCont;
	gxListCtrl	m_CheckList;
//	CXPEdit	m_XpEditReq;
	CFileEditCtrl	m_XpEdit;
	CFileEditCtrl	m_XpEdit2;
//	CXPEdit	m_XpEditDay;
	CComboBox	m_CspBox;
	int		m_RadioDisk;
	UINT	m_Day;
	CString	m_Pwd;
	CString	m_Pwd2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCertInfoPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_CertFormat;
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCaCertInfoPage)
	afx_msg void OnRCsp();
	afx_msg void OnRadioDisk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCainfoRadioder();
	afx_msg void OnCainfoRadiopem();
	afx_msg void OnEVP2CSP();
	afx_msg void OnP11();
	afx_msg void OnGuid();
	//}}AFX_MSG
	BOOL OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()

private:
	CString m_IniPathName;
	CString GetDefaultCsp();
	CString m_PathName;
	BOOL EnumCsp();
	void SetLVCheck (WPARAM ItemIndex, BOOL bCheck = TRUE);
	CStringArray  m_ArrayInside;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACERTINFOPAGE_H__9B8E259D_C2BD_4B1C_A42D_A41E78DDCFE0__INCLUDED_)
