// CertInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CertInfoPage.h"
#include "MiniMainDlg.h"
#include <locale.h>
#include "Evp.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCertInfoPage property page
const UINT uIniTextLen = 20;
IMPLEMENT_DYNCREATE(CCertInfoPage, CPropertyPage)

CCertInfoPage::CCertInfoPage() : CPropertyPage(CCertInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CCertInfoPage)
	//}}AFX_DATA_INIT
}

CCertInfoPage::~CCertInfoPage()
{
}

void CCertInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCertInfoPage)
//	DDX_Control(pDX, IDC_EDIT_PATH, m_XpEdit);
	DDX_Control(pDX, IDC_CERTINFO_READ, m_Bread);
	DDX_Control(pDX, IDC_EDIT_INFO, m_RichEdit);
	DDX_Control(pDX, IDC_LIST_MOD, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCertInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCertInfoPage)
	ON_BN_CLICKED(IDC_CERTINFO_READ, OnBRead)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MOD, OnItemchangedListMod)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCertInfoPage message handlers

BOOL CCertInfoPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_Bread.SetThemeHelper(((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
	m_Bread.SetIcon(IDI_ICON_SELECTDIR);
	m_Bread.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	// TODO: Add extra initialization here
	m_List.InsertColumn(0,MiniCT_1500,LVCFMT_LEFT,160); //字符域
	m_List.InsertColumn(1,MiniCT_1501,LVCFMT_LEFT,300); //"值"
	m_List.SetExtendedStyle(LVS_EX_UNDERLINECOLD|LVS_EX_FULLROWSELECT);
	
	CImageList * pImgList = ((CMiniCaApp *)AfxGetApp())->GetImgList();
	m_List.SetImageList(pImgList,LVSIL_SMALL);//用来改变LISTCTRL行宽度

/*	CHARFORMAT cfDefault;
	memset(&cfDefault,0,sizeof(cfDefault));
	cfDefault.cbSize=sizeof(cfDefault);
	cfDefault.dwMask=CFM_SIZE|CFM_SPACING;
	cfDefault.yHeight=200;
	m_RichEdit.SetDefaultCharFormat(cfDefault);//设置模式
*/

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
	//	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_PATH), CMiniCaApp::NormalCode("被解析的证书"));
		m_toolTip.AddTool(GetDlgItem(IDC_CERTINFO_READ), CMiniCaApp::NormalCode("选择要解析证书文件"));

		// TODO: Use one of the following forms to add controls:
	}


	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_EDIT_PATH)->m_hWnd,TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCertInfoPage::OnBRead() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, MiniCT_0721,NULL);

	CString strTitle = MiniCT_0719;
	dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条

	if(dlgOpen.DoModal()!=IDOK) return;
	SetDlgItemText(IDC_EDIT_PATH,dlgOpen.GetPathName());
	char buf[256]={0};
	CString file = dlgOpen.GetPathName();
	if(!ViewCertInfo(file.GetBuffer(0), 0, NULL, buf))
		AddMsg(MiniCT_1502,	M_ERROR);
}

void CCertInfoPage::OnItemchangedListMod(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (pNMListView->uChanged == LVIF_STATE)
	{
		if (pNMListView->uNewState)//选择改变
		{
			CString str=m_List.GetItemText(pNMListView->iItem,1);//得到内容
			if(strcmp(m_List.GetItemText(pNMListView->iItem,0), MiniCT_1510)==0)
			{
				CString strL,strR(m_strPubKey),strtemp;
				str.Empty();
				for(;;)
				{
					if(strlen(strR)==0)
						break;
					strL=strR.Left(4);
					strR=strR.Right(strR.GetLength()-4);
					strR.TrimLeft();
					strtemp.Format("%s ",strL);
					str+=strtemp;
				}
			}
			else
			{
				str.TrimRight("\n");
				for(int nStart=0;;)
				{
					int state=str.Find("\n",nStart);
					if(state==-1)
						break;
					str.Insert(state,"\r");
					nStart=state+2;
				}
			}
			SetDlgItemText(IDC_EDIT_INFO,str);
		}
	}

	*pResult = 0;
}

BOOL CCertInfoPage::ViewCertInfo(char * cert, UINT certLen,void *x509, char *buf)
{
	m_List.SetRedraw(FALSE);
	m_List.DeleteAllItems();
	SetDlgItemText(IDC_EDIT_INFO,"");

	if(certLen == 0 && x509 == 0)//表示拖动操作
	{
		SetDlgItemText(IDC_EDIT_PATH, cert);
	}

	stuCERTINFO info;
	if(!GetCertInfo(cert,certLen,x509,info,buf))
	{
		m_List.SetRedraw(TRUE);
		return FALSE;
	}

	CString str;
	int index=0;

	m_List.InsertItem(index, MiniCT_1503, 7);	//版本
	str.Format("V%d",info.VER);
	m_List.SetItemText(index++,1,str);
	if(strlen(info.SN))//序号
	{
		m_List.InsertItem(index, MiniCT_1504, 7);//序列号
		str.Format("%04s",info.SN);
		m_List.SetItemText(index++,1,str);
	}
	if(strlen(info.SIGNATURE))//签名算法
	{
		m_List.InsertItem(index, MiniCT_1505, 7); //签名算法
		str.Format("%s",info.SIGNATURE);
		m_List.SetItemText(index++,1,str);
	}
	if(strlen(info.ISSUER))//颁发者
	{
		m_List.InsertItem(index, MiniCT_1506, 7); //颁发者
		str.Format("%s",info.ISSUER);
		m_List.SetItemText(index++,1,str);

	}
	if(strlen(info.NOTBEFORE))//起始日期
	{
		m_List.InsertItem(index, MiniCT_1507, 7);
		str.Format("%s",info.NOTBEFORE);
		m_List.SetItemText(index++,1,CMiniCaApp::NormalCode(str));
	}
	if(strlen(info.NOTAFTER))//中止日期
	{
		m_List.InsertItem(index, MiniCT_1508, 7);
		str.Format("%s",info.NOTAFTER);
		m_List.SetItemText(index++,1,CMiniCaApp::NormalCode(str));
	}
	if(strlen(info.SUBJECT))//主题
	{
		m_List.InsertItem(index, MiniCT_1509, 7);
		str.Format("%s",info.SUBJECT);
		m_List.SetItemText(index++,1,str);
	}
	if(strlen(info.PUBKEY))//公钥
	{
		m_List.InsertItem(index, MiniCT_1510, 7);
		str.Format("%s (%d Bits)",info.PUBTYPE,info.PUBLEN);
		m_List.SetItemText(index++,1,str);
		m_strPubKey.Format("%s",info.PUBKEY);//保存公钥
	}
	if(strlen(info.V3EXT))//扩展信息
	{
		str.Format("%s",info.V3EXT);
		CString strtemp,strL,strR(str),strl,strr;
		for(;;)
		{
			int state=strR.Find("\r\n");
			if(state==-1)
				break;
			strL=strR.Left(state);
			strR=strR.Right(strR.GetLength()-state-1);
			strR.TrimLeft();
			strtemp.Format("%s",strL);
			int j=strtemp.Find(":");
			strl=strtemp.Left(j);
			strr=strtemp.Right(strtemp.GetLength()-j-1);
			strr.TrimLeft();
			m_List.InsertItem(index,CMiniCaApp::NormalCode(strl),8);
			m_List.SetItemText(index++,1,CMiniCaApp::NormalCode(strr));

		}
	}
	if(strlen(info.THUMB))//缩微图算法
	{
		m_List.InsertItem(index, MiniCT_1511, 9);
		str.Format("%s",info.THUMB);
		m_List.SetItemText(index++,1,str);
	}
	if(strlen(info.THUMBPRINT))//缩微图
	{
		m_List.InsertItem(index, MiniCT_1512, 9);
		str.Format("%s",info.THUMBPRINT);
		CString strL,strR(str),strtemp;
		str.Empty();
		for(;;)
		{
			if(strlen(strR)==0)
				break;
			strL=strR.Left(4);
			strR=strR.Right(strR.GetLength()-4);
			strR.TrimLeft();
			strtemp.Format("%s ",strL);
			str+=strtemp;
		}

		m_List.SetItemText(index++,1,str);
	}

	m_List.SetRedraw();
	return TRUE;
}

void CCertInfoPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)GetParent())->AddMsg(MiniCT_0004,info, type); //"数证解析"
}

/*void CCertInfoPage::OnDropFiles(HDROP hDropInfo) 
{
	//取得被拖动文件的数目
	UINT uiFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
    for (UINT uiFile = 0; uiFile < uiFiles; uiFile++)
    {
		//取得第uiFile个拖动文件名所占字节数
        UINT uiLen = ::DragQueryFile(hDropInfo, uiFile, 0, 0);
        TCHAR *pszFileName = new TCHAR[uiLen + 1];
        ::DragQueryFile(hDropInfo, uiFile, pszFileName, uiLen + 1);
		SetDlgItemText(IDC_EDIT_PATH,pszFileName);
		char buf[256]={0};
		ViewCertInfo(pszFileName, NULL, buf);
        delete[] pszFileName;
	}
	::DragFinish(hDropInfo);
}
*/

BOOL CCertInfoPage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CCertInfoPage::Utf8ToAnsi(const UCHAR * lpsrc,const int srclen, LPSTR lpdst, int& dstlen)
{
	WCHAR * Unicode;
    int len = MultiByteToWideChar ( CP_UTF8 , 0 ,(char*) lpsrc ,-1 ,NULL,0);
    Unicode = new WCHAR[len * sizeof(WCHAR)];
    MultiByteToWideChar ( CP_UTF8 , 0 ,( char * ) lpsrc, -1, Unicode , len );
    len = WideCharToMultiByte(CP_ACP,0,Unicode,-1,NULL,0,NULL,NULL);
    dstlen = WideCharToMultiByte (CP_ACP,0,Unicode,-1,lpdst,len,NULL,NULL);
    delete []Unicode;
}

BOOL CCertInfoPage::GetCertName(void *name,char * outName)
{
	if(name == NULL)		//X509_NAME_get_text_by_NID
		return FALSE;
	int num = X509_NAME_entry_count((X509_NAME *)name);
	X509_NAME_ENTRY * entry;
	ASN1_OBJECT * obj;
	ASN1_STRING * str;
	char objtmp[80] = {0};
	char  pmbbuf[3] = {0};
	int fn_nid = 0;
	const char * objbuf;


	setlocale(LC_CTYPE, ""); //NULL,””,”C”代表操作系统的缺省代码页 ; ”.ACP”，代表操作系统当前使用的代
	
	for(int i = 0 ;i < num;i ++)
	{
		entry = (X509_NAME_ENTRY *)X509_NAME_get_entry((X509_NAME *)name,i);
		obj = X509_NAME_ENTRY_get_object(entry);
		str = X509_NAME_ENTRY_get_data(entry);//得到 ASN1_STRING
		//判断str是否为UTF-8
		int len = 0;
		unsigned char * tmp = NULL; 
		if (str && ASN1_STRING_type(str) == V_ASN1_UTF8STRING)
		{
			len = ASN1_STRING_length(str);
			if (len >= 0)
			{
				tmp = (UCHAR *)OPENSSL_malloc(len+1);
				if (tmp)
				{
					memcpy(tmp, ASN1_STRING_data(str), len);
					tmp[len] = '\0';
				}
			}
		}
	    else /* not a UTF8 name */		///////////转UTF-8
		{
			len = ASN1_STRING_to_UTF8(&tmp, str);
		}
		char buf[1024] = {0};
		int dstlen = 0;
		Utf8ToAnsi(tmp, len, buf, dstlen);
		OPENSSL_free(tmp);
		fn_nid = OBJ_obj2nid(obj);
		if(fn_nid == NID_undef)
			OBJ_obj2txt(objtmp, sizeof objtmp, obj, 1);
		else
		{
			objbuf = OBJ_nid2sn(fn_nid);
			strcpy(objtmp,objbuf);
			//objbuf = OBJ_nid2ln(fn_nid);
		}

		BIO *mem = BIO_new(BIO_s_mem());
		BIO_set_close(mem, BIO_CLOSE); 
		ASN1_STRING_print_ex(mem,str,ASN1_STRFLGS_ESC_QUOTE );
		BUF_MEM * bptr;
		BIO_get_mem_ptr(mem, &bptr);
		len = bptr->length;
		char * pbuf = new char[len+1];
		memset(pbuf,0,len+1);
		memcpy(pbuf,bptr->data,len);
		
		delete [] pbuf;
		if (mem != NULL) BIO_free(mem);
		OBJ_cleanup();

		strcat(outName,objtmp);//C
		strcat(outName,"=");//=
		strcat(outName,buf);
		strcat(outName," ");
		strcat(outName,"\n");
	}
	return TRUE;
}

char * CCertInfoPage::GetCertTime(const ASN1_UTCTIME *s)
{
	char buf[128]={0};
	char * p=buf;
	struct tm tm;

	memset(&tm,'\0',sizeof tm);

#define g2(p) (((p)[0]-'0')*10+(p)[1]-'0')
	tm.tm_year=g2(s->data);
	if(tm.tm_year < 50)
		tm.tm_year+=100;
	tm.tm_mon=g2(s->data+2);
	tm.tm_mday=g2(s->data+4);
	tm.tm_hour=g2(s->data+6);
	tm.tm_min=g2(s->data+8);
	tm.tm_sec=g2(s->data+10);
#undef g2

	TIME_ZONE_INFORMATION TimeZoneInformation;
	GetTimeZoneInformation(&TimeZoneInformation);
 
	sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",tm.tm_year+1900,tm.tm_mon,tm.tm_mday,
		tm.tm_hour-(TimeZoneInformation.Bias)/60,tm.tm_min,tm.tm_sec);
	return p;
}

int CCertInfoPage::GetExtensions(BIO *bp,STACK_OF(X509_EXTENSION) *exts)
{
	const UINT uLen = 82;
	char objtmp[uLen]={0};
	int fn_nid;

	int count=sk_X509_EXTENSION_num(exts);
	if( count<= 0) return 1;
	for (int i=0; i<count; i++)
	{
		ASN1_OBJECT *obj;
		X509_EXTENSION *ex;
		ex=sk_X509_EXTENSION_value(exts, i);
	/*	if(i!=0)
		{
			if (BIO_printf(bp,"%*s",indent, "") <= 0) return 0;//indent空格长度
		}
*/		obj=X509_EXTENSION_get_object(ex);

		fn_nid = OBJ_obj2nid(obj);
		if(fn_nid==NID_undef)
			OBJ_obj2txt(objtmp, sizeof objtmp, obj, 1);
		else
		{
			switch(fn_nid)
			{
			case 82://"X509v3 Subject Key Identifier" 
				strncpy(objtmp, MiniCT_1513, uLen);	//使用者密钥标识符
				break;
			case 83://"X509v3 Key Usage"  
				strncpy(objtmp, MiniCT_1514, uLen); //密钥用法
				break;
			case 84://"X509v3 Private Key Usage Period" 
				strncpy(objtmp, MiniCT_1515, uLen);	//私钥周期
				break;
			case 85://"X509v3 Subject Alternative Name"
				strncpy(objtmp,MiniCT_1516, uLen);	//使用者备用名称
				break;
			case 86://"X509v3 Issuer Alternative Name" 3 
				strncpy(objtmp, MiniCT_1517, uLen); //颁发机构备用名称
				break;
			case 87://"X509v3 Basic Constraints" 4
				strncpy(objtmp, MiniCT_1518, uLen); //基本限制
				break;
			case 88://"X509v3 CRL Number"
				strncpy(objtmp, MiniCT_1519, uLen); //CRL数量
				break;
			case 141://"X509v3 CRL Reason Code"
				strncpy(objtmp, MiniCT_1520, uLen); //CRL吊销原因
				break;
			case 103://"X509v3 CRL Distribution Points"
				strncpy(objtmp, MiniCT_1521, uLen);	//CRL 分发点
				break;
			case 89://"X509v3 Certificate Policies"
				strncpy(objtmp, MiniCT_1522, uLen); //证书策略
				break;
			case 90://"X509v3 Authority Key Identifier"
				strncpy(objtmp, MiniCT_1523, uLen);
				break;
			case 126://"X509v3 Authority Key Identifier"
				strncpy(objtmp, MiniCT_1524, uLen); //增强型密钥用法
				break;
			case 177://Authority Information Access
				strncpy(objtmp, MiniCT_1525, uLen);	//颁发机构信息访问
				break;
			default:
				i2t_ASN1_OBJECT(objtmp,sizeof(objtmp),obj);
				break;

			}
		}

		BIO_printf(bp,"%s: ",objtmp);//输入字符域
		if(!X509V3_EXT_print(bp, ex, X509_FLAG_COMPAT, 1))//输出可以识别项
		{
			M_ASN1_OCTET_STRING_print(bp,ex->value);//输出不可识别项目
		}
		BIO_puts(bp, "\r\n");//分割
	}
	return 1;
}

UINT CCertInfoPage::GetKeyLen(const void * px509)
{
	UINT uReturn = 0;
	X509 * x509 = NULL;
	if(px509 != NULL)
		x509 = (X509 *)px509;
	//得到公钥
	EVP_PKEY * pkey = NULL;
	pkey = X509_get_pubkey(x509);
	if(pkey == NULL)
	{
		return 0;
	}
	
	if(EVP_PKEY_type(pkey->type) == EVP_PKEY_RSA)/////////RSA
	{
		RSA * rsa = EVP_PKEY_get1_RSA(pkey);
		if(rsa != NULL)
		{
			//公钥长度 
			uReturn = EVP_PKEY_bits(pkey);
			RSA_free(rsa);
		}
	}
	return uReturn;

}

BOOL CCertInfoPage::GetSubjectInfo(const void * px509,char * name,char * msg)
{
	X509 * x509=NULL;
	if(px509!=NULL)
		x509=(X509 *)px509;
	BOOL b = GetCertName(X509_get_subject_name(x509),(char *)name);
	if(!b)
	{
		strncpy(msg, MiniCT_1526, 24); //取得证书个体名称失败
	}
	return b;
}

//得到证书信息
BOOL CCertInfoPage::GetCertInfo(const char * pCert,
				 const UINT certLen,
				 const void * px509,
				 stuCERTINFO & certinfo,
				 char * msg)
{
	BOOL bFreeX509 = FALSE; //是否释放X509标识,如果内部合成的,最后释放,如果外部传入的,不释放
	BOOL bRet = TRUE;	//操作结果
	char buf[255] = {0};
	UINT bits = 0;
	char * certBuf = NULL;
	X509 * x509 = NULL;
	EVP_PKEY * pkey = NULL;
	char * stringval = NULL;
	BIO * mem = NULL;
	BUF_MEM *bptr = NULL;
	X509_CINF *ci = NULL;
	int len = 0;
	int type ;
	if(pCert)
		len = strlen(pCert);	
	if(px509 != NULL)
		x509 = (X509 *)px509;
	else if(len != 0)
	{
		x509 = CCertKey::LoadCert((char *)pCert,certLen,"",msg);
		if (x509 == NULL)
		{
			strncpy(msg, MiniCT_1527, uIniTextLen); //转换证书失败
			bRet =  FALSE;
			goto err;
		}
		bFreeX509 = TRUE;
	}
	else
	{
		strncpy(msg, MiniCT_1528, uIniTextLen); //没有任何证书信息
		bRet =  FALSE;
		goto err;
	}
	ci = x509->cert_info;
	mem = BIO_new(BIO_s_mem());
	BIO_set_close(mem, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */

	//版本
	certinfo.VER=X509_get_version(x509)+1;

	//序列号
	stringval = i2s_ASN1_INTEGER(NULL,X509_get_serialNumber(x509)); //leak
	sprintf(certinfo.SN,"0%X",atoi(stringval));
	OPENSSL_free(stringval);

	//签名算法
	i2t_ASN1_OBJECT(certinfo.SIGNATURE,1024,ci->signature->algorithm);

	//颁发者
	GetCertName(X509_get_issuer_name(x509),certinfo.ISSUER);

	//起始日期
	strcpy(certinfo.NOTBEFORE,GetCertTime(X509_get_notBefore(x509)));
	//中止日期
	strcpy(certinfo.NOTAFTER,GetCertTime(X509_get_notAfter(x509)));

	//主题
	GetCertName(X509_get_subject_name(x509),certinfo.SUBJECT);

	//得到公钥
	pkey = X509_get_pubkey(x509);
	if(pkey == NULL)
	{
		strncpy(msg, MiniCT_1529, uIniTextLen); //取得公钥失败
		bRet =  FALSE;
		goto err;
	}
	
	type = EVP_PKEY_type(pkey->type);


	if(type==EVP_PKEY_RSA)/////////RSA
	{
	//	char * sign=BN_bn2hex(pkey->pkey.rsa->n);
		//转换公钥rsa->der->数字
		strcpy(certinfo.PUBTYPE,"RSA");
		RSA * rsa = EVP_PKEY_get1_RSA(pkey);
		if(rsa != NULL)
		{
			//公钥长度 
			bits = EVP_PKEY_bits(pkey);
			certinfo.PUBLEN = bits;
			certBuf = new char[bits+1];
			i2d_RSAPublicKey_bio(mem,rsa);
			BIO_get_mem_ptr(mem, &bptr);
			UINT len = bptr->length;
			char * pbuf = new char[len+1];
			memset(pbuf,0,len+1);
			memcpy(pbuf,bptr->data,len);
			for(DWORD i = 0, j = 0; i < len, j < len*2; i++, j+=2)
			{
				unsigned char t = pbuf[i];
				sprintf((char *)&certBuf[j],"%X",t>>4);
				sprintf((char *)&certBuf[j+1],"%X",t&0x0F);
			}
			UINT keylen = sizeof(certinfo.PUBKEY);
			if(keylen < len*2)
				strncpy(certinfo.PUBKEY,certBuf,keylen);
			else
				strncpy(certinfo.PUBKEY,certBuf,bits+1);
			delete []pbuf;
			delete []certBuf;
			RSA_free(rsa);
		}
		else
			strncpy(certinfo.PUBKEY, MiniCT_1530, uIniTextLen); //取得RSA失败
	}
	else if(type == EVP_PKEY_DSA)/////////DSA
	{
	//	char * sign=BN_bn2hex(pkey->pkey.rsa->n);
		//转换公钥rsa->der->数字
		strcpy(certinfo.PUBTYPE,"DSA");
		DSA * dsa = EVP_PKEY_get1_DSA(pkey);
		if(dsa!=NULL)
		{
			//公钥长度 
			bits=BN_num_bits(dsa->pub_key);
			certinfo.PUBLEN=bits;
			certBuf=new char[bits+1];

			i2d_DSA_PUBKEY_bio(mem,dsa);
			BIO_get_mem_ptr(mem, &bptr);
			UINT len=bptr->length;
			char * pbuf=new char[len+1];
			memset(pbuf,0,len+1);
			memcpy(pbuf,bptr->data,len);
			for(DWORD i = 0,j = 0; i < len, j<len*2; i++, j+=2)
			{
				unsigned char t = pbuf[i];
				sprintf((char *)&certBuf[j],"%X",t>>4);
				sprintf((char *)&certBuf[j+1],"%X",t&0x0F);
			}
			UINT keylen = sizeof(certinfo.PUBKEY);
			if(keylen < len*2)
				strncpy(certinfo.PUBKEY,certBuf,keylen);
			else
				strncpy(certinfo.PUBKEY,certBuf,bits+1);
			delete []pbuf;
			DSA_free(dsa);
		}
		else
			strncpy(certinfo.PUBKEY, MiniCT_1531, uIniTextLen); //取得DSA失败
	}


	//扩展信息
	BIO_reset(mem);//恢复bio
	if(GetExtensions(mem,ci->extensions))
	{
		BIO_get_mem_ptr(mem, &bptr);
		UINT len = bptr->length;
		char * pbuf = new char[len+1];
		memset(pbuf, 0, len+1);
		memcpy(pbuf, bptr->data, len);
		if(sizeof(certinfo.V3EXT) < len)
			strncpy(certinfo.V3EXT, pbuf, sizeof(certinfo.V3EXT));
		else
			strncpy(certinfo.V3EXT, pbuf, len);
		delete [] pbuf;
	}
	else
		strncpy(certinfo.V3EXT, MiniCT_1532, uIniTextLen); //取得扩展信息失败
	//取得证书摘要
	if(len!=0)
	{
		strcpy(certinfo.THUMB,"sha1");
		unsigned char md_value[MAX_MD_SIZE]="";
		UINT md_len=0;

		if(!CEvp::Digest("sha1",(char *)pCert,certLen,md_value,&md_len,msg))
			strncpy(certinfo.THUMBPRINT, MiniCT_1533, uIniTextLen); //缩微图失败
		else
		{
			char buf[MAX_MD_SIZE*2+1]="";
			for(unsigned i=0;i<md_len;i++)
			{
				sprintf((char *)&buf[i*2],"%02X",md_value[i]);//02x标示1个16进制变为2个字符，空补零
			}
			strcpy(certinfo.THUMBPRINT,buf);
		}
	}

err:
	if(x509 && bFreeX509)
	{
		X509_free(x509);	//////////////////
	}
	if (mem != NULL)
		BIO_free_all(mem);
	if(pkey)
		EVP_PKEY_free(pkey); /////////////////

	return bRet;
}

void CCertInfoPage::TranslateCT()	//繙譯諸如樹型控件,列錶控件等內容
{
	SetDlgItemText(IDC_CERTINFO_READ, MiniCT_10901);
}