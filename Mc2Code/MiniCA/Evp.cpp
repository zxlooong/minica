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

//��������:�ԳƼӽ���
//����:		[in] cpName - �㷨����
//			[in] inStream - �����ļ����ڴ�����
//			[in] inLen - ���볤��,0->��ʾ����Ϊ�ļ�
//			[in] outStream - ���Ϊ�ļ������ڴ�����
//			[in,out] outLen - ����Ϊ����ڴ泤��,���Ϊʵ�ʼ��ܽ������
//			[in] char * pwd - ���ڳ�ʼiv������
//			[in] type - 1-���ܡ�0-����
//			[out] outMsg - �����Ϣ
BOOL CEvp::Crypt(const char * cpName, const char * inStream, UINT inLen, 
		   char * outStream, UINT & outLen, char * pwd, const int type,
		   char * outMsg)
{
	unsigned char inbuf[BUFLEN]="";
	unsigned char outbuf[BUFLEN + EVP_MAX_BLOCK_LENGTH]="";//�����㷨��Ŀ鳤�ȡ�
	unsigned char key[EVP_MAX_KEY_LENGTH]="";//�㷨���KEY����
    unsigned char iv[EVP_MAX_IV_LENGTH]="";//�㷨���IV����
	int infilelen=0, outlen=0;
	BOOL ret=true;
	const EVP_CIPHER *cipher=NULL;
	EVP_CIPHER_CTX ctx;
	memset(&ctx,0,sizeof(ctx));
	long fileLen=0;//�ļ�����
	long finishLen=0;//��ɳ���
	int uMaxMem = outLen;	//����Ϊ����,������UINT
	outLen = 0;
	UINT len = 0;

	FILE * outfd=NULL,* infd=NULL;
	
	if(inLen==0&&(strlen(inStream)==0||strlen(outStream)==0))
	{
		strcpy(outMsg,"NO specify input or output file");
		return FALSE;
	}
	
	if(inLen==0)//�ļ�
	{
		if ((infd = fopen (inStream, "rb")) == NULL)//ԭ��
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//�õ��ļ�����
		if ((outfd = fopen (outStream, "wb")) == NULL)//����
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
	
	//��ʾ�a���� key �ǽo��һ�N cipher �õ�
	//�^������ʹ�õ� hash ���㷨,
	//�Á���ܵ� salt�����ٞ�˂� bytes����t���NULL,
	//�ظ��״� hashing �Ą���������������r�g����������Ҳ����ȫ
	if(!EVP_BytesToKey(cipher,EVP_md5(),NULL,(unsigned char *)pwd,
		strlen(pwd),1,key,iv))
	{
		strcpy(outMsg,"Crypt��ʼ��key or iv ʧ��");
		ret=FALSE;
		goto err;
	}
	
	EVP_CIPHER_CTX_init(&ctx);//��ʼ��һ��EVP_CIPHER_CTX�ṹ��
	
	//typeΪ1������ܣ����typeΪ0������ܣ����type�ǣ�1���򲻸ı�����
	if(!EVP_CipherInit_ex(&ctx, cipher, NULL/*NULLʹ��ȱʡ��ʵ���㷨*/,key,iv,type))//��ʼ��
	{
		strcpy(outMsg,"Crypt��ʼ���ӽ��ܽṹ��ʧ��");
		ret=FALSE;
		goto err;		
	}
	
	//�ú������м����㷨�ṹEVP_CIPHER_CTX��Կ���ȵ����á�
	//����㷨��һ����Կ���ȹ̶����㷨��
	//��ô������õ���Կ���ȸ����̶��ĳ��Ȳ�һ�£��ͻ��������
	//	EVP_CIPHER_CTX_set_key_length(&ctx, 10); 		
	//	EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, type);
	if(inLen==0)//�ļ�
	{
		for(;;)
		{                       
			infilelen = fread(inbuf, sizeof(char), BUFLEN, infd);
			if(infilelen <= 0) break;
			if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, infilelen))//���g�^�� 
			{
				strcpy(outMsg,"Crypt�м���̴���");
				ret=FALSE;
				goto err;
			}
			fwrite(outbuf, sizeof(char), outlen, outfd);
			finishLen+=infilelen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}               
		if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))    //��K���E-�������Final����һ������
		{                       
			strcpy(outMsg,"Crypt���չ��̴���"); 
			ret=FALSE;
			goto err;
		}               
		fwrite(outbuf, sizeof(char), outlen, outfd);
	}
	else//�ڴ�
	{
		fileLen = inLen;
		
		for(UINT i=0;;i++)
		{
			//ÿ��1024
			len=(inLen>BUFLEN)?BUFLEN:inLen;
			inLen-=len;
			if(!EVP_CipherUpdate(&ctx, outbuf, &outlen,(UCHAR *)(inStream+i*BUFLEN), len))//���g�^��
			{
				strcpy(outMsg,"Crypt�м���̴���");
				ret=FALSE;
				goto err;
			}
			//���㻺���������Ƿ������ɽ�����ݼ�
			uMaxMem -= outlen;
			if(uMaxMem < 0)
			{
				strcpy(outMsg,"��������С,�������ɲ������"); 
				ret = FALSE;
				goto err;
			}

			memcpy(outStream + outLen, outbuf, outlen);//���������ú���
			outLen+=outlen;
			if(inLen <= 0) break;          
		}		

		if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)) //��K���E-�������Final����һ������
		{   
			strcpy(outMsg,"Crypt���չ��̴���"); 
			ret=FALSE;
			goto err;
		}               
	
		uMaxMem -= outlen;
		if(uMaxMem < 0)
		{
			strcpy(outMsg,"��������С,�������ɲ������"); 
			ret = FALSE;
			goto err;
		}
		memcpy(outStream + outLen,outbuf,outlen);//���������ú���		
		outLen += outlen;
		
	}
err:
	if(infd!=NULL)
		fclose(infd);
	if(outfd!=NULL)
		fclose(outfd);
	if(ctx.cipher) EVP_CIPHER_CTX_cleanup(&ctx); //���������Ϣ�ͷ��ڴ�
	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
	return ret;
}

/*��ϢժҪ*/ 
BOOL CEvp::Digest(const char * mdname/*ժҪ�㷨*/,const char * infoin/*����,�ļ����ڴ�*/,
			UINT inlen/*�ļ�ʱ��Ϊ0,�ڴ�Ϊ����*/,unsigned char * md_value/*����ժҪ*/,
			unsigned int * md_len/*ժҪ����*/, char * outMsg)
{

const EVP_MD *md=NULL;
	char inbuf[BUFLEN]="";
	UINT len=0;
	BOOL ret=true;
	EVP_MD_CTX md_ctx;
	memset(&md_ctx,0,sizeof(md_ctx));
	FILE * infd=NULL;
	long fileLen=0;//�ļ�����
	long finishLen=0;//��ɳ���
	
	if(strlen(infoin)==0)
	{
		strcpy(outMsg,"NO specify input");
		return FALSE;
	}
	
	if(inlen==0)//����Ϊ�ļ�
	{
		if ((infd = fopen (infoin, "rb")) == NULL)//ԭ��
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		fileLen=filelength(fileno(infd));//�õ��ļ�����
	}

	
	//ʹEVP_Digestϵ�к���֧��������Ч����ϢժҪ�㷨
	OpenSSL_add_all_digests();
    
	//�����������ϢժҪ���������ֵõ���Ӧ��EVP_MD�㷨�ṹ
	md = EVP_get_digestbyname(mdname);  
	if(!md) 
	{
		sprintf(outMsg,"Unknown message digest %s",mdname);
		ret=FALSE;
		goto err;
	}
	//��ʼ����ϢժҪ�ṹmd_ctx��
	EVP_MD_CTX_init(&md_ctx);
	
	//ʹ��md���㷨�ṹ����mdctx�ṹ��implΪNULL����ʹ��ȱʡʵ�ֵ��㷨��openssl�����ṩ����ϢժҪ�㷨��
	if(!EVP_DigestInit_ex(&md_ctx, md, NULL/*impl*/))
	{
		strcpy(outMsg,"Digest��ʼ���㷨�ṹ����");
		ret=FALSE;
		goto err;
	}
	
	//��ϢժҪ����
	if(inlen==0)//����Ϊ�ļ�
	{
		for(;;)
		{
			len = fread(inbuf, 1, BUFLEN, infd);
			if(len <= 0) break;          
			if(!EVP_DigestUpdate(&md_ctx, inbuf, len))
			{
				strcpy(outMsg,"Digest�м���̴���");
				ret=FALSE;
				goto err;
			}
			finishLen+=len;
			DrawProg(finishLen * HUNDRED / fileLen);
		}
	}
	else//�ڴ�����
	{
		for(UINT i=0;;i++)
		{
			//ÿ��BUFLEN
			len=(inlen>BUFLEN)?BUFLEN:inlen;
			inlen-=len;
			if(!EVP_DigestUpdate(&md_ctx, (infoin+i*BUFLEN), len))
			{
				strcpy(outMsg,"Digest�м���̴���");
				ret=FALSE;
				goto err;
			}
			if(inlen <= 0) break;          
		}		
	}
	//�����ϢժҪ������̣�����ɵ�ժҪ��Ϣ�洢��md_value����,������Ϣ�洢��md_len����
	if(!EVP_DigestFinal_ex(&md_ctx, md_value, md_len))
	{	
		strcpy(outMsg,"Digest���չ��̴���");
		ret=FALSE;
	}
	
err:
	//ʹ�øú����ͷ�mdctxռ�õ���Դ�����ʹ��_exϵ�к��������Ǳ�����õġ�
	if(infd!=NULL)
		fclose(infd);
	if(md_ctx.digest) EVP_MD_CTX_cleanup(&md_ctx);
	EVP_cleanup();
	return ret;
}

/*����ǩ��*/
BOOL CEvp::Sign(const char * key/*˽Կ*/,const int keylen/*0-�ڴ�˽Կ*/,const char * pwd,
				const char * mdname/*ǩ���㷨*/,const char * infoin/*�����ļ�*/,
				int inlen,/*���볤��,0->�ڴ�*/char * infout/*���*/,
				UINT & outlen/*�������,�����ļ�ʱ=0*/,char * outMsg)
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
	long fileLen=0;//�ļ�����
	long finishLen=0;//��ɳ���

	BOOL bType=inlen;

	if(bType==0&&(strlen(infoin)==0||strlen(infout)==0))//�����ļ�ʱ��
	{
		strcpy(outMsg,"NO specify input or output file");
		return FALSE;
	}
	if(bType==0)//�ļ�
	{
		if ((infd = fopen (infoin, "rb")) == NULL)//ԭ��
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		if ((outfd = fopen (infout, "wb")) == NULL)//����
		{
			strcpy(outMsg,"open output file error");
			fclose(infd);
			return FALSE;
		}
		fileLen=filelength(fileno(infd));//�õ��ļ�����
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
		strcpy(outMsg,"��ʼ���㷨�ṹ����");
		ret=FALSE;
		goto err;
	}

	if(bType==0)//�ļ�
	{	
		for(;;)
		{
			len = fread(inbuf, sizeof(char), BUFLEN, infd);
			if(len <= 0) break;
			if(!EVP_SignUpdate(&md_ctx, inbuf, len))
			{
				sprintf(outMsg,"�м���̳���");
				ret=FALSE;
				goto err;
			}
			finishLen+=len;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
	}

	else//�ڴ�����
	{
		for(UINT i=0;;i++)
		{
			//ÿ��BUFLEN
			len=(inlen>BUFLEN)?BUFLEN:inlen;
			inlen-=len;
			if(!EVP_SignUpdate(&md_ctx, (infoin+i*BUFLEN), len))
			{
				strcpy(outMsg,"�м���̴���");
				ret=FALSE;
				goto err;
			}
			if(inlen <= 0) break;          
		}		
	}

	sig_len =EVP_PKEY_size(pkey);
	sig_buf=new unsigned char[sig_len];
	
	// ������ĽY��������ǂ����ҕ private key �L�Ȳ�ͬ����ͬ������ EVP_PKEY_size() ȡ��
	if(!EVP_SignFinal (&md_ctx, sig_buf, &sig_len, pkey))
	{
		sprintf(outMsg,"���չ��̳���");
		ret=FALSE;
	}
	else if(bType!=0)//�ڴ�
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

/*����ǩ����֤*/
BOOL CEvp::VerifySign(const char * cert/*��Կ*/,const int certlen,
					  const char * pwd,const char * mdname/*ǩ���㷨*/,
					  const char * infoin,/*ԭʼ��Ϣ*/ 
					  int inlen/*���볤��0->�ļ�*/,char * sign/*ǩ�����*/,
					  char * outMsg)
{
	const EVP_MD *md=NULL;
	unsigned char inbuf[BUFLEN]="";
	BOOL ret=true;
	unsigned char * sig_buf=NULL;
	unsigned int sig_len,len=0;
	FILE * outfd=NULL,* infd=NULL;
	long fileLen=0;//�ļ�����
	long finishLen=0;//��ɳ���
	X509 *	x509=NULL;
	EVP_PKEY * pcert=NULL;
	EVP_MD_CTX md_ctx;
	memset(&md_ctx,0,sizeof(md_ctx));
	
	BOOL bType=inlen;

	if(bType==0&&(strlen(infoin)==0||strlen(sign)==0))//�����ļ�ʱ��
	{
		strcpy(outMsg,"NO specify input file");
		return FALSE;
	}
	if(bType==0)//�ڴ�
	{
		if ((infd = fopen (infoin, "rb")) == NULL)//ԭ��
		{
			strcpy(outMsg,"open input file error");
			return FALSE;
		}
		if ((outfd = fopen (sign, "rb")) == NULL)//����
		{
			strcpy(outMsg,"open output file error");
			fclose(infd);
			return FALSE;
		}
		fileLen=filelength(fileno(infd));//�õ��ļ�����
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
		strcpy(outMsg,"��ʼ���㷨�ṹ����");
		ret=FALSE;
		goto err;
	}

	if(bType==0)//�ļ�
	{		
		for(;;)
		{
			len = fread(inbuf, sizeof(char), BUFLEN, infd);                  
			if(len <= 0) break;
			if(!EVP_VerifyUpdate (&md_ctx, inbuf, len))
			{
				strcpy(outMsg,"�м���̳���");
				ret=FALSE;
				goto err;
			}
			finishLen+=len;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
	}	

	else//�ڴ�����
	{
		for(UINT i=0;;i++)
		{
			//ÿ��BUFLEN
			len=(inlen>BUFLEN)?BUFLEN:inlen;
			inlen-=len;
			if(!EVP_VerifyUpdate(&md_ctx, (infoin+i*BUFLEN), len))
			{
				strcpy(outMsg,"�м���̴���");
				ret=FALSE;
				goto err;
			}
			if(inlen <= 0) break;          
		}		
	}

	sig_len =EVP_PKEY_size(pcert);//����Ӧ�øĶ�
	sig_buf=new unsigned char[sig_len];
	if(bType==0)//�ļ�
	{		
		fread(sig_buf,sizeof(char),sig_len,outfd);
	}
	else
	{
		memcpy(sig_buf,sign,sig_len);
	}
	if(!EVP_VerifyFinal (&md_ctx, sig_buf, sig_len, pcert))
	{
		strcpy(outMsg,"���չ��̳���");
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


/*�����ŷ� - ��֤�� */
//�ļ��ṹ
//			���ܺ���Ҫ��Ϣ����	DWORD
//			���ܺ���Ҫ��Ϣ���� CRCУ����	UINT
//			���ܺ���Ҫ��Ϣ
//			-------------------------------��Ҫ��Ϣ��ʼ--------------------------------
//			��Կ����n
//			n��
//				��Կ���ܵĶԳ���Կ����
//				��Կ���ܵĶԳ���Կ����
//			����IV
//			-------------------------------��Ҫ��Ϣ����--------------------------------
//			�����ļ�����

BOOL CEvp::Seal(stuCertLink * pCERT/*[in]�Ϸ��û���Կ��*/,
		  char * cpname/*�㷨����*/,
		  char * filein/*�����ļ�*/, 
		  char * fileout/*����ļ�*/,
		  char * outMsg)//д��
{
	//����������֣�seeded����
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
 	DWORD fileLen=0;//�ļ�����
	DWORD finishLen=0;//��ɳ���
	UINT uCrc16 = 0; //���ܺ���Ҫ��ϢCRC��
	DWORD dOutLen = 0;//�ڴ��ļ�����
	BYTE * pOut = NULL; //�ڴ��ļ�������
	DWORD dFileInfoLen = 0;

	if(strlen(filein)==0||strlen(fileout)==0)
	{
		strcpy(outMsg,"NO specify input or output file");
		return false;
	}

	if ((infd = fopen (filein, "rb")) == NULL)//ԭ��
	{
		strcpy(outMsg,"open input file error");
		return false;
	}
	if ((outfd = fopen (fileout, "wb")) == NULL)//����
	{
		strcpy(outMsg,"open output file error");
		fclose(infd);
		return false;
	}

	//���ؼ��ܹ�Կ
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_CERT),"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert = LoadResource(NULL,hRsrc);
	LPSTR lpCert = (LPSTR)LockResource(hgCert);	


	CMemFile MemFile;				//���ڴ����Ҫ��Ϣ���ڴ��ļ�

	DWORD dInLen = 0;
	BYTE * pIn = NULL;				//����MemFile.Dectch();


	fileLen = filelength(fileno(infd));//�õ��ļ�����

	OpenSSL_add_all_algorithms();

	int nCountCert = 0;	//֤�����

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
			sprintf(outMsg,"���ص�%d��֤��ʧ��",nCountCert);
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
		ekeylen,//������� ek �L��
		iv,//�Զ�����
		pubKey,//���� ek �õ� public key(s)
		nCountCert))
	{
		ret=false;
		goto err;
	}

	MemFile.Write(&nCountCert, sizeof(int));	//1 - д�빫Կ����

	//fwrite((char*)&nCountCert,sizeof(char), sizeof(int),outfd);//д�빫Կ����
	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Write(&ekeylen[index], sizeof(int));
	//	fwrite(&ekeylen[index],sizeof(char), sizeof(int),outfd);//д��n������Կ���ܵĶԳ���Կ����
		MemFile.Write(ekey[index], ekeylen[index]);
	//	fwrite(ekey[index],sizeof(char) ,ekeylen[index],outfd);//д��n������Կ���ܵĶԳ���Կ
	}

	MemFile.Write(iv, sizeof(iv));
//	fwrite(iv,sizeof(char),sizeof(iv),outfd);//д������


	//������ܽ�� ����+1 �ڴ�
	dFileInfoLen = CEvp::GetEncLen(lpCert, lenCert, MemFile.GetLength());

	dOutLen = dFileInfoLen + 1;
	pOut = new BYTE[dOutLen];

	dInLen = MemFile.GetLength();
	pIn = MemFile.Detach();

	//������Ҫ��Ϣ
	if(!CEvp::RSAPubEnc((BYTE *)lpCert, lenCert, "", pIn, 
		dInLen, pOut, dOutLen, outMsg))
	{
		//����ʧ��
		ret=false;
		goto err;
	}

	//�õ����ܺ󳤶ȵ�CRCУ��
	if(!CRC::GetCrc16((char*)&dOutLen, sizeof(DWORD), uCrc16))
	{
		//����ʧ��
		ret=false;
		goto err;
	}

	//д����ܺ���Ҫ��Ϣ���Ⱥ�CRCУ��ͼ��ܺ���Ҫ��Ϣ����
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
			strcpy(outMsg,"�м���̳���");
			ret=false;
			goto err;
		}
		fwrite(ebuf,sizeof(char),ebuflen,outfd);
		finishLen += readlen;
		DrawProg(finishLen*HUNDRED/fileLen);//ǰ���

	}
	
	if(!EVP_SealFinal(&eCtx, ebuf, &ebuflen))
	{
		strcpy(outMsg,"���չ��̳���");
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

/*��������ŷ�*/
BOOL CEvp::OpenSeal(char * key/*˽Կ*/, int keylen, char * pwd/*˽Կ����*/, 
			   char * cpname/*�㷨����*/, char * filein/*�����ļ�*/, 
			   char * fileout/*����ļ�*/,char * outMsg)
{
	unsigned char buf[BUFLEN]="";
	unsigned char ebuf[BUFLEN + EVP_MAX_BLOCK_LENGTH]="";
	unsigned char iv[EVP_MAX_IV_LENGTH]="";
	const EVP_CIPHER *cipher=NULL;
	int ebuflen=0,readlen=0;
	bool ret=true;
	FILE *outfd,*infd;
	DWORD fileLen=0;//�ļ�����
	DWORD finishLen=0;//��ɳ���
	int nCountCert = 0;	//��Կ����
	int index = 0;
	UCHAR * ekey [128] = {0}; 
	int ekeylen[128];
	BOOL bInit = FALSE;//�Ƿ��ʼ���ɹ�
	DWORD dFileInfoLen = 0;//���ܺ���Ҫ��Ϣ����
	UINT uCrc16 = 0; //���ܺ���Ҫ��ϢCRC��


	EVP_CIPHER_CTX eCtx;
	memset(&eCtx,0,sizeof(eCtx));
	EVP_PKEY * pkey=NULL;

	if(strlen(filein)==0||strlen(fileout)==0)
	{
		strcpy(outMsg,"NO specify input or output file");
		return false;
	}
	
    if ((infd = fopen (filein, "rb")) == NULL)//ԭ��
	{
		strcpy(outMsg,"open input file error");
		return false;
	}
	if ((outfd = fopen (fileout, "wb")) == NULL)//����
	{
		strcpy(outMsg,"open output file error");
		fclose(infd);
		return false;
	}
	fileLen = filelength(fileno(infd));//�õ��ļ�����

	OpenSSL_add_all_algorithms();//digests   and ciphers
	/* Read private key */
	pkey = CCertKey::LoadKey(key,keylen,pwd,outMsg);
	if (pkey == NULL)
	{ 
		fclose(infd);
		fclose(outfd);
		return FALSE;
	}

	//�õ���Ҫ��Ϣ���Ⱥ�CRCУ����
	readlen = fread(&dFileInfoLen, sizeof(char),sizeof(DWORD),infd);
	finishLen += readlen;

	readlen = fread(&uCrc16, sizeof(char),sizeof(UINT),infd);
	finishLen += readlen;

	if(!CRC::CheckCrc((char *)&dFileInfoLen, sizeof(DWORD), uCrc16))
	{
		strcpy(outMsg ,"�ļ�ͷCRCУ�����");
		return FALSE;
	}

	//���ٻ�����,���ɼ��ܺ����Ҫ�ļ���Ϣ
	BYTE * pIn = NULL;
	pIn = new BYTE[dFileInfoLen + 1];
	readlen = fread(pIn, sizeof(char), dFileInfoLen, infd);
	finishLen += readlen;

	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_KEY),"CERT");
	DWORD lenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey = LoadResource(NULL,hRsrc);
	LPSTR lpKey = (LPSTR)LockResource(hgKey);

	DWORD dOutLen = dFileInfoLen;

	//���ٻ�����,���ɼ��ܺ����Ϣ��
	BYTE * pOut = NULL;
	pOut = new BYTE[dOutLen];
	if(!CEvp::RSAPrivDec((BYTE *)lpKey, lenKey, pwd, pIn, 
		dFileInfoLen, pOut, dOutLen, outMsg))
	{
		//����ʧ��
		strcpy(outMsg ,"��Ҫ��Ϣ����ʧ��");
		fclose(infd);
		fclose(outfd);
		delete [] pIn;
		delete [] pOut;
		return FALSE;
	}

	CMemFile MemFile;
	MemFile.Attach(pOut, dOutLen);

	//�����ǹ�Կ����
	MemFile.Read(&nCountCert, sizeof(int));

	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Read(&ekeylen[index], sizeof(int));
		ekey[index] = new unsigned char[ekeylen[index]];
		MemFile.Read(ekey[index], ekeylen[index]);
	}

	MemFile.Read(iv, sizeof(iv));

	MemFile.Close();

	//�����ļ��Ѿ�ƫ�Ƶ����ܺ���ļ�
	
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
		sprintf(outMsg,"ָ��֤�鲻�ܽ��ܴ��ļ�");
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
			sprintf(outMsg,"�м���̴���");
			ret=false;
			goto err;
		}
		fwrite(ebuf,sizeof(char),ebuflen,outfd);
		finishLen += readlen;
		DrawProg(finishLen*HUNDRED/fileLen);
	}

	if(!EVP_OpenFinal(&eCtx, ebuf, &ebuflen))
	{
		sprintf(outMsg,"���չ��̴���");
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

//��Կ����
BOOL CEvp::RSAPubEnc(const BYTE * pCertBuf/*[in]��Կ*/,const UINT nCertLen/*[in]��Կ����*/,
			const char * pPwd/*[in]��Կ����,ֻ���PFX���ļ�*/,const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
			DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,BYTE * pOutStream/*[out]���ܺ������*/,
			DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
			char * OperMsg/*[out]���ز���������Ϣ*/)
{
	UINT iblock_size = 0, //����鳤��
		oblock_size = 0; //����鳤��
	unsigned char * bufin=NULL,
				  * bufout=NULL;
	int inlen = 0,
		outlen = 0;
	BOOL ret = TRUE;
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���
	X509 * x509 = NULL;
	EVP_PKEY * pcert = NULL;
	RSA * rsa = NULL;
	int memtemplen = 0; //����ڴ�ƫ��
	UINT len = 0; //�����ڴ�ʣ�೤��

	int uMaxMem = nOutlen;	//����Ϊ����,������UINT


	BOOL bOutType = FALSE;//����ļ�����,�ڴ�-FALSE,�ļ�-TRUE;

	if(nOutlen == 0) 
	{
		//����ļ����Ƿ��ܹ���,�Է�pOutStreamΪ�ļ�
		bOutType = TRUE;
	}

	FILE * outfd = NULL,
		* infd = NULL;

	if(nInlen == 0)//����Ϊ�ļ�
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"δָ�������ļ�");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//�õ��ļ�����
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//���Ϊ�ļ�
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"δָ������ļ�");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pOutStream);
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
		sprintf(OperMsg,"ȡ�ù�Կ��Կʧ��");
		ret = FALSE;
		goto err;
	}

    if (!(rsa = EVP_PKEY_get1_RSA(pcert)))
	{
		sprintf(OperMsg,"ȡ��RSA��Կʧ��");
		ret=FALSE;
		goto err;
    }

	iblock_size = BN_num_bytes(rsa->n) - 11;//Ԥ���ܳ���,117
	oblock_size = BN_num_bytes(rsa->n);//���ܺ󳤶�,128
	bufin = new unsigned char[iblock_size];
	memset(bufin,0,iblock_size);
	bufout = new unsigned char[oblock_size];
	memset(bufout,0,oblock_size);
	if(nInlen==0)//�ļ�
	{	
		for(;;)
		{
			inlen = fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;
			outlen = RSA_public_encrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen, bufout, outlen);//���������ú���
				memtemplen += outlen;
			}
			finishLen += inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}

	else//�ڴ�����
	{
		for(UINT i=0;;i++)
		{
			//ÿ�� iblock_size ��ʵ�ʳ���
			len = (nInlen>iblock_size)?iblock_size:nInlen;//�ڴ����򳤶�
			nInlen -= len;
			outlen=RSA_public_encrypt(len,(UCHAR *)(pInStream+i*iblock_size),bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}

				memcpy(pOutStream + memtemplen,bufout,outlen);//���������ú���
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


//˽Կ����	
BOOL CEvp::RSAPrivDec(const BYTE * pKeyBuf/*[in]˽Կ*/,const UINT nKeyLen/*[in]˽Կ����*/,
			const char * pPwd/*[in]˽Կ����*/,const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
			DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,BYTE * pOutStream/*[out]���ܺ������*/,
			DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
		    char * OperMsg/*[out]���ز���������Ϣ*/)
{
	unsigned char * bufin = NULL,
				  * bufout = NULL;
	UINT iblock_size = 0,
		oblock_size = 0,
		outlen = 0,
		inlen = 0;
	BOOL ret = true;
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���
	RSA * rsa = NULL;
	EVP_PKEY * pkey = NULL;
	FILE * outfd = NULL,
		 * infd = NULL;
	int memtemplen = 0; //����ڴ�ƫ��
	UINT len = 0; //�����ڴ�ʣ�೤��

	int uMaxMem = nOutlen;	//����Ϊ����,������UINT


	BOOL bOutType = FALSE;//����ļ�����,�ڴ�-FALSE,�ļ�-TRUE;
	if(nOutlen == 0) bOutType = TRUE;

	if(nInlen == 0)//����Ϊ�ļ�
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"δָ�������ļ�");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//�õ��ļ�����
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//���Ϊ�ļ�
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"δָ������ļ�");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pOutStream);
			return FALSE;
		}
	}

	pkey=CCertKey::LoadKey((char *)pKeyBuf,nKeyLen,(char *)pPwd,OperMsg);
	if (pkey == NULL)
	{ 
//		sprintf(OperMsg,"ȡ��˽Կ��Կʧ��");
		ret=FALSE;
		goto err;
	}
	
    if (!(rsa = EVP_PKEY_get1_RSA(pkey)))
	{
		sprintf(OperMsg,"ȡ��RSA��Կʧ��");
		ret=FALSE;
		goto err;
    }


	iblock_size = BN_num_bytes(rsa->n);//Ԥ���ܳ��� 128
	oblock_size = BN_num_bytes(rsa->n) - 11;//���ܺ󳤶� 117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];
	if(nInlen == 0)//�ļ�
	{
		for(;;)
		{
			inlen=fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;//117,128
			outlen = RSA_private_decrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}

				memcpy(pOutStream + memtemplen,bufout,outlen);//���������ú���
				memtemplen += outlen;
			}
			finishLen+=inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}
	else//�ڴ�
	{
		for(UINT i=0;;i++)
		{
			//ÿ�� iblock_size ��ʵ�ʳ���
			len = (nInlen>iblock_size)?iblock_size:nInlen;//�ڴ����򳤶�
			nInlen -= len;
			outlen = RSA_private_decrypt(len,(UCHAR *)(pInStream+i*iblock_size),bufout,
				rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}

				memcpy(pOutStream + memtemplen,bufout,outlen);//���������ú���
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

/*˽Կ����(ǩ��)*/
BOOL CEvp::RSAPrivEnc(const BYTE * pKeyBuf/*[in]˽Կ*/,const UINT nKeyLen/*[in]˽Կ����*/,
			const char * pPwd/*[in]˽Կ����*/,const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
			DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,BYTE * pOutStream/*[out]���ܺ������*/,
			DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
			char * OperMsg/*[out]���ز���������Ϣ*/)
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
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���
	
	int memtemplen = 0; //����ڴ�ƫ��
	UINT len = 0; //�����ڴ�ʣ�೤��
	int uMaxMem = nOutlen;	//����Ϊ����,������UINT

	FILE * outfd = NULL,
		 * infd = NULL;

	BOOL bOutType = FALSE;//����ļ�����,�ڴ�-FALSE,�ļ�-TRUE;
	if(nOutlen == 0) bOutType = TRUE;


	if(nInlen == 0)//����Ϊ�ļ�
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"δָ�������ļ�");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//�õ��ļ�����
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//���Ϊ�ļ�
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"δָ������ļ�");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pOutStream);
			return FALSE;
		}
	}

	pkey=CCertKey::LoadKey((char *)pKeyBuf,nKeyLen,(char *)pPwd,OperMsg);
	if (pkey == NULL)
	{ 
//		sprintf(OperMsg,"ȡ��˽Կ��Կʧ��");
		ret=FALSE;
		goto err;
	}
	
    if (!(rsa = EVP_PKEY_get1_RSA(pkey)))
	{
		sprintf(OperMsg,"ȡ��RSA��Կʧ��");
		ret=FALSE;
		goto err;
    }


	iblock_size = BN_num_bytes(rsa->n) - 11;//���볤�� 117
 	oblock_size = BN_num_bytes(rsa->n);//���ܺ󳤶� 128
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];
	if(nInlen == 0)//�ļ�
	{
		for(;;)
		{
			inlen=fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;//117,128
			outlen = RSA_private_encrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//���������ú���
				memtemplen += outlen;
			}
			finishLen+=inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}
	else//�ڴ�
	{
		for(UINT i=0;;i++)
		{
			//ÿ�� iblock_size ��ʵ�ʳ���
			len = (nInlen>iblock_size)?iblock_size:nInlen;//�ڴ����򳤶�
			nInlen -= len;
			outlen = RSA_private_encrypt(len,(UCHAR *)(pInStream+i*iblock_size),
				bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//���������ú���
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

//��Կ����
BOOL CEvp::RSAPubDec(const BYTE * pCertBuf/*[in]��Կ*/,const UINT nCertLen/*[in]��Կ����*/,
			const char * pPwd/*[in]��Կ����,ֻ���PFX���ļ�*/,const BYTE * pInStream/*[in]�����ļ����ڴ�*/,
			DWORD nInlen/*[in]���ݳ���,Ϊ0��ʾpInStreamΪ�ļ���*/,BYTE * pOutStream/*[out]���ܺ������*/,
			DWORD & nOutlen/*[in,out]inΪ0ʱ���ʾpOutStreamΪ�ļ���,out���ܺ����ݳ���*/,
			char * OperMsg/*[out]���ز���������Ϣ*/)
{
	UINT iblock_size = 0, //����鳤��
		oblock_size = 0; //����鳤��
	unsigned char * bufin=NULL,
				  * bufout=NULL;
	int inlen = 0,
		outlen = 0;
	BOOL ret = TRUE;
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���
	X509 * x509 = NULL;
	EVP_PKEY * pcert = NULL;
	RSA * rsa = NULL;
	int memtemplen = 0; //����ڴ�ƫ��
	UINT len = 0; //�����ڴ�ʣ�೤��
	int uMaxMem = nOutlen;	//����Ϊ����,������UINT

	BOOL bOutType = FALSE;//����ļ�����,�ڴ�-FALSE,�ļ�-TRUE;

	if(nOutlen == 0) bOutType = TRUE;

	FILE * outfd = NULL,
		* infd = NULL;

	if(nInlen == 0)//����Ϊ�ļ�
	{
		if(pInStream == NULL || strlen((char*)pInStream)==0)
		{
			strcpy(OperMsg,"δָ�������ļ�");
			return FALSE;
		}
		
		if ((infd = fopen ((char *)pInStream, "rb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pInStream);
			return FALSE;
		}
		fileLen = filelength(fileno(infd));//�õ��ļ�����
	}
	else
		fileLen = nInlen;

	if(nOutlen == 0)//���Ϊ�ļ�
	{
		if(pOutStream == NULL || strlen((char*)pOutStream)==0)
		{
			strcpy(OperMsg,"δָ������ļ�");
			return FALSE;
		}
		
		if ((outfd = fopen ((char *)pOutStream, "wb")) == NULL)//ԭ��
		{
			sprintf(OperMsg,"���ļ�%sʧ��",pOutStream);
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
		sprintf(OperMsg,"ȡ�ù�Կ��Կʧ��");
		ret = FALSE;
		goto err;
	}

    if (!(rsa = EVP_PKEY_get1_RSA(pcert)))
	{
		sprintf(OperMsg,"ȡ��RSA��Կʧ��");
		ret=FALSE;
		goto err;
    }
	iblock_size = BN_num_bytes(rsa->n);//Ԥ���ܳ���128
	oblock_size = BN_num_bytes(rsa->n) - 11;//���ܺ󳤶�117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];
	if(nInlen==0)//�ļ�
	{	
		for(;;)
		{
			inlen = fread(bufin,sizeof(char),iblock_size,infd);
			if(!inlen)
				break;
			outlen = RSA_public_decrypt(inlen,bufin,bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//���������ú���
				memtemplen += outlen;
			}
			finishLen += inlen;
			DrawProg(finishLen*HUNDRED/fileLen);
		}
		nOutlen = memtemplen;
	}

	else//�ڴ�����
	{
		for(UINT i=0;;i++)
		{
			//ÿ�� iblock_size ��ʵ�ʳ���
			len = (nInlen>iblock_size)?iblock_size:nInlen;//�ڴ����򳤶�
			nInlen -= len;
			outlen = RSA_public_decrypt(len,(UCHAR *)(pInStream+i*iblock_size),bufout,rsa,RSA_PKCS1_PADDING);
			if (outlen == -1)//���ܺ����ϳ���
			{
				sprintf(OperMsg,"RSA����ʧ��");
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
				uMaxMem -= outlen;	//ʣ�໺���С
				if(uMaxMem < 0)
				{
					strcpy(OperMsg, "��������С,�������ɲ������"); 
					ret = FALSE;
					goto err;
				}
				memcpy(pOutStream + memtemplen,bufout,outlen);//���������ú���
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

//����:�õ����ܺ󻺳�����С
//����:
//		pCertBuf [in] - ��Կ
//		nCertLen [in] - ��Կ����
//		nInlen   [in] - ���ݳ���
//����ֵ:�����������С -1 ��ʾ��������ʧ��
DWORD CEvp::GetEncLen(const char * pCertBuf, const UINT nCertLen, const DWORD nInlen)
{
	X509 * x509 = NULL;
	EVP_PKEY * pcert = NULL;
	RSA * rsa = NULL;
	DWORD len = 0;
	UINT iblock_size = 0, //����鳤��
		oblock_size = 0; //����鳤��
	UINT uMultiple = 0;//�����ǿ鳤�ȵı���,���ڼ������
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
		sprintf(OperMsg,"ȡ�ù�Կ��Կʧ��");
		len = -1;
		goto err;
	}

    if (!(rsa = EVP_PKEY_get1_RSA(pcert)))
	{
		sprintf(OperMsg,"ȡ��RSA��Կʧ��");
		len = 0;
		goto err;
    }

	iblock_size = BN_num_bytes(rsa->n) - 11;//Ԥ���ܳ���,117
	oblock_size = BN_num_bytes(rsa->n);//���ܺ󳤶�,128

	if(nInlen%iblock_size == 0)//��������
		uMultiple = nInlen/iblock_size;
	else
		uMultiple = nInlen/iblock_size + 1;

	len = (nInlen > iblock_size) ? (uMultiple * oblock_size) : oblock_size;//�ڴ����򳤶�

err:
	if(pcert) EVP_PKEY_free(pcert);
    if(x509) X509_free(x509);
	if(rsa) RSA_free(rsa);
	return len;
}

//Ŀ¼����


//����:��ʼ���ԳƼ��ܽṹ
//֧��Ŀ¼����
//����ֵ : �ɹ�TRUE
//		   ʧ��FALSE
//���� : 
//			strCpName - �㷨����
//			strPwd - ��Կ
//			type - ���� 1 - ����; 0 - ����
//			Ctx - ���ؼ�����Ϣ��
//����ʱ��: ����ǰ
BOOL CEvp::InitCrypt2(const CString strCpName, CString strPwd, const int type, EVP_CIPHER_CTX & Ctx)
{
	const EVP_CIPHER * cipher=NULL;
	unsigned char key[EVP_MAX_KEY_LENGTH]="";//�㷨���KEY����
    unsigned char iv[EVP_MAX_IV_LENGTH]="";//�㷨���IV����
	OpenSSL_add_all_algorithms();//add digests   and ciphers
	
	cipher = EVP_get_cipherbyname(strCpName);
	if(cipher==NULL)
	{
//		sprintf(outMsg,"Unknown cipher name %s\n",cpname);
		return FALSE;
	}
	
	//��ʾ�a���� key �ǽo��һ�N cipher �õ�
	//�^������ʹ�õ� hash ���㷨,
	//�Á���ܵ� salt�����ٞ�˂� bytes����t���NULL,
	//�ظ��״� hashing �Ą���������������r�g����������Ҳ����ȫ

	if(!EVP_BytesToKey(cipher,EVP_md5(),NULL,(unsigned char *)strPwd.GetBuffer(0),
		strPwd.GetLength(),1,key,iv))
	{
//		strcpy(outMsg,"Crypt��ʼ��key or iv ʧ��");
		return FALSE;
	}
	
	//EVP_CIPHER_CTX Ctx;
	EVP_CIPHER_CTX_init(&Ctx);//��ʼ��һ��EVP_CIPHER_CTX�ṹ��
	
	//typeΪ1������ܣ����typeΪ0������ܣ����type�ǣ�1���򲻸ı�����
	if(!EVP_CipherInit_ex(&Ctx, cipher, NULL/*NULLʹ��ȱʡ��ʵ���㷨*/,key,iv,type))//��ʼ��
	{
//		strcpy(outMsg,"Crypt��ʼ���ӽ��ܽṹ��ʧ��");
		return FALSE;
	}
	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
	return TRUE;

}

//����:����ԳƼ��ܽṹ
//����:������Ϻ�
void CEvp::CleanCrypt2(EVP_CIPHER_CTX & Ctx)
{
	if(Ctx.cipher) 
		EVP_CIPHER_CTX_cleanup(&Ctx); //���������Ϣ�ͷ��ڴ�
	CRYPTO_cleanup_all_ex_data();
}

//����ֵ : ����ʱ�򷵻ؼ��ܺ��ļ���С
//		   ����ʱ�򷵻ؽ��ܺ��ļ���С
//���� : 
//			pCtx - �Ѿ���ʼ���ļ�����Ϣ��
//			strFileName - ����ΪҪ���ܵ��ļ���,����Ϊ���ܺ󱣴���ļ���
//			File - �ļ�ָ�� ,����λ�õ�ƫ���е����߸���
//			dBolckLen - �鳤��,���ʾĳ���ļ����ܺ������,ֻ�ڽ���ʱ����Ч,���Խ���ĳ���ļ�
//						 ����ʱ����Ч,Ϊ0
//			outStr - ���ش�����Ϣ
DWORD CEvp::Crypt2(EVP_CIPHER_CTX * pCtx, const CString & strFileName,
					CFile & File, DWORD dBolckLen, CString & outStr)	   
{
	if(File.m_hFile == CFile::hFileNull)
		return -1;

	unsigned char inbuf[BUFLEN] = "";
	unsigned char outbuf[BUFLEN + EVP_MAX_BLOCK_LENGTH] = "";//�����㷨��Ŀ鳤�ȡ�
	int infilelen = 0,
		outlen=0;
	BOOL ret = true;
	DWORD dFileLen = 0;//�ļ�����
	DWORD dFinishLen = 0;//��ɳ���
	long lReadLen = 0; //ÿ�ζ�ȡ�Ŀ鳤��
	DWORD dReturnLen = 0;//���ؼӽ��ܽ������

	CFile inSideFile;//�ڲ��ļ�
	if(0 == dBolckLen)//�鳤��Ϊ0��ʾҪ����
	{
		if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
		{
			outStr.Format("���ļ�%sʧ��", strFileName);
			return -1;
		}

		dFileLen = inSideFile.GetLength();

		for(;;)
		{                       
			infilelen = inSideFile.Read(inbuf, BUFLEN);
			if(infilelen <= 0) break;
			if(!EVP_CipherUpdate(pCtx, outbuf, &outlen, inbuf, infilelen))//���g�^�� 
			{
				/* Error */              
				outStr = "Crypt�м���̴���";
				ret = FALSE;
				goto err;
			}
			File.Write(outbuf, outlen);
			dFinishLen += infilelen;
			dReturnLen += outlen;
			DrawProg(dFinishLen*HUNDRED/dFileLen);
		} 

		if(!EVP_CipherFinal_ex(pCtx, outbuf, &outlen))    //��K���E-�������Final����һ������
		{   /* Error */                       
			outStr = "Crypt���չ��̴���"; 
			ret = FALSE;
			goto err;
		}               
		File.Write(outbuf, outlen);

	}
	else	//����
	{
		if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			outStr.Format("�����ļ�%sʧ��", strFileName);
			return -1;
		}

		dFileLen = dBolckLen;

		for(;;)
		{
			//ÿ�� iblock_size ��ʵ�ʳ���
			lReadLen = (dBolckLen > BUFLEN) ? BUFLEN:dBolckLen;//�ڴ����򳤶�
			infilelen = File.Read(inbuf, lReadLen);
			if(0 == infilelen)
				break;


			if(!EVP_CipherUpdate(pCtx, outbuf, &outlen, inbuf, infilelen))//���g�^�� 
			{
				/* Error */              
				outStr = "Crypt�м���̴���";
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

		if(!EVP_CipherFinal_ex(pCtx, outbuf, &outlen))    //��K���E-�������Final����һ������
		{   /* Error */                       
			outStr = "Crypt���չ��̴���"; 
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
//		outStr = "δָ��ժҪ�㷨";
		return FALSE;
	}
	//ʹEVP_Digestϵ�к���֧��������Ч����ϢժҪ�㷨
	OpenSSL_add_all_digests();
    
	//�����������ϢժҪ���������ֵõ���Ӧ��EVP_MD�㷨�ṹ
	md = EVP_get_digestbyname(strMdName.GetBuffer(0));  
	strMdName.ReleaseBuffer();
	if(!md) 
	{
//		outStr.Format("δ֪ժҪ�㷨���� %s",strMdName);
		EVP_cleanup();
		return FALSE;
	}

	//��ʼ����ϢժҪ�ṹmd_ctx��
	EVP_MD_CTX_init(&Ctx);
	
	//ʹ��md���㷨�ṹ����mdctx�ṹ��implΪNULL����ʹ��ȱʡʵ�ֵ��㷨��openssl�����ṩ����ϢժҪ�㷨��
	if(!EVP_DigestInit_ex(&Ctx, md, NULL/*impl*/))
	{
//		outStr.Format("Digest��ʼ���㷨�ṹ����");
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


/*��ϢժҪ*/ 
//����ֵ:ժҪ�󳤶�,-1��ʾʧ��
//���� :	strMdName - ժҪ�㷨����
//			strFileName  - �����ļ�����
//			File - �Ѿ��򿪵Ľ���ļ�
//			outStr - ���ش�����Ϣ
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
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���

	unsigned char md_value[MAX_MD_SIZE]="";
	UINT md_len = 0;/*ժҪ����*/
	

	if(strFileName.IsEmpty())
	{
		outStr = "δָ�������ļ�";
		return -1;
	}
	
	CFile inSideFile;//�ڲ��ļ�
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "���ܴ�ָ���ļ�";
		return -1;
	}


	for(;;)
	{
		len = fread(inbuf, 1, BUFLEN, infd);
		if(len <= 0) break;          
		if(!EVP_DigestUpdate(&md_ctx, inbuf, len))
		{
			outStr.Format("Digest�м���̴���");
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen * HUNDRED / fileLen);
	}
	//�����ϢժҪ������̣�����ɵ�ժҪ��Ϣ�洢��md_value����,������Ϣ�洢��md_len����
	if(!EVP_DigestFinal_ex(&md_ctx, md_value, &md_len))
	{	
		outStr.Format("Digest���չ��̴���");
		ret=FALSE;
	}

	
err:
	//ʹ�øú����ͷ�mdctxռ�õ���Դ�����ʹ��_exϵ�к��������Ǳ�����õġ�
	if(ret)
	{
		//��¼�ļ�
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
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���

	unsigned char md_value[MAX_MD_SIZE]="";
	UINT md_len = 0;/*ժҪ����*/
	

	if(strFileName.IsEmpty())
	{
		outStr = "δָ�������ļ�";
		return FALSE;
	}
	
	CFile inSideFile;//�ڲ��ļ�
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("�ļ�%s��ʧ", strFileName);
		return FALSE;
	}


	for(;;)
	{
		len = fread(inbuf, 1, BUFLEN, infd);
		if(len <= 0) break;          
		if(!EVP_DigestUpdate(&md_ctx, inbuf, len))
		{
			outStr.Format("Digest�м���̴���");
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen * HUNDRED / fileLen);
	}
	//�����ϢժҪ������̣�����ɵ�ժҪ��Ϣ�洢��md_value����,������Ϣ�洢��md_len����
	if(!EVP_DigestFinal_ex(&md_ctx, md_value, &md_len))
	{	
		outStr.Format("Digest���չ��̴���");
		ret=FALSE;
	}

	
err:
	//ʹ�øú����ͷ�mdctxռ�õ���Դ�����ʹ��_exϵ�к��������Ǳ�����õġ�
	if(ret)
	{
		//�Ƚ��ļ�ժҪ�����Ƿ���ͬ
		if(md_len != dBolckLen)
		{
			outStr.Format("ժҪУ�����");
			ret=FALSE;
		}
		else
		{
			char * pmdbuf = new char[dBolckLen + 1];
			File.Read(pmdbuf, dBolckLen);
			if(memcmp(pmdbuf, md_value, dBolckLen) == 0)
			{
				outStr.Format("ժҪУ����ȷ");
				ret=TRUE;
			}
			else
			{
				outStr.Format("ժҪУ�����");
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
	//����������֣�seeded����
	UCHAR iv[EVP_MAX_IV_LENGTH]="";
	UCHAR * ekey [128]; 
	EVP_PKEY * pubKey[128];
	int ekeylen[128];
	int index = 0;

	const EVP_CIPHER *cipher=NULL;
	BOOL ret=true;
 //	DWORD fileLen=0;//�ļ�����
//	DWORD finishLen=0;//��ɳ���
	UINT uCrc16 = 0; //���ܺ���Ҫ��ϢCRC��
	DWORD dOutLen = 0;//�ڴ��ļ�����
	BYTE * pOut = NULL; //�ڴ��ļ�������
	DWORD dFileInfoLen = 0;

	//���ؼ��ܹ�Կ
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_CERT),"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert = LoadResource(NULL,hRsrc);
	LPSTR lpCert = (LPSTR)LockResource(hgCert);	


	CMemFile MemFile;				//���ڴ����Ҫ��Ϣ���ڴ��ļ�

	DWORD dInLen = 0;
	BYTE * pIn = NULL;				//����MemFile.Dectch();


//	fileLen = filelength(fileno(infd));//�õ��ļ�����

	OpenSSL_add_all_algorithms();

	int nCountCert = 0;	//֤�����

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
			outStr.Format("���ص� %d ��֤�� %s ʧ��", nCountCert+1, temp->pCert);
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
		ekeylen,//������� ek �L��
		iv,//�Զ�����
		pubKey,//���� ek �õ� public key(s)
		nCountCert))
	{
		ret=false;
		goto err;
	}

	MemFile.Write(&nCountCert, sizeof(int));	//1 - д�빫Կ����

	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Write(&ekeylen[index], sizeof(int));
		MemFile.Write(ekey[index], ekeylen[index]);
	}

	MemFile.Write(iv, sizeof(iv));

	//������ܽ�� ����+1 �ڴ�
	dFileInfoLen = CEvp::GetEncLen(lpCert, lenCert, MemFile.GetLength());

	dOutLen = dFileInfoLen + 1;
	pOut = new BYTE[dOutLen];

	dInLen = MemFile.GetLength();
	pIn = MemFile.Detach();

	//������Ҫ��Ϣ
	if(!CEvp::RSAPubEnc((BYTE *)lpCert, lenCert, "", pIn, 
		dInLen, pOut, dOutLen, outStr.GetBuffer(255)))
	{
		//����ʧ��
		ret=false;
		goto err;
	}

	//�õ����ܺ󳤶ȵ�CRCУ��
	if(!CRC::GetCrc16((char*)&dOutLen, sizeof(DWORD), uCrc16))
	{
		//����ʧ��
		ret=false;
		goto err;
	}

	//д����ܺ���Ҫ��Ϣ���Ⱥ�CRCУ��ͼ��ܺ���Ҫ��Ϣ����
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
		EVP_CIPHER_CTX_cleanup(&sealCtx); //���������Ϣ�ͷ��ڴ�
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
	CFile inSideFile;//�ڲ��ļ�
	DWORD dProgLen = 0; //�ܹ���Ҫ���ܵĳ���
	DWORD dProgFinshLen = 0;//�Ѿ����ܵĳ���

	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "���ܴ�ָ���ļ�";
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
			outStr = "�м���̳���";
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
		outStr = "���չ��̳���";
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
	int nCountCert = 0;	//��Կ����
	int index = 0;
	UCHAR * ekey [128] = {0}; 
	int ekeylen[128];
	BOOL bInit = FALSE;//�Ƿ��ʼ���ɹ�
	DWORD dFileInfoLen = 0;//���ܺ���Ҫ��Ϣ����
	UINT uCrc16 = 0; //���ܺ���Ҫ��ϢCRC��

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

	//�õ���Ҫ��Ϣ���Ⱥ�CRCУ����
	readlen = File.Read(&dFileInfoLen, sizeof(DWORD));
//	finishLen += readlen;

	readlen = File.Read(&uCrc16, sizeof(UINT));
//	finishLen += readlen;

	if(!CRC::CheckCrc((char *)&dFileInfoLen, sizeof(DWORD), uCrc16))
	{
		outStr = "�ļ�ͷCRCУ�����";
		ret = false;
		goto err;
	}

	//���ٻ�����,���ɼ��ܺ����Ҫ�ļ���Ϣ
	pIn = new BYTE[dFileInfoLen + 1];
	dOutLen = dFileInfoLen;

	readlen = File.Read(pIn, dFileInfoLen);
//	finishLen += readlen;



	//���ٻ�����,���ɼ��ܺ����Ϣ��
	pOut = new BYTE[dOutLen];
	if(!CEvp::RSAPrivDec((BYTE *)lpKey, lenKey, "", pIn, 
		dFileInfoLen, pOut, dOutLen, outStr.GetBuffer(255)))
	{
		//����ʧ��
		outStr.ReleaseBuffer();
		outStr = "��Ҫ��Ϣ����ʧ��";
		ret = false;
		goto err;
	}

	outStr.ReleaseBuffer();
	MemFile.Attach(pOut, dOutLen);

	//�����ǹ�Կ����
	MemFile.Read(&nCountCert, sizeof(int));

	for(index = 0; index < nCountCert; index++)
	{
		MemFile.Read(&ekeylen[index], sizeof(int));
		ekey[index] = new unsigned char[ekeylen[index]];
		MemFile.Read(ekey[index], ekeylen[index]);
	}

	MemFile.Read(iv, sizeof(iv));

	MemFile.Close();

	//�����ļ��Ѿ�ƫ�Ƶ����ܺ���ļ�
	
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
		outStr = "ָ��֤�鲻�ܽ����ļ�,��ȷ��֤��Ϸ���";
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

	DWORD dProgLen = dBolckLen; //�ܹ���Ҫ����ĳ���
	DWORD dProgFinshLen = 0;//�Ѿ�����ĳ���


	CFile inSideFile;//�ڲ��ļ�

	if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		outStr.Format("�����ļ�%s����", strFileName);
		return -1;
	}
	for(;;)
	{
		//ÿ�� iblock_size ��ʵ�ʳ���
		fileLen = (dBolckLen > BUFLEN) ? BUFLEN:dBolckLen;//�ڴ����򳤶�
		readlen = File.Read(buf, fileLen);
		if(readlen == 0)//���ܼ���ʱ�ļ�����Ϊ0
			break;

		if(!EVP_OpenUpdate(&eCtx, ebuf, &ebuflen, buf, readlen))
		{
			outStr = "�м���̴���";
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
		outStr = "���չ��̴���";
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
		EVP_CIPHER_CTX_cleanup(&eCtx); //���������Ϣ�ͷ��ڴ�
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
}


BOOL CEvp::GetPubRsa(const BYTE * pCertBuf/*[in]��Կ*/, const UINT nCertLen/*[in]��Կ����*/, RSA *& pRsa)
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
		outStr = "ȡ�ù�Կ��Կʧ��";
		ret = FALSE;
		goto err;
	}

    if (!(pRsa = EVP_PKEY_get1_RSA(pcert)))
	{
		outStr = "ȡ��RSA��Կʧ��";
		ret=FALSE;
		goto err;
    }

err:
	if(pcert) EVP_PKEY_free(pcert);
    if(x509) X509_free(x509);
	return ret;
}

BOOL CEvp::GetPrivRsa(const BYTE * pKeyBuf/*[in]˽Կ*/,const UINT nKeyLen/*[in]˽Կ����*/,
			const char * pPwd/*[in]˽Կ����*/, RSA *& pRsa)
{
	EVP_PKEY * pkey = NULL;
	CString outStr;
	pkey = CCertKey::LoadKey((char *)pKeyBuf,nKeyLen,(char *)pPwd,outStr.GetBuffer(255));
	outStr.ReleaseBuffer();
	BOOL ret = TRUE;

	if (pkey == NULL)
	{ 
		outStr = "ȡ��˽Կ��Կʧ��";
		ret=FALSE;
		goto err;
	}
	
    if (!(pRsa = EVP_PKEY_get1_RSA(pkey)))
	{
		outStr = "ȡ��RSA��Կʧ��";
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


//��Կ����
DWORD CEvp::RSAPubEnc2(RSA * pRsa, const CString strFileName, CFile & File, CString & outStr)
{
	if(pRsa == NULL)
	{
		outStr = "RSA�Ƿ�";
		return -1;
	}
	UINT iblock_size = 0, //����鳤��
		 oblock_size = 0; //����鳤��

	unsigned char * bufin=NULL,
				  * bufout=NULL;

	int inlen = 0,
		outlen = 0;

	DWORD dProgLen = 0; //�ܹ���Ҫ����ĳ���
	DWORD dProgFinshLen = 0;//�Ѿ�����ĳ���

	BOOL ret = TRUE;
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���
	DWORD returnLen = 0;//��󷵻��ļ����ܺ󳤶�

	CFile inSideFile;//�ڲ��ļ�
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "���ܴ�ָ���ļ�";
		return -1;
	}

	dProgLen = inSideFile.GetLength();

	iblock_size = BN_num_bytes(pRsa->n) - 11;//Ԥ���ܳ���,117
	oblock_size = BN_num_bytes(pRsa->n);//���ܺ󳤶�,128
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
		if (outlen == -1)//���ܺ����ϳ���
		{
			outStr = "RSA����ʧ��";
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

//˽Կ����	
DWORD CEvp::RSAPrivDec2(RSA * pRsa, CFile & File, DWORD dBolckLen, const CString strFileName, CString & outStr)
{
	if(pRsa == NULL)
	{
		outStr = "RSA�Ƿ�";
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

	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���

	DWORD dProgLen = dBolckLen; //�ܹ���Ҫ����ĳ���
	DWORD dProgFinshLen = 0;//�Ѿ�����ĳ���


	CFile inSideFile;//�ڲ��ļ�

	if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			return -1;

	iblock_size = BN_num_bytes(pRsa->n);//Ԥ���ܳ��� 128
	oblock_size = BN_num_bytes(pRsa->n) - 11;//���ܺ󳤶� 117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];

	for(;;)
	{
		//ÿ�� iblock_size ��ʵ�ʳ���
		readlen = (dBolckLen > iblock_size) ? iblock_size:dBolckLen;//�ڴ����򳤶�
		inlen = File.Read(bufin, readlen);

		if(inlen == 0)//���ܼ���ʱ�ļ�����Ϊ0
			break;

		outlen = RSA_private_decrypt(inlen,bufin,bufout,pRsa,RSA_PKCS1_PADDING);
		if (outlen == -1)//���ܺ����ϳ���
		{
			outStr = "RSA����ʧ��";
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
		outStr = "RSA�Ƿ�";
		return -1;
	}
	UINT iblock_size = 0, //����鳤��
		 oblock_size = 0; //����鳤��

	unsigned char * bufin=NULL,
				  * bufout=NULL;

	int inlen = 0,
		outlen = 0;

	BOOL ret = TRUE;
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���
	DWORD returnLen = 0;//��󷵻��ļ����ܺ󳤶�

	DWORD dProgLen = 0; //�ܹ���Ҫ����ĳ���
	DWORD dProgFinshLen = 0;//�Ѿ�����ĳ���


	CFile inSideFile;//�ڲ��ļ�
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "���ܴ�ָ���ļ�";
		return -1;
	}

	dProgLen = inSideFile.GetLength();

	iblock_size = BN_num_bytes(pRsa->n) - 11;//Ԥ���ܳ���,117
	oblock_size = BN_num_bytes(pRsa->n);//���ܺ󳤶�,128
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
		if (outlen == -1)//���ܺ����ϳ���
		{
			outStr = "RSA����ʧ��";
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
		outStr = "RSA�Ƿ�";
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
	long fileLen = 0;//�ļ�����
	long finishLen = 0;//��ɳ���

	DWORD dProgLen = dBolckLen; //�ܹ���Ҫ����ĳ���
	DWORD dProgFinshLen = 0;//�Ѿ�����ĳ���


	CFile inSideFile;//�ڲ��ļ�

	if(!inSideFile.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			return -1;

	iblock_size = BN_num_bytes(pRsa->n);//Ԥ���ܳ��� 128
	oblock_size = BN_num_bytes(pRsa->n) - 11;//���ܺ󳤶� 117
	bufin=new unsigned char[iblock_size];
	bufout=new unsigned char[oblock_size];

	for(;;)
	{
		//ÿ�� iblock_size ��ʵ�ʳ���
		readlen = (dBolckLen > iblock_size) ? iblock_size:dBolckLen;//�ڴ����򳤶�
		inlen = File.Read(bufin, readlen);
		if(inlen == 0)//���ܼ���ʱ�ļ�����Ϊ0
			break;
		outlen = RSA_public_decrypt(inlen,bufin,bufout,pRsa,RSA_PKCS1_PADDING);
		if (outlen == -1)//���ܺ����ϳ���
		{
			outStr = "RSA����ʧ��";
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

/*����ǩ��*/

//����:��ʼ���ԳƼ��ܽṹ
//֧��Ŀ¼����
//����ֵ : �ɹ�TRUE
//		   ʧ��FALSE
//���� : 
//			strCpName - �㷨����
//			strPwd - ��Կ
//			type - ���� 1 - ����; 0 - ����
//			Ctx - ���ؼ�����Ϣ��
//����ʱ��: ����ǰ
BOOL CEvp::InitSign2(CString strMdName, EVP_MD_CTX & md_Ctx, const char * key/*ǩ��ʱ����˽Կ,��֤ʱ���ù�Կ*/,
				  const int keylen, const char * pwd/*˽Կ����,Ϊ��ʱ���ʾ����Ϊ��Կ*/, X509 *& x509, EVP_PKEY *& pkey)
{
	const EVP_MD *md = NULL;
	if(strMdName.IsEmpty())
	{
//		outStr = "δָ��ժҪ�㷨";
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

	//ʹEVP_Digestϵ�к���֧��������Ч����ϢժҪ�㷨
	OpenSSL_add_all_digests();
   
	//�����������ϢժҪ���������ֵõ���Ӧ��EVP_MD�㷨�ṹ
	md = EVP_get_digestbyname(strMdName.GetBuffer(0));  
	strMdName.ReleaseBuffer();
	if(!md) 
	{
//		outStr.Format("δ֪ժҪ�㷨���� %s",strMdName);
		EVP_cleanup();
		return FALSE;
	}

	//��ʼ����ϢժҪ�ṹmd_ctx��
	EVP_MD_CTX_init(&md_Ctx);
	
	if(!EVP_SignInit_ex(&md_Ctx,md,NULL))
	{
		EVP_cleanup();
		return FALSE;
	}

	EVP_cleanup();
	return TRUE;
}

//����:����ԳƼ��ܽṹ
//����:������Ϻ�
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

	CFile inSideFile;//�ڲ��ļ�

	long fileLen=0;//�ļ�����
	long finishLen=0;//��ɳ���


	if(pCtx == NULL)
	{
		outStr = "δ��ʼ�����ܽṹ";
		return -1;
	}
	if(pkey == NULL)
	{
		outStr = "˽Կ�Ƿ�";
		return -1;
	}
	/* Do the signature */
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "���ܴ�ָ���ļ�";
		return -1;
	}


	for(;;)
	{
		len = inSideFile.Read(inbuf, BUFLEN);
		if(len <= 0) break;
		if(!EVP_SignUpdate(pCtx, inbuf, len))
		{
			outStr = "�м���̳���";
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen*HUNDRED/fileLen);
	}

	sig_len = EVP_PKEY_size(pkey);
	sig_buf = new unsigned char[sig_len];
	
	// ������ĽY��������ǂ����ҕ private key �L�Ȳ�ͬ����ͬ������ EVP_PKEY_size() ȡ��
	if(!EVP_SignFinal (pCtx, sig_buf, &sig_len, pkey))
	{
		outStr = "���չ��̳���";
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
	long fileLen=0;//�ļ�����
	long finishLen=0;//��ɳ���

	if(pCtx == NULL)
	{
		outStr = "δ��ʼ�����ܽṹ";
		return FALSE;
	}
	if(pkey == NULL)
	{
		outStr = "��Կ�Ƿ�";
		return FALSE;
	}

	CFile inSideFile;
	if(!inSideFile.Open(strFileName, CFile::modeRead | CFile::typeBinary))
	{
		outStr = "���ܴ�ָ���ļ�";
		return FALSE;
	}
	for(;;)
	{
		len = File.Read(inbuf, BUFLEN);                  
		if(len <= 0) break;
		if(!EVP_VerifyUpdate (pCtx, inbuf, len))
		{
			outStr = "�м���̳���";
			ret=FALSE;
			goto err;
		}
		finishLen+=len;
		DrawProg(finishLen*HUNDRED/fileLen);
	}

	sig_len = EVP_PKEY_size(pkey);//����Ӧ�øĶ�
	sig_buf = new unsigned char[sig_len + 1];

	File.Read(sig_buf, sig_len);

	if(!EVP_VerifyFinal (pCtx, sig_buf, sig_len, pkey))
	{
		outStr = "���չ��̳���";
		ret=FALSE;
	}
err:
	delete [] sig_buf;
	return ret;
}

