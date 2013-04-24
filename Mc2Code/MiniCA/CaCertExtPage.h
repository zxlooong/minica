#if !defined(AFX_CACERTEXTPAGE_H__85FCE443_840E_4423_82E4_50066ECE697A__INCLUDED_)
#define AFX_CACERTEXTPAGE_H__85FCE443_840E_4423_82E4_50066ECE697A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCertExtPage.h : header file
//
#include ".\Control\StaticTree\StaticTreeCtrl.h"
#include ".\Control\CheckList\ListCtrl.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCaCertExtPage dialog

class CCaCertExtPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCertExtPage)

// Construction
public:
	void GetIniInfo(CString strIniPath);
	void GetCert(stuCERTEXT *& pCertExt);
	void GetCertInfo(CStaticTreeCtrl * p_WizardTree,
					 HTREENODE & hInfoBase) ;
	void TranslateCT();

	CCaCertExtPage();
	~CCaCertExtPage();

// Dialog Data
	//{{AFX_DATA(CCaCertExtPage)
	enum { IDD = IDD_PROPPAGE_CAEXT };
//	CRichEditCtrl	m_RichEdit;
	gxListCtrl	m_CheckList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCertExtPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCaCertExtPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangingList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void SetLVCheck (WPARAM ItemIndex, BOOL bCheck = TRUE);
	CStringArray m_ArrayInside;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACERTEXTPAGE_H__85FCE443_840E_4423_82E4_50066ECE697A__INCLUDED_)
