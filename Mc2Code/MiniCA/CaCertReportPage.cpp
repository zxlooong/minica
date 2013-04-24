// CaCertReportPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CaCertReportPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "minict.h"


//#include "CryptuiAPI.h"

//#include "e:\SDK\include\WinCrypt.h"
//#include "D:\SDK\include\CryptuiAPI.h"
//d:\sdk\lib\CryptUI.lib 
//D:\SDK\INCLUDE
//D:\SDK\LIB

/*
CryptUIDlgViewContext on XP and Windows Server 2003
CertViewProperties on Windows 2000 Pro and Server.
CryptUIDlgViewCertificate on Win98+*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCertReportPage property page

IMPLEMENT_DYNCREATE(CCaCertReportPage, CPropertyPage)

CCaCertReportPage::CCaCertReportPage() : CPropertyPage(CCaCertReportPage::IDD)
{
	//{{AFX_DATA_INIT(CCaCertReportPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPageCaType = NULL;
	m_pPageCaInfo = NULL;
	m_pPageCaExt = NULL;
	m_hCertRoot = NULL;
}

CCaCertReportPage::~CCaCertReportPage()
{
	// Free the DLL module.
}

void CCaCertReportPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCertReportPage)
	DDX_Control(pDX, IDC_STATIC_WIZARD, m_WizardTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaCertReportPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCertReportPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCertReportPage message handlers

BOOL CCaCertReportPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_WizardTree
		.SetTextFont( 10, FALSE, FALSE, "Arial Unicode MS" )
		.SetDefaultTextColor( RGB(0,64,255));
	
	m_toolTip.Create(this);
	m_toolTip.AddTool(GetDlgItem(IDC_STATIC_WIZARD), "������֤�����ݱ����");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCaCertReportPage::GetWizard(CCaCertTypePage * pPageCaType, 
							 CCaCertInfoPage * pPageCaInfo,
							 CCaCertExtPage * pPageCaExt)
{
	if(pPageCaType && pPageCaInfo && pPageCaExt)
	{
		m_pPageCaType = pPageCaType;
		m_pPageCaInfo = pPageCaInfo;
		m_pPageCaExt = pPageCaExt;
		return TRUE;
	}
	else
		return FALSE;
}

void CCaCertReportPage::ViewWizardInfo()
{
	// Tree data
	if(m_hCertRoot)
		m_WizardTree.DeleteNode(m_hCertRoot);

	m_hCertRoot	= m_WizardTree.InsertChild( HTOPNODE, _T(""));
	m_WizardTree.SetNodeColor(m_hCertRoot,RGB(0,64,255));

	HTREENODE hCertExt	= m_WizardTree.InsertChild( m_hCertRoot, _T(MiniCT_0500)); //MiniCT_0500 "��֤��չ"

	HTREENODE hCertInfo	= m_WizardTree.InsertChild( m_hCertRoot, _T(MiniCT_0501));  //MiniCT_0501 "��֤��Ϣ"
			HTREENODE hInfoSave = m_WizardTree.InsertChild( hCertInfo, _T(MiniCT_0502)); //MiniCT_0502 "�洢��Ϣ"
			HTREENODE hInfoHold = m_WizardTree.InsertChild( hCertInfo, _T(MiniCT_0503));	//MiniCT_0503 "Լ����Ϣ"
			HTREENODE hInfoBase = m_WizardTree.InsertChild( hCertInfo, _T(MiniCT_0504));	//MiniCT_0504 "������Ϣ"

	HTREENODE hCertType	= m_WizardTree.InsertChild( m_hCertRoot, _T(MiniCT_0505));	//MiniCT_0505 "��֤����"
			HTREENODE hTypeEkusage = m_WizardTree.InsertChild( hCertType, _T(MiniCT_0506));	//MiniCT_0506 "��ǿ��Կ����"
			HTREENODE hTypeKusage = m_WizardTree.InsertChild( hCertType, _T(MiniCT_0507));	//MiniCT_0507 "��Կ����"
			HTREENODE hTypeType = m_WizardTree.InsertChild( hCertType, _T(MiniCT_0508));	//MiniCT_0508 "֤������"

	m_pPageCaType->GetCertTypeInfo(&m_WizardTree, hTypeType, hTypeKusage, hTypeEkusage);
	BOOL bFull = m_pPageCaInfo->GetCertInfo(&m_WizardTree, hInfoBase, hInfoHold, hInfoSave);
	m_pPageCaExt->GetCertInfo(&m_WizardTree, hCertExt);

	
	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCaCertWizardSheet * pWizard = (CCaCertWizardSheet *)pMain->GetPage("CCaCertWizardSheet");
	CCaCertManPage * pMan = (CCaCertManPage *)pWizard->GetPage("CCaCertManPage");
	if(!pMan->IsDbReady())//���ݿ�����ʧ��
	{
		pWizard->GetDlgItem(IDC_B_MADE)->EnableWindow(FALSE);
	}

	if(!bFull ) //��Ϣ������
	{
		//�رհ�ť
		if(::IsWindow(pWizard->m_hWnd))
		{
			pWizard->GetDlgItem(IDC_B_V)->EnableWindow(FALSE);
			pWizard->GetDlgItem(IDC_B_MADE)->EnableWindow(FALSE);
		}
		m_WizardTree.SetNodeText(m_hCertRoot , MiniCT_0509);	////MiniCT_0509 "��֤�򵼱��棺�쳣"
		m_WizardTree.SetNodeColor(m_hCertRoot, RGB(255,0,0));
		
	}
	else
	{
		//���Ű�ť
		if(::IsWindow(pWizard->m_hWnd))
		{
			pWizard->GetDlgItem(IDC_B_V)->EnableWindow(true);
		}
		m_WizardTree.SetNodeText(m_hCertRoot , MiniCT_0510);		////MiniCT_0510 "��֤�򵼱��棺����"
		if(pMan->IsDbReady())
			pWizard->GetDlgItem(IDC_B_MADE)->EnableWindow(true);
//		m_WizardTree.SetNodeColor(m_hCertRoot, RGB(0,128,0));
	}
}

BOOL CCaCertReportPage::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}
