// CertExtPage.cpp : implementation file
//

#include "stdafx.h"
#include "MiniCA.h"
#include "CertExtPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "MiniCt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCertExtPage property page

IMPLEMENT_DYNCREATE(CCertExtPage, CPropertyPage)

CCertExtPage::CCertExtPage() : CPropertyPage(CCertExtPage::IDD)
{
	//{{AFX_DATA_INIT(CCertExtPage)
	m_TypeP12 = 0;
	m_TypeCert = 0;
	//}}AFX_DATA_INIT
	m_CertFormat = PEM;
}

CCertExtPage::~CCertExtPage()
{
}

void CCertExtPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCertExtPage)
//	DDX_Control(pDX, IDC_EDIT_PASSWORD4, m_XpEditPwd4);
//	DDX_Control(pDX, IDC_EDIT_PASSWORD3, m_XpEditPwd3);
//	DDX_Control(pDX, IDC_EDIT_PASSWORD2, m_XpEditPwd2);
//	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_XpEditPwd);
	DDX_Control(pDX, IDC_B_OK2, m_Bok2);
	DDX_Control(pDX, IDC_B_OK, m_Bok);
	DDX_Radio(pDX, IDC_R_PARSE, m_TypeP12);
	DDX_Radio(pDX, IDC_RADIO_DERTOPEM, m_TypeCert);
	//}}AFX_DATA_MAP
    DDX_FileEditCtrl(pDX, IDC_EDIT_PATH, m_XpEditPath, FEC_FOLDER);
    DDX_FileEditCtrl(pDX, IDC_EDIT_P12, m_XpEditP12, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT_KEY, m_XpEditKey, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT_INFILE, m_XpEditInfile, FEC_FILEOPEN);
    DDX_FileEditCtrl(pDX, IDC_EDIT_OUTFILE, m_XpEditOutfile, FEC_FILEOPEN);
}


BEGIN_MESSAGE_MAP(CCertExtPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCertExtPage)
	ON_BN_CLICKED(IDC_B_OK, OnBOk)
	ON_BN_CLICKED(IDC_R_PARSE, OnRParse)
	ON_BN_CLICKED(IDC_R_CREATE, OnRCreate)
	ON_BN_CLICKED(IDC_R_CHANGEP, OnRChangep)
	ON_BN_CLICKED(IDC_RADIO_PEMTODER, OnRadioPemtoder)
	ON_BN_CLICKED(IDC_RADIO_DERTOPEM, OnRadioDertopem)
	ON_BN_CLICKED(IDC_RADIO_ROOTCH, OnRadioRootch)
	ON_BN_CLICKED(IDC_RADIO_CRLCH, OnRadioCrlch)
	ON_BN_CLICKED(IDC_RADIO_TIEMCH, OnRadioTiemch)
	ON_BN_CLICKED(IDC_RADIO_KEYPAIRCH, OnRadioKeypairch)
	ON_BN_CLICKED(IDC_B_OK2, OnBOk2)
	ON_BN_CLICKED(IDC_R_PARSE2, OnRParse2)
	ON_BN_CLICKED(IDC_R_CREATE2, OnRCreate2)
	ON_BN_CLICKED(IDC_EXT_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCertExtPage message handlers
BOOL CCertExtPage::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
    UINT nID = pTTTStruct->idFrom;
	switch(nID)
	{
	case IDC_EDIT_P12://0-P12��ȡ 1-P12�ϲ� 2-P12������ 3-��ȡP7 4-�ϲ�P7
		{
			if(m_TypeP12 == 0 || m_TypeP12 == 2)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ѡ��PFX�ļ�"));//����
			}
			else if(m_TypeP12 == 1 || m_TypeP12 == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ѡ��Կ֤��"));//����
			}
			else if(m_TypeP12 == 3)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ѡ��P7B�ļ�"));//����
			}
			return TRUE;
		}
	case IDC_EDIT_KEY://ѡ��PFX,ѡ��Կ
		{
			if(m_TypeP12 == 1)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ѡ��˽Կ֤��"));//����
			}
			else if(m_TypeP12 == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ѡ��������ļ�"));//����
			}
			else
			{
				strcpy(pTTT->szText, "");//����
			}
			return TRUE;
		}
	case IDC_EDIT_PASSWORD://PFX������Կ
		{
			//������Ӧ����ʾ�ִ�
			if(m_TypeP12 == 0)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("���ڽ���PFX������Կ"));//����
			}
			else if(m_TypeP12 == 1)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("���ڽ���˽Կ����Կ"));//����
			}
			else
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("PFX��ԭ��Կ"));//����
			}
			return TRUE;
		}
	case IDC_EDIT_PASSWORD2://PFX������Կ
		{
			//������Ӧ����ʾ�ִ�
			if(m_TypeP12 == 0)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("���ڼ���˽Կ����Կ"));//����
			}
			else if(m_TypeP12 == 1)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("���ڼ���PFX������Կ"));//����
			}
			else
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("PFX������Կ"));//����
			}
			return TRUE;
		}
	case IDC_B_OK://PFX������Կ
		{
			CString str;
			GetDlgItemText(IDC_EDIT_PASSWORD2,str);//������Կ
			
			//������Ӧ����ʾ�ִ�
			if(m_TypeP12 == 0)
			{
				if(!str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("��ȡ��˽Կ֤��\r��ǰ˽Կ����"));//����
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("��ȡ��˽Կ֤��\r��ǰ˽Կδ����"));//����
			}
			else if(m_TypeP12 == 1)
			{
				if(!str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("�ϲ���˽Կ֤��\r����PFX��������"));//����
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("�ϲ���˽Կ֤��\r����PFX��"));//����
			}
			else if(m_TypeP12 == 2)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("�޸�PFX������Ϊָ������"));//����
			}
			else if(m_TypeP12 == 3)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("�ֽ�P7�ļ�\r�ֽ�Ϊ��Կ֤��ͺ������ļ�"));//����
			}
			else if(m_TypeP12 == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("�ϲ�P7�ļ�\r�ϲ���Կ֤��ͺ������ļ�"));//����
			}
			return TRUE;
		}
	case IDC_EDIT_INFILE://ѡ��
		{
			//������Ӧ����ʾ�ִ�
			if(m_TypeCert == 0 || m_TypeCert == 1 )
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ѡ��֤���ļ�"));//����
			}
			else
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ѡ��Կ֤��"));//����
			}
			return TRUE;
		}
	case IDC_EDIT_OUTFILE://PFX������Կ
		{
			//������Ӧ����ʾ�ִ�
			if(m_TypeCert == 0)
			{
				strcpy(pTTT->szText, "ת�����PEM��ʽ֤��洢��Ϣ");//����
			}
			else if(m_TypeCert == 1)
			{
				strcpy(pTTT->szText, "ת�����DER��ʽ֤��洢��Ϣ");//����
			}
			else if(m_TypeCert == 2)
			{
				strcpy(pTTT->szText, "ѡ���֤��");//����
			}
			else if(m_TypeCert == 3)
			{
				strcpy(pTTT->szText, "ѡ�������");//����
			}
			else if(m_TypeCert == 5)
			{
				strcpy(pTTT->szText, "ѡ��˽Կ�ļ�");//����
			}
			return TRUE;
		}
	case IDC_B_OK2://PFX������Կ
		{
			//������Ӧ����ʾ�ִ�
			if(m_TypeCert == 0)
			{
				CString str;
				GetDlgItemText(IDC_EDIT_PASSWORD4,str);//������Կ
				if(str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("ת��֤��ΪPEM��ʽ\r��ǰ˽Կδ����"));//����
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("ת��֤��ΪPEM��ʽ\r��ǰ˽Կ����"));//����
			}
			else if(m_TypeCert == 1)
			{
				CString str;
				GetDlgItemText(IDC_EDIT_PASSWORD4,str);//������Կ
				if(str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("ת��֤��ΪDER��ʽ\r��ǰ˽Կδ����"));//����
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("ת��֤��ΪDER��ʽ\r��ǰ˽Կ����"));//����
			}
			else if(m_TypeCert == 2)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("��֤����֤\r��֤֤���Ƿ��ɸ�֤��䷢"));//����
			}
			else if(m_TypeCert == 3)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("��������֤\r��֤֤���Ƿ��������б���"));//����
			}
			else if(m_TypeCert == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("ʱЧ����֤\r��֤֤��ʹ�������Ƿ���Ч��"));//����
			}
			else if(m_TypeCert == 5)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("��˽Կ����֤\r��֤��˽Կ֤����Կ���Ƿ�ƥ��"));//����
			}
			return TRUE;
		}
		
	}
	return(FALSE);
}

BOOL CCertExtPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_R_PARSE)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_R_CREATE)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_R_CHANGEP)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_R_PARSE2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_R_CREATE2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EXT_CHECK1)->m_hWnd,TRUE);
		
		ClassXP(GetDlgItem(IDC_RADIO_DERTOPEM)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO_ROOTCH)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO_TIEMCH)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO_PEMTODER)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO_KEYPAIRCH)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO_CRLCH)->m_hWnd,TRUE);

		ClassXP(GetDlgItem(IDC_EDIT_PASSWORD)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_PASSWORD2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_PASSWORD3)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_PASSWORD4)->m_hWnd,TRUE);
	}
	SetDlgItemText(IDC_EDIT_PASSWORD,  "MiniCA");
	SetDlgItemText(IDC_EDIT_PASSWORD2, "MiniCA");
	SetDlgItemText(IDC_EDIT_PASSWORD3, "MiniCA");
	SetDlgItemText(IDC_EDIT_PASSWORD4, "MiniCA");

	SetCheck();

	OnRadioDertopem();

	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
//	m_Bsout.SetIcon(IDI_ICON_SELECTDIR);
//	m_Bsout.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

//	m_Bsin.SetIcon(IDI_ICON_SELECTDIR);
//	m_Bsin.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

//	m_Bpub.SetIcon(IDI_ICON_SELECTDIR);
//	m_Bpub.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

//	m_Bpath.SetIcon(IDI_ICON_DIR);
//	m_Bpath.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bok2.SetIcon(IDI_ICON20);
	m_Bok2.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);


	m_Bok.SetIcon(IDI_ICON20);
	m_Bok.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);


//	m_Bkey.SetIcon(IDI_ICON_SELECTDIR);
//	m_Bkey.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);


    HANDLE handle = ::LoadImage(AfxGetInstanceHandle(),
                                MAKEINTRESOURCE(IDI_ICON_DIR),
                                IMAGE_ICON,
                                0,
                                0,
                                LR_DEFAULTCOLOR);

    m_XpEditPath.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEditPath.SetButtonWidth(18);

    m_XpEditP12.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEditP12.SetButtonWidth(18);
//	m_XpEditP12.ModifyStyleEx(0, WS_EX_ACCEPTFILES);


    m_XpEditKey.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEditKey.SetButtonWidth(18);

    m_XpEditInfile.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEditInfile.SetButtonWidth(18);

    m_XpEditOutfile.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEditOutfile.SetButtonWidth(18);
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);

		// TODO: Use one of the following forms to add controls:
//		m_toolTip.AddTool(GetDlgItem(IDC_B_PATH), CMiniCaApp::NormalCode("�ļ��洢·��"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PATH), "��������ļ��洢·��");
		m_toolTip.AddTool(GetDlgItem(IDC_R_PARSE), CMiniCaApp::NormalCode("�ֽ�PKCS#12��\r��PKCS#12������ȡ��˽Կ�ļ�"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_CREATE), CMiniCaApp::NormalCode("�ϲ�PKCS#12��\r����˽Կ�ļ��ϲ���PKCS#12��"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_CHANGEP), CMiniCaApp::NormalCode("�޸�PKCS#12������"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_DERTOPEM), CMiniCaApp::NormalCode("ת��֤���ʽΪPEM"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_PEMTODER), CMiniCaApp::NormalCode("ת��֤���ʽΪDER"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_ROOTCH), CMiniCaApp::NormalCode("����֤���Ƿ�Ϊ�������䷢"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_CRLCH), CMiniCaApp::NormalCode("����֤���Ƿ��ں�������"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_TIEMCH), CMiniCaApp::NormalCode("����֤���Ƿ�����Ч����"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_KEYPAIRCH), CMiniCaApp::NormalCode("���鹫˽Կ���Ƿ�ƥ��"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PASSWORD3), CMiniCaApp::NormalCode("������Կ\r����˽Կ�ļ�������"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PASSWORD4), CMiniCaApp::NormalCode("������Կ\r����˽Կ�ļ�������"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_PARSE2), CMiniCaApp::NormalCode("�ֽ�PKCS#7��\r��PKCS#7������ȡ��Կ�ͺ������ļ�"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_CREATE2), CMiniCaApp::NormalCode("�ϲ�PKCS#7��\r����Կ�ͺ������ļ��ϲ���PKCS#7��"));
		m_toolTip.AddTool(GetDlgItem(IDC_EXT_CHECK1), "������֤���ļ���ʽ");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_P12), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_KEY), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PASSWORD), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PASSWORD2), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_B_OK), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_STATIC_EXT10), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_B_OK2), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_STATIC_EXT11), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INFILE), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OUTFILE), LPSTR_TEXTCALLBACK);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*void CCertExtPage::OnBPath() //�������·�������·�������ļ��򸲸�
{
	// TODO: Add your control notification handler code here
	LPITEMIDLIST pidlRoot = NULL; 
	LPMALLOC pMalloc;   
	if(NOERROR == SHGetSpecialFolderLocation(m_hWnd,CSIDL_DRIVES ,&pidlRoot))
	{
		BROWSEINFO bi;   //���봫��Ĳ���,�����������ṹ�Ĳ����ĳ�ʼ�� 
		CString strDisplayName;// = CMiniCaApp::NormalCode("ѡ���ļ���");;   //�����õ�,��ѡ��Ļ�ҳ��·��,�൱���ṩһ�������� 
		bi.hwndOwner = GetSafeHwnd();   //�õ�������Handleֵ 
		bi.pidlRoot=0;   //���������������������õ���. 
		bi.pszDisplayName = strDisplayName.GetBuffer(MAX_PATH+1);   //�õ�������ָ��, 
		CString strTitle = CMiniCaApp::NormalCode("ѡ���ļ����·��,·�������е�ͬ���ļ���������!!");
		bi.lpszTitle = strTitle;  //���ñ��� 
		bi.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_DONTGOBELOWDOMAIN  ;   //���ñ�־ 
		bi.lpfn=NULL; 
		bi.lParam=0; 
		bi.iImage=0;   //���������һЩ�޹صĲ���������,�����������, 
		ITEMIDLIST * pidl;  
		pidl = SHBrowseForFolder(&bi);   //�򿪶Ի��� 
		if(!pidl)
			return;
		SHGetPathFromIDList(pidl,bi.pszDisplayName);	
		strDisplayName.ReleaseBuffer();   //�������GetBuffer()���Ӧ 
		m_Path = strDisplayName;
		if(m_Path.Right(1) != "\\")//���һ���ַ�����"/"
			m_Path += "\\";
		SetDlgItemText(IDC_EDIT_PATH,m_Path);	

		if(SUCCEEDED( SHGetMalloc ( &pMalloc ))) 
		{ 
			pMalloc->Free(pidlRoot); 
			pMalloc->Release(); 
		} 
	}
}
*/

/*void CCertExtPage::OnBPub() //ѡ��Կ��ѡ��p12
{
	// TODO: Add your control notification handler code here
	if(m_TypeP12==1)//ѡ��Կ
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("��Կ�ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"),NULL);
		CString strTitle = CMiniCaApp::NormalCode("ѡ��Կ֤���ļ�");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//������
		if(dlgOpen.DoModal()!=IDOK) return;
		m_cert = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_cert);
		
	}
	else if(m_TypeP12==0 || m_TypeP12==2)//ѡ��p12
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("������Ϣ����(*.pfx;*.p12)|*.pfx;*.p12|�����ļ�(*.*)|*.*||"),NULL);

		CString strTitle = CMiniCaApp::NormalCode("ѡ�������Ϣ�����ļ�");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//������
		if(dlgOpen.DoModal()!=IDOK) return;
		m_p12 = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_p12);
	}
	if(m_TypeP12==3)//ѡ��Կ
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
			CMiniCaApp::NormalCode("������Ϣ�﷨��׼(*.p7b)|*.p7b|�����ļ�(*.*)|*.*||"),NULL);
		CString strTitle = CMiniCaApp::NormalCode("������Ϣ�﷨��׼�ļ�");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//������
		if(dlgOpen.DoModal()!=IDOK) return;
		m_p7 = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_p7);
	}
	if(m_TypeP12==4)//ѡ��P7B
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
			CMiniCaApp::NormalCode("��Կ�ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"),NULL);
		CString strTitle = CMiniCaApp::NormalCode("ѡ��Կ֤���ļ�(�ɶ�ѡ)");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//������
		if(dlgOpen.DoModal()!=IDOK) return;
		m_cert = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_cert);
	}
}

void CCertExtPage::OnBKey() //ѡ��˽Կ
{
	// TODO: Add your control notification handler code here
	if(m_TypeP12!=4)//ѡ��P7B
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("˽Կ֤���ļ�(*.crt;*.cer;*.der;*.pem)|*.crt;*.cer;*.der;*.pem|�����ļ�(*.*)|*.*||"),NULL);
		
		CString strTitle = CMiniCaApp::NormalCode("ѡ��˽Կ֤���ļ�");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//������
		
		if(dlgOpen.DoModal()!=IDOK) return;
		m_key = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_KEY,m_key);
	}
	else
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("������(*.crl)|*.crl|�����ļ�(*.*)|*.*||"),NULL);
		
		CString strTitle = CMiniCaApp::NormalCode("ѡ��������ļ�");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//������
		if(dlgOpen.DoModal()!=IDOK) return;
		m_crl = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_KEY,m_crl);
	}
	
}*/

void CCertExtPage::OnBOk() //ִ�в���
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_EDIT_PATH, m_Path);
	if(m_TypeP12!=2&&m_Path=="") 
	{
		AddMsg(MiniCT_0112, M_WARING);			//MiniCT_0112 "��ѡ�����·��!"
		return;
	}
	
	if(m_TypeP12==0)//��ȡ֤�����
	{
		//��ȡp12�ļ�
		GetDlgItemText(IDC_EDIT_P12, m_p12);
		if(m_p12.IsEmpty())
		{
			AddMsg(MiniCT_0700,M_WARING);		//MiniCT_0700 "��ѡ��PFX�ļ�!"
			return;
		}
		CString pwd,pwd2;
		GetDlgItemText(IDC_EDIT_PASSWORD,pwd); //p12 ��Կ
		CString filein,certout,keyout;
		certout=m_Path+"\\UserCert.crt";
		keyout=m_Path+"\\UserPriKey.crt";	
		GetDlgItemText(IDC_EDIT_PASSWORD2,pwd2); //���� ˽Կ ,��Կ
		
		char out[100]={0};
		if(!ParsePfx(m_p12.GetBuffer(0),0,pwd.GetBuffer(0),certout.GetBuffer(0),
			keyout.GetBuffer(0),pwd2.GetBuffer(0),m_CertFormat,out))
			AddMsg(out,M_ERROR);
		
		else//ok
			AddMsg(MiniCT_0701);		//MiniCT_0701 "�ֽ�PFX�ļ��ɹ�"
	}
	
	else if(m_TypeP12==1)//�ϲ�֤�����
	{
		GetDlgItemText(IDC_EDIT_P12, m_cert);
		GetDlgItemText(IDC_EDIT_KEY, m_key);
		if(m_cert.IsEmpty() || m_key.IsEmpty())
		{
			AddMsg(MiniCT_0702,M_WARING);	//MiniCT_0702 "��ѡ��Կ����˽Կ�ļ�!"
			return;
		}
		CString fileout;
		fileout = m_Path+"\\Pkcs12.pfx";
		CString pwd,pwd2;
		GetDlgItemText(IDC_EDIT_PASSWORD2,pwd2);//����PFX��Կ
		GetDlgItemText(IDC_EDIT_PASSWORD,pwd);//����˽Կ��Կ

		char StreamP12[10240] = {0};
		UINT uP12Len = 10240;
		
		char out[100]={0};

		if(CreatePfx(StreamP12, uP12Len, pwd2.GetBuffer(0),
			   "MiniCA",/*IN �ü�����*/
			   m_cert.GetBuffer(0), 0,
			   m_key.GetBuffer(0), 0, pwd.GetBuffer(0),out))
		{
			CFile File;
			if(File.Open(fileout, CFile::modeCreate|CFile::modeWrite))	//���ļ�
			{
				File.Write(StreamP12, uP12Len);
				File.Close();
				AddMsg(MiniCT_0703);	//MiniCT_0703 "�ϲ�֤���ļ��ɹ�"
			}
			else
			{
				AddMsg(MiniCT_0704, M_ERROR);	//MiniCT_0704 "����P12�ļ�ʧ��"
			}

		}

		else
			AddMsg(out,M_ERROR);
	}
	else if(m_TypeP12 == 2)//�޸���Կ
	{
		GetDlgItemText(IDC_EDIT_P12, m_p12);
		if(m_p12.IsEmpty())
		{
			AddMsg(MiniCT_0700,M_WARING);	//MiniCT_0700 "��ѡ��PFX�ļ�!"
			return;
		}
		CString pwd,pwd2,fileOut;
		GetDlgItemText(IDC_EDIT_PASSWORD,pwd);
		GetDlgItemText(IDC_EDIT_PASSWORD2,pwd2);
		char out[100]={0};
		if(ChangePfxPwd(m_p12.GetBuffer(0),pwd.GetBuffer(0),pwd2.GetBuffer(0),
			m_p12.GetBuffer(0),out))
			AddMsg(MiniCT_0705);	//MiniCT_0705 "�޸�PFX����ɹ�"
		else
			AddMsg(out,M_ERROR);
	}
	else if(m_TypeP12 == 3)//��ȡP7
	{
		GetDlgItemText(IDC_EDIT_P12, m_p7);
		if(m_p7.IsEmpty())
		{
			AddMsg(MiniCT_0706,M_WARING);	//MiniCT_0706 "��ѡ��P7B�ļ�!"
			return;
		}
		CString certout,keyout;
		certout = m_Path + "\\P7Cert";
		keyout = m_Path + "\\P7Crl";	
		
		char out[100]={0};
		
		if(!ParseP7b(m_p7.GetBuffer(0),	0,	m_CertFormat, certout, keyout, NULL, out))
			AddMsg(out,M_ERROR);
		else//ok
			AddMsg(MiniCT_0707);		//MiniCT_0707 "�ֽ�P7�ļ��ɹ�" 
	}
	else if(m_TypeP12==4)//�ϲ�P7����
	{
		GetDlgItemText(IDC_EDIT_P12, m_cert);
		GetDlgItemText(IDC_EDIT_KEY, m_crl);
		if(m_cert.IsEmpty() && m_crl.IsEmpty())
		{
			AddMsg(MiniCT_0708,M_WARING);	//MiniCT_0708 "��ѡ��Կ����CRL�ļ�!"
			return;
		}

		CString fileout;
		fileout = m_Path + "\\P7.p7b";
		
		char out[100]={0};
		
		std::list<std::string> CertList;
		
		// call GetStartPosition() to get the position of the first file in the control
		POSITION pos = m_XpEditP12.GetStartPosition();
		while (pos)
		{
			// add the file paths to the list
			CString str = m_XpEditP12.GetNextPathName(pos);
			CertList.push_back(str.GetBuffer(0));
			str.ReleaseBuffer();
		}
			
		if(CreateP7b(&CertList, m_crl.GetBuffer(0),
			fileout.GetBuffer(0), m_CertFormat, out))
			AddMsg(MiniCT_0709);	//MiniCT_0709 "�ϲ�֤���ļ��ɹ�"
		else
			AddMsg(out,M_ERROR);
	}
	
}

void CCertExtPage::SetCheck()
{
//	SetDlgItemText(IDC_EDIT_P12,"");
//	SetDlgItemText(IDC_EDIT_KEY,"");
	GetDlgItem(IDC_EXT_CHECK1)->EnableWindow(0);

	if(m_TypeP12 == 4)
		m_XpEditP12.ModifyFlags(0, FEC_MULTIPLE);
	else
        m_XpEditP12.ModifyFlags(FEC_MULTIPLE, 0);

	if(m_TypeP12==0)	//�ֽ�P12
	{
		//0,3,4
		GetDlgItem(IDC_EXT_CHECK1)->EnableWindow(1);

		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);

		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0710);//MiniCT_0710 "ѡ��PFX:"
		m_XpEditP12.SetCaption(MiniCT_0711);//MiniCT_0711 "ѡ�������Ϣ�����ļ�"
		m_XpEditP12.SetFilter(MiniCT_0712);//MiniCT_0712 "������Ϣ����(*.pfx;*.p12)|*.pfx;*.p12|�����ļ�(*.*)|*.*||"
 
		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0713);//MiniCT_0713 "ѡ��˽Կ:"
		m_XpEditKey.SetCaption(MiniCT_0714);//MiniCT_0714 "ѡ��˽Կ֤���ļ�"
		m_XpEditKey.SetFilter(MiniCT_0715);//MiniCT_0715 "˽Կ֤���ļ�(*.crt;*.cer;*.der;*.pem)|*.crt;*.cer;*.der;*.pem|�����ļ�(*.*)|*.*||"


		SetDlgItemText(IDC_STATIC_DEC,MiniCT_0716);//MiniCT_0716 "����PFX����Կ��"
		SetDlgItemText(IDC_STATIC_ENC,MiniCT_0717);//MiniCT_0717 "����˽Կ��Կ��"
//		SetDlgItemText(IDC_EDIT_P12,m_p12);	//��ԭԭѡ��
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);	
		
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(0);	
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);

		SetDlgItemText(IDC_B_OK,MiniCT_0718);//MiniCT_0718 "��ȡ֤��"
	//	GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(0);
	}
	else if(m_TypeP12==1)	//�ϲ�P12
	{
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0719);//MiniCT_0719 "ѡ��Կ:"
		m_XpEditP12.SetCaption(MiniCT_0720);//MiniCT_0720 "ѡ��Կ֤���ļ�"
		m_XpEditP12.SetFilter(MiniCT_0721);//MiniCT_0721 "��Կ�ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_DEC,MiniCT_0722);//MiniCT_0722 "����˽Կ��Կ��"
		SetDlgItemText(IDC_STATIC_ENC,MiniCT_0723);//MiniCT_0723 "����PFX����Կ��"

		SetDlgItemText(IDC_STATIC_EXT9, MiniCT_0713);//MiniCT_0713
		m_XpEditKey.SetCaption(MiniCT_0714);//MiniCT_0714
		m_XpEditKey.SetFilter(MiniCT_0715);//MiniCT_0715
		SetDlgItemText(IDC_EDIT_P12, m_cert);	//��ԭԭѡ��
		SetDlgItemText(IDC_EDIT_KEY, m_key);	//��ԭԭѡ��
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);	
		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);	
		
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		SetDlgItemText(IDC_B_OK,MiniCT_0724);//MiniCT_0724 "�ϲ�֤��"
	}
	else if(m_TypeP12 == 2) //�޸�P12����
	{
		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0710);//MiniCT_0710 "ѡ��PFX:"
		m_XpEditP12.SetCaption(MiniCT_0711);//MiniCT_0711 "ѡ�������Ϣ�����ļ�"
		m_XpEditP12.SetFilter(MiniCT_0712);//MiniCT_0712 "������Ϣ����(*.pfx;*.p12)|*.pfx;*.p12|�����ļ�(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0713);//MiniCT_0713 "ѡ��˽Կ:"
		m_XpEditKey.SetCaption(MiniCT_0714);//MiniCT_0714 "ѡ��˽Կ֤���ļ�"
		m_XpEditKey.SetFilter(MiniCT_0715);//MiniCT_0715 "˽Կ֤���ļ�(*.crt;*.cer;*.der;*.pem)|*.crt;*.cer;*.der;*.pem|�����ļ�(*.*)|*.*||"
	
		SetDlgItemText(IDC_STATIC_DEC,MiniCT_0725);//MiniCT_0725 "ԭPFX����Կ��"
		SetDlgItemText(IDC_STATIC_ENC,MiniCT_0726);//MiniCT_0726 "��PFX����Կ��"
		SetDlgItemText(IDC_EDIT_P12, m_p12);	//��ԭԭѡ��
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);	
		
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(0);	
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);	
		SetDlgItemText(IDC_B_OK,MiniCT_0727);//MiniCT_0727 "�޸�����"
	}
	else if(m_TypeP12 == 3) //P7�ֽ�
	{
		//0,3
		GetDlgItem(IDC_EXT_CHECK1)->EnableWindow(1);
	
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		//������Ϣ�﷨��ע
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0728);//MiniCT_0728 "ѡ��P7B:"
		m_XpEditP12.SetCaption(MiniCT_0729);//MiniCT_0729 "ѡ�������Ϣ�﷨��׼�ļ�"
		m_XpEditP12.SetFilter(MiniCT_0730);//MiniCT_0730 "������Ϣ�﷨��׼(*.p7b)|*p7b|�����ļ�(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0731);//MiniCT_0731 "ѡ��CRL:"
		m_XpEditKey.SetCaption(MiniCT_0732);//MiniCT_0732 "ѡ��������ļ�"
		m_XpEditKey.SetFilter(MiniCT_0733);//MiniCT_0733 "������(*.crl)|*.crl|�����ļ�(*.*)|*.*||"
		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(0);
//		SetDlgItemText(IDC_EDIT_P12, m_cert);	//��ԭԭѡ��
//		SetDlgItemText(IDC_EDIT_KEY, m_key);	//��ԭԭѡ��
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(0);	
		
		SetDlgItemText(IDC_B_OK,MiniCT_0718);//MiniCT_0719
	}
	else//P7�ϲ�
	{
		GetDlgItem(IDC_EXT_CHECK1)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0719);//MiniCT_0719 "ѡ��Կ:"
		m_XpEditP12.SetCaption(MiniCT_0734);//MiniCT_0734 "ѡ��Կ֤���ļ�(�ɶ�ѡ)"
		m_XpEditP12.SetFilter(MiniCT_0721);//MiniCT_0721 "��Կ�ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0731);//MiniCT_0731 "ѡ��CRL:"
		m_XpEditKey.SetCaption(MiniCT_0732);//MiniCT_0732 "ѡ��������ļ�"
		m_XpEditKey.SetFilter(MiniCT_0733);//MiniCT_0733 "������(*.crl)|*.crl|�����ļ�(*.*)|*.*||"

		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(0);
		SetDlgItemText(IDC_EDIT_P12, m_cert);	//��ԭԭѡ��
		SetDlgItemText(IDC_EDIT_KEY, m_key);	//��ԭԭѡ��
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(1);	
		
		SetDlgItemText(IDC_B_OK,MiniCT_0724);//MiniCT_0724 "�ϲ�֤��"
	}


}

void CCertExtPage::OnRParse() //��ȡ֤�����
{
	// TODO: Add your control notification handler code here
	m_TypeP12=0;
	SetCheck();
}

void CCertExtPage::OnRCreate() //�ϲ�֤�����
{
	// TODO: Add your control notification handler code here
	m_TypeP12=1;
	SetCheck();
}

void CCertExtPage::OnRChangep() //�޸���Կ����
{
	// TODO: Add your control notification handler code here
	m_TypeP12=2;
	SetCheck();
}

void CCertExtPage::OnRParse2() //P7֤����ȡ
{
	// TODO: Add your control notification handler code here
	m_TypeP12=3;
	SetCheck();
}

void CCertExtPage::OnRCreate2() //�ϲ�֤�鵽P7
{
	// TODO: Add your control notification handler code here
	m_TypeP12 = 4;
	SetCheck();
}

void CCertExtPage::OnRadioDertopem() //TO->PEM
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 0;

	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0735);//MiniCT_0735 "ѡ��֤��:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0736);//MiniCT_0736 "���PEM:"

	m_XpEditInfile.SetCaption(MiniCT_0747);//MiniCT_0747 "ѡ��֤���ļ�"
	m_XpEditInfile.SetFilter(MiniCT_0748);//MiniCT_0748 "֤���ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"

	m_XpEditOutfile.SetCaption(MiniCT_0749);//MiniCT_0749 "ѡ��Ҫ�����֤���ļ�"
	m_XpEditOutfile.SetFilter(MiniCT_0748);//MiniCT_0748 "֤���ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"

	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1); 
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(1);
}

void CCertExtPage::OnRadioPemtoder() //->DER
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 1;
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0735);//MiniCT_0735 "ѡ��֤��:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0737);//MiniCT_0737 "���DER:"

	m_XpEditInfile.SetCaption(MiniCT_0747);//MiniCT_0747 "ѡ��֤���ļ�"
	m_XpEditInfile.SetFilter(MiniCT_0748);//MiniCT_0748 "֤���ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"

	m_XpEditOutfile.SetCaption(MiniCT_0749);//MiniCT_0749 "ѡ��Ҫ�����֤���ļ�"
	m_XpEditOutfile.SetFilter(MiniCT_0748);//MiniCT_0748 "֤���ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"


	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1);
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(1);
}

void CCertExtPage::OnRadioRootch() //ROOT CHECK
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 2;
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "ѡ��Կ:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0738);//MiniCT_0738 "ѡ��ROOT:"

	m_XpEditInfile.SetCaption(MiniCT_0720);//MiniCT_0720
	m_XpEditInfile.SetFilter(MiniCT_0721);//MiniCT_0721

	m_XpEditOutfile.SetCaption(MiniCT_0750);//MiniCT_0750 "ѡ���֤�鹫Կ�ļ�"
	m_XpEditOutfile.SetFilter(MiniCT_0751);//MiniCT_0751 "��֤���ļ�(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|�����ļ�(*.*)|*.*||"


	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1);
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(0);
}

void CCertExtPage::OnRadioCrlch() //CRL CHECK
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 3;
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "ѡ��Կ:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0731);//MiniCT_0731 "ѡ��CRL:"

	m_XpEditInfile.SetCaption(MiniCT_0720);//MiniCT_0720
	m_XpEditInfile.SetFilter(MiniCT_0721);//MiniCT_0721

	m_XpEditOutfile.SetCaption(MiniCT_0732);//MiniCT_0732
	m_XpEditOutfile.SetFilter(MiniCT_0733);//MiniCT_0733
	
	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1);
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(0);
}

void CCertExtPage::OnRadioTiemch() //TIME CHECK
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 4; 
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "ѡ��Կ:"

	m_XpEditInfile.SetCaption(MiniCT_0720);//MiniCT_0720
	m_XpEditInfile.SetFilter(MiniCT_0721);//MiniCT_0721

	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(0);
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(0);
}

void CCertExtPage::OnRadioKeypairch() //KEY PARI CHECK
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 5;	
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "ѡ��Կ:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0713);//MiniCT_0713 "ѡ��˽Կ:"

	m_XpEditInfile.SetCaption(MiniCT_0720);//MiniCT_0720
	m_XpEditInfile.SetFilter(MiniCT_0721);//MiniCT_0721

	m_XpEditOutfile.SetCaption(MiniCT_0714);//MiniCT_0714
	m_XpEditOutfile.SetFilter(MiniCT_0715);//MiniCT_0715


	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1);
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(0);
}

void CCertExtPage::OnBOk2() 
{
	// TODO: Add your control notification handler code here
	//��������ļ�������ļ�
	CString infile,outfile;
	char out[100]={0};
	GetDlgItemText(IDC_EDIT_INFILE,infile);
	GetDlgItemText(IDC_EDIT_OUTFILE,outfile);

	CString strPwdEnc,strPwdDec;//������Կ,�ͽ�����Կ
	GetDlgItemText(IDC_EDIT_PASSWORD3,strPwdDec);
	GetDlgItemText(IDC_EDIT_PASSWORD4,strPwdEnc);

	
	if(m_TypeCert == 0)//der -> pem
	{
		if(infile=="")
		{
			AddMsg(MiniCT_0739,M_WARING);//MiniCT_0739 "û��ѡ��֤���ļ�"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0740,M_WARING);//MiniCT_0740 "û��ָ��PEM�ļ�"
			return;
		}
		
		if(CertFormatConver(infile.GetBuffer(0),0,strPwdDec.GetBuffer(0),
			outfile.GetBuffer(0),0,strPwdEnc.GetBuffer(0),PEM,out))
			AddMsg(out);
		else
			AddMsg(out,M_ERROR);
		
	}
	else if(m_TypeCert == 1) //pem -> der
	{
		if(infile=="")
		{
			AddMsg(MiniCT_0739,M_WARING);//MiniCT_0739 "û��ѡ��֤���ļ�"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0741,M_WARING);//MiniCT_0741 "û��ָ��DER�ļ�"
			return;
		}
		
		if(CertFormatConver(infile.GetBuffer(0),0,strPwdDec.GetBuffer(0),
			outfile.GetBuffer(0),0,strPwdEnc.GetBuffer(0),DER,out))
			AddMsg(out);
		else
			AddMsg(out,M_ERROR);	
	}
	else if(m_TypeCert == 2)
	{
		if(infile=="")
		{
			AddMsg(MiniCT_0739,M_WARING);//MiniCT_0739 "û��ѡ��֤���ļ�"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0742,M_WARING);//MiniCT_0742 "û��ѡ���֤���ļ�"
			return;
		}
		if(CheckCertWithRoot(infile.GetBuffer(0),0,outfile.GetBuffer(0),0,out))
		{
			AddMsg(out);
		}
		else
		{
			AddMsg(out,M_ERROR);
		}
	}
	else if(m_TypeCert == 3)
	{
		if(infile=="")
		{
			AddMsg(MiniCT_0743,M_WARING);//MiniCT_0743 "û��ѡ��Կ�ļ�"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0744,M_WARING);//MiniCT_0744 "û��ѡ��������ļ�"
			return;
		}
		if(CheckCertWithCrl(infile.GetBuffer(0),0,outfile.GetBuffer(0),0,out))
		{
			AddMsg(out);
		}
		else
		{
			AddMsg(out,M_ERROR);
		}
	}
	else if(m_TypeCert == 4)
	{
		if(infile=="")
		{
			AddMsg(MiniCT_0743,M_WARING);//MiniCT_0743 "û��ѡ��Կ�ļ�"
			return;
		}
		if(CheckCertLife(infile.GetBuffer(0),0,out))
		{
			AddMsg(out);
		}
		else
		{
			AddMsg(out,M_ERROR);
		}
	}
	else if(m_TypeCert == 5)
	{
		if(infile=="")
		{
			AddMsg(MiniCT_0743,M_WARING);//MiniCT_0743 "û��ѡ��Կ�ļ�"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0745,M_WARING);//MiniCT_0745 "û��ѡ��˽Կ�ļ�"
			return;
		}
		BOOL bCheck = CertPairCheck(infile.GetBuffer(0),outfile.GetBuffer(0),out,strPwdDec.GetBuffer(0));

		infile.ReleaseBuffer();
		outfile.ReleaseBuffer();
		strPwdDec.ReleaseBuffer();

		if(bCheck)
			AddMsg(MiniCT_0746);//MiniCT_0746 "��˽Կ��ƥ��"
		else
			AddMsg(out,M_ERROR);		
	}
}

BOOL CCertExtPage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CCertExtPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)GetParent())->AddMsg("��֤����",info, type);	//MiniCT_0001

}

void CCertExtPage::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	int check=((CButton *)GetDlgItem(IDC_EXT_CHECK1))->GetCheck();
	if(check)
	{
		m_CertFormat = DER;
	}
	else
	{
		m_CertFormat = PEM;
	}
}

void CCertExtPage::TranslateCT()
{
	SetDlgItemText(IDC_STATIC_EXT1, MiniCT_11201);
	SetDlgItemText(IDC_STATIC_EXT2, MiniCT_11202);
	SetDlgItemText(IDC_STATIC_EXT3, MiniCT_11203);
	SetDlgItemText(IDC_STATIC_EXT4, MiniCT_11204);
	SetDlgItemText(IDC_STATIC_EXT5, MiniCT_11205);
	SetDlgItemText(IDC_STATIC_EXT6, MiniCT_11206);
	SetDlgItemText(IDC_STATIC_EXT7, MiniCT_11207);
	SetDlgItemText(IDC_STATIC_EXT8, MiniCT_11208);
	SetDlgItemText(IDC_STATIC_EXT9, MiniCT_11209);
	SetDlgItemText(IDC_STATIC_EXT10, MiniCT_11210);
	SetDlgItemText(IDC_STATIC_EXT11, MiniCT_11211);
	SetDlgItemText(IDC_STATIC_DEC, MiniCT_11212);
	SetDlgItemText(IDC_STATIC_ENC, MiniCT_11213);
	SetDlgItemText(IDC_STATIC_DEC2, MiniCT_11214);
	SetDlgItemText(IDC_STATIC_ENC2, MiniCT_11215);
	SetDlgItemText(IDC_R_PARSE, MiniCT_11216);
	SetDlgItemText(IDC_R_CREATE, MiniCT_11217);
	SetDlgItemText(IDC_R_CHANGEP, MiniCT_11218);
	SetDlgItemText(IDC_B_OK, MiniCT_11219);
	SetDlgItemText(IDC_RADIO_DERTOPEM, MiniCT_11220);
	SetDlgItemText(IDC_RADIO_PEMTODER, MiniCT_11221);
	SetDlgItemText(IDC_RADIO_CRLCH, MiniCT_11222);
	SetDlgItemText(IDC_RADIO_ROOTCH, MiniCT_11223);
	SetDlgItemText(IDC_RADIO_KEYPAIRCH, MiniCT_11224);
	SetDlgItemText(IDC_RADIO_TIEMCH, MiniCT_11225);
	SetDlgItemText(IDC_B_OK2, MiniCT_11226);
	SetDlgItemText(IDC_EXT_CHECK1, MiniCT_11227);
	SetDlgItemText(IDC_R_PARSE2, MiniCT_11228);
	SetDlgItemText(IDC_R_CREATE2, MiniCT_11229);
}
