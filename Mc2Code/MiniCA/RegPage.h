#if !defined(AFX_REGPAGE_H__5D1D4645_E696_43BF_B112_AC881A39275C__INCLUDED_)
#define AFX_REGPAGE_H__5D1D4645_E696_43BF_B112_AC881A39275C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegPage dialog
#include ".\Control\tooltip\OXToolTipCtrl.h"

class CRegPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRegPage)

// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void AddMsg(CString info,DWORD type = M_OK);
	void TranslateCT();	//繙譯諸如樹型控件,列錶控件等內容
	BOOL GetRegFlag();
	CRegPage();
	~CRegPage();

// Dialog Data
	//{{AFX_DATA(CRegPage)
	enum { IDD = IDD_PROPPAGE_REG };
//	CXPEdit	m_XpEditUser;
//	CXPEdit	m_XpEditReg;
	CStatic	m_ThankStatic;
	CXPStyleButtonST	m_Breg;
	CXPStyleButtonST	m_Bgetreg;
	CXPStyleButtonST	m_Bgetcheck;
	CXPStyleButtonST	m_Bselect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRegPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CRegPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBgetCheck();
	afx_msg void OnBgetReg();
	afx_msg void OnBreg();
	afx_msg void OnBSelect();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnBpost();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL SendMail();
	CString GetMachineCode();
	BOOL CheckRegCode(CString username,CString regcode,BOOL type = TRUE);//验证注册码
	CString GetRegCode(CString name,BOOL bRegE = TRUE);//得到指定长度的注册码
//	int HexToTen(const char * pHex);//16--->10
	BOOL GetRegInfo(CString & User,CString & RegCode);
	CString GetSelfCode(CString filePath);//计算自身验证吗
//	CString LmToHText(UCHAR * pValue,UINT uLen);
//	UINT HTextToLm(char * pValue,char * pOut); //16进制转乱码
	CString m_ReqFile;	//已经生成的req文件
	struct RegReq //注册请求文件,用以客户端注册时候保存客户注册信息
	{
		BOOL RegType;//0 个人 1 企业
		char UserName[18];
		char Mac[42];
		RegReq()
		{
			memset(this,0,sizeof(RegReq));
		}
	};
	struct RegRes //注册应答文件,服务器发放给客户端的注册文件
	{
		RegReq UserInfo;
		char RegCode[2048];
		RegRes()
		{
			memset(this,0,sizeof(RegRes));
		}
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGPAGE_H__5D1D4645_E696_43BF_B112_AC881A39275C__INCLUDED_)
