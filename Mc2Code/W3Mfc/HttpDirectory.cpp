/*
Module : HttpDirectory.cpp
Purpose: Implementation for the CHttpDirectory class
Created: PJN / 21-02-2003
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
#include "..\resource.h"
#include "HttpDirectory.h"
#include "HttpClient.h"



//////////////// Macros //////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

CHttpDirectory::CHttpDirectory()
{
  m_bDirectoryListing = FALSE;
  m_bScript = FALSE;
  m_bWritable = FALSE;
}

CHttpDirectory::~CHttpDirectory()
{
}

void CHttpDirectory::SetAlias(const CString& sAlias)
{
  //Must be some alias
  ASSERT(sAlias.GetLength());

  //Ensure the virtual directory begins with a "\"
  if (sAlias.Find(_T('\\')) == -1)
  {
    if (sAlias.Find(_T('/')) == 0)
      m_sAlias = _T('\\') + sAlias.Mid(1);
    else
      m_sAlias = _T('\\') + sAlias;
  }  
  else
    m_sAlias = sAlias;

  //Ensure there is a \ on the end of the directory if 
  //length is greater than 1
  int nLen = m_sAlias.GetLength();
  if (nLen > 1 && (m_sAlias.GetAt(nLen - 1) != _T('\\')))
    m_sAlias += _T('\\');
}

void CHttpDirectory::SetDirectory(const CString& sDirectory)
{
  m_sDirectory = sDirectory;  
}

void CHttpDirectory::SetDefaultFile(const CString& sDefaultFile)
{
  m_sDefaultFile = sDefaultFile;
}

BOOL CHttpDirectory::HandleDirectoryAuthorization(CHttpClient* pClient)
{
  //Validate our parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

  //Do we need to do virtual directory authentication
  if (m_sUsername.GetLength())
  {
    //If the username and password matches up
    if (m_sUsername == pClient->m_Request.m_sUsername && m_sPassword == pClient->m_Request.m_sPassword)
      return TRUE;
    else
    {
      if (pSettings->m_bAllowBasicAuthentication || pSettings->m_bAllowNTLMAuthentication)
      {
        //Fail the request with the challenge sent back to the client
        pClient->ReturnUnauthorizedMessage(m_sRealm.GetLength() ? m_sRealm : m_sAlias);
      }
      else
      {
        //Report the error
        CString sError;
        sError.Format(_T("CHttpDirectory::HandleDirectoryAuthorization, Directory is protected but all authentication mechanisms are disabled, All requests for this directory will fail!!, %s"), m_sAlias);
        pClient->m_pServer->OnError(sError);

        pClient->ReturnErrorMessage(500); //Internal server error
      }    

      return FALSE;
    }
  }
  else
    return TRUE;
}

void CHttpDirectory::TransmitDirectory(CHttpClient* pClient)
{
  //Validate our parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

	//For correct operation of the T2A macro, see MFC Tech Note 59
  USES_CONVERSION;

  //Look for all files in the specified directory
  CFileFind finder;
  CString strWildcard(pClient->m_Request.m_sLocalFile);
  strWildcard += _T("\\*.*");
  BOOL bWorking = finder.FindFile(strWildcard);
  if (bWorking)
  {
    //Load up the template of the body
    CString sTemp;
    sTemp.LoadString(IDS_DIRECTORY_LISTING_HEADER);
    CString sBody(sTemp);

    FILETIME ftLastModified;
    ZeroMemory(&ftLastModified, sizeof(FILETIME));
    BOOL bHaveLastModified = FALSE;

    TCHAR cLastChar = pClient->m_Request.m_sURL.GetAt(pClient->m_Request.m_sURL.GetLength()-1);
    CString sDirectoryURL;
    if (cLastChar != _T('\\') && (cLastChar != _T('/')))
      sDirectoryURL = pClient->m_Request.m_sURL + _T("/");
    else
      sDirectoryURL = pClient->m_Request.m_sURL;

    //Iterate through all the files in this directory
    sBody += _T("<table>\r\n");
    while (bWorking)
    {
      bWorking = finder.FindNextFile();
      if (!finder.IsDots())
      {
        //Get the last modified time for the file
        CString sLine;
        FILETIME ft;
        finder.GetLastWriteTime(&ft);
        if (CompareFileTime(&ft, &ftLastModified) > 0)
        {
          CopyMemory(&ftLastModified, &ft, sizeof(FILETIME));
          bHaveLastModified = TRUE;
        }

        //Form the URL of the file
        CString sFilename = finder.GetFileName();
        CString sEncodedFilename = CHttpClient::URLEncode(sFilename);
        CString sURL = sDirectoryURL + sEncodedFilename;

        //Get the last modified date as a string
        TCHAR sDate[20];
        SYSTEMTIME st;
        FileTimeToSystemTime(&ft, &st);
        GetDateFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &st, NULL, sDate, 20);

        //Get the last modified time as a string
        TCHAR sTod[20];
        GetTimeFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &st, NULL, sTod, 20);

        //Form all the info into a row of the table
        sLine.Format(_T("<tr>\r\n<td><a href=%s>%s</a></td><td>%dKB</td><td>%s</td><td>%s</td>\r\n</tr>"), 
                     sURL, sFilename, (finder.GetLength()+1023)/1024, sDate, sTod);
        sBody += sLine;
      }
    }
    finder.Close();
    sBody += _T("</table>\r\n");
    sTemp.LoadString(IDS_DIRECTORY_LISTING_FOOTER);
    sBody += sTemp;

    //replace any "%1"'s with m_Request.m_sURL
    sBody.Replace(_T("%1"), pClient->m_Request.m_sURL);

    //Make a local copy of the field we are going to parse
    char* pszBody = T2A((LPTSTR) (LPCTSTR) sBody);

    //Get the current system time in UTC
    SYSTEMTIME stCurTime;
    FILETIME ftCurTime;
    ::GetSystemTime(&stCurTime);
    ::SystemTimeToFileTime(&stCurTime, &ftCurTime);

    //Form the header of the response
    CHttpResponseHeader responseHdr;
    responseHdr.AddStatusCode(200);
    responseHdr.AddDate(stCurTime);
    responseHdr.AddServer(pSettings->m_sServerName);
    responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
    SYSTEMTIME stLastModified;
    if (bHaveLastModified && ::FileTimeToSystemTime(&ftLastModified, &stLastModified))
      responseHdr.AddLastModified(stLastModified);
    if (pClient->m_bResponseKeepAlive)
      responseHdr.AddKeepAlive();
    int nBodyLength = strlen(pszBody);
    responseHdr.AddContentLength(nBodyLength);
    responseHdr.AddContentType(_T("text/html"));

    //Send the header and body all in one
    pClient->TransmitBuffer(pClient->m_Socket, responseHdr, (BYTE*)pszBody, nBodyLength, pSettings->m_dwWritableTimeout);

    //Log the information
    pClient->PostLog(200, nBodyLength);
  }
  else
  {
    DWORD dwSize = pClient->ReturnErrorMessage(500); //Internal server error

    //Log the information
    pClient->PostLog(500, dwSize);
  }
}

void CHttpDirectory::TransmitFile(CHttpClient* pClient)
{
  //Validate our settings
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);
  ASSERT(pSettings->m_pMimeManager);

  //Variable used to generate the header response
  CHttpResponseHeader responseHdr;

  //Open the file (use the FILE_FLAG_SEQUENTIAL_SCAN to improve performance)
  HANDLE hFile = ::CreateFile(pClient->m_Request.m_sLocalFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    //Get the last modified time for the file / directory
    FILETIME ftFile;
    FILETIME ftRequest;
    SYSTEMTIME stFile;
    BOOL bHaveFileTime = ::GetFileTime(hFile, NULL, NULL, &ftFile);
    if (bHaveFileTime)
    {
      ::FileTimeToSystemTime(&ftFile, &stFile);

      //deliberately remove the milliseconds as they do not appear in HTTP responses.
      stFile.wMilliseconds = 0;
      SystemTimeToFileTime(&stFile, &ftFile);
    }

    //Get the current system time in UTC
    SYSTEMTIME stCurTime;
    FILETIME ftCurTime;
    ::GetSystemTime(&stCurTime);
    ::SystemTimeToFileTime(&stCurTime, &ftCurTime);

    //Ensure that the file time is not past the server time
    if (CompareFileTime(&ftFile, &ftCurTime) == 1)
    {
      CopyMemory(&ftFile, &ftCurTime, sizeof(FILETIME));
      CopyMemory(&stFile, &stCurTime, sizeof(SYSTEMTIME));
    }

    //Handle conditional GET of the file
    if (pClient->m_Request.m_Verb == CHttpRequest::HTTP_VERB_GET && 
        bHaveFileTime && pClient->m_Request.m_bIfModifiedSincePresent &&
        ::SystemTimeToFileTime(&pClient->m_Request.m_IfModifiedSince, &ftRequest) &&
        ::CompareFileTime(&ftFile, &ftRequest) != 1)
    {
      //Form the header
      responseHdr.AddStatusCode(304); //File Not modified
      responseHdr.AddDate(stCurTime);
      responseHdr.AddServer(pSettings->m_sServerName);
      responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
	    if (pSettings->m_bAutoExpire)
		    responseHdr.AddExpires(stFile);		
      if (pClient->m_bResponseKeepAlive)
        responseHdr.AddKeepAlive();
      responseHdr.AddLastModified(stFile);
      responseHdr.AddContentLength(0);
      responseHdr.AddContentType(_T("text/html"));

      //Send the header
#ifdef W3MFC_SSL_SUPPORT
      responseHdr.Send(pClient->m_Socket, pSettings->m_dwWritableTimeout, pClient->m_SSL);
#else
      responseHdr.Send(pClient->m_Socket, pSettings->m_dwWritableTimeout);
#endif

      //No body is sent for a 304 status

      //Log the information
      pClient->PostLog(304, 0);
    }
    else
    {
      //Get the length of the file
      DWORD dwFileLength = GetFileSize(hFile, NULL);

      if (pClient->m_Request.m_dwHttpVersion > MAKELONG(9, 0))
      {
        CString sMime = pSettings->m_pMimeManager->GetMimeType(pClient->m_Request);

        //Form the header of the response
        responseHdr.AddStatusCode(200);
        responseHdr.AddDate(stCurTime);
        responseHdr.AddServer(pSettings->m_sServerName);
        responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
	      if (pSettings->m_bAutoExpire)
		      responseHdr.AddExpires(stFile);		
        if (pClient->m_bResponseKeepAlive)
          responseHdr.AddKeepAlive();
        if (bHaveFileTime)
          responseHdr.AddLastModified(stFile);
        responseHdr.AddContentLength(dwFileLength);
        responseHdr.AddContentType(sMime);

        //Send back the file contents (if not a HEAD request)
        if (pClient->m_Request.m_Verb == CHttpRequest::HTTP_VERB_HEAD)
        {
          //Just send the header
#ifdef W3MFC_SSL_SUPPORT
          responseHdr.Send(pClient->m_Socket, pSettings->m_dwWritableTimeout, pClient->m_SSL);
#else
          responseHdr.Send(pClient->m_Socket, pSettings->m_dwWritableTimeout);
#endif
        }
        else
        {
          //Send the header and body all in one
          pClient->TransmitFile(pClient->m_Socket, responseHdr, hFile, dwFileLength, pSettings->m_dwWritableTimeout);
        }
      }
      else
      {
        //No header sent for Http 0.9

        //Send back the file contents (if not a HEAD request)
        if (pClient->m_Request.m_Verb != CHttpRequest::HTTP_VERB_HEAD)
        {
          try
          {
            char sBuf[4096];
            DWORD dwBytesRead = 0;
            do 
            {
              if (::ReadFile(hFile, sBuf, 4096, &dwBytesRead, NULL) && dwBytesRead)
#ifdef W3MFC_SSL_SUPPORT
                pClient->m_Socket.SendWithRetry(sBuf, dwBytesRead, pSettings->m_dwWritableTimeout, pClient->m_SSL);
#else
                pClient->m_Socket.SendWithRetry(sBuf, dwBytesRead, pSettings->m_dwWritableTimeout);
#endif
            } 
            while (dwBytesRead);
          }
          catch(CWSocketException* pEx)
          {
            //Report the error
            CString sError;
            sError.Format(_T("CHttpDirectory::TransmitFile, Failed to send to socket, Error:%d"), pEx->m_nError);
            pClient->m_pServer->OnError(sError);

            pEx->Delete();  
          }
        }
      }

      //Log the information
      pClient->PostLog(200, dwFileLength);
    }

    //Don't forget to close the file
    CloseHandle(hFile);
  }
  else
  {
    DWORD dwLastError = ::GetLastError();
    if (dwLastError == ERROR_ACCESS_DENIED && ((GetFileAttributes(pClient->m_Request.m_sLocalFile) & FILE_ATTRIBUTE_DIRECTORY) == 0) && 
        (pSettings->m_bAllowBasicAuthentication || 
         pSettings->m_bAllowNTLMAuthentication))
      pClient->ReturnUnauthorizedMessage(pClient->m_Request.m_sURL);
    else
      pClient->ReturnErrorMessage(404); //File not found
  }
}

void CHttpDirectory::HandleDirectory(CHttpClient* pClient, BOOL bDirectory)
{
  //Validate the parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);

  //Do the directory level authentication
  if (!HandleDirectoryAuthorization(pClient))
    return;

  if (m_bScript)
  {
    BOOL bSentSomething = FALSE;

  #ifndef W3MFC_NO_ISAPI_SUPPORT
    CString sDLL = pSettings->m_pISAPIManager->GetISAPIExtension(pClient->m_Request);
    if (sDLL.GetLength())
    {
      bSentSomething = TRUE;
      TransmitISAPIRequest(pClient, sDLL); //Do our ISAPI implementation
    }
  #endif  
  #ifndef W3MFC_NO_CGI_SUPPORT
    if (!bSentSomething)
    {
      bSentSomething = TRUE;
      pSettings->m_pCGI->TransmitCGIResponse(pClient); //Do our CGI implementation
    }
  #endif

    if (!bSentSomething)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpDirectory::HandleDirectoryAuthorization, Directory is a script directory but ISAPI and / or CGI is not supported in this configuration, All requests for this directory will fail!!, %s"), m_sAlias);
      pClient->m_pServer->OnError(sError);

      pClient->ReturnErrorMessage(500); //Internal server error
    }
  }
  else if (bDirectory)
    TransmitDirectory(pClient); //Return a directory listing back to the client
  else
    TransmitFile(pClient); //Return the file back to the client
}

#ifndef W3MFC_NO_ISAPI_SUPPORT
void CHttpDirectory::TransmitISAPIRequest(CHttpClient* pClient, const CString& sDLL)
{
  //Validate the parameters
  ASSERT(pClient);
  ASSERT(pClient->m_pServer);
  CHttpServerSettings* pSettings = pClient->m_pServer->GetSettings();
  ASSERT(pSettings);
  ASSERT(pSettings->m_pISAPI);

  //Reset the data counter
  pClient->m_dwDataSentViaWriteClient = 0;
  pClient->m_nHttpStatusCodeSent = 0;
  pClient->m_bResponseKeepAlive = FALSE;

  //Assume the worst
  BOOL bSuccess = FALSE;

  if (pSettings->m_bCacheISAPI)
  {
    CHttpISAPIExtension* pISAPI = pSettings->m_pISAPI->CachedLoad(sDLL);
    if (pISAPI)
      bSuccess = pSettings->m_pISAPI->CallHttpExtensionProc(pClient, *pISAPI);
  }
  else
  {
    CHttpISAPIExtension isapi;
    if (pSettings->m_pISAPI->UncachedLoad(sDLL, isapi))
      bSuccess = pSettings->m_pISAPI->CallHttpExtensionProc(pClient, isapi);
  }

  if (!bSuccess)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpDirectory::TransmitISAPIRequest, Failed calling the function HttpExtensionProc in the ISAPI extension %s"), sDLL);
    pClient->m_pServer->OnError(sError);

    pClient->ReturnErrorMessage(500); //Internal server error
  }
  else
    pClient->PostLog(pClient->m_nHttpStatusCodeSent, pClient->m_dwDataSentViaWriteClient);
}
#endif
