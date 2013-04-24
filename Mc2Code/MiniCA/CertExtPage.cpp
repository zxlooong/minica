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
	case IDC_EDIT_P12://0-P12提取 1-P12合并 2-P12改密码 3-提取P7 4-合并P7
		{
			if(m_TypeP12 == 0 || m_TypeP12 == 2)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("选择PFX文件"));//设置
			}
			else if(m_TypeP12 == 1 || m_TypeP12 == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("选择公钥证书"));//设置
			}
			else if(m_TypeP12 == 3)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("选择P7B文件"));//设置
			}
			return TRUE;
		}
	case IDC_EDIT_KEY://选择PFX,选择公钥
		{
			if(m_TypeP12 == 1)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("选择私钥证书"));//设置
			}
			else if(m_TypeP12 == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("选择黑名单文件"));//设置
			}
			else
			{
				strcpy(pTTT->szText, "");//设置
			}
			return TRUE;
		}
	case IDC_EDIT_PASSWORD://PFX包的密钥
		{
			//设置相应的显示字串
			if(m_TypeP12 == 0)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("用于解密PFX包的密钥"));//设置
			}
			else if(m_TypeP12 == 1)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("用于解密私钥的密钥"));//设置
			}
			else
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("PFX包原密钥"));//设置
			}
			return TRUE;
		}
	case IDC_EDIT_PASSWORD2://PFX包的密钥
		{
			//设置相应的显示字串
			if(m_TypeP12 == 0)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("用于加密私钥的密钥"));//设置
			}
			else if(m_TypeP12 == 1)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("用于加密PFX包的密钥"));//设置
			}
			else
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("PFX包新密钥"));//设置
			}
			return TRUE;
		}
	case IDC_B_OK://PFX包的密钥
		{
			CString str;
			GetDlgItemText(IDC_EDIT_PASSWORD2,str);//加密密钥
			
			//设置相应的显示字串
			if(m_TypeP12 == 0)
			{
				if(!str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("提取公私钥证书\r当前私钥保护"));//设置
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("提取公私钥证书\r当前私钥未保护"));//设置
			}
			else if(m_TypeP12 == 1)
			{
				if(!str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("合并公私钥证书\r生成PFX包并加密"));//设置
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("合并公私钥证书\r生成PFX包"));//设置
			}
			else if(m_TypeP12 == 2)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("修改PFX包密码为指定密码"));//设置
			}
			else if(m_TypeP12 == 3)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("分解P7文件\r分解为公钥证书和黑名单文件"));//设置
			}
			else if(m_TypeP12 == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("合并P7文件\r合并公钥证书和黑名单文件"));//设置
			}
			return TRUE;
		}
	case IDC_EDIT_INFILE://选择
		{
			//设置相应的显示字串
			if(m_TypeCert == 0 || m_TypeCert == 1 )
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("选择证书文件"));//设置
			}
			else
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("选择公钥证书"));//设置
			}
			return TRUE;
		}
	case IDC_EDIT_OUTFILE://PFX包的密钥
		{
			//设置相应的显示字串
			if(m_TypeCert == 0)
			{
				strcpy(pTTT->szText, "转换后的PEM格式证书存储信息");//设置
			}
			else if(m_TypeCert == 1)
			{
				strcpy(pTTT->szText, "转换后的DER格式证书存储信息");//设置
			}
			else if(m_TypeCert == 2)
			{
				strcpy(pTTT->szText, "选择根证书");//设置
			}
			else if(m_TypeCert == 3)
			{
				strcpy(pTTT->szText, "选择黑名单");//设置
			}
			else if(m_TypeCert == 5)
			{
				strcpy(pTTT->szText, "选择私钥文件");//设置
			}
			return TRUE;
		}
	case IDC_B_OK2://PFX包的密钥
		{
			//设置相应的显示字串
			if(m_TypeCert == 0)
			{
				CString str;
				GetDlgItemText(IDC_EDIT_PASSWORD4,str);//加密密钥
				if(str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("转换证书为PEM格式\r当前私钥未保护"));//设置
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("转换证书为PEM格式\r当前私钥保护"));//设置
			}
			else if(m_TypeCert == 1)
			{
				CString str;
				GetDlgItemText(IDC_EDIT_PASSWORD4,str);//加密密钥
				if(str.IsEmpty())
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("转换证书为DER格式\r当前私钥未保护"));//设置
				else
					strcpy(pTTT->szText,CMiniCaApp::NormalCode("转换证书为DER格式\r当前私钥保护"));//设置
			}
			else if(m_TypeCert == 2)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("根证书验证\r验证证书是否由根证书颁发"));//设置
			}
			else if(m_TypeCert == 3)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("黑名单验证\r验证证书是否在作废列表中"));//设置
			}
			else if(m_TypeCert == 4)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("时效性验证\r验证证书使用期限是否有效期"));//设置
			}
			else if(m_TypeCert == 5)
			{
				strcpy(pTTT->szText,CMiniCaApp::NormalCode("公私钥对验证\r验证公私钥证书密钥对是否匹配"));//设置
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
//		m_toolTip.AddTool(GetDlgItem(IDC_B_PATH), CMiniCaApp::NormalCode("文件存储路径"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PATH), "操作结果文件存储路径");
		m_toolTip.AddTool(GetDlgItem(IDC_R_PARSE), CMiniCaApp::NormalCode("分解PKCS#12包\r从PKCS#12包中提取公私钥文件"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_CREATE), CMiniCaApp::NormalCode("合并PKCS#12包\r将公私钥文件合并成PKCS#12包"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_CHANGEP), CMiniCaApp::NormalCode("修改PKCS#12包密码"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_DERTOPEM), CMiniCaApp::NormalCode("转换证书格式为PEM"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_PEMTODER), CMiniCaApp::NormalCode("转换证书格式为DER"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_ROOTCH), CMiniCaApp::NormalCode("检验证书是否为根机构颁发"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_CRLCH), CMiniCaApp::NormalCode("检验证书是否在黑名单中"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_TIEMCH), CMiniCaApp::NormalCode("检验证书是否在有效期内"));
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_KEYPAIRCH), CMiniCaApp::NormalCode("检验公私钥对是否匹配"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PASSWORD3), CMiniCaApp::NormalCode("解密密钥\r解密私钥文件的密码"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PASSWORD4), CMiniCaApp::NormalCode("加密密钥\r加密私钥文件的密码"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_PARSE2), CMiniCaApp::NormalCode("分解PKCS#7包\r从PKCS#7包中提取公钥和黑名单文件"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_CREATE2), CMiniCaApp::NormalCode("合并PKCS#7包\r将公钥和黑名单文件合并成PKCS#7包"));
		m_toolTip.AddTool(GetDlgItem(IDC_EXT_CHECK1), "新生成证书文件格式");
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

/*void CCertExtPage::OnBPath() //设置输出路径，如果路径下有文件则覆盖
{
	// TODO: Add your control notification handler code here
	LPITEMIDLIST pidlRoot = NULL; 
	LPMALLOC pMalloc;   
	if(NOERROR == SHGetSpecialFolderLocation(m_hWnd,CSIDL_DRIVES ,&pidlRoot))
	{
		BROWSEINFO bi;   //必须传入的参数,下面就是这个结构的参数的初始化 
		CString strDisplayName;// = CMiniCaApp::NormalCode("选择文件夹");;   //用来得到,你选择的活页夹路径,相当于提供一个缓冲区 
		bi.hwndOwner = GetSafeHwnd();   //得到父窗口Handle值 
		bi.pidlRoot=0;   //这个变量就是我们在上面得到的. 
		bi.pszDisplayName = strDisplayName.GetBuffer(MAX_PATH+1);   //得到缓冲区指针, 
		CString strTitle = CMiniCaApp::NormalCode("选择文件输出路径,路径下已有的同名文件将被覆盖!!");
		bi.lpszTitle = strTitle;  //设置标题 
		bi.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_DONTGOBELOWDOMAIN  ;   //设置标志 
		bi.lpfn=NULL; 
		bi.lParam=0; 
		bi.iImage=0;   //上面这个是一些无关的参数的设置,最好设置起来, 
		ITEMIDLIST * pidl;  
		pidl = SHBrowseForFolder(&bi);   //打开对话框 
		if(!pidl)
			return;
		SHGetPathFromIDList(pidl,bi.pszDisplayName);	
		strDisplayName.ReleaseBuffer();   //和上面的GetBuffer()相对应 
		m_Path = strDisplayName;
		if(m_Path.Right(1) != "\\")//最后一个字符不是"/"
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

/*void CCertExtPage::OnBPub() //选择公钥，选择p12
{
	// TODO: Add your control notification handler code here
	if(m_TypeP12==1)//选择共钥
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("公钥文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"),NULL);
		CString strTitle = CMiniCaApp::NormalCode("选择公钥证书文件");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条
		if(dlgOpen.DoModal()!=IDOK) return;
		m_cert = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_cert);
		
	}
	else if(m_TypeP12==0 || m_TypeP12==2)//选择p12
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("个人信息交换(*.pfx;*.p12)|*.pfx;*.p12|所有文件(*.*)|*.*||"),NULL);

		CString strTitle = CMiniCaApp::NormalCode("选择个人信息交换文件");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条
		if(dlgOpen.DoModal()!=IDOK) return;
		m_p12 = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_p12);
	}
	if(m_TypeP12==3)//选择共钥
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
			CMiniCaApp::NormalCode("加密消息语法标准(*.p7b)|*.p7b|所有文件(*.*)|*.*||"),NULL);
		CString strTitle = CMiniCaApp::NormalCode("加密消息语法标准文件");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条
		if(dlgOpen.DoModal()!=IDOK) return;
		m_p7 = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_p7);
	}
	if(m_TypeP12==4)//选择P7B
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
			CMiniCaApp::NormalCode("公钥文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"),NULL);
		CString strTitle = CMiniCaApp::NormalCode("选择公钥证书文件(可多选)");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条
		if(dlgOpen.DoModal()!=IDOK) return;
		m_cert = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_P12,m_cert);
	}
}

void CCertExtPage::OnBKey() //选择私钥
{
	// TODO: Add your control notification handler code here
	if(m_TypeP12!=4)//选择P7B
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("私钥证书文件(*.crt;*.cer;*.der;*.pem)|*.crt;*.cer;*.der;*.pem|所有文件(*.*)|*.*||"),NULL);
		
		CString strTitle = CMiniCaApp::NormalCode("选择私钥证书文件");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条
		
		if(dlgOpen.DoModal()!=IDOK) return;
		m_key = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_KEY,m_key);
	}
	else
	{
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			CMiniCaApp::NormalCode("黑名单(*.crl)|*.crl|所有文件(*.*)|*.*||"),NULL);
		
		CString strTitle = CMiniCaApp::NormalCode("选择黑名单文件");
		dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条
		if(dlgOpen.DoModal()!=IDOK) return;
		m_crl = dlgOpen.GetPathName();
		SetDlgItemText(IDC_EDIT_KEY,m_crl);
	}
	
}*/

void CCertExtPage::OnBOk() //执行操作
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_EDIT_PATH, m_Path);
	if(m_TypeP12!=2&&m_Path=="") 
	{
		AddMsg(MiniCT_0112, M_WARING);			//MiniCT_0112 "请选择输出路径!"
		return;
	}
	
	if(m_TypeP12==0)//提取证书操作
	{
		//读取p12文件
		GetDlgItemText(IDC_EDIT_P12, m_p12);
		if(m_p12.IsEmpty())
		{
			AddMsg(MiniCT_0700,M_WARING);		//MiniCT_0700 "请选择PFX文件!"
			return;
		}
		CString pwd,pwd2;
		GetDlgItemText(IDC_EDIT_PASSWORD,pwd); //p12 密钥
		CString filein,certout,keyout;
		certout=m_Path+"\\UserCert.crt";
		keyout=m_Path+"\\UserPriKey.crt";	
		GetDlgItemText(IDC_EDIT_PASSWORD2,pwd2); //加密 私钥 ,密钥
		
		char out[100]={0};
		if(!ParsePfx(m_p12.GetBuffer(0),0,pwd.GetBuffer(0),certout.GetBuffer(0),
			keyout.GetBuffer(0),pwd2.GetBuffer(0),m_CertFormat,out))
			AddMsg(out,M_ERROR);
		
		else//ok
			AddMsg(MiniCT_0701);		//MiniCT_0701 "分解PFX文件成功"
	}
	
	else if(m_TypeP12==1)//合并证书操作
	{
		GetDlgItemText(IDC_EDIT_P12, m_cert);
		GetDlgItemText(IDC_EDIT_KEY, m_key);
		if(m_cert.IsEmpty() || m_key.IsEmpty())
		{
			AddMsg(MiniCT_0702,M_WARING);	//MiniCT_0702 "请选择公钥或者私钥文件!"
			return;
		}
		CString fileout;
		fileout = m_Path+"\\Pkcs12.pfx";
		CString pwd,pwd2;
		GetDlgItemText(IDC_EDIT_PASSWORD2,pwd2);//加密PFX密钥
		GetDlgItemText(IDC_EDIT_PASSWORD,pwd);//解密私钥密钥

		char StreamP12[10240] = {0};
		UINT uP12Len = 10240;
		
		char out[100]={0};

		if(CreatePfx(StreamP12, uP12Len, pwd2.GetBuffer(0),
			   "MiniCA",/*IN 好记名称*/
			   m_cert.GetBuffer(0), 0,
			   m_key.GetBuffer(0), 0, pwd.GetBuffer(0),out))
		{
			CFile File;
			if(File.Open(fileout, CFile::modeCreate|CFile::modeWrite))	//存文件
			{
				File.Write(StreamP12, uP12Len);
				File.Close();
				AddMsg(MiniCT_0703);	//MiniCT_0703 "合并证书文件成功"
			}
			else
			{
				AddMsg(MiniCT_0704, M_ERROR);	//MiniCT_0704 "保存P12文件失败"
			}

		}

		else
			AddMsg(out,M_ERROR);
	}
	else if(m_TypeP12 == 2)//修改密钥
	{
		GetDlgItemText(IDC_EDIT_P12, m_p12);
		if(m_p12.IsEmpty())
		{
			AddMsg(MiniCT_0700,M_WARING);	//MiniCT_0700 "请选择PFX文件!"
			return;
		}
		CString pwd,pwd2,fileOut;
		GetDlgItemText(IDC_EDIT_PASSWORD,pwd);
		GetDlgItemText(IDC_EDIT_PASSWORD2,pwd2);
		char out[100]={0};
		if(ChangePfxPwd(m_p12.GetBuffer(0),pwd.GetBuffer(0),pwd2.GetBuffer(0),
			m_p12.GetBuffer(0),out))
			AddMsg(MiniCT_0705);	//MiniCT_0705 "修改PFX密码成功"
		else
			AddMsg(out,M_ERROR);
	}
	else if(m_TypeP12 == 3)//提取P7
	{
		GetDlgItemText(IDC_EDIT_P12, m_p7);
		if(m_p7.IsEmpty())
		{
			AddMsg(MiniCT_0706,M_WARING);	//MiniCT_0706 "请选择P7B文件!"
			return;
		}
		CString certout,keyout;
		certout = m_Path + "\\P7Cert";
		keyout = m_Path + "\\P7Crl";	
		
		char out[100]={0};
		
		if(!ParseP7b(m_p7.GetBuffer(0),	0,	m_CertFormat, certout, keyout, NULL, out))
			AddMsg(out,M_ERROR);
		else//ok
			AddMsg(MiniCT_0707);		//MiniCT_0707 "分解P7文件成功" 
	}
	else if(m_TypeP12==4)//合并P7操作
	{
		GetDlgItemText(IDC_EDIT_P12, m_cert);
		GetDlgItemText(IDC_EDIT_KEY, m_crl);
		if(m_cert.IsEmpty() && m_crl.IsEmpty())
		{
			AddMsg(MiniCT_0708,M_WARING);	//MiniCT_0708 "请选择公钥或者CRL文件!"
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
			AddMsg(MiniCT_0709);	//MiniCT_0709 "合并证书文件成功"
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

	if(m_TypeP12==0)	//分解P12
	{
		//0,3,4
		GetDlgItem(IDC_EXT_CHECK1)->EnableWindow(1);

		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);

		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0710);//MiniCT_0710 "选择PFX:"
		m_XpEditP12.SetCaption(MiniCT_0711);//MiniCT_0711 "选择个人信息交换文件"
		m_XpEditP12.SetFilter(MiniCT_0712);//MiniCT_0712 "个人信息交换(*.pfx;*.p12)|*.pfx;*.p12|所有文件(*.*)|*.*||"
 
		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0713);//MiniCT_0713 "选择私钥:"
		m_XpEditKey.SetCaption(MiniCT_0714);//MiniCT_0714 "选择私钥证书文件"
		m_XpEditKey.SetFilter(MiniCT_0715);//MiniCT_0715 "私钥证书文件(*.crt;*.cer;*.der;*.pem)|*.crt;*.cer;*.der;*.pem|所有文件(*.*)|*.*||"


		SetDlgItemText(IDC_STATIC_DEC,MiniCT_0716);//MiniCT_0716 "解密PFX包密钥："
		SetDlgItemText(IDC_STATIC_ENC,MiniCT_0717);//MiniCT_0717 "加密私钥密钥："
//		SetDlgItemText(IDC_EDIT_P12,m_p12);	//复原原选择
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);	
		
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(0);	
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);

		SetDlgItemText(IDC_B_OK,MiniCT_0718);//MiniCT_0718 "提取证书"
	//	GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(0);
	}
	else if(m_TypeP12==1)	//合并P12
	{
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0719);//MiniCT_0719 "选择公钥:"
		m_XpEditP12.SetCaption(MiniCT_0720);//MiniCT_0720 "选择公钥证书文件"
		m_XpEditP12.SetFilter(MiniCT_0721);//MiniCT_0721 "公钥文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_DEC,MiniCT_0722);//MiniCT_0722 "解密私钥密钥："
		SetDlgItemText(IDC_STATIC_ENC,MiniCT_0723);//MiniCT_0723 "加密PFX包密钥："

		SetDlgItemText(IDC_STATIC_EXT9, MiniCT_0713);//MiniCT_0713
		m_XpEditKey.SetCaption(MiniCT_0714);//MiniCT_0714
		m_XpEditKey.SetFilter(MiniCT_0715);//MiniCT_0715
		SetDlgItemText(IDC_EDIT_P12, m_cert);	//复原原选择
		SetDlgItemText(IDC_EDIT_KEY, m_key);	//复原原选择
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
		SetDlgItemText(IDC_B_OK,MiniCT_0724);//MiniCT_0724 "合并证书"
	}
	else if(m_TypeP12 == 2) //修改P12密码
	{
		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0710);//MiniCT_0710 "选择PFX:"
		m_XpEditP12.SetCaption(MiniCT_0711);//MiniCT_0711 "选择个人信息交换文件"
		m_XpEditP12.SetFilter(MiniCT_0712);//MiniCT_0712 "个人信息交换(*.pfx;*.p12)|*.pfx;*.p12|所有文件(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0713);//MiniCT_0713 "选择私钥:"
		m_XpEditKey.SetCaption(MiniCT_0714);//MiniCT_0714 "选择私钥证书文件"
		m_XpEditKey.SetFilter(MiniCT_0715);//MiniCT_0715 "私钥证书文件(*.crt;*.cer;*.der;*.pem)|*.crt;*.cer;*.der;*.pem|所有文件(*.*)|*.*||"
	
		SetDlgItemText(IDC_STATIC_DEC,MiniCT_0725);//MiniCT_0725 "原PFX包密钥："
		SetDlgItemText(IDC_STATIC_ENC,MiniCT_0726);//MiniCT_0726 "新PFX包密钥："
		SetDlgItemText(IDC_EDIT_P12, m_p12);	//复原原选择
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(1);	
		
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(0);	
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(1);	
		SetDlgItemText(IDC_B_OK,MiniCT_0727);//MiniCT_0727 "修改密码"
	}
	else if(m_TypeP12 == 3) //P7分解
	{
		//0,3
		GetDlgItem(IDC_EXT_CHECK1)->EnableWindow(1);
	
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		//加密消息语法标注
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0728);//MiniCT_0728 "选择P7B:"
		m_XpEditP12.SetCaption(MiniCT_0729);//MiniCT_0729 "选择加密消息语法标准文件"
		m_XpEditP12.SetFilter(MiniCT_0730);//MiniCT_0730 "加密消息语法标准(*.p7b)|*p7b|所有文件(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0731);//MiniCT_0731 "选择CRL:"
		m_XpEditKey.SetCaption(MiniCT_0732);//MiniCT_0732 "选择黑名单文件"
		m_XpEditKey.SetFilter(MiniCT_0733);//MiniCT_0733 "黑名单(*.crl)|*.crl|所有文件(*.*)|*.*||"
		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(0);
//		SetDlgItemText(IDC_EDIT_P12, m_cert);	//复原原选择
//		SetDlgItemText(IDC_EDIT_KEY, m_key);	//复原原选择
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(0);	
		
		SetDlgItemText(IDC_B_OK,MiniCT_0718);//MiniCT_0719
	}
	else//P7合并
	{
		GetDlgItem(IDC_EXT_CHECK1)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_P12)->EnableWindow(1);
		SetDlgItemText(IDC_STATIC_EXT8,MiniCT_0719);//MiniCT_0719 "选择公钥:"
		m_XpEditP12.SetCaption(MiniCT_0734);//MiniCT_0734 "选择公钥证书文件(可多选)"
		m_XpEditP12.SetFilter(MiniCT_0721);//MiniCT_0721 "公钥文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"

		SetDlgItemText(IDC_STATIC_EXT9,MiniCT_0731);//MiniCT_0731 "选择CRL:"
		m_XpEditKey.SetCaption(MiniCT_0732);//MiniCT_0732 "选择黑名单文件"
		m_XpEditKey.SetFilter(MiniCT_0733);//MiniCT_0733 "黑名单(*.crl)|*.crl|所有文件(*.*)|*.*||"

		GetDlgItem(IDC_STATIC_DEC)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_ENC)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(0);
		SetDlgItemText(IDC_EDIT_P12, m_cert);	//复原原选择
		SetDlgItemText(IDC_EDIT_KEY, m_key);	//复原原选择
		GetDlgItem(IDC_STATIC_EXT8)->EnableWindow(1);	
		
		GetDlgItem(IDC_EDIT_KEY)->EnableWindow(1);
		GetDlgItem(IDC_STATIC_EXT9)->EnableWindow(1);	
		
		SetDlgItemText(IDC_B_OK,MiniCT_0724);//MiniCT_0724 "合并证书"
	}


}

void CCertExtPage::OnRParse() //提取证书操作
{
	// TODO: Add your control notification handler code here
	m_TypeP12=0;
	SetCheck();
}

void CCertExtPage::OnRCreate() //合并证书操作
{
	// TODO: Add your control notification handler code here
	m_TypeP12=1;
	SetCheck();
}

void CCertExtPage::OnRChangep() //修改密钥操作
{
	// TODO: Add your control notification handler code here
	m_TypeP12=2;
	SetCheck();
}

void CCertExtPage::OnRParse2() //P7证书提取
{
	// TODO: Add your control notification handler code here
	m_TypeP12=3;
	SetCheck();
}

void CCertExtPage::OnRCreate2() //合并证书到P7
{
	// TODO: Add your control notification handler code here
	m_TypeP12 = 4;
	SetCheck();
}

void CCertExtPage::OnRadioDertopem() //TO->PEM
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 0;

	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0735);//MiniCT_0735 "选择证书:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0736);//MiniCT_0736 "输出PEM:"

	m_XpEditInfile.SetCaption(MiniCT_0747);//MiniCT_0747 "选择证书文件"
	m_XpEditInfile.SetFilter(MiniCT_0748);//MiniCT_0748 "证书文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"

	m_XpEditOutfile.SetCaption(MiniCT_0749);//MiniCT_0749 "选择要保存的证书文件"
	m_XpEditOutfile.SetFilter(MiniCT_0748);//MiniCT_0748 "证书文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"

	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1); 
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(1);
}

void CCertExtPage::OnRadioPemtoder() //->DER
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 1;
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0735);//MiniCT_0735 "选择证书:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0737);//MiniCT_0737 "输出DER:"

	m_XpEditInfile.SetCaption(MiniCT_0747);//MiniCT_0747 "选择证书文件"
	m_XpEditInfile.SetFilter(MiniCT_0748);//MiniCT_0748 "证书文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"

	m_XpEditOutfile.SetCaption(MiniCT_0749);//MiniCT_0749 "选择要保存的证书文件"
	m_XpEditOutfile.SetFilter(MiniCT_0748);//MiniCT_0748 "证书文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"


	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1);
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(1);
}

void CCertExtPage::OnRadioRootch() //ROOT CHECK
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 2;
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "选择公钥:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0738);//MiniCT_0738 "选择ROOT:"

	m_XpEditInfile.SetCaption(MiniCT_0720);//MiniCT_0720
	m_XpEditInfile.SetFilter(MiniCT_0721);//MiniCT_0721

	m_XpEditOutfile.SetCaption(MiniCT_0750);//MiniCT_0750 "选择根证书公钥文件"
	m_XpEditOutfile.SetFilter(MiniCT_0751);//MiniCT_0751 "根证书文件(*.der;*.cer;*.crt;*.pem)|*.der;*.cer;*.crt;*.pem|所有文件(*.*)|*.*||"


	GetDlgItem(IDC_EDIT_OUTFILE)->EnableWindow(1);
	GetDlgItem(IDC_STATIC_EXT11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_PASSWORD3)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_PASSWORD4)->EnableWindow(0);
}

void CCertExtPage::OnRadioCrlch() //CRL CHECK
{
	// TODO: Add your control notification handler code here
	m_TypeCert = 3;
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "选择公钥:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0731);//MiniCT_0731 "选择CRL:"

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
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "选择公钥:"

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
	SetDlgItemText(IDC_STATIC_EXT10,MiniCT_0719);//MiniCT_0719 "选择公钥:"
	SetDlgItemText(IDC_STATIC_EXT11,MiniCT_0713);//MiniCT_0713 "选择私钥:"

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
	//获得输入文件和输出文件
	CString infile,outfile;
	char out[100]={0};
	GetDlgItemText(IDC_EDIT_INFILE,infile);
	GetDlgItemText(IDC_EDIT_OUTFILE,outfile);

	CString strPwdEnc,strPwdDec;//加密密钥,和解密密钥
	GetDlgItemText(IDC_EDIT_PASSWORD3,strPwdDec);
	GetDlgItemText(IDC_EDIT_PASSWORD4,strPwdEnc);

	
	if(m_TypeCert == 0)//der -> pem
	{
		if(infile=="")
		{
			AddMsg(MiniCT_0739,M_WARING);//MiniCT_0739 "没有选择证书文件"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0740,M_WARING);//MiniCT_0740 "没有指定PEM文件"
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
			AddMsg(MiniCT_0739,M_WARING);//MiniCT_0739 "没有选择证书文件"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0741,M_WARING);//MiniCT_0741 "没有指定DER文件"
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
			AddMsg(MiniCT_0739,M_WARING);//MiniCT_0739 "没有选择证书文件"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0742,M_WARING);//MiniCT_0742 "没有选择根证书文件"
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
			AddMsg(MiniCT_0743,M_WARING);//MiniCT_0743 "没有选择公钥文件"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0744,M_WARING);//MiniCT_0744 "没有选择黑名单文件"
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
			AddMsg(MiniCT_0743,M_WARING);//MiniCT_0743 "没有选择公钥文件"
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
			AddMsg(MiniCT_0743,M_WARING);//MiniCT_0743 "没有选择公钥文件"
			return;
		}
		if(outfile=="")
		{
			AddMsg(MiniCT_0745,M_WARING);//MiniCT_0745 "没有选择私钥文件"
			return;
		}
		BOOL bCheck = CertPairCheck(infile.GetBuffer(0),outfile.GetBuffer(0),out,strPwdDec.GetBuffer(0));

		infile.ReleaseBuffer();
		outfile.ReleaseBuffer();
		strPwdDec.ReleaseBuffer();

		if(bCheck)
			AddMsg(MiniCT_0746);//MiniCT_0746 "公私钥对匹配"
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
	((CMiniMainDlg *)GetParent())->AddMsg("数证辅助",info, type);	//MiniCT_0001

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
