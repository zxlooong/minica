// CA.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include <locale.h>
#include "ca.h"
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <openssl/rand.h>
//#include <sys/stat.h>   stat() ���Եõ��ļ�״̬

#define EXT_COPY_NONE	0
#define EXT_COPY_ADD	1
#define EXT_COPY_ALL	2

#define MAX_CERT_LEN 8192 //���Կ����
#define MAX_KEY_LEN 4096 //���˽Կ����

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call) 
	{ 
	case DLL_PROCESS_ATTACH: 
		OpenSSL_add_all_algorithms();	//leak EVP_cleanup(void);
		ERR_load_crypto_strings();	// leak  ERR_free_strings()
		//ENGINE_load_builtin_engines();
		break;
		
	case DLL_PROCESS_DETACH: 
		CONF_modules_unload(1);
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data(); 
		ERR_remove_state(0);
		ERR_free_strings(); 
		CONF_modules_free();
		break; 
	}
	return TRUE;
}

void LastError(char * info)
{
/*	BIO * mem = BIO_new(BIO_s_mem());
	BIO_set_close(mem, BIO_CLOSE); 
	ERR_print_errors(mem);
	BUF_MEM * bptr = NULL;
	BIO_get_mem_ptr(mem, &bptr);
	int len = bptr->length;
	char * pbuf = new char[len+1];
	memset(pbuf,0,len+1);
	memcpy(pbuf,bptr->data,len);
	delete [] pbuf;*/
	
}
/*�˺������Խ�DER��PEM��P12�ļ���Կ������*/
X509 *load_cert(BIO * pBioCert/*����BIO*/, const int iFormat/*��ʽ*/,
				const char * lpszPwd,/*P12����*/
				char * outMsg) //��DER��PEM��P12��ʽ�м��ع�Կ֤��
{
	X509 * x = NULL;
	if(iFormat == DER)
		x = d2i_X509_bio(pBioCert,NULL);
	else if (iFormat == PEM)
		x = PEM_read_bio_X509(pBioCert,NULL,NULL,NULL);//PEM_read_bio_X509_AUX
	else if (iFormat == P12)
	{
		OpenSSL_add_all_algorithms();
		PKCS12 *p12 = d2i_PKCS12_bio(pBioCert, NULL);
		PKCS12_parse(p12, lpszPwd, NULL, &x, NULL);
		PKCS12_free(p12);
		p12 = NULL;
//		EVP_cleanup();
	}
	else
	{
		sprintf(outMsg,"bad input format specified for input cert\n");
		goto end;
	}
end:
	if (x == NULL)
	{
		sprintf(outMsg,"unable to load certificate\n");
	}
	else
		sprintf(outMsg,"");
	return(x);
}

X509 * LoadCert(const char * lpszCert,const int iCertlen,
				const char * lpszPass,char * outMsg)//ö��DER/PEM��ʽ
{
	BIO * in = NULL;
	X509 * x509 = NULL;

	if(iCertlen == 0)//����Ϊ�����ļ�
	{
		if((in = BIO_new_file(lpszCert, "r")) == NULL)
		{
			sprintf(outMsg,"open CA certificate file error");
			return NULL;
		}
	}
	else//����Ϊ�ڴ����ļ�
	{
		if((in = BIO_new_mem_buf((void *)lpszCert,iCertlen)) == NULL)//ֻ������
		{
			sprintf(outMsg,"Make Mem Bio Error");
			return NULL;
		}
	}
	if((x509 = load_cert(in,DER,NULL,outMsg)) == NULL)//����DER
	{
		BIO_reset(in);//�ָ�bio
		if((x509 = load_cert(in,PEM,NULL,outMsg)) == NULL)//����PEM
		{
			BIO_reset(in);//�ָ�bio
			x509 = load_cert(in,P12,lpszPass,outMsg);//����P12
		}
	}
	if (in != NULL) BIO_free(in);
	return x509;
}

EVP_PKEY * load_key(BIO * pBioKey, const int iFormat, const char * lpszPass,char * outMsg)//ö��DER/PEM��ʽ
{
	EVP_PKEY * pkey = NULL;
	OpenSSL_add_all_algorithms();
	
	if (iFormat == DER)
	{
		if(NULL == lpszPass || strlen(lpszPass) == 0)
		{
			pkey = d2i_PrivateKey_bio(pBioKey, NULL);
		}
		else
		{
			//��ӽ�����
			unsigned char key[EVP_MAX_KEY_LENGTH] = "";//�㷨���KEY����
			unsigned char iv[EVP_MAX_IV_LENGTH] = "";//�㷨���IV����
			BIO * bDec = NULL;
			bDec = BIO_new(BIO_f_cipher());
			const EVP_CIPHER * cipher = NULL;
			cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
			if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)lpszPass,
				strlen(lpszPass),1,key,iv))
			{
				BIO_set_cipher(bDec,cipher,key,iv, 0);//1-���ܡ�0-����
				pBioKey = BIO_push(bDec, pBioKey); 
				BIO_flush(pBioKey);
		
				pkey = d2i_PrivateKey_bio(pBioKey, NULL);//˽Կ����
		
				pBioKey = BIO_pop(pBioKey);
				BIO_free(bDec);
			}
			else
			{
				strcpy(outMsg,"��ʼ��key or iv ʧ��");
				goto end;
			}
		}
	}
	else if (iFormat == PEM)
	{
		pkey = PEM_read_bio_PrivateKey(pBioKey,NULL,NULL,(void * )lpszPass);
	}
	else if (iFormat == P12)
	{
		PKCS12 *p12 = d2i_PKCS12_bio(pBioKey, NULL);
		PKCS12_parse(p12, lpszPass, &pkey, NULL, NULL);
		PKCS12_free(p12);
		p12 = NULL;
	}
	else
	{
		sprintf(outMsg,"bad input format specified for key\n");
		goto end;
	}
end:
//	EVP_cleanup();
	if (pkey == NULL)
		sprintf(outMsg,"unable to load Private Key\n");
	else
		sprintf(outMsg,"");
	return(pkey);
}

EVP_PKEY * LoadKey(const char * lpszkey,const int iKeylen,const char * lpszPass,char * outMsg) //leak 4772
{
	EVP_PKEY *pkey = NULL;
	BIO * in = NULL;

	if(iKeylen ==0 )//����Ϊ�����ļ�
	{
		if((in = BIO_new_file(lpszkey, "r")) == NULL)
		{
			sprintf(outMsg,"open CA certificate file error");
			goto end;
		}
	}
	else//����Ϊ�ڴ����ļ�
	{
		if((in = BIO_new_mem_buf((void *)lpszkey,iKeylen)) == NULL)//ֻ������
		{
			sprintf(outMsg,"Make Mem Bio Error");
			goto end;
		}
	}

	if((pkey = load_key(in,DER,lpszPass,outMsg)) == NULL)//����DER
	{
		BIO_reset(in);//BIO�ǿɶ�д�ģ���ô��BIO�������ݶ��ᱻ��գ�
						//�����BIO��ֻ���ģ���ô�ò���ֻ��򵥽�ָ
						//��ָ��ԭʼλ�ã���������ݿ����ٶ�.
		if((pkey = load_key(in,PEM,lpszPass,outMsg)) == NULL)//����PEM
		{
			BIO_reset(in);
			pkey = load_key(in,P12,lpszPass,outMsg);
		}
	}
end:
	if (in != NULL)
	{
		BIO_flush(in);
		BIO_free_all(in);
	}
	return pkey;
}

int Rand(const char * file,const int dont_warn,char * outMsg)//���������,return 0 ---�ɹ�
{
	int consider_randfile = (file == NULL);
	char buffer[200];
	
	RAND_screen();
	if (file == NULL)
		file = RAND_file_name(buffer, sizeof buffer);
	else if (RAND_egd(file) > 0)
	{
	/* we try if the given filename is an EGD socket.
		if it is, we don't write anything back to the file. */
		return 1;
	}
	if (file == NULL || !RAND_load_file(file, -1))
	{
		if (RAND_status() == 0 && !dont_warn)
		{
			sprintf(outMsg,"unable to load 'random state'\n");
			sprintf(outMsg,"This means that the random number generator has not been seeded\n");
			if (consider_randfile) /* explanation does not apply when a file is explicitly named */
			{
				sprintf(outMsg,"Consider setting the RANDFILE environment variable to point at a file that\n");
				sprintf(outMsg,"'random' data can be kept in (the file will be overwritten).\n");
			}
		}
		return 0;
	}
	return 1;
}


///////////////////////// end ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////// begin //////////////////////////////////////

/* Add extension using V3 code: we can set the config file as NULL
* because we wont reference any other sections.
*/
int Add_ExtCert(X509 * pCert/*������ӵ�֤��*/,
				X509 * pRoot/*��֤�飨���еõ���Ϣ)*/,
				const int iNid, const char * lpszValue)
{
	X509_EXTENSION * ex = NULL;
	X509V3_CTX ctx;
	/* This sets the 'context' of the extensions. */
	/* No configuration database */
	X509V3_set_ctx_nodb(&ctx);
	/* Issuer and subject certs: both the target since it is self signed,
	* no request and no CRL
	*/
	X509V3_set_ctx(&ctx, pRoot, pCert, NULL, NULL, 0);
	ex = X509V3_EXT_nconf_nid(NULL, &ctx, iNid, (char *)lpszValue);//X509V3_EXT_nconf
	if (!ex){
		return 0;
	}	
	X509_add_ext(pCert,ex,-1);
	X509_EXTENSION_free(ex);
	return 1;
}

int Add_ExtCert(X509 *pCert/*������ӵ�֤��*/,
				X509 * pRoot/*��֤�飨���еõ���Ϣ)*/, 
				const char * lpszName, const char *lpszValue)
{
	X509_EXTENSION * ex = NULL;
	X509V3_CTX ctx;
	/* This sets the 'context' of the extensions. */
	/* No configuration database */
	X509V3_set_ctx_nodb(&ctx);
	/* Issuer and subject certs: both the target since it is self signed,
	* no request and no CRL
	*/
	X509V3_set_ctx(&ctx, pRoot, pCert, NULL, NULL, 0);
	ex = X509V3_EXT_conf(NULL, &ctx, (char *)lpszName, (char *)lpszValue);//X509V3_EXT_nconf
	if (!ex)
	{
		return 0;
	}	
	X509_add_ext(pCert,ex,-1);
	X509_EXTENSION_free(ex);
	return 1;
}

int AddExtCert(X509 *pCert/*������ӵ�֤��*/,
			   X509 * pRoot/*��֤�飨���еõ���Ϣ)*/, 
			   const stuCERTEXT * pCertExt)
{
	/* This sets the 'context' of the extensions. */
	/* No configuration database */
	/* Issuer and subject certs: both the target since it is self signed,
	* no request and no CRL
	*/
	//��������Note if the CA option is FALSE the pathlen option should be omitted. 
//	Add_ExtCert(pCert,pCert,NID_friendlyName, "130203197703060618");

	//������Կ��ʾ��--------����ӵ���߶����Կ
//	Add_ExtCert(pCert,pCert,NID_subject_key_identifier, "hash");

	//Authority��Կ��ʾ��----���ַ������ж��ǩ����Կʱ
//	Add_ExtCert(pCert,pRoot, NID_authority_key_identifier, "keyid,issuer:always");
	

	//��Կ�÷� ----����ǩ�������ɷ����ԡ���Կ���ܡ����ݼ��ܡ���ԿЭ�̡�֤��ǩ����

		//�䷢�߱�������,URL:http://my.url.here/����֧��email  copy
//	Add_ExtCert(pCert,pCert, NID_issuer_alt_name, 
//		"DNS:hpxs,email:hpxs@hotmail.com,RID:1.2.3.4,URI:https://hpxs,IP:192.168.0.22");
//
	//֤�����
//	Add_ExtCert(pCert,pCert,NID_certificate_policies,"requireExplicitPolicy:3");
	//�䷢������Ϣ����
//	Add_ExtCert(pCert,pCert,NID_info_access,"OCSP;URI:https://hpxs");//����caIssuers;URI:http://my.ca/ca.html
	//CRL�ַ���
//	Add_ExtCert(pCert,pCert, NID_crl_distribution_points, "URI:https://hpxs/hpxs.crl");

	/* Some Netscape specific extensions */
//	Add_ExtCert(ret,ret, NID_crl_number, "sslCA");

//	Add_ExtCert(ret,px509, NID_netscape_comment, "See http://cert.umd.edu/root for details.");
	/* In each case the 'value' of the extension is placed directly in the
	extension. Currently supported extensions in this category are: nsBaseUrl,
	nsRevocationUrl, nsCaRevocationUrl, nsRenewalUrl, nsCaPolicyUrl,
	nsSslServerName and nsComment	*/

//	Add_ExtCert(ret,px509, NID_netscape_cert_type, "client, server, email,objsign, reserved, sslCA,emailCA, objCA");
	/*nsCertType (netscape certificate type) takes the flags: client, server, email,
	objsign, reserved, sslCA, emailCA, objCA.*/
	/* Maybe even add our own extension based on existing */
	
//	nid = OBJ_create("1.3.6.1.4.1.5315.100.2.5", "Hpxs", "OC61108551");0.9.2342.19200300.100.1.1
//	X509V3_EXT_add_alias(nid, NID_netscape_comment);
//	Add_ExtCert(ret,ret, nid, "OC61108551");

	
	while(pCertExt!=NULL)//��������
	{
		if(OBJ_sn2nid(pCertExt->cName) == NID_authority_key_identifier)
		{
			Add_ExtCert(pCert, pRoot, OBJ_sn2nid(pCertExt->cName), (char *)pCertExt->cInfo);
		}
		else if(strstr(pCertExt->cName, ".") == NULL) //û�з���.
		{
			Add_ExtCert(pCert, pCert, OBJ_sn2nid(pCertExt->cName), (char *)pCertExt->cInfo);
		}
		else
		{				//�����Զ�����Ϣbegin
			int nid;
			nid = OBJ_create(pCertExt->cName, pCertExt->cName, pCertExt->cName);
			if(nid != NID_undef)
			{
				X509V3_EXT_add_alias(nid, NID_netscape_comment);
				Add_ExtCert(pCert,pCert, nid, pCertExt->cInfo);
			}
			OBJ_cleanup();	
		}		
		pCertExt = pCertExt->Link;
	}
	return 1;
}

void Ansi2Utf8(const LPSTR lpsrc, const int srclen, LPSTR lpdst, int& dstlen)
{
	WCHAR * pwUnicode;      
    int len = MultiByteToWideChar(CP_ACP,0,(char*)lpsrc,srclen,NULL,0);      
    pwUnicode = new WCHAR[len];  
    memset(pwUnicode,0,len);
    MultiByteToWideChar(CP_ACP,0 ,(char*)lpsrc,srclen,pwUnicode,len);      
    dstlen = WideCharToMultiByte(CP_UTF8,0,pwUnicode,len,NULL,0,NULL,NULL);      
    WideCharToMultiByte(CP_UTF8,0,pwUnicode,len,lpdst,dstlen,NULL,NULL);      
    delete []pwUnicode;      
}

BOOL Add_Name(X509_NAME * px509Name,
			  const int iNid/*c\cn*/,
			  const int iType/*V_ASN1_UTF8STRING*/,
			  const char * lpszInput/*�й�*/,
			  const int iLen/*���볤��*/)//֧����������
{
	if(NULL == lpszInput || strlen(lpszInput) == 0)
		return FALSE;
	if(iType == V_ASN1_UTF8STRING)
	{
		char lpdst[1024] = {0};
		int dstlen = 0;
		char input[256]={0};
		strncpy(input, lpszInput, iLen);
		Ansi2Utf8(input, iLen, lpdst, dstlen);
		X509_NAME_add_entry_by_NID(px509Name,iNid,V_ASN1_UTF8STRING,(UCHAR *)lpdst,dstlen, -1, 0);	
	}
	else
		X509_NAME_add_entry_by_NID(px509Name,iNid,iType,(UCHAR *)lpszInput,iLen, -1, 0);	
	return TRUE;
}

BOOL Add_Name(X509_NAME * px509Name,
			  const char * field/*c\cn*/,
			  const char * lpszInput/*�й�*/,
			  const int iLen/*���볤��*/)//֧����������
{
	if(NULL == lpszInput || strlen(lpszInput) == 0)
		return FALSE;

	BOOL bRet = FALSE;
	int nid = NID_undef;
	int iType = V_ASN1_UTF8STRING;

	if(strstr(field, ".") == NULL) //û�з���.
	{
		nid = OBJ_txt2nid(field);
	}
	else
	{
		nid = OBJ_create(field, field, field);
	}
	if(nid != NID_undef)
	{
		if(NID_pkcs9_emailAddress == nid)
		{
			iType = V_ASN1_IA5STRING;
		}
		else
		{
			iType = V_ASN1_UTF8STRING;
		}

		bRet = Add_Name(px509Name, nid, iType, lpszInput, iLen);
	}
	OBJ_cleanup();	

	return bRet;
}

void AddName(X509_NAME * px509Name,const stuSUBJECT * pSubJect)
{
	setlocale(LC_CTYPE, "");

	while(pSubJect != NULL)//��������
	{
		Add_Name(px509Name, 
				 (char *)pSubJect->cName, (char *)pSubJect->cInfo, 
				 strlen((char *)pSubJect->cInfo));
		pSubJect = pSubJect->Link;
	}

/*
	Add_Name(px509Name,NID_stateOrProvinceName,V_ASN1_UTF8STRING,(char *)pSubJect->ST,strlen((char *)pSubJect->ST));
	Add_Name(px509Name,NID_localityName,V_ASN1_UTF8STRING,(char *)pSubJect->L,strlen((char *)pSubJect->L));
	Add_Name(px509Name,NID_organizationName,V_ASN1_UTF8STRING,(char *)pSubJect->O,strlen((char *)pSubJect->O));
	Add_Name(px509Name,NID_organizationalUnitName,V_ASN1_UTF8STRING,(char *)pSubJect->OU,strlen((char *)pSubJect->OU));
	Add_Name(px509Name,NID_commonName,V_ASN1_UTF8STRING,(char *)pSubJect->CN,strlen((char *)pSubJect->CN));
	Add_Name(px509Name,NID_pkcs9_emailAddress,V_ASN1_IA5STRING,(char *)pSubJect->MAIL,strlen((char *)pSubJect->MAIL));

	Add_Name(px509Name,NID_title,V_ASN1_UTF8STRING,(char *)pSubJect->T,strlen((char *)pSubJect->T));
//	Add_Name(px509Name,NID_description,(char *)pSubJect->D,strlen((char *)pSubJect->D));
	Add_Name(px509Name,NID_givenName,V_ASN1_UTF8STRING,(char *)pSubJect->G,strlen((char *)pSubJect->G));
//	Add_Name(px509Name,NID_initials,(char *)pSubJect->I,strlen((char *)pSubJect->I));
//	Add_Name(px509Name,NID_name,(char *)pSubJect->NAME,strlen((char *)pSubJect->NAME));	
	Add_Name(px509Name,NID_surname,V_ASN1_UTF8STRING,(char *)pSubJect->S,strlen((char *)pSubJect->S));
//	Add_Name(px509Name,NID_dnQualifier,(char *)pSubJect->QUAL,strlen((char *)pSubJect->QUAL));
//	Add_Name(px509Name,NID_pkcs9_unstructuredName,(char *)pSubJect->STN,strlen((char *)pSubJect->STN));
//	Add_Name(px509Name,NID_pkcs9_challengePassword,(char *)pSubJect->PW,strlen((char *)pSubJect->PW));
//	Add_Name(px509Name,NID_pkcs9_unstructuredAddress,(char *)pSubJect->ADD,strlen((char *)pSubJect->ADD));
	Add_Name(px509Name,NID_streetAddress,V_ASN1_UTF8STRING,(char *)pSubJect->CN,strlen((char *)pSubJect->CN));
	Add_Name(px509Name,NID_postalCode,V_ASN1_UTF8STRING,(char *)pSubJect->CN,strlen((char *)pSubJect->CN));
	Add_Name(px509Name,NID_homePostalAddress,V_ASN1_UTF8STRING,(char *)pSubJect->CN,strlen((char *)pSubJect->CN));
	Add_Name(px509Name,"2.5.4.20",V_ASN1_UTF8STRING,(char *)pSubJect->CN,strlen((char *)pSubJect->CN));
*/
}


//BOOL MakeRootPub(stuSUBJECT * rootInfo,/*��Ϣ*/ char * certMem/*֤���ļ�*/,int * certLen,char * outMsg/*�������*/)
/*{
	//����֤��Ԥ�����0,λ��384,������1,��ʽDER
	BIO * memcert = NULL;//���֤�鹫˽Կ
	BUF_MEM *bptrcert = NULL;
	X509 *x509=NULL;
	EVP_PKEY *pkey=NULL;
	memcert= BIO_new(BIO_s_mem());
	BOOL bRet = FALSE;
	if(mkRoot(rootInfo,&x509,&pkey,384,1,1,outMsg))
	{
		if(i2d_X509_bio(memcert,x509))//returns 1 for success  д�빫Կ
		{
			BIO_get_mem_ptr(memcert, &bptrcert);
			*certLen=bptrcert->length;
			memcpy(certMem,bptrcert->data,*certLen);
			bRet = TRUE;
			goto end;
		}
		else
		{
			strcpy(outMsg,"�洢֤�鷢���쳣");
		}
	}
end:
	BIO_free_all(memcert);
	memcert = NULL;
	EVP_PKEY_free(pkey);
	pkey = NULL;
	X509_free(x509);
	x509 = NULL;
	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
	CRYPTO_cleanup_all_ex_data();
	return bRet;
}*/

/*��BIOת��δ�ڴ�,�ͳ���*/
BOOL Bio2Mem(BIO * memBio/*[IN]*/,char * lpszMem/*[OUT]*/,
			 UINT * memLen/*[IN,OUT],����Ϊmem����,���Ϊʵ�ʳ���*/)
{
	if(memBio == NULL || memLen == NULL || (*memLen !=0 && lpszMem == NULL))
		return FALSE;
	//if(BIO_method_type(memBio))//�ж��Ƿ��ڴ�����BIO
	UINT uCopy = 0;//����ʵ�ʳ���
	BUF_MEM * bptrBio = NULL;
	BIO_get_mem_ptr(memBio, &bptrBio);
	(*memLen >(UINT)bptrBio->length)?(uCopy = bptrBio->length):(uCopy = *memLen);
	memcpy(lpszMem,bptrBio->data,uCopy);//�������볤��,����Ϊ0ʱ��,�ⶨ����
	*memLen = bptrBio->length;//����ʵ�ʳ���
	return TRUE;
}

/*X509ת��ΪMem*/
BOOL X5092Mem(X509 *px509/*[IN]*/, const int iType /*[in]����pem-der*/,
			  char *x509Mem/*[OUT]*/,UINT *x509Len /*[OUT]*/)
{
	if(px509 == NULL || x509Len == NULL)
		return FALSE;
	BIO * memcert = NULL;
	memcert = BIO_new(BIO_s_mem());
	BIO_set_close(memcert, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
	BOOL ret = FALSE;
	if (iType == DER)
	{
		ret = i2d_X509_bio(memcert,px509);//returns 1 for success
	}
	else if(iType == PEM)
	{
		ret = PEM_write_bio_X509(memcert,px509);
	}
	else
	{
//		strcpy(outMsg,"ָ��֤���ʽ����");
//		ret = 0;
		goto err;
	}
	if(ret)
	{
		 ret = Bio2Mem(memcert,x509Mem,x509Len);
	}
err:
	BIO_free_all(memcert);
	return ret;
}

/*EVP_PKEYת��ΪBIO*/
BOOL Key2Mem(EVP_PKEY * pkey/*[IN]*/,const int iType/*[in]����pem-der*/,
			 const char * priPwd/*[in]˽Կ����*/,
			 char * keyMem/*[OUT]*/,UINT * keyLen/*[OUT]*/)
{
	if(pkey == NULL || keyLen == NULL)
		return FALSE;
	BIO * memkey = NULL;
	BIO * bEnc = NULL;
	memkey = BIO_new(BIO_s_mem());
	BIO_set_close(memkey, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
	BOOL ret = FALSE;
	if (iType==DER)
	{
		if(NULL == priPwd || strlen(priPwd) == 0)			//д��˽Կ
			ret=i2d_PrivateKey_bio(memkey,pkey);//˽Կδ����
		else
		{
			//��Ӽ�����
			unsigned char key[EVP_MAX_KEY_LENGTH]="";//�㷨���KEY����
			unsigned char iv[EVP_MAX_IV_LENGTH]="";//�㷨���IV����
			bEnc = BIO_new(BIO_f_cipher());
			const EVP_CIPHER * cipher = NULL;
			cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
			if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)priPwd,
				strlen(priPwd),1,key,iv))
			{
				BIO_set_cipher(bEnc,cipher,key,iv, 1);//1-���ܡ�0-����
				memkey = BIO_push(bEnc, memkey); 
				ret = i2d_PrivateKey_bio(memkey,pkey);//˽Կ����
				BIO_flush(memkey);
				memkey = BIO_pop(memkey);
				BIO_free(bEnc);
			}
			else
			{
				ret = FALSE;
			}
		}
	}
	else if(iType == PEM)
	{
		if(NULL == priPwd || strlen(priPwd) == 0)
			ret = PEM_write_bio_PrivateKey(memkey,pkey,NULL,NULL,0,NULL, NULL); //˽Կ������
		else
			ret = PEM_write_bio_PrivateKey(memkey,pkey,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)priPwd); //˽Կ����
	}
	else
	{
		ret = 0;
		goto err;
	}
	if(ret)
	{
		 ret = Bio2Mem(memkey,keyMem,keyLen);
	}
err:
	BIO_free_all(memkey);
	return ret;
	
}

/*��stuCertPair�ṹת��ΪX509,EVP_PKEY,����֤��Կ��*/
BOOL CertPair2XE(const stuCertPair & RootPair/*[IN]*/,X509 *& pCert, EVP_PKEY *& pKey ,char * outMsg)
{
	pCert = NULL;
	pKey = NULL;
	//���stuCertPair�ṹ�Ϸ���
	//��⹫Կ
	if(RootPair.memCert == NULL || strlen(RootPair.memCert) == 0 )//û�й�Կ��Ϣ
	{
		sprintf(outMsg,"�ṹ��ȱ�ٹ�Կ��Ϣ");
		return FALSE;
	}
	else if(RootPair.lenCert > MAX_CERT_LEN)//��⹫Կ����,�޶�Ϊ8192
	{
		sprintf(outMsg,"��Կ���ȳ���ϵͳ����");
		return FALSE;
	}
	//���˽Կ
	else if(RootPair.memKey == NULL || strlen(RootPair.memKey)==0 )//û��˽Կ��Ϣ
	{
		sprintf(outMsg,"�ṹ��ȱ��˽Կ��Ϣ");
		return FALSE;
	}
	else if(RootPair.lenKey > MAX_KEY_LEN) //���˽Կ����,�޶�Ϊ4096
	{
		sprintf(outMsg,"˽Կ���ȳ���ϵͳ����");
		return FALSE;
	}

	if(RootPair.pwdKey != NULL && strlen(RootPair.pwdKey))//˽Կ������
	{
//		OpenSSL_add_all_algorithms();
	}

	pKey = LoadKey(RootPair.memKey,RootPair.lenKey,RootPair.pwdKey,outMsg);
	if (pKey == NULL)
	{
	//	EVP_cleanup();
	//	CRYPTO_cleanup_all_ex_data();
		return FALSE;
	}

	pCert = LoadCert(RootPair.memCert,RootPair.lenCert,RootPair.pwdKey,outMsg);
	if (pCert == NULL)
	{
//		EVP_cleanup();
	//	CRYPTO_cleanup_all_ex_data();
		return FALSE;
	}
	if (!X509_check_private_key(pCert,pKey))
	{
		sprintf(outMsg,"��˽Կ�Բ�ƥ��");
//		EVP_cleanup();
	//	CRYPTO_cleanup_all_ex_data();
		return FALSE;
	}
	return TRUE;
}

//���ڴ��д���PFX
BOOL CreateMemPfx(X509 * pCert, EVP_PKEY * key, const  char * p12Pwd, const char * friendlyName,
				  char * memP12/*OUT*/, UINT * p12Len/*OUT*/) //
{
 	BIO * memBio = NULL;
	BUF_MEM * bptrP12 = NULL;
 	memBio = BIO_new(BIO_s_mem());
	OpenSSL_add_all_algorithms();
	PKCS12 * p12 = PKCS12_create((char *)p12Pwd,(char *)friendlyName, key, pCert, NULL, 0,0,0,0,0);
	if(!p12)
	{
		BIO_free_all(memBio);
		return FALSE;
	}
	i2d_PKCS12_bio(memBio, p12);
	BIO_get_mem_ptr(memBio, &bptrP12);
	*p12Len = bptrP12->length;
	memcpy(memP12,bptrP12->data,*p12Len);
	PKCS12_free(p12);
	BIO_free_all(memBio);
//	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
//	CRYPTO_cleanup_all_ex_data();
	return TRUE;
}

BOOL mkRoot(const stuSUBJECT *rootInfo,
			X509 ** x509p/*out��Կ*/, 
			EVP_PKEY ** pkeyp/*out˽Կ*/, 
		    int bits/*λ��*/, 
			const long serial/*���к�*/, 
			const int days/*��Ч��*/,
			const char * kusage,
			const char * ekusage,
			const stuCERTEXT *pCertExt,/*��չ*/
			char * outMsg/*�������*/)
{
	if(bits < 384)
		bits = 384;

	X509		* x = NULL;
	EVP_PKEY	* pk = NULL;
	RSA			* rsa = NULL;
	X509_NAME	* name = NULL;

	int i = 0,
		len = 0;
	char altname[255] = {0};

	if ((pkeyp == NULL) || (*pkeyp == NULL))
	{
		if ((pk = EVP_PKEY_new()) == NULL)
		{
			abort(); 
			return FALSE;
		}
	}
	else
		pk= *pkeyp;
	
	if ((x509p == NULL) || (*x509p == NULL))
	{
		if ((x=X509_new()) == NULL)
			goto err;
	}
	else
		x= *x509p;

	Rand(NULL,1,outMsg);//�������������
	rsa = RSA_generate_key(bits,RSA_F4,0/*�ص�����callback*/,NULL);//������Կ��,//RSA�洢�˹�Կ˽Կ
	if (!EVP_PKEY_assign_RSA(pk,rsa))//���RSA��Կ��pkey�ṹ��ʼ����,��pk��ΪNULL��ʱ�򣬷���1�����򷵻�0
	{
		abort();
		goto err;
	}
	rsa = NULL;
	
	X509_set_version(x,2);//�汾�ţ���ʾ+1
	ASN1_INTEGER_set(X509_get_serialNumber(x),serial);//���к�
	X509_gmtime_adj(X509_get_notBefore(x),0);//��ʼʱ��
	X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*days);//����ʱ��
	X509_set_pubkey(x,pk);//��Կ
	
	name = X509_get_subject_name(x);
	
	/* This function creates and adds the entry, working out the
	* correct string type and performing checks on its length.
	* Normally we'd check the return value for errors...
	*/

	//C-����,ST-ʡ,L-����,O-��֯,OU-����,CN-����,T-title,D-description,G-givenName,I-initials,
	//Email-emailAddress,S-surname,SN-serialNumber,dnQualifier-dnQualifier,unstructuredName,challengePassword,unstructuredAddress,

	AddName(name,rootInfo);
	/* Its self signed so set the issuer name to be the same as the
	* subject.
	*/

	X509_set_issuer_name(x,name);//���÷��������Ƶ�ͬ�������

	if(kusage && strlen(kusage))
		Add_ExtCert(x,x, NID_key_usage, kusage);

	AddExtCert(x, x, pCertExt);//��չ

	if(ekusage && strlen(ekusage))
		Add_ExtCert(x,x,NID_ext_key_usage,ekusage);

	X509V3_EXT_cleanup();//cleanup the extension code if any custom extensions have been added
	OBJ_cleanup();
	if (!X509_sign(x,pk,EVP_sha1()))//ǩ���㷨EVP_sha1,EVP_md5,��˽Կǩ����Կ
	{
		strcpy(outMsg,"֤��ǩ��ʧ��");
		goto err;
	}
	*x509p = x;
	*pkeyp = pk;
	return TRUE;

err:
	return FALSE;
}

/*�����־,������ĳ��(certMem,��certLen == NULL),���������*/
BOOL MakeRoot(const stuSUBJECT * rootInfo,/*[in]��Ϣ*/
			  const char * friendlyName/*�üǵ�����*/,
			  const int bits/*[in]λ��*/, 
			  const long serial/*[in]���к�*/, 
			  const int days/*[in]��Ч��*/,
			  const char * priPwd/*[in]˽Կ����*/,
			  const char * cKusage,/*��Կ�÷�*/  const char * cEkusage,/*��չ��Կ�÷�*/
			  const stuCERTEXT * pCertExt,/*֤����չ*/
			  char * certMem/*[OUT]֤��*/,   UINT * certLen/*[OUT]*/,
			  char * keyMem/*[OUT]˽Կ*/,    UINT * keyLen/*[OUT]*/,
			  char * p12Mem/*[OUT]pkcs#12*/, UINT * p12Len/*[OUT]*/,
			  char * outMsg,/*�������*/
			  const int iType/*[in]����pem-der*/)
{
	X509		* px509 = NULL;
	EVP_PKEY	* pkey = NULL;

	BIO * memcert = NULL,
		* memkey = NULL;

	memcert = BIO_new(BIO_s_mem());
	memkey = BIO_new(BIO_s_mem());
	BIO_set_close(memcert, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
	BIO_set_close(memkey, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */

	BOOL ret = TRUE;
	int i = 0,
		j = 0;

	if(mkRoot(rootInfo,&px509,&pkey,bits,serial,days,cKusage,cEkusage,pCertExt,outMsg))
	{
		X5092Mem(px509,iType,certMem,certLen);
		Key2Mem(pkey,iType,priPwd,keyMem,keyLen);
		CreateMemPfx(px509,pkey,priPwd,friendlyName,p12Mem,p12Len);
	} 
	else
		ret = FALSE;

	BIO_free_all(memcert);
	BIO_free_all(memkey);
	X509_free(px509);
	EVP_PKEY_free(pkey);
	return ret;
}

///////////////////////// end ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////// begin //////////////////////////////////////

/* Add extension using V3 code: we can set the config file as NULL
 * because we wont reference any other sections.
 */
int Add_ExtReq(STACK_OF(X509_REQUEST) *sk,const int iNid,const char *lpszValue)
{
	X509_EXTENSION *ex;
	ex = X509V3_EXT_conf_nid(NULL, NULL, iNid, (char *)lpszValue);
	if (!ex)
		return 0;
	sk_X509_EXTENSION_push(sk, ex);
	
	return 1;
}
	
int mkReq(const stuSUBJECT * reqInfo,
		  X509_REQ **req,
		  EVP_PKEY **pkeyp, 
		  int bits,
		  char * outMsg)
{
	if(bits < 384)
		bits = 384;
	X509_REQ *x  = NULL;
	EVP_PKEY *pk = NULL;
	RSA *rsa = NULL;
	X509_NAME * name=NULL;
	ASN1_STRING stmp, 
		*str = &stmp;
	char altname[255] = {0};
	
	STACK_OF(X509_EXTENSION) *exts = NULL;
	
	if ((pk=EVP_PKEY_new()) == NULL)
		goto err;
	
	if ((x=X509_REQ_new()) == NULL)
		goto err;
	Rand(NULL,1,outMsg);//�������������
	rsa=RSA_generate_key(bits,RSA_F4,0/*�ص�����callback*/,NULL);//������Կ��
	//PEM_write_bio_RSAPrivateKey
	if (!EVP_PKEY_assign_RSA(pk,rsa))
		goto err;
	
	rsa=NULL;
	
	X509_REQ_set_pubkey(x,pk);
	
	name=X509_REQ_get_subject_name(x);
	
	/* This function creates and adds the entry, working out the
	* correct string type and performing checks on its length.
	* Normally we'd check the return value for errors...
	*/

	AddName(name, reqInfo);

	/* Certificate requests can contain extensions, which can be used
		* to indicate the extensions the requestor would like added to 
		* their certificate. CAs might ignore them however or even choke
		* if they are present.
	*/
	
	/* For request extensions they are all packed in a single attribute.
	* We save them in a STACK and add them all at once later...
	*/
	
	exts = sk_X509_EXTENSION_new_null();
	/* Standard extenions */
	//���ⱸ������,URL:http://my.url.here/��֧��email  copy
//	sprintf(altname,"email:%s",(char *)reqInfo->MAIL);
//	Add_ExtReq(exts, NID_subject_alt_name, altname);

	//�����Զ�����չ
//	int nid;
//	nid = OBJ_create("1.3.6.1.4.1.5315.100.2.5", "UserID", "User ID Number");
//	X509V3_EXT_add_alias(nid, NID_netscape_comment);
//	Add_ExtReq(exts, nid, "ID130203197703060618");
	/* Now we've created the extensions we add them to the request */

	X509_REQ_add_extensions(x, exts);
	sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);	
	X509V3_EXT_cleanup();//cleanup the extension code if any custom extensions have been added
	OBJ_cleanup();	
	if (!X509_REQ_sign(x,pk,EVP_sha1()))//���Լ��Ĺ�Կǩ��˽Կ
		goto err;
	
	*req=x;
	*pkeyp=pk;
	return(1);
err:
	return(0);
}

BOOL MakeReq(const stuSUBJECT * reqInfo,/*������ϢIN*/
			 const int bits/*λ��IN*/,
			 const char * priPwd/*˽Կ����,IN*/,
			 char * reqMem/*֤�������ļ�OUT*/,    UINT * reqLen/*֤�������ļ�����OUT*/,
			 char * priMem/*˽Կ�ļ�OUT*/,        UINT * priLen/*˽Կ�ļ��ļ�����OUT*/,
			 char * outMsg/*�������OUT*/,
			 const int iType/*����pem-der*/)
{
	X509_REQ * req=NULL;
	EVP_PKEY * pkey=NULL;

	BIO * breq = NULL,
		* bkey = NULL;

	int i = 0,
		j = 0;

	breq = BIO_new(BIO_s_mem());
	bkey = BIO_new(BIO_s_mem());
	BIO_set_close(breq, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
	BIO_set_close(bkey, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */

/*	if(((breq = BIO_new_file(reqFile, "w"))== NULL)||((bkey = BIO_new_file(priFile, "w")) == NULL))
	{
		strcpy(outMsg,"Create File Error");
		return FALSE;
	}
	*/
	if(!mkReq(reqInfo,&req,&pkey,bits,outMsg))
	{
		strcpy(outMsg,"Make CertReq Error");
		return FALSE;
	}

	if(iType == PEM)
	{
		i = PEM_write_bio_X509_REQ(breq,req);		//д�빫Կ
		if(NULL == priPwd || strlen(priPwd) == 0)
			j = PEM_write_bio_PrivateKey(bkey,pkey,NULL,NULL,0,NULL, NULL); //˽Կ������
		else
			j = PEM_write_bio_PrivateKey(bkey,pkey,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)priPwd); //˽Կ����
	}
	else if(iType == DER)
	{
		i = i2d_X509_REQ_bio(breq,req);	//д�빫Կ

		if(NULL == priPwd || strlen(priPwd) == 0)			//д��˽Կ
			j = i2d_PrivateKey_bio(bkey,pkey);//˽Կδ����
		else
		{
			//��Ӽ�����
			unsigned char key[EVP_MAX_KEY_LENGTH] = "";//�㷨���KEY����
			unsigned char iv[EVP_MAX_IV_LENGTH] = "";//�㷨���IV����
			BIO * bEnc = NULL;
			bEnc = BIO_new(BIO_f_cipher());
			const EVP_CIPHER * cipher = NULL;
			cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
			if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)priPwd,
				strlen(priPwd),1,key,iv))
			{
				BIO_set_cipher(bEnc,cipher,key,iv, 1);//1-���ܡ�0-����
				bkey = BIO_push(bEnc, bkey); 
				j = i2d_PrivateKey_bio(bkey,pkey);//˽Կ����
				BIO_flush(bkey);
				bkey = BIO_pop(bkey);
				BIO_free(bEnc);
			}
			else
				strcpy(outMsg,"��ʼ��key or iv ʧ��,");
		}
	}
	//ת��BIO->MEM
	if(i&&j)
	{
		i = Bio2Mem(breq, reqMem, reqLen);
		j = Bio2Mem(bkey, priMem, priLen);
	}

	BIO_free(breq);
	BIO_free_all(bkey);
	X509_REQ_free(req);
	EVP_PKEY_free(pkey);
	if(!i||!j)
	{
		strcat(outMsg,"Save Cert or Key File Error");
		return FALSE;
	}
	return TRUE;
}
////////////////////////// end //////////////////////////////////////
////////////////////////////////////////////////////////////////////
/////////////////////////// begin ////////////////////////////////////////
int copy_extensions(X509 *x, X509_REQ *req, const int copy_type)//��֤���м���req�Դ���չ��Ϣ
{
	STACK_OF(X509_EXTENSION) * exts = NULL;
	X509_EXTENSION * ext = NULL,
		           * tmpext = NULL;
	ASN1_OBJECT * obj = NULL;
	int i = 0,
		idx = 0,
		ret = 0;
	if (!x || !req || (copy_type == EXT_COPY_NONE))
		return 1;

	exts = X509_REQ_get_extensions(req);
	
	for(i = 0; i < sk_X509_EXTENSION_num(exts); i++)
	{
		ext = sk_X509_EXTENSION_value(exts, i);
		obj = X509_EXTENSION_get_object(ext);
		idx = X509_get_ext_by_OBJ(x, obj, -1);
		/* Does extension exist? */
		if (idx != -1) 
		{
			/* If normal copy don't override existing extension */
			if (copy_type == EXT_COPY_ADD)
				continue;
			/* Delete all extensions of same type */
			do
			{
				tmpext = X509_get_ext(x, idx);
				X509_delete_ext(x, idx);
				X509_EXTENSION_free(tmpext);
				idx = X509_get_ext_by_OBJ(x, obj, -1);
			} while (idx != -1);
		}
		if (!X509_add_ext(x, ext, -1))
			goto end;
	}
	
	ret = 1;
	
end:
	
	sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);
	
	return ret;
}


int do_body(X509 **xret, EVP_PKEY *pkey, X509 *px509, 
			const EVP_MD *dgst,const long serial,
			const char *startdate, const char *enddate,
			const int days,X509_REQ * req,const char * kusage,
			const char * ekusage,const stuCERTEXT * pCertExt,char * outMsg)
{
	X509_NAME * name = NULL, * CAname = NULL;
	X509 * ret = NULL;
	X509_CINF * ci = NULL;
	EVP_PKEY * pktmp = NULL;
	int ok= -1,
		i=0;
//	STACK_OF (X509_EXTENSION) * req_exts;//����ͷ�??
	name = X509_REQ_get_subject_name(req);
	if ((ret = X509_new()) == NULL) 
	{
		ok=0;
		goto err;
	}
	ci = ret->cert_info;

	/* Make it an X509 v3 certificate. �汾1��չ*/
	if (!X509_set_version(ret,2L)) //�汾
	{
		ok=0;
		goto err;
	}
	
	ASN1_INTEGER_set(X509_get_serialNumber(ret),serial);//���к�

	if (!X509_set_issuer_name(ret,X509_get_subject_name(px509)))//������
	{
		ok=0;
		goto err;
	}

	//X509_gmtime_adj(X509_get_notBefore(x),0);//��ʼʱ��
	//X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*days);//����ʱ��

	if (strcmp(startdate,"today") == 0)
		X509_gmtime_adj(X509_get_notBefore(ret),0);//��ʼ����
	else ASN1_UTCTIME_set_string(X509_get_notBefore(ret),(char *)startdate);
	
	if (enddate == NULL)
		X509_gmtime_adj(X509_get_notAfter(ret),(long)60*60*24*days);//��������
	else ASN1_UTCTIME_set_string(X509_get_notAfter(ret),(char *)enddate);

//	X509_gmtime_adj(X509_get_notBefore(ret),0);//��ʼʱ��
//	X509_gmtime_adj(X509_get_notAfter(ret),(long)60*60*24*days);//����ʱ��

	if (!X509_set_subject_name(ret,name)) //���� ---������
	{
		ok=0;
		goto err;
	}
	
	pktmp = X509_REQ_get_pubkey(req);
	i = X509_set_pubkey(ret,pktmp);//��Կ
	EVP_PKEY_free(pktmp);
	if (!i) 
	{
		ok = 0;
		goto err;
	}
	
	//����req�Դ���չ��Ϣ������REQ�ļ�ʱ������
	if (!copy_extensions(ret, req, EXT_COPY_ALL))
	{
		strcpy("�����Դ���չ��Ϣʧ��",outMsg);
		goto err;
	}

	//CRLǩ�����������ܡ���������
		
	if(kusage && strlen(kusage))
		Add_ExtCert(ret,ret, NID_key_usage, kusage);

	AddExtCert(ret,px509,pCertExt);

	if(ekusage && strlen(ekusage))
		Add_ExtCert(ret,ret,NID_ext_key_usage,ekusage);

	/*
	Application			keyUsage  Values 
	SSL Client			digitalSignature 
	SSL Server			keyEncipherment 
	S/MIME Signing		digitalSignature 
	S/MIME Encryption	keyEncipherment 
	Certificate			Signing keyCertSign 
	Object Signing		digitalSignature */


	//�����Զ�����Ϣend
	X509V3_EXT_cleanup();//cleanup the extension code if any custom extensions have been added
	OBJ_cleanup();
	if (!X509_sign(ret,pkey,dgst))//����ǩ����ǩ���㷨
	{
		ok=0;
		goto err;
	}
	ok=1;
err:
	if (CAname != NULL)
		X509_NAME_free(CAname);
	if (ok <= 0)
	{
		if (ret != NULL) X509_free(ret);
		ret=NULL;
	}
	else
		* xret = ret;
	return(ok);
}

int certify(X509 **xret, X509_REQ *req, EVP_PKEY *pkey, 
			X509 *px509,const EVP_MD *dgst,
			const long serial, const char *startdate, const char *enddate, 
			const int days,const char * KUSAGE,
			const char * EKUSAGE,const stuCERTEXT * pCertExt,char * outMsg)
{
	//���ع�Կ֤�飬�����ļ�����˽Կ������Կ��
	EVP_PKEY *pktmp=NULL;
	int ok= -1,i=0;	
	if ((pktmp=X509_REQ_get_pubkey(req)) == NULL) //�õ���Կ
	{
		sprintf(outMsg,"error unpacking public key\n");
		ok=0;
		goto err;
	}
	i=X509_REQ_verify(req,pktmp); //֤������������˽Ҫǩ����������֤һ�£����˹�Կ�����Ƿ����˽Կ
	EVP_PKEY_free(pktmp);
	if (i < 0)
	{
		ok=0;
		sprintf(outMsg,"Signature verification problems....\n");
		goto err;
	}
	if (i == 0)
	{
		ok=0;
		sprintf(outMsg,"Signature did not match the certificate request\n");
		goto err;
	}
	
	ok=do_body(xret,pkey,px509,dgst,serial,startdate, enddate,
		days,req,KUSAGE,EKUSAGE,pCertExt,outMsg);
	
err:
	return(ok);
}


BOOL MakeCert(const stuCertPair & RootPair/*��֤���*/,
			  const long serial/*���к�*/,
			  const char *enddate/*��������*/,
			  const int days/*��Ч��*/, 
			  const char * reqMem/*�����ļ����ڴ�*/,
			  const int reqLen/*����ĳ���,0-��ʾ�ļ�*/,
			  const char * KUSAGE/*��Կ�÷�*/,
			  const char * EKUSAGE/*��ǿ��Կ�÷�*/,
			  const stuCERTEXT * pCertExt,/*֤����չ*/
			  char * outMem/*�����Կ�ļ�*/,
			  UINT * MemLen/*�������*/,
			  char * outMsg/*�������*/,
			  const int type/*�������DER,PEM*/)//ͨ��֤�����󣬵õ�֤��
{
	int ret=1;
	char * md=NULL;
	EVP_PKEY *pkey=NULL;
	X509 * px509=NULL;
	X509_REQ *req=NULL;
	X509 * x=NULL;
	BIO * bcert=NULL,* reqbio=NULL;
	int j;
	const EVP_MD *dgst=NULL;

	OpenSSL_add_all_algorithms();//����ǩ���㷨
	X509 * x509 = NULL;

	if(reqLen == 0)//����Ϊ�����ļ�
	{
		if((reqbio=BIO_new_file(reqMem, "r")) == NULL)
		{
			strcpy(outMsg,"�Ҳ���֤�������ļ�");
			ret=0;
			goto err;
		}
	}
	else//����Ϊ�ڴ����ļ�
	{
		if((reqbio = BIO_new_mem_buf((void *)reqMem, reqLen)) == NULL)//ֻ������
		{
			sprintf(outMsg,"Make Mem Bio Error");
			ret=0;
			goto err;
		}
	}

	
	if ((req=PEM_read_bio_X509_REQ(reqbio,NULL,NULL,NULL)) == NULL)//PEM_read_X509_REQ 
	{
		BIO_reset(reqbio);
		if ((req=d2i_X509_REQ_bio(reqbio,NULL)) == NULL)
		{
			sprintf(outMsg,"Error get certificate request");
			ret=0;
			goto err;
		}
	}

	if(!CertPair2XE(RootPair,px509,pkey,outMsg))
	{
		ret = 0;
		goto err;
	}

	md="sha1";//////////!!!!!!!!!!!!!!!!!////////////////////////////
	if ((dgst=EVP_get_digestbyname(md)) == NULL)//return an EVP_MD structure when passed a digest name
	{
		sprintf(outMsg,"%s is an unsupported message digest type\n",md);
		ret = 0;
		goto err;
	}
	j=certify(&x,req,pkey,px509,dgst,//��Կ֤��out�������ļ�����˽Կ������Կ��
		serial,"today",enddate,days,KUSAGE,EKUSAGE,pCertExt,outMsg);
	if (j <= 0) 
	{
		ret=0;
		goto err;
	}

	bcert = BIO_new(BIO_s_mem());
	BIO_set_close(bcert, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */

	if (type==DER)
	{
		ret = i2d_X509_bio(bcert,x); 
	}
	else if(type==PEM)
	{
		ret = PEM_write_bio_X509(bcert,x);
	}

	//ת��BIO->�ڴ�
	if(ret)
	{
		ret = Bio2Mem(bcert, outMem, MemLen);
	}
err:
	if (reqbio != NULL) BIO_free(reqbio);
	BIO_free_all(bcert);
	EVP_PKEY_free(pkey);
	X509_free(px509);
	X509_free(x);
	if (req != NULL) X509_REQ_free(req);
//	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
//	CRYPTO_cleanup_all_ex_data();
	return ret;

}

///////////////////////// end ////////////////////////////////////////
BOOL DirectCert(const stuCertPair &RootPair /*��֤���*/,
				const int bits /*[IN]*/,
				const long serial /*[IN]���к�*/,
				const char *enddate /*[IN]��������*/,
				const int days /*[IN]��Ч��*/,
				const char *priPwd /*˽Կ��������,IN*/,
				const stuSUBJECT *sSUBJECT /*[IN]�û���Ϣ*/,
				const char *friendlyName /*�üǵ�����*/,
				const char *keyUsage /*��Կ�÷�*/,   const char *ekeyUsage /*��չ��Կ�÷�*/,
				const stuCERTEXT * pCertExt, /*֤����չ�ṹ*/
				char *pCert /*[OUT]���֤�鹫Կ*/,   UINT *certl /*[OUT]����*/,
				char *key /*[OUT]���֤��˽Կ*/,     UINT *keyl /*[OUT]����*/,
				char *p12 /*[OUT]���֤��˽Կ*/,     UINT *p12l /*[OUT]����*/,
				char *outMsg /*[OUT]���ش�����Ϣ*/,
				const int iType /*�������DER,PEM*/)
{
	X509_REQ * req=NULL;
	EVP_PKEY * pkey=NULL, 
			 * prkey=NULL;//֤��˽Կ,//��˽Կ
	X509 * px509=NULL,
		 * x=NULL;//����Կ��֤�鹫Կ
	int ret=1;
	char * md=NULL;
	int i=0,
		j=0,
		ok=0;
	const EVP_MD *dgst=NULL;

	OpenSSL_add_all_algorithms();//����ǩ���㷨

	if(!CertPair2XE(RootPair,px509,prkey,outMsg))
	{
		ret = 0;
		goto err;
	}

	if(!mkReq(sSUBJECT,&req,&pkey, bits,outMsg))
	{
		sprintf(outMsg,"Make CertReq error");
		ret = 0;
		goto err;
	}

	md = "sha1"; //////////!!!!!!!!!!!!!!!!!////////////////////////////
	if ((dgst = EVP_get_digestbyname(md)) == NULL) //return an EVP_MD structure when passed a digest name
	{
		sprintf(outMsg,"%s is an unsupported message digest type\n",md);
		ret = 0;
		goto err;
	}
	ok = certify(&x,req,prkey,px509,dgst, //��Կ֤��out�����󣬸�˽Կ������Կ��
		serial,"today",enddate,days,keyUsage,ekeyUsage,pCertExt,outMsg);
	if (ok > 0) 
	{
		X5092Mem(x,iType,pCert,certl);
		Key2Mem(pkey,iType,priPwd,key,keyl);
		CreateMemPfx(x, pkey, priPwd, friendlyName, p12, p12l);
	}

err:
	EVP_PKEY_free(pkey);
	EVP_PKEY_free(prkey);
	X509_free(px509);
	X509_free(x);
	if (req != NULL) X509_REQ_free(req);
//	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
//	CRYPTO_cleanup_all_ex_data();
	return ret;
}

//////////////////////////////////////////////////////////////////////
///////////////////////// begin //////////////////////////////////////
int Add_ExtCrl(X509_CRL *crl/*������ӵ�֤��*/,X509 * pRoot/*��֤�飨���еõ���Ϣ)*/, 
			   const int iNid, const char *lpszValue)
{
	X509V3_CTX ctx;
	X509_EXTENSION *ex;

	/* This sets the 'context' of the extensions. */
	/* No configuration database */
//	X509V3_set_ctx_nodb(&ctx);
    X509V3_set_ctx(&ctx, pRoot, NULL, NULL, crl, 0);
//	issuerAltName  authorityKeyIdentifier
	ex = X509V3_EXT_conf_nid(NULL, &ctx, iNid, (char *)lpszValue);
	if (!ex)
		return 0;
	X509_CRL_add_ext(crl,ex,-1);
	X509_EXTENSION_free(ex);
	return 1;
}


BOOL MakeCrl(const stuCertPair & RootPair/*��֤���*/,
			 const stuREVOKE * Head/*��������*/,
			 const PNewCrlMem NewCrlMem/*�ص�����*/,
			 char *& outCrl,
			 int * crll,
			 char * outfile/*crl�ļ�*/,
			 char * outMsg/*�������*/)
{
	X509_CRL_INFO *ci = NULL;
	X509_CRL *crl = NULL;
	int ret = 1,
		i = 0;
	char *key = NULL;
	char *md = NULL;
	EVP_PKEY *pkey = NULL;
	X509 *px509 = NULL;
	BIO *in = NULL,
		*out = NULL;
	const EVP_MD *dgst = NULL;
	X509_REVOKED *r = NULL;
	long crldays = 30;
	long crlhours = 0;
//	stuREVOKE * temp =NULL;
	BIO * memcrl = NULL;
	BUF_MEM *bptrcrl = NULL;

	OpenSSL_add_all_algorithms();

	if(!CertPair2XE(RootPair,px509,pkey,outMsg))
	{
		ret = 0;
		goto err;
	}

	md="md5";//////////!!!!!!!!!!!!!!!!!////////////////////////////
	if ((dgst=EVP_get_digestbyname(md)) == NULL)//return an EVP_MD structure when passed a digest name
	{
		sprintf(outMsg,"%s is an unsupported message digest type\n",md);
		ret = 0;
		goto err;
	}
	
	if ((crl = X509_CRL_new()) == NULL)
	{
		ret = 0;
		goto err;
	}
	ci = crl->crl;
	X509_NAME_free(ci->issuer);
	ci->issuer = X509_NAME_dup(px509->cert_info->subject);
	if (ci->issuer == NULL)
	{
		ret = 0;
		goto err;
	}
	X509_gmtime_adj(ci->lastUpdate,0);
	if (ci->nextUpdate == NULL)
		ci->nextUpdate=ASN1_UTCTIME_new();
	X509_gmtime_adj(ci->nextUpdate,(crldays*24+crlhours)*60*60);

	if(!ci->revoked)
		ci->revoked = sk_X509_REVOKED_new_null();


	while(Head!=NULL)//��������
	{
		X509_REVOKED *r = NULL;
        BIGNUM *serial_bn = NULL;
        r = X509_REVOKED_new();
		ASN1_TIME_set(r->revocationDate,Head->time);//ʱ��
		char index[100];
        BN_hex2bn(&serial_bn,itoa(Head->Index,index,10));//���  -  leak BN_free
        ASN1_INTEGER *tmpser = BN_to_ASN1_INTEGER(serial_bn, r->serialNumber);
		BN_free(serial_bn);
        sk_X509_REVOKED_push(ci->revoked,r);
		Head=Head->Link;
	}

//	sk_X509_REVOKED_sort(ci->revoked);
	for (i=0; i<sk_X509_REVOKED_num(ci->revoked); i++)
	{
		r=sk_X509_REVOKED_value(ci->revoked,i);
		r->sequence=i;
	}

    if (ci->version == NULL)
    if ((ci->version=ASN1_INTEGER_new()) == NULL)
	{
		ret = 0;
		goto err;
	}
    ASN1_INTEGER_set(ci->version,1);
	//	issuerAltName  authorityKeyIdentifier
	Add_ExtCrl(crl,px509,NID_subject_alt_name,"DNS:hpxs,email:hpxs@hotmail.com,RID:1.2.3.4,URI:https://hpxs,IP:192.168.0.22");
	Add_ExtCrl(crl,px509,NID_issuer_alt_name, "DNS:hpxs,email:hpxs@hotmail.com,RID:1.2.3.4,URI:https://hpxs,IP:192.168.0.22");
	Add_ExtCrl(crl,px509,NID_authority_key_identifier, "keyid,issuer:always");
	if (!X509_CRL_sign(crl,pkey,dgst))
	{
		ret = 0;
		goto err;
	}
	if(NewCrlMem)//����ڴ�
	{
		memcrl= BIO_new(BIO_s_mem());
		BIO_set_close(memcrl, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
		PEM_write_bio_X509_CRL(memcrl,crl);
		BIO_get_mem_ptr(memcrl, &bptrcrl);
		*crll=bptrcrl->length;
		outCrl=NewCrlMem(*crll);
		memcpy(outCrl,bptrcrl->data,*crll);
	}
	if(outfile)//����ļ�
	{
		out=BIO_new_file(outfile, "w");
		if(out==NULL)
		{
			sprintf(outMsg,"%s is error",outfile);
			ret = 0;
			goto err;
		}
		PEM_write_bio_X509_CRL(out,crl);
	}

	X509V3_EXT_cleanup();//cleanup the extension code if any custom extensions have been added
	OBJ_cleanup();
err:
	if(out)
		BIO_free_all(out);
	if(memcrl)
		BIO_free_all(memcrl);
	if(in)
		BIO_free(in);
	if(pkey)
		EVP_PKEY_free(pkey);
	if(px509)
		X509_free(px509);
	if(crl)
		X509_CRL_free(crl);
//	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
//	CRYPTO_cleanup_all_ex_data();
	if(ret==1)
		strcpy(outMsg,"CRL�����ɹ�");
	return ret;
}

BOOL CertFormatConver(const char * buf/*�ļ����ݻ��ļ�����*/,
					  const int len/*�ڴ泤��Ϊ0��bufΪ�ļ���*/,
					  const char * pwd/*p12�ļ�����,���߽���˽Կ����*/,
					  char * pem /*����ڴ�*/,
					  UINT * pOutLen,/*�����ڴ泤��,���ʵ������ĳ���*/
					  char * keyPwd,/*����˽Կ����,ֻ��˽Կʱ��������*/
					  const int outformat, char * outMsg/*�������*/)
{
	BOOL bRet = TRUE;
	EVP_PKEY *key=NULL;
	X509 *pCert=NULL;
	BIO * biout=NULL;
	int i=0;
	//����ļ�
	if(pOutLen == 0)	//��ʾ���Ϊ�ļ�
	{
		if ((biout=BIO_new_file(pem, "w")) == NULL)
		{
			return FALSE;
		}		
	}
	//���
	else
	{
		biout = BIO_new(BIO_s_mem());
		BIO_set_close(biout, BIO_CLOSE); 
	}

	pCert = LoadCert(buf,len,pwd,outMsg);//���ȳ��Թ�Կ,pBioKey����д

	if(pCert)//�����ļ�Ϊ��Կ�ļ�
	{

		if 	(outformat == DER)
			i=i2d_X509_bio(biout,pCert);
		else if (outformat == PEM)
		{
		//	if (trustout) i=PEM_write_bio_X509_AUX(biout,x);
			i=PEM_write_bio_X509(biout,pCert);
		}
		if(!i)//ʧ��
			strcpy(outMsg,"���湫Կʧ��");
		else
			strcpy(outMsg,"��Կ֤���ʽת���ɹ�");
	}
	else//�����ļ�Ϊ˽Կ�ļ�
	{
		key=LoadKey(buf,len,pwd,outMsg);
		if(!key) 
		{
			strcpy(outMsg,"����ʶ����ļ���ʽ");
			return FALSE;
		}
		if(outformat==PEM)
		{	
			if(keyPwd == NULL || strlen(keyPwd) == 0)
				PEM_write_bio_PrivateKey(biout, key, NULL, NULL, 0, 0, NULL); //˽Կ������
			else
				PEM_write_bio_PrivateKey(biout,key,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)keyPwd); //˽Կ����
		}
		if(outformat==DER)
		{
			if(NULL == keyPwd || strlen(keyPwd) == 0)			//д��˽Կ
				i2d_PrivateKey_bio(biout,key);//˽Կδ����
			else
			{
				//��Ӽ�����
				unsigned char keyl[EVP_MAX_KEY_LENGTH]="";//�㷨���KEY����
				unsigned char iv[EVP_MAX_IV_LENGTH]="";//�㷨���IV����
				BIO * bEnc = NULL;
				if((bEnc = BIO_new(BIO_f_cipher())) != NULL)
				{
					const EVP_CIPHER * cipher = NULL;
					cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
					if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)keyPwd,
						strlen(keyPwd),1,keyl,iv))
					{
						BIO_set_cipher(bEnc,cipher,keyl,iv, 1);//1-���ܡ�0-����
						biout = BIO_push(bEnc, biout); 
						i2d_PrivateKey_bio(biout,key);//˽Կ����
						BIO_flush(biout);
						biout = BIO_pop(biout);
						BIO_free(bEnc);
					}
					else
						strcpy(outMsg,"��ʼ��key or iv ʧ��,");
				}
			}
		}
		
		strcpy(outMsg,"˽Կ֤���ʽת���ɹ�");
	}

	if(*pOutLen != 0)	//��ʾ���Ϊ�ļ�
	{
		Bio2Mem(biout, pem, pOutLen);
	}
	
	if (biout != NULL)
		BIO_free_all(biout);
	if(pCert)
		X509_free(pCert);
	if(key)
		EVP_PKEY_free(key);

	return TRUE;
}

//�ֽ�p12��
BOOL ParsePfx(const char * strP12/*���ļ����ڴ�*/,
			  const UINT iP12Len/* �����Ϊ�ļ�,��Ϊ0,����Ϊ�ڴ泤��*/,
			  const char * strPwdP12/*P12����*/,
			  const char * strCert/*��Կ���*/,
			  const char * strkey/*˽Կ���*/,
			  const char * keyPwd/*˽Կ����*/,
			  const int outformat/*�����ʽ*/,
			  char * outMsg/*���ؽ��*/)
{
	BOOL bRet = TRUE;
	EVP_PKEY * key = NULL;
	X509 * pCert = NULL;
//	STACK_OF(X509) *ca = NULL;
	BIO * bio = NULL, 
		* bioCert = NULL,
		* bioKey = NULL;
	PKCS12 * p12 = NULL;
	int i = 0,
		j = 0;
	
	if(iP12Len == 0)//����Ϊ�����ļ�
	{
		if((bio = BIO_new_file(strP12, "r")) == NULL)
		{
			strcpy(outMsg,"����PFX�ļ�����");
			bRet = FALSE;
			goto err;
		}
	}
	else//����Ϊ�ڴ����ļ�
	{
		if((bio = BIO_new_mem_buf((void *)strP12,iP12Len)) == NULL)//ֻ������
		{
			sprintf(outMsg,"Make Mem Bio Error");
			bRet = FALSE;
			goto err;
		}
	}

	OpenSSL_add_all_algorithms();	//����ȥ��
	p12 = d2i_PKCS12_bio(bio, NULL);
	if (!PKCS12_parse(p12, strPwdP12, &key, &pCert/*PEM*/, /*&ca*/NULL)) 
	{
		strcpy(outMsg,"�����ļ�ʧ��");
		bRet = FALSE;
		goto err;
	}

	//����ļ� - ���ܲ������Կ
	if (strCert != NULL && strlen(strCert) >0)
	{
		if((bioCert = BIO_new_file(strCert, "w")) == NULL)
		{
			bRet = FALSE;
			goto err;
		}

	}
	if (strkey!= NULL && strlen(strkey) >0 )
	{
		if((bioKey = BIO_new_file(strkey, "w")) == NULL)
		{
			bRet = FALSE;
			goto err;
		}
	}
	
	if(outformat == DER)
	{
		if(bioCert)
			i = i2d_X509_bio(bioCert,pCert); //д�빫Կ
		if(bioKey)
		{
			if(NULL == keyPwd || strlen(keyPwd) == 0)			//д��˽Կ
				i2d_PrivateKey_bio(bioKey,key);//˽Կδ����
			else
			{
				//��Ӽ�����
				unsigned char keyl[EVP_MAX_KEY_LENGTH]="";//�㷨���KEY����
				unsigned char iv[EVP_MAX_IV_LENGTH]="";//�㷨���IV����
				BIO * bEnc = NULL;
				bEnc = BIO_new(BIO_f_cipher());
				const EVP_CIPHER * cipher = NULL;
				cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
				if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)keyPwd,
					strlen(keyPwd),1,keyl,iv))
				{
					BIO_set_cipher(bEnc,cipher,keyl,iv, 1);//1-���ܡ�0-����
					bioKey = BIO_push(bEnc, bioKey); 
					i2d_PrivateKey_bio(bioKey,key);//˽Կ����
					BIO_flush(bioKey);
					bioKey = BIO_pop(bioKey);
					BIO_free(bEnc);
				}
				else
				{
					strcpy(outMsg,"��ʼ��key or iv ʧ��,");
					bRet = FALSE;
					goto err;
				}
			}
		}
	}
	else if (outformat == PEM)
	{
		if(bioCert)
			i=PEM_write_bio_X509(bioCert,pCert);
		if(bioKey)
		{
			if(NULL == keyPwd || strlen(keyPwd) == 0)
				j=PEM_write_bio_PrivateKey(bioKey, key, NULL, NULL, 0, 0, NULL);//˽Կ������
			else
				j=PEM_write_bio_PrivateKey(bioKey,key,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)keyPwd); //˽Կ����
		}
	}

err:
	if (bio != NULL)
		BIO_free(bio);
	if (bioCert != NULL) 
		BIO_free(bioCert);
	if (bioKey != NULL) 
		BIO_free_all(bioKey);
	if(pCert)
		X509_free(pCert);
	if(key)
		EVP_PKEY_free(key);
	if(p12)
		PKCS12_free(p12);
//	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
//	CRYPTO_cleanup_all_ex_data();
	if(bRet && i !=0 || j != 0)
	{
		return TRUE;
	}
	return FALSE;
}


//���p12��
BOOL CreatePfx(char * StreamP12/*OUT���ļ�·�����ڴ�����*/,
			   UINT & uP12Len,/*����ʱ��Ϊ���ڱ��������ڴ����򳤶�, ���Ϊʵ��P12�ڴ�ĳ���*/
			   const char * P12EncPwd/*IN ���ڼ���P12������*/,
			   const char * FriendName,/*IN �ü�����*/
			   const char * StreamCert/*IN��Կ*/,
			   const UINT uCertLen,/*IN,��Կ�ڴ����򳤶�, 0 - ��ʾ����Ϊ�����ļ�*/
			   const char * Streamkey/*IN˽Կ*/,
			   const UINT uKeyLen,/*IN,˽Կ�ڴ����򳤶�, 0 - ��ʾ����Ϊ�����ļ�*/
			   const char * KeyDecPwd/*����˽Կ����*/,
			   char * outMsg/*���ؽ��*/)
{
	BOOL bRet = TRUE;
	EVP_PKEY *key = NULL;
	X509 *pCert = NULL;
	PKCS12 * p12 = NULL;
	int i=0;

	//����ļ�
	BIO * biout = BIO_new(BIO_s_mem());
	BIO_set_close(biout, BIO_CLOSE); 

	pCert = LoadCert(StreamCert, uCertLen, "", outMsg);
	if(!pCert) 
	{
		strcpy(outMsg,"���ع�Կ�ļ�ʧ��");
		bRet = FALSE;
		goto err;
	}

	key = LoadKey(Streamkey, uKeyLen, KeyDecPwd, outMsg);//˽Կ
	if(!key) 
	{
		strcpy(outMsg,"����˽Կ�ļ�ʧ��");
		bRet = FALSE;
		goto err;
	}

	OpenSSL_add_all_algorithms();

	p12 = PKCS12_create(const_cast<char *>(P12EncPwd), const_cast<char *>(FriendName), key, pCert, NULL, 0,0,0,0,0);
	if(!p12)
	{
		strcpy(outMsg,"����p12�ṹʧ��");
		bRet = FALSE;
		goto err;
	}

	i2d_PKCS12_bio(biout, p12);

	bRet = Bio2Mem(biout, StreamP12, &uP12Len);

err:
	if(pCert)
		X509_free(pCert);
	if(key)
		EVP_PKEY_free(key);
	if (biout != NULL)
		BIO_free(biout);
	if(p12)
		PKCS12_free(p12);
//	EVP_cleanup();
	return bRet;
}

//�޸�p12������
BOOL ChangePfxPwd(const char * strP12/*in�ϰ��ļ�*/,
			   const char * strPwd/*INԭ����*/,
			   const char * strPwd2/*IN������*/,
			   const char * strOutP12/*in�°��ļ�*/,
			   char * outMsg/*���ؽ��*/)
{
	BIO *bin = NULL;
	BIO *biout = NULL;
	EVP_PKEY *key = NULL;
	X509 *pCert = NULL;
	PKCS12 *p12 = NULL;
	int len = 0,
		wlen = 0;

	BOOL bRet = TRUE;


	OpenSSL_add_all_algorithms();
	//�����ļ� 
	if ((bin=BIO_new_file(strP12, "rw")) == NULL)
	{
		strcpy(outMsg,"����PFX�ļ�����");
		bRet = FALSE;
		goto err;
	}		
	p12 = d2i_PKCS12_bio(bin, NULL);
	if (!p12) 
	{
		strcpy(outMsg,"���ذ��ļ�ʧ��");
		bRet = FALSE;
		goto err;
	}
	if (!PKCS12_parse(p12, strPwd, &key, &pCert, NULL)) 
	{
		strcpy(outMsg,"���ʧ��");
		bRet = FALSE;
		goto err;
	}
	///////////////////////////////////////
	if(p12)
		PKCS12_free(p12);

	p12 = NULL;

	p12 = PKCS12_create((char *)strPwd2,"MiniCA", key, pCert, NULL, 0,0,0,0,0);
	if(!p12)
	{
		strcpy(outMsg,"����p12�ṹʧ��");
		bRet = FALSE;
		goto err;
	}
	//����ļ�
	if ((biout = BIO_new_file(strP12, "w")) == NULL)
	{
		strcpy(outMsg,"��������ļ�ʧ��");
		bRet = FALSE;
		goto err;
	}
	i2d_PKCS12_bio(biout, p12);

err:
	if(p12)
		PKCS12_free(p12);
	if(pCert)
		X509_free(pCert);
	if(key)
		EVP_PKEY_free(key);
	if (bin)
		BIO_free(bin);
	if (biout)
		BIO_free(biout);
//	EVP_cleanup();//frees all three stacks and sets their pointers to NULL ---- EVP_CIPHER
	return bRet;
}

//���鹫Կ��˽Կ�Ƿ����
BOOL CertPairCheck(const char * pCert,
				   const char * key,
				   char * outMsg,
				   const char * priPwd )//���鹫Կ��˽Կ�Ƿ����
{
	EVP_PKEY *pkey=NULL;
	X509 *px509=NULL;
	px509=LoadCert(pCert,0,"",outMsg);
	if(px509==NULL)
	{
		strcpy(outMsg,"���ܼ��ع�Կ�ļ�");
		return FALSE;
	}
	pkey=LoadKey(key,0,priPwd,outMsg);
	if(pkey==NULL)
	{
		strcpy(outMsg,"���ܼ���˽Կ�ļ�");
		X509_free(px509);
		return FALSE;
	}
	if(X509_check_private_key(px509,pkey))//ƥ��
	{
		X509_free(px509);
		EVP_PKEY_free(pkey);
		return TRUE;
	}
	else
	{
		strcpy(outMsg,"��˽Կ�Բ�ƥ��");
		X509_free(px509);
		EVP_PKEY_free(pkey);
		return FALSE;
	}

}

int HexToTen(const char * pHex)
{
    DWORD dwHexNum=0;
    for (; *pHex!=0 ; pHex++)
    {
        dwHexNum *= 16;
        if ((*pHex>='0') && (*pHex<='9'))
            dwHexNum += *pHex-'0';
        else if ((*pHex>='a') && (*pHex<='f'))
            dwHexNum += *pHex-'a'+10;
        else if ((*pHex>='A') && (*pHex<='F'))
            dwHexNum += *pHex-'A'+10;
        else
            -1;
    }
	return dwHexNum;
}

void Utf8ToAnsi(const UCHAR * lpsrc,const int srclen, LPSTR lpdst, int& dstlen)
{
	WCHAR * Unicode;
    int len = MultiByteToWideChar ( CP_UTF8 , 0 ,(char*) lpsrc ,-1 ,NULL,0);
    Unicode = new WCHAR[len * sizeof(WCHAR)];
    MultiByteToWideChar ( CP_UTF8 , 0 ,( char * ) lpsrc, -1, Unicode , len );
    len = WideCharToMultiByte(CP_ACP,0,Unicode,-1,NULL,0,NULL,NULL);
    dstlen = WideCharToMultiByte (CP_ACP,0,Unicode,-1,lpdst,len,NULL,NULL);
    delete []Unicode;
}



/////////////////////////////////////////////////////////////////////////////
// ͨ����������֤֤�飬��֤ͨ�������棬���򷵻ؼ�
BOOL CheckCertWithCrl(const char *pubCert,const int pubCertLen,
					  const char *crlData,const int crlLen,char * outMsg)
{
	BOOL bRet = TRUE;
	int i = 0;
	BOOL bf = TRUE;
	int num;
	ASN1_INTEGER *serial = NULL;
	STACK_OF(X509_REVOKED) *revoked = NULL;
	X509_REVOKED *rc;
	X509_CRL * crl = NULL;
	BIO * in=NULL;

	X509 *x509 = LoadCert(pubCert,pubCertLen,NULL,outMsg);
	if (x509 == NULL)
	{
		strcpy(outMsg,"����֤��ʧ��");
		bRet = FALSE;
		goto end;
	}
	if(crlLen==0)
	{
		if((in=BIO_new_file(crlData, "r"))==NULL)
		{		
			strcpy(outMsg,"Create File Error");
			bRet = FALSE;
			goto end;
		}
	}
	else
	{
		if((in=BIO_new_mem_buf((void *)crlData,crlLen))== NULL)
		{		
			strcpy(outMsg,"GlobalLock mem Error");
			bRet = FALSE;
			goto end;
		}
	}
	crl = d2i_X509_CRL_bio(in,NULL); //DER ��ʽ?
	if(crl == NULL)
	{
		BIO_reset(in);//�ָ�bio
		crl = PEM_read_bio_X509_CRL(in,NULL,NULL,NULL); //PEM��ʽ?
	}
	if(crl == NULL)
	{		
		strcpy(outMsg,"���� crl ʧ��");
		bRet = FALSE;
		goto end;
	}
	revoked = crl->crl->revoked;

    
	serial = X509_get_serialNumber(x509);
	num = sk_X509_REVOKED_num(revoked);
	for(i=0;i<num;i++)
	{
		rc=sk_X509_REVOKED_pop(revoked);	//leak
		if(ASN1_INTEGER_cmp(serial,rc->serialNumber)==0)
		{
			strcpy(outMsg,"֤��������");
			bf = FALSE;
		}
		X509_REVOKED_free(rc);	//leak
	}
    
end:
	if(crl)
		X509_CRL_free(crl);
	if(x509)
		X509_free(x509);
	if(in)
		BIO_free(in);
	if(bRet)
	{
		if(bf)
			strcpy(outMsg,"֤����Ч");
	}
	return bf;
}
    
/////////////////////////////////////////////////////////////////////////////
// ͨ����֤����֤֤��ENGINE_load_private_key
BOOL CheckCertWithRoot(const char *pubCert,const int pubCertLen,
					   const char *rootCert,const int rootCertLen,char * outMsg)
{
	OpenSSL_add_all_algorithms();
	BOOL bRet = TRUE;
	EVP_PKEY * pcert = NULL;
	X509 *pRoot = NULL;
	int ret =0;

	X509 *x509 = LoadCert(pubCert,pubCertLen,NULL,outMsg);
	if (x509 == NULL)
	{
		strcpy(outMsg,"����֤��ʧ��");
		bRet =  FALSE;
		goto end;
	}

	pRoot = LoadCert(rootCert,rootCertLen,NULL,outMsg);
	if (pRoot == NULL)
	{
		strcpy(outMsg,"���ظ�֤��ʧ��");
		bRet =  FALSE;
		goto end;
	}
    
	pcert = X509_get_pubkey(pRoot);
	if (pcert == NULL)
	{
		strcpy(outMsg,"��ȡ��֤�鹫Կ��Ϣʧ��");
		bRet =  FALSE;
		goto end;
	}
	
	ret = X509_verify(x509,pcert);

end:
	if(pcert)
		EVP_PKEY_free (pcert); 
    if(x509)
		X509_free(x509);
	if(pRoot)
		X509_free(pRoot);
//	EVP_cleanup();
	if(bRet)
	{
		if(ret==1)
		{
			strcpy(outMsg,"֤�����֤��ƥ��");
			return TRUE;
		}
		else
		{
			strcpy(outMsg,"֤�����֤�鲻ƥ��");
			return FALSE;
		}
	}
	else
		return FALSE;
}
    
/////////////////////////////////////////////////////////////////////////////
// ���֤����Ч��,����Ч���ڷ����棬���򷵻ؼ�
BOOL CheckCertLife(const char *pubCert,const int pubCertLen,char * outMsg)
{
	X509 *x509 = LoadCert(pubCert,pubCertLen,NULL,outMsg);
	if (x509 == NULL)
	{
		strcpy(outMsg,"����֤��ʧ��");
		return FALSE;
	}
	time_t ct;
	time( &ct );
	asn1_string_st *before=X509_get_notBefore(x509),
		*after=X509_get_notAfter(x509);
	ASN1_UTCTIME *be=ASN1_STRING_dup(before),
		*af=ASN1_STRING_dup(after);
	BOOL bf;
	if(ASN1_UTCTIME_cmp_time_t(be,ct)>=0||ASN1_UTCTIME_cmp_time_t(af,ct)<=0)
	{
		strcpy(outMsg,"֤�鳬����Ч��");
		bf = FALSE;
	}
	else
	{
		strcpy(outMsg,"֤����Ч");
		bf = TRUE;
	}
	M_ASN1_UTCTIME_free(be);
	M_ASN1_UTCTIME_free(af);
	X509_free(x509);
	return bf;
}
 

/*���P7��*/
BOOL CreateP7b(std::list<std::string> * pCertList,
			   const char * lpszCrl,
			   const char * outP7b,
			   const int outformat,
			   char * outMsg/*���ؽ��*/)
{
	int i = 0;
	BIO *in = NULL,
		*out = NULL;
	PKCS7 *p7 = NULL;
	PKCS7_SIGNED *p7s = NULL;
	X509_CRL *crl = NULL;
	STACK *certflst = NULL;
	STACK_OF(X509_CRL) *crl_stack = NULL;
	STACK_OF(X509) *cert_stack = NULL;
	in = BIO_new(BIO_s_file());
	if(lpszCrl && strlen(lpszCrl) != 0)	//�к������ļ�
	{
		//����CRL�ṹ
		if (BIO_read_filename(in,lpszCrl) <= 0)
		{
			sprintf(outMsg, "����CRL%sʧ��", lpszCrl);
			goto end;
		}
		crl = d2i_X509_CRL_bio(in,NULL); //DER ��ʽ?
		if(crl == NULL)
		{
			BIO_reset(in);//�ָ�bio
			crl = PEM_read_bio_X509_CRL(in,NULL,NULL,NULL); //PEM��ʽ?
		}
	}
	if ((p7 = PKCS7_new()) == NULL) goto end;
	if ((p7s = PKCS7_SIGNED_new()) == NULL) goto end;
	p7->type = OBJ_nid2obj(NID_pkcs7_signed);
	p7->d.sign = p7s;
	p7s->contents->type = OBJ_nid2obj(NID_pkcs7_data);
	
	if (!ASN1_INTEGER_set(p7s->version,1))
		goto end;
	if ((crl_stack=sk_X509_CRL_new_null()) == NULL)
		goto end;
	p7s->crl = crl_stack;
	if (crl != NULL)
	{
		sk_X509_CRL_push(crl_stack,crl);
		crl = NULL; /* now part of p7 for OPENSSL_freeing */
	}
	
	if ((cert_stack = sk_X509_new_null()) == NULL) goto end;
	p7s->cert = cert_stack;
	if(pCertList)
	{
		std::list<std::string>::const_iterator pos;
		for(pos = pCertList->begin(); pos != pCertList->end(); ++pos)
		{
			std::string strCert = *pos;
			X509 * pX509 = LoadCert(strCert.c_str(), 0, 0, outMsg);
			if(pX509)
			{
				sk_X509_push(cert_stack, pX509);
			}
		}
	}
	out = BIO_new(BIO_s_file());
	if (BIO_write_filename(out, const_cast<char *>(outP7b)) <= 0)
	{
		goto end;
	}

	if(outformat == DER)
		i=i2d_PKCS7_bio(out,p7);
	else if(outformat == PEM)
		i=PEM_write_bio_PKCS7(out,p7);
	
end:
	if (in != NULL) BIO_free(in);
	if (out != NULL) BIO_free_all(out);
	if (p7 != NULL) PKCS7_free(p7);			//free cert_stack
	if (crl != NULL) X509_CRL_free(crl);
	return i;
}


/*�ֽ�P7��*/
BOOL ParseP7b(const char * lpszInP7b/*���ļ����ڴ�*/,
			  const UINT iP12Len/* �����Ϊ�ļ�,��Ϊ0,����Ϊ�ڴ泤��*/,
			  const int outformat/*�����ʽ*/,
			  const char * lpszCert/*��Կ���*/,
			  const char * lpszCrl/*CRL���*/,
			  const char * lpszOutP7b/*����ת��P7��ʽ������ļ���*/,
			  char * outMsg/*���ؽ��*/)
{
	PKCS7 *p7 = NULL;
	int i,badops=0;
	BIO *in = NULL,
		*out = NULL;
	int ret = 1;
	STACK_OF(X509) *certs=NULL;
	STACK_OF(X509_CRL) *crls=NULL;

	in=BIO_new(BIO_s_file());
	out=BIO_new(BIO_s_file());
	if ((in == NULL) || (out == NULL))
	{
		ret = 0;
        goto end;
    }

	if (BIO_read_filename(in, lpszInP7b) <= 0)
	if (in == NULL)
	{
		sprintf(outMsg, "��ȡP7B%sʧ��", lpszInP7b);
		ret = 0;
		goto end;
	}

	p7 = d2i_PKCS7_bio(in,NULL);
	if(!p7)
	{
		BIO_reset(in);//�ָ�bio
		p7=PEM_read_bio_PKCS7(in,NULL,NULL,NULL);
	}
	if (p7 == NULL)
	{
		sprintf(outMsg, "����P7B%sʧ��", lpszInP7b);
		ret = 0;
		goto end;
	}
	
	i=OBJ_obj2nid(p7->type);
	switch (i)
	{
	case NID_pkcs7_signed:
		certs=p7->d.sign->cert;
		crls=p7->d.sign->crl;
		break;
	case NID_pkcs7_signedAndEnveloped:
		certs=p7->d.signed_and_enveloped->cert;
		crls=p7->d.signed_and_enveloped->crl;
		break;
	default:
		break;
	}
	
	if (certs != NULL && lpszCert && strlen(lpszCert) !=0 )
	{
		X509 *x;
		for (i=0; i<sk_X509_num(certs); i++)
		{
			BIO * bioCert = NULL;
			char buf[256] = {0};
			sprintf(buf, "%s-%d.Cer", lpszCert, i);
			if((bioCert = BIO_new_file(buf, "w")) == NULL)
			{
				goto end;
				ret = 0;
			}
			x = sk_X509_value(certs,i); //����֤�鹫Կ
			if (outformat == DER)
			{
				i2d_X509_bio(bioCert,x);
			}
			else
			{
				PEM_write_bio_X509(bioCert,x);
			}
			BIO_free(bioCert);
		}
	}
	if (crls != NULL  && lpszCrl && strlen(lpszCrl) !=0 )
	{
		X509_CRL *crl;
		
		for (i=0; i<sk_X509_CRL_num(crls); i++)
		{
			BIO * bioCrl = NULL;
			char buf[256] = {0};
			sprintf(buf, "%s-%d.Crl", lpszCrl, i);
			if((bioCrl = BIO_new_file(buf, "w")) == NULL)
			{
				ret = 0;
				goto end;
			}
			crl = sk_X509_CRL_value(crls,i);
			PEM_write_bio_X509_CRL(bioCrl, crl);
			BIO_free(bioCrl);
		}
	}

	//ת��P7��ʽ
	if(lpszOutP7b && strlen(lpszOutP7b) != 0)
	{
		if (BIO_write_filename(out, const_cast<char *>(lpszOutP7b)) <= 0)
		{
			ret = 0;
			goto end;
		}

		if(outformat == DER)
			i=i2d_PKCS7_bio(out,p7);
		else
			i=PEM_write_bio_PKCS7(out,p7);
		if (!i)
		{
			sprintf(outMsg, "����P7B%sʧ��", lpszOutP7b);
			ret = 0;
			goto end;
		}
	}
end:
	if (p7 != NULL) PKCS7_free(p7);
	if (in != NULL) BIO_free(in);
	if (out != NULL) BIO_free_all(out);
	return ret;
}


//ת��֤�鵽REQ
BOOL X5092Req(char * Cert/*��Կ*/,int Certlen,
			  char * Key/*˽Կ,����ǩ����Կ*/,int Keylen,
			  char * Keypwd/*����*/,
			  char * outFile/*����ļ�,req���߹�Կ֤��*/,
			  char * outMsg,/*�������*/
			  char * Rootcert,/*��֤�鹫Կ*/int Rootlen,/*Ϊ0��certfileΪ�����ļ�������Ϊ�ڴ�����*/
			  char * Rootkey/*��֤��˽Կ*/,int RKeylen,
			  char * Pwd/*˽Կ����*/)
{
	X509_REQ * req = NULL;
	EVP_PKEY * pkey = NULL,
		     * prkey = NULL;//֤��˽Կ,//��˽Կ

	X509 * x509 = NULL,
		 * x = NULL,
		 * newx = NULL;//����Կ��֤�鹫Կ,�¹�Կ֤��

	BIO * memcert = NULL, 
		* memkey = NULL;//���֤�鹫˽Կ

	BUF_MEM * bptrcert = NULL,
		    * bptrkey = NULL;

	char * md=NULL;

	BOOL bReq = TRUE;//����REQ����CERT ,TRUE ΪREQ

	int i = 0,
		j = 0,
		ok = 0,
		ret = 1;

	const EVP_MD * dgst = NULL;

	BIO * out = BIO_new_file(outFile, "w");

	OpenSSL_add_all_algorithms();//����ǩ���㷨

	memcert= BIO_new(BIO_s_mem());
	memkey= BIO_new(BIO_s_mem());
	BIO_set_close(memcert, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
	BIO_set_close(memkey, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */

	pkey = LoadKey(Key, Keylen, Keypwd, outMsg);//����˽Կ
	if (pkey == NULL)
	{
		ret = 0;
		goto err;
	}

	x = LoadCert(Cert,Certlen,"",outMsg);//���ع�Կ
	if (x == NULL)
	{
		ret = 0;
		goto err;
	}

	if (!X509_check_private_key(x,pkey))
	{
		sprintf(outMsg,"CA certificate and CA private key do not match\n");
		ret = 0;
		goto err;
	}

	//�õ���ԿREQ
	req = X509_to_X509_REQ(x,pkey,EVP_sha1());//EVP_md5
	if (req == NULL)
	{
		strcpy(outMsg,"֤��ת��ΪREQʧ��");
		ret = 0;
		goto err;
	}


/*	prkey = LoadKey(Rootkey,Keylen,Pwd,outMsg);//���ظ�˽Կ
	if (prkey != NULL)
	{
		x509 = LoadCert(Rootcert,Rootlen,"",outMsg);
		if (x509 != NULL)
		{
			if(X509_check_private_key(x509,prkey))//ƥ��
			{
				bReq = FALSE;//����֤��
			}
			else
				sprintf(outMsg,"��֤�鹫˽Կ�Բ�ƥ��");
		}
		else
			sprintf(outMsg,"���ܼ��ظ�֤�鹫Կ");
	}
	else
		sprintf(outMsg,"���ܼ��ظ�֤��˽Կ");

	if(bReq)//REQ
	{
		PEM_write_bio_X509_REQ(out,rq);
	}
	else //֤��
	{
		md="sha1";//////////!!!!!!!!!!!!!!!!!////////////////////////////
		if ((dgst=EVP_get_digestbyname(md)) == NULL)//return an EVP_MD structure when passed a digest name
		{
			sprintf(outMsg,"%s is an unsupported message digest type\n",md);
			ret = 0;
			goto err;
		}
		j=certify(&newx,req,prkey,x509,dgst,//��Կ֤��out�������ļ�����˽Կ������Կ��
			serial,"today",enddate,days,KUSAGE,EKUSAGE,outMsg);
		if (j <= 0) 
		{
			ret=0;
			goto err;
		}
		
		if(((bcert=BIO_new_file(outfile, "w"))== NULL))
		{
			strcpy(outMsg,"Create File Error");
			goto err;
		}
		if (iType==DER)
		{
			i2d_X509_bio(bcert,x);
		}
		else if(iType==PEM)
		{
			PEM_write_bio_X509(bcert,x);
		}
	}
	
	*/
err:
	return TRUE;
}

/*

����ת��������  
           int            CodePageConvert(UINT  SrcCodePage,  LPCTSTR  pBuff,  int  iBuffLen,  UINT  DestCodePage,  char*  &lpCodePage)  
           {  
                       int  iWideCharCnt  =  ::MultiByteToWideChar(SrcCodePage,  0,  pBuff,  iBuffLen,  NULL,  0);  
                       LPWSTR  lpszWideChar  =  new  wchar_t[iWideCharCnt  +  1];  
                       memset(lpszWideChar,  0,  (iWideCharCnt  +  1)  *  sizeof(WCHAR));  
                       iWideCharCnt  =  MultiByteToWideChar(SrcCodePage,  0,  pBuff,  iBuffLen,  lpszWideChar,  iWideCharCnt);  
 
                       if(DestCodePage  ==  54936    
                                               &&  !IsValidCodePage(54936))  
                                   DestCodePage  =  936;  
 
                       int  iDestCnt  =  WideCharToMultiByte(DestCodePage,  0,  lpszWideChar,  iWideCharCnt,  NULL,  0,  NULL,  NULL);  
                       lpCodePage  =  new  char[iDestCnt  +  1];  
                       memset(lpCodePage,  0,  iDestCnt  +  1);  
                       iDestCnt  =  WideCharToMultiByte(DestCodePage,  0,  lpszWideChar,  iWideCharCnt,  lpCodePage,  iDestCnt,  NULL,  NULL);  
 
                       delete  []lpszWideChar;              
                       return  iDestCnt;  
           }  
�����ǵ��÷�����  
     utf-8  ��  gbk  
           int  nLen  =  CodePageConvertUnix("UTF-8",_T("��׼"),2,"GBK",lpOut);  
     gbk  ��utf-8  
int  nLen  =  CodePageConvertUnix("UTF-8",_T("��׼"),2,"GBK",lpOut); 



 	//������չ��Ϣ
//	Add_ExtCert(ret,ret,NID_basic_constraints, "critical,CA:FALSE,pathlen:1");

//	Add_ExtCert(x,x,NID_basic_constraints, "critical,CA:TRUE,pathlen:1");

	//������Կ��ʾ��---���������ж��ǩ����Կʱ
//	Add_ExtCert(x,x,NID_subject_key_identifier, "hash");
	//�䷢������Կ��ʾ��
//	Add_ExtCert(x,x,NID_authority_key_identifier, "keyid:always");

	//
//	Add_ExtCert(x,x,NID_certificate_policies, "serverAuth");
	
	//���ⱸ������,URL:http://my.url.here/��֧��email  copy
//	Add_ExtCert(x ,x, NID_subject_alt_name, "email:camgr@rog.majki.net,URI:altname:/mirabile/,URI:otherName://tygScaowlOmi,email:nc-glaserth@netcologne.de");

	//�����Զ�����Ϣbegin
/*	int nid;
	nid = OBJ_create("1.2.3.4.9", "Hpxs", "I love you!");
	X509V3_EXT_add_alias(nid, NID_netscape_comment);
	Add_ExtCert(x, nid, "I love you");
	//�����Զ�����Ϣend
*/

  
	
	/* Lets add the extensions, if there are any �����׼��չ*/

