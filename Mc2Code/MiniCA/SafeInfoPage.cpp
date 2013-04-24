// SafeInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "SafeInfoPage.h"
#include "MiniMainDlg.h"
#include "Evp.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

//ϵͳĬ�϶�ջΪ1M,����1M�����鶨���ʧ��
const int dMaxMem = 1024*100;
const UINT uMsgLen = MAX_PATH + 100;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSafeInfoPage property page

IMPLEMENT_DYNCREATE(CSafeInfoPage, CPropertyPage)

CSafeInfoPage::CSafeInfoPage() : CPropertyPage(CSafeInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CSafeInfoPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPageCdb = NULL;
}

CSafeInfoPage::~CSafeInfoPage()
{
}

void CSafeInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSafeInfoPage)
//	DDX_Control(pDX, IDC_EDIT_PWD, m_XpEditPwd);
//	DDX_Control(pDX, IDC_EDIT_OUTFILE, m_XpEditOutFile);
//	DDX_Control(pDX, IDC_EDIT_INFILE, m_XpEditInfile);
	DDX_Control(pDX, IDC_BVSIGN, m_Bvsign);
	DDX_Control(pDX, IDC_BSIGN, m_Bsign);
	DDX_Control(pDX, IDC_BRSAKEY2, m_Brsakey2);
	DDX_Control(pDX, IDC_BRSAKEY, m_Brsakey);
	DDX_Control(pDX, IDC_BRSACERT2, m_Brsacert2);
	DDX_Control(pDX, IDC_BRSACERT, m_Brsacert);
	DDX_Control(pDX, IDC_BENC, m_Benc);
	DDX_Control(pDX, IDC_BDIGEST, m_Bdigest);
	DDX_Control(pDX, IDC_BDEC, m_Bdec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSafeInfoPage, CPropertyPage)
//{{AFX_MSG_MAP(CSafeInfoPage)
	ON_BN_CLICKED(IDC_BENC, OnBenc)
	ON_BN_CLICKED(IDC_BDEC, OnBdec)
	ON_BN_CLICKED(IDC_BDIGEST, OnBdigest)
	ON_BN_CLICKED(IDC_BSIGN, OnBsign)
	ON_BN_CLICKED(IDC_BVSIGN, OnBvsign)
	ON_BN_CLICKED(IDC_BRSACERT, OnBrsacert)
	ON_BN_CLICKED(IDC_BRSAKEY, OnBrsakey)
	ON_BN_CLICKED(IDC_BRSAKEY2, OnBrsakey2)
	ON_BN_CLICKED(IDC_BRSACERT2, OnBrsacert2)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSafeInfoPage message handlers
void CSafeInfoPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)AfxGetMainWnd())->AddMsg(MiniCT_0002, info, type);
}


BOOL CSafeInfoPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());

	m_Bvsign.SetIcon(IDI_ICON_DEC);
	m_Bvsign.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bsign.SetIcon(IDI_ICON_ENC);
	m_Bsign.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Brsakey2.SetIcon(IDI_ICON_ENC);
	m_Brsakey2.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Brsakey.SetIcon(IDI_ICON_DEC);
	m_Brsakey.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Brsacert2.SetIcon(IDI_ICON_DEC);
	m_Brsacert2.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Brsacert.SetIcon(IDI_ICON_ENC);
	m_Brsacert.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Benc.SetIcon(IDI_ICON_ENC);
	m_Benc.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bdigest.SetIcon(IDI_ICON_DEC);
	m_Bdigest.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bdec.SetIcon(IDI_ICON_DEC);
	m_Bdec.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);


	SetDlgItemText(IDC_COMBO_CRYPT,"idea-cbc");
	SetDlgItemText(IDC_COMBO_DIGEST,"sha1");
	SetDlgItemText(IDC_EDIT_PWD,"MiniCA");
	((CComboBox *)GetDlgItem(IDC_COMBO_CRYPT))->SetCurSel(16);
	((CComboBox *)GetDlgItem(IDC_COMBO_DIGEST))->SetCurSel(4);
	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_COMBO_CRYPT)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_COMBO_DIGEST)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_PWD)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_OUTFILE)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_INFILE)->m_hWnd,TRUE);
	}

	SetDlgItemText(IDC_EDIT_INFILE, "����ǰ�и�����������۵�ѧ�ߡ�"
		"һ�죬����ĳ��᳡�����ǽ��ϵ۾��Բ����ܴ��ڡ������ڸо�����֮����ʱ��"
		"����������ϵ���ս˵�����ϵۣ�������������飬����������"
		"�������Ⱥ����ǰ����ɱ�������Ǳ��������Ǵ��ڵ��ˣ���"
		"�����⾲���صȺ��˼����ӣ���Ȼ�ϵ�û������ɱ������"
		"����������ε�������˵�������Ƕ������ˣ��ϵ۸��������ڣ��� "
		"��֪��һλ���ˣ�ͷ�Ϲ���һ���̽�վ��������˵����������������ۺܸ�����"
		"���Ǹ���ѧ֮ʿ����ֻ��һ��ũ�帾�ˣ��������㷴����"
		"ֻ������ش������е�һ�����⣺���ŷ�Ү�ն���������"
		"�����������ľȶ���ʮ�ֿ��֣��Ҹ�������ʥ������Խ��Խ��ζ��"
		"�����г���Ү�ո��ҵİ�ο����Ϊ�ŷ�Ү�գ������������Ŀ��֡�"
		"���ʣ���������ʱ�����ϵ۸��������ڣ�Ү�ղ����ϵ۵Ķ��ӣ�ʥ��ȫ���ɿ���"
		"����һ�����ŷ�Ү�գ���ʧ��ʲô���� ������ѧ�����˺�һ�����"
		"ȫ���ž�����������Ҳ��ͬ��ũ�帾�˵�����"
		"��ѧ��Ҳ��̾�õ������߼����������ش𣺡�Ůʿ��������һ�����ʧҲû�С���"
		"ũ�帾������ѧ��˵������лл�������õĻش�"
		"�����л���һ�����⣺��������ʱ�򣬼����㷢�ֹ������ϵۣ�"
		"ʥ����ǧ����ȷ��Ү�չ�Ȼ����Ķ��ӣ�Ҳ�����ú͵����Ĵ��ڣ�"
		"�������ʣ�����ʧ��ʲô����ѧ��������ã��������Զԡ�");
	

	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	if(pMain)
	{
		CWinEvpSheet * pSheet = (CWinEvpSheet *)(pMain->GetPage("CWinEvpSheet"));
		if(pSheet)
			m_pPageCdb = (CSafeDataCdbPage*)pSheet->GetPage("CSafeDataCdbPage");
	}

	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
		m_toolTip.AddTool(GetDlgItem(IDC_COMBO_CRYPT), CMiniCaApp::NormalCode("����ʹ�õļ����㷨"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_PWD), CMiniCaApp::NormalCode("����ʹ�õ���Կ"));
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_INFILE), "ԭʼ��Ϣ");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_OUTFILE), "�������");
		m_toolTip.AddTool(GetDlgItem(IDC_BENC), CMiniCaApp::NormalCode("������Ϣ"));
		m_toolTip.AddTool(GetDlgItem(IDC_BDEC), CMiniCaApp::NormalCode("������Ϣ"));
		m_toolTip.AddTool(GetDlgItem(IDC_COMBO_DIGEST), "����ʹ�õ�ǩ���㷨");
		m_toolTip.AddTool(GetDlgItem(IDC_BDIGEST), "��ָ����Ϣ����ժҪ����");
		m_toolTip.AddTool(GetDlgItem(IDC_BSIGN), "��ָ����Ϣ��������ǩ������");
		m_toolTip.AddTool(GetDlgItem(IDC_BVSIGN), "��ָ����Ϣ��ǩ�����������֤");

		m_toolTip.AddTool(GetDlgItem(IDC_BRSACERT), "�ù�Կ�Զ�ָ����Ϣ���м���\r ��Կ���Ⱦ�������ʱ��");
		m_toolTip.AddTool(GetDlgItem(IDC_BRSAKEY), "��˽Կ�Զ�ָ����Ϣ���н���\r ��Կ���Ⱦ�������ʱ��");
		m_toolTip.AddTool(GetDlgItem(IDC_BRSAKEY2), "��˽Կ�Զ�ָ����Ϣ���м���\r ��Կ���Ⱦ�������ʱ��");
		m_toolTip.AddTool(GetDlgItem(IDC_BRSACERT2), "�ù�Կ�Զ�ָ����Ϣ���н���\r ��Կ���Ⱦ�������ʱ��");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSafeInfoPage::OnBenc() 
{
	// TODO: Add your control notification handler code here
	CString strIn;
	GetDlgItemText(IDC_EDIT_INFILE,strIn);
	UINT len = strIn.GetLength();
	if(len == 0)
	{
		AddMsg(MiniCT_0800,M_ERROR);		//MiniCT_0800 "������Ҫ������Ϣ"
		return;
	}
	unsigned char outbuf[dMaxMem] = "";
	CString cpname,pwd;
	char outMsg[uMsgLen] = "";
	UINT templen = dMaxMem;
	GetDlgItemText(IDC_COMBO_CRYPT,cpname);
	GetDlgItemText(IDC_EDIT_PWD,pwd);
	if(pwd.IsEmpty())
	{
		AddMsg(MiniCT_0801,M_ERROR);		//MiniCT_0801 "�����������Կ"
		return;
	}
	
	if(!CEvp::Crypt(cpname.GetBuffer(0),strIn.GetBuffer(0),len,(char *)outbuf,templen,
		pwd.GetBuffer(0),1,outMsg))
		AddMsg(outMsg,M_ERROR);
	else
	{
		CString out = CMiniCaApp::LmToHText(outbuf,templen);
		SetDlgItemText(IDC_EDIT_INFILE, out);
//		SetDlgItemText(IDC_EDIT_OUTFILE, strIn);
		Disply();
		AddMsg(MiniCT_0802);	//MiniCT_0802 "������Ϣ�ɹ�"
	}	
}

void CSafeInfoPage::OnBdec() 
{
	// TODO: Add your control notification handler code here
	CString strIn;
	GetDlgItemText(IDC_EDIT_INFILE,strIn);
	UINT len = strIn.GetLength();
	if(len == 0)
	{
		AddMsg(MiniCT_0803,M_ERROR);		//MiniCT_0803 "������Ҫ����Ϣ"
		return;
	}
	unsigned char outbuf[dMaxMem] = "";//�����㷨��Ŀ鳤�ȡ�
	CString cpname,pwd;
	char outMsg[uMsgLen] = "";
	UINT templen = dMaxMem;
	GetDlgItemText(IDC_COMBO_CRYPT,cpname);
	GetDlgItemText(IDC_EDIT_PWD,pwd);
	if(pwd.IsEmpty())
	{
		AddMsg(MiniCT_0804,M_ERROR);		//MiniCT_0804 "�����������Կ"
		return;
	}
	char strLm[2*dMaxMem] = {0};
	UINT uLm = CMiniCaApp::HTextToLm(strIn.GetBuffer(0),strLm);
	if(uLm == 0)
		return;
	if(!CEvp::Crypt(cpname.GetBuffer(0),strLm,uLm,(char *)outbuf,templen,
		pwd.GetBuffer(0),0,outMsg))
	{
		//		m_XpEditInfile.SetTextColor(RGB(128,0,0));
		//SetDlgItemText(IDC_EDIT_INFILE,(char *)outbuf);
		AddMsg(outMsg,M_ERROR);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_INFILE,(char *)outbuf);
//		SetDlgItemText(IDC_EDIT_OUTFILE, strIn);
		Disply();
		AddMsg(MiniCT_0805);	//MiniCT_0805 "������Ϣ�ɹ�"
	}
}

void CSafeInfoPage::OnBdigest() 
{
	// TODO: Add your control notification handler code here
	CString strIn;
	GetDlgItemText(IDC_EDIT_INFILE,strIn);
	if(strIn.IsEmpty())
	{
		AddMsg(MiniCT_0806,M_ERROR);//MiniCT_0806 "������ҪժҪ��Ϣ"
		return;
	}
	CString mdname;
	char outMsg[uMsgLen]="";
	unsigned char md_value[MAX_MD_SIZE]="";
	char buf[MAX_MD_SIZE*2] = "";
	unsigned int md_len;
	GetDlgItemText(IDC_COMBO_DIGEST,mdname);
	if(!CEvp::Digest(mdname.GetBuffer(0),strIn.GetBuffer(0),strlen(strIn),md_value,
		&md_len, outMsg))
	{
		AddMsg(outMsg,M_ERROR);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_OUTFILE,CMiniCaApp::LmToHText(md_value,md_len));
		SetDlgItemText(IDC_EDIT_INFILE, strIn);
		Disply(TRUE); 
		AddMsg(MiniCT_0807);	//MiniCT_0807 "��ϢժҪ�ɹ�"
	}	
}

void CSafeInfoPage::OnBsign() 
{
	// TODO: Add your control notification handler code here
	CString mdname,strIn;
	char outMsg[uMsgLen]="";
	char strOut[dMaxMem] = {0};
	GetDlgItemText(IDC_COMBO_DIGEST,mdname);
	GetDlgItemText(IDC_EDIT_INFILE,strIn);
	if(strIn.IsEmpty())
	{
		AddMsg(MiniCT_0808,M_ERROR);		//MiniCT_0808 "������Ҫǩ����Ϣ"
		return;
	}
	UINT outlen=0;

	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SIGN, FALSE, m_strKey,m_lenKey,m_p12Pwd))//�õ�˽Կ
	{
		if(!CEvp::Sign(m_strKey,m_lenKey,m_p12Pwd.GetBuffer(0),mdname.GetBuffer(0),
			strIn.GetBuffer(0),strlen(strIn),
			strOut,outlen,outMsg))
		{
			AddMsg(outMsg,M_ERROR);
		}
		else
		{
			SetDlgItemText(IDC_EDIT_OUTFILE,CMiniCaApp::LmToHText((UCHAR *)strOut,outlen));
			SetDlgItemText(IDC_EDIT_INFILE, strIn);
			Disply(TRUE);
			AddMsg(MiniCT_0809);	//MiniCT_0809 "��Ϣǩ���ɹ�"
		}
	}
	else
		AddMsg(MiniCT_0810,M_ERROR);	//MiniCT_0810
}

void CSafeInfoPage::OnBvsign() 
{
	// TODO: Add your control notification handler code here
	CString mdname,filein,fileout;
	char outMsg[uMsgLen]="";
	GetDlgItemText(IDC_COMBO_DIGEST,mdname);
	GetDlgItemText(IDC_EDIT_INFILE,filein);
	GetDlgItemText(IDC_EDIT_OUTFILE,fileout);
	if(filein.IsEmpty())
	{
		AddMsg(MiniCT_0811,M_ERROR);		//MiniCT_0811
		return;
	}
	
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SIGN, TRUE, m_strKey,m_lenKey,m_p12Pwd))//�õ���Կ
	{
		char strLm[dMaxMem] = {0};
		UINT uLm = CMiniCaApp::HTextToLm(fileout.GetBuffer(0),strLm);
		if(!CEvp::VerifySign(m_strKey,m_lenKey,m_p12Pwd.GetBuffer(0),
			mdname.GetBuffer(0),filein.GetBuffer(0),strlen(filein),
			strLm,outMsg))
		{
			AddMsg(outMsg,M_ERROR);
		}
		else
		{
			AddMsg(MiniCT_0812);//MiniCT_0812
			Disply(TRUE);
//			SetDlgItemText(IDC_EDIT_OUTFILE, fileout);
//			SetDlgItemText(IDC_EDIT_INFILE, filein);

		}
	}
	else
		AddMsg(MiniCT_0810,M_ERROR);	//MiniCT_0810 "ȡ����Կʧ��"
}

void CSafeInfoPage::OnBrsacert() 
{
	// TODO: Add your control notification handler code here
	CString cert,filein,fileout;
	char outMsg[uMsgLen]="";
	char strOut[dMaxMem] ={0};
	GetDlgItemText(IDC_EDIT_INFILE,filein);
	if(filein.IsEmpty())
	{
		AddMsg(MiniCT_0800,M_ERROR);		//MiniCT_0800 "������Ҫ������Ϣ"
		return;
	}
	DWORD len = dMaxMem;

	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, TRUE, m_strKey,m_lenKey,m_p12Pwd))//�õ���Կ
	{
		if(!CEvp::RSAPubEnc((BYTE *)m_strKey,m_lenKey,m_p12Pwd.GetBuffer(0),
			(BYTE *)filein.GetBuffer(0),strlen(filein),(BYTE *)strOut,len,outMsg))
		{
			AddMsg(outMsg,M_ERROR);
		}
		else
		{
			AddMsg(MiniCT_0813);		//MiniCT_0813 "��Կ������Ϣ�ɹ�"
//			m_XpEditInfile.SetTextColor(RGB(0,0,0));
			SetDlgItemText(IDC_EDIT_INFILE, CMiniCaApp::LmToHText((UCHAR *)strOut,len));
			Disply();
		}
		
	}
	else
		AddMsg(MiniCT_0810,M_ERROR);
	
}

void CSafeInfoPage::OnBrsakey() 
{
	// TODO: Add your control notification handler code here
	CString key,filein,fileout;
	char outMsg[uMsgLen]="";
	char strOut[dMaxMem] = {0};
	GetDlgItemText(IDC_EDIT_INFILE,filein);
	DWORD len = dMaxMem;
	if(filein.IsEmpty())
	{
		AddMsg(MiniCT_0814,M_ERROR);		
		return;
	}
	
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PrivEnc, FALSE, m_strKey,m_lenKey,m_p12Pwd))//�õ�˽Կ
	{
		char strLm[dMaxMem] = {0};
		UINT uLm = CMiniCaApp::HTextToLm(filein.GetBuffer(0),strLm);
		if(!CEvp::RSAPrivDec((BYTE *)m_strKey,m_lenKey,m_p12Pwd.GetBuffer(0),(BYTE *)strLm, 
			uLm, (BYTE *)strOut,len ,outMsg))
		{
//			m_XpEditInfile.SetTextColor(RGB(128,0,0));
			AddMsg(outMsg,M_ERROR);
		}
		else
		{
//			m_XpEditInfile.SetTextColor(RGB(0,128,0));
			SetDlgItemText(IDC_EDIT_INFILE,(char *)strOut);
			Disply();
			AddMsg(MiniCT_0815);
		}
		
	}
	else
		AddMsg(MiniCT_0810,M_ERROR);	
}

void CSafeInfoPage::OnBrsakey2() 
{
	// TODO: Add your control notification handler code here
	CString key,filein,fileout;
	char outMsg[uMsgLen]="";
	GetDlgItemText(IDC_EDIT_INFILE,filein);
	if(filein.IsEmpty())
	{
		AddMsg(MiniCT_0800,M_ERROR);		
		return;
	}		
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PrivEnc, FALSE, m_strKey,m_lenKey,m_p12Pwd))//�õ�˽Կ
	{
		char strLm[dMaxMem] = {0};
		DWORD len = dMaxMem;
		if(!CEvp::RSAPrivEnc((BYTE *)m_strKey,m_lenKey,m_p12Pwd.GetBuffer(0),(BYTE *)filein.GetBuffer(0), 
			filein.GetLength(),(BYTE *)strLm,len,outMsg))
		{
			AddMsg(outMsg,M_ERROR);
		}
		else
		{
//			m_XpEditInfile.SetTextColor(RGB(0,0,0));
			SetDlgItemText(IDC_EDIT_INFILE, CMiniCaApp::LmToHText((UCHAR *)strLm,len));
			Disply();
			AddMsg(MiniCT_0816);
		}	
	}
	else
		AddMsg(MiniCT_0810,M_ERROR);	
}
	
void CSafeInfoPage::OnBrsacert2() 
{
	// TODO: Add your control notification handler code here
	CString key,filein,fileout;
	char outMsg[uMsgLen]="";
	char strOut[dMaxMem] = {0};
	GetDlgItemText(IDC_EDIT_INFILE,filein);
	DWORD len = dMaxMem;
	if(filein.IsEmpty())
	{
		AddMsg(MiniCT_0814,M_ERROR);		
		return;
	}
	
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, TRUE, m_strKey,m_lenKey,m_p12Pwd))//�õ���Կ
	{
		char strLm[dMaxMem] = {0};
		DWORD uLm = CMiniCaApp::HTextToLm(filein.GetBuffer(0),strLm);
		if(!CEvp::RSAPubDec((BYTE *)m_strKey,m_lenKey,m_p12Pwd.GetBuffer(0),(BYTE *)strLm, 
			uLm,(BYTE *)strOut,len,outMsg))
		{
//			m_XpEditInfile.SetTextColor(RGB(128,0,0));
			AddMsg(outMsg,M_ERROR);
		}
		else
		{
//			m_XpEditInfile.SetTextColor(RGB(0,128,0));
			SetDlgItemText(IDC_EDIT_INFILE,(char *)strOut);
			Disply();
			AddMsg(MiniCT_0817);
		}
		
	}
	else
		AddMsg(MiniCT_0810,M_ERROR);	
}

void CSafeInfoPage::Disply(BOOL bDisply)
{
	CWnd * pInWnd = GetDlgItem(IDC_EDIT_INFILE);
	CWnd * pOutWnd = GetDlgItem(IDC_EDIT_OUTFILE);
	CRect Rect;

	if(bDisply)
	{
		GetDlgItem(IDC_STATIC_SHORT)->GetWindowRect(Rect);
		pOutWnd->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_STATIC_LONG)->GetWindowRect(Rect);
		pOutWnd->ShowWindow(SW_HIDE);
	}

	pInWnd->SetWindowPos(NULL, 0, 0, Rect.Width(), Rect.Height(),
		SWP_NOMOVE|SWP_NOZORDER | SWP_NOACTIVATE|SWP_SHOWWINDOW);

}

void CSafeInfoPage::TranslateCT()
{
	SetDlgItemText(IDC_STATIC_EVP5 ,MiniCT_11601);
	SetDlgItemText(IDC_STATIC_EVP6 ,MiniCT_11602);
	SetDlgItemText(IDC_STATIC_EVP7 ,MiniCT_11603);
	SetDlgItemText(IDC_STATIC_EVP8 ,MiniCT_11604);
	SetDlgItemText(IDC_STATIC_EVP9 ,MiniCT_11605);
	SetDlgItemText(IDC_BENC ,MiniCT_11606);
	SetDlgItemText(IDC_BDEC ,MiniCT_11607);
	SetDlgItemText(IDC_BDIGEST ,MiniCT_11608);
	SetDlgItemText(IDC_BSIGN ,MiniCT_11609);
	SetDlgItemText(IDC_BVSIGN ,MiniCT_11610);
	SetDlgItemText(IDC_BRSACERT ,MiniCT_11611);
	SetDlgItemText(IDC_BRSAKEY ,MiniCT_11612);
	SetDlgItemText(IDC_BRSAKEY2 ,MiniCT_11613);
	SetDlgItemText(IDC_BRSACERT2 ,MiniCT_11614);
}

BOOL CSafeInfoPage::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}
