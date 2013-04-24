//	zxloong@hotmail.com 

#include "stdafx.h"
#include <string.h>
#include "CBase64.h"


int CBase64::Decode(const TCHAR source[], uchar  *des, ulong *len)
{
	int ret = base64decodingT(source,des,len);
	if(ret)
		return 0;
	else
		return 1;
}

int CBase64::Encode(const uchar source[], ulong len1, TCHAR *des, ulong *len)
{
	int ret = base64encodingT(source,len1,des,len);
	if(ret)
		return 0;
	else
		return 1;
}



int CBase64::change2base64T(uchar a, uchar b, uchar c, TCHAR *d, int e)
{
	TCHAR change[] = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
	ulong j = 0;
	ulong k, l;

	k = a & 0xfc;
	k >>= 2;
	d[j] = change[k];
	j++;

	k = a & 0x3;
	k <<= 4;
	l = b & 0xf0;
	l >>= 4;
	k |= l;
	d[j] = change[k];
	j++;

	k = b & 0x0f;
	k <<= 2;
	l = c & 0xc0;
	l >>= 6;
	k |= l;
	d[j] = change[k];
	j++;

	k = c & 0x3f;
	d[j] = change[k];

	if(e < 3 )
		d[j] = _T('=');
	if(e < 2)
		d[j-1] = _T('=');

	return 1;
}

int CBase64::change2binaryT(TCHAR a, TCHAR b, TCHAR c, TCHAR d, uchar  *e)
{
	TCHAR change[] = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
	unsigned long j = 0;
	unsigned long k, l;

	k = 0;
	while(change[k] != a) k++;
	l = 0;
	while(change[l] != b) l++;
	k <<= 2;
	l >>= 4;
	k |= l;
	e[j++] = (unsigned char )k;
	if(c == _T('='))
	{
		e[j] = _T('\0');
		return 1;
	}

	k = 0;
	while(change[k] != b) k++;
	l = 0;
	while(change[l] != c) l++;
	k &= 0x0f; 
	k <<= 4;
	l >>= 2;
	k |= l;
	e[j++] = (unsigned char )k;
	if(d == _T('='))
	{
		e[j] = _T('\0');
		return 1;
	}

	k = 0;
	while(change[k] != c) k++;
	l = 0;
	while(change[l] != d) l++;
	k &= 0x03;
	k <<= 6;
	k |= l;
	e[j++] = (unsigned char )k;
	
	e[j] = _T('\0');
	return 1;
}

int CBase64::base64encodingT(const uchar source[], ulong len1, TCHAR *des, ulong *len)
{
	if(len1*4 > *len*3)
	{
		*len = 0;
		return 0;
	}

	unsigned long i, count;
	uchar  fill1,fill2;

	fill1 = 0x0;
	fill2 = 0x0;

	*len = 0;
	count = 0;
	for(i=0;i<len1;)
	{
		if(i+2<len1)
			change2base64T(source[i],source[i+1],source[i+2],&des[*len],3);
		else if(i+1<len1)
			change2base64T(source[i],source[i+1],fill1,&des[*len],2);
		else 
			change2base64T(source[i],fill2,fill1,&des[*len],1);
		*len += 4;
		i += 3;
		count += 4;
	}
	
	des[*len] = _T('\0');
	return 1;
}

int CBase64::base64decodingT(const TCHAR source[], uchar  *des, ulong *len)
{
// 	if(len1*3 > *len*4)
// 	{
// 		*len = 0;
// 		return 0;
// 	}
	
	unsigned long i,j,lab1,lab2;
	unsigned long len1;
	TCHAR  a,b,c,d;
	lab1 = lab2 = 0;
	len1 = _tcslen(source);
	
	*len = 0;
	for(i=0;i<len1;)
	{
		j = 0;
		while( !(  (source[i+j] >= _T('a') && source[i+j] <= _T('z')) 
				|| (source[i+j] >= _T('A') && source[i+j] <= _T('Z')) 
				|| (source[i+j] >= _T('0') && source[i+j] <= _T('9')) 
				|| (source[i+j] == _T('=')) 
				|| (source[i+j] == _T('/')) 
				|| (source[i+j] == _T('+')) ) 
				&& i+j < len1 ) 
			j++;
		if(i+j < len1)
			a = source[i+j];
		else
		{
			if(source[lab2] == _T('='))
			{
				*len -= 1;
				des[*len] = _T('\0');
			}
			if(source[lab1] == _T('='))
			{
				*len -= 1;
				des[*len] = _T('\0');
			}
			return 1;
		}
		j++;
		while( !(  (source[i+j] >= _T('a') && source[i+j] <= _T('z')) 
				|| (source[i+j] >= _T('A') && source[i+j] <= _T('Z')) 
				|| (source[i+j] >= _T('0') && source[i+j] <= _T('9')) 
				|| (source[i+j] == _T('=')) 
				|| (source[i+j] == _T('/')) 
				|| (source[i+j] == _T('+')) ) 
				&& i+j < len1 ) 
			j++;
		if(i+j < len1)
			b = source[i+j];
		else
			b = _T('=');
		j++;
		while( !(  (source[i+j] >= _T('a') && source[i+j] <= _T('z')) 
				|| (source[i+j] >= _T('A') && source[i+j] <= _T('Z')) 
				|| (source[i+j] >= _T('0') && source[i+j] <= _T('9')) 
				|| (source[i+j] == _T('=')) 
				|| (source[i+j] == _T('/')) 
				|| (source[i+j] == _T('+')) ) 
				&& i+j < len1 ) 
			j++;
		if(i+j < len1)
			c = source[i+j];
		else
			c = _T('=');
		lab1 = i + j;
		j++;
		while( !(  (source[i+j] >= _T('a') && source[i+j] <= _T('z')) 
				|| (source[i+j] >= _T('A') && source[i+j] <= _T('Z')) 
				|| (source[i+j] >= _T('0') && source[i+j] <= _T('9')) 
				|| (source[i+j] == _T('=')) 
				|| (source[i+j] == _T('/')) 
				|| (source[i+j] == _T('+')) ) 
				&& i+j < len1 ) 
			j++;
		if(i+j < len1)
			d = source[i+j];
		else
			d = _T('=');
		lab2 = i + j;
		j++;
		
		change2binaryT(a,b,c,d,&des[*len]);
		*len += 3;
		i += j;
	}
	
	if(source[lab2] == _T('='))
	{
		*len -= 1;
		des[*len] = _T('\0');
	}
	if(source[lab1] == _T('='))
	{
		*len -= 1;
		des[*len] = _T('\0');
	}
	
	return 1;
}


int CBase64::IsBase64String(const TCHAR *pStr)
{
	if(NULL == *pStr || NULL == pStr)
		return -1;
	int iLen = _tcslen(pStr);
	if(iLen % 4 != 0)
		return -2;

	for(int i = 0; i < iLen; i++)
	{
		if(!Isbase64char(pStr[i]))
		{
			if(pStr[i] ==_T('='))
			{
				if(i == iLen-1)
				{
					return 0;
				}
				else if((i == iLen-2)&&(pStr[iLen-1] ==_T('=')))
				{
					return 0;
				}
				else
					return i+1;
			}
		}
	}
	return 0;
}
bool CBase64::Isbase64char(TCHAR ch)
{
	if(ch >= _T('A')&&ch <= _T('Z'))
		return true;
	if(ch >= _T('a')&&ch <= _T('z'))
		return true;
	if(ch >= _T('0')&&ch <= _T('9'))
		return true;
	if(ch == _T('+')||ch ==_T('/'))
		return true;
	return false;
}
