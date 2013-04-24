#if !defined(AFX_UPDATEPAGE_H__CBE1368F_C845_4DF0_9D19_AD4A8DE0C588__INCLUDED_)
#define AFX_UPDATEPAGE_H__CBE1368F_C845_4DF0_9D19_AD4A8DE0C588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdatePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUpdatePage dialog
#include <WinINet.h>
#include ".\Control\ProgressCtrlST\ProgressCtrlST.h"

class CUpdatePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CUpdatePage)

public:
	struct REQUEST_CONTEXT
	{
		HWND          hWindow;     // Main window handle
		int           nProgOne;    // ID of the edit box with the URL
		int           nProgTwo;    // ID of the edit box for the header info
		DWORD         m_dwFtpHead;  // 次数
		DWORD		  dFinishLen;	//完成长度	
		DWORD         dFileLen;	   // 文件总长度
		DWORD	      dTotalLen;	//批次长度
		DWORD	      dTotalFinish;	//批次完成长度
		BOOL		  bBegin;		//是否上传下载
		CUpdatePage    *  pUpdate;//   
	}m_Request;

private:
	void AddMsg(CString info,DWORD type = M_OK);

	CString GetFileGuid(CString strFile);
	enum
	{
		INTERNET_ERROR_OPENURL=1,
		INTERNET_ERROR_FILEOPEN,
		INTERNET_ERROR_READFILE,
		INTERNET_ERROR_OPEN
	};
	BOOL FileOperate(char * FilePath,char * Operate);
	void GetLastError();
	BOOL RegSvr(char * DllPath,BOOL bType = TRUE); //默认注册
	BOOL SystemShutdown(UINT uFlags,DWORD dwReason);
	DWORD m_TaskLen,//总共长度
		  m_UpLen,//下载的总长度
		  m_OneLen;//单个文件大小
	BOOL m_bUpdate;//是否升级中
	BOOL m_bReset;//是否需要重启动
	UINT InternetGetFile (HINTERNET IN hOpen,LPCTSTR  szUrl,
		LPCTSTR szFilePatch,UINT uIndex,LPCTSTR szFileName,BOOL bShowProg = TRUE);
	CString LmToHText(UCHAR * pValue, UINT uLen); //乱码转16进制
	struct stuUPDATE
	{
		char FileName[256] ;//文件名称
		char FileGuid[256] ;//文件校验码
		DWORD FileLength ;//文件长度
		char FilePath[256] ;//相对与程序给定路径的文件存储路径
		char FileUrl[256] ;//在服务器中存放位置
		char FileOperate[5] ;//操作方式 Down-下载 ,Reg-注册 ,Run - 运行
		stuUPDATE()
		{
			memset(this,0,sizeof(stuUPDATE));
		}
		stuUPDATE(const char * filename,char * fileguid,DWORD filelength,const char * filepath,
			const char * fileurl,const char * fileoper)
		{
			memset(this,0,sizeof(stuUPDATE));
			strncpy(FileName,filename,255);
			strncpy(FileGuid,fileguid,255);
			FileLength = filelength;
			strncpy(FilePath,filepath,255);
			strncpy(FileUrl,fileurl,255);
			strncpy(FileOperate,fileoper,5);
		}
		
	};
	HINTERNET m_hSession,
			  m_hConnect;

	// Construction
	public:
	void TranslateCT();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CWnd * m_pParent;

	UINT DownHttpFile(LPCTSTR lpszUrl,LPCTSTR lpszLocal,UINT uIndex,LPCTSTR szFileName,
		BOOL bShowProg = TRUE);

	BOOL ConnectFtp(CString sServerName,UINT uServerPort,
							CString sUserName,CString sUserPwd);
	BOOL DownFtpFile(LPCTSTR lpszNewRemoteFile,LPCTSTR lpszLocalFile);

	void DisFtpConnect();

	void AddEvent(int iState ,CString Info);

	CString m_sIniUrl;////升级信息文件
	CString m_sExePath;//可执行文件路径
	CString m_sIniGuid;
//	CPtrList m_PtrList;//升级信息链表
	void CheckDonwloadThread();

	CUpdatePage();
	~CUpdatePage();
		
	// Dialog Data
	//{{AFX_DATA(CUpdatePage)
	enum { IDD = IDD_PROPPAGE_UPDATE };
	CXPStyleButtonST	m_Bup;
	CProgressCtrlST	m_Prog2;
	CProgressCtrlST	m_Prog1;
	CListCtrl	m_UpList;
	//}}AFX_DATA
		
		
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUpdatePage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
	protected:
	CToolTipCtrl m_tooltip;
	void GetErrInfo(DWORD dError, char * tmp_msg);

	// Generated message map functions
	//{{AFX_MSG(CUpdatePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBup();
	afx_msg void OnRclickListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
			
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEPAGE_H__CBE1368F_C845_4DF0_9D19_AD4A8DE0C588__INCLUDED_)
