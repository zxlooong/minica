// WebServerPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "WebServerPage.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef W3MFC_SSL_SUPPORT
CCriticalSection* CWebServerPage::m_pOpenSSLCritSections = NULL;
#endif

CStaticTreeCtrl * CWebServerPage::m_pWebTree = NULL;
CTreeNode * CWebServerPage::m_hWebClient = NULL;
CTreeNode * CWebServerPage::m_hWebServer = NULL;

void CMiniCaHttpServer::OnError(const CString& sError)
{
	CWebServerPage::AddMsg(sError, FALSE, FALSE);
}

IMPLEMENT_DYNCREATE(CMiniCAHttpClient, CHttpClient)

void CMiniCAHttpClient::PostLog(int nHTTPStatusCode, DWORD dwBodyLength)
{
	//Log each request to the console window using the W3C Common
	//log format.
	
	//Get the current date and time
	time_t now = time(NULL);
	tm* pNow = localtime(&now);
	
	//Get the time zone information
	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);
	
	//Format the date and time appropiately
	TCHAR sDateTime[64];
	_tcsftime(sDateTime, 64, _T("[%d/%b/%Y:%H:%M:%S"), pNow);
	
	//Display the connections to the console window
	CString sUser(m_Request.m_sUsername);
	if (sUser.IsEmpty())
		sUser = _T("-");
	
	CString strMsg;

	strMsg.Format(_T("%d.%d.%d.%d - %s %s %04d] \"%s\" %d %d\n"), 
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, 
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3, 
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4, 
		sUser, sDateTime, tzi.Bias, m_Request.m_sRequest, nHTTPStatusCode, dwBodyLength);
//	CWebServerPage::AddMsg(strMsg);
	

/*	_tprintf(_T("%d.%d.%d.%d - %s %s %04d] \"%s\" %d %d\n"), 
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, 
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3, 
		m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4, 
		sUser, sDateTime, tzi.Bias, m_Request.m_sRequest, nHTTPStatusCode, dwBodyLength);*/
}
/////////////////////////////////////////////////////////////////////////////
// CWebServerPage property page


IMPLEMENT_DYNCREATE(CWebServerPage, CPropertyPage)

CWebServerPage::CWebServerPage() : CPropertyPage(CWebServerPage::IDD)
{
	//{{AFX_DATA_INIT(CWebServerPage)
	//}}AFX_DATA_INIT
	m_hWebRoot = NULL;
	m_hWebServer = NULL;
	m_hWebClient = NULL;
}

CWebServerPage::~CWebServerPage()
{
}

void CWebServerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebServerPage)
	DDX_Control(pDX, IDC_STATIC_WEB, m_WebTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebServerPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWebServerPage)
	ON_BN_CLICKED(IDC_SERVER, OnServer)
	ON_BN_CLICKED(IDC_BSTOP, OnBstop)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BRESERT, OnBresert)
	ON_BN_CLICKED(IDC_BCLEAR, OnBclear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebServerPage message handlers
CWinThread	* WebServiceThread = NULL;		

UINT _WebServiceThread(LPVOID lparam)
{
	CWebServerPage * p=(CWebServerPage*)lparam;
	if(!p->Service())
	{
		WebServiceThread = NULL;
		p->m_WebTree.SetNodeColor(p->m_hWebRoot,RGB(255,0,0));
		p->m_WebTree.SetNodeColor(p->m_hWebClient,RGB(255,0,0));
		p->m_WebTree.SetNodeColor(p->m_hWebServer,RGB(255,0,0),TRUE);
		return 0;
	}
	else//开放 停止,重启,关闭 启动
	{
	}
	return 1;
}


void CWebServerPage::OnServer() 
{
	// TODO: Add your control notification handler code here
	if(!WebServiceThread)
		WebServiceThread = ::AfxBeginThread(_WebServiceThread,this,THREAD_PRIORITY_IDLE);
}

#ifdef W3MFC_SSL_SUPPORT
void __cdecl CWebServerPage::OpenSSLLockingCallback(int mode, int type, const char* /*file*/, int /*line*/)
{
	ASSERT(m_pOpenSSLCritSections);
	if (mode & CRYPTO_LOCK)
		m_pOpenSSLCritSections[type].Lock();
	else
		m_pOpenSSLCritSections[type].Unlock();
}
#endif

BOOL CWebServerPage::Service()
{	
	//Use an ini file in the same location as the exe
/*	TCHAR pszIni[_MAX_PATH];
	GetModuleFileName(NULL, pszIni, _MAX_PATH);
	TCHAR pszDrive[_MAX_DRIVE];
	TCHAR pszDir[_MAX_DIR];
	TCHAR pszFname[_MAX_FNAME];
	_tsplitpath(pszIni, pszDrive, pszDir, pszFname, NULL);
	_tmakepath(pszIni, pszDrive, pszDir, pszFname, _T("ini"));
//	free((void*)m_pszProfileName);
	m_pszProfileName=_tcsdup(pszIni);*/

	if(m_hWebRoot)
		m_WebTree.DeleteNode(m_hWebRoot);

	m_hWebRoot	= m_WebTree.InsertChild( HTOPNODE, _T(MiniCT_1000));				//MiniCT_1000 "MiniCA Web Server(Stop)"

	m_WebTree.SetNodeColor(m_hWebRoot,RGB(255,0,0));

	m_hWebClient = m_WebTree.InsertChild( m_hWebRoot, _T(MiniCT_1001));	//MiniCT_1001 "Client"

	m_hWebServer = m_WebTree.InsertChild( m_hWebRoot, _T(MiniCT_1002));	//MiniCT_1002 "Server"

	CMiniCaApp * pApp = (CMiniCaApp *)AfxGetApp();

	CString lpszIni = pApp->GetAppPath() + "\\WebServer.ini";
	
	pApp->m_pszProfileName = lpszIni;
	
	//setup all the virtual directories
	CHttpServerSettings settings;
	int nDirectories = pApp->GetProfileInt(_T("General"), _T("Directories"), 0);
	if (nDirectories == 0)
	{
	//	AddMsg(_T("Usage: W3MFC [Configuration Ini File Path]\n"));
		AddMsg(_T("Could not read any directory settings from the ini file"), FALSE);
		return FALSE; 
	}
	
	settings.FreeDirectoryArray();
	
	for (int i=0; i<nDirectories; i++)
	{
		//Create each directory. Client samples could instantiate a derived version
		//to customize the functionality
		CHttpDirectory* pDirectory = new CHttpDirectory;
		
		CString sSection;
		sSection.Format(_T("Dir%d"), i);
		
		CString sAlias = pApp->GetProfileString(sSection, _T("Alias"), _T(""));
		pDirectory->SetAlias(sAlias);
		
		CString sDirectory = pApp->GetProfileString(sSection, _T("Path"), _T(""));
		//查找Path是否带有":",如果有就是绝对对路径,否则就是相对路径
		if(sDirectory.FindOneOf(":") == -1) //相对路径
		{
			sDirectory = pApp->GetAppPath() + "\\" + sDirectory;
		}
		pDirectory->SetDirectory(sDirectory);
		
		CString sDefaultFile = pApp->GetProfileString(sSection, _T("DefaultFile"), _T("index.html"));
		pDirectory->SetDefaultFile(sDefaultFile);
		
		BOOL bDirectoryListing = (BOOL) pApp->GetProfileInt(sSection, _T("DirectoryListing"), FALSE);
		pDirectory->SetDirectoryListing(bDirectoryListing);
		
		BOOL bWritable = (BOOL) pApp->GetProfileInt(sSection, _T("Writable"), FALSE);
		pDirectory->SetWritable(bWritable);
		
		BOOL bScript = (BOOL) pApp->GetProfileInt(sSection, _T("Script"), FALSE);
		pDirectory->SetScript(bScript);
		
		CString sUsername = pApp->GetProfileString(sSection, _T("Username"), _T(""));
		pDirectory->SetUsername(sUsername);
		
		CString sPassword = pApp->GetProfileString(sSection, _T("Password"), _T(""));
		pDirectory->SetPassword(sPassword);
		
		CString sRealm = pApp->GetProfileString(sSection, _T("Realm"), _T(""));
		pDirectory->SetRealm(sRealm);
		
		settings.m_Directories.SetAtGrow(i, pDirectory);
	}
	
	//Do not use port 80 for this sample app. Helps avoid any possible
	//conflict with exisiting web servers which may be running
	settings.m_nPort = (unsigned short) (pApp->GetProfileInt(_T("General"), _T("Port"), 80));
	
	//Setup the runtime client class
	settings.m_pRuntimeClientClass = RUNTIME_CLASS(CMiniCAHttpClient);
	
	//Comment out the following line to not do reverse DNS lookups on client connections
	settings.m_bDNSLookup = pApp->GetProfileInt(_T("General"), _T("DNSLookup"), FALSE);
	
	//Should we bind to an IP address
	CString sAddress = pApp->GetProfileString(_T("General"), _T("BindAddress"), _T(""));
	if (!sAddress.IsEmpty())
	{
		settings.m_bBind = TRUE;
		settings.m_sBindAddress = sAddress;
	}
	
	//All the other misc settings
	settings.m_dwIdleClientTimeout = pApp->GetProfileInt(_T("General"), _T("Timeout"), settings.m_dwIdleClientTimeout);
	settings.m_sServerName = pApp->GetProfileString(_T("General"), _T("ServerName"), settings.m_sServerName);
	settings.m_sUsername = pApp->GetProfileString(_T("General"), _T("Username"), settings.m_sUsername);
	settings.m_sPassword = pApp->GetProfileString(_T("General"), _T("Password"), settings.m_sPassword);
	settings.m_nThreadPoolSize = pApp->GetProfileInt(_T("General"), _T("ThreadPoolSize"), settings.m_nThreadPoolSize);
	settings.m_dwWritableTimeout = pApp->GetProfileInt(_T("General"), _T("WritableTimeout"), settings.m_dwWritableTimeout);
	settings.m_bEnableThreadLifetime = pApp->GetProfileInt(_T("General"), _T("EnableThreadLifetime"), settings.m_bEnableThreadLifetime);
	settings.m_dwThreadLifetime = pApp->GetProfileInt(_T("General"), _T("ThreadLifetime"), settings.m_dwThreadLifetime);
	settings.m_bUseIOCPQueue = pApp->GetProfileInt(_T("General"), _T("UseIOCPQueue"), settings.m_bUseIOCPQueue);
	settings.m_bKeepAlives = (BOOL) pApp->GetProfileInt(_T("General"), _T("KeepAlives"), settings.m_bKeepAlives);
	settings.m_bAutoExpire = (BOOL) pApp->GetProfileInt(_T("General"), _T("AutoExpire"), settings.m_bAutoExpire);
	settings.m_bAllowDeleteRequest = (BOOL) pApp->GetProfileInt(_T("General"), _T("AllowDeletes"), settings.m_bAllowDeleteRequest);
#ifndef W3MFC_NO_CGI_SUPPORT
	settings.m_dwCGIResponseBufferSize = pApp->GetProfileInt(_T("General"), _T("CGIResponseBufferSize"), settings.m_dwCGIResponseBufferSize);
#endif
	settings.m_bAllowAnonymous                   = (BOOL) pApp->GetProfileInt(_T("General"), _T("AllowAnonymous"), settings.m_bAllowAnonymous);
	settings.m_bAllowBasicAuthentication         = (BOOL) pApp->GetProfileInt(_T("General"), _T("AllowBasicAuthentication"), settings.m_bAllowBasicAuthentication);
	settings.m_bAllowNTLMAuthentication          = (BOOL) pApp->GetProfileInt(_T("General"), _T("AllowNTLMAuthentication"), settings.m_bAllowNTLMAuthentication);
	settings.m_bPerformPassthroughAuthentication = (BOOL) pApp->GetProfileInt(_T("General"), _T("PerformPassthroughAuthentication"), settings.m_bPerformPassthroughAuthentication);
	
#ifdef W3MFC_SSL_SUPPORT
	settings.m_SSLProtocol             = (CHttpServerSettings::SSL_PROTOCOL) pApp->GetProfileInt(_T("SSL"), _T("Protocol"), settings.m_SSLProtocol);
	settings.m_sServerCertificateFile  = pApp->GetProfileString(_T("SSL"), _T("ServerCertificateFile"), settings.m_sServerCertificateFile);
	settings.m_sPrivateKeyFile         = pApp->GetProfileString(_T("SSL"), _T("PrivateKeyFile"), settings.m_sPrivateKeyFile);
	settings.m_bReuseSessions          = pApp->GetProfileInt(_T("SSL"), _T("ReuseSessions"), settings.m_bReuseSessions);
	settings.m_dwSSLSessionTimeout     = pApp->GetProfileInt(_T("SSL"), _T("SessionTimeout"), settings.m_dwSSLSessionTimeout);
	settings.m_dwSSLNegotiationTimeout = pApp->GetProfileInt(_T("SSL"), _T("NegotiationTimeout"), settings.m_dwSSLNegotiationTimeout);
	settings.m_sSSLRandomnessFile      = pApp->GetProfileString(_T("SSL"), _T("RandomnessFile"), settings.m_sSSLRandomnessFile);
	
	//Standard OpenSSL initialization
	SSL_load_error_strings();
	SSL_library_init();
	
	//Setup SSL to work correctly in a multithreaded environment
	if (m_pOpenSSLCritSections)
	{
		delete [] m_pOpenSSLCritSections;
		m_pOpenSSLCritSections = NULL;
	}

	ASSERT(m_pOpenSSLCritSections == NULL);
	m_pOpenSSLCritSections = new CCriticalSection[CRYPTO_num_locks()];
	if (m_pOpenSSLCritSections == NULL)
	{
		//Report the error
		AddMsg(_T("Failed to create SSL critical sections"));
		return FALSE;
	}
	CRYPTO_set_locking_callback(OpenSSLLockingCallback);
#endif
	
	//Initialize the Mime manager we are going to use with a mime map as taken from the ini file
	CIniHttpMimeManager iniMimeManager;
	int nSizeOfMap = iniMimeManager.Initialize(pApp->m_pszProfileName, _T("Mime"));
	if (nSizeOfMap == 0)
	{
		AddMsg(_T("Failed to read any MIME map settings from ini file"));
		return FALSE; 
	}
	settings.m_pMimeManager = &iniMimeManager;
	
#ifndef W3MFC_NO_ISAPI_SUPPORT
	//Initialize the ISAPI manager we are going to use with a extension map as taken from the ini file
	settings.m_bCacheISAPI                = (BOOL) pApp->GetProfileInt(_T("ISAPI"), _T("Cache"), settings.m_bCacheISAPI);
	CIniHttpISAPIManager iniISAPIManager;
	iniISAPIManager.Initialize(pApp->m_pszProfileName, _T("ISAPIMap"));
	settings.m_pISAPIManager = &iniISAPIManager;
	
	//Also initialize the ISAPI implementation
	int nISAPIHashTableSize = pApp->GetProfileInt(_T("ISAPI"), _T("HashTableSize"), 7929); 
	CHttpISAPI isapi(nISAPIHashTableSize);
	settings.m_pISAPI = &isapi;
#endif
	
	//And the CGI implementation
#ifndef W3MFC_NO_CGI_SUPPORT
	CHttpCGI cgi;
	settings.m_pCGI = &cgi;
#endif
	
	//and start it up
	AddMsg(_T("MiniCA Web server is starting...\n"), FALSE);
	CMiniCaHttpServer theWebServer;
	pTheWebServer = &theWebServer;
	if (!theWebServer.Start(&settings))
	{
		AddMsg(_T("Failed to start up the web server\n"), FALSE);
		return FALSE;
	}
	
	//Display some additional info when the web servering is starting
	CString strMsg;
	strMsg.Format(_T("Listening for incoming connections on port %d\n"), settings.m_nPort);
	AddMsg(strMsg, FALSE);
	if (settings.m_bDNSLookup)
		AddMsg(_T("Reverse DNS lookups will be performed on client requests\n"), FALSE);
	if (settings.m_bBind)
	{
		strMsg.Format(_T("Binding to local address: %s\n"), settings.m_sBindAddress);
		AddMsg(strMsg, FALSE);
	}
	strMsg.Format(_T("Client idle timeout: %d ms\n"), settings.m_dwIdleClientTimeout);
	AddMsg(strMsg, FALSE);
	strMsg.Format(_T("Client write timeout: %d ms\n"), settings.m_dwWritableTimeout);
	AddMsg(strMsg, FALSE);
	strMsg.Format(_T("Configured Server Name: %s\n"), settings.m_sServerName);
	AddMsg(strMsg, FALSE);
	strMsg.Format(_T("Thread pool size: %d\n"), settings.m_nThreadPoolSize);
	AddMsg(strMsg, FALSE);
	if (settings.m_bEnableThreadLifetime)
	{
		strMsg.Format(_T("Threads in the thread pool will be recycled every %d minute(s)\n"), settings.m_dwThreadLifetime);
		AddMsg(strMsg, FALSE);
	}

	if (settings.m_bUseIOCPQueue)
	{
		AddMsg(_T("Using IOCP Thread pool queue\n"), FALSE);
	}
	else
		AddMsg(_T("Using Directed Thread pool queue\n"), FALSE);
	if (settings.m_bKeepAlives)
		AddMsg(_T("Using Keep Alives\n"), FALSE);
	if (settings.m_bAutoExpire)
		AddMsg(_T("Reponses will be marked with an immediate \"Expires\" header\n"), FALSE);
	if (settings.m_bAllowDeleteRequest)
		AddMsg(_T("HTTP DELETE verb will be allowed\n"), FALSE);
	else
		AddMsg(_T("HTTP DELETE verb will not be allowed\n"), FALSE);
#ifndef W3MFC_NO_CGI_SUPPORT
	{
		strMsg.Format(_T("CGI Response Buffer size: %d\n"), settings.m_dwCGIResponseBufferSize);
		AddMsg(strMsg, FALSE);
	}
#else
	{
		strMsg.Format(_T("CGI support is disabled in this configuration\n"), FALSE);
		AddMsg(strMsg);
	}

#endif
	
#ifndef W3MFC_NO_ISAPI_SUPPORT
	AddMsg(_T("ISAPI support is provided in this configuration\n"), FALSE);
	if (settings.m_bCacheISAPI)
		AddMsg(_T("ISAPI dlls will be cached in memory\n"), FALSE);
	else
		AddMsg(_T("ISAPI dlls will not be cached in memory\n"), FALSE);
#else
	AddMsg(_T("ISAPI support is disabled in this configuration\n"), FALSE);
#endif
	
	if (settings.m_bAllowAnonymous)
		AddMsg(_T("Anonymous Connections allowed\n"), FALSE);
	else
		AddMsg(_T("Anonymous Connections are not allowed\n"), FALSE);
	if (settings.m_bAllowBasicAuthentication)
		AddMsg(_T("Basic Authentication allowed\n"), FALSE);
	if (settings.m_bAllowNTLMAuthentication)
		AddMsg(_T("NTLM Authentication allowed\n"), FALSE);
	if (settings.m_bPerformPassthroughAuthentication)
		AddMsg(_T("Passthrough Authentication will be allowed\n"), FALSE);
	else
		AddMsg(_T("Passthrough Authentication is not allowed\n"), FALSE);
	
#ifdef W3MFC_SSL_SUPPORT
	if (settings.m_SSLProtocol != CHttpServerSettings::SSL_NONE)
	{
		switch (settings.m_SSLProtocol)
		{
		case CHttpServerSettings::SSL_V2:
			{
				AddMsg(_T("Server will use SSL v2 protocol\n"), FALSE);
				break;
			}
		case CHttpServerSettings::SSL_V3:
			{
				AddMsg(_T("Server will use SSL v3 protocol\n"), FALSE);
				break;
			}
		case CHttpServerSettings::SSL_V2_OR_V3:
			{
				AddMsg(_T("Server will use SSL v3 protocol (downgrading to V2 if necessary)\n"), FALSE);
				break;
			}
		case CHttpServerSettings::TLS_V1:
			{
				AddMsg(_T("Server will use TLS v1 protocol\n"), FALSE);
				break;
			}
		default:
			{
				break;
			}
		}
		strMsg.Format(_T("Server certificate is %s\n"), settings.m_sServerCertificateFile);
		AddMsg(strMsg, FALSE);
		if (!settings.m_sPrivateKeyFile.IsEmpty())
		{
			strMsg.Format(_T("Private key is %s\n"), settings.m_sPrivateKeyFile);
			AddMsg(strMsg, FALSE);
		}
		if (!settings.m_sSSLRandomnessFile.IsEmpty())
		{
			strMsg.Format(_T("OpenSSL Randomness file is %s\n"), settings.m_sSSLRandomnessFile);
			AddMsg(strMsg, FALSE);
		}
		strMsg.Format(_T("SSL Negotiation timeout will be %d milliseconds\n"), settings.m_dwSSLNegotiationTimeout);
		AddMsg(strMsg, FALSE);
		if (settings.m_bReuseSessions)
		{
			AddMsg(_T("SSL Sessions can be reused\n"), FALSE);
			strMsg.Format(_T("SSL Session timeout will be %d seconds\n"), settings.m_dwSSLSessionTimeout);
			AddMsg(strMsg, FALSE);
		}
		else
			AddMsg(_T("SSL Sessions can not be reused\n"), FALSE);
	}  
#endif
	
	//Check that everything in the thread pool also started up ok
	if (!theWebServer.GetThreadPool().WaitForThreadsInitInstance())
		return FALSE;
	
	//Register the console handler to allow the program to be gracefully terminated
	
	//Wait until the server finishes
	m_WebTree.SetNodeText( m_hWebRoot, _T(MiniCT_1003));		//MiniCT_1003
	m_WebTree.SetNodeColor(m_hWebRoot,RGB(0,128,0));
	m_WebTree.SetNodeColor(m_hWebClient,RGB(0,128,0));
	m_WebTree.SetNodeColor(m_hWebServer,RGB(0,128,0),TRUE);

	GetDlgItem(IDC_BRESERT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BSTOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_SERVER)->EnableWindow(FALSE);

	theWebServer.Wait();
	AddMsg(_T("Web server has shut down...\n"), FALSE);
	return TRUE;
	
}

BOOL CWebServerPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

//	g_pWebList = &m_WebList;

	CString strMsg;
	
	// TODO: Add extra initialization here
	//Initialise Sockets
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) 
	{
		strMsg.Format(_T("Failed to initialise Winsock, GetLastError:%d\n"), ::GetLastError());
		AddMsg(strMsg);
		return FALSE;
	}
	
	//Confirm that the WinSock DLL supports 2.0 
	if (LOBYTE(wsaData.wVersion) != 2) 
	{
		strMsg.Format(_T("Failed to initialize a compatible Winsock 2 TCP/IP stack, GetLastError:%d\n"), ::GetLastError());
		AddMsg(strMsg);
		return FALSE; 
	}
	m_WebTree
		.SetTextFont( 9, FALSE, FALSE, "Arial Unicode MS" )
		.SetDefaultTextColor( RGB(0,64,255));

	m_hWebRoot	= m_WebTree.InsertChild( HTOPNODE, _T(MiniCT_1000));		//MiniCT_1000

	m_WebTree.SetNodeColor(m_hWebRoot,RGB(255,0,0));

	m_hWebClient = m_WebTree.InsertChild( m_hWebRoot, _T(MiniCT_1001));	//MiniCT_1001
	m_WebTree.SetNodeColor(m_hWebClient,RGB(255,0,0));

	m_hWebServer = m_WebTree.InsertChild( m_hWebRoot, _T(MiniCT_1002));	//MiniCT_1002
	m_WebTree.SetNodeColor(m_hWebServer,RGB(255,0,0));

	CString strIniPathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\WebServer.ini";

	CFile file;
	if(!file.Open(strIniPathName,CFile::modeRead))
	{
		/*得到配置*/
		HRSRC hRsrc = 0;
		hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_WEBSERVER_INI),"INI");

		DWORD lenCert = SizeofResource(NULL, hRsrc); 
		HGLOBAL hgCert=LoadResource(NULL,hRsrc);
		LPSTR lpCert=(LPSTR)LockResource(hgCert);
		
		file.Open(strIniPathName,CFile::modeCreate|CFile::modeWrite);	//存文件
		{
			file.Write(lpCert,lenCert);
		}
		
	}
	file.Close();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWebServerPage::AddMsg(CString strMsg, BOOL bClient, BOOL bSucceed) 
{
	if(!bClient && !bSucceed)	//错误信息
	{
		HTREENODE hTemp = m_pWebTree->InsertChild(m_hWebServer, strMsg);
		m_pWebTree->SetNodeColor(hTemp,RGB(255,0,0));
	}
	else
	{
		if(bClient)
			m_pWebTree->InsertChild(m_hWebClient, strMsg);
		else
			m_pWebTree->InsertChild(m_hWebServer, strMsg);
	}
}

void CWebServerPage::OnBstop() 
{
	// TODO: Add your control notification handler code here
	StopServer();
}

void CWebServerPage::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	OnBstop();
//	Sleep(2000);
	//Close down sockets

//	CPropertyPage::OnClose();
}

void CWebServerPage::StopServer()
{
	if(WebServiceThread)
	{
		pTheWebServer->Stop();
		m_WebTree.SetNodeText( m_hWebRoot, _T(MiniCT_1000)); 	//MiniCT_1000
		m_WebTree.SetNodeColor(m_hWebRoot,RGB(255,0,0));
		m_WebTree.SetNodeColor(m_hWebClient,RGB(255,0,0));
		m_WebTree.SetNodeColor(m_hWebServer,RGB(255,0,0),TRUE);

		WebServiceThread = NULL;

		GetDlgItem(IDC_BRESERT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BSTOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVER)->EnableWindow(TRUE);
	}
}

BOOL CWebServerPage::IsWebRun()
{
	return (WebServiceThread == NULL ? FALSE : TRUE);
}

void CWebServerPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
/*#ifdef W3MFC_SSL_SUPPORT
  //Clean up the SSL critical sections
  if (m_pOpenSSLCritSections)
  {
    delete [] m_pOpenSSLCritSections;
    m_pOpenSSLCritSections = NULL;
  }
#endif*/
	WSACleanup();
	
}

void CWebServerPage::OnBresert() //重启服务
{
	// TODO: Add your control notification handler code here
	StopServer();
	OnServer();	
}

void CWebServerPage::OnBclear() //信息清除
{
	// TODO: Add your control notification handler code here
	if(m_hWebClient)
		m_WebTree.DeleteNode(m_hWebClient);
	if(m_hWebServer)
		m_WebTree.DeleteNode(m_hWebServer);
	m_hWebClient = m_WebTree.InsertChild( m_hWebRoot, _T(MiniCT_1001));	

	m_hWebServer = m_WebTree.InsertChild( m_hWebRoot, _T(MiniCT_1002));
	m_WebTree.SetNodeColor(m_hWebServer,RGB(255,0,0));
}

void CWebServerPage::TranslateCT()	//繙譯諸如樹型控件,列錶控件等內容
{
	SetDlgItemText(IDC_BCLEAR,	MiniCT_11901);
	SetDlgItemText(IDC_BSTOP,	MiniCT_11902);
	SetDlgItemText(IDC_BRESERT, MiniCT_11903);
	SetDlgItemText(IDC_SERVER,	MiniCT_11904);
}