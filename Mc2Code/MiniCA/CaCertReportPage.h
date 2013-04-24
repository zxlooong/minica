#if !defined(AFX_CACERTWIZARDPAGE_H__7BD66579_C87B_4334_9E45_C9D200CA54FC__INCLUDED_)
#define AFX_CACERTWIZARDPAGE_H__7BD66579_C87B_4334_9E45_C9D200CA54FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCertReportPage.h : header file
//
#include "CaCertTypePage.h"
#include "CaCertInfoPage.h"
#include "CaCertExtPage.h"
#include ".\Control\StaticTree\StaticTreeCtrl.h"
#include ".\Control\XpButtonST\XPStyleButtonST.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCaCertReportPage dialog

class CCaCertReportPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCertReportPage)

// Construction
public:
	void ViewWizardInfo();
	BOOL GetWizard(CCaCertTypePage * pPageCaType,
		CCaCertInfoPage * pPageCaInfo,
		CCaCertExtPage * pPageCaExt);
	CCaCertReportPage();
	~CCaCertReportPage();

// Dialog Data
	//{{AFX_DATA(CCaCertReportPage)
	enum { IDD = IDD_PROPPAGE_CAREPORT };
	CStaticTreeCtrl	m_WizardTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCertReportPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCaCertReportPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CCaCertTypePage * m_pPageCaType;
	CCaCertInfoPage * m_pPageCaInfo;
	CCaCertExtPage * m_pPageCaExt;
	HTREENODE m_hCertRoot; //Ê÷¸ù


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACERTWIZARDPAGE_H__7BD66579_C87B_4334_9E45_C9D200CA54FC__INCLUDED_)
