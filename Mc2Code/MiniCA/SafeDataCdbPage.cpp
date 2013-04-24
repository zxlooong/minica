// SafeDataCdbPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "SafeDataCdbPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSafeDataCdbPage property page

IMPLEMENT_DYNCREATE(CSafeDataCdbPage, CPropertyPage)

CSafeDataCdbPage::CSafeDataCdbPage() : CPropertyPage(CSafeDataCdbPage::IDD)
{
	//{{AFX_DATA_INIT(CSafeDataCdbPage)
	//}}AFX_DATA_INIT
}

CSafeDataCdbPage::~CSafeDataCdbPage()
{
}

void CSafeDataCdbPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSafeDataCdbPage)
//	DDX_Control(pDX, IDC_EDIT9, m_XpEdit9);
//	DDX_Control(pDX, IDC_EDIT6, m_XpEdit6);
//	DDX_Control(pDX, IDC_EDIT3, m_XpEdit3);
	//}}AFX_DATA_MAP
    DDX_FileEditCtrl(pDX, IDC_EDIT8, m_Edit8, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT7, m_Edit7, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT5, m_Edit5, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT4, m_Edit4, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT2, m_Edit2, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT1, m_Edit1, FEC_FILEOPEN);
}


BEGIN_MESSAGE_MAP(CSafeDataCdbPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSafeDataCdbPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSafeDataCdbPage message handlers

BOOL CSafeDataCdbPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
    HANDLE handle = ::LoadImage(AfxGetInstanceHandle(),
                                MAKEINTRESOURCE(IDI_ICON_DIR),
                                IMAGE_ICON,
                                0,
                                0,
                                LR_DEFAULTCOLOR);

    m_Edit8.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_Edit8.SetButtonWidth(18);
	m_Edit8.SetCaption(MiniCT_0714);	//私鈅
	m_Edit8.SetFilter(MiniCT_0715);

    m_Edit7.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_Edit7.SetButtonWidth(18);
	m_Edit7.SetCaption(MiniCT_0720);	//公鈅
	m_Edit7.SetFilter(MiniCT_0721);

    m_Edit5.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_Edit5.SetButtonWidth(18);
	m_Edit5.SetCaption(MiniCT_0714);
	m_Edit5.SetFilter(MiniCT_0715);

    m_Edit4.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_Edit4.SetButtonWidth(18);
	m_Edit4.SetCaption(MiniCT_0720);
	m_Edit4.SetFilter(MiniCT_0721);

    m_Edit2.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_Edit2.SetButtonWidth(18);
	m_Edit2.SetCaption(MiniCT_0714);
	m_Edit2.SetFilter(MiniCT_0715);

    m_Edit1.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_Edit1.SetButtonWidth(18);
	m_Edit1.SetCaption(MiniCT_0734);	//选择公钥证书文件(可多选)
	m_Edit1.SetFilter(MiniCT_0721);
	m_Edit1.ModifyFlags(0, FEC_MULTIPLE);

	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_EDIT3)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT6)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT9)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_SAFEDS_CHECK1)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_SAFEDS_CHECK2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_SAFEDS_CHECK3)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_SAFEDS_CHECK4)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_SAFEDS_CHECK5)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_SAFEDS_CHECK6)->m_hWnd,TRUE);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSafeDataCdbPage::GetCertPair(EncType eType, BOOL bKeyType, 
							   char * sCert, DWORD & dLen,
							   CString & strPwd, CColorListBox * pList)
{
	CColorListBox * p_List = NULL;
	if(!pList)
	{
		p_List = &((CMiniMainDlg *)AfxGetMainWnd())->m_ListBox;
	}
	else
		p_List = pList;
	BOOL bSucceed = TRUE;
	CString strPath;
	dLen = 0;
	switch(eType)
	{
	case SEAL:
		if(bKeyType)//公钥
		{
			GetDlgItemText(IDC_EDIT1, strPath);
			if(strPath.IsEmpty()) //调用内部证书
			{
				bSucceed = 	((CMiniCaApp *)AfxGetApp())->GetCertPair(300, sCert, dLen, strPwd, TRUE, p_List);
			}
			else
			{
				strcpy(sCert, strPath.GetBuffer(0));
				if(IsWindow(p_List->m_hWnd))
					p_List->AddMsg(MiniCT_1400, M_WARING);	//MiniCT_1400 加载外部证书...
			}

		}
		else//私钥
		{
			GetDlgItemText(IDC_EDIT2, strPath);
			GetDlgItemText(IDC_EDIT3, strPwd);
			if(strPath.IsEmpty()) //调用内部证书
			{
				bSucceed = 	((CMiniCaApp *)AfxGetApp())->GetCertPair(310, sCert, dLen, strPwd, TRUE, p_List);
			}
			else
			{
				strcpy(sCert, strPath.GetBuffer(0));
				if(IsWindow(p_List->m_hWnd))
					p_List->AddMsg(MiniCT_1400, M_WARING);
			}
		}
		break;
	case SIGN:
		if(bKeyType)//公钥
		{
			GetDlgItemText(IDC_EDIT4, strPath);
			if(strPath.IsEmpty()) //调用内部证书
			{
				bSucceed = 	((CMiniCaApp *)AfxGetApp())->GetCertPair(500, sCert, dLen, strPwd, TRUE, p_List);
			}
			else
			{
				strcpy(sCert, strPath.GetBuffer(0));
				if(IsWindow(p_List->m_hWnd))
					p_List->AddMsg(MiniCT_1400, M_WARING);
			}
		}
		else//私钥
		{
			GetDlgItemText(IDC_EDIT5, strPath);
			GetDlgItemText(IDC_EDIT6, strPwd);
			if(strPath.IsEmpty()) //调用内部证书
			{
				bSucceed = 	((CMiniCaApp *)AfxGetApp())->GetCertPair(510, sCert, dLen, strPwd, TRUE, p_List);
			}
			else
			{
				strcpy(sCert, strPath.GetBuffer(0));
				if(IsWindow(p_List->m_hWnd))
					p_List->AddMsg(MiniCT_1400, M_WARING);
			}
		}
		break;
	case PubEnc:
	case PrivEnc:
		if(bKeyType)//公钥
		{
			GetDlgItemText(IDC_EDIT7, strPath);
			if(strPath.IsEmpty()) //调用内部证书
			{
				bSucceed = 	((CMiniCaApp *)AfxGetApp())->GetCertPair(400, sCert, dLen, strPwd, TRUE, p_List);
			}
			else
			{
				strcpy(sCert, strPath.GetBuffer(0));
				if(IsWindow(p_List->m_hWnd))
					p_List->AddMsg(MiniCT_1400, M_WARING);
			}
		}
		else//私钥
		{
			GetDlgItemText(IDC_EDIT8, strPath);
			GetDlgItemText(IDC_EDIT9, strPwd);
			if(strPath.IsEmpty()) //调用内部证书
			{
				bSucceed = 	((CMiniCaApp *)AfxGetApp())->GetCertPair(410, sCert, dLen, strPwd, TRUE, p_List);
			}
			else
			{
				strcpy(sCert, strPath.GetBuffer(0));
				if(IsWindow(p_List->m_hWnd))
					p_List->AddMsg(MiniCT_1400, M_WARING);
			}
		}
		break;
	}
	return bSucceed;
}


void CSafeDataCdbPage::TranslateCT()
{
	SetDlgItemText(IDC_SAFEDS_STATIC1,	MiniCT_11501);
	SetDlgItemText(IDC_SAFEDS_STATIC2,	MiniCT_11502);
	SetDlgItemText(IDC_SAFEDS_STATIC3,	MiniCT_11503);
	SetDlgItemText(IDC_SAFEDS_STATIC4,	MiniCT_11504);
	SetDlgItemText(IDC_SAFEDS_STATIC5,	MiniCT_11505);
	SetDlgItemText(IDC_SAFEDS_STATIC6,	MiniCT_11506);
	SetDlgItemText(IDC_SAFEDS_CHECK1,	MiniCT_11507);
	SetDlgItemText(IDC_SAFEDS_CHECK2,	MiniCT_11508);
	SetDlgItemText(IDC_SAFEDS_CHECK3,	MiniCT_11509);
	SetDlgItemText(IDC_SAFEDS_CHECK4,	MiniCT_11510);
	SetDlgItemText(IDC_SAFEDS_CHECK5,	MiniCT_11511);
	SetDlgItemText(IDC_SAFEDS_CHECK6,	MiniCT_11512);
	SetDlgItemText(IDC_SAFEDS_SAVE,	MiniCT_11513);
}