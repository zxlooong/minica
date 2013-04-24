/*
Module : ThrdPool.h
Purpose: Interface for an MFC wrapper class for thread pools
Created: PJN / 15-04-2001

Copyright (c) 2002 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////////////////////////

#ifndef __THRDPOOL_H__
#define __THRDPOOL_H__

#ifndef THRDPOOL_EXT_CLASS
#define THRDPOOL_EXT_CLASS
#endif




////////////////////////////////// Macros / Defines ///////////////////////////////////////////////

const DWORD THREADPOOL_SHUTDOWN_REQUEST = 0;
const DWORD THREADPOOL_USER_DEFINED_REQUEST = 1;



//////////////////////////////////// Classes //////////////////////////////////////////////////////

//Forward declaration
class CThreadPoolServer;

//Represents an instances of a request as stored on the thread pools queue
class THRDPOOL_EXT_CLASS CThreadPoolRequest
{
public:
//Constructors / Destructors
  CThreadPoolRequest();
  CThreadPoolRequest(const CThreadPoolRequest& request);
  virtual ~CThreadPoolRequest();

//methods
  CThreadPoolRequest& operator=(const CThreadPoolRequest& request);

//Member variables
  BOOL  m_bDirectedRequest;        //If this to be a directed request i.e. targetted at a specific thread
#if _MSC_VER >= 1300
  INT_PTR m_nDirectedRequestIndex; //The index of the associated thread if this is a directed request
#else  
  int     m_nDirectedRequestIndex; //The index of the associated thread if this is a directed request
#endif  
  DWORD m_dwID;                    //Any item data you want to associate with the request
  void* m_pData;                   //Any opaque data you want to associate with the requested
};



//Class which you derive from to implement your own thread pool behaviour
class THRDPOOL_EXT_CLASS CThreadPoolClient : public CObject
{
public:
//Constructors / Destructors
  CThreadPoolClient();
  virtual ~CThreadPoolClient(); 

//Methods
  virtual void SetRequestToStop();                     //Called by the manager when CThreadPoolServer::Stop is called. You can use this or 
                                                       //the member variable m_bRequestToStop to effect a premature exit from it, rather than
                                                       //waiting for a stop request to arrive via the thread pool queue

protected:
//Virtual methods
  virtual BOOL InitInstance();                         //Similiar to the MFC function of the same name
  virtual int  ExitInstance();                         //Similiar to the MFC function of the same name
  virtual BOOL Run(const CThreadPoolRequest& request); //The function which is called to handle client requests

  DECLARE_DYNCREATE(CThreadPoolClient)
  
//Member variables
  CWinThread*        m_pWorkerThread;          //The actual worker thread object
  CThreadPoolServer* m_pPoolServer;            //Reference to this objects manager 
  volatile BOOL      m_bRequestToStop;         //Set to TRUE by the manager when CThreadPoolServer::Stop is called. You can use this inside of
                                               //your derived Run method to effect a premature exit from it, rather than waiting for a Stop
                                               //request to arrive via the thread pool queueu
  CEvent             m_evtInitCompleted;       //Signalled when InitInstance has completed, Use by CThreadPoolServer::WaitForThreadsInitInstance
  BOOL               m_bInitOK;                //Has InitInstance completed successfully
  int                m_nStartupThreadPriority; //The thread priority which this thread started with
  UINT               m_nStackSize;             //The size of the stack this thread started with
  int                m_nThreadIndex;           //The thread index of this thread in the thread pool

//The worker thread
  static UINT _Run(LPVOID pParam);
};



//Base class which implements a Queue for the thread pool
class THRDPOOL_EXT_CLASS CThreadPoolQueue : public CObject
{
public:
//Constructors / Destructors
  CThreadPoolQueue();
  virtual ~CThreadPoolQueue();  

//Methods
  virtual BOOL Create(DWORD /*dwMaxQSize*/) { return FALSE; };
  virtual BOOL PostRequest(const CThreadPoolRequest& /*request*/, DWORD /*dwMilliseconds*/ = INFINITE) { return FALSE; };
  virtual BOOL PostRequestWithoutLimitCheck(const CThreadPoolRequest& /*request*/) { return FALSE; };
  virtual BOOL GetRequest(CThreadPoolRequest& /*request*/, int /*nThreadIndexForDirectedRequest*/, DWORD /*dwMilliseconds*/ = INFINITE) { return FALSE; };
  virtual BOOL IsCreated() const { return FALSE; };
  virtual BOOL SupportsDirectedRequests() const { return FALSE; };

protected:
  DECLARE_DYNCREATE(CThreadPoolQueue);
};



//The class which manages the thread pool
class THRDPOOL_EXT_CLASS CThreadPoolServer
{
public:
//Constructors / Destructors
  CThreadPoolServer();
  virtual ~CThreadPoolServer();

//Methods  
  virtual BOOL               Start(CRuntimeClass* pRuntimeClient, CRuntimeClass* pRuntimeQueue, int nPoolSize, int nQueueSize, BOOL bSuspended = FALSE, 
                                   int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0); //Starts up the thread pool
  virtual void               Stop();                                                                                                                                    //Closes down the thread pool
  virtual CThreadPoolClient* GetAtClient(int nIndex);
  virtual BOOL               WaitForThreadsInitInstance();
  BOOL                       SetMaxThreadClientLifetime(BOOL bEnableThreadLifetime, DWORD dwMinutes);
  BOOL                       GetEnabledThreadClientLifetime() const { return m_bMaxLifetime; };
  DWORD                      GetMaxThreadClientLifetime() const { return m_dwMaxLifetime; };
  int                        GetPoolSize() const { return (int) m_Threads.GetSize(); };
  CThreadPoolQueue*          GetQueue() const { return m_pQueue; };

protected:
//typedefs
  typedef HANDLE (*lpfnCreateWaitableTimer)(LPSECURITY_ATTRIBUTES, BOOL, LPCTSTR);
  typedef BOOL (*lpfnSetWaitableTimer)(HANDLE, const LARGE_INTEGER*, LONG, PTIMERAPCROUTINE, LPVOID, BOOL);

//Methods
  static UINT _Monitor(LPVOID pParam);
  virtual UINT Monitor();
  virtual BOOL RecycleThread();

//Member variables
  CArray<CThreadPoolClient*, CThreadPoolClient*&> m_Threads; //The actual thread pool
  CThreadPoolQueue* m_pQueue;                                //The queue class instance
  CCriticalSection m_csThreads;                              //Serializes access to "m_Threads"
  BOOL m_bMaxLifetime;                                       //Should threads be limited to a certain lifetime
  DWORD m_dwMaxLifetime;                                     //Lifetime of threads if m_bMaxLifetime is TRUE
  CWinThread* m_pLifetimeMonitorThread;                      //The thread which recycles the client threads
  CEvent m_evtRequestLifetimeThread;                         //Event which gets set to request the lifetime monitoring thread to exit
  int    m_nLifetimeThreadIndex;                             //The index of the next thread to be recycled
  lpfnCreateWaitableTimer m_lpfnCreateWaitableTimer;         //Waitable timer function pointers
  lpfnSetWaitableTimer m_lpfnSetWaitableTimer;               //Waitable timer function pointers
};

#endif //__THRDPOOL_H__
