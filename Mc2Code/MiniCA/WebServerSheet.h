#if !defined(AFX_WEBSERVERSHEET_H__F2F6AE21_E6FD_4F83_B65E_EC18B32D6037__INCLUDED_)
#define AFX_WEBSERVERSHEET_H__F2F6AE21_E6FD_4F83_B65E_EC18B32D6037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebServerSheet.h : header file
//
#include "WebServerPage.h"
#include "WebCertManPage.h"
#include ".\Control\TabSheet\TabSheet.h"
/////////////////////////////////////////////////////////////////////////////
// CWebServerSheet dialog

class CWebServerSheet : public CPropertyPage
{
	DECLARE_DYNCREATE(CWebServerSheet)

// Construction
public:
	BOOL IsWebRun();
	void Language();

	CWebServerSheet();
	~CWebServerSheet();

// Dialog Data
	//{{AFX_DATA(CWebServerSheet)
	enum { IDD = IDD_PROPPAGE_WEBSHEET };
	CTabSheet	m_WebSheet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWebServerSheet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWebServerSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CImageList m_ImgList;
	CWebServerPage m_PageWebServer;
	CWebCertManPage m_PageWebCertMan;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBSERVERSHEET_H__F2F6AE21_E6FD_4F83_B65E_EC18B32D6037__INCLUDED_)
