//
// MODULE: oledb2.h
//
// AUTHOR: Carlos Antollini <cantollini@hotmail.com>
//
// Copyright (c) 2001-2005. All Rights Reserved.
//
// Date: August 08, 2004
//
// Version 1.01
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEDB2_H_INCLUDED_)
#define AFX_OLEDB2_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning (disable: 4192)
#include "ado2.h"
//#pragma message ("Make sure you go to Tools.Options.Directories.Library files and add the paths to oledb32.dll and msjro.dll will usually be in C:\\Program Files\\Common Files\\System\\ole db")
//#import "C:\Program Files\Common Files\System\ole db\oledb32.dll" no_namespace
#import <oledb32.dll> no_namespace

#pragma warning (default: 4192)

class COLEDBDataLink
{
public:
	COLEDBDataLink();
	virtual ~COLEDBDataLink();
	CString New(HWND hWnd = NULL);
	void Edit(LPCSTR lpstrConnectionString, HWND hWnd);

private:
	IDataSourceLocatorPtr m_pDataLink;
};


/////////////////////////////////////////////////////////////////////
//
//		COLEDBException Class
//

class COLEDBException
{
public:
	COLEDBException() :
		m_lErrorCode(0),
		m_strError(_T(""))
		{
		}

	COLEDBException(long lErrorCode) :
		m_lErrorCode(lErrorCode),
		m_strError(_T(""))
		{
		}

	COLEDBException(long lErrorCode, const CString& strError) :
		m_lErrorCode(lErrorCode),
		m_strError(strError)
		{
		}

	COLEDBException(const CString& strError) :
		m_lErrorCode(0),
		m_strError(strError)
		{
		}

	COLEDBException(long lErrorCode, const char* szError) :
		m_lErrorCode(lErrorCode),
		m_strError(szError)
		{
		}

	COLEDBException(const char* szError) :
		m_lErrorCode(0),
		m_strError(szError)
		{
		}

	virtual ~COLEDBException()
		{
		}

	CString GetErrorMessage() const
		{return m_strError;};
	void SetError(LPCSTR lpstrError = _T(""))
		{m_strError = lpstrError;};
	long GetError()
		{return m_lErrorCode;};
	void SetError(long lErrorCode = 0)
		{m_lErrorCode = lErrorCode;};

protected:
	CString m_strError;
	long m_lErrorCode;
};

#endif //AFX_OLEDB2_H_INCLUDED_
