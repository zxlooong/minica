/*
Module : HttpCGI.cpp
Purpose: Implementation for the CHttpCGI class
Created: PJN / 26-02-2003
History: None

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
#include "HttpCGI.h"
#include "Win32Handle.h"
#include "..\resource.h"
#include "HttpClient.h"



//////////////// Macros //////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

CHttpCGI::CHttpCGI()
{
}

CHttpCGI::~CHttpCGI()
{
}

void CHttpCGI::TransmitCGIResponse(CHttpClient* pClient)
{
  //Validate our parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);

  //Check to make sure the file exists become we try to run it
  CFileStatus fs;
  if (!CFile::GetStatus(pClient->m_Request.m_sLocalFile, fs))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpCGI::TransmitCGIResponse, Requested file %s does not exist"), pClient->m_Request.m_sLocalFile);
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //First need to create an anonymous pipe for the child process's redirected 
  //STDOUT
  SECURITY_ATTRIBUTES sa;
  sa.nLength= sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;
  CW32Handle ChildStdoutReadTemp;
  CW32Handle ChildStdoutWrite;
  if (!CreatePipe(&ChildStdoutReadTemp, &ChildStdoutWrite, &sa, 0)) 
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpCGI::TransmitCGIResponse, Failed to create anonymous pipes for STDOUT for CGI child process, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //Next need to create an anonymous pipe for the child process's redirected 
  //STDIN
  CW32Handle ChildStdinRead;
  CW32Handle ChildStdinWriteTemp;
  if (pClient->m_Request.m_dwRawEntitySize)
  {
    if (!CreatePipe(&ChildStdinRead, &ChildStdinWriteTemp, &sa, 0)) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpCGI::TransmitCGIResponse, Failed to create anonymous pipes for STDIN for CGI child process, Error:%d"), GetLastError());
      pClient->m_pServer->OnError(sError);

      //Handle the error
      pClient->ReturnErrorMessage(500);
      return;
    }
  }

  //Create a duplicate of the child's STDOUT write handle for the STDERR write handle.
  //This is necessary in case the child application closes one of its std output handles
  CW32Handle ChildStderrWrite;
  if (!DuplicateHandle(GetCurrentProcess(), ChildStdoutWrite, GetCurrentProcess(), &ChildStderrWrite, 0, TRUE, DUPLICATE_SAME_ACCESS))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpCGI::TransmitCGIResponse, Failed to duplicate STDOUT handle for STDERR handle CGI child process, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //Create new STDOUT read handle (which is non-inheritable). if we did not do this then the child inherits the 
  //properties and, as a result non-closeable handles to the pipes are created
  CW32Handle ChildStdoutRead;
  if (!DuplicateHandle(GetCurrentProcess(), ChildStdoutReadTemp, GetCurrentProcess(), &ChildStdoutRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpCGI::TransmitCGIResponse, Failed to duplicate STDOUT read handle, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //Close the temp STDOUT read handle now that we have duplicated it successfully
  ChildStdoutReadTemp.Close();

  //Create new write handle (if we need to which is non-inheritable). if we did not do this then the child inherits the 
  //properties and, as a result non-closeable handles to the pipes are created
  CW32Handle ChildStdinWrite;
  if (pClient->m_Request.m_dwRawEntitySize)
  {
    if (!DuplicateHandle(GetCurrentProcess(), ChildStdinWriteTemp, GetCurrentProcess(), &ChildStdinWrite, 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpCGI::TransmitCGIResponse, Failed to duplicate STDIN write handle, Error:%d"), GetLastError());
      pClient->m_pServer->OnError(sError);

      //Handle the error
      pClient->ReturnErrorMessage(500);
      return;
    }

    //Close the temp STDIN write handle now that we have duplicated it successfully
    ChildStdinWriteTemp.Close();
  }

  //Now setup the structures for a call to CreateProcess
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  ZeroMemory(&si,sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.hStdOutput = ChildStdoutWrite;
  si.hStdInput  = ChildStdinRead;
  si.hStdError  = ChildStderrWrite;
  si.wShowWindow = SW_HIDE;

  //Get the environment variables we will be sending down to the CGI process
  CString sEnvironment = FormCGIEnvironment(pClient);

  //Convert the "sEnvironment" CString into a MULTI_SZ suitable for calling CreateProcess with
  DWORD dwEnvironmentSize = (sEnvironment.GetLength() + 2);
  TCHAR* pszEnvironment = new TCHAR[dwEnvironmentSize];
  _tcscpy(pszEnvironment, sEnvironment);
  pszEnvironment[dwEnvironmentSize-1] = _T('\0'); //Double NULL terminate the data
  //Replace all '\n' with '\0'
  for (DWORD i=0; i<dwEnvironmentSize; i++)
  {
    if (pszEnvironment[i] == _T('\n'))
      pszEnvironment[i] = _T('\0');
  }
 
  //Setup the creation flags
  DWORD dwCreationFlags = CREATE_NEW_CONSOLE;
  #ifdef _UNICODE
  dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
  #endif

  //Get the working directory of the script
  TCHAR sDrive[_MAX_DRIVE];
  TCHAR sDir[_MAX_DIR];
  _tsplitpath(pClient->m_Request.m_sLocalFile, sDrive, sDir, NULL, NULL);
  TCHAR sCWD[_MAX_PATH];
  _tmakepath(sCWD, sDrive, sDir, NULL, NULL);

  //Form the correct command line for the call to CreateProcess
  CString sCommandLine = GetCGICommandLine(pClient);

  //Launch the process that we want to redirect
  BOOL bSuccess = CreateProcess(NULL, sCommandLine.GetBuffer(sCommandLine.GetLength()), NULL, NULL, TRUE, dwCreationFlags, pszEnvironment, sCWD, &si, &pi);
  DWORD dwError = GetLastError();
  sCommandLine.ReleaseBuffer();

  //Tidy up the temp heap memory we have used
  delete [] pszEnvironment;

  //Close the STDOUT write handle as we have no use for it now
  ChildStdoutWrite.Close();

  //Close the STDIN read handle as we have no use for it now
  if (pClient->m_Request.m_dwRawEntitySize)
    ChildStdinRead.Close();

  //Close the STDERR write handle as we have no use for it now
  ChildStderrWrite.Close();

  //Handle the error if we could not run the child process
  if (!bSuccess)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpCGI::TransmitCGIResponse, Failed to run the CGI child process, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Return an appropriate error page
    if (dwError == ERROR_FILE_NOT_FOUND)
      pClient->ReturnErrorMessage(404);
    else
      pClient->ReturnErrorMessage(500);
    return;
  }

  //Write to stdin of the client
  if (pClient->m_Request.m_dwRawEntitySize)
  {
    WriteToChildStdin(pClient, ChildStdinWrite);

    //Close the pipe handle now that we are finished with it
    ChildStdinWrite.Close();
  }

  //Read from stdout of the client and send the data back down the socket
  DWORD dwDataSent = ReadFromClientStdout(pClient, ChildStdoutRead, pClient->m_bResponseKeepAlive);
  if (dwDataSent == 0)
    pClient->ReturnErrorMessage(500);
  else
    pClient->PostLog(200, dwDataSent);

  //Close the anonymous pipe handles
  ChildStdoutRead.Close();

  //Wait until the child process exits
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles.     
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

CString CHttpCGI::FormCGIEnvironment(CHttpClient* pClient)
{
  //Validate our parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

  //Form the environment string to send to the child process
  CString sEnvironment;
  CString sLine;

  //Add all the environment variables W3MFC has before we add the custom ones
  for (TCHAR** szVariable = _tenviron; *szVariable; szVariable++ )
  {
    sEnvironment += *szVariable;
    sEnvironment += _T("\n");
  }    

  if (pClient->m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_PLAINTEXT)
  {
    sEnvironment += _T("AUTH_TYPE=Basic\n");

    sLine.Format(_T("AUTH_REALM=%s\n"), pClient->m_Request.m_sURL);
    sEnvironment += sLine;

    sLine.Format(_T("REMOTE_USER=%s\n"), pClient->m_Request.m_sUsername);
    sEnvironment += sLine;

    sLine.Format(_T("AUTH_USER=%s\n"), pClient->m_Request.m_sUsername);
    sEnvironment += sLine;

    sLine.Format(_T("REMOTE_PASSWORD=%s\n"), pClient->m_Request.m_sPassword);
    sEnvironment += sLine;

    sLine.Format(_T("AUTH_PASSWORD=%s\n"), pClient->m_Request.m_sPassword);
    sEnvironment += sLine;
  }
  else if (pClient->m_Request.m_AuthorizationType == CHttpRequest::HTTP_AUTHORIZATION_NTLM)
  {
    sEnvironment += _T("AUTH_TYPE=NTLM\n");

    sLine.Format(_T("REMOTE_USER=%s\n"), pClient->m_Request.m_sUsername);
    sEnvironment += sLine;

    sLine.Format(_T("AUTH_USER=%s\n"), pClient->m_Request.m_sUsername);
    sEnvironment += sLine;
  }

  sEnvironment += _T("GATEWAY_INTERFACE=CGI/1.1\n");

  if (pSettings->m_sBindAddress.GetLength())
    sLine.Format(_T("SERVER_NAME=%s\n"), pSettings->m_sBindAddress);
  else
  {
    char pszHostName[256];
    if (gethostname(pszHostName, 256) == 0)
    {
      CString sName(pszHostName);
      sLine.Format(_T("SERVER_NAME=%s\n"), sName);
    }
    else
      sLine = (_T("SERVER_NAME=\n"));
  }
  sEnvironment += sLine;

  if (pClient->m_Request.m_nContentLength)
  {
    sLine.Format(_T("CONTENT_LENGTH=%d\n"), pClient->m_Request.m_nContentLength);
    sEnvironment += sLine;
  }

  if (pClient->m_Request.m_sContentType.GetLength())
  {
    sLine.Format(_T("CONTENT_TYPE=%s\n"), pClient->m_Request.m_sContentType);
    sEnvironment += sLine;
  }

  sLine.Format(_T("SERVER_PORT=%d\n"), pSettings->m_nPort);
  sEnvironment += sLine;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol == CHttpServerSettings::SSL_NONE)
    sEnvironment += _T("SERVER_PROTOCOL=HTTP/1.0\n");
  else
    sEnvironment += _T("SERVER_PROTOCOL=HTTPS/1.0\n");
#else
  sEnvironment += _T("SERVER_PROTOCOL=HTTP/1.0\n");
#endif

  sLine.Format(_T("SERVER_SOFTWARE=%s\n"), pSettings->m_sServerName);
  sEnvironment += sLine;

  CString sScriptName = pClient->m_Request.m_sLocalFile;
  sScriptName.Replace(_T('\\'), _T('/')); //Ensure we use unix directory separators
  if (sScriptName.GetLength() && sScriptName.GetAt(0) != _T('/'))
    sScriptName = _T("/") + sScriptName;
  sLine.Format(_T("SCRIPT_NAME=%s\n"), sScriptName);
  sEnvironment += sLine;

  //Form the absolute path for PATH_TRANSLATED
  LPTSTR pszFilePart;
  TCHAR pszAbsolutePath[_MAX_PATH];
  GetFullPathName(pClient->m_Request.m_sLocalFile, _MAX_PATH, pszAbsolutePath, &pszFilePart);
  sLine.Format(_T("PATH_TRANSLATED=%s\n"), pszAbsolutePath);
  sEnvironment += sLine;

  sLine.Format(_T("REQUEST_LINE=%s\n"), pClient->m_Request.m_sRequest);
  sEnvironment += sLine;

  sLine.Format(_T("QUERY_STRING=%s\n"), pClient->m_Request.m_sRawExtra);
  sEnvironment += sLine;
  
  if (!pClient->m_Request.m_sRemoteHost.IsEmpty())
  {
    sLine.Format(_T("REMOTE_HOST=%s\n"), pClient->m_Request.m_sRemoteHost);
    sEnvironment += sLine;
  }
  else
  {
    sLine.Format(_T("REMOTE_HOST=%d.%d.%d.%d\n"), pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
                 pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3,
                 pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4);
    sEnvironment += sLine;
  }

  sLine.Format(_T("REMOTE_ADDR=%d.%d.%d.%d\n"), pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
               pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3,
               pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4);
  sEnvironment += sLine;

  sLine.Format(_T("PATH_INFO=%s\n"), pClient->m_Request.m_sPathInfo);
  sEnvironment += sLine;

  sLine.Format(_T("REQUEST_METHOD=%s\n"), pClient->m_Request.m_sVerb);
  sEnvironment += sLine;

  int nKeySize = 0;
  if (pClient->GetKeySizeServerVariable(nKeySize))
  {
    sLine.Format(_T("HTTPS_KEYSIZE=%d\nCERT_KEYSIZE=%d\n"), nKeySize, nKeySize);
    sEnvironment += sLine;
  }

  if (pClient->GetServerKeySizeServerVariable(nKeySize))
  {
    sLine.Format(_T("HTTPS_SECRETKEYSIZE=%d\nCERT_SECRETKEYSIZE=%d\n"), nKeySize, nKeySize);
    sEnvironment += sLine;
  }

  sEnvironment += sLine;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol == CHttpServerSettings::SSL_NONE)
    sLine = _T("HTTPS=off\n");
  else
    sLine = _T("HTTPS=on\n");
#else
  sLine = _T("HTTPS=off\n");
#endif
  sEnvironment += sLine;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
  {
    sLine.Format(_T("SERVER_PORT_SECURE=%d\n"), pSettings->m_nPort);
    sEnvironment += sLine;
  }
#endif

  long nSerialNumber = 0;
  if (pClient->GetCertSerialNumberServerVariable(nSerialNumber))
  {
    sLine.Format(_T("CERT_SERIALNUMBER=%d\n"), nSerialNumber);
    sEnvironment += sLine;
  }

  sLine.Format(_T("REQUEST_URI=%s\n"), pClient->m_Request.m_sURL);
  sEnvironment += sLine;

  sLine.Format(_T("URL=%s\n"), pClient->m_Request.m_sURL);
  sEnvironment += sLine;

  if (pClient->m_Request.m_bLoggedOn)
  {
    sLine.Format(_T("LOGON_USER=%s\n"), pClient->m_Request.m_sUsername);
    sEnvironment += sLine;
  }
  else if (pSettings->m_sUsername.GetLength())
  {
    sLine.Format(_T("LOGON_USER=%s\n"), pSettings->m_sUsername);
    sEnvironment += sLine;
  }

  char* szIssuer = NULL;
  if (pClient->GetCertIssuerServerVariable(szIssuer))
  { 
    CString sIssuer(szIssuer);
    sLine.Format(_T("CERT_SERVER_ISSUER=%s\nHTTPS_SERVER_ISSUER=%s\n"), sIssuer, sIssuer);
    sEnvironment += sLine;
  }

  char* szSubject = NULL;
  if (pClient->GetCertSubjectServerVariable(szSubject))
  { 
    CString sSubject(szSubject);
    sLine.Format(_T("CERT_SERVER_SUBJECT=%s\nHTTPS_SERVER_SUBJECT=%s\n"), sSubject, sSubject);
    sEnvironment += sLine;
  }

  sLine.Format(_T("HTTP_VERSION=%d.%d\n"), HIWORD(pClient->m_Request.m_dwHttpVersion), LOWORD(pClient->m_Request.m_dwHttpVersion));
  sEnvironment += sLine;

  POSITION posMap = pClient->m_Request.m_HeaderMap.GetStartPosition();
  while (posMap)
  {
    CString sKey;
    CString sValue;
    pClient->m_Request.m_HeaderMap.GetNextAssoc(posMap, sKey, sValue);

    sLine.Format(_T("HTTP_%s=%s\n"), sKey, sValue);
    sEnvironment += sLine;
  }

  return sEnvironment;
}

CString CHttpCGI::GetCGICommandLine(CHttpClient* pClient)
{
  //What will be the return value
  CString sCommandLine(pClient->m_Request.m_sLocalFile);

  //First get the extension of the file
  TCHAR sExt[_MAX_EXT];
  _tsplitpath(pClient->m_Request.m_sLocalFile, NULL, NULL, NULL, sExt);

  //Now get the command line for this extension from the registry
  TCHAR pszValue[1024/sizeof(TCHAR)];
  DWORD nData = 1024;
  HKEY hExtKey = NULL;
  if (RegOpenKeyEx(HKEY_CLASSES_ROOT, sExt, 0, KEY_READ, &hExtKey) == ERROR_SUCCESS)
  {
	  if (RegQueryValueEx(hExtKey, NULL, NULL, NULL, (LPBYTE)pszValue, &nData) == ERROR_SUCCESS)
	  {
      //Close the registry key now that we are finished with it
      RegCloseKey(hExtKey);

		  //pszValue is now the Extension File Subkey to use.
		  CString sValue;
		  sValue.Format(_T("%s\\Shell\\Open\\Command"), pszValue);
		  nData = 1024; //reset the size parameter

      HKEY hExtensionFileKey = NULL;
      if (RegOpenKeyEx(HKEY_CLASSES_ROOT, sValue, 0, KEY_READ, &hExtensionFileKey) == ERROR_SUCCESS)
		  {
 			  if (RegQueryValueEx(hExtensionFileKey, NULL, NULL, NULL, (LPBYTE)pszValue, &nData) == ERROR_SUCCESS)
			  {
          //Expand the string with any environment variables it may contain
          nData = 1024;
					LPTSTR szBuf = sCommandLine.GetBuffer(nData);
					DWORD  dwRet = ExpandEnvironmentStrings(pszValue, szBuf, nData); 
					sCommandLine.ReleaseBuffer();

          if (dwRet != 0)
          {
			      sCommandLine.Replace(_T("%L"), pClient->m_Request.m_sLocalFile);
			      sCommandLine.Replace(_T("%1"), pClient->m_Request.m_sLocalFile);
			      sCommandLine.Replace(_T("%*"), _T("")); 
          }
			  }

        //Close the registry key now that we are finished with it
			  RegCloseKey(hExtensionFileKey);
		  }
	  }
  }

  return sCommandLine;
}

void CHttpCGI::WriteToChildStdin(CHttpClient* pClient, HANDLE hChildStdin)
{
  //Write the entity body to STDIN of the CGI child process
  DWORD dwWritten;
  ASSERT(pClient->m_Request.m_pRawEntity);
  WriteFile(hChildStdin, pClient->m_Request.m_pRawEntity, pClient->m_Request.m_dwRawEntitySize, &dwWritten, NULL);
}

DWORD CHttpCGI::ReadFromClientStdout(CHttpClient* pClient, HANDLE hChildStdout, BOOL& bFoundKeepAlive) 
{ 
  //Validate our parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

  //Initialize the out parameters
  bFoundKeepAlive = FALSE;
  DWORD dwDataSent = 0;

  //Allocate the receive / re transmit buffer
  char* pszBuf = (char*) alloca(pSettings->m_dwCGIResponseBufferSize); 

  //Read output from the child process, and write to parent's STDOUT. 
  BOOL bMore = TRUE;
  BOOL bFirstBuffer = TRUE;
  while (bMore) 
  { 
    DWORD dwRead;
    if (!ReadFile(hChildStdout, pszBuf, pSettings->m_dwCGIResponseBufferSize-1, &dwRead, NULL) || dwRead == 0) 
      bMore = FALSE; 
    else
    {
      //Used as a return value to indicate how much was sent to the client
      dwDataSent += dwRead;
      BOOL bFoundHTTPReturnCodeLine = FALSE;

      if (bFirstBuffer)
      {
        //NULL terminate the data (so that we can use strstr)
        pszBuf[dwRead] = '\0';

        //See if the "Connection: Keep Alive" is being transmitted
        bFoundKeepAlive = (strstr(pszBuf, "Connection: Keep-Alive") != NULL);
        if (bFoundKeepAlive)
        {
          //Note it is important that "Content-Length" be also sent when using keep alives
          //as otherwise client browsers do not know when to stop reading to get the end
          //of the first response
          bFoundKeepAlive = (strstr(pszBuf, "Content-Length: ") != NULL);
        }
        
        //Do we have a HTTP return code line
        bFoundHTTPReturnCodeLine = (strstr(pszBuf,"HTTP/") != NULL);

        //We only do this parse on the first buffer
        bFirstBuffer = FALSE;
      }

      //Send the data back down the socket
      try
      {
 			  char* pszTTPReturnCodeLine = "HTTP/1.0 200 OK\n";
#ifdef W3MFC_SSL_SUPPORT
        if (!bFoundHTTPReturnCodeLine)
			    pClient->m_Socket.SendWithRetry(pszTTPReturnCodeLine, strlen(pszTTPReturnCodeLine), pSettings->m_dwWritableTimeout, pClient->m_SSL);
        pClient->m_Socket.SendWithRetry(pszBuf, dwRead, pSettings->m_dwWritableTimeout, pClient->m_SSL);
#else
        if (!bFoundHTTPReturnCodeLine)
			    pClient->m_Socket.SendWithRetry(pszTTPReturnCodeLine, strlen(pszTTPReturnCodeLine), pSettings->m_dwWritableTimeout);
        pClient->m_Socket.SendWithRetry(pszBuf, dwRead, pSettings->m_dwWritableTimeout);
#endif
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CHttpCGI::ReadFromClientStdout, Failed to send to socket, Error:%d"), pEx->m_nError);
        pClient->m_pServer->OnError(sError);

        pEx->Delete();  

        bMore = FALSE;
      }
    }
  } 

  return dwDataSent;
} 
