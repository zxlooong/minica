/*
Module : HttpResponseHeader.h
Purpose: Defines the interface for a class to simplify sending Http response headers
Created: PJN / 22-04-1999

Copyright (c) 1999 - 2005 by PJ Naughter.  

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __HTTPRESPONSEHEADER_H__
#define __HTTPRESPONSEHEADER_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif



/////////////////////////////// Includes //////////////////////////////////////

#include "HttpSocket.h"



/////////////////////////////// Classes ///////////////////////////////////////

//Class which is used to simplify forming and returning a header to the client
class W3MFC_EXT_CLASS CHttpResponseHeader
{
public:
//Constructors / Destructors
  CHttpResponseHeader();

//Methods
  void    AddStatusCode(int nStatusCode);
  void    AddStatus(LPCSTR pszStatusString);
  void    AddContentLength(int nSize);
  void    AddContentType(const CString& sMediaType);
  void    AddKeepAlive();
  void    AddDate(const SYSTEMTIME& st);
  void    AddLastModified(const SYSTEMTIME& st);
  void    AddWWWAuthenticateBasic(const CString& sRealm);
  void    AddWWWAuthenticateNTLM(const CString& sMessage);
  void    AddExpires(const SYSTEMTIME& st);
  void    AddLocation(const CString& sLocation);
  void    AddServer(const CString& sServer);
  void    AddW3MfcAllowFields(BOOL bAllowDeleteRequest);
  void    AddExtraHeaders(LPCSTR pszHeaders);
  void    SetAddEntitySeparator(BOOL bSeparator);

  CString DateToStr(const SYSTEMTIME& st);
#ifdef W3MFC_SSL_SUPPORT
  BOOL    Send(CHttpSocket& socket, DWORD dwTimeout, CSSL& ssl);
#endif
  BOOL    Send(CHttpSocket& socket, DWORD dwTimeout);
  char*   GetData(DWORD& dwSize);

protected:
  CString m_sHeader;
  BOOL    m_bEntitySeparator;
};

#endif //__HTTPRESPONSEHEADER_H__
