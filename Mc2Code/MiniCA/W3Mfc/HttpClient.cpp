/*
Module : HttpClient.cpp
Purpose: Implementation for the CHttpClient class
Created: PJN / 22-04-1999
History: None

Copyright (c) 1999 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/



//////////////// Includes ////////////////////////////////////////////

#include "stdafx.h"
#ifndef __AFXPRIV_H__
#pragma message("To avoid this message please put afxpriv.h in your PCH (normally stdafx.h)")
#include <afxpriv.h>
#endif
#include "W3Mfc.h"
#include "HttpResponseHeader.h"
#include "Base64.h"
#include "HttpClient.h"
#include "Win32Handle.h"



//////////////// Macros / Defines ////////////////////////////////////

#ifndef RT_HTML
#define RT_HTML         MAKEINTRESOURCE(23)
#endif

#ifndef SEC_E_OK
#define SEC_E_OK                         ((SECURITY_STATUS)0x0000)
#endif

#ifndef SEC_I_CONTINUE_NEEDED
#define SEC_I_CONTINUE_NEEDED            ((SECURITY_STATUS)0x1012)
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

#ifndef HSE_REQ_CLOSE_CONNECTION
#define HSE_REQ_CLOSE_CONNECTION                 (HSE_REQ_END_RESERVED+17)
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//The local variable which handle the function pointers

_W3MFC_DATA _W3MFCData;



//////////////// Implementation //////////////////////////////////////

_W3MFC_DATA::_W3MFC_DATA()
{
  //Initialize the function pointers to sane defaults
  m_lpfnTransmitFile = NULL;
  m_lpfnTransmitPackets = NULL;

  //Only use the TransmitFile and TransmitPacket API's if we are running on 2000/XP/.NET Server
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL bNT = (GetVersionEx(&osvi) && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
  if (bNT && (osvi.dwMajorVersion > 4))
  {
    m_hMSWsock = LoadLibrary(_T("MSWSOCK.DLL"));
    if (m_hMSWsock)
    {
      m_lpfnTransmitFile = (LPTRANSMITFILE) GetProcAddress(m_hMSWsock, "TransmitFile");
      m_lpfnTransmitPackets = (LPTRANSMITPACKETS) GetProcAddress(m_hMSWsock, "TransmitPackets");
    }
  }
}

_W3MFC_DATA::~_W3MFC_DATA()
{
  if (m_hMSWsock)
  {
    FreeLibrary(m_hMSWsock);
    m_hMSWsock = NULL;
  }
}

#ifndef W3MFC_NO_ISAPI_SUPPORT
BOOL _W3MFC_DATA::TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HSE_TF_INFO* pInfo)
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  if (pInfo->pfnHseIO)
  {
    SetLastError(ERROR_INVALID_PARAMETER); //we do not support ascynchronous notifications
    return FALSE;
  }
  if (pInfo->Offset)
  {
    SetLastError(ERROR_INVALID_PARAMETER); //we do not support partials sens thro TransmitFile because that requires overlapped IO
    return FALSE;
  }

  if (pInfo->dwFlags & HSE_IO_SEND_HEADERS)
  {
    if (m_lpfnTransmitPackets && m_lpfnTransmitFile)
    {
      TRANSMIT_PACKETS_ELEMENT tpe[2];
      tpe[0].pBuffer = (void*) pInfo->pszStatusCode;
      tpe[0].dwElFlags = TP_ELEMENT_MEMORY;
      tpe[0].cLength = strlen(pInfo->pszStatusCode);

      tpe[1].pBuffer = responseHdr.GetData(tpe[1].cLength);
      tpe[1].dwElFlags = TP_ELEMENT_MEMORY;

      //Call the TransmitPackets function
      bSuccess = m_lpfnTransmitPackets(socket, tpe, 2, 0, NULL, TF_USE_KERNEL_APC);

      //And the TransmitFile function
      if (bSuccess)
        bSuccess = m_lpfnTransmitFile(socket, pInfo->hFile, pInfo->BytesToWrite, 0, NULL, NULL, TF_USE_KERNEL_APC);
    }
    else
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  }
  else
  {
    if (m_lpfnTransmitFile)
    {
      TRANSMIT_FILE_BUFFERS tfb;
      tfb.Head = pInfo->pHead;
      tfb.HeadLength = pInfo->HeadLength;
      tfb.Tail = pInfo->pTail;
      tfb.TailLength = pInfo->TailLength;

      //Call the TransmitFile function
      bSuccess = m_lpfnTransmitFile(socket, pInfo->hFile, pInfo->BytesToWrite, 0, NULL, &tfb, TF_USE_KERNEL_APC);
    }
    else
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  }

  return bSuccess;
}
#endif

BOOL _W3MFC_DATA::TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize)
{
  ASSERT(m_lpfnTransmitFile);

  //Setup the TFB ready for the call to the "TransmitFile" function
  TRANSMIT_FILE_BUFFERS tfb;
  tfb.Head = responseHdr.GetData(tfb.HeadLength);
  tfb.Tail = NULL;
  tfb.TailLength = 0;

  //Call the TransmitFile function
  BOOL bSuccess = m_lpfnTransmitFile(socket, hFile, dwSize, 0, NULL, &tfb, TF_USE_KERNEL_APC);

  //Tidy up the heap memory we have used
  delete [] tfb.Head;

  return bSuccess;
}

BOOL _W3MFC_DATA::TransmitBuffer(CHttpSocket& socket, CHttpResponseHeader& responseHdr, BYTE* byData, DWORD dwSize)
{
  ASSERT(m_lpfnTransmitPackets);

  //Setup the TFB ready for the call to the "TransmitPackets" function
  TRANSMIT_PACKETS_ELEMENT tpe[2];
  tpe[0].pBuffer = responseHdr.GetData(tpe[0].cLength);
  tpe[0].dwElFlags = TP_ELEMENT_MEMORY;
  tpe[1].pBuffer = byData;
  tpe[1].cLength = dwSize;
  tpe[0].dwElFlags = TP_ELEMENT_MEMORY;

  //Call the TransmitPackets function
  return m_lpfnTransmitPackets(socket, tpe, 2, 0, NULL, TF_USE_KERNEL_APC);
}



IMPLEMENT_DYNCREATE(CHttpClient, CThreadPoolClient)

CHttpClient::CHttpClient()
{
  m_pServer = NULL;
#ifdef W3MFC_SSL_SUPPORT
  m_pSSLContext = NULL;
#endif
#ifndef W3MFC_NO_ISAPI_SUPPORT
  m_dwDataSentViaWriteClient = 0;
  m_nHttpStatusCodeSent = 0;
#endif
}

CHttpClient::~CHttpClient()
{
}

BOOL CHttpClient::AllowThisConnection()
{
  return TRUE;
}

#ifdef W3MFC_SSL_SUPPORT
BOOL CHttpClient::InitializeSSLConnection()
{
  SSL* pSSL = SSL_new(m_pSSLContext->operator SSL_CTX*());
  if (pSSL == NULL)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpClient::InitializeSSLConnection, Failed to create create SSL connection object"));
    ASSERT(m_pServer);
    m_pServer->OnError(sError);

    m_pServer->LogSSLErrors();

    return FALSE;
  }
  else
    m_SSL.Attach(pSSL);

  //Associate the raw socket with the SSL connection object
  if (SSL_set_fd(m_SSL, m_Socket) != 1)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpClient::InitializeSSLConnection, Failed to create create SSL connection object"));
    ASSERT(m_pServer);
    m_pServer->OnError(sError);

    m_pServer->LogSSLErrors();

    return FALSE;
  }

  return TRUE;
}

BOOL CHttpClient::DoSSLNegotiation()
{
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  BOOL bNegotiationComplete = FALSE;
  while (!bNegotiationComplete)
  {
    int nSSLAccept = SSL_accept(m_SSL);
    if (nSSLAccept != 1)
    {
      BOOL bRetry = FALSE;
      int nSSL_get_error = SSL_get_error(m_SSL, nSSLAccept);
      if (nSSL_get_error == SSL_ERROR_WANT_READ)
      {
        if (m_Socket.IsReadible(pSettings->m_dwSSLNegotiationTimeout))
          bRetry = TRUE;
      }

      if (!bRetry)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CHttpClient::DoSSLNegotiation, Failed to perform SSL handshake, SSL_accept:%d SSL_get_error:%d"), nSSLAccept, nSSL_get_error);
        m_pServer->OnError(sError);

        m_pServer->LogSSLErrors();

        return FALSE;
      }
    }
    else
      bNegotiationComplete = TRUE;
  }

  return TRUE;
}
#endif

BOOL CHttpClient::Run(const CThreadPoolRequest& request)
{
  //Validate our parameters
  ASSERT(request.m_pData);
  CHttpThreadPoolRequest* pHttpRequest = (CHttpThreadPoolRequest*) request.m_pData;

  //Hive away the parameters in member variables
  SOCKET clientSocket = pHttpRequest->m_ClientSocket.Detach();
  m_Socket.Attach(clientSocket);
  CopyMemory(&m_Request.m_ClientAddress, &pHttpRequest->m_ClientAddress, sizeof(sockaddr_in));
#ifdef W3MFC_SSL_SUPPORT
  m_pSSLContext = pHttpRequest->m_pSSLContext;
#endif

  //Call the helper function which does all of the work
  HandleClient();

#ifdef W3MFC_SSL_SUPPORT
  //Close the SSL connection
  m_SSL.Close();
#endif

  //Close down the connection
  m_Socket.Close();

  //Tidy up our heap memory after ourselves
  delete pHttpRequest;

  //Reset the request data
  m_Request = CHttpRequest();

  return TRUE;
}

int CHttpClient::ExitInstance()
{
  //Tidy up per thread SSL structures if we are using SSL
#ifdef W3MFC_SSL_SUPPORT
  ERR_remove_state(0);
#endif

  return CThreadPoolClient::ExitInstance();
}

void CHttpClient::HandleClient()
{
	//Validate our parameters
	ASSERT(m_pServer);
	CHttpServerSettings* pSettings = m_pServer->GetSettings();
	ASSERT(pSettings);
	
	//Do the reverse DNS lookup if configured to do so
	m_Request.m_sRemoteHost.Empty();
	if (pSettings->m_bDNSLookup)
	{
		HOSTENT* pHostEnt = gethostbyaddr((const char*) &m_Request.m_ClientAddress.sin_addr, sizeof(IN_ADDR), AF_INET);
		if (pHostEnt)
			m_Request.m_sRemoteHost = pHostEnt->h_name;
	}
	
	//Should we allow this client to connect
	if (!AllowThisConnection())
	{
		ReturnErrorMessage(400); //Bad Request
		return;    
	}
	
	//Create the SSL connection if required
#ifdef W3MFC_SSL_SUPPORT
	if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
	{
		if (!InitializeSSLConnection())
			return;
		
		//Do the SSL negotiation
		if (!DoSSLNegotiation())
			return;
	}
#endif
	
	//Use a Win32 event notification on the socket
	CEvent dataEvent;
	int nError = WSAEventSelect(m_Socket, dataEvent, FD_READ | FD_CLOSE);
	if (nError == SOCKET_ERROR)
	{
		DWORD dwError = ::GetLastError();
		
		//Report the error
		CString sError;
		sError.Format(_T("CHttpClient::HandleClient, Failed in call to WSAEventSelect, GetLastError:%d"), dwError);
		m_pServer->OnError(sError);
		
		return;
	}
	
	//Also create a waitable timer if we can
	CWaitableTimer dataTimer;
	if (dataTimer.Create(TRUE))
		dataTimer.SetOnceOffRelative(pSettings->m_dwIdleClientTimeout);
	
	BOOL bMoreRequests = FALSE;
#ifndef W3MFC_NO_SSPI_SUPPORT
	BOOL bImpersonatedUsingSSPI = FALSE;
#endif
	do
	{
		m_bResponseKeepAlive = FALSE;
		
#ifdef W3MFC_SSL_SUPPORT
		//Read the client request
		BOOL bReadResponse = FALSE;
		if (dataTimer)
			bReadResponse = m_Socket.ReadResponse(m_Request, pSettings->m_dwIdleClientTimeout, 4096, m_SSL, *this, dataTimer, m_StopEvent, dataEvent);
		else
			bReadResponse = m_Socket.ReadResponse(m_Request, pSettings->m_dwIdleClientTimeout, 4096, m_SSL, *this);
#else
		BOOL bReadResponse = FALSE;
		if (dataTimer)
			bReadResponse = m_Socket.ReadResponse(m_Request, pSettings->m_dwIdleClientTimeout, 4096, *this, dataTimer, m_StopEvent, dataEvent);
		else
			bReadResponse = m_Socket.ReadResponse(m_Request, pSettings->m_dwIdleClientTimeout, 4096, *this);
#endif
		if (bReadResponse)
		{
			//Parse the client request
			if (ParseRequest())
			{
				m_bResponseKeepAlive = pSettings->m_bKeepAlives && m_Request.m_bKeepAlive;
				
				//Impersonate the client credentials if authorization type is PLAINTEXT
				CW32Handle hImpersonation;
				BOOL bLoggedOn = FALSE;
				if (m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_PLAINTEXT && pSettings->m_bPerformPassthroughAuthentication)
				{
					LPTSTR pszUser = m_Request.m_sUsername.GetBuffer(m_Request.m_sUsername.GetLength());
					LPTSTR pszPassword = m_Request.m_sPassword.GetBuffer(m_Request.m_sPassword.GetLength());
					bLoggedOn = LogonUser(pszUser, NULL, pszPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hImpersonation);
					if (bLoggedOn)
					{
						ImpersonateLoggedOnUser(hImpersonation);
						m_Request.m_bLoggedOn = TRUE;
					}
					else
					{
						//Report the error
						CString sError;
						sError.Format(_T("CHttpClient::HandleClient, Failed to logon using user name: %s, GetLastError:%d"), pszUser, ::GetLastError());
						m_pServer->OnError(sError);
					}
					m_Request.m_sUsername.ReleaseBuffer();
					m_Request.m_sPassword.ReleaseBuffer();
				} 
#ifndef W3MFC_NO_SSPI_SUPPORT
				else if (m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_NTLM && !bImpersonatedUsingSSPI)
				{
					bImpersonatedUsingSSPI = TRUE;
					SECURITY_STATUS ss = ImpersonateSecurityContext(&m_Request.m_ContentHandle);
					bLoggedOn = (ss == SEC_E_OK);
					if (bLoggedOn)
					{
						//Pull out some values from the SSPI context handle and stuff them in the request object
						SecPkgContext_Names names;
						if (QueryContextAttributes(&m_Request.m_ContentHandle, SECPKG_ATTR_NAMES, &names) == SEC_E_OK)
						{
							m_Request.m_sUsername = names.sUserName;
							FreeContextBuffer(names.sUserName);
						}
						
						SecPkgContext_Authority authority;
						if (QueryContextAttributes(&m_Request.m_ContentHandle, SECPKG_ATTR_AUTHORITY, &authority) == SEC_E_OK)
						{
							m_Request.m_sAuthorityName = authority.sAuthorityName;
							FreeContextBuffer(authority.sAuthorityName);
						}
					}
					else
					{
						//Report the error
						CString sError;
						sError.Format(_T("CHttpClient::HandleClient, Failed to impersonate client credentials using SSPI: %s, Error:%d"), ss);
						m_pServer->OnError(sError);
					}
				}
#endif
				
				m_Request.m_hImpersonation = hImpersonation;
				
				if (m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_ANONYMOUS && !pSettings->m_bAllowAnonymous)
				{
					//Return an unauthorized message if some form of authentication is enabled
					if (pSettings->m_bAllowBasicAuthentication || pSettings->m_bAllowNTLMAuthentication)
						ReturnUnauthorizedMessage(m_Request.m_sURL);
					else
					{
						//Report the error
						m_pServer->OnError(_T("CHttpClient::HandleClient, Anonymous access is disabled in addition to all authentication mechanisms, All requests will Fail!!"));
						
						ReturnErrorMessage(500); //Internal server error
					}
				}
				else
				{
					if (m_Request.m_Verb == CHttpRequest::HTTP_VERB_GET || m_Request.m_Verb == CHttpRequest::HTTP_VERB_HEAD || m_Request.m_Verb == CHttpRequest::HTTP_VERB_POST)
					{
						if (!PreHandleGetPostHead()) //Allow derived classes to handle GET, HEAD or POST
						{
							BOOL bDirectory = FALSE;
							CHttpDirectory* pDirectory = NULL;
							DWORD dwMatchingURL = 0;
							DWORD dwMatchingPath = 0;
							if (MapURLToLocalFilename(m_Request.m_sURL, m_Request.m_sLocalFile, m_Request.m_sPathInfo, bDirectory, pDirectory, dwMatchingURL, dwMatchingPath))
							{
								ASSERT(pDirectory);
								pDirectory->HandleDirectory(this, bDirectory);
							}
							else
								ReturnErrorMessage(404); //Not Found
						}
					}
					else if (pSettings->m_bAllowDeleteRequest && m_Request.m_Verb == CHttpRequest::HTTP_VERB_DELETE)
					{
						//By default, only allow deletion of a file if we are using authorization
						if (m_Request.m_AuthorizationType != CHttpRequest::HTTP_AUTHORIZATION_ANONYMOUS)
						{
							CString sLocalFile;
							BOOL bDirectory = FALSE;
							CHttpDirectory* pDirectory = NULL;
							DWORD dwMatchingURL = 0;
							DWORD dwMatchingPath = 0;
							if (MapURLToLocalFilename(m_Request.m_sURL, m_Request.m_sLocalFile, m_Request.m_sPathInfo, bDirectory, pDirectory, dwMatchingURL, dwMatchingPath) && !bDirectory && pDirectory->GetWritable())
							{
								if (DeleteFile(m_Request.m_sLocalFile))
									ReturnFileDeletedOkMessage(m_Request.m_sLocalFile);
								else
								{
									if (::GetLastError() == ERROR_ACCESS_DENIED && !bDirectory)
										ReturnUnauthorizedMessage(m_Request.m_sURL);
									else 
										ReturnErrorMessage(500); //Internal server error
								}
							}
							else
								ReturnErrorMessage(404); //Not Found
						}
						else if (pSettings->m_bAllowBasicAuthentication || pSettings->m_bAllowNTLMAuthentication)
							ReturnUnauthorizedMessage(m_Request.m_sURL); //Not authorized
						else
							ReturnErrorMessage(404); //Not Found
					}
					else
						ReturnErrorMessage(501); //Not implemented
				}
				
				//Restore our usual security priviledges
				if (m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_PLAINTEXT)
				{
					//Revert to the usual security settings
					RevertToSelf();
				}
      }
      else
		  ReturnErrorMessage(400); //Bad Request
    }
	
    //Should we service another request
    bMoreRequests = m_bResponseKeepAlive;
	
    //Remember some values from the old request
    BOOL bAuthenticationCompleted = m_Request.m_bAuthenticationComplete;
    CHttpRequest::HttpAuthorization Authorization = m_Request.m_AuthorizationType;
    CString sRemoteHost = m_Request.m_sRemoteHost;
    CString sUsername = m_Request.m_sUsername;
    CString sAuthorityName = m_Request.m_sAuthorityName;
    sockaddr_in clientAddress;
    CopyMemory(&clientAddress, &m_Request.m_ClientAddress, sizeof(sockaddr_in));
	
    //Reset the request data before we (potentially) loop around or exit
    m_Request = CHttpRequest();
	
    //Reinstate the Authentication completed flag. This prevents the need for reauthentications if the connection is keep alive
    m_Request.m_bAuthenticationComplete = bAuthenticationCompleted;
	
    //Reinstate the Authentication type and the Authority type and username (if it requires keep alives, which is only NTLM currently)
    if (Authorization == CHttpRequest::HTTP_AUTHORIZATION_NTLM)
    {
		m_Request.m_AuthorizationType = Authorization;
		m_Request.m_sUsername = sUsername;
		m_Request.m_sAuthorityName = sAuthorityName;
    }
	
    //Reinstate the client connection details
    CopyMemory(&m_Request.m_ClientAddress, &clientAddress, sizeof(sockaddr_in));
    m_Request.m_sRemoteHost = sRemoteHost;
  }
  while (bMoreRequests);
  
  //Undo the SSPI impersonation
#ifndef W3MFC_NO_SSPI_SUPPORT
  if (bImpersonatedUsingSSPI)
	  RevertSecurityContext(&m_Request.m_ContentHandle);
  
  //Free up the context handle
  DeleteSecurityContext(&m_Request.m_ContentHandle);
#endif
}

BOOL CHttpClient::ParseSimpleRequestLine(const CString& sLine)
{
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //Make a local copy of the string for parsing purposes
  TCHAR* pszLine = new TCHAR[sLine.GetLength() + 1];
  _tcscpy(pszLine, sLine);

  //Assume the worst
  BOOL bSuccess = FALSE;

  //First parse out the VERB
  TCHAR seps[] = _T(" ");
  TCHAR* pszVerb = _tcstok(pszLine, seps);
  if (pszVerb)
  {
    m_Request.m_sVerb = pszVerb;
    if (_tcsicmp(pszVerb, _T("GET")) == 0)
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_GET;
    else if (_tcsicmp(pszVerb, _T("POST")) == 0)
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_POST;
    else if (_tcsicmp(pszVerb, _T("HEAD")) == 0)
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_HEAD;
    else if (_tcsicmp(pszVerb, _T("PUT")) == 0)
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_PUT;
    else if (_tcsicmp(pszVerb, _T("LINK")) == 0)
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_LINK;
    else if (_tcsicmp(pszVerb, _T("DELETE")) == 0)
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_DELETE;
    else if (_tcsicmp(pszVerb, _T("UNLINK")) == 0)
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_UNLINK;
    else
      m_Request.m_Verb = CHttpRequest::HTTP_VERB_UNKNOWN;

    //Parse out the URL
    TCHAR* pszURL = _tcstok(NULL, seps);
    if (pszURL)
    {
      m_Request.m_sRawURL = pszURL;
      m_Request.m_sURL = UTF8ToCString(URLDecode(pszURL)); //Convert any embedded escape sequences to their unencoded format 
                                                           //as well as handling UTF8 input data

      //Handle the Search path i.e everything after the ?
      int nQuestion = m_Request.m_sURL.Find(_T('?'));
      if (nQuestion != -1)
      {
        m_Request.m_sExtra = m_Request.m_sURL.Right(m_Request.m_sURL.GetLength() - nQuestion - 1);
        m_Request.m_sURL = m_Request.m_sURL.Left(nQuestion);
      }
      nQuestion = m_Request.m_sRawURL.Find(_T('?'));
      if (nQuestion != -1)
        m_Request.m_sRawExtra = m_Request.m_sRawURL.Right(m_Request.m_sRawURL.GetLength() - nQuestion - 1);

      //Parse out the HTTP version
      TCHAR* pszVersion = _tcstok(NULL, seps);
      if (pszVersion)
      {
        if (_tcsstr(pszVersion, _T("HTTP/")) == pszVersion)
        {
          TCHAR sepsVer[] = _T(".");
          TCHAR* pszMajorVersion = _tcstok(pszVersion+5, sepsVer);
          if (pszMajorVersion)
          {
            WORD wMajorVersion = (WORD) _ttoi(pszMajorVersion);
            TCHAR* pszMinorVersion = _tcstok(NULL, sepsVer);
            if (pszMinorVersion)
            {
              WORD wMinorVersion = (WORD) _ttoi(pszMinorVersion);
              m_Request.m_dwHttpVersion = MAKELONG(wMinorVersion, wMajorVersion);
              bSuccess = TRUE;
            }
          }
        }
      }
      else
      {
        //No version included in the request, so set it to HTTP v0.9
        m_Request.m_dwHttpVersion = MAKELONG(9, 0);
        bSuccess = m_Request.m_Verb == CHttpRequest::HTTP_VERB_GET; //"GET" is only allowed with HTTP v0.9
      }  
    }
  }

  //Tidy up the heap memory we have used before we return
  delete [] pszLine;

  return bSuccess;
}

BOOL CHttpClient::ParseWeekDay(char* pszToken, int& nWeekDay)
{
  BOOL bSuccess = TRUE;
  if (strcmpi(pszToken, "Sun") == 0 || strcmpi(pszToken, "Sunday") == 0)
    nWeekDay = 0;
  else if (strcmpi(pszToken, "Mon") == 0 || strcmpi(pszToken, "Monday") == 0)
    nWeekDay = 1;
  else if (strcmpi(pszToken, "Tue") == 0 || strcmpi(pszToken, "Tuesday") == 0)
    nWeekDay = 2;
  else if (strcmpi(pszToken, "Wed") == 0 || strcmpi(pszToken, "Wednesday") == 0)
    nWeekDay = 3;
  else if (strcmpi(pszToken, "Thu") == 0 || strcmpi(pszToken, "Thursday") == 0)
    nWeekDay = 4;
  else if (strcmpi(pszToken, "Fri") == 0 || strcmpi(pszToken, "Friday") == 0)
    nWeekDay = 5;
  else if (strcmpi(pszToken, "Sat") == 0 || strcmpi(pszToken, "Saturday") == 0)
    nWeekDay = 6;
  else
    bSuccess = FALSE;
  return bSuccess;
}

BOOL CHttpClient::ParseMonth(char* pszToken, int& nMonth)
{
  BOOL bSuccess = TRUE;
  if (strcmpi(pszToken, "Jan") == 0)
    nMonth = 1;
  else if (strcmpi(pszToken, "Feb") == 0)
    nMonth = 2;
  else if (strcmpi(pszToken, "Mar") == 0)
    nMonth = 3;
  else if (strcmpi(pszToken, "Apr") == 0)
    nMonth = 4;
  else if (strcmpi(pszToken, "May") == 0)
    nMonth = 5;
  else if (strcmpi(pszToken, "Jun") == 0)
    nMonth = 6;
  else if (strcmpi(pszToken, "Jul") == 0)
    nMonth = 7;
  else if (strcmpi(pszToken, "Aug") == 0)
    nMonth = 8;
  else if (strcmpi(pszToken, "Sep") == 0)
    nMonth = 9;
  else if (strcmpi(pszToken, "Oct") == 0)
    nMonth = 10;
  else if (strcmpi(pszToken, "Nov") == 0)
    nMonth = 11;
  else if (strcmpi(pszToken, "Dec") == 0)
    nMonth = 12;
  else
    bSuccess = FALSE;
  return bSuccess;
}

BOOL CHttpClient::ParseDate(const CString& sField, SYSTEMTIME& time)
{
  //This method understands RFC 1123, RFC 850 and asctime formats

	//For correct operation of the T2A macro, see MFC Tech Note 59
  USES_CONVERSION;

  BOOL bSuccess = FALSE;

  //Make a local copy of the field we are going to parse
  char* pszField = T2A((LPTSTR) (LPCTSTR) sField);

  //Http times never include a millisecond field, so just set it to zero
  time.wMilliseconds = 0;

  int nLength = strlen(pszField);
  if (nLength > 5)
  {
    if (pszField[3] == ',') //Parsing a RFC 1123 format date
    {
      //First the weekday
      char seps[] = ", :";
      char* pszToken = strtok(pszField, seps);
      if (pszToken == NULL)
        return FALSE;
      int nWeekDay;
      bSuccess = ParseWeekDay(pszToken, nWeekDay);
      if (bSuccess)
        time.wDayOfWeek = (WORD) nWeekDay;

      //Then the day of the month
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wDay = (WORD) atoi(pszToken);

      //Then the month
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      int nMonth = 0;
      bSuccess = bSuccess && ParseMonth(pszToken, nMonth);
      if (bSuccess)
        time.wMonth = (WORD) nMonth;

      //And the year
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wYear = (WORD) atoi(pszToken);

      //And the hour
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wHour = (WORD) atoi(pszToken);

      //And the minute
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wMinute = (WORD) atoi(pszToken);

      //And the second
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wSecond = (WORD) atoi(pszToken);
    }
    else if (pszField[3] == ' ') //Parsing an asctime format date
    {
      //First the weekday
      char seps[] = ", :";
      char* pszToken = strtok(pszField, seps);
      if (pszToken == NULL)
        return FALSE;
      int nWeekDay;
      bSuccess = ParseWeekDay(pszToken, nWeekDay);
      if (bSuccess)
        time.wDayOfWeek = (WORD) nWeekDay;

      //Then the month
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      int nMonth = 0;
      bSuccess = bSuccess && ParseMonth(pszToken, nMonth);
      if (bSuccess)
        time.wMonth = (WORD) nMonth;

      //Then the day of the month
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wDay = (WORD) atoi(pszToken);

      //And the hour
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wHour = (WORD) atoi(pszToken);

      //And the minute
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wMinute = (WORD) atoi(pszToken);

      //And the second
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wSecond = (WORD) atoi(pszToken);

      //And the year
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wYear = (WORD) atoi(pszToken);
    }
    else //Must be a RFC 850 format date
    {
      //First the weekday
      char seps[] = ", :-";
      char* pszToken = strtok(pszField, seps);
      if (pszToken == NULL)
        return FALSE;
      int nWeekDay;
      bSuccess = ParseWeekDay(pszToken, nWeekDay);
      if (bSuccess)
        time.wDayOfWeek = (WORD) nWeekDay;

      //Then the day of the month
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wDay = (WORD) atoi(pszToken);

      //Then the month
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      int nMonth = 0;
      bSuccess = bSuccess && ParseMonth(pszToken, nMonth);
      if (bSuccess)
        time.wMonth = (WORD) nMonth;

      //And the year (2 Digits only, so make some intelligent assumptions)
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wYear = (WORD) atoi(pszToken);
      if (time.wYear < 50)
        time.wYear += 2000;
      else if (time.wYear < 100)
        time.wYear += 1900; 

      //And the hour
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wHour = (WORD) atoi(pszToken);

      //And the minute
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wMinute = (WORD) atoi(pszToken);

      //And the second
      pszToken = strtok(NULL, seps);
      if (pszToken == NULL)
        return FALSE;
      time.wSecond = (WORD) atoi(pszToken);
    }
  }

  return bSuccess;
}

BOOL CHttpClient::ParseAuthorizationLine(const CString& sField)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //By default assume the best
  BOOL bSuccess = TRUE;

	//For correct operation of the T2A macro, see MFC Tech Note 59
  USES_CONVERSION;

  //Make a local copy of the field we are going to parse
  char* pszField = T2A((LPTSTR) (LPCTSTR) sField);

  char seps[] = " ";
  char* pszToken = strtok(pszField, seps);
  if (pszToken)
  {
    //Parse out the base64 encoded username and password if we are doing Basic authentication
    if ((strcmpi(pszToken, "Basic") == 0) && pSettings->m_bAllowBasicAuthentication)
    {
      //Move to the base64 encoded data after the text "Basic"
      pszToken = strtok(NULL, seps);
      if (pszToken)
      {
        //Decode the base64 string passed to us
        CBase64 base64;
        int nEncodedLength = strlen(pszToken);
        int nDecodedLength = base64.DecodeGetRequiredLength(nEncodedLength);
        BYTE* pszOutput = new BYTE[nDecodedLength + 1];
        int nOutputLength = 0;
        if (base64.Decode(pszToken, nEncodedLength, pszOutput, &nOutputLength))
        {
          //NULL terminate the decoded data
          pszOutput[nOutputLength] = '\0';

          CString sOutput(pszOutput);
          int nColon = sOutput.Find(_T(":"));
          if (nColon != -1)
          {
            m_Request.m_AuthorizationType = CHttpRequest::HTTP_AUTHORIZATION_PLAINTEXT;
            m_Request.m_sUsername = sOutput.Left(nColon);
            m_Request.m_sPassword = sOutput.Right(sOutput.GetLength() - nColon - 1);
          }
        }
        else
          bSuccess = FALSE;

        //Tidy up the heap memory we have been using
        delete [] pszOutput;
      }
      else
        bSuccess = FALSE;
    }
  #ifndef W3MFC_NO_SSPI_SUPPORT
    else if ((strcmpi(pszToken, "NTLM") == 0) && pSettings->m_bAllowNTLMAuthentication && !m_Request.m_bAuthenticationComplete)
    {
      //Move to the encoded data after the text "NTLM"
      pszToken = strtok(NULL, seps);
      if (pszToken)
      {
        CBase64 base64;
        int nEncodedLength = strlen(pszToken);
        int nDecodedLength = base64.DecodeGetRequiredLength(nEncodedLength);
        BYTE* pszInput = new BYTE[nDecodedLength + 1];
        int nInputLength = 0;
        if (base64.Decode(pszToken, nEncodedLength, pszInput, &nInputLength))
        {
          //Get SSPI to act on the received data

          //First setup the buffers
          SecBuffer secBufferIn[1];
          secBufferIn[0].BufferType = SECBUFFER_TOKEN;
          secBufferIn[0].cbBuffer = nInputLength;
          secBufferIn[0].pvBuffer = pszInput; 

          SecBufferDesc secbufDescriptorIn;
          secbufDescriptorIn.ulVersion = SECBUFFER_VERSION;
          secbufDescriptorIn.cBuffers = 1;
          secbufDescriptorIn.pBuffers = secBufferIn;

          SecBuffer secBufferOut[1];
          secBufferOut[0].BufferType = SECBUFFER_TOKEN;
          secBufferOut[0].cbBuffer = 0;
          secBufferOut[0].pvBuffer = NULL;

          SecBufferDesc secbufDescriptorOut;
          secbufDescriptorOut.ulVersion = SECBUFFER_VERSION;
          secbufDescriptorOut.cBuffers = 1;
          secbufDescriptorOut.pBuffers = secBufferOut;

          //Call SSPI
          ULONG nContextAttributes;
          SECURITY_STATUS ss = AcceptSecurityContext(m_pServer->GetCredentialHandle(), m_Request.m_bFirstAuthenticationRequest ? NULL : &m_Request.m_ContentHandle, 
                                                     &secbufDescriptorIn, ASC_REQ_STREAM | ASC_REQ_ALLOCATE_MEMORY, SECURITY_NETWORK_DREP, 
                                                     &m_Request.m_ContentHandle, &secbufDescriptorOut, &nContextAttributes, NULL);

          //It's no longer the first request
          m_Request.m_bFirstAuthenticationRequest = FALSE;

          //Seup the output data (if any)
          if (secBufferOut[0].cbBuffer)
          {
            nEncodedLength = base64.EncodeGetRequiredLength(secBufferOut[0].cbBuffer, BASE64_FLAG_NOCRLF);
            char* pszOutput = new char[nEncodedLength + 1];

            //Base 64 encode the data to be sent
            base64.Encode((const BYTE*) secBufferOut[0].pvBuffer, secBufferOut[0].cbBuffer, pszOutput, &nEncodedLength, BASE64_FLAG_NOCRLF);

            //Null terminate the data
            pszOutput[nEncodedLength] = '\0';

            //Set the data to return to the client
            m_Request.m_sAuthenticationResponse = pszOutput;

            //Tidy up the heap memory we have been using
            delete [] pszOutput;

            //Also free the data allocated by SSPI
            FreeContextBuffer(secBufferOut[0].pvBuffer);
          }

          //Do we need more data from the client?
          m_Request.m_bAuthenticationComplete = (ss != SEC_I_CONTINUE_NEEDED);

          //Is the authentication handshake completed
          if (ss == SEC_E_OK)
            m_Request.m_AuthorizationType = CHttpRequest::HTTP_AUTHORIZATION_NTLM;

          bSuccess = (ss == SEC_E_OK || ss == SEC_I_CONTINUE_NEEDED);
        }
        else
          bSuccess = FALSE;

        //Tidy up the heap memory we have been using
        delete [] pszInput;
      }
      else
        bSuccess = FALSE;      
    }
  #endif //W3MFC_NO_SSPI_SUPPORT
  }

  return bSuccess;
}

BOOL CHttpClient::ParseRequestLine(const CString& sCurrentLine, BOOL bFirstLine, const CString& sField, const CString& sValue)
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  if (bFirstLine)
  {
    //Handle the first line
    m_Request.m_sRequest = sCurrentLine;
    bSuccess = ParseSimpleRequestLine(sCurrentLine);
  }
  else
  {
    bSuccess = TRUE;

    //Also add the header line to the header map
    CString sKey(sField);
    sKey.MakeUpper();
    m_Request.m_HeaderMap.SetAt(sKey, sValue);

    //Handle any other request headers  
    if (sField.CompareNoCase(_T("If-Modified-Since")) == 0)
    {
      //Handle the If-Modified-Since header
      SYSTEMTIME time;
      if (ParseDate(sValue, time))
      {
        m_Request.m_bIfModifiedSincePresent = TRUE; 
        CopyMemory(&m_Request.m_IfModifiedSince, &time, sizeof(SYSTEMTIME));
      }
    }
    else if (sField.CompareNoCase(_T("Authorization")) == 0)
    {
      //Handle the Authorization header
      bSuccess = ParseAuthorizationLine(sValue);
    }
    else if (sField.CompareNoCase(_T("Content-Type")) == 0)
      m_Request.m_sContentType = sValue;
    else if (sField.CompareNoCase(_T("Content-Length")) == 0)
      m_Request.m_nContentLength = _ttoi(sValue);
    else if ((sField.CompareNoCase(_T("Connection")) == 0) && (sValue.CompareNoCase(_T("Keep-Alive")) == 0))
      m_Request.m_bKeepAlive = TRUE;
  }

  return bSuccess;
}

LPSTR CHttpClient::FindNextTerminatorFromRequest(LPSTR pszLine)
{
  LPSTR pszCurrentLine = pszLine;
  while (TRUE)
  {
    ++pszCurrentLine;
    if (pszCurrentLine >= ((LPSTR) m_Request.m_pRawEntity))
      return pszCurrentLine;
    else if (*pszCurrentLine == '\n')
      return pszCurrentLine;
  }
  
  return NULL;
}

BOOL CHttpClient::ParseRequest()
{
  //By default assume the worst 
  BOOL bSuccess = FALSE;

  //Also store a pointer to entity body.
  LPSTR pszEntityTerminator = strstr((LPSTR) m_Request.m_pRawRequest, "\r\n\r\n");
  if (pszEntityTerminator)
  {
    m_Request.m_pRawEntity = (BYTE*) (pszEntityTerminator+4);
    m_Request.m_dwRawEntitySize = m_Request.m_dwRawRequestSize - (m_Request.m_pRawEntity - m_Request.m_pRawRequest);
  }
  else
  {
    m_Request.m_pRawEntity = NULL;
    m_Request.m_dwRawEntitySize = 0;
  }

  //Process each line
  BOOL bFirstLine = TRUE;
  LPSTR pszLine = (LPSTR) m_Request.m_pRawRequest;
  LPSTR pszTerminator = strstr(pszLine, "\n");
  BOOL bMoreLines = TRUE;
  do 
  {
    CString sLine;
    if (pszTerminator)
    {
      //Form the current line
      int nCurSize = pszTerminator - pszLine;
      char* pszCurrentLine = new char[nCurSize];
      strncpy(pszCurrentLine, pszLine, nCurSize-1);
      pszCurrentLine[nCurSize-1] = '\0'; 

      //Transfer to the CString variable
      sLine = pszCurrentLine;

      //Tidy up the heap memory now that we are finished with it
      delete [] pszCurrentLine; 

      //Parse each line using the virtual ParseRequestLine method
      if (sLine.GetLength())
      {
      CString sField;
        CString sValue;
        if (!bFirstLine)
        {
          m_Socket.SplitRequestLine(sLine, sField, sValue);
          bSuccess = ParseRequestLine(sLine, FALSE, sField, sValue);
        }
        else
        {
          bSuccess = ParseRequestLine(sLine, TRUE, sField, sValue);
          bFirstLine = FALSE;
        }
      }

      //Move onto the next line
      if (pszTerminator)
      {
        pszLine = pszTerminator+1;
        if (pszLine >= (LPSTR) m_Request.m_pRawEntity)
          bMoreLines = FALSE;
        else 
          pszTerminator = FindNextTerminatorFromRequest(pszLine);
      }
    }  
    else
      bMoreLines = FALSE;
  }
  while (bMoreLines && bSuccess);

  m_Request.m_hImpersonation = m_pServer->GetImpersonationHandle();

  return bSuccess;
}

DWORD CHttpClient::ReturnErrorMessage(int nStatusCode)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //Form the body of the response
  DWORD dwBodyLength = 0;
  char* pszBody = m_pServer->LoadHTML(nStatusCode, dwBodyLength);

  if (m_Request.m_dwHttpVersion > MAKELONG(9, 0))
  {
    //Form the header of the response
    CHttpResponseHeader responseHdr;
    responseHdr.AddStatusCode(nStatusCode);
    SYSTEMTIME st;
    GetSystemTime(&st);
    responseHdr.AddDate(st);
    responseHdr.AddServer(pSettings->m_sServerName);
    responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
    if (m_bResponseKeepAlive)
      responseHdr.AddKeepAlive();
    responseHdr.AddContentLength(dwBodyLength);
    responseHdr.AddContentType(_T("text/html"));

    //Send the header and body all in one
    TransmitBuffer(m_Socket, responseHdr, (BYTE*)pszBody, dwBodyLength, pSettings->m_dwWritableTimeout);
  }
  else
  {
    //No header sent for HTTP v0.9
    //so just send the body
    try
    {
#ifdef W3MFC_SSL_SUPPORT
      m_Socket.SendWithRetry(pszBody, dwBodyLength, pSettings->m_dwWritableTimeout, m_SSL);
#else    
      m_Socket.SendWithRetry(pszBody, dwBodyLength, pSettings->m_dwWritableTimeout);
#endif
    }
    catch(CWSocketException* pEx)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpClient::ReturnErrorMessage, Failed to send to socket, Error:%d"), pEx->m_nError);
      ASSERT(m_pServer);
      m_pServer->OnError(sError);

      pEx->Delete();  
    }
  }

  //Log the information
  PostLog(nStatusCode, dwBodyLength);

  //Return the body length
  return dwBodyLength;
}

DWORD CHttpClient::ReturnRedirectMessage(const CString& sURL)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //Form the body of the response
  DWORD dwBodyLength = 0;
  char* pszBody = m_pServer->LoadHTML(302, dwBodyLength);

  //Form the header of the response
  CHttpResponseHeader responseHdr;
  responseHdr.AddStatusCode(302);
  SYSTEMTIME st;
  GetSystemTime(&st);
  responseHdr.AddDate(st);
  responseHdr.AddServer(pSettings->m_sServerName);
  responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
  responseHdr.AddLocation(sURL);
  if (m_bResponseKeepAlive)
    responseHdr.AddKeepAlive();
  responseHdr.AddContentLength(dwBodyLength);
  responseHdr.AddContentType(_T("text/html"));

  //Send the header and body all in one
  TransmitBuffer(m_Socket, responseHdr, (BYTE*)pszBody, dwBodyLength, pSettings->m_dwWritableTimeout);

  //Log the information
  PostLog(302, dwBodyLength);

  return dwBodyLength;
}

DWORD CHttpClient::ReturnUnauthorizedMessage(const CString& sRealm)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);
  ASSERT(pSettings->m_bAllowBasicAuthentication || pSettings->m_bAllowNTLMAuthentication);

  //Form the body of the response
  DWORD dwBodyLength = 0;
  char* pszBody = m_pServer->LoadHTML(401, dwBodyLength);

  //Form the header of the response
  CHttpResponseHeader responseHdr;
  responseHdr.AddStatusCode(401);
  SYSTEMTIME st;
  GetSystemTime(&st);
  responseHdr.AddDate(st);
  responseHdr.AddServer(pSettings->m_sServerName);
  responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);

  if (pSettings->m_bAllowNTLMAuthentication)
    responseHdr.AddWWWAuthenticateNTLM(m_Request.m_sAuthenticationResponse);
  if (pSettings->m_bAllowBasicAuthentication)
    responseHdr.AddWWWAuthenticateBasic(sRealm);
  if (m_bResponseKeepAlive)
    responseHdr.AddKeepAlive();
  responseHdr.AddContentLength(dwBodyLength);
  responseHdr.AddContentType(_T("text/html"));

  //Send the header and body all in one
  TransmitBuffer(m_Socket, responseHdr, (BYTE*)pszBody, dwBodyLength, pSettings->m_dwWritableTimeout);

  //Log the information
  PostLog(401, dwBodyLength);

  return dwBodyLength;
}

DWORD CHttpClient::ReturnFileDeletedOkMessage(const CString& /*sFile*/)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //Form the body of the response
  DWORD dwBodyLength = 0;
  char* pszBody = m_pServer->GetFileDeletedHTML(dwBodyLength);

  //Form the header of the response
  CHttpResponseHeader responseHdr;
  responseHdr.AddStatusCode(200);
  SYSTEMTIME st;
  GetSystemTime(&st);
  responseHdr.AddDate(st);
  responseHdr.AddServer(pSettings->m_sServerName);
  responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
  if (m_bResponseKeepAlive)
    responseHdr.AddKeepAlive();
  responseHdr.AddContentLength(dwBodyLength);
  responseHdr.AddContentType(_T("text/html"));

  //Send the header and body all in one
  TransmitBuffer(m_Socket, responseHdr, (BYTE*)pszBody, dwBodyLength, pSettings->m_dwWritableTimeout);

  //Log the information
  PostLog(200, dwBodyLength);

  //Don't forget to free up the memory
  delete [] pszBody;

  return dwBodyLength;
}

void CHttpClient::TransmitBuffer(BYTE* byData, DWORD dwSize, CHttpDirectory* /*pDirectory*/, BOOL bForceExpire)
{
  //validate our settings
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);
  ASSERT(pSettings->m_pMimeManager);

  CHttpResponseHeader responseHdr;

  //Get the current system time in UTC
  SYSTEMTIME stCurTime;
  ::GetSystemTime(&stCurTime);

  if (m_Request.m_dwHttpVersion > MAKELONG(9, 0)) //No header sent for Http 0.9
  {
    //Get the mime type for extension we are about to return
    CString sMime = pSettings->m_pMimeManager->GetMimeType(m_Request);

    //Form the header of the response
    responseHdr.AddStatusCode(200);
    responseHdr.AddDate(stCurTime);
    responseHdr.AddServer(pSettings->m_sServerName);
    responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
	  if (bForceExpire)
		  responseHdr.AddExpires(stCurTime);		
    if (m_bResponseKeepAlive)
      responseHdr.AddKeepAlive();
    responseHdr.AddContentLength(dwSize);
    responseHdr.AddContentType(sMime);

    //Send the header and body all in one
    TransmitBuffer(m_Socket, responseHdr, byData, dwSize, pSettings->m_dwWritableTimeout);
  }
  else
  {
    //Send back the file contents (if not a HEAD request)
    if (m_Request.m_Verb != CHttpRequest::HTTP_VERB_HEAD)
    {
      try
      {
        m_Socket.SendWithRetry(byData, dwSize, pSettings->m_dwWritableTimeout);
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CHttpClient::TransmitBuffer, Failed to send to socket, Error:%d"), pEx->m_nError);
        m_pServer->OnError(sError);

        pEx->Delete();  
      }
    }
  }

  //Log the information
  PostLog(200, dwSize);
}
 
CHttpDirectory* CHttpClient::GetVirtualDirectory(const CString& sDirectory)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  CHttpDirectory* pDirectory = NULL;
  for (int i=0; i<pSettings->m_Directories.GetSize() && pDirectory == NULL; i++)  
  {
    CHttpDirectory* pDir = pSettings->m_Directories.GetAt(i);
    if (sDirectory.CompareNoCase(pDir->GetAlias()) == 0)
      pDirectory = pDir;
  }

  return pDirectory;  
}

int CHttpClient::HexToInt(TCHAR ch)
{
  //character is in range of 0-9 subtract ascii value of '0' to get integer
  if ((ch >= _T('0')) && (ch <= _T('9')))
    return ch - _T('0');

  //character is in range of a-z or A-Z subtract ascii value of 'a' and add 10 to get it converted into int
  if ((ch >= _T('a')) && (ch <= _T('f')))
    return ch - _T('a') + 10;
  else if ((ch >= _T('A')) && (ch <= _T('F')))
    return ch - _T('A') + 10;

  //Character is not a Hex Digit
  return -1;
}

TCHAR CHttpClient::IntToHex(int Character)
{
  //This function only expects nibbles
  ASSERT(Character >= 0 && Character <= 15);
  
  if (Character <= 9)
    return (TCHAR)(Character + _T('0'));
  else
    return (TCHAR)(Character - 10 + _T('A'));
}

CString CHttpClient::UTF8ToCString(const CString& sURL)
{
  //Get the length of the string to convert from UTF
  int nInputLen = sURL.GetLength();

  //Allocate memory to hold the Unicode string
  LPWSTR lpwszURL = new WCHAR[nInputLen * 4];

  //Convert the UTF input string to Unicode
  int nInputIndex = 0;
  int nOutputIndex = 0;
  while (nInputIndex < nInputLen)
  {
    BYTE cInputChar1 = (BYTE) sURL.GetAt(nInputIndex);
    if (cInputChar1 <= 0x7f)
    {
      //Form the current output character
      lpwszURL[nOutputIndex] = cInputChar1;

      //Move onto the next input character
      nInputIndex++;
    }
    else if (nInputIndex < nInputLen-1 && cInputChar1 <= 0xdf)
    {
      //Form the current output character
      lpwszURL[nOutputIndex] = (wchar_t) (((cInputChar1 & 0x1f) << 6) + 
                                          (sURL.GetAt(nInputIndex+1) & 0x3f));

      //Move onto the next input character
      nInputIndex += 2;
    }
    else if (nInputIndex < nInputLen-2)
    {

      //Form the current output character
      lpwszURL[nOutputIndex] = (wchar_t) (((cInputChar1 & 0x0f) << 12) + 
                                          ((sURL.GetAt(nInputIndex+1) & 0x3f) << 6) +
                                          (sURL.GetAt(nInputIndex+2) & 0x3f));

      //Move onto the next input character
      nInputIndex += 3;
    }
    else
    {
      //skip illegal sequence
      nInputIndex++;
    }

    //Move onto the next output character
    nOutputIndex++;
  }

  //Don't forget to NULL terminate
  lpwszURL[nOutputIndex] = _T('\0');

  //Form a CString version of the Unicode string
  CString sDecodedURL(lpwszURL);

  //Tidy up the heap memory we have been using
  delete [] lpwszURL;

  //Return the string
  return sDecodedURL;
}

CString CHttpClient::URLEncode(const CString& sURL)
{
  CString sEncodedURL;
  int nLength = sURL.GetLength();
  LPTSTR pszEncodedURL = sEncodedURL.GetBufferSetLength((nLength*3) + 1);
  int nOutputIndex = 0;
  for (int i=0; i<nLength; i++)
  {
    //Pull out the current character to evaluate
    BYTE CurrentChar = (BYTE) sURL.GetAt(i);

    //Should we encode the character or not? See RFC 1738 for the details.
    if ((CurrentChar >= '0' && CurrentChar <= '9') ||
        (CurrentChar >= 'a' && CurrentChar <= 'z') ||
        (CurrentChar >= 'A' && CurrentChar <= 'Z') ||
        (CurrentChar == '$') ||
        (CurrentChar == '-') ||
        (CurrentChar == '_') ||
        (CurrentChar == '.') ||
        (CurrentChar == '+') ||
        (CurrentChar == '!') ||
        (CurrentChar == '*') ||
        (CurrentChar == '\'') ||
        (CurrentChar == '(') ||
        (CurrentChar == ')') ||
        (CurrentChar == ','))
    {
      pszEncodedURL[nOutputIndex] = CurrentChar;
      ++nOutputIndex;
    }
    else 
    {
      pszEncodedURL[nOutputIndex] = _T('%');
      ++nOutputIndex;

      TCHAR nNibble = IntToHex((CurrentChar & 0xF0) >> 4);
      pszEncodedURL[nOutputIndex] = nNibble;
      ++nOutputIndex;

      nNibble = IntToHex(CurrentChar & 0x0F);  
      pszEncodedURL[nOutputIndex] = nNibble;
      ++nOutputIndex;
    }
  }
  //Don't forget to NULL terminate
  pszEncodedURL[nOutputIndex] = _T('\0');
  sEncodedURL.ReleaseBuffer();

  return sEncodedURL;
}

CString CHttpClient::URLDecode(const CString& sURL)
{
  CString sDecodedURL;
  int nLength = sURL.GetLength();
  LPTSTR pszDecodedURL = sDecodedURL.GetBufferSetLength(nLength + 1);
  int nOutputIndex = 0;
  for (int i=0; i<nLength; i++)
  {
    TCHAR c1 = sURL[i];
    if (c1 != _T('%'))
    {
      if (c1 == '+')
        pszDecodedURL[nOutputIndex] = ' ';  
      else
        pszDecodedURL[nOutputIndex] = c1;
      nOutputIndex++;
    }
    else
    {
      if (i < nLength-2)
      {
        int msb = HexToInt(sURL[i+1]);
        int lsb = HexToInt(sURL[i+2]);
        if (msb != -1 && lsb != -1)
        {
          int nChar = (msb << 4) + lsb;
          pszDecodedURL[nOutputIndex] = TCHAR(nChar);
          nOutputIndex++;
          i += 2;
        }
        else
        {
          pszDecodedURL[nOutputIndex] = c1;
          nOutputIndex++;
        }
      }
      else
      {
        pszDecodedURL[nOutputIndex] = c1;
        nOutputIndex++;
      }
    }
  }
  //Don't forget to NULL terminate
  pszDecodedURL[nOutputIndex] = _T('\0');
  sDecodedURL.ReleaseBuffer();

  return sDecodedURL;
}

BOOL CHttpClient::MapURLToLocalFilename(CString& sURL, CString& sLocalFile, CString& sPathInfo, BOOL& bDirectory, CHttpDirectory*& pDirectory, DWORD& dwMatchingURL, DWORD& dwMatchingPath)
{
  //Setup the default return value from this function
  BOOL bSuccess = FALSE;
  sLocalFile.Empty();
  sPathInfo.Empty();
  bDirectory = FALSE;
  pDirectory = NULL;
  dwMatchingURL = 0;
  dwMatchingPath = 0;

  //Convert from Unix to Windows format
  CString sClientURL(sURL);
  sClientURL.Replace(_T("/"), _T("\\"));

  //As a security precaution do not allow any URL's which contains any relative parts in it 
  //(as an example trying to request a file outside of the directories we are serving)
  //We also exclude URL's with a ":" in them as this is how NTFS streams are accessed
  if (sClientURL.Find(_T("..")) == -1 && sClientURL.Find(_T(":")) == -1)
  {
    pDirectory = NULL;
    TCHAR sDrive[_MAX_DRIVE];
    TCHAR sDir[_MAX_DIR];
    TCHAR sFname[_MAX_FNAME];
    TCHAR sExt[_MAX_EXT];
    CString sVirtualDir(sClientURL);
    sVirtualDir += _T("\\"); //Initially try a default directory
    do
    {
      _tsplitpath(sVirtualDir, sDrive, sDir, sFname, sExt);
      if (_tcslen(sDir))
      {
        pDirectory = GetVirtualDirectory(sDir);
        if (pDirectory == NULL)
        {
          sVirtualDir = sDir;
          sVirtualDir = sVirtualDir.Left(sVirtualDir.GetLength()-1);
        }
      }
    }
    while (pDirectory == NULL && _tcslen(sDir));

    if (pDirectory)
    {
	  CString strPath = pDirectory->GetDirectory();
      ASSERT(pDirectory->GetDirectory().GetLength());
      ASSERT(pDirectory->GetAlias().GetLength());

      //Ignore the alias part of the URL now that we have got the virtual directory
      CString sAlias = pDirectory->GetAlias();
      CString sRelativeFile(sClientURL);
      sRelativeFile = sRelativeFile.Right(sRelativeFile.GetLength() - sAlias.GetLength());

      //Form the local filename from the requested URL
      CString sDirectory = pDirectory->GetDirectory();
      int nLength = sDirectory.GetLength();
      if (sDirectory.GetAt(nLength-1) != _T('\\'))
        sDirectory += _T("\\");
      sLocalFile = sDirectory; 

      //Asking for the default filename
      if (sRelativeFile.IsEmpty())
      {
        bDirectory = pDirectory->GetDirectoryListing();
        if (!bDirectory)
          sLocalFile += pDirectory->GetDefaultFile(); 

        dwMatchingURL = sURL.GetLength();
        dwMatchingPath = sLocalFile.GetLength();
      }
      else
      {
        //Ensure that we don't have two "\" separating the filename from the directory
        if (sClientURL.Find(_T('\\')) == 0)
          sLocalFile += sRelativeFile.Right(sRelativeFile.GetLength());
        else
          sLocalFile += sRelativeFile; 

        dwMatchingURL = sURL.GetLength();
        dwMatchingPath = sLocalFile.GetLength();

        //Keep parsing left to right until we find a piece of the path which is a file.
        //This is used to work out the PathInfo value
        CString sTemp(sRelativeFile);
        int nLocalFileData = 0;
        BOOL bContinueParse = TRUE;
        while (bContinueParse)
        {
          int nSlash = sTemp.Find(_T('\\'));
          if (nSlash != -1)
          {
            nLocalFileData += nSlash;
            CString sFile(sDirectory + sRelativeFile.Left(nLocalFileData));
            sTemp = sTemp.Right(sTemp.GetLength() - nSlash - 1);

            DWORD dwAttributes = GetFileAttributes(sFile);
            if (dwAttributes == INVALID_FILE_ATTRIBUTES)
            {
              bContinueParse = FALSE;
              sPathInfo = sLocalFile.Right(sLocalFile.GetLength() - sFile.GetLength() + nSlash);
              sPathInfo.Replace(_T("\\"), _T("/"));
              sLocalFile = sFile.Left(sFile.GetLength() - nSlash - 1);

              //Remove the PathInfo from the incoming parameter
              sURL = sURL.Left(sURL.GetLength() - sPathInfo.GetLength());

              dwMatchingURL = sClientURL.GetLength() - sPathInfo.GetLength();
              dwMatchingPath = sLocalFile.GetLength();
            }
            else if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
              bContinueParse = FALSE;
              sPathInfo = sLocalFile.Right(sLocalFile.GetLength() - sFile.GetLength() - 1);
              sPathInfo.Replace(_T("\\"), _T("/"));
              sLocalFile = sFile;

              //Remove the PathInfo from the incoming parameter
              sURL = sURL.Left(sURL.GetLength() - sPathInfo.GetLength() - 1);

              dwMatchingURL = sClientURL.GetLength() - sPathInfo.GetLength();
              dwMatchingPath = sLocalFile.GetLength();
            }

            nLocalFileData++; //Move over the directory separator
          }
          else
          {
            bContinueParse = FALSE;

            CString sFile(sDirectory + sRelativeFile.Left(nLocalFileData) + sTemp);

            DWORD dwAttributes = GetFileAttributes(sFile);
            if (dwAttributes == INVALID_FILE_ATTRIBUTES)
            {
              sPathInfo = sTemp;
              sPathInfo.Replace(_T("\\"), _T("/"));
              sLocalFile = sDirectory + sRelativeFile.Left(nLocalFileData);

              //Remove the PathInfo from the incoming parameter
              sURL = sURL.Left(sURL.GetLength() - sPathInfo.GetLength());

              dwMatchingURL = sClientURL.GetLength() - sPathInfo.GetLength();
              dwMatchingPath = sLocalFile.GetLength();
            }
          }
        }

        bDirectory = pDirectory->GetDirectoryListing() && (((GetFileAttributes(sLocalFile) & FILE_ATTRIBUTE_DIRECTORY) != 0));
        if (bDirectory)
        {
          int nURLLength = sURL.GetLength();
          if (nURLLength && sURL.GetAt(nURLLength-1) != _T('/'))
            sURL += _T("/");
        }

      }

      bSuccess = TRUE;
    }
  }

  return bSuccess;
}

#ifdef _DEBUG
void CHttpClient::PostLog(int nHTTPStatusCode, DWORD dwBodyLength)
#else
void CHttpClient::PostLog(int /*nHTTPStatusCode*/, DWORD /*dwBodyLength*/)
#endif
{
#ifdef _DEBUG
  //The default is to just TRACE the details

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
  TRACE(_T("%d.%d.%d.%d - %s %s %04d] \"%s\" %d %d\n"), 
        m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
        m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, 
        m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3, 
        m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4, 
        sUser, sDateTime, tzi.Bias, m_Request.m_sRequest, nHTTPStatusCode, dwBodyLength);
#endif        
}

BOOL CHttpClient::TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize, DWORD dwTimeout)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //Assume the worst
  BOOL bSuccess = FALSE;

#ifdef W3MFC_SSL_SUPPORT
  BOOL bTryUsingSendExtension = (pSettings->m_SSLProtocol == CHttpServerSettings::SSL_NONE);
#else
  BOOL bTryUsingSendExtension = TRUE;
#endif

  if (bTryUsingSendExtension && _W3MFCData.m_lpfnTransmitFile)
  {
    bSuccess = _W3MFCData.TransmitFile(socket, responseHdr, hFile, dwSize);

    //Handle the error
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpClient::TransmitFile, Failed to send response via TransmitFile, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }
  }
  else
  {
    //Send the header
#ifdef W3MFC_SSL_SUPPORT
    bSuccess = responseHdr.Send(m_Socket, dwTimeout, m_SSL);
#else
    bSuccess = responseHdr.Send(socket, dwTimeout);
#endif
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpClient::TransmitFile, Failed to send to response header, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }

    //Send the body
    if (bSuccess)
    {
      try
      {
        char sBuf[4096];
        DWORD dwBytesRead = 0;
        do 
        {
          if (::ReadFile(hFile, sBuf, 4096, &dwBytesRead, NULL) && dwBytesRead)
#ifdef W3MFC_SSL_SUPPORT
            socket.SendWithRetry(sBuf, dwBytesRead, dwTimeout, m_SSL);
#else
            socket.SendWithRetry(sBuf, dwBytesRead, dwTimeout);
#endif
        } 
        while (dwBytesRead);
        bSuccess = TRUE;
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CHttpClient::TransmitFile, Failed to send response file data, Error:%d"), pEx->m_nError);
        m_pServer->OnError(sError);

        pEx->Delete();  
      }
    }
  }

  return bSuccess;
}

BOOL CHttpClient::TransmitBuffer(CHttpSocket& socket, CHttpResponseHeader& responseHdr, BYTE* byData, DWORD dwSize, DWORD dwTimeout)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //Assume the worst
  BOOL bSuccess = FALSE;

#ifdef W3MFC_SSL_SUPPORT
  BOOL bTryUsingSendExtension = (pSettings->m_SSLProtocol == CHttpServerSettings::SSL_NONE);
#else
  BOOL bTryUsingSendExtension = TRUE;
#endif

  if (bTryUsingSendExtension && _W3MFCData.m_lpfnTransmitPackets)
  {
    bSuccess = _W3MFCData.TransmitBuffer(socket, responseHdr, byData, dwSize);
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpClient::TransmitBuffer, Failed to send response via TransmitPackets, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }
  }
  else
  {
    //Send the header
#ifdef W3MFC_SSL_SUPPORT
    bSuccess = responseHdr.Send(socket, dwTimeout, m_SSL);
#else
    bSuccess = responseHdr.Send(socket, dwTimeout);
#endif
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpClient::TransmitBuffer, Failed to send to response header, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }

    //Send the body
    if (bSuccess && dwSize)
    {
      try
      {
#ifdef W3MFC_SSL_SUPPORT
        socket.SendWithRetry(byData, dwSize, dwTimeout, m_SSL);
#else
        socket.SendWithRetry(byData, dwSize, dwTimeout);
#endif
        bSuccess = TRUE;
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CHttpClient::TransmitBuffer, Failed to send to response body, Error:%d"), pEx->m_nError);
        m_pServer->OnError(sError);

        pEx->Delete();  
      }
    }
  }

  return bSuccess;
}

void CHttpClient::SetRequestToStop()
{
  //Let the base class do its thing
  CThreadPoolClient::SetRequestToStop();

  //Set the event which signals that we want this worker thread to exit
  m_StopEvent.SetEvent();
}

BOOL CHttpClient::GetKeySizeServerVariable(int& nKeySize)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //What will be the return value
  BOOL bSuccess = FALSE;
  nKeySize = 0;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
  {
    SSL_get_cipher_bits(m_SSL, &nKeySize);
    bSuccess = TRUE;
  }
#endif

  return bSuccess;
}

BOOL CHttpClient::GetServerKeySizeServerVariable(int& nKeySize)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //What will be the return value
  BOOL bSuccess = FALSE;
  nKeySize = 0;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
  {
    EVP_PKEY* pKey = SSL_get_privatekey(m_SSL);
    if (pKey)
    {
      nKeySize = EVP_PKEY_bits(pKey);
      bSuccess = TRUE;
    }
  }
#endif

  return bSuccess;
}

BOOL CHttpClient::GetCertSerialNumberServerVariable(long& nSerialNumber)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //What will be the return value
  BOOL bSuccess = FALSE;
  nSerialNumber = 0;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
  {
    X509* pCert = SSL_get_certificate(m_SSL);
    if (pCert)
    {
      ASN1_INTEGER* pSerialNumber = X509_get_serialNumber(pCert);
      if (pSerialNumber)
      {
        nSerialNumber = ASN1_INTEGER_get(pSerialNumber);
        bSuccess = TRUE;
      }
    }
  }
#endif

  return bSuccess;
}

BOOL CHttpClient::GetCertIssuerServerVariable(char*& szIssuer)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //What will be the return value
  BOOL bSuccess = FALSE;
  szIssuer = NULL;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
  {
    X509* pCert = SSL_get_certificate(m_SSL);
    if (pCert)
    {
      szIssuer = X509_NAME_oneline(X509_get_issuer_name(pCert), NULL, NULL);
      bSuccess = TRUE;
    }
  }
#endif

  return bSuccess;  
}

BOOL CHttpClient::GetCertSubjectServerVariable(char*& szSubject)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);

  //What will be the return value
  BOOL bSuccess = FALSE;
  szSubject = NULL;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
  {
    X509* pCert = SSL_get_certificate(m_SSL);
    if (pCert)
    {
      szSubject = X509_NAME_oneline(X509_get_subject_name(pCert), NULL, NULL);
      bSuccess = TRUE;
    }
  }
#endif

  return bSuccess;
}

#ifndef W3MFC_NO_ISAPI_SUPPORT
BOOL CHttpClient::TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HSE_TF_INFO* pInfo)
{
  return _W3MFCData.TransmitFile(socket, responseHdr, pInfo);
}
#endif

BOOL CHttpClient::PreHandleGetPostHead()
{
  //This allows derived classes to internally handle GET, POST or HEAD requests.
  //Return TRUE in your derived classes to tell the framework that the request
  //has been handled and stops the framework from implementing its default processing
  //for a URL

  //In this base class we of course allow default processing to occur
  return FALSE;
}
