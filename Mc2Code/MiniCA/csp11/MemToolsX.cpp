// MemTools.cpp: implementation of the CMemTools class.
//
// Description:
// 根据"RAII 资源获取即初始化"的思想，将资源的获取和释放与对象的构造和
// 析构对应起来，从而确保在对象的生存期内资源始终有效，对象销毁时资源必被释放。
//
// notification:
// 注意申请内存的时候要保证内存申请的足够大，一旦发生内存越界，在释放资源
// 的时候，就会发生崩溃或者不可预知的结果。
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//Comment it if not need in vc'precompiled headers;
#include "stdafx.h"  


#include <assert.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <string>
using namespace std;



#include "MemToolsX.h"
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 


//////////////////////////////////////////////////////////////////////////
using namespace MemToolsX;

// CByteBuf::CByteBuf(int ilen)
// {
// 	ptr = 0;   ptr2 = 0;	//this ptr is used in openssl i2d/d2i functions
// 	ptr = (unsigned char *)malloc(ilen);
// 	assert(ptr);
// 	memset(ptr, 0, ilen);
// 	ptr2 = ptr;
// }
// 
// CByteBuf::~CByteBuf()
// {
// 	free(ptr);
// 	ptr = 0;  ptr2 = 0;
// }



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






/*
	string 转换为 wstring 
*/
wstring c2w(const char *pc)
{
	wstring val = L"";

	if(NULL == pc)
	{
		return val;
	}
	//size_t size_of_ch = strlen(pc)*sizeof(char);
	//size_t size_of_wc = get_wchar_size(pc);
	size_t size_of_wc;
	size_t destlen = mbstowcs(0,pc,0);
	if (destlen ==(size_t)(-1))
	{
		return val;
	}
	size_of_wc = destlen+1;
	wchar_t * pw  = new wchar_t[size_of_wc];
	mbstowcs(pw,pc,size_of_wc);
	val = pw;
	delete pw;
	return val;
}
/*
	wstring 转换为 string
*/
string w2c(const wchar_t * pw)
{
	string val = "";
	if(!pw)
	{
   		return val;
	}
	size_t size = wcslen(pw)*sizeof(wchar_t);
	char *pc = NULL;
	if(!(pc = (char*)malloc(size)))
	{
   		return val;
	}
	size_t destlen = wcstombs(pc,pw,size);
	/*转换不为空时，返回值为-1。s如果为空，返回值0*/
	if (destlen ==(size_t)(0))
	{
		return val;
	}
	val = pc;
	delete pc;
	return val;
}

