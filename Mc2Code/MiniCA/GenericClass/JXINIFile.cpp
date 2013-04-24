/************************************
  REVISION LOG ENTRY
  Written and Revision By: Zhang, Zhefu
  Contact: codetiger@hotmail
  URL: http://www.chi.c.dendai.ac.jp/~zhang/index.htm
  Revised on 2001/09/27 in FujiSoftABC Ltd. Japan
  Comment: JXINIFile Implementation File
 ************************************/
//
//
//Last Modified 2001/9/4
//Reason: NotePad add 0xFF FE to the text file header, make the first
//        section unrecognizable. 
//Fixment: Add member m_pTrueData to pass the 2 byte head

#include "stdAfx.h"
#include "JXIniFile.h"


//dwSize = -1 if szMultiByte null terminated
//dwSize not include the ending null
//return WCHARs
LPWSTR _A2W(LPSTR szMultiByte, DWORD dwSize, DWORD& dwRetSize)
{
	static WCHAR szInternalBuffer[8096];
	if(szMultiByte == NULL || (dwSize == -1 && ::strlen(szMultiByte) == 0)) 
	{ dwRetSize = 0; return NULL; }
	int nLen = MultiByteToWideChar(CP_ACP, 0,szMultiByte, dwSize, NULL, NULL);
	if(nLen > 8096) { dwRetSize = 0; return NULL; } 
	//dwRetSize in WCHARs, include null if szMultiByte nulled ended
    dwRetSize = MultiByteToWideChar(CP_ACP, 0, szMultiByte, dwSize, szInternalBuffer, 8096);
	if(dwSize == -1)
		dwRetSize--;
	else
	    szInternalBuffer[dwRetSize] = WCHAR('\0');
    return szInternalBuffer;
}

//return BYTEs
LPSTR _W2A(LPWSTR szUnicode, DWORD dwSize, DWORD& dwRetSize)
{
	static char szInternalBuffer[8096];
	if(szUnicode == NULL || (dwSize == -1 && ::wcslen(szUnicode) == 0)) 
	{ dwRetSize = 0; return NULL; }
	int nLen = WideCharToMultiByte(CP_ACP, 0,szUnicode, dwSize, NULL, 0, NULL, FALSE);
	if(nLen > 8096) { dwRetSize = 0; return NULL; } 
	//Ret: in BYTEs, The number includes the byte for the null terminator. if szUnicode null ended
	dwRetSize = WideCharToMultiByte(CP_ACP, 0, szUnicode, dwSize, szInternalBuffer, nLen, NULL, FALSE);
    if(dwSize == -1) //dwRetSize include null, szInternalBuffer null ended
		dwRetSize--;
	else
		szInternalBuffer[dwRetSize] = char('\0');
	return szInternalBuffer;
}

CJXINIFile::CJXINIFile() 
{
	m_strFilename = _T("");
	m_hFile = INVALID_HANDLE_VALUE;
	m_hMMF = NULL;
	m_dwSize = 0;
	m_pData = 0;
	m_bSectionMapDirty = TRUE; //No Section Map
	m_bSectionMapLock = FALSE;
	m_mapSection.InitHashTable(700, TRUE);
}

CJXINIFile::~CJXINIFile()
{
	UnloadIniFile();
	if(m_pData)
	{
		::UnmapViewOfFile(m_pData);
		m_pData = NULL;
	}
	if(m_hMMF != NULL)
	{
		::CloseHandle(m_hMMF);
		m_hMMF = NULL;
	}
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

//CMap m_mapSection; 
void CJXINIFile::UpdateMapA()
{
	if(m_bSectionMapLock) return;
	LPSTR pHead = (LPSTR)m_pTrueData;
	LPSTR p1 = pHead;
	LPSTR p2 = pHead;

	p1 = pHead;
    while(p1- pHead < (int)(m_dwUseSize/sizeof(char)) - 1)
	{
		while(p1- pHead < (int)(m_dwUseSize/sizeof(char)) - 1 &&
			*p1 != char('['))
			p1++;
		p2 = p1;
		while(p2- pHead < (int)(m_dwUseSize/sizeof(char)) - 1 &&
			*p2 != char(']'))
			p2++;
		if(p1- pHead < (int)(m_dwUseSize/sizeof(char)) &&
			p2- pHead < (int)(m_dwUseSize/sizeof(char)))
		{
			//Found, Ready To Map
			int lenSection = p2 - p1 - 1;
			//Section Name Length must >= 1
			if(lenSection >= 1)
			{
				p1++;
				CString strSection;
#ifdef _UNICODE
                DWORD dwRetSize;
				PWSTR pTemp = _A2W(p1, lenSection, dwRetSize);
				::wcsncpy(strSection.GetBuffer(dwRetSize), pTemp, dwRetSize);
                strSection.ReleaseBuffer(dwRetSize);				
#else
				::strncpy(strSection.GetBuffer(lenSection), p1, lenSection);
                strSection.ReleaseBuffer(lenSection);
#endif
				p1--; //point to [
				m_mapSection.SetAt(strSection, (LPVOID)p1);
			} 
		}
		p1 = p2 + 1;
	}
	//Debug Part
	m_bSectionMapDirty = FALSE;
    return ;
}

void CJXINIFile::UpdateMapW()
{
	if(m_bSectionMapLock) return;
	LPWSTR pHead = (LPWSTR)m_pTrueData;
	LPWSTR p1 = pHead;
	LPWSTR p2 = pHead;
    //Calculate the section number
//	int nSection = 0;
//    while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1)
//	{
//		if(*p1 == WCHAR('[')) nSection++;
//		p1++;
//	}
//	if(nSection < 13) nSection = 13;
//	//allow 20% margine
//	m_mapSection.InitHashTable((UINT)(1.2*nSection), TRUE);
    
	p1 = pHead;
    while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1)
	{
		while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p1 != WCHAR('['))
			p1++;
		p2 = p1;
		while(p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p2 != WCHAR(']'))
			p2++;
		if(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) &&
			p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)))
		{
			//Found, Ready To Map
			int lenSection = p2 - p1 - 1;
			//Section Name Length must >= 1
			if(lenSection >= 1)
			{
				p1++;
				CString strSection;
#ifdef _UNICODE
				::wcsncpy(strSection.GetBuffer(lenSection), p1, lenSection);
                strSection.ReleaseBuffer(lenSection);
#else
				DWORD dwRetSize;
				PSTR pTemp = _W2A(p1, lenSection, dwRetSize);
                ::strncpy(strSection.GetBuffer(dwRetSize), pTemp, dwRetSize);
                strSection.ReleaseBuffer(dwRetSize);
#endif
				p1--; //point to [
				m_mapSection.SetAt(strSection, (LPVOID)p1);
			} 
		}
		p1 = p2 + 1;
	}
	//Debug Part
	m_bSectionMapDirty = FALSE;

//  int count = m_mapSection.GetCount();
//	CString str; str.Format(_T("UpdateW %d"), count); 
//	AfxMessageBox(str);
/*
	POSITION pos = m_mapSection.GetStartPosition();
	while(pos)
	{
		CString strKey;
		LPVOID lpPos;
		m_mapSection.GetNextAssoc(pos, strKey, lpPos);
		if(AfxMessageBox(strKey, MB_YESNO | MB_ICONINFORMATION) == IDNO)
			return;
	}
*/
	return ;
}

void CJXINIFile::ResizeMap(int nNewSize)
{
	ASSERT(nNewSize > 0);
	m_mapSection.RemoveAll();
	m_mapSection.InitHashTable(nNewSize);
}

// return 0 -- no key, size too small to put value
//return -1  -- error
//return string length (of course > 1)
//Ex Version use Map
DWORD CJXINIFile::GetPrivateProfileStringExA(
         LPCSTR lpAppName,        // section name
         LPCSTR lpKeyName,        // key name
         LPCSTR lpDefault,        // default string
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              
		 // size of destination buffer,Specifies the size, in TCHARs, 
         LPCSTR lpFileName        // initialization file name
    )
{
	BOOL bRet = LoadIniFile(lpFileName, 1);
	if(!bRet) return (UINT)-1;
	if(lpDefault == NULL) return -1;
	if(m_dwUseSize == 0) return -1;

	//Section Map Not Available
	if(m_bSectionMapDirty && m_bSectionMapLock) //
		return GetPrivateProfileStringA(lpAppName,lpKeyName,
		   lpDefault,lpReturnedString,nSize,lpFileName);
	else if(!m_bSectionMapLock && m_bSectionMapDirty)
	{
		UpdateMapA();        
	}
 
	//AfxMessageBox(_T("Using Section Map"));
	//Now Map Info Available
	if(lpAppName == NULL) //get all section
	{
		return GetSectionsExA(lpReturnedString, nSize, lpFileName);
	}
	if(lpKeyName == NULL) //get all key in one section
	{
		return GetKeysExA(lpAppName, lpReturnedString, nSize, lpFileName);
	}

	LPSTR pHead = (LPSTR)m_pTrueData;
	UINT uSec, uKey;
	uSec = (UINT) -1;
	uKey = (UINT)-1;
	uKey = GetKeyExA(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1) //not key
	{
		if(::strlen(lpDefault) > (int)nSize) return -1;
		::strncpy(lpReturnedString, lpDefault, ::strlen(lpDefault));
		//append a null
        lpReturnedString[::strlen(lpDefault)+1] = char('\0');
		return 0;
	}
	char buffer[MAX_PATH];

    LPSTR pValue = pHead + uKey;
	while(pValue - pHead < (int)((m_dwUseSize)/sizeof(char)) && 
		 *pValue != char('\r') && *pValue != char('\n'))
	{
		pValue++;  //in unicode, +2 byte
	}

	int len = pValue - pHead - uKey;
	pValue = pHead + uKey;
	::strncpy(buffer, pValue, len); 
	if(nSize <= (UINT)len)
	{
		::strncpy(lpReturnedString, pValue, nSize);
        lpReturnedString[nSize - 1] = char('\0');
		return 0;
	}
	else
	{
		::strncpy(lpReturnedString, pValue, len);
		//append null
        lpReturnedString[len] = char('\0');
		return len;			
	}
	return (UINT) -1;
}

// return 0 -- no key, size too small to put value
//return -1  -- error
//return string length (of course > 1)
//Ex Version use Map
DWORD CJXINIFile::GetPrivateProfileStringExW(
         LPCWSTR lpAppName,        // section name
         LPCWSTR lpKeyName,        // key name
         LPCWSTR lpDefault,        // default string
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              
		 // size of destination buffer,Specifies the size, in TCHARs, 
         LPCWSTR lpFileName        // initialization file name
    )
{
	BOOL bRet = LoadIniFile(lpFileName, 2);
	if(!bRet) return (UINT)-1;
	if(lpDefault == NULL) return -1;
	if(m_dwUseSize == 0) return -1;

	//Section Map Not Available
	if(m_bSectionMapDirty && m_bSectionMapLock) //
		return GetPrivateProfileStringW(lpAppName,lpKeyName,
		   lpDefault,lpReturnedString,nSize,lpFileName);
	else if(!m_bSectionMapLock && m_bSectionMapDirty)
	{
		UpdateMapW();        
	}
 
	//AfxMessageBox(_T("Using Section Map"));
	//Now Map Info Available
	if(lpAppName == NULL) //get all section
	{
		return GetSectionsExW(lpReturnedString, nSize, lpFileName);
	}
	if(lpKeyName == NULL) //get all key in one section
	{
		return GetKeysExW(lpAppName, lpReturnedString, nSize, lpFileName);
	}

	LPWSTR pHead = (LPWSTR)m_pTrueData;
	UINT uSec, uKey;
	uSec = (UINT) -1;
	uKey = (UINT)-1;
	uKey = GetKeyExW(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1) //not key
	{
		if(::wcslen(lpDefault) > (int)nSize) return -1;
		::wcsncpy(lpReturnedString, lpDefault, ::wcslen(lpDefault));
		//append a null
        lpReturnedString[::wcslen(lpDefault)+1] = WCHAR('\0');
		return 0;
	}
	WCHAR buffer[MAX_PATH];

    LPWSTR pValue = pHead + uKey;
	while(pValue - pHead < (int)((m_dwUseSize)/sizeof(WCHAR)) && 
		 *pValue != WCHAR('\r') && *pValue != WCHAR('\n'))
	{
		pValue++;  //in unicode, +2 byte
	}

	int len = pValue - pHead - uKey;
	pValue = pHead + uKey;
	::wcsncpy(buffer, pValue, len); 
	if(nSize <= (UINT)len)
	{
		::wcsncpy(lpReturnedString, pValue, nSize);
        lpReturnedString[nSize - 1] = WCHAR('\0');
		return 0;
	}
	else
	{
		::wcsncpy(lpReturnedString, pValue, len);
		//append null
        lpReturnedString[len] = WCHAR('\0');
		return len;			
	}
	return (UINT) -1;
}

//User Query All Section Name in INI File
DWORD CJXINIFile::GetSectionsExA(
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize, // size of destination buffer, In WCHAR
         LPCSTR lpFileName        // initialization file name
    )
{
	ASSERT(!m_bSectionMapDirty);
    BYTE wNULL = 0x00; 
	int posRet = 0;
	POSITION pos = m_mapSection.GetStartPosition();
	while(pos)
	{
		CString strKey;
		LPVOID lpPos;
		m_mapSection.GetNextAssoc(pos, strKey, lpPos);
#ifdef _UNICODE
		int lenSection = strKey.GetLength();
		DWORD dwRetSize;
		LPSTR pTemp = _W2A(strKey.GetBuffer(lenSection), lenSection, dwRetSize);
		if(dwRetSize + 1 + posRet > (int)nSize) return 0; //nSize - 1;
        ::strncpy(lpReturnedString + posRet, pTemp, dwRetSize);
        posRet += dwRetSize;
#else
		int lenSection = strKey.GetLength();
		if(lenSection + 1 + posRet > (int)nSize) return 0; //nSize - 1;
		::strncpy(lpReturnedString + posRet, (LPCSTR)strKey, lenSection);
        posRet += lenSection;
#endif
		//NULL = 0x00 , 2 Byte
		::memcpy(lpReturnedString + posRet, &wNULL, 1);
        posRet += 1;
	}
	//last NULL 
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &wNULL, 1);
	posRet += 1;
	return posRet;
}

//User Query All Section Name in INI File
DWORD CJXINIFile::GetSectionsExW(
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize, // size of destination buffer, In WCHAR
         LPCWSTR lpFileName        // initialization file name
    )
{
	ASSERT(!m_bSectionMapDirty);
    WORD wNULL = 0x0000; 
	int posRet = 0;
	POSITION pos = m_mapSection.GetStartPosition();
	while(pos)
	{
		CString strKey;
		LPVOID lpPos;
		m_mapSection.GetNextAssoc(pos, strKey, lpPos);
#ifdef _UNICODE
		int lenSection = strKey.GetLength();
		if(lenSection + 1 + posRet > (int)nSize) return 0; //nSize - 1;
		::wcsncpy(lpReturnedString + posRet, (LPCWSTR)strKey, lenSection);
        posRet += lenSection;
#else
		int lenSection = strKey.GetLength();
		DWORD dwRetSize;
		PWSTR pTemp = _A2W(strKey.GetBuffer(lenSection), lenSection, dwRetSize);
		if(dwRetSize + 1 + posRet > (int)nSize) return 0; //nSize - 1;
        ::wcsncpy(lpReturnedString + posRet, pTemp, dwRetSize);
        posRet += dwRetSize;
#endif
		//NULL = 0x00 00, 2 Byte
		::memcpy(lpReturnedString + posRet, &wNULL, 2);
        posRet += 1;

	}
	//last NULL 
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &wNULL, 2);
	posRet += 1;
	return posRet;
}

//User Query All Keys in a Given Section
DWORD CJXINIFile::GetKeysExA(
		 LPCSTR lpAppName,        // section name
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    )
{
	int posRet = 0;
	int secLen = ::strlen(lpAppName);
	LPSTR pHead = (LPSTR)m_pTrueData;
	
	LPSTR pSecHead = NULL; //pointing to [
	//Section Map Must be Available
	ASSERT(!m_bSectionMapDirty);

#ifdef _UNICODE
	DWORD dwRetSize;
	LPWSTR pTemp = _A2W((LPSTR)lpAppName, -1, dwRetSize);
    if(0 == m_mapSection.Lookup((LPCWSTR)pTemp, (LPVOID&)pSecHead))
		return 0;
#else
	if(0 == m_mapSection.Lookup(lpAppName, (LPVOID&)pSecHead))
		return 0;	
#endif

	LPSTR pSecTail = NULL; //point to next [
	LPSTR p1 = pSecHead;
	LPSTR p2;
	        
	BYTE wNULL = 0x00; 
	while(p1- pHead < (int)(m_dwUseSize/sizeof(char)) - 1)
	{
		p1++;
		if(*p1 == char('['))
		{
			pSecTail = p1;
			break;
		}
	}
	if(pSecHead == NULL) return -1;
	if(pSecTail == NULL)
		pSecTail = pHead + m_dwUseSize/sizeof(char);
	pSecTail--;
    
	//Enum Keys in Section
    p1 = pSecHead;
	int keyLen;
	while(p1 < pSecTail - 1)
	{
		while(p1 < pSecTail - 1 &&
			*p1 != char('\r') && *p1 != char('\n'))
			p1++;
		while(p1 < pSecTail - 1 &&
			(*p1 == char('\r') || *p1 == char('\n')))
			p1++;
		p2 = p1;
		while(p2 < pSecTail - 1 &&
			*p2 != char('='))
			p2++;
		if(p1 < pSecTail - 1 && p2 < pSecTail - 1)
		{			
			//Found A Key
			keyLen = p2 - p1;

//			CString str;
//			::lstrcpyn(str.GetBuffer(keyLen), p1, keyLen);
//			str.ReleaseBuffer(keyLen);
//			PopMsg(_T("Used %d KeyLen %d, %s"), posRet, keyLen, str);
            
			if(keyLen >= 1)
			{
				if(keyLen + 1 + posRet > (int)nSize) 
				{
					//PopMsg(_T("You Want Fetch Sections, Buffer %d, Keylen %d, Used %d, in %s"),
					//	nSize, keyLen, posRet, m_strFilename);
					return 0; //nSize - 1;
				}
				::strncpy(lpReturnedString + posRet, p1, keyLen);
                posRet += keyLen;
				//NULL = 0x00 , 1 Byte
				::memcpy(lpReturnedString + posRet, &wNULL, 1);
                posRet += 1;
			}			
		}
		p1 = p2 + 1;
	}
	//last NULL 
	//PopMsg(_T("%d"), posRet);
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &wNULL, 1);
	posRet += 1;
	return posRet;
}

//User Query All Keys in a Given Section
DWORD CJXINIFile::GetKeysExW(
		 LPCWSTR lpAppName,        // section name
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    )
{
	int posRet = 0;
	int secLen = ::wcslen(lpAppName);
	LPWSTR pHead = (LPWSTR)m_pTrueData;
	
	LPWSTR pSecHead = NULL; //pointing to [
	//Section Map Must be Available
	ASSERT(!m_bSectionMapDirty);
#ifdef _UNICODE
	if(0 == m_mapSection.Lookup(lpAppName, (LPVOID&)pSecHead))
		return 0;
#else
	DWORD dwRetSize;
	PSTR pTemp = _W2A((LPWSTR)lpAppName, -1, dwRetSize);
    if(0 == m_mapSection.Lookup((LPCSTR)pTemp, (LPVOID&)pSecHead))
		return 0;
#endif
	LPWSTR pSecTail = NULL; //point to next [
	LPWSTR p1 = pSecHead;
	LPWSTR p2;
	        
	WORD wNULL = 0x0000; 
	while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1)
	{
		p1++;
		if(*p1 == WCHAR('['))
		{
			pSecTail = p1;
			break;
		}
	}
	if(pSecHead == NULL) return -1;
	if(pSecTail == NULL)
		pSecTail = pHead + m_dwUseSize/sizeof(WCHAR);
	pSecTail--;
    
	//Enum Keys in Section
    p1 = pSecHead;
	int keyLen;
	while(p1 < pSecTail - 1)
	{
		while(p1 < pSecTail - 1 &&
			*p1 != WCHAR('\r') && *p1 != WCHAR('\n'))
			p1++;
		while(p1 < pSecTail - 1 &&
			(*p1 == WCHAR('\r') || *p1 == WCHAR('\n')))
			p1++;
		p2 = p1;
		while(p2 < pSecTail - 1 &&
			*p2 != WCHAR('='))
			p2++;
		if(p1 < pSecTail - 1 && p2 < pSecTail - 1)
		{
			
			//Found A Key
			keyLen = p2 - p1;

//			CString str;
//			::lstrcpyn(str.GetBuffer(keyLen), p1, keyLen);
//			str.ReleaseBuffer(keyLen);
//			PopMsg(_T("Used %d KeyLen %d, %s"), posRet, keyLen, str);
            
			if(keyLen >= 1)
			{
				if(keyLen + 1 + posRet > (int)nSize) 
				{
					//PopMsg(_T("You Want Fetch Sections, Buffer %d, Keylen %d, Used %d, in %s"),
					//	nSize, keyLen, posRet, m_strFilename);
					return 0; //nSize - 1;
				}
				::wcsncpy(lpReturnedString + posRet, p1, keyLen);
                posRet += keyLen;
				//NULL = 0x00 00, 2 Byte
				::memcpy(lpReturnedString + posRet, &wNULL, 2);
                posRet += 1;
			}			
		}
		p1 = p2 + 1;
	}
	//last NULL 
	//PopMsg(_T("%d"), posRet);
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &wNULL, 2);
	posRet += 1;
	return posRet;
}

// return 0 -- no key, size too small to put value
//return -1  -- error
//return string length (of course > 1)
DWORD CJXINIFile::GetPrivateProfileStringW(
         LPCWSTR lpAppName,        // section name
         LPCWSTR lpKeyName,        // key name
         LPCWSTR lpDefault,        // default string
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              
		 // size of destination buffer,Specifies the size, in TCHARs, 
         LPCWSTR lpFileName        // initialization file name
    )
{
	BOOL bRet = LoadIniFile(lpFileName, 2);
	if(!bRet) return (UINT)-1;
	if(lpDefault == NULL) return -1;
	if(m_dwUseSize == 0) return -1;
	if(lpAppName == NULL)
		return GetSectionsW(lpReturnedString, nSize, lpFileName);
	if(lpKeyName == NULL)
		return GetKeysW(lpAppName, lpReturnedString, nSize, lpFileName);

	LPWSTR pHead = (LPWSTR)m_pTrueData;
	UINT uSec, uKey;
	uSec = (UINT) -1;
	uKey = (UINT)-1;
	uKey = GetKeyW(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1) //not key
	{
		if(::wcslen(lpDefault) > (int)nSize) return -1;
		::wcsncpy(lpReturnedString, lpDefault, ::wcslen(lpDefault));
		//append a null
        lpReturnedString[::wcslen(lpDefault)+1] = WCHAR('\0');
		return 0;
	}
	WCHAR buffer[MAX_PATH];

    LPWSTR pValue = pHead + uKey;
	while(pValue - pHead < (int)((m_dwUseSize)/sizeof(WCHAR)) && 
		 *pValue != WCHAR('\r') && *pValue != WCHAR('\n'))
	{
		pValue++;  //in unicode, +2 byte
	}

	int len = pValue - pHead - uKey;
	pValue = pHead + uKey;
	::wcsncpy(buffer, pValue, len); 
	if(nSize <= (UINT)len)
	{
		::wcsncpy(lpReturnedString, pValue, nSize);
        lpReturnedString[nSize - 1] = WCHAR('\0');
		return 0;
	}
	else
	{
		::wcsncpy(lpReturnedString, pValue, len);
		//append null
        lpReturnedString[len] = WCHAR('\0');
		return len;			
	}
	return (UINT) -1;
}

BOOL CJXINIFile::WritePrivateProfileStringExA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName,  // key name
         LPCSTR lpString,   // string to add
         LPCSTR lpFileName  // initialization file
    )
{
	BOOL bRet = this->LoadIniFile(lpFileName, 1);
	if(!bRet) return FALSE;
	m_bDirty = TRUE;

	if(lpKeyName == NULL)
		return DeleteSectionExA(lpAppName);
	else if(lpString == NULL)
		return this->DeleteKeyExA(lpAppName, lpKeyName);

	LPSTR pHead = (LPSTR)m_pTrueData;
	//2001/10/19 if lpString.len > MAX_PATH error 
	//WCHAR buffer[MAX_PATH];
	LPSTR buffer = NULL;

	UINT uSec, uKey;  
	//Warning: uKey, uSec is measured in WCHARs!!!!!
	//in UNICODE now
	if(!m_bSectionMapDirty)
		uKey = GetKeyExA(pHead, (UINT)m_dwUseSize, 
		   lpAppName, lpKeyName, uSec);
	else if(!m_bSectionMapLock)
	{
		UpdateMapA();
		uKey = GetKeyExA(pHead, (UINT)m_dwUseSize, 
		   lpAppName, lpKeyName, uSec);
	}
	else
        uKey = GetKeyA(pHead, (UINT)m_dwUseSize, 
		   lpAppName, lpKeyName, uSec);

    if(uKey == (UINT)-1 && uSec == (UINT)-1) //no section
	{
		int len = strlen(lpAppName) + strlen(lpKeyName) + strlen(lpString);
		len += 7;
		buffer = new char[len+1];
		::sprintf(buffer, "[%s]\r\n%s=%s\r\n", lpAppName, lpKeyName, lpString);
		buffer[len] = 0;
		LPSTR pStr = pHead;
		pStr += m_dwUseSize/sizeof(char);
		::memcpy(pStr, buffer, len*sizeof(char));
		m_dwUseSize += len*sizeof(char);

		delete []buffer;
		buffer = NULL;
        
		CString strSection;
#ifdef _UNICODE
		DWORD dwRetSize;
		LPWSTR pTemp = _A2W((LPSTR)lpAppName, -1, dwRetSize);
        ::wcsncpy(strSection.GetBuffer(dwRetSize), pTemp, dwRetSize);
		strSection.ReleaseBuffer(dwRetSize);
#else
		strSection = lpAppName;		
#endif

		if(!m_bSectionMapLock)
		{
			m_mapSection.SetAt(strSection, (LPVOID)pStr);
		}
		else
			m_bSectionMapDirty = TRUE;
		//No Change //m_bSectionMapDirty = ;
		bRet = TRUE;
	}
	else if(uKey == (UINT)-1 && uSec != (UINT)-1) 
	//section exist but value do not exist
	{
		// Following Write Just After [section],
		//that new item become the first of the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		/*
		LPWSTR pSection = pHead + uSec;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
	    	 *pSection != WCHAR('\r') && *pSection != WCHAR('\n'))
		{
			pSection++;
		}
    	while(*pSection == WCHAR('\r') || *pSection == WCHAR('\n'))
			pSection++;
		UINT uMoveHead = (UINT)(pSection - pHead); 
        //move memory
		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
		int len = ::wcslen(buffer);
		//also ok
		//::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(WCHAR));
		for(int i = (int)(m_dwUseSize - sizeof(WCHAR)); 
		    i >= (int)(uMoveHead*sizeof(WCHAR)); i -= sizeof(WCHAR))
		{
			::memcpy(pHead + i/sizeof(WCHAR) + len, 
				pHead + i/sizeof(WCHAR), sizeof(WCHAR));
		}
		*/
		//Following Write new item the last item in the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        LPSTR pSection = pHead + uSec + 1;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(char)) && 
	    	 *pSection != char('['))
		{
			pSection++;
		}
//		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
//		int len = ::wcslen(buffer);

		int len = strlen(lpKeyName) + strlen(lpString);
		len += 3;
		buffer = new char[len+1];
		::sprintf(buffer, "%s=%s\r\n", lpKeyName, lpString);
		buffer[len] = 0;
    	if(pSection - pHead == (int)(m_dwUseSize/sizeof(char)))
		{
			//no move needed, file end
			//no need to Update Section map
		}
		else
		{
			UINT uMoveHead = (UINT)(pSection - pHead); 
            //move memory
		    //also ok
		    //::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(WCHAR));
		    for(int i = (int)(m_dwUseSize - sizeof(char)); 
		       i >= (int)(uMoveHead*sizeof(char)); i -= sizeof(char))
			{
				   ::memcpy(pHead + i/sizeof(char) + len, 
				     pHead + i/sizeof(char), sizeof(char));
			}
			if(m_bSectionMapLock)
			{ 
				m_bSectionMapDirty = TRUE;
			}
			else //update section map
			{
				POSITION pos = m_mapSection.GetStartPosition();
	            while(pos)
				{
					CString strKey;
		            LPVOID lpPos;
		            m_mapSection.GetNextAssoc(pos, strKey, lpPos);
					//adjust Section Map
				    if((LPSTR)lpPos - pSection > 0)
					{
						LPWSTR pTemp = (LPWSTR)lpPos;
						pTemp += len;
						lpPos = (LPVOID)pTemp;
						m_mapSection.SetAt(strKey, lpPos);
					}
				}
			}
		}
		::memcpy(pSection, buffer, sizeof(char)*len);
		m_dwUseSize += len*sizeof(char);
		delete buffer;
		buffer = NULL;
		bRet = TRUE;
	}
	else //key exist
	{
		ASSERT(uKey != (UINT)-1);
        LPSTR pValue = pHead + uKey;
	    while(pValue - pHead < (int)(m_dwUseSize/sizeof(char)) && 
		    *pValue != char('\r') && *pValue != char('\n'))
		{
			pValue++;
		}

	    while(*pValue == char('\r') || *pValue == char('\n'))
			pValue++;
		UINT uMoveHead = (UINT)(pValue - pHead);
		
		UINT uPrevLen = uMoveHead - uKey;

		int len = strlen(lpString);
		len += 2;
		buffer = new char[len+1];
		::sprintf(buffer, "%s\r\n", lpString);
        buffer[len] = 0;
		UINT uCurLen = len;
        //move have 2 direction
		if(uCurLen > uPrevLen)
		{
			int charDiff = uCurLen - uPrevLen;
			int begin = (int)(m_dwUseSize - sizeof(char));
			int end = (int)uMoveHead*sizeof(char);
			for(int i = begin; i >= end; i -= sizeof(char))
			{
				::memcpy(pHead + i/sizeof(char) + charDiff,
					pHead + i/sizeof(char), sizeof(char));
			}
			if(m_bSectionMapLock)
				m_bSectionMapDirty = TRUE;
			else //update section map
			{
				POSITION pos = m_mapSection.GetStartPosition();
	            while(pos)
				{
					CString strKey;
		            LPVOID lpPos;
		            m_mapSection.GetNextAssoc(pos, strKey, lpPos);
					//adjust Section Map
				    if((LPSTR)lpPos - pHead > (int)uMoveHead)
					{
						LPSTR pTemp = (LPSTR)lpPos;
						pTemp += charDiff;
						lpPos = (LPVOID)pTemp;
						m_mapSection.SetAt(strKey, lpPos);
					}
				}
			}
		}
		else if(uCurLen < uPrevLen)
		{
			int charDiff = uPrevLen - uCurLen;
			int begin = (int)uMoveHead*sizeof(char);
			int end = (int)(m_dwUseSize - sizeof(char));
			for(int i = begin; i <= end;  i+=sizeof(char))
			{
				::memcpy(pHead + i/sizeof(char) - charDiff,
					pHead + i/sizeof(char), sizeof(char));
			}
			if(m_bSectionMapLock)
				m_bSectionMapDirty = TRUE;
			else //update section map
			{
				POSITION pos = m_mapSection.GetStartPosition();
	            while(pos)
				{
					CString strKey;
		            LPVOID lpPos;
		            m_mapSection.GetNextAssoc(pos, strKey, lpPos);
					//adjust Section Map
				    if((LPSTR)lpPos - pHead > (int)uMoveHead)
					{
						LPSTR pTemp = (LPSTR)lpPos;
						pTemp -= charDiff;
						lpPos = (LPVOID)pTemp;
						m_mapSection.SetAt(strKey, lpPos);
					}
				}
			}
		}
		pValue = pHead + uKey;
		::memcpy(pValue, buffer, sizeof(char)*uCurLen);
        m_dwUseSize += (uCurLen-uPrevLen)*sizeof(char);

		delete buffer;
		buffer = NULL;
        bRet = TRUE;
	}

	
	if(!bRet) 
	{
		UnloadIniFile();
		return FALSE;
	}
	//If the File is large enough, flush the file
	if((int)(m_dwUseSize - m_dwSize) > JXFlushIncrement - 5*JXPage)
	{
		//Flush Back to HardDisk
		UnloadIniFile();
	}
	return TRUE;
}

BOOL CJXINIFile::WritePrivateProfileStringExW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName,  // key name
         LPCWSTR lpString,   // string to add
         LPCWSTR lpFileName  // initialization file
    )
{
	BOOL bRet = this->LoadIniFile(lpFileName, 2);
	if(!bRet) return FALSE;
	m_bDirty = TRUE;

	if(lpKeyName == NULL)
		return DeleteSectionExW(lpAppName);
	else if(lpString == NULL)
		return this->DeleteKeyExW(lpAppName, lpKeyName);

	LPWSTR pHead = (LPWSTR)m_pTrueData;
	//2001/10/19 if lpString.len > MAX_PATH error 
	//WCHAR buffer[MAX_PATH];
	LPWSTR buffer = NULL;

	UINT uSec, uKey;  
	//Warning: uKey, uSec is measured in WCHARs!!!!!
	//in UNICODE now
	if(!m_bSectionMapDirty)
		uKey = GetKeyExW(pHead, (UINT)m_dwUseSize, 
		   lpAppName, lpKeyName, uSec);
	else if(!m_bSectionMapLock)
	{
		UpdateMapW();
		uKey = GetKeyExW(pHead, (UINT)m_dwUseSize, 
		   lpAppName, lpKeyName, uSec);
	}
	else
        uKey = GetKeyW(pHead, (UINT)m_dwUseSize, 
		   lpAppName, lpKeyName, uSec);

    if(uKey == (UINT)-1 && uSec == (UINT)-1) //no section
	{
		int len = wcslen(lpAppName) + wcslen(lpKeyName) +wcslen(lpString);
		len += 7;
		buffer = new WCHAR[len+1];
		::swprintf(buffer, L"[%s]\r\n%s=%s\r\n", lpAppName, lpKeyName, lpString);
		buffer[len] = 0;
		LPWSTR pStr = pHead;
		pStr += m_dwUseSize/sizeof(WCHAR);
		::memcpy(pStr, buffer, len*sizeof(WCHAR));
		m_dwUseSize += len*sizeof(WCHAR);

		delete []buffer;
		buffer = NULL;
        
		CString strSection;
#ifdef _UNICODE
		strSection = lpAppName;
#else
		DWORD dwRetSize;
		LPSTR pTemp = _W2A((LPWSTR)lpAppName, -1, dwRetSize);
        ::strncpy(strSection.GetBuffer(dwRetSize), pTemp, dwRetSize);
		strSection.ReleaseBuffer(dwRetSize);
#endif

		if(!m_bSectionMapLock)
		{
			m_mapSection.SetAt(strSection, (LPVOID)pStr);
		}
		else
			m_bSectionMapDirty = TRUE;
		//No Change //m_bSectionMapDirty = ;
		bRet = TRUE;
	}
	else if(uKey == (UINT)-1 && uSec != (UINT)-1) 
	//section exist but value do not exist
	{
		// Following Write Just After [section],
		//that new item become the first of the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		/*
		LPWSTR pSection = pHead + uSec;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
	    	 *pSection != WCHAR('\r') && *pSection != WCHAR('\n'))
		{
			pSection++;
		}
    	while(*pSection == WCHAR('\r') || *pSection == WCHAR('\n'))
			pSection++;
		UINT uMoveHead = (UINT)(pSection - pHead); 
        //move memory
		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
		int len = ::wcslen(buffer);
		//also ok
		//::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(WCHAR));
		for(int i = (int)(m_dwUseSize - sizeof(WCHAR)); 
		    i >= (int)(uMoveHead*sizeof(WCHAR)); i -= sizeof(WCHAR))
		{
			::memcpy(pHead + i/sizeof(WCHAR) + len, 
				pHead + i/sizeof(WCHAR), sizeof(WCHAR));
		}
		*/
		//Following Write new item the last item in the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        LPWSTR pSection = pHead + uSec + 1;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
	    	 *pSection != WCHAR('['))
		{
			pSection++;
		}
//		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
//		int len = ::wcslen(buffer);

		int len = wcslen(lpKeyName) + wcslen(lpString);
		len += 3;
		buffer = new WCHAR[len+1];
		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
		buffer[len] = 0;
    	if(pSection - pHead == (int)(m_dwUseSize/sizeof(WCHAR)))
		{
			//no move needed, file end
			//no need to Update Section map
		}
		else
		{
			UINT uMoveHead = (UINT)(pSection - pHead); 
            //move memory
		    //also ok
		    //::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(WCHAR));
		    for(int i = (int)(m_dwUseSize - sizeof(WCHAR)); 
		       i >= (int)(uMoveHead*sizeof(WCHAR)); i -= sizeof(WCHAR))
			{
				   ::memcpy(pHead + i/sizeof(WCHAR) + len, 
				     pHead + i/sizeof(WCHAR), sizeof(WCHAR));
			}
			if(m_bSectionMapLock)
			{ 
				m_bSectionMapDirty = TRUE;
			}
			else //update section map
			{
				POSITION pos = m_mapSection.GetStartPosition();
	            while(pos)
				{
					CString strKey;
		            LPVOID lpPos;
		            m_mapSection.GetNextAssoc(pos, strKey, lpPos);
					//adjust Section Map
				    if((LPWSTR)lpPos - pSection > 0)
					{
						LPWSTR pTemp = (LPWSTR)lpPos;
						pTemp += len;
						lpPos = (LPVOID)pTemp;
						m_mapSection.SetAt(strKey, lpPos);
					}
				}
			}
		}
		::memcpy(pSection, buffer, sizeof(WCHAR)*len);
		m_dwUseSize += len*sizeof(WCHAR);
		delete buffer;
		buffer = NULL;
		bRet = TRUE;
	}
	else //key exist
	{
		ASSERT(uKey != (UINT)-1);
        LPWSTR pValue = pHead + uKey;
	    while(pValue - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
		    *pValue != WCHAR('\r') && *pValue != WCHAR('\n'))
		{
			pValue++;
		}

	    while(*pValue == WCHAR('\r') || *pValue == WCHAR('\n'))
			pValue++;
		UINT uMoveHead = (UINT)(pValue - pHead);
		
		UINT uPrevLen = uMoveHead - uKey;

		int len = wcslen(lpString);
		len += 2;
		buffer = new WCHAR[len+1];
		::swprintf(buffer, L"%s\r\n", lpString);
        buffer[len] = 0;
		UINT uCurLen = len;
        //move have 2 direction
		if(uCurLen > uPrevLen)
		{
			int charDiff = uCurLen - uPrevLen;
			int begin = (int)(m_dwUseSize - sizeof(WCHAR));
			int end = (int)uMoveHead*sizeof(WCHAR);
			for(int i = begin; i >= end; i -= sizeof(WCHAR))
			{
				::memcpy(pHead + i/sizeof(WCHAR) + charDiff,
					pHead + i/sizeof(WCHAR), sizeof(WCHAR));
			}
			if(m_bSectionMapLock)
				m_bSectionMapDirty = TRUE;
			else //update section map
			{
				POSITION pos = m_mapSection.GetStartPosition();
	            while(pos)
				{
					CString strKey;
		            LPVOID lpPos;
		            m_mapSection.GetNextAssoc(pos, strKey, lpPos);
					//adjust Section Map
				    if((LPWSTR)lpPos - pHead > (int)uMoveHead)
					{
						LPWSTR pTemp = (LPWSTR)lpPos;
						pTemp += charDiff;
						lpPos = (LPVOID)pTemp;
						m_mapSection.SetAt(strKey, lpPos);
					}
				}
			}
		}
		else if(uCurLen < uPrevLen)
		{
			int charDiff = uPrevLen - uCurLen;
			int begin = (int)uMoveHead*sizeof(WCHAR);
			int end = (int)(m_dwUseSize - sizeof(WCHAR));
			for(int i = begin; i <= end;  i+=sizeof(WCHAR))
			{
				::memcpy(pHead + i/sizeof(WCHAR) - charDiff,
					pHead + i/sizeof(WCHAR), sizeof(WCHAR));
			}
			if(m_bSectionMapLock)
				m_bSectionMapDirty = TRUE;
			else //update section map
			{
				POSITION pos = m_mapSection.GetStartPosition();
	            while(pos)
				{
					CString strKey;
		            LPVOID lpPos;
		            m_mapSection.GetNextAssoc(pos, strKey, lpPos);
					//adjust Section Map
				    if((LPWSTR)lpPos - pHead > (int)uMoveHead)
					{
						LPWSTR pTemp = (LPWSTR)lpPos;
						pTemp -= charDiff;
						lpPos = (LPVOID)pTemp;
						m_mapSection.SetAt(strKey, lpPos);
					}
				}
			}
		}
		pValue = pHead + uKey;
		::memcpy(pValue, buffer, sizeof(WCHAR)*uCurLen);
        m_dwUseSize += (uCurLen-uPrevLen)*sizeof(WCHAR);

		delete buffer;
		buffer = NULL;
        bRet = TRUE;
	}

	
	if(!bRet) 
	{
		UnloadIniFile();
		return FALSE;
	}
	//If the File is large enough, flush the file
	if((int)(m_dwUseSize - m_dwSize) > JXFlushIncrement - 5*JXPage)
	{
		//Flush Back to HardDisk
		UnloadIniFile();
	}
	return TRUE;
}

BOOL CJXINIFile::WritePrivateProfileStringW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName,  // key name
         LPCWSTR lpString,   // string to add
         LPCWSTR lpFileName  // initialization file
    )
{
	BOOL bRet = this->LoadIniFile(lpFileName, 2);
	if(!bRet) return FALSE;
	m_bDirty = TRUE;

	if(lpKeyName == NULL)
		return DeleteSectionW(lpAppName);
	else if(lpString == NULL)
		return this->DeleteKeyW(lpAppName, lpKeyName);

	LPWSTR pHead = (LPWSTR)m_pTrueData;
	//2001/10/19 if lpString.len > MAX_PATH error 
	//WCHAR buffer[MAX_PATH];
	LPWSTR buffer = NULL;

	UINT uSec, uKey;  
	//Warning: uKey, uSec is measured in WCHARs!!!!!
	//in UNICODE now
	uKey = GetKeyW(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1 && uSec == (UINT)-1) //no section
	{
//		::swprintf(buffer, L"[%s]\r\n%s=%s\r\n", lpAppName, lpKeyName, lpString);
//		int len = ::wcslen(buffer);
//		LPWSTR pStr = pHead;
//		pStr += m_dwUseSize/sizeof(WCHAR);
//		::memcpy(pStr, buffer, len*sizeof(WCHAR));
//		m_dwUseSize += len*sizeof(WCHAR);

		int len = wcslen(lpAppName) + wcslen(lpKeyName) +wcslen(lpString);
		len += 7;
		buffer = new WCHAR[len+1];
		::swprintf(buffer, L"[%s]\r\n%s=%s\r\n", lpAppName, lpKeyName, lpString);
		buffer[len] = 0;
		LPWSTR pStr = pHead;
		pStr += m_dwUseSize/sizeof(WCHAR);
		::memcpy(pStr, buffer, len*sizeof(WCHAR));
		m_dwUseSize += len*sizeof(WCHAR);

		delete []buffer;
		buffer = NULL;
		bRet = TRUE;
	}
	else if(uKey == (UINT)-1 && uSec != (UINT)-1) 
	//section exist but value do not exist
	{
		// Following Write Just After [section],
		//that new item become the first of the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		/*
		LPWSTR pSection = pHead + uSec;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
	    	 *pSection != WCHAR('\r') && *pSection != WCHAR('\n'))
		{
			pSection++;
		}
    	while(*pSection == WCHAR('\r') || *pSection == WCHAR('\n'))
			pSection++;
		UINT uMoveHead = (UINT)(pSection - pHead); 
        //move memory
		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
		int len = ::wcslen(buffer);
		//also ok
		//::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(WCHAR));
		for(int i = (int)(m_dwUseSize - sizeof(WCHAR)); 
		    i >= (int)(uMoveHead*sizeof(WCHAR)); i -= sizeof(WCHAR))
		{
			::memcpy(pHead + i/sizeof(WCHAR) + len, 
				pHead + i/sizeof(WCHAR), sizeof(WCHAR));
		}
		*/
		//Following Write new item the last item in the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        LPWSTR pSection = pHead + uSec + 1;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
	    	 *pSection != WCHAR('['))
		{
			pSection++;
		}
//		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
//		int len = ::wcslen(buffer);

		int len = wcslen(lpKeyName) + wcslen(lpString);
		len += 3;
		buffer = new WCHAR[len+1];
		::swprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
		buffer[len] = 0;
    	if(pSection - pHead == (int)(m_dwUseSize/sizeof(WCHAR)))
		{
			//no move needed, file end
		}
		else
		{
			UINT uMoveHead = (UINT)(pSection - pHead); 
            //move memory
		    //also ok
		    //::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(WCHAR));
		    for(int i = (int)(m_dwUseSize - sizeof(WCHAR)); 
		       i >= (int)(uMoveHead*sizeof(WCHAR)); i -= sizeof(WCHAR))
			{
				   ::memcpy(pHead + i/sizeof(WCHAR) + len, 
				     pHead + i/sizeof(WCHAR), sizeof(WCHAR));
			}
		}
				
		::memcpy(pSection, buffer, sizeof(WCHAR)*len);
		
		m_dwUseSize += len*sizeof(WCHAR);
		delete buffer;
		buffer = NULL;
		bRet = TRUE;
	}
	else //key exist
	{
		ASSERT(uKey != (UINT)-1);
        LPWSTR pValue = pHead + uKey;
	    while(pValue - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
		    *pValue != WCHAR('\r') && *pValue != WCHAR('\n'))
		{
			pValue++;
		}

	    while(*pValue == WCHAR('\r') || *pValue == WCHAR('\n'))
			pValue++;
		UINT uMoveHead = (UINT)(pValue - pHead);
		
		UINT uPrevLen = uMoveHead - uKey;

		int len = wcslen(lpString);
		len += 2;
		buffer = new WCHAR[len+1];
		::swprintf(buffer, L"%s\r\n", lpString);
        buffer[len] = 0;
		UINT uCurLen = len;
        //move have 2 direction
		if(uCurLen > uPrevLen)
		{
			int charDiff = uCurLen - uPrevLen;
			int begin = (int)(m_dwUseSize - sizeof(WCHAR));
			int end = (int)uMoveHead*sizeof(WCHAR);
			for(int i = begin; i >= end; i -= sizeof(WCHAR))
			{
				::memcpy(pHead + i/sizeof(WCHAR) + charDiff,
					pHead + i/sizeof(WCHAR), sizeof(WCHAR));
			}
		}
		else if(uCurLen < uPrevLen)
		{
			int charDiff = uPrevLen - uCurLen;
			int begin = (int)uMoveHead*sizeof(WCHAR);
			int end = (int)(m_dwUseSize - sizeof(WCHAR));
			for(int i = begin; i <= end;  i+=sizeof(WCHAR))
			{
				::memcpy(pHead + i/sizeof(WCHAR) - charDiff,
					pHead + i/sizeof(WCHAR), sizeof(WCHAR));
			}
		}
		pValue = pHead + uKey;
		::memcpy(pValue, buffer, sizeof(WCHAR)*uCurLen);
        m_dwUseSize += (uCurLen-uPrevLen)*sizeof(WCHAR);

		delete buffer;
		buffer = NULL;
        bRet = TRUE;
	}

	m_bSectionMapDirty = TRUE;
	if(!bRet) 
	{
		UnloadIniFile();
		return FALSE;
	}
	//If the File is large enough, flush the file
	if((int)(m_dwUseSize - m_dwSize) > JXFlushIncrement - 5*JXPage)
	{
		//Flush Back to HardDisk
		UnloadIniFile();
	}
	return TRUE;
}

//User Query All Section Name in INI File
DWORD CJXINIFile::GetSectionsW(
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    )
{
    int posRet = 0;
	LPWSTR pHead = (LPWSTR)m_pTrueData;
	LPWSTR p1 = pHead;
	LPWSTR p2 = pHead;
        
	WORD wNULL = 0x0000; 
	while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1)
	{
		while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p1 != WCHAR('['))
			p1++;
		p2 = p1;
		while(p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p2 != WCHAR(']'))
			p2++;
		if(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) &&
			p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)))
		{
			//Found, Ready To Copy To lpReturnedString
			int lenSection = p2 - p1 - 1;
			
			if(lenSection >= 1)
			{
				p1++;
				if(lenSection + 1 + posRet > (int)nSize) return 0; //nSize - 1;
				::wcsncpy(lpReturnedString + posRet, p1, lenSection);
                posRet += lenSection;
				//NULL = 0x00 00, 2 Byte
				::memcpy(lpReturnedString + posRet, &wNULL, 2);
                posRet += 1;
			} 
		}
		p1 = p2 + 1;
	}
	//last NULL 
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &wNULL, 2);
	posRet += 1;
	return posRet;
}

//User Query All Keys in a Given Section
DWORD CJXINIFile::GetKeysW(
		 LPCWSTR lpAppName,        // section name
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    )
{
	int posRet = 0;
	int secLen = ::wcslen(lpAppName);
	LPWSTR pHead = (LPWSTR)m_pTrueData;
	LPWSTR p1 = pHead;
	LPWSTR p2 = pHead;
	LPWSTR pSecHead = NULL; //pointing to [
	LPWSTR pSecTail = NULL; //point to next [
        
	WORD wNULL = 0x0000; 
	while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1)
	{
		while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p1 != WCHAR('['))
			p1++;
		p2 = p1;
		while(p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p2 != WCHAR(']'))
			p2++;
		//p2 points to ]
		if(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) &&
			p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)))
		{
			//Found A Section
			if(pSecHead != NULL)
			{
				pSecTail = p1;
				break;
			}
			else //pSecHead == NULL
			{
				if(secLen == p2 - p1 - 1) //same length
				{
					p1++;
					LPWSTR p3 = p1; //p3 points to ["X"
				    for(int i = 0; i < secLen; i++)
					{
						if(*p1 != (WCHAR)(lpAppName[i])) break;	
						p1++;
					}
				    if(i == secLen) //Found;
						pSecHead = p3 - 1;
				}
			}
		}
		p1 = p2 + 1;
	}
	if(pSecHead == NULL) return -1;
	if(pSecTail == NULL)
		pSecTail = pHead + m_dwUseSize/sizeof(WCHAR);
	pSecTail--;
    
	//Enum Keys in Section
    p1 = pSecHead;
	int keyLen;
	while(p1 < pSecTail - 1)
	{
		while(p1 < pSecTail - 1 &&
			*p1 != WCHAR('\r') && *p1 != WCHAR('\n'))
			p1++;
		while(p1 < pSecTail - 1 &&
			(*p1 == WCHAR('\r') || *p1 == WCHAR('\n')))
			p1++;
		p2 = p1;
		while(p2 < pSecTail - 1 &&
			*p2 != WCHAR('='))
			p2++;
		if(p1 < pSecTail - 1 && p2 < pSecTail - 1)
		{
			//Found A Key
			keyLen = p2 - p1;
			if(keyLen >= 1)
			{
				if(keyLen + 1 + posRet > (int)nSize) return 0; //nSize - 1;
				::wcsncpy(lpReturnedString + posRet, p1, keyLen);
                posRet += keyLen;
				//NULL = 0x00 00, 2 Byte
				::memcpy(lpReturnedString + posRet, &wNULL, 2);
                posRet += 1;
			}			
		}
		p1 = p2 + 1;
	}
	//last NULL 
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &wNULL, 2);
	posRet += 1;
	return posRet;
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//ANSI Version ----------------------------------------------
// return 0 -- no key, size too small to put value
//return -1  -- error
//return string length (of course > 1)
DWORD CJXINIFile::GetPrivateProfileStringA(
         LPCSTR lpAppName,        // section name
         LPCSTR lpKeyName,        // key name
         LPCSTR lpDefault,        // default string
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              
		 // size of destination buffer,Specifies the size, in TCHARs, 
         LPCSTR lpFileName        // initialization file name
    )
{
	BOOL bRet = LoadIniFile(lpFileName, 1);
	if(!bRet) return (UINT)-1;
	if(lpDefault == NULL) return -1;
	if(m_dwUseSize == 0) return -1;
	if(lpAppName == NULL)
		return GetSectionsA(lpReturnedString, nSize, lpFileName);
	if(lpKeyName == NULL)
		return GetKeysA(lpAppName, lpReturnedString, nSize, lpFileName);

	LPSTR pHead = (LPSTR)m_pTrueData;
	UINT uSec, uKey;
	uSec = (UINT) -1;
	uKey = (UINT)-1;
	uKey = GetKeyA(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1) //not key
	{
		if(::strlen(lpDefault) > (int)nSize) return -1;
		::strncpy(lpReturnedString, lpDefault, ::strlen(lpDefault));
		//append a null
        lpReturnedString[::strlen(lpDefault)+1] = char('\0');
		return 0;
	}
	char buffer[MAX_PATH];

    LPSTR pValue = pHead + uKey;
	while(pValue - pHead < (int)((m_dwUseSize)/sizeof(char)) && 
		 *pValue != char('\r') && *pValue != char('\n'))
	{
		pValue++;  //in unicode, +2 byte
	}

	int len = pValue - pHead - uKey;
	pValue = pHead + uKey;
	::strncpy(buffer, pValue, len); 
	if(nSize <= (UINT)len)
	{
		::strncpy(lpReturnedString, pValue, nSize);
        lpReturnedString[nSize - 1] = char('\0');
		return 0;
	}
	else
	{
		::strncpy(lpReturnedString, pValue, len);
		//append null
        lpReturnedString[len] = char('\0');
		return len;			
	}
	return (UINT) -1;
}

BOOL CJXINIFile::WritePrivateProfileStringA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName,  // key name
         LPCSTR lpString,   // string to add
         LPCSTR lpFileName  // initialization file
    )
{
	BOOL bRet = this->LoadIniFile(lpFileName, 1);
	if(!bRet) return FALSE;
	m_bDirty = TRUE;

	LPSTR pHead = (LPSTR)m_pTrueData;
	//2001/10/19 if lpString.len > MAX_PATH error 
	//char buffer[MAX_PATH];
	LPSTR buffer = NULL;

	UINT uSec, uKey;  
	//Warning: uKey, uSec is measured in WCHARs!!!!!
	//in UNICODE now
	uKey = GetKeyA(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1 && uSec == (UINT)-1) //no section
	{
		int len = strlen(lpAppName) + strlen(lpKeyName) +strlen(lpString);
		len += 7;
		buffer = new char[len+1];
		::sprintf(buffer, "[%s]\r\n%s=%s\r\n", lpAppName, lpKeyName, lpString);
		buffer[len] = 0;
		LPSTR pStr = pHead;
		pStr += m_dwUseSize/sizeof(char);
		::memcpy(pStr, buffer, len*sizeof(char));
		m_dwUseSize += len*sizeof(char);

		delete []buffer;
		buffer = NULL;
		bRet = TRUE;
	}
	else if(uKey == (UINT)-1 && uSec != (UINT)-1) 
	//section exist but value do not exist
	{
		// Following Write Just After [section],
		//that new item become the first of the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		/*
		LPSTR pSection = pHead + uSec;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(char)) && 
	    	 *pSection != char('\r') && *pSection != char('\n'))
		{
			pSection++;
		}
    	while(*pSection == char('\r') || *pSection == char('\n'))
			pSection++;
		UINT uMoveHead = (UINT)(pSection - pHead); 
        //move memory
		::sprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
		int len = ::strlen(buffer);
		//also ok
		//::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(char));
		for(int i = (int)(m_dwUseSize - sizeof(char)); 
		    i >= (int)(uMoveHead*sizeof(char)); i -= sizeof(char))
		{
			::memcpy(pHead + i/sizeof(char) + len, 
				pHead + i/sizeof(char), sizeof(char));
		}
		*/
		//Following Write new item the last item in the section
        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        LPSTR pSection = pHead + uSec + 1;
		while(pSection - pHead < (int)(m_dwUseSize/sizeof(char)) && 
	    	 *pSection != char('['))
		{
			pSection++;
		}
//		::sprintf(buffer, L"%s=%s\r\n", lpKeyName, lpString);
//		int len = ::strlen(buffer);

		int len = strlen(lpKeyName) + strlen(lpString);
		len += 3;
		buffer = new char[len+1];
		::sprintf(buffer, "%s=%s\r\n", lpKeyName, lpString);
		buffer[len] = 0;
    	if(pSection - pHead == (int)(m_dwUseSize/sizeof(char)))
		{
			//no move needed, file end
		}
		else
		{
			UINT uMoveHead = (UINT)(pSection - pHead); 
            //move memory
		    //also ok
		    //::memmove(pHead + uMoveHead + len, pHead + uMoveHead, m_dwUseSize-uMoveHead*sizeof(char));
		    for(int i = (int)(m_dwUseSize - sizeof(char)); 
		       i >= (int)(uMoveHead*sizeof(char)); i -= sizeof(char))
			{
				   ::memcpy(pHead + i/sizeof(char) + len, 
				     pHead + i/sizeof(char), sizeof(char));
			}
		}
				
		::memcpy(pSection, buffer, sizeof(char)*len);
		
		m_dwUseSize += len*sizeof(char);
		delete buffer;
		buffer = NULL;
		bRet = TRUE;
	}
	else //key exist
	{
		ASSERT(uKey != (UINT)-1);
        LPSTR pValue = pHead + uKey;
	    while(pValue - pHead < (int)(m_dwUseSize/sizeof(char)) && 
		    *pValue != char('\r') && *pValue != char('\n'))
		{
			pValue++;
		}

	    while(*pValue == char('\r') || *pValue == char('\n'))
			pValue++;
		UINT uMoveHead = (UINT)(pValue - pHead);
		
		UINT uPrevLen = uMoveHead - uKey;

		int len = strlen(lpString);
		len += 2;
		buffer = new char[len+1];
		::sprintf(buffer, "%s\r\n", lpString);
        buffer[len] = 0;
		UINT uCurLen = len;
        //move have 2 direction
		if(uCurLen > uPrevLen)
		{
			int charDiff = uCurLen - uPrevLen;
			int begin = (int)(m_dwUseSize - sizeof(char));
			int end = (int)uMoveHead*sizeof(char);
			for(int i = begin; i >= end; i -= sizeof(char))
			{
				::memcpy(pHead + i/sizeof(char) + charDiff,
					pHead + i/sizeof(char), sizeof(char));
			}
		}
		else if(uCurLen < uPrevLen)
		{
			int charDiff = uPrevLen - uCurLen;
			int begin = (int)uMoveHead*sizeof(char);
			int end = (int)(m_dwUseSize - sizeof(char));
			for(int i = begin; i <= end;  i+=sizeof(char))
			{
				::memcpy(pHead + i/sizeof(char) - charDiff,
					pHead + i/sizeof(char), sizeof(char));
			}
		}
		pValue = pHead + uKey;
		::memcpy(pValue, buffer, sizeof(char)*uCurLen);
        m_dwUseSize += (uCurLen-uPrevLen)*sizeof(char);

		delete buffer;
		buffer = NULL;
        bRet = TRUE;
	}

	m_bSectionMapDirty = TRUE;
	if(!bRet) 
	{
		UnloadIniFile();
		return FALSE;
	}
	//If the File is large enough, flush the file
	if((int)(m_dwUseSize - m_dwSize) > JXFlushIncrement - 5*JXPage)
	{
		//Flush Back to HardDisk
		UnloadIniFile();
	}
	return TRUE;
}

//User Query All Section Name in INI File
DWORD CJXINIFile::GetSectionsA(
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    )
{
    int posRet = 0;
	LPSTR pHead = (LPSTR)m_pData;
	LPSTR p1 = pHead;
	LPSTR p2 = pHead;
        
	BYTE wNULL = 0x0000; 
	while(p1- pHead < (int)(m_dwUseSize/sizeof(char)) - 1)
	{
		while(p1- pHead < (int)(m_dwUseSize/sizeof(char)) - 1 &&
			*p1 != char('['))
			p1++;
		p2 = p1;
		while(p2- pHead < (int)(m_dwUseSize/sizeof(char)) - 1 &&
			*p2 != char(']'))
			p2++;
		if(p1- pHead < (int)(m_dwUseSize/sizeof(char)) &&
			p2- pHead < (int)(m_dwUseSize/sizeof(char)))
		{
			//Found, Ready To Copy To lpReturnedString
			int lenSection = p2 - p1 - 1;
			
			if(lenSection >= 1)
			{
				p1++;
				if(lenSection + 1 + posRet > (int)nSize) return 0; //nSize - 1;
				::strncpy(lpReturnedString + posRet, p1, lenSection);
                posRet += lenSection;
				//NULL = 0x00,  1Byte
				::memcpy(lpReturnedString + posRet, &wNULL, 1);
                posRet += 1;
			} 
		}
		p1 = p2 + 1;
	}
	//last NULL 
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &wNULL, 1);
	posRet += 1;
	return posRet;
}


//User Query All Keys in a Given Section
DWORD CJXINIFile::GetKeysA(
         LPCSTR lpAppName,        // section name
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    )
{
	int posRet = 0;
	int secLen = ::strlen(lpAppName);
	LPSTR pHead = (LPSTR)m_pTrueData;
	LPSTR p1 = pHead;
	LPSTR p2 = pHead;
	LPSTR pSecHead = NULL; //pointing to [
	LPSTR pSecTail = NULL; //point to next [
        
	BYTE byteNULL = 0x00; 
	while(p1- pHead < (int)(m_dwUseSize/sizeof(char)) - 1)
	{
		while(p1- pHead < (int)(m_dwUseSize/sizeof(char)) - 1 &&
			*p1 != char('['))
			p1++;
		p2 = p1;
		while(p2- pHead < (int)(m_dwUseSize/sizeof(char)) - 1 &&
			*p2 != char(']'))
			p2++;
		//p2 points to ]
		if(p1- pHead < (int)(m_dwUseSize/sizeof(char)) &&
			p2- pHead < (int)(m_dwUseSize/sizeof(char)))
		{
			//Found A Section
			if(pSecHead != NULL)
			{
				pSecTail = p1;
				break;
			}
			else //pSecHead == NULL
			{
				if(secLen == p2 - p1 - 1) //same length
				{
					p1++;
					LPSTR p3 = p1; //p3 points to ["X"
				    for(int i = 0; i < secLen; i++)
					{
						if(*p1 != (char)(lpAppName[i])) break;	
						p1++;
					}
				    if(i == secLen) //Found;
						pSecHead = p3 - 1;
				}
			}
		}
		p1 = p2 + 1;
	}
	if(pSecHead == NULL) return -1;
	if(pSecTail == NULL)
		pSecTail = pHead + m_dwUseSize/sizeof(char);
	pSecTail--;
    
	//Enum Keys in Section
    p1 = pSecHead;
	int keyLen;
	while(p1 < pSecTail - 1)
	{
		while(p1 < pSecTail - 1 &&
			*p1 != char('\r') && *p1 != char('\n'))
			p1++;
		while(p1 < pSecTail - 1 &&
			(*p1 == char('\r') || *p1 == char('\n')))
			p1++;
		p2 = p1;
		while(p2 < pSecTail - 1 &&
			*p2 != char('='))
			p2++;
		if(p1 < pSecTail - 1 && p2 < pSecTail - 1)
		{
			//Found A Key
			keyLen = p2 - p1;
			if(keyLen >= 1)
			{
				if(keyLen + 1 + posRet > (int)nSize) return 0; //nSize - 1;
				::strncpy(lpReturnedString + posRet, p1, keyLen);
                posRet += keyLen;
				//NULL = 0x00 , 1 Byte
				::memcpy(lpReturnedString + posRet, &byteNULL, 1);
                posRet += 1;
			}			
		}
		p1 = p2 + 1;
	}
	//last NULL 
    if(1 + posRet > (int)nSize) return 0; //nSize - 1; 
    ::memcpy(lpReturnedString + posRet, &byteNULL, 1);
	posRet += 1;
	return posRet;
}

BOOL CJXINIFile::LoadIniFile(CString strFilename, DWORD dwMode)
{
	if(dwMode == 0 && sizeof(TCHAR) == 1) m_bUnicode = FALSE;
	if(dwMode == 0 && sizeof(TCHAR) == 2) m_bUnicode = TRUE;
    if(dwMode == 1) m_bUnicode = FALSE;
	if(dwMode == 2) m_bUnicode = TRUE;

	if(strFilename == _T("")) return FALSE;
	//Already Loaded
	if(m_strFilename == strFilename) return TRUE; 

	if(m_strFilename != _T("")) //Unload First
	{
		UnloadIniFile();
	}
    
	m_strFilename = strFilename;
	m_bDirty = FALSE;
	m_bSectionMapDirty = TRUE;
    m_bSectionMapLock = FALSE;

	DWORD dwHigh, dwLow;
	
	DWORD dwTotal, dwError;
	//Open File Handle
	m_hFile = ::CreateFile(m_strFilename, GENERIC_READ | GENERIC_WRITE,
		 FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		 FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		//Measure File Size
	    dwLow = GetFileSize(m_hFile, &dwHigh);
	    dwTotal = 0;
  	    if(dwLow == -1 && (dwError = GetLastError()) != NO_ERROR)
		{
			::MessageBox(NULL, _T("File Size Failure"),
				_T("CJXINIFile::Load"), MB_OK | MB_ICONSTOP);
		    if(m_hFile != INVALID_HANDLE_VALUE)
				::CloseHandle(m_hFile);
		    m_dwSize = 0;
		    m_strFilename = _T("");
	        return FALSE;
		}
		m_dwSize = dwLow;  //32bit -> 4Gb
	}
	else
	{
		m_hFile = ::CreateFile(m_strFilename, GENERIC_READ | GENERIC_WRITE,
		   FILE_SHARE_READ, NULL, OPEN_ALWAYS, 
		   FILE_ATTRIBUTE_NORMAL, NULL);
	    if(m_hFile == INVALID_HANDLE_VALUE)
		{
			::MessageBox(NULL, _T("CreateFile Failure"),
              _T("CJXINIFile::Load"), MB_OK | MB_ICONSTOP);
		    if(m_hFile != INVALID_HANDLE_VALUE)
				::CloseHandle(m_hFile);
		    m_dwSize = 0;
		    m_strFilename = _T("");
	        return FALSE;
		}
		m_dwSize = 0;
	}
        
	//Open Memory-Mapping File, maximum size = File Size + JXFlushIncrement
	dwLow = m_dwSize + JXFlushIncrement;
	m_hMMF = ::CreateFileMapping(m_hFile, NULL,
		PAGE_READWRITE,  0 , dwLow, NULL);
	if(NULL == m_hMMF)
	{
		::MessageBox(NULL, _T("CreateFileMapping Failure"),
                _T("CJXINIFile::Load"), MB_OK | MB_ICONSTOP);
		if(m_hMMF != NULL)
			::CloseHandle(m_hMMF);
	    if(m_hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(m_hFile);
		m_dwSize = 0;
		m_strFilename = _T("");
		return FALSE;
	}

	//Map pointer
	m_pData = ::MapViewOfFile(m_hMMF, FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, dwLow);
    if(m_pData == NULL)
	{
		//ReportErr(_T(""));
		if(m_hMMF != NULL)
			::CloseHandle(m_hMMF);
	    if(m_hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(m_hFile);
		m_dwSize = 0;
		m_strFilename = _T("");
		::MessageBox(NULL, _T("MapViewOfFile Failure"),
               _T("CJXINIFile::Load"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	m_dwUseSize = m_dwSize;
    
	CutZeroTail();

	//if(sizeof(TCHAR) == 2) //UNICODE only
	if(this->m_bUnicode)
	//Pass Potentail 0xFF FE header if edited by MS NotePad
	{
		LPBYTE pByte = (LPBYTE)m_pData;
	    if(m_dwUseSize >= 2 && *pByte == 0xFF && *(pByte +1) == 0xFE)
		{
			m_pTrueData = (LPVOID)(pByte + 2);
			m_dwUseSize -= 2;
		}
	    else
		{
			if(m_dwUseSize == 0) //a new file, add 0xFF FE header
				                 //for Win2k SP1 need 0xFF FE
			{
				*pByte = 0xFF;
				*(pByte +1) = 0xFE;
				//m_dwUseSize = 2;
				m_pTrueData = (LPVOID)(pByte+2);
			}
			else //old format without 0xFF FE
			{
				::memmove((LPVOID)(pByte+2), (LPVOID)pByte, m_dwUseSize);
				*pByte = 0xFF;
				*(pByte +1) = 0xFE;
				//m_dwUseSize += 2;
                m_pTrueData = (LPVOID)(pByte+2);
			}
		}
	}
	else
		m_pTrueData = m_pData;
    return TRUE;
}

BOOL CJXINIFile::UnloadIniFile()
{
	DWORD dwFileSize;
	//if(sizeof(TCHAR) == 2) //UNICODE leading 0xFF FE
	if(this->m_bUnicode)
		dwFileSize = m_dwUseSize + 2;
	else
		dwFileSize = m_dwUseSize;
	if(m_strFilename == _T("")) return TRUE; 
    if(!m_bDirty)
	{
		if(m_pData)
		{
			::UnmapViewOfFile(m_pData);
			m_pData = NULL;
		}
	    if(m_hMMF != NULL)
		{
			::CloseHandle(m_hMMF);
			m_hMMF = NULL;
		}
		LONG highDist = 0;
	    ::SetFilePointer(
			m_hFile,                // handle to file
            dwFileSize,        // bytes to move pointer
            &highDist,  // bytes to move pointer
            FILE_BEGIN           // starting point
        );
	    ::SetEndOfFile(m_hFile);
	    if(m_hFile != INVALID_HANDLE_VALUE)
		{
		    ::CloseHandle(m_hFile);
		    m_hFile = INVALID_HANDLE_VALUE;
		}
		m_strFilename = _T("");
		return TRUE;
	}
	//Modified
	ASSERT(m_pData);
	if(!::FlushViewOfFile(m_pData, m_dwUseSize))
	{
		::MessageBox(NULL, _T("FlushViewMMF"), _T(""), MB_OK);
		::UnmapViewOfFile(m_pData);
	    ::CloseHandle(m_hMMF);
		LONG highDist = 0;
	    ::SetFilePointer(
		  m_hFile,                // handle to file
          dwFileSize,        // bytes to move pointer
          &highDist,  // bytes to move pointer
          FILE_BEGIN           // starting point
        );
		::SetEndOfFile(m_hFile);
	    ::CloseHandle(m_hFile);
		m_strFilename = _T("");
		m_pData = NULL;
		m_hMMF = NULL;
		m_hFile = INVALID_HANDLE_VALUE;
		m_dwSize = 0;
		return FALSE;
	}
	::UnmapViewOfFile(m_pData);
	::CloseHandle(m_hMMF);
	//Adjust File Size
	LONG highDist = 0;
	::SetFilePointer(
		m_hFile,                // handle to file
        dwFileSize,        // bytes to move pointer
        &highDist,  // bytes to move pointer
        FILE_BEGIN           // starting point
    );
	::SetEndOfFile(m_hFile);
	::CloseHandle(m_hFile);
	m_strFilename = _T("");
	m_pData = NULL;
	m_hMMF = NULL;
	m_hFile = INVALID_HANDLE_VALUE;
	m_dwSize = 0;
	//
	m_mapSection.RemoveAll();
	m_bSectionMapDirty = TRUE;
	
	m_bSectionMapLock = FALSE;
	return TRUE;
}

//return -1 if not find key
//return first character of the key line 
//[section name]\r\n
//keyname = valuename\r\n
//Ex Version use Map
//posSection & ret all in WCHARs
UINT CJXINIFile::GetKeyExA(LPSTR pHead, UINT uSize, 
						LPCSTR szSection, LPCSTR szKey, UINT& posSection)
{
	//Map Must Be Ready Here
	ASSERT(!m_bSectionMapDirty);
	LPSTR pSecHead = NULL; //pointing to [

#ifdef _UNICODE
	DWORD dwRetSize;
	LPWSTR pTemp = _A2W((LPSTR)szSection, -1, dwRetSize);
    if(0 == m_mapSection.Lookup(pTemp, (LPVOID&)pSecHead))
	{
        posSection = (UINT)-1;
		return (UINT)-1; 
	}
#else
	if(0 == m_mapSection.Lookup(szSection, (LPVOID&)pSecHead))
	{
        posSection = (UINT)-1;
		return (UINT)-1; 
	}
#endif

	posSection = -1;
	LPSTR p1 = pSecHead;
	LPSTR p2 = pSecHead;
	int lenSection = ::strlen(szSection);
	char szSection2[MAX_PATH];
	//section --> [section]
    ::strncpy(szSection2+1, szSection, lenSection);
	szSection2[0] = char('[');
	szSection2[lenSection+1] = char(']');
	szSection2[lenSection+2] = char('\0');
        
	BOOL bSection = FALSE; //found the section
	BOOL bKey = FALSE;     //found the key
	char buffer[MAX_PATH]; 
	int line;
	
	while(p1- pHead < (int)(uSize/sizeof(char)) - 1)
	{
		//WCHAR s1 = (WCHAR)'\r'; //13
		//WCHAR s2 = (WCHAR)'\n'; //10
		//WCHAR s3 = (WCHAR)0x0A; //10
		//01/7/5 //avoid leak blank line
		int rInc, nInc;
		rInc = nInc = 0;
		while(p1- pHead < (int)(uSize/sizeof(char)-1) && 
			(*p1 == (char)'\r' || *p1 == (char)'\n'))
		{
			if(*p1 == (char)'\r') rInc++;
            if(*p1 == (char)'\n') nInc++;
			p1++;		
		}
				
		if(rInc == 1 && nInc == 1)
		{
		}
		else if(rInc == nInc && rInc > 1)
			line += rInc - 1;
		else if(rInc != nInc)
		{
			line += rInc > nInc ? rInc - 1 : nInc - 1;
		}
			
		p2 = p1;
		while(p2- pHead < (int)(uSize/sizeof(char))-1)
		{
			//$$$ some file end  with \n only
			if(*p2 == (char)'\r' && *(p2+1) == (char)'\n') 
			{
				break; //touch it
			}
            if(*p2 == (char)'\n') break;
			if(*p2 == (char)'\r') break;
		    p2++;
		}
		if(p2 - pHead == (int)(uSize/sizeof(char))-1)
		{
			//the last sentence 
            while(*p2 == (char)'\r' || *p2 == (char)'\n') p2--;
			int len = p2 - p1 + 1; 
			line++;
			if(len > 1)
			{
				if(len > MAX_PATH) 
				{
					p1 = p2 + 1;
					continue;
				}
				::strncpy(buffer, p1, len);
				buffer[len] = char('\0');

				//a section ?
			    if(buffer[0] == char('[') && 
				    buffer[len-1] == char(']'))
				{
					//AfxMessageBox(buffer);
					if(::strcmp(buffer,szSection2) == 0) //match
					{
						if(!bSection) 
						{
							bSection = TRUE;
						    posSection = (UINT)(p1 - pHead);
						}
					}
				    else
					{
						if(bSection) return (UINT)-1;
					}
				}
			    else //a key line
				{
					if(!bSection)
					{
					}
				    else
					{
						//key name = value name
						int lenKey = ::strlen(szKey);
					    //find =
					    int sam;
					    for(sam = 0; sam < len; sam++)
							if(buffer[sam] == char('=')) break;
					    if(sam >= lenKey && sam < len)
						{
							for(int q = 0; q < sam; q++)
							{
								if(q < lenKey && buffer[q] != szKey[q]) break;
							    if(q > lenKey && buffer[q] != char(' ')) break;
							}
						    if(q == sam) //found
							{
								bKey = TRUE;
							    return (UINT)(p1 - pHead + sam + 1);
							    //return (UINT)(p1 - pHead);
							}
						}					
					}
				}

				line++;
			}
            break;
		}
		p2--;
		//now p2 points to the ending of the sentence
        int len = p2 - p1 + 1; //in chars
		line++;
		if(len > 1)
		{
			if(len > MAX_PATH) 
			{
				p1 = p2 + 1;
				continue;
			}
			//this sentence must shorter than MAX_PATH
			::strncpy(buffer, p1, len);
			buffer[len] = char('\0');
			
			//???, lstrcpy seems not add null-trminator
			//buffer[len] = WCHAR('\0');
			
			//a section ?
			if(buffer[0] == char('[') && 
				buffer[len-1] == char(']'))
			{
				//AfxMessageBox(buffer);
				if(::strcmp(buffer,szSection2) == 0) //match
				{
					if(!bSection) 
					{
						bSection = TRUE;
						posSection = (UINT)(p1 - pHead);
					}
				}
				else
				{
					if(bSection) return (UINT)-1;
				}
			}
			else //a key line
			{
				if(!bSection)
				{
				}
				else
				{
					//key name = value name
					int lenKey = ::strlen(szKey);
					//find =
					int sam;
					for(sam = 0; sam < len; sam++)
						if(buffer[sam] == char('=')) break;
					if(sam >= lenKey && sam < len)
					{
						for(int q = 0; q < sam; q++)
						{
							if(q < lenKey && buffer[q] != szKey[q]) break;
							if(q > lenKey && buffer[q] != char(' ')) break;
						}
						if(q == sam) //found
						{
							bKey = TRUE;
							return (UINT)(p1 - pHead + sam + 1);
							//return (UINT)(p1 - pHead);
						}
					}					
				}
			}
		}
		p1 = p2 + 1;
	}
	return (UINT)-1;	
}

//return -1 if not find key
//return first character of the key line 
//[section name]\r\n
//keyname = valuename\r\n
//Ex Version use Map
//posSection & ret all in WCHARs
UINT CJXINIFile::GetKeyExW(LPWSTR pHead, UINT uSize, 
						LPCWSTR szSection, LPCWSTR szKey, UINT& posSection)
{
	//Map Must Be Ready Here
	ASSERT(!m_bSectionMapDirty);
	LPWSTR pSecHead = NULL; //pointing to [
#ifdef _UNICODE
	if(0 == m_mapSection.Lookup(szSection, (LPVOID&)pSecHead))
	{
        posSection = (UINT)-1;
		return (UINT)-1; 
	}
#else
	DWORD dwRetSize;
	LPSTR pTemp = _W2A((LPWSTR)szSection, -1, dwRetSize);
    if(0 == m_mapSection.Lookup(pTemp, (LPVOID&)pSecHead))
	{
        posSection = (UINT)-1;
		return (UINT)-1; 
	}
#endif
	posSection = -1;
	LPWSTR p1 = pSecHead;
	LPWSTR p2 = pSecHead;
	int lenSection = ::wcslen(szSection);
	WCHAR szSection2[MAX_PATH];
	//section --> [section]
    ::wcsncpy(szSection2+1, szSection, lenSection);
	szSection2[0] = WCHAR('[');
	szSection2[lenSection+1] = WCHAR(']');
	szSection2[lenSection+2] = WCHAR('\0');
        
	BOOL bSection = FALSE; //found the section
	BOOL bKey = FALSE;     //found the key
	WCHAR buffer[MAX_PATH]; 
	int line;
	
	while(p1- pHead < (int)(uSize/sizeof(WCHAR)) - 1)
	{
		//WCHAR s1 = (WCHAR)'\r'; //13
		//WCHAR s2 = (WCHAR)'\n'; //10
		//WCHAR s3 = (WCHAR)0x0A; //10
		//01/7/5 //avoid leak blank line
		int rInc, nInc;
		rInc = nInc = 0;
		while(p1- pHead < (int)(uSize/sizeof(WCHAR)-1) && 
			(*p1 == (WCHAR)'\r' || *p1 == (WCHAR)'\n'))
		{
			if(*p1 == (WCHAR)'\r') rInc++;
            if(*p1 == (WCHAR)'\n') nInc++;
			p1++;		
		}
				
		if(rInc == 1 && nInc == 1)
		{
		}
		else if(rInc == nInc && rInc > 1)
			line += rInc - 1;
		else if(rInc != nInc)
		{
			line += rInc > nInc ? rInc - 1 : nInc - 1;
		}
			
		p2 = p1;
		while(p2- pHead < (int)(uSize/sizeof(WCHAR))-1)
		{
			//$$$ some file end  with \n only
			if(*p2 == (WCHAR)'\r' && *(p2+1) == (WCHAR)'\n') 
			{
				break; //touch it
			}
            if(*p2 == (WCHAR)'\n') break;
			if(*p2 == (WCHAR)'\r') break;
		    p2++;
		}
		if(p2 - pHead == (int)(uSize/sizeof(WCHAR))-1)
		{
			//the last sentence 
            while(*p2 == (WCHAR)'\r' || *p2 == (WCHAR)'\n') p2--;
			int len = p2 - p1 + 1; 
			line++;
			if(len > 1)
			{
				if(len > MAX_PATH) 
				{
					p1 = p2 + 1;
					continue;
				}
				::wcsncpy(buffer, p1, len);
				buffer[len] = WCHAR('\0');

				//a section ?
			    if(buffer[0] == WCHAR('[') && 
				    buffer[len-1] == WCHAR(']'))
				{
					//AfxMessageBox(buffer);
					if(::wcscmp(buffer,szSection2) == 0) //match
					{
						if(!bSection) 
						{
							bSection = TRUE;
						    posSection = (UINT)(p1 - pHead);
						}
					}
				    else
					{
						if(bSection) return (UINT)-1;
					}
				}
			    else //a key line
				{
					if(!bSection)
					{
					}
				    else
					{
						//key name = value name
						int lenKey = ::wcslen(szKey);
					    //find =
					    int sam;
					    for(sam = 0; sam < len; sam++)
							if(buffer[sam] == WCHAR('=')) break;
					    if(sam >= lenKey && sam < len)
						{
							for(int q = 0; q < sam; q++)
							{
								if(q < lenKey && buffer[q] != szKey[q]) break;
							    if(q > lenKey && buffer[q] != WCHAR(' ')) break;
							}
						    if(q == sam) //found
							{
								bKey = TRUE;
							    return (UINT)(p1 - pHead + sam + 1);
							    //return (UINT)(p1 - pHead);
							}
						}					
					}
				}

				line++;
			}
            break;
		}
		p2--;
		//now p2 points to the ending of the sentence
        int len = p2 - p1 + 1; //in WCHARs
		line++;
		if(len > 1)
		{
			if(len > MAX_PATH) 
			{
				p1 = p2 + 1;
				continue;
			}
			//this sentence must shorter than MAX_PATH
			::wcsncpy(buffer, p1, len);
			buffer[len] = WCHAR('\0');
			
			//???, lstrcpy seems not add null-trminator
			//buffer[len] = WCHAR('\0');
			
			//a section ?
			if(buffer[0] == WCHAR('[') && 
				buffer[len-1] == WCHAR(']'))
			{
				//AfxMessageBox(buffer);
				if(::wcscmp(buffer,szSection2) == 0) //match
				{
					if(!bSection) 
					{
						bSection = TRUE;
						posSection = (UINT)(p1 - pHead);
					}
				}
				else
				{
					if(bSection) return (UINT)-1;
				}
			}
			else //a key line
			{
				if(!bSection)
				{
				}
				else
				{
					//key name = value name
					int lenKey = ::wcslen(szKey);
					//find =
					int sam;
					for(sam = 0; sam < len; sam++)
						if(buffer[sam] == WCHAR('=')) break;
					if(sam >= lenKey && sam < len)
					{
						for(int q = 0; q < sam; q++)
						{
							if(q < lenKey && buffer[q] != szKey[q]) break;
							if(q > lenKey && buffer[q] != WCHAR(' ')) break;
						}
						if(q == sam) //found
						{
							bKey = TRUE;
							return (UINT)(p1 - pHead + sam + 1);
							//return (UINT)(p1 - pHead);
						}
					}					
				}
			}
		}
		p1 = p2 + 1;
	}
	return (UINT)-1;	
}

//return -1 if not find key
//return first character of the key line 
//[section name]\r\n
//keyname = valuename\r\n
//posSection & ret all in WCHARs
UINT CJXINIFile::GetKeyW(LPWSTR pHead, UINT uSize, 
						LPCWSTR szSection, LPCWSTR szKey, UINT& posSection)
{
	posSection = -1;
	LPWSTR p1 = pHead;
	LPWSTR p2 = pHead;
	int lenSection = ::wcslen(szSection);
	WCHAR szSection2[MAX_PATH];
	//section --> [section]
    ::wcsncpy(szSection2+1, szSection, lenSection);
	szSection2[0] = WCHAR('[');
	szSection2[lenSection+1] = WCHAR(']');
	szSection2[lenSection+2] = WCHAR('\0');
        
	BOOL bSection = FALSE; //found the section
	BOOL bKey = FALSE;     //found the key
	WCHAR buffer[MAX_PATH]; 
	int line;
	
	while(p1- pHead < (int)(uSize/sizeof(WCHAR)) - 1)
	{
		//WCHAR s1 = (WCHAR)'\r'; //13
		//WCHAR s2 = (WCHAR)'\n'; //10
		//WCHAR s3 = (WCHAR)0x0A; //10
		//01/7/5 //avoid leak blank line
		int rInc, nInc;
		rInc = nInc = 0;
		while(p1- pHead < (int)(uSize/sizeof(WCHAR)-1) && 
			(*p1 == (WCHAR)'\r' || *p1 == (WCHAR)'\n'))
		{
			if(*p1 == (WCHAR)'\r') rInc++;
            if(*p1 == (WCHAR)'\n') nInc++;
			p1++;		
		}
				
		if(rInc == 1 && nInc == 1)
		{
		}
		else if(rInc == nInc && rInc > 1)
			line += rInc - 1;
		else if(rInc != nInc)
		{
			line += rInc > nInc ? rInc - 1 : nInc - 1;
		}
			
		p2 = p1;
		while(p2- pHead < (int)(uSize/sizeof(WCHAR))-1)
		{
			//$$$ some file end  with \n only
			if(*p2 == (WCHAR)'\r' && *(p2+1) == (WCHAR)'\n') 
			{
				break; //touch it
			}
            if(*p2 == (WCHAR)'\n') break;
			if(*p2 == (WCHAR)'\r') break;
		    p2++;
		}
		if(p2 - pHead == (int)(uSize/sizeof(WCHAR))-1)
		{
			//the last sentence 
            while(*p2 == (WCHAR)'\r' || *p2 == (WCHAR)'\n') p2--;
			int len = p2 - p1 + 1; 
			line++;
			if(len > 1)
			{
				if(len > MAX_PATH) 
				{
					p1 = p2 + 1;
					continue;
				}
				::wcsncpy(buffer, p1, len);
				buffer[len] = WCHAR('\0');

				//a section ?
			    if(buffer[0] == WCHAR('[') && 
				    buffer[len-1] == WCHAR(']'))
				{
					//AfxMessageBox(buffer);
					if(::wcscmp(buffer,szSection2) == 0) //match
					{
						if(!bSection) 
						{
							bSection = TRUE;
						    posSection = (UINT)(p1 - pHead);
						}
					}
				    else
					{
						if(bSection) return (UINT)-1;
					}
				}
			    else //a key line
				{
					if(!bSection)
					{
					}
				    else
					{
						//key name = value name
						int lenKey = ::wcslen(szKey);
					    //find =
					    int sam;
					    for(sam = 0; sam < len; sam++)
							if(buffer[sam] == WCHAR('=')) break;
					    if(sam >= lenKey && sam < len)
						{
							for(int q = 0; q < sam; q++)
							{
								if(q < lenKey && buffer[q] != szKey[q]) break;
							    if(q > lenKey && buffer[q] != WCHAR(' ')) break;
							}
						    if(q == sam) //found
							{
								bKey = TRUE;
							    return (UINT)(p1 - pHead + sam + 1);
							    //return (UINT)(p1 - pHead);
							}
						}					
					}
				}

				line++;
			}
            break;
		}
		p2--;
		//now p2 points to the ending of the sentence
        int len = p2 - p1 + 1; //in WCHARs
		line++;
		if(len > 1)
		{
			if(len > MAX_PATH) 
			{
				p1 = p2 + 1;
				continue;
			}
			//this sentence must shorter than MAX_PATH
			::wcsncpy(buffer, p1, len);
			buffer[len] = WCHAR('\0');
			
			//???, lstrcpy seems not add null-trminator
			//buffer[len] = WCHAR('\0');
			
			//a section ?
			if(buffer[0] == WCHAR('[') && 
				buffer[len-1] == WCHAR(']'))
			{
				//AfxMessageBox(buffer);
				if(::wcscmp(buffer,szSection2) == 0) //match
				{
					if(!bSection) 
					{
						bSection = TRUE;
						posSection = (UINT)(p1 - pHead);
					}
				}
				else
				{
					if(bSection) return (UINT)-1;
				}
			}
			else //a key line
			{
				if(!bSection)
				{
				}
				else
				{
					//key name = value name
					int lenKey = ::wcslen(szKey);
					//find =
					int sam;
					for(sam = 0; sam < len; sam++)
						if(buffer[sam] == WCHAR('=')) break;
					if(sam >= lenKey && sam < len)
					{
						for(int q = 0; q < sam; q++)
						{
							if(q < lenKey && buffer[q] != szKey[q]) break;
							if(q > lenKey && buffer[q] != WCHAR(' ')) break;
						}
						if(q == sam) //found
						{
							bKey = TRUE;
							return (UINT)(p1 - pHead + sam + 1);
							//return (UINT)(p1 - pHead);
						}
					}					
				}
			}
		}
		p1 = p2 + 1;
	}
	return (UINT)-1;	
}

UINT CJXINIFile::GetKeyA(LPSTR pHead, UINT uSize, 
						LPCSTR szSection, LPCSTR szKey, UINT& posSection)
{
	posSection = -1;
	LPSTR p1 = pHead;
	LPSTR p2 = pHead;
	int lenSection = ::strlen(szSection);
	char szSection2[MAX_PATH];
	//section --> [section]
    ::strncpy(szSection2+1, szSection, lenSection);
	szSection2[0] = char('[');
	szSection2[lenSection+1] = char(']');
	szSection2[lenSection+2] = char('\0');
        
	BOOL bSection = FALSE; //found the section
	BOOL bKey = FALSE;     //found the key
	char buffer[MAX_PATH]; 
	int line;
	
	while(p1- pHead < (int)(uSize/sizeof(char)) - 1)
	{
		//char s1 = (char)'\r'; //13
		//char s2 = (char)'\n'; //10
		//char s3 = (char)0x0A; //10
		//01/7/5 //avoid leak blank line
		int rInc, nInc;
		rInc = nInc = 0;
		while(p1- pHead < (int)(uSize/sizeof(char)-1) && 
			(*p1 == (char)'\r' || *p1 == (char)'\n'))
		{
			if(*p1 == (char)'\r') rInc++;
            if(*p1 == (char)'\n') nInc++;
			p1++;		
		}
				
		if(rInc == 1 && nInc == 1)
		{
		}
		else if(rInc == nInc && rInc > 1)
			line += rInc - 1;
		else if(rInc != nInc)
		{
			line += rInc > nInc ? rInc - 1 : nInc - 1;
		}
			
		p2 = p1;
		while(p2- pHead < (int)(uSize/sizeof(char))-1)
		{
			//$$$ some file end  with \n only
			if(*p2 == (char)'\r' && *(p2+1) == (char)'\n') 
			{
				break; //touch it
			}
            if(*p2 == (char)'\n') break;
			if(*p2 == (char)'\r') break;
		    p2++;
		}
		if(p2 - pHead == (int)(uSize/sizeof(char))-1)
		{
			//the last sentence 
            while(*p2 == (char)'\r' || *p2 == (char)'\n') p2--;
			int len = p2 - p1 + 1; 
			line++;
			if(len > 1)
			{
				if(len > MAX_PATH) 
				{
					p1 = p2 + 1;
					continue;
				}
				::strncpy(buffer, p1, len);
				buffer[len] = char('\0');

				//a section ?
			    if(buffer[0] == char('[') && 
				    buffer[len-1] == char(']'))
				{
					//AfxMessageBox(buffer);
					if(::strcmp(buffer,szSection2) == 0) //match
					{
						if(!bSection) 
						{
							bSection = TRUE;
						    posSection = (UINT)(p1 - pHead);
						}
					}
				    else
					{
						if(bSection) return (UINT)-1;
					}
				}
			    else //a key line
				{
					if(!bSection)
					{
					}
				    else
					{
						//key name = value name
						int lenKey = ::strlen(szKey);
					    //find =
					    int sam;
					    for(sam = 0; sam < len; sam++)
							if(buffer[sam] == char('=')) break;
					    if(sam >= lenKey && sam < len)
						{
							for(int q = 0; q < sam; q++)
							{
								if(q < lenKey && buffer[q] != szKey[q]) break;
							    if(q > lenKey && buffer[q] != char(' ')) break;
							}
						    if(q == sam) //found
							{
								bKey = TRUE;
							    return (UINT)(p1 - pHead + sam + 1);
							    //return (UINT)(p1 - pHead);
							}
						}					
					}
				}

				line++;
			}
            break;
		}
		p2--;
		//now p2 points to the ending of the sentence
        int len = p2 - p1 + 1; //in WCHARs
		line++;
		if(len > 1)
		{
			if(len > MAX_PATH) 
			{
				p1 = p2 + 1;
				continue;
			}
			//this sentence must shorter than MAX_PATH
			::strncpy(buffer, p1, len);
			buffer[len] = char('\0');
			
			//???, lstrcpy seems not add null-trminator
			//buffer[len] = char('\0');
			
			//a section ?
			if(buffer[0] == char('[') && 
				buffer[len-1] == char(']'))
			{
				//AfxMessageBox(buffer);
				if(::strcmp(buffer,szSection2) == 0) //match
				{
					if(!bSection) 
					{
						bSection = TRUE;
						posSection = (UINT)(p1 - pHead);
					}
				}
				else
				{
					if(bSection) return (UINT)-1;
				}
			}
			else //a key line
			{
				if(!bSection)
				{
				}
				else
				{
					//key name = value name
					int lenKey = ::strlen(szKey);
					//find =
					int sam;
					for(sam = 0; sam < len; sam++)
						if(buffer[sam] == char('=')) break;
					if(sam >= lenKey && sam < len)
					{
						for(int q = 0; q < sam; q++)
						{
							if(q < lenKey && buffer[q] != szKey[q]) break;
							if(q > lenKey && buffer[q] != char(' ')) break;
						}
						if(q == sam) //found
						{
							bKey = TRUE;
							return (UINT)(p1 - pHead + sam + 1);
							//return (UINT)(p1 - pHead);
						}
					}					
				}
			}
		}
		p1 = p2 + 1;
	}
	return (UINT)-1;	
}

void CJXINIFile::CutZeroTail()
{
	//AfxMessageBox(m_strFilename);
	//::PopMsg(_T("%s - %d"), m_strFilename, m_dwUseSize);
	if(m_dwUseSize == 0) return; //empty file
	int i = m_dwUseSize - 1;
	LPBYTE pByte = (LPBYTE)m_pData; 
	pByte += m_dwUseSize - sizeof(TCHAR);
    //if(sizeof(TCHAR) == 1) //MBCS
	if(!this->m_bUnicode)
	{
		while(*pByte == 0x00 && pByte - (LPBYTE)m_pData > 0)
		{
			pByte--;
			m_dwUseSize--;
		}
	}
	else //UNICODE
	{
		while(*pByte == 0x00 && *(pByte+1) == 0x00 &&
			pByte - (LPBYTE)m_pTrueData > 0)
		{
			pByte -= 2;
			m_dwUseSize -= 2;
		}
	}
	//DWORD   m_dwSize; //Current Open File Size in BYTEs
	//DWORD   m_dwUseSize; //Curent Using Size in BYTEs
	//AfxMessageBox(_T("")); //Šm”F‚n‚j
	return;
}

DWORD CJXINIFile::DeleteSectionW(
		 LPCWSTR lpAppName        // section name
    )
{
    int lenTargetSec = wcslen(lpAppName);
	LPWSTR pSecHead = NULL;
	LPWSTR pHead = (LPWSTR)m_pTrueData;
	LPWSTR p1 = pHead;
	LPWSTR p2 = pHead;
        
	while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1)
	{
		while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p1 != WCHAR('['))
			p1++;
		p2 = p1;
		while(p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p2 != WCHAR(']'))
			p2++;
		if(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) &&
			p2- pHead < (int)(m_dwUseSize/sizeof(WCHAR)))
		{
			//Found A Section, Check
			int lenSection = p2 - p1 - 1;
			if(lenSection == lenTargetSec)
			{
				pSecHead = p1;
				for(int i = 0; i < lenTargetSec; i++)
				{
					if(lpAppName[i] != (WCHAR)*(p1+1+i))
					{
                        pSecHead = NULL;
						break;
					}
				}
			} 
		}
		if(pSecHead != NULL) break;
		p1 = p2 + 1;
	}

	if(pSecHead == NULL) return -1;
    p1 = pSecHead + lenTargetSec;

	while(p1- pHead < (int)(m_dwUseSize/sizeof(WCHAR)) - 1 &&
			*p1 != WCHAR('['))
			p1++;
	   
	if(p1- pHead == (int)(m_dwUseSize/sizeof(WCHAR)) - 1)
	{
		//This is the Last Section
        m_dwUseSize = sizeof(WCHAR)*(pSecHead-pHead);
		return 0;
	}
	else
	{
		int memLen = m_dwUseSize - (p1 - pHead)*sizeof(WCHAR);
		//C++ Run-time Lib handle overlapped mem
        memmove(pSecHead, p1, memLen);
		m_dwUseSize -= sizeof(WCHAR)*(p1 - pSecHead);
	}
	return 0;
}

DWORD CJXINIFile::DeleteKeyW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName  // key name
    )
{
	LPWSTR pHead = (LPWSTR)m_pTrueData;
	UINT uSec, uKey;  
	//Warning: uKey, uSec is measured in WCHARs!!!!!
	//in UNICODE now
	uKey = GetKeyW(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1 && uSec == (UINT)-1) //no section
	{
		return -1;
	}
	else if(uKey == (UINT)-1 && uSec != (UINT)-1) 
	//section exist but value do not exist
	{
		return -1;		
	}
	else //key exist
	{
		ASSERT(uKey != (UINT)-1);
        LPWSTR pValue = pHead + uKey;
	    while(pValue - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
		    *pValue != WCHAR('\r') && *pValue != WCHAR('\n'))
		{
			pValue++;
		}

	    while(*pValue == WCHAR('\r') || *pValue == WCHAR('\n'))
			pValue++;
		int lenKey = pValue - pHead - uKey;
        LPWSTR pKeyHead = pHead + uKey;
		int lenMove = m_dwUseSize - (pValue - pHead)*sizeof(WCHAR);
		memmove(pKeyHead, pValue, lenMove);
        m_dwUseSize -= lenKey*sizeof(WCHAR);
	}
	this->m_bSectionMapDirty = TRUE;
	return 0;
}

DWORD CJXINIFile::DeleteSectionExW(
		 LPCWSTR lpAppName        // section name
    )
{
	ASSERT(!this->m_bSectionMapLock);
	if(this->m_bSectionMapDirty)
		this->UpdateMapW();

    LPWSTR lpHead = (LPWSTR)m_pTrueData;    
	CString strKey;
	LPWSTR lpPos;
#ifdef _UNICODE
	strKey = lpAppName;
#else
	DWORD dwRetSize;
	LPSTR pTemp = _W2A((LPWSTR)lpAppName, -1, dwRetSize);
	::strncpy(strKey.GetBuffer(dwRetSize), pTemp, dwRetSize);
	strKey.ReleaseBuffer(dwRetSize);
#endif
	//No Such Section Exists
	LPVOID lpVoid;
	if(!m_mapSection.Lookup(strKey, lpVoid)) return -1;
    ASSERT(m_mapSection.RemoveKey(strKey));
	lpPos = (LPWSTR)lpVoid;
	LPWSTR lpSection = (LPWSTR)lpPos;
    
	LPWSTR lpNextSec = NULL;
	int distance = -1;

	POSITION pos = m_mapSection.GetStartPosition();
	while(pos)
	{
		CString strKey2;
		LPWSTR lpPos2;
		m_mapSection.GetNextAssoc(pos, strKey2, lpVoid);
		lpPos2 = (LPWSTR)lpVoid;
		if(lpPos2 - lpPos > 0)
		{
			if(distance == -1) 
			{
				distance = lpPos2 - lpPos;
				lpNextSec = lpPos2;
			}
			else
			{
				if(lpPos2 - lpPos < distance)
				{
					distance = lpPos2 - lpPos;
				    lpNextSec = lpPos2;
				}
			}

		}
	}
    
	
	if(lpNextSec == NULL)
	{
		ASSERT(distance == -1);
		//This is the Last Section
        m_dwUseSize = sizeof(WCHAR)*(lpPos-lpHead);
		return 0;
	}
	else
	{
		int memLen = m_dwUseSize - (lpNextSec - lpHead)*sizeof(WCHAR);
		//C++ Run-time Lib handle overlapped mem
        memmove((LPVOID)lpPos, (LPVOID)lpNextSec, memLen);
		m_dwUseSize -= sizeof(WCHAR)*(lpNextSec - lpPos);
	}
    
	//m_bSectionMapDirty = TRUE;
	//return TRUE;

	int lenSec = lpNextSec - lpPos;
	if(m_bSectionMapLock)
	{ 
		m_bSectionMapDirty = TRUE;
	}
	else //update section map
	{
		POSITION pos = m_mapSection.GetStartPosition();
	    while(pos)
		{
			CString strTemp;
	        LPVOID lpTemp;
	        m_mapSection.GetNextAssoc(pos, strTemp, lpTemp);
	    	//adjust Section Map
			if((LPWSTR)lpTemp - lpSection > 0)
			{
				LPWSTR lp = (LPWSTR)lpTemp;
				lp -= lenSec;
				lpTemp = (LPVOID)lp;
				m_mapSection.SetAt(strTemp, lpTemp);
			}
		}
	}
	return TRUE;
}

DWORD CJXINIFile::DeleteKeyExW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName  // key name
    )
{
	LPWSTR pHead = (LPWSTR)m_pTrueData;
	UINT uSec, uKey;  
	//Warning: uKey, uSec is measured in WCHARs!!!!!
	//in UNICODE now
	uKey = GetKeyExW(pHead, (UINT)m_dwUseSize, 
		lpAppName, lpKeyName, uSec);
    if(uKey == (UINT)-1 && uSec == (UINT)-1) //no section
	{
		return -1;
	}
	else if(uKey == (UINT)-1 && uSec != (UINT)-1) 
	//section exist but value do not exist
	{
		return -1;		
	}
	else //key exist
	{
		ASSERT(uKey != (UINT)-1);
		LPWSTR pSection = pHead + uSec;
        LPWSTR pValue = pHead + uKey;
	    while(pValue - pHead < (int)(m_dwUseSize/sizeof(WCHAR)) && 
		    *pValue != WCHAR('\r') && *pValue != WCHAR('\n'))
		{
			pValue++;
		}

	    while(*pValue == WCHAR('\r') || *pValue == WCHAR('\n'))
			pValue++;
		int lenKey = pValue - pHead - uKey;
        LPWSTR pKeyHead = pHead + uKey;
		int lenMove = m_dwUseSize - (pValue - pHead)*sizeof(WCHAR);
		memmove(pKeyHead, pValue, lenMove);
        m_dwUseSize -= lenKey*sizeof(WCHAR);

		if(m_bSectionMapLock)
		{ 
			m_bSectionMapDirty = TRUE;
		}
		else //update section map
		{
			POSITION pos = m_mapSection.GetStartPosition();
	        while(pos)
			{
				CString strKey;
		        LPVOID lpPos;
		        m_mapSection.GetNextAssoc(pos, strKey, lpPos);
		    	//adjust Section Map
				if((LPWSTR)lpPos - pSection > 0)
				{
					LPWSTR pTemp = (LPWSTR)lpPos;
					pTemp -= lenKey;
					lpPos = (LPVOID)pTemp;
					m_mapSection.SetAt(strKey, lpPos);
				}
			}
		}
	}
	return 0;
}

DWORD CJXINIFile::DeleteSectionExA(
		 LPCSTR lpAppName        // section name
    )
{
	return 0;
}

DWORD CJXINIFile::DeleteKeyExA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName  // key name
    )
{
	return 0;
}

DWORD CJXINIFile::DeleteSectionA(
		 LPCSTR lpAppName        // section name
    )
{
	return 0;
}

DWORD CJXINIFile::DeleteKeyA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName  // key name
    )
{
	return 0;
}

//-------------------------------------------------------------------
//-------------Export Part ------------------------------------------
//UNICODE & ANSI Version
CJXINIFile& GetINIFileClass()
{
	static CJXINIFile g_clsINIFile;
	return g_clsINIFile;
}

DWORD JXGetPrivateProfileStringW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpKeyName,        // key name
  LPCWSTR lpDefault,        // default string
  LPWSTR lpReturnedString,  // destination buffer
  DWORD nSize,              // size of destination buffer
  LPCWSTR lpFileName        // initialization file name
 )
{
	return GetINIFileClass().GetPrivateProfileStringExW(
		lpAppName,        // section name
        lpKeyName,        // key name
        lpDefault,        // default string
        lpReturnedString,  // destination buffer
        nSize,              // size of destination buffer
        lpFileName        // initialization file name
		);
}

BOOL JXWritePrivateProfileStringW(
  LPCWSTR lpAppName,  // section namen
  LPCWSTR lpKeyName,  // key name
  LPCWSTR lpString,   // string to add
  LPCWSTR lpFileName  // initialization file
 )
{
	return GetINIFileClass().WritePrivateProfileStringExW(
        lpAppName,  // section namen
        lpKeyName,  // key name
        lpString,   // string to add
        lpFileName  // initialization file
		);
}


//ANSI VERSION
DWORD JXGetPrivateProfileStringA(
  LPCSTR lpAppName,        // section name
  LPCSTR lpKeyName,        // key name
  LPCSTR lpDefault,        // default string
  LPSTR lpReturnedString,  // destination buffer
  DWORD nSize,              // size of destination buffer
  LPCSTR lpFileName        // initialization file name
 )
{
	return GetINIFileClass().GetPrivateProfileStringExA(
		lpAppName,        // section name
        lpKeyName,        // key name
        lpDefault,        // default string
        lpReturnedString,  // destination buffer
        nSize,              // size of destination buffer
        lpFileName        // initialization file name
		);
}

//Warning: Using namespace for WinAPI have the same name
BOOL JXWritePrivateProfileStringA(
  LPCSTR lpAppName,  // section namen
  LPCSTR lpKeyName,  // key name
  LPCSTR lpString,   // string to add
  LPCSTR lpFileName  // initialization file
 )
{
	return GetINIFileClass().WritePrivateProfileStringExA(
        lpAppName,  // section namen
        lpKeyName,  // key name
        lpString,   // string to add
        lpFileName  // initialization file
		);
}

DWORD ControlINI(DWORD dwControl)
{
	#define INI_SET_DIRTY_MAP  0
#define INI_SET_LOCK_MAP   1
#define INI_SET_UNLOCK_MAP 2
#define INI_UPDATE_MAP     3
	switch(dwControl)
	{
	case INI_SET_DIRTY_MAP:
        GetINIFileClass().m_bSectionMapDirty = TRUE;
		break;
	case INI_SET_LOCK_MAP:
        GetINIFileClass().m_bSectionMapLock = TRUE;
		break;
    case INI_SET_UNLOCK_MAP:
        GetINIFileClass().m_bSectionMapLock = FALSE;
		break;
	case INI_UPDATE_MAP:
#ifdef _UNICODE
		GetINIFileClass().UpdateMapW();
#else
		GetINIFileClass().UpdateMapA();
#endif
		break;
	default:
		break;
	}
	return 0;
}

//Flush Memory Data To Disk File and Close File Handle
BOOL JXUnloadPrivateProfile()
{
	return GetINIFileClass().UnloadIniFile();
}

BOOL JXRenameSectionExW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpNewName,        // new section name
  LPCWSTR lpFileName        // initialization file name
 )
{
	return GetINIFileClass().RenameSectionExW(
		lpAppName,        // section name
        lpNewName,        // new section name
        lpFileName        // initialization file name
		);
}

BOOL JXCopySectionExW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpNewName,        // new section name
  LPCWSTR lpFileName        // initialization file name
 )
{
	return GetINIFileClass().CopySectionExW(
		lpAppName,        // section name
        lpNewName,        // new section name
        lpFileName        // initialization file name
		);
}

BOOL CJXINIFile::RenameSectionExW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpNewName,        // new section name
  LPCWSTR lpFileName        // initialization file name
 )
{
	BOOL bRet = LoadIniFile(lpFileName, 2);  //Unicode Text
	if(!bRet) 
	{
		AfxMessageBox(_T("CJXINIFile::RenameSectionExW - LoadIniFile Err"));
		return FALSE;
	}
	
	//If Section Locked And Section Dirty ASSERT(FALSE);
    ASSERT(!(m_bSectionMapLock && m_bSectionMapDirty));
	if(m_bSectionMapDirty) this->UpdateMapW();

    LPWSTR pHead = (LPWSTR)m_pTrueData;    
	CString strKey, strNewKey;
	LPWSTR lpPos;
#ifdef _UNICODE
	strKey = lpAppName;
	strNewKey = lpNewName;
#else
	DWORD dwRetSize;
	LPSTR pTemp = _W2A((LPWSTR)lpAppName, -1, dwRetSize);
	::strncpy(strKey.GetBuffer(dwRetSize), pTemp, dwRetSize);
	strKey.ReleaseBuffer(dwRetSize);

	pTemp = _W2A((LPWSTR)lpNewName, -1, dwRetSize);
	::strncpy(strNewKey.GetBuffer(dwRetSize), pTemp, dwRetSize);
	strNewKey.ReleaseBuffer(dwRetSize);
#endif
	//No Such Section Exists
	LPVOID lpVoid;
	if(!m_mapSection.Lookup(strKey, lpVoid)) 
	{
#ifdef _DEBUG
		AfxMessageBox(_T("CJXINIFile::RenameSectionExW - lpAppName Not Found") + strKey);
#endif
		return FALSE;
	}
    m_mapSection.RemoveKey(strKey);
	lpPos = (LPWSTR)lpVoid;
	LPWSTR lpSection = (LPWSTR)lpPos;
    
	int oldLen = ::wcslen(lpAppName);
	int newLen = ::wcslen(lpNewName);
	m_mapSection.SetAt(strNewKey, (LPVOID)lpSection);
	if(newLen == oldLen)
	{
		::wcscspn((LPWSTR)(lpSection+1), (LPCWSTR)lpNewName); //[
		return TRUE;
	}
	//Hard Time Now
	if(newLen < oldLen)
	{
		int memLen = m_dwUseSize - (lpSection - pHead + oldLen + 2)*sizeof(WCHAR);
		//C++ Run-time Lib handle overlapped mem
		LPWSTR buffer = new WCHAR[newLen+2];
		::swprintf(buffer, L"[%s]", lpNewName);
		memmove(lpSection+2+newLen, lpSection+2+oldLen, memLen);
		::wcsncpy(lpSection, buffer, newLen+2);
		m_dwUseSize -= sizeof(WCHAR)*(oldLen-newLen);
	}
	else
	{
		int memLen = m_dwUseSize - (lpSection - pHead + oldLen + 2)*sizeof(WCHAR);
		//C++ Run-time Lib handle overlapped mem
		LPWSTR buffer = new WCHAR[newLen+2];
		::swprintf(buffer, L"[%s]", lpNewName);
		memmove(lpSection+2+newLen, lpSection+2+oldLen, memLen);
		::wcsncpy(lpSection, buffer, newLen+2);
		m_dwUseSize += sizeof(WCHAR)*(newLen-oldLen);
	}
 
	//Adjust Section Map if Applicable
	if(m_bSectionMapLock)
	{ 
		m_bSectionMapDirty = TRUE;
	}
	else //update section map
	{
		POSITION pos = m_mapSection.GetStartPosition();
	    while(pos)
		{
			CString strKey;
	        LPVOID lpPos;
	        m_mapSection.GetNextAssoc(pos, strKey, lpPos);
	    	//adjust Section Map
			if((LPWSTR)lpPos - lpSection > 0)
			{
				LPWSTR pTemp = (LPWSTR)lpPos;
				pTemp += newLen-oldLen;
				lpPos = (LPVOID)pTemp;
				m_mapSection.SetAt(strKey, lpPos);
			}
		}
	}
	return TRUE;
}

BOOL CJXINIFile::CopySectionExW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpNewName,        // new section name
  LPCWSTR lpFileName        // initialization file name
 )
{
    BOOL bRet = LoadIniFile(lpFileName, 2);  //Unicode Text
	if(!bRet) 
	{
		AfxMessageBox(_T("CJXINIFile::CopySectionExW - LoadIniFile Err"));
		return FALSE;
	}

	if(m_bSectionMapLock) 
	{
		AfxMessageBox(_T("CJXINIFile::CopySectionExW - Map Locked Err"));
		return FALSE;
	}
	if(m_bSectionMapDirty) this->UpdateMapW();
	
	m_bSectionMapLock = FALSE;
	LPWSTR lpHead = (LPWSTR)m_pTrueData;    
	CString strKey, strNewKey;
	LPWSTR lpPos;
#ifdef _UNICODE
	strKey = lpAppName;
	strNewKey = lpNewName;
#else
	DWORD dwRetSize;
	LPSTR pTemp = _W2A((LPWSTR)lpAppName, -1, dwRetSize);
	::strncpy(strKey.GetBuffer(dwRetSize), pTemp, dwRetSize);
	strKey.ReleaseBuffer(dwRetSize);

	pTemp = _W2A((LPWSTR)lpNewName, -1, dwRetSize);
	::strncpy(strNewKey.GetBuffer(dwRetSize), pTemp, dwRetSize);
	strNewKey.ReleaseBuffer(dwRetSize);
#endif

	//No Such Section Exists
	LPVOID lpVoid;
	if(!m_mapSection.Lookup(strKey, lpVoid)) 
	{
#ifdef _DEBUG
		AfxMessageBox(_T("CJXINIFile::CopySectionExW - lpAppName Not Found") + strKey);
#endif
		return FALSE;
	}
    lpPos = (LPWSTR)lpVoid;
	LPWSTR lpSection = (LPWSTR)lpPos;
   
	//The lpNewName -- New Section Name Should not Exist
    if(m_mapSection.Lookup(strNewKey, lpVoid)) 
	{
		AfxMessageBox(_T("CJXINIFile::CopySectionExW - lpNewName Exist"));
	    return FALSE;
	}

	UINT secBodySize = GetSectionSizeExW(lpAppName, lpFileName);
    secBodySize -= 2 + ::wcslen(lpAppName);
	if((int)(m_dwUseSize - m_dwSize + 
		(secBodySize+::wcslen(lpNewName))*sizeof(WCHAR)) > JXFlushIncrement - 5*JXPage)
	{
		//::MessageBoxW(NULL, lpAppName, lpNewName, MB_OK);
		//if the Section Reproduction Need Too Much Space
		//Flush Back to HardDisk First. Then Append It
		LPVOID lpMem = ::VirtualAlloc(NULL, (secBodySize+::wcslen(lpNewName))*sizeof(WCHAR),
			MEM_COMMIT, PAGE_READWRITE);
		if(!lpMem) 
		{
			::MessageBox(NULL, _T("CopySection"), _T("VirtualAlloc Err"), MB_OK);
			return FALSE;
		}
		::memcpy(lpMem, lpSection + 2 + ::wcslen(lpAppName), secBodySize*sizeof(WCHAR));
		UnloadIniFile();
		m_bSectionMapDirty = TRUE;

		//Write To Disk
        HANDLE hFile = ::CreateFileW(lpFileName, GENERIC_WRITE,
		 FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		 FILE_ATTRIBUTE_NORMAL, NULL);
	    if(hFile == INVALID_HANDLE_VALUE) 
		{
			::MessageBox(NULL, _T("CopySectionExW"), _T("CreateFileW Err"), MB_OK);
			return FALSE;
		}
		LONG l = 0;
		::SetFilePointer(hFile, 0, &l, FILE_END);
		LPWSTR buffer = new WCHAR[2 + ::wcslen(lpNewName)];
        ::swprintf(buffer, L"[%s]", lpNewName);
		DWORD dwRet;
		::WriteFile(hFile, buffer, sizeof(WCHAR)*(2 + ::wcslen(lpNewName)), &dwRet, NULL);
		::WriteFile(hFile, lpMem, sizeof(WCHAR)*secBodySize, &dwRet, NULL);
		::CloseHandle(hFile);
		return TRUE;
	}
	else
	{
		LPWSTR lpNewSec = lpHead + m_dwUseSize/sizeof(WCHAR);
		if(!m_bSectionMapLock)
		{
			m_mapSection.SetAt(strNewKey, (LPVOID)lpNewSec);
		}
		else
			m_bSectionMapDirty = TRUE;	
        ::swprintf(lpNewSec, L"[%s]", lpNewName);
		lpNewSec += 2 + ::wcslen(lpNewName);
		::memcpy(lpNewSec, lpSection + 2 + ::wcslen(lpAppName), sizeof(WCHAR)*secBodySize);
        m_dwUseSize += (secBodySize + ::wcslen(lpNewName) + 2)*sizeof(WCHAR); 			
	}
	return TRUE;
}


//in WCHARs, return 0 when err, Section Map Version
UINT CJXINIFile::GetSectionSizeExW(
	   LPCWSTR lpAppName,        // section name
       LPCWSTR lpFileName)        // initialization file name
{
	BOOL bRet = LoadIniFile(lpFileName, 2);  //Unicode Text
	if(!bRet) return FALSE;

	CString strKey;
#ifdef _UNICODE
	strKey = lpAppName;
#else
	DWORD dwRetSize;
	LPSTR pTemp = _W2A((LPWSTR)lpAppName, -1, dwRetSize);
	::strncpy(strKey.GetBuffer(dwRetSize), pTemp, dwRetSize);
	strKey.ReleaseBuffer(dwRetSize);
#endif
	LPWSTR pHead = (LPWSTR)m_pTrueData; 
	//if No Such Section Exists
	LPVOID lpVoid;
	if(!m_mapSection.Lookup(strKey, lpVoid)) return 0;
    LPWSTR lpPos = (LPWSTR)lpVoid;
	LPWSTR lpSection = (LPWSTR)lpPos;

	LPWSTR lpNextSec = NULL;
	int distance = -1;

	POSITION pos = m_mapSection.GetStartPosition();
	while(pos)
	{
		CString strKey2;
		LPWSTR lpPos2;
		m_mapSection.GetNextAssoc(pos, strKey2, lpVoid);
		lpPos2 = (LPWSTR)lpVoid;
		if(lpPos2 - lpPos > 0)
		{
			if(distance == -1) 
			{
				distance = lpPos2 - lpPos;
				lpNextSec = lpPos2;
			}
			else
			{
				if(lpPos2 - lpPos < distance)
				{
					distance = lpPos2 - lpPos;
				    lpNextSec = lpPos2;
				}
			}
		}
	}
 	
	if(lpNextSec == NULL)
	{
		ASSERT(distance == -1);
		//This is the Last Section
        return m_dwUseSize/sizeof(WCHAR) - (lpPos-pHead);
	}
	else
	{
        return lpNextSec - lpPos;
	}
    return 0;
}

////in WCHARs, return 0 when err,
UINT CJXINIFile::GetSectionSizeW(
	   LPCWSTR lpAppName,        // section name
       LPCWSTR lpFileName)        // initialization file name
{
	return 0;
}