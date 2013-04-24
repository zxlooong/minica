/*
Module : HttpDirectory.h
Purpose: Defines the interface for the CHttpDirectory class
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

#ifndef __HTTPDIRECTORY_H__
#define __HTTPDIRECTORY_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif



/////////////////////////////// Classes ///////////////////////////////////////

/// Forward declaration
class CHttpClient;

//The values relating to a single directory which the web server handles
class W3MFC_EXT_CLASS CHttpDirectory
{
public:
//Constructors / Destructors
  CHttpDirectory();
  ~CHttpDirectory();

//Accessors / Mutators
  void SetAlias(const CString& sAlias);
  void SetDirectory(const CString& sDirectory);
  void SetDefaultFile(const CString& sDefaultFile);
  void SetDirectoryListing(BOOL bListing) { m_bDirectoryListing = bListing; };
  void SetWritable(BOOL bWritable) { m_bWritable = bWritable; };
  void SetScript(BOOL bScript) { m_bScript = bScript; };
  void SetUsername(const CString& sUsername) { m_sUsername = sUsername; };
  void SetPassword(const CString& sPassword) { m_sPassword = sPassword; };
  void SetRealm(const CString& sRealm) { m_sRealm = sRealm; };
  CString GetAlias() const { return m_sAlias; };
  CString GetDirectory() const { return m_sDirectory; };
  CString GetDefaultFile() const { return m_sDefaultFile; };
  BOOL    GetDirectoryListing() const { return m_bDirectoryListing; };
  BOOL    GetWritable() const { return m_bWritable; };
  BOOL    GetScript() const { return m_bScript; };
  CString GetUsername() const { return m_sUsername; };
  CString GetPassword() const { return m_sPassword; };
  CString GetRealm() const { return m_sRealm; };

//Methods
  virtual void HandleDirectory(CHttpClient* pClient, BOOL bDirectory);

protected:
//Virtual methods
#ifndef W3MFC_NO_ISAPI_SUPPORT
  virtual void    TransmitISAPIRequest(CHttpClient* pClient, const CString& sDLL);
#endif
  virtual void    TransmitDirectory(CHttpClient* pClient);
  virtual void    TransmitFile(CHttpClient* pClient);
  virtual BOOL    HandleDirectoryAuthorization(CHttpClient* pClient);

//Member variables
  CString      m_sAlias;            //The directory which clients see e.g. "/cgi-bin"
  CString      m_sDirectory;        //The local directory to map requests to
  CString      m_sDefaultFile;      //The file to send when requesting this direcory without a filename
  BOOL         m_bDirectoryListing; //If TRUE then a directory listing will be returned in preference to the default file
  BOOL         m_bScript;           //If TRUE then this directory allows script execution
  BOOL         m_bWritable;         //If TRUE then write access is allowed on this directory, need when supporting "DELETE"
  CString      m_sUsername;         //If non-empty the username required to get access to this directory
  CString      m_sPassword;         //the password associated with "m_sUsername" above.
  CString      m_sRealm;            //The realm to display for this directory, if left empty then "m_sAlias" will be used instead
};

#endif //__HTTPDIRECTORY_H__
