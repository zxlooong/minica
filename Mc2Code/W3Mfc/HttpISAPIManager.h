/*
Module : HttpISAPIManager.h
Purpose: Defines the interface for the ISAPI Manager classes
Created: PJN / 23-02-2003

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

#ifndef __HTTPISAPIMANAGER_H__
#define __HTTPISAPIMANAGER_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif



/////////////////////////////// Includes //////////////////////////////////////

#include "HttpRequest.h"



/////////////////////////////// Classes ///////////////////////////////////////

//Class which does ISAPI extension lookup based on the extension of the request
class W3MFC_EXT_CLASS CHttpISAPIManager
{
public:
//Methods
  virtual CString GetISAPIExtension(const CHttpRequest& request) = 0;
};



//Class which does ISAPI extension lookups based on a section in an ini file e.g. "W3MFC.ini"
class W3MFC_EXT_CLASS CIniHttpISAPIManager : public CHttpISAPIManager
{
public:
//constructors / Destructors
  CIniHttpISAPIManager();
  virtual ~CIniHttpISAPIManager();

//Methods
  int Initialize(const CString& sIniPath, const CString& sSection);
  virtual CString GetISAPIExtension(const CHttpRequest& request);

protected:
//Methods
  void FullArraysFromMultiSZString(LPTSTR pszString);

//Member variables
  CMapStringToString m_sExtensionMap;
  CCriticalSection   m_CS;
};

#endif //__HTTPISAPIMANAGER_H__
