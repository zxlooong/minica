#if !defined(AFX_COLORLISTBOX_H__4DC6E762_4FBB_4F05_B65A_82DD2A7F7B68__INCLUDED_)
#define AFX_COLORLISTBOX_H__4DC6E762_4FBB_4F05_B65A_82DD2A7F7B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorListBox.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CColorListBox window
#define M_OK		10000	//MB_OK
#define M_ERROR		20000	//MB_ICONERROR
#define M_WARING	30000	//MB_ICONWARNING

class CColorListBox : public CListBox
{
// Construction
public:
	CColorListBox();
	int AddMsg(CString lpszItem, DWORD info = M_OK);
	void DeleteLast();

	BOOL SetBkImage(UINT nIDResource);
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSaveFile(CString strFileName);
	void DestroyBtn();
	void AddButInfo(CString lpszItem,DWORD info = M_OK);
	void SetBkColor(COLORREF bkColor);
	virtual ~CColorListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorListBox)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
//	afx_msg void OnBNo();
//	afx_msg void OnBYes();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bFirst;
	CFont	m_font;
	COLORREF m_BkColor;
//	CButtonST * m_pButYes;
//	CButtonST * m_pButNo;
	CBitmap m_bitmap;
	CBrush m_brBmp;
	CString m_FileName;//右键保存时候的文件名

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORLISTBOX_H__4DC6E762_4FBB_4F05_B65A_82DD2A7F7B68__INCLUDED_)
