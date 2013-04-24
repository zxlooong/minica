// Copyright (C) 1991 - 1999 Rational Software Corporation

#ifndef _INC_CBASE64_3D5B1312000E_INCLUDED
#define _INC_CBASE64_3D5B1312000E_INCLUDED

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include <tchar.h>

typedef unsigned char uchar;
typedef unsigned long ulong;

class CBase64 
{
public:
	virtual ~CBase64(){};
	CBase64(){};
public:
	static int Decode(const TCHAR source[], uchar  *des, ulong *len);
	static int Encode(const uchar source[], ulong len1, TCHAR *des, ulong *len);
	static int IsBase64String(const TCHAR *pStr);
	static bool Isbase64char(TCHAR ch);
private:
	static int base64decodingT(const TCHAR source[], uchar  *des, ulong *len);
	static int base64encodingT(const uchar source[], ulong len1, TCHAR *des, ulong *len);
	static int change2binaryT(TCHAR a, TCHAR b, TCHAR c, TCHAR d, uchar  *e);
	static int change2base64T(uchar a, uchar b, uchar c, TCHAR *d, int e);

};

#endif /* _INC_CBASE64_3D5B1312000E_INCLUDED */
