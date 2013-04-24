#if !defined(AFX_ASN1PARSEPAGE_H__82FDAB26_8921_44E8_B1BC_3C9DE96A2F73__INCLUDED_)
#define AFX_ASN1PARSEPAGE_H__82FDAB26_8921_44E8_B1BC_3C9DE96A2F73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Asn1ParsePage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CAsn1ParsePage dialog
#include ".\Control\tooltip\OXToolTipCtrl.h"
class CAsn1ParsePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAsn1ParsePage)

// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void GetAsn1Info(CString strpath);
	void TranslateCT();	//繙譯諸如樹型控件,列錶控件等內容
	CAsn1ParsePage();
	~CAsn1ParsePage();
	//ASN1解码结构
	struct stuTREE
	{
		UINT EXCURSION;//偏移
		UINT DEPTH;//深度
		UINT LENGTH;//内容长度
		char TYPE[24];//类型
		char INFO[128];//内容
		stuTREE * Link;
		stuTREE()
		{
			memset(this,0,sizeof(stuTREE));
		}

		stuTREE(UINT Excursion,UINT Depth,UINT Length,char * Type)
		{
			memset(this,0,sizeof(stuTREE));
			EXCURSION=Excursion;
			DEPTH=Depth;
			LENGTH=Length;
			strncpy(TYPE,Type,sizeof(TYPE));
			Link=NULL;
		}
	};
// Dialog Data
	//{{AFX_DATA(CAsn1ParsePage)
	enum { IDD = IDD_PROPPAGE_ASN1 };
//	CXPEdit	m_XpEdit;
	CTreeCtrl	m_Tree;
	CXPStyleButtonST m_Bread;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAsn1ParsePage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	void AddMsg(CString info, DWORD type);
	struct stuNODE
	{
		HTREEITEM NODE;
		UINT EXCURSION;//偏移
		UINT LENGTH;//内容长度
		char INFO[128];
		stuNODE()
		{
			memset(this,0,sizeof(stuNODE));
		}
	};
	CPtrList m_NodeList;
	HTREEITEM m_TreeItem;	//右击树控件选中的项

	// Generated message map functions
	//{{AFX_MSG(CAsn1ParsePage)
	afx_msg void OnBRead();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedAsn1Tree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnRclickAsn1Tree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuitemExpane();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ExpandAll(HTREEITEM item,UINT nCode);
	int AddTree(stuTREE *& Head,UINT Excursion,UINT Depth,UINT Length,char * Type);
	void FreeTree(stuTREE *& Head);
	int asn1_print_info(int tag, int xclass, int constructed,
					int indent,char * str);
	void Utf8ToAnsi(UCHAR * lpsrc, int srclen, LPSTR lpdst, int& dstlen);
	BOOL GetAsn1Str(ASN1_STRING * str,char * outName);
	int asn1_parse2(stuTREE *& TREE, unsigned char **pp, long length, int offset,
				int depth, int indent, int dump,char * msg);
	int ASN1_parsedump(stuTREE *& TREE, unsigned char *pp, long len,char * msg);
	BOOL Asn1Parse(const char * infile,stuTREE *& TREE,const int informat,char * msg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASN1PARSEPAGE_H__82FDAB26_8921_44E8_B1BC_3C9DE96A2F73__INCLUDED_)
