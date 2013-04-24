// CaCertWizardSheet.cpp : implementation file
//

#include "stdafx.h"
#define _WIN32_WINNT  0x0400
#include "wincrypt.h"


#include "csp11/CBase64.h"
#include "csp11/Certificate.h"
#include "csp11/CertKitCsp.h"
#include "csp11/MemTools.h"
using namespace MemTools;
//#include "csp11/MemToolsX.h"
#include "csp11/CPEleven.h"


#ifdef X509_NAME
#	undef X509_NAME
#endif
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
//#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <openssl/rand.h>


#include "minica.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "minict.h"
#include "Ossl.h"
//using namespace Ossl;

#include "CaCertWizardSheet.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCertWizardSheet property page

IMPLEMENT_DYNCREATE(CCaCertWizardSheet, CPropertyPage)

CCaCertWizardSheet::CCaCertWizardSheet() : CPropertyPage(CCaCertWizardSheet::IDD)
{
	//{{AFX_DATA_INIT(CCaCertWizardSheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_HinstLib = LoadLibrary(TEXT("Cryptui.dll"));
}

CCaCertWizardSheet::~CCaCertWizardSheet()
{
    FreeLibrary(m_HinstLib); 
	m_ImgList.DeleteImageList();
}

void CCaCertWizardSheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCertWizardSheet)
	DDX_Control(pDX, IDC_B_V, m_BV);
	DDX_Control(pDX, IDC_B_NEXT, m_BNext);
	DDX_Control(pDX, IDC_B_MADE, m_BMade);
	DDX_Control(pDX, IDC_B_LAST, m_BLast);
	DDX_Control(pDX, IDC_TAB_SHEET, m_CaWizardSheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaCertWizardSheet, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCertWizardSheet)
	ON_BN_CLICKED(IDC_B_V, OnBV)
	ON_BN_CLICKED(IDC_B_MADE, OnBMade)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SHEET, OnSelchangeTabSheet)
	ON_BN_CLICKED(IDC_B_LAST, OnBLast)
	ON_BN_CLICKED(IDC_B_NEXT, OnBNext)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCertWizardSheet message handlers
BOOL CCaCertWizardSheet::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
    UINT nID = pTTTStruct->idFrom;
	CString strText;
	switch(nID)
	{
	case IDC_B_MADE:
		CCaCertInfoPage::stuCERTINFO CERTINFO;
		stuSUBJECT * pCERT = NULL;
		m_PageInfo.GetCert(pCERT,CERTINFO);
		pCERT->RemoveAll(pCERT); //�ͷ�֤��ṹ
		CString strFormat;
		strFormat = (CERTINFO.uCertFormat == 1) ? "DER" : "PEM";
		strText.Format("��������֤��\r��Կ����:%d\n ��Ч��:%d\n ֤���ʽ%s", 
			CERTINFO.uCertLen, CERTINFO.uCertDay, strFormat);
		_tcscpy(pTTT->szText, strText);//����
		return TRUE;
	}
	return FALSE;
}

BOOL CCaCertWizardSheet::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here

	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());

	m_BLast.SetIcon(IDI_ICON_LAST);//��һ��
	m_BLast.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_BNext.SetIcon(IDI_ICON_NEXT);//��һ��
	m_BNext.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_BV.SetIcon(IDI_ICON13);//Ԥ��
	m_BV.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_BMade.SetIcon(IDI_ICON12);//����
	m_BMade.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);



	// TODO: Add extra initialization here
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
		m_toolTip.AddTool(GetDlgItem(IDC_B_LAST), CMiniCaApp::NormalCode("������һ������"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_NEXT), CMiniCaApp::NormalCode("������һ������"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_V), CMiniCaApp::NormalCode("Ԥ������֤��\r��Կ����:384\n ��Ч��:1��\n ���100"));
		m_toolTip.AddTool(GetDlgItem(IDC_B_MADE), LPSTR_TEXTCALLBACK);

		// TODO: Use one of the following forms to add controls:
	}
	// Tree initialization


	m_ImgList.Create(16,16,TRUE|ILC_COLOR24,16,1);
	
	HICON hIcon =  NULL;
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTYPE), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//0
	DestroyIcon(hIcon);
	
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTINFO), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//1
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTEXT), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//2
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTREPORT), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//3
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTSETUP), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//4
	DestroyIcon(hIcon);
	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTMAN), IMAGE_ICON, 16, 16, 0);
	m_ImgList.Add(hIcon);//5
	DestroyIcon(hIcon);
	
	m_CaWizardSheet.SetImageList(&m_ImgList);
	m_CaWizardSheet.AddPage(MiniCT_0200, 0, &m_PageType, IDD_PROPPAGE_CATYPE);	//MiniCT_0200 "����"
	m_CaWizardSheet.AddPage(MiniCT_0201, 1, &m_PageInfo, IDD_PROPPAGE_CAINFO);	//MiniCT_0201 "��Ϣ"
	m_CaWizardSheet.AddPage(MiniCT_0202, 2, &m_PageExt, IDD_PROPPAGE_CAEXT);		//MiniCT_0202 "��չ"
	m_CaWizardSheet.AddPage(MiniCT_0203, 3, &m_PageReport, IDD_PROPPAGE_CAREPORT);//MiniCT_0203 "����"
	m_CaWizardSheet.AddPage(MiniCT_0204, 4, &m_PageIniSet, IDD_PROPPAGE_CAINI);	//MiniCT_0204 "����"
	m_CaWizardSheet.AddPage(MiniCT_0205, 5, &m_PageMan, IDD_PROPPAGE_CAMAN);		//MiniCT_0205 "����"

	m_CaWizardSheet.Show();
	m_PageReport.GetWizard(&m_PageType, &m_PageInfo, &m_PageExt);

	//����û�б���,ԭ������CCertDB�л���б���,���ҶԴ洢·����ֵ
	//���ﱨ�������Ϊ·��δ��ֵ�����ֱ�����ʾ��������
	//m_PageReport.ViewWizardInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCaCertWizardSheet::OnBV() //���CԤ��,ֻ����DER��ʽ��֤��
{
	// TODO: Add your control notification handler code here
	int type = -1;
	char out[256] = {0};
	char certBuf[10240] = {0};
	char keyBuf[10240] = {0};
	char p12Buf[10240] = {0};

	UINT certLen = 10240,
		 keyLen = 10240,
		 p12Len = 10240;
	
	DWORD lenCert=0,lenKey=0;//��Կ����,˽Կ����
	CString strCert,strKey,strPwd;//��˽Կ·��������,p12��Կ

	CString KUSAGE,EKUSAGE;
	m_PageType.GetCert(KUSAGE,EKUSAGE,type);

	stuSUBJECT * pCERT = NULL;
	CCaCertInfoPage::stuCERTINFO CERTINFO;
	m_PageInfo.GetCert(pCERT,CERTINFO);

	stuCERTEXT * pCertExt = NULL;
	m_PageExt.GetCert(pCertExt);

	AddMsg(MiniCT_0206, M_WARING);		//MiniCT_0206 "������֤Ԥ��,֤����Կ����384,֤�����100,��Ч��1........"

	if(type == 0)//��֤��
	{
		if(MakeRoot(pCERT,"(MiniCA)",384,100,0,
		"",NULL,NULL,pCertExt,certBuf,
		&certLen,keyBuf,&keyLen,p12Buf,
		&p12Len,out, DER))
		{
			SelectViewCert(certBuf,certLen);
		}
		else
			AddMsg(out,M_ERROR);
	}
	else
	{
		char strCert[10240] = {0};
		char strKey[10240] = {0};

		if(((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd))
		{		
			stuCertPair RootPair(strCert,lenCert,strKey,lenKey,strPwd.GetBuffer(0));
			
			strPwd.ReleaseBuffer();
			
			if(DirectCert(RootPair,384,100,0,1,"",pCERT,"(MiniCA)",KUSAGE.GetBuffer(0),
				EKUSAGE.GetBuffer(0),pCertExt,
				certBuf,&certLen,keyBuf,&keyLen,p12Buf,&p12Len,out, DER))
			{
				SelectViewCert(certBuf,certLen);
			}
			else
				AddMsg(out,M_ERROR);
			
			KUSAGE.ReleaseBuffer();
			EKUSAGE.ReleaseBuffer();
		}
	}
	pCERT->RemoveAll(pCERT);	//ɾ��֤��ṹ
	pCertExt->RemoveAll(pCertExt);	//ɾ����չ�ṹ
	
}

void CCaCertWizardSheet::OnBMade() 
{
	// TODO: Add your control notification handler code here
	int type = -1;

	CString KUSAGE,EKUSAGE;
	m_PageType.GetCert(KUSAGE,EKUSAGE,type);

	stuSUBJECT *pCERTSub =	NULL;
	CCaCertInfoPage::stuCERTINFO CERTINFO;
	m_PageInfo.GetCert(pCERTSub, CERTINFO);

	stuCERTEXT * pCertExt = NULL;
	m_PageExt.GetCert(pCertExt);


	// uCertGenType  :   0 evp; 1 evp->csp; 2 csp; 3 p11;
	if(2 > CERTINFO.uCertGenType){
		switch(type)
		{
		case 0://��֤��
			MakeRootCert(pCERTSub, CERTINFO);
			break;
		default:
			MakeCert(pCERTSub, CERTINFO, type, KUSAGE, EKUSAGE, pCertExt);
			break;
		}
	}

	if (2 == CERTINFO.uCertGenType)	 //csp
	{
		switch(type)
		{
		case 0://��֤��
			MakeRootCert(pCERTSub, CERTINFO);
			break;
		default:
			MakeCertCsp(pCERTSub, CERTINFO, type, KUSAGE, EKUSAGE, pCertExt);
			break;
		}		
	}

	if (3 == CERTINFO.uCertGenType)  //p11
	{
		switch(type)
		{
		case 0://��֤��
			MakeRootCert(pCERTSub,CERTINFO);
			break;
		default:
			MakeCertP11(pCERTSub,CERTINFO,type,KUSAGE,EKUSAGE, pCertExt);
			break;
		}		
	}



	pCERTSub->RemoveAll(pCERTSub); //�ͷ�֤��ṹ
	pCertExt->RemoveAll(pCertExt); //�ͷ�֤����չ�ṹ	
}

BOOL CCaCertWizardSheet::ViewCert(char * cert,UINT uCertLen)
{
	//���Ȼ�ú�����ַ GetProcAddress
	//BOOL WINAPI CryptUIDlgViewContext(
	//DWORD dwContextType,
	//const void* pvContext,
	//HWND hwnd,
	//LPCWSTR pwszTitle,
	//DWORD dwFlags,
	//void* pvReserved
	//);

	typedef BOOL (WINAPI *ViewContext) (DWORD , const void * ,HWND , LPCWSTR , DWORD , void* ); //ע��˵ص�WINAPI
	ViewContext CryptUIDlgViewContext; 
	if(m_HinstLib)
	{
		CryptUIDlgViewContext = (ViewContext) GetProcAddress(m_HinstLib, TEXT("CryptUIDlgViewContext")); 
        // If the function address is valid, call the function.
        if (NULL != CryptUIDlgViewContext) 
        {
			PCCERT_CONTEXT	pCertContext = NULL;				//֤�������� �ſ��� commer  (����) ̨����
			pCertContext = CertCreateCertificateContext(
				X509_ASN_ENCODING,	// The encoding type.
				(UCHAR *)cert,			// The encoded data from
				uCertLen// the certificate retrieved.
				);		
			if (pCertContext == NULL)
			{
				return FALSE;
			}

			
			BSTR bstrTitle = MiniCT_0207.AllocSysString();

			(CryptUIDlgViewContext)(CERT_STORE_CERTIFICATE_CONTEXT,   // Display a certificate.
					 pCertContext,                     // Pointer to the certificate
					 m_hWnd,
					 bstrTitle,			//MiniCT_0207 "MiniCA ֤��Ԥ��"
					 0,
					 NULL);

			::SysFreeString(bstrTitle);

			if (pCertContext != NULL)
				CertFreeCertificateContext(pCertContext);
        }
		else 
		{
			((CMiniMainDlg *)GetParent())->ViewCertInfo(cert,uCertLen);
			AddMsg(MiniCT_0208, M_WARING);		//MiniCT_0208 "�˲���ϵͳ��֧���ⲿ��֤Ԥ��,��ת���ڲ�Ԥ��"
		}
	}
	return TRUE;
}

void CCaCertWizardSheet::SelectViewCert(char * pCert,UINT uLenCert)
{
	if(m_HinstLib)
	{
		CRect Rect;
		GetDlgItem(IDC_B_V)->GetWindowRect(&Rect);
//		CPoint point;
//		GetCursorPos(&point); // ��ǰ�������
		BCMenu menu;
		menu.LoadMenu(IDR_MENU_VIEWCERT);
		menu.LoadToolbar(IDR_MINICAMENU);

		CLanguage::TranslateMenu(&menu, MAKEINTRESOURCE(IDR_MENU_VIEWCERT));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup);
		UINT nSelection = pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|
			TPM_NONOTIFY|TPM_RETURNCMD,Rect.left + Rect.Width()/2, Rect.top + Rect.Height()/2,
			this, NULL);
		menu.DestroyMenu();
		//���ز˵�id
		if(nSelection == ID_MENUITEM_INSIDE)//MiniCAԤ��
		{
			AddMsg(MiniCT_0207);										//MiniCT_0207 "MiniCA��֤Ԥ��"
			((CMiniMainDlg *)GetParent())->ViewCertInfo(pCert,uLenCert,0);
		}
		else if(nSelection == ID_MENUITEM_OUTSIDE)//ϵͳԤ��
		{
			AddMsg(MiniCT_0209);											//MiniCT_0209 "�ⲿ��֤Ԥ��"
			ViewCert(pCert,uLenCert);
		}
	}
	else
	{
		AddMsg(MiniCT_0207);											//MiniCT_0207 "MiniCA��֤Ԥ��"
		((CMiniMainDlg *)GetParent())->ViewCertInfo(pCert,uLenCert,0);
	}	
}

void CCaCertWizardSheet::MakeRootCert(stuSUBJECT * pSUBJECT, CCaCertInfoPage::stuCERTINFO & CERTINFO)
{
	CString m_Path(CERTINFO.cCertDir), 
			m_ExtName(".cer");
	char out[256] = {0};
	char certBuf[10240] = {0};
	char keyBuf[10240] = {0};
	char p12Buf[10240] = {0};

	UINT certLen = 10240,
		 keyLen = 10240,
		 p12Len = 10240;

	//�õ�֤�����
	DWORD dCertSn = m_PageMan.GetCertSn();

	if(MakeRoot(pSUBJECT, 
				"(MiniCA)",
				CERTINFO.uCertLen, dCertSn, 
				CERTINFO.uCertDay, 
				CERTINFO.cCertPwd, 
				NULL, NULL, NULL, 
				certBuf, &certLen, 
				keyBuf, &keyLen, 
				p12Buf, &p12Len, 
				out, 
				CERTINFO.uCertFormat))
	{
		UINT m_NameType = 0;
		CMiniMainDlg *pMain = (CMiniMainDlg *)AfxGetMainWnd();
		CCertDbPage *pDb = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));
		if(pDb) {
			m_NameType = pDb->GetNameType();
		}

		CString strName;
		if(0 == m_NameType){  //�û���
			strName.Format("\\R%s", pSUBJECT->GetCN());
		}else if(1 == m_NameType){  //���
			strName.Format("\\R%d", dCertSn);
		}else if(2 == m_NameType){  //�û��� + ���
			strName.Format("\\R%s%d", pSUBJECT->GetCN(), dCertSn);
		}else if(3 == m_NameType){  //��� + �û���
			strName.Format("\\R%d%s", dCertSn, pSUBJECT->GetCN());
		}

		//save file
		CString outCert = m_Path + strName + "Cert";
		outCert += m_ExtName;
		CString outKey = m_Path + strName + "Key";
		outKey += m_ExtName;
		CString p12 = m_Path + strName + ".pfx";
		
		FILE * pfc = fopen(outCert, "wb");
		if(pfc != NULL)
		{
			fwrite(certBuf,sizeof(char),certLen,pfc);
			fclose(pfc);
			pfc = fopen(outKey,"wb");
			if(pfc)
			{
				fwrite(keyBuf,sizeof(char),keyLen,pfc);
				fclose(pfc);
				AddMsg(MiniCT_0117);				//MiniCT_0117
			}
			else
			{
				AddMsg(MiniCT_0116,M_ERROR);		//MiniCT_0116
				return;
			}

			if(CERTINFO.bCertP12)
			{
				pfc = fopen(p12,"wb");
				if(pfc != 0)
				{
					fwrite(p12Buf,sizeof(char),p12Len,pfc);
					fclose(pfc);
					AddMsg(MiniCT_0119);		//MiniCT_0119
				}
				else
				{
					AddMsg(MiniCT_0118);		//MiniCT_0118
					return;
				}
			}
		}
		else
		{
			AddMsg(MiniCT_0115,M_ERROR);		//MiniCT_0115
			return;
		}
		
		//֤����Ϣ��������ݿ�
		m_PageMan.SaveDb(pSUBJECT, CERTINFO.uCertLen, 0, 
			CERTINFO.uCertDay, p12Buf, p12Len, CERTINFO.cCertPwd);
	}
}


void CCaCertWizardSheet::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)GetParent())->AddMsg(MiniCT_0000,info, type);		//MiniCT_0000 "��֤��"
}

void  CCaCertWizardSheet::ShowError()
{
	char Buffer[200];
	memset(Buffer,0,200);
	FormatMessage(                                                                                                                                                                                                                                                                                                                                                                                                     
		FORMAT_MESSAGE_FROM_SYSTEM,     // source and processing options
		NULL,							// pointer to  message source
		GetLastError(),							// requested message identifier
		0,								// language identifier for requested message
		Buffer,							// pointer to message buffer
		200,							// maximum size of message buffer
		NULL							// pointer to array of message inserts
		);
	AddMsg(Buffer,M_ERROR);
}

BOOL CCaCertWizardSheet::ImportCSP(char * memP12,UINT lenP12,CString szCSPName,CString szKeycon)
{
	// TODO: Add your control notification handler code here
	long	ret = 0;
	
	HCRYPTPROV		hCryptProv = NULL;					//ָ��CSP���
	BYTE 			*szContainerName = NULL;			//CONTAINER����
	HCRYPTKEY		hCryptKey = NULL ;		//��Կ���
	PCCERT_CONTEXT	pCertContext = NULL;				//֤��������
	DWORD			dwKeySpec;
	
	CRYPT_DATA_BLOB pPFX;
	HCERTSTORE		hStore;
	HCRYPTKEY		hUser;
	BYTE			bData[10240];
	DWORD			cbData = 10240;
	
	pPFX.cbData = lenP12;
	pPFX.pbData = (UCHAR *)memP12;
	
	//	//new ��Ϊ��̬����

	//��PFX BLOB���뵽ָ��֤���
	hStore = PFXImportCertStore(&pPFX,
								L"",  //Password
								CRYPT_EXPORTABLE); //�õ�֤�����
	
	if (hStore == NULL)
	{
		ShowError();
		return FALSE;
	}
	
	// search any certificate ͨ��֤���õ�֤�������� ---  Ҳ���ǲ쿴������ļ����Ƿ���֤����Ϣ
	if (!(pCertContext=CertFindCertificateInStore(
								hStore,
								X509_ASN_ENCODING,
								0,
								CERT_FIND_ANY,
								NULL,
								NULL))) 
	{
		ShowError();
		return FALSE;
	}
	
	//new ��Ϊ��̬����
	// ����ָ��֤�������ĵõ�һ��HCRYPTPROV����� dwKeySpec 
	ret = CryptAcquireCertificatePrivateKey(//ͨ��֤�������ĵõ�CSP���
								pCertContext,
								CRYPT_ACQUIRE_CACHE_FLAG,
								NULL,
								&hCryptProv,//CSP���
								&dwKeySpec,//���ܻ���ǩ�� AT_KEYEXCHANGE 1 AT_SIGNATURE 2 
								NULL
								);
//	CString strType;
//	strType.Format("��Կ����%d",dwKeySpec);
//	AfxMessageBox(strType);
	
	if (!ret)
	{
		ShowError();
		return FALSE;
	}
	
	ret = CryptGetUserKey(//�õ���Կ�Ծ��
								hCryptProv,
								dwKeySpec,
								&hUser
								);
	
	if (!ret)
	{
		ShowError();
		return FALSE;
	}
	
	ret = CryptExportKey( //��CSP�е���Կ����
								hUser,
								0,
								PUBLICKEYBLOB,
								0,
								bData,
								&cbData
								);  //PUBLICKEYBLOB -148         PRIVATEKEYBLOB - 596
	
	if (!ret)
	{
		ShowError();
		return FALSE;
	}
	
	CryptDestroyKey(hUser);
	
	CryptReleaseContext(hCryptProv, 0);
	
	
	hCryptProv = 0;
	
	if (RCRYPT_FAILED(CryptAcquireContext(&hCryptProv, szKeycon,
		szCSPName, PROV_RSA_FULL, CRYPT_NEWKEYSET)))
	{
		long err = GetLastError();
		if(0x8009000f == err)//�����Ѿ����� 
		{
			if(::MessageBox(this->m_hWnd,MiniCT_0211, MiniCT_0212,MB_ICONQUESTION | MB_YESNO)==IDYES)
				//MiniCT_0211 "֤������Ѿ�����,�Ƿ񸲸�"
				//MiniCT_0212 ,"CSP��ʾ"
			{
				if (CryptAcquireContext(&hCryptProv, szKeycon,
					szCSPName, PROV_RSA_FULL, CRYPT_DELETEKEYSET))//ɾ������
				{
					if (RCRYPT_FAILED(CryptAcquireContext(&hCryptProv, szKeycon,
					szCSPName, PROV_RSA_FULL, CRYPT_NEWKEYSET)))//���´���
					{
						ShowError();
						return FALSE;
					}
				}
			}
			else
			{
				return FALSE;
			}

		}
		else
		{
			ShowError();
			return FALSE;
		}

	}
	
	if (RCRYPT_FAILED(CryptImportKey(
		hCryptProv,
		bData,
		cbData,
		0,
		0,
		&hUser)))
	{
		
		ShowError();
		return FALSE;
	}
	
	CryptDestroyKey(hUser);
	
/*	if (RCRYPT_FAILED(CryptGetUserKey(hCryptProv,
		dwKeySpec,
		&hUser)))
	{
		if (GetLastError() == ERROR_INVALID_PARAMETER)
		{
		}
		else
		{
		}
	}
	
	if (RCRYPT_FAILED(CryptSetKeyParam(
		hUser,
		KP_CERTIFICATE,
		pCertContext->pbCertEncoded,
		0)))
	{
		ShowError();
		return FALSE;
	}
	
	CryptDestroyKey(hUser);
*/
	if (RCRYPT_FAILED(CryptReleaseContext(hCryptProv, 0)))
	{
		ShowError();
		return FALSE;
	}
	
	CertFreeCertificateContext(pCertContext);
	
	CertCloseStore(hStore,0);
	AddMsg(MiniCT_0213);		//MiniCT_0213 "֤�鵼��CSP�ɹ�"
	return TRUE;
}

long CCaCertWizardSheet::GetCspCertInfo(LPTSTR szCSPName, LPTSTR ContainerName, DWORD KeySpec)
{
	long	ret = 0;
	HCRYPTPROV		hCryptProv = NULL;					//ָ��CSP���
	BYTE 			*szContainerName = NULL;			//CONTAINER����
///	DWORD			cbContainerName, maxcbContainerName;//CONTAINER����
	BYTE			CertBuff[4096];						//֤����Ϣ
	HCRYPTKEY		hCryptKey = NULL;					//��Կ���
	PCCERT_CONTEXT	pCertContext = NULL;				//֤��������
//	DWORD			dwKeySpec;
//	DWORD			dwFlags;
	DWORD			dwCertLength;
//	char			PIN[10];

	__try
	{
		//ȡ��ָ��CSP�ķ��ʾ��
		ret = CryptAcquireContext(&hCryptProv,
			ContainerName,
			szCSPName,
			PROV_RSA_FULL,
			0);
		if (ret == 0)
		{
			 ShowError();
			__leave;
		}
		
		//ȡ��CONTAINER�������б�����󳤶�
/*		ret	= CryptGetProvParam(hCryptProv,
			PP_ENUMCONTAINERS,
			NULL,				//ȡ���������Ƴ���
			&cbContainerName, 
			0);
		if (ret == 0)
		{
			ShowError();
			__leave;
		}
		
		maxcbContainerName = cbContainerName;
		szContainerName = (BYTE*)malloc(cbContainerName);
		
		// ��ȡ��һ��CONTAINER����
		dwFlags = CRYPT_FIRST;
		cbContainerName = maxcbContainerName;
		ret = CryptGetProvParam(hCryptProv,
			PP_ENUMCONTAINERS,
			szContainerName,
			&cbContainerName,
			dwFlags);
		if (ret == 0)
		{
			ShowError();
			__leave;
		}
		
		memset(PIN, 0, 10);
		PIN[0]='0';
		PIN[1]='0';
		PIN[2]='0';
		PIN[3]='0';
		//����PINֵ
		ret = CryptSetProvParam(hCryptProv,
			PP_KEYEXCHANGE_PIN,
			(unsigned char *)PIN,
			0);
		if (ret == 0)
		{
			ShowError();
			__leave;
		}
	*/	
		//��ȡCONTAINER�е�KEY
		ret = CryptGetUserKey(hCryptProv,
			AT_KEYEXCHANGE, 
			&hCryptKey);
		if (ret == 0)
		{
			ShowError();
			__leave;
		}
		
		//ȡ��ָ����Կ��ص�֤��
		dwCertLength = sizeof(CertBuff);
		ret = CryptGetKeyParam(hCryptKey,
			KP_CERTIFICATE,
			CertBuff,
			&dwCertLength,
			0);
		if (ret == 0)
		{
			ShowError();
			__leave;
		}

		char temp[100] = {0};
		pCertContext = CertCreateCertificateContext(
										X509_ASN_ENCODING,	// The encoding type.
										CertBuff,				// The encoded data from
															// the certificate retrieved.
										dwCertLength		);
		if (pCertContext == NULL) 
		{//Not X.509 Certification
			__leave;
		}

		//new ��Ϊ��̬����
/*		CertGetNameString(   
			   pCertContext,   
			   CERT_NAME_SIMPLE_DISPLAY_TYPE,   
			   0,
			   NULL,   
			   (char *)temp,   
			   128);

		CertGetNameString(   
			   pCertContext,   
			   CERT_NAME_EMAIL_TYPE,   
			   0,
			   NULL,   
			   (char *)temp,   
			   128);

		CertGetNameString(   
			   pCertContext,   
			   CERT_NAME_SIMPLE_DISPLAY_TYPE,   
			   CERT_NAME_ISSUER_FLAG,
			   NULL,   
			   (char *)temp,   
			   128);*/

/*		CryptUIDlgViewContext(
		CERT_STORE_CERTIFICATE_CONTEXT,   // Display a certificate.
		pCertContext,                     // Pointer to the certificate
		m_hWnd,
		L"MiniCA ֤��Ԥ��",
		0,
		NULL);*/


		__leave;
	
	}
	__finally
	{
		if(pCertContext)
			CertFreeCertificateContext(pCertContext);
		if (hCryptKey)
			CryptDestroyKey(hCryptKey);
		if (hCryptProv)
			CryptReleaseContext(hCryptProv,0);
		if (szContainerName)
			free(szContainerName);
		return ret;
	}
}

void CCaCertWizardSheet::OnSelchangeTabSheet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	int iSelect = m_CaWizardSheet.GetCurSel();
	if(iSelect > 3)//����
	{
		GetDlgItem(IDC_B_LAST)->EnableWindow(FALSE);	
		GetDlgItem(IDC_B_NEXT)->EnableWindow(FALSE);	
	}
	else
	{
		GetDlgItem(IDC_B_LAST)->EnableWindow(TRUE);	
		GetDlgItem(IDC_B_NEXT)->EnableWindow(TRUE);	
	}

	m_PageReport.ViewWizardInfo();
	*pResult = 0;
}

void CCaCertWizardSheet::OnBLast() 
{
	// TODO: Add your control notification handler code here
	int iSelect = m_CaWizardSheet.GetCurSel();
	if(iSelect >= 4)
	{
		return;
	}
	m_CaWizardSheet.SetCurSel(iSelect - 1);
	m_PageReport.ViewWizardInfo();
}

void CCaCertWizardSheet::OnBNext() 
{
	// TODO: Add your control notification handler code here
	int iSelect = m_CaWizardSheet.GetCurSel();
	if(iSelect >= 3)
		return;
	m_CaWizardSheet.SetCurSel(iSelect + 1);
	m_PageReport.ViewWizardInfo();
}

//ͨ�������������ƶ�����ҳ�Ķ���ָ��
CWnd * CCaCertWizardSheet::GetPage(CString strPageName)
{
	//ö������ҳ
	CRuntimeClass * prt = NULL;

	prt = m_PageType.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageType;

	prt = m_PageInfo.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageInfo;

	prt = m_PageExt.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageExt;


	prt = m_PageReport.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageReport;

	prt = m_PageIniSet.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageIniSet;

	prt = m_PageMan.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageMan;

	return NULL;
}

void CCaCertWizardSheet::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	// TODO: Add your message handler code here
	m_PageType.DestroyWindow();
	m_PageInfo.DestroyWindow();
	m_PageExt.DestroyWindow();
	m_PageMan.DestroyWindow();
	m_PageReport.DestroyWindow();
	m_PageIniSet.DestroyWindow();
}

void CCaCertWizardSheet::Language()
{
	CLanguage::TranslateDialog(m_PageType.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CATYPE));
	m_PageType.TranslateCT();

	CLanguage::TranslateDialog(m_PageInfo.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CAINFO));
	m_PageInfo.TranslateCT();

	CLanguage::TranslateDialog(m_PageExt.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CAEXT));
	m_PageExt.TranslateCT();

	CLanguage::TranslateDialog(m_PageReport.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CAREPORT));
	m_PageReport.ViewWizardInfo();

	CLanguage::TranslateDialog(m_PageMan.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CAMAN));
	m_PageMan.TranslateCT();

	CLanguage::TranslateDialog(m_PageIniSet.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CAINI));
	m_PageIniSet.TranslateCT();

	CLanguage::TranslateDialog(this->m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CASHEET));

	m_CaWizardSheet.SetPageName(0, MiniCT_0200);
	m_CaWizardSheet.SetPageName(1, MiniCT_0201);
	m_CaWizardSheet.SetPageName(2, MiniCT_0202);
	m_CaWizardSheet.SetPageName(3, MiniCT_0203);
	m_CaWizardSheet.SetPageName(4, MiniCT_0204);
	m_CaWizardSheet.SetPageName(5, MiniCT_0205);

	//����STATIC
	SetDlgItemText(IDC_B_NEXT,	MiniCT_10701);
	SetDlgItemText(IDC_B_LAST,	MiniCT_10702);
	SetDlgItemText(IDC_B_MADE,	MiniCT_10703);
	SetDlgItemText(IDC_B_V,		MiniCT_10704);
}
BOOL CCaCertWizardSheet::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

////////////////////////////////////////////////////////////////////////////////////////
void CCaCertWizardSheet::MakeCert(stuSUBJECT * pCERT,
								 CCaCertInfoPage::stuCERTINFO & CERTINFO,
								 const int type,
								 CString KUSAGE,  CString EKUSAGE,
								 const stuCERTEXT * pCertExt)
{
	DWORD lenCert=0,lenKey=0;  //��Կ����,˽Կ����
	CString strPwd;  //��˽Կ·��������,p12��Կ
	char cert[10240]={0}, key[10240]={0}, p12[10240]={0};
	UINT certl=10240,keyl=10240,p12l=10240;
	char out[256] = {0};
	int save = 0;
	CString m_Path(CERTINFO.cCertDir);
	CString pwd,path;
	char strCert[10240] = {0};
	char strKey[10240] = {0};

	if(((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd))
	{		
		if(strlen(CERTINFO.cCertDir) == 0)//CSP
		{
			pwd = "";
			save = 0;
		}
		else
		{
			pwd.Format("%s",CERTINFO.cCertPwd);
			path.Format("%s",CERTINFO.cCertDir);
			save = 1;
		}
		
		
		stuCertPair RootPair(strCert,lenCert,strKey,lenKey,strPwd.GetBuffer(0));
		
		//�Ƿ�����֤��ɹ���,����ɹ���д�����ݿ�,����д��
		DWORD dCertSn = m_PageMan.GetCertSn();
		if(dCertSn == -1)
		{
			AddMsg(MiniCT_0210,M_ERROR);			//MiniCT_0210 "��ѯ֤�����ʧ��" 
			return;
		}
		
		//���ȵõ����ݿ���������
		if(DirectCert(RootPair,CERTINFO.uCertLen,
			dCertSn,0,CERTINFO.uCertDay,pwd.GetBuffer(0),pCERT,"(MiniCA)",KUSAGE.GetBuffer(0),
			EKUSAGE.GetBuffer(0),pCertExt,
			cert,&certl,key,&keyl,p12,&p12l,out,CERTINFO.uCertFormat))
		{
			
			if(save)//����
			{
				UINT m_NameType = 0;
				CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
				CCertDbPage * pDb = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));
				if(pDb)
				{
					m_NameType = pDb->GetNameType();
				}
				
				CString strName;
				if(0 == m_NameType)//�û���
				{
					strName.Format("\\%s", pCERT->GetCN());
				}
				else if(1 == m_NameType)//���
				{
					strName.Format("\\%d", dCertSn);
				}
				else if(2 == m_NameType)//�û��� + ���
				{
					strName.Format("\\%s%d", pCERT->GetCN(), dCertSn);
				}
				else if(3 == m_NameType)//��� + �û���
				{
					strName.Format("\\%d%s", dCertSn, pCERT->GetCN());
				}
				
				CString outCert = path + strName + "Cert.Cer";
				CString outKey = path + strName + "Key.Cer";
				CString p12path = path + strName + ".Pfx";
				
				FILE * pfc = fopen(outCert,"wb");
				if(!pfc)
				{
					AddMsg(MiniCT_0128,M_ERROR);	//MiniCT_0128 "���湫Կ�ļ�ʧ��"
					return;
				}
				
				fwrite(cert,sizeof(char),certl,pfc);
				fclose(pfc);
				
				pfc = fopen(outKey,"wb");
				if(!pfc)
				{
					AddMsg(MiniCT_0127,M_ERROR);	//MiniCT_0127 "����˽Կ�ļ�ʧ��"
					return;
				}
				
				fwrite(key,sizeof(char),keyl,pfc);
				fclose(pfc);
				AddMsg(MiniCT_0124);			//MiniCT_0124 "����֤��ɹ�"
				
				
				if(CERTINFO.bCertP12)
				{
					pfc = fopen(p12path,"wb");
					if(!pfc)
					{
						AddMsg(MiniCT_0126,M_ERROR);	//MiniCT_0126 "����PFX�ļ�ʧ��"
						return;
					}
					fwrite(p12,sizeof(char),p12l,pfc);
					fclose(pfc);
					AddMsg(MiniCT_0125);		//MiniCT_0125 "֤��PFX�������ɹ�"
				}
			}
			else//CSP
			{
				ImportCSP(p12,p12l,CERTINFO.cCertCsp,CERTINFO.cCertCon);
				//GetCspCertInfo(CERTINFO.cCertCsp,CERTINFO.cCertCon,0);
			}
			//֤����Ϣ��������ݿ�
			m_PageMan.SaveDb(pCERT, CERTINFO.uCertLen, type, 
				CERTINFO.uCertDay, p12, p12l, pwd.GetBuffer(0));
			
		}
		else
			AddMsg(out,M_ERROR);
	}
	else
		AddMsg(MiniCT_0123,M_ERROR);			//MiniCT_0123 "���ظ�֤��ʧ��"
}


void CCaCertWizardSheet::MakeRootCsp(stuSUBJECT * pCERT,
								 CCaCertInfoPage::stuCERTINFO & CERTINFO,
								 const int type,
								 CString KUSAGE,  CString EKUSAGE,
								 const stuCERTEXT * pCertExt)
{
	char		outMsg[500] = {0};
	long		lret = 0;
	int			iret = 0;
	bool		bok	 = true;
	
	DWORD	lenCert=0;	//��Կ����
	DWORD	lenKey=0;	//˽Կ����
 	CString strPwd;		//��˽Կ·��������,p12��Կ
	char	strCert[10240] = {0};
	char	strKey[10240] = {0};


	X509        *xcert  = NULL;
	X509_REQ	*xreq  = NULL;
	RSA			*rsa   = NULL;
	X509_NAME	*xname = NULL;
	BIO			*bcert = NULL;
	const EVP_MD *dgst = NULL;


	X509		*px509root = NULL;
	EVP_PKEY	*pkroot    = NULL;

	char		outMem[10240] = {0} /*�����Կ�ļ�*/;
	UINT		MemLen = sizeof outMem;/*�������*/;


	Ossl::COssl		ossl;		// Openssl Object
	CCertKitCsp		csp;		// Csp Object
	tstring		szP10;				

	
	// 	OpenSSL_add_all_algorithms();	//leak EVP_cleanup(void);
	// 	ERR_load_crypto_strings();	// leak  ERR_free_strings()

	dgst = EVP_get_digestbyname("sha1");
	if (!dgst)
		return;

	do 
	{
		// CSP  Create CSR(PKCS#10)
		csp.SetCspName(CERTINFO.cCertCsp);
		csp.SetCtnName(CERTINFO.cCertCon);
		csp.SetReqRegKey("MINICA_CSP_REQ");
		lret = csp.CreateP10(szP10);
		
		if(0 == szP10.size()){
			char err[30]={0};
			sprintf(err, "csp ����p10 ���� 0x%08x", lret);
			AddMsg(err, M_ERROR);
			break;
		}
			
		// csr  d2i ossl
 		CByteBuf memP10(szP10.length());
		CBase64::Decode(szP10.c_str(), memP10.ptr, (unsigned long*)&memP10.len);
		
		xreq = d2i_X509_REQ(NULL, (const unsigned char**)&memP10.ptr, memP10.len);
		if(!xreq){
			AddMsg("p10֤������opensslת������", M_ERROR);
			break;
		}

		//csr add subject;  
		//  !!! this  destroy the csr  signature;
		xname = X509_REQ_get_subject_name(xreq);
		ossl.AddName(xname, (const struct Ossl::stuCERTEXT*)pCERT);
		
		// get root cert & key
		if(!((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd)){
			AddMsg("��ȡroot֤��ʧ��", M_ERROR);
			break;
		}
		
		//root cert & key 2 EVP & X509
		Ossl::stuCertPair RootPair(strCert,lenCert,strKey,lenKey,strPwd.GetBuffer(0));
		ossl.CertPair2XE(RootPair,px509root,pkroot,outMsg);
		
		// 
		int iret = ossl.certify(&xcert, xreq, pkroot, px509root, dgst,
								GenCertNumDateInt(), "today", 0, CERTINFO.uCertDay,
								KUSAGE,EKUSAGE,(const struct Ossl::stuCERTEXT *)pCertExt, outMsg);
		
		
		if(iret <= 0){
			AddMsg("����֤��ʧ�� ossl.certify", M_ERROR);
			break;
		}


		bcert = BIO_new(BIO_s_mem());
		BIO_set_close(bcert, BIO_CLOSE);
		iret = i2d_X509_bio(bcert,xcert); 
		
		if(iret){
			iret = ossl.Bio2Mem(bcert, outMem, &MemLen);
		}
		
		if (iret)		{
			CByteBuf certbuf(MemLen*2);
			CBase64::Encode((const unsigned char*)outMem, MemLen, (char*)certbuf.ptr, (unsigned long*)&certbuf.len);
			csp.AcceptCert((TCHAR*)certbuf.ptr, certbuf.len);
		}

	} while (0);

}



void CCaCertWizardSheet::MakeCertCsp(stuSUBJECT * pCERT,
									 CCaCertInfoPage::stuCERTINFO & CERTINFO,
									 const int type,
									 CString KUSAGE,  CString EKUSAGE,
									 const stuCERTEXT * pCertExt)
{
	char		outMsg[500] = {0};
	long		lret = 0;
	int			iret = 0;
	bool		bok	 = true;
	
	DWORD	lenCert=0;	//��Կ����
	DWORD	lenKey=0;	//˽Կ����
 	CString strPwd;		//��˽Կ·��������,p12��Կ
	char	strCert[10240] = {0};
	char	strKey[10240] = {0};


	X509        *xcert  = NULL;
	X509_REQ	*xreq  = NULL;
	RSA			*rsa   = NULL;
	X509_NAME	*xname = NULL;
	BIO			*bcert = NULL;
	const EVP_MD *dgst = NULL;


	X509		*px509root = NULL;
	EVP_PKEY	*pkroot    = NULL;

	char		outMem[10240] = {0} /*�����Կ�ļ�*/;
	UINT		MemLen = sizeof outMem;/*�������*/;


	Ossl::COssl		ossl;		// Openssl Object
	CCertKitCsp		csp;		// Csp Object
	tstring		szP10;				

	
	// 	OpenSSL_add_all_algorithms();	//leak EVP_cleanup(void);
	// 	ERR_load_crypto_strings();	// leak  ERR_free_strings()

	dgst = EVP_get_digestbyname("sha1");
	if (!dgst)
		return;

	do 
	{
		// CSP  Create CSR(PKCS#10)
		csp.SetCspName(CERTINFO.cCertCsp);
		csp.SetCtnName(CERTINFO.cCertCon);
		csp.SetReqRegKey("MINICA_CSP_REQ");
		lret = csp.CreateP10(szP10);
		
		if(0 == szP10.size()){
			char err[30]={0};
			sprintf(err, "csp ����p10 ���� 0x%08x", lret);
			AddMsg(err, M_ERROR);
			break;
		}
			
		// csr  d2i ossl
 		CByteBuf memP10(szP10.length());
		CBase64::Decode(szP10.c_str(), memP10.ptr, (unsigned long*)&memP10.len);
		
		xreq = d2i_X509_REQ(NULL, (const unsigned char**)&memP10.ptr2, memP10.len);
		if(!xreq){
			AddMsg("p10֤������opensslת������", M_ERROR);
			break;
		}

		//csr add subject;  
		//  !!! this  destroy the csr  signature;
		xname = X509_REQ_get_subject_name(xreq);
		ossl.AddName(xname, (const struct Ossl::stuCERTEXT*)pCERT);
		
		// get root cert & key
		if(!((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd)){
			AddMsg("��ȡroot֤��ʧ��", M_ERROR);
			break;
		}
		
		//root cert & key 2 EVP & X509
		Ossl::stuCertPair RootPair(strCert,lenCert,strKey,lenKey,strPwd.GetBuffer(0));
		ossl.CertPair2XE(RootPair,px509root,pkroot,outMsg);
		
		// 
		int iret = ossl.certify(&xcert, xreq, pkroot, px509root, dgst,
								GenCertNumDateInt(), "today", 0, CERTINFO.uCertDay,
								KUSAGE,EKUSAGE,(const struct Ossl::stuCERTEXT *)pCertExt, outMsg);
		
		
		if(iret <= 0){
			AddMsg("����֤��ʧ�� ossl.certify", M_ERROR);
			break;
		}


		bcert = BIO_new(BIO_s_mem());
		BIO_set_close(bcert, BIO_CLOSE);
		iret = i2d_X509_bio(bcert,xcert); 
		
		if(iret){
			iret = ossl.Bio2Mem(bcert, outMem, &MemLen);
		}
		
		if (iret)		{
			CByteBuf certbuf(MemLen*2);
			CBase64::Encode((const unsigned char*)outMem, MemLen, (char*)certbuf.ptr, (unsigned long*)&certbuf.len);
			csp.AcceptCert((TCHAR*)certbuf.ptr, certbuf.len);
		}

	} while (0);


}


void CCaCertWizardSheet::MakeCertP11(stuSUBJECT * pCERT,
									 CCaCertInfoPage::stuCERTINFO & CERTINFO,
									 const int type,
									 CString KUSAGE,  CString EKUSAGE,
									 const stuCERTEXT * pCertExt)
{
	char		outMsg[500] = {0};
	long		lret = 0;
	int			iret = 0;
	bool		bok	 = true;
	
	DWORD	lenCert=0;	//��Կ����
	DWORD	lenKey=0;	//˽Կ����
 	CString strPwd;		//��˽Կ·��������,p12��Կ
	char	strCert[10240] = {0};
	char	strKey[10240] = {0};


	X509        *xcert  = NULL;
	X509_REQ	*xreq  = NULL;
	RSA			*rsa   = NULL;
	X509_NAME	*xname = NULL;
	BIO			*bcert = NULL;
	const EVP_MD *dgst = NULL;


	X509		*px509root = NULL;
	EVP_PKEY	*pkroot    = NULL;

	char		outMem[10240] = {0} /*�����Կ�ļ�*/;
	UINT		MemLen = sizeof outMem;/*�������*/;


	Ossl::COssl		ossl;		// Openssl Object
	CPEleven		cp11;		// P11 Object
	tstring			szP10;				

	
	// 	OpenSSL_add_all_algorithms();	//leak EVP_cleanup(void);
	// 	ERR_load_crypto_strings();	// leak  ERR_free_strings()

	dgst = EVP_get_digestbyname("sha1");
	if (!dgst)
		return;

	do 
	{
		// P11  Create CSR(PKCS#10)
		cp11.SetP11Path(CERTINFO.cP11dir);
		cp11.LoginToken((const unsigned char *)CERTINFO.cCertPwd2, strlen(CERTINFO.cCertPwd2));
		lret = cp11.CreateReqP10(szP10); 
		
		if(0 == szP10.size()){
			char err[30]={0};
			sprintf(err, "csp ����p10 ���� 0x%08x", lret);
			AddMsg(err, M_ERROR);
			break;
		}
			
		// csr  d2i ossl
 		CByteBuf memP10(szP10.length()*2);
		CBase64::Decode(szP10.c_str(), memP10.ptr, (unsigned long*)&memP10.len);
		
		xreq = d2i_X509_REQ(NULL, (const unsigned char**)&memP10.ptr2, memP10.len);
		if(!xreq){
			AddMsg("p10֤������opensslת������", M_ERROR);
			break;
		}

		//csr add subject;  
		//  !!! this  destroy the csr  signature;
		xname = X509_REQ_get_subject_name(xreq);
		ossl.AddName(xname, (const struct Ossl::stuCERTEXT*)pCERT);
		
		// get root cert & key
		if(!((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd)){
			AddMsg("��ȡroot֤��ʧ��", M_ERROR);
			break;
		}
		
		//root cert & key 2 EVP & X509
		Ossl::stuCertPair RootPair(strCert,lenCert,strKey,lenKey,strPwd.GetBuffer(0));
		ossl.CertPair2XE(RootPair,px509root,pkroot,outMsg);
		
		// 
		int iret = ossl.certify(&xcert, xreq, pkroot, px509root, dgst,
								GenCertNumDateInt(), "today", 0, CERTINFO.uCertDay,
								KUSAGE,EKUSAGE,(const struct Ossl::stuCERTEXT *)pCertExt, outMsg);
		
		
		if(iret <= 0){
			AddMsg("����֤��ʧ�� ossl.certify", M_ERROR);
			break;
		}


		bcert = BIO_new(BIO_s_mem());
		BIO_set_close(bcert, BIO_CLOSE);
		iret = i2d_X509_bio(bcert,xcert); 
		
		if(iret) {
			iret = ossl.Bio2Mem(bcert, outMem, &MemLen);
		}
		
		if (iret) {
			cp11.InstallCert((const unsigned char*)outMem, MemLen);   //.AcceptCert((TCHAR*)certbuf.ptr, certbuf.len);
		}

	} while (0);

	return;

}
									 
int CCaCertWizardSheet::GenCertNumDateInt()
{
// 	char str[20]={0};
// 	SYSTEMTIME stime;
// 	GetLocalTime(&stime);
// 	sprintf(str, "%02d%02d%02d%02d%02d",stime.wYear, stime.wMonth, stime.wDay, stime.wMinute, stime.wSecond);

	//TODO

	srand(time(NULL));
	return rand();
}


