// CaCenterPage.cpp : implementation file
//

#include "stdafx.h"
#include "MiniCA.h"
#include "CaCenterPage.h"
#include "CaCertIniSetPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "MiniCT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCenterPage property page

IMPLEMENT_DYNCREATE(CCaCenterPage, CPropertyPage)

CCaCenterPage::CCaCenterPage() : CPropertyPage(CCaCenterPage::IDD)
{
	//{{AFX_DATA_INIT(CCaCenterPage)
	m_C = _T("");
	m_ST = _T("");
	m_L = _T("");
	m_O = _T("");
	m_OU = _T("");
	m_CN = _T("");
	m_E = _T("");
	m_S = _T("");
	m_G = _T("");
	m_T = _T("");
	//}}AFX_DATA_INIT
	m_CertFormat = PEM;
	m_ExtName = ".cer";
	m_MadePfx = FALSE;
	m_IniPathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniCA.ini";
	m_NameType = 0;
}

CCaCenterPage::~CCaCenterPage()
{
}

void CCaCenterPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCenterPage)
	DDX_Control(pDX, IDC_STATIC_CN, m_StaticCN);
	DDX_Control(pDX, IDC_B_CRL2, m_Bcrl2);
	DDX_Control(pDX, IDC_B_MKREQ, m_Bmkreq);
	DDX_Control(pDX, IDC_B_MKCERT2, m_Bmkcert2);
	DDX_Control(pDX, IDC_B_MKCERT, m_Bmkcert);
	DDX_Control(pDX, IDC_B_CRL, m_Bcrl);
	DDX_Control(pDX, IDC_B_MKROOT, m_Bmkroot);
	DDX_Text(pDX, IDC_EDIT_C, m_C);
	DDV_MaxChars(pDX, m_C, 8);
	DDX_Text(pDX, IDC_EDIT_ST, m_ST);
	DDV_MaxChars(pDX, m_ST, 8);
	DDX_Text(pDX, IDC_EDIT_L, m_L);
	DDV_MaxChars(pDX, m_L, 16);
	DDX_Text(pDX, IDC_EDIT_O, m_O);
	DDV_MaxChars(pDX, m_O, 48);
	DDX_Text(pDX, IDC_EDIT_OU, m_OU);
	DDV_MaxChars(pDX, m_OU, 48);
	DDX_Text(pDX, IDC_EDIT_CN, m_CN);
	DDV_MaxChars(pDX, m_CN, 48);
	DDX_Text(pDX, IDC_EDIT_E, m_E);
	DDV_MaxChars(pDX, m_E, 48);
	DDX_Text(pDX, IDC_EDIT_S, m_S);
	DDV_MaxChars(pDX, m_S, 16);
	DDX_Text(pDX, IDC_EDIT_G, m_G);
	DDV_MaxChars(pDX, m_G, 16);
	DDX_Text(pDX, IDC_EDIT_T, m_T);
	DDV_MaxChars(pDX, m_T, 16);
	//}}AFX_DATA_MAP
    DDX_FileEditCtrl(pDX, IDC_EDIT_PATH, m_DirEdit, FEC_FOLDER);
    DDX_FileEditCtrl(pDX, IDC_EDIT4, m_XpEdit4, FEC_FILEOPEN);
//    if (m_validate.GetCheck())
 //       DDV_FileEditCtrl(pDX, IDC_EDIT1);
}


BEGIN_MESSAGE_MAP(CCaCenterPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCenterPage)
//	ON_BN_CLICKED(IDC_B_PATH4, OnBPath4)
	ON_BN_CLICKED(IDC_B_CRL, OnBCrl)
	ON_BN_CLICKED(IDC_B_MKROOT, OnBMkroot)
	ON_BN_CLICKED(IDC_B_MKREQ, OnBMkreq)
	ON_BN_CLICKED(IDC_B_MKCERT, OnBMkcert)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_BN_CLICKED(IDC_B_MKCERT2, OnBMkcert2)
	ON_BN_CLICKED(IDC_CHECK_P12, OnCheckP12)
	ON_BN_CLICKED(IDC_B_CRL2, OnBCrl2)
	//}}AFX_MSG_MAP
    ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCenterPage message handlers
BOOL CCaCenterPage::OnToolTipNotify(UINT /*id*/, NMHDR* pTTTStruct, LRESULT* /*pResult*/)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
    UINT nID = pTTTStruct->idFrom;
	
	BOOL bCheckDer = ((CButton * )GetDlgItem(IDC_CHECK))->GetCheck();
	BOOL bCheckP12 = ((CButton * )GetDlgItem(IDC_CHECK_P12))->GetCheck();
	CString strPwd(""),
			strText;
	switch(nID)
	{
	case IDC_B_MKROOT:
		strText = CMiniCaApp::NormalCode("制作根证书");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER格式");
		else
			strText += CMiniCaApp::NormalCode("\rPME格式");
		if(bCheckP12)
			strText += CMiniCaApp::NormalCode(",附带PFX包");
		if(!strPwd.IsEmpty())
			strText += CMiniCaApp::NormalCode(",私钥加密");
		_tcscpy(pTTT->szText,_T(strText));//设置

		return TRUE;
	case IDC_B_MKCERT2:
		strText = CMiniCaApp::NormalCode("制作证书");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER格式");
		else
			strText += CMiniCaApp::NormalCode("\rPME格式");
		if(bCheckP12)
			strText += CMiniCaApp::NormalCode(",附带PFX包");
		if(!strPwd.IsEmpty())
			strText += CMiniCaApp::NormalCode(",私钥加密");
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_B_MKREQ:
		strText = CMiniCaApp::NormalCode("生成证书请求");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER格式");
		else
			strText += CMiniCaApp::NormalCode("\rPME格式");
		if(!strPwd.IsEmpty())
			strText += CMiniCaApp::NormalCode(",私钥加密");
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_B_MKCERT:
		strText = CMiniCaApp::NormalCode("通过证书请求制作证书");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER格式");
		else
			strText += CMiniCaApp::NormalCode("\rPME格式");
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
		
	}
	return FALSE;
}
	
BOOL CCaCenterPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	//	m_CheckP12.SetIcon(IDI_ICON14, IDI_ICON15);
	//	m_CheckP12.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	
	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_CHECK)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK_P12)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_COMBO_L)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_CN)->m_hWnd,TRUE);
		
		ClassXP(GetDlgItem(IDC_EDIT_PWD)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_E)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_C)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_O)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_OU)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_ST)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_L)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_T)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_G)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_S)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_DAY)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_SN)->m_hWnd,TRUE);
	}
	
	// m_DirEdit.ModifyStyleEx(0, WS_EX_ACCEPTFILES);
	//	m_DirEdit.SetClientTipText("CString text");
	//	m_DirEdit.ModifyStyleEx(0, FEC_BUTTONTIP);
	//	m_DirEdit.ModifyStyleEx(0, FEC_CLIENTTIP);
	//    m_DirEdit.ModifyFlags(FEC_FILEOPEN, FEC_FOLDER);
	
    HANDLE handle = ::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_ICON_DIR),
		IMAGE_ICON,
		0,
		0,
		LR_DEFAULTCOLOR);
    m_DirEdit.SetButtonImage(handle, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_DirEdit.SetButtonWidth(18);
	
    HANDLE handleCsr = ::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_ICON_VIEW),
		IMAGE_ICON,
		0,
		0,
		LR_DEFAULTCOLOR);
    m_XpEdit4.SetButtonImage(handleCsr, PJAI_ICON | PJAI_AUTODELETE | PJAI_STRETCHED );
	m_XpEdit4.SetButtonWidth(18);
	m_XpEdit4.SetCaption(MiniCT_0100);							//MiniCT_0100 "选择证书请求文件"
	m_XpEdit4.SetFilter(MiniCT_0101);	//MiniCT_0101 "证书请求(*.csr)|*.csr|所有文件(*.*)|*.*||"
	
	
	//	m_DirEdit.ModifyFlags(0, FEC_FLAT);
	//	m_DirEdit.ModifyFlags(FEC_FLAT | FEC_GRAYSCALE, 0);
	//	m_Bpath.SetIcon(IDI_ICON_DIR);//path
	//	m_Bpath.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
	
	//	m_Bpath4.SetIcon(IDI_ICON_VIEW);
	//	m_Bpath4.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	m_Bmkreq.SetIcon(IDI_ICON19);
	m_Bmkreq.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	m_Bmkcert2.SetIcon(IDI_ICON_CERT);
	m_Bmkcert2.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	m_Bmkcert.SetIcon(IDI_ICON_VCERT, (int)BTNST_AUTO_GRAY);//req - cert
	m_Bmkcert.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	m_Bcrl.SetIcon(IDI_ICON_MCRL);//CRL
	m_Bcrl.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	m_Bcrl2.SetIcon(IDI_ICON_EDIT);//CRL
	m_Bcrl2.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	m_Bmkroot.SetIcon(IDI_ICON_MROOT);
	m_Bmkroot.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	((CButton *)GetDlgItem(IDC_CHECK_P12))->SetCheck(m_MadePfx);
	
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
		m_toolTip.AddTool(GetDlgItem(IDC_CHECK), CMiniCaApp::NormalCode("证书格式\rDER或PEM格式"));
		m_toolTip.AddTool(GetDlgItem(IDC_CHECK_P12), CMiniCaApp::NormalCode("附带PFX包\r制作证书请求(REQ)时无效"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PATH), CMiniCaApp::NormalCode("证书存放路径"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT4), CMiniCaApp::NormalCode("证书请求存放路径"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_CRL2), CMiniCaApp::NormalCode("选择证书作废信息文件\r此文件是记录了作废证\n书序号的文本文件"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDITCRL), CMiniCaApp::NormalCode("作废证书序号列表\r以','分割的一系列证书序号"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_CRL), CMiniCaApp::NormalCode("制作黑名单"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_MKROOT), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_B_MKCERT2), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_B_MKREQ), LPSTR_TEXTCALLBACK);
		m_toolTip.AddTool(GetDlgItem(IDC_B_MKCERT), LPSTR_TEXTCALLBACK);
	
		// Change the color of one of the tooltips
	/*	OXTOOLINFO ToolInfo;
		if (m_toolTip.GetToolInfo(ToolInfo, GetDlgItem(IDC_CHECK)))
		{
			ToolInfo.clrBackColor = RGB(255, 255, 255);
			ToolInfo.clrTextColor = RGB(  0,   0, 255);
			m_toolTip.SetToolInfo(&ToolInfo);
		}*/
		
		// TODO: Use one of the following forms to add controls:
	}
	
	
	//加载配置信息
	//检测是否已经打开,如果打不开,则重新写入
	CFile file;
	if(!file.Open(m_IniPathName,CFile::modeRead))
	{
		/*得到配置*/
		HRSRC hRsrc = 0;
		if(CMiniCaApp::IsBig())
		{
			hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_MINICA_INI_BG),"INI");
		}
		else
			hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_MINICA_INI),"INI");
		
		DWORD lenCert = SizeofResource(NULL, hRsrc); 
		HGLOBAL hgCert=LoadResource(NULL,hRsrc);
		LPSTR lpCert=(LPSTR)LockResource(hgCert);
		
		file.Open(m_IniPathName,CFile::modeCreate|CFile::modeWrite);	//存文件
		{
			file.Write(lpCert,lenCert);
		}
		
	}
	file.Close();

	TranslateCT();
	
	char buf[255]={0};
	
	GetPrivateProfileString("CA", "RSALEN","1024",buf,50, m_IniPathName);
	//查找加载项是否有效512 768 1024 1536 2048
	int index = ((CComboBox *)GetDlgItem(IDC_COMBO_L))->FindString(-1,buf);
	if(index == -1)
		index = 2;
	((CComboBox *)GetDlgItem(IDC_COMBO_L))->SetCurSel(index);
	
	
	GetPrivateProfileString("CA", "DAY","365",buf,50, m_IniPathName);
	SetDlgItemText(IDC_EDIT_DAY,buf);
	
	GetPrivateProfileString("CA", "SN","1",buf,50, m_IniPathName);
	SetDlgItemText(IDC_EDIT_SN,buf);
	
	GetPrivateProfileString("CRL", "Revoke","1,2,3",buf,50, m_IniPathName);
	SetDlgItemText(IDC_EDITCRL,buf);
	
	
	//	m_StaticCN.SetTextColor(RGB(64,128,0));
	
	/*	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCertDbPage * pSetup = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));
	
	  if(pSetup) 
	  {
	  CString m_PathName = pSetup->GetCertPath(0);
	  SetDlgItemText(IDC_EDIT_PATH,m_PathName);
	  m_Path = m_PathName;
	}*/


	return TRUE;
}


void CCaCenterPage::OnBMkroot() //制作根证书
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_EDIT_PATH, m_Path);

	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);				//MiniCT_0112 " 请选择输出路径!"
		return;
	}
	char out[100]={0};

	CString str,strSN;

	stuSUBJECT * pRoot = NULL;

	GetDlgItemText(IDC_EDIT_C,str);
	pRoot->Add(pRoot, "C", str);

	GetDlgItemText(IDC_EDIT_ST,str);
	pRoot->Add(pRoot, "ST", str);

	GetDlgItemText(IDC_EDIT_L,str);
	pRoot->Add(pRoot, "L", str);

	GetDlgItemText(IDC_EDIT_O,str);
	pRoot->Add(pRoot, "O", str);

	GetDlgItemText(IDC_EDIT_OU,str);
	pRoot->Add(pRoot, "OU", str);

	CString strCN;
	GetDlgItemText(IDC_EDIT_CN,strCN);
	if(strCN.IsEmpty())
	{
		AddMsg(MiniCT_0113,M_WARING);			//MiniCT_0113 "请给出个体信息!"
		pRoot->RemoveAll(pRoot);	//删除证书结构
		return;
	}
	pRoot->Add(pRoot, "CN", strCN);


	GetDlgItemText(IDC_EDIT_E,str);
	pRoot->Add(pRoot, "emailAddress", str);

	GetDlgItemText(IDC_EDIT_T,str);
	pRoot->Add(pRoot, "title", str);

	GetDlgItemText(IDC_EDIT_G,str);
	pRoot->Add(pRoot, "givenName", str);

	GetDlgItemText(IDC_EDIT_S,str);
	pRoot->Add(pRoot, "SN", str);

	
	GetDlgItemText(IDC_EDIT_DAY,str);
	if(str.IsEmpty())
	{
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "请给出有效期!"
		pRoot->RemoveAll(pRoot);	//删除证书结构
		return;
	}

	char certBuf[10240] = {0};
	char keyBuf[10240] = {0};
	char p12Buf[10240] = {0};

	UINT certLen = 10240,
		 keyLen = 10240,
		 p12Len = 10240;

	GetDlgItemText(IDC_EDIT_SN,strSN);

	CString len;
	GetDlgItemText(IDC_COMBO_L,len);

	CString pwd("");
//	GetDlgItemText(IDC_EDIT_PWD,pwd);


	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCertDbPage * pDb = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));

	if(pDb)
	{
		m_NameType = pDb->GetNameType();
	}

	CString strName;
	if(0 == m_NameType)//用户名
	{
		strName.Format("\\R%s", strCN);
	}
	else if(1 == m_NameType)//序号
	{
		strName.Format("\\R%s", strSN);
	}
	else if(2 == m_NameType)//用户名 + 序号
	{
		strName.Format("\\R%s%s", strCN, strSN);
	}
	else if(3 == m_NameType)//序号 + 用户名
	{
		strName.Format("\\R%s%s", strSN, strCN);
	}

	BOOL bRet = MakeRoot(pRoot,"(MiniCA)", atoi(len),atoi(strSN),atoi(str),
		pwd.GetBuffer(0),"","",NULL,certBuf,
		&certLen,keyBuf,&keyLen,p12Buf,&p12Len,out,m_CertFormat);

	pRoot->RemoveAll(pRoot);	//删除证书结构

	if(bRet)
	{

		CString outCert = m_Path + strName + "_Cert";
		outCert += m_ExtName;
		
		CString outKey = m_Path + strName + "_Key";
		outKey += m_ExtName;
		
		CString p12 = m_Path + strName + ".pfx";
		
		FILE * pfc = fopen(outCert,"wb");
		if(!pfc)
		{
			AddMsg(MiniCT_0115,M_ERROR);		//MiniCT_0115 "保存根公钥失败"
			return;
		}
		fwrite(certBuf,sizeof(char),certLen,pfc);
		fclose(pfc);

		pfc = fopen(outKey,"wb");
		if(!pfc)
		{
			AddMsg(MiniCT_0116,M_ERROR);		//MiniCT_0116 "保存根私钥失败"
			return;
		}
		fwrite(keyBuf,sizeof(char),keyLen,pfc);
		fclose(pfc);
		AddMsg(MiniCT_0117);				//MiniCT_0117 "根证书制作成功"
		
		if(m_MadePfx)
		{
			pfc = fopen(p12,"wb");
			if(!pfc)
			{
				AddMsg(MiniCT_0118,M_ERROR);			//MiniCT_0118 "保存PFX文件失败"
				return;
			}
			fwrite(p12Buf,sizeof(char),p12Len,pfc);
			fclose(pfc);
			AddMsg(MiniCT_0119);				//MiniCT_0119 "根证书PFX包制作成功"
		}
		//序号增加一
		int iSn = atoi(strSN) + 1;
		CString sSn;
		sSn.Format("%d",iSn);
		WritePrivateProfileString("CA", "SN",sSn,m_IniPathName);
		SetDlgItemText(IDC_EDIT_SN,sSn);
	}
	else
		AddMsg(out,M_ERROR);	
	pwd.ReleaseBuffer();
}

void CCaCenterPage::OnBMkreq() //制作REG
{
	// TODO: Add your control notification handler code here
	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);		//MiniCT_0112 "请选择输出路径!"
		return;
	}
	
	char out[100]="";

	CString str,strCN;

	stuSUBJECT * pReq = NULL;

	GetDlgItemText(IDC_EDIT_C,str);
	pReq->Add(pReq, "C", str);

	GetDlgItemText(IDC_EDIT_ST,str);
	pReq->Add(pReq, "ST", str);

	GetDlgItemText(IDC_EDIT_L,str);
	pReq->Add(pReq, "L", str);

	GetDlgItemText(IDC_EDIT_O,str);
	pReq->Add(pReq, "O", str);

	GetDlgItemText(IDC_EDIT_OU,str);
	pReq->Add(pReq, "OU", str);

	GetDlgItemText(IDC_EDIT_CN,strCN);
	if(strCN.IsEmpty())
	{
		AddMsg(MiniCT_0113,M_WARING);			//MiniCT_0113 "请给出个体信息!"
		pReq->RemoveAll(pReq);	//删除证书结构
		return;
	}
	pReq->Add(pReq, "CN", strCN);


	GetDlgItemText(IDC_EDIT_E,str);
	pReq->Add(pReq, "emailAddress", str);

	GetDlgItemText(IDC_EDIT_T,str);
	pReq->Add(pReq, "title", str);

	GetDlgItemText(IDC_EDIT_G,str);
	pReq->Add(pReq, "givenName", str);

	GetDlgItemText(IDC_EDIT_S,str);
	pReq->Add(pReq, "SN", str);
	
	GetDlgItemText(IDC_EDIT_DAY,str);
	if(str=="")
	{
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "请给出有效期!"
		pReq->RemoveAll(pReq);	//删除证书结构
		return;
	}

	CString reqf, key;
	reqf.Format("%s\\%s%s", m_Path, strCN, "Req.csr");

	key.Format("%s\\%s%s%s", m_Path, strCN, "Key", m_ExtName);
	
	CString len;
	GetDlgItemText(IDC_COMBO_L,len);

	CString pwd("");
//	GetDlgItemText(IDC_EDIT_PWD,pwd);

	char reqBuf[10240] = {0};
	char keyBuf[10240] = {0};
	
	UINT reqLen = 10240,
		 keyLen = 10240;

	if(MakeReq(pReq, atoi(len), pwd.GetBuffer(0), reqBuf, &reqLen, keyBuf, &keyLen, out, m_CertFormat))
	{
		FILE * pfc = fopen(reqf,"wb");
		if(pfc != NULL)
		{
			fwrite(reqBuf,sizeof(char),reqLen,pfc);
			fclose(pfc);
			
			pfc = fopen(key,"wb");
			if(pfc != NULL)
			{
				fwrite(keyBuf,sizeof(char),keyLen,pfc);
				fclose(pfc);
			}
		}
		SetDlgItemText(IDC_EDIT4,reqf);
		AddMsg(MiniCT_0120);					//MiniCT_0120 "Req制作成功"
		pReq->RemoveAll(pReq);	//删除证书结构
	}
	else
		AddMsg(out,M_ERROR);
	pwd.ReleaseBuffer();
	pReq->RemoveAll(pReq);	//删除证书结构

}


void CCaCenterPage::OnBMkcert() //req 制作证书
{
	// TODO: Add your control notification handler code here
	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);		//MiniCT_0112 "请选择输出路径!"
		return;
	}
	
	CString req,day,outCert,strSN;
	GetDlgItemText(IDC_EDIT4,req);
	if(req=="")
	{
		AddMsg(MiniCT_0121,M_WARING);	//MiniCT_0121 "请选择证书请求文件!"
		return;
	}
	GetDlgItemText(IDC_EDIT_DAY,day);
	if(day=="")
	{
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "请给出有效期!"
		return;
	}
	char out[1024]={0};
	
	CString KUSAGE,EKUSAGE;
	//	GetKeyUsage(KUSAGE,EKUSAGE);//通过配置文件得到密钥用法
	
	CString RootP12,RootPwd;
	BOOL ret=FALSE;
	DWORD lenCert=0,lenKey=0;//公钥长度,私钥长度
	CString strPwd;//公私钥路径或内容,p12密钥
	
	char strCert[10240] = {0};
	char strKey[10240] = {0};

	char certBuf[10240] = {0};
	UINT certLen = 10240;


	if(((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd))
	{
		GetDlgItemText(IDC_EDIT_SN,strSN);
		outCert=m_Path+"\\UserCert";
		outCert+=m_ExtName;
		
		stuCertPair RootPair(strCert,lenCert,strKey,lenKey,
			strPwd.GetBuffer(0));
		
		strPwd.ReleaseBuffer();
		
		if(MakeCert(RootPair,
			atoi(strSN),0,atoi(day),
			req.GetBuffer(0),
			0,
			KUSAGE.GetBuffer(0),
			EKUSAGE.GetBuffer(0),
			NULL,
			certBuf,
			&certLen,
			out,m_CertFormat))
		{
			FILE * pfc = fopen(outCert,"wb");
			if(pfc != NULL)
			{
				fwrite(certBuf,sizeof(char),certLen,pfc);
				fclose(pfc);
			}
			AddMsg(MiniCT_0122);		//MiniCT_0122 "证书制作成功"
			//序号增加一
			int iSn = atoi(strSN) + 1;
			CString sSn;
			sSn.Format("%d",iSn);
			WritePrivateProfileString("CA", "SN",sSn,m_IniPathName);
			SetDlgItemText(IDC_EDIT_SN,sSn);
		}
		else
			AddMsg(out,M_ERROR);
		
		req.ReleaseBuffer();
		KUSAGE.ReleaseBuffer();
		EKUSAGE.ReleaseBuffer();
		outCert.ReleaseBuffer();
	}
	else
		AddMsg(MiniCT_0123,M_ERROR);		//MiniCT_0123 "加载根证书失败"
		
}

void CCaCenterPage::OnBMkcert2() //直接生成证书
{
	// TODO: Add your control notification handler code here
	char out[1024] = {0};
	char cert[10240] = {0},
		 key[10240] = {0},
		 p12[10240] = {0};

	UINT certl = 10240,
		keyl = 10240,
		p12l = 10240;

	CString str,day,strSN;

	stuSUBJECT * pCERT = NULL;

	GetDlgItemText(IDC_EDIT_C,str);
	pCERT->Add(pCERT, "C", str);

	GetDlgItemText(IDC_EDIT_ST,str);
	pCERT->Add(pCERT, "ST", str);

	GetDlgItemText(IDC_EDIT_L,str);
	pCERT->Add(pCERT, "L", str);

	GetDlgItemText(IDC_EDIT_O,str);
	pCERT->Add(pCERT, "O", str);

	GetDlgItemText(IDC_EDIT_OU,str);
	pCERT->Add(pCERT, "OU", str);

	CString strCN;
	GetDlgItemText(IDC_EDIT_CN,strCN);
	if(strCN.IsEmpty())
	{
		AddMsg(MiniCT_0113,M_WARING);		//MiniCT_0113 "请给出个体信息!"
		return;
	}
	pCERT->Add(pCERT, "CN", strCN);


	GetDlgItemText(IDC_EDIT_E,str);
	pCERT->Add(pCERT, "emailAddress", str);

	GetDlgItemText(IDC_EDIT_T,str);
	pCERT->Add(pCERT, "title", str);

	GetDlgItemText(IDC_EDIT_G,str);
	pCERT->Add(pCERT, "givenName", str);

	GetDlgItemText(IDC_EDIT_S,str);
	pCERT->Add(pCERT, "SN", str);

	GetDlgItemText(IDC_EDIT_DAY,day);
	CString KUSAGE,EKUSAGE;

	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);		//MiniCT_0112 "请选择输出路径!"
		pCERT->RemoveAll(pCERT);	//删除证书结构
		return;
	}

	if(day=="")
	{
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "请给出有效期!"
		pCERT->RemoveAll(pCERT);	//删除证书结构
		return;
	}

	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCertDbPage * pDb = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));

	if(pDb)
	{
		m_NameType = pDb->GetNameType();
	}

	CString strName;
	if(0 == m_NameType)//用户名
	{
		strName.Format("\\%s", strCN);
	}
	else if(1 == m_NameType)//序号
	{
		strName.Format("\\%s", strSN);
	}
	else if(2 == m_NameType)//用户名 + 序号
	{
		strName.Format("\\%s%s", strCN, strSN);
	}
	else if(3 == m_NameType)//序号 + 用户名
	{
		strName.Format("\\%s%s", strSN, strCN);
	}

	DWORD lenCert=0,lenKey=0;//公钥长度,私钥长度
	CString strPwd;//公私钥路径或内容,p12密钥

	char strCert[10240] = {0};
	char strKey[10240] = {0};

	if(((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd))
	{
		GetDlgItemText(IDC_EDIT_SN,strSN);
		
		CString len;
		GetDlgItemText(IDC_COMBO_L,len);
		
		CString pwd("");
		//			GetDlgItemText(IDC_EDIT_PWD,pwd);
		
		stuCertPair RootPair(strCert,lenCert,strKey,
			lenKey,strPwd.GetBuffer(0));
		
		strPwd.ReleaseBuffer();
		
		BOOL bRet = DirectCert(RootPair,atoi(len),atoi(strSN),
			0,atoi(day),pwd.GetBuffer(0),pCERT,"(MiniCA)",KUSAGE.GetBuffer(0),
			EKUSAGE.GetBuffer(0),NULL,
			cert,&certl,key,&keyl,p12,&p12l,out,m_CertFormat);
		
		pCERT->RemoveAll(pCERT);	//删除证书结构
		
		
		if(bRet)
		{
			CString outCert  =m_Path + strName + "Cert.cer" ;
			CString outKey = m_Path + strName + "Key.cer";
			CString p12path = m_Path + strName + ".pfx";
			
			FILE * pfc = fopen(outCert,"wb");
			if(pfc != NULL)
			{
				fwrite(cert,sizeof(char),certl,pfc);
				fclose(pfc);
				
				pfc = fopen(outKey,"wb");
				if(pfc != NULL)
				{
					fwrite(key,sizeof(char),keyl,pfc);
					fclose(pfc);
					AddMsg(MiniCT_0124);		//MiniCT_0124 "生成证书成功"
					if(m_MadePfx)
					{
						pfc = fopen(p12path,"wb");
						if(pfc != NULL)
						{
							fwrite(p12,sizeof(char),p12l,pfc);
							fclose(pfc);
							AddMsg(MiniCT_0125);		//MiniCT_0125 "证书PFX包制作成功"
						}
						else
							AddMsg(MiniCT_0126,M_ERROR);	//MiniCT_0126 "证书PFX包保存失败"
						
					}
				}
				else
					AddMsg(MiniCT_0127,M_ERROR);		//MiniCT_0127 "保存证书私钥失败"
				
				
				//序号增加一
				int iSn = atoi(strSN) + 1;
				CString sSn;
				sSn.Format("%d",iSn);
				WritePrivateProfileString("CA", "SN",sSn,m_IniPathName);
				SetDlgItemText(IDC_EDIT_SN,sSn);
				
			}
			else
				AddMsg(MiniCT_0128,M_ERROR);		//MiniCT_0128 "保存证书公钥失败"
			
			pwd.ReleaseBuffer();
			KUSAGE.ReleaseBuffer();
			EKUSAGE.ReleaseBuffer();
			
		}
		else
			AddMsg(out, M_ERROR);
		
	}
	else
		AddMsg(MiniCT_0123,M_ERROR);		//MiniCT_0123 "加载根证书失败"
}

void CCaCenterPage::OnBCrl2() //加载信息文件到文本框
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		MiniCT_0129,NULL);	//MiniCT_0129 "证书作废信息文件(*.txt)|*.txt|所有文件(*.*)|*.*||"

	CString strTitle = MiniCT_0130;	//MiniCT_0130  证书作废信息文件
	dlgOpen.m_ofn.lpstrTitle= strTitle;//标题条

	if(dlgOpen.DoModal()!=IDOK) return;
	CString crlinfo = dlgOpen.GetPathName();
	CString info,temp;
	if(!crlinfo.IsEmpty())
	{
		if(CStdioFile stdFile(crlinfo,CFile::modeRead))
		{
			while(stdFile.ReadString(temp))
			{
				info += temp;
			}
			
			stdFile.Close();
		}
	}
	SetDlgItemText(IDC_EDITCRL,info);
}

void CCaCenterPage::OnBCrl() 
{
	// TODO: Add your control notification handler code here
	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);	//MiniCT_0112 "请选择输出路径!"
		return;
	}

	char out[100]={0};
	CString outCrl=m_Path+"MiniCA.crl";
	CString RootP12,RootPwd;
	DWORD lenCert=0,lenKey=0;//公钥长度,私钥长度
	CString strPwd;//公私钥路径或内容,p12密钥

	char strCert[10240] = {0};
	char strKey[10240] = {0};

	if(((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd))
	{
		time_t t;
		time(&t);
		char * Crl=NULL;
		stuREVOKE * Head = NULL;//构造空链表
		CString strRevoke;
		strRevoke.Replace(" ",",");
		CString strL;
		GetDlgItemText(IDC_EDITCRL,strRevoke);
		strRevoke += ",";
		for(;;)
		{
			int state=strRevoke.Find(",");
			if(state==-1&&strRevoke.GetLength())
				strL=strRevoke;
			else if(strRevoke.GetLength()==0)
				break;
			else strL=strRevoke.Left(state);
			if(!strL.IsEmpty())
			{
				ULONG index = atol(strL.GetBuffer(0));
				if(index >0)
					Head->AddRevoke(Head,index,t);//增加表象
			}
			strRevoke=strRevoke.Right(strRevoke.GetLength()-state-1);
			strRevoke.TrimLeft();
		}
		
		stuCertPair RootPair(strCert,lenCert,strKey,lenKey,
			strPwd.GetBuffer(0));
		
		if(MakeCrl(RootPair,Head,NULL,Crl,NULL,outCrl.GetBuffer(0),out))
			AddMsg(out);
		else
			AddMsg(out,M_ERROR);
		Head->RemoveAll(Head);
	}
	else
		AddMsg(MiniCT_0123,M_ERROR);			//MiniCT_0123 "加载根证书失败"
}

void CCaCenterPage::OnCheck() //选择证书格式---〉输出
{
	// TODO: Add your control notification handler code here
	int check=((CButton *)GetDlgItem(IDC_CHECK))->GetCheck();
	if(check)
	{
		m_CertFormat=DER;
		m_ExtName=".cer";
	}
	else
	{
		m_CertFormat=PEM;
		m_ExtName=".cer";
	}
}

void CCaCenterPage::OnCheckP12() 
{
	// TODO: Add your control notification handler code here
	m_MadePfx=((CButton *)GetDlgItem(IDC_CHECK_P12))->GetCheck();
}

BOOL CCaCenterPage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CCaCenterPage::AddMsg(CString info, DWORD type)
{
	CMiniMainDlg * pMain = (CMiniMainDlg *)GetParent();
	pMain->AddMsg("MiniCA V2.0",info, type);
}

void CCaCenterPage::SetNamePath(const CString &strPath)	//CCertDbPage类中使用
{
	if(!strPath.IsEmpty())
	{
		SetDlgItemText(IDC_EDIT_PATH,strPath);
		m_Path = strPath;
	}
}

void CCaCenterPage::TranslateCT()	//繙譯諸如樹型控件,列錶控件等內容
{
	SetDlgItemText(IDC_EDIT_C, MiniCT_0102); //MiniCT_0102 "中国"
	
	SetDlgItemText(IDC_EDIT_ST, MiniCT_0103);	//MiniCT_0103 "河北"
	
	SetDlgItemText(IDC_EDIT_L, MiniCT_0104);//MiniCT_0104 
	
	SetDlgItemText(IDC_EDIT_O, MiniCT_0105);	//MiniCT_0105 "MiniSoft"
	
	SetDlgItemText(IDC_EDIT_OU, MiniCT_0106);	//MiniCT_0106 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_CN, MiniCT_0107);	//MiniCT_0107 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_T, MiniCT_0108);	//MiniCT_0108 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_G, MiniCT_0109);	//MiniCT_0109 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_S, MiniCT_0110);	//MiniCT_0110 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_E, MiniCT_0111);//MiniCT_0111 "hpxs@hotmail.com"

	//翻译STATIC文字
	SetDlgItemText(IDC_STATIC_CA1,	MiniCT_10101); 
	SetDlgItemText(IDC_STATIC_CN,	MiniCT_10102); 
	SetDlgItemText(IDC_STATIC_CA2,	MiniCT_10103); 
	SetDlgItemText(IDC_STATIC_CA3,	MiniCT_10104); 
	SetDlgItemText(IDC_STATIC_CA4,	MiniCT_10105); 
	SetDlgItemText(IDC_STATIC_CA5,	MiniCT_10106); 
	SetDlgItemText(IDC_STATIC_CA6,	MiniCT_10107); 
	SetDlgItemText(IDC_STATIC_CA7,	MiniCT_10108); 
	SetDlgItemText(IDC_STATIC_CA8,	MiniCT_10109); 
	SetDlgItemText(IDC_STATIC_CA9,	MiniCT_10110); 
	SetDlgItemText(IDC_STATIC_CA10, MiniCT_10111); 
	SetDlgItemText(IDC_STATIC_CA11, MiniCT_10112); 
	SetDlgItemText(IDC_STATIC_CA12, MiniCT_10113); 
	SetDlgItemText(IDC_STATIC_CA13, MiniCT_10114); 
	SetDlgItemText(IDC_STATIC_CA14, MiniCT_10115); 
	SetDlgItemText(IDC_STATIC_CA15, MiniCT_10116); 
	SetDlgItemText(IDC_STATIC_CA16, MiniCT_10117); 
	SetDlgItemText(IDC_STATIC_CA17, MiniCT_10118); 
	SetDlgItemText(IDC_STATIC_CA18, MiniCT_10119); 
	SetDlgItemText(IDC_STATIC_CA19, MiniCT_10120); 
	SetDlgItemText(IDC_CHECK, MiniCT_10121); 
	SetDlgItemText(IDC_CHECK_P12, MiniCT_10122); 
	SetDlgItemText(IDC_B_MKROOT, MiniCT_10123); 
	SetDlgItemText(IDC_B_MKCERT2, MiniCT_10124); 
	SetDlgItemText(IDC_B_CRL, MiniCT_10125); 
	SetDlgItemText(IDC_B_CRL2, MiniCT_10126); 
	SetDlgItemText(IDC_B_MKREQ, MiniCT_10127); 
	SetDlgItemText(IDC_B_MKCERT, MiniCT_10128);
}