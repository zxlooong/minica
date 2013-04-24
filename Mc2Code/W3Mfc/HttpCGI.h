/*
Module : HttpCGI.h
Purpose: Defines the interface for the CHttpCGI class
Created: PJN / 26-02-2003

Copyright (c) 2003 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __HTTPCGI_H__
#define __HTTPCGI_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif



/////////////////////////////// Classes ///////////////////////////////////////

/// Forward declaration
class CHttpClient;


class W3MFC_EXT_CLASS CHttpCGI
{
public:
//Constructors / Destructors
  CHttpCGI();
  virtual ~CHttpCGI();

//Virtual methods
  virtual void    TransmitCGIResponse(CHttpClient* pClient);
  virtual CString FormCGIEnvironment(CHttpClient* pClient);
  virtual CString GetCGICommandLine(CHttpClient* pClient);
  virtual DWORD   ReadFromClientStdout(CHttpClient* pClient, HANDLE hChildStdout, BOOL& bFoundKeepAlive);
  virtual void    WriteToChildStdin(CHttpClient* pClient, HANDLE hChildStdin);
};

#endif //__HTTPCGI_H__
