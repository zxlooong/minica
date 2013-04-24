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
		DWORD         m_dwFtpHead;  // ����
		DWORD		  dFinishLen;	//��ɳ���	
		DWORD         dFileLen;	   // �ļ��ܳ���
		DWORD	      dTotalLen;	//���γ���
		DWORD	      dTotalFinish;	//������ɳ���
		BOOL		  bBegin;		//�Ƿ��ϴ�����
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
	BOOL RegSvr(char * DllPath,BOOL bType = TRUE); //Ĭ��ע��
	BOOL SystemShutdown(UINT uFlags,DWORD dwReason);
	DWORD m_TaskLen,//�ܹ�����
		  m_UpLen,//���ص��ܳ���
		  m_OneLen;//�����ļ���С
	BOOL m_bUpdate;//�Ƿ�������
	BOOL m_bReset;//�Ƿ���Ҫ������
	UINT InternetGetFile (HINTERNET IN hOpen,LPCTSTR  szUrl,
		LPCTSTR szFilePatch,UINT uIndex,LPCTSTR szFileName,BOOL bShowProg = TRUE);
	CString LmToHText(UCHAR * pValue, UINT uLen); //����ת16����
	struct stuUPDATE
	{
		char FileName[256] ;//�ļ�����
		char FileGuid[256] ;//�ļ�У����
		DWORD FileLength ;//�ļ�����
		char FilePath[256] ;//�����������·�����ļ��洢·��
		char FileUrl[256] ;//�ڷ������д��λ��
		char FileOperate[5] ;//������ʽ Down-���� ,Reg-ע�� ,Run - ����
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

	CString m_sIniUrl;////������Ϣ�ļ�
	CString m_sExePath;//��ִ���ļ�·��
	CString m_sIniGuid;
//	CPtrList m_PtrList;//������Ϣ����
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
