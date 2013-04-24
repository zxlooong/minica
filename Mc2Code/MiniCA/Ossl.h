// Ossl.h: interface for the COssl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OSSL_H__A3244E3A_00BE_4EF3_AD97_5B5891A1A98A__INCLUDED_)
#define AFX_OSSL_H__A3244E3A_00BE_4EF3_AD97_5B5891A1A98A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include <time.h>
#define DER			    1 //FORMAT_ASN1
#define PEM			    3	/*定义格式*/
#define NET				4
#define P12				5
#define MAX_MD_SIZE		(16+20) /* The SSLv3 md5+sha1 type */

#pragma warning(disable:4786)
#include <string>
#include <list>

typedef char * (* PNewCrlMem)(UINT len);


namespace Ossl
{


struct stuCertPair //公私钥结构
{
	const char * memCert;//公钥文件名或公钥内存
	UINT lenCert;//文件 - 0,内存 - 内存长度
	const char * memKey;//私钥文件名或私钥内存
	UINT lenKey;//文件 - 0,内存 - 内存长度
	const char * pwdKey;//私钥密码
	stuCertPair()
	{
		memset(this,0,sizeof(stuCertPair));
	}
	stuCertPair(const char * memcert,const UINT lencert,
		        const char * memkey,const UINT lenkey,
				const char * pwdkey)
	{
		memCert = memcert;
		lenCert = lencert;
		memKey = memkey;
		lenKey = lenkey;
		pwdKey = pwdkey;
	}
};

typedef struct stuCERTEXT//证书信息和扩展结构
{
	char * cName;//内部标准名称
	char * cInfo;//内容
	stuCERTEXT * Link;
	stuCERTEXT()
	{
		memset(this,0,sizeof(stuCERTEXT));
	}

	stuCERTEXT(const stuCERTEXT & Link)
	{
		this->cName = Link.cName;
		this->cInfo = Link.cInfo;
		this->Link = Link.Link;
	}

	stuCERTEXT(const char *  cName,const char * cInfo)
	{
		this->cName =  new char[strlen(cName)+1];
		strcpy(this->cName,cName);

		this->cInfo = new char[strlen(cInfo)+1];
		strcpy(this->cInfo,cInfo);
		Link = NULL;
	}

	~stuCERTEXT()
	{
		delete [] this->cName;
		delete [] this->cInfo;
	}

	void Add(stuCERTEXT *& Head,const char * cName,const char * cInfo)
	{
		stuCERTEXT * End = new stuCERTEXT(cName,cInfo);//钥增加的节点
		if(Head == NULL)
		{
			Head = End;
		}
		else 
		{
			stuCERTEXT * p = Head;
			while(p->Link != NULL)  
				p = p->Link;
			p->Link = End; 
		}
	}

	const char * GetCN()
	{
		//查找CN
		return "hpxs";
	}

	void RemoveAll(stuCERTEXT *& Head)
	{
		while(Head!=NULL)//遍历链表
		{
			stuCERTEXT * temp = NULL;
			temp = Head;
			Head = Head->Link;
			delete temp;
		}
	}
}stuSUBJECT;

struct stuREVOKE//证书作废结构链表
{
	ULONG Index;//证书序号
	time_t time;//吊销时间
	stuREVOKE * Link;
	stuREVOKE()
	{
		memset(this,0,sizeof(stuREVOKE));
	}

	stuREVOKE(ULONG index,time_t t)
	{
		Index = index;
		time = t;
		Link = NULL;
	}

	void AddRevoke(stuREVOKE *& Head,const ULONG index,const time_t time)
	{
		stuREVOKE * End = new stuREVOKE(index,time);//钥增加的节点
		if(Head == NULL)
		{
			Head = End;
		}
		else 
		{
			stuREVOKE * p = Head;
			while(p->Link != NULL)  
				p = p->Link;
			p->Link = End; 
		}
	}

	void RemoveAll(stuREVOKE *& Head)
	{
		while(Head != NULL)//遍历链表
		{
			stuREVOKE * temp =NULL;
			temp = Head;
			Head = Head->Link;
			delete temp;
		}
	}
};


class COssl  
{
public:
	COssl();
	virtual ~COssl();
public:

/*证书格式转换函数*/
BOOL CertFormatConver(const char * buf/*文件内容或文件名称*/,
					  const int len/*内存长度为0则buf为文件名*/,
					  const char * pwd/*p12文件密码,或者解密私钥密码*/,
					  char * pem/*输出内存*/,
					  UINT * pOutLen,/*输入内存长度,输出实际输出的长度*/
					  char * keyPwd,/*加密私钥密码,只在私钥时候起作用*/
					  const int outformat,
					  char * outMsg/*操作结果*/);

/*根证书生成函数，根据rootInfo信息，生成根证书公、私钥文件*/
BOOL MakeRoot(const stuSUBJECT * RootInfo,/*[in]信息*/
			  const char * FriendlyName/*好记的名称,出现在P12包里面*/,
			  const int iKeyBits/*[in]位数*/, 
			  const long CertSerial/*[in]序列号*/, 
			  const int CertDays/*[in]有效期*/,
			  const char * KeyEncPwd/*[in]加密私钥密码*/,
			  const char * Kusage,/*密钥用法*/
			  const char * Ekusage,/*扩展密钥用法*/
			  const stuCERTEXT * pCertExt,/*证书扩展*/
			  char * CertMem/*[OUT]证书*/,
			  UINT * CertLen/*[OUT]*/,
			  char * KeyMem/*[OUT]私钥*/,
			  UINT * KeyLen/*[OUT]*/,
			  char * P12Mem/*[OUT]pkcs#12*/,
			  UINT * P12Len/*[OUT]*/,
			  char * outMsg,/*操作结果*/
			  const int type = DER/*[in]类型pem-der*/);

/*证书请求生成函数，根据reqInfo信息，生成用户证书私钥文件、证书请求文件*/
BOOL MakeReq(const stuSUBJECT * ReqInfo,/*请求信息IN*/
			 const int KeyBits/*位数IN*/,
			 const char * KeyEncPwd/*私钥密码,IN*/,
			 char * ReqMem/*证书请求文件OUT*/,
			 UINT * ReqLen/*证书请求文件长度OUT*/,
			 char * KeyMem/*私钥文件OUT*/,
			 UINT * KeyLen/*私钥文件文件长度OUT*/,
			 char * outMsg/*操作结果OUT*/,
			 const int iType = DER/*类型pem-der*/);

/*证书生成函数，通过证书请求，生成用户证书公钥文件*/
BOOL MakeCert(const stuCertPair & RootPair/*根证书对*/,
			  const long CertSerial/*序列号*/,
			  const char * CertEndDate/*作废日期*/,
			  const int CertDays/*有效期*/, 
			  const char * StreamReq/*请求文件或内存*/,
			  const int ReqLen/*请求的长度,0-标示文件*/,
			  const char * KUSAGE/*密钥用法*/,
			  const char * EKUSAGE/*增强密钥用法*/,
			  const stuCERTEXT * pCertExt,/*证书扩展*/
			  char * CertMem/*结果公钥文件*/,
			  UINT * CertLen/*结果长度*/,
			  char * outMsg/*操作结果*/,
			  const int type = DER/*结果类型DER,PEM*/);//通过证书请求，得到证书

//直接生成公私钥
BOOL DirectCert(const stuCertPair & RootPair/*根证书对*/,
				const int KeyBits/*[IN]*/,
				const long CertSerial/*[IN]序列号*/,
				const char * CertEndDate/*[IN]作废日期*/,
				const int CertDays/*[IN]有效期*/,
				const char * KeyEncPwd /*私钥加密密码,IN*/,
				const stuSUBJECT * sSUBJECT/*[IN]用户信息*/,
				const char * FriendlyName/*好记的名称*/,
				const char * KeyUsage/*密钥用法*/,
				const char * EkeyUsage/*扩展密钥用法*/,
				const stuCERTEXT * pCertExt,
				char * CertMem/*[OUT]输出证书公钥*/,
				UINT * CertLen/*[OUT]长度*/,
				char * KeyMem/*[OUT]输出证书私钥*/,
				UINT * KeyLen/*[OUT]长度*/,
				char * P12Mem/*[OUT]输出证书私钥*/,
				UINT * P12Len/*[OUT]长度*/,
				char * outMsg,
				const int type = DER/*结果类型DER,PEM*/);//直接生成公私钥

/*黑名单生成函数*/
BOOL MakeCrl(const stuCertPair & RootPair/*根证书对*/,
			 const stuREVOKE * Head/*作废链表*/,
			 const PNewCrlMem NewCrlMem/*回调函数*/,
			 char *& outCrl,
			 int * crll,
			 char * outfile/*crl文件*/,
			 char * outMsg/*操作结果*/);

/*转换证书到REQ,如果根证书公私钥无法加载,则生成REQ否则生成证书*/
//BOOL X5092Req(const char * Cert/*公钥*/,const int Certlen,const char * Key/*私钥,用于签名公钥*/,const int Keylen,
//			  const char * Keypwd/*密码*/,
//			  const char * outFile/*输出文件,req或者公钥证书*/,char * outMsg,/*操作结果*/
//			  char * Rootcert,/*根证书公钥*/int Rootlen,/*为0则certfile为磁盘文件，否则为内存区域*/
//			  char * Rootkey/*根证书私钥*/,int RKeylen,char * Pwd/*私钥密码*/);

/*分解p12包*/
BOOL ParsePfx(const char * StreamP12/*包文件或内存*/,
			  const UINT iP12Len/* 如果包为文件,则为0,否则为内存长度*/,
			  const char * P12DecPwd/*P12密码*/,
			  const char * PathCert/*公钥存放*/,
			  const char * PathKey/*私钥存放*/,
			  const char * KeyEncPwd/*私钥密码*/,
			  const int OutFormat/*输出格式*/,
			  char * outMsg/*返回结果*/);

/*组合p12包*/
BOOL CreatePfx(char * StreamP12/*OUT包文件路径或内存区域*/,
			   UINT & uP12Len,/*输入时候为用于保存结果的内存区域长度, 输出为实际P12内存的长度*/
			   const char * P12EncPwd/*IN 用于加密P12的密码*/,
			   const char * FriendName,/*IN 好记名称*/
			   const char * StreamCert/*IN公钥*/,
			   const UINT uCertLen,/*IN,公钥内存区域长度, 0 - 标示输入为磁盘文件*/
			   const char * Streamkey/*IN私钥*/,
			   const UINT uKeyLen,/*IN,私钥内存区域长度, 0 - 标示输入为磁盘文件*/
			   const char * KeyDecPwd/*解密私钥密码*/,
			   char * outMsg/*返回结果*/);

BOOL ChangePfxPwd(const char * strP12/*in老包文件*/,
			   const char * strPwd/*IN原密码*/,
			   const char * strPwd2/*IN新密码*/,
			   const char * strOutP12/*in新包文件*/,
			   char * outMsg/*返回结果*/);

//检验公钥、私钥是否配对
BOOL CertPairCheck(const char * cert,
				   const char * key,
				   char * outMsg,
				   const char * priPwd = NULL/*私钥密码,IN*/);//检验公钥、私钥是否配对


// 通过黑名单验证证书，验证通过返回真，否则返回假
BOOL CheckCertWithCrl(const char *pubCert,const int pubCertLen,const char *crlData,const int crlLen,char * outMsg);

// 通过根证书验证证书
BOOL CheckCertWithRoot(const char *pubCert,const int pubCertLen,const char *rootCert,const int rootCertLen,char * outMsg);

//检查证书有效期,在有效期内返回真，否则返回假
BOOL CheckCertLife(const char *pubCert,const int pubCertLen,char * outMsg);

//根证书预览
//BOOL MakeRootPub(stuSUBJECT * rootInfo,/*信息*/ char * certMem/*证书文件*/,int * certLen,char * outMsg/*操作结果*/);

/*组合P7包*/
BOOL CreateP7b(std::list<std::string> * pCertList,
			   const char * lpszCrl,
			   const char * lpszP7b,
			   const int outformat,
			   char * outMsg/*返回结果*/);


/*分解P7包*/
BOOL ParseP7b(const char * lpszInP7b/*包文件或内存*/,
			  const UINT iP12Len/* 如果包为文件,则为0,否则为内存长度,预留将来用*/,
			  const int outformat/*输出格式*/,
			  const char * lpszCert/*公钥存放*/,
			  const char * lpszCrl/*CRL存放*/,
			  const char * lpszOutP7b/*用于转换P7格式的输出文件名*/,
			  char * outMsg/*返回结果*/);

public:
	void LastError(char * info);
	X509 *load_cert(BIO * pBioCert/*输入BIO*/, const int iFormat/*格式*/,
					const char * lpszPwd,/*P12密码*/
					char * outMsg); //从DER、PEM、P12格式中加载公钥证书
	X509 * LoadCert(const char * lpszCert,const int iCertlen,
					const char * lpszPass,char * outMsg);//枚举DER/PEM格式
	EVP_PKEY * load_key(BIO * pBioKey, const int iFormat, const char * lpszPass,char * outMsg); //枚举DER/PEM格式
	EVP_PKEY * LoadKey(const char * lpszkey,const int iKeylen,const char * lpszPass,char * outMsg); //leak 4772
	int Rand(const char * file,const int dont_warn,char * outMsg); //产生随机数,return 0 ---成功
	int Add_ExtCert(X509 * pCert/*正被添加的证书*/,
					X509 * pRoot/*根证书（从中得到信息)*/,
					const int iNid, const char * lpszValue);
	int Add_ExtCert(X509 *pCert/*正被添加的证书*/,
					X509 * pRoot/*根证书（从中得到信息)*/, 
					const char * lpszName, const char *lpszValue);
	int AddExtCert(X509 *pCert/*正被添加的证书*/,
					X509 * pRoot/*根证书（从中得到信息)*/, 
					const stuCERTEXT * pCertExt);
	void Ansi2Utf8(const LPSTR lpsrc, const int srclen, LPSTR lpdst, int& dstlen);
	BOOL Add_Name(X509_NAME * px509Name,
					const int iNid/*c\cn*/,
					const int iType/*V_ASN1_UTF8STRING*/,
					const char * lpszInput/*中国*/,
					const int iLen/*输入长度*/);  //支持中文名称
	BOOL Add_Name(X509_NAME * px509Name,
					const char * field/*c\cn*/,
					const char * lpszInput/*中国*/,
					const int iLen/*输入长度*/); //支持中文名称
	void AddName(X509_NAME * px509Name,const stuSUBJECT * pSubJect);
	BOOL Bio2Mem(BIO * memBio/*[IN]*/,char * lpszMem/*[OUT]*/,
				UINT * memLen/*[IN,OUT],输入为mem长度,输出为实际长度*/);
	BOOL X5092Mem(X509 *px509/*[IN]*/, const int iType /*[in]类型pem-der*/,
					char *x509Mem/*[OUT]*/,UINT *x509Len /*[OUT]*/);
	BOOL Key2Mem(EVP_PKEY * pkey/*[IN]*/,const int iType/*[in]类型pem-der*/,
				const char * priPwd/*[in]私钥密码*/,
				char * keyMem/*[OUT]*/,UINT * keyLen/*[OUT]*/);
	BOOL CertPair2XE(const stuCertPair & RootPair/*[IN]*/,X509 *& pCert, EVP_PKEY *& pKey ,char * outMsg);
	BOOL CreateMemPfx(X509 * pCert, EVP_PKEY * key, const  char * p12Pwd, const char * friendlyName,
						char * memP12/*OUT*/, UINT * p12Len/*OUT*/);
	BOOL mkRoot(const stuSUBJECT *rootInfo,
				X509 ** x509p/*out公钥*/, 
				EVP_PKEY ** pkeyp/*out私钥*/, 
				int bits/*位数*/, 
				const long serial/*序列号*/, 
				const int days/*有效期*/,
				const char * kusage,
				const char * ekusage,
				const stuCERTEXT *pCertExt,/*扩展*/
				char * outMsg/*操作结果*/);
	int Add_ExtReq(STACK_OF(X509_REQUEST) *sk,const int iNid,const char *lpszValue);
	int mkReq(const stuSUBJECT * reqInfo,
				X509_REQ **req,
				EVP_PKEY **pkeyp, 
				int bits,
				char * outMsg);
	
	int copy_extensions(X509 *x, X509_REQ *req, const int copy_type); //在证书中加入req自带扩展信息
	int do_body(X509 **xret, EVP_PKEY *pkey, X509 *px509, 
				const EVP_MD *dgst,const long serial,
				const char *startdate, const char *enddate,
				const int days,X509_REQ * req,const char * kusage,
				const char * ekusage,const stuCERTEXT * pCertExt,char * outMsg);

	int certify(X509 **xret, X509_REQ *req, EVP_PKEY *pkey, 
				X509 *px509,const EVP_MD *dgst,
				const long serial, const char *startdate, const char *enddate, 
				const int days,const char * KUSAGE,
				const char * EKUSAGE,const stuCERTEXT * pCertExt,char * outMsg);
	int Add_ExtCrl(X509_CRL *crl/*正被添加的证书*/,X509 * pRoot/*根证书（从中得到信息)*/, 
			   const int iNid, const char *lpszValue);
	int HexToTen(const char * pHex);
	void Utf8ToAnsi(const UCHAR * lpsrc,const int srclen, LPSTR lpdst, int& dstlen);
	BOOL X5092Req(char * Cert/*公钥*/,int Certlen,
					char * Key/*私钥,用于签名公钥*/,int Keylen,
					char * Keypwd/*密码*/,
					char * outFile/*输出文件,req或者公钥证书*/,
					char * outMsg,/*操作结果*/
					char * Rootcert,/*根证书公钥*/int Rootlen,/*为0则certfile为磁盘文件，否则为内存区域*/
					char * Rootkey/*根证书私钥*/,int RKeylen,
					char * Pwd/*私钥密码*/);
	

};

}

#endif // !defined(AFX_OSSL_H__A3244E3A_00BE_4EF3_AD97_5B5891A1A98A__INCLUDED_)
