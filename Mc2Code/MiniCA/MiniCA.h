// MiniCA.h : main header file for the MiniCA application
//

#if !defined(AFX_MiniCA_H__41C2EAF4_A86A_4078_B274_DD0AD886E153__INCLUDED_)
#define AFX_MiniCA_H__41C2EAF4_A86A_4078_B274_DD0AD886E153__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include ".\Control\XpButtonST\ThemeHelperST.h"

/////////////////////////////////////////////////////////////////////////////
// CMiniCaApp:
// See MiniCA.cpp for the implementation of this class
//.\Evp\Debug\Evp.lib .\CA\Release\CA.lib
#define UNIQE_NAME "{98D9ED48-66C1-4efa-82FF-68B27B514511}"//定义唯一的，以免冲突 GUIDGEN.EXE

enum OsType
{
	Win32s,
	WinNT3,
	Win95,
	Win98,
	WinME,
	WinNT4,
	Win2000,
	WinXP
};


class CMiniCaApp : public CWinApp
{
public:
	BOOL IsXpStyle();
	CThemeHelperST * GetThemeHelperST()
	{
		return &m_ThemeHelper;
	}
	static CString NormalCode(CString);
	static BOOL IsBig();
	static CString LmToHText(UCHAR * pValue,UINT uLen);
	static UINT HTextToLm(char * pValue,char * pOut); //16进制转乱码
	static int HexToTen(const char * pHex);//16--->10
	static BOOL AddRootToStore(char * lpCert, UINT lenCert);//加入der格式根证书

	static LPTSTR CMiniCaApp::_tCharAlloc(UINT uSize)
	{
		return (LPTSTR)malloc(sizeof(TCHAR) * uSize);
	}

	static VOID CMiniCaApp::_tCharFree(LPVOID p)
	{
		free(p);
	}
	//创建多级目录
	static BOOL WINAPI MakeSureDirectoryPathExists(LPCTSTR pszDirPath);

	BOOL IsHpxs();
	CString GetAppPath();
	CImageList * GetImgList();
	OsType GetOsType();
	BOOL GetCertPair(UINT ID, char * sCert, DWORD & dLen, CString & sPwd, 
		BOOL bInsideOnly = FALSE/*只内部标示*/, CColorListBox * pList = NULL);
	BOOL GetRootCert(char * sCert, DWORD & dCertLen, char * sKey, DWORD & dKeyLen, CString & sPwd,
						 BOOL bInsideOnly = FALSE/*只内部标示*/, CColorListBox * pList = NULL);
	CString GetMachineCode();
	UINT IsReg();
	CMiniCaApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniCaApp)
	public:
	virtual BOOL InitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMiniCaApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	HANDLE m_hOneInstance;
	CString m_AppPath;
	BOOL m_bHpxs;
	CImageList m_ImgList;
	BOOL CheckRegCode(CString username,CString regcode,int type = TRUE);//验证注册码
//	int HexToTen(const char * pHex);//16--->10
	BOOL CheckSolfCode(CString pathExt/*被验证文件路径*/,CString dllName/*dll名称*/,
		UINT dllId = 1/*资源ID*/);//检验验证码
	CStringArray m_HelpArray;
	static BOOL m_bIsBig;
	static CString Gb2Big5(CString sGb);
	static CString Big2Gb(CString sBig);
	BOOL CharSetIsBig();
	CThemeHelperST	m_ThemeHelper;
	//	Enable checkbox only if running under XP
	//	if (m_ThemeHelper.IsAppThemed())	GetDlgItem(IDC_CHKTOOLBAR)->EnableWindow(TRUE);

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MiniCA_H__41C2EAF4_A86A_4078_B274_DD0AD886E153__INCLUDED_)
