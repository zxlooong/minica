#if !defined(AFX_WEBCERTMANPAGE_H__FB9FC48C_717A_47E8_952A_86CAB432D40B__INCLUDED_)
#define AFX_WEBCERTMANPAGE_H__FB9FC48C_717A_47E8_952A_86CAB432D40B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebCertManPage.h : header file
//
#include ".\Control\SuperList\SuperGridCtrl.h"
#include ".\Control\Ado\ado2.h"
#include ".\Control\BitmapSlider\BitmapSlider.h"

/////////////////////////////////////////////////////////////////////////////
// CWebCertManPage dialog
const UINT ReqMaxEntry = 11;		//用户申请的条目个数
const UINT Multi_RecodePerPage = 15;		//每页的最大记录数
const UINT Single_RecodePerPage = 1;		//每页的最大记录数

class CWebCertManPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWebCertManPage)

// Construction
public:
	CWebCertManPage();
	~CWebCertManPage();
public:
	BOOL GetRecordset(CString strId, CADORecordset & pRs);
	void TranslateCT();

// Dialog Data
	//{{AFX_DATA(CWebCertManPage)
	enum { IDD = IDD_PROPPAGE_WEBCERTMAN };
	CBitmapSlider	m_Slider;
	CSuperGridCtrl	m_ListOne;
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWebCertManPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AddList();
	// Generated message map functions
	//{{AFX_MSG(CWebCertManPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBquery();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnDblclkListOne(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBcview();
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBitmapSliderMoving(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	void BatchCert();
	void RevokeCert(CString strID);
	void MadeCert(CString strID, CADORecordset & pRs);
	void CancelMark(CString strID);
	void MarkCert(CString strID);
	void MadeCsrOnly(CString strId);
	void AddMsg(CString info, DWORD type = M_OK);
	void GoToPage(register int nPageIndex, CString strId = "");

	enum ReqEntry	//共计11项 , 通过ISAPI传入的用户申请
	{
		USERINFO = 0,	
		KEYUSAGE,
		EKEYSUAGE,
		FRINEDNAME,
		KEYLEN,
		INPUTIME,
		CERTDAY,
		CERTYPE,
		CERTSTATE,
		MADETIME,
		REVOKETIME
	};

	enum CertType	//证书处理状态
	{
		REQ = 0,	//已经申请
		//MARKCSR,	//标记制作CSR
		MADECSR,	//已经制作CSR(别的尚未制作)
		MARKCERT,	//标记制作证书(标记后可批量制作)
		MADECERT,	//已经制作全部证书
		REVOKE		//证书已经作废
	};

	BOOL m_bIsSignleRec;	//是否单记录
	CADORecordset m_pRs;
	CADODatabase m_pDb;

	DWORD m_dCurrentPage;
	CImageList m_Image;//Must provide an imagelist
	CSuperGridCtrl::CTreeItem * m_pRootItem;
	CSuperGridCtrl::CTreeItem * m_ItemInfo[ReqMaxEntry];	//[]高于* ,整体上是数组
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBCERTMANPAGE_H__FB9FC48C_717A_47E8_952A_86CAB432D40B__INCLUDED_)
