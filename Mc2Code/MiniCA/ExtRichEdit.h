#if !defined(AFX_EXTRICHEDIT_H__971ECB21_8956_411A_B90C_5FB1C6888240__INCLUDED_)
#define AFX_EXTRICHEDIT_H__971ECB21_8956_411A_B90C_5FB1C6888240__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtRichEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtRichEdit window

class CExtRichEdit : public CRichEditCtrl
{
// Construction
public:
	CExtRichEdit();

// Attributes
public:

// Operations
public:
	void AddMsg(CString strMSG, COLORREF crNewColor=RGB(0, 128, 0), BOOL bUnderLine = FALSE, BOOL bBold = FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtRichEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtRichEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtRichEdit)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRICHEDIT_H__971ECB21_8956_411A_B90C_5FB1C6888240__INCLUDED_)
