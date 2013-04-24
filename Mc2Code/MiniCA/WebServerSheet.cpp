// WebServerSheet.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "WebServerSheet.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebServerSheet property page

IMPLEMENT_DYNCREATE(CWebServerSheet, CPropertyPage)

CWebServerSheet::CWebServerSheet() : CPropertyPage(CWebServerSheet::IDD)
{
	//{{AFX_DATA_INIT(CWebServerSheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWebServerSheet::~CWebServerSheet()
{
}

void CWebServerSheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebServerSheet)
	DDX_Control(pDX, IDC_TAB_SHEET, m_WebSheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebServerSheet, CPropertyPage)
	//{{AFX_MSG_MAP(CWebServerSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebServerSheet message handlers

BOOL CWebServerSheet::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
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

	m_WebSheet.SetImageList(&m_ImgList);
	m_WebSheet.AddPage(MiniCT_0900, 0, &m_PageWebServer, IDD_PROPPAGE_WEBSERVER);	//MiniCT_0900  "Web Server"
	m_WebSheet.AddPage(MiniCT_0901, 1, &m_PageWebCertMan, IDD_PROPPAGE_WEBCERTMAN); //MiniCT_0901  "Web证书管理"
	
	m_WebSheet.Show();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWebServerSheet::IsWebRun()
{
	return m_PageWebServer.IsWebRun();
}

void CWebServerSheet::Language()
{
	CLanguage::TranslateDialog(m_PageWebServer.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_WEBSERVER));
	CLanguage::TranslateDialog(m_PageWebCertMan.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_WEBCERTMAN));
	CLanguage::TranslateDialog(this->m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_WEBSHEET));
	m_PageWebServer.TranslateCT();
	m_PageWebCertMan.TranslateCT();
}