#if !defined(AFX_CACERTMANPAGE_H__31E76277_59E6_4677_B9DD_7B845FC6D1F8__INCLUDED_)
#define AFX_CACERTMANPAGE_H__31E76277_59E6_4677_B9DD_7B845FC6D1F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCertManPage.h : header file
//
#include ".\Control\SuperList\SuperGridCtrl.h"
#include ".\Control\Ado\ado2.h"
#include ".\Control\tooltip\OXToolTipCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCaCertManPage dialog

class CCaCertManPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCertManPage)

// Construction
public:
	BOOL IsDbReady();
	void QueryCert(BOOL bDelAll);
	BOOL SelectCert(CString strSQL);
	BOOL SelectRevoke(CString strSQL);
	void SetNamePath(const CString &strPath);
	void TranslateCT();

	UINT GetCertSn();
	CCaCertManPage();
	~CCaCertManPage();
	void SaveDb(stuSUBJECT * pSUBJECT, const int uCertLen, const int iCertType,
		const int uCertDay, void * p12, const int p12l, CString pwd);

// Dialog Data
	//{{AFX_DATA(CCaCertManPage)
	enum { IDD = IDD_PROPPAGE_CAMAN };
	CSuperGridCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCertManPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	void SetCertSn(long lSn);
	BOOL GetCertType();
	// Generated message map functions
	//{{AFX_MSG(CCaCertManPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InsertCert(CString CN, CString SN, CString Key,  CString  Info, CString Begin, CString  Day, UINT Type);
	void AddMsg(CString info, DWORD type = M_OK);
	CADODatabase m_pDb;
	CString m_Path;
	BOOL m_bMaxSn;
	BOOL m_bDbReady;	//是否连接数据库成功
	CImageList m_Image;//Must provide an imagelist
	CSuperGridCtrl::CTreeItem * m_pRootItem;
	CSuperGridCtrl::CTreeItem * m_pRevokwItem;
	CPtrList m_ObCerType;//列表
	struct stuCerType
	{
		CSuperGridCtrl::CTreeItem * m_pItem;
		UINT m_Type;
		stuCerType()
		{
			memset(this,0,sizeof(stuCerType));
		}
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACERTMANPAGE_H__31E76277_59E6_4677_B9DD_7B845FC6D1F8__INCLUDED_)
