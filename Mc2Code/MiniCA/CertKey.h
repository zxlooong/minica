// CertKey.h: interface for the CCertKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CERTKEY_H__1727C3A6_9A84_4000_837B_9327D096738D__INCLUDED_)
#define AFX_CERTKEY_H__1727C3A6_9A84_4000_837B_9327D096738D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <openssl/ssl.h>
#include <openssl/pkcs12.h>
#include <openssl/rand.h>
#include <openssl/err.h>

class CCertKey  
{
public:
	CCertKey()
	{
	}
	virtual ~CCertKey()
	{
	}

public:
	static X509 * CCertKey::LoadCert(char * cert,int certlen,char * pass,char * outMsg)//枚举DER/PEM格式
	{
		BIO * in = NULL;
		X509 * x509 = NULL;
		
		if(certlen==0)//输入为磁盘文件
		{
			if((in = BIO_new_file(cert, "r")) == NULL)
			{
				sprintf(outMsg,"open CA certificate file %s error", cert);
				return NULL;
			}
		}
		else//输入为内存中文件
		{
			if((in = BIO_new_mem_buf(cert,certlen))== NULL)//只读类型
			{
				sprintf(outMsg,"Make Mem Bio Error");
				return NULL;
			}
			BIO_set_close(in, BIO_CLOSE); 
		}
		if((x509 = load_cert(in,DER,NULL,outMsg))==NULL)//尝试DER
		{
			BIO_reset(in);//恢复bio
			if((x509 = load_cert(in,PEM,NULL,outMsg))==NULL)//尝试PEM
			{
				BIO_reset(in);//恢复bio
				x509 = load_cert(in,P12,pass,outMsg);//尝试P12
			}
		}
		if (in != NULL) BIO_free(in);
		if(x509)
			sprintf(outMsg, "");
//		CRYPTO_cleanup_all_ex_data();
		return x509;
	}
	
	static EVP_PKEY * CCertKey::LoadKey(char * key,int keylen,char * pass,char * outMsg)
	{
		EVP_PKEY *pkey=NULL;
		BIO * in=NULL;
		
		if(keylen==0)//输入为磁盘文件
		{
			if((in=BIO_new_file(key, "r")) == NULL)
			{
				sprintf(outMsg,"open CA certificate file %s error", key);
				return NULL;
			}
		}
		else//输入为内存中文件
		{
			if((in=BIO_new_mem_buf(key,keylen))== NULL)//只读类型
			{
				sprintf(outMsg,"Make Mem Bio Error");
				return NULL;
			}
		}
		
		if((pkey=load_key(in,DER,pass,outMsg))==NULL)//尝试DER
		{
			BIO_reset(in);//BIO是可读写的，那么该BIO所有数据都会被清空；
			//如果该BIO是只读的，那么该操作只会简单将指
			//针指向原始位置，里面的数据可以再读.
			if((pkey=load_key(in,PEM,pass,outMsg))==NULL)//尝试PEM
			{
				BIO_reset(in);
				pkey=load_key(in,P12,pass,outMsg);
			}
		}
		if (in != NULL) BIO_free(in);
		if(pkey)
			sprintf(outMsg, "");
//		CRYPTO_cleanup_all_ex_data();
		return pkey;
	}
	
private:

	static X509 * CCertKey::load_cert(BIO *cert/*输入BIO*/, int format/*格式*/,char * pwd,/*P12密钥*/char * outMsg)
	{
		ASN1_HEADER *ah=NULL;
		BUF_MEM *buf=NULL;
		X509 *x=NULL;
		
		if 	(format == DER)
			x=d2i_X509_bio(cert,NULL);
		else if (format == PEM)
			x=PEM_read_bio_X509(cert,NULL,NULL,NULL);//PEM_read_bio_X509_AUX
		else if (format == P12)
		{
			SSLeay_add_all_algorithms();
			PKCS12 *p12 = d2i_PKCS12_bio(cert, NULL);
			PKCS12_parse(p12, pwd, NULL, &x, NULL);
			PKCS12_free(p12);
			p12 = NULL;
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

		if (ah != NULL) ASN1_HEADER_free(ah);
		if (buf != NULL) BUF_MEM_free(buf);
		return(x);
	}
	
	static EVP_PKEY * CCertKey::load_key(BIO *bio, int format, char * pass,char * outMsg)
	{
		EVP_PKEY *pkey=NULL;
		SSLeay_add_all_algorithms();
		
		if (format == DER)
		{
			if(NULL == pass || strlen(pass) == 0)
			{
				pkey=d2i_PrivateKey_bio(bio, NULL);
			}
			else
			{
				//添加解密链
				unsigned char key[EVP_MAX_KEY_LENGTH]="";//算法最长的KEY长度
				unsigned char iv[EVP_MAX_IV_LENGTH]="";//算法最长的IV长度
				BIO * bDec = NULL;
				bDec = BIO_new(BIO_f_cipher());
				const EVP_CIPHER * cipher = NULL;
				cipher = EVP_des_ede3_cbc(); // 3DES-EDE-CBC 
				if(EVP_BytesToKey(cipher,EVP_sha1(),NULL,(unsigned char *)pass,
					strlen(pass),1,key,iv))
				{
					BIO_set_cipher(bDec,cipher,key,iv, 0);//1-加密、0-解密
					bio = BIO_push(bDec, bio); 
					BIO_flush(bio);
					pkey = d2i_PrivateKey_bio(bio, NULL);//私钥解密
					bio = BIO_pop(bio);
					BIO_free(bDec);


				}
				else
				{
					strcpy(outMsg,CMiniCaApp::NormalCode("初始化key or iv 失败"));
					goto end;
				}
			}
		}
		else if (format == PEM)
		{
			pkey=PEM_read_bio_PrivateKey(bio,NULL,NULL,pass);
		}
		else if (format == P12)
		{
			SSLeay_add_all_algorithms();
			PKCS12 *p12 = d2i_PKCS12_bio(bio, NULL);
			PKCS12_parse(p12, pass, &pkey, NULL, NULL);
			PKCS12_free(p12);
			p12 = NULL;
		}
		else
		{
			sprintf(outMsg,"bad input format specified for key\n");
			goto end;
		}
end:
		if (pkey == NULL)
			sprintf(outMsg,"unable to load Private Key\n");
		else
			sprintf(outMsg,"");
//		EVP_cleanup();
		return(pkey);
	}
};

#endif // !defined(AFX_CERTKEY_H__1727C3A6_9A84_4000_837B_9327D096738D__INCLUDED_)
