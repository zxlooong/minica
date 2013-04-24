// Evp.h: interface for the CEvp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVP_H__3857E3C8_085F_4691_ABCF_5D7F2E9536BA__INCLUDED_)
#define AFX_EVP_H__3857E3C8_085F_4691_ABCF_5D7F2E9536BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include ".\Control\ProgressCtrlST\ProgressCtrlST.h"

class CEvp  
{
public:
	CEvp();
	virtual ~CEvp();
public:
	void static SetProgRess(CProgressCtrlST * pProgress);
	struct stuCertLink
	{
		char * pCert;//合法用户公钥
		UINT CertLen;//公钥长度
		stuCertLink * Link;
		stuCertLink()
		{
			memset(this,0,sizeof(stuCertLink));
		}
		
		stuCertLink(const stuCertLink & Link)
		{
			this->pCert = Link.pCert;
			this->CertLen = Link.CertLen;
			this->Link = Link.Link;
		}
		
		stuCertLink(const char * pcert, const UINT certLen)
		{
			this->CertLen = certLen;
			this->pCert = new char[strlen(pcert)+1];
			strcpy(this->pCert,pcert);
			Link = NULL;
		}
		
		~stuCertLink()
		{
			delete [] this->pCert;
		}
		
		
		void AddCert(stuCertLink *& Head, const char * pCert, const UINT uCertLen = 0)
		{
			stuCertLink * End = new stuCertLink(pCert, uCertLen);//钥增加的节点
			if(Head == NULL)
			{
				Head = End;
			}
			else 
			{
				stuCertLink * p = Head;
				while(p->Link != NULL)  
					p = p->Link;
				p->Link = End; 
			}
		}
		
		void RemoveAll(stuCertLink *& Head)
		{
			while(Head!=NULL)//遍历链表
			{
				stuCertLink * temp = NULL;
				temp = Head;
				Head = Head->Link;
				delete temp;
			}
		}
	};

	/*算法支持 内存 -> 内存  文件 -> 文件*/

	static BOOL Crypt(const char * cpName, const char * inStream, UINT inLen, 
		   char * outStream, UINT & outLen, char * pwd, const int type,
		   char * outMsg);


	/*消息摘要*/
	static BOOL Digest(const char * mdname/*摘要算法*/,
				const char * infoin/*输入,文件或内存*/,
				UINT inlen/*文件时候为0,内存为长度*/,
				unsigned char * md_value/*返回摘要*/,
				unsigned int * md_len/*摘要长度*/,
				char * outMsg);

	/*数字签名*/
	static BOOL Sign(const char * key/*私钥*/,
			  const int keylen/*0-内存私钥*/,
			  const char * pwd,
			  const char * mdname/*签名算法*/,
			  const char * infoin/*输入文件*/,
			  int inlen,/*输入长度,0->内存*/
			  char * infout/*输出*/,
			  UINT & outlen/*输出长度,输入文件时=0*/,
			  char * outMsg);

	/*数字签名验证*/
	static BOOL VerifySign(const char * cert/*公钥*/,
					const int certlen,
					const char * pwd,
					const char * mdname/*签名算法*/,
					const char * infoin,/*原始信息*/ 
					int inlen/*输入长度0->文件*/,
					char * sign/*签名结果*/,
					char * outMsg);

	/*数字信封 - 多证书 */
	static BOOL Seal(stuCertLink * pCERT/*[in]合法用户公钥链*/,
			  char * cpname/*算法名称*/,
			  char * filein/*输入文件*/, 
			  char * fileout/*输出文件*/,
			  char * outMsg);

	/*拆封数字信封 - 多证书*/
	static BOOL OpenSeal(char * key/*私钥*/, int keylen, char * pwd/*私钥密码*/, 
				   char * cpname/*算法名称*/, char * filein/*输入文件*/, 
				   char * fileout/*输出文件*/,char * outMsg);

	//公钥加密
	static BOOL RSAPubEnc(const BYTE * pCertBuf/*[in]公钥*/,
				   const UINT nCertLen/*[in]公钥长度*/,
				   const char * pPwd/*[in]公钥密码,只针对PFX包文件*/,
				   const BYTE * pInStream/*[in]输入文件或内存*/,
				   DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,
				   BYTE * pOutStream/*[out]加密后的数据*/,
				   DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out加密后数据长度*/,
				   char * OperMsg/*[out]返回操作错误信息*/);

	//私钥解密
	static BOOL RSAPrivDec(const BYTE * pKeyBuf/*[in]私钥*/,
					const UINT nKeyLen/*[in]私钥长度*/,
					const char * pPwd/*[in]私钥密码*/,
					const BYTE * pInStream/*[in]输入文件或内存*/,
					DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,
					BYTE * pOutStream/*[out]解密后的数据*/,
					DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out解密后数据长度*/,
					char * OperMsg/*[out]返回操作错误信息*/);

	//私钥加密(签名)
	static BOOL RSAPrivEnc(const BYTE * pKeyBuf/*[in]私钥*/,
					const UINT nKeyLen/*[in]私钥长度*/,
					const char * pPwd/*[in]私钥密码*/,
					const BYTE * pInStream/*[in]输入文件或内存*/,
					DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,
					BYTE * pOutStream/*[out]加密后的数据*/,
					DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out加密后数据长度*/,
					char * OperMsg/*[out]返回操作错误信息*/);

	//公钥解密
	static BOOL RSAPubDec(const BYTE * pCertBuf/*[in]公钥*/,
				   const UINT nCertLen/*[in]公钥长度*/,
				   const char * pPwd/*[in]公钥密码,只针对PFX包文件*/,
				   const BYTE * pInStream/*[in]输入文件或内存*/,
				   DWORD nInlen/*[in]数据长度,为0表示pInStream为文件名*/,
				   BYTE * pOutStream/*[out]加密后的数据*/,
				   DWORD & nOutlen/*[in,out]in为0时候表示pOutStream为文件名,out加密后数据长度*/,
				   char * OperMsg/*[out]返回操作错误信息*/);

	//得到公钥加密后的缓冲区长度 加密 117 -> 128, 解密 128 -> 117 ,所以解密后缓冲区一定小于源缓冲区
	static DWORD GetEncLen(const char * pCertBuf, const UINT nCertLen, const DWORD nInlen);

	//目录操作
	static BOOL InitCrypt2(const CString strCpName, CString strPwd, const int type, EVP_CIPHER_CTX & cipherCtx);
	static DWORD Crypt2(EVP_CIPHER_CTX * pcipherCtx, const CString & strFileName,
						CFile & File, DWORD dBolckLen, CString & outStr);
	static void CleanCrypt2(EVP_CIPHER_CTX & cipherCtx);

	
	static BOOL InitDigest2(CString strMdName, EVP_MD_CTX & mdCtx);
	static DWORD Digest2(EVP_MD_CTX * mdCtx, const CString strFileName,
			CFile & File, CString & outStr);
	static BOOL VerifyDigest2(EVP_MD_CTX * mdCtx, const CString strFileName,
			CFile & File, DWORD dBolckLen, CString & outStr);
	static void CleanDigest2(EVP_MD_CTX & mdCtx);

	static BOOL InitSign2(CString strMdName, EVP_MD_CTX & md_Ctx, const char * key,
				  const int keylen, const char * pwd, X509 *& x509, EVP_PKEY *& pkey);
	static DWORD Sign2(EVP_MD_CTX * pCtx, EVP_PKEY *& pkey, const CString strFileName,
				  CFile & File, CString & outStr);
	static BOOL VerifySign2(EVP_MD_CTX * pCtx, EVP_PKEY *& pkey, const CString strFileName,
				  CFile & File, DWORD dBolckLen, CString & outStr);
	static void CleanSign2(EVP_MD_CTX & md_Ctx, X509 *& px509, EVP_PKEY *& pkey);


	static BOOL InitSeal2(stuCertLink * pCERT, const CString strCpName,
			  CFile & File, EVP_CIPHER_CTX & eCtx, CString & outStr);
	static DWORD Seal2(EVP_CIPHER_CTX * pCtx,const CString strFileName, CFile & File, CString & outStr);
	static void CleanSeal2(EVP_CIPHER_CTX & sealCtx);

	static BOOL InitOpenSeal2(char * key, int keylen, char * pwd, const CString strCpName,
			  CFile & File, EVP_CIPHER_CTX & eCtx, CString & outStr);
	static DWORD OpenSeal2(EVP_CIPHER_CTX & eCtx, CFile & File, DWORD dBolckLen, 
				const CString strFileName, CString & outStr);
	static void CleanOpenSeal2(EVP_CIPHER_CTX & eCtx);

	static BOOL GetPubRsa(const BYTE * pCertBuf, const UINT nCertLen, RSA *& pRsa);
	static BOOL GetPrivRsa(const BYTE * pKeyBuf,const UINT nKeyLen, const char * pPwd, RSA *& pRsa);

	static void FreeRsa(RSA *& pRsa);

	static DWORD RSAPubEnc2(RSA * pRsa, const CString strFileName, CFile & File, CString & outStr);
	static DWORD RSAPrivDec2(RSA * pRsa, CFile & File, DWORD dBolckLen, const CString strFileName, CString & outStr);

	static DWORD RSAPrivEnc2(RSA * pRsa, const CString strFileName, CFile & File, CString & outStr);
	static DWORD RSAPubDec2(RSA * pRsa, CFile & File, DWORD dBolckLen, const CString strFileName, CString & outStr);

private:
	static void DrawProg(float fPot);
	static CProgressCtrlST * m_pProgress;
};

#endif // !defined(AFX_EVP_H__3857E3C8_085F_4691_ABCF_5D7F2E9536BA__INCLUDED_)
