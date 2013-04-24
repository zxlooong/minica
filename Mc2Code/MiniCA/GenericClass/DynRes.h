// Res1.h: interface for the CRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RES1_H__6D156E54_F657_47B0_93D1_81689334280D__INCLUDED_)
#define AFX_RES1_H__6D156E54_F657_47B0_93D1_81689334280D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct __ResInfo
{
	int Type_Type;//0 = NUMBER,other string
	CString m_Name;
	CString	m_Type;
	CString m_ReadibleName;
	__ResInfo(const CString &name, const CString& type):m_Name(name),m_Type(type),Type_Type(0){};
	__ResInfo():m_Type(_T("")),Type_Type(0){};
	__ResInfo(const __ResInfo &info){*this = info;};
	__ResInfo& operator= (const __ResInfo& info)
	{
		m_Name = info.m_Name ;
		m_Type = info.m_Type ;
		m_ReadibleName = info.m_ReadibleName ;
		Type_Type = info.Type_Type ;
		return *this;
	}

};
class CRes  
{
public:
	CRes();
	CRes(HMODULE Exe);
	virtual ~CRes();
public:
	// Lock the resource for furthre usage. It return all data
	// of specified resource.
    LPSTR LockRes();
	// Get the loaded resource¡¯s handle 
    HRSRC GetLoadedRes();
	// Load all resource¡¯s list from executable files.
    BOOL LoadAllResource();
	// static function for name in the current executable file.
    static BOOL EnumNames( HANDLE hModule, LPCTSTR lpType, 
		LPTSTR lpName, LONG lParam);
	// static function to enumerate all types in the current executable file.
    static BOOL EnumTypesFunc( HANDLE hModule, LPSTR lpType, LONG lParam);
	// Load  currenct resource
    BOOL LoadResource();
	// Find the given specified resource from executable files
    BOOL FindResource( int res,LPSTR type);
	// Free the executable files locked while reading resource from it .
    void FreeIt();
	// Load the executable files, return TRUE if is successfully
	// reading the executable file
    BOOL LoadExe();
	// The path to store the executable file
    CString m_Path;
	// List of resource in the current executable file
    CArray<__ResInfo,__ResInfo&> m_Infos;
protected:
	HRSRC hResLoad;     // handle to loaded resource 
	HANDLE hExe;        // handle to existing .EXE file 
	HRSRC hRes;         // handle/ptr. to res. info. in hExe 
	//HANDLE hUpdateRes;  // update resource handle 
	char *lpResLock;    // pointer to resource data 
};

#endif // !defined(AFX_RES1_H__6D156E54_F657_47B0_93D1_81689334280D__INCLUDED_)
