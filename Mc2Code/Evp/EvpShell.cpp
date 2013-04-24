// EvpShell.cpp : Implementation of CEvpShell
#include "stdafx.h"
#include "Evp.h"
#include "EvpShell.h"

/////////////////////////////////////////////////////////////////////////////
// CEvpShell

HRESULT CEvpShell::Initialize ( LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID ) 
{ 
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP hDrop; 
	
	// 在数据对象内查找 CF_HDROP 型数据. 
	if ( FAILED( pDataObj->GetData ( &fmt, &stg ))) 
	{ 
		// Nope! Return an "invalid argument" error back to Explorer.
		return E_INVALIDARG; 
	}
	
	// 获得指向实际数据的指针
	hDrop = (HDROP) GlobalLock ( stg.hGlobal );
	
	// 检查非NULL. 
	if ( NULL == hDrop ) 
	{ 
		return E_INVALIDARG;
	}
	// 有效性检查 – 保证最少有一个文件名. 
	UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 ); 
	if ( 0 == uNumFiles )
	{ 
		GlobalUnlock ( stg.hGlobal );
		ReleaseStgMedium ( &stg ); 
		return E_INVALIDARG; 
	} 
	HRESULT hr = S_OK; 
	
	// 取得第一个文件名并把它保存在类成员m_szFile 中.
	if ( 0 == DragQueryFile ( hDrop, 0, m_szFile, MAX_PATH ))
	{ 
		hr = E_INVALIDARG; 
	} 
	GlobalUnlock ( stg.hGlobal ); 
	ReleaseStgMedium ( &stg ); 
	return hr; 
}

HRESULT CEvpShell::QueryContextMenu ( HMENU hMenu,UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags )
{          
	// 如果标志包含 CMF_DEFAULTONLY 我们不作任何事情. 
	if ( uFlags & CMF_DEFAULTONLY ) 
	{ 
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 ); 
	}
	InsertMenu(hMenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd, _T("MiniCA Evp")); 
	m_hBmp = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_BITMAP));   
	SetMenuItemBitmaps(hMenu, uMenuIndex, MF_BYPOSITION, m_hBmp, m_hBmp);  
	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 1 );
}

HRESULT CEvpShell::GetCommandString( UINT idCmd, UINT uFlags,UINT* pwReserved, LPSTR pszName, UINT cchMax ) 
{
	USES_CONVERSION; 
	//检查 idCmd, 它必须是0，因为我们仅有一个添加的菜单项. 
	if ( 0 != idCmd ) 
		return E_INVALIDARG; 
	
	// 如果 Explorer 要求帮助字符串，就将它拷贝到提供的缓冲区中. 
	if ( uFlags & GCS_HELPTEXT ) 
	{
		LPCTSTR szText = _T("用MiniCA Evp打开");              
		if ( uFlags & GCS_UNICODE )
		{ 
			// 我们需要将 pszName 转化为一个 Unicode 字符串, 接着使用Unicode字符串拷贝 API. 
			lstrcpynW ( (LPWSTR) pszName, T2CW(szText), cchMax ); 
		} 
		else
		{ 
			// 使用 ANSI 字符串拷贝API 来返回帮助字符串. 
			lstrcpynA ( pszName, T2CA(szText), cchMax ); 
		} 
		return S_OK;
	} 
	return E_INVALIDARG; 
}

HRESULT CEvpShell::InvokeCommand ( LPCMINVOKECOMMANDINFO pCmdInfo )
{
    // 如果lpVerb 实际指向一个字符串, 忽略此次调用并退出.
    if ( 0 != HIWORD( pCmdInfo->lpVerb ))
        return E_INVALIDARG;
    // 点击的命令索引 – 在这里，唯一合法的索引为0.
    switch ( LOWORD( pCmdInfo->lpVerb ))
	{
	case 0:
		{
  //          TCHAR szMsg [MAX_PATH + 32];
   //         wsprintf ( szMsg, _T("The selected file was:\n\n%s"), m_szFile );
   //         MessageBox ( pCmdInfo->hwnd, szMsg, _T("SimpleShlExt"),
//				MB_ICONINFORMATION );
			::ShellExecute(0, _T("open"), m_szFile, NULL, NULL, SW_SHOWNORMAL); 
            return S_OK;
		}
        break;
	default:
		return E_INVALIDARG;
        break;
	}
	DeleteObject(m_hBmp);
}
