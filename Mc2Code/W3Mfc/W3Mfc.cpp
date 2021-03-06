/*
Module : W3Mfc.cpp
Purpose: Implementation for a simple MFC class encapsulation of a HTTP server
Created: PJN / 22-04-1999
History: PJN / 24-06-1999 1.  Implemented support for "HEAD" command
                          2.  Sample provided now also displays the HTTP verb used
                          3.  Sample provided now also displays the date and time of each request
                          4.  Now fully supports multiple virtual directories
                          5.  Now fully supports URL's with encoded characters
                          6.  Implemented support for "DELETE" command
                          7.  Now returns an "Allow:" HTTP header
                          8.  Timeout for requests is now 90 seconds if built for debug
                          9.  Now supports directory listing
                          10. User name is now displayed in the log window
         PJN / 29-02-2000 1.  Fixed a number of VC 6 level 4 warnings
                          2.  Now empties the username and password once they are used
                          3.  Now implements W3C Common Logfile logging to the console window
         PJN / 15-04-2001 1.  Updated copyright information   
                          2.  A new generalised thread pool wrapper class is now included which can
                          be used in other projects.
                          3.  A new generalised sockets wrapper class is now included which can be 
                          used in other winsock projects
         PJN / 23-04-2001 1. Fixed a stack overwrite issue in CHttpResponseHeader::DateToStr
                          2. Fixed a bug in CHttpClient::TransmitFile which was causing "304 Not Modified"
                          headers to never be returned
         PJN / 25-07-2001 1. Code now uses Winsock 2 functionality, namely "WSAEventSelect" to avoid a busy loop
                          in the listening socket thread. This means that you will have to have Winsock 2 
                          downloaded and installed if running on Windows 95 which did not ship with 
                          Winsock 2 built in.
         PJN / 23-08-2001 1. Added support for CGI v1.1 (has been verfied with perl scripts and batch files)
                          2. Added support for reverse DNS lookups
                          3. Now fully supports POST HTTP method.
                          4. Following values have been added to CHttpRequest:
                             m_pszRawRequest, m_pszRawEntity, m_sRemoteHost and m_sContentType
                          5. Improved the robustness of the code by making sure exceptions are caught
         PJN / 30-09-2001 1. Fixed a bug in CWSocket::Receive which can occur when a disconnection occurs
                          while a receive is in progress
                          2. Fixed a bug where entities sent i.e. m_pszRawEntity did not correctly handle the 
                          case where it had enbedded NULL's e.g. a file upload. Thanks to Christian Hett for 
                          spotting this problem.
                          3. Removed some unnecessary memcpy's from CHttpClient::ParseRequest
                          4. Made the main program configurable via an ini file instead of hard coding all the
                          settings.
                          5. Fixed a resource leak on an anonymous pipe handle in CHttpClient::TransmitCGIResponse.
         PJN / 03-11-2001 1. Fixed an issue where socket sends would sometimes throw the exception WSAEWOULDBLOCK.
                          The httpsocket class now includes a "SendWithRetry" method which handles this case.
         PJN / 27-12-2001 1. Fixed an exception memory leak in the method CHttpSocket::SendWithRetry. Thanks 
                          to Olivier Meyer for spotting this problem.
                          2. Removed an unreferrenced variable from CHttpSocket::ReadResponse
                          3. Added a flag to TransmitFile to allow the transmission of an "Expires: " header
                          4. Added CHttpDirectory parameter to all transmit methods. Allows derived classes
                          more control over the response procedure.
                          5. Added optional Non-NT authentication to the web server at the virtual directory
                          level. 
                          6. Reviewed TRACE statements throughtout W3MFC for completeness and validity
                          7. Addition of a CHttpClient::TransmitBuffer method to allow for easy transmission of 
                          in-memory generated responses. Thanks to "Golden Crater" for most of those suggestions.
                          8. W3MFC now takes advantage of the MS Winsock extension API "TransmitFile". This
                          should improve the performance of the code by avoiding inordinate kernel transitions
                          on NT based operating systems.
                          9. Includes a new error reporting mechanism via the CHttpServer::OnError method
                          10. Now includes a nice icon for the generated binary
                          11. Version info resource is now included in the generated binary
                          12. Modified "Return..." functions to return body length sent
                          13. Check all PostLog calls to ensure indicated parameter size is correct
                          14. W3MFC now takes advantage of the MS Winsock extension API "TransmitPackets". This
                          should improve the performance of the code by avoiding inordinate kernel transitions
                          on Windows XP or later. Please note that I have been unable to test this feature so 
                          feeback on this would be appreciated.
         PJN / 08-01-2002 1. Code only uses TransmitFile and TransmitPackets APIs if running on NT/2000/XP/.Net Server
                          2. Provided a virtual CHttpClient::TransmitFile and CHttpClient::TransmitBuffer functions. 
                          The default implemenations just pass the buck to the _W3MFC_DATA function pointers if 
                          available.
                          3. Made CHttpClient::HandleClient function virtual.
                          4. Fixed an issue in CHttpClient::TransmitDirectory where created URL's could sometimes forget 
                          to put in directory separators in the URL.
                          5. Updated copyright messages to be 2002.
                          6. Provided a virtual CHttpClient::ParseRequestLine to allow derived classes to easily pull
                          out any additional http headers e.g. "User-Agent" or "Accept-Language" etc.
                          7. Provided a virtual CHttpClient::FormCGIEnvironment to allow derived classes to easily change
                          the environment variables sent to CGI processes.
                          8. Fixed a serious bug in CHttpResponseHeader::GetData while code was compiled for UNICODE
                          9. Addition of a very handy new "CW32Handle" class which implements a "Smart Pointer" class
                          for Win32 Handles. For one thing it looks after calling CloseHandle in the destructor. The class
                          turns out to be very handy when you are implementing CGI in this web server as you have to
                          create loads of Win32 handles such as anonymous pipes etc.
                          10. Improved the error reporting in CHttpClient::TransmitCGIResponse
                          11. Rewrote the CGI handling code in CHttpClient::TransmitCGIResponse to use the information 
                          provided by MS Knowledge base article "Q190351".
         PJN / 26-01-2002 1. CIOCP9x clas: added an additional semaphore as I have forgotten that we need to synchronise 
                          the removal of items from the queue in addition to synchronising the addition of items to the 
                          queue.
                          2. Fixed a level 4 warning in the CHttpClient class.
                          3. Only now uses TransmitPackets and TransmitFile if running 2000 / XP or .Net Server. Thanks 
                          to Olivier Meyer for spotting this problem on NT 4.
         PJN / 21-02-2002 1. Fixed an issue in CHttpClient::GetCGICommandLine where the code was using RegQueryValue 
                          instead of RegQueryValueEx which is the recommended way of doing registry queries in Win32.
                          Thanks to Josh Clarke for spotting this problem.
                          2. CGI command line is now expanded with any environment variables it may contain. Again 
                          thanks goes to Josh Clarke for this nice update.
         PJN / 23-03-2002 1. W3MFC ships with a sample CGI app "TestCGI.exe"
                          2. CHttpClient::GetCGICommandLine function now defaults to the local file path if a registry 
                          entry cannot be found for it. Thanks to Gilad Novik for this suggestion.
         PJN / 23-05-2002 1. Now fully supports SSL via the OpenSSL open source Library.
                          2. Removed IOCP9x module from project as not required.
                          3. Fixed a bug in CHttpServer::ListenSocketFunction which was causing the thread pool to initialize
                          incorrectly.
         PJN / 24-05-2002 1. Fixed a bug in the SSL code path which was causing a crash in CHttpClient::TransmitBuffer.
                          2. Provision of wrapper classes for OpenSSL C type variables used by W3MFC
                          3. Tidied up the SSL code in CHttpClient::HandleClient
         PJN / 25-05-2002 1. Improved the performance of CHttpClient::URLDecode method
                          2. Improved the performance of CHttpClient::TransmitDirectory. Thanks to "Jim" for this fix.
                          3. URL's transmitedd down to client in CHttpClient::TransmitDirectory are now URL encoded.
                          4. Fixed a crash which was occurring for some requests in CHttpClient::ParseRequest where the code
                          ended up parsing into the entity body instead of stopping when it reached the separator after the
                          http header. Thanks to Harold B Johns for spotting this.
         PJN / 29-05-2002 1. Environment variables for CGI processes now includes all the environment variables which W3MFC
                          itself has. These missing environment variables were causing CGI programs to fail in calls to
                          CoInitialize if they were using COM. Thanks to Harold B Johns for spotting this.
         PJN / 30-05-2002 1. Fixed a #include issue when doing a non SSL build. Thanks to Harold B Johns for spotting this.
                          2. Fixed an issue where a request for a non existent CGI file caused the web server to become
                          unresponsive. Thanks to Harold B Johns for spotting this.
                          3. Fixed an issue in the CGI code when SSL is enabled. Thanks to Gilad Novik for reporting this
                          problem.
         PJN / 14-06-2002 1. Fixed a problem in CHttpClient::FindNextTerminatorFromRequest which was parsing out header
                          details incorrectly. Thanks to Gilad Novik for reporting this problem.                         
         PJN / 28-07-2002 1. Now CHttpMimeManager can be replaced at runtime. This allows the mime map to be easily configured
                          or replaced with a custom implementation.
                          2. Mime map as used by the sample app is now configured from the W3MFC ini file instead of from
                          the system registry. In addition the shipped W3MFC ini file now includes a fully fleshed out mime 
                          section
                          3. You can now improve the robustness of the SSL behaviour by initializing the OpenSSL PRNG (Pseudo
                          Random Number generator) with the contents of a file as specified via 
                          CHttpServerSettings::m_sSSLRandomnessFile.
                          4. A fresh default web site is now shown using the default ini settings, instead of expecting a web 
                          site to be configured at c:\inetpub\wwwroot
                          5. Improved the performance of looking up mime types by implementing a hash table for storage
         PJN / 18-08-2002 1. Now uses v1.09 of the CThreadPoolServer class.                 
                          2. Sample app now allows you to decide via a ini value whether or not to use the CIOCPThreadPoolQueue
                          class instead of the CDirectedThreadPoolQueue class. While I was at it, I avoiding an issue with
                          leaving an item on the Q if updating the "Get" semaphores failed.
         PJN / 17-10-2002 1. Fixed a bug in CHttpClient::HexToInt which was causing it to not work for upper case characters.
                          Thanks to Frank Schmidt for spotting this problem.
                          Failure to setup thread pool recycling is now reported as an error.
         PJN / 16-11-2002 1. Class now supports the use of the "Connection: Keep-Alive" Header.
                          2. Removed the StringReplace method and replaced it with CString::Replace.
                          3. Fixed some Log message's text to correctly reflect the function they are used in.
                          4. Code now makes the call to CHttpClient::AllowThisConnection before the incoming request is parsed.
                          5. Fixed a typo in the name of the CHttpClient::HandleDirectoryAuthorization function
                          6. Made the operation of the delivery of the "Expires" header contingent on a configurable setting
                          7. CHttpResponseHeader::AddW3MfcAllowFields now returns that it can handle POST requests
                          8. The "DELETE" verb support is now contingent on a configurable setting.
                          9. Fixed a typo in the creation of the "SERVER_PROTOCOL" CGI environment variable
                          10. Updated the sample CGI app to also send a Content-Length HTTP header. This allows the sample
                          app to work correctly when it is used in conjuction with HTTP Keep Alives  
         PJN / 17-11-2002 1. Optimized the use of the Keep Alives variables in the function CHttpClient::HandleClient
                          2. Implemented code to find whether CGI programs send Keep Alives.
         PJN / 18-11-2002 1. Now allows anonymous access to be enabled / disabled
                          2. Now allows Basic authentication to be enabled / disabled.
                          3. Reworked the Base64 class to be based on the ATL Server Base64 routines in ATL Server in VC.Net
                          4. Renamed and Reworked the CHttpClient::ReturnUnauthorizedMessage method.
                          5. Impersonation handle used during Basic authentication now is a CW32Handle instance instead of a 
                          raw SDK handle
                          6. Now fully supports NTLM Authentication via the SSPI interface. The code is enabled for support
                          for Kerberos, "Negotiate" and Digest authentication which will be added in a future release. The use 
                          of SSPI support can be changed via the compile time define "W3MFC_NO_SSPI_SUPPORT". Thanks to 
                          Harold B Johns for suggesting this.
                          7. Fixed a problem in CHttpClient::ParseRequest which was causing failures to parse a certain line
                          to be overwritten upon successfull parsing of subsequent lines. 
                          8. test CGI app now is implemented without MFC support and deliberatly does not use a Content-Length
                          or Keep-Alive header for testing purposes.
                          9. Tidied up the importing of various header files throughout W3MFC and also the associated
                          #pragma messages which these something display.
         PJN / 18-11-2002 1. CHttpSocket::ReadResponse now uses waitable timers (if it can) instead of a loop which has calls
                          to Sleep. This should improve the scalabilty of the code under heavy load. Please note that if you
                          want to compile the code, then you will need to download the CWaitableTimer source code separately
                          from my web site and copy in the wtimer.h & wtimer.cpp into your W3MFC's source code directory.
                          2. Added code to protect against NTFS alternate streams being accessed by client browsers.
                          3. Improved the robustness of parsing the authorization header fields.
         PJN / 20-11-2002 1. Fixed a number of level 4 warnings about unreferrenced variables. Thanks to Harold Johns for 
                          reporting these.
                          2. Updated the documentation to refer to the fact that you will need an up to date Platform SDK 
                          to be installed to compile W3MFC with SSPI support.
                          3. Fixed a memory leak of CHttpRequest objects which can occur when you shutdown the web server and
                          you are using a directed thread pool queue and there are outstanding items in the queue which are
                          not shutdown requests
         PJN / 03-02-2003 1. W3MFC now compares HTTP headers without regard to case. Thanks to Gilad Novik and Frank Hahn for 
                          reporting these problems.
                          2. Tidied up inclusion of afxpriv.h throughout W3MFC modules.
         PJN / 21-02-2003 1. Made the m_Directories member of CHttpServerSettings a pointer array. This allows client code
                          to add their own derived instances of CHttpDirectory to the array. This allows per directory 
                          customisation of the web server. This change also necessitated changing the settings class of the 
                          CHttpServer to be a pointer also. Thanks to "Edgar" for this update.
                          2. Remove the digest authentication boolean from the settings class as Digest authentication is 
                          currently not supported.
                          3. Made the CHttpClient::LoadHTMLResource method virtual
                          4. Moved a lot of the CHttpClient implementation code to CHttpDirectory. This allows more additional
                          complex client customisation of the code.
                          5. Split off the CHttpDirectory class into its own module.
                          6. Implemented a virtual CHttpDirectory::HandleClient. This allows customisation of per directory
                          responses. Thanks to "Edgar" for this suggestion.
         PJN / 22-02-2003 1. Fixed a bug in the parsing of SSL requests which was causing heap corruption problems on
                          subsequent reads of SSL requests on the same thread.
                          2. Reworked the CHttpClient::HandleClient to avoid having to call various functions when the code
                          needs to exit prematurely
                          3. Added a member variable "m_bResponseKeepAlive" to avoid having to pass a keep alive variable
                          throughout all the CHttpClient code.
                          4. Fixed a deadlock bug in CHttpServer::Stop. Thanks to "Edgar" for reporting this problem.
                          5. modified the Mime manager class method "GetMimeType" to be sent the full request rather than just
                          the extension of the URL when it is called to determine the mime type.
                          6. Addition of a string version of the HTTP verb to CHttpRequest. This speeds up the CGI and 
                          ISAPI implementations somewhat.
                          7. Addition of a hash table in the request structure which contains all the HTTP request headers. These
                          values are now available via CGI or ISAPI
                          8. Split of the CGI implementation into its own module and made it optional via a preprocessor directive
                          9. Split of the ISAPI implementation into its own module and made it optional via a preprocessor directive
                          10. W3MFC now uses Win32 TransmitFile API in SSL configuration and only falls back to user mode sockets
                          if SSL is actively being used.
                          11. W3MFC now supports PATH_INFO information in URL's
                          12. Optimized loading of error pages which W3MFC uses
         PJN / 02-03-2002 1. Removed the function CHttpDirectory::SetClient as it can lead to thread synchronisation problems. 
                          Thanks to "Edgar" for reporting this problem.
         PJN / 03-03-2202 1. Added a few defines to allow the W3MFC to compile if you do not have the latest version of the Platform
                          SDK installed. Thanks to "Edgar" for reporting this.
                          2. Fixed a copy and paste gremlin in CHttpServer::Start which was causing a critical section to not be
                          acquired. Thanks to "Edgar" for reporting this.
                          3. Removed the use of "friend classes" entirely throughout the W3MFC codebase. This avoids potential 
                          compilation problems in any derived classes used by client code of W3MFC.
                          4. Addition of a number of preprocessor defines, namely W3MFC_EXT_CLASS, THRDPOOL_EXT_CLASS and 
                          SOCKMFC_EXT_CLASS. This allows the classes to easily be added and exported from a MFC extension dll.
         PJN / 12-09-2003 1. Removed double definition of SCRIPT_NAME from CGI environment. Thanks to Dave Horner for reporting
                          this.
                          2. SCRIPT_NAME CGI environment variable now uses forward slashes rather than back slashes as
                          directory separators. Thanks to Dave Horner for reporting this problem.
                          3. Added a "REQUEST_URI" CGI environment variable. Thanks to Dave Horner for reporting this.
                          4. CGI implementation now returns a HTTP return code line if one if the CGI script does not provide one.
                          Again thanks to Dave Horner for reporting this issue.
                          5. CGI implementation now returns immedately from CHttpCGI::ReadFromClientStdout from a socket error is
                          detected. Again thanks to Dave Horner for reporting this issue.
                          6. "PATH_TRANSLATED" CGI and ISAPI environment variable is now reported as an absolute path. Again thanks 
                          to Dave Horner for reporting this issue.
                          7. "SCRIPT_NAME" CGI and ISAPI environment variable now includes an initial "/". Again thanks to Dave 
                          Horner for reporting this issue.
                          8. CGI now uses pClient->m_Request.m_dwRawEntitySize variable when determining when W3MFC needs to 
                          write to the CGI child process.
                          9. ISAPI implementation now sends just the HTTP body to client ISAPI dlls instead of the whole HTTP
                          request. Thanks to Michael St. Laurent for reporting this problem.
         PJN / 03-11-2003 1. Simplified the code in CHttpSocket::ReadResponse. Thanks to Clarke Brunt for reporting this issue.
         PJN / 10-11-2003 1. URL Encoded spaces as "+" are now correctly handled. Thanks to Dan Baker for reporting this issue. 
                          2. CHttpRequest now includes a m_sRawURL member variable which provides access to the raw URL 
                          before it is URL decoded. Thanks to Dan Baker for suggesting this addition.
                          3. Shipped binary for W3MFC is now linked with the OpenSSL 0.9.6l which is the latest version currently.
         PJN / 30-03-2004 1. Tidied up the interaction of the various classes by removing all friend classes
         PJN / 25-08-2004 1. The binaries included in the download now link against OpenSSL 0.9.7d.
                          2. Per thread cleanup is now done for OpenSSL. Again thanks to Leandro Gustavo Biss Becker for reporting 
                          this problem.
                          3. OpenSSL is now configured by the W3MFC sample app to operate in a thread safe manner. Again thanks 
                          to Leandro Gustavo Biss Becker for reporting this problem.
                          4. Application initialization of OpenSSL has been taken out of W3MFC and is now the responsibility of 
                          your application. See the sample W3MFC application code in main.cpp for an example on how to correctly
                          initialize and terminate OpenSSL.
                          5. CSSL::Close() now calls SSL_shutdown to ensure SSL connections are properly terminated. Thanks to 
                          to Leandro Gustavo Biss Becker for reporting this problem.
                          6. W3MFC now correctly handles UTF8 encoded URL requests. Thanks to Huang Wei for reporting this problem
                          and providing the fix.
         PJN / 15-10-2004 1. Removed an unnecessary ASSERT from CHttpClient::MapURLToLocalFilename. Thanks to Jan Bares for 
                          reporting this problem.
                          2. Changed the behaviour of the code in CHttpClient::MapURLToLocalFilename as follows:

                          Before Change

                          Requesting the URL "/name" would return the file "name" in the root directory of the web server even if
                          a "/name" virtual directory existed. If "name" did not exist in the root directory then a 404 error 
                          would be returned

                          After Change

                          Requesting the URL "/name" will return the virtual directory "/name" if such a virtual directory exists. 
                          If not then the file "name" in the root directory will be returned.

                          Thanks to Jan Bares for pointing out this behaviour which is inconsistent with other Web Server implementations.

                          3. URLs which include "@" now are parsed correctly. Previously the code was parsing the URI
                          expecting it to contain username and password in the URL itself. Instead when the URI arrives at the 
                          server it is not in the URI itself but is represented in the HTTP request headers. Thanks to Jan Bares for 
                          pointing out this problem.
                          4. Passthrough authentication can now be disabled via a new CHttpServerSettings::m_bPerformPassthroughAuthentication
                          setting. This is useful where you want to setup per directory protection using a username / password pair but
                          you do not want to use these credentials in an attempt to impersonate that user by calling the SDK function 
                          "LogonUser". Again thanks to Jan Bares for this suggestion.
         PJN / 11-11-2004 1. Added a m_sRawExtra variable to CHttpRequest. This value is now passed to CGI and ISAPI instead of m_sExtra. 
                          This allows apps to correctly parse form parameters. Thanks to Jan Bares for reporting this problem.
         PJN / 19-02-2004 1. Fixed a bug when directory lists were displayed by W3MFC. It incorrectly always dropped the last entry from
                          the listing. Thanks to Pablo Romero for reporting this bug.

Copyright (c) 1999 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "SocMFC.h"
#include "HttpClient.h"
#include "W3Mfc.h"
#include "Win32Handle.h"
#ifdef W3MFC_SSL_SUPPORT
#include "OpenSSLMfc.h"
#include <openssl\rand.h>
#endif
#ifndef __AFXPRIV_H__
#pragma message("To avoid this message please put afxpriv.h in your PCH (normally stdafx.h)")
#include <afxpriv.h>
#endif



//////////////// Macros / Defines /////////////////////////////////////

//automatically pull in Winsock 2
#pragma comment(lib, "Ws2_32.lib")

//automatically pull in SSPI (if required)
#ifndef W3MFC_NO_SSPI_SUPPORT
//#pragma comment(lib, "E:\\sdk\\Lib\\secur32.lib")
#endif

#ifndef SEC_E_OK
#define SEC_E_OK ((SECURITY_STATUS)0x0000)
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

CHttpServerSettings::CHttpServerSettings()
{
  m_nPort = 80;                  //Default to the standard HTTP port
  m_bBind = FALSE;               //Default to not binding to a specific IP address

#ifdef _DEBUG
  m_dwIdleClientTimeout = 90000; //Default to client idle timeout of 90 seconds (when in debug mode)
#else
  m_dwIdleClientTimeout = 30000; //Default to client idle timeout of 30 seconds
#endif

  //Default root directory will be where the exe is running from
  TCHAR sPath[_MAX_PATH];
  GetModuleFileName(NULL, sPath, _MAX_PATH);
  TCHAR sDrive[_MAX_DRIVE];   
  TCHAR sDir[_MAX_DIR];
  _tsplitpath(sPath, sDrive, sDir, NULL, NULL);
  _tmakepath(sPath, sDrive, sDir, NULL, NULL);
  CHttpDirectory* pDir = new CHttpDirectory;
  pDir->SetDirectory(sPath);    
  pDir->SetAlias(_T('/'));
  pDir->SetDefaultFile(_T("default.htm")); //Default filename returned for root requests will be "default.htm"
  m_Directories.Add(pDir);

  m_sServerName = _T("MiniCA Web Server"); //Default server name will be the name of the MFC classes i.e "W3MFC" plus the current version number 
  m_pRuntimeClientClass = RUNTIME_CLASS(CHttpClient); //Default class to use is CHttpClient
  m_nThreadPoolSize = 10;
  m_bDNSLookup = FALSE;
  m_dwWritableTimeout = 10000;
  m_bEnableThreadLifetime = FALSE;
  m_dwThreadLifetime = 120;
  m_pMimeManager = NULL;
  m_bUseIOCPQueue = FALSE;
  m_bKeepAlives = TRUE;
  m_bAutoExpire = FALSE;
  m_bAllowDeleteRequest = FALSE;
#ifndef W3MFC_NO_CGI_SUPPORT
  m_dwCGIResponseBufferSize = 4096;
#endif
  m_bAllowAnonymous = TRUE;
  m_bAllowBasicAuthentication = TRUE;
  m_bAllowNTLMAuthentication = FALSE;
  m_bPerformPassthroughAuthentication = TRUE;
#ifndef W3MFC_NO_ISAPI_SUPPORT
  m_bCacheISAPI = TRUE;
  m_pISAPIManager = NULL;
  m_pISAPI = NULL;
#endif
#ifndef W3MFC_NO_CGI_SUPPORT
  m_pCGI = NULL;
#endif
#ifdef W3MFC_SSL_SUPPORT
  m_SSLProtocol = SSL_NONE;
  m_sServerCertificateFile = _T("W3MFC.pem");
  m_bReuseSessions = TRUE;
  m_dwSSLSessionTimeout = 300;
  m_dwSSLNegotiationTimeout = 10000;
#endif
}

CHttpServerSettings::~CHttpServerSettings()
{
  FreeDirectoryArray();
}

void CHttpServerSettings::FreeDirectoryArray()
{
  for (int i=0; i<m_Directories.GetSize(); i++)
  {
    CHttpDirectory* pDir = m_Directories.GetAt(i);
    delete pDir; 
    pDir = NULL;
  }
  m_Directories.RemoveAll();
}



IMPLEMENT_DYNCREATE(CHttpDirectedThreadPoolQueue, CDirectedThreadPoolQueue)

CHttpDirectedThreadPoolQueue::~CHttpDirectedThreadPoolQueue()
{
  //Before we are destroyed, deallocate any user requests
  //which may still be lying around in the requests array
  for (int i=0; i<m_Requests.GetSize(); i++)
  {
    CThreadPoolRequest& request = m_Requests.ElementAt(i);
    if (request.m_dwID == THREADPOOL_USER_DEFINED_REQUEST)
    {
      CHttpThreadPoolRequest* pHttpRequest = (CHttpThreadPoolRequest*) request.m_pData;
      delete pHttpRequest;
      pHttpRequest = NULL;
    }
  }
}



IMPLEMENT_DYNAMIC(CHttpServer, CObject)

CHttpServer::CHttpServer() : m_ListenStartEvent(TRUE), m_pListenThread(NULL), m_hImpersonation(NULL)
{
#ifndef W3MFC_NO_SSPI_SUPPORT
  ZeroMemory(&m_hCredHandle, sizeof(m_hCredHandle));
#endif

  LoadHTMLResources();
}

void CHttpServer::LoadHTMLResources()
{
  m_psz302HTML = NULL;
  m_dw302HTML = 0;
  m_psz400HTML = NULL;
  m_dw400HTML = 0;
  m_psz401HTML = NULL;
  m_dw401HTML = 0;
  m_psz404HTML = NULL;
  m_dw404HTML = 0;
  m_psz500HTML = NULL;
  m_dw500HTML = 0;
  m_pszDeletedHTML = NULL;
  m_dwDeletedHTML = 0;

  LoadHTMLResource(IDH_302, m_psz302HTML, m_dw302HTML);
  LoadHTMLResource(IDH_400, m_psz400HTML, m_dw400HTML);
  LoadHTMLResource(IDH_401, m_psz401HTML, m_dw401HTML);
  LoadHTMLResource(IDH_404, m_psz404HTML, m_dw404HTML);
  LoadHTMLResource(IDH_500, m_psz500HTML, m_dw500HTML);
  LoadHTMLResource(IDH_FILE_DELETED_OK, m_pszDeletedHTML, m_dwDeletedHTML);
}

CHttpServer::~CHttpServer()
{
  Stop();
  FreeHTMLResources();
}

void CHttpServer::FreeHTMLResources()
{
  if (m_psz302HTML)
  {
    delete [] m_psz302HTML;
    m_psz302HTML = NULL;
    m_dw302HTML = 0;
  }
  if (m_psz400HTML)
  {
    delete [] m_psz400HTML;
    m_psz400HTML = NULL;
    m_dw400HTML = 0;
  }
  if (m_psz401HTML)
  {
    delete [] m_psz401HTML;
    m_psz401HTML = NULL;
    m_dw401HTML = 0;
  }
  if (m_psz404HTML)
  {
    delete [] m_psz404HTML;
    m_psz404HTML = NULL;
    m_dw404HTML = 0;
  }
  if (m_psz500HTML)
  {
    delete [] m_psz500HTML;
    m_psz500HTML = NULL;
    m_dw500HTML = 0;
  }
  if (m_pszDeletedHTML)
  {
    delete [] m_pszDeletedHTML;
    m_pszDeletedHTML = NULL;
    m_dwDeletedHTML = 0;
  }
}

#ifndef W3MFC_NO_SSPI_SUPPORT
BOOL CHttpServer::AcquireSSPI()
{
  //Get a connection to SSPI
  TimeStamp tsExpires;
  SECURITY_STATUS ss = AcquireCredentialsHandle(NULL, _T("NTLM"), SECPKG_CRED_INBOUND, NULL, NULL, NULL, NULL, &m_hCredHandle, &tsExpires);
  BOOL bSuccess = (ss == SEC_E_OK);

  //Report the error
  if (!bSuccess)
  {
    CString sError;
    sError.Format(_T("CHttpServer::AcquireSSPI, Failed in call to AcquireCredentialsHandle, Error:%d"), ss);
    OnError(sError);
  }

  return bSuccess;
}

void CHttpServer::ReleaseSSPI()
{
  SECURITY_STATUS ss = FreeCredentialsHandle(&m_hCredHandle);
  if (ss != SEC_E_OK)
  {
    CString sError;
    sError.Format(_T("CHttpServer::ReleaseSSPI, Failed in call to FreeCredentialsHandle, Error:%d"), ss);
    OnError(sError);
  }
}
#endif

BOOL CHttpServer::Start(CHttpServerSettings* pSettings)
{
  CSingleLock sl(&m_csListenThread, TRUE); //synchronize access to the listening thread

  ASSERT(m_pListenThread == NULL); //Trying to start an already started server
  ASSERT(pSettings);
  ASSERT(pSettings->m_pMimeManager); //must be assigned before you call this function
#ifndef W3MFC_NO_ISAPI_SUPPORT
  ASSERT(pSettings->m_pISAPIManager); //must be assigned before you call this function
  ASSERT(pSettings->m_pISAPI); //must be assigned before you call this function
  pSettings->m_pISAPI->SetServer(this);
#endif
#ifndef W3MFC_NO_CGI_SUPPORT
  ASSERT(pSettings->m_pCGI); //must be assigned before you call this function
#endif

  //Initialize SSPI
#ifndef W3MFC_NO_SSPI_SUPPORT
  if (!AcquireSSPI())
    return FALSE;
#endif

  //Start the listener thread
  m_pSettings = pSettings;
  m_bListenerRunningOK = FALSE;
  m_StopEvent.ResetEvent();
  m_ListenStartEvent.ResetEvent();
  m_pListenThread = AfxBeginThread(ListenSocketFunction, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  if (m_pListenThread == NULL)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::Start, Failed to call to create listener thread, please check settings"));
    OnError(sError);
    return FALSE;
  }
  m_pListenThread->m_bAutoDelete = FALSE;
  m_pListenThread->ResumeThread();

  //Wait until the thread has completely started and return the success indicator
  ::WaitForSingleObject(m_ListenStartEvent, INFINITE);
  return m_bListenerRunningOK;
}

BOOL CHttpServer::Wait()
{
  CSingleLock sl(&m_csListenThread, TRUE); //synchronize access to the listening thread

  //If the listener thread is running, then just wait for it to exit
  if (m_pListenThread)
  {
    ::WaitForSingleObject(m_pListenThread->m_hThread, INFINITE);
    delete m_pListenThread;
    m_pListenThread = NULL;
    return TRUE;
  }
  else
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::Wait, Http server is not running, so no need to wait"));
    OnError(sError);
    return FALSE;
  }
}

BOOL CHttpServer::Stop()
{
  //Signal the listener thread to stop and wait for it to do so, Note that we do 
  //this before we enter the critical section because otherwise we could end
  //up in a deadlock situation between this function and Wait
  m_StopEvent.SetEvent();

  CSingleLock sl(&m_csListenThread, TRUE); //synchronize access to the listening thread

  //If the listener thread is running, then stop it
  if (m_pListenThread)
  {
    ::WaitForSingleObject(m_pListenThread->m_hThread, INFINITE);
    delete m_pListenThread;
    m_pListenThread = NULL;

    //Release SSPI
#ifndef W3MFC_NO_SSPI_SUPPORT
    ReleaseSSPI();
#endif
  }

  return TRUE;
}

BOOL CHttpServer::ReverseDNSLookup(in_addr sin_addr, CString& sDomainName)
{
  BOOL bSuccess = FALSE;
  HOSTENT* pHostEnt = gethostbyaddr((const char*) &sin_addr, sizeof(sin_addr), AF_INET); 
  if (pHostEnt)
  {
    bSuccess = TRUE;
    sDomainName = pHostEnt->h_name;
  }

  return bSuccess;
}

#ifdef W3MFC_SSL_SUPPORT
void CHttpServer::LogSSLErrors()
{
  char sError[1024];
  unsigned long nError;
  do
  { 
    nError = ERR_get_error();
    if (nError)
    {
      ERR_error_string(nError, sError);
      OnError(sError);
    }
  }
  while (nError);
}
#endif


CString CHttpServer::ErrorString()
{
	DWORD err = ::GetLastError();
	CString Error;
	LPTSTR s;
	if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		0x0409,
		(LPTSTR)&s,
		0,
		NULL) == 0)
    { /* failed */
		// Unknown error code %08x (%d)
		CString fmt;
		CString t;
		fmt.LoadString(IDS_UNKNOWN_ERROR);
		t.Format(fmt, err, LOWORD(err));
		Error = t;
    } /* failed */
	else
    { /* success */
		LPTSTR p = _tcschr(s, _T('\r'));
		if(p != NULL)
        { /* lose CRLF */
			*p = _T('\0');
        } /* lose CRLF */
		Error = s;
		::LocalFree(s);
    } /* success */
	return Error;
}
                               
void CHttpServer::ListenSocketFunction()
{
  USES_CONVERSION;

  //Setup the thread pool 
  ASSERT(m_pSettings);
  ASSERT(m_pSettings->m_pRuntimeClientClass);
  if (!m_ThreadPool.Start(m_pSettings->m_pRuntimeClientClass, m_pSettings->m_bUseIOCPQueue ? RUNTIME_CLASS(CIOCPThreadPoolQueue) : RUNTIME_CLASS(CHttpDirectedThreadPoolQueue), 
                          m_pSettings->m_nThreadPoolSize, m_pSettings->m_nThreadPoolSize, TRUE))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to create thread pool, GetLastError:%d"), ::GetLastError());
    OnError(sError);
    m_ListenStartEvent.SetEvent();
    return;
  }

  //Setup the pointers in the thread pool instances and resume the threads
  for (int i=0; i<m_pSettings->m_nThreadPoolSize; i++)
  {
    CHttpClient* pClient = (CHttpClient*) m_ThreadPool.GetAtClient(i);
    ASSERT(pClient);
    ASSERT(pClient->IsKindOf(RUNTIME_CLASS(CHttpClient)));
    pClient->m_pServer = this;
    pClient->m_pWorkerThread->ResumeThread();
  }

  //Setup the recycling of threads
  if (!m_ThreadPool.SetMaxThreadClientLifetime(m_pSettings->m_bEnableThreadLifetime, m_pSettings->m_dwThreadLifetime))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::ListenSocketFunction, Failed in call to m_ThreadPool.SetMaxThreadClientLifetime, GetLastError:%d"), GetLastError());
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    return;
  }

  //Create the server socket
  CHttpSocket serverSocket;
  try
  {
    serverSocket.Create();
  }
  catch(CWSocketException* pEx)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to create server socket, GetLastError:%d"), pEx->m_nError);
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    pEx->Delete();  
    return;
  }

  //Bind the server socket
  try
  {
    if (m_pSettings->m_bBind)
      serverSocket.Bind(m_pSettings->m_nPort, m_pSettings->m_sBindAddress);
    else
      serverSocket.Bind(m_pSettings->m_nPort);
  }
  catch(CWSocketException* pEx)
  {
    //Report the error
    CString sError;
    sError.Format(_T("Error Code:%d  %s"), pEx->m_nError,  ErrorString());
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    pEx->Delete();  
    return;
  }

  //Put the server socket in a listening state
  try
  {
    serverSocket.Listen();
  }
  catch(CWSocketException* pEx)
  {
    //Report the error
    CString sError;
    sError.Format(_T("Error Code:%d  %s"), pEx->m_nError,  ErrorString());
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    pEx->Delete();
    return;
  }

  //Run the server under a different account if configured to do so
  BOOL bUseAccount = (m_pSettings->m_sUsername.GetLength() != 0);
  CW32Handle hImpersonation;
  BOOL bLoggedOn = FALSE;
  BOOL bImpersonated = FALSE;
  if (bUseAccount)
  {
    LPTSTR pszUser = m_pSettings->m_sUsername.GetBuffer(m_pSettings->m_sUsername.GetLength());
    LPTSTR pszPassword = m_pSettings->m_sPassword.GetBuffer(m_pSettings->m_sPassword.GetLength());
    HANDLE hRawImpersonation = NULL;
    bLoggedOn = LogonUser(pszUser, NULL, pszPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hImpersonation);
    if (bLoggedOn)
    {
      hImpersonation.Attach(hRawImpersonation);
      bImpersonated = ImpersonateLoggedOnUser(hImpersonation);
    }
    else
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to logon using user name: %s, GetLastError:%d"), pszUser, ::GetLastError());
      OnError(sError);
    }
    m_pSettings->m_sUsername.ReleaseBuffer();
    m_pSettings->m_sPassword.ReleaseBuffer();

    //Clear the password now that we are finished with it
    m_pSettings->m_sPassword.Empty();
  } 
  m_hImpersonation = hImpersonation; 

  //Handle the case if the impersonation failed
  if (bUseAccount && !bImpersonated)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to impersonate using supplied user credentials"));
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    return;
  }

  //Initialize SSL context if required to do so
#ifdef W3MFC_SSL_SUPPORT
  //Initialize the PRNG in OpenSSL
  if (m_pSettings->m_sSSLRandomnessFile.IsEmpty())
  {
    //Get the default rand file from OpenSSL
    char sSSLFile[1024];
    const char* pszRandFile = RAND_file_name(sSSLFile, sizeof(sSSLFile));
    if (pszRandFile)
      m_pSettings->m_sSSLRandomnessFile = pszRandFile;
    else
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to get the default RAND file from OpenSSL"));
      OnError(sError);

      LogSSLErrors();
    }
  }
  RAND_load_file(T2A((LPTSTR)(LPCTSTR)m_pSettings->m_sSSLRandomnessFile), -1);
  if (RAND_status() == 0)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to initialize the PRNG in OpenSSL"));
    OnError(sError);

    LogSSLErrors();

    //Failing to initialize the PRNG is not considered critical for W3MFC
  }

  CSSLContext sslContext;
  if (m_pSettings->m_SSLProtocol != CHttpServerSettings::SSL_NONE)
  {
    //Pick the SSL protocol to use
    SSL_METHOD* pSSLMethod = NULL;
    switch (m_pSettings->m_SSLProtocol)
    {
      case CHttpServerSettings::SSL_V2:
      {
        pSSLMethod = SSLv23_server_method();
        break;
      }
      case CHttpServerSettings::SSL_V3:
      {
        pSSLMethod = SSLv3_server_method();
        break;
      }
      case CHttpServerSettings::SSL_V2_OR_V3:
      {
        pSSLMethod = SSLv23_server_method();
        break;
      }
      case CHttpServerSettings::TLS_V1:
      {
        pSSLMethod = TLSv1_server_method();
        break;
      }
      default:
      {
        ASSERT(FALSE);
        break;
      }
    }

    //Create the SSL context object
    ASSERT(pSSLMethod != NULL);
    SSL_CTX* pSSLContext = SSL_CTX_new(pSSLMethod);
    if (pSSLContext == NULL) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to create SSL context object"));
      OnError(sError);

      LogSSLErrors();

      m_ListenStartEvent.SetEvent();
      return;
    }
    else
      sslContext.Attach(pSSLContext);

    //Set the certificate to use
    if (SSL_CTX_use_certificate_file(sslContext, T2A((LPTSTR) (LPCTSTR) m_pSettings->m_sServerCertificateFile), SSL_FILETYPE_PEM) != 1) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to load up server certificate"));
      OnError(sError);

      LogSSLErrors();

      m_ListenStartEvent.SetEvent();
      return;
    }

    //Set the private key to use
    if (SSL_CTX_use_PrivateKey_file(sslContext, m_pSettings->m_sPrivateKeyFile.GetLength() ? 
                                    T2A((LPTSTR) (LPCTSTR) m_pSettings->m_sPrivateKeyFile) : 
                                    T2A((LPTSTR) (LPCTSTR) m_pSettings->m_sServerCertificateFile), SSL_FILETYPE_PEM) != 1) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpServer::ListenSocketFunction, Failed to load up private key"));
      OnError(sError);

      LogSSLErrors();

      m_ListenStartEvent.SetEvent();
      return;
    }

    //Check that we the private key is ok
    if (SSL_CTX_check_private_key(sslContext) != 1) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CHttpServer::ListenSocketFunction, Private key does not match the certificate public key"));
      OnError(sError);

      LogSSLErrors();  

      m_ListenStartEvent.SetEvent();
      return;
    }

    //Set the SSL session settings
    if (m_pSettings->m_bReuseSessions)
    {
      int nOldTimeout;
      nOldTimeout = SSL_CTX_set_timeout(sslContext, m_pSettings->m_dwSSLSessionTimeout);
    }
    else
      SSL_CTX_set_timeout(sslContext, 0);
  }
#endif

  //Use a Win32 event notification on the server socket
  int nError = WSAEventSelect(serverSocket, m_SocketEvent, FD_ACCEPT);
  if (nError == SOCKET_ERROR)
  {
    DWORD dwError = ::GetLastError();

    //Report the error
    CString sError;
    sError.Format(_T("CHttpServer::ListenSocketFunction, Failed in call to WSAEventSelect, GetLastError:%d"), dwError);
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    return;
  }

  //We're now ready for accepting client connections, inform
  //the main thread that everthing is ok
  m_bListenerRunningOK = TRUE;
  m_ListenStartEvent.SetEvent();

  //Create the wait handles array (used for waiting for an event)
  HANDLE Handles[2];
  Handles[0] = m_StopEvent;
  Handles[1] = m_SocketEvent;

  //Wait for any incoming connections and the signal to exit the thread m_ListenStopEvent
  BOOL bWantStop = FALSE;
  CThreadPoolQueue* pQueue = m_ThreadPool.GetQueue();
  ASSERT(pQueue);
  while (!bWantStop)
  {
    DWORD dwWait = WaitForMultipleObjects(2, (CONST HANDLE*) &Handles, FALSE, INFINITE);
    int nSignaledHandle = dwWait - WAIT_OBJECT_0;
    if (nSignaledHandle == 0) //It was the stop request
      bWantStop = TRUE;
    else if (nSignaledHandle == 1) //It was a socket event
    {
      CHttpThreadPoolRequest* pRequest = new CHttpThreadPoolRequest;  
      try
      {
#ifdef W3MFC_SSL_SUPPORT
        pRequest->m_pSSLContext = &sslContext;
#endif

        //Accept the client connection  
        serverSocket.Accept(pRequest->m_ClientSocket, pRequest->m_ClientAddress);

        //Let a thread pool client class instance handle the work
        CThreadPoolRequest poolRequest;
        poolRequest.m_dwID = 1;
        poolRequest.m_pData = pRequest;  
        pQueue->PostRequest(poolRequest);
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CHttpServer::ListenSocketFunction, An error occurred accepting a client connection, GetLastError:%d"), pEx->m_nError);
        OnError(sError);

        pEx->Delete();
      }
    }
  }

  //Revert back to normal security settings
  if (bUseAccount)
  {
    //Revert to the usual security settings
    if (bImpersonated)
      RevertToSelf();
  }

  //Bring down the thread pool
  m_ThreadPool.Stop();
}

UINT CHttpServer::ListenSocketFunction(LPVOID pParam)
{
  //Get back the "this" pointer from the pParam parameter
  CHttpServer* pServer = (CHttpServer*) pParam;
  ASSERT(pServer);
  ASSERT(pServer->IsKindOf(RUNTIME_CLASS(CHttpServer)));

  //Call the run method of the CHttpServer instance
  pServer->ListenSocketFunction();

#ifdef W3MFC_SSL_SUPPORT
  //cleanup SSL thread state
  ERR_remove_state(0);
#endif

  //Return the thread exit code
  return TRUE;
}

#ifdef _DEBUG
void CHttpServer::OnError(const CString& sError) //To avoid level 4 warning in release
#else
void CHttpServer::OnError(const CString& /*sError*/)
#endif
{
#ifdef _DEBUG
  //The default is to just TRACE the details
  TRACE(_T("%s\n"), sError);
#endif
}

BOOL CHttpServer::LoadHTMLResource(UINT nID, char*& pszHTML, DWORD& dwSize)
{
  BOOL bSuccess = FALSE;

  HMODULE hModule = AfxFindResourceHandle(MAKEINTRESOURCE(nID), RT_HTML);
  HRSRC hRsrc = ::FindResource(hModule, MAKEINTRESOURCE(nID), RT_HTML);
  if (hRsrc)
  {
    //Load up the resource
    dwSize = ::SizeofResource(hModule, hRsrc); 
    HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);

    //Allocate a new char array and copy the HTML resource into it 
    if (hGlobal)
    {
      pszHTML = new char[dwSize + 1];
      char* pszResource = (char*) ::LockResource(hGlobal);
      if (pszResource)
      {
        strncpy(pszHTML, pszResource, dwSize);
        pszHTML[dwSize] = _T('\0');
        bSuccess = TRUE;
      }
      else
      {
        //Report the error
        TRACE(_T("CHttpServer::LoadHTMLResource, Failed to load HTML resource, GetLastError:%d\n"), GetLastError());
      }
    }
  }

  return bSuccess;
}

char* CHttpServer::LoadHTML(int nStatusCode, DWORD& dwSize)
{
  //What will be the return value
  char* pszHTML = NULL;

  dwSize = 0;

  switch(nStatusCode)
  {
    case 302:
    {
      pszHTML = m_psz302HTML;
      dwSize = m_dw302HTML;
      break;
    }
    case 400:
    {
      pszHTML = m_psz400HTML;
      dwSize = m_dw400HTML;
      break;
    }
    case 401:
    {
      pszHTML = m_psz401HTML;
      dwSize = m_dw401HTML;
      break;
    }
    case 404:
    {
      pszHTML = m_psz404HTML;
      dwSize = m_dw404HTML;
      break;
    }
    default:
    {
      pszHTML = m_psz500HTML;
      dwSize = m_dw500HTML;
      break;
    }
  }

  return pszHTML;
}

char* CHttpServer::GetFileDeletedHTML(DWORD& dwSize)
{
  dwSize = m_dwDeletedHTML;
  return m_pszDeletedHTML;
}
