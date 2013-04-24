// MiniCA.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MiniCA.h"
#include "..\html\include\htmlhelp.h"
#include "Evp.h"
#include "MiniMainDlg.h"
#define _WIN32_WINNT  0x0400
#include "wincrypt.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\GetHardSoftInfo.h"
#include ".\GenericClass\cgfiltyp.h"	//	CGCFileTypeAccess
#include <locale.h>
#include ".\Control\Splash\SplashWnd.h"
#include ".\GenericClass\Language.h"
#include "MiniCt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CMiniCaApp::m_bIsBig = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CMiniCaApp

BEGIN_MESSAGE_MAP(CMiniCaApp, CWinApp)
	//{{AFX_MSG_MAP(CMiniCaApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)//���δ˾����ȥ��������ť
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiniCaApp construction

CMiniCaApp::CMiniCaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
//	m_bServer = FALSE;
	m_bHpxs = FALSE;
	m_HelpArray.Add("ca_ext.htm");
	m_HelpArray.Add("ca_center.htm");
	m_HelpArray.Add("ca_sslclient.htm");
	m_HelpArray.Add("ca_sslserver.htm");
	m_HelpArray.Add("ca_evp.htm");
	m_HelpArray.Add("ca_raserver.htm");
	m_HelpArray.Add("ca_asn1.htm");
	m_HelpArray.Add("ca_ocspserver.htm");
	m_HelpArray.Add("ca_ocspclient.htm");
	m_HelpArray.Add("cacert_info.htm");
	m_HelpArray.Add("ca_reg.htm");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMiniCaApp object

CMiniCaApp theApp;
HINSTANCE g_hInst = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMiniCaApp initialization

int CMiniCaApp::HexToTen(const char * pHex)//16--->10
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
            -1;
    }
	return dwHexNum;
}

BOOL CMiniCaApp::CheckSolfCode(CString pathExt/*����֤�ļ�·��*/,
							   CString dllName/*dll����*/,UINT dllId/*��ԴID*/)//������֤��
{
	return TRUE;


	static HINSTANCE hLibrary;
	if ((hLibrary = LoadLibrary(dllName)) == NULL)
	{
		return FALSE;
	}
	else
	{
		//�õ�ժҪ���
		CString mdname("sha1");
		char outMsg[100]="";
		unsigned char md_value[MAX_MD_SIZE]="";
		char bufsing[MAX_MD_SIZE*2]="";
		unsigned int md_len;
		if(!CEvp::Digest(mdname.GetBuffer(0),pathExt.GetBuffer(0),0,md_value,&md_len, outMsg))
		{
			return FALSE;
		}

		/*�õ���Կ*/
		HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_VERIFY_CERT),"CERT");
		DWORD lenCert = SizeofResource(NULL, hRsrc); 
		HGLOBAL hgCert=LoadResource(NULL,hRsrc);
		LPSTR lpCert=(LPSTR)LockResource(hgCert);

		//ȡ��DLL�е���֤��
		char buf[513]={0};//����512
		int nBufferMax=513;
		if(LoadString(hLibrary,dllId,buf,nBufferMax)==0)//ȡ�ô���IDS_STR_MINICA
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
				);
			TRACE0((LPTSTR)lpMsgBuf);
			LocalFree( lpMsgBuf );
			return FALSE;
		}

		char lm[257]={0};//��������
		char temp[3]={0};//��ʱ����

		//regcode ����16�������� ����B5C3 D6F8->��Ӧ2�����뺺��
		//ȡ��B5ת����10����,���������һ��λ��
		for(UINT j=0;j<strlen(buf);j+=2)
		{
			strncpy(temp,buf+j,2);
			lm[j/2]=HexToTen(temp);
		}
		if(CEvp::VerifySign(lpCert,lenCert,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//�ڴ�����
			lm/*ǩ�����,ע���->ת��*/,outMsg))
		{
			return TRUE;
		}
		else
			return FALSE;
	}
}

BOOL CMiniCaApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	//	if(IsReg() == 1)
	//		m_bServer = FALSE;
	//	else if(IsReg() == 2)
	//	m_bServer = TRUE;//�Ƿ񿪷�����������

	
	// get full file path to program executable file
//	GetModuleFileName(NULL,sMiniCA.GetBufferSetLength (MAX_PATH+1),MAX_PATH);
//	sMiniCA.ReleaseBuffer();
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	TCHAR	szProgPath[MAX_PATH * 2];
	::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath)/sizeof(TCHAR));

	CString sMiniCA;
	sMiniCA.Format("%s", szProgPath);

	
	//�õ���ִ���ļ�·��
	int nPos;
	nPos = sMiniCA.ReverseFind ('\\');
	m_AppPath = sMiniCA.Left (nPos);
	
	CharSetIsBig();

	CString strCmdLine;
	strCmdLine.Format("%s", m_lpCmdLine);

	if(strCmdLine.Find("I love hpxs") != -1)
	{
		CSplashWnd::EnableSplashScreen(0);	//splash 3 - 2
		m_bHpxs = TRUE;
		//		m_bServer = TRUE;
	}
	else
	{
		CSplashWnd::EnableSplashScreen(1);	//splash 3 - 2

		if(!CheckSolfCode(sMiniCA,"CA.DLL"))
		{
			if(IsBig())//����
			{
				MessageBox(NULL, Gb2Big5("MiniCA ����֤ʧ��"),"MiniCA System",MB_ICONSTOP);
			}
			else 
				MessageBox(NULL, NormalCode("MiniCA ����֤ʧ��"),"MiniCA System",MB_ICONSTOP);
			return FALSE;
		}
		
	}

	//�õ������в���,�ж��Ƿ�WINEVP
	CString strEvpFile;
	strEvpFile.Format("%s", strCmdLine);
	//�������Ǵ�""��,��Ҫȥ��""
	strEvpFile.Replace("\"", " ");
	strEvpFile.TrimLeft();
	strEvpFile.TrimRight();
	CString ext;
	ext = strEvpFile.Right(3);

// 	if(ext.CompareNoCase("Evp") == 0)
// 	{
// 		CSplashWnd::ShowSplashScreen(3000, "JPG", "IDR_SPLASHEVP", NULL);	//splash 3 - 1
// 	}
// 	else
// 	{
// 		CSplashWnd::ShowSplashScreen(3000, "JPG", "IDR_SPLASHCA", NULL);	//splash 3 - 1
// 	}
	
	
	if (!AfxOleInit())///��ʼ��COM��,MFC �������ջ�COM��      �����з�֧��
	{
		AfxMessageBox(NormalCode("��ʼ��ADOʧ��"));
		return FALSE;
	}
	

	::AfxInitRichEdit();

//	AfxEnableControlContainer(); //SimpleBrowser �ؼ�ר��


	//��ʼ��OPENSSL
	SSL_load_error_strings();		// leak  ERR_free_strings()
	OpenSSL_add_all_algorithms();	//leak EVP_cleanup(void);



	//�����ļ�������ʼ - ע���ɾ��ֻ�ܿ�ж�س���
	CString csTempText;

	CGCFileTypeAccess TheFTA;
	TheFTA.SetExtension("evp");

	// just pass file path in quotes on command line
	csTempText  = szProgPath;
	csTempText += " \"%1\"";
	TheFTA.SetShellOpenCommand(csTempText);
	TheFTA.SetDocumentShellOpenCommand(csTempText);

	TheFTA.SetDocumentClassName("evpfile");

	// use first icon in program
	csTempText  = szProgPath;
	csTempText += ",0";
	TheFTA.SetDocumentDefaultIcon(csTempText);

	TheFTA.RegSetAllInfo();

	//���ù�������
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
								/*	CMiniCaSheet dlg("MiniCA System V2.0 δע��");
								dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW  |PSH_USEHICON |PSH_HASHELP ;// |PSH_WIZARD
								//	dlg.m_psh.dwFlags &= ~PSH_HASHELP;//�Ƴ�����������ť,����ҳ��ҲҪȫ��ȥ��
								dlg.m_psh.nStartPage = 0;
								//	dlg.m_psh.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
								
								  //�滻�����ļ���ʽ
								  CString strHelpFile = m_pszHelpFilePath;
								  strHelpFile.Replace(".HLP", ".chm");
								  free((void*)m_pszHelpFilePath);
								  m_pszHelpFilePath = _tcsdup(strHelpFile);
								  
	*/
	m_ImgList.Create(16,16,TRUE|ILC_COLOR24,16,1);
	
	HICON hIcon =  NULL;
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_NONE), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//0
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_ALL), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//1
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_SOME), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//2
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON15), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//3
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON14), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//4
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON18), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//5
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON4), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//6
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_MOD1), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//7
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_MOD1), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//8
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_MOD3), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//9
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_ASN1VIEW), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//10
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_BLUE), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//11
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_LIST), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//12
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_GREEN), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//13
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_RED), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//14
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_RESERT), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//15
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_ERROR), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//16
	DestroyIcon(hIcon);
	
	CMiniMainDlg dlg;
	//��װ��֤��
	LPSTR lpCert = NULL;
	DWORD lenCert=0;//��Կ����,˽Կ����
	/*�õ���Կ*/
	HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_DERCERT_ROOT),"CERT");
	lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert=LoadResource(NULL,hRsrc);
	lpCert = (LPSTR)LockResource(hgCert);

	AddRootToStore(lpCert, lenCert);
	m_pMainWnd = &dlg;
	//����������ǰһ��ʵ���򲻼�����ݿ�
	//	if ((_stricmp(m_lpCmdLine,"reset")!=0))//����
	{
		//����Ƿ��Ѿ���,����򲻿�,������д��
		CString lpszFile = 	m_AppPath + "\\MiniCA.mdb";
		CFile file;
		if(!file.Open(lpszFile, CFile::modeRead))
		{
			HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_INI_MDB),"INI");
			DWORD lenCert = SizeofResource(NULL, hRsrc); 
			HGLOBAL hgCert=LoadResource(NULL,hRsrc);
			LPSTR lpCert=(LPSTR)LockResource(hgCert);
			
			if(file.Open(lpszFile,CFile::modeCreate|CFile::modeWrite))	//���ļ�
			{
				file.Write(lpCert,lenCert);
				file.Close();
			}
		}
	}
	

	g_hInst = theApp.m_hInstance;

	int nResponse = dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CMiniCaApp::AddRootToStore(char * lpCert, UINT lenCert) //����der��ʽ��֤��
{
	HCERTSTORE      hLinkStoreHandle;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL bAdd = FALSE;
	char outBuf[10240] = {0};
	UINT outLen = 10240;
	char out[1024] = {0};

	//ת��֤���ʽΪDER

	if(CertFormatConver(lpCert, lenCert, "",
			outBuf,	&outLen,	"",	DER,out))
	{
		pCertContext = CertCreateCertificateContext(PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,
			(PBYTE)outBuf,	outLen);
		
		if(!pCertContext)
			return FALSE;
		
		
		hLinkStoreHandle = CertOpenSystemStore(NULL,"ROOT");//Դ
		// Add a certificate to hLinkStoreHandle store.
		bAdd = CertAddCertificateContextToStore(
			hLinkStoreHandle,             // The store handle.
			pCertContext,                  // A pointer to a Cert.
			CERT_STORE_ADD_USE_EXISTING,  // If a matching cert exists use it.
			NULL                          // Do not make any extra copy of the certificate.
			);
		CertCloseStore(hLinkStoreHandle,CERT_CLOSE_STORE_CHECK_FLAG);
	}
	return bAdd;
}

void CMiniCaApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString strHelp,strId;
	strHelp=m_pszHelpFilePath;
	strHelp+="::/html/";

//	int index=((CPropertySheet *)AfxGetMainWnd())->GetActiveIndex();
//	strHelp+=m_HelpArray[index];
//	HtmlHelp(NULL,strHelp, HH_DISPLAY_TOPIC, 0);
	//CWinApp::WinHelp(dwData, nCmd);
}

CString CMiniCaApp::GetMachineCode()//�õ�������
{

	DWORD dwCpuSerial = 0;

	CGetMachineInfo m_Info;
	CString strIdeSerial(m_Info.HardDriveSerialNumber);

	if(strIdeSerial.IsEmpty()) //û��ȡ��Ӳ�����
	{
		HKEY hKey;
		LONG rc = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
		if(rc == ERROR_SUCCESS)
		{
			DWORD cbBuffer = sizeof (DWORD);
			rc = ::RegQueryValueEx(hKey, "FeatureSet", NULL, NULL, (LPBYTE)(&dwCpuSerial), &cbBuffer);
			RegCloseKey (hKey);
		}
	}

//	GetVolumeInformation("C:\\",NULL,NULL,&strIdeSerial,NULL,NULL,NULL,NULL);//�õ�c�����

	CString strIDE;
	strIDE.Format("%X%s",dwCpuSerial * 2, strIdeSerial);
//	AfxMessageBox(strIDE);
	//���ȸ���Ӳ�����sha1
	CString mdname("sha1");
	char outMsg[100]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[1024*2]="";
	unsigned int md_len;
	UCHAR bufsign[1024]={0};
	UINT lensign=0;
//	CString strADD;//������Ϣ
//	strADD.LoadString(IDS_STR_MINICA);
//	strIDE += strADD;
	
	if(!CEvp::Digest(mdname.GetBuffer(0),strIDE.GetBuffer(0),strIDE.GetLength(),
		md_value,&md_len, outMsg))//��ϢժҪ
	{
		return "";
	}
	for(UINT i=0;i<md_len;i++)
	{
		sprintf((char *)&buf[i*2],"%02X",md_value[i]);//02x��ʾ1��16���Ʊ�Ϊ2���ַ����ղ���
	}
	CString str512(buf);
	return str512;
}

BOOL CMiniCaApp::CheckRegCode(CString username,CString regcode,BOOL type)//����ע����
{
	return TRUE;


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
		//	m_RegListB.AddMsg(outMsg,	M_ERROR);
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
		lm[j/2]=HexToTen(temp);
	}
	if(CEvp::VerifySign(lpCert,lenCert,NULL,mdname.GetBuffer(0),(char *)md_value,md_len,//�ڴ�����
		lm/*ǩ�����,ע���->ת��*/,outMsg))
	{
		return TRUE;
	}
	else
		return FALSE;
	
}

UINT CMiniCaApp::IsReg()
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
	CString User = CString(lUser); 
	CString RegCode = CString(lRegCode); 
	delete [] lUser; 
	delete [] lRegCode;
	//У���û�����ע����
	BOOL bType = FALSE;
	UINT len = RegCode.GetLength();
	if(len == 256)
		bType = TRUE;
	else if(len == 128)
		bType = FALSE;

	ret = CheckRegCode(User,RegCode,bType);
	if(!ret)
	{
		::RegCloseKey(hKEY); 
		return 0;
	}
	::RegCloseKey(hKEY); 
	return (bType)?2:1; //0 - δע�� 1 �� ���˰� 2 ����ҵ��

}
//��֤��            100    110
//������֤��        200    210 
//�ͻ���֤��        300    310
//����֤��          400    410 
//ǩ��֤��          500    510
//ע���û����Ȳ�ѯ�ⲿ֤�鲻���ڻ�ʹ����ȡ���ڲ�֤��
//δע���û�ȡ���ڲ�֤��
//���bInsideOnly == TRUE,��ֻ��ȡ�ڲ�֤��
BOOL CMiniCaApp::GetCertPair(UINT ID,  char * sCert, DWORD & dLen, CString & sPwd,
						 BOOL bInsideOnly, CColorListBox * pList)
{
	BOOL bExter = FALSE;//�ⲿ
	UINT uResource = 0;
	CString strInfo;
	CColorListBox * p_List = NULL;
	if(!pList)
	{
		p_List = &((CMiniMainDlg *)AfxGetMainWnd())->m_ListBox;
	}
	else
	{
		p_List = pList;
	}
	switch(ID) //��,������(����),�ͻ���(ǩ��)
	{
	case 100:
		uResource = IDR_ROOT_CERT;
		strInfo = "����Կ";
		break;
	case 110:
		uResource = IDR_ROOT_KEY;
		strInfo = "��˽Կ";
		break;
	case 200:
	case 400:
		strInfo = "���ܹ�Կ";
		uResource = IDR_ENC_CERT;
		break;
	case 210:
	case 410:
		strInfo = "����˽Կ";
		uResource = IDR_ENC_KEY;
		break;
	case 300:
	case 500:
		strInfo = "��֤��Կ";
		uResource = IDR_SIGN_CERT;
		break;
	case 310:
	case 510:
		strInfo = "ǩ��˽Կ";
		uResource = IDR_SIGN_KEY;
		break;
	case 600:
		strInfo = "�ļ���Կ";
		uResource = IDR_FILE_CERT;
		break;
	case 610:
		strInfo = "�ļ�˽Կ";
		uResource = IDR_FILE_KEY;
		break;
	default:
		return FALSE;
	}
	if(!bInsideOnly && IsReg())
	{
		CFile file;
		CString m_CdbPath = GetAppPath() + "\\CertDB.CDB";
		if(file.Open(m_CdbPath,CFile::modeRead))
		{
			for(;;)
			{
				stuCertDB CertDB;
				int len = file.Read(&CertDB,sizeof(stuCertDB));
				if(len==0)
					break;
				CString str;
	//			if(CertDB._bUSED)//֤����д�����Ŀ����ʹ��
				if(CertDB._uID == ID && CertDB._bUSED)//֤����д�����Ŀ����ʹ��
				{
					//sCert = CertDB._chINFO;
					memcpy(sCert, CertDB._chINFO, CertDB._uLENGTH);
					dLen = CertDB._uLENGTH;
					sPwd = CertDB._chPWD;
					bExter = TRUE;		//�Ѿ����ص���
					if(IsWindow(p_List->m_hWnd))
					{
						CString str;
				//		str.Format("�����ⲿ...",strInfo);
						str.Format("�����ⲿ֤��...",strInfo);
						p_List->AddMsg(str,M_WARING);
						break;
					}
					
				}
			}
			file.Close();
		}
	}
	if(!bExter)	//û�м��ص��ⲿ֤��
	{
		HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(uResource),"CERT");
		dLen = SizeofResource(NULL, hRsrc); 
		HGLOBAL hgCert=LoadResource(NULL,hRsrc);
		memcpy(sCert, (LPSTR)LockResource(hgCert), dLen);
		if(IsWindow(p_List->m_hWnd))
		{
			CString str;
//			str.Format("�����ڲ�%s...",strInfo);
			str.Format("�����ڲ�֤��...",strInfo);
			p_List->AddMsg(str,M_WARING);
		}
	}
	return TRUE;
}
//new �ⲿ֤��ȫ����Ϊ��֤��,Ҳ���ǿ�����5����֤����ѡ���κ�һ��
BOOL CMiniCaApp::GetRootCert(char * sCert, DWORD & dCertLen, char * sKey, DWORD & dKeyLen, CString & sPwd,
						 BOOL bInsideOnly, CColorListBox * pList)
{
	BOOL bExter = FALSE;//�ⲿ
	UINT uResource = 0;
	CString strInfo;
	CColorListBox * p_List = NULL;
	if(!pList)
	{
		p_List = &((CMiniMainDlg *)AfxGetMainWnd())->m_ListBox;
	}
	else
	{
		p_List = pList;
	}

	if(!bInsideOnly && IsReg())
	{
		stuCertDB CertDB[10];		//zui��10��֤��
		stuCertDB * pCertDB[10];	//ָ������ ��˳��ָ�������֤�� [0] -> 100, [1] -> 110 , [2] -> [200];
//		int len = sizeof(stuCertDB * [10]);
		memset(pCertDB, 0, sizeof(stuCertDB * [10]));
		int nIndex = 0;
		CFile file;
		CString m_CdbPath = GetAppPath() + "\\CertDB.CDB";
		if(file.Open(m_CdbPath, CFile::modeRead))
		{
			for(nIndex = 0; ;nIndex++)
			{
				//���ڱ���֤���������,���¿��ܹ��_�ں�,˽�_��ǰ,����˳��Ҳ��һ����100,110,200,210
				int len = file.Read(&CertDB[nIndex], sizeof(stuCertDB));
				if(len==0)
					break;
			}
			file.Close();
		}

		//˳������ָ��
		for(int i = 0; i <= nIndex; i++)
		{
			switch(CertDB[i]._uID)
			{
			case 100:
				pCertDB[0] = &CertDB[i];
				break;
			case 110:
				pCertDB[1] = &CertDB[i];
				break;
			case 200:
				pCertDB[2] = &CertDB[i];
			case 210:
				pCertDB[3] = &CertDB[i];
				break;
			case 300:
				pCertDB[4] = &CertDB[i];
				break;
			case 310:
				pCertDB[5] = &CertDB[i];
				break;
			case 400:
				pCertDB[6] = &CertDB[i];
				break;
			case 410:
				pCertDB[7] = &CertDB[i];
				break;
			case 500:
				pCertDB[8] = &CertDB[i];
				break;
			case 510:
				pCertDB[9] = &CertDB[i];
				break;
			}
		}

		for(i = 0; i < 5; i++)
		{
			int j = i*2;
			if(pCertDB[j] && pCertDB[j+1])	//�����ж��Ƿ���Ч
			{
				if(pCertDB[j]->_bUSED && pCertDB[j+1]->_bUSED)//֤����д�����Ŀ����ʹ��
				{
					//sCert = CertDB._chINFO;
					memcpy(sCert, pCertDB[j]->_chINFO, pCertDB[j]->_uLENGTH);
					dCertLen = pCertDB[j]->_uLENGTH;
					
					memcpy(sKey, pCertDB[j+1]->_chINFO, pCertDB[j+1]->_uLENGTH);
					dKeyLen = pCertDB[j+1]->_uLENGTH;
					sPwd = pCertDB[j+1]->_chPWD;
					
					bExter = TRUE;		//�Ѿ����ص���
					if(IsWindow(p_List->m_hWnd))
					{
						CString str;
						CString strInfo;

						if(i == 0)
							strInfo = MiniCT_1801;
						else if(i == 1)
							strInfo = MiniCT_1806;
						else if(i == 2)
							strInfo = MiniCT_1807;
						else if(i == 3)
							strInfo = MiniCT_1808;
						else if(i == 4)
							strInfo = MiniCT_1809;


						str.Format("�����ⲿ%s...", strInfo);
						p_List->AddMsg(str,M_WARING);
					}
					break;
				}
			}
		}	
	}

	if(!bExter)	//û�м��ص��ⲿ֤��
	{
		HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_ROOT_CERT),"CERT");
		dCertLen = SizeofResource(NULL, hRsrc); 
		HGLOBAL hgCert = LoadResource(NULL,hRsrc);
		memcpy(sCert, (LPSTR)LockResource(hgCert), dCertLen);

		hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_ROOT_KEY),"CERT");
		dKeyLen = SizeofResource(NULL, hRsrc); 
		hgCert = LoadResource(NULL,hRsrc);
		memcpy(sKey, (LPSTR)LockResource(hgCert), dKeyLen);

		if(IsWindow(p_List->m_hWnd))
		{
			CString str;
//			str.Format("�����ڲ�%s...",strInfo);
			str.Format("�����ڲ�֤��...",strInfo);
			p_List->AddMsg(str,M_WARING);
		}
	}

	return TRUE;
}

OsType CMiniCaApp::GetOsType()
{
	OsType  ShellType;
	DWORD winVer;
	OSVERSIONINFO *osvi;
	
	winVer=GetVersion();
	if(winVer<0x80000000)
	{/*NT */
		ShellType=WinNT3;
		osvi= (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if (osvi!=NULL)
		{
			memset(osvi,0,sizeof(OSVERSIONINFO));
			osvi->dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			GetVersionEx(osvi);
			if(osvi->dwMajorVersion==4L)ShellType=WinNT4;
			else if(osvi->dwMajorVersion==5L&&osvi->dwMinorVersion==0L)ShellType=Win2000;
			else if(osvi->dwMajorVersion==5L&&osvi->dwMinorVersion==1L)ShellType=WinXP;
			free(osvi);
		}
	}
	else if(LOBYTE(LOWORD(winVer))<4)
		ShellType=Win32s;
	else
	{
		ShellType=Win95;
		osvi= (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if (osvi!=NULL)
		{
			memset(osvi,0,sizeof(OSVERSIONINFO));
			osvi->dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			GetVersionEx(osvi);
			if(osvi->dwMajorVersion==4L&&osvi->dwMinorVersion==10L)ShellType=Win98;
			else if(osvi->dwMajorVersion==4L&&osvi->dwMinorVersion==90L)ShellType=WinME;
			free(osvi);
		}
	}
	return ShellType;
}

CImageList * CMiniCaApp::GetImgList()
{
	return &m_ImgList;
}

CString CMiniCaApp::GetAppPath()
{
	return m_AppPath;
}

BOOL CMiniCaApp::IsHpxs()
{
	return m_bHpxs;
}

//--------------------------------------------------------------------------- 
//��������Gb�ַ�������Big5�ַ� 
//--------------------------------------------------------------------------- 
CString CMiniCaApp::Gb2Big5(CString sGb) 
{ 
	char* pszGbt=NULL; //Gb����ķ����ַ� 
	char* pszGbs=NULL; //Gb����ļ����ַ� 
	wchar_t* wszUnicode=NULL; //Unicode������ַ� 
	char* pszBig5=NULL; //Big5������ַ� 
	CString sBig5; //���ص��ַ��� 
	int iLen=0; //��Ҫת�����ַ��� 
	
	pszGbs = sGb.GetBuffer(0); //������Ҫת�����ַ����� 
	
	//����ת�����ַ��� 
	iLen = MultiByteToWideChar (936, 0, pszGbs, -1, NULL,0) ; 
	
	//��pszGbt�����ڴ� 
	pszGbt = new char[iLen*2+1]; 
	//ת��Gb����嵽Gb�뷱�壬ʹ��API����LCMapString 
	int iSucceed = LCMapString(0x0804,LCMAP_TRADITIONAL_CHINESE, pszGbs, -1, pszGbt, iLen*2); 
	
	//��wszUnicode�����ڴ� 
	wszUnicode = new wchar_t[iLen+1]; 
	//ת��Gb�뵽Unicode�룬ʹ����API����MultiByteToWideChar 
	iSucceed = MultiByteToWideChar (936, 0, pszGbt, -1, wszUnicode,iLen); 
	
	//����ת�����ַ��� 
	iLen = WideCharToMultiByte (950, 0, (PWSTR) wszUnicode, -1, NULL,0, NULL, NULL) ; 
	//��pszBig5�����ڴ� 
	pszBig5 = new char[iLen+1]; 
	//ת��Unicode�뵽Big5�룬ʹ��API����WideCharToMultiByte 
	iSucceed = WideCharToMultiByte(950, 0, (PWSTR) wszUnicode, -1, pszBig5,iLen, NULL, NULL) ; 
	
	//����Big5���ַ� 
	sBig5 = pszBig5; 
	
	//�ͷ��ڴ� 
	delete [] wszUnicode; 
	delete [] pszGbt; 
	delete [] pszBig5; 
	
	return sBig5; 
} 

//---------------------------- 
//��������Big5�ַ�������Gb�����ַ� 
//---------------------------- 
CString CMiniCaApp::Big2Gb(CString sBig) 
{ 
   char* pszBig5=NULL; //Big5������ַ� 
   wchar_t* wszUnicode=NULL; //Unicode������ַ� 
   char* pszGbt=NULL; //Gb����ķ����ַ� 
   char* pszGbs=NULL; //Gb����ļ����ַ� 
   CString sGb; //���ص��ַ��� 
   int iLen=0; //��Ҫת�����ַ��� 
   
   pszBig5=sBig.GetBuffer(0); //������Ҫת�����ַ����� 
   
   //����ת�����ַ��� 
   iLen=MultiByteToWideChar (950, 0, pszBig5, -1, NULL,0) ; 
   //��wszUnicode�����ڴ� 
   wszUnicode=new wchar_t[iLen+1]; 
   //ת��Big5�뵽Unicode�룬ʹ����API����MultiByteToWideChar 
   MultiByteToWideChar (950, 0, pszBig5, -1, wszUnicode,iLen);    
   //����ת�����ַ��� 
   iLen=WideCharToMultiByte (936, 0, (PWSTR) wszUnicode, -1, NULL,0, NULL, NULL) ; 
   //��pszGbt�����ڴ� 
   pszGbt=new char[iLen+1]; 
   //��pszGbs�����ڴ� 
   pszGbs=new char[iLen+1]; 
   //ת��Unicode�뵽Gb�뷱�壬ʹ��API����WideCharToMultiByte 
   WideCharToMultiByte (936, 0, (PWSTR) wszUnicode, -1, pszGbt,iLen, NULL, NULL) ; 

//ת��Gb�뷱�嵽Gb����壬ʹ��API����LCMapString 
   LCMapString(0x0804,LCMAP_SIMPLIFIED_CHINESE, pszGbt, -1, pszGbs, iLen); 
   
   //����Gb������ַ� 
   sGb=pszGbs; 
   
   //�ͷ��ڴ� 
   delete [] wszUnicode; 
   delete [] pszGbt; 
   delete [] pszGbs; 
   
   return sGb; 
} 	

BOOL CMiniCaApp::CharSetIsBig()
{
	HDC hDc = GetWindowDC(NULL);
	int CharSet = GetTextCharset(hDc);
	ReleaseDC(NULL, hDc);
	m_bIsBig = (CharSet == CHINESEBIG5_CHARSET) ? TRUE : FALSE;
	return m_bIsBig;
}

BOOL CMiniCaApp::IsBig()
{
	return m_bIsBig;
}

CString CMiniCaApp::NormalCode(CString strGb)
{
	if(m_bIsBig) //���廷��
		return Gb2Big5(strGb);
	else 
		return strGb;
}

CString CMiniCaApp::LmToHText(UCHAR * pValue, UINT uLen) //����ת16����
{
	char * pBuf = new char[2*uLen+1];
	memset(pBuf, 0, 2*uLen+1);
	for(unsigned i=0; i<uLen; i++)
	{
		sprintf(pBuf+i*2,"%02X",pValue[i]);//02x��ʾ1��16���Ʊ�Ϊ2���ַ����ղ���
	}
	CString str;
	str.Format("%s",pBuf);
	delete [] pBuf;
	return str;
}

UINT CMiniCaApp::HTextToLm(char * pValue,char * pOut) //16����ת����
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
	delete [] lm;
	return uLen/2;
}

BOOL CMiniCaApp::IsXpStyle()
{
	return m_ThemeHelper.IsAppThemed();
}

/*
1�����h�ч����ĳ�spy by sby ����ʽ����...
2�����h�����x��O����help��ԃ����
3����߾��a��4096�����..���F�ڵļ���Ҫ���Էǳ�Ҫ��
4�����h�ṩ������Ո�{�C�C�ơ� 
5�����h���댦Ŀ䛙n�������ܽ��ܵĹ���...
6�����h������һ֧�_���g�[��������ļ��n�ĳ�ʽ^^*/
int CMiniCaApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
//	_CrtDumpMemoryLeaks();	// leak �ڴ�й©��� 2 - 2

	m_ImgList.DeleteImageList();

	//����ĳЩ�������ܻ�ʹ�������쳣,��Ҫ������
/*	CRYPTO_cleanup_all_ex_data();
	ERR_remove_state(0);
	CONF_modules_unload(1);
	CONF_modules_free();
	ERR_free_strings();
	EVP_cleanup();*/
	return CWinApp::ExitInstance();
}

BOOL CMiniCaApp::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
	{
        return TRUE;	//splash 3 -3 
    }	
	return CWinApp::PreTranslateMessage(pMsg);
}

BOOL WINAPI CMiniCaApp::MakeSureDirectoryPathExists(LPCTSTR pszDirPath)
{
    LPTSTR p, pszDirCopy;
    DWORD dwAttributes;

    // Make a copy of the string for editing.

    __try
    {
        pszDirCopy = (LPTSTR)_tCharAlloc(lstrlen(pszDirPath) + 1);

        if(pszDirCopy == NULL)
            return FALSE;

        lstrcpy(pszDirCopy, pszDirPath);

        p = pszDirCopy;

        //  If the second character in the path is "\", then this is a UNC
        //  path, and we should skip forward until we reach the 2nd \ in the path.

        if((*p == TEXT('\\')) && (*(p+1) == TEXT('\\')))
        {
            p++;            // Skip over the first \ in the name.
            p++;            // Skip over the second \ in the name.

            //  Skip until we hit the first "\" (\\Server\).

            while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            // Advance over it.

            if(*p)
            {
                p++;
            }

            //  Skip until we hit the second "\" (\\Server\Share\).

            while(*p && *p != TEXT('\\'))
            {
                p = CharNext(p);
            }

            // Advance over it also.

            if(*p)
            {
                p++;
            }

        }
        else if(*(p+1) == TEXT(':')) // Not a UNC.  See if it's <drive>:
        {
            p++;
            p++;

            // If it exists, skip over the root specifier

            if(*p && (*p == TEXT('\\')))
            {
                p++;
            }
        }

		while(*p)
        {
            if(*p == TEXT('\\'))
            {
                *p = TEXT('\0');
                dwAttributes = GetFileAttributes(pszDirCopy);

                // Nothing exists with this name.  Try to make the directory name and error if unable to.
                if(dwAttributes == 0xffffffff)
                {
                    if(!CreateDirectory(pszDirCopy, NULL))
                    {
                        if(GetLastError() != ERROR_ALREADY_EXISTS)
                        {
                            _tCharFree(pszDirCopy);
                            return FALSE;
                        }
                    }
                }
                else
                {
                    if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
                    {
                        // Something exists with this name, but it's not a directory... Error
                        _tCharFree(pszDirCopy);
                        return FALSE;
                    }
                }
 
                *p = TEXT('\\');
            }

            p = CharNext(p);
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // SetLastError(GetExceptionCode());
        _tCharFree(pszDirCopy);
        return FALSE;
    }

    _tCharFree(pszDirCopy);
    return TRUE;
}
