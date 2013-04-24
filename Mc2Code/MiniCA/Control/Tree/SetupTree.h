#if !defined(AFX_SETUPTREE_H__F49CA222_B740_4034_BC23_8833D0982883__INCLUDED_)
#define AFX_SETUPTREE_H__F49CA222_B740_4034_BC23_8833D0982883__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupTree.h : header file
// 动态按钮和文本框
#include "..\..\certdb.h"
/////////////////////////////////////////////////////////////////////////////
// CSetupTree window

class CSetupTree : public CTreeCtrl
{
// Construction
public:
	CSetupTree();

// Attributes
public:
	CPtrList m_HtreeList;
	void Savecdb(); //保存证书库
	void LoadCdb(); //加载证书库
	void SelectFile() ;
	void ExpandParent(HTREEITEM item,UINT nCode );//展开,收缩

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSetupTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSetupTree)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOptionsButtonId();
	afx_msg void OnDestroy();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void CreateNewChildControl(HTREEITEM hItem);
	void UpdateTreeControlTextFromChildControl();
	void TravelChild(HTREEITEM hItem, int nState);
	void TravelSiblingAndParent(HTREEITEM hItem, int nState);
	CEdit * m_pEdit;
	CXPStyleButtonST * m_pButton;
	CString m_CdbPath;//证书库文件路径
	HTREEITEM m_hControlItem;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPTREE_H__F49CA222_B740_4034_BC23_8833D0982883__INCLUDED_)
