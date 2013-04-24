#include <time.h>
#define DER			    1 //FORMAT_ASN1
#define PEM			    3	/*�����ʽ*/
#define NET				4
#define P12				5
#define MAX_MD_SIZE		(16+20) /* The SSLv3 md5+sha1 type */

#pragma warning(disable:4786)
#include <string>
#include <list>

typedef char * (* PNewCrlMem)(UINT len);

/*struct stuSUBJECT//������Ϣ  �����ı����������ƶ����Ĳ�������,ֻ������һ�����峤��
{
	UCHAR C[16];//����
	UCHAR ST[16];//ʡ��
	UCHAR L[32];//����
	UCHAR O[100];//��֯
	UCHAR OU[100];//��֯����
	UCHAR CN[100];//������Ϣ
	UCHAR T[32];//ͷ��
	UCHAR G[32];//������
	UCHAR S[32];//����
	UCHAR MAIL[96];//�����ʼ�

	stuSUBJECT()
	{
		memset(this,0,sizeof(stuSUBJECT));
	}
};*/

struct stuCertPair //��˽Կ�ṹ
{
	const char * memCert;//��Կ�ļ�����Կ�ڴ�
	UINT lenCert;//�ļ� - 0,�ڴ� - �ڴ泤��
	const char * memKey;//˽Կ�ļ�����˽Կ�ڴ�
	UINT lenKey;//�ļ� - 0,�ڴ� - �ڴ泤��
	const char * pwdKey;//˽Կ����
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

typedef struct stuCERTEXT//֤����Ϣ����չ�ṹ
{
	char * cName;//�ڲ���׼����
	char * cInfo;//����
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
		stuCERTEXT * End = new stuCERTEXT(cName,cInfo);//Կ���ӵĽڵ�
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
		//����CN
		return "hpxs";
	}

	void RemoveAll(stuCERTEXT *& Head)
	{
		while(Head!=NULL)//��������
		{
			stuCERTEXT * temp = NULL;
			temp = Head;
			Head = Head->Link;
			delete temp;
		}
	}
}stuSUBJECT;

struct stuREVOKE//֤�����Ͻṹ����
{
	ULONG Index;//֤�����
	time_t time;//����ʱ��
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
		stuREVOKE * End = new stuREVOKE(index,time);//Կ���ӵĽڵ�
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
		while(Head != NULL)//��������
		{
			stuREVOKE * temp =NULL;
			temp = Head;
			Head = Head->Link;
			delete temp;
		}
	}
};

/*֤���ʽת������*/
BOOL CertFormatConver(const char * buf/*�ļ����ݻ��ļ�����*/,
					  const int len/*�ڴ泤��Ϊ0��bufΪ�ļ���*/,
					  const char * pwd/*p12�ļ�����,���߽���˽Կ����*/,
					  char * pem/*����ڴ�*/,
					  UINT * pOutLen,/*�����ڴ泤��,���ʵ������ĳ���*/
					  char * keyPwd,/*����˽Կ����,ֻ��˽Կʱ��������*/
					  const int outformat,
					  char * outMsg/*�������*/);

/*��֤�����ɺ���������rootInfo��Ϣ�����ɸ�֤�鹫��˽Կ�ļ�*/
BOOL MakeRoot(const stuSUBJECT * RootInfo,/*[in]��Ϣ*/
			  const char * FriendlyName/*�üǵ�����,������P12������*/,
			  const int iKeyBits/*[in]λ��*/, 
			  const long CertSerial/*[in]���к�*/, 
			  const int CertDays/*[in]��Ч��*/,
			  const char * KeyEncPwd/*[in]����˽Կ����*/,
			  const char * Kusage,/*��Կ�÷�*/
			  const char * Ekusage,/*��չ��Կ�÷�*/
			  const stuCERTEXT * pCertExt,/*֤����չ*/
			  char * CertMem/*[OUT]֤��*/,
			  UINT * CertLen/*[OUT]*/,
			  char * KeyMem/*[OUT]˽Կ*/,
			  UINT * KeyLen/*[OUT]*/,
			  char * P12Mem/*[OUT]pkcs#12*/,
			  UINT * P12Len/*[OUT]*/,
			  char * outMsg,/*�������*/
			  const int type = DER/*[in]����pem-der*/);

/*֤���������ɺ���������reqInfo��Ϣ�������û�֤��˽Կ�ļ���֤�������ļ�*/
BOOL MakeReq(const stuSUBJECT * ReqInfo,/*������ϢIN*/
			 const int KeyBits/*λ��IN*/,
			 const char * KeyEncPwd/*˽Կ����,IN*/,
			 char * ReqMem/*֤�������ļ�OUT*/,
			 UINT * ReqLen/*֤�������ļ�����OUT*/,
			 char * KeyMem/*˽Կ�ļ�OUT*/,
			 UINT * KeyLen/*˽Կ�ļ��ļ�����OUT*/,
			 char * outMsg/*�������OUT*/,
			 const int iType = DER/*����pem-der*/);

/*֤�����ɺ�����ͨ��֤�����������û�֤�鹫Կ�ļ�*/
BOOL MakeCert(const stuCertPair & RootPair/*��֤���*/,
			  const long CertSerial/*���к�*/,
			  const char * CertEndDate/*��������*/,
			  const int CertDays/*��Ч��*/, 
			  const char * StreamReq/*�����ļ����ڴ�*/,
			  const int ReqLen/*����ĳ���,0-��ʾ�ļ�*/,
			  const char * KUSAGE/*��Կ�÷�*/,
			  const char * EKUSAGE/*��ǿ��Կ�÷�*/,
			  const stuCERTEXT * pCertExt,/*֤����չ*/
			  char * CertMem/*�����Կ�ļ�*/,
			  UINT * CertLen/*�������*/,
			  char * outMsg/*�������*/,
			  const int type = DER/*�������DER,PEM*/);//ͨ��֤�����󣬵õ�֤��

//ֱ�����ɹ�˽Կ
BOOL DirectCert(const stuCertPair & RootPair/*��֤���*/,
				const int KeyBits/*[IN]*/,
				const long CertSerial/*[IN]���к�*/,
				const char * CertEndDate/*[IN]��������*/,
				const int CertDays/*[IN]��Ч��*/,
				const char * KeyEncPwd /*˽Կ��������,IN*/,
				const stuSUBJECT * sSUBJECT/*[IN]�û���Ϣ*/,
				const char * FriendlyName/*�üǵ�����*/,
				const char * KeyUsage/*��Կ�÷�*/,
				const char * EkeyUsage/*��չ��Կ�÷�*/,
				const stuCERTEXT * pCertExt,
				char * CertMem/*[OUT]���֤�鹫Կ*/,
				UINT * CertLen/*[OUT]����*/,
				char * KeyMem/*[OUT]���֤��˽Կ*/,
				UINT * KeyLen/*[OUT]����*/,
				char * P12Mem/*[OUT]���֤��˽Կ*/,
				UINT * P12Len/*[OUT]����*/,
				char * outMsg,
				const int type = DER/*�������DER,PEM*/);//ֱ�����ɹ�˽Կ

/*���������ɺ���*/
BOOL MakeCrl(const stuCertPair & RootPair/*��֤���*/,
			 const stuREVOKE * Head/*��������*/,
			 const PNewCrlMem NewCrlMem/*�ص�����*/,
			 char *& outCrl,
			 int * crll,
			 char * outfile/*crl�ļ�*/,
			 char * outMsg/*�������*/);

/*ת��֤�鵽REQ,�����֤�鹫˽Կ�޷�����,������REQ��������֤��*/
//BOOL X5092Req(const char * Cert/*��Կ*/,const int Certlen,const char * Key/*˽Կ,����ǩ����Կ*/,const int Keylen,
//			  const char * Keypwd/*����*/,
//			  const char * outFile/*����ļ�,req���߹�Կ֤��*/,char * outMsg,/*�������*/
//			  char * Rootcert,/*��֤�鹫Կ*/int Rootlen,/*Ϊ0��certfileΪ�����ļ�������Ϊ�ڴ�����*/
//			  char * Rootkey/*��֤��˽Կ*/,int RKeylen,char * Pwd/*˽Կ����*/);

/*�ֽ�p12��*/
BOOL ParsePfx(const char * StreamP12/*���ļ����ڴ�*/,
			  const UINT iP12Len/* �����Ϊ�ļ�,��Ϊ0,����Ϊ�ڴ泤��*/,
			  const char * P12DecPwd/*P12����*/,
			  const char * PathCert/*��Կ���*/,
			  const char * PathKey/*˽Կ���*/,
			  const char * KeyEncPwd/*˽Կ����*/,
			  const int OutFormat/*�����ʽ*/,
			  char * outMsg/*���ؽ��*/);

/*���p12��*/
BOOL CreatePfx(char * StreamP12/*OUT���ļ�·�����ڴ�����*/,
			   UINT & uP12Len,/*����ʱ��Ϊ���ڱ��������ڴ����򳤶�, ���Ϊʵ��P12�ڴ�ĳ���*/
			   const char * P12EncPwd/*IN ���ڼ���P12������*/,
			   const char * FriendName,/*IN �ü�����*/
			   const char * StreamCert/*IN��Կ*/,
			   const UINT uCertLen,/*IN,��Կ�ڴ����򳤶�, 0 - ��ʾ����Ϊ�����ļ�*/
			   const char * Streamkey/*IN˽Կ*/,
			   const UINT uKeyLen,/*IN,˽Կ�ڴ����򳤶�, 0 - ��ʾ����Ϊ�����ļ�*/
			   const char * KeyDecPwd/*����˽Կ����*/,
			   char * outMsg/*���ؽ��*/);

BOOL ChangePfxPwd(const char * strP12/*in�ϰ��ļ�*/,
			   const char * strPwd/*INԭ����*/,
			   const char * strPwd2/*IN������*/,
			   const char * strOutP12/*in�°��ļ�*/,
			   char * outMsg/*���ؽ��*/);

//���鹫Կ��˽Կ�Ƿ����
BOOL CertPairCheck(const char * cert,
				   const char * key,
				   char * outMsg,
				   const char * priPwd = NULL/*˽Կ����,IN*/);//���鹫Կ��˽Կ�Ƿ����


// ͨ����������֤֤�飬��֤ͨ�������棬���򷵻ؼ�
BOOL CheckCertWithCrl(const char *pubCert,const int pubCertLen,const char *crlData,const int crlLen,char * outMsg);

// ͨ����֤����֤֤��
BOOL CheckCertWithRoot(const char *pubCert,const int pubCertLen,const char *rootCert,const int rootCertLen,char * outMsg);

//���֤����Ч��,����Ч���ڷ����棬���򷵻ؼ�
BOOL CheckCertLife(const char *pubCert,const int pubCertLen,char * outMsg);

//��֤��Ԥ��
//BOOL MakeRootPub(stuSUBJECT * rootInfo,/*��Ϣ*/ char * certMem/*֤���ļ�*/,int * certLen,char * outMsg/*�������*/);

/*���P7��*/
BOOL CreateP7b(std::list<std::string> * pCertList,
			   const char * lpszCrl,
			   const char * lpszP7b,
			   const int outformat,
			   char * outMsg/*���ؽ��*/);


/*�ֽ�P7��*/
BOOL ParseP7b(const char * lpszInP7b/*���ļ����ڴ�*/,
			  const UINT iP12Len/* �����Ϊ�ļ�,��Ϊ0,����Ϊ�ڴ泤��,Ԥ��������*/,
			  const int outformat/*�����ʽ*/,
			  const char * lpszCert/*��Կ���*/,
			  const char * lpszCrl/*CRL���*/,
			  const char * lpszOutP7b/*����ת��P7��ʽ������ļ���*/,
			  char * outMsg/*���ؽ��*/);




