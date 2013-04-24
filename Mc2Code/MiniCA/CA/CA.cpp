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
//#include <sys/stat.h>   stat() 可以得到文件状态

#define EXT_COPY_NONE	0
#define EXT_COPY_ADD	1
#define EXT_COPY_ALL	2

#define MAX_CERT_LEN 8192 //最大公钥长度
#define MAX_KEY_LEN 4096 //最大私钥长度

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
/*此函数可以将DER、PEM、P12文件公钥读出来*/
X509 *load_cert(BIO * pBioCert/*输入BIO*/, const int iFormat/*格式*/,
				const char * lpszPwd,/*P12密码*/
				char * outMsg) //从DER、PEM、P12格式中加载公钥证书
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
				const char * lpszPass,char * outMsg)//枚举DER/PEM格式
{
	BIO * in = NULL;
	X509 * x509 = NULL;

	if(iCertlen == 0)//输入为磁盘文件
	{
		if((in = BIO_new_file(lpszCert, "r")) == NULL)
		{
			sprintf(outMsg,"open CA certificate file error");
			return NULL;
		}
	}
	else//输入为内存中文件
	{
		if((in = BIO_new_mem_buf((void *)lpszCert,iCertlen)) == NULL)//只读类型
		{
			sprintf(outMsg,"Make Mem Bio Error");
			return NULL;
		}
	}
	if((x509 = load_cert(in,DER,NULL,outMsg)) == NULL)//尝试DER
	{
		BIO_reset(in);//恢复bio
		if((x509 = load_cert(in,PEM,NULL,outMsg)) == NULL)//尝试PEM
		{
			BIO_reset(in);//恢复bio
			x509 = load_cert(in,P12,lpszPass,outMsg);//尝试P12
		}
	}
	if (in != NULL) BIO_free(in);
	return x509;
}

EVP_PKEY * load_key(BIO * pBioKey, const int iFormat, const char * lpszPass,char * outMsg)//枚举DER/PEM格式
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
			//添加解密链
			unsigned char key[EVP_MAX_KEY_LENGTH] = "";//算法最长的KEY长度
			unsigned char iv[EVP_MAX_IV_LENGTH] = "";//算法最长的IV长度
			BIO * bDec = NULL;
			bDec = BIO_new(BIO_f_cipher());
			const EVP_CIPHER * cipher = NULL;
			cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
			if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)lpszPass,
				strlen(lpszPass),1,key,iv))
			{
				BIO_set_cipher(bDec,cipher,key,iv, 0);//1-加密、0-解密
				pBioKey = BIO_push(bDec, pBioKey); 
				BIO_flush(pBioKey);
		
				pkey = d2i_PrivateKey_bio(pBioKey, NULL);//私钥解密
		
				pBioKey = BIO_pop(pBioKey);
				BIO_free(bDec);
			}
			else
			{
				strcpy(outMsg,"初始化key or iv 失败");
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

	if(iKeylen ==0 )//输入为磁盘文件
	{
		if((in = BIO_new_file(lpszkey, "r")) == NULL)
		{
			sprintf(outMsg,"open CA certificate file error");
			goto end;
		}
	}
	else//输入为内存中文件
	{
		if((in = BIO_new_mem_buf((void *)lpszkey,iKeylen)) == NULL)//只读类型
		{
			sprintf(outMsg,"Make Mem Bio Error");
			goto end;
		}
	}

	if((pkey = load_key(in,DER,lpszPass,outMsg)) == NULL)//尝试DER
	{
		BIO_reset(in);//BIO是可读写的，那么该BIO所有数据都会被清空；
						//如果该BIO是只读的，那么该操作只会简单将指
						//针指向原始位置，里面的数据可以再读.
		if((pkey = load_key(in,PEM,lpszPass,outMsg)) == NULL)//尝试PEM
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

int Rand(const char * file,const int dont_warn,char * outMsg)//产生随机数,return 0 ---成功
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
int Add_ExtCert(X509 * pCert/*正被添加的证书*/,
				X509 * pRoot/*根证书（从中得到信息)*/,
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

int Add_ExtCert(X509 *pCert/*正被添加的证书*/,
				X509 * pRoot/*根证书（从中得到信息)*/, 
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

int AddExtCert(X509 *pCert/*正被添加的证书*/,
			   X509 * pRoot/*根证书（从中得到信息)*/, 
			   const stuCERTEXT * pCertExt)
{
	/* This sets the 'context' of the extensions. */
	/* No configuration database */
	/* Issuer and subject certs: both the target since it is self signed,
	* no request and no CRL
	*/
	//基本限制Note if the CA option is FALSE the pathlen option should be omitted. 
//	Add_ExtCert(pCert,pCert,NID_friendlyName, "130203197703060618");

	//主题密钥标示符--------区分拥有者多对密钥
//	Add_ExtCert(pCert,pCert,NID_subject_key_identifier, "hash");

	//Authority密钥标示符----区分发行者有多个签名密钥时
//	Add_ExtCert(pCert,pRoot, NID_authority_key_identifier, "keyid,issuer:always");
	

	//密钥用法 ----数字签名、不可否认性、密钥加密、数据加密、密钥协商、证书签名、

		//颁发者备用名称,URL:http://my.url.here/、不支持email  copy
//	Add_ExtCert(pCert,pCert, NID_issuer_alt_name, 
//		"DNS:hpxs,email:hpxs@hotmail.com,RID:1.2.3.4,URI:https://hpxs,IP:192.168.0.22");
//
	//证书策略
//	Add_ExtCert(pCert,pCert,NID_certificate_policies,"requireExplicitPolicy:3");
	//颁发机构信息访问
//	Add_ExtCert(pCert,pCert,NID_info_access,"OCSP;URI:https://hpxs");//或者caIssuers;URI:http://my.ca/ca.html
	//CRL分发点
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

	
	while(pCertExt!=NULL)//遍历链表
	{
		if(OBJ_sn2nid(pCertExt->cName) == NID_authority_key_identifier)
		{
			Add_ExtCert(pCert, pRoot, OBJ_sn2nid(pCertExt->cName), (char *)pCertExt->cInfo);
		}
		else if(strstr(pCertExt->cName, ".") == NULL) //没有发现.
		{
			Add_ExtCert(pCert, pCert, OBJ_sn2nid(pCertExt->cName), (char *)pCertExt->cInfo);
		}
		else
		{				//加入自定义信息begin
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
			  const char * lpszInput/*中国*/,
			  const int iLen/*输入长度*/)//支持中文名称
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
			  const char * lpszInput/*中国*/,
			  const int iLen/*输入长度*/)//支持中文名称
{
	if(NULL == lpszInput || strlen(lpszInput) == 0)
		return FALSE;

	BOOL bRet = FALSE;
	int nid = NID_undef;
	int iType = V_ASN1_UTF8STRING;

	if(strstr(field, ".") == NULL) //没有发现.
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

	while(pSubJect != NULL)//遍历链表
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


//BOOL MakeRootPub(stuSUBJECT * rootInfo,/*信息*/ char * certMem/*证书文件*/,int * certLen,char * outMsg/*操作结果*/)
/*{
	//所以证书预览序号0,位数384,有限期1,格式DER
	BIO * memcert = NULL;//输出证书公私钥
	BUF_MEM *bptrcert = NULL;
	X509 *x509=NULL;
	EVP_PKEY *pkey=NULL;
	memcert= BIO_new(BIO_s_mem());
	BOOL bRet = FALSE;
	if(mkRoot(rootInfo,&x509,&pkey,384,1,1,outMsg))
	{
		if(i2d_X509_bio(memcert,x509))//returns 1 for success  写入公钥
		{
			BIO_get_mem_ptr(memcert, &bptrcert);
			*certLen=bptrcert->length;
			memcpy(certMem,bptrcert->data,*certLen);
			bRet = TRUE;
			goto end;
		}
		else
		{
			strcpy(outMsg,"存储证书发生异常");
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

/*将BIO转换未内存,和长度*/
BOOL Bio2Mem(BIO * memBio/*[IN]*/,char * lpszMem/*[OUT]*/,
			 UINT * memLen/*[IN,OUT],输入为mem长度,输出为实际长度*/)
{
	if(memBio == NULL || memLen == NULL || (*memLen !=0 && lpszMem == NULL))
		return FALSE;
	//if(BIO_method_type(memBio))//判断是否内存类型BIO
	UINT uCopy = 0;//拷贝实际长度
	BUF_MEM * bptrBio = NULL;
	BIO_get_mem_ptr(memBio, &bptrBio);
	(*memLen >(UINT)bptrBio->length)?(uCopy = bptrBio->length):(uCopy = *memLen);
	memcpy(lpszMem,bptrBio->data,uCopy);//拷贝输入长度,输入为0时候,测定长度
	*memLen = bptrBio->length;//返回实际长度
	return TRUE;
}

/*X509转换为Mem*/
BOOL X5092Mem(X509 *px509/*[IN]*/, const int iType /*[in]类型pem-der*/,
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
//		strcpy(outMsg,"指定证书格式错误");
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

/*EVP_PKEY转换为BIO*/
BOOL Key2Mem(EVP_PKEY * pkey/*[IN]*/,const int iType/*[in]类型pem-der*/,
			 const char * priPwd/*[in]私钥密码*/,
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
		if(NULL == priPwd || strlen(priPwd) == 0)			//写入私钥
			ret=i2d_PrivateKey_bio(memkey,pkey);//私钥未加密
		else
		{
			//添加加密链
			unsigned char key[EVP_MAX_KEY_LENGTH]="";//算法最长的KEY长度
			unsigned char iv[EVP_MAX_IV_LENGTH]="";//算法最长的IV长度
			bEnc = BIO_new(BIO_f_cipher());
			const EVP_CIPHER * cipher = NULL;
			cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
			if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)priPwd,
				strlen(priPwd),1,key,iv))
			{
				BIO_set_cipher(bEnc,cipher,key,iv, 1);//1-加密、0-解密
				memkey = BIO_push(bEnc, memkey); 
				ret = i2d_PrivateKey_bio(memkey,pkey);//私钥加密
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
			ret = PEM_write_bio_PrivateKey(memkey,pkey,NULL,NULL,0,NULL, NULL); //私钥不加密
		else
			ret = PEM_write_bio_PrivateKey(memkey,pkey,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)priPwd); //私钥加密
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

/*将stuCertPair结构转换为X509,EVP_PKEY,并验证密钥对*/
BOOL CertPair2XE(const stuCertPair & RootPair/*[IN]*/,X509 *& pCert, EVP_PKEY *& pKey ,char * outMsg)
{
	pCert = NULL;
	pKey = NULL;
	//检测stuCertPair结构合法性
	//检测公钥
	if(RootPair.memCert == NULL || strlen(RootPair.memCert) == 0 )//没有公钥信息
	{
		sprintf(outMsg,"结构中缺少公钥信息");
		return FALSE;
	}
	else if(RootPair.lenCert > MAX_CERT_LEN)//检测公钥长度,限定为8192
	{
		sprintf(outMsg,"公钥长度超过系统限制");
		return FALSE;
	}
	//检测私钥
	else if(RootPair.memKey == NULL || strlen(RootPair.memKey)==0 )//没有私钥信息
	{
		sprintf(outMsg,"结构中缺少私钥信息");
		return FALSE;
	}
	else if(RootPair.lenKey > MAX_KEY_LEN) //检测私钥长度,限定为4096
	{
		sprintf(outMsg,"私钥长度超过系统限制");
		return FALSE;
	}

	if(RootPair.pwdKey != NULL && strlen(RootPair.pwdKey))//私钥代密码
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
		sprintf(outMsg,"公私钥对不匹配");
//		EVP_cleanup();
	//	CRYPTO_cleanup_all_ex_data();
		return FALSE;
	}
	return TRUE;
}

//在内存中创建PFX
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
			X509 ** x509p/*out公钥*/, 
			EVP_PKEY ** pkeyp/*out私钥*/, 
		    int bits/*位数*/, 
			const long serial/*序列号*/, 
			const int days/*有效期*/,
			const char * kusage,
			const char * ekusage,
			const stuCERTEXT *pCertExt,/*扩展*/
			char * outMsg/*操作结果*/)
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

	Rand(NULL,1,outMsg);//产生随机数种子
	rsa = RSA_generate_key(bits,RSA_F4,0/*回调函数callback*/,NULL);//产生密钥对,//RSA存储了公钥私钥
	if (!EVP_PKEY_assign_RSA(pk,rsa))//完成RSA密钥的pkey结构初始工作,当pk不为NULL的时候，返回1，否则返回0
	{
		abort();
		goto err;
	}
	rsa = NULL;
	
	X509_set_version(x,2);//版本号，显示+1
	ASN1_INTEGER_set(X509_get_serialNumber(x),serial);//序列号
	X509_gmtime_adj(X509_get_notBefore(x),0);//起始时间
	X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*days);//结束时间
	X509_set_pubkey(x,pk);//公钥
	
	name = X509_get_subject_name(x);
	
	/* This function creates and adds the entry, working out the
	* correct string type and performing checks on its length.
	* Normally we'd check the return value for errors...
	*/

	//C-国家,ST-省,L-城市,O-组织,OU-部门,CN-个体,T-title,D-description,G-givenName,I-initials,
	//Email-emailAddress,S-surname,SN-serialNumber,dnQualifier-dnQualifier,unstructuredName,challengePassword,unstructuredAddress,

	AddName(name,rootInfo);
	/* Its self signed so set the issuer name to be the same as the
	* subject.
	*/

	X509_set_issuer_name(x,name);//设置发行者名称等同于上面的

	if(kusage && strlen(kusage))
		Add_ExtCert(x,x, NID_key_usage, kusage);

	AddExtCert(x, x, pCertExt);//扩展

	if(ekusage && strlen(ekusage))
		Add_ExtCert(x,x,NID_ext_key_usage,ekusage);

	X509V3_EXT_cleanup();//cleanup the extension code if any custom extensions have been added
	OBJ_cleanup();
	if (!X509_sign(x,pk,EVP_sha1()))//签名算法EVP_sha1,EVP_md5,用私钥签名公钥
	{
		strcpy(outMsg,"证书签名失败");
		goto err;
	}
	*x509p = x;
	*pkeyp = pk;
	return TRUE;

err:
	return FALSE;
}

/*输出标志,如果输出某项(certMem,或certLen == NULL),则不输出本项*/
BOOL MakeRoot(const stuSUBJECT * rootInfo,/*[in]信息*/
			  const char * friendlyName/*好记的名称*/,
			  const int bits/*[in]位数*/, 
			  const long serial/*[in]序列号*/, 
			  const int days/*[in]有效期*/,
			  const char * priPwd/*[in]私钥密码*/,
			  const char * cKusage,/*密钥用法*/  const char * cEkusage,/*扩展密钥用法*/
			  const stuCERTEXT * pCertExt,/*证书扩展*/
			  char * certMem/*[OUT]证书*/,   UINT * certLen/*[OUT]*/,
			  char * keyMem/*[OUT]私钥*/,    UINT * keyLen/*[OUT]*/,
			  char * p12Mem/*[OUT]pkcs#12*/, UINT * p12Len/*[OUT]*/,
			  char * outMsg,/*操作结果*/
			  const int iType/*[in]类型pem-der*/)
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
	Rand(NULL,1,outMsg);//产生随机数种子
	rsa=RSA_generate_key(bits,RSA_F4,0/*回调函数callback*/,NULL);//产生密钥对
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
	//主题备用名称,URL:http://my.url.here/、支持email  copy
//	sprintf(altname,"email:%s",(char *)reqInfo->MAIL);
//	Add_ExtReq(exts, NID_subject_alt_name, altname);

	//加入自定义扩展
//	int nid;
//	nid = OBJ_create("1.3.6.1.4.1.5315.100.2.5", "UserID", "User ID Number");
//	X509V3_EXT_add_alias(nid, NID_netscape_comment);
//	Add_ExtReq(exts, nid, "ID130203197703060618");
	/* Now we've created the extensions we add them to the request */

	X509_REQ_add_extensions(x, exts);
	sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);	
	X509V3_EXT_cleanup();//cleanup the extension code if any custom extensions have been added
	OBJ_cleanup();	
	if (!X509_REQ_sign(x,pk,EVP_sha1()))//用自己的公钥签名私钥
		goto err;
	
	*req=x;
	*pkeyp=pk;
	return(1);
err:
	return(0);
}

BOOL MakeReq(const stuSUBJECT * reqInfo,/*请求信息IN*/
			 const int bits/*位数IN*/,
			 const char * priPwd/*私钥密码,IN*/,
			 char * reqMem/*证书请求文件OUT*/,    UINT * reqLen/*证书请求文件长度OUT*/,
			 char * priMem/*私钥文件OUT*/,        UINT * priLen/*私钥文件文件长度OUT*/,
			 char * outMsg/*操作结果OUT*/,
			 const int iType/*类型pem-der*/)
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
		i = PEM_write_bio_X509_REQ(breq,req);		//写入公钥
		if(NULL == priPwd || strlen(priPwd) == 0)
			j = PEM_write_bio_PrivateKey(bkey,pkey,NULL,NULL,0,NULL, NULL); //私钥不加密
		else
			j = PEM_write_bio_PrivateKey(bkey,pkey,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)priPwd); //私钥加密
	}
	else if(iType == DER)
	{
		i = i2d_X509_REQ_bio(breq,req);	//写入公钥

		if(NULL == priPwd || strlen(priPwd) == 0)			//写入私钥
			j = i2d_PrivateKey_bio(bkey,pkey);//私钥未加密
		else
		{
			//添加加密链
			unsigned char key[EVP_MAX_KEY_LENGTH] = "";//算法最长的KEY长度
			unsigned char iv[EVP_MAX_IV_LENGTH] = "";//算法最长的IV长度
			BIO * bEnc = NULL;
			bEnc = BIO_new(BIO_f_cipher());
			const EVP_CIPHER * cipher = NULL;
			cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
			if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)priPwd,
				strlen(priPwd),1,key,iv))
			{
				BIO_set_cipher(bEnc,cipher,key,iv, 1);//1-加密、0-解密
				bkey = BIO_push(bEnc, bkey); 
				j = i2d_PrivateKey_bio(bkey,pkey);//私钥加密
				BIO_flush(bkey);
				bkey = BIO_pop(bkey);
				BIO_free(bEnc);
			}
			else
				strcpy(outMsg,"初始化key or iv 失败,");
		}
	}
	//转换BIO->MEM
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
int copy_extensions(X509 *x, X509_REQ *req, const int copy_type)//在证书中加入req自带扩展信息
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
//	STACK_OF (X509_EXTENSION) * req_exts;//如何释放??
	name = X509_REQ_get_subject_name(req);
	if ((ret = X509_new()) == NULL) 
	{
		ok=0;
		goto err;
	}
	ci = ret->cert_info;

	/* Make it an X509 v3 certificate. 版本1扩展*/
	if (!X509_set_version(ret,2L)) //版本
	{
		ok=0;
		goto err;
	}
	
	ASN1_INTEGER_set(X509_get_serialNumber(ret),serial);//序列号

	if (!X509_set_issuer_name(ret,X509_get_subject_name(px509)))//发行者
	{
		ok=0;
		goto err;
	}

	//X509_gmtime_adj(X509_get_notBefore(x),0);//起始时间
	//X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*days);//结束时间

	if (strcmp(startdate,"today") == 0)
		X509_gmtime_adj(X509_get_notBefore(ret),0);//开始日期
	else ASN1_UTCTIME_set_string(X509_get_notBefore(ret),(char *)startdate);
	
	if (enddate == NULL)
		X509_gmtime_adj(X509_get_notAfter(ret),(long)60*60*24*days);//结束日期
	else ASN1_UTCTIME_set_string(X509_get_notAfter(ret),(char *)enddate);

//	X509_gmtime_adj(X509_get_notBefore(ret),0);//起始时间
//	X509_gmtime_adj(X509_get_notAfter(ret),(long)60*60*24*days);//结束时间

	if (!X509_set_subject_name(ret,name)) //主体 ---所有者
	{
		ok=0;
		goto err;
	}
	
	pktmp = X509_REQ_get_pubkey(req);
	i = X509_set_pubkey(ret,pktmp);//公钥
	EVP_PKEY_free(pktmp);
	if (!i) 
	{
		ok = 0;
		goto err;
	}
	
	//加入req自带扩展信息，生成REQ文件时候加入的
	if (!copy_extensions(ret, req, EXT_COPY_ALL))
	{
		strcpy("加入自带扩展信息失败",outMsg);
		goto err;
	}

	//CRL签名、仅仅加密、仅仅解密
		
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


	//加入自定义信息end
	X509V3_EXT_cleanup();//cleanup the extension code if any custom extensions have been added
	OBJ_cleanup();
	if (!X509_sign(ret,pkey,dgst))//加入签名，签名算法
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
	//返回公钥证书，请求文件，根私钥，根公钥，
	EVP_PKEY *pktmp=NULL;
	int ok= -1,i=0;	
	if ((pktmp=X509_REQ_get_pubkey(req)) == NULL) //得到公钥
	{
		sprintf(outMsg,"error unpacking public key\n");
		ok=0;
		goto err;
	}
	i=X509_REQ_verify(req,pktmp); //证书请求里面有私要签名，这里验证一下，看此公钥主人是否持有私钥
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


BOOL MakeCert(const stuCertPair & RootPair/*根证书对*/,
			  const long serial/*序列号*/,
			  const char *enddate/*作废日期*/,
			  const int days/*有效期*/, 
			  const char * reqMem/*请求文件或内存*/,
			  const int reqLen/*请求的长度,0-标示文件*/,
			  const char * KUSAGE/*密钥用法*/,
			  const char * EKUSAGE/*增强密钥用法*/,
			  const stuCERTEXT * pCertExt,/*证书扩展*/
			  char * outMem/*结果公钥文件*/,
			  UINT * MemLen/*结果长度*/,
			  char * outMsg/*操作结果*/,
			  const int type/*结果类型DER,PEM*/)//通过证书请求，得到证书
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

	OpenSSL_add_all_algorithms();//加入签名算法
	X509 * x509 = NULL;

	if(reqLen == 0)//输入为磁盘文件
	{
		if((reqbio=BIO_new_file(reqMem, "r")) == NULL)
		{
			strcpy(outMsg,"找不到证书请求文件");
			ret=0;
			goto err;
		}
	}
	else//输入为内存中文件
	{
		if((reqbio = BIO_new_mem_buf((void *)reqMem, reqLen)) == NULL)//只读类型
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
	j=certify(&x,req,pkey,px509,dgst,//公钥证书out，请求文件，根私钥，根公钥，
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

	//转换BIO->内存
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
BOOL DirectCert(const stuCertPair &RootPair /*根证书对*/,
				const int bits /*[IN]*/,
				const long serial /*[IN]序列号*/,
				const char *enddate /*[IN]作废日期*/,
				const int days /*[IN]有效期*/,
				const char *priPwd /*私钥加密密码,IN*/,
				const stuSUBJECT *sSUBJECT /*[IN]用户信息*/,
				const char *friendlyName /*好记的名称*/,
				const char *keyUsage /*密钥用法*/,   const char *ekeyUsage /*扩展密钥用法*/,
				const stuCERTEXT * pCertExt, /*证书扩展结构*/
				char *pCert /*[OUT]输出证书公钥*/,   UINT *certl /*[OUT]长度*/,
				char *key /*[OUT]输出证书私钥*/,     UINT *keyl /*[OUT]长度*/,
				char *p12 /*[OUT]输出证书私钥*/,     UINT *p12l /*[OUT]长度*/,
				char *outMsg /*[OUT]返回错误信息*/,
				const int iType /*结果类型DER,PEM*/)
{
	X509_REQ * req=NULL;
	EVP_PKEY * pkey=NULL, 
			 * prkey=NULL;//证书私钥,//根私钥
	X509 * px509=NULL,
		 * x=NULL;//根公钥，证书公钥
	int ret=1;
	char * md=NULL;
	int i=0,
		j=0,
		ok=0;
	const EVP_MD *dgst=NULL;

	OpenSSL_add_all_algorithms();//加入签名算法

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
	ok = certify(&x,req,prkey,px509,dgst, //公钥证书out，请求，根私钥，根公钥，
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
int Add_ExtCrl(X509_CRL *crl/*正被添加的证书*/,X509 * pRoot/*根证书（从中得到信息)*/, 
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


BOOL MakeCrl(const stuCertPair & RootPair/*根证书对*/,
			 const stuREVOKE * Head/*作废链表*/,
			 const PNewCrlMem NewCrlMem/*回调函数*/,
			 char *& outCrl,
			 int * crll,
			 char * outfile/*crl文件*/,
			 char * outMsg/*操作结果*/)
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


	while(Head!=NULL)//遍历链表
	{
		X509_REVOKED *r = NULL;
        BIGNUM *serial_bn = NULL;
        r = X509_REVOKED_new();
		ASN1_TIME_set(r->revocationDate,Head->time);//时间
		char index[100];
        BN_hex2bn(&serial_bn,itoa(Head->Index,index,10));//序号  -  leak BN_free
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
	if(NewCrlMem)//输出内存
	{
		memcrl= BIO_new(BIO_s_mem());
		BIO_set_close(memcrl, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
		PEM_write_bio_X509_CRL(memcrl,crl);
		BIO_get_mem_ptr(memcrl, &bptrcrl);
		*crll=bptrcrl->length;
		outCrl=NewCrlMem(*crll);
		memcpy(outCrl,bptrcrl->data,*crll);
	}
	if(outfile)//输出文件
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
		strcpy(outMsg,"CRL制作成功");
	return ret;
}

BOOL CertFormatConver(const char * buf/*文件内容或文件名称*/,
					  const int len/*内存长度为0则buf为文件名*/,
					  const char * pwd/*p12文件密码,或者解密私钥密码*/,
					  char * pem /*输出内存*/,
					  UINT * pOutLen,/*输入内存长度,输出实际输出的长度*/
					  char * keyPwd,/*加密私钥密码,只在私钥时候起作用*/
					  const int outformat, char * outMsg/*操作结果*/)
{
	BOOL bRet = TRUE;
	EVP_PKEY *key=NULL;
	X509 *pCert=NULL;
	BIO * biout=NULL;
	int i=0;
	//输出文件
	if(pOutLen == 0)	//表示输出为文件
	{
		if ((biout=BIO_new_file(pem, "w")) == NULL)
		{
			return FALSE;
		}		
	}
	//输出
	else
	{
		biout = BIO_new(BIO_s_mem());
		BIO_set_close(biout, BIO_CLOSE); 
	}

	pCert = LoadCert(buf,len,pwd,outMsg);//首先尝试公钥,pBioKey被改写

	if(pCert)//输入文件为公钥文件
	{

		if 	(outformat == DER)
			i=i2d_X509_bio(biout,pCert);
		else if (outformat == PEM)
		{
		//	if (trustout) i=PEM_write_bio_X509_AUX(biout,x);
			i=PEM_write_bio_X509(biout,pCert);
		}
		if(!i)//失败
			strcpy(outMsg,"保存公钥失败");
		else
			strcpy(outMsg,"公钥证书格式转换成功");
	}
	else//输入文件为私钥文件
	{
		key=LoadKey(buf,len,pwd,outMsg);
		if(!key) 
		{
			strcpy(outMsg,"不能识别的文件格式");
			return FALSE;
		}
		if(outformat==PEM)
		{	
			if(keyPwd == NULL || strlen(keyPwd) == 0)
				PEM_write_bio_PrivateKey(biout, key, NULL, NULL, 0, 0, NULL); //私钥不加密
			else
				PEM_write_bio_PrivateKey(biout,key,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)keyPwd); //私钥加密
		}
		if(outformat==DER)
		{
			if(NULL == keyPwd || strlen(keyPwd) == 0)			//写入私钥
				i2d_PrivateKey_bio(biout,key);//私钥未加密
			else
			{
				//添加加密链
				unsigned char keyl[EVP_MAX_KEY_LENGTH]="";//算法最长的KEY长度
				unsigned char iv[EVP_MAX_IV_LENGTH]="";//算法最长的IV长度
				BIO * bEnc = NULL;
				if((bEnc = BIO_new(BIO_f_cipher())) != NULL)
				{
					const EVP_CIPHER * cipher = NULL;
					cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
					if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)keyPwd,
						strlen(keyPwd),1,keyl,iv))
					{
						BIO_set_cipher(bEnc,cipher,keyl,iv, 1);//1-加密、0-解密
						biout = BIO_push(bEnc, biout); 
						i2d_PrivateKey_bio(biout,key);//私钥加密
						BIO_flush(biout);
						biout = BIO_pop(biout);
						BIO_free(bEnc);
					}
					else
						strcpy(outMsg,"初始化key or iv 失败,");
				}
			}
		}
		
		strcpy(outMsg,"私钥证书格式转换成功");
	}

	if(*pOutLen != 0)	//表示输出为文件
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

//分解p12包
BOOL ParsePfx(const char * strP12/*包文件或内存*/,
			  const UINT iP12Len/* 如果包为文件,则为0,否则为内存长度*/,
			  const char * strPwdP12/*P12密码*/,
			  const char * strCert/*公钥存放*/,
			  const char * strkey/*私钥存放*/,
			  const char * keyPwd/*私钥密码*/,
			  const int outformat/*输出格式*/,
			  char * outMsg/*返回结果*/)
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
	
	if(iP12Len == 0)//输入为磁盘文件
	{
		if((bio = BIO_new_file(strP12, "r")) == NULL)
		{
			strcpy(outMsg,"加载PFX文件错误");
			bRet = FALSE;
			goto err;
		}
	}
	else//输入为内存中文件
	{
		if((bio = BIO_new_mem_buf((void *)strP12,iP12Len)) == NULL)//只读类型
		{
			sprintf(outMsg,"Make Mem Bio Error");
			bRet = FALSE;
			goto err;
		}
	}

	OpenSSL_add_all_algorithms();	//不能去掉
	p12 = d2i_PKCS12_bio(bio, NULL);
	if (!PKCS12_parse(p12, strPwdP12, &key, &pCert/*PEM*/, /*&ca*/NULL)) 
	{
		strcpy(outMsg,"解析文件失败");
		bRet = FALSE;
		goto err;
	}

	//输出文件 - 可能不输出公钥
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
			i = i2d_X509_bio(bioCert,pCert); //写入公钥
		if(bioKey)
		{
			if(NULL == keyPwd || strlen(keyPwd) == 0)			//写入私钥
				i2d_PrivateKey_bio(bioKey,key);//私钥未加密
			else
			{
				//添加加密链
				unsigned char keyl[EVP_MAX_KEY_LENGTH]="";//算法最长的KEY长度
				unsigned char iv[EVP_MAX_IV_LENGTH]="";//算法最长的IV长度
				BIO * bEnc = NULL;
				bEnc = BIO_new(BIO_f_cipher());
				const EVP_CIPHER * cipher = NULL;
				cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
				if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)keyPwd,
					strlen(keyPwd),1,keyl,iv))
				{
					BIO_set_cipher(bEnc,cipher,keyl,iv, 1);//1-加密、0-解密
					bioKey = BIO_push(bEnc, bioKey); 
					i2d_PrivateKey_bio(bioKey,key);//私钥加密
					BIO_flush(bioKey);
					bioKey = BIO_pop(bioKey);
					BIO_free(bEnc);
				}
				else
				{
					strcpy(outMsg,"初始化key or iv 失败,");
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
				j=PEM_write_bio_PrivateKey(bioKey, key, NULL, NULL, 0, 0, NULL);//私钥不加密
			else
				j=PEM_write_bio_PrivateKey(bioKey,key,EVP_des_ede3_cbc(),NULL,0,NULL, (void *)keyPwd); //私钥加密
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


//组合p12包
BOOL CreatePfx(char * StreamP12/*OUT包文件路径或内存区域*/,
			   UINT & uP12Len,/*输入时候为用于保存结果的内存区域长度, 输出为实际P12内存的长度*/
			   const char * P12EncPwd/*IN 用于加密P12的密码*/,
			   const char * FriendName,/*IN 好记名称*/
			   const char * StreamCert/*IN公钥*/,
			   const UINT uCertLen,/*IN,公钥内存区域长度, 0 - 标示输入为磁盘文件*/
			   const char * Streamkey/*IN私钥*/,
			   const UINT uKeyLen,/*IN,私钥内存区域长度, 0 - 标示输入为磁盘文件*/
			   const char * KeyDecPwd/*解密私钥密码*/,
			   char * outMsg/*返回结果*/)
{
	BOOL bRet = TRUE;
	EVP_PKEY *key = NULL;
	X509 *pCert = NULL;
	PKCS12 * p12 = NULL;
	int i=0;

	//输出文件
	BIO * biout = BIO_new(BIO_s_mem());
	BIO_set_close(biout, BIO_CLOSE); 

	pCert = LoadCert(StreamCert, uCertLen, "", outMsg);
	if(!pCert) 
	{
		strcpy(outMsg,"加载公钥文件失败");
		bRet = FALSE;
		goto err;
	}

	key = LoadKey(Streamkey, uKeyLen, KeyDecPwd, outMsg);//私钥
	if(!key) 
	{
		strcpy(outMsg,"加载私钥文件失败");
		bRet = FALSE;
		goto err;
	}

	OpenSSL_add_all_algorithms();

	p12 = PKCS12_create(const_cast<char *>(P12EncPwd), const_cast<char *>(FriendName), key, pCert, NULL, 0,0,0,0,0);
	if(!p12)
	{
		strcpy(outMsg,"创建p12结构失败");
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

//修改p12包密码
BOOL ChangePfxPwd(const char * strP12/*in老包文件*/,
			   const char * strPwd/*IN原密码*/,
			   const char * strPwd2/*IN新密码*/,
			   const char * strOutP12/*in新包文件*/,
			   char * outMsg/*返回结果*/)
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
	//输入文件 
	if ((bin=BIO_new_file(strP12, "rw")) == NULL)
	{
		strcpy(outMsg,"加载PFX文件错误");
		bRet = FALSE;
		goto err;
	}		
	p12 = d2i_PKCS12_bio(bin, NULL);
	if (!p12) 
	{
		strcpy(outMsg,"加载包文件失败");
		bRet = FALSE;
		goto err;
	}
	if (!PKCS12_parse(p12, strPwd, &key, &pCert, NULL)) 
	{
		strcpy(outMsg,"解包失败");
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
		strcpy(outMsg,"创建p12结构失败");
		bRet = FALSE;
		goto err;
	}
	//输出文件
	if ((biout = BIO_new_file(strP12, "w")) == NULL)
	{
		strcpy(outMsg,"创建输出文件失败");
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

//检验公钥、私钥是否配对
BOOL CertPairCheck(const char * pCert,
				   const char * key,
				   char * outMsg,
				   const char * priPwd )//检验公钥、私钥是否配对
{
	EVP_PKEY *pkey=NULL;
	X509 *px509=NULL;
	px509=LoadCert(pCert,0,"",outMsg);
	if(px509==NULL)
	{
		strcpy(outMsg,"不能加载公钥文件");
		return FALSE;
	}
	pkey=LoadKey(key,0,priPwd,outMsg);
	if(pkey==NULL)
	{
		strcpy(outMsg,"不能加载私钥文件");
		X509_free(px509);
		return FALSE;
	}
	if(X509_check_private_key(px509,pkey))//匹配
	{
		X509_free(px509);
		EVP_PKEY_free(pkey);
		return TRUE;
	}
	else
	{
		strcpy(outMsg,"公私钥对不匹配");
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
// 通过黑名单验证证书，验证通过返回真，否则返回假
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
		strcpy(outMsg,"加载证书失败");
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
	crl = d2i_X509_CRL_bio(in,NULL); //DER 格式?
	if(crl == NULL)
	{
		BIO_reset(in);//恢复bio
		crl = PEM_read_bio_X509_CRL(in,NULL,NULL,NULL); //PEM格式?
	}
	if(crl == NULL)
	{		
		strcpy(outMsg,"加载 crl 失败");
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
			strcpy(outMsg,"证书已作废");
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
			strcpy(outMsg,"证书有效");
	}
	return bf;
}
    
/////////////////////////////////////////////////////////////////////////////
// 通过根证书验证证书ENGINE_load_private_key
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
		strcpy(outMsg,"加载证书失败");
		bRet =  FALSE;
		goto end;
	}

	pRoot = LoadCert(rootCert,rootCertLen,NULL,outMsg);
	if (pRoot == NULL)
	{
		strcpy(outMsg,"加载根证书失败");
		bRet =  FALSE;
		goto end;
	}
    
	pcert = X509_get_pubkey(pRoot);
	if (pcert == NULL)
	{
		strcpy(outMsg,"读取根证书公钥信息失败");
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
			strcpy(outMsg,"证书与根证书匹配");
			return TRUE;
		}
		else
		{
			strcpy(outMsg,"证书与根证书不匹配");
			return FALSE;
		}
	}
	else
		return FALSE;
}
    
/////////////////////////////////////////////////////////////////////////////
// 检查证书有效期,在有效期内返回真，否则返回假
BOOL CheckCertLife(const char *pubCert,const int pubCertLen,char * outMsg)
{
	X509 *x509 = LoadCert(pubCert,pubCertLen,NULL,outMsg);
	if (x509 == NULL)
	{
		strcpy(outMsg,"加载证书失败");
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
		strcpy(outMsg,"证书超出有效期");
		bf = FALSE;
	}
	else
	{
		strcpy(outMsg,"证书有效");
		bf = TRUE;
	}
	M_ASN1_UTCTIME_free(be);
	M_ASN1_UTCTIME_free(af);
	X509_free(x509);
	return bf;
}
 

/*组合P7包*/
BOOL CreateP7b(std::list<std::string> * pCertList,
			   const char * lpszCrl,
			   const char * outP7b,
			   const int outformat,
			   char * outMsg/*返回结果*/)
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
	if(lpszCrl && strlen(lpszCrl) != 0)	//有黑名单文件
	{
		//加载CRL结构
		if (BIO_read_filename(in,lpszCrl) <= 0)
		{
			sprintf(outMsg, "加载CRL%s失败", lpszCrl);
			goto end;
		}
		crl = d2i_X509_CRL_bio(in,NULL); //DER 格式?
		if(crl == NULL)
		{
			BIO_reset(in);//恢复bio
			crl = PEM_read_bio_X509_CRL(in,NULL,NULL,NULL); //PEM格式?
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


/*分解P7包*/
BOOL ParseP7b(const char * lpszInP7b/*包文件或内存*/,
			  const UINT iP12Len/* 如果包为文件,则为0,否则为内存长度*/,
			  const int outformat/*输出格式*/,
			  const char * lpszCert/*公钥存放*/,
			  const char * lpszCrl/*CRL存放*/,
			  const char * lpszOutP7b/*用于转换P7格式的输出文件名*/,
			  char * outMsg/*返回结果*/)
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
		sprintf(outMsg, "读取P7B%s失败", lpszInP7b);
		ret = 0;
		goto end;
	}

	p7 = d2i_PKCS7_bio(in,NULL);
	if(!p7)
	{
		BIO_reset(in);//恢复bio
		p7=PEM_read_bio_PKCS7(in,NULL,NULL,NULL);
	}
	if (p7 == NULL)
	{
		sprintf(outMsg, "加载P7B%s失败", lpszInP7b);
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
			x = sk_X509_value(certs,i); //保存证书公钥
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

	//转换P7格式
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
			sprintf(outMsg, "保存P7B%s失败", lpszOutP7b);
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


//转换证书到REQ
BOOL X5092Req(char * Cert/*公钥*/,int Certlen,
			  char * Key/*私钥,用于签名公钥*/,int Keylen,
			  char * Keypwd/*密码*/,
			  char * outFile/*输出文件,req或者公钥证书*/,
			  char * outMsg,/*操作结果*/
			  char * Rootcert,/*根证书公钥*/int Rootlen,/*为0则certfile为磁盘文件，否则为内存区域*/
			  char * Rootkey/*根证书私钥*/,int RKeylen,
			  char * Pwd/*私钥密码*/)
{
	X509_REQ * req = NULL;
	EVP_PKEY * pkey = NULL,
		     * prkey = NULL;//证书私钥,//根私钥

	X509 * x509 = NULL,
		 * x = NULL,
		 * newx = NULL;//根公钥，证书公钥,新公钥证书

	BIO * memcert = NULL, 
		* memkey = NULL;//输出证书公私钥

	BUF_MEM * bptrcert = NULL,
		    * bptrkey = NULL;

	char * md=NULL;

	BOOL bReq = TRUE;//制作REQ还是CERT ,TRUE 为REQ

	int i = 0,
		j = 0,
		ok = 0,
		ret = 1;

	const EVP_MD * dgst = NULL;

	BIO * out = BIO_new_file(outFile, "w");

	OpenSSL_add_all_algorithms();//加入签名算法

	memcert= BIO_new(BIO_s_mem());
	memkey= BIO_new(BIO_s_mem());
	BIO_set_close(memcert, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */
	BIO_set_close(memkey, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */

	pkey = LoadKey(Key, Keylen, Keypwd, outMsg);//加载私钥
	if (pkey == NULL)
	{
		ret = 0;
		goto err;
	}

	x = LoadCert(Cert,Certlen,"",outMsg);//加载公钥
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

	//得到公钥REQ
	req = X509_to_X509_REQ(x,pkey,EVP_sha1());//EVP_md5
	if (req == NULL)
	{
		strcpy(outMsg,"证书转换为REQ失败");
		ret = 0;
		goto err;
	}


/*	prkey = LoadKey(Rootkey,Keylen,Pwd,outMsg);//加载根私钥
	if (prkey != NULL)
	{
		x509 = LoadCert(Rootcert,Rootlen,"",outMsg);
		if (x509 != NULL)
		{
			if(X509_check_private_key(x509,prkey))//匹配
			{
				bReq = FALSE;//制作证书
			}
			else
				sprintf(outMsg,"根证书公私钥对不匹配");
		}
		else
			sprintf(outMsg,"不能加载根证书公钥");
	}
	else
		sprintf(outMsg,"不能加载根证书私钥");

	if(bReq)//REQ
	{
		PEM_write_bio_X509_REQ(out,rq);
	}
	else //证书
	{
		md="sha1";//////////!!!!!!!!!!!!!!!!!////////////////////////////
		if ((dgst=EVP_get_digestbyname(md)) == NULL)//return an EVP_MD structure when passed a digest name
		{
			sprintf(outMsg,"%s is an unsupported message digest type\n",md);
			ret = 0;
			goto err;
		}
		j=certify(&newx,req,prkey,x509,dgst,//公钥证书out，请求文件，根私钥，根公钥，
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

这是转换函数：  
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
下面是调用方法：  
     utf-8  到  gbk  
           int  nLen  =  CodePageConvertUnix("UTF-8",_T("标准"),2,"GBK",lpOut);  
     gbk  到utf-8  
int  nLen  =  CodePageConvertUnix("UTF-8",_T("标准"),2,"GBK",lpOut); 



 	//加入扩展信息
//	Add_ExtCert(ret,ret,NID_basic_constraints, "critical,CA:FALSE,pathlen:1");

//	Add_ExtCert(x,x,NID_basic_constraints, "critical,CA:TRUE,pathlen:1");

	//主题密钥标示符---当发行者有多个签名密钥时
//	Add_ExtCert(x,x,NID_subject_key_identifier, "hash");
	//颁发机构密钥标示符
//	Add_ExtCert(x,x,NID_authority_key_identifier, "keyid:always");

	//
//	Add_ExtCert(x,x,NID_certificate_policies, "serverAuth");
	
	//主题备用名称,URL:http://my.url.here/、支持email  copy
//	Add_ExtCert(x ,x, NID_subject_alt_name, "email:camgr@rog.majki.net,URI:altname:/mirabile/,URI:otherName://tygScaowlOmi,email:nc-glaserth@netcologne.de");

	//加入自定义信息begin
/*	int nid;
	nid = OBJ_create("1.2.3.4.9", "Hpxs", "I love you!");
	X509V3_EXT_add_alias(nid, NID_netscape_comment);
	Add_ExtCert(x, nid, "I love you");
	//加入自定义信息end
*/

  
	
	/* Lets add the extensions, if there are any 加入标准扩展*/

