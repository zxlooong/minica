#include "CertKey.h"
#include "rc.h"//CRC
#include "CertInfoPage.h"

struct stuCertDB	
{
	UINT _uID;//����֤�����
	UINT _uLENGTH;//֤�鳤��
	char _chINFO[2022];//֤������
	char _chPWD[10];//֤����Կ
	BOOL _bUSED;//�Ƿ�ʹ��
	UINT _uCRC;//У����
	BOOL SetCRC()
	{
		return CRC::GetCrc16((char *)this,sizeof(stuCertDB) 
			- sizeof(UINT) ,_uCRC);
	}
	BOOL CheckCRC()
	{
		UINT CRC16;
		CRC::GetCrc16((char *)this,sizeof(stuCertDB) 
			- sizeof(UINT) ,CRC16);
		if(CRC16 == _uCRC)
			return TRUE;
		else
			return FALSE;
	}
	stuCertDB()
	{
		memset(this,0,sizeof(stuCertDB));
	}
	BOOL LoadFromDB(CString & str)
	{
		if(CheckCRC())
		{
			char msg[100] = {0};
			if(_uID%100 == 10)//˽Կ
			{
				EVP_PKEY * pKey = CCertKey::LoadKey(_chINFO,_uLENGTH,_chPWD,msg);
				if(!pKey)
				{
					str = "��Ч";
					EVP_PKEY_free(pKey);			
					return FALSE;
				}
				else
				{
					str = "��Ч";
					EVP_PKEY_free(pKey);			
					return TRUE;
				}

			}
			else
			{
				char subject[512]={0};
				char out[100]={0};
				CString strSubject;
				X509 *peer = CCertKey::LoadCert(_chINFO,_uLENGTH,_chPWD,out);
				if (peer != NULL)
				{
					//	Server certificate
					UINT uKeyLen = CCertInfoPage::GetKeyLen(peer);
					if(!CCertInfoPage::GetSubjectInfo(peer,subject,out))	//929
					{
						strSubject = "NoSubject";
					}
					else
					{
						str.Format("%s",subject);
						int state=str.Find("CN=",0);
						int state1=str.Find("\n",state);
						strSubject = str.Mid(state+3,state1-state-4);
					}
					str.Format("%s:%db", strSubject, uKeyLen);
					X509_free(peer);
					return TRUE;
				}
				else
				{
					str = "��Կ��Ч";
					return FALSE;
				}
				
			}
			
		}
		else
		{
			str = "CRC����";
			return FALSE;
		}
		return TRUE;
		
	}
	BOOL LoadFromCert(char * pCertFile,UINT uId,char * pPwd,BOOL bUsed,CString & strCause)//��֤���ļ����ص��ṹ
	{
		strcpy(_chPWD,pPwd);
		_bUSED = bUsed;
		CFile file;
		if(file.Open(pCertFile,CFile::modeRead))
		{
			int len = file.GetLength();
			if(len < 2022)
			{
				_uID = uId;
				_uLENGTH = len;
				file.Read(_chINFO,_uLENGTH);
				file.Close();
			}
			else
			{
				file.Close();
				strCause = " ���ȳ���ϵͳ����,�ָ�ԭ������";
				return FALSE;
			}
			
			char msg[100] = {0};
			if(uId%100!=10)//��˽Կ
			{
				if(!CCertKey::LoadCert(pCertFile,0,pPwd,msg))
				{
					strCause = " �޷�����,�ָ�ԭ������";
					return FALSE;
				}
			}
			else
			{
				if(!CCertKey::LoadKey(pCertFile,0,pPwd,msg))
				{
					strCause = " �޷�����,�ָ�ԭ������";
					return FALSE;
				}
			}
			return SetCRC();
		}
		else
		{
			strCause = " �޷���,�ָ�ԭ������";
			return FALSE;
		}
	}
};