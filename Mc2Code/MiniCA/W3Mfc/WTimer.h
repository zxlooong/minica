/*
Module : WTimer.H
Purpose: Defines the interface for an MFC wrapper class for waitable timers
Created: PJN / 06-08-2000

Copyright (c) 2000 - 2004 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __WTIMER_H__
#define __WTIMER_H__

//The following defines allow the code to compile without requiring the Platform SDK to be installed.
#ifndef TIMER_QUERY_STATE
#define TIMER_QUERY_STATE       0x0001
#endif

#ifndef TIMER_MODIFY_STATE
#define TIMER_MODIFY_STATE      0x0002
#endif

#ifndef TIMER_ALL_ACCESS
#define TIMER_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|TIMER_QUERY_STATE|TIMER_MODIFY_STATE)
#endif



/////////////////////////////// Classes ///////////////////////////////////////

class CWaitableTimer
{
public:
//Constructors / Destructors
  CWaitableTimer();
  virtual ~CWaitableTimer();

//typedefs
  typedef void (COMPLETION_ROUTINE)(LPVOID, const FILETIME&);
  typedef COMPLETION_ROUTINE* LPCOMPLETION_ROUTINE;

//General methods
  BOOL     Open(LPCTSTR pszName, BOOL bInherit = FALSE, DWORD dwAccess = TIMER_ALL_ACCESS);
  BOOL     Create(BOOL bManualReset, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES lpTimerAttributes = NULL);
  void     Close();
  operator HANDLE() { return m_hTimer; };
  BOOL     Attach(HANDLE hTimer);
  HANDLE   Detach();

//Timer specific methods
  BOOL     SetOnceOffAbsolute(const SYSTEMTIME& DueTime);
  BOOL     SetOnceOffRelative(DWORD dwMilliseconds);
  BOOL     SetPeriodicAbsolute(const SYSTEMTIME& DueTime, DWORD dwPeriod);
  BOOL     SetPeriodicRelative(DWORD dwMilliseconds, DWORD dwPeriod);
  void     SetSignalWhenSuspended(BOOL bSignal) { m_bSignalWhenSuspended = bSignal; };
  BOOL     Cancel();
  void     SetCompletionRoutine(LPCOMPLETION_ROUTINE lpCompletionRoutine, LPVOID lpArgToCompletionRoutine = NULL) { m_lpfnCompletionRoutine = lpCompletionRoutine; m_lpArgToCompletionRoutine = lpArgToCompletionRoutine; };

protected:
//Methods
  static VOID APIENTRY _CompletionRoutine(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue);

//Member variables
  HANDLE               m_hTimer;
  BOOL                 m_bSignalWhenSuspended;
  LPCOMPLETION_ROUTINE m_lpfnCompletionRoutine;
  LPVOID               m_lpArgToCompletionRoutine;
};

#endif //__WTIMER_H__
