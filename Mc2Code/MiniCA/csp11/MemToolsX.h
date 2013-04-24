// MemTools.h: interface for the CMemTools class.
//
// Description:
// 根据"RAII"资源获取即初始化"的思想，将资源的获取和释放与对象的构造和
// 析构对应起来，从而确保在对象的生存期内资源始终有效，对象销毁时资源必被释放。
//
// notification:
// 1.注意申请内存的时候要保证内存申请的足够大，一旦发生内存越界，在释放资源
// 的时候，就会发生崩溃或者不可预知的结果。
//
//////////////////////////////////////////////////////////////////////
#if !defined(INCD_MEMTOOLSX_H__8A620B1D_52C9_A4F9_70DD_C75AB6AF2B5F56)
#define INCD_MEMTOOLSX_H__8A620B1D_52C9_A4F9_70DD_C75AB6AF2B5F56

////////////////////////////////////////////////////////////////////////


#include <vector>
#include <string>
using namespace std;

////////////////////////////////////////////////////////////////////////
namespace MemToolsX
{

// 	class  CByteBuf
// 	{
// 	public:
// 		unsigned char* ptr;
// 		unsigned char* ptr2;
// 		CByteBuf(int ilen);
// 		~CByteBuf();
// 	};


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

	wstring c2w(const char *pc);
	string  w2c(const wchar_t * pw);

}







#endif // !defined(INCLUDED_MEMTOOLS_H__8A620B1D_52C9_A4F9_70DD_C75AB6AF2B5F56)
