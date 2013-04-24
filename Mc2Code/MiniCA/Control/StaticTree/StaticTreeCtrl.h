#if !defined(AFX_STATICTREECTRL_H__A9777424_A55F_4A55_BE9D_5308028DB601__INCLUDED_)
#define AFX_STATICTREECTRL_H__A9777424_A55F_4A55_BE9D_5308028DB601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticTreeCtrl.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#define		CM_INSERTCHILD				WM_APP + 10000
#define		CM_INSERTSIBLING			WM_APP + 10001
#define		CM_DELETENODE				WM_APP + 10002
#define		CM_MODIFYNODETEXT			WM_APP + 10003
#define		CM_CHANGENODECOLOR			WM_APP + 10004
#define		CM_TOGGLECONNECTINGLINES	WM_APP + 10010
#define		CM_SETCONNECTINGLINESCOLOR	WM_APP + 10011
#define		CM_SETFONT					WM_APP + 10020
#define		CM_SETDEFAULTCOLOR			WM_APP + 10021
#define		CM_SETBACKGROUNDBITMAP		WM_APP + 10022
#define		CM_TOGGLEMENUSOUND			WM_APP + 10030

#if defined( _UNICODE )
	#define STRCPY(x,y)				wcscpy(x,y)
#else
	#define STRCPY(x,y)				strcpy(x,y)
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeNode class

class CTreeNode
{
public:
	CTreeNode()
	{
		csLabel.Empty();
		rNode.SetRectEmpty();

		bUseDefaultTextColor = TRUE;

		bOpen		= TRUE;

		pParent		= NULL;
		pSibling	= NULL;
		pChild		= NULL;
	}

	virtual ~CTreeNode()
	{
		csLabel.Empty();
	}

	CString		csLabel;
	CRect		rNode;
	
	COLORREF	crText;
	BOOL		bUseDefaultTextColor;

	BOOL		bOpen;

	CTreeNode*	pParent;
	CTreeNode*	pSibling;
	CTreeNode*	pChild;
};

#define    HTREENODE   CTreeNode*
#define    HTOPNODE    ( (HTREENODE) -0x10000 )

/////////////////////////////////////////////////////////////////////////////
// CStaticTreeCtrl window

class CStaticTreeCtrl : public CStatic
{
// Construction
public:
	CStaticTreeCtrl();

// Attributes
protected:
	LOGFONT			m_lgFont;
	CFont			m_Font;
	COLORREF		m_crDefaultTextColor;
	COLORREF		m_crConnectingLines;

	BOOL			m_bShowLines;

	CBitmap			m_bmpBackground;
	
	int				m_iDocHeight;
	BOOL			m_bScrollBarMessage;
	
	int				m_iLineHeight;
	int				m_iIndent;
	int				m_iPadding;

	HTREENODE		m_pTopNode;
	HTREENODE		m_pSelected;

	BOOL			m_bAudioOn;

// Operations
public:
	virtual CStaticTreeCtrl&	SetTextFont			( LONG nHeight, BOOL bBold, BOOL bItalic, const CString& csFaceName );
	virtual CStaticTreeCtrl&	SetDefaultTextColor	( COLORREF crText );
	
	HTREENODE		InsertSibling	(	HTREENODE pInsertAfter, const CString& csLabel,
										COLORREF crText = 0, BOOL bUseDefaultTextColor = TRUE,
										BOOL bInvalidate = FALSE );
	HTREENODE		InsertChild		(	HTREENODE pParent, const CString& csLabel,
										COLORREF crText = 0, BOOL bUseDefaultTextColor = TRUE,
										BOOL bInvalidate = FALSE );
	void			DeleteNode		( HTREENODE pNode, BOOL bInvalidate = FALSE );

	void			ToggleNode		( HTREENODE pNode, BOOL bInvalidate = FALSE );
	void			SetNodeColor	( HTREENODE pNode, COLORREF crText, BOOL bInvalidate = FALSE );

	void			SetBackgroundBitmap	( BOOL bInvalidate = FALSE );

	void			SetNodeText(HTREENODE pNode, CString strText);

protected:
	void			DeleteNodeRecursive	( HTREENODE pNode );		// Recursive delete

	int				DrawNodesRecursive	( CDC* pDC, HTREENODE pNode, int x, int y, CRect rFrame );
	int				HowMuchTextFits		( CDC* pDC, int iAvailableWidth, CString csText );

	void			DrawLinesRecursive	( CDC* pDC, HTREENODE pNode );

	void			ResetScrollBar		();

	HTREENODE		FindNodeByPoint		( const CPoint& point, HTREENODE pNode );
	BOOL			NodeTextDlg			( CString& csText );

	// Message handlers
/*	void			OnCM_InsertChild();
	void			OnCM_InsertSibling();
	void			OnCM_DeleteNode();
	void			OnCM_ModifyNodeText();
	void			OnCM_ChangeNodeColor();
	void			OnCM_ToggleConnectingLines();
	void			OnCM_SetConnectingLinesColor();
	void			OnCM_SetFont();
	void			OnCM_SetDefaultColor();
	void			OnCM_SetBackgroundBitmap();
	void			OnCM_ToggleMenuSound();*/

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticTreeCtrl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticTreeCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICTREECTRL_H__A9777424_A55F_4A55_BE9D_5308028DB601__INCLUDED_)
