// MemTools.cpp: implementation of the CMemTools class.
//////////////////////////////////////////////////////////////////////////////// 
// Description:
// 根据"RAII 资源获取即初始化"的思想，将资源的获取和释放与对象的构造和
// 析构对应起来，从而确保在对象的生存期内资源始终有效，对象销毁时资源必被释放。
//
// notification:
// 注意申请内存的时候要保证内存申请的足够大，一旦发生内存越界，在释放资源
// 的时候，就会发生崩溃或者不可预知的结果。
// 注意使用的时候使用命名空间: using namespace MemTools;
//
////////////////////////////////////////////////////////////////////////////////
//
//   Author      : zilla.dragon
//   Data        : 2011-07-26
//   Tab width   : 4
//   Platform    : Win32
//
////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MemTools.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
using namespace MemTools;

CByteBuf::CByteBuf()
{
    ptr = NULL;
    int ilen  = 4096;
    ptr = (BYTE *)malloc(ilen);
    assert(ptr);
    memset(ptr, 0, ilen);
	len = 4096;
	ptr2 = ptr;
}

CByteBuf::CByteBuf(int ilen)
{
    ptr = NULL;
	ptr = (BYTE *)malloc(ilen);
	assert(ptr);
	memset(ptr, 0, ilen);
	len = ilen;
	ptr2 = ptr;
}

CByteBuf::~CByteBuf()
{ 
	free(ptr);
	ptr = 0;  ptr2 = 0;
}


//////////////////////////////////////////////////////////////////////////
CTCharBuf::CTCharBuf(int len)
{
    ptr = NULL;
    int ilen  = 0;
    ilen = sizeof(TCHAR)*len + 128;
	ptr = (TCHAR*)malloc(ilen);
	assert(ptr);
    memset(ptr, 0, ilen);
}
CTCharBuf::~CTCharBuf()
{ 
	free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CW2ACharBuf::CW2ACharBuf(const wchar_t* lp)
{
	ptr = NULL;
	int nConvertedLen = WideCharToMultiByte(CP_ACP, 0, lp,-1, NULL, 0, NULL, NULL);
	ptr =(char*)malloc(nConvertedLen*sizeof(wchar_t));
	assert(ptr);
	memset(ptr, 0, nConvertedLen*sizeof(wchar_t));
	WideCharToMultiByte(CP_ACP, 0, lp, -1, ptr, nConvertedLen, NULL, NULL);		
}
CW2ACharBuf::~CW2ACharBuf()
{ 
	free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CW2TCharBuf::CW2TCharBuf(const wchar_t* lp)
{
    ptr = NULL;
#if defined(UNICODE) || defined(_UNICODE)
	ptr = (TCHAR*)malloc((_tcslen(lp)+1)*sizeof(TCHAR));
	assert(ptr);
    memset(ptr, 0, (_tcslen(lp)+1)*sizeof(TCHAR));
    _tcscpy(ptr, lp);
#else
    int nConvertedLen = WideCharToMultiByte(CP_ACP, 0, lp,-1, NULL, 0, NULL, NULL);
    ptr =(char*)malloc(nConvertedLen*sizeof(TCHAR));
	assert(ptr);
    memset(ptr, 0, nConvertedLen*sizeof(TCHAR));
    WideCharToMultiByte(CP_ACP, 0, lp, -1, ptr, nConvertedLen, NULL,NULL);	
#endif
}
CW2TCharBuf::~CW2TCharBuf()
{ 
    free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CA2TCharBuf::CA2TCharBuf(const char* lp)
{
    ptr = NULL;
	int len = 0;
#if defined(UNICODE) || defined(_UNICODE)
	int nConvertedLen = MultiByteToWideChar(CP_ACP, 0, lp,-1, NULL, NULL);
	ptr = (wchar_t*)malloc(nConvertedLen*sizeof(wchar_t));
	assert(ptr);
	if (ptr != NULL){
		memset(ptr, 0, nConvertedLen*sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, lp, -1, ptr, nConvertedLen);
	}
#else
	len =  (_tcslen(lp)+1)*sizeof(TCHAR);
	ptr = (TCHAR*)malloc(len);
    assert(ptr);
    memset(ptr, 0, len);
    _tcscpy(ptr, lp);
#endif
}
CA2TCharBuf::~CA2TCharBuf()
{ 
    free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CA2WCharBuf::CA2WCharBuf(const char* lp)
{
    int len = 0;
	ptr = NULL;
	int nConvertedLen = MultiByteToWideChar(CP_ACP, 0, lp,-1, NULL, NULL);
	len = sizeof(wchar_t)*nConvertedLen;
    ptr = (wchar_t*)malloc(len);
    assert(ptr);
	if (ptr != NULL){
        memset(ptr, 0, len);
        MultiByteToWideChar(CP_ACP, 0, lp, -1, ptr, nConvertedLen);		
	}
}
CA2WCharBuf::~CA2WCharBuf()
{ 
	free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CT2WCharBuf::CT2WCharBuf(const TCHAR* lp)
{
    ptr = NULL;
    int len = 0;
#if defined(UNICODE) || defined(_UNICODE)
    len  = (_tcslen(lp)+1)*sizeof(TCHAR);
	ptr = (TCHAR *)malloc(len);
	assert(ptr);
	memset(ptr, 0, len);
	_tcscpy(ptr, lp);
#else
	ptr = NULL;
	int nConvertedLen = MultiByteToWideChar(CP_ACP, 0, lp,-1, NULL, NULL);
	ptr =(wchar_t*)malloc(sizeof(wchar_t)*nConvertedLen);
	assert(ptr);
	if (ptr != NULL){
		memset(ptr, 0, sizeof(wchar_t)*nConvertedLen);
		MultiByteToWideChar(CP_ACP, 0, lp, -1, ptr, nConvertedLen);		
	}
#endif
}
CT2WCharBuf::~CT2WCharBuf()
{ 
	free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CT2ACharBuf::CT2ACharBuf(const TCHAR* lp)
{
    ptr = NULL;
    int len = 0;

#if defined(UNICODE) || defined(_UNICODE)
	int nConvertedLen = WideCharToMultiByte(CP_ACP, 0, lp,-1, NULL, 0, NULL, NULL);
	ptr =(char*)malloc(nConvertedLen);
	assert(ptr);
	memset(ptr, 0, nConvertedLen);
	WideCharToMultiByte(CP_ACP, 0, lp, -1, ptr, nConvertedLen, NULL, NULL);	
#else
    len = (_tcslen(lp)+1)*sizeof(TCHAR);
	ptr = (TCHAR*)malloc(len);
	assert(ptr);
	memset(ptr, 0, len);
	_tcscpy(ptr, lp);
#endif
}
CT2ACharBuf::~CT2ACharBuf()
{ 
	free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CUT82TCharBuf::CUT82TCharBuf(const char* lp)
{
    ptr = NULL;
    int len = 0;
#if defined(UNICODE) || defined(_UNICODE)
    int nConvertedLen = MultiByteToWideChar(CP_UTF8, 0, lp,-1, NULL, NULL);
    len = nConvertedLen*sizeof(wchar_t);
    ptr = (wchar_t*)malloc(len);
    assert(ptr);
    if (ptr != NULL){
        memset(ptr, 0, len);
        MultiByteToWideChar(CP_UTF8, 0, lp, -1, ptr, nConvertedLen);
    }
#else
    // utf8 -> wchar -> ansi
	int ilen = _tcslen(lp);
	int iSize1 = 0;
	int iSize2 = 0;

	iSize1 = MultiByteToWideChar(CP_UTF8, 0, lp, -1, NULL, 0);
	if( iSize1<= 0)
		return;

	wchar_t* pwszStr = (wchar_t*)malloc(iSize1*(sizeof(wchar_t)));
	if (!pwszStr)
		return;
	assert(pwszStr);
	memset(pwszStr, 0, iSize1 * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, lp, -1, pwszStr, iSize1);

	iSize2 = WideCharToMultiByte(CP_ACP, 0, pwszStr, -1, NULL, 0, NULL, NULL);
	if(iSize2 <= 0){
		free(pwszStr); return;
	}

	char* pszStr = (char*)malloc(iSize2);
	if(!pszStr){
		free(pwszStr); return;
	}

	assert(pszStr);
	memset(pszStr, 0, iSize2);

	WideCharToMultiByte(CP_ACP, 0, pwszStr, -1, pszStr, iSize2, NULL, NULL);

	free(pwszStr);
	ptr = pszStr;
	assert(ptr);

#endif
}
CUT82TCharBuf::~CUT82TCharBuf()
{ 
    free(ptr);
}

//////////////////////////////////////////////////////////////////////////
CT2UTF8CharBuf::CT2UTF8CharBuf(const TCHAR* lp)
{
    ptr = NULL;
    int len = 0;
#if defined(UNICODE) || defined(_UNICODE)
    int nConvertedLen = WideCharToMultiByte(CP_UTF8, 0, lp,-1, NULL, 0, NULL, NULL);
    ptr =(char*)malloc(nConvertedLen);
    assert(ptr);
    memset(ptr, 0, nConvertedLen);
    WideCharToMultiByte(CP_UTF8, 0, lp, -1, ptr, nConvertedLen, NULL, NULL);	
#else
    // ansi -> wchar -> utf8

	int ilen = _tcslen(lp);
	int iSize1 = 0;
	int iSize2 = 0;
		
	iSize1 = MultiByteToWideChar(CP_ACP, 0, lp, -1, NULL, 0);
	if( iSize1<= 0)
		return;

	wchar_t* pwszStr = (wchar_t*)malloc(iSize1*(sizeof(wchar_t)));
	if (!pwszStr)
		return;
	assert(pwszStr);
	memset(pwszStr, 0, iSize1 * sizeof(wchar_t));

	MultiByteToWideChar(CP_ACP, 0, lp, -1, pwszStr, iSize1);

	iSize2 = WideCharToMultiByte(CP_UTF8, 0, pwszStr, -1, NULL, 0, NULL, NULL);
	if(iSize2 <= 0){
		free(pwszStr); return;
	}

	char* pszStr = (char*)malloc(iSize2);
	if(!pszStr){
		free(pwszStr); return;
	}
	assert(pszStr);
	memset(pszStr, 0, iSize2);

	WideCharToMultiByte(CP_UTF8, 0, pwszStr, -1, pszStr, iSize2, NULL, NULL);

	free(pwszStr);
	ptr = pszStr;
    assert(ptr);

#endif
}
CT2UTF8CharBuf::~CT2UTF8CharBuf()
{ 
    free(ptr);
}

//////////////////////////////////////////////////////////////////////////
ReleaseKeys& ReleaseKeys::add(HKEY hkey)
{
	m_keys.push_back(hkey);
	return *this;
}
ReleaseKeys::~ReleaseKeys()
{
	for( unsigned int i = 0 ; i < m_keys.size() && m_keys[i] != 0 ; i++ )
		RegCloseKey(m_keys[i]);
}


//////////////////////////////////////////////////////////////////////////
ReleasePtrs& ReleasePtrs::add(byte* ptr)
{
	m_ptrs.push_back(ptr);
		return *this;
}
ReleasePtrs::~ReleasePtrs()
{
	for( unsigned int i = 0 ; i < m_ptrs.size() && m_ptrs[i] != 0 ; i++ )
		free(m_ptrs[i]);
}

//////////////////////////////////////////////////////////////////////////
AutoHProv::operator HCRYPTPROV()
{
    return hprov;
}
AutoHProv::operator HCRYPTPROV*()
{
    return &hprov;
}
AutoHProv::AutoHProv()
{
    hprov = NULL;
}
AutoHProv::~AutoHProv()
{
    if (hprov) 
        CryptReleaseContext(hprov, 0);
}

//////////////////////////////////////////////////////////////////////////
AutoHHash::operator HCRYPTHASH()
{
    return hhash;
}
AutoHHash::operator HCRYPTHASH*()
{
    return &hhash;
}
AutoHHash::AutoHHash()
{
    hhash = NULL;
}
AutoHHash::~AutoHHash()
{
    if (hhash) 
        CryptDestroyHash(hhash);
}

//////////////////////////////////////////////////////////////////////////
AutoHPkey::operator HCRYPTKEY()
{
    return hkey;
}
AutoHPkey::operator HCRYPTKEY*()
{
    return &hkey;
}
AutoHPkey::AutoHPkey()
{
    hkey = NULL;
}
AutoHPkey::~AutoHPkey()
{
    if (hkey) 
        CryptDestroyKey(hkey);
}

//////////////////////////////////////////////////////////////////////////
AutoRegKey::operator HKEY()
{
    return hkey;
}
AutoRegKey::operator HKEY*()
{
    return &hkey;
}
AutoRegKey::AutoRegKey()
{
    hkey = NULL;
}
AutoRegKey::~AutoRegKey()
{
    if (hkey) 
        RegCloseKey(hkey);
}

//////////////////////////////////////////////////////////////////////////

