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
		char * pCert;//�Ϸ��û���Կ
		UINT CertLen;//��Կ����
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
			stuCertLink * End = new stuCertLink(pCert, uCertLen);//Կ���ӵĽڵ�
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
			while(Head!=NULL)//��������
			{
				stuCertLink * temp = NULL;
				temp = Head;
				Head = Head->Link;
				delete temp;
			}
		}
	};

	/*�㷨֧�� �ڴ� -> �ڴ�  �ļ� -> �ļ�*/

	static BOOL Crypt(const char * cpName, const char * inStream, UINT inLen, 
		   char * outStream, UINT & outLen, char * pwd, const int type,
		   char * outMsg);


	/*��ϢժҪ*/
	static BOOL Digest(const char * mdname/*ժҪ�㷨*/,
				const char * infoin/*����,�ļ����ڴ�*/,
				UINT inlen/*�ļ�ʱ��Ϊ0,�ڴ�Ϊ����*/,
				unsigned char * md_value/*����ժҪ*/,
				unsigned int * md_len/*ժҪ����*/,
				char * outMsg);

	/*����ǩ��*/
	static BOOL Sign(const char * key/*˽Կ*/,
			  const int keylen/*0-�ڴ�˽Կ*/,
			  const char * pwd,
			  const char * mdname/*ǩ���㷨*/,
			  const char * infoin/*�����ļ�*/,
			  int inlen,/*���볤��,0->�ڴ�*/
			  char * infout/*���*/,
			  UINT & outlen/*�������,�����ļ�ʱ=0*/,
			  char * outMsg);

	/*����ǩ����֤*/
	static BOOL VerifySign(const char * cert/*��Կ*/,
					const int certlen,
					const char * pwd,
					const char * mdname/*ǩ���㷨*/,
					const char * infoin,/*ԭʼ��Ϣ*/ 
					int inlen/*���볤��0->�ļ�*/,
					char * sign/*ǩ�����*/,
					char * outMsg);

	/*�����ŷ� - ��֤�� */
	static BOOL Seal(stuCertLink * pCERT/*[in]�Ϸ��û���Կ��*/,
			  char * cpname/*�㷨����*/,
			  char * filein/*�����ļ�*/, 
			  char * fileout/*����ļ�*/,
			  char * outMsg);

	/*��������ŷ� - ��֤��*/
	static BOOL OpenSeal(char * key/*˽Կ*/, int keylen, char * pwd/*˽Կ����*/, 
				   char * cpname/*�㷨����*/, char * filein/*�����ļ�*/, 
				   char * fileout/*����ļ�*/,char * outMsg);

	//��Կ����
	static BOOL RSAPubEnc(const BYTE * pCertBuf/*[in]��Կ*/,
				   const UINT nCertLen/*[in]��Կ����*/,
				   const char * pPwd/*[in]��Կ����,ֻ���PFX���ļ�*/,
				   const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
				   DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,
				   BYTE * pOutStream/*[out]���ܺ������*/,
				   DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
				   char * OperMsg/*[out]���ز���������Ϣ*/);

	//˽Կ����
	static BOOL RSAPrivDec(const BYTE * pKeyBuf/*[in]˽Կ*/,
					const UINT nKeyLen/*[in]˽Կ����*/,
					const char * pPwd/*[in]˽Կ����*/,
					const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
					DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,
					BYTE * pOutStream/*[out]���ܺ������*/,
					DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
					char * OperMsg/*[out]���ز���������Ϣ*/);

	//˽Կ����(ǩ��)
	static BOOL RSAPrivEnc(const BYTE * pKeyBuf/*[in]˽Կ*/,
					const UINT nKeyLen/*[in]˽Կ����*/,
					const char * pPwd/*[in]˽Կ����*/,
					const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
					DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,
					BYTE * pOutStream/*[out]���ܺ������*/,
					DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
					char * OperMsg/*[out]���ز���������Ϣ*/);

	//��Կ����
	static BOOL RSAPubDec(const BYTE * pCertBuf/*[in]��Կ*/,
				   const UINT nCertLen/*[in]��Կ����*/,
				   const char * pPwd/*[in]��Կ����,ֻ���PFX���ļ�*/,
				   const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
				   DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,
				   BYTE * pOutStream/*[out]���ܺ������*/,
				   DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
				   char * OperMsg/*[out]���ز���������Ϣ*/);

	//�õ���Կ���ܺ�Ļ��������� ���� 117 -> 128, ���� 128 -> 117 ,���Խ��ܺ󻺳���һ��С��Դ������
	static DWORD GetEncLen(const char * pCertBuf, const UINT nCertLen, const DWORD nInlen);

	//Ŀ¼����
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
