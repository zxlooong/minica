/*
Module : HttpISAPI.cpp
Purpose: Implementation for the CHttpISAPI class
Created: PJN / 21-02-2003
History: PJN / 22-02-2004 1. Fixed a memory leak in CHttpISAPI::CachedLoad. Thanks to "mori" for
                          reporting and suggesting the fix for this bug.

Copyright (c) 2003 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "HttpISAPI.h"
#include "HttpClient.h"

//////////////// Macros / Defines ////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef HSE_REQ_CLOSE_CONNECTION
#define HSE_REQ_CLOSE_CONNECTION                 (HSE_REQ_END_RESERVED+17)
#endif



//////////////// Implementation //////////////////////////////////////

CHttpISAPIExtension::CHttpISAPIExtension()
{
  m_hDLL = NULL;
  m_lpGetExtensionVersion = NULL;
  m_lpHttpExtensionProc = NULL;
  m_lpTerminateExtension = NULL;
}

CHttpISAPIExtension::~CHttpISAPIExtension()
{
  if (m_hDLL)
  {
    //Call the terminate function (if it exists)
    if (m_lpTerminateExtension)
      CallTerminateExtension();
      
    //Finally free up the library
    FreeLibrary(m_hDLL);
  }
}

BOOL CHttpISAPIExtension::CallGetExtensionVersion(HSE_VERSION_INFO* phseVerInfo)
{
  //Validate our parameters
  ASSERT(m_lpGetExtensionVersion);

  //Assume the worst
  BOOL bSuccess = FALSE;

  __try
  {
    bSuccess = m_lpGetExtensionVersion(phseVerInfo);
  }
  __except(1)
  {
    bSuccess = FALSE;
  }

  return bSuccess;
}

BOOL CHttpISAPIExtension::CallTerminateExtension()
{
  ASSERT(m_lpTerminateExtension);

  //Assume the worst
  BOOL bSuccess = FALSE;

  __try
  {
    bSuccess = m_lpTerminateExtension(HSE_TERM_MUST_UNLOAD);
  }
  __except(1)
  {
    bSuccess = FALSE;
  }

  return bSuccess;
}



CHttpISAPI::CHttpISAPI(int nISAPIHashTableSize) : m_mapExtensions(nISAPIHashTableSize)
{
  m_pServer = NULL;
}

CHttpISAPI::~CHttpISAPI()
{
  FreeMapEntries();
}

void CHttpISAPI::SetServer(CHttpServer* pServer)
{
  m_pServer = pServer;
}

void CHttpISAPI::FreeMapEntries()
{
  //Prevent the hash table from being manipulated
  //by multiple threads at the one time
  CSingleLock sl(&m_CS, TRUE);

  //Remove all the entries in the hash table
  for (POSITION pos = m_mapExtensions.GetStartPosition(); pos != NULL; )
  {
    CHttpISAPIExtension* pEntry = NULL;
    CString sKey;
    m_mapExtensions.GetNextAssoc(pos, sKey, (void*&) pEntry);
    if (pEntry)
      delete pEntry;
  }
  m_mapExtensions.RemoveAll();
}

BOOL CHttpISAPI::UncachedLoad(const CString& sPath, CHttpISAPIExtension& extension)
{
  //Validate our parameters
  ASSERT(m_pServer);

  //What will be the return value
  BOOL bSuccess = FALSE;
 
  //Load up the DLL
  ASSERT(extension.m_hDLL == NULL);
  extension.m_sPath = sPath;
  extension.m_hDLL = LoadLibraryEx(sPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
  if (extension.m_hDLL)
  {
    //Get the "GetExtensionVersion" function
    extension.m_lpGetExtensionVersion = (CHttpISAPIExtension::LPGETEXTENSIONVERSION) GetProcAddress(extension.m_hDLL, "GetExtensionVersion");
    if (extension.m_lpGetExtensionVersion == NULL)    
    {
      //Report the error that we could not find the function
      CString sError;
      sError.Format(_T("CHttpISAPI::UncachedLoad, Could not find entry point GetExtensionVersion in dll %s"), sPath);
      m_pServer->OnError(sError);
    }

    //Get the HttpExtensionProc function
    extension.m_lpHttpExtensionProc = (CHttpISAPIExtension::LPHTTPEXTENSIONPROC) GetProcAddress(extension.m_hDLL, "HttpExtensionProc");
    if (extension.m_lpHttpExtensionProc == NULL)    
    {
      //Report the error that we could not find the function
      CString sError;
      sError.Format(_T("CHttpISAPI::UncachedLoad, Could not find entry point HttpExtensionProc in dll %s"), sPath);
      m_pServer->OnError(sError);
    }

    if (extension.m_lpGetExtensionVersion && extension.m_lpHttpExtensionProc)
    {
      //Now find the optional "TerminateExtension" function (if initialisation succeeded)
      extension.m_lpTerminateExtension = (CHttpISAPIExtension::LPTERMINATEEXTENSION) GetProcAddress(extension.m_hDLL, "TerminateExtension");

      //Also call the GetExtensionVersion function
      HSE_VERSION_INFO hseVerInfo;
      CString sError;
      bSuccess = extension.CallGetExtensionVersion(&hseVerInfo);
      if (!bSuccess)
      {
        //Report the error that we could not call the GetExtensionVersion function
        sError.Format(_T("CHttpISAPI::UncachedLoad, Failed in call to GetExtensionVersion in dll %s, error:%d"), sPath, GetLastError());
        m_pServer->OnError(sError);
      }
    }
  }
  else
  {
    //Report the error that we could not find the function
    DWORD dwError = GetLastError();
    CString sError;
    sError.Format(_T("CHttpISAPI::UncachedLoad, Could not load the dll %s, error:%d"), sPath, dwError);
    m_pServer->OnError(sError);
  }

  return bSuccess;
}

CHttpISAPIExtension* CHttpISAPI::CachedLoad(const CString& sPath)
{
  //Validate our parameters
  ASSERT(m_pServer);
  CHttpServerSettings* pSettings = m_pServer->GetSettings();
  ASSERT(pSettings);
  ASSERT(pSettings->m_bCacheISAPI);

  //Look up in the hash table case insensitively
  CString sKey(sPath);
  sKey.MakeUpper(); 

  //What will be the return value
  CHttpISAPIExtension* pExtension = NULL;

  //Prevent the hash table from being manipulated
  //by multiple threads at the one time
  CSingleLock sl(&m_CS, TRUE);

  //try looking it up in the cache first
  if (!m_mapExtensions.Lookup(sKey, (void*&) pExtension))
  {
    //Only created if not found in hash
    pExtension = new CHttpISAPIExtension;
 
    //Call the other Load method to do most of the work
    if (!UncachedLoad(sPath, *pExtension))
    {
      delete pExtension;
      pExtension = NULL;
    }
    else
    {
      //Add it to the hash if caching is enabled
      m_mapExtensions.SetAt(sKey, pExtension);
    }
  }

  return pExtension;
}

BOOL CHttpISAPI::GetServerVariable(HCONN hConn, LPSTR lpszVariableName, void* lpBuffer, DWORD* lpdwBufferSize)
{
  USES_CONVERSION;

  //Validate our parameters
  CHttpClient* pClient = (CHttpClient*) hConn;
  if (pClient == NULL)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }  
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

  //Assume the best
  BOOL bSuccess = TRUE;

  if (strcmpi(lpszVariableName, "SERVER_PORT") == 0)
  {
    char szPort[10];
    sprintf(szPort, "%d", pSettings->m_nPort);
    DWORD dwSize = strlen(szPort) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szPort);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "SERVER_PORT_SECURE") == 0)
  {
  #ifdef W3MFC_SSL_SUPPORT
    if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
    {
      char szSecure[10];
      sprintf(szSecure, "%d", pSettings->m_nPort);
      DWORD dwSize = strlen(szSecure) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szSecure);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  #else
    SetLastError(ERROR_NO_DATA);
    bSuccess = FALSE;
  #endif
  }
  else if (strcmpi(lpszVariableName, "SERVER_PROTOCOL") == 0)
  {
    char szProtocol[10];
  #ifdef W3MFC_SSL_SUPPORT
    if (pSettings->m_SSLProtocol == CHttpServerSettings::SSL_NONE)
      strcpy(szProtocol, "HTTP/1.0");
    else
      strcpy(szProtocol, "HTTPS/1.0");
  #else
    strcpy(szProtocol, "HTTP/1.0");
  #endif

    DWORD dwSize = strlen(szProtocol) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szProtocol);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "SERVER_SOFTWARE") == 0)
  {
    char* szSoftware = T2A((LPTSTR) (LPCTSTR) pSettings->m_sServerName);
    DWORD dwSize = strlen(szSoftware) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szSoftware);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "URL") == 0)
  {
    char* szURL = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sURL);
    DWORD dwSize = strlen(szURL) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szURL);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "QUERY_STRING") == 0)
  {
    char* szQueryString = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sRawExtra);
    DWORD dwSize = strlen(szQueryString) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szQueryString);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "PATH_INFO") == 0)
  {
    char* szPathInfo = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sPathInfo);
    DWORD dwSize = strlen(szPathInfo) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szPathInfo);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "REMOTE_ADDR") == 0)
  {
    char szRemote[20];
    sprintf(szRemote, "%d.%d.%d.%d", pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
               pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3,
               pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4);
    DWORD dwSize = strlen(szRemote) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szRemote);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "REMOTE_HOST") == 0)
  {
    if (pClient->m_Request.m_sRemoteHost.GetLength())
    {
      char* szRemoteHost = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sRemoteHost);
      DWORD dwSize = strlen(szRemoteHost) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szRemoteHost);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      //just return the IP address if the remote host string is empty
      return GetServerVariable(hConn, "REMOTE_ADDR", lpBuffer, lpdwBufferSize);
    }
  }
  else if ((strcmpi(lpszVariableName, "REMOTE_USER") == 0) ||
           (strcmpi(lpszVariableName, "AUTH_USER") == 0))
  {
    char* szUser = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sUsername);
    DWORD dwSize = strlen(szUser) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szUser);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "REQUEST_METHOD") == 0)
  {
    char* szVerb = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sVerb);
    DWORD dwSize = strlen(szVerb) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szVerb);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "SERVER_NAME") == 0)
  {
    char* szServerName = NULL;
    char szHostName[256];
    if (pSettings->m_sBindAddress.GetLength())
      szServerName = T2A((LPTSTR) (LPCTSTR) pSettings->m_sBindAddress);
    else
    {
      if (gethostname(szHostName, 256) == 0)
        szServerName = szHostName; 
      else
        szServerName = "";
    }

    DWORD dwSize = strlen(szServerName) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szServerName);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "HTTPS") == 0)
  {
    char szHTTPS[10];
  #ifdef W3MFC_SSL_SUPPORT
    if (pSettings->m_SSLProtocol == CHttpServerSettings::SSL_NONE)
      strcpy(szHTTPS, "off");
    else
      strcpy(szHTTPS, "on");
  #else
    strcpy(szHTTPS, "off");
  #endif

    DWORD dwSize = strlen(szHTTPS) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szHTTPS);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "AUTH_TYPE") == 0)
  {
    char szType[10];
    if (pClient->m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_PLAINTEXT)
      strcpy(szType, "Basic");
    else if (pClient->m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_NTLM)
      strcpy(szType, "NTLM");
    else
      strcpy(szType, "");

    DWORD dwSize = strlen(szType) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szType);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if ((strcmpi(lpszVariableName, "AUTH_PASSWORD") == 0) ||
           (strcmpi(lpszVariableName, "REMOTE_PASSWORD") == 0))
  {
    if (pClient->m_Request.m_sPassword.GetLength())
    {
      char* szPassword = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sPassword);
      DWORD dwSize = strlen(szPassword) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szPassword);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "AUTH_REALM") == 0)
  {
    if (pClient->m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_PLAINTEXT)
    {
      char* szRealm = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sURL);
      DWORD dwSize = strlen(szRealm) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szRealm);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "CONTENT_LENGTH") == 0)
  {
    if (pClient->m_Request.m_nContentLength)
    {
      char szContentLength[10];
      sprintf(szContentLength, "%d", pClient->m_Request.m_nContentLength);
      DWORD dwSize = strlen(szContentLength) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szContentLength);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "CONTENT_TYPE") == 0)
  {
    if (pClient->m_Request.m_sContentType.GetLength())
    {
      char* szContentType = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sContentType);
      DWORD dwSize = strlen(szContentType) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szContentType);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "LOGON_USER") == 0)
  {
    if (pClient->m_Request.m_bLoggedOn)
    {
      char* szUser = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sUsername);
      DWORD dwSize = strlen(szUser) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szUser);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else if (pSettings->m_sUsername.GetLength())
    {
      char* szUser = T2A((LPTSTR) (LPCTSTR) pSettings->m_sUsername);
      DWORD dwSize = strlen(szUser) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szUser);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if ((strcmpi(lpszVariableName, "HTTPS_SERVER_SUBJECT") == 0) ||
           (strcmpi(lpszVariableName, "CERT_SERVER_SUBJECT") == 0))
  {
    char* szSubject = NULL;
    if (pClient->GetCertIssuerServerVariable(szSubject))
    {
      DWORD dwSize = strlen(szSubject) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szSubject);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if ((strcmpi(lpszVariableName, "HTTPS_SERVER_ISSUER") == 0) ||
           (strcmpi(lpszVariableName, "CERT_SERVER_ISSUER") == 0))
  { 
    char* szIssuer = NULL;
    if (pClient->GetCertIssuerServerVariable(szIssuer))
    {
      DWORD dwSize = strlen(szIssuer) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szIssuer);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "CERT_SERIALNUMBER") == 0)
  {
    long nSerialNumber = 0;
    if (pClient->GetCertSerialNumberServerVariable(nSerialNumber))
    {
      char szSerialNumber[10];
      sprintf(szSerialNumber, "%d", nSerialNumber);
      DWORD dwSize = strlen(szSerialNumber) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szSerialNumber);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if ((strcmpi(lpszVariableName, "HTTPS_SECRETKEYSIZE") == 0) ||
           (strcmpi(lpszVariableName, "CERT_SECRETKEYSIZE") == 0))
  {
    int nBits = 0;
    if (pClient->GetServerKeySizeServerVariable(nBits))
    {
      char szBits[10];
      sprintf(szBits, "%d", nBits);
      DWORD dwSize = strlen(szBits) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szBits);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if ((strcmpi(lpszVariableName, "HTTPS_KEYSIZE") == 0) ||
           (strcmpi(lpszVariableName, "CERT_KEYSIZE") == 0)) 
  {
    int nBits = 0;
    if (pClient->GetKeySizeServerVariable(nBits))
    {
      char szBits[10];
      sprintf(szBits, "%d", nBits);
      DWORD dwSize = strlen(szBits) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szBits);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "PATH_TRANSLATED") == 0)
  {
    LPTSTR pszFilePart;
    TCHAR pszAbsolutePath[_MAX_PATH];
    GetFullPathName(pClient->m_Request.m_sLocalFile, _MAX_PATH, pszAbsolutePath, &pszFilePart);
    char* szAbsolutePath = T2A(pszAbsolutePath);
    DWORD dwSize = strlen(szAbsolutePath) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szAbsolutePath);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "SCRIPT_NAME") == 0)
  {
    CString sScriptName = pClient->m_Request.m_sLocalFile;
    sScriptName.Replace(_T('\\'), _T('/')); //Ensure we use unix directory separators
    if (sScriptName.GetLength() && sScriptName.GetAt(0) != _T('/'))
      sScriptName = _T("/") + sScriptName;

    char* szScriptName = T2A((LPTSTR) (LPCTSTR) sScriptName);
    DWORD dwSize = strlen(szScriptName) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szScriptName);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "REQUEST_LINE") == 0)
  {
    char* szRequest = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sRequest);
    DWORD dwSize = strlen(szRequest) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szRequest);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "ALL_HTTP") == 0)
  {
    CString sAllHeaders;
    POSITION posMap = pClient->m_Request.m_HeaderMap.GetStartPosition();
    while (posMap)
    {
      CString sKey;
      CString sValue;
      pClient->m_Request.m_HeaderMap.GetNextAssoc(posMap, sKey, sValue);

      CString sLine;
      if (posMap)  
        sLine.Format(_T("HTTP_%s=%s\n"), sKey, sValue);
      else
        sLine.Format(_T("HTTP_%s=%s"), sKey, sValue);
        
      sAllHeaders += sLine;
    }
    
    char* pszAllHeaders = T2A((LPTSTR) (LPCTSTR) sAllHeaders);
    DWORD dwSize = strlen(pszAllHeaders) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, pszAllHeaders);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "ALL_RAW") == 0)
  {
    DWORD dwSize = 0;
    if (pClient->m_Request.m_pRawEntity)
      dwSize = pClient->m_Request.m_pRawEntity - pClient->m_Request.m_pRawRequest + 1;
    else
      dwSize = pClient->m_Request.m_dwRawRequestSize;
    if (*lpdwBufferSize >= dwSize)
    {
      CopyMemory(lpBuffer, pClient->m_Request.m_pRawRequest, dwSize);
      ((BYTE*) lpBuffer)[dwSize] = '\0';
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (strcmpi(lpszVariableName, "HTTP_VERSION") == 0)
  {
    char szVersion[10];
    sprintf(szVersion, "%d.%d", HIWORD(pClient->m_Request.m_dwHttpVersion), LOWORD(pClient->m_Request.m_dwHttpVersion));
    DWORD dwSize = strlen(szVersion) + 1;
    if (*lpdwBufferSize >= dwSize)
    {
      strcpy((char*) lpBuffer, szVersion);
      *lpdwBufferSize = dwSize;
    }
    else
    { 
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      bSuccess = FALSE;
    }
  }
  else if (_strnicmp(lpszVariableName, "HTTP_", 5) == 0)
  {
    CString sKey(lpszVariableName);
    sKey = sKey.Right(sKey.GetLength() - 5);
    sKey.MakeUpper();
    CString sValue;
    if (pClient->m_Request.m_HeaderMap.Lookup(sKey, sValue))
    {
      char* szValue = T2A((LPTSTR) (LPCTSTR) sValue);
      DWORD dwSize = strlen(szValue) + 1;
      if (*lpdwBufferSize >= dwSize)
      {
        strcpy((char*) lpBuffer, szValue);
        *lpdwBufferSize = dwSize;
      }
      else
      { 
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bSuccess = FALSE;
      }
    }
    else
    {
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    }
  }
  else
  {
    SetLastError(ERROR_INVALID_INDEX);
    bSuccess = FALSE;
  }

  return bSuccess;
}

BOOL CHttpISAPI::ReadClient(HCONN hConn, LPVOID /*lpvBuffer*/, LPDWORD lpdwSize)
{
  //nothing to read, so set the value
  *lpdwSize = 0;

  //Validate our parameters
  CHttpClient* pClient = (CHttpClient*) hConn;
  if (pClient == NULL)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }  

  //We do not support reading from the client, since we supply all of the data in the EXTENSION_CONTROL_BLOCK
  SetLastError(ERROR_NO_DATA);
  return FALSE;
}

BOOL CHttpISAPI::WriteClient(HCONN hConn, LPVOID Buffer, LPDWORD lpdwBytes, DWORD dwSync)
{
  //Validate our parameters
  CHttpClient* pClient = (CHttpClient*) hConn;
  if ((pClient == NULL) || (dwSync == HSE_IO_ASYNC))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }  
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

  //Assume the worst
  BOOL bSuccess = FALSE;

  try
  {
  #ifdef W3MFC_SSL_SUPPORT
    pClient->m_Socket.SendWithRetry(Buffer, *lpdwBytes, pSettings->m_dwWritableTimeout, pClient->m_SSL);
  #else    
    pClient->m_Socket.SendWithRetry(Buffer, *lpdwBytes, pSettings->m_dwWritableTimeout);
  #endif

    pClient->m_dwDataSentViaWriteClient += *lpdwBytes;
    bSuccess = TRUE;
  }
  catch(CWSocketException* pEx)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpISAPI::WriteClient, Failed to send to socket, error:%d"), pEx->m_nError);
    pClient->m_pServer->OnError(sError);

    //Allow callers to see what the error was
    SetLastError(pEx->m_nError);

    pEx->Delete();  
  }
  return bSuccess;
}

BOOL CHttpISAPI::ServerSupportFunction(HCONN hConn, DWORD dwHSERRequest, LPVOID lpvBuffer, LPDWORD lpdwSize, LPDWORD lpdwDataType)
{
  USES_CONVERSION;

  //Validate our parameters
  CHttpClient* pClient = (CHttpClient*) hConn;
  if (pClient == NULL)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }  
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

  //Assume the best
  BOOL bSuccess = TRUE;

  switch (dwHSERRequest)
  {
    case HSE_REQ_CLOSE_CONNECTION:
    {
      //We support this function but do a NOOP since
      //the connection will be closed when HttpExtensionProc
      //returns normally
      break;
    }
    case HSE_REQ_GET_IMPERSONATION_TOKEN:
    {
      if (pClient->m_Request.m_hImpersonation)
        *((HANDLE*) lpvBuffer) = pClient->m_Request.m_hImpersonation;
      else
      {
        SetLastError(ERROR_NO_DATA);
        bSuccess = FALSE;
      }
      break;
    }
    case HSE_REQ_GET_SSPI_INFO:
    {
    #ifndef W3MFC_NO_SSPI_SUPPORT
      CopyMemory(((CtxtHandle*) lpvBuffer), &pClient->m_Request.m_hImpersonation, sizeof(CtxtHandle));
      CopyMemory(((CredHandle*) lpdwDataType), pClient->m_pServer->GetCredentialHandle(), sizeof(CredHandle));
    #else
      SetLastError(ERROR_NO_DATA);
      bSuccess = FALSE;
    #endif
      break;
    }
    case HSE_REQ_IS_KEEP_CONN:
    {
      *((BOOL*) lpvBuffer) = pClient->m_bResponseKeepAlive;
      break;
    }
    case HSE_REQ_MAP_URL_TO_PATH:
    {
      LPSTR pszURL = (LPSTR) lpvBuffer;
      HSE_URL_MAPEX_INFO* pUMI = (HSE_URL_MAPEX_INFO*) lpdwDataType;
      pUMI->dwFlags = 0;

      CString sURL(pszURL);
      BOOL bDirectory = FALSE;
      CHttpDirectory* pDirectory = NULL;
      CString sLocalFile;
      CString sPathInfo;
      if (pClient->MapURLToLocalFilename(sURL, sLocalFile, sPathInfo, bDirectory, pDirectory, pUMI->cchMatchingURL, pUMI->cchMatchingPath))
      {
        LPSTR pszLocalFile = T2A((LPTSTR) (LPCTSTR) sLocalFile);
        DWORD dwSize = strlen(pszLocalFile) + 1;
        if (*lpdwSize >= dwSize)
        {
          strcpy(pszURL, pszLocalFile);

          if (!pDirectory->GetScript())
            pUMI->dwFlags |= HSE_URL_FLAGS_READ;
          else
          {
            pUMI->dwFlags |= HSE_URL_FLAGS_EXECUTE;
            pUMI->dwFlags |= HSE_URL_FLAGS_SCRIPT;
          }
          if (pDirectory->GetWritable())
            pUMI->dwFlags |= HSE_URL_FLAGS_WRITE;

        #ifdef W3MFC_SSL_SUPPORT
          if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
             pUMI->dwFlags |= HSE_URL_FLAGS_SSL;
        #endif
        }
        else
        {
          SetLastError(ERROR_INSUFFICIENT_BUFFER);
          bSuccess = FALSE;
        }
      }
      else
      {
        SetLastError(ERROR_NO_DATA);
        bSuccess = FALSE;
      }
      break;
    }
    case HSE_REQ_MAP_URL_TO_PATH_EX:
    {
      LPSTR pszURL = (LPSTR) lpvBuffer;
      HSE_URL_MAPEX_INFO* pUMI = (HSE_URL_MAPEX_INFO*) lpdwDataType;
      pUMI->dwFlags = 0;

      CString sURL(pszURL);
      BOOL bDirectory = FALSE;
      CHttpDirectory* pDirectory = NULL;
      CString sLocalFile;
      CString sPathInfo;
      if (pClient->MapURLToLocalFilename(sURL, sLocalFile, sPathInfo, bDirectory, pDirectory, pUMI->cchMatchingURL, pUMI->cchMatchingPath))
      {
        LPSTR pszLocalFile = T2A((LPTSTR) (LPCTSTR) sLocalFile);
        strcpy(pUMI->lpszPath, pszLocalFile);

        if (!pDirectory->GetScript())
          pUMI->dwFlags |= HSE_URL_FLAGS_READ;
        else
        {
          pUMI->dwFlags |= HSE_URL_FLAGS_EXECUTE;
          pUMI->dwFlags |= HSE_URL_FLAGS_SCRIPT;
        }
        if (pDirectory->GetWritable())
          pUMI->dwFlags |= HSE_URL_FLAGS_WRITE;

      #ifdef W3MFC_SSL_SUPPORT
        if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
           pUMI->dwFlags |= HSE_URL_FLAGS_SSL;
      #endif
      }
      else
      {
        SetLastError(ERROR_NO_DATA);
        bSuccess = FALSE;
      }
      break;
    }
    case HSE_REQ_SEND_URL:
    case HSE_REQ_SEND_URL_REDIRECT_RESP:
    {
      LPSTR pszURL = (LPSTR) lpvBuffer;
      pClient->ReturnRedirectMessage(pszURL);
      pClient->m_nHttpStatusCodeSent = 302;
      break;
    }
    case HSE_REQ_SEND_RESPONSE_HEADER:
    {
      LPSTR pszStatusString = (LPSTR) lpvBuffer;
      LPSTR pszExtraHeaders = (LPSTR) lpdwDataType;

      CHttpResponseHeader responseHdr;
      if (pszStatusString && strlen(pszStatusString))
      {
        responseHdr.AddStatus(pszStatusString);
        pClient->m_nHttpStatusCodeSent = atoi(pszStatusString);
      }
      else
      {
        responseHdr.AddStatusCode(200);
        pClient->m_nHttpStatusCodeSent = 200;
      }

      SYSTEMTIME stCurTime;
      ::GetSystemTime(&stCurTime);
      responseHdr.AddDate(stCurTime);
      responseHdr.AddServer(pSettings->m_sServerName);
      responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
      if (pszExtraHeaders && strlen(pszExtraHeaders))
        responseHdr.AddExtraHeaders(pszExtraHeaders);
      pClient->m_bResponseKeepAlive = FALSE;
      responseHdr.SetAddEntitySeparator(FALSE);
      
      //Send the header 
      pClient->TransmitBuffer(pClient->m_Socket, responseHdr, NULL, 0, pSettings->m_dwWritableTimeout);
      break;
    }
    case HSE_REQ_SEND_RESPONSE_HEADER_EX:
    {
      HSE_SEND_HEADER_EX_INFO* pHeader = (HSE_SEND_HEADER_EX_INFO*) lpvBuffer;

      CHttpResponseHeader responseHdr;
      if (pHeader->pszStatus && strlen(pHeader->pszStatus))
      {
        responseHdr.AddStatus(pHeader->pszStatus);
        pClient->m_nHttpStatusCodeSent = atoi(pHeader->pszStatus);
      }
      else
      {
        responseHdr.AddStatusCode(200);
        pClient->m_nHttpStatusCodeSent = 200;
      }

      SYSTEMTIME stCurTime;
      ::GetSystemTime(&stCurTime);
      responseHdr.AddDate(stCurTime);
      responseHdr.AddServer(pSettings->m_sServerName);
      responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
      if (pHeader->pszHeader && strlen(pHeader->pszHeader))
        responseHdr.AddExtraHeaders(pHeader->pszHeader);
      if (pHeader->fKeepConn)
        responseHdr.AddKeepAlive();
      pClient->m_bResponseKeepAlive = pHeader->fKeepConn;
      responseHdr.SetAddEntitySeparator(FALSE);

      //Send the header 
      pClient->TransmitBuffer(pClient->m_Socket, responseHdr, NULL, 0, pSettings->m_dwWritableTimeout);
      break;
    }
    case HSE_REQ_TRANSMIT_FILE:
    {
    #ifdef W3MFC_SSL_SUPPORT
      BOOL bAvailable = (pSettings->m_SSLProtocol == CHttpServerSettings::SSL_NONE);
    #else
      BOOL bAvailable = TRUE;
    #endif
      if (bAvailable)
      {
        CHttpResponseHeader responseHdr;
        SYSTEMTIME stCurTime;
        ::GetSystemTime(&stCurTime);
        responseHdr.AddDate(stCurTime);
        responseHdr.AddServer(pSettings->m_sServerName);
        responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
        pClient->m_bResponseKeepAlive = FALSE;

        HSE_TF_INFO* pInfo = (HSE_TF_INFO*) lpvBuffer;
        bSuccess = pClient->TransmitFile(pClient->m_Socket, responseHdr, pInfo);
      }
      else
      {
        SetLastError(ERROR_INVALID_INDEX);
        bSuccess = FALSE;
      }
      break;
    }
    default:
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpISAPI::ServerSupportFunction, An invalid HSERequest value was specified %s, HSERequest:%d"), pClient->m_Request.m_sURL, dwHSERRequest);
      pClient->m_pServer->OnError(sError);

      SetLastError(ERROR_INVALID_INDEX);
      bSuccess = FALSE;
      break;
    }
  }

  return bSuccess;
}

void CHttpISAPI::ReportHttpExtensionProcError(CHttpClient* pClient, CHttpISAPIExtension& extension, DWORD dwError)
{
  //Validate our parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);

  //Report the error
  CString sError;
  sError.Format(_T("CHttpISAPI::ReportHTTPExtensionProcError, Failed calling the function HttpExtensionProc in the ISAPI extension %s, error:%d"), extension.m_sPath, dwError);
  pClient->m_pServer->OnError(sError);
}

BOOL CHttpISAPI::CallHttpExtensionProc(CHttpClient* pClient, CHttpISAPIExtension& extension)
{
  USES_CONVERSION;

  //Validate our parameters
  ASSERT(extension.m_lpHttpExtensionProc);
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);

  //Assume the worst
  BOOL bSuccess = FALSE;

  __try
  {
    //Setup the structure
    EXTENSION_CONTROL_BLOCK ecb;
    ecb.cbSize = sizeof(EXTENSION_CONTROL_BLOCK);
    ecb.dwVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);
    ecb.ConnID = (HCONN) pClient;
    ecb.dwHttpStatusCode = 200;
    strcpy(ecb.lpszLogData, "");
    ecb.lpszMethod = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sVerb);
    ecb.lpszQueryString = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sRawExtra);
    ecb.lpszPathInfo = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sPathInfo);
    ecb.lpszPathTranslated = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sLocalFile);
    ecb.cbTotalBytes = pClient->m_Request.m_dwRawEntitySize;
    ecb.cbAvailable = ecb.cbTotalBytes;
    ecb.lpbData = pClient->m_Request.m_pRawEntity;
    ecb.lpszContentType = T2A((LPTSTR) (LPCTSTR) pClient->m_Request.m_sContentType);
    ecb.GetServerVariable = GetServerVariable;
    ecb.WriteClient = WriteClient;
    ecb.ReadClient = ReadClient;
    ecb.ServerSupportFunction = ServerSupportFunction;

    //Finally call the function with the structure
    DWORD dwSuccess = extension.m_lpHttpExtensionProc(&ecb);
    bSuccess = (dwSuccess == HSE_STATUS_SUCCESS);
    if (dwSuccess == HSE_STATUS_SUCCESS_AND_KEEP_CONN)
    {
      pClient->m_bResponseKeepAlive = TRUE;
      bSuccess = TRUE;
    }

    if (!bSuccess)
      ReportHttpExtensionProcError(pClient, extension, dwSuccess);
  }
  __except(1)
  {
    bSuccess = FALSE;
  }

  return bSuccess;
}
