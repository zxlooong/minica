/*
Module : IOCPThreadPoolQueue.cpp
Purpose: Implementation for an MFC class which implements a queue for CThreadPoolServer using an SDK IO Completion port
Created: PJN / 18-08-2002
History: PJN / 10-11-2002 1. Fixed an unreferrenced variable in the function CIOCPThreadPoolQueue::GetRequest, Thanks to
                          Michael K. O'Neill for reporting this issue.

Copyright (c) 2002 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/



/////////////////// Includes //////////////////////////////////////////////////

#include "stdafx.h"
#include "IOCPThreadPoolQueue.h"



/////////////////// Macros / Defines //////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



///////////////// Implementation //////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CIOCPThreadPoolQueue, CThreadPoolQueue)

CIOCPThreadPoolQueue::CIOCPThreadPoolQueue()
{
  m_hIOCP = NULL;
}

CIOCPThreadPoolQueue::~CIOCPThreadPoolQueue()
{
  Close();
}

BOOL CIOCPThreadPoolQueue::Create(DWORD dwMaxQSize)
{
  //Close if already created
  Close();

  //Create the IOCP
  m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwMaxQSize);
  if (m_hIOCP == NULL)
  {
    TRACE(_T("CIOCPThreadPoolQueue::Create, Failed to create a IOCP, Error:%d\n"), GetLastError());
    Close();
    return FALSE;
  }

  return TRUE;
}

BOOL CIOCPThreadPoolQueue::PostRequest(const CThreadPoolRequest& request, DWORD /*dwMilliseconds*/)
{
  ASSERT(IsCreated()); //Must have been created

  //An IOCP does not support directed requests
  if (request.m_bDirectedRequest)
  {
    TRACE(_T("CIOCPThreadPoolQueue::PostRequest, An IOCP based thread pool queue does not support directed requests\n"));
    return FALSE;
  }
  else
  {
    if (!PostQueuedCompletionStatus(m_hIOCP, request.m_dwID, NULL, (LPOVERLAPPED) request.m_pData))
    {
      TRACE(_T("CIOCPThreadPoolQueue::PostRequest, Failed in call to PostQueuedCompletionStatus, Error:%d\n"), GetLastError());
      return FALSE;
    }
  }

  return TRUE;
}

BOOL CIOCPThreadPoolQueue::GetRequest(CThreadPoolRequest& request, int /*nThreadIndexForDirectedRequest*/, DWORD dwMilliseconds)
{
  ASSERT(IsCreated()); //Must have been created

  //Pull of a request from the IOCP
  DWORD dwBytesTransferred = 0;
  DWORD dwCompletionKey = 0;
  LPOVERLAPPED lpOverlapped = NULL;
  if (!GetQueuedCompletionStatus(m_hIOCP, &dwBytesTransferred, &dwCompletionKey, &lpOverlapped, dwMilliseconds))
  {
    TRACE(_T("CIOCPThreadPoolQueue::GetRequest, Failed while waiting for the item on the Q, Error:%d\n"), GetLastError());
    return FALSE;
  }

  request.m_bDirectedRequest = FALSE; //Always will be a non-directed request from an IOCP queue
  request.m_nDirectedRequestIndex = -1; 
  request.m_dwID = dwBytesTransferred;
  request.m_pData = lpOverlapped;

  return TRUE;
}

BOOL CIOCPThreadPoolQueue::Close()
{
  //Free up the IOCP
  if (m_hIOCP)
  {
    CloseHandle(m_hIOCP);
    m_hIOCP = NULL;
  }

  return TRUE;
}

BOOL CIOCPThreadPoolQueue::IsCreated() const
{
  return (m_hIOCP != NULL);
}
