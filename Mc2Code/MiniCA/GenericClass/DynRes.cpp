// Res1.cpp: implementation of the CRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynRes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRes::CRes()
{
	hExe = NULL;
	hRes = NULL;
	hResLoad = NULL;
	lpResLock = NULL;
}

CRes::CRes(HMODULE Exe)
{
	hExe = Exe;
	hRes = NULL;
	hResLoad = NULL;
	lpResLock = NULL;
}

CRes::~CRes()
{
	FreeIt();
}

BOOL CRes::LoadExe()
{
	FreeIt();
	hExe = LoadLibrary(m_Path ); 
 
	if (hExe == NULL) 
		return FALSE;
	return TRUE;
}

void CRes::FreeIt()
{
	if ( hExe)
		::FreeLibrary ((HMODULE)hExe);
	hExe=NULL;
}



BOOL CRes::FindResource(int res,LPSTR type)
{
	if ( hExe)
		hRes = ::FindResource((HMODULE)hExe, MAKEINTRESOURCE(res), type); 
	else
		return FALSE;
 	if (hRes == NULL) 
		return FALSE;
	return TRUE;
}

BOOL CRes::LoadResource()
{
	if ( hExe)
		if ( hRes)
			hResLoad =(HRSRC):: LoadResource((HMODULE)hExe,hRes); 
	else
		return FALSE;
 
	if (hResLoad == NULL)
		return FALSE;
	return TRUE;
}

BOOL CRes::EnumTypesFunc(HANDLE hModule, LPSTR lpType, LONG lParam)
{
      EnumResourceNames((HINSTANCE)hModule, 
        lpType, 
        (ENUMRESNAMEPROC)EnumNames, 
        lParam); 
 
    return TRUE; 
}

BOOL CRes::EnumNames(HANDLE hModule, LPCTSTR lpType, LPTSTR lpName, LONG lParam)
{
	CArray<__ResInfo,__ResInfo&> *infos =(	CArray<__ResInfo,__ResInfo&> *)lParam;
	__ResInfo info;
	
	
    if ((ULONG)lpName & 0xFFFF0000) 
    { 
		info.m_Name = lpName;
    } 
    else 
    { 
		info.m_Name .Format ("%u",(USHORT)lpName);
    } 
	if ((ULONG)lpType & 0xFFFF0000) 
    { 
       info.m_Type = lpType; 
	   info.Type_Type =1;
    } 
    else 
    { 
		info.Type_Type =0;
		info.m_Type .Format ("%u",(USHORT)lpType);
	} 
 
	if (infos)
		infos->Add (info);
    
    return TRUE; 
}

BOOL CRes::LoadAllResource()
{
  if ( hExe)
 {
	return EnumResourceTypes((HMODULE)hExe,              // module handle 
    (ENUMRESTYPEPROC)EnumTypesFunc,  // callback function 
    (LONG)&m_Infos);                              // extra parameter 
	
  }
  return FALSE;
}

HRSRC CRes::GetLoadedRes()
{
	return hResLoad;
}

LPSTR CRes::LockRes()
{
	lpResLock =(char*)::LockResource(hResLoad);
	return lpResLock;
}
