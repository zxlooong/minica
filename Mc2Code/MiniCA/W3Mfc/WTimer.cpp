/*
Module : WTimer.cpp
Purpose: Implementation for an MFC wrapper class for waitable timers
Created: PJN / 06-08-2000
History: PJN / 08-11-2002 1. Removed some unnecessary comments in the module
                          2. Updated copyright details in the module
         PJN / 08-02-2004 1. Completion routine can now be passed an LPVOID parameter just like 
                          the SDK equivalent. Thanks to Erenst F. Wepener for suggesting this 
                          addition.
                          2. Minor updates to the code to allow the code to compile on VC 5 
                          without the requirement for the Platform SDK to be installed.

Copyright (c) 2000 - 2004 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "WTimer.h"



//////////////// Macros / Locals /////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

//Class which handles dynamically calling function which must be constructed at run time
//since waitable timers are not implemented on NT 3.51 or Windows 95. To avoid the loader
//bringing up a message such as "Failed to load due to missing export...", the
//function is constructed using GetProcAddress. The CWaitableTimer function then checks 
//to see if these function pointer is NULL and if it returns FALSE and sets the last error
//to ERROR_CALL_NOT_IMPLEMENTED which is what 95/NT 3.51 would have done if it had implemented 
//a stub for it in the first place !!

class _WAITABLE_TIMER_DATA
{
public:
//Constructors /Destructors
  _WAITABLE_TIMER_DATA();
  ~_WAITABLE_TIMER_DATA();

  HINSTANCE m_hKernel32;

  typedef HANDLE (__stdcall OPENWAITABLETIMER)(DWORD, BOOL, LPCTSTR);
  typedef OPENWAITABLETIMER* LPOPENWAITABLETIMER;

  typedef HANDLE (__stdcall CREATEWAITABLETIMER)(LPSECURITY_ATTRIBUTES, BOOL, LPCTSTR);
  typedef CREATEWAITABLETIMER* LPCREATEWAITABLETIMER;

  typedef VOID (APIENTRY *PTIMERAPCROUTINE)(LPVOID, DWORD, DWORD);
  typedef BOOL (__stdcall SETWAITABLETIMER)(HANDLE, const LARGE_INTEGER*, LONG, PTIMERAPCROUTINE, LPVOID, BOOL);
  typedef SETWAITABLETIMER* LPSETWAITABLETIMER;

  typedef BOOL (__stdcall CANCELWAITABLETIMER)(HANDLE);
  typedef CANCELWAITABLETIMER* LPCANCELWAITABLETIMER;

  LPOPENWAITABLETIMER   m_lpfnOpenWaitableTimer;
  LPCREATEWAITABLETIMER m_lpfnCreateWaitableTimer;
  LPSETWAITABLETIMER    m_lpfnSetWaitableTimer;
  LPCANCELWAITABLETIMER m_lpfnCancelWaitableTimer;
};

_WAITABLE_TIMER_DATA::_WAITABLE_TIMER_DATA()
{
  m_hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));
  VERIFY(m_hKernel32 != NULL);
#ifdef _UNICODE
  m_lpfnOpenWaitableTimer = (LPOPENWAITABLETIMER) GetProcAddress(m_hKernel32, "OpenWaitableTimerW");
  m_lpfnCreateWaitableTimer = (LPCREATEWAITABLETIMER) GetProcAddress(m_hKernel32, "CreateWaitableTimerW");
#else
  m_lpfnOpenWaitableTimer = (LPOPENWAITABLETIMER) GetProcAddress(m_hKernel32, "OpenWaitableTimerA");
  m_lpfnCreateWaitableTimer = (LPCREATEWAITABLETIMER) GetProcAddress(m_hKernel32, "CreateWaitableTimerA");
#endif
  m_lpfnSetWaitableTimer = (LPSETWAITABLETIMER) GetProcAddress(m_hKernel32, "SetWaitableTimer");
  m_lpfnCancelWaitableTimer = (LPCANCELWAITABLETIMER) GetProcAddress(m_hKernel32, "CancelWaitableTimer");
}

_WAITABLE_TIMER_DATA::~_WAITABLE_TIMER_DATA()
{
  FreeLibrary(m_hKernel32);
  m_hKernel32 = NULL;
}


//The local variable which handle the function pointers

_WAITABLE_TIMER_DATA _WaitableTimerData;



CWaitableTimer::CWaitableTimer()
{
  m_hTimer = NULL;
  m_bSignalWhenSuspended = FALSE;
  m_lpfnCompletionRoutine = NULL;
  m_lpArgToCompletionRoutine = NULL;
}

CWaitableTimer::~CWaitableTimer()
{
  Close();
}

BOOL CWaitableTimer::Attach(HANDLE hTimer)
{
  Close();
  m_hTimer = hTimer;
  return TRUE;
}

HANDLE CWaitableTimer::Detach()
{
  HANDLE hReturn = m_hTimer;
  m_hTimer = NULL;
  return hReturn;
}

BOOL CWaitableTimer::Open(LPCTSTR pszName, BOOL bInherit, DWORD dwAccess)
{
  //Free up any existant timer
  Close();

  //Call the SDK function
  if (_WaitableTimerData.m_lpfnOpenWaitableTimer)
  {
    m_hTimer = _WaitableTimerData.m_lpfnOpenWaitableTimer(dwAccess, bInherit, pszName);
    return (m_hTimer != NULL);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::Create(BOOL bManualReset, LPCTSTR pszName, LPSECURITY_ATTRIBUTES lpTimerAttributes)
{
  //Free up any existant timer
  Close();

  //Call the SDK function
  if (_WaitableTimerData.m_lpfnCreateWaitableTimer)
  {
    m_hTimer = _WaitableTimerData.m_lpfnCreateWaitableTimer(lpTimerAttributes, bManualReset, pszName);
    return (m_hTimer != NULL);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

void CWaitableTimer::Close()
{
  if (m_hTimer)
  {
    //Free up any existant timer
    ::CloseHandle(m_hTimer);
    m_hTimer = NULL;
    m_bSignalWhenSuspended = FALSE;
  }
}

BOOL CWaitableTimer::SetOnceOffAbsolute(const SYSTEMTIME& DueTime)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (_WaitableTimerData.m_lpfnSetWaitableTimer)
  {
    //Convert from the SystemTime to the filetime struct
    FILETIME ft;
    if (!::SystemTimeToFileTime(&DueTime, &ft))
    {
      TRACE(_T("Failed in call to SystemTimeToFileTime in SetOnceOffAbsolute\n"));
      return FALSE;
    }

    LARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime; 
    return _WaitableTimerData.m_lpfnSetWaitableTimer(m_hTimer, &li, 0, m_lpfnCompletionRoutine ? 
                                                     _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::SetOnceOffRelative(DWORD dwMilliseconds)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (_WaitableTimerData.m_lpfnSetWaitableTimer)
  {
    //Convert from the milliseconds to FILETIME representation (100-nanoseconds)
    //We also need to make it negative
    __int64 nFileTimes = ((__int64)-10000) * ((__int64)dwMilliseconds);
    LARGE_INTEGER li;
    li.LowPart = (DWORD) (nFileTimes & 0xFFFFFFFF);
    li.HighPart = (LONG) (nFileTimes >> 32); 
    return _WaitableTimerData.m_lpfnSetWaitableTimer(m_hTimer, &li, 0, m_lpfnCompletionRoutine ? 
                                                     _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::SetPeriodicAbsolute(const SYSTEMTIME& DueTime, DWORD dwPeriod)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (_WaitableTimerData.m_lpfnSetWaitableTimer)
  {
    //Convert from the SystemTime to the filetime struct
    FILETIME ft;
    if (!::SystemTimeToFileTime(&DueTime, &ft))
    {
      TRACE(_T("Failed in call to SystemTimeToFileTime in SetPeriodicAbsolute\n"));
      return FALSE;
    }

    LARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime; 
    return _WaitableTimerData.m_lpfnSetWaitableTimer(m_hTimer, &li, dwPeriod, m_lpfnCompletionRoutine ? 
                                                     _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::SetPeriodicRelative(DWORD dwMilliseconds, DWORD dwPeriod)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (_WaitableTimerData.m_lpfnSetWaitableTimer)
  {
    //Convert from the milliseconds to FILETIME representation (100-nanoseconds)
    //We also need to make it negative
    __int64 nFileTimes = ((__int64)-10000) * ((__int64)dwMilliseconds);
    LARGE_INTEGER li;
    li.LowPart = (DWORD) (nFileTimes & 0xFFFFFFFF);
    li.HighPart = (LONG) (nFileTimes >> 32); 
    return _WaitableTimerData.m_lpfnSetWaitableTimer(m_hTimer, &li, dwPeriod, m_lpfnCompletionRoutine ? 
                                                     _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::Cancel()
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (_WaitableTimerData.m_lpfnCancelWaitableTimer)
    return _WaitableTimerData.m_lpfnCancelWaitableTimer(m_hTimer);
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

VOID CWaitableTimer::_CompletionRoutine(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
  //Convert from the SDK world to the C++ world
  CWaitableTimer* pTimer = (CWaitableTimer*) lpArgToCompletionRoutine;
  ASSERT(pTimer);

  //Call the C++ callback method if there is one
  if (pTimer->m_lpfnCompletionRoutine)
  {
    FILETIME ft;
    ft.dwLowDateTime = dwTimerLowValue;
    ft.dwHighDateTime = dwTimerHighValue;

    //Call the C++ callback method
    pTimer->m_lpfnCompletionRoutine(pTimer->m_lpArgToCompletionRoutine, ft);
  }
}
