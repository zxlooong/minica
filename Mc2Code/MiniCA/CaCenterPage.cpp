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
		strText = CMiniCaApp::NormalCode("������֤��");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER��ʽ");
		else
			strText += CMiniCaApp::NormalCode("\rPME��ʽ");
		if(bCheckP12)
			strText += CMiniCaApp::NormalCode(",����PFX��");
		if(!strPwd.IsEmpty())
			strText += CMiniCaApp::NormalCode(",˽Կ����");
		_tcscpy(pTTT->szText,_T(strText));//����

		return TRUE;
	case IDC_B_MKCERT2:
		strText = CMiniCaApp::NormalCode("����֤��");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER��ʽ");
		else
			strText += CMiniCaApp::NormalCode("\rPME��ʽ");
		if(bCheckP12)
			strText += CMiniCaApp::NormalCode(",����PFX��");
		if(!strPwd.IsEmpty())
			strText += CMiniCaApp::NormalCode(",˽Կ����");
		_tcscpy(pTTT->szText,strText);//����
		return TRUE;
	case IDC_B_MKREQ:
		strText = CMiniCaApp::NormalCode("����֤������");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER��ʽ");
		else
			strText += CMiniCaApp::NormalCode("\rPME��ʽ");
		if(!strPwd.IsEmpty())
			strText += CMiniCaApp::NormalCode(",˽Կ����");
		_tcscpy(pTTT->szText,strText);//����
		return TRUE;
	case IDC_B_MKCERT:
		strText = CMiniCaApp::NormalCode("ͨ��֤����������֤��");
		if(bCheckDer)
			strText += CMiniCaApp::NormalCode("\rDER��ʽ");
		else
			strText += CMiniCaApp::NormalCode("\rPME��ʽ");
		_tcscpy(pTTT->szText,strText);//����
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
	m_XpEdit4.SetCaption(MiniCT_0100);							//MiniCT_0100 "ѡ��֤�������ļ�"
	m_XpEdit4.SetFilter(MiniCT_0101);	//MiniCT_0101 "֤������(*.csr)|*.csr|�����ļ�(*.*)|*.*||"
	
	
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
		m_toolTip.AddTool(GetDlgItem(IDC_CHECK), CMiniCaApp::NormalCode("֤���ʽ\rDER��PEM��ʽ"));
		m_toolTip.AddTool(GetDlgItem(IDC_CHECK_P12), CMiniCaApp::NormalCode("����PFX��\r����֤������(REQ)ʱ��Ч"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PATH), CMiniCaApp::NormalCode("֤����·��"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT4), CMiniCaApp::NormalCode("֤��������·��"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_CRL2), CMiniCaApp::NormalCode("ѡ��֤��������Ϣ�ļ�\r���ļ��Ǽ�¼������֤\n����ŵ��ı��ļ�"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDITCRL), CMiniCaApp::NormalCode("����֤������б�\r��','�ָ��һϵ��֤�����"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_CRL), CMiniCaApp::NormalCode("����������"));
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
	
	
	//����������Ϣ
	//����Ƿ��Ѿ���,����򲻿�,������д��
	CFile file;
	if(!file.Open(m_IniPathName,CFile::modeRead))
	{
		/*�õ�����*/
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
		
		file.Open(m_IniPathName,CFile::modeCreate|CFile::modeWrite);	//���ļ�
		{
			file.Write(lpCert,lenCert);
		}
		
	}
	file.Close();

	TranslateCT();
	
	char buf[255]={0};
	
	GetPrivateProfileString("CA", "RSALEN","1024",buf,50, m_IniPathName);
	//���Ҽ������Ƿ���Ч512 768 1024 1536 2048
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


void CCaCenterPage::OnBMkroot() //������֤��
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_EDIT_PATH, m_Path);

	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);				//MiniCT_0112 " ��ѡ�����·��!"
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
		AddMsg(MiniCT_0113,M_WARING);			//MiniCT_0113 "�����������Ϣ!"
		pRoot->RemoveAll(pRoot);	//ɾ��֤��ṹ
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
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "�������Ч��!"
		pRoot->RemoveAll(pRoot);	//ɾ��֤��ṹ
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
	if(0 == m_NameType)//�û���
	{
		strName.Format("\\R%s", strCN);
	}
	else if(1 == m_NameType)//���
	{
		strName.Format("\\R%s", strSN);
	}
	else if(2 == m_NameType)//�û��� + ���
	{
		strName.Format("\\R%s%s", strCN, strSN);
	}
	else if(3 == m_NameType)//��� + �û���
	{
		strName.Format("\\R%s%s", strSN, strCN);
	}

	BOOL bRet = MakeRoot(pRoot,"(MiniCA)", atoi(len),atoi(strSN),atoi(str),
		pwd.GetBuffer(0),"","",NULL,certBuf,
		&certLen,keyBuf,&keyLen,p12Buf,&p12Len,out,m_CertFormat);

	pRoot->RemoveAll(pRoot);	//ɾ��֤��ṹ

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
			AddMsg(MiniCT_0115,M_ERROR);		//MiniCT_0115 "�������Կʧ��"
			return;
		}
		fwrite(certBuf,sizeof(char),certLen,pfc);
		fclose(pfc);

		pfc = fopen(outKey,"wb");
		if(!pfc)
		{
			AddMsg(MiniCT_0116,M_ERROR);		//MiniCT_0116 "�����˽Կʧ��"
			return;
		}
		fwrite(keyBuf,sizeof(char),keyLen,pfc);
		fclose(pfc);
		AddMsg(MiniCT_0117);				//MiniCT_0117 "��֤�������ɹ�"
		
		if(m_MadePfx)
		{
			pfc = fopen(p12,"wb");
			if(!pfc)
			{
				AddMsg(MiniCT_0118,M_ERROR);			//MiniCT_0118 "����PFX�ļ�ʧ��"
				return;
			}
			fwrite(p12Buf,sizeof(char),p12Len,pfc);
			fclose(pfc);
			AddMsg(MiniCT_0119);				//MiniCT_0119 "��֤��PFX�������ɹ�"
		}
		//�������һ
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

void CCaCenterPage::OnBMkreq() //����REG
{
	// TODO: Add your control notification handler code here
	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);		//MiniCT_0112 "��ѡ�����·��!"
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
		AddMsg(MiniCT_0113,M_WARING);			//MiniCT_0113 "�����������Ϣ!"
		pReq->RemoveAll(pReq);	//ɾ��֤��ṹ
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
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "�������Ч��!"
		pReq->RemoveAll(pReq);	//ɾ��֤��ṹ
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
		AddMsg(MiniCT_0120);					//MiniCT_0120 "Req�����ɹ�"
		pReq->RemoveAll(pReq);	//ɾ��֤��ṹ
	}
	else
		AddMsg(out,M_ERROR);
	pwd.ReleaseBuffer();
	pReq->RemoveAll(pReq);	//ɾ��֤��ṹ

}


void CCaCenterPage::OnBMkcert() //req ����֤��
{
	// TODO: Add your control notification handler code here
	if(m_Path=="") 
	{
		AddMsg(MiniCT_0112,M_WARING);		//MiniCT_0112 "��ѡ�����·��!"
		return;
	}
	
	CString req,day,outCert,strSN;
	GetDlgItemText(IDC_EDIT4,req);
	if(req=="")
	{
		AddMsg(MiniCT_0121,M_WARING);	//MiniCT_0121 "��ѡ��֤�������ļ�!"
		return;
	}
	GetDlgItemText(IDC_EDIT_DAY,day);
	if(day=="")
	{
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "�������Ч��!"
		return;
	}
	char out[1024]={0};
	
	CString KUSAGE,EKUSAGE;
	//	GetKeyUsage(KUSAGE,EKUSAGE);//ͨ�������ļ��õ���Կ�÷�
	
	CString RootP12,RootPwd;
	BOOL ret=FALSE;
	DWORD lenCert=0,lenKey=0;//��Կ����,˽Կ����
	CString strPwd;//��˽Կ·��������,p12��Կ
	
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
			AddMsg(MiniCT_0122);		//MiniCT_0122 "֤�������ɹ�"
			//�������һ
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
		AddMsg(MiniCT_0123,M_ERROR);		//MiniCT_0123 "���ظ�֤��ʧ��"
		
}

void CCaCenterPage::OnBMkcert2() //ֱ������֤��
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
		AddMsg(MiniCT_0113,M_WARING);		//MiniCT_0113 "�����������Ϣ!"
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
		AddMsg(MiniCT_0112,M_WARING);		//MiniCT_0112 "��ѡ�����·��!"
		pCERT->RemoveAll(pCERT);	//ɾ��֤��ṹ
		return;
	}

	if(day=="")
	{
		AddMsg(MiniCT_0114,M_WARING);		//MiniCT_0114 "�������Ч��!"
		pCERT->RemoveAll(pCERT);	//ɾ��֤��ṹ
		return;
	}

	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCertDbPage * pDb = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));

	if(pDb)
	{
		m_NameType = pDb->GetNameType();
	}

	CString strName;
	if(0 == m_NameType)//�û���
	{
		strName.Format("\\%s", strCN);
	}
	else if(1 == m_NameType)//���
	{
		strName.Format("\\%s", strSN);
	}
	else if(2 == m_NameType)//�û��� + ���
	{
		strName.Format("\\%s%s", strCN, strSN);
	}
	else if(3 == m_NameType)//��� + �û���
	{
		strName.Format("\\%s%s", strSN, strCN);
	}

	DWORD lenCert=0,lenKey=0;//��Կ����,˽Կ����
	CString strPwd;//��˽Կ·��������,p12��Կ

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
		
		pCERT->RemoveAll(pCERT);	//ɾ��֤��ṹ
		
		
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
					AddMsg(MiniCT_0124);		//MiniCT_0124 "����֤��ɹ�"
					if(m_MadePfx)
					{
						pfc = fopen(p12path,"wb");
						if(pfc != NULL)
						{
							fwrite(p12,sizeof(char),p12l,pfc);
							fclose(pfc);
							AddMsg(MiniCT_0125);		//MiniCT_0125 "֤��PFX�������ɹ�"
						}
						else
							AddMsg(MiniCT_0126,M_ERROR);	//MiniCT_0126 "֤��PFX������ʧ��"
						
					}
				}
				else
					AddMsg(MiniCT_0127,M_ERROR);		//MiniCT_0127 "����֤��˽Կʧ��"
				
				
				//�������һ
				int iSn = atoi(strSN) + 1;
				CString sSn;
				sSn.Format("%d",iSn);
				WritePrivateProfileString("CA", "SN",sSn,m_IniPathName);
				SetDlgItemText(IDC_EDIT_SN,sSn);
				
			}
			else
				AddMsg(MiniCT_0128,M_ERROR);		//MiniCT_0128 "����֤�鹫Կʧ��"
			
			pwd.ReleaseBuffer();
			KUSAGE.ReleaseBuffer();
			EKUSAGE.ReleaseBuffer();
			
		}
		else
			AddMsg(out, M_ERROR);
		
	}
	else
		AddMsg(MiniCT_0123,M_ERROR);		//MiniCT_0123 "���ظ�֤��ʧ��"
}

void CCaCenterPage::OnBCrl2() //������Ϣ�ļ����ı���
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		MiniCT_0129,NULL);	//MiniCT_0129 "֤��������Ϣ�ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||"

	CString strTitle = MiniCT_0130;	//MiniCT_0130  ֤��������Ϣ�ļ�
	dlgOpen.m_ofn.lpstrTitle= strTitle;//������

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
		AddMsg(MiniCT_0112,M_WARING);	//MiniCT_0112 "��ѡ�����·��!"
		return;
	}

	char out[100]={0};
	CString outCrl=m_Path+"MiniCA.crl";
	CString RootP12,RootPwd;
	DWORD lenCert=0,lenKey=0;//��Կ����,˽Կ����
	CString strPwd;//��˽Կ·��������,p12��Կ

	char strCert[10240] = {0};
	char strKey[10240] = {0};

	if(((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd))
	{
		time_t t;
		time(&t);
		char * Crl=NULL;
		stuREVOKE * Head = NULL;//���������
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
					Head->AddRevoke(Head,index,t);//���ӱ���
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
		AddMsg(MiniCT_0123,M_ERROR);			//MiniCT_0123 "���ظ�֤��ʧ��"
}

void CCaCenterPage::OnCheck() //ѡ��֤���ʽ---�����
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

void CCaCenterPage::SetNamePath(const CString &strPath)	//CCertDbPage����ʹ��
{
	if(!strPath.IsEmpty())
	{
		SetDlgItemText(IDC_EDIT_PATH,strPath);
		m_Path = strPath;
	}
}

void CCaCenterPage::TranslateCT()	//���g�T����Ϳؼ�,���l�ؼ��ȃ���
{
	SetDlgItemText(IDC_EDIT_C, MiniCT_0102); //MiniCT_0102 "�й�"
	
	SetDlgItemText(IDC_EDIT_ST, MiniCT_0103);	//MiniCT_0103 "�ӱ�"
	
	SetDlgItemText(IDC_EDIT_L, MiniCT_0104);//MiniCT_0104 
	
	SetDlgItemText(IDC_EDIT_O, MiniCT_0105);	//MiniCT_0105 "MiniSoft"
	
	SetDlgItemText(IDC_EDIT_OU, MiniCT_0106);	//MiniCT_0106 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_CN, MiniCT_0107);	//MiniCT_0107 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_T, MiniCT_0108);	//MiniCT_0108 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_G, MiniCT_0109);	//MiniCT_0109 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_S, MiniCT_0110);	//MiniCT_0110 "MiniCA"
	
	SetDlgItemText(IDC_EDIT_E, MiniCT_0111);//MiniCT_0111 "hpxs@hotmail.com"

	//����STATIC����
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