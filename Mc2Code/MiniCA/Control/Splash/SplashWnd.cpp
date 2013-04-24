// SplashWnd.cpp : implementation file
//
// ©1998-2001 Codejock Software, All Rights Reserved.
// Based on the Visual C++ splash screen component.
//
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplashWnd.h"

#include "..\BitmapSlider\memdc.h" // "Flicker Free Drawing In MFC" by Keith Rule

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL        CSplashWnd::m_bShowSplashWnd;
CSplashWnd* CSplashWnd::m_pSplashWnd;
UINT		CSplashWnd::m_nTimerID;


CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	// Kill the screen if it's still active
	if( m_pSplashWnd ) HideSplashScreen();
	
	// Clear the static window pointer.
	ASSERT(m_pSplashWnd == this);
	m_pSplashWnd = NULL;
	
	// Kill the timer
	if( m_nTimerID != 0 ) KillTimer( m_nTimerID );
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	m_bShowSplashWnd = bEnable;
}

//BOOL CSplashWnd::ShowSplashScreen(UINT m_nTimerID, UINT uBitmapID, CWnd* pParentWnd /*= NULL*/)
BOOL CSplashWnd::ShowSplashScreen(UINT m_nTimerID, CString sResourceType, CString sResource, CWnd* pParentWnd /*= NULL*/)
{
	ASSERT(m_nTimerID && sResource);
	
	if (!m_bShowSplashWnd || m_pSplashWnd != NULL) {
		return FALSE;
	}

	// Allocate a new splash screen, and create the window.
	m_pSplashWnd = new CSplashWnd;

	if(!m_pSplashWnd->m_cPicture.Load(sResourceType, sResource))
		return FALSE;

	CDC DC;
	DC.Attach(::GetDC(NULL));

	CSize picSize = m_pSplashWnd->m_cPicture.GetSize(&DC);

	::ReleaseDC(NULL, DC.Detach());

//	if (!m_pSplashWnd->m_bitmap.LoadBitmap(uBitmapID)) {
//		return FALSE;
//	}

//	BITMAP bm;
//	m_pSplashWnd->m_bitmap.GetBitmap(&bm);

	CString strWndClass = AfxRegisterWndClass(0,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	if (!m_pSplashWnd->CreateEx(0, strWndClass, NULL, WS_POPUP | WS_VISIBLE,
		0, 0, picSize.cx, picSize.cy, pParentWnd->GetSafeHwnd(), NULL))
	{
		TRACE0("Failed to create splash screen.\n");
		delete m_pSplashWnd;
		return FALSE;
	}

	// Center the window.
	m_pSplashWnd->CenterWindow();
	m_pSplashWnd->UpdateWindow();

	// Set a timer to destroy the splash screen.
	m_pSplashWnd->SetTimer(1, m_nTimerID, NULL);

	return TRUE;
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (m_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		m_pSplashWnd->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);
	CMemDC cDC(&dc);
	CSize size = m_cPicture.GetSize(&dc);

	m_cPicture.Draw(&cDC, CPoint(0, 0), size);

/*	CDC dcImage;
	if (dcImage.CreateCompatibleDC(&dc))
	{
		BITMAP bm;
		m_bitmap.GetBitmap(&bm);

		// Paint the image.
		CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
		dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
		dcImage.SelectObject(pOldBitmap);

//		char buf[128];
//		sprintf(buf,"Licenzed to me!");
//		dc.TextOut(10,10,buf,strlen(buf));
	}*/
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreen();
}

void CSplashWnd::KillSplashScreen( void )
{
	if( m_pSplashWnd == NULL ) return;
	
	if( m_nTimerID ) m_pSplashWnd->KillTimer( m_nTimerID );
	m_nTimerID = 0;
	m_pSplashWnd->HideSplashScreen();
}
