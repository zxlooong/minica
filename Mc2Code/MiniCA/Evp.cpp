// Evp.cpp: implementation of the CEvp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "minica.h"
#include "Evp.h"
#include <io.h>
#include "CertKey.h"
#include "rc.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CProgressCtrlST *  CEvp::m_pProgress = NULL;
const UINT BUFLEN = 1024;
const float HUNDRED = 100.0;
CEvp::CEvp()
{

}

CEvp::~CEvp()
{

}

//函数功能:对称加解密
//参数:		[in] cpName - 算法名称
//			[in] inStream - 输入文件或内存区域
//			[in] inLen - 输入长度,0->标示输入为文件
//			[in] outStream - 输出为文件名或内存区域
//			[in,out] outLen - 输入为结果内存长度,输出为实际加密结果长度
//			[in] char * pwd - 用于初始iv的密码
//			[in] type - 1-加密、0-解密
//			[out] outMsg - 输出信息
BOOL CEvp::Crypt(const char * cpName, const char * inStream, UINT inLen, 
		   char * outStream, UINT & outLen, char * pwd, const int type,
		   char * outMsg)
{
	unsigned char inbuf[BUFLEN]="";
	unsigned char outbuf[BUFLEN + EVP_MAX_BLOCK_LENGTH]="";//所有算法最长的块长度。
	unsigned char key[EVP_MAX_KEY_LENGTH]="";//算法最长的KEY长度
    unsigned char iv[EVP_MAX_IV_LENGTH]="";//算法最长的IV长度
	int infilelen=0, outlen=0;
	BOOL ret=true;
	const EVP_CIPHER *cipher=NULL;
	EVP_CIPHER_CTX ctx;
	memset(&ctx,0,sizeof(ctx));
	long fileLen=0;//文件长度
	long finishLen=0;//完成长度
	int uMaxMem = outLen;	//可能为负数,不能用UINT
	outLen = 0;
	UINT len = 0;

	FILE * outfd=NULL,* infd=NULL;
	
	if(inLen==0&&(strlen(inStream)==0||strlen(outStream)==0))
	{
		strcpy(outMsg,"NO specify input or output file");
		return FALSE;
	}
	
	if(inLen==0)//文件
	{
		if ((infd = fopen (inStream, "rb")) == NULL)//原文
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//得到文件长度
		if ((outfd = fopen (outStream, "wb")) == NULL)//密文
		{
			strcpy(outMsg,"open output file error");
			fclose(infd);
			return FALSE;
		}
	}
	
	OpenSSL_add_all_algorithms();//add digests   and ciphers
	cipher=EVP_get_cipherbyname(cpName);
	if(cipher==NULL)
	{
		sprintf(outMsg,"Unknown cipher name %s\n",cpName);
		ret=FALSE;
		goto err;
	}
	
	//表示產生的 key 是給哪一種 cipher 用的
	//過程中所使用的 hash 演算法,
	//用來加密的 salt，至少為八個 bytes，否則必須NULL,
	//重覆幾次 hashing 的動作，愈多次愈花時間，但相對地也愈安全
	if(!EVP_BytesToKey(cipher,EVP_md5(),NULL,(unsigned char *)pwd,
		strlen(pwd),1,key,iv))
	{
		strcpy(outMsg,"Crypt初始化key or iv 失败");
		ret=FALSE;
		goto err;
	}
	
	EVP_CIPHER_CTX_init(&ctx);//初始化一个EVP_CIPHER_CTX结构体
	
	//type为1，则加密；如果type为0，则解密；如果type是－1，则不改变数据
	if(!EVP_CipherInit_ex(&ctx, cipher, NULL/*NULL使用缺省的实现算法*/,key,iv,type))//初始化
	{
		strcpy(outMsg,"Crypt初始化加解密结构体失败");
		ret=FALSE;
		goto err;		
	}
	
	//该函数进行加密算法结构EVP_CIPHER_CTX密钥长度的设置。
	//如果算法是一个密钥长度固定的算法，
	//那么如果设置的密钥长度跟它固定的长度不一致，就会产生错误。
	//	EVP_CIPHER_CTX_set_key_length(&ctx, 10); 		
	//	EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, type);
	if(inLen==0)//文件
	{
		for(;;)
		{                       
			infilelen = fread(inbuf, sizeof(char), BUFLEN, infd);
			if(infilelen <= 0) break;
			if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, infilelen))//中間過程 
			{
				strcpy(outMsg,"Crypt中间过程错误");
				ret=FALSE;
				goto err;
			}
			fwrite(outbuf, sizeof(char), outlen, outfd);
			finishLen+=infilelen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}               
		if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))    //最終步驟-处理最后（Final）的一段数据
		{                       
			strcpy(outMsg,"Crypt最终过程错误"); 
			ret=FALSE;
			goto err;
		}               
		fwrite(outbuf, sizeof(char), outlen, outfd);
	}
	else//内存
	{
		fileLen = inLen;
		
		for(UINT i=0;;i++)
		{
			//每次1024
			len=(inLen>BUFLEN)?BUFLEN:inLen;
			inLen-=len;
			if(!EVP_CipherUpdate(&ctx, outbuf, &outlen,(UCHAR *)(inStream+i*BUFLEN), len))//中間過程
			{
				strcpy(outMsg,"Crypt中间过程错误");
				ret=FALSE;
				goto err;
			}
			//计算缓冲区长度是否能容纳结果数据集
			uMaxMem -= outlen;
			if(uMaxMem < 0)
			{
				strcpy(outMsg,"输出缓冲过小,不能容纳操作结果"); 
				ret = FALSE;
				goto err;
			}

			memcpy(outStream + outLen, outbuf, outlen);//拷贝到调用函数
			outLen+=outlen;
			if(inLen <= 0) break;          
		}		

		if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)) //最終步驟-处理最后（Final）的一段数据
		{   
			strcpy(outMsg,"Crypt最终过程错误"); 
			ret=FALSE;
			goto err;
		}               
	
		uMaxMem -= outlen;
		if(uMaxMem < 0)
		{
			strcpy(outMsg,"输出缓冲过小,不能容纳操作结果"); 
			ret = FALSE;
			goto err;
		}
		memcpy(outStream + outLen,outbuf,outlen);//拷贝到调用函数		
		outLen += outlen;
		
	}
err:
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	if(ctx.cipher) EVP_CIPHER_CTX_cleanup(&ctx); //清除所有信息释放内存
	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
	return ret;
}

/*消息摘要*/ 
BOOL CEvp::Digest(const char * mdname/*摘要算法*/,const char * infoin/*输入,文件或内存*/,
			UINT inlen/*文件时候为0,内存为长度*/,unsigned char * md_value/*返回摘要*/,
			unsigned int * md_len/*摘要长度*/, char * outMsg)
{

const EVP_MD *md=NULL;
	char inbuf[BUFLEN]="";
	UINT len=0;
	BOOL ret=true;
	EVP_MD_CTX md_ctx;
	memset(&md_ctx,0,sizeof(md_ctx));
	FILE * infd=NULL;
	long fileLen=0;//文件长度
	long finishLen=0;//完成长度
	
	if(strlen(infoin)==0)
	{
		strcpy(outMsg,"NO specify input");
		return FALSE;
	}
	
	if(inlen==0)//输入为文件
	{
		if ((infd = fopen (infoin, "rb")) == NULL)//原文
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		fileLen=filelength(fileno(infd));//得到文件长度
	}

	
	//使EVP_Digest系列函数支持所有有效的信息摘要算法
	OpenSSL_add_all_digests();
    
	//根据输入的信息摘要函数的名字得到相应的EVP_MD算法结构
	md = EVP_get_digestbyname(mdname);  
	if(!md) 
	{
		sprintf(outMsg,"Unknown message digest %s",mdname);
		ret=FALSE;
		goto err;
	}
	//初始化信息摘要结构md_ctx。
	EVP_MD_CTX_init(&md_ctx);
	
	//使用md的算法结构设置mdctx结构，impl为NULL，即使用缺省实现的算法（openssl本身提供的信息摘要算法）
	if(!EVP_DigestInit_ex(&md_ctx, md, NULL/*impl*/))
	{
		strcpy(outMsg,"Digest初始化算法结构错误");
		ret=FALSE;
		goto err;
	}
	
	//信息摘要运算
	if(inlen==0)//输入为文件
	{
		for(;;)
		{
			len = fread(inbuf, 1, BUFLEN, infd);
			if(len <= 0) break;          
			if(!EVP_DigestUpdate(&md_ctx, inbuf, len))
			{
				strcpy(outMsg,"Digest中间过程错误");
				ret=FALSE;
				goto err;
			}
			finishLen+=len;
			DrawProg(finishLen * HUNDRED / fileLen);
		}
	}
	else//内存区域
	{
		for(UINT i=0;;i++)
		{
			//每次BUFLEN
			len=(inlen>BUFLEN)?BUFLEN:inlen;
			inlen-=len;
			if(!EVP_DigestUpdate(&md_ctx, (infoin+i*BUFLEN), len))
			{
				strcpy(outMsg,"Digest中间过程错误");
				ret=FALSE;
				goto err;
			}
			if(inlen <= 0) break;          
		}		
	}
	//完成信息摘要计算过程，将完成的摘要信息存储在md_value里面,长度信息存储在md_len里面
	if(!EVP_DigestFinal_ex(&md_ctx, md_value, md_len))
	{	
		strcpy(outMsg,"Digest最终过程错误");
		ret=FALSE;
	}
	
err:
	//使用该函数释放mdctx占用的资源，如果使用_ex系列函数，这是必须调用的。
	if(infd!=NULL)
		fclose(infd);
	if(md_ctx.digest) EVP_MD_CTX_cleanup(&md_ctx);
	EVP_cleanup();
	return ret;
}

/*数字签名*/
BOOL CEvp::Sign(const char * key/*私钥*/,const int keylen/*0-内存私钥*/,const char * pwd,
				const char * mdname/*签名算法*/,const char * infoin/*输入文件*/,
				int inlen,/*输入长度,0->内存*/char * infout/*输出*/,
				UINT & outlen/*输出长度,输入文件时=0*/,char * outMsg)
{
	BOOL ret=true;
	unsigned char * sig_buf=NULL;
	unsigned int sig_len,len=0;
	unsigned char inbuf[BUFLEN]="";
	EVP_PKEY * pkey=NULL;
	const EVP_MD *md=NULL;
	EVP_MD_CTX md_ctx;
	memset(&md_ctx,0,sizeof(md_ctx));
	BIO *in=NULL;
	FILE * outfd=NULL,* infd=NULL;
	long fileLen=0;//文件长度
	long finishLen=0;//完成长度

	BOOL bType=inlen;

	if(bType==0&&(strlen(infoin)==0||strlen(infout)==0))//输入文件时候
	{
		strcpy(outMsg,"NO specify input or output file");
		return FALSE;
	}
	if(bType==0)//文件
	{
		if ((infd = fopen (infoin, "rb")) == NULL)//原文
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		if ((outfd = fopen (infout, "wb")) == NULL)//密文
		{
			strcpy(outMsg,"open output file error");
			fclose(infd);
			return FALSE;
		}
		fileLen=filelength(fileno(infd));//得到文件长度
	}
	
	OpenSSL_add_all_digests();
	
	pkey = CCertKey::LoadKey((char *)key,keylen,(char *)pwd,outMsg);
	if (pkey == NULL)
	{ 
		ret=FALSE;
		goto err;
	}
	
	/* Do the signature */
	md = EVP_get_digestbyname(mdname);
	if(!md)
	{
		sprintf(outMsg,"Unknown message digest %s",mdname);
		ret=FALSE;
		goto err;
	}
	EVP_MD_CTX_init(&md_ctx);
	if(!EVP_SignInit_ex(&md_ctx,md,NULL))
	{
		strcpy(outMsg,"初始化算法结构出错");
		ret=FALSE;
		goto err;
	}

	if(bType==0)//文件
	{	
		for(;;)
		{
			len = fread(inbuf, sizeof(char), BUFLEN, infd);
			if(len <= 0) break;
			if(!EVP_SignUpdate(&md_ctx, inbuf, len))
			{
				sprintf(outMsg,"中间过程出错");
				ret=FALSE;
				goto err;
			}
			finishLen+=len;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
	}

	else//内存区域
	{
		for(UINT i=0;;i++)
		{
			//每次BUFLEN
			len=(inlen>BUFLEN)?BUFLEN:inlen;
			inlen-=len;
			if(!EVP_SignUpdate(&md_ctx, (infoin+i*BUFLEN), len))
			{
				strcpy(outMsg,"中间过程错误");
				ret=FALSE;
				goto err;
			}
			if(inlen <= 0) break;          
		}		
	}

	sig_len =EVP_PKEY_size(pkey);
	sig_buf=new unsigned char[sig_len];
	
	// 簽名後的結果，必須是個夠大視 private key 長度不同而不同，可以 EVP_PKEY_size() 取得
	if(!EVP_SignFinal (&md_ctx, sig_buf, &sig_len, pkey))
	{
		sprintf(outMsg,"最终过程出错");
		ret=FALSE;
	}
	else if(bType!=0)//内存
	{
		memcpy(infout,sig_buf,sig_len);
		outlen=sig_len;
	}

	if(outfd!=NULL)
	{
		fwrite(sig_buf,sizeof(char),sig_len,outfd);
		outlen=0;
	}
	
err:
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	BIO_free(in);
	
	if(pkey) EVP_PKEY_free (pkey);
	if(md_ctx.digest) EVP_MD_CTX_cleanup(&md_ctx);
	delete [] sig_buf;
	EVP_cleanup();
	return ret;
}

/*数字签名验证*/
BOOL CEvp::VerifySign(const char * cert/*公钥*/,const int certlen,
					  const char * pwd,const char * mdname/*签名算法*/,
					  const char * infoin,/*原始信息*/ 
					  int inlen/*输入长度0->文件*/,char * sign/*签名结果*/,
					  char * outMsg)
{
	const EVP_MD *md=NULL;
	unsigned char inbuf[BUFLEN]="";
	BOOL ret=true;
	unsigned char * sig_buf=NULL;
	unsigned int sig_len,len=0;
	FILE * outfd=NULL,* infd=NULL;
	long fileLen=0;//文件长度
	long finishLen=0;//完成长度
	X509 *	x509=NULL;
	EVP_PKEY * pcert=NULL;
	EVP_MD_CTX md_ctx;
	memset(&md_ctx,0,sizeof(md_ctx));
	
	BOOL bType=inlen;

	if(bType==0&&(strlen(infoin)==0||strlen(sign)==0))//输入文件时候
	{
		strcpy(outMsg,"NO specify input file");
		return FALSE;
	}
	if(bType==0)//内存
	{
		if ((infd = fopen (infoin, "rb")) == NULL)//原文
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		if ((outfd = fopen (sign, "rb")) == NULL)//密文
		{
			strcpy(outMsg,"open output file error");
			fclose(infd);
			return FALSE;
		}
		fileLen=filelength(fileno(infd));//得到文件长度
	}

	OpenSSL_add_all_digests();
	x509 = CCertKey::LoadCert((char *)cert,certlen,(char *)pwd,outMsg);
	if (x509 == NULL)
	{
		ret=FALSE;
		goto err;
	}
	pcert=X509_get_pubkey(x509);
	if (pcert == NULL)
	{
		sprintf(outMsg,"Read Public Key Failed!");
		ret=FALSE;
		goto err;
	}
	
	md = EVP_get_digestbyname(mdname);  
	if(!md) 
	{
		sprintf(outMsg,"Unknown message digest %s",mdname);
		ret=FALSE;
		goto err;
	}
	
	EVP_MD_CTX_init(&md_ctx);	
	if(!EVP_VerifyInit_ex(&md_ctx,md,NULL))
	{
		strcpy(outMsg,"初始化算法结构出错");
		ret=FALSE;
		goto err;
	}

	if(bType==0)//文件
	{		
		for(;;)
		{
			len = fread(inbuf, sizeof(char), BUFLEN, infd);                  
			if(len <= 0) break;
			if(!EVP_VerifyUpdate (&md_ctx, inbuf, len))
			{
				strcpy(outMsg,"中间过程出错");
				ret=FALSE;
				goto err;
			}
			finishLen+=len;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
	}	

	else//内存区域
	{
		for(UINT i=0;;i++)
		{
			//每次BUFLEN
			len=(inlen>BUFLEN)?BUFLEN:inlen;
			inlen-=len;
			if(!EVP_VerifyUpdate(&md_ctx, (infoin+i*BUFLEN), len))
			{
				strcpy(outMsg,"中间过程错误");
				ret=FALSE;
				goto err;
			}
			if(inlen <= 0) break;          
		}		
	}

	sig_len =EVP_PKEY_size(pcert);//这里应该改动
	sig_buf=new unsigned char[sig_len];
	if(bType==0)//文件
	{		
		fread(sig_buf,sizeof(char),sig_len,outfd);
	}
	else
	{
		memcpy(sig_buf,sign,sig_len);
	}
	if(!EVP_VerifyFinal (&md_ctx, sig_buf, sig_len, pcert))
	{
		strcpy(outMsg,"最终过程出错");
		ret=FALSE;
	}
err:
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	if(pcert) EVP_PKEY_free (pcert);
	if(x509) X509_free(x509);
	if(md_ctx.digest) EVP_MD_CTX_cleanup(&md_ctx);
	delete [] sig_buf;
	EVP_cleanup();
	return ret;
}


/*数字信封 - 多证书 */
//文件结构
//			加密后重要信息长度	DWORD
//			加密后重要信息长度 CRC校验码	UINT
//			加密后重要信息
//			-------------------------------重要信息开始--------------------------------
//			公钥个数n
//			n个
//				公钥加密的对称密钥长度
//				公钥加密的对称密钥内容
//			向量IV
//			-------------------------------重要信息结束--------------------------------
//			加密文件内容

BOOL CEvp::Seal(stuCertLink * pCERT/*[in]合法用户公钥链*/,
		  char * cpname/*算法名称*/,
		  char * filein/*输入文件*/, 
		  char * fileout/*输出文件*/,
		  char * outMsg)//写信
{
	//对随机数播种（seeded）。
	UCHAR iv[EVP_MAX_IV_LENGTH]="";
	UCHAR * ekey [128]; 
	EVP_PKEY * pubKey[128];
	int ekeylen[128];
	UCHAR buf[BUFLEN]="";
	UCHAR ebuf[BUFLEN + EVP_MAX_BLOCK_LENGTH]="";
	int readlen=0;
	int ebuflen = 0;
	EVP_CIPHER_CTX eCtx;
	memset(&eCtx,0,sizeof(eCtx));
	int index = 0;

	const EVP_CIPHER *cipher=NULL;
	BOOL ret=true;
	FILE *outfd,*infd;
 	DWORD fileLen=0;//文件长度
	DWORD finishLen=0;//完成长度
	UINT uCrc16 = 0; //加密后重要信息CRC码
	DWORD dOutLen = 0;//内存文件长度
	BYTE * pOut = NULL; //内存文件缓冲区
	DWORD dFileInfoLen = 0;

	if(strlen(filein)==0||strlen(fileout)==0)
	{
		strcpy(outMsg,"NO specify input or output file");
		return false;
	}

	if ((infd = fopen (filein, "rb")) == NULL)//原文
	{
		strcpy(outMsg,"open input file error");
		return false;
	}
	if ((outfd = fopen (fileout, "wb")) == NULL)//密文
	{
		strcpy(outMsg,"open output file error");
		fclose(infd);
		return false;
	}

	//加载加密公钥
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_CERT),"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert = LoadResource(NULL,hRsrc);
	LPSTR lpCert = (LPSTR)LockResource(hgCert);	


	CMemFile MemFile;				//用于存放重要信息的内存文件

	DWORD dInLen = 0;
	BYTE * pIn = NULL;				//用于MemFile.Dectch();


	fileLen = filelength(fileno(infd));//得到文件长度

	OpenSSL_add_all_algorithms();

	int nCountCert = 0;	//证书个数

	for(stuCertLink * temp = pCERT; temp != NULL; temp=temp->Link, nCountCert++)
	{
		X509 * pX509 = NULL;
		pX509 = CCertKey::LoadCert(temp->pCert, temp->CertLen, NULL, outMsg);
	    if(pX509)
		{
			pubKey[nCountCert] = X509_get_pubkey(pX509);
			if (pubKey[nCountCert] == NULL)
			{
				sprintf(outMsg,"Read Public Key Failed!");
				ret=false;
				goto err;
			}
			ekey[nCountCert] = new unsigned char[EVP_PKEY_size(pubKey[nCountCert])];
			X509_free(pX509);
		}
		else
		{
			sprintf(outMsg,"加载第%d个证书失败",nCountCert);
			ret = false;
			goto err;
		}
	}
	
	cipher = EVP_get_cipherbyname(cpname);
	if(cipher==NULL)
	{
		sprintf(outMsg,"Unknown cipher name %s\n",cpname);
		ret=false;
		goto err;
	}

	if(!EVP_SealInit(&eCtx,cipher,
		ekey,
		ekeylen,//存放所有 ek 長度
		iv,//自动生成
		pubKey,//加密 ek 用的 public key(s)
		nCountCert))
	{
		ret=false;
		goto err;
	}

	MemFile.Write(&nCountCert, sizeof(int));	//1 - 写入公钥个数

	//fwrite((char*)&nCountCert,sizeof(char), sizeof(int),outfd);//写入公钥个数
	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Write(&ekeylen[index], sizeof(int));
	//	fwrite(&ekeylen[index],sizeof(char), sizeof(int),outfd);//写入n个经公钥加密的对称密钥长度
		MemFile.Write(ekey[index], ekeylen[index]);
	//	fwrite(ekey[index],sizeof(char) ,ekeylen[index],outfd);//写入n个经公钥加密的对称密钥
	}

	MemFile.Write(iv, sizeof(iv));
//	fwrite(iv,sizeof(char),sizeof(iv),outfd);//写入向量


	//构造加密结果 长度+1 内存
	dFileInfoLen = CEvp::GetEncLen(lpCert, lenCert, MemFile.GetLength());

	dOutLen = dFileInfoLen + 1;
	pOut = new BYTE[dOutLen];

	dInLen = MemFile.GetLength();
	pIn = MemFile.Detach();

	//加密重要信息
	if(!CEvp::RSAPubEnc((BYTE *)lpCert, lenCert, "", pIn, 
		dInLen, pOut, dOutLen, outMsg))
	{
		//加密失败
		ret=false;
		goto err;
	}

	//得到加密后长度的CRC校验
	if(!CRC::GetCrc16((char*)&dOutLen, sizeof(DWORD), uCrc16))
	{
		//加密失败
		ret=false;
		goto err;
	}

	//写入加密后重要信息长度和CRC校验和加密后重要信息内容
	fwrite(&dOutLen,sizeof(char),sizeof(DWORD),outfd);

	fwrite(&uCrc16,sizeof(char),sizeof(UINT),outfd);

	fwrite(pOut,sizeof(char),dOutLen,outfd);



	for(;;)
	{
		readlen = fread(buf, sizeof(char),sizeof(buf),infd);
		
		if (readlen <= 0)
		{
			break;
		}
		
		if(!EVP_SealUpdate(&eCtx, ebuf, &ebuflen, buf, readlen))
		{
			strcpy(outMsg,"中间过程出错");
			ret=false;
			goto err;
		}
		fwrite(ebuf,sizeof(char),ebuflen,outfd);
		finishLen += readlen;
		DrawProg(finishLen*HUNDRED/fileLen);//前半程

	}
	
	if(!EVP_SealFinal(&eCtx, ebuf, &ebuflen))
	{
		strcpy(outMsg,"最终过程出错");
		ret=false;
		goto err;
	}
	
	fwrite(ebuf, sizeof(char), ebuflen,outfd);

err:
	fclose(infd);
	fclose(outfd);
	
	free(pIn);

	delete [] pOut;
	
	for(int i = 0;i< nCountCert;i++)
	{
		if(pubKey[i]) EVP_PKEY_free(pubKey[i]);
		if(ekey[i]) delete [] ekey[i];
	}
	if(eCtx.cipher) EVP_CIPHER_CTX_cleanup(&eCtx);
	EVP_cleanup();
	return ret;
}

/*拆封数字信封*/
BOOL CEvp::OpenSeal(char * key/*私钥*/, int keylen, char * pwd/*私钥密码*/, 
			   char * cpname/*算法名称*/, char * filein/*输入文件*/, 
			   char * fileout/*输出文件*/,char * outMsg)
{
	unsigned char buf[BUFLEN]="";
	unsigned char ebuf[BUFLEN + EVP_MAX_BLOCK_LENGTH]="";
	unsigned char iv[EVP_MAX_IV_LENGTH]="";
	const EVP_CIPHER *cipher=NULL;
	int ebuflen=0,readlen=0;
	bool ret=true;
	FILE *outfd,*infd;
	DWORD fileLen=0;//文件长度
	DWORD finishLen=0;//完成长度
	int nCountCert = 0;	//公钥个数
	int index = 0;
	UCHAR * ekey [128] = {0}; 
	int ekeylen[128];
	BOOL bInit = FALSE;//是否初始化成功
	DWORD dFileInfoLen = 0;//加密后重要信息长度
	UINT uCrc16 = 0; //加密后重要信息CRC码


	EVP_CIPHER_CTX eCtx;
	memset(&eCtx,0,sizeof(eCtx));
	EVP_PKEY * pkey=NULL;

	if(strlen(filein)==0||strlen(fileout)==0)
	{
		strcpy(outMsg,"NO specify input or output file");
		return false;
	}
	
    if ((infd = fopen (filein, "rb")) == NULL)//原文
	{
		strcpy(outMsg,"open input file error");
		return false;
	}
	if ((outfd = fopen (fileout, "wb")) == NULL)//密文
	{
		strcpy(outMsg,"open output file error");
		fclose(infd);
		return false;
	}
	fileLen = filelength(fileno(infd));//得到文件长度

	OpenSSL_add_all_algorithms();//digests   and ciphers
	/* Read private key */
	pkey = CCertKey::LoadKey(key,keylen,pwd,outMsg);
	if (pkey == NULL)
	{ 
		fclose(infd);
		fclose(outfd);
		return FALSE;
	}

	//得到重要信息长度和CRC校验码
	readlen = fread(&dFileInfoLen, sizeof(char),sizeof(DWORD),infd);
	finishLen += readlen;

	readlen = fread(&uCrc16, sizeof(char),sizeof(UINT),infd);
	finishLen += readlen;

	if(!CRC::CheckCrc((char *)&dFileInfoLen, sizeof(DWORD), uCrc16))
	{
		strcpy(outMsg ,"文件头CRC校验错误");
		return FALSE;
	}

	//开辟缓冲区,容纳加密后的重要文件信息
	BYTE * pIn = NULL;
	pIn = new BYTE[dFileInfoLen + 1];
	readlen = fread(pIn, sizeof(char), dFileInfoLen, infd);
	finishLen += readlen;

	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_KEY),"CERT");
	DWORD lenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey = LoadResource(NULL,hRsrc);
	LPSTR lpKey = (LPSTR)LockResource(hgKey);

	DWORD dOutLen = dFileInfoLen;

	//开辟缓冲区,容纳加密后的信息体
	BYTE * pOut = NULL;
	pOut = new BYTE[dOutLen];
	if(!CEvp::RSAPrivDec((BYTE *)lpKey, lenKey, pwd, pIn, 
		dFileInfoLen, pOut, dOutLen, outMsg))
	{
		//解密失败
		strcpy(outMsg ,"重要信息解密失败");
		fclose(infd);
		fclose(outfd);
		delete [] pIn;
		delete [] pOut;
		return FALSE;
	}

	CMemFile MemFile;
	MemFile.Attach(pOut, dOutLen);

	//首先是公钥个数
	MemFile.Read(&nCountCert, sizeof(int));

	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Read(&ekeylen[index], sizeof(int));
		ekey[index] = new unsigned char[ekeylen[index]];
		MemFile.Read(ekey[index], ekeylen[index]);
	}

	MemFile.Read(iv, sizeof(iv));

	MemFile.Close();

	//输入文件已经偏移到加密后的文件
	
	cipher = EVP_get_cipherbyname(cpname);

	if(cipher == NULL)
	{
		sprintf(outMsg,"Unknown cipher name %s\n",cpname);
		ret = false;
		goto err;
	}

	for(index = 0; index<nCountCert; index++)
	{
		if (ekeylen[index] != EVP_PKEY_size(pkey))
		{
			continue;
		}

		if(EVP_OpenInit(&eCtx,cipher, ekey[index],ekeylen[index],iv,pkey))
		{
			bInit = TRUE;
			break;
		}
	}
	if(!bInit)
	{
		sprintf(outMsg,"指定证书不能解密此文件");
		ret=false;
		goto err;
	}
	for(;;)
	{
		readlen = fread(buf, sizeof(char),sizeof(buf),infd);
		if (readlen <= 0)
		{
			break;
		}
		
		if(!EVP_OpenUpdate(&eCtx, ebuf, &ebuflen, buf, readlen))
		{
			sprintf(outMsg,"中间过程错误");
			ret=false;
			goto err;
		}
		fwrite(ebuf,sizeof(char),ebuflen,outfd);
		finishLen += readlen;
		DrawProg(finishLen*HUNDRED/fileLen);
	}

	if(!EVP_OpenFinal(&eCtx, ebuf, &ebuflen))
	{
		sprintf(outMsg,"最终过程错误");
		ret=false;
		goto err;
	}

	fwrite(ebuf, sizeof(char), ebuflen,outfd);
err:
	fclose(infd);
	fclose(outfd);
	
	delete [] pIn;
	delete [] pOut;

	for(int i = 0;i< index;i++)
	{
		if(ekey[i]) delete ekey[i];
	}
	if(pkey) EVP_PKEY_free(pkey);
	if((&eCtx)->cipher) EVP_CIPHER_CTX_cleanup(&eCtx);

	EVP_cleanup();

	return ret;
}

//公钥加密
BOOL CEvp::RSAPubEnc(const BYTE * pCertBuf/*[in]公钥*/,const UINT nCertLen/*[in]公钥长度*/,
			const char * pPwd/*[in]公钥密码,只针对PFX包文件*/,const BYTE * pInStream/*[in]输入文件或内存*/,
			DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,BYTE * pOutStream/*[out]加密后的数据*/,
			DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out加密后数据长度*/,
			char * OperMsg/*[out]返回操作错误信息*/)
{
	UINT iblock_size = 0, //输入块长度
		oblock_size = 0; //输出块长度
	unsigned char * bufin=NULL,
				  * bufout=NULL;
	int inlen = 0,
		outlen = 0;
	BOOL ret = TRUE;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度
	X509 * x509 = NULL;
	EVP_PKEY * pcert = NULL;
	RSA * rsa = NULL;
	int memtemplen = 0; //输出内存偏移
	UINT len = 0; //输入内存剩余长度

	int uMaxMem = nOutlen;	//可能为负数,不能用UINT


	BOOL bOutType = FALSE;//输出文件类型,内存-FALSE,文件-TRUE;

	if(nOutlen == 0) 
	{
		//检测文件名是否能够打开,以防pOutStream为文件
		bOutType = TRUE;
	}

	FILE * outfd = NULL,
		* infd = NULL;

	if(nInlen == 0)//输入为文件
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"未指定输入文件");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//得到文件长度
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//输出为文件
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"未指定输出文件");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pOutStream);
			return FALSE;
		}
	}

	x509 = CCertKey::LoadCert((char *)pCertBuf,nCertLen,(char *)pPwd,OperMsg);
	if (x509 == NULL)
	{
		ret = FALSE;
		goto err;
	}

	pcert = X509_get_pubkey(x509);
	if(pcert==NULL)
	{
		sprintf(OperMsg,"取得公钥密钥失败");
		ret = FALSE;
		goto err;
	}

    if (!(rsa = EVP_PKEY_get1_RSA(pcert)))
	{
		sprintf(OperMsg,"取得RSA密钥失败");
		ret=FALSE;
		goto err;
    }

	iblock_size = BN_num_bytes(rsa->n) - 11;//预加密长度,117
	oblock_size = BN_num_bytes(rsa->n);//加密后长度,128
	bufin = new unsigned char[iblock_size];
	memset(bufin,0,iblock_size);
	bufout = new unsigned char[oblock_size];
	memset(bufout,0,oblock_size);
	if(nInlen==0)//文件
	{	
		for(;;)
		{
			inlen = fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;
			outlen = RSA_public_encrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA加密失败");
				ret=FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,oblock_size);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen, bufout, outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen += inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}

	else//内存区域
	{
		for(UINT i=0;;i++)
		{
			//每次 iblock_size 或实际长度
			len = (nInlen>iblock_size)?iblock_size:nInlen;//内存区域长度
			nInlen -= len;
			outlen=RSA_public_encrypt(len,(UCHAR *)(pInStream+i*iblock_size),bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA加密失败");
				ret = FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,outlen);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}

				memcpy(pOutStream + memtemplen,bufout,outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen += len;
			DrawProg(finishLen*HUNDRED/fileLen);
			if(nInlen <= 0) break;     

		}	
		nOutlen = memtemplen;
	}
err:
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	if(pcert) EVP_PKEY_free(pcert);
    if(x509) X509_free(x509);
	if(rsa) RSA_free(rsa);
	delete [] bufin;
	delete [] bufout;
	return ret;
}


//私钥解密	
BOOL CEvp::RSAPrivDec(const BYTE * pKeyBuf/*[in]私钥*/,const UINT nKeyLen/*[in]私钥长度*/,
			const char * pPwd/*[in]私钥密码*/,const BYTE * pInStream/*[in]输入文件或内存*/,
			DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,BYTE * pOutStream/*[out]解密后的数据*/,
			DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out解密后数据长度*/,
		    char * OperMsg/*[out]返回操作错误信息*/)
{
	unsigned char * bufin = NULL,
				  * bufout = NULL;
	UINT iblock_size = 0,
		oblock_size = 0,
		outlen = 0,
		inlen = 0;
	BOOL ret = true;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度
	RSA * rsa = NULL;
	EVP_PKEY * pkey = NULL;
	FILE * outfd = NULL,
		 * infd = NULL;
	int memtemplen = 0; //输出内存偏移
	UINT len = 0; //输入内存剩余长度

	int uMaxMem = nOutlen;	//可能为负数,不能用UINT


	BOOL bOutType = FALSE;//输出文件类型,内存-FALSE,文件-TRUE;
	if(nOutlen == 0) bOutType = TRUE;

	if(nInlen == 0)//输入为文件
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"未指定输入文件");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//得到文件长度
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//输出为文件
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"未指定输出文件");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pOutStream);
			return FALSE;
		}
	}

	pkey=CCertKey::LoadKey((char *)pKeyBuf,nKeyLen,(char *)pPwd,OperMsg);
	if (pkey == NULL)
	{ 
//		sprintf(OperMsg,"取得私钥密钥失败");
		ret=FALSE;
		goto err;
	}
	
    if (!(rsa = EVP_PKEY_get1_RSA(pkey)))
	{
		sprintf(OperMsg,"取得RSA密钥失败");
		ret=FALSE;
		goto err;
    }


	iblock_size = BN_num_bytes(rsa->n);//预接密长度 128
	oblock_size = BN_num_bytes(rsa->n) - 11;//杰密后长度 117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];
	if(nInlen == 0)//文件
	{
		for(;;)
		{
			inlen=fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;//117,128
			outlen = RSA_private_decrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA解密失败");
				ret=FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,oblock_size);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}

				memcpy(pOutStream + memtemplen,bufout,outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen+=inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}
	else//内存
	{
		for(UINT i=0;;i++)
		{
			//每次 iblock_size 或实际长度
			len = (nInlen>iblock_size)?iblock_size:nInlen;//内存区域长度
			nInlen -= len;
			outlen = RSA_private_decrypt(len,(UCHAR *)(pInStream+i*iblock_size),bufout,
				rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA解密失败");
				ret = FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,outlen);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}

				memcpy(pOutStream + memtemplen,bufout,outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen += len;
			DrawProg(finishLen*HUNDRED/fileLen);
			if(nInlen <= 0) break;          
		}	
		nOutlen = memtemplen;
	}
err:
	if(pkey) EVP_PKEY_free(pkey);
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	if(rsa) RSA_free(rsa);
	delete [] bufin;
	delete [] bufout;
	return ret;
}

/*私钥加密(签名)*/
BOOL CEvp::RSAPrivEnc(const BYTE * pKeyBuf/*[in]私钥*/,const UINT nKeyLen/*[in]私钥长度*/,
			const char * pPwd/*[in]私钥密码*/,const BYTE * pInStream/*[in]输入文件或内存*/,
			DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,BYTE * pOutStream/*[out]加密后的数据*/,
			DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out加密后数据长度*/,
			char * OperMsg/*[out]返回操作错误信息*/)
{
	RSA * rsa = NULL;
	EVP_PKEY * pkey = NULL;
	UINT iblock_size = 0,
		oblock_size = 0,
		outlen = 0,
		inlen = 0;
	BOOL ret = TRUE;
	UCHAR * bufin = NULL,
		  * bufout = NULL;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度
	
	int memtemplen = 0; //输出内存偏移
	UINT len = 0; //输入内存剩余长度
	int uMaxMem = nOutlen;	//可能为负数,不能用UINT

	FILE * outfd = NULL,
		 * infd = NULL;

	BOOL bOutType = FALSE;//输出文件类型,内存-FALSE,文件-TRUE;
	if(nOutlen == 0) bOutType = TRUE;


	if(nInlen == 0)//输入为文件
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"未指定输入文件");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//得到文件长度
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//输出为文件
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"未指定输出文件");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pOutStream);
			return FALSE;
		}
	}

	pkey=CCertKey::LoadKey((char *)pKeyBuf,nKeyLen,(char *)pPwd,OperMsg);
	if (pkey == NULL)
	{ 
//		sprintf(OperMsg,"取得私钥密钥失败");
		ret=FALSE;
		goto err;
	}
	
    if (!(rsa = EVP_PKEY_get1_RSA(pkey)))
	{
		sprintf(OperMsg,"取得RSA密钥失败");
		ret=FALSE;
		goto err;
    }


	iblock_size = BN_num_bytes(rsa->n) - 11;//输入长度 117
 	oblock_size = BN_num_bytes(rsa->n);//加密后长度 128
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];
	if(nInlen == 0)//文件
	{
		for(;;)
		{
			inlen=fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;//117,128
			outlen = RSA_private_encrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA加密失败");
				ret=FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,oblock_size);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen+=inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}
	else//内存
	{
		for(UINT i=0;;i++)
		{
			//每次 iblock_size 或实际长度
			len = (nInlen>iblock_size)?iblock_size:nInlen;//内存区域长度
			nInlen -= len;
			outlen = RSA_private_encrypt(len,(UCHAR *)(pInStream+i*iblock_size),
				bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA加密失败");
				ret = FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,outlen);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen += len;
			DrawProg(finishLen*HUNDRED/fileLen);
			if(nInlen <= 0) break;          
		}	
		nOutlen = memtemplen;
	}

err:
	if(pkey) EVP_PKEY_free(pkey);
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	if(rsa) RSA_free(rsa);
	delete [] bufin;
	delete [] bufout;
	return ret;
}

//公钥解密
BOOL CEvp::RSAPubDec(const BYTE * pCertBuf/*[in]公钥*/,const UINT nCertLen/*[in]公钥长度*/,
			const char * pPwd/*[in]公钥密码,只针对PFX包文件*/,const BYTE * pInStream/*[in]输入文件或内存*/,
			DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,BYTE * pOutStream/*[out]加密后的数据*/,
			DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out加密后数据长度*/,
			char * OperMsg/*[out]返回操作错误信息*/)
{
	UINT iblock_size = 0, //输入块长度
		oblock_size = 0; //输出块长度
	unsigned char * bufin=NULL,
				  * bufout=NULL;
	int inlen = 0,
		outlen = 0;
	BOOL ret = TRUE;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度
	X509 * x509 = NULL;
	EVP_PKEY * pcert = NULL;
	RSA * rsa = NULL;
	int memtemplen = 0; //输出内存偏移
	UINT len = 0; //输入内存剩余长度
	int uMaxMem = nOutlen;	//可能为负数,不能用UINT

	BOOL bOutType = FALSE;//输出文件类型,内存-FALSE,文件-TRUE;

	if(nOutlen == 0) bOutType = TRUE;

	FILE * outfd = NULL,
		* infd = NULL;

	if(nInlen == 0)//输入为文件
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"未指定输入文件");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//得到文件长度
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//输出为文件
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"未指定输出文件");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//原文
		{
			sprintf(OperMsg,"打开文件%s失败",pOutStream);
			return FALSE;
		}
	}

	x509 = CCertKey::LoadCert((char *)pCertBuf,nCertLen,(char *)pPwd,OperMsg);
	if (x509 == NULL)
	{
		ret = FALSE;
		goto err;
	}

	pcert = X509_get_pubkey(x509);
	if(pcert==NULL)
	{
		sprintf(OperMsg,"取得公钥密钥失败");
		ret = FALSE;
		goto err;
	}

    if (!(rsa = EVP_PKEY_get1_RSA(pcert)))
	{
		sprintf(OperMsg,"取得RSA密钥失败");
		ret=FALSE;
		goto err;
    }
	iblock_size = BN_num_bytes(rsa->n);//预解密长度128
	oblock_size = BN_num_bytes(rsa->n) - 11;//解密后长度117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];
	if(nInlen==0)//文件
	{	
		for(;;)
		{
			inlen = fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;
			outlen = RSA_public_decrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA解密失败");
				ret=FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,oblock_size);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen += inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}

	else//内存区域
	{
		for(UINT i=0;;i++)
		{
			//每次 iblock_size 或实际长度
			len = (nInlen>iblock_size)?iblock_size:nInlen;//内存区域长度
			nInlen -= len;
			outlen = RSA_public_decrypt(len,(UCHAR *)(pInStream+i*iblock_size),bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//加密后资料长度
			{
				sprintf(OperMsg,"RSA解密失败");
				ret = FALSE;
				goto err;
			}
			if(bOutType)
			{
				fwrite(bufout,sizeof(char),outlen,outfd);
				memset(bufout,0,outlen);
			}
			else
			{
				uMaxMem -= outlen;	//剩余缓冲大小
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "输出缓冲过小,不能容纳操作结果"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//拷贝到调用函数
				memtemplen += outlen;
			}
			finishLen += len;
			DrawProg(finishLen*HUNDRED/fileLen);
			if(nInlen <= 0) break;     

		}	
		nOutlen = memtemplen;
	}
err:
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	if(pcert) EVP_PKEY_free(pcert);
    if(x509) X509_free(x509);
	if(rsa) RSA_free(rsa);
	delete [] bufin;
	delete [] bufout;
	return ret;

}

void CEvp::DrawProg(float fPot)
{
	if(m_pProgress)
		m_pProgress->SetPos((int)fPot);
}

void CEvp::SetProgRess(CProgressCtrlST *pProgress)
{
	m_pProgress = pProgress;
}

//功能:得到加密后缓冲区大小
//参数:
//		pCertBuf [in] - 公钥
//		nCertLen [in] - 公钥长度
//		nInlen   [in] - 数据长度
//返回值:输出缓冲区大小 -1 标示函数操作失败
DWORD CEvp::GetEncLen(const char * pCertBuf, const UINT nCertLen, const DWORD nInlen)
{
	X509 * x509 = NULL;
	EVP_PKEY * pcert = NULL;
	RSA * rsa = NULL;
	DWORD len = 0;
	UINT iblock_size = 0, //输入块长度
		oblock_size = 0; //输出块长度
	UINT uMultiple = 0;//输入是块长度的倍数,用于计算输出
	char OperMsg[100] = {0};
	x509 = CCertKey::LoadCert((char *)pCertBuf,nCertLen,"",OperMsg);
	if (x509 == NULL)
	{
		len = -1;
		goto err;
	}
	pcert = X509_get_pubkey(x509);
	if(pcert==NULL)
	{
		sprintf(OperMsg,"取得公钥密钥失败");
		len = -1;
		goto err;
	}

    if (!(rsa = EVP_PKEY_get1_RSA(pcert)))
	{
		sprintf(OperMsg,"取得RSA密钥失败");
		len = 0;
		goto err;
    }

	iblock_size = BN_num_bytes(rsa->n) - 11;//预加密长度,117
	oblock_size = BN_num_bytes(rsa->n);//加密后长度,128

	if(nInlen%iblock_size == 0)//正好整除
		uMultiple = nInlen/iblock_size;
	else
		uMultiple = nInlen/iblock_size + 1;

	len = (nInlen > iblock_size) ? (uMultiple * oblock_size) : oblock_size;//内存区域长度

err:
	if(pcert) EVP_PKEY_free(pcert);
    if(x509) X509_free(x509);
	if(rsa) RSA_free(rsa);
	return len;
}

//目录部分


//功能:初始化对称加密结构
//支持目录加密
//返回值 : 成功TRUE
//		   失败FALSE
//参数 : 
//			strCpName - 算法名称
//			strPwd - 密钥
//			type - 类型 1 - 加密; 0 - 解密
//			Ctx - 返回加密信息体
//调用时机: 加密前
BOOL CEvp::InitCrypt2(const CString strCpName, CString strPwd, const int type, EVP_CIPHER_CTX & Ctx)
{
	const EVP_CIPHER * cipher=NULL;
	unsigned char key[EVP_MAX_KEY_LENGTH]="";//算法最长的KEY长度
    unsigned char iv[EVP_MAX_IV_LENGTH]="";//算法最长的IV长度
	OpenSSL_add_all_algorithms();//add digests   and ciphers
	
	cipher = EVP_get_cipherbyname(strCpName);
	if(cipher==NULL)
	{
//		sprintf(outMsg,"Unknown cipher name %s\n",cpname);
		return FALSE;
	}
	
	//表示產生的 key 是給哪一種 cipher 用的
	//過程中所使用的 hash 演算法,
	//用來加密的 salt，至少為八個 bytes，否則必須NULL,
	//重覆幾次 hashing 的動作，愈多次愈花時間，但相對地也愈安全

	if(!EVP_BytesToKey(cipher,EVP_md5(),NULL,(unsigned char *)strPwd.GetBuffer(0),
		strPwd.GetLength(),1,key,iv))
	{
//		strcpy(outMsg,"Crypt初始化key or iv 失败");
		return FALSE;
	}
	
	//EVP_CIPHER_CTX Ctx;
	EVP_CIPHER_CTX_init(&Ctx);//初始化一个EVP_CIPHER_CTX结构体
	
	//type为1，则加密；如果type为0，则解密；如果type是－1，则不改变数据
	if(!EVP_CipherInit_ex(&Ctx, cipher, NULL/*NULL使用缺省的实现算法*/,key,iv,type))//初始化
	{
//		strcpy(outMsg,"Crypt初始化加解密结构体失败");
		return FALSE;
	}
	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
	return TRUE;

}

//功能:清除对称加密结构
//调用:加密完毕后
void CEvp::CleanCrypt2(EVP_CIPHER_CTX & Ctx)
{
	if(Ctx.cipher) 
		EVP_CIPHER_CTX_cleanup(&Ctx); //清除所有信息释放内存
	CRYPTO_cleanup_all_ex_data();
}

//返回值 : 加密时候返回加密后文件大小
//		   解密时候返回解密后文件大小
//参数 : 
//			pCtx - 已经初始化的加密信息体
//			strFileName - 加密为要加密的文件名,解密为解密后保存的文件名
//			File - 文件指针 ,具体位置的偏移有调用者负责
//			dBolckLen - 块长度,块标示某个文件加密后的内容,只在解密时候有效,可以解密某个文件
//						 加密时候无效,为0
//			outStr - 返回错误信息
DWORD CEvp::Crypt2(EVP_CIPHER_CTX * pCtx, const CString & strFileName,
					CFile & File, DWORD dBolckLen, CString & outStr)	   
{
	if(File.m_hFile == CFile::hFileNull)
		return -1;

	unsigned char inbuf[BUFLEN] = "";
	unsigned char outbuf[BUFLEN + EVP_MAX_BLOCK_LENGTH] = "";//所有算法最长的块长度。
	int infilelen = 0,
		outlen=0;
	BOOL ret = true;
	DWORD dFileLen = 0;//文件长度
	DWORD dFinishLen = 0;//完成长度
	long lReadLen = 0; //每次读取的块长度
	DWORD dReturnLen = 0;//返回加解密结果长度

	CFile inSideFile;//内部文件
	if(0 == dBolckLen)//块长度为0标示要加密
	{
		if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
		{
			outStr.Format("打开文件%s失败", strFileName);
			return -1;
		}

		dFileLen = inSideFile.GetLength();

		for(;;)
		{                       
			infilelen = inSideFile.Read(inbuf, BUFLEN);
			if(infilelen <= 0) break;
			if(!EVP_CipherUpdate(pCtx, outbuf, &outlen, inbuf, infilelen))//中間過程 
			{
				/* Error */              
				outStr = "Crypt中间过程错误";
				ret = FALSE;
				goto err;
			}
			File.Write(outbuf, outlen);
			dFinishLen += infilelen;
			dReturnLen += outlen;
			DrawProg(dFinishLen*HUNDRED/dFileLen);
		} 

		if(!EVP_CipherFinal_ex(pCtx, outbuf, &outlen))    //最終步驟-处理最后（Final）的一段数据
		{   /* Error */                       
			outStr = "Crypt最终过程错误"; 
			ret = FALSE;
			goto err;
		}               
		File.Write(outbuf, outlen);

	}
	else	//解密
	{
		if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			outStr.Format("创建文件%s失败", strFileName);
			return -1;
		}

		dFileLen = dBolckLen;

		for(;;)
		{
			//每次 iblock_size 或实际长度
			lReadLen = (dBolckLen > BUFLEN) ? BUFLEN:dBolckLen;//内存区域长度
			infilelen = File.Read(inbuf, lReadLen);
			if(0 == infilelen)
				break;


			if(!EVP_CipherUpdate(pCtx, outbuf, &outlen, inbuf, infilelen))//中間過程 
			{
				/* Error */              
				outStr = "Crypt中间过程错误";
				ret = FALSE;
				goto err;
			}

			inSideFile.Write(outbuf, outlen);
			dReturnLen += outlen;
			dFinishLen += infilelen;
			DrawProg(dFinishLen*HUNDRED/dFileLen);
			dBolckLen -= infilelen;
			if(dBolckLen <= 0) break;
		}

		if(!EVP_CipherFinal_ex(pCtx, outbuf, &outlen))    //最終步驟-处理最后（Final）的一段数据
		{   /* Error */                       
			outStr = "Crypt最终过程错误"; 
			ret = FALSE;
			goto err;
		}	               
		inSideFile.Write(outbuf, outlen);
	}

	dReturnLen += outlen;

err:
	if(inSideFile.m_hFile != CFile::hFileNull )
		inSideFile.Close();
	if(ret)
		return dReturnLen;
	else return -1;
}

BOOL CEvp::InitDigest2(CString strMdName, EVP_MD_CTX & Ctx)
{
	const EVP_MD *md = NULL;
	if(strMdName.IsEmpty())
	{
//		outStr = "未指定摘要算法";
		return FALSE;
	}
	//使EVP_Digest系列函数支持所有有效的信息摘要算法
	OpenSSL_add_all_digests();
    
	//根据输入的信息摘要函数的名字得到相应的EVP_MD算法结构
	md = EVP_get_digestbyname(strMdName.GetBuffer(0));  
	strMdName.ReleaseBuffer();
	if(!md) 
	{
//		outStr.Format("未知摘要算法名称 %s",strMdName);
		EVP_cleanup();
		return FALSE;
	}

	//初始化信息摘要结构md_ctx。
	EVP_MD_CTX_init(&Ctx);
	
	//使用md的算法结构设置mdctx结构，impl为NULL，即使用缺省实现的算法（openssl本身提供的信息摘要算法）
	if(!EVP_DigestInit_ex(&Ctx, md, NULL/*impl*/))
	{
//		outStr.Format("Digest初始化算法结构错误");
		EVP_cleanup();
		return FALSE;
	}
	EVP_cleanup();
	return TRUE;
}


void CEvp::CleanDigest2(EVP_MD_CTX & mdCtx)
{
	if(mdCtx.digest) EVP_MD_CTX_cleanup(&mdCtx);
	CRYPTO_cleanup_all_ex_data();
}


/*消息摘要*/ 
//返回值:摘要后长度,-1标示失败
//参数 :	strMdName - 摘要算法名称
//			strFileName  - 输入文件名称
//			File - 已经打开的结果文件
//			outStr - 返回错误信息
DWORD CEvp::Digest2(EVP_MD_CTX * pCtx, const CString strFileName,
			CFile & File, CString & outStr)
{
	if(File.m_hFile == CFile::hFileNull)
		return -1;
	char inbuf[BUFLEN] = "";
	UINT len = 0;
	BOOL ret = true;
	EVP_MD_CTX md_ctx;
	memset(&md_ctx,0,sizeof(md_ctx));
	FILE * infd = NULL;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度

	unsigned char md_value[MAX_MD_SIZE]="";
	UINT md_len = 0;/*摘要长度*/
	

	if(strFileName.IsEmpty())
	{
		outStr = "未指定输入文件";
		return -1;
	}
	
	CFile inSideFile;//内部文件
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "不能打开指定文件";
		return -1;
	}


	for(;;)
	{
		len = fread(inbuf, 1, BUFLEN, infd);
		if(len <= 0) break;          
		if(!EVP_DigestUpdate(&md_ctx, inbuf, len))
		{
			outStr.Format("Digest中间过程错误");
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen * HUNDRED / fileLen);
	}
	//完成信息摘要计算过程，将完成的摘要信息存储在md_value里面,长度信息存储在md_len里面
	if(!EVP_DigestFinal_ex(&md_ctx, md_value, &md_len))
	{	
		outStr.Format("Digest最终过程错误");
		ret=FALSE;
	}

	
err:
	//使用该函数释放mdctx占用的资源，如果使用_ex系列函数，这是必须调用的。
	if(ret)
	{
		//记录文件
		File.Write(md_value, md_len);
		return md_len;
	}
	else
		return -1;
}

BOOL CEvp::VerifyDigest2(EVP_MD_CTX * mdCtx, const CString strFileName,
			CFile & File, DWORD dBolckLen, CString & outStr)
{
	if(File.m_hFile == CFile::hFileNull)
		return -1;
	char inbuf[BUFLEN] = "";
	UINT len = 0;
	BOOL ret = true;
	EVP_MD_CTX md_ctx;
	memset(&md_ctx,0,sizeof(md_ctx));
	FILE * infd = NULL;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度

	unsigned char md_value[MAX_MD_SIZE]="";
	UINT md_len = 0;/*摘要长度*/
	

	if(strFileName.IsEmpty())
	{
		outStr = "未指定输入文件";
		return FALSE;
	}
	
	CFile inSideFile;//内部文件
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("文件%s丢失", strFileName);
		return FALSE;
	}


	for(;;)
	{
		len = fread(inbuf, 1, BUFLEN, infd);
		if(len <= 0) break;          
		if(!EVP_DigestUpdate(&md_ctx, inbuf, len))
		{
			outStr.Format("Digest中间过程错误");
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen * HUNDRED / fileLen);
	}
	//完成信息摘要计算过程，将完成的摘要信息存储在md_value里面,长度信息存储在md_len里面
	if(!EVP_DigestFinal_ex(&md_ctx, md_value, &md_len))
	{	
		outStr.Format("Digest最终过程错误");
		ret=FALSE;
	}

	
err:
	//使用该函数释放mdctx占用的资源，如果使用_ex系列函数，这是必须调用的。
	if(ret)
	{
		//比较文件摘要与结果是否相同
		if(md_len != dBolckLen)
		{
			outStr.Format("摘要校验错误");
			ret=FALSE;
		}
		else
		{
			char * pmdbuf = new char[dBolckLen + 1];
			File.Read(pmdbuf, dBolckLen);
			if(memcmp(pmdbuf, md_value, dBolckLen) == 0)
			{
				outStr.Format("摘要校验正确");
				ret=TRUE;
			}
			else
			{
				outStr.Format("摘要校验错误");
				ret=FALSE;
			}
			delete [] pmdbuf;
		}
	}

	return ret;
}

BOOL CEvp::InitSeal2(stuCertLink * pCERT, const CString strCpName,
			  CFile & File, EVP_CIPHER_CTX & eCtx, CString & outStr)
{
	//对随机数播种（seeded）。
	UCHAR iv[EVP_MAX_IV_LENGTH]="";
	UCHAR * ekey [128]; 
	EVP_PKEY * pubKey[128];
	int ekeylen[128];
	int index = 0;

	const EVP_CIPHER *cipher=NULL;
	BOOL ret=true;
 //	DWORD fileLen=0;//文件长度
//	DWORD finishLen=0;//完成长度
	UINT uCrc16 = 0; //加密后重要信息CRC码
	DWORD dOutLen = 0;//内存文件长度
	BYTE * pOut = NULL; //内存文件缓冲区
	DWORD dFileInfoLen = 0;

	//加载加密公钥
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_CERT),"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert = LoadResource(NULL,hRsrc);
	LPSTR lpCert = (LPSTR)LockResource(hgCert);	


	CMemFile MemFile;				//用于存放重要信息的内存文件

	DWORD dInLen = 0;
	BYTE * pIn = NULL;				//用于MemFile.Dectch();


//	fileLen = filelength(fileno(infd));//得到文件长度

	OpenSSL_add_all_algorithms();

	int nCountCert = 0;	//证书个数

	for(stuCertLink * temp = pCERT; temp != NULL; temp=temp->Link, nCountCert++)
	{
		X509 * pX509 = NULL;
		pX509 = CCertKey::LoadCert(temp->pCert, temp->CertLen, NULL, outStr.GetBuffer(255));
		outStr.ReleaseBuffer();
	    if(pX509)
		{
			pubKey[nCountCert] = X509_get_pubkey(pX509);
			if (pubKey[nCountCert] == NULL)
			{
				outStr = "Read Public Key Failed!";
				ret=false;
				goto err;
			}
			ekey[nCountCert] = new unsigned char[EVP_PKEY_size(pubKey[nCountCert])];
			X509_free(pX509);
		}
		else
		{
			outStr.Format("加载第 %d 个证书 %s 失败", nCountCert+1, temp->pCert);
			ret = false;
			goto err;
		}
	}
	
	cipher = EVP_get_cipherbyname(strCpName);
	if(cipher==NULL)
	{
		outStr.Format("Unknown cipher name %s\n",strCpName);
		ret=false;
		goto err;
	}

	if(!EVP_SealInit(&eCtx,cipher,
		ekey,
		ekeylen,//存放所有 ek 長度
		iv,//自动生成
		pubKey,//加密 ek 用的 public key(s)
		nCountCert))
	{
		ret=false;
		goto err;
	}

	MemFile.Write(&nCountCert, sizeof(int));	//1 - 写入公钥个数

	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Write(&ekeylen[index], sizeof(int));
		MemFile.Write(ekey[index], ekeylen[index]);
	}

	MemFile.Write(iv, sizeof(iv));

	//构造加密结果 长度+1 内存
	dFileInfoLen = CEvp::GetEncLen(lpCert, lenCert, MemFile.GetLength());

	dOutLen = dFileInfoLen + 1;
	pOut = new BYTE[dOutLen];

	dInLen = MemFile.GetLength();
	pIn = MemFile.Detach();

	//加密重要信息
	if(!CEvp::RSAPubEnc((BYTE *)lpCert, lenCert, "", pIn, 
		dInLen, pOut, dOutLen, outStr.GetBuffer(255)))
	{
		//加密失败
		ret=false;
		goto err;
	}

	//得到加密后长度的CRC校验
	if(!CRC::GetCrc16((char*)&dOutLen, sizeof(DWORD), uCrc16))
	{
		//加密失败
		ret=false;
		goto err;
	}

	//写入加密后重要信息长度和CRC校验和加密后重要信息内容
	File.Write(&dOutLen, sizeof(DWORD));
	File.Write(&uCrc16, sizeof(UINT));
	File.Write(pOut, dOutLen);

err:
	outStr.ReleaseBuffer();

	free(pIn);
	delete [] pOut;
	
	for(int i = 0;i< nCountCert;i++)
	{
		if(pubKey[i]) EVP_PKEY_free(pubKey[i]);
		if(ekey[i]) delete [] ekey[i];
	}
	
	//if(eCtx.cipher) EVP_CIPHER_CTX_cleanup(&eCtx);
	EVP_cleanup();
	return ret;
}

void CEvp::CleanSeal2(EVP_CIPHER_CTX & sealCtx)
{
	if(sealCtx.cipher) 
		EVP_CIPHER_CTX_cleanup(&sealCtx); //清除所有信息释放内存
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
}


DWORD CEvp::Seal2(EVP_CIPHER_CTX * pCtx,const CString strFileName, CFile & File, CString & outStr)
{
	int readlen = 0;
	int ebuflen = 0;
	UCHAR buf[BUFLEN]="";
	UCHAR ebuf[BUFLEN + EVP_MAX_BLOCK_LENGTH]="";
	BOOL ret = TRUE;
	DWORD finishLen = 0;
	CFile inSideFile;//内部文件
	DWORD dProgLen = 0; //总共需要加密的长度
	DWORD dProgFinshLen = 0;//已经加密的长度

	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "不能打开指定文件";
		return -1;
	}

	dProgLen = inSideFile.GetLength();

	for(;;)
	{
		readlen = inSideFile.Read(buf, sizeof(buf));
		
		if (readlen <= 0)
		{
			break;
		}
		
		if(!EVP_SealUpdate(pCtx, ebuf, &ebuflen, buf, readlen))
		{
			outStr = "中间过程出错";
			ret=false;
			goto err;
		}
		File.Write(ebuf, ebuflen);
		finishLen += ebuflen;

		dProgFinshLen += readlen;
		DrawProg(dProgFinshLen*HUNDRED/dProgLen);
	}
	
	if(!EVP_SealFinal(pCtx, ebuf, &ebuflen))
	{
		outStr = "最终过程出错";
		ret = false;
		goto err;
	}
	File.Write(ebuf, ebuflen);
	finishLen += ebuflen;

err:
	inSideFile.Close();
	if(ret)
		return finishLen;
	else
		return ret;
}

BOOL CEvp::InitOpenSeal2(char * key, int keylen, char * pwd, const CString strCpName,
			  CFile & File, EVP_CIPHER_CTX & eCtx, CString & outStr)
{
	if(File.m_hFile == CFile::hFileNull)
		return FALSE;

	unsigned char iv[EVP_MAX_IV_LENGTH]="";
	const EVP_CIPHER *cipher=NULL;
	int ebuflen=0,readlen=0;
	bool ret=true;
	int nCountCert = 0;	//公钥个数
	int index = 0;
	UCHAR * ekey [128] = {0}; 
	int ekeylen[128];
	BOOL bInit = FALSE;//是否初始化成功
	DWORD dFileInfoLen = 0;//加密后重要信息长度
	UINT uCrc16 = 0; //加密后重要信息CRC码

	EVP_PKEY * pkey=NULL;
	CMemFile MemFile;
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_KEY),"CERT");
	DWORD lenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey = LoadResource(NULL,hRsrc);
	LPSTR lpKey = (LPSTR)LockResource(hgKey);
	BYTE * pIn = NULL;
	BYTE * pOut = NULL;
	DWORD dOutLen = 0;

	OpenSSL_add_all_algorithms();//digests   and ciphers
	/* Read private key */
	pkey = CCertKey::LoadKey(key,keylen,pwd,outStr.GetBuffer(255));
	outStr.ReleaseBuffer();
	if (pkey == NULL)
	{ 
		ret = false;
		goto err;
	}

	//得到重要信息长度和CRC校验码
	readlen = File.Read(&dFileInfoLen, sizeof(DWORD));
//	finishLen += readlen;

	readlen = File.Read(&uCrc16, sizeof(UINT));
//	finishLen += readlen;

	if(!CRC::CheckCrc((char *)&dFileInfoLen, sizeof(DWORD), uCrc16))
	{
		outStr = "文件头CRC校验错误";
		ret = false;
		goto err;
	}

	//开辟缓冲区,容纳加密后的重要文件信息
	pIn = new BYTE[dFileInfoLen + 1];
	dOutLen = dFileInfoLen;

	readlen = File.Read(pIn, dFileInfoLen);
//	finishLen += readlen;



	//开辟缓冲区,容纳加密后的信息体
	pOut = new BYTE[dOutLen];
	if(!CEvp::RSAPrivDec((BYTE *)lpKey, lenKey, "", pIn, 
		dFileInfoLen, pOut, dOutLen, outStr.GetBuffer(255)))
	{
		//解密失败
		outStr.ReleaseBuffer();
		outStr = "重要信息解密失败";
		ret = false;
		goto err;
	}

	outStr.ReleaseBuffer();
	MemFile.Attach(pOut, dOutLen);

	//首先是公钥个数
	MemFile.Read(&nCountCert, sizeof(int));

	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Read(&ekeylen[index], sizeof(int));
		ekey[index] = new unsigned char[ekeylen[index]];
		MemFile.Read(ekey[index], ekeylen[index]);
	}

	MemFile.Read(iv, sizeof(iv));

	MemFile.Close();

	//输入文件已经偏移到加密后的文件
	
	cipher = EVP_get_cipherbyname(strCpName);

	if(cipher == NULL)
	{
		outStr.Format("Unknown cipher name %s\n",strCpName);
		ret = false;
		goto err;
	}

	for(index = 0; index<nCountCert; index++)
	{
		if (ekeylen[index] != EVP_PKEY_size(pkey))
		{
			continue;
		}

		if(EVP_OpenInit(&eCtx,cipher, ekey[index],ekeylen[index],iv,pkey))
		{
			bInit = TRUE;
			break;
		}
	}
	if(!bInit)
	{
		outStr = "指定证书不能解密文件,请确认证书合法性";
		ret=false;
		goto err;
	}
err:
	delete [] pIn;
	delete [] pOut;

	for(int i = 0;i< nCountCert;i++)
	{
		if(ekey[i]) delete ekey[i];
	}
	if(pkey) EVP_PKEY_free(pkey);

	EVP_cleanup();

	return ret;

}


DWORD CEvp::OpenSeal2(EVP_CIPHER_CTX & eCtx, CFile & File, DWORD dBolckLen, const CString strFileName, CString & outStr)
{
	unsigned char buf[BUFLEN]="";
	unsigned char ebuf[BUFLEN + EVP_MAX_BLOCK_LENGTH]="";
	int ebuflen = 0,
		readlen = 0,
		fileLen = 0,
		finishLen = 0;
	bool ret=true;

	DWORD dProgLen = dBolckLen; //总共需要处理的长度
	DWORD dProgFinshLen = 0;//已经处理的长度


	CFile inSideFile;//内部文件

	if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		outStr.Format("创建文件%s错误", strFileName);
		return -1;
	}
	for(;;)
	{
		//每次 iblock_size 或实际长度
		fileLen = (dBolckLen > BUFLEN) ? BUFLEN:dBolckLen;//内存区域长度
		readlen = File.Read(buf, fileLen);
		if(readlen == 0)//可能加密时文件长度为0
			break;

		if(!EVP_OpenUpdate(&eCtx, ebuf, &ebuflen, buf, readlen))
		{
			outStr = "中间过程错误";
			ret=false;
			goto err;
		}

		inSideFile.Write(ebuf, ebuflen);


		finishLen += ebuflen;
		dBolckLen -= fileLen;

		dProgFinshLen += readlen;
		DrawProg(dProgFinshLen*HUNDRED/dProgLen);

		if(dBolckLen <= 0) break;
	}

		//DrawProg(finishLen*HUNDRED/fileLen);
	if(!EVP_OpenFinal(&eCtx, ebuf, &ebuflen))
	{
		outStr = "最终过程错误";
		ret=false;
		goto err;
	}

	inSideFile.Write(ebuf, ebuflen);
	finishLen += ebuflen;

err:
	inSideFile.Close();
	if(ret)
		return finishLen;
	else
		return -1;
}

void CEvp::CleanOpenSeal2(EVP_CIPHER_CTX & eCtx)
{
	if(eCtx.cipher) 
		EVP_CIPHER_CTX_cleanup(&eCtx); //清除所有信息释放内存
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
}


BOOL CEvp::GetPubRsa(const BYTE * pCertBuf/*[in]公钥*/, const UINT nCertLen/*[in]公钥长度*/, RSA *& pRsa)
{
	X509 * x509 = NULL;
	EVP_PKEY * pcert = NULL;
	CString outStr;
	BOOL ret = TRUE;
	
	x509 = CCertKey::LoadCert((char *)pCertBuf, nCertLen, "", outStr.GetBuffer(255));
	outStr.ReleaseBuffer();
	if (x509 == NULL)
	{
		ret = FALSE;
		goto err;
	}

	pcert = X509_get_pubkey(x509);
	if(pcert==NULL)
	{
		outStr = "取得公钥密钥失败";
		ret = FALSE;
		goto err;
	}

    if (!(pRsa = EVP_PKEY_get1_RSA(pcert)))
	{
		outStr = "取得RSA密钥失败";
		ret=FALSE;
		goto err;
    }

err:
	if(pcert) EVP_PKEY_free(pcert);
    if(x509) X509_free(x509);
	return ret;
}

BOOL CEvp::GetPrivRsa(const BYTE * pKeyBuf/*[in]私钥*/,const UINT nKeyLen/*[in]私钥长度*/,
			const char * pPwd/*[in]私钥密码*/, RSA *& pRsa)
{
	EVP_PKEY * pkey = NULL;
	CString outStr;
	pkey = CCertKey::LoadKey((char *)pKeyBuf,nKeyLen,(char *)pPwd,outStr.GetBuffer(255));
	outStr.ReleaseBuffer();
	BOOL ret = TRUE;

	if (pkey == NULL)
	{ 
		outStr = "取得私钥密钥失败";
		ret=FALSE;
		goto err;
	}
	
    if (!(pRsa = EVP_PKEY_get1_RSA(pkey)))
	{
		outStr = "取得RSA密钥失败";
		ret=FALSE;
		goto err;
    }
err:
	if(pkey) EVP_PKEY_free(pkey);
	return ret;
}

void CEvp::FreeRsa(RSA *& pRsa)
{
	if(pRsa) RSA_free(pRsa);
	CRYPTO_cleanup_all_ex_data();
}


//公钥加密
DWORD CEvp::RSAPubEnc2(RSA * pRsa, const CString strFileName, CFile & File, CString & outStr)
{
	if(pRsa == NULL)
	{
		outStr = "RSA非法";
		return -1;
	}
	UINT iblock_size = 0, //输入块长度
		 oblock_size = 0; //输出块长度

	unsigned char * bufin=NULL,
				  * bufout=NULL;

	int inlen = 0,
		outlen = 0;

	DWORD dProgLen = 0; //总共需要处理的长度
	DWORD dProgFinshLen = 0;//已经处理的长度

	BOOL ret = TRUE;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度
	DWORD returnLen = 0;//最后返回文件加密后长度

	CFile inSideFile;//内部文件
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "不能打开指定文件";
		return -1;
	}

	dProgLen = inSideFile.GetLength();

	iblock_size = BN_num_bytes(pRsa->n) - 11;//预加密长度,117
	oblock_size = BN_num_bytes(pRsa->n);//加密后长度,128
	bufin = new unsigned char[iblock_size];
	memset(bufin,0,iblock_size);
	bufout = new unsigned char[oblock_size];
	memset(bufout,0,oblock_size);
	for(;;)
	{
		inlen = inSideFile.Read(bufin, iblock_size);
		if(!inlen)
			break;
		outlen = RSA_public_encrypt(inlen,bufin,bufout,pRsa,RSA_PKCS1_PADDING);
		if (outlen == -1)//加密后资料长度
		{
			outStr = "RSA加密失败";
			ret=FALSE;
			goto err;
		}
		File.Write(bufout, outlen);
		returnLen += outlen;
		memset(bufout,0,oblock_size);

		dProgFinshLen += inlen;
		DrawProg(dProgFinshLen*HUNDRED/dProgLen);

	}

err:
	inSideFile.Close();
//	if(pRsa) RSA_free(pRsa);
	delete [] bufin;
	delete [] bufout;
	if(!ret)
		return -1;
	else
		return returnLen;
}

//私钥解密	
DWORD CEvp::RSAPrivDec2(RSA * pRsa, CFile & File, DWORD dBolckLen, const CString strFileName, CString & outStr)
{
	if(pRsa == NULL)
	{
		outStr = "RSA非法";
		return -1;
	}
	unsigned char * bufin = NULL,
				  * bufout = NULL;
	DWORD dReturnLen = 0;
	UINT iblock_size = 0,
		oblock_size = 0,
		outlen = 0,
		inlen = 0,
		readlen = 0;
	BOOL ret = true;

	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度

	DWORD dProgLen = dBolckLen; //总共需要处理的长度
	DWORD dProgFinshLen = 0;//已经处理的长度


	CFile inSideFile;//内部文件

	if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			return -1;

	iblock_size = BN_num_bytes(pRsa->n);//预接密长度 128
	oblock_size = BN_num_bytes(pRsa->n) - 11;//杰密后长度 117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];

	for(;;)
	{
		//每次 iblock_size 或实际长度
		readlen = (dBolckLen > iblock_size) ? iblock_size:dBolckLen;//内存区域长度
		inlen = File.Read(bufin, readlen);

		if(inlen == 0)//可能加密时文件长度为0
			break;

		outlen = RSA_private_decrypt(inlen,bufin,bufout,pRsa,RSA_PKCS1_PADDING);
		if (outlen == -1)//加密后资料长度
		{
			outStr = "RSA解密失败";
			ret=FALSE;
			goto err;
		}
		inSideFile.Write(bufout, outlen);

		dReturnLen += outlen;
		memset(bufout,0,oblock_size);

		dBolckLen -= inlen;
		if(dBolckLen <= 0) break;

		dProgFinshLen += inlen;
		DrawProg(dProgFinshLen*HUNDRED/dProgLen);
	}

err:
	inSideFile.Close();
//	if(pRsa) RSA_free(pRsa);
	delete [] bufin;
	delete [] bufout;
	if(!ret)
		return -1;
	else return dReturnLen;
}

DWORD CEvp::RSAPrivEnc2(RSA * pRsa, const CString strFileName, CFile & File, CString & outStr)
{
	if(pRsa == NULL)
	{
		outStr = "RSA非法";
		return -1;
	}
	UINT iblock_size = 0, //输入块长度
		 oblock_size = 0; //输出块长度

	unsigned char * bufin=NULL,
				  * bufout=NULL;

	int inlen = 0,
		outlen = 0;

	BOOL ret = TRUE;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度
	DWORD returnLen = 0;//最后返回文件加密后长度

	DWORD dProgLen = 0; //总共需要处理的长度
	DWORD dProgFinshLen = 0;//已经处理的长度


	CFile inSideFile;//内部文件
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "不能打开指定文件";
		return -1;
	}

	dProgLen = inSideFile.GetLength();

	iblock_size = BN_num_bytes(pRsa->n) - 11;//预加密长度,117
	oblock_size = BN_num_bytes(pRsa->n);//加密后长度,128
	bufin = new unsigned char[iblock_size];
	memset(bufin,0,iblock_size);
	bufout = new unsigned char[oblock_size];
	memset(bufout,0,oblock_size);
	for(;;)
	{
		inlen = inSideFile.Read(bufin, iblock_size);
		if(!inlen)
			break;
		outlen = RSA_private_encrypt(inlen,bufin,bufout,pRsa,RSA_PKCS1_PADDING);
		if (outlen == -1)//加密后资料长度
		{
			outStr = "RSA加密失败";
			ret=FALSE;
			goto err;
		}
		File.Write(bufout, outlen);
		returnLen += outlen;
		memset(bufout,0,oblock_size);

		dProgFinshLen += inlen;
		DrawProg(dProgFinshLen*HUNDRED/dProgLen);
	}

err:
	inSideFile.Close();
//	if(pRsa) RSA_free(pRsa);
	delete [] bufin;
	delete [] bufout;
	if(!ret)
		return -1;
	else
		return returnLen;
}

DWORD CEvp::RSAPubDec2(RSA * pRsa, CFile & File, DWORD dBolckLen, const CString strFileName, CString & outStr)
{
	if(pRsa == NULL)
	{
		outStr = "RSA非法";
		return -1;
	}
	unsigned char * bufin = NULL,
				  * bufout = NULL;
	DWORD dReturnLen = 0;
	UINT iblock_size = 0,
		oblock_size = 0,
		outlen = 0,
		inlen = 0,
		readlen = 0;
	BOOL ret = true;
	long fileLen = 0;//文件长度
	long finishLen = 0;//完成长度

	DWORD dProgLen = dBolckLen; //总共需要处理的长度
	DWORD dProgFinshLen = 0;//已经处理的长度


	CFile inSideFile;//内部文件

	if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			return -1;

	iblock_size = BN_num_bytes(pRsa->n);//预接密长度 128
	oblock_size = BN_num_bytes(pRsa->n) - 11;//杰密后长度 117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];

	for(;;)
	{
		//每次 iblock_size 或实际长度
		readlen = (dBolckLen > iblock_size) ? iblock_size:dBolckLen;//内存区域长度
		inlen = File.Read(bufin, readlen);
		if(inlen == 0)//可能加密时文件长度为0
			break;
		outlen = RSA_public_decrypt(inlen,bufin,bufout,pRsa,RSA_PKCS1_PADDING);
		if (outlen == -1)//加密后资料长度
		{
			outStr = "RSA解密失败";
			ret=FALSE;
			goto err;
		}
		inSideFile.Write(bufout, outlen);

		dReturnLen += outlen;
		memset(bufout,0,oblock_size);

		dBolckLen -= inlen;
		if(dBolckLen <= 0) break;

		dProgFinshLen += inlen;
		DrawProg(dProgFinshLen*HUNDRED/dProgLen);
	}

err:
	inSideFile.Close();
//	if(pRsa) RSA_free(pRsa);
	delete [] bufin;
	delete [] bufout;
	if(!ret)
		return -1;
	else return dReturnLen;
}

/*数字签名*/

//功能:初始化对称加密结构
//支持目录加密
//返回值 : 成功TRUE
//		   失败FALSE
//参数 : 
//			strCpName - 算法名称
//			strPwd - 密钥
//			type - 类型 1 - 加密; 0 - 解密
//			Ctx - 返回加密信息体
//调用时机: 加密前
BOOL CEvp::InitSign2(CString strMdName, EVP_MD_CTX & md_Ctx, const char * key/*签名时候用私钥,验证时候用公钥*/,
				  const int keylen, const char * pwd/*私钥密码,为空时候标示输入为公钥*/, X509 *& x509, EVP_PKEY *& pkey)
{
	const EVP_MD *md = NULL;
	if(strMdName.IsEmpty())
	{
//		outStr = "未指定摘要算法";
		return FALSE;
	}

	char outMsg[100] = {0};

	if(pwd != NULL && strlen(pwd) != 0)
	{
		pkey = CCertKey::LoadKey((char *)key,keylen,(char *)pwd,outMsg);
		if (pkey == NULL)
		{ 
			return FALSE;
		}
	}
	else
	{
		x509 = CCertKey::LoadCert((char *)key,keylen,(char *)pwd,outMsg);
		if (x509 == NULL)
		{
			return FALSE;
		}
		pkey = X509_get_pubkey(x509);
		if (pkey == NULL)
		{
			return FALSE;
		}
	}

	//使EVP_Digest系列函数支持所有有效的信息摘要算法
	OpenSSL_add_all_digests();
   
	//根据输入的信息摘要函数的名字得到相应的EVP_MD算法结构
	md = EVP_get_digestbyname(strMdName.GetBuffer(0));  
	strMdName.ReleaseBuffer();
	if(!md) 
	{
//		outStr.Format("未知摘要算法名称 %s",strMdName);
		EVP_cleanup();
		return FALSE;
	}

	//初始化信息摘要结构md_ctx。
	EVP_MD_CTX_init(&md_Ctx);
	
	if(!EVP_SignInit_ex(&md_Ctx,md,NULL))
	{
		EVP_cleanup();
		return FALSE;
	}

	EVP_cleanup();
	return TRUE;
}

//功能:清除对称加密结构
//调用:加密完毕后
void CEvp::CleanSign2(EVP_MD_CTX & md_Ctx, X509 *& px509, EVP_PKEY *& pkey)
{
	if(md_Ctx.digest) 
		EVP_MD_CTX_cleanup(&md_Ctx);

	if(px509) X509_free(px509);
	
	if(pkey) EVP_PKEY_free (pkey);
	
	EVP_cleanup();

	CRYPTO_cleanup_all_ex_data();
//	CRYPTO_mem_leaks(bio_err);
}

DWORD CEvp::Sign2(EVP_MD_CTX * pCtx,
				  EVP_PKEY *& pkey,
				  const CString strFileName,
				  CFile & File, CString & outStr)
{
	BOOL ret=true;
	unsigned char * sig_buf=NULL;
	unsigned int sig_len,len=0;
	unsigned char inbuf[BUFLEN]="";

	CFile inSideFile;//内部文件

	long fileLen=0;//文件长度
	long finishLen=0;//完成长度


	if(pCtx == NULL)
	{
		outStr = "未初始化加密结构";
		return -1;
	}
	if(pkey == NULL)
	{
		outStr = "私钥非法";
		return -1;
	}
	/* Do the signature */
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "不能打开指定文件";
		return -1;
	}


	for(;;)
	{
		len = inSideFile.Read(inbuf, BUFLEN);
		if(len <= 0) break;
		if(!EVP_SignUpdate(pCtx, inbuf, len))
		{
			outStr = "中间过程出错";
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen*HUNDRED/fileLen);
	}

	sig_len = EVP_PKEY_size(pkey);
	sig_buf = new unsigned char[sig_len];
	
	// 簽名後的結果，必須是個夠大視 private key 長度不同而不同，可以 EVP_PKEY_size() 取得
	if(!EVP_SignFinal (pCtx, sig_buf, &sig_len, pkey))
	{
		outStr = "最终过程出错";
		ret = FALSE;
	}
	File.Write(sig_buf, sig_len);
	
err:
	inSideFile.Close();
	
	delete [] sig_buf;
	if(ret)
		return sig_len;
	else
		return -1;
}

BOOL CEvp::VerifySign2(EVP_MD_CTX * pCtx, EVP_PKEY *& pkey, const CString strFileName,
				  CFile & File, DWORD dBolckLen, CString & outStr)
{
	unsigned char * sig_buf=NULL;
	unsigned int sig_len,len=0;
	unsigned char inbuf[BUFLEN]="";
	BOOL ret = FALSE;
	long fileLen=0;//文件长度
	long finishLen=0;//完成长度

	if(pCtx == NULL)
	{
		outStr = "未初始化加密结构";
		return FALSE;
	}
	if(pkey == NULL)
	{
		outStr = "公钥非法";
		return FALSE;
	}

	CFile inSideFile;
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "不能打开指定文件";
		return FALSE;
	}
	for(;;)
	{
		len = File.Read(inbuf, BUFLEN);                  
		if(len <= 0) break;
		if(!EVP_VerifyUpdate (pCtx, inbuf, len))
		{
			outStr = "中间过程出错";
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen*HUNDRED/fileLen);
	}

	sig_len = EVP_PKEY_size(pkey);//这里应该改动
	sig_buf = new unsigned char[sig_len + 1];

	File.Read(sig_buf, sig_len);

	if(!EVP_VerifyFinal (pCtx, sig_buf, sig_len, pkey))
	{
		outStr = "最终过程出错";
		ret=FALSE;
	}
err:
	delete [] sig_buf;
	return ret;
}

