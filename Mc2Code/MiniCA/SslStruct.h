/*���������ͻ����õ���һЩ�ṹ*/
#include <openssl/ssl.h>
#include <openssl/pkcs12.h>
#include <openssl/rand.h>
#include <openssl/ocsp.h>

#define DER			    1 //FORMAT_ASN1
#define PEM			    3	/*�����ʽ*/
#define NET				4
#define P12				5

struct stuISSU//���⡢������
{
	char sub[BUFSIZ];
	char iss[BUFSIZ];
	stuISSU()
	{
		memset(this,0,sizeof(stuISSU));
	}
};

struct stuSERVERINFO
{
	stuISSU peerChain;//�Է�֤����
	char selfChain[BUFSIZ];//�Լ�֤����
	stuISSU peerCert;//�Է�֤��
	char Cert[4*BUFSIZ];//�Է�֤��-pem
	char CiphersVer[64];//SSL�汾
	char CiphersName[64];//SSL�㷨
	int read;//��ȡ
	int written;//д��
	int CertBit;//��Կλ��
	UCHAR Session[4*BUFSIZ];//�Ự
	stuSERVERINFO()
	{
		memset(this,0,sizeof(stuSERVERINFO));
	}

};

struct stuCLIENTINFO
{
	UCHAR Session[4*BUFSIZ];
	char Cert[4*BUFSIZ];//�Է�֤��-pem
	stuISSU peerCert;//�Է�֤��
	char Shared_ciphers[4*BUFSIZ];
	char Current_cipher[64];
	stuCLIENTINFO()
	{
		memset(this,0,sizeof(stuCLIENTINFO));
	}
};

struct stuSENDINFO	//���ڷ��͵Ľṹ
{
	BYTE Info[1024];//����
	BOOL bType;//��������,�ļ�,��Ϣ
	DWORD dLen;//����
	char cName[256];//�ļ���
	stuSENDINFO()
	{
		memset(this,0,sizeof(stuSENDINFO));
	}
};
