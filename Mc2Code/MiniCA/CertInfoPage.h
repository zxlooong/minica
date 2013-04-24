#if !defined(AFX_MODCERTPAGE_H__2F06664F_5C4A_412D_947F_38E9C3FF7484__INCLUDED_)
#define AFX_MODCERTPAGE_H__2F06664F_5C4A_412D_947F_38E9C3FF7484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CertInfoPage.h : header file
//
#include "ExtRichEdit.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCertInfoPage dialog

class CCertInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCertInfoPage)

// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL ViewCertInfo(char * cert, UINT certLen,void *x509, char *buf);
	static BOOL GetSubjectInfo(const void * px509,char * name,char * msg);
	static UINT CCertInfoPage::GetKeyLen(const void * px509);
	CCertInfoPage();
	~CCertInfoPage();
	void TranslateCT();
// Dialog Data
	//{{AFX_DATA(CCertInfoPage)
	enum { IDD = IDD_PROPPAGE_CERTINFO };
//	CXPEdit	m_XpEdit;
	CXPStyleButtonST	m_Bread;
	CExtRichEdit	m_RichEdit;
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCertInfoPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCertInfoPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBRead();
	afx_msg void OnItemchangedListMod(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	struct stuCERTINFO
	{
		long VER;//版本
		char SN[10];//序列号
		char SIGNATURE[30];//签名算法
		char ISSUER[512];//颁发者
		char NOTBEFORE[24];//起始日期
		char NOTAFTER[24];//中止日期
		char SUBJECT[512];//主题
		char PUBTYPE[4];//公钥类型
		char PUBKEY[2096];//公钥
		long PUBLEN;//公钥长度
		char V3EXT[4096];//扩展信息
		char THUMB[6];//缩微图算法
		char THUMBPRINT[MAX_MD_SIZE*2+1];//缩微图
		stuCERTINFO()
		{
			memset(this,0,sizeof(stuCERTINFO));
		}
	};

	CString m_strPubKey;

	void AddMsg(CString info, DWORD type);

	static void Utf8ToAnsi(const UCHAR * lpsrc,const int srclen, LPSTR lpdst,
		int& dstlen);

	//其实为X509_NAME,直接写,不能编译
	static BOOL GetCertName(void *name,char * outName);

	static char * GetCertTime(const ASN1_UTCTIME *s);

	static int GetExtensions(BIO *bp,STACK_OF(X509_EXTENSION) *exts);

	static BOOL GetCertInfo(const char * pCert, const UINT certLen, 
		const void * px509,	stuCERTINFO & certinfo, char * msg);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODCERTPAGE_H__2F06664F_5C4A_412D_947F_38E9C3FF7484__INCLUDED_)
