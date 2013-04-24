// TabSheet.cpp : implementation file
//

#include "stdafx.h"
#include "TabSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSheet

CTabSheet::CTabSheet()
{
	m_nNumOfPages = 0;
	m_nCurrentPage = 0;
}

CTabSheet::~CTabSheet()
{
}


BEGIN_MESSAGE_MAP(CTabSheet, CTabCtrl)
	//{{AFX_MSG_MAP(CTabSheet)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSheet message handlers

BOOL CTabSheet::AddPage(LPCTSTR title, UINT uImgIndex, CDialog *pDialog, UINT ID)
{
	if( MAXPAGE == m_nNumOfPages )
		return FALSE;
	
	m_IDD[m_nNumOfPages] = ID;
	m_Title[m_nNumOfPages] = title;
	
//	pDialog->DestroyWindow();
	pDialog->Create(m_IDD[m_nNumOfPages], this->GetParent());	//经验 TAB改变标题,但是控件不隐藏的做法 this->GetParent()
	
	m_pPages[m_nNumOfPages] = pDialog;
	
	InsertItem( m_nNumOfPages, m_Title[m_nNumOfPages], uImgIndex);
	
	m_nNumOfPages++;
	
	return TRUE;
}

void CTabSheet::SetRect()
{
	int cx, cy, x, y, nWidth, nHeight;;

	CRect tabRect, itemRect;
	GetItemRect (0, &itemRect);
	GetClientRect(&tabRect);
	cx = tabRect.Width();
	cy = tabRect.Height();
	
	DWORD dwStyle = GetStyle();
	
	if (dwStyle & TCS_VERTICAL) 
	{ // Vertical tabs
		int nTabWidth =	itemRect.Width () * GetRowCount ();
		x = (dwStyle & TCS_RIGHT) ? 4 : nTabWidth + 4;
		y = 4;
		nWidth = cx - nTabWidth - 8;
		nHeight = cy - 8;
	}
	else 
	{ // Horizontal tabs
		int nTabHeight = itemRect.Height () * GetRowCount ();
		x = 4;
		y = (dwStyle & TCS_BOTTOM) ? 4 : nTabHeight + 4;
		nWidth = cx - 8;
		nHeight = cy - nTabHeight - 8;
	}
	
	m_pPages[0]->SetWindowPos(&wndTop, x, y, nWidth, nHeight, SWP_SHOWWINDOW ); 
	for( int nCount=1; nCount < m_nNumOfPages; nCount++ )
		m_pPages[nCount]->SetWindowPos(&wndTop, x, y, nWidth, nHeight, SWP_HIDEWINDOW);
	
}

void CTabSheet::Show()
{
	SetRect();
}

void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_nCurrentPage != GetCurFocus())
	{
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		m_nCurrentPage=GetCurFocus();
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
//		m_pPages[m_nCurrentPage]->SetFocus();
	}
}

int CTabSheet::SetCurSel(int nItem)
{
	if( nItem < 0 || nItem >= m_nNumOfPages)
		return -1;

	int ret = m_nCurrentPage;

	if(m_nCurrentPage != nItem )
	{
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
		m_nCurrentPage = nItem;
		m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
//		m_pPages[m_nCurrentPage]->SetFocus();
		CTabCtrl::SetCurSel(nItem);
	}

	return ret;
}

int CTabSheet::GetCurSel()
{
	return CTabCtrl::GetCurSel();
}

void CTabSheet::SetPageName(UINT uPageIndex, CString strTitle)
{
	m_Title[uPageIndex] = strTitle;
	TC_ITEM tcItem;
	tcItem.mask = TCIF_TEXT | TCIF_RTLREADING;
	tcItem.pszText = (LPTSTR)((LPCTSTR)strTitle);
	BOOL b = SetItem(uPageIndex, &tcItem );
}
