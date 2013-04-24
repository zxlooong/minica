/*
Module : HttpMimeManager.h
Purpose: Defines the interface for the Mime Manager classes
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

#ifndef __HTTPMIMEMANAGER_H__
#define __HTTPMIMEMANAGER_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif



/////////////////////////////// Includes //////////////////////////////////////

#include "HttpRequest.h"



/////////////////////////////// Classes ///////////////////////////////////////


//Base class which you derive from to implement a Mime Lookup manager i.e.
//a class which encapsulates retrieval of MIME types given a filename 
//extension
class W3MFC_EXT_CLASS CHttpMimeManager
{
public:
  virtual CString GetMimeType(const CHttpRequest& request) = 0;
};



//Class which does mime lookups based on the Win32 registry mime settings
class W3MFC_EXT_CLASS CRegistryHttpMimeManager : public CHttpMimeManager
{
public:
//constructors / Destructors
  CRegistryHttpMimeManager();
  virtual ~CRegistryHttpMimeManager();

//Methods
  virtual CString GetMimeType(const CHttpRequest& request);

protected:
  CMapStringToString m_sMimeMap;
  CCriticalSection m_CS;
};



//Class which does mime lookups based on a section in an ini file e.g. "W3MFC.ini"
class W3MFC_EXT_CLASS CIniHttpMimeManager : public CHttpMimeManager
{
public:
//constructors / Destructors
  CIniHttpMimeManager();
  virtual ~CIniHttpMimeManager();

//Methods
  int Initialize(const CString& sIniPath, const CString& sSection);
  virtual CString GetMimeType(const CHttpRequest& request);

protected:
//Methods
  void FullArraysFromMultiSZString(LPTSTR pszString);

//Member variables
  CMapStringToString m_sMimeMap;
  CCriticalSection   m_CS;
};

#endif //__HTTPMIMEMANAGER_H__
