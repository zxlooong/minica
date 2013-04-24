// MINISAPI.CPP - Implementation file for your Internet Server
//    MiniSapi Extension

#include "stdafx.h"
#include "MiniSapi.h"
#include "..\CA\ca.h"
#include <afxdisp.h>

///////////////////////////////////////////////////////////////////////
// command-parsing map

BEGIN_PARSE_MAP(CMiniSapiExtension, CHttpServer)
	// TODO: insert your ON_PARSE_COMMAND() and 
	// ON_PARSE_COMMAND_PARAMS() here to hook up your commands.
	// For example:
	ON_PARSE_COMMAND(CertReq, CMiniSapiExtension, ITS_PSTR ITS_PSTR ITS_PSTR ITS_PSTR ITS_I2 ITS_I2 ITS_I2)
	ON_PARSE_COMMAND_PARAMS("UserInfo KeyUsage EkeyUsage FriendName CertKeyLen CertValidity CertType")

	ON_PARSE_COMMAND(Query, CMiniSapiExtension, ITS_PSTR)
	ON_PARSE_COMMAND_PARAMS("QueryPwd")

	ON_PARSE_COMMAND(Default, CMiniSapiExtension, ITS_EMPTY)
	DEFAULT_PARSE_COMMAND(Default, CMiniSapiExtension)
END_PARSE_MAP(CMiniSapiExtension)


///////////////////////////////////////////////////////////////////////
// The one and only CMiniSapiExtension object

CMiniSapiExtension theExtension;


///////////////////////////////////////////////////////////////////////
// CMiniSapiExtension implementation

CMiniSapiExtension::CMiniSapiExtension()
{
	m_Title = "MiniCA 在线证书发放系统";
}

CMiniSapiExtension::~CMiniSapiExtension()
{
}

/*
ISA（Internet Server Application）也可称为ISAPI DLL，
其功能和CGI程序的功能直接相对应，使用方法和CGI也类似，
由客户端在URL中指定其名称而激活。
例如下面的请求将调用服务器的虚拟可执行目录Scripts下的function.dll
（ISAPI DLL必须放在服务器的虚拟可执行目录下）： 
http://www.abc.com/Scripts/function.dll?

ISA和服务器之间的接口主要有两个：
GetExtentionVersion( )和HttpExtentionProc( )。
任何ISA都必须在其PE文件头的引出表中定义这两个引出函数，
以供Web服务器在适当的时候调用。 

1、当服务器刚加载ISA时，它会调用ISA提供的GetExtentionVersion( )
来获得该ISA所需要的服务器版本，并与自己的版本相比较，以保证版本兼容*/

BOOL CMiniSapiExtension::GetExtensionVersion(HSE_VERSION_INFO* pVer)
{
	// Call default implementation for initialization
/*
	typedef struct _HSE_VERSION_INFO 
	{ 
		DWORD dwExtensionVersion; //版本号 
		CHAR lpszExtensionDesc[HSE_MAX_EXT_DLL_NAME_LEN]; //关于ISA的描述字符串 
	} HSE_VERSION_INFO, *LPHSE_VERSION_INFO; 
*/
	CHttpServer::GetExtensionVersion(pVer);

	HMODULE hMiniCA = GetModuleHandle("MiniCA.exe");
	CString strMiniPath;
	if(hMiniCA)
	{
		GetModuleFileName(hMiniCA, strMiniPath.GetBuffer(MAX_PATH), MAX_PATH);
		strMiniPath.ReleaseBuffer();
		int nPos;
		nPos = strMiniPath.ReverseFind ('\\');
		CString strMdbPath = strMiniPath.Left (nPos);
		strMdbPath += "\\MiniCA.mdb";

		CString Msg;
		if(!ConnectDB(strMdbPath, Msg))//连接数据库失败
		{
			//	return FALSE;无法显示网页HTTP 500 - 内部服务器错误 
		}
	}
	else	//MiniCA外的调用
	{
		return FALSE;
	}

	//通过可执行文件路径得到数据库路径

	// Load description string
	TCHAR sz[HSE_MAX_EXT_DLL_NAME_LEN+1];
	ISAPIVERIFY(::LoadString(AfxGetResourceHandle(),
			IDS_SERVER, sz, HSE_MAX_EXT_DLL_NAME_LEN));
	_tcscpy(pVer->lpszExtensionDesc, sz);
	return TRUE;
}

BOOL CMiniSapiExtension::TerminateExtension(DWORD dwFlags)
{
	// extension is being terminated
	//TODO: Clean up any per-instance resources
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// CMiniSapiExtension command handlers

void CMiniSapiExtension::Default(CHttpServerContext* pCtxt)
{
	StartContent(pCtxt);
	SetTitle("默认页面");
	WriteTitle(pCtxt);
	*pCtxt << _T("欢迎来到数字证书自作-CERTSELF\r\n<P>");
	*pCtxt << _T(" 填写个人信息后,选择需要的证书类型就可以自作您需要的证书了.\r\n");
	EndContent(pCtxt);
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMiniSapiExtension, CHttpServer)
	//{{AFX_MSG_MAP(CMiniSapiExtension)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0



///////////////////////////////////////////////////////////////////////
// If your extension will not use MFC, you'll need this code to make
// sure the extension objects can find the resource handle for the
// module.  If you convert your extension to not be dependent on MFC,
// remove the comments arounn the following AfxGetResourceHandle()
// and DllMain() functions, as well as the g_hInstance global.

/****

static HINSTANCE g_hInstance;

HINSTANCE AFXISAPI AfxGetResourceHandle()
{
	return g_hInstance;
}

BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ulReason,
					LPVOID lpReserved)
{
	if (ulReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInst;
	}

	return TRUE;
}

****/
void CMiniSapiExtension::Query(CHttpServerContext* pCtxt, LPCSTR sQueryPwd)
{
	StartContent(pCtxt);
	SetTitle("用户查询");
	WriteTitle(pCtxt);

	if (!m_pDb.IsOpen())
	{
		*pCtxt<< "连接数据库错误,服务器发生故障";
		return;
	}
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		CString strSQL;
		strSQL.Format("Select CERTSTATE From WEBCERT Where QUERYPWD = '%s'", sQueryPwd);
		if(pRs->Open(strSQL, CADORecordset::openQuery))
		{
			if(!pRs->IsEof())//存在记录
			{
				int iState = 0;
				pRs->GetFieldValue("CERTSTATE", iState);
				switch(iState)
				{
				case 0:		//申请未作处理
					*pCtxt<< "您的申请还未被处理,请耐心等待";
					break;
				case 1:		//可以下载CSR
					*pCtxt<< "<p>您可以下载证书申请,点击<a href=\"http://down\">证书申请</a>下载</p>";
					break;
				case 2:		//证书正在处理中
					*pCtxt<< "您的申请正在处理中,请稍后查询";
					break;
				case 3:		//可以下载,公钥,CSR,PFX
					*pCtxt<< "<p>您可以下载<a href=\"http://6\">申请</a>,<a href=\"http://1\">\
						证书</a>,<a href=\"http://哦哦\">PFX包</a>,点击下载</p>";
					break;
				case 4:		//证书已经作废
					*pCtxt<< "很抱歉,您的证书已经作废";
					break;
				default:
					break;
				}
			}
			else//查询密码错误
			{
				*pCtxt<< "不存在的查询密码,请先申请证书,然后查询.";
			}
			pRs->Close();
		}
	}
	catch(CADOException & eAdo)
	{
		CString str = eAdo.GetErrorMessage();
	}
	delete pRs;


	EndContent(pCtxt);


/*	int iRAWSize = 0;
	CByteArray caImage;
	CString cImage(sQueryPwd);
	cImage.Remove('\'');

	HRSRC tSrc = ::FindResource(AfxGetResourceHandle() ,cImage,"BINARY");
	if (tSrc == NULL) return;

	iRAWSize = ::SizeofResource(AfxGetResourceHandle(),tSrc);
	caImage.SetSize(iRAWSize);

	HGLOBAL hImage;
	hImage = ::LoadResource(AfxGetResourceHandle(),tSrc);
	if (hImage == NULL) return;

	BYTE* pImage = NULL;
	pImage = (BYTE*) ::LockResource(hImage);
	if (pImage == NULL) return;

	for (int iCount = 0; iCount < iRAWSize; iCount++)
		caImage.ElementAt(iCount) = pImage[iCount];

	*pCtxt << caImage;*/
}

BOOL CMiniSapiExtension::ConnectDB(CString strMdb, CString & Msg)
{
	if(m_pDb.IsOpen())
		return TRUE;

	CString strConnection;
	strConnection.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Persist Security Info=False;\
		Jet OLEDB:Database Password=hpxs", strMdb);

	try
	{
		m_pDb.Open(strConnection);
	}
	catch(CADOException & eAdo)
	{
		CString str = eAdo.GetErrorMessage();
		return FALSE;
	}
	return TRUE;
}
//USERINFO,KEYLEN,DAY,FRINEDNAME,CERTTYPE,KEYUSAGE,KEYEUSAGE
BOOL CMiniSapiExtension::InsertDB(const char * lpszUserInfo, const char * lpszKeyUsage,
								  const char * lpszKeyEusage, const char * lpszFriendName,
								  const int iKeyLen, const int iCertValidity,
								  const short iCertType, CString & strQueryPwd, CString & strMsg)
{
	//产生查询密码,查询密码为一性保证 CoCreateGuid 或用时间time_t+ID(后者可以被用户猜到,所以选择前者)
	if (!m_pDb.IsOpen())
	{
		strMsg = "连接数据库错误";
		return FALSE;
	}
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		if(pRs->Open("WEBCERT", CADORecordset::openTable))
		{
			pRs->AddNew();
			
			GUID  Guid;
			CoCreateGuid(&Guid);
			strQueryPwd.Format("%u%d%d%d",  Guid.Data1, Guid.Data2, Guid.Data3, Guid.Data4);

			CString strUserInfo(lpszUserInfo);
			CString strFriendName(lpszFriendName);
			CString strKeyUsage(lpszKeyUsage);
			CString strEkeyUsage(lpszKeyEusage);

			pRs->SetFieldValue("QUERYPWD", strQueryPwd);
			pRs->SetFieldValue("USERINFO", strUserInfo);
			pRs->SetFieldValue("KEYLEN", iKeyLen);
			pRs->SetFieldValue("DAY", iCertValidity);
			pRs->SetFieldValue("FRINEDNAME", strFriendName);
			pRs->SetFieldValue("CERTTYPE", iCertType);
			pRs->SetFieldValue("KEYUSAGE", strKeyUsage);
			pRs->SetFieldValue("KEYEUSAGE", strEkeyUsage);
			
			COleDateTime time = COleDateTime::GetCurrentTime();
			pRs->SetFieldValue("INPUTTIME", time);

			pRs->Update();
			pRs->Close();
			
		}
	}
	catch(CADOException & eAdo)
	{
		CString str = eAdo.GetErrorMessage();
	}
	delete pRs;
	return TRUE;
}

//功能:提供给用户申请证书的功能
void CMiniSapiExtension::CertReq(CHttpServerContext* pCtxt, LPTSTR pszUserInfo, LPTSTR pszKeyUsage,
								 LPTSTR pszEkeyUsage, LPTSTR pszFriendName, short iCertKeyLen, short iCertValidity,
								 short iCertType)
{
	//首先进行输入的有效性

	//出错则跳转到错误页面


	//添加用户信息到数据库中

	CString  strQueryPwd,
			 strMsg;

	BOOL bSucceed = InsertDB(pszUserInfo, pszKeyUsage, pszEkeyUsage, pszFriendName, iCertKeyLen, iCertValidity,
								  iCertType, strQueryPwd, strMsg);
	StartContent(pCtxt);
	WriteTitle(pCtxt);
	if(bSucceed)
	{
		*pCtxt << _T("恭喜证书申请已经成功提交,请牢记下面的查询密码,它是您获取证书的钥匙哟\r\n");
		*pCtxt << _T(strQueryPwd);
	}
	else
		*pCtxt << _T(strMsg);

	EndContent(pCtxt);
	return;

	{
	
	//	AddHeader(pCtxt, _T("Content-Type: application/OCTET-STREAM\r\n"));
	//	AddHeader(pCtxt, _T("Content-Disposition: attachment ; filename = 1.pfx\r\n"));
		
	//	CString strLength;
	//	strLength.Format("Content-length: %ld\r\n", p12l);
	//	AddHeader(pCtxt, strLength);

	//	CByteArray caCert;
	//	caCert.SetSize(p12l);
	//	
	//	for (int iCount = 0; iCount < p12l; iCount++)
	//		caCert.ElementAt(iCount) = p12[iCount];


//		* pCtxt << caCert;

//		pCtxt->WriteClient((BYTE *)p12, (LPDWORD)&p12l);
/*		HANDLE m_hPhysicsFile = CreateFile( "c:\\1.pfx", GENERIC_READ|GENERIC_WRITE, 0, 
                NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, NULL );
		if(!pCtxt->TransmitFile(m_hPhysicsFile, HSE_REQ_TRANSMIT_FILE))
		{
			DWORD dError = GetLastError();
			CString str;
			str.Format("%d", dError);
			* pCtxt << str;
		}*/
	}

//	EndContent(pCtxt);
	
}


/*
ISA的真正入口是HttpExtentionProc( )，
它相当于普通C程序的main( )函数，
在这个函数中根据不同的客户请求作不同的处理。
服务器和HttpExtentionProc( )之间是通过扩展控制块
（Extention Control Block）来进行通信的，
即ECB中存放入口参数和出口参数，
包括服务器提供的几个回调函数的入口地址*/


LPCTSTR CMiniSapiExtension::GetTitle() const
{
	// TODO: Add your specialized code here and/or call the base class
	return m_Title;
}
