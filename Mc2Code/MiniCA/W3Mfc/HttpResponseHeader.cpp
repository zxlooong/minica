/*
Module : HttpResponseHeader.cpp
Purpose: Implementation for a class to simplify sending Http response headers
Created: PJN / 22-04-1999
History: None                    

Copyright (c) 1999 - 2005 by PJ Naughter.  

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
#include "HttpResponseHeader.h"



//////////////// Macros //////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

CHttpResponseHeader::CHttpResponseHeader()
{
  m_bEntitySeparator = TRUE;
}

void CHttpResponseHeader::SetAddEntitySeparator(BOOL bSeparator)
{
  m_bEntitySeparator = bSeparator;
}

void CHttpResponseHeader::AddStatus(LPCSTR pszStatusString)
{
  CString sLine;
  sLine.Format(_T("HTTP/1.0 %s\r\n"), pszStatusString);
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddExtraHeaders(LPCSTR pszHeaders)
{
  m_sHeader += pszHeaders;
}

void CHttpResponseHeader::AddStatusCode(int nStatusCode)
{
  CString sLine;
  switch (nStatusCode)
  {
    case 200: sLine = _T("HTTP/1.0 200 OK\r\n");                    break;
    case 201: sLine = _T("HTTP/1.0 201 Created\r\n");               break;
    case 202: sLine = _T("HTTP/1.0 202 Accepted\r\n");              break;
    case 204: sLine = _T("HTTP/1.0 204 No Content\r\n");            break;
    case 300: sLine = _T("HTTP/1.0 300 Multiple Choices\r\n");      break;
    case 301: sLine = _T("HTTP/1.0 301 Moved Permanently\r\n");     break;
    case 302: sLine = _T("HTTP/1.0 302 Moved Temporarily\r\n");     break;
    case 304: sLine = _T("HTTP/1.0 304 Not Modified\r\n");          break;
    case 400: sLine = _T("HTTP/1.0 400 Bad Request\r\n");           break;
    case 401: sLine = _T("HTTP/1.0 401 Unauthorized\r\n");          break;
    case 403: sLine = _T("HTTP/1.0 403 Forbidden\r\n");             break;
    case 404: sLine = _T("HTTP/1.0 404 Not Found\r\n");             break;
    case 500: sLine = _T("HTTP/1.0 500 Internal Server Error\r\n"); break;
    case 501: sLine = _T("HTTP/1.0 501 Not Implemented\r\n");       break;
    case 502: sLine = _T("HTTP/1.0 502 Bad Gateway\r\n");           break;
    case 503: sLine = _T("HTTP/1.0 503 Service Unavailable\r\n");   break;
    default: sLine.Format(_T("HTTP/1.0 %d\r\n"), nStatusCode);      break;
  }
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddContentLength(int nSize)
{
  CString sLine;
  sLine.Format(_T("Content-Length: %d\r\n"), nSize);
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddContentType(const CString& sMediaType)
{
  CString sLine;
  sLine.Format(_T("Content-Type: %s\r\n"), sMediaType);
  m_sHeader += sLine;
}

CString CHttpResponseHeader::DateToStr(const SYSTEMTIME& st)
{
  static TCHAR* sMonth[] =  
  {
    _T(""),
    _T("Jan"),
    _T("Feb"),
    _T("Mar"), 
    _T("Apr"),
    _T("May"),
    _T("Jun"),
    _T("Jul"),
    _T("Aug"),
    _T("Sep"),
    _T("Oct"),
    _T("Nov"),
    _T("Dec"),
  };

  static TCHAR* sDay[] =
  {
    _T("Sun"),
    _T("Mon"),
    _T("Tue"), 
    _T("Wed"),
    _T("Thu"),
    _T("Fri"),
    _T("Sat"),
  };

  CString sDate;
  sDate.Format(_T("%s, %02d %s %04d %02d:%02d:%02d GMT"), sDay[st.wDayOfWeek], 
               st.wDay, sMonth[st.wMonth], st.wYear, st.wHour, st.wMinute, st.wSecond);
  return sDate;
}

void CHttpResponseHeader::AddW3MfcAllowFields(BOOL bAllowDeleteRequest)
{
  if (bAllowDeleteRequest)
    m_sHeader += _T("Allow: GET, POST, HEAD, DELETE\r\n");
  else
    m_sHeader += _T("Allow: GET, POST, HEAD\r\n");
}

void CHttpResponseHeader::AddDate(const SYSTEMTIME& st)
{
  CString sDate = DateToStr(st);
  CString sLine;
  sLine.Format(_T("Date: %s\r\n"), sDate);
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddLastModified(const SYSTEMTIME& st)
{
  CString sDate = DateToStr(st);
  CString sLine;
  sLine.Format(_T("Last-Modified: %s\r\n"), sDate);
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddExpires(const SYSTEMTIME& st)
{
  CString sDate = DateToStr(st);
  CString sLine;
  sLine.Format(_T("Expires: %s\r\n"), sDate);
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddKeepAlive()
{
  m_sHeader += _T("Connection: Keep-Alive\r\n");
}

void CHttpResponseHeader::AddWWWAuthenticateBasic(const CString& sRealm)
{
  CString sLine;
  sLine.Format(_T("WWW-Authenticate: Basic realm=%s\r\n"), sRealm);
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddWWWAuthenticateNTLM(const CString& sMessage)
{
  CString sLine;
  if (sMessage.GetLength())
    sLine.Format(_T("WWW-Authenticate: NTLM %s\r\n"), sMessage);
  else
    sLine = _T("WWW-Authenticate: NTLM\r\n");
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddLocation(const CString& sLocation)
{
  CString sLine;
  sLine.Format(_T("Location: %s\r\n"), sLocation);
  m_sHeader += sLine;
}

void CHttpResponseHeader::AddServer(const CString& sServer)
{
  CString sLine;
  sLine.Format(_T("Server: %s\r\n"), sServer);
  m_sHeader += sLine;    
}

BOOL CHttpResponseHeader::Send(CHttpSocket& socket, DWORD dwTimeout)
{
  //Get the ascii version of the data
  DWORD dwSize = 0;
  char* pszData = GetData(dwSize);

  //Send it down the socket  
  BOOL bSuccess = TRUE; //assume the best
  try
  {
    socket.SendWithRetry(pszData, dwSize, dwTimeout);
  }
  catch(CWSocketException* pEx)
  {
    TRACE(_T("CHttpResponseHeader::Send, Error:%d\n"), pEx->m_nError);

    pEx->Delete();  
    bSuccess = FALSE;
  }

  //Tidy up the heap memory we have used
  delete [] pszData;

  return bSuccess;
}

#ifdef W3MFC_SSL_SUPPORT
BOOL CHttpResponseHeader::Send(CHttpSocket& socket, DWORD dwTimeout, CSSL& ssl)
{
  BOOL bSuccess = TRUE; //assume the best

  if (ssl.operator SSL*())
  {
    //Get the ascii version of the data
    DWORD dwSize = 0;
    char* pszData = GetData(dwSize);

    //Send it down the socket  
    try
    {
      socket.SendWithRetry(pszData, dwSize, dwTimeout, ssl);
    }
    catch(CWSocketException* pEx)
    {
      TRACE(_T("CHttpResponseHeader::Send, Error:%d\n"), pEx->m_nError);

      pEx->Delete();  
      bSuccess = FALSE;
    }

    //Tidy up the heap memory we have used
    delete [] pszData;
  }
  else
    bSuccess = Send(socket, dwTimeout);

  return bSuccess;
}
#endif

char* CHttpResponseHeader::GetData(DWORD& dwSize)
{
  //Allocate some string space from the heap
  int nLength = m_sHeader.GetLength();
#ifdef _UNICODE
	int nBytes = WideCharToMultiByte(CP_ACP, 0, m_sHeader, nLength, NULL, NULL, NULL, NULL);
  nBytes += 3;
  char* pszData = new char[nBytes];
  nBytes = WideCharToMultiByte(CP_ACP, 0, m_sHeader, nLength, pszData, nBytes, NULL, NULL);  
  pszData[nBytes] = '\0';
#else
  dwSize = nLength + 3;
  char* pszData = new char[dwSize];
  strcpy(pszData, m_sHeader);
#endif
  if (m_bEntitySeparator)
    strcat(pszData, "\r\n");
  dwSize = strlen(pszData);

  return pszData;
}
