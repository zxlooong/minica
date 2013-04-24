/*
Module : HttpClient.h
Purpose: Defines the interface for the CHttpClient classes
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

#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif



/////////////////////////////// Includes //////////////////////////////////////

#include "HttpSocket.h"
#include "ThrdPool.h"
#include "W3Mfc.h"
#include "HttpResponseHeader.h"



/////////////////////////////// Classes ///////////////////////////////////////

//Class which represents the thread pool request
class W3MFC_EXT_CLASS CHttpThreadPoolRequest
{
public:
  sockaddr_in m_ClientAddress;
  CHttpSocket m_ClientSocket;
#ifdef W3MFC_SSL_SUPPORT
  CSSLContext* m_pSSLContext;
#endif
};



//forward declarations
class CHttpServer;
class CHttpClient;



//Class which handles dynamically calling function which must be constructed at run time
//since we do not want to depend on the MS extensions to Winsock in mswsock.dll. To avoid 
//the loader bringing up a message such as "Failed to load due to missing export...", the
//function is constructed using GetProcAddress. The W3MFC code then checks to see if these 
//function pointers are NULL.

class W3MFC_EXT_CLASS _W3MFC_DATA
{
public:
//Constructors /Destructors
  _W3MFC_DATA();
  ~_W3MFC_DATA();

//typedefs
  typedef struct _TRANSMIT_FILE_BUFFERS 
  {
    LPVOID Head;
    DWORD HeadLength;
    LPVOID Tail;
    DWORD TailLength;
  } TRANSMIT_FILE_BUFFERS, *PTRANSMIT_FILE_BUFFERS, FAR *LPTRANSMIT_FILE_BUFFERS;

  typedef BOOL (__stdcall TRANSMITFILE)(SOCKET, HANDLE, DWORD, DWORD, LPOVERLAPPED, LPTRANSMIT_FILE_BUFFERS, DWORD);
  typedef TRANSMITFILE* LPTRANSMITFILE;

#ifndef TP_ELEMENT_MEMORY
  #define TP_ELEMENT_MEMORY   1
#endif

  typedef struct _TRANSMIT_PACKETS_ELEMENT 
  { 
    ULONG dwElFlags; 
    ULONG cLength; 
    union 
    {
      typedef struct _HDATA
      {
        LARGE_INTEGER nFileOffset;
        HANDLE        hFile;
      };
      PVOID pBuffer;
    };
  } TRANSMIT_PACKETS_ELEMENT, *PTRANSMIT_PACKETS_ELEMENT, FAR *LPTRANSMIT_PACKETS_ELEMENT;

  typedef BOOL (__stdcall TRANSMITPACKETS)(SOCKET, LPTRANSMIT_PACKETS_ELEMENT, DWORD, DWORD, LPOVERLAPPED, DWORD);
  typedef TRANSMITPACKETS* LPTRANSMITPACKETS;

//Defines
#ifndef TF_USE_KERNEL_APC
  #define TF_USE_KERNEL_APC     0x20
#endif

//Methods
  BOOL TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize);
  BOOL TransmitBuffer(CHttpSocket& socket, CHttpResponseHeader& responseHdr, BYTE* byData, DWORD dwSize);
#ifndef W3MFC_NO_ISAPI_SUPPORT
  BOOL TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HSE_TF_INFO* pInfo);
#endif

//Data
  HINSTANCE         m_hMSWsock;
  LPTRANSMITFILE    m_lpfnTransmitFile;
  LPTRANSMITPACKETS m_lpfnTransmitPackets;
};



//Class which handles the HTTP client connection
class W3MFC_EXT_CLASS CHttpClient : public CThreadPoolClient
{
public:
//Constructors / Destructors
  CHttpClient();
  virtual ~CHttpClient();

//Methods
  virtual BOOL  Run(const CThreadPoolRequest& request);
  virtual int   ExitInstance();
  virtual DWORD ReturnErrorMessage(int nStatusCode);
  virtual DWORD ReturnRedirectMessage(const CString& sURL);
  virtual DWORD ReturnFileDeletedOkMessage(const CString& sFile);
  virtual DWORD ReturnUnauthorizedMessage(const CString& sRealm);
  virtual void  PostLog(int nHTTPStatusCode, DWORD dwBodyLength);
  virtual BOOL  GetKeySizeServerVariable(int& nKeySize);
  virtual BOOL  GetServerKeySizeServerVariable(int& nKeySize);
  virtual BOOL  GetCertSerialNumberServerVariable(long& nSerialNumber);
  virtual BOOL  GetCertIssuerServerVariable(char*& szIssuer);
  virtual BOOL  GetCertSubjectServerVariable(char*& szSubject);
  virtual BOOL  TransmitBuffer(CHttpSocket& socket, CHttpResponseHeader& responseHdr, BYTE* byData, DWORD dwSize, DWORD dwTimeout);
  virtual BOOL  TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize, DWORD dwTimeout);
#ifndef W3MFC_NO_ISAPI_SUPPORT
  virtual BOOL  TransmitFile(CHttpSocket& socket, CHttpResponseHeader& responseHdr, HSE_TF_INFO* pInfo);
#endif
  virtual BOOL  MapURLToLocalFilename(CString& sURL, CString& sLocalFile, CString& sPathInfo, BOOL& bDirectory, CHttpDirectory*& pDirectory, DWORD& dwMatchingURL, DWORD& dwMatchingPath);
  virtual void  SetRequestToStop();
  virtual BOOL  PreHandleGetPostHead();

//Static Methods
  static int     HexToInt(TCHAR ch);
  static TCHAR   IntToHex(int Character);
  static CString URLDecode(const CString& sURL);
  static CString URLEncode(const CString& sURL);
  static CString UTF8ToCString(const CString& sURL);
  static BOOL    ParseDate(const CString& sField, SYSTEMTIME& time);
  static BOOL    ParseWeekDay(char* pszToken, int& nWeekDay);
  static BOOL    ParseMonth(char* pszToken, int& nMonth);

//Member variables
  CHttpServer* m_pServer;
  CHttpRequest m_Request;
  BOOL         m_bResponseKeepAlive;
  CHttpSocket  m_Socket;
#ifndef W3MFC_NO_ISAPI_SUPPORT
  DWORD        m_dwDataSentViaWriteClient;
  int          m_nHttpStatusCodeSent;
#endif
#ifdef W3MFC_SSL_SUPPORT
  CSSLContext* m_pSSLContext;
  CSSL m_SSL;
#endif

protected:
//Methods
  virtual BOOL            ParseRequest();
  virtual BOOL            ParseSimpleRequestLine(const CString& sLine);
  virtual BOOL            ParseAuthorizationLine(const CString& sLine);
  virtual BOOL            ParseRequestLine(const CString& sCurrentLine, BOOL bFirstLine, const CString& sField, const CString& sValue);
  virtual void            TransmitBuffer(BYTE* byData, DWORD dwSize, CHttpDirectory* pDirectory, BOOL bForceExpire);
  virtual BOOL            AllowThisConnection();
  virtual CHttpDirectory* GetVirtualDirectory(const CString& sDirectory);
  virtual void            HandleClient();
  virtual LPSTR           FindNextTerminatorFromRequest(LPSTR pszLine);

#ifdef W3MFC_SSL_SUPPORT
  virtual BOOL            DoSSLNegotiation();
  virtual BOOL            InitializeSSLConnection();
#endif

//Member variables
  CEvent m_StopEvent;

  DECLARE_DYNCREATE(CHttpClient)
};

#endif //__HTTPCLIENT_H__
