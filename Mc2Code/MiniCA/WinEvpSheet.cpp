// WinEvpSheet.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "WinEvpSheet.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinEvpSheet property page

IMPLEMENT_DYNCREATE(CWinEvpSheet, CPropertyPage)

CWinEvpSheet::CWinEvpSheet() : CPropertyPage(CWinEvpSheet::IDD)
{
	//{{AFX_DATA_INIT(CWinEvpSheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWinEvpSheet::~CWinEvpSheet()
{
}

void CWinEvpSheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinEvpSheet)
	DDX_Control(pDX, IDC_TAB_SHEET, m_PageWinEvpSheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWinEvpSheet, CPropertyPage)
	//{{AFX_MSG_MAP(CWinEvpSheet)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinEvpSheet message handlers

BOOL CWinEvpSheet::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ImgList.Create(16,16,TRUE|ILC_COLOR24,16,1);
	
	HICON hIcon =  NULL;
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_INFO), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//0
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_DATA), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//1
	DestroyIcon(hIcon);

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_SETUP), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//1
	DestroyIcon(hIcon);
	
	m_PageWinEvpSheet.SetImageList(&m_ImgList);
	m_PageWinEvpSheet.AddPage(MiniCT_1200, 0, &m_PageSafeInfo, IDD_PROPPAGE_SAFEI);			//MiniCT_1200
	m_PageWinEvpSheet.AddPage(MiniCT_1201, 1, &m_PageSafeDataSet, IDD_PROPPAGE_SAFED);		//MiniCT_1201
	m_PageWinEvpSheet.AddPage(MiniCT_1202, 2, &m_PageSafeDataCdb, IDD_PROPPAGE_SAFEDS);		//MiniCT_1202

	m_PageWinEvpSheet.Show();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWinEvpSheet::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	m_PageSafeInfo.DestroyWindow();
	m_PageSafeDataSet.DestroyWindow();
	m_PageSafeDataCdb.DestroyWindow();
}

//通过类名，返回制定属性页的对象指针
CWnd * CWinEvpSheet::GetPage(CString strPageName)
{
	//枚举属性页
	CRuntimeClass * prt = NULL;

	prt = m_PageSafeInfo.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageSafeInfo;

	prt = m_PageSafeDataSet.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageSafeDataSet;

	prt = m_PageSafeDataCdb.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageSafeDataCdb;

	return NULL;
}

void CWinEvpSheet::Language()
{
	CLanguage::TranslateDialog(m_PageSafeInfo.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_SAFEI));
	CLanguage::TranslateDialog(m_PageSafeDataSet.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_SAFED));
	CLanguage::TranslateDialog(m_PageSafeDataCdb.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_SAFEDS));
	CLanguage::TranslateDialog(this->m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_EVPSHEET));

	m_PageWinEvpSheet.SetPageName(0, MiniCT_1200);
	m_PageWinEvpSheet.SetPageName(1, MiniCT_1201);
	m_PageWinEvpSheet.SetPageName(2, MiniCT_1202);

	m_PageSafeInfo.TranslateCT();
	m_PageSafeDataSet.TranslateCT();
	m_PageSafeDataCdb.TranslateCT();
}