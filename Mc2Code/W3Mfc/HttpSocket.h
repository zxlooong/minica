/*
Module : HttpSocket.h
Purpose: Defines the interface for a simple MFC socket wrapper class
Created: PJN / 22-04-1999

Copyright (c) 1999 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __HTTPSOCKET_H__
#define __HTTPSOCKET_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif



/////////////////////////////// Includes //////////////////////////////////////

#include "SocMFC.h"
#include "HttpRequest.h"
#include "W3MFC.h"
#ifdef W3MFC_SSL_SUPPORT
#include "OpenSSLMfc.h"
#endif
#include "WTimer.h" //If the code fails to compile because of this missing include then you need to download 
                    //the CWaitableTimer class from my web site and copy the relavent files to the W3MFC directory



/////////////////////////////// Classes ///////////////////////////////////////

class CHttpClient;

class W3MFC_EXT_CLASS CHttpSocket : public CWSocket
{
public:
//methods
  BOOL SplitRequestLine(LPSTR pszLine, CString& sField, CString& sValue);
  BOOL SplitRequestLine(const CString& sLine, CString& sField, CString& sValue);
  BOOL ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CHttpClient& client);
  BOOL ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CHttpClient& client, CWaitableTimer& timer, HANDLE hStopEvent, HANDLE hDataEvent);
  void SendWithRetry(void* pBuffer, int nBuf, DWORD dwTimeout);
#ifdef W3MFC_SSL_SUPPORT
  BOOL ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CSSL& ssl, CHttpClient& client);
  BOOL ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CSSL& ssl, CHttpClient& client, CWaitableTimer& timer, HANDLE hStopEvent, HANDLE hDataEvent);
  void SendWithRetry(void* pBuffer, int nBuf, DWORD dwTimeout, CSSL& ssl);
#endif

protected:
  CEvent m_WSABlockEvent;  //Used to implemented SocketSendWithRetry
};

#endif //__HTTPSOCKET_H__
