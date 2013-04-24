/*
Module : HttpSocket.cpp
Purpose: Implementation for a simple MFC socket wrapper class
Created: PJN / 22-04-1999
History: None                    

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
#include "HttpSocket.h"
#include "HttpClient.h"

#ifndef _WINSOCK2API_
#include <winsock2.h>
#pragma message("To avoid this message please put winsock2.h in your PCH (normally stdafx.h)")
#endif



//////////////// Macros //////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////

BOOL CHttpSocket::SplitRequestLine(const CString& sLine, CString& sField, CString& sValue)
{
  BOOL bSuccess = FALSE;
  
  //Find the first ":" in the line
  int nColon = sLine.Find(_T(':'));
  if (nColon != -1)
  {
    sField = sLine.Left(nColon);
    sValue = sLine.Right(sLine.GetLength()-nColon-1);

    //Trim any leading and trailing spaces
    sField.TrimLeft();
    sField.TrimRight();
    sValue.TrimLeft();
    sValue.TrimRight();
  
    bSuccess = TRUE;
  }

  return bSuccess;
}

BOOL CHttpSocket::SplitRequestLine(LPSTR pszLine, CString& sField, CString& sValue)
{
  return SplitRequestLine(CString(pszLine), sField, sValue);
}

BOOL CHttpSocket::ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CHttpClient& /*client*/, CWaitableTimer& timer, HANDLE hStopEvent, HANDLE hDataEvent)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  //The local variables which will receive the data
  ASSERT(request.m_pRawRequest == NULL);
  request.m_pRawRequest = new BYTE[nGrowBy];
  LPSTR pszTerminator = "\r\n\r\n";
  int nContentLength = -1;
  int nBufSize = nGrowBy;
  
  //retrieve the reponse
	request.m_dwRawRequestSize = 0;
  DWORD dwMaxDataToReceive = (DWORD)-1;
  BOOL bJustFoundTerminator = FALSE;
  BOOL bMoreDataToRead = TRUE;
	while (bMoreDataToRead)
	{
    HANDLE hWaitHandles[3];
    hWaitHandles[0] = hDataEvent;
    hWaitHandles[1] = timer;
    hWaitHandles[2] = hStopEvent;

    //Wait for something interesting to happen
    DWORD dwWait = WaitForMultipleObjects(3, hWaitHandles, FALSE, INFINITE);
    int nSignaledHandle = dwWait - WAIT_OBJECT_0;

    //Work out what the return value from WFMO means!
    if (nSignaledHandle == 0)
    {
      //Reset the timer
      timer.SetOnceOffRelative(dwTimeout);

      //check to see if we have read all the data
      if ((dwMaxDataToReceive != (DWORD)-1) && (request.m_dwRawRequestSize >= dwMaxDataToReceive))
      {
        bMoreDataToRead = FALSE;
        break;
      }

		  //receive the data from the socket
      int nBufRemaining = nBufSize - request.m_dwRawRequestSize - 1; //Allows allow one space for the NULL terminator
      if (nBufRemaining < 0)
        nBufRemaining = 0;

      int nData = 0;
      try
      {
	      nData = Receive(request.m_pRawRequest + request.m_dwRawRequestSize, nBufRemaining);
      }
      catch(CWSocketException* pEx)
      {
        request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
        TRACE(_T("CHttpSocket::ReadResponse, An error occurred reading data from the socket, Error:%d\n"), pEx->m_nError);
        pEx->Delete();
        return FALSE;
      }

      //Increment the count of data received
		  request.m_dwRawRequestSize += nData;							   

      //NULL terminate the data received
	    request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';

      if ((nBufRemaining-nData) == 0) //No space left in the current buffer
      {
        //Allocate the new receive buffer
        nBufSize += nGrowBy; //Grow the buffer by the specified amount
        LPBYTE pNewBuf = new BYTE[nBufSize];

        //copy the old contents over to the new buffer and assign 
        //the new buffer to the local variable used for retreiving 
        //from the socket
        CopyMemory(pNewBuf, request.m_pRawRequest, request.m_dwRawRequestSize);
        delete [] request.m_pRawRequest;
        request.m_pRawRequest = pNewBuf;
      }

      //Check to see if the terminator character(s) have been found
      LPSTR pszTempTerminator = NULL;
      if (!bJustFoundTerminator)
      {
        pszTempTerminator = strstr((LPSTR)request.m_pRawRequest, pszTerminator);
        bJustFoundTerminator = (pszTempTerminator != NULL);
      }

      //Now that we have found the terminator we can get the Content-length if any
      if (bJustFoundTerminator)
      {
        //To cause this code to only be executed once
        bJustFoundTerminator = FALSE;

        //Process each line looking for a content-length
        LPSTR pszLine = (LPSTR) request.m_pRawRequest;
        LPSTR pszNextLine = strstr(pszLine, "\r\n");
        BOOL bMoreLines = TRUE;
        while (bMoreLines) 
        {
          //Form the current line
          int nCurSize = pszNextLine - pszLine + 1;
          char* pszCurrentLine = new char[nCurSize];
          strncpy(pszCurrentLine, pszLine, nCurSize-1);
          pszCurrentLine[nCurSize-1] = '\0'; 

          //Parse the current request line
          CString sField;
          CString sValue;
          if (SplitRequestLine(pszCurrentLine, sField, sValue))
          {
            //Handle any other request headers  
            if (sField.CompareNoCase(_T("Content-Length")) == 0)
              nContentLength = _ttoi(sValue);
          }

          //Tidy up the temp heap memory we have used
          delete [] pszCurrentLine;

          //Move onto the next line
          if (pszNextLine)
          {
            pszLine = pszNextLine+2;
            pszNextLine = strstr(pszLine, "\r\n");

            if (pszNextLine == NULL)
              bMoreLines = FALSE;
          }
        }

        //Found no content length in the header, in that case 
        //we can assume there will be content entity-body and
        //can stop looking for more data
        if (nContentLength == -1)
          bMoreDataToRead = FALSE;
        else
        {
          ASSERT(pszTempTerminator);
          dwMaxDataToReceive = (pszTempTerminator - (LPSTR)(request.m_pRawRequest)) + strlen(pszTerminator) + nContentLength;

          if ((dwMaxDataToReceive != (DWORD)-1) && (request.m_dwRawRequestSize >= dwMaxDataToReceive))
  		    {
            bMoreDataToRead = FALSE;
            break;
	  	    }
        }
      }
    }
    else if (nSignaledHandle == 1)
    {
  	  request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
      TRACE(_T("CHttpSocket::ReadResponse, Timed out waiting for response from socket\n"));
			return FALSE;
    }
    else
    {
  	  request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
      TRACE(_T("CHttpSocket::ReadResponse, Thread being asked to exit\n"));
			return FALSE;
    }
	}

  return TRUE;
}

BOOL CHttpSocket::ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CHttpClient& client)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  //The local variables which will receive the data
  ASSERT(request.m_pRawRequest == NULL);
  request.m_pRawRequest = new BYTE[nGrowBy];
  LPSTR pszTerminator = "\r\n\r\n";
  int nContentLength = -1;
  int nBufSize = nGrowBy;
  
  //retrieve the reponse
	request.m_dwRawRequestSize = 0;
  DWORD dwMaxDataToReceive = (DWORD)-1;
  BOOL bJustFoundTerminator = FALSE;
  BOOL bMoreDataToRead = TRUE;
	while (bMoreDataToRead)
	{
    //Is the thread being asked to exit
    if (client.m_bRequestToStop)
    {
  	  request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
      TRACE(_T("CHttpSocket::ReadResponse, Thread being asked to exit\n"));
			return FALSE;
    }

    //check to see if we have read all the data
    if ((dwMaxDataToReceive != (DWORD)-1) && (request.m_dwRawRequestSize >= dwMaxDataToReceive))
    {
      bMoreDataToRead = FALSE;
      break;
    }

    //check the socket for readability
    try
    {
      if (!IsReadible(dwTimeout))
      {
  	    request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
        TRACE(_T("CHttpSocket::ReadResponse, Timed out waiting for response from socket\n"));
        return FALSE;
      }
    }
    catch(CWSocketException* pEx)
    {
      request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
      TRACE(_T("CHttpSocket::ReadResponse, An error occurred checking the readibility of the socket, Error:%d\n"), pEx->m_nError);
      pEx->Delete();
      return FALSE;
    }

		//receive the data from the socket
    int nBufRemaining = nBufSize - request.m_dwRawRequestSize - 1; //Allows allow one space for the NULL terminator
    if (nBufRemaining < 0)
      nBufRemaining = 0;

    int nData = 0;
    try
    {
	    nData = Receive(request.m_pRawRequest + request.m_dwRawRequestSize, nBufRemaining);
    }
    catch(CWSocketException* pEx)
    {
      request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
      TRACE(_T("CHttpSocket::ReadResponse, An error occurred reading data from the socket, Error:%d\n"), pEx->m_nError);
      pEx->Delete();
      return FALSE;
    }

    //Increment the count of data received
		request.m_dwRawRequestSize += nData;							   

    //NULL terminate the data received
	  request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';

    if ((nBufRemaining-nData) == 0) //No space left in the current buffer
    {
      //Allocate the new receive buffer
      nBufSize += nGrowBy; //Grow the buffer by the specified amount
      LPBYTE pNewBuf = new BYTE[nBufSize];

      //copy the old contents over to the new buffer and assign 
      //the new buffer to the local variable used for retreiving 
      //from the socket
      CopyMemory(pNewBuf, request.m_pRawRequest, request.m_dwRawRequestSize);
      delete [] request.m_pRawRequest;
      request.m_pRawRequest = pNewBuf;
    }

    //Check to see if the terminator character(s) have been found
    LPSTR pszTempTerminator = NULL;
    if (!bJustFoundTerminator)
    {
      pszTempTerminator = strstr((LPSTR)request.m_pRawRequest, pszTerminator);
      bJustFoundTerminator = (pszTempTerminator != NULL);
    }

    //Now that we have found the terminator we can get the Content-length if any
    if (bJustFoundTerminator)
    {
      //To cause this code to only be executed once
      bJustFoundTerminator = FALSE;

      //Process each line looking for a content-length
      LPSTR pszLine = (LPSTR) request.m_pRawRequest;
      LPSTR pszNextLine = strstr(pszLine, "\r\n");
      BOOL bMoreLines = TRUE;
      while (bMoreLines) 
      {
        //Form the current line
        int nCurSize = pszNextLine - pszLine + 1;
        char* pszCurrentLine = new char[nCurSize];
        strncpy(pszCurrentLine, pszLine, nCurSize-1);
        pszCurrentLine[nCurSize-1] = '\0'; 

        //Parse the current request line
        CString sField;
        CString sValue;
        if (SplitRequestLine(pszCurrentLine, sField, sValue))
        {
          //Handle any other request headers  
          if (sField.CompareNoCase(_T("Content-Length")) == 0)
            nContentLength = _ttoi(sValue);
        }

        //Tidy up the temp heap memory we have used
        delete [] pszCurrentLine;

        //Move onto the next line
        if (pszNextLine)
        {
          pszLine = pszNextLine+2;
          pszNextLine = strstr(pszLine, "\r\n");

          if (pszNextLine == NULL)
            bMoreLines = FALSE;
        }
      }

      //Found no content length in the header, in that case 
      //we can assume there will be content entity-body and
      //can stop looking for more data
      if (nContentLength == -1)
        bMoreDataToRead = FALSE;
      else
      {
        ASSERT(pszTempTerminator);
        dwMaxDataToReceive = (pszTempTerminator - (LPSTR)(request.m_pRawRequest)) + strlen(pszTerminator) + nContentLength;

        if ((dwMaxDataToReceive != (DWORD)-1) && (request.m_dwRawRequestSize >= dwMaxDataToReceive))
  		  {
          bMoreDataToRead = FALSE;
          break;
	  	  }
      }
    }
	}

  return TRUE;
}

#ifdef W3MFC_SSL_SUPPORT
BOOL CHttpSocket::ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CSSL& ssl, CHttpClient& client)
{
  if (ssl.operator SSL *())
  {
    //must have been created first
    ASSERT(m_hSocket != INVALID_SOCKET);

    //The local variables which will receive the data
    ASSERT(request.m_pRawRequest == NULL);
    request.m_pRawRequest = new BYTE[nGrowBy];
    LPSTR pszTerminator = "\r\n\r\n";
    int nContentLength = -1;
    int nBufSize = nGrowBy;
  
    //retrieve the reponse
	  request.m_dwRawRequestSize = 0;
    DWORD dwMaxDataToReceive = (DWORD)-1;
    BOOL bJustFoundTerminator = FALSE;
    BOOL bMoreDataToRead = TRUE;
	  while (bMoreDataToRead)
	  {
      //Is the thread being asked to exit
      if (client.m_bRequestToStop)
      {
  	    request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
        TRACE(_T("CHttpSocket::ReadResponse, Thread being asked to exit\n"));
			  return FALSE;
      }

      //check to see if we have read all the data
      if ((dwMaxDataToReceive != (DWORD)-1) && (request.m_dwRawRequestSize >= dwMaxDataToReceive))
      {
        bMoreDataToRead = FALSE;
        break;
      }

      //check the socket for readability
      try
      {
        if (!SSL_pending(ssl)) //only check the socket for readibility if SSL_pending fails
        {
          if (!IsReadible(dwTimeout))
          {
  	        request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
            TRACE(_T("CHttpSocket::ReadResponse, Timed out waiting for response from socket\n"));
			      return FALSE;
          }
        }
      }
      catch(CWSocketException* pEx)
      {
        request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
        TRACE(_T("CHttpSocket::ReadResponse, An error occurred checking the readibility of the socket, Error:%d\n"), pEx->m_nError);
        pEx->Delete();
        return FALSE;
      }

		  //receive the data from the socket
      int nBufRemaining = nBufSize - request.m_dwRawRequestSize - 1; //Allows allow one space for the NULL terminator
      if (nBufRemaining < 0)
        nBufRemaining = 0;

      //Read the data from the socket
      int nData = SSL_read(ssl, request.m_pRawRequest + request.m_dwRawRequestSize, nBufRemaining);
      if (nData <= 0)
      {
        int nSSLError = SSL_get_error(ssl, nData);
        if (nSSLError != SSL_ERROR_WANT_READ)
        {
          request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';
          TRACE(_T("CHttpSocket::ReadResponse, An error occurred reading data from the socket, SSL_read:%d, SSL_get_error:%d\n"), nData, nSSLError);
          return FALSE;
        }
      }
      else if (nData)
      {
        //Increment the count of data received
		    request.m_dwRawRequestSize += nData;							   

        //NULL terminate the data received
	      request.m_pRawRequest[request.m_dwRawRequestSize] = '\0';

        if ((nBufRemaining-nData) == 0) //No space left in the current buffer
        {
          //Allocate the new receive buffer
          nBufSize += nGrowBy; //Grow the buffer by the specified amount
          LPBYTE pNewBuf = new BYTE[nBufSize];

          //copy the old contents over to the new buffer and assign 
          //the new buffer to the local variable used for retreiving 
          //from the socket
          CopyMemory(pNewBuf, request.m_pRawRequest, request.m_dwRawRequestSize);
          delete [] request.m_pRawRequest;
          request.m_pRawRequest = pNewBuf;
        }

        //Check to see if the terminator character(s) have been found
        LPSTR pszTempTerminator = NULL;
        if (!bJustFoundTerminator)
        {
          pszTempTerminator = strstr((LPSTR)request.m_pRawRequest, pszTerminator);
          bJustFoundTerminator = (pszTempTerminator != NULL);
        }

        //Now that we have found the terminator we can get the Content-length if any
        if (bJustFoundTerminator)
        {
          //To cause this code to only be executed once
          bJustFoundTerminator = FALSE;

          //Process each line looking for a content-length
          LPSTR pszLine = (LPSTR) request.m_pRawRequest;
          LPSTR pszNextLine = strstr(pszLine, "\r\n");
          BOOL bMoreLines = TRUE;
          while (bMoreLines) 
          {
            //Form the current line
            int nCurSize = pszNextLine - pszLine + 1;
            char* pszCurrentLine = new char[nCurSize];
            strncpy(pszCurrentLine, pszLine, nCurSize-1);
            pszCurrentLine[nCurSize-1] = '\0'; 

            //Parse the current request line
            CString sField;
            CString sValue;
            if (SplitRequestLine(pszCurrentLine, sField, sValue))
            {
              //Handle any other request headers  
              if (sField.CompareNoCase(_T("Content-Length")) == 0)
                nContentLength = _ttoi(sValue);
            }

            //Tidy up the temp heap memory we have used
            delete [] pszCurrentLine;

            //Move onto the next line
            if (pszNextLine)
            {
              pszLine = pszNextLine+2;
              pszNextLine = strstr(pszLine, "\r\n");

              if (pszNextLine == NULL)
                bMoreLines = FALSE;
            }
          }

          //Found no content length in the header, in that case 
          //we can assume there will be content entity-body and
          //can stop looking for more data
          if (nContentLength == -1)
            bMoreDataToRead = FALSE;
          else
          {
            ASSERT(pszTempTerminator);
            dwMaxDataToReceive = (pszTempTerminator - (LPSTR)(request.m_pRawRequest)) + strlen(pszTerminator) + nContentLength;

            if ((dwMaxDataToReceive != (DWORD)-1) && (request.m_dwRawRequestSize >= dwMaxDataToReceive))
  		      {
              bMoreDataToRead = FALSE;
              break;
	  	      }
          }
        }
      }
	  }

    return TRUE;
  }
  else
    return ReadResponse(request, dwTimeout, nGrowBy, client); 
}

BOOL CHttpSocket::ReadResponse(CHttpRequest& request, DWORD dwTimeout, int nGrowBy, CSSL& ssl, CHttpClient& client, CWaitableTimer& timer, HANDLE hStopEvent, HANDLE hDataEvent)
{
  if (ssl.operator SSL *())
  {
    //We do not support using events for the OpenSSL code path
    return ReadResponse(request, dwTimeout, nGrowBy, ssl, client);
  }
  else
    return ReadResponse(request, dwTimeout, nGrowBy, client, timer, hStopEvent, hDataEvent); 
}
#endif

void CHttpSocket::SendWithRetry(void* pBuffer, int nBuf, DWORD dwTimeout)
{
  BOOL bSent = FALSE;

  do
  {
    try
    {
      //Let the socket do the send
      CWSocket::Send(pBuffer, nBuf);
      bSent = TRUE;
    }
    catch(CWSocketException* pEx)
    {
      //Pull out the exceptions details before we delete it
      int nError = pEx->m_nError;

      //Delete the exception before we go any further
      pEx->Delete();

      if (nError == WSAEWOULDBLOCK)
      {
        //If a WSAEWOULDBLOCK error occurred then wait until the socket becomes
        //writtable or the timeout occurs
        int nEvent = WSAEventSelect(*this, m_WSABlockEvent, FD_WRITE);

        //Handle the error
        if (nEvent != 0)
          AfxThrowWSocketException();

        //Wait for the socket to become writable
        DWORD dwWait = WaitForSingleObject(m_WSABlockEvent, dwTimeout);

        //Cancel the notification
        WSAEventSelect(*this, m_WSABlockEvent, 0);

        //If the socket does not become writable in the timeout period, 
        //then rethrow the exception
        if (dwWait != WAIT_OBJECT_0)
          AfxThrowWSocketException(WSAEWOULDBLOCK);
      }
      else
      {
        //We caught an exception which we do not know how to handle.
        //In this case just rethrow the exception
        AfxThrowWSocketException(nError);
      }
    }
  }
  while (!bSent);
}

#ifdef W3MFC_SSL_SUPPORT
void CHttpSocket::SendWithRetry(void* pBuffer, int nBuf, DWORD dwTimeout, CSSL& ssl)
{
  if (ssl.operator SSL *())
  {
    BOOL bSent = FALSE;

    do
    {
      int nSSLWrite = SSL_write(ssl, pBuffer, nBuf);
      if (nSSLWrite < 1)
      {
        if (SSL_get_error(ssl, nSSLWrite) == SSL_ERROR_WANT_WRITE)
        {
          if (!IsWritable(dwTimeout))
            AfxThrowWSocketException(WSAEWOULDBLOCK);
        }
      }
      else
        bSent = TRUE;
    }
    while (!bSent);
  }
  else
    SendWithRetry(pBuffer, nBuf, dwTimeout);
}
#endif
