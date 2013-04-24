#if !defined(AFX_WEBSERVERPAGE_H__7CB72461_171C_4269_B3C8_C0824E6BD5AC__INCLUDED_)
#define AFX_WEBSERVERPAGE_H__7CB72461_171C_4269_B3C8_C0824E6BD5AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebServerPage.h : header file
//
#include ".\W3Mfc\W3Mfc.h"
#include ".\W3Mfc\HttpClient.h"
#include ".\Control\StaticTree\StaticTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CWebServerPage dialog
class CMiniCaHttpServer : public CHttpServer
{
public:
	void OnError(const CString& sError);
};

class CMiniCAHttpClient : public CHttpClient
{
//	friend class CWebServerPage;
public:
  virtual void PostLog(int nHTTPStatusCode, DWORD dwBodyLength);
  DECLARE_DYNCREATE(CMiniCAHttpClient)
};

class CWebServerPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWebServerPage)

// Construction
public:
	static void AddMsg(CString strMsg, BOOL bClient = TRUE, 
		BOOL bSucceed = TRUE); //默认正确消息,并向客户端添加
	void TranslateCT();
	BOOL IsWebRun();
	void StopServer();
	BOOL Service();
	CMiniCaHttpServer* pTheWebServer;
	static CStaticTreeCtrl * m_pWebTree;
	HTREENODE m_hWebRoot;
	static HTREENODE m_hWebServer;
	static HTREENODE m_hWebClient;

	CWebServerPage();
	~CWebServerPage();
//Methods
#ifdef W3MFC_SSL_SUPPORT
  static CCriticalSection* m_pOpenSSLCritSections;
  static void __cdecl OpenSSLLockingCallback(int mode, int type, const char* file, int line);
#endif

// Dialog Data
	//{{AFX_DATA(CWebServerPage)
	enum { IDD = IDD_PROPPAGE_WEBSERVER };
	CStaticTreeCtrl	m_WebTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWebServerPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWebServerPage)
	afx_msg void OnServer();
	virtual BOOL OnInitDialog();
	afx_msg void OnBstop();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnBresert();
	afx_msg void OnBclear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBSERVERPAGE_H__7CB72461_171C_4269_B3C8_C0824E6BD5AC__INCLUDED_)
