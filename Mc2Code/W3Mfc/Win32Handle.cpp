/*
Module : Win32Handle.cpp
Purpose: Implementation for the CW32Handle class
Created: PJN / 08-01-2002

Copyright (c) 2002 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "Win32Handle.h"



//////////////// Macros //////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

CW32Handle::CW32Handle()
{
  //Initialize our member variable to sane defaults
  m_hHandle = NULL;
}

CW32Handle::CW32Handle(HANDLE hHandle)
{
  m_hHandle = hHandle;
}

CW32Handle::CW32Handle(CW32Handle& handle)
{
  Attach(handle.Detach());
}

CW32Handle::~CW32Handle()
{
  Close();
}

void CW32Handle::Close()
{
  if (m_hHandle)
  {
    CloseHandle(m_hHandle);
    m_hHandle = NULL;
  }
}

CW32Handle::operator HANDLE() const
{
  return m_hHandle;
}

HANDLE* CW32Handle::operator&()
{
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the "m_hHandle" member explicitly.
  ASSERT(m_hHandle == NULL);
  return &m_hHandle;
}

void CW32Handle::Attach(HANDLE hHandle)
{
  Close();
  m_hHandle = hHandle;
}

CW32Handle& CW32Handle::operator=(const CW32Handle& Handle)
{
  Close();
  m_hHandle = Handle;
  return *this;
}

HANDLE CW32Handle::Detach()
{
  HANDLE hTemp = m_hHandle;
  m_hHandle = NULL;
  return hTemp;
}
