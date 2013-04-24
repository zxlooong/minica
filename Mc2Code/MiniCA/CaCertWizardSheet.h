#if !defined(AFX_CACERTSHEET_H__29E0E9CA_CF91_4CEA_8833_697D9FEE41FF__INCLUDED_)
#define AFX_CACERTSHEET_H__29E0E9CA_CF91_4CEA_8833_697D9FEE41FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCertWizardSheet.h : header file
//
#include ".\Control\TabSheet\TabSheet.h"
#include "CaCertTypePage.h"
#include "CaCertInfoPage.h"
#include "CaCertExtPage.h"
#include "CaCertManPage.h"
#include "CaCertReportPage.h"
#include "CaCertIniSetPage.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCaCertWizardSheet dialog

class CCaCertWizardSheet : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCertWizardSheet)

// Construction
public:
	CCaCertWizardSheet();
	~CCaCertWizardSheet();
	CWnd * GetPage(CString strPageName);
	void Language();


// Dialog Data
	//{{AFX_DATA(CCaCertWizardSheet)
	enum { IDD = IDD_PROPPAGE_CASHEET };
	CXPStyleButtonST	m_BV;
	CXPStyleButtonST	m_BNext;
	CXPStyleButtonST	m_BMade;
	CXPStyleButtonST	m_BLast;
	CTabSheet	m_CaWizardSheet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCertWizardSheet)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCaCertWizardSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnBV();
	afx_msg void OnBMade();
	afx_msg void OnSelchangeTabSheet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBLast();
	afx_msg void OnBNext();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()
private:
	HINSTANCE m_HinstLib; //¶¯Ì¬Á¬½Ó¿â¾ä±ú
	CImageList m_ImgList;
	CCaCertTypePage m_PageType;
	CCaCertInfoPage m_PageInfo;
	CCaCertExtPage  m_PageExt;
	CCaCertManPage  m_PageMan;
	CCaCertReportPage m_PageReport;
	CCaCertIniSetPage	m_PageIniSet;
//	CToolTipCtrl m_tooltip;
	BOOL ViewCert(char * cert,UINT uCertLen);
	void AddMsg(CString info,DWORD type = M_OK);
	void SelectViewCert(char * pCert,UINT uLenCert);
	void ShowError();
	BOOL ImportCSP(char * memP12,UINT lenP12,CString szCSPName,CString szKeycon);
	long GetCspCertInfo(LPTSTR szCSPName, LPTSTR ContainerName, DWORD KeySpec);

	void MakeRootCert(stuSUBJECT * pSUBJECT,CCaCertInfoPage::stuCERTINFO & CERTINFO);

	void MakeCert(stuSUBJECT * pCERT,CCaCertInfoPage::stuCERTINFO & CERTINFO,
		const int type, CString KUSAGE,CString EKUSAGE, const stuCERTEXT * pCertExt);

	void MakeCertCsp(stuSUBJECT * pCERT,CCaCertInfoPage::stuCERTINFO & CERTINFO,
		const int type, CString KUSAGE,CString EKUSAGE, const stuCERTEXT * pCertExt);
	void MakeRootCsp(stuSUBJECT * pCERT,CCaCertInfoPage::stuCERTINFO & CERTINFO,
		const int type, CString KUSAGE,CString EKUSAGE, const stuCERTEXT * pCertExt);
	
	void MakeCertP11(stuSUBJECT * pCERT,CCaCertInfoPage::stuCERTINFO & CERTINFO,
		const int type, CString KUSAGE,CString EKUSAGE, const stuCERTEXT * pCertExt);
	void MakeRootP11(stuSUBJECT * pCERT,CCaCertInfoPage::stuCERTINFO & CERTINFO,
		const int type, CString KUSAGE,CString EKUSAGE, const stuCERTEXT * pCertExt);

	int GenCertNumDateInt();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACERTSHEET_H__29E0E9CA_CF91_4CEA_8833_697D9FEE41FF__INCLUDED_)
