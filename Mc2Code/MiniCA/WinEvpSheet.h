#if !defined(AFX_WINEVPSHEET_H__78865AFF_0E2D_4961_BACF_61214FF05952__INCLUDED_)
#define AFX_WINEVPSHEET_H__78865AFF_0E2D_4961_BACF_61214FF05952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinEvpSheet.h : header file
//
#include ".\Control\TabSheet\TabSheet.h"
#include "SafeInfoPage.h"
#include "SafeDataSetPage.h"
#include "SafeDataCdbPage.h"

/////////////////////////////////////////////////////////////////////////////
// CWinEvpSheet dialog

class CWinEvpSheet : public CPropertyPage
{
	DECLARE_DYNCREATE(CWinEvpSheet)

// Construction
public:
	CWnd * GetPage(CString strPageName);
	void Language();
	CWinEvpSheet();
	~CWinEvpSheet();

// Dialog Data
	//{{AFX_DATA(CWinEvpSheet)
	enum { IDD = IDD_PROPPAGE_EVPSHEET };
	CTabSheet	m_PageWinEvpSheet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWinEvpSheet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWinEvpSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CImageList m_ImgList;
	CSafeInfoPage m_PageSafeInfo;
	CSafeDataSetPage m_PageSafeDataSet;
	CSafeDataCdbPage m_PageSafeDataCdb;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINEVPSHEET_H__78865AFF_0E2D_4961_BACF_61214FF05952__INCLUDED_)
