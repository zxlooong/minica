#if !defined(AFX_MINIMAINDLG_H__21A7DD29_96C1_416A_8A3C_6C3C108E7BF1__INCLUDED_)
#define AFX_MINIMAINDLG_H__21A7DD29_96C1_416A_8A3C_6C3C108E7BF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiniMainDlg.h : header file
//
#include "CertExtPage.h"
#include "CaCenterPage.h"

#include "WinEvpSheet.h"

#include "WebServerSheet.h"

#include "Asn1ParsePage.h"

#include "CertInfoPage.h"
#include "RegPage.h"
#include "CertDbPage.h"

#include "UpdatePage.h"

#include ".\Control\Tree\BmpTree.h"

#include "CaCertWizardSheet.h"

//#include "SslSheet.h"

//#include "OcspSheet.h"
#include ".\Control\TaskbarNotifier\TaskbarNotifier.h"
#include ".\Control\PageLabel\PrefsStatic.h"
#include ".\Control\StatusBar\Extstatuscontrolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CMiniMainDlg dialog
#define WM_SYSTEMTRAY WM_USER+100	//系统托盘消息

class CMiniMainDlg : public CDialog
{
// Construction
public:
	CMiniMainDlg(CWnd* pParent = NULL);   // standard constructor
public:
	void ShowNotifier(CString strNotifier);
	void Language();
	CWnd * GetPage(CString strPageName);
	void SetupShell(CString & strName,const HICON hIcon);//设置托盘图标与信息
	void SetItemIcon(CString pageName,UINT uIcon);	//设置树控件图标,用于显示操作进行中
	void ViewCertInfo(char * cert,UINT certlen = 0, void * x509 = 0);
	BOOL GotoPage(CString pageName);//转到某页
	void SetRegFlag(BOOL bFlag);
	int AddMsg(CString caption,CString info, DWORD type = M_OK);
	void DeleteLast();
	CColorListBox * GetListBox();
	NOTIFYICONDATA m_nid;//托盘
	CTaskbarNotifier m_wndTaskbarNotifier;
// Dialog Data
	//{{AFX_DATA(CMiniMainDlg)
	enum { IDD = IDD_MAIN_DIALOG };
	CPrefsStatic	m_CaptionBarCtrl;
	CColorListBox	m_ListBox;
	CBmpTree		m_TreeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniMainDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	virtual void OnOK();
	virtual void OnCancel();
// Implementation
protected:
//	CExtStatusControlBar m_bar;

	HICON m_hIcon;
	HICON m_hIconSelect;
	BOOL m_RegFlag;//是否注册标志
	CImageList m_ImgList;
	BOOL m_bHide;	//窗口是否隐藏，如果隐藏出现MSN提示

	CMenu m_LangMenu; //系统菜单的语言子菜单,必须不能定义为局部变量,否则菜单项不会被改变

	CCaCenterPage m_PageCa;

	CCaCertWizardSheet m_PageCertWizardSheet;

	CCertExtPage m_PageExt;

	CWinEvpSheet m_PageWinEvpSheet;

	CWebServerSheet m_PageWebSheet;

	CAsn1ParsePage m_PageAsn1Parse;
	CCertInfoPage m_PageCertInfo;
	CRegPage m_PageReg;

	CCertDbPage m_PageCertDb;
	
	CUpdatePage m_PageUp;

	bool m_bDlgCreated;//是否全部初始化
	DWORD m_dLastData;//前一个项目
	CString ExpandShortcut(char * inFile);


	// Generated message map functions
	//{{AFX_MSG(CMiniMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void TranslateCT();

	HTREEITEM m_hPageCaItem;
	HTREEITEM m_hPageCertWizardSheetItem;
	HTREEITEM m_hPageExtItem;
	HTREEITEM m_hPageWinEvpSheetItem;
	HTREEITEM m_hPageWebSheetItem;
	HTREEITEM m_hPageAsn1ParseItem;
	HTREEITEM m_hPageCertInfoItem;
	HTREEITEM m_hPageRegItem;
	HTREEITEM m_hPageCertDbItem;
	HTREEITEM m_hPageUpItem;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIMAINDLG_H__21A7DD29_96C1_416A_8A3C_6C3C108E7BF1__INCLUDED_)
