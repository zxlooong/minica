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
		long VER;//�汾
		char SN[10];//���к�
		char SIGNATURE[30];//ǩ���㷨
		char ISSUER[512];//�䷢��
		char NOTBEFORE[24];//��ʼ����
		char NOTAFTER[24];//��ֹ����
		char SUBJECT[512];//����
		char PUBTYPE[4];//��Կ����
		char PUBKEY[2096];//��Կ
		long PUBLEN;//��Կ����
		char V3EXT[4096];//��չ��Ϣ
		char THUMB[6];//��΢ͼ�㷨
		char THUMBPRINT[MAX_MD_SIZE*2+1];//��΢ͼ
		stuCERTINFO()
		{
			memset(this,0,sizeof(stuCERTINFO));
		}
	};

	CString m_strPubKey;

	void AddMsg(CString info, DWORD type);

	static void Utf8ToAnsi(const UCHAR * lpsrc,const int srclen, LPSTR lpdst,
		int& dstlen);

	//��ʵΪX509_NAME,ֱ��д,���ܱ���
	static BOOL GetCertName(void *name,char * outName);

	static char * GetCertTime(const ASN1_UTCTIME *s);

	static int GetExtensions(BIO *bp,STACK_OF(X509_EXTENSION) *exts);

	static BOOL GetCertInfo(const char * pCert, const UINT certLen, 
		const void * px509,	stuCERTINFO & certinfo, char * msg);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODCERTPAGE_H__2F06664F_5C4A_412D_947F_38E9C3FF7484__INCLUDED_)
