#if !defined(AFX_BmpTree_H__E692EF41_30F0_42ED_A93D_5DF6CAC3EA53__INCLUDED_)
#define AFX_BmpTree_H__E692EF41_30F0_42ED_A93D_5DF6CAC3EA53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpTree window

class CBmpTree : public CTreeCtrl
{
// Construction
public:
	CBmpTree();

// Attributes
public:
	BOOL SetBkImage(UINT nIDResource);
	void ExpandParent(HTREEITEM item,UINT nCode );////展开,收缩
	BOOL GotoPage(CString pageName);//转到某页
	HTREEITEM FindItem(HTREEITEM hItem, CString pageName);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBmpTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpTree)
//	afx_msg void OnDropFiles(HDROP hDropInfo);
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
//	CString ExpandShortcut(char * inFile);
	CBitmap m_bitmap;
	HCURSOR	m_hCursor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BmpTree_H__E692EF41_30F0_42ED_A93D_5DF6CAC3EA53__INCLUDED_)
