// CaCertInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CaCertInfoPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#define _WIN32_WINNT  0x0400
#include "WinCrypt.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCertInfoPage property page

IMPLEMENT_DYNCREATE(CCaCertInfoPage, CPropertyPage)

CCaCertInfoPage::CCaCertInfoPage() : CPropertyPage(CCaCertInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CCaCertInfoPage)
	m_RadioDisk = 0;
	m_Day = 0;
	m_Pwd = _T("");
	//}}AFX_DATA_INIT
	m_IniPathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniCA.ini";
	m_CertFormat = DER;
}

CCaCertInfoPage::~CCaCertInfoPage()
{
	//清除链表内容
	m_ArrayInside.RemoveAll();
}

void CCaCertInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCertInfoPage)
//	DDX_Control(pDX, IDC_EDIT_CONT, m_XpEditCont);
	DDX_Control(pDX, IDC_LIST, m_CheckList);
//	DDX_Control(pDX, IDC_EDIT_PWD, m_XpEditReq);
//	DDX_Control(pDX, IDC_EDIT_PATH, m_XpEdit);
//	DDX_Control(pDX, IDC_EDIT_DAY, m_XpEditDay);
	DDX_Control(pDX, IDC_COMBO_CSP, m_CspBox);
	DDX_Radio(pDX, IDC_RADIO_DISK, m_RadioDisk);
	DDX_Text(pDX, IDC_EDIT_DAY, m_Day);
	DDV_MinMaxUInt(pDX, m_Day, 1, 3650);
	DDX_Text(pDX, IDC_EDIT_PWD, m_Pwd);
	DDV_MaxChars(pDX, m_Pwd, 8);
	DDX_Text(pDX, IDC_EDIT_PWD, m_Pwd2);
	DDV_MaxChars(pDX, m_Pwd2, 8);
	//}}AFX_DATA_MAP
    DDX_FileEditCtrl(pDX, IDC_EDIT_PATH, m_XpEdit, FEC_FOLDER);
	DDX_FileEditCtrl(pDX, IDC_EDIT_PATH2, m_XpEdit2, FEC_FILE);
}


BEGIN_MESSAGE_MAP(CCaCertInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCertInfoPage)
	ON_BN_CLICKED(IDC_R_CSP, OnRCsp)
	ON_BN_CLICKED(IDC_RADIO_DISK, OnRadioDisk)
	ON_BN_CLICKED(IDC_CAINFO_RADIODER, OnCainfoRadioder)
	ON_BN_CLICKED(IDC_CAINFO_RADIOPEM, OnCainfoRadiopem)
	ON_BN_CLICKED(IDC_STATIC_EVP2CSP, OnEVP2CSP)
	ON_BN_CLICKED(IDC_R_P11, OnP11)
	ON_BN_CLICKED(IDC_B_GUID, OnGuid)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCertInfoPage message handlers
BOOL CCaCertInfoPage::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
    UINT nID = pTTTStruct->idFrom;
	BOOL bRadioDisk = ((CButton * )GetDlgItem(IDC_RADIO_DISK))->GetCheck();
	BOOL bRadioCsp = ((CButton * )GetDlgItem(IDC_R_CSP))->GetCheck();
	BOOL bPkcs12 = ((CButton * )GetDlgItem(IDC_STATIC_CAINFO21))->GetCheck();
	BOOL bDer = ((CButton * )GetDlgItem(IDC_CAINFO_RADIODER))->GetCheck();
	CString strPwd(""),strText;
	GetDlgItemText(IDC_EDIT_PWD, strPwd);
	switch(nID)
	{
	case IDC_EDIT_PATH:
		if(bRadioDisk)
		{
			strText = CMiniCaApp::NormalCode("证书存储路径");
		}
		else
		{
			strText = "";
		}
		_tcscpy(pTTT->szText, strText);//设置
		return TRUE;
	case IDC_COMBO_CSP:
		if(bRadioCsp)
		{
			strText = CMiniCaApp::NormalCode("CSP名称");
		}
		else
		{
			strText = "";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_EDIT_CONT:
		if(bRadioCsp)
		{
			strText = CMiniCaApp::NormalCode("密钥容器名称");
		}
		else
		{
			strText = "";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_CAINFO_RADIODER:
	case IDC_CAINFO_RADIOPEM:
		if(bDer)
		{
			strText = "证书格式\r当前DER格式";
		}
		else
		{
			strText = "证书格式\r当前PEM格式";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_STATIC_CAINFO21:	//PKCS12
		if(bPkcs12)
		{
			strText = "是否生成PFX包\r当前生成";
		}
		else
		{
			strText = "是否生成PFX包\r当前不生成";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_EDIT_PWD:
		if(!strPwd.IsEmpty())
		{
			_tcscpy(pTTT->szText, "私钥和PFX包密码\r密码有效");//设置
		}
		else
		{
			_tcscpy(pTTT->szText, "私钥和PFX包密码\r密码无效");//设置
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CCaCertInfoPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	//默认选中PFX
	((CButton * )GetDlgItem(IDC_STATIC_CAINFO21))->SetCheck(1);
	((CButton * )GetDlgItem(IDC_CAINFO_RADIODER))->SetCheck(1);

	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_CAINFO_RADIODER)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CAINFO_RADIOPEM)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_STATIC_CAINFO21)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_COMBO_L)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO_DISK)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_R_CSP)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_R_P11)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_COMBO_CSP)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_CONT)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_PWD)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_PWD2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_DAY)->m_hWnd,TRUE);
	}

    HANDLE handle = ::LoadImage(AfxGetInstanceHandle(),
                                MAKEINTRESOURCE(IDI_ICON_DIR),
                                IMAGE_ICON,
                                0,
                                0,
                                LR_DEFAULTCOLOR);
    m_XpEdit.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEdit.SetButtonWidth(18);
    m_XpEdit2.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEdit2.SetButtonWidth(18);


	//CMiniCaApp::NormalCode(buf);

	CImageList * pImgList = ((CMiniCaApp *)AfxGetApp())->GetImgList();
	m_CheckList.SetImageList(pImgList,LVSIL_SMALL);//用来改变LISTCTRL行宽度

	ListView_SetExtendedListViewStyle(m_CheckList.m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_SUBITEMIMAGES |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect rect;
	m_CheckList.GetClientRect(rect);
	int width = rect.Width();

	m_CheckList.InsertColumn(0, MiniCT_0300, LVCFMT_LEFT, width/3);	//MiniCT_0300 "字段名称"
	m_CheckList.InsertColumn(1, MiniCT_0301, LVCFMT_LEFT, width*2/3 - 15);	//MiniCT_0301 "内容"

	GetIniInfo(m_IniPathName);

	char buf[256] = {0};

	GetPrivateProfileString("CA", "RSALEN", "2048", buf, 50, m_IniPathName);
	//查找加载项是否有效512 768 1024 1536 2048
	int index = ((CComboBox *)GetDlgItem(IDC_COMBO_L))->FindString(-1, buf);
	if(index == -1)
		index = 2;
	((CComboBox *)GetDlgItem(IDC_COMBO_L))->SetCurSel(index);


	GetPrivateProfileString("CA", "DAY", "365", buf, 50, m_IniPathName);
	SetDlgItemText(IDC_EDIT_DAY, buf);

	GetPrivateProfileString("CA", "SN", "1", buf, 50, m_IniPathName);
	SetDlgItemText(IDC_EDIT_SN, buf);

	GetPrivateProfileString("CA", "KEYPWD", "MiniCA", buf, 50, m_IniPathName);
	SetDlgItemText(IDC_EDIT_PWD, buf);

	GetPrivateProfileString("CA", "PIN", "MiniCA", buf, 50, m_IniPathName);
	SetDlgItemText(IDC_EDIT_PWD2, buf);

//	m_PathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\Cert\\";
//	SetDlgItemText(IDC_EDIT_PATH,m_PathName);

//	GetPrivateProfileString("PATH", "Cert","",m_PathName.GetBuffer(256),255, m_IniPathName);
//	m_PathName.ReleaseBuffer();//在 GetBuffer 和 ReleaseBuffer 之间这个范围，一定不能使用你要操作的这个缓冲的 CString 对象的任何方法

/*	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCertDbPage * pSetup = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));

	if(pSetup) 
	{
		CString m_PathName = pSetup->GetCertPath(2);
		SetDlgItemText(IDC_EDIT_PATH,m_PathName);
	}*/

	
	//BE98E302-2CAA-ED1A-93CF-A77559FEC4AC
	//SetDlgItemText(IDC_EDIT_CONT,"BE98E302-2CAA-ED1A-93CF-A77559FEC4AC");
	OnGuid();
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
		m_toolTip.AddTool(GetDlgItem(IDC_RADIO_DISK), CMiniCaApp::NormalCode("磁盘证书"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_DAY), CMiniCaApp::NormalCode("证书有效期限\r从申请时刻计起"));
		m_toolTip.AddTool(GetDlgItem(IDC_COMBO_L), CMiniCaApp::NormalCode("密钥长度\r生成高强度密钥可能要很长时间"));
		m_toolTip.AddTool(GetDlgItem(IDC_R_CSP), CMiniCaApp::NormalCode("CSP证书"));
		m_toolTip.AddTool(GetDlgItem(IDC_LIST), "证书基本信息列表\r详细设置请转到配置选项卡");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PATH), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_COMBO_CSP), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_CONT), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_CAINFO_RADIODER), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_CAINFO_RADIOPEM), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_STATIC_CAINFO21), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PWD), LPSTR_TEXTCALLBACK);
	}

	do {  //csp list refresh
		CString csp;
		GetDlgItemText(IDC_COMBO_CSP,csp);
		if(csp.IsEmpty())
		{
			m_CspBox.ResetContent();
			EnumCsp();
			CString str = GetDefaultCsp();
			int select = m_CspBox.FindString(-1,str);
			if(select!=-1)
				m_CspBox.SetCurSel(select);
		}
	} while (0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCaCertInfoPage::OnRadioDisk() //磁盘文件
{
	// TODO: Add your control notification handler code here
	m_CspBox.EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PATH)->EnableWindow();
	GetDlgItem(IDC_EDIT_PATH2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PWD2)->EnableWindow(FALSE);
//	GetDlgItem(IDC_STATIC1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CONT)->EnableWindow(FALSE);	
	
	if(((CButton * )GetDlgItem(IDC_STATIC_EVP2CSP))->GetCheck()){		
		CString csp;
		GetDlgItemText(IDC_COMBO_CSP,csp);
		if(csp.IsEmpty())
		{
			m_CspBox.ResetContent();
			EnumCsp();
			CString str = GetDefaultCsp();
			int select = m_CspBox.FindString(-1,str);
			if(select!=-1)
				m_CspBox.SetCurSel(select);
		}
			
		GetDlgItem(IDC_EDIT_CONT)->EnableWindow();
		((CListBox*)GetDlgItem(IDC_COMBO_CSP))->EnableWindow();
	}else{
		GetDlgItem(IDC_EDIT_CONT)->EnableWindow(FALSE);
		((CListBox*)GetDlgItem(IDC_COMBO_CSP))->EnableWindow(FALSE);
	}
}

void CCaCertInfoPage::OnRCsp() //枚举CSP
{
	// TODO: Add your control notification handler code here
	m_CspBox.EnableWindow();
	GetDlgItem(IDC_EDIT_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PATH2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PWD2)->EnableWindow(FALSE);
//	GetDlgItem(IDC_STATIC1)->EnableWindow();
	GetDlgItem(IDC_EDIT_CONT)->EnableWindow();

// 	CString csp;
// 	GetDlgItemText(IDC_COMBO_CSP,csp);
// 	if(csp.IsEmpty())
// 	{
		m_CspBox.ResetContent();
		EnumCsp();
		CString str = GetDefaultCsp();
		int select = m_CspBox.FindString(-1,str);
		if(select!=-1)
			m_CspBox.SetCurSel(select);
//	}
}

BOOL CCaCertInfoPage::EnumCsp()
{
	DWORD       cbName;
    DWORD       dwType;
    DWORD       dwIndex;
    CHAR        *pszName = NULL; 
	
    // Loop through enumerating providers.
    dwIndex = 0;
    while(CryptEnumProviders(
		dwIndex,
		NULL,
		0,
		&dwType,
		NULL,
		&cbName
		))
    {
		
        //--------------------------------------------------------------------
        //  cbName returns the length of the name of the next provider.
        //  Allocate memory in a buffer to retrieve that name.
		
        if (!(pszName = (LPTSTR)LocalAlloc(LMEM_ZEROINIT, cbName)))
        {
			return FALSE;
        }
        //--------------------------------------------------------------------
        //  Get the provider name.
        if (CryptEnumProviders(
			dwIndex++,
			NULL,
			0,
			&dwType,
			pszName,
			&cbName
			))
        {
			if(PROV_RSA_FULL == dwType)
				m_CspBox.AddString(pszName);
   //         printf ("     %4.0d\t%s\n",dwType, pszName);
        }
        LocalFree(pszName);
		
    } // End of while loop
	return FALSE;
}


CString CCaCertInfoPage::GetDefaultCsp()
{
	DWORD       cbProvName=0;
    LPTSTR      pbProvName=NULL;
	CString str;
    // Get the length of the RSA_FULL default provider name.
    if (!(CryptGetDefaultProvider(
		PROV_RSA_FULL, 
		NULL, 
		CRYPT_MACHINE_DEFAULT,
		NULL, 
		&cbProvName))) 
    { 
		return "";
    }
	
    // Allocate local memory for the name of the default provider.
    if (!(pbProvName = (LPTSTR)LocalAlloc(LMEM_ZEROINIT, cbProvName)))
    {
		return "";
    }
	
    // Get the default provider name.
    if (CryptGetDefaultProvider(
        PROV_RSA_FULL, 
        NULL, 
        CRYPT_MACHINE_DEFAULT,
        pbProvName,
        &cbProvName)) 
    {
		str = pbProvName;
    }
    else
    {
		str = "";
    }
	
    // Free resouces when done.
    LocalFree(pbProvName);
	return str;
}

BOOL CCaCertInfoPage::GetCertInfo(CStaticTreeCtrl * p_WizardTree,
										 HTREENODE & hInfoBase, 
										 HTREENODE &  hInfoHold, 
										 HTREENODE &  hInfoSave)
{
	BOOL bFull = FALSE;//信息是否完整,是否可以制作数证
	CString info,name,matter,temp,pkcs,error,format;
	CString sInfoBase;
	HTREENODE hTemp;
	COLORREF colError = RGB(255,0,0);
	//数证基本信息

	CString str;

	int nCount = m_CheckList.GetItemCount();
	
	DWORD dData = 0;
	
	for(int i = nCount -1; i >= 0; i--)
	{
		//检查状态
		if(ListView_GetCheckState(m_CheckList.m_hWnd,i))//此项选中
		{
			name = m_CheckList.GetItemText(i,0);
			matter = m_CheckList.GetItemText(i,1);
			
			if(name.IsEmpty() || matter.IsEmpty())
				continue;
			sInfoBase.Format("%s: %s",name, matter);
			hTemp = p_WizardTree->InsertChild( hInfoBase, sInfoBase);
			
	/*		if(name == "CN" || name == "commonName" && matter.IsEmpty())
			{
				p_WizardTree->SetNodeColor(hTemp, colError);
			}*/
			bFull = TRUE;
		}
	}

	if(!bFull)
	{
		p_WizardTree->SetNodeColor(hInfoBase, colError);
	}
	//数证约束信息
	//密钥长度,
	//证书有效期
	//私鈅保护
	GetDlgItemText(IDC_EDIT_PWD, matter);
	name = MiniCT_0302;								//MiniCT_0302 "私钥保护:"
	CString strTemp;
	if(matter.IsEmpty())
	{
		strTemp = MiniCT_0303;							//MiniCT_0303 "未保护"
		temp.Format("%s%s",name, strTemp);
	}
	else
	{
		strTemp = MiniCT_0304;							//MiniCT_0304 "保护"
		temp.Format("%s%s",name, strTemp);
	}
	p_WizardTree->InsertChild( hInfoHold, CMiniCaApp::NormalCode(temp));

	GetDlgItemText(IDC_STATIC_CAINFO15, name);
	GetDlgItemText(IDC_EDIT_DAY, matter); //   10 - 
	sInfoBase.Format("%s %s",name,matter);

	hTemp = p_WizardTree->InsertChild( hInfoHold, sInfoBase);
	if(matter.IsEmpty())
	{
		p_WizardTree->SetNodeColor(hTemp, colError);
		bFull = FALSE;
	}

	GetDlgItemText(IDC_STATIC_CAINFO16, name);
	GetDlgItemText(IDC_COMBO_L, matter); //   10 - 
	sInfoBase.Format("%s %s",name,matter);
	hTemp = p_WizardTree->InsertChild( hInfoHold, sInfoBase);
	if(matter.IsEmpty())
	{
		p_WizardTree->SetNodeColor(hTemp, colError);
		bFull = FALSE;
	}




	
	
	//约束信息

	//数证格式
	if(((CButton*)GetDlgItem(IDC_CAINFO_RADIODER))->GetCheck())//选中,DER格式
	{
		format.Format("%s: DER", MiniCT_0305);						//MiniCT_0305 证书格式
		m_CertFormat = DER;
	}
	else 
	{
		format.Format("%s: PEM", MiniCT_0305);						//MiniCT_0305
		m_CertFormat = PEM;
	}

	p_WizardTree->InsertChild( hInfoHold, CMiniCaApp::NormalCode(format));

	if(((CButton*)GetDlgItem(IDC_STATIC_CAINFO21))->GetCheck())//选中
	{
		pkcs.Format("Pkcs#12%s", MiniCT_0306);							//MiniCT_0306 "包: 附带"
	}
	else
	{
		pkcs.Format("Pkcs#12%s", MiniCT_0307);							//MiniCT_0307 "包: 不附带"
	}
	
	p_WizardTree->InsertChild( hInfoHold, CMiniCaApp::NormalCode(pkcs));

	//存储信息
	if(((CButton*)GetDlgItem(IDC_RADIO_DISK))->GetCheck())//磁盘文件
	{
		CString str;
		GetDlgItemText(IDC_EDIT_PATH,temp);
		//尝试建立目录
		//测试路径是否存在

		BOOL bDir = FALSE;

//		if(_mkdir(temp) == -1 && errno != EEXIST)
		temp += "\\";
		if(!CMiniCaApp::MakeSureDirectoryPathExists(temp))
		{
			str.Format("%s %s", MiniCT_0308, temp);	 //MiniCT_0308 存储路径无效
			bFull = FALSE;
		}
		else
		{
			str.Format("%s %s",MiniCT_0309, temp);	//MiniCT_0309 存储路径: 
			bDir = TRUE;
		}
		str.Replace("\\", "\\ ");
		HTREENODE hTemp = p_WizardTree->InsertChild( hInfoSave, CMiniCaApp::NormalCode(str));
		if(!bDir)
			p_WizardTree->SetNodeColor(hTemp, RGB(255,0,0));
	}
	else if(((CButton*)GetDlgItem(IDC_R_CSP))->GetCheck())//CSP
	{
		BOOL bDir = FALSE;

		CString str;
		GetDlgItemText(IDC_COMBO_CSP,str);
		temp.Format("%s %s", MiniCT_0310, str);			//MiniCT_0310 CSP名称:
		p_WizardTree->InsertChild( hInfoSave, CMiniCaApp::NormalCode(temp));

		GetDlgItemText(IDC_EDIT_CONT,temp);
		if(temp.IsEmpty())
		{
			str.Format(MiniCT_0311);		////MiniCT_0311 "密钥容器无效"
			bFull = FALSE;
			bDir = FALSE;
		}
		else
		{
			str.Format("%s %s\n", MiniCT_0312, temp);		//MiniCT_0312 密钥容器:
			bDir = TRUE;
		}
		HTREENODE hTemp = p_WizardTree->InsertChild( hInfoSave, CMiniCaApp::NormalCode(str));
		if(!bDir)
			p_WizardTree->SetNodeColor(hTemp, RGB(255,0,0));

	}
	return bFull;
}

void CCaCertInfoPage::GetCert(stuSUBJECT *& pCERT, stuCERTINFO & certInfo)
{
	CString str;

	int nCount = m_CheckList.GetItemCount();
	CString strName;
	DWORD dData = 0;
	for(int i = 0;i<nCount;i++)
	{
		//检查状态
		if(ListView_GetCheckState(m_CheckList.m_hWnd,i))//此项选中
		{
			strName = m_CheckList.GetItemText(i,1);
			pCERT->Add(pCERT,m_ArrayInside[i], strName);
		}
	}

	certInfo.uCertLen = GetDlgItemInt(IDC_COMBO_L);
	certInfo.uCertDay = GetDlgItemInt(IDC_EDIT_DAY);
	certInfo.uCertFormat = m_CertFormat;
	certInfo.bCertP12 = ((CButton*)GetDlgItem(IDC_STATIC_CAINFO21))->GetCheck();

	GetDlgItemText(IDC_EDIT_PWD,str);
	strncpy(certInfo.cCertPwd,str.GetBuffer(0),sizeof(certInfo.cCertPwd));
	str.ReleaseBuffer();

	if(((CButton*)GetDlgItem(IDC_RADIO_DISK))->GetCheck())//选中磁盘
	{
		GetDlgItemText(IDC_EDIT_PATH,str);
		strncpy(certInfo.cCertDir,str.GetBuffer(0),sizeof(certInfo.cCertDir));
		str.ReleaseBuffer();
	}

	do{ // csp, containername
		GetDlgItemText(IDC_COMBO_CSP,str);
		strncpy(certInfo.cCertCsp,str.GetBuffer(0),sizeof(certInfo.cCertCsp));
		str.ReleaseBuffer();
		
		GetDlgItemText(IDC_EDIT_CONT,str);
		strncpy(certInfo.cCertCon,str.GetBuffer(0),sizeof(certInfo.cCertCon));
		str.ReleaseBuffer();
	}while(0);

	do{ // p11 path ; pin
		GetDlgItemText(IDC_EDIT_PATH2,str);
		strncpy(certInfo.cP11dir, str.GetBuffer(0), sizeof(certInfo.cP11dir));
		str.ReleaseBuffer();
		
		GetDlgItemText(IDC_EDIT_PWD2,str);
		strncpy(certInfo.cCertPwd2, str.GetBuffer(0), sizeof(certInfo.cCertPwd2));
		str.ReleaseBuffer();
	} while (0);


	do {
		if(((CButton * )GetDlgItem(IDC_RADIO_DISK))->GetCheck()){
			if(((CButton * )GetDlgItem(IDC_STATIC_EVP2CSP))->GetCheck())
				certInfo.uCertGenType = 1;
			else
				certInfo.uCertGenType = 0;
		}

		if(((CButton * )GetDlgItem(IDC_R_CSP))->GetCheck())
			certInfo.uCertGenType = 2;

		if(((CButton * )GetDlgItem(IDC_R_P11))->GetCheck())
			certInfo.uCertGenType = 3;
	} while (0);
}


BOOL CCaCertInfoPage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CCaCertInfoPage::SetNamePath(const CString &strPath)
{
	if(!strPath.IsEmpty())
	{
		SetDlgItemText(IDC_EDIT_PATH,strPath);
		//更新报告信息
		CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
		CCaCertWizardSheet * pWizard = (CCaCertWizardSheet *)(pMain->GetPage("CCaCertWizardSheet"));
		if(pWizard)
		{
			CCaCertReportPage * pReportPage = (CCaCertReportPage *)(pWizard->GetPage("CCaCertReportPage"));
			if(pReportPage)
				pReportPage->ViewWizardInfo();
		}
	}
}

void CCaCertInfoPage::SetLVCheck (WPARAM ItemIndex, BOOL bCheck)
{
	ListView_SetItemState(m_CheckList.m_hWnd, ItemIndex, 
		UINT((int(bCheck) + 1) << 12), LVIS_STATEIMAGEMASK);
}

void CCaCertInfoPage::GetIniInfo(CString strIniPath)
{
	//删除原来内容
	m_CheckList.DeleteAllItems();

	//清除链表内容
	m_ArrayInside.RemoveAll();

	char buf[255] = {0};
	char info[255] = {0};
	char inside[255] = {0};

	GetPrivateProfileString("TOTAL", "ToTal","0",buf,50, m_IniPathName);
	CString strIndex;
	int uMax = atoi(buf);
	for(int i = 0, j = 0; i<uMax; i++)
	{
		strIndex.Format("INFO%d",i+1);
		//首先读取是否显示标志
		GetPrivateProfileString(strIndex, "IsDisp","0",buf,50, m_IniPathName);
		if(atoi(buf) <= 0)
			continue;

		GetPrivateProfileString(strIndex, "InsideName","0",inside,254, m_IniPathName);//内部名称
		if(strlen(inside) == 0)
			continue;
		else
			m_ArrayInside.Add(inside);

		GetPrivateProfileString(strIndex, "DispName","0",buf,254, m_IniPathName);
		if(strlen(buf) == 0)
			continue;

		GetPrivateProfileString(strIndex, "Info","0",info,254, m_IniPathName);
		if(strlen(info) == 0)
			continue;

		m_CheckList.InsertItem(j, buf, 7);
		m_CheckList.SetItemText(j, 1, info);

		GetPrivateProfileString(strIndex, "IsCheck","78",buf,50, m_IniPathName);//是否选中
		SetLVCheck (j, atoi(buf));
		j++;
	}

}

void CCaCertInfoPage::TranslateCT()
{
	CHeaderCtrl * pHeader = m_CheckList.GetHeaderCtrl();
	if (pHeader)
	{
		CString str = MiniCT_0300;
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(0, &hdi);
		
		str = MiniCT_0301;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(1, &hdi);
	}
	//翻译STATIC
	SetDlgItemText(IDC_STATIC_CAINFO1,	MiniCT_10301); 
	SetDlgItemText(IDC_STATIC_CAINFO12,	MiniCT_10302); 
	SetDlgItemText(IDC_STATIC_CAINFO13,	MiniCT_10303); 
	SetDlgItemText(IDC_STATIC_CAINFO14,	MiniCT_10304); 
	SetDlgItemText(IDC_STATIC_CAINFO15,	MiniCT_10305); 
	SetDlgItemText(IDC_STATIC_CAINFO16,	MiniCT_10306); 
	SetDlgItemText(IDC_STATIC_CAINFO17,	MiniCT_10307); 
	SetDlgItemText(IDC_STATIC_CAINFO18,	MiniCT_10308); 
	SetDlgItemText(IDC_STATIC_CAINFO19,	MiniCT_10309); 
	SetDlgItemText(IDC_STATIC_CAINFO20,	MiniCT_10310); 
	SetDlgItemText(IDC_STATIC_CAINFO21,	MiniCT_10311); 
	SetDlgItemText(IDC_STATIC_CAINFO22,	MiniCT_10312); 
	SetDlgItemText(IDC_STATIC_CAINFO23,	MiniCT_10313); 
	SetDlgItemText(IDC_CAINFO_RADIODER,	MiniCT_10314); 
	SetDlgItemText(IDC_CAINFO_RADIOPEM,	MiniCT_10315); 
	SetDlgItemText(IDC_RADIO_DISK,		MiniCT_10316); 
	SetDlgItemText(IDC_R_CSP,			MiniCT_10317); 
}

void CCaCertInfoPage::OnCainfoRadioder() 
{
	// TODO: Add your control notification handler code here
	m_CertFormat = DER;
}

void CCaCertInfoPage::OnCainfoRadiopem() 
{
	// TODO: Add your control notification handler code here
	m_CertFormat = PEM;
}

void CCaCertInfoPage::OnEVP2CSP()
{
	if(((CButton * )GetDlgItem(IDC_RADIO_DISK))->GetCheck()){
		if(((CButton * )GetDlgItem(IDC_STATIC_EVP2CSP))->GetCheck()){		
			
			CString csp;
			GetDlgItemText(IDC_COMBO_CSP,csp);
			if(csp.IsEmpty())
			{
				m_CspBox.ResetContent();
				EnumCsp();
				CString str = GetDefaultCsp();
				int select = m_CspBox.FindString(-1,str);
				if(select!=-1)
					m_CspBox.SetCurSel(select);
			}

			GetDlgItem(IDC_EDIT_CONT)->EnableWindow();
			((CListBox*)GetDlgItem(IDC_COMBO_CSP))->EnableWindow();
		}else{
			GetDlgItem(IDC_EDIT_CONT)->EnableWindow(FALSE);
			((CListBox*)GetDlgItem(IDC_COMBO_CSP))->EnableWindow(FALSE);
		}
	}	
}

void CCaCertInfoPage::OnP11()
{
	m_CspBox.EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CONT)->EnableWindow(FALSE);	
	GetDlgItem(IDC_EDIT_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PATH2)->EnableWindow();
	GetDlgItem(IDC_EDIT_PWD2)->EnableWindow(TRUE);	
}

#pragma comment(lib, "Rpcrt4.lib")
void CCaCertInfoPage::OnGuid()
{
	CString szname;
	UUID uuidName;
	
#if defined(UNICODE) || defined(_UNICODE)
	unsigned short* UNamePtr = NULL;
#else
	unsigned char* UNamePtr = NULL;	
#endif
    
	UuidCreate(&uuidName);
	UuidToString(&uuidName,&UNamePtr);
	
	szname = (TCHAR*)UNamePtr;
	RpcStringFree(&UNamePtr);
	szname.MakeUpper();
	
	SetDlgItemText(IDC_EDIT_CONT, (LPCTSTR)(LPCTSTR)szname);
}
