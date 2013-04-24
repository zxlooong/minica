/*
Module : Win32Handle.h
Purpose: Defines the interface for the CWin32Handle class
Created: PJN / 08-01-2002
History: None

Copyright (c) 2002 - 2005 by PJ Naughter.  
All rights reserved.

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __WIN32HANDLE_H__
#define __WIN32HANDLE_H__

#ifndef W32HANDLE_EXT_CLASS
#define W32HANDLE_EXT_CLASS
#endif



/////////////////////////////// Classes ///////////////////////////////////////

//class which makes dealing with Win32 handles so much easier. Helpful when implementing
//CGI support and you get pipe handles left right and center !!
class W32HANDLE_EXT_CLASS CW32Handle 
{
public:
//Constructors / Destructors
  CW32Handle();
  CW32Handle(HANDLE hHandle);
  CW32Handle(CW32Handle& handle);
  ~CW32Handle();

//Methods
  void Close();
  void Attach(HANDLE hHandle);
  HANDLE Detach();

//operators
  operator HANDLE() const;
  HANDLE* operator&();
  CW32Handle& CW32Handle::operator=(const CW32Handle& Handle);

//Data
  HANDLE m_hHandle; //The unenderlying Win32 handle   
};

#endif //__WIN32HANDLE_H__
