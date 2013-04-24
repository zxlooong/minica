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


	((CButton * )GetDlgItem(IDC_REG_RADIO1))->SetCheck(1); //是否注册
	((CButton * )GetDlgItem(IDC_RADIO_E))->SetCheck(1); //企业版

	
	if(((CMiniCaApp * )AfxGetApp())->IsHpxs())//后门
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
		m_toolTip.AddTool(GetDlgItem(IDC_REG_RADIO1), "生成注册请求");
		m_toolTip.AddTool(GetDlgItem(IDC_REG_RADIO2), "注册系统\r必须持有注册应答");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_REGCODE), "注册应答存储路径");
		m_toolTip.AddTool(GetDlgItem(IDC_B_SELECT), "选择注册应答文件");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_USER), "要注册的用户名");
		m_toolTip.AddTool(GetDlgItem(IDC_BREG), "生成注册请求");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_USER2), "用户邮箱\rMiniCA通过此邮箱向用户发送注册应答");
		m_toolTip.AddTool(GetDlgItem(IDC_BPOST), "发送邮件进行注册\r用户邮箱必须真实\n 否则收不到答复");

		// TODO: Use one of the following forms to add controls:
		// m_toolTip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_toolTip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CRegPage::GetMachineCode()//得到机器码
{
	return ((CMiniCaApp *)AfxGetApp())->GetMachineCode();
}

CString CRegPage::GetSelfCode(CString filePath)//计算文件验证吗
{
	//首先对文件摘要,然后用私钥签名
	CString mdname("sha1");
	char outMsg[100]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[1024*2]="";
	unsigned int md_len;
	UCHAR bufsign[1024]={0};
	UINT lensign=0;
		
	if(!CEvp::Digest(mdname.GetBuffer(0),filePath.GetBuffer(0),0,//文件
		md_value,&md_len, outMsg))//消息摘要
	{
		AddMsg(outMsg,M_ERROR);
		return "Error";
	}

	/*私钥*/
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
	
	if(!CEvp::Sign(lpKey,lenKey,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//表示内存
		(char *)bufsign,lensign,/*表示内存*/outMsg))
	{
		AddMsg(outMsg,M_ERROR);
		return "Error";
	}
	 
	for(UINT i=0;i<lensign;i++)
	{
		sprintf((char *)&buf[i*2],"%02X",bufsign[i]);//02x标示1个16进制变为2个字符，空补零
	}
	CString str(buf);
	return str;
}

BOOL CRegPage::CheckRegCode(CString username,CString regcode,BOOL type)//检验注册码
{
	//首先根据用户名sha1
	CString mdname("sha1");
	char outMsg[100]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[1024*2]="";
	unsigned int md_len;
	UCHAR bufsign[1024]={0};
	int lensign=0;
	char lm[257]={0};//保存乱码
	char temp[3]={0};//临时变量
	
	username += GetMachineCode();

	CString strADD = "父景存&&母桂珍&&雪松&&妻会平&&儿子健";//附加信息
//	strADD.LoadString(IDS_STR_MINICA);
	username += strADD;
	
	if(!CEvp::Digest(mdname.GetBuffer(0),username.GetBuffer(0),username.GetLength(),
		md_value,&md_len, outMsg))//消息摘要
	{
		//	AddMsg(outMsg,M_ERROR);
		return FALSE;
	}
	
	char * Cert = 0;
	if(type)
	{
		Cert = MAKEINTRESOURCE(IDR_REGE_CERT);//企业
	}
	else
		Cert = MAKEINTRESOURCE(IDR_REGI_CERT);//个人

	HRSRC hRsrc=FindResource(NULL,Cert,"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert=LoadResource(NULL,hRsrc);
	LPSTR lpCert=(LPSTR)LockResource(hgCert);
	
	//regcode 保存16进制数据 类似B5C3 D6F8->对应2个乱码汉字
	//取出B5转换成10进制,存入乱码的一个位置
	for(int j=0;j<regcode.GetLength();j+=2)
	{
		strncpy(temp,regcode.GetBuffer(0)+j,2);
		lm[j/2] = CMiniCaApp::HexToTen(temp);
	}
	if(CEvp::VerifySign(lpCert,lenCert,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//内存区域
		lm/*签名结果,注册表->转换*/,outMsg))
	{
		return TRUE;
	}
	else
		return FALSE;
	
}

void CRegPage::OnBgetCheck() //得到验证码
{
	// TODO: Add your control notification handler code here
	CString file;
	GetDlgItemText(IDC_EDIT_REGCODE,file);
	file.TrimLeft();
	file.TrimRight();
	if(file.IsEmpty())
	{
		AddMsg("文件名不能为空",M_ERROR);
		return;
	}
	//拷贝到注册表
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

CString CRegPage::GetRegCode(CString username,BOOL bRegE)//得到注册码
{
	//首先根据用户名sha1
	CString mdname("sha1");
	char outMsg[100]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[1024*2]="";
	unsigned int md_len;
	UCHAR bufsign[1024]={0};
	UINT lensign=0;

	CString strADD = "父景存&&母桂珍&&雪松&&妻会平&&儿子健";//附加信息
//	strADD.LoadString(IDS_STR_MINICA);
	username += strADD;

	if(!CEvp::Digest(mdname.GetBuffer(0),username.GetBuffer(0),username.GetLength(),
		md_value,&md_len, outMsg))//消息摘要
	{
		AddMsg(outMsg,M_ERROR);
		return "";
	}
	char * Key = 0;
	if(bRegE)
	{
		Key = MAKEINTRESOURCE(IDR_REGE_KEY);//企业
	}
	else
		Key = MAKEINTRESOURCE(IDR_REGI_KEY);//个人
	/*私钥*/
	HRSRC hRsrc = FindResource(NULL,Key,"CERT");
	DWORD lenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey = LoadResource(NULL,hRsrc);
	LPSTR lpKey = (LPSTR)LockResource(hgKey);

	if(!CEvp::Sign(lpKey,lenKey,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//表示内存
		(char *)bufsign,lensign,/*表示内存*/outMsg))
	{
		AddMsg(outMsg,M_ERROR);
		return "";
	}
	 
	for(UINT i=0;i<lensign;i++)
	{
		sprintf((char *)&buf[i*2],"%02X",bufsign[i]);//02x标示1个16进制变为2个字符，空补零
	}
	CString str512(buf);
	return str512;
}

void CRegPage::OnBgetReg() //生成mres文件
{
	// TODO: Add your control notification handler code here
	CString mci;
	GetDlgItemText(IDC_EDIT_REGCODE,mci);
	if(mci.IsEmpty())
	{
		AddMsg("请选择*.mreq文件",M_ERROR);
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
	/*私钥*/
	HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_REGI_KEY),"CERT");
	DWORD dlenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey=LoadResource(NULL,hRsrc);
	LPSTR Key=(LPSTR)LockResource(hgKey);
	
	RegReq regReq;
	//私钥解密注册请求
	len = sizeof(RegReq);
	if(CEvp::RSAPrivDec((BYTE *)Key,dlenKey,"",(BYTE *)strLm,uLm, 
		(BYTE *)&regReq,len ,outMsg))
	{
		//通过用户名和机器码生成序列号
		//把类别,用户名,机器码,注册码,通过私钥加密写入MCR文件中
		//用户方面 通过公钥解密 ,验证 机器吗,写入注册表
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
		
		//私钥加密
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
			strRes += MiniCT_1900;//"  生成成功"
			AddMsg(strRes);
		}

	}
	else
	{
		AddMsg(MiniCT_1901, M_ERROR); //应答文件生成错误
	}

//	SetDlgItemText(IDC_EDIT_REGCODE,);
}

void CRegPage::OnBreg() //注册
{
	// TODO: Add your control notification handler code here
	CString username,regcode,resfile;

	int iStateM = ((CButton * )GetDlgItem(IDC_REG_RADIO1))->GetCheck(); //是否生成请求
	int iStateR = ((CButton * )GetDlgItem(IDC_REG_RADIO2))->GetCheck(); //是否注册

	/*公钥*/
	HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_REGI_CERT),"CERT");
	DWORD dlenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey=LoadResource(NULL,hRsrc);
	LPSTR Key=(LPSTR)LockResource(hgKey);

	BOOL bE = TRUE;//指定为企业版本FALSE;//是否为企业版用户

	if(iStateM)//生成注册文件
	{
		GetDlgItemText(IDC_EDIT_USER,username);
		if(username.IsEmpty())
		{
			AddMsg(MiniCT_1902, M_ERROR); //请输入用户名
			return;
		}
		int iStateE = ((CButton * )GetDlgItem(IDC_RADIO_E))->GetCheck();
		int iStateI = ((CButton * )GetDlgItem(IDC_RADIO_I))->GetCheck();
/*		if(iStateE == 0 && iStateI == 0)
		{
			AddMsg("请选择一种注册方式",M_ERROR);
			return;
		}
		CString sInfo;
		if(iStateE == 1)
		{
			sInfo.Format("您确信要注册为企业版用户吗?");
			bE = TRUE;
		}
		else if(iStateI == 1)
		{
			sInfo.Format("您确信要注册为个人版用户吗?");
			bE = FALSE;
		}
*/		
		if(MessageBox(MiniCT_1903, //您确信要注册吗?
			MiniCT_1904,MB_ICONQUESTION | MB_YESNO) == IDYES ) //注册提示
		{
			//生成注册文件
			RegReq regReq;
			regReq.RegType = bE;
			strncpy(regReq.UserName,username,18);

			CString strHard = GetMachineCode();
			strncpy(regReq.Mac, strHard, 42);
//			strHard = "机器码:" + strHard;
//			AddMsg(strHard);

			//加载公要(个人版)
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
				CFile MciFile; //注册请求 *.mci
				CString strReq;
				strReq.Format("%s\\%s.mreq", ((CMiniCaApp *)AfxGetApp())->GetAppPath(), username);
				if(MciFile.Open(strReq,CFile::modeCreate|CFile::typeBinary|CFile::modeWrite))
				{
					MciFile.Write(strReg.GetBuffer(0),strReg.GetLength());
					MciFile.Close();
				}
				else
					AddMsg("保存文件失败");
				//保存注册文件,在邮寄时候检测是否有此文件
				m_ReqFile = strReq;

				strReq += MiniCT_1900;
				AddMsg(strReq);
				//发送
			//	SendMail();
			}
			return;
			
		}
		else
			return;
	}
	else if(iStateR)//校验注册文件
	{
		
		//得到注册类型，用户名，机器码，注册码
//		CString strReg = CMiniCaApp::LmToHText((UCHAR *)McR,len);
		//打开MCR文件
		GetDlgItemText(IDC_EDIT_REGCODE,username);
		if(username.IsEmpty())
		{
			AddMsg(MiniCT_1905, M_ERROR); //请选择注册应答文件
			return;
		}
		CFile McrFile; //注册文件 *.mcr
		McrFile.Open(username,CFile::typeBinary|CFile::modeRead);
		DWORD mcrLen = McrFile.GetLength();
		char * pMcr = new char[mcrLen+1];
		memset(pMcr,0,mcrLen+1);
		McrFile.Read(pMcr,mcrLen);
		McrFile.Close();

		//16进制转乱码
		//16 -> LM
		char * plm = new char[mcrLen/2+1];
		memset(plm,0,mcrLen/2+1);
		UINT uLm = CMiniCaApp::HTextToLm(pMcr,plm);
		delete [] pMcr;

		//用个人版公钥解密
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
			//校验机器码
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
					//写入注册表
					HKEY hKey = NULL; 
					
					CString strMiniCA,strUser,strRegCode;
					strMiniCA.LoadString(IDS_REG_KEY);// IDS_REG_KEY为在注册表中的子目录字符串 
					strUser.LoadString(IDS_REG_USER);
					strRegCode.LoadString(IDS_REG_CODE);
					
					if(RegCreateKey(HKEY_LOCAL_MACHINE,TEXT(strMiniCA),&hKey ) != ERROR_SUCCESS )//没有则创建
					{
						AddMsg("创建主键错误",M_ERROR);
						return;
					}
					if(RegSetValueEx(hKey,TEXT(strUser),0,REG_SZ,(LPBYTE)username.GetBuffer(0),username.GetLength()+1)!=ERROR_SUCCESS)
					{
						AddMsg("创建用户名键错误",M_ERROR);
						return;
					}
					if (RegSetValueEx(hKey,TEXT(strRegCode),0,REG_SZ,(LPBYTE)regcode.GetBuffer(0),regcode.GetLength()+1)!=ERROR_SUCCESS)
					{
						AddMsg("创建注册码键错误",M_ERROR);
						return;
					}
					
					RegCloseKey(hKey);
					
					((CMiniMainDlg*)AfxGetMainWnd())->SetRegFlag(TRUE);
					//		AfxGetMainWnd ()->SetWindowText("MiniCA V2.0 配置&&注册");
					AddMsg(MiniCT_1911); //注册成功
					if(strlen(regRes.RegCode) == 128)
					{
//123						((CMiniCaSheet*)AfxGetMainWnd())->RemovePage(this);
//123						((CMiniCaSheet*)AfxGetMainWnd())->AddPage(((CMiniCaApp *)AfxGetApp())->m_pPageSetup);
//123						UINT nCount = ((CMiniCaSheet*)AfxGetMainWnd())->GetPageCount();
//123						((CMiniCaSheet*)AfxGetMainWnd())->SetActivePage(nCount -1 );
					}
					else //注册为企业用户 重启程序
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
	HKEY hKEY;//定义有关的 hKEY, 在查询结束时要关闭。 
	CString strMiniCA,strUser,strRegCode;
	strMiniCA.LoadString(IDS_REG_KEY);// IDS_REG_KEY为在注册表中的子目录字符串 
	strUser.LoadString(IDS_REG_USER);
	strRegCode.LoadString(IDS_REG_CODE);
	
	//hKEY，第一个参数为根键名称，第二个参数表。 
	//表示要访问的键的位置，第三个参数必须为0，KEY_READ表示以查询的方式。 
	//访问注册表，hKEY则保存此函数所打开的键的句柄。 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,strMiniCA, 0, KEY_READ, &hKEY)); 
	if(ret0!=ERROR_SUCCESS)//如果无法打开hKEY，则终止程序的执行 
	{
		return FALSE;
	} 
	//查询用户名 
	DWORD Type=REG_SZ;
	DWORD UserLen=256; 
	LPBYTE lUser=new BYTE[UserLen]; 
	//hKEY为刚才RegOpenKeyEx()函数所打开的键的句柄，″RegisteredOwner″。 
	//表示要查 询的键值名，type_1表示查询数据的类型，owner_Get保存所。 
	//查询的数据，cbData_1表示预设置的数据长度。 
	long ret=::RegQueryValueEx(hKEY, strUser, NULL, &Type, lUser, &UserLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		return FALSE; 
	} 
	//查询注册吗
	DWORD RegLen=513; 
	LPBYTE lRegCode=new BYTE[RegLen]; 
	ret=::RegQueryValueEx(hKEY, strRegCode, NULL,&Type,lRegCode,&RegLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		delete [] lRegCode;
		return FALSE; 
	} 
	//将 owner_Get 和 company_Get 转换为 CString 字符串, 以便显示输出。 
	User=CString(lUser); 
	RegCode=CString(lRegCode); 
	delete [] lUser; 
	delete [] lRegCode;
	//校验用户名和注册吗
	::RegCloseKey(hKEY); 
	return TRUE;
}

BOOL CRegPage::GetRegFlag()//查看是否注册
{
	HKEY hKEY;//定义有关的 hKEY, 在查询结束时要关闭。 
	CString strMiniCA,strUser,strRegCode;
	strMiniCA.LoadString(IDS_REG_KEY);// IDS_REG_KEY为在注册表中的子目录字符串 
	strUser.LoadString(IDS_REG_USER);
	strRegCode.LoadString(IDS_REG_CODE);
	
	
	//hKEY，第一个参数为根键名称，第二个参数表。 
	//表示要访问的键的位置，第三个参数必须为0，KEY_READ表示以查询的方式。 
	//访问注册表，hKEY则保存此函数所打开的键的句柄。 
	long ret0=(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,strMiniCA, 0, KEY_READ, &hKEY)); 
	if(ret0!=ERROR_SUCCESS)//如果无法打开hKEY，则终止程序的执行 
	{
		return FALSE;
	} 
	//查询用户名 
	DWORD Type=REG_SZ;
	DWORD UserLen=256; 
	LPBYTE lUser=new BYTE[UserLen]; 
	//hKEY为刚才RegOpenKeyEx()函数所打开的键的句柄，″RegisteredOwner″。 
	//表示要查 询的键值名，type_1表示查询数据的类型，owner_Get保存所。 
	//查询的数据，cbData_1表示预设置的数据长度。 
	long ret=::RegQueryValueEx(hKEY, strUser, NULL, &Type, lUser, &UserLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		::RegCloseKey(hKEY); 
		return FALSE; 
	} 
	//查询注册吗
	DWORD RegLen=513; //长度+1
	LPBYTE lRegCode=new BYTE[RegLen]; 
	ret=::RegQueryValueEx(hKEY, strRegCode, NULL,&Type,lRegCode,&RegLen); 
	if(ret!=ERROR_SUCCESS) 
	{ 
		delete [] lUser;
		delete [] lRegCode;
		::RegCloseKey(hKEY); 
		return FALSE; 
	} 
	//将 owner_Get 和 company_Get 转换为 CString 字符串, 以便显示输出。 
	CString User=CString(lUser); 
	CString RegCode=CString(lRegCode); 
	delete [] lUser; 
	delete [] lRegCode;
	//校验用户名和注册吗
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
		strFilite.Format("MiniCA注册(*.mres;*.mreq)|*.mres;*.mreq|所有文件(*.*)|*.*||",NULL);
	else
		strFilite.Format("MiniCA注册(*.mres)|*.mres|");

	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		CMiniCaApp::NormalCode(strFilite),NULL);

	CString strTitle = MiniCT_1914;
	dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条

	if(dlgOpen.DoModal()!=IDOK) return;
	CString fileName=dlgOpen.GetPathName();
	SetDlgItemText(IDC_EDIT_REGCODE,fileName);	
}

/*
CString CRegPage::LmToHText(UCHAR * pValue, UINT uLen) //乱码转16进制
{
	char * pBuf = new char[2*uLen+1];
	memset(pBuf,0,2*uLen+1);
	for(unsigned i=0;i<uLen;i++)
	{
		sprintf(pBuf+i*2,"%02X",pValue[i]);//02x标示1个16进制变为2个字符，空补零
	}
	CString str;
	str.Format("%s",pBuf);
	delete pBuf;
	return str;
}

UINT CRegPage::HTextToLm(char * pValue,char * pOut) //16进制转乱码
{
	UINT uLen = strlen(pValue);
	char temp[3]={0};//临时变量
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
//用户注册过程
//1.用户将注册请求信息用个人版证书公钥加密,然后上传
//2.注册机用个人版私钥解密,通过信息分别用企业版或个人版私钥生成注册号,然后用个人证书私钥加密,传给用户
//3.用户将注册认证文件用个人公钥解密,然后验证通过用户名,机器码,注册号验证

//发送邮件

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

	//收件人结构信息
    MapiRecipDesc recip;
    memset(&recip,0,sizeof(MapiRecipDesc));
    recip.lpszAddress = "minicareg.yahoo.com.cn";
    recip.ulRecipClass = MAPI_TO;

	//附件信息
	TCHAR szPath[_MAX_PATH] = "c:\\MiniCA.mreq";
	TCHAR szTitle[_MAX_PATH] = "MiniCA.mreq";
	MapiFileDesc fileDesc;
	memset(&fileDesc, 0, sizeof(fileDesc));
	fileDesc.nPosition = (ULONG)-1;
	fileDesc.lpszPathName = szPath;
	fileDesc.lpszFileName = szTitle;
	
	//邮件结构信息
    MapiMessage message;
    memset(&message, 0, sizeof(message));
    message.nFileCount     = 1;                         //文件个数
    message.lpFiles        = &fileDesc;                 //文件信息
    message.nRecipCount    = 1;                         //收件人个数
    message.lpRecips       = &recip;                    //收件人
    message.lpszSubject    = "MiniCA System 注册";      //主题
	message.lpszNoteText   = "MiniCA:你好,我要注册为你的用户,工作顺利。"; //正文内容 
  
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

void CRegPage::OnRadio1() //请求
{
	// TODO: Add your control notification handler code here
//	GetDlgItem(IDC_STATIC11)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_REGCODE)->EnableWindow(0);
	GetDlgItem(IDC_B_SELECT)->EnableWindow(0);

//	GetDlgItem(IDC_STATIC10)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(1);
	SetDlgItemText(IDC_BREG,CMiniCaApp::NormalCode("生  成"));
}

void CRegPage::OnRadio2() 
{
	// TODO: Add your control notification handler code here
//	GetDlgItem(IDC_STATIC11)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_REGCODE)->EnableWindow(1);
	GetDlgItem(IDC_B_SELECT)->EnableWindow(1);
	
//	GetDlgItem(IDC_STATIC10)->EnableWindow(0);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(0);
	SetDlgItemText(IDC_BREG,CMiniCaApp::NormalCode("注  册"));
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

void CRegPage::TranslateCT()	//繙譯諸如樹型控件,列錶控件等內容
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
void CRegPage::OnBpost() //发送注册邮件
{
	// TODO: Add your control notification handler code here
	if(m_ReqFile.IsEmpty())
	{
		AddMsg("必须先生成注册请求", M_ERROR);
		return;
	}

	CString strOutMail;
	GetDlgItemText(IDC_EDIT_USER2,	strOutMail);
	if(strOutMail.IsEmpty())
	{
		AddMsg("用户回复地址不能为空", M_ERROR);
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
