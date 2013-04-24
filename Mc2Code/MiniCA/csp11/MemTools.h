// MemTools.cpp: implementation of the CMemTools class.
//////////////////////////////////////////////////////////////////////////////// 
// Description:
// ����"RAII ��Դ��ȡ����ʼ��"��˼�룬����Դ�Ļ�ȡ���ͷ������Ĺ����
// ������Ӧ�������Ӷ�ȷ���ڶ��������������Դʼ����Ч����������ʱ��Դ�ر��ͷš�
//
// notification:
// ע�������ڴ��ʱ��Ҫ��֤�ڴ�������㹻��һ�������ڴ�Խ�磬���ͷ���Դ
// ��ʱ�򣬾ͻᷢ���������߲���Ԥ֪�Ľ����
// ע��ʹ�õ�ʱ��ʹ�������ռ�: using namespace MemTools;
//
////////////////////////////////////////////////////////////////////////////////
//
//   Author      : zilla.dragon
//   Data        : 2011-07-26
//   Tab width   : 4
//   Platform    : Win32
//
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMTOOLS_H__C3BE95F2_81F0_4B34_9F2C_EC210DACBFAC__INCLUDED_)
#define AFX_MEMTOOLS_H__C3BE95F2_81F0_4B34_9F2C_EC210DACBFAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <WinCrypt.h>

////////////////////////////////////////////////////////////////////////
namespace MemTools
{
	class  CByteBuf
	{
	public:
		unsigned int len;
		unsigned char * ptr;
		unsigned char * ptr2;
		CByteBuf();
        CByteBuf(int ilen);
		~CByteBuf();
	};

	class  CTCharBuf
	{
	public:
		TCHAR *ptr;
		CTCharBuf(int len);
		~CTCharBuf();
	};

	class  CW2ACharBuf
	{
	public:
		char *ptr;
		CW2ACharBuf(const wchar_t* lp);
		~CW2ACharBuf();
	};

	class CW2TCharBuf
	{
	public:
		TCHAR* ptr;
		CW2TCharBuf(const wchar_t* lp);
		~CW2TCharBuf();
	};

	class  CA2WCharBuf
	{
	public:
		WCHAR *ptr;
		CA2WCharBuf(const char* lp);
		~CA2WCharBuf();
	};	

	class  CA2TCharBuf
	{
	public:
		TCHAR *ptr;
		CA2TCharBuf(const char* lp);
		~CA2TCharBuf();
	};

	class  CT2WCharBuf
	{
	public:
		wchar_t *ptr;
		CT2WCharBuf(const TCHAR* lp);
		~CT2WCharBuf();
	};

	class  CT2ACharBuf
	{
	public:
		char *ptr;
		CT2ACharBuf(const TCHAR* lp);
		~CT2ACharBuf();
	};

    class  CUT82TCharBuf
    {
    public:
        TCHAR *ptr;
        CUT82TCharBuf(const char* lp);
        ~CUT82TCharBuf();
    };

    class  CT2UTF8CharBuf
    {
    public:
        char *ptr;
        CT2UTF8CharBuf(const TCHAR* lp);
        ~CT2UTF8CharBuf();
    };
	//////////////////////////////////////////////////////////////////////////
	class ReleaseKeys
	{
		std::vector<HKEY> m_keys;
	public:
		ReleaseKeys & add(HKEY hkey);
		~ReleaseKeys();
	};


	class ReleasePtrs
	{
		std::vector<byte*> m_ptrs;
	public:
		ReleasePtrs& add(byte* ptr);
		~ReleasePtrs();
	};

	//////////////////////////////////////////////////////////////////////////
    class AutoHProv
    {
    public:
        HCRYPTPROV hprov;
        operator HCRYPTPROV();
        operator HCRYPTPROV*();
        AutoHProv();
        ~AutoHProv();
    };

    class AutoHHash
    {
    public:
        HCRYPTHASH hhash;
        operator HCRYPTHASH();
        operator HCRYPTHASH*();
        AutoHHash();
        ~AutoHHash();
    };

    class AutoHPkey
    {
    public:
        HCRYPTKEY hkey;
        operator HCRYPTKEY();
        operator HCRYPTKEY*();
        AutoHPkey();
        ~AutoHPkey();
    };

    class AutoRegKey
    {
    public:
        HKEY hkey;
        operator HKEY();
        operator HKEY*();
        AutoRegKey();
        ~AutoRegKey();
    };
    
}
















#endif // !defined(AFX_MEMTOOLS_H__C3BE95F2_81F0_4B34_9F2C_EC210DACBFAC__INCLUDED_)
