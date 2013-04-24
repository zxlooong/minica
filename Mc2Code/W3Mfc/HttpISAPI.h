/*
Module : HttpISAPI.h
Purpose: Defines the interface for the CHttpISAPI class
Created: PJN / 21-02-2003

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

#ifndef __HTTPISAPI_H__
#define __HTTPISAPI_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif

//#include <afxisapi.h>


/////////////////////////////// Classes ///////////////////////////////////////

//forward declarations
class CHttpServer;
class CHttpClient;

class W3MFC_EXT_CLASS CHttpISAPIExtension
{
public:
//typedefs
  typedef BOOL (_stdcall GETEXTENSIONVERSION)(HSE_VERSION_INFO*);
  typedef GETEXTENSIONVERSION* LPGETEXTENSIONVERSION;
  typedef DWORD (_stdcall HTTPEXTENSIONPROC)(EXTENSION_CONTROL_BLOCK*);
  typedef HTTPEXTENSIONPROC* LPHTTPEXTENSIONPROC;
  typedef BOOL (_stdcall TERMINATEEXTENSION)(DWORD);
  typedef TERMINATEEXTENSION* LPTERMINATEEXTENSION;
  
//Constructors / Destructors
  CHttpISAPIExtension();
  virtual ~CHttpISAPIExtension();

//methods
  BOOL CallGetExtensionVersion(HSE_VERSION_INFO* phseVerInfo);
  BOOL CallTerminateExtension();

//Member variables
  HMODULE               m_hDLL;                  //The module instance;
  LPGETEXTENSIONVERSION m_lpGetExtensionVersion; //The GetExtensionVersion function
  LPHTTPEXTENSIONPROC   m_lpHttpExtensionProc;   //The HttpExtensionProc function
  LPTERMINATEEXTENSION  m_lpTerminateExtension;  //The TerminateExtension function
  CString               m_sPath;                 //The fully qualified path for the extension (just here for logging purposes)
};

//The values relating to a single directory which the web server handles
class W3MFC_EXT_CLASS CHttpISAPI
{
public:
//Constructors / Destructors
  CHttpISAPI(int nISAPIHashTableSize);
  virtual ~CHttpISAPI();

//Methods
  virtual CHttpISAPIExtension* CachedLoad(const CString& sPath);
  virtual BOOL                 UncachedLoad(const CString& sPath, CHttpISAPIExtension& extension);
  virtual BOOL                 CallHttpExtensionProc(CHttpClient* pClient, CHttpISAPIExtension& extension);
  void                         ReportHttpExtensionProcError(CHttpClient* pClient, CHttpISAPIExtension& extension, DWORD dwError);
  void                         FreeMapEntries();
  void                         SetServer(CHttpServer* pServer);
  static BOOL WINAPI           GetServerVariable(HCONN hConn, LPSTR lpszVariableName, void* lpBuffer, DWORD* lpdwBufferSize);
  static BOOL WINAPI           ServerSupportFunction(HCONN hConn, DWORD dwHSERRequest, LPVOID lpvBuffer, LPDWORD lpdwSize, LPDWORD lpdwDataType);
  static BOOL WINAPI           WriteClient(HCONN ConnID, LPVOID Buffer, LPDWORD lpdwBytes, DWORD dwSync);
  static BOOL WINAPI           ReadClient(HCONN hConn, LPVOID lpvBuffer, LPDWORD lpdwSize);

protected:
  CMapStringToPtr  m_mapExtensions; //The hash table which maintains filename to CHttpISAPIExtension mappings
  CHttpServer*     m_pServer;       //The controlling web server instance
  CCriticalSection m_CS;            //Thread protects access to the hash table from multiple threads
};

#endif //__HTTPISAPI_H__
