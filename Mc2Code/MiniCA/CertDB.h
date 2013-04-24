#include "CertKey.h"
#include "rc.h"//CRC
#include "CertInfoPage.h"

struct stuCertDB	
{
	UINT _uID;//所属证书类别
	UINT _uLENGTH;//证书长度
	char _chINFO[2022];//证书内容
	char _chPWD[10];//证书密钥
	BOOL _bUSED;//是否使用
	UINT _uCRC;//校验码
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
			if(_uID%100 == 10)//私钥
			{
				EVP_PKEY * pKey = CCertKey::LoadKey(_chINFO,_uLENGTH,_chPWD,msg);
				if(!pKey)
				{
					str = "无效";
					EVP_PKEY_free(pKey);			
					return FALSE;
				}
				else
				{
					str = "有效";
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
					str = "公钥无效";
					return FALSE;
				}
				
			}
			
		}
		else
		{
			str = "CRC错误";
			return FALSE;
		}
		return TRUE;
		
	}
	BOOL LoadFromCert(char * pCertFile,UINT uId,char * pPwd,BOOL bUsed,CString & strCause)//从证书文件加载到结构
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
				strCause = " 长度超过系统限制,恢复原有配置";
				return FALSE;
			}
			
			char msg[100] = {0};
			if(uId%100!=10)//非私钥
			{
				if(!CCertKey::LoadCert(pCertFile,0,pPwd,msg))
				{
					strCause = " 无法加载,恢复原有配置";
					return FALSE;
				}
			}
			else
			{
				if(!CCertKey::LoadKey(pCertFile,0,pPwd,msg))
				{
					strCause = " 无法加载,恢复原有配置";
					return FALSE;
				}
			}
			return SetCRC();
		}
		else
		{
			strCause = " 无法打开,恢复原有配置";
			return FALSE;
		}
	}
};