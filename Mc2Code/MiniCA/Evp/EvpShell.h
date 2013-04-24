// EvpShell.h : Declaration of the CEvpShell

#ifndef __EVPSHELL_H_
#define __EVPSHELL_H_

#include "resource.h"       // main symbols
#include "shlobj.h"
#include "comdef.h"

/////////////////////////////////////////////////////////////////////////////
// CEvpShell
class ATL_NO_VTABLE CEvpShell : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEvpShell, &CLSID_EvpShell>,
	public IDispatchImpl<IEvpShell, &IID_IEvpShell, &LIBID_EVPLib>,
	public IShellExtInit,
	public IContextMenu 
	{
public:
	CEvpShell()
	{
		m_hBmp = NULL;
	}
	
	DECLARE_REGISTRY_RESOURCEID(IDR_EVPSHELL)
		
		DECLARE_PROTECT_FINAL_CONSTRUCT()
		
		BEGIN_COM_MAP(CEvpShell)
		COM_INTERFACE_ENTRY(IEvpShell)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IShellExtInit) 
		COM_INTERFACE_ENTRY(IContextMenu) 
		END_COM_MAP()
		
		// IEvpShell
public:
protected: 
	TCHAR m_szFile [MAX_PATH]; 
	HBITMAP m_hBmp;
public: 
	// IShellExtInit 
	STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);
	// IContextMenu
	STDMETHOD(GetCommandString)(UINT, UINT, UINT*, LPSTR, UINT);
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO); 
	STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);
};

#endif //__EVPSHELL_H_
