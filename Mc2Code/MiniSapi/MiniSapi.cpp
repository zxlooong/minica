// MINISAPI.CPP - Implementation file for your Internet Server
//    MiniSapi Extension

#include "stdafx.h"
#include "MiniSapi.h"
#include "..\CA\ca.h"
#include <afxdisp.h>

///////////////////////////////////////////////////////////////////////
// command-parsing map

BEGIN_PARSE_MAP(CMiniSapiExtension, CHttpServer)
	// TODO: insert your ON_PARSE_COMMAND() and 
	// ON_PARSE_COMMAND_PARAMS() here to hook up your commands.
	// For example:
	ON_PARSE_COMMAND(CertReq, CMiniSapiExtension, ITS_PSTR ITS_PSTR ITS_PSTR ITS_PSTR ITS_I2 ITS_I2 ITS_I2)
	ON_PARSE_COMMAND_PARAMS("UserInfo KeyUsage EkeyUsage FriendName CertKeyLen CertValidity CertType")

	ON_PARSE_COMMAND(Query, CMiniSapiExtension, ITS_PSTR)
	ON_PARSE_COMMAND_PARAMS("QueryPwd")

	ON_PARSE_COMMAND(Default, CMiniSapiExtension, ITS_EMPTY)
	DEFAULT_PARSE_COMMAND(Default, CMiniSapiExtension)
END_PARSE_MAP(CMiniSapiExtension)


///////////////////////////////////////////////////////////////////////
// The one and only CMiniSapiExtension object

CMiniSapiExtension theExtension;


///////////////////////////////////////////////////////////////////////
// CMiniSapiExtension implementation

CMiniSapiExtension::CMiniSapiExtension()
{
	m_Title = "MiniCA ����֤�鷢��ϵͳ";
}

CMiniSapiExtension::~CMiniSapiExtension()
{
}

/*
ISA��Internet Server Application��Ҳ�ɳ�ΪISAPI DLL��
�书�ܺ�CGI����Ĺ���ֱ�����Ӧ��ʹ�÷�����CGIҲ���ƣ�
�ɿͻ�����URL��ָ�������ƶ����
������������󽫵��÷������������ִ��Ŀ¼Scripts�µ�function.dll
��ISAPI DLL������ڷ������������ִ��Ŀ¼�£��� 
http://www.abc.com/Scripts/function.dll?

ISA�ͷ�����֮��Ľӿ���Ҫ��������
GetExtentionVersion( )��HttpExtentionProc( )��
�κ�ISA����������PE�ļ�ͷ���������ж�������������������
�Թ�Web���������ʵ���ʱ����á� 

1�����������ռ���ISAʱ���������ISA�ṩ��GetExtentionVersion( )
����ø�ISA����Ҫ�ķ������汾�������Լ��İ汾��Ƚϣ��Ա�֤�汾����*/

BOOL CMiniSapiExtension::GetExtensionVersion(HSE_VERSION_INFO* pVer)
{
	// Call default implementation for initialization
/*
	typedef struct _HSE_VERSION_INFO 
	{ 
		DWORD dwExtensionVersion; //�汾�� 
		CHAR lpszExtensionDesc[HSE_MAX_EXT_DLL_NAME_LEN]; //����ISA�������ַ��� 
	} HSE_VERSION_INFO, *LPHSE_VERSION_INFO; 
*/
	CHttpServer::GetExtensionVersion(pVer);

	HMODULE hMiniCA = GetModuleHandle("MiniCA.exe");
	CString strMiniPath;
	if(hMiniCA)
	{
		GetModuleFileName(hMiniCA, strMiniPath.GetBuffer(MAX_PATH), MAX_PATH);
		strMiniPath.ReleaseBuffer();
		int nPos;
		nPos = strMiniPath.ReverseFind ('\\');
		CString strMdbPath = strMiniPath.Left (nPos);
		strMdbPath += "\\MiniCA.mdb";

		CString Msg;
		if(!ConnectDB(strMdbPath, Msg))//�������ݿ�ʧ��
		{
			//	return FALSE;�޷���ʾ��ҳHTTP 500 - �ڲ����������� 
		}
	}
	else	//MiniCA��ĵ���
	{
		return FALSE;
	}

	//ͨ����ִ���ļ�·���õ����ݿ�·��

	// Load description string
	TCHAR sz[HSE_MAX_EXT_DLL_NAME_LEN+1];
	ISAPIVERIFY(::LoadString(AfxGetResourceHandle(),
			IDS_SERVER, sz, HSE_MAX_EXT_DLL_NAME_LEN));
	_tcscpy(pVer->lpszExtensionDesc, sz);
	return TRUE;
}

BOOL CMiniSapiExtension::TerminateExtension(DWORD dwFlags)
{
	// extension is being terminated
	//TODO: Clean up any per-instance resources
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// CMiniSapiExtension command handlers

void CMiniSapiExtension::Default(CHttpServerContext* pCtxt)
{
	StartContent(pCtxt);
	SetTitle("Ĭ��ҳ��");
	WriteTitle(pCtxt);
	*pCtxt << _T("��ӭ��������֤������-CERTSELF\r\n<P>");
	*pCtxt << _T(" ��д������Ϣ��,ѡ����Ҫ��֤�����;Ϳ�����������Ҫ��֤����.\r\n");
	EndContent(pCtxt);
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMiniSapiExtension, CHttpServer)
	//{{AFX_MSG_MAP(CMiniSapiExtension)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0



///////////////////////////////////////////////////////////////////////
// If your extension will not use MFC, you'll need this code to make
// sure the extension objects can find the resource handle for the
// module.  If you convert your extension to not be dependent on MFC,
// remove the comments arounn the following AfxGetResourceHandle()
// and DllMain() functions, as well as the g_hInstance global.

/****

static HINSTANCE g_hInstance;

HINSTANCE AFXISAPI AfxGetResourceHandle()
{
	return g_hInstance;
}

BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ulReason,
					LPVOID lpReserved)
{
	if (ulReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInst;
	}

	return TRUE;
}

****/
void CMiniSapiExtension::Query(CHttpServerContext* pCtxt, LPCSTR sQueryPwd)
{
	StartContent(pCtxt);
	SetTitle("�û���ѯ");
	WriteTitle(pCtxt);

	if (!m_pDb.IsOpen())
	{
		*pCtxt<< "�������ݿ����,��������������";
		return;
	}
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		CString strSQL;
		strSQL.Format("Select CERTSTATE From WEBCERT Where QUERYPWD = '%s'", sQueryPwd);
		if(pRs->Open(strSQL, CADORecordset::openQuery))
		{
			if(!pRs->IsEof())//���ڼ�¼
			{
				int iState = 0;
				pRs->GetFieldValue("CERTSTATE", iState);
				switch(iState)
				{
				case 0:		//����δ������
					*pCtxt<< "�������뻹δ������,�����ĵȴ�";
					break;
				case 1:		//��������CSR
					*pCtxt<< "<p>����������֤������,���<a href=\"http://down\">֤������</a>����</p>";
					break;
				case 2:		//֤�����ڴ�����
					*pCtxt<< "�����������ڴ�����,���Ժ��ѯ";
					break;
				case 3:		//��������,��Կ,CSR,PFX
					*pCtxt<< "<p>����������<a href=\"http://6\">����</a>,<a href=\"http://1\">\
						֤��</a>,<a href=\"http://ŶŶ\">PFX��</a>,�������</p>";
					break;
				case 4:		//֤���Ѿ�����
					*pCtxt<< "�ܱ�Ǹ,����֤���Ѿ�����";
					break;
				default:
					break;
				}
			}
			else//��ѯ�������
			{
				*pCtxt<< "�����ڵĲ�ѯ����,��������֤��,Ȼ���ѯ.";
			}
			pRs->Close();
		}
	}
	catch(CADOException & eAdo)
	{
		CString str = eAdo.GetErrorMessage();
	}
	delete pRs;


	EndContent(pCtxt);


/*	int iRAWSize = 0;
	CByteArray caImage;
	CString cImage(sQueryPwd);
	cImage.Remove('\'');

	HRSRC tSrc = ::FindResource(AfxGetResourceHandle() ,cImage,"BINARY");
	if (tSrc == NULL) return;

	iRAWSize = ::SizeofResource(AfxGetResourceHandle(),tSrc);
	caImage.SetSize(iRAWSize);

	HGLOBAL hImage;
	hImage = ::LoadResource(AfxGetResourceHandle(),tSrc);
	if (hImage == NULL) return;

	BYTE* pImage = NULL;
	pImage = (BYTE*) ::LockResource(hImage);
	if (pImage == NULL) return;

	for (int iCount = 0; iCount < iRAWSize; iCount++)
		caImage.ElementAt(iCount) = pImage[iCount];

	*pCtxt << caImage;*/
}

BOOL CMiniSapiExtension::ConnectDB(CString strMdb, CString & Msg)
{
	if(m_pDb.IsOpen())
		return TRUE;

	CString strConnection;
	strConnection.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Persist Security Info=False;\
		Jet OLEDB:Database Password=hpxs", strMdb);

	try
	{
		m_pDb.Open(strConnection);
	}
	catch(CADOException & eAdo)
	{
		CString str = eAdo.GetErrorMessage();
		return FALSE;
	}
	return TRUE;
}
//USERINFO,KEYLEN,DAY,FRINEDNAME,CERTTYPE,KEYUSAGE,KEYEUSAGE
BOOL CMiniSapiExtension::InsertDB(const char * lpszUserInfo, const char * lpszKeyUsage,
								  const char * lpszKeyEusage, const char * lpszFriendName,
								  const int iKeyLen, const int iCertValidity,
								  const short iCertType, CString & strQueryPwd, CString & strMsg)
{
	//������ѯ����,��ѯ����Ϊһ�Ա�֤ CoCreateGuid ����ʱ��time_t+ID(���߿��Ա��û��µ�,����ѡ��ǰ��)
	if (!m_pDb.IsOpen())
	{
		strMsg = "�������ݿ����";
		return FALSE;
	}
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		if(pRs->Open("WEBCERT", CADORecordset::openTable))
		{
			pRs->AddNew();
			
			GUID  Guid;
			CoCreateGuid(&Guid);
			strQueryPwd.Format("%u%d%d%d",  Guid.Data1, Guid.Data2, Guid.Data3, Guid.Data4);

			CString strUserInfo(lpszUserInfo);
			CString strFriendName(lpszFriendName);
			CString strKeyUsage(lpszKeyUsage);
			CString strEkeyUsage(lpszKeyEusage);

			pRs->SetFieldValue("QUERYPWD", strQueryPwd);
			pRs->SetFieldValue("USERINFO", strUserInfo);
			pRs->SetFieldValue("KEYLEN", iKeyLen);
			pRs->SetFieldValue("DAY", iCertValidity);
			pRs->SetFieldValue("FRINEDNAME", strFriendName);
			pRs->SetFieldValue("CERTTYPE", iCertType);
			pRs->SetFieldValue("KEYUSAGE", strKeyUsage);
			pRs->SetFieldValue("KEYEUSAGE", strEkeyUsage);
			
			COleDateTime time = COleDateTime::GetCurrentTime();
			pRs->SetFieldValue("INPUTTIME", time);

			pRs->Update();
			pRs->Close();
			
		}
	}
	catch(CADOException & eAdo)
	{
		CString str = eAdo.GetErrorMessage();
	}
	delete pRs;
	return TRUE;
}

//����:�ṩ���û�����֤��Ĺ���
void CMiniSapiExtension::CertReq(CHttpServerContext* pCtxt, LPTSTR pszUserInfo, LPTSTR pszKeyUsage,
								 LPTSTR pszEkeyUsage, LPTSTR pszFriendName, short iCertKeyLen, short iCertValidity,
								 short iCertType)
{
	//���Ƚ����������Ч��

	//��������ת������ҳ��


	//����û���Ϣ�����ݿ���

	CString  strQueryPwd,
			 strMsg;

	BOOL bSucceed = InsertDB(pszUserInfo, pszKeyUsage, pszEkeyUsage, pszFriendName, iCertKeyLen, iCertValidity,
								  iCertType, strQueryPwd, strMsg);
	StartContent(pCtxt);
	WriteTitle(pCtxt);
	if(bSucceed)
	{
		*pCtxt << _T("��ϲ֤�������Ѿ��ɹ��ύ,���μ�����Ĳ�ѯ����,��������ȡ֤���Կ��Ӵ\r\n");
		*pCtxt << _T(strQueryPwd);
	}
	else
		*pCtxt << _T(strMsg);

	EndContent(pCtxt);
	return;

	{
	
	//	AddHeader(pCtxt, _T("Content-Type: application/OCTET-STREAM\r\n"));
	//	AddHeader(pCtxt, _T("Content-Disposition: attachment ; filename = 1.pfx\r\n"));
		
	//	CString strLength;
	//	strLength.Format("Content-length: %ld\r\n", p12l);
	//	AddHeader(pCtxt, strLength);

	//	CByteArray caCert;
	//	caCert.SetSize(p12l);
	//	
	//	for (int iCount = 0; iCount < p12l; iCount++)
	//		caCert.ElementAt(iCount) = p12[iCount];


//		* pCtxt << caCert;

//		pCtxt->WriteClient((BYTE *)p12, (LPDWORD)&p12l);
/*		HANDLE m_hPhysicsFile = CreateFile( "c:\\1.pfx", GENERIC_READ|GENERIC_WRITE, 0, 
                NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, NULL );
		if(!pCtxt->TransmitFile(m_hPhysicsFile, HSE_REQ_TRANSMIT_FILE))
		{
			DWORD dError = GetLastError();
			CString str;
			str.Format("%d", dError);
			* pCtxt << str;
		}*/
	}

//	EndContent(pCtxt);
	
}


/*
ISA�����������HttpExtentionProc( )��
���൱����ͨC�����main( )������
����������и��ݲ�ͬ�Ŀͻ���������ͬ�Ĵ���
��������HttpExtentionProc( )֮����ͨ����չ���ƿ�
��Extention Control Block��������ͨ�ŵģ�
��ECB�д����ڲ����ͳ��ڲ�����
�����������ṩ�ļ����ص���������ڵ�ַ*/


LPCTSTR CMiniSapiExtension::GetTitle() const
{
	// TODO: Add your specialized code here and/or call the base class
	return m_Title;
}
