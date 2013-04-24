// RegPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "RegPage.h"
#include "MiniMainDlg.h"
#include "mapi.h"                     
#include "Evp.h"
#include ".\GenericClass\Language.h"
#include "minicT.h"
#include ".\Control\Smtp\Smtp.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegPage property page

IMPLEMENT_DYNCREATE(CRegPage, CPropertyPage)

CRegPage::CRegPage() : CPropertyPage(CRegPage::IDD)
{
	//{{AFX_DATA_INIT(CRegPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CRegPage::~CRegPage()
{
}

void CRegPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegPage)
//	DDX_Control(pDX, IDC_EDIT_USER, m_XpEditUser);
//	DDX_Control(pDX, IDC_EDIT_REGCODE, m_XpEditReg);
	DDX_Control(pDX, IDC_STATICTHANK, m_ThankStatic);
	DDX_Control(pDX, IDC_BREG, m_Breg);
	DDX_Control(pDX, IDC_BGET_REG, m_Bgetreg);
	DDX_Control(pDX, IDC_BGET_CHECK, m_Bgetcheck);
	DDX_Control(pDX, IDC_B_SELECT, m_Bselect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRegPage)
	ON_BN_CLICKED(IDC_BGET_CHECK, OnBgetCheck)
	ON_BN_CLICKED(IDC_BGET_REG, OnBgetReg)
	ON_BN_CLICKED(IDC_BREG, OnBreg)
	ON_BN_CLICKED(IDC_B_SELECT, OnBSelect)
	ON_BN_CLICKED(IDC_REG_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_REG_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_BPOST, OnBpost)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegPage message handlers

BOOL CRegPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_REG_RADIO1)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_REG_RADIO2)->m_hWnd,TRUE);
	}

	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
	m_Breg.SetIcon(IDI_ICON10);
	m_Breg.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bgetreg.SetIcon(IDI_ICON10);
	m_Bgetreg.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bgetcheck.SetIcon(IDI_ICON_SELECTDIR);
	m_Bgetcheck.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bselect.SetIcon(IDI_ICON_SELECTDIR);
	m_Bselect.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

//	m_ThankStatic.SetTextColor(RGB(128,87,28));


	((CButton * )GetDlgItem(IDC_REG_RADIO1))->SetCheck(1); //�Ƿ�ע��
	((CButton * )GetDlgItem(IDC_RADIO_E))->SetCheck(1); //��ҵ��

	
	if(((CMiniCaApp * )AfxGetApp())->IsHpxs())//����
	{
		GetDlgItem(IDC_BGET_CHECK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BGET_REG)->ShowWindow(SW_SHOW);
//		GetDlgItem(IDC_STATIC11)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_REGCODE)->EnableWindow(1);
		GetDlgItem(IDC_B_SELECT)->EnableWindow(1);
	}
	else
	{
		GetDlgItem(IDC_BGET_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BGET_REG)->ShowWindow(SW_HIDE);
	}
	// TODO: Add extra initialization here
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
		m_toolTip.AddTool(GetDlgItem(IDC_REG_RADIO1), "����ע������");
		m_toolTip.AddTool(GetDlgItem(IDC_REG_RADIO2), "ע��ϵͳ\r�������ע��Ӧ��");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_REGCODE), "ע��Ӧ��洢·��");
		m_toolTip.AddTool(GetDlgItem(IDC_B_SELECT), "ѡ��ע��Ӧ���ļ�");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_USER), "Ҫע����û���");
		m_toolTip.AddTool(GetDlgItem(IDC_BREG), "����ע������");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_USER2), "�û�����\rMiniCAͨ�����������û�����ע��Ӧ��");
		m_toolTip.AddTool(GetDlgItem(IDC_BPOST), "�����ʼ�����ע��\r�û����������ʵ\n �����ղ�����");

		// TODO: Use one of the following forms to add controls:
		// m_toolTip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_toolTip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CRegPage::GetMachineCode()//�õ�������
{
	return ((CMiniCaApp *)AfxGetApp())->GetMachineCode();
}

CString CRegPage::GetSelfCode(CString filePath)//�����ļ���֤��
{
	//���ȶ��ļ�ժҪ,Ȼ����˽Կǩ��
	CString mdname("sha1");
	char outMsg[100]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[1024*2]="";
	unsigned int md_len;
	UCHAR bufsign[1024]={0};
	UINT lensign=0;
		
	if(!CEvp::Digest(mdname.GetBuffer(0),filePath.GetBuffer(0),0,//�ļ�
		md_value,&md_len, outMsg))//��ϢժҪ
	{
		AddMsg(outMsg,M_ERROR);
		return "Error";
	}

	/*˽Կ*/
	HRSRC hRsrc = NULL;
	DWORD lenKey = 0;
	HGLOBAL hgKey = NULL;
	LPSTR lpKey;

	hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_VERIFY_KEY),"CERT");
	if(hRsrc)
	{
		lenKey = SizeofResource(NULL, hRsrc); 
		hgKey=LoadResource(NULL,hRsrc);
		if(hgKey)
			lpKey=(LPSTR)LockResource(hgKey);
	}
	
	if(!CEvp::Sign(lpKey,lenKey,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//��ʾ�ڴ�
		(char *)bufsign,lensign,/*��ʾ�ڴ�*/outMsg))
	{
		AddMsg(outMsg,M_ERROR);
		return "Error";
	}
	 
	for(UINT i=0;i<lensign;i++)
	{
		sprintf((char *)&buf[i*2],"%02X",bufsign[i]);//02x��ʾ1��16���Ʊ�Ϊ2���ַ����ղ���
	}
	CString str(buf);
	return str;
}

BOOL CRegPage::CheckRegCode(CString username,CString regcode,BOOL type)//����ע����
{
	//���ȸ����û���sha1
	CString mdname("sha1");
	char outMsg[100]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[1024*2]="";
	unsigned int md_len;
	UCHAR bufsign[1024]={0};
	int lensign=0;
	char lm[257]={0};//��������
	char temp[3]={0};//��ʱ����
	
	username += GetMachineCode();

	CString strADD = "������&&ĸ����&&ѩ��&&�޻�ƽ&&���ӽ�";//������Ϣ
//	strADD.LoadString(IDS_STR_MINICA);
	username += strADD;
	
	if(!CEvp::Digest(mdname.GetBuffer(0),username.GetBuffer(0),username.GetLength(),
		md_value,&md_len, outMsg))//��ϢժҪ
	{
		//	AddMsg(outMsg,M_ERROR);
		return FALSE;
	}
	
	char * Cert = 0;
	if(type)
	{
		Cert = MAKEINTRESOURCE(IDR_REGE_CERT);//��ҵ
	}
	else
		Cert = MAKEINTRESOURCE(IDR_REGI_CERT);//����

	HRSRC hRsrc=FindResource(NULL,Cert,"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert=LoadResource(NULL,hRsrc);
	LPSTR lpCert=(LPSTR)LockResource(hgCert);
	
	//regcode ����16�������� ����B5C3 D6F8->��Ӧ2�����뺺��
	//ȡ��B5ת����10����,���������һ��λ��
	for(int j=0;j<regcode.GetLength();j+=2)
	{
		strncpy(temp,regcode.GetBuffer(0)+j,2);
		lm[j/2] = CMiniCaApp::HexToTen(temp);
	}
	if(CEvp::VerifySign(lpCert,lenCert,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//�ڴ�����
		lm/*ǩ�����,ע���->ת��*/,outMsg))
	{
		return TRUE;
	}
	else
		return FALSE;
	
}

void CRegPage::OnBgetCheck() //�õ���֤��
{
	// TODO: Add your control notification handler code here
	CString file;
	GetDlgItemText(IDC_EDIT_REGCODE,file);
	file.TrimLeft();
	file.TrimRight();
	if(file.IsEmpty())
	{
		AddMsg("�ļ�������Ϊ��",M_ERROR);
		return;
	}
	//������ע���
	CString code = GetSelfCode(file);
	SetDlgItemText(IDC_EDIT_USER,code);
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, code.GetLength()+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(code.GetBuffer(0)));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}

}

CString CRegPage::GetRegCode(CString username,BOOL bRegE)//�õ�ע����
{
	//���ȸ����û���sha1
	CString mdname("sha1");
	char outMsg[100]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[1024*2]="";
	unsigned int md_len;
	UCHAR bufsign[1024]={0};
	UINT lensign=0;

	CString strADD = "������&&ĸ����&&ѩ��&&�޻�ƽ&&���ӽ�";//������Ϣ
//	strADD.LoadString(IDS_STR_MINICA);
	username += strADD;

	if(!CEvp::Digest(mdname.GetBuffer(0),username.GetBuffer(0),username.GetLength(),
		md_value,&md_len, outMsg))//��ϢժҪ
	{
		AddMsg(outMsg,M_ERROR);
		return "";
	}
	char * Key = 0;
	if(bRegE)
	{
		Key = MAKEINTRESOURCE(IDR_REGE_KEY);//��ҵ
	}
	else
		Key = MAKEINTRESOURCE(IDR_REGI_KEY);//����
	/*˽Կ*/
	HRSRC hRsrc = FindResource(NULL,Key,"CERT");
	DWORD lenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey = LoadResource(NULL,hRsrc);
	LPSTR lpKey = (LPSTR)LockResource(hgKey);

	if(!CEvp::Sign(lpKey,lenKey,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//��ʾ�ڴ�
		(char *)bufsign,lensign,/*��ʾ�ڴ�*/outMsg))
	{
		AddMsg(outMsg,M_ERROR);
		return "";
	}
	 
	for(UINT i=0;i<lensign;i++)
	{
		sprintf((char *)&buf[i*2],"%02X",bufsign[i]);//02x��ʾ1��16���Ʊ�Ϊ2���ַ����ղ���
	}
	CString str512(buf);
	return str512;
}

void CRegPage::OnBgetReg() //����mres�ļ�
{
	// TODO: Add your control notification handler code here
	CString mci;
	GetDlgItemText(IDC_EDIT_REGCODE,mci);
	if(mci.IsEmpty())
	{
		AddMsg("��ѡ��*.mreq�ļ�",M_ERROR);
		return;
	}
	CFile MciFile;
	MciFile.Open(mci,CFile::typeBinary|CFile::modeRead);
	DWORD len = MciFile.GetLength();
	char * info = new char[len+1];
	memset(info,0,len+1);
	MciFile.Read(info,len);
	MciFile.Close();

	//16 -> LM
	char strLm[2048] = {0};
	char outMsg[100];
	UINT uLm = CMiniCaApp::HTextToLm(info,strLm);

	delete [] info;
	/*˽Կ*/
	HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_REGI_KEY),"CERT");
	DWORD dlenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey=LoadResource(NULL,hRsrc);
	LPSTR Key=(LPSTR)LockResource(hgKey);
	
	RegReq regReq;
	//˽Կ����ע������
	len = sizeof(RegReq);
	if(CEvp::RSAPrivDec((BYTE *)Key,dlenKey,"",(BYTE *)strLm,uLm, 
		(BYTE *)&regReq,len ,outMsg))
	{
		//ͨ���û����ͻ������������к�
		//�����,�û���,������,ע����,ͨ��˽Կ����д��MCR�ļ���
		//�û����� ͨ����Կ���� ,��֤ ������,д��ע���
		RegRes regRes;
		regRes.UserInfo = regReq;
		CString sName,
			    sMac,
				sVer,
				sTemp;

		sName.Format("%s",regReq.UserName);
		sTemp.Format("UserName: %s",sName);
		AddMsg(sTemp,M_WARING);

		sMac.Format("%s",regReq.Mac);
		sTemp.Format("HardCode: %s",sMac);
		AddMsg(sTemp,M_WARING);

		sVer = sName + sMac;
		strcpy(regRes.RegCode,GetRegCode(sVer,regReq.RegType));
		
		//˽Կ����
		char McR[20480] = {0};
		len = 20480;
		if(!CEvp::RSAPrivEnc((BYTE *)Key,dlenKey,"",
				(BYTE *)&regRes,sizeof(regRes),(BYTE *)McR,len,outMsg))
		{
			AddMsg(outMsg,M_ERROR);
		}
		else
		{
			CString strReg = CMiniCaApp::LmToHText((UCHAR *)McR,len);
			CFile McrFile;
			CString strFielName = sName + ".mres";
			strFielName = "\\" + strFielName;
			CString strRes = ((CMiniCaApp *)AfxGetApp())->GetAppPath() + strFielName;
			McrFile.Open(strRes,CFile::modeCreate|CFile::typeBinary|CFile::modeWrite);
			McrFile.Write(strReg.GetBuffer(0),strReg.GetLength());
			McrFile.Close();
			strRes += MiniCT_1900;//"  ���ɳɹ�"
			AddMsg(strRes);
		}

	}
	else
	{
		AddMsg(MiniCT_1901, M_ERROR); //Ӧ���ļ����ɴ���
	}

//	SetDlgItemText(IDC_EDIT_REGCODE,);
}

void CRegPage::OnBreg() //ע��
{
	// TODO: Add your control notification handler code here
	CString username,regcode,resfile;

	int iStateM = ((CButton * )GetDlgItem(IDC_REG_RADIO1))->GetCheck(); //�Ƿ���������
	int iStateR = ((CButton * )GetDlgItem(IDC_REG_RADIO2))->GetCheck(); //�Ƿ�ע��

	/*��Կ*/
	HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_REGI_CERT),"CERT");
	DWORD dlenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey=LoadResource(NULL,hRsrc);
	LPSTR Key=(LPSTR)LockResource(hgKey);

	BOOL bE = TRUE;//ָ��Ϊ��ҵ�汾FALSE;//�Ƿ�Ϊ��ҵ���û�

	if(iStateM)//����ע���ļ�
	{
		GetDlgItemText(IDC_EDIT_USER,username);
		if(username.IsEmpty())
		{
			AddMsg(MiniCT_1902, M_ERROR); //�������û���
			return;
		}
		int iStateE = ((CButton * )GetDlgItem(IDC_RADIO_E))->GetCheck();
		int iStateI = ((CButton * )GetDlgItem(IDC_RADIO_I))->GetCheck();
/*		if(iStateE == 0 && iStateI == 0)
		{
			AddMsg("��ѡ��һ��ע�᷽ʽ",M_ERROR);
			return;
		}
		CString sInfo;
		if(iStateE == 1)
		{
			sInfo.Format("��ȷ��Ҫע��Ϊ��ҵ���û���?");
			bE = TRUE;
		}
		else if(iStateI == 1)
		{
			sInfo.Format("��ȷ��Ҫע��Ϊ���˰��û���?");
			bE = FALSE;
		}
*/		
		if(MessageBox(MiniCT_1903, //��ȷ��Ҫע����?
			MiniCT_1904,MB_ICONQUESTION | MB_YESNO) == IDYES ) //ע����ʾ
		{
			//����ע���ļ�
			RegReq regReq;
			regReq.RegType = bE;
			strncpy(regReq.UserName,username,18);

			CString strHard = GetMachineCode();
			strncpy(regReq.Mac, strHard, 42);
//			strHard = "������:" + strHard;
//			AddMsg(strHard);

			//���ع�Ҫ(���˰�)
			char outMsg[100] = {0};
			char McR[2048] ={0};
			DWORD len = 2048;
			if(!CEvp::RSAPubEnc((BYTE *)Key,dlenKey,"",
				(BYTE *)&regReq,sizeof(regReq),(BYTE *)McR,len,outMsg))
			{
				AddMsg(outMsg,M_ERROR);
			}
			else
			{
				CString strReg = CMiniCaApp::LmToHText((UCHAR *)McR,len);
				CFile MciFile; //ע������ *.mci
				CString strReq;
				strReq.Format("%s\\%s.mreq", ((CMiniCaApp *)AfxGetApp())->GetAppPath(), username);
				if(MciFile.Open(strReq,CFile::modeCreate|CFile::typeBinary|CFile::modeWrite))
				{
					MciFile.Write(strReg.GetBuffer(0),strReg.GetLength());
					MciFile.Close();
				}
				else
					AddMsg("�����ļ�ʧ��");
				//����ע���ļ�,���ʼ�ʱ�����Ƿ��д��ļ�
				m_ReqFile = strReq;

				strReq += MiniCT_1900;
				AddMsg(strReq);
				//����
			//	SendMail();
			}
			return;
			
		}
		else
			return;
	}
	else if(iStateR)//У��ע���ļ�
	{
		
		//�õ�ע�����ͣ��û����������룬ע����
//		CString strReg = CMiniCaApp::LmToHText((UCHAR *)McR,len);
		//��MCR�ļ�
		GetDlgItemText(IDC_EDIT_REGCODE,username);
		if(username.IsEmpty())
		{
			AddMsg(MiniCT_1905, M_ERROR); //��ѡ��ע��Ӧ���ļ�
			return;
		}
		CFile McrFile; //ע���ļ� *.mcr
		McrFile.Open(username,CFile::typeBinary|CFile::modeRead);
		DWORD mcrLen = McrFile.GetLength();
		char * pMcr = new char[mcrLen+1];
		memset(pMcr,0,mcrLen+1);
		McrFile.Read(pMcr,mcrLen);
		McrFile.Close();

		//16����ת����
		//16 -> LM
		char * plm = new char[mcrLen/2+1];
		memset(plm,0,mcrLen/2+1);
		UINT uLm = CMiniCaApp::HTextToLm(pMcr,plm);
		delete [] pMcr;

		//�ø��˰湫Կ����
		RegRes regRes;
		DWORD len = sizeof(regRes);
		char outMsg[100] = {0};
		if(!CEvp::RSAPubDec((BYTE *)Key,dlenKey,"",(BYTE *)plm, uLm, 
			(BYTE *)&regRes,len ,outMsg))
		{
			AddMsg(outMsg,M_ERROR);
		}
		else 
		{
			//У�������
			//
			CString strHard,strRes;
			strHard = GetMachineCode();
			strRes.Format("%s",regRes.UserInfo.Mac);
			if(strcmp(strHard.GetBuffer(0),strRes.GetBuffer(0)) != 0)
			{
				strHard = "HardCode:" + strHard;
				AddMsg(strHard);

				strRes = "ResCode:" + strRes;
				AddMsg(strRes);

				AddMsg(MiniCT_1906,M_ERROR);
				return;
			}
			if(CheckRegCode(regRes.UserInfo.UserName,regRes.RegCode,regRes.UserInfo.RegType))
			{
				username.Format("%s",regRes.UserInfo.UserName);
				regcode.Format("%s",regRes.RegCode);
				CString sType,sInfo;
				if(regRes.UserInfo.RegType)
					sType.Format(MiniCT_1907);
				else
					sType.Format(MiniCT_1908);
				sInfo.Format("%s%s,%s,%s",MiniCT_1909,regRes.UserInfo.UserName,sType,MiniCT_1910);
				if(MessageBox(CMiniCaApp::NormalCode(sInfo),MiniCT_1904,
					MB_ICONQUESTION | MB_YESNO) == IDYES )
				{
					//д��ע���
					HKEY hKey = NULL; 
					
					CString strMiniCA,strUser,strRegCode;
					strMiniCA.LoadString(IDS_REG_KEY);// IDS_REG_KEYΪ��ע����е���Ŀ¼�ַ��� 
					strUser.LoadString(IDS_REG_USER);
					strRegCode.LoadString(IDS_REG_CODE);
					
					if(RegCreateKey(HKEY_LOCAL_MACHINE,TEXT(strMiniCA),&hKey ) != ERROR_SUCCESS )//û���򴴽�
					{
						AddMsg("������������",M_ERROR);
						return;
					}
					if(RegSetValueEx(hKey,TEXT(strUser),0,REG_SZ,(LPBYTE)username.GetBuffer(0),username.GetLength()+1)!=ERROR_SUCCESS)
					{
						AddMsg("�����û���������",M_ERROR);
						return;
					}
					if (RegSetValueEx(hKey,TEXT(strRegCode),0,REG_SZ,(LPBYTE)regcode.GetBuffer(0),regcode.GetLength()+1)!=ERROR_SUCCESS)
					{
						AddMsg("����ע���������",M_ERROR);
						return;
					}
					
					RegCloseKey(hKey);
					
					((CMiniMainDlg*)AfxGetMainWnd())->SetRegFlag(TRUE);
					//		AfxGetMainWnd ()->SetWindowText("MiniCA V2.0 ����&&ע��");
					AddMsg(MiniCT_1911); //ע��ɹ�
					if(strlen(regRes.RegCode) == 128)
					{
//123						((CMiniCaSheet*)AfxGetMainWnd())->RemovePage(this);
//123						((CMiniCaSheet*)AfxGetMainWnd())->AddPage(((CMiniCaApp *)AfxGetApp())->m_pPageSetup);
//123						UINT nCount = ((CMiniCaSheet*)AfxGetMainWnd())->GetPageCount();
//123						((CMiniCaSheet*)AfxGetMainWnd())->SetActivePage(nCount -1 );
					}
					else //ע��Ϊ��ҵ�û� ��������
					{
						AddMsg(MiniCT_1912,M_WARING);
						AfxGetMainWnd()->SendMessage(WM_CLOSE,0,0);
						CString sMiniCA;
						GetModuleFileName(NULL,sMiniCA.GetBufferSetLength (MAX_PATH+1),MAX_PATH);
						sMiniCA.ReleaseBuffer();
//						sMiniCA += " reset";
						WinExec(sMiniCA, SW_SHOW); 
					}			
				}
			}
			else
			{
				AddMsg(MiniCT_1913,M_ERROR);
			}
		}
		delete [] plm;
	}
}

BOOL CRegPage::GetRegInfo(CString & User,CString & RegCode)
{
	HKEY hKEY;//�����йص� hKEY, �ڲ�ѯ����ʱҪ�رա� 
	CString strMiniCA,strUser,strRegCode;
	strMiniCA.LoadString(IDS_REG_KEY);// IDS_REG_KEYΪ��ע����е���Ŀ¼�ַ��� 
	strUser.LoadString(IDS_REG_USER);
	strRegCode.LoadString(IDS_REG_CODE);
	
	//hKEY����һ������Ϊ�������ƣ��ڶ��������� 
	//��ʾҪ���ʵļ���λ�ã���������������Ϊ0��KEY_READ��ʾ�Բ�ѯ�ķ�ʽ�� 
	//����ע���hKEY�򱣴�˺������򿪵ļ��ľ���� 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,strMiniCA, 0, KEY_READ, &hKEY)); 
	if(ret0!=ERROR_SUCCESS)//����޷���hKEY������ֹ�����ִ�� 
	{
		return FALSE;
	} 
	//��ѯ�û��� 
	DWORD Type=REG_SZ;
	DWORD UserLen=256; 
	LPBYTE lUser=new BYTE[UserLen]; 
	//hKEYΪ�ղ�RegOpenKeyEx()�������򿪵ļ��ľ������RegisteredOwner�塣 
	//��ʾҪ�� ѯ�ļ�ֵ����type_1��ʾ��ѯ���ݵ����ͣ�owner_Get�������� 
	//��ѯ�����ݣ�cbData_1��ʾԤ���õ����ݳ��ȡ� 
	long ret=::RegQueryValueEx(hKEY, strUser, NULL, &Type, lUser, &UserLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		return FALSE; 
	} 
	//��ѯע����
	DWORD RegLen=513; 
	LPBYTE lRegCode=new BYTE[RegLen]; 
	ret=::RegQueryValueEx(hKEY, strRegCode, NULL,&Type,lRegCode,&RegLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		delete [] lRegCode;
		return FALSE; 
	} 
	//�� owner_Get �� company_Get ת��Ϊ CString �ַ���, �Ա���ʾ����� 
	User=CString(lUser); 
	RegCode=CString(lRegCode); 
	delete [] lUser; 
	delete [] lRegCode;
	//У���û�����ע����
	::RegCloseKey(hKEY); 
	return TRUE;
}

BOOL CRegPage::GetRegFlag()//�鿴�Ƿ�ע��
{
	HKEY hKEY;//�����йص� hKEY, �ڲ�ѯ����ʱҪ�رա� 
	CString strMiniCA,strUser,strRegCode;
	strMiniCA.LoadString(IDS_REG_KEY);// IDS_REG_KEYΪ��ע����е���Ŀ¼�ַ��� 
	strUser.LoadString(IDS_REG_USER);
	strRegCode.LoadString(IDS_REG_CODE);
	
	
	//hKEY����һ������Ϊ�������ƣ��ڶ��������� 
	//��ʾҪ���ʵļ���λ�ã���������������Ϊ0��KEY_READ��ʾ�Բ�ѯ�ķ�ʽ�� 
	//����ע���hKEY�򱣴�˺������򿪵ļ��ľ���� 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,strMiniCA, 0, KEY_READ, &hKEY)); 
	if(ret0!=ERROR_SUCCESS)//����޷���hKEY������ֹ�����ִ�� 
	{
		return FALSE;
	} 
	//��ѯ�û��� 
	DWORD Type=REG_SZ;
	DWORD UserLen=256; 
	LPBYTE lUser=new BYTE[UserLen]; 
	//hKEYΪ�ղ�RegOpenKeyEx()�������򿪵ļ��ľ������RegisteredOwner�塣 
	//��ʾҪ�� ѯ�ļ�ֵ����type_1��ʾ��ѯ���ݵ����ͣ�owner_Get�������� 
	//��ѯ�����ݣ�cbData_1��ʾԤ���õ����ݳ��ȡ� 
	long ret=::RegQueryValueEx(hKEY, strUser, NULL, &Type, lUser, &UserLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		::RegCloseKey(hKEY); 
		return FALSE; 
	} 
	//��ѯע����
	DWORD RegLen=513; //����+1
	LPBYTE lRegCode=new BYTE[RegLen]; 
	ret=::RegQueryValueEx(hKEY, strRegCode, NULL,&Type,lRegCode,&RegLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		delete [] lRegCode;
		::RegCloseKey(hKEY); 
		return FALSE; 
	} 
	//�� owner_Get �� company_Get ת��Ϊ CString �ַ���, �Ա���ʾ����� 
	CString User=CString(lUser); 
	CString RegCode=CString(lRegCode); 
	delete [] lUser; 
	delete [] lRegCode;
	//У���û�����ע����
	ret=CheckRegCode(User,RegCode);
	if(!ret)
	{
		::RegCloseKey(hKEY); 
		return FALSE;
	}
	::RegCloseKey(hKEY); 
	return TRUE;
}

void CRegPage::OnBSelect() 
{
	// TODO: Add your control notification handler code here
	CString strFilite;
	int iStateM = GetDlgItem(IDC_BGET_REG)->IsWindowVisible();
	if(iStateM)
		strFilite.Format("MiniCAע��(*.mres;*.mreq)|*.mres;*.mreq|�����ļ�(*.*)|*.*||",NULL);
	else
		strFilite.Format("MiniCAע��(*.mres)|*.mres|");

	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		CMiniCaApp::NormalCode(strFilite),NULL);

	CString strTitle = MiniCT_1914;
	dlgOpen.m_ofn.lpstrTitle = strTitle;//������

	if(dlgOpen.DoModal()!=IDOK) return;
	CString fileName=dlgOpen.GetPathName();
	SetDlgItemText(IDC_EDIT_REGCODE,fileName);	
}

/*
CString CRegPage::LmToHText(UCHAR * pValue, UINT uLen) //����ת16����
{
	char * pBuf = new char[2*uLen+1];
	memset(pBuf,0,2*uLen+1);
	for(unsigned i=0;i<uLen;i++)
	{
		sprintf(pBuf+i*2,"%02X",pValue[i]);//02x��ʾ1��16���Ʊ�Ϊ2���ַ����ղ���
	}
	CString str;
	str.Format("%s",pBuf);
	delete pBuf;
	return str;
}

UINT CRegPage::HTextToLm(char * pValue,char * pOut) //16����ת����
{
	UINT uLen = strlen(pValue);
	char temp[3]={0};//��ʱ����
	char * lm = new char[uLen];
	memset(lm,0,uLen);
	for(UINT j=0;j<uLen;j+=2)
	{
		strncpy(temp,pValue+j,2);
		lm[j/2]=HexToTen(temp);
	}
	memcpy(pOut,lm,uLen/2);
	delete lm;
	return uLen/2;
}

int CRegPage::HexToTen(const char * pHex)//16--->10
{
    DWORD dwHexNum=0;
    for (; *pHex!=0 ; pHex++)
    {
        dwHexNum *= 16;
        if ((*pHex>='0') && (*pHex<='9'))
            dwHexNum += *pHex-'0';
        else if ((*pHex>='a') && (*pHex<='f'))
            dwHexNum += *pHex-'a'+10;
        else if ((*pHex>='A') && (*pHex<='F'))
            dwHexNum += *pHex-'A'+10;
        else
            dwHexNum = -1;
    }
	return dwHexNum;
}

*/
//�û�ע�����
//1.�û���ע��������Ϣ�ø��˰�֤�鹫Կ����,Ȼ���ϴ�
//2.ע����ø��˰�˽Կ����,ͨ����Ϣ�ֱ�����ҵ�����˰�˽Կ����ע���,Ȼ���ø���֤��˽Կ����,�����û�
//3.�û���ע����֤�ļ��ø��˹�Կ����,Ȼ����֤ͨ���û���,������,ע�����֤

//�����ʼ�

BOOL CRegPage::SendMail()
{
	HMODULE hMod = LoadLibrary("MAPI32.DLL");
	
	if (hMod == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
		return FALSE;
	}
	
	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = GetProcAddress(hMod, "MAPISendMail");
	
	if (lpfnSendMail == NULL)
	{
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
        FreeLibrary(hMod);
		return FALSE;
	}
	
	ASSERT(lpfnSendMail != NULL);

	//�ռ��˽ṹ��Ϣ
    MapiRecipDesc recip;
    memset(&recip,0,sizeof(MapiRecipDesc));
    recip.lpszAddress = "minicareg.yahoo.com.cn";
    recip.ulRecipClass = MAPI_TO;

	//������Ϣ
	TCHAR szPath[_MAX_PATH] = "c:\\MiniCA.mreq";
	TCHAR szTitle[_MAX_PATH] = "MiniCA.mreq";
	MapiFileDesc fileDesc;
	memset(&fileDesc, 0, sizeof(fileDesc));
	fileDesc.nPosition = (ULONG)-1;
	fileDesc.lpszPathName = szPath;
	fileDesc.lpszFileName = szTitle;
	
	//�ʼ��ṹ��Ϣ
    MapiMessage message;
    memset(&message, 0, sizeof(message));
    message.nFileCount     = 1;                         //�ļ�����
    message.lpFiles        = &fileDesc;                 //�ļ���Ϣ
    message.nRecipCount    = 1;                         //�ռ��˸���
    message.lpRecips       = &recip;                    //�ռ���
    message.lpszSubject    = "MiniCA System ע��";      //����
	message.lpszNoteText   = "MiniCA:���,��Ҫע��Ϊ����û�,����˳����"; //�������� 
  
	int nError = lpfnSendMail(0, 0,
		&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);
	
	// after returning from the MAPISendMail call, the window must
	// be re-enabled and focus returned to the frame to undo the workaround
	// done before the MAPI call.
	if (nError != SUCCESS_SUCCESS &&
		nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
        FreeLibrary(hMod);
		return FALSE;
	}
	else
	{
        FreeLibrary(hMod);
		return TRUE;
	}
}

void CRegPage::OnRadio1() //����
{
	// TODO: Add your control notification handler code here
//	GetDlgItem(IDC_STATIC11)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_REGCODE)->EnableWindow(0);
	GetDlgItem(IDC_B_SELECT)->EnableWindow(0);

//	GetDlgItem(IDC_STATIC10)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(1);
	SetDlgItemText(IDC_BREG,CMiniCaApp::NormalCode("��  ��"));
}

void CRegPage::OnRadio2() 
{
	// TODO: Add your control notification handler code here
//	GetDlgItem(IDC_STATIC11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_REGCODE)->EnableWindow(1);
	GetDlgItem(IDC_B_SELECT)->EnableWindow(1);
	
//	GetDlgItem(IDC_STATIC10)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(0);
	SetDlgItemText(IDC_BREG,CMiniCaApp::NormalCode("ע  ��"));
}

void CRegPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)AfxGetMainWnd())->AddMsg(MiniCT_0008,info, type);

}

BOOL CRegPage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CRegPage::TranslateCT()	//���g�T����Ϳؼ�,���l�ؼ��ȃ���
{
	SetDlgItemText(IDC_STATIC_REG1, MiniCT_11302); 
	SetDlgItemText(IDC_STATIC_REG2, MiniCT_11303); 
	SetDlgItemText(IDC_STATIC_REG3, MiniCT_11304); 
	SetDlgItemText(IDC_STATIC_REG4, MiniCT_11305); 
	SetDlgItemText(IDC_STATIC_REG5, MiniCT_11306); 
	SetDlgItemText(IDC_STATIC_REG6, MiniCT_11307); 
	SetDlgItemText(IDC_REG_RADIO1,	MiniCT_11309); 
	SetDlgItemText(IDC_REG_RADIO2,	MiniCT_11310); 
	SetDlgItemText(IDC_B_SELECT,	MiniCT_11313); 
	SetDlgItemText(IDC_BREG,		MiniCT_11314); 
}
void CRegPage::OnBpost() //����ע���ʼ�
{
	// TODO: Add your control notification handler code here
	if(m_ReqFile.IsEmpty())
	{
		AddMsg("����������ע������", M_ERROR);
		return;
	}

	CString strOutMail;
	GetDlgItemText(IDC_EDIT_USER2,	strOutMail);
	if(strOutMail.IsEmpty())
	{
		AddMsg("�û��ظ���ַ����Ϊ��", M_ERROR);
		return;
	}

	CSmtp smtp("");
	CMIMEMessage msg;
	CBase64 auth;
	CString m_sErr;

	CString smtpServer = "smtp.126.com",
			smtpUser = "minica",//"minicareg@gmail.com",
			smtpPwd = "13613339962_",//"13613339962_hpxs_minica",
			emailBox = "minica@126.com",//"minicareg@gmail.com",
			reqFile;

	if(!smtpServer.IsEmpty())
		smtp.SetServerProperties(smtpServer, 25);
	else
		AfxMessageBox("smtp error");
	smtp.m_User = auth.Encode(smtpUser,	smtpUser.GetLength());
	smtp.m_Pass = auth.Encode(smtpPwd,	smtpPwd.GetLength());
	//	m_sErr.Format("user: %s\r\npass: %s\r\n",smtp.m_User,smtp.m_Pass);
	//	AfxMessageBox(m_sErr);

	msg.m_sFrom = _T(emailBox);
	msg.AddMultipleRecipients("minicareg@gmail.com");

	CString strInfo;
	strInfo.Format("%s", strOutMail);

	msg.m_sSubject = "MiniCA Reg";
	msg.m_sBody = strInfo;
	msg.AddMIMEPart(m_ReqFile);
	if(!smtp.Connect()) {
		AfxMessageBox(smtp.GetLastError());
		return ;
	}

	if(!smtp.Auth())
	{
		AfxMessageBox(smtp.GetLastError());
		return ;
	}

	if(!smtp.SendMessage(&msg)) {
		AfxMessageBox(smtp.GetLastError());
		smtp.Disconnect();
		return ;
	}
	smtp.Disconnect();
//	m_Files.RemoveAll();
	AfxMessageBox(_T("Message sent successfully"));

}
