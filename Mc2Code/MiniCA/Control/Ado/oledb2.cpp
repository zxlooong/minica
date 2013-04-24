//
// MODULE: oledb2.cpp
//
// AUTHOR: Carlos Antollini <cantollini@hotmail.com>
//
// Copyright (c) 2001-2004. All Rights Reserved.
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

#include "oledb2.h"

COLEDBDataLink::COLEDBDataLink()
{
	m_pDataLink = NULL;
}

COLEDBDataLink::~COLEDBDataLink()
{
	m_pDataLink = NULL;
}

CString COLEDBDataLink::New(HWND hWnd /*= NULL*/)
{
	m_pDataLink = NULL;
	m_pDataLink.CreateInstance(__uuidof(DataLinks));
	try
	{
		if(hWnd = NULL) m_pDataLink->PuthWnd(reinterpret_cast<long>(hWnd));
		IDispatchPtr pDisp = m_pDataLink->PromptNew();
		
		_ConnectionPtr conn = pDisp;

		CString strReturn = conn->GetConnectionString().copy();

		m_pDataLink.Release();
		m_pDataLink = NULL;
		return strReturn;
	}
	catch(_com_error &e)
	{
		throw COLEDBException(e.WCode(), e.Description());
	}
}

void COLEDBDataLink::Edit(LPCSTR lpstrConnectionString, HWND hWnd)
{
	BOOL bRet;
	m_pDataLink = NULL;
	IDispatch* pDispatch = NULL;
	_ConnectionPtr pAdoConnection;

	m_pDataLink.CreateInstance(__uuidof(DataLinks));
	pAdoConnection.CreateInstance(__uuidof(ADODB::Connection));
	
	try
	{
		m_pDataLink->PuthWnd(reinterpret_cast<long>(hWnd));
		pAdoConnection->PutConnectionString(_bstr_t(lpstrConnectionString));
		pAdoConnection.QueryInterface(IID_IDispatch, (LPVOID*)&pDispatch);

		bRet = m_pDataLink->PromptEdit(&pDispatch) == VARIANT_TRUE;

		m_pDataLink.Release();
		pAdoConnection.Release();
	}
	catch(_com_error &e)
	{
		throw COLEDBException(e.WCode(), e.Description());
	}
}