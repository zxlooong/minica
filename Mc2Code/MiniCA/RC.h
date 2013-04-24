// RC.h: interface for the CRC class.
//
//////////////////////////////////////////////////////////////////////
/* crc-16 is based on the polynomial x^16+x^15+x^2+1 */
/*  The data is assumed to be fed in from least to most significant bit */
/* crc-ccitt is based on the polynomial x^16+x^12+x^5+1 */
/*  The data is fed in from most to least significant bit */
/* The prescription for determining the mask to use for a given polynomial
	is as follows:
		1.  Represent the polynomial by a 17-bit number
		2.  Assume that the most and least significant bits are 1
		3.  Place the right 16 bits into an integer
		4.  Bit reverse if serial LSB's are sent first
*/
#if !defined(AFX_RC_H__C48B6156_A568_4FE0_B9B7_67B4315FE781__INCLUDED_)
#define AFX_RC_H__C48B6156_A568_4FE0_B9B7_67B4315FE781__INCLUDED_

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#define		M16	0xA001		/* crc-16 mask */
#define		MTT	0x1021		/* crc-ccitt mask */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRC  
{
public:
	CRC()
	{
	}
	static BOOL GetMemCrc(CMemFile & MemFile, unsigned int & crc16)
	{
		crc16 = 0;
		int ch = 0;
		while(MemFile.Read(&ch, sizeof(char)) != -1)
		{
			crc16 = updcrcr(crc16,ch,M16);
		}
		return TRUE;
	}
	static BOOL GetCrc16(char * buf,int buflen,unsigned int & crc16)
	{
		crc16 = 0;
		int ch = 0;
		if(buflen == 0)//文件
		{
			FILE * fp = NULL;
			if((fp=fopen(buf,"rb"))==NULL)
			{
				return FALSE;
			}
			while((ch=fgetc(fp))!=EOF)
			{
				crc16 = updcrcr(crc16,ch,M16);
				//crctt=updcrc(crctt,ch,MTT);
			}
			fclose(fp);
			return TRUE;
		}
		else//内存
		{
			for(int i=0;i<buflen;i++)
			{
				crc16 = updcrcr(crc16,buf[i],M16);
			}
			return TRUE;
		}
	}
	//校验CRC值
	static BOOL CheckCrc(char * buf,int buflen,unsigned int crc16)
	{
		UINT uCrc = 0;
		if(GetCrc16(buf, buflen, uCrc))
		{
			return (uCrc == crc16)?TRUE:FALSE;
		}
		else
			return FALSE;
	}
	virtual ~CRC()
	{
	}
private:
	/* update crc */
	static unsigned int updcrc(unsigned int crc,int c,unsigned int mask)
	{
		int i;
		c<<=8;
		for(i=0;i<8;i++)
		{
			if((crc ^ c) & 0x8000) crc=(crc<<1)^mask;
			else crc<<=1;
			c<<=1;
		}
		return crc;
	}
	
	/* update crc16 reverse */
	static unsigned int updcrcr(unsigned int crc,int c,unsigned int mask)
	{
		int i;
		for(i=0;i<8;i++)
		{
			if((crc ^ c) & 1) crc=(crc>>1)^mask;
			else crc>>=1;
			c>>=1;
		}
		return crc;
	}
	
};
#endif // !defined(AFX_RC_H__C48B6156_A568_4FE0_B9B7_67B4315FE781__INCLUDED_)
