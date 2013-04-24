// CaCertTypePage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CaCertTypePage.h"
#include ".\GenericClass\Language.h"
#include "minict.h"
#include "MiniMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCertTypePage property page
char * CERTUSAGE[] =
{
	"Digital Signature",
	"Non Repudiation",
	"Key Encipherment",
	"Data Encipherment",
	"Key Agreement",
	"Key CertSign",
	"Crl Sign",
	"Encipher Only",
	"Decipher Only",
	"1.3.6.1.4.1.311.10.3.1",
	"1.3.6.1.4.1.311.10.3.10",
	"1.3.6.1.4.1.311.10.3.11",
	"1.3.6.1.4.1.311.10.6.2",
	"1.3.6.1.4.1.311.10.3.13",
	"1.3.6.1.4.1.311.10.3.2",
	"1.3.6.1.4.1.311.10.3.4",
	"1.3.6.1.4.1.311.10.3.4.1",
	"1.3.6.1.4.1.311.10.3.9",
	"1.3.6.1.4.1.311.10.5.1",
	"1.3.6.1.4.1.311.10.6.1",
	"1.3.6.1.4.1.311.10.3.12",
	"1.3.6.1.4.1.311.20.2.1",
	"1.3.6.1.4.1.311.20.2.2",
	"1.3.6.1.4.1.311.21.5",
	"1.3.6.1.4.1.311.10.3.5",
	"1.3.6.1.4.1.311.21.6",
	"1.3.6.1.5.5.7.3.3",
	"1.3.6.1.5.5.7.3.8",
	"1.3.6.1.5.5.7.3.2",
	"1.3.6.1.5.5.7.3.4",
	"1.3.6.1.5.5.7.3.1",
	"1.3.6.1.5.5.7.3.7",
	"1.3.6.1.5.5.8.2.2",
	"1.3.6.1.4.1.311.10.3.7",
	"1.3.6.1.5.5.7.3.5",
	"1.3.6.1.5.5.7.3.6",
	"1.3.6.1.4.1.311.21.19",
	"1.3.6.1.4.1.311.10.3.8",
	"1.3.6.1.4.1.311.10.3.6",
	"2.5.29.32.0",
	NULL
};


IMPLEMENT_DYNCREATE(CCaCertTypePage, CPropertyPage)

CCaCertTypePage::CCaCertTypePage() : CPropertyPage(CCaCertTypePage::IDD)
{
	//{{AFX_DATA_INIT(CCaCertTypePage)
	m_CertType = 1;
	//}}AFX_DATA_INIT
	m_bTestCheck = TRUE;
}

CCaCertTypePage::~CCaCertTypePage()
{
}

void CCaCertTypePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCertTypePage)
	DDX_Radio(pDX, IDC_RADIO01, m_CertType);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK2, m_bTestCheck);
}


BEGIN_MESSAGE_MAP(CCaCertTypePage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCertTypePage)
	ON_BN_CLICKED(IDC_RADIO01, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO02, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO03, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO04, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO05, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO06, OnRadio6)
	ON_BN_CLICKED(IDC_RADIO07, OnRadio7)
	ON_BN_CLICKED(IDC_RADIO08, OnRadio8)
	ON_BN_CLICKED(IDC_RADIO09, OnRadio9)
	ON_BN_CLICKED(IDC_RADIO10, OnRadio10)
	ON_BN_CLICKED(IDC_RADIO11, OnRadio11)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCertTypePage message handlers

BOOL CCaCertTypePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	for(int i = IDC_CHECK2; i <= IDC_CHECK10; i++)
	{
		((CButton*)GetDlgItem(i))->SetCheck(true);
	}
	// TODO: Add extra initialization here
	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_RADIO01)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO02)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO03)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO04)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO05)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO06)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO07)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO08)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO09)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO10)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO11)->m_hWnd,TRUE);
		
		ClassXP(GetDlgItem(IDC_CHECK2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK3)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK4)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK5)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK6)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK7)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK8)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK9)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK10)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK11)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK12)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK13)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK14)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK15)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK16)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK17)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK18)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK19)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK20)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK21)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK22)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK23)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK24)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK25)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK26)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK27)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK28)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK29)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK30)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK31)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK32)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK33)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK34)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK35)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK36)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK37)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK38)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK39)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK40)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CHECK41)->m_hWnd,TRUE);
	}

	//	m_CertType = 2;
	OnRadio2();
	return TRUE;
	// return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCaCertTypePage::GetCertTypeInfo(CStaticTreeCtrl * p_WizardTree,
										 HTREENODE & hTypeType, 
										 HTREENODE &  hTypeKusage, 
										 HTREENODE &  hTypeEkusage)
{
	CString info,type,keyuse,ekeyuse;
 //  数证类型 ,单选按钮
	for(int i = IDC_RADIO11; i >= IDC_RADIO01; i--)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck())//选中
		{
			GetDlgItemText(i,type);
			m_CertType = i - IDC_RADIO01;
			break;
		}
	}

 //  密钥用法 ,复选按钮
	for(i = IDC_CHECK10; i >= IDC_CHECK2; i--)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck())//选中
		{
			CString str,temp;
			GetDlgItemText(i,temp);
			str.Format("%s-%s(%d)", temp, CERTUSAGE[i - IDC_CHECK2], i-IDC_CHECK2);
			p_WizardTree->InsertChild( hTypeKusage, _T(str) );
			keyuse += str;
		}
	}
	if(keyuse.IsEmpty())
	{
		p_WizardTree->InsertChild( hTypeKusage, _T(CMiniCaApp::NormalCode("未指定")));
	}

 //   增强密钥用法	
	for(i = IDC_CHECK41; i >= IDC_CHECK11; i--)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck())//选中
		{
			CString str,temp;
			GetDlgItemText(i,temp);
			str.Format("%s-%s(%d)", temp, CERTUSAGE[i - IDC_CHECK2], i-IDC_CHECK11);
			p_WizardTree->InsertChild( hTypeEkusage, _T(str) );
			ekeyuse += str;
		}
	}
	if(ekeyuse.IsEmpty())
	{
		p_WizardTree->InsertChild( hTypeEkusage, _T(CMiniCaApp::NormalCode("未指定")));
	}
	
	CString strType;
	strType.Format("%s(%d)",type,m_CertType);
	p_WizardTree->InsertChild( hTypeType, strType);

//	info.Format("\n密钥用法:\n%s增强型密钥用法:\n%s",,
//		keyuse,ekeyuse);
}

void CCaCertTypePage::GetCert(CString & kusage, CString & ekusage, int & type)
{
	CString strTemp,strkusage,strekusage;
	for(int i = IDC_CHECK2; i <= IDC_CHECK10; i++)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck())//选中
		{
			//得到静态数组对应的内容 i - IDC_CHECK2
			strkusage += CERTUSAGE[i - IDC_CHECK2];
			strkusage += ",";
		}
	}

	if(!strkusage.IsEmpty())
	{
		kusage = strkusage.Left(strkusage.GetLength() - 1);//去掉,
	}

	for(i = IDC_CHECK11; i <= IDC_CHECK41; i++)
	{
		if(((CButton*)GetDlgItem(i))->GetCheck())//选中
		{
			strekusage += CERTUSAGE[i - IDC_CHECK2];
			strekusage += ",";
		}
	}

	if(!strekusage.IsEmpty())
	{
		ekusage = strekusage.Left(strekusage.GetLength() - 1);//去掉,
	}

	type = m_CertType;

}


void CCaCertTypePage::OnRadio1() //根证书
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"1,5,6");
}

void CCaCertTypePage::OnRadio2() //自定证书
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"");	
}

void CCaCertTypePage::OnRadio3() //代码签名
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,26");
}

void CCaCertTypePage::OnRadio4() //计算机
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,4,28,30");
}

void CCaCertTypePage::OnRadio5() //WEB服务器
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,1,2,3,4,30");
}

void CCaCertTypePage::OnRadio6() //客户端
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,1,2,3,28");
}

void CCaCertTypePage::OnRadio7() //信任列表签名
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,9");
}

void CCaCertTypePage::OnRadio8() //时间戳
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,1,2,3,27");	
}

void CCaCertTypePage::OnRadio9() //IPSEC
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,1,2,3,32");	
}

void CCaCertTypePage::OnRadio10() //安全Email
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,1,2,3,29");	
}

void CCaCertTypePage::OnRadio11() //智能卡登陆
{
	// TODO: Add your control notification handler code here
	SetState(FALSE,"0,4,8,11,15,22");	
}

void CCaCertTypePage::SetState(BOOL bEnable,CString  strCheck)
{
	for(int i = IDC_CHECK2; i <= IDC_CHECK41; i++)
	{
	//	GetDlgItem(i)->EnableWindow(bEnable);
		if(!bEnable)
			((CButton*)GetDlgItem(i))->SetCheck(FALSE);
	}

	strCheck += ",";

	CString strL;
	for(;;)
	{
		int state=strCheck.Find(",");
		if(state==-1&&strCheck.GetLength())
			strL=strCheck;
		else if(strCheck.GetLength()==0)
			break;
		else strL=strCheck.Left(state);
		if(!strL.IsEmpty())
		{
			UINT index = atoi(strL.GetBuffer(0));
			strL.ReleaseBuffer();
			if(index >= 0 && index<= IDC_CHECK41 - IDC_CHECK2)
				((CButton*)GetDlgItem(index+IDC_CHECK2))->SetCheck(TRUE);
		}
		strCheck=strCheck.Right(strCheck.GetLength()-state-1);
		strCheck.TrimLeft();
	}

}

void CCaCertTypePage::TranslateCT()
{
	SetDlgItemText(IDC_STATIC_CERT , MiniCT_10801);
	SetDlgItemText(IDC_STATIC_KUSE , MiniCT_10802);
	SetDlgItemText(IDC_STATIC_EKUSE , MiniCT_10803);
	SetDlgItemText(IDC_RADIO01 , MiniCT_10804);
	SetDlgItemText(IDC_RADIO02 , MiniCT_10805);
	SetDlgItemText(IDC_RADIO03 , MiniCT_10806);
	SetDlgItemText(IDC_RADIO04 , MiniCT_10807);
	SetDlgItemText(IDC_RADIO05 , MiniCT_10808);
	SetDlgItemText(IDC_RADIO06 , MiniCT_10809);
	SetDlgItemText(IDC_RADIO07 , MiniCT_10810);
	SetDlgItemText(IDC_RADIO08 , MiniCT_10811);
	SetDlgItemText(IDC_RADIO09 , MiniCT_10812);
	SetDlgItemText(IDC_RADIO10 , MiniCT_10813);
	SetDlgItemText(IDC_RADIO11 , MiniCT_10814);
	SetDlgItemText(IDC_CHECK2 , MiniCT_10815);
	SetDlgItemText(IDC_CHECK3 , MiniCT_10816);
	SetDlgItemText(IDC_CHECK4 , MiniCT_10817);
	SetDlgItemText(IDC_CHECK5 , MiniCT_10818);
	SetDlgItemText(IDC_CHECK6 , MiniCT_10819);
	SetDlgItemText(IDC_CHECK7 , MiniCT_10820);
	SetDlgItemText(IDC_CHECK8 , MiniCT_10821);
	SetDlgItemText(IDC_CHECK9 , MiniCT_10822);
	SetDlgItemText(IDC_CHECK10 , MiniCT_10823);
	SetDlgItemText(IDC_CHECK11 , MiniCT_10824);
	SetDlgItemText(IDC_CHECK12 , MiniCT_10825);
	SetDlgItemText(IDC_CHECK13 , MiniCT_10826);
	SetDlgItemText(IDC_CHECK14 , MiniCT_10827);
	SetDlgItemText(IDC_CHECK15 , MiniCT_10828);
	SetDlgItemText(IDC_CHECK16 , MiniCT_10829);
	SetDlgItemText(IDC_CHECK17 , MiniCT_10830);
	SetDlgItemText(IDC_CHECK18 , MiniCT_10831);
	SetDlgItemText(IDC_CHECK19 , MiniCT_10832);
	SetDlgItemText(IDC_CHECK20 , MiniCT_10833);
	SetDlgItemText(IDC_CHECK21 , MiniCT_10834);
	SetDlgItemText(IDC_CHECK22 , MiniCT_10835);
	SetDlgItemText(IDC_CHECK23 , MiniCT_10836);
	SetDlgItemText(IDC_CHECK24 , MiniCT_10837);
	SetDlgItemText(IDC_CHECK25 , MiniCT_10838);
	SetDlgItemText(IDC_CHECK26 , MiniCT_10839);
	SetDlgItemText(IDC_CHECK27 , MiniCT_10840);
	SetDlgItemText(IDC_CHECK28 , MiniCT_10841);
	SetDlgItemText(IDC_CHECK29 , MiniCT_10842);
	SetDlgItemText(IDC_CHECK30 , MiniCT_10843);
	SetDlgItemText(IDC_CHECK31 , MiniCT_10844);
	SetDlgItemText(IDC_CHECK32 , MiniCT_10845);
	SetDlgItemText(IDC_CHECK33 , MiniCT_10846);
	SetDlgItemText(IDC_CHECK34 , MiniCT_10847);
	SetDlgItemText(IDC_CHECK35 , MiniCT_10848);
	SetDlgItemText(IDC_CHECK36 , MiniCT_10849);
	SetDlgItemText(IDC_CHECK37 , MiniCT_10850);
	SetDlgItemText(IDC_CHECK38 , MiniCT_10851);
	SetDlgItemText(IDC_CHECK39 , MiniCT_10852);
	SetDlgItemText(IDC_CHECK40 , MiniCT_10853);
	SetDlgItemText(IDC_CHECK41 , MiniCT_10854);
}
