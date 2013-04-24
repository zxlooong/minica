// CertDbPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CertDbPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCertDbPage property page

IMPLEMENT_DYNCREATE(CCertDbPage, CPropertyPage)

CCertDbPage::CCertDbPage() : CPropertyPage(CCertDbPage::IDD)
{
	//{{AFX_DATA_INIT(CCertDbPage)
	//}}AFX_DATA_INIT
	m_IniPathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniCA.ini";
}

CCertDbPage::~CCertDbPage()
{
}

void CCertDbPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCertDbPage)
	DDX_Control(pDX, IDC_DB_SAVECDB, m_Bsavecdb);
	DDX_Control(pDX, IDC_DB_RESERTDB, m_Bresertdb);
	DDX_Control(pDX, IDC_TREE_SETUP, m_TreeSetup);
//	DDX_Control(pDX, IDC_EDIT_SAVE, m_CertSave);
	DDX_Control(pDX, IDC_DB_SELECT, m_Bselect);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCertDbPage, CPropertyPage)
//{{AFX_MSG_MAP(CCertDbPage)
	ON_BN_CLICKED(IDC_DB_RESERTDB, OnBResertdb)
	ON_BN_CLICKED(IDC_DB_SAVECDB, OnBSavecdb)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_SETUP, OnRclickTreeSetup)
	ON_BN_CLICKED(IDC_DB_SELECT, OnBSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCertDbPage message handlers

BOOL CCertDbPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
	m_Bsavecdb.SetIcon(IDI_ICON11);
	m_Bsavecdb.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_Bresertdb.SetIcon(IDI_ICON12);
	m_Bresertdb.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	// TODO: Add extra initialization here

	CImageList * pImgList = ((CMiniCaApp *)AfxGetApp())->GetImgList();
	m_TreeSetup.SetImageList(pImgList,TVSIL_NORMAL);//9 - 15


	//24��ÿ��ͼƬ�Ŀ��Ϊ24
	//24��ÿ��ͼƬ�ĸ߶�Ϊ24
	//TRUE  ��ͼ��͸����ʽ��ʾ
	//ILC_COLOR24,  ��ɫ��Ϊ24λ��������ѡ��������й�API)
	//16:    ����ʱ����Ϊ3��ͼƬ
	//1:    ͼƬ����������ǰ����ʱ���Զ����ݣ�ÿ������1��ͼƬ������

/*	m_DbList.SetImageList(pImgList, LVSIL_SMALL);

	m_DbList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

	m_DbList.InsertColumn(0, CMiniCaApp::NormalCode("֤�����"), LVCFMT_LEFT, 130);	
	m_DbList.InsertColumn(1, CMiniCaApp::NormalCode("���"), LVCFMT_LEFT, 40);	
	m_DbList.InsertColumn(2, CMiniCaApp::NormalCode("��Կ"), LVCFMT_LEFT, 40);	
	m_DbList.InsertColumn(3, CMiniCaApp::NormalCode("�û���Ϣ"), LVCFMT_LEFT, 135);	
	m_DbList.InsertColumn(4, CMiniCaApp::NormalCode("��ʼʱ��"), LVCFMT_LEFT, 70);	
	m_DbList.InsertColumn(5, CMiniCaApp::NormalCode("��Ч��"), LVCFMT_LEFT, 50);	
*/

//	m_TreeSetup.m_HtreeList.RemoveAll();//����б�

	m_hItem[0] = m_TreeSetup.InsertItem(MiniCT_1800, 0, 0); //֤��� m_hItem[0]
	m_TreeSetup.SetItemImage(m_hItem[0],0,0);
	m_TreeSetup.SetItemData(m_hItem[0],0);//��Ϊ0
	HTREEITEM hItemSecond = TVI_ROOT;//����
	HTREEITEM hItemThird = TVI_ROOT;//����
	HTREEITEM hItemFour = TVI_ROOT;//�ļ�


	m_hItem[1] = m_TreeSetup.InsertItem(MiniCT_1801, 0, 0, m_hItem[0]); //��֤��(Highest) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[1],0,0);
	m_TreeSetup.SetItemData(m_hItem[1],1);//��֤��Ϊ1

	m_hItem[2] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[1]); //��֤�鹫Կ hItemThird
	m_TreeSetup.SetItemImage(m_hItem[2],0,0);
	m_TreeSetup.SetItemData(m_hItem[2],10);//��֤�鹫Կ10

	m_hItem[3] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[2]); //��Կ�ļ� hItemFour
	m_TreeSetup.SetItemImage(m_hItem[3],3,3);
	m_TreeSetup.SetItemData(m_hItem[3],100);//��֤�鹫Կ�ļ�Ϊ100
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[3]);

	m_hItem[4] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[1]); //��֤��˽Կ hItemThird
	m_TreeSetup.SetItemImage(m_hItem[4],0,0); 
	m_TreeSetup.SetItemData(m_hItem[4],11);//��֤��˽ԿΪ11

	m_hItem[5] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[4]); //˽Կ�ļ� hItemFour
	m_TreeSetup.SetItemImage(m_hItem[5],3,3);
	m_TreeSetup.SetItemData(m_hItem[5],110);//��֤��˽Կ�ļ�Ϊ110
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[5]);

	m_hItem[6] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[4]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[6],5,5);
	m_TreeSetup.SetItemData(m_hItem[6],111);//��֤��˽Կ��Կ111

	m_hItem[7] = m_TreeSetup.InsertItem(MiniCT_1806, 0, 0,m_hItem[0]); //��֤��(Higher) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[7],0,0);
	m_TreeSetup.SetItemData(m_hItem[7],2);//������֤��Ϊ2

	m_hItem[8] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[7]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[8],0,0);
	m_TreeSetup.SetItemData(m_hItem[8],20);//��������Կ20

	m_hItem[9] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[8]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[9],3,3);
	m_TreeSetup.SetItemData(m_hItem[9],200);//��Կ�ļ�200
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[9]);

	m_hItem[10] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[7]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[10],0,0);
	m_TreeSetup.SetItemData(m_hItem[10],21);//������˽ԿΪ21

	m_hItem[11] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[10]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[11],3,3);
	m_TreeSetup.SetItemData(m_hItem[11],210);//˽Կ�ļ�Ϊ210
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[11]);

	m_hItem[12] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[10]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[12],5,5);
	m_TreeSetup.SetItemData(m_hItem[12],211);//˽Կ��Կ211

	m_hItem[13] = m_TreeSetup.InsertItem(MiniCT_1807, 0, 0,m_hItem[0]); //��֤��(High) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[13],0,0);
	m_TreeSetup.SetItemData(m_hItem[13],3);//�ͻ���֤��Ϊ3

	m_hItem[14] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[13]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[14],0,0);
	m_TreeSetup.SetItemData(m_hItem[14],30);//�ͻ��˹�ԿΪ30

	m_hItem[15] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[14]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[15],3,3);
	m_TreeSetup.SetItemData(m_hItem[15],300);//�ͻ��˹�Կ�ļ�Ϊ300
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[15]);

	m_hItem[16] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[13]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[16],0,0);
	m_TreeSetup.SetItemData(m_hItem[16],31);//�ͻ���˽ԿΪ31

	m_hItem[17] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[16]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[17],3,3);
	m_TreeSetup.SetItemData(m_hItem[17],310);//˽Կ�ļ�Ϊ310
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[17]);

	m_hItem[18] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[16]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[18],5,5);
	m_TreeSetup.SetItemData(m_hItem[18],311);//˽Կ��ԿΪ311

	m_hItem[19] = m_TreeSetup.InsertItem(MiniCT_1808, 0, 0,m_hItem[0]); //��֤��(Low) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[19],0,0);
	m_TreeSetup.SetItemData(m_hItem[19],4);//����֤��4

	m_hItem[20] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[19]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[20],0,0);
	m_TreeSetup.SetItemData(m_hItem[20],40);//��ԿΪ40

	m_hItem[21] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[20]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[21],3,3);
	m_TreeSetup.SetItemData(m_hItem[21],400);//��Կ�ļ�400
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[21]);

	m_hItem[22] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[19]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[22],0,0);
	m_TreeSetup.SetItemData(m_hItem[22],41);//˽ԿΪ41

	m_hItem[23] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[22]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[23],3,3);
	m_TreeSetup.SetItemData(m_hItem[23],410);//˽Կ�ļ�Ϊ410
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[23]);

	m_hItem[24] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[22]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[24],5,5);
	m_TreeSetup.SetItemData(m_hItem[24],411);//˽Կ��ԿΪ411

	m_hItem[25] = m_TreeSetup.InsertItem(MiniCT_1809, 0, 0,m_hItem[0]); //hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[25],0,0);
	m_TreeSetup.SetItemData(m_hItem[25],5);//ǩ��Ϊ5

	m_hItem[26] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[25]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[26],0,0);
	m_TreeSetup.SetItemData(m_hItem[26],50);//��֤��Կ50

	m_hItem[27] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[26]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[27],3,3);
	m_TreeSetup.SetItemData(m_hItem[27],500);//��Կ�ļ�Ϊ500
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[27]);

	m_hItem[28] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[25]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[28],0,0);
	m_TreeSetup.SetItemData(m_hItem[28],51);//ǩ��˽ԿΪ51

	m_hItem[29] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[28]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[29],3,3);
	m_TreeSetup.SetItemData(m_hItem[29],510);//˽Կ�ļ�510
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[29]);

	m_hItem[30] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[28]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[30],5,5);
	m_TreeSetup.SetItemData(m_hItem[30],511);//˽Կ��Կ511



//	hItemFour = m_TreeSetup.InsertItem(_T("Your First Name (Example of edit box)"), 10, 10);
 //  m_TreeSetup.AddEditBox(hItemFour, RUNTIME_CLASS(CDemoEdit));


	m_TreeSetup.EnsureVisible(m_hItem[0]);

//	m_TreeSetup.Expand(m_TreeSetup.GetRootItem(),TVE_EXPAND);
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
		m_toolTip.AddTool(GetDlgItem(IDC_TREE_SETUP), "��֤���\r�����������ͬ����ĸ�֤��");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RESERTDB), "����֤���\r�������ɻָ�");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_SAVECDB), "����֤���\r�������������Ч");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_SELECT), "ѡ��ȫ��֤��洢·��");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SAVE), "ȫ��֤��洢·��\rĬ������µ�֤��洢·��");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO1), "ȫ��֤��������ʽ\r����:hpxs.Cer");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO2), "ȫ��֤��������ʽ\r����:hpxs100.Cer");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO3), "ȫ��֤��������ʽ\r����:100hpxs.Cer");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO4), "ȫ��֤��������ʽ\r����:100.Cer");

		// TODO: Use one of the following forms to add controls:
	}

	((CButton*)GetDlgItem(IDC_DB_RADIO2))->SetCheck(true);

	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_DB_RADIO1)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_DB_RADIO2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_DB_RADIO3)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_DB_RADIO4)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT_SAVE)->m_hWnd,TRUE);
	}

	CXPStyleButtonST::SetAllThemeHelper(this, 
		((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());

	m_Bselect.SetIcon(IDI_ICON12);
	m_Bselect.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	
	CString strRoot = ((CMiniCaApp *)AfxGetApp())->GetAppPath();
	CString strPath;
	GetPrivateProfileString("PATH", "Cert","",strPath.GetBuffer(256),255, m_IniPathName);
	strPath.ReleaseBuffer();//�� GetBuffer �� ReleaseBuffer ֮�������Χ��һ������ʹ����Ҫ�������������� CString ������κη���
	if(strPath.IsEmpty())
		SetCaPath(strRoot);
	else
		SetCaPath(strPath);



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/*void CCertDbPage::OnBSave() //д��ע���
{
	// TODO: Add your control notification handler code here
	CString strRegCode,strRootCert,strRootP12,strRootPwd,strServerP12,strServerPwd,strClientP12,strClientPwd;
	
	GetDlgItemText(IDC_EDIT2,strRootP12);
	GetDlgItemText(IDC_EDIT5,strRootPwd);
	
	GetDlgItemText(IDC_EDIT3,strServerP12);
	GetDlgItemText(IDC_EDIT6,strServerPwd);
	
	GetDlgItemText(IDC_EDIT4,strClientP12);
	GetDlgItemText(IDC_EDIT7,strClientPwd);
	
	GetDlgItemText(IDC_EDIT_REGCODE,strRegCode);
	
	CString RegMiniCA,RegRootCert,RegRootP12,RegRootPwd,RegServerP12,RegServerPwd,RegClientP12,
		RegClientPwd;//ע�����
	
	RegMiniCA.LoadString(IDS_REG_KEY);
	RegRootCert.LoadString(IDS_ROOT_CERT);
	RegRootP12.LoadString(IDS_ROOT_P12);
	RegRootPwd.LoadString(IDS_ROOT_PWD);
	RegServerP12.LoadString(IDS_SERVER_P12);
	RegServerPwd.LoadString(IDS_SERVER_PWD);
	RegClientP12.LoadString(IDS_CLIENT_P12);
	RegClientPwd.LoadString(IDS_CLIENT_PWD);
	
	HKEY hKey = NULL; 
	
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,RegMiniCA, 0, KEY_SET_VALUE, &hKey)!=ERROR_SUCCESS) //�򿪴���
	{
		return;
	}
	
	//���ܸ���Ϣ ע������Ϊpwd
	char encInfo[512]={0};
	UINT encLen=0;
	char outMsg[100]={0};
	
	
	if(strRootP12.IsEmpty())//���Ϊ��,�򲻽��м���,ֱ��ɾ��Reg
	{
		RegDeleteValue(hKey,RegRootP12);
	}
	else if(Crypt("idea-cbc",strRootP12.GetBuffer(0),strRootP12.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegRootP12,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("����RootP12������",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strRootPwd.IsEmpty())//���Ϊ��,�򲻽��м���,ֱ��ɾ��Reg
	{
		RegDeleteValue(hKey,RegRootPwd);
	}
	else if(Crypt("idea-cbc",strRootPwd.GetBuffer(0),strRootPwd.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegRootPwd,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("����RegRootPwd������",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strServerP12.IsEmpty())//���Ϊ��,�򲻽��м���,ֱ��ɾ��Reg
	{
		RegDeleteValue(hKey,RegServerP12);
	}
	else if(Crypt("idea-cbc",strServerP12.GetBuffer(0),strServerP12.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegServerP12,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("����RegServerP12������",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strServerPwd.IsEmpty())//���Ϊ��,�򲻽��м���,ֱ��ɾ��Reg
	{
		RegDeleteValue(hKey,RegServerPwd);
	}
	else if(Crypt("idea-cbc",strServerPwd.GetBuffer(0),strServerPwd.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if(RegSetValueEx(hKey,RegServerPwd,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("����RegServerPwd������",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strClientP12.IsEmpty())//���Ϊ��,�򲻽��м���,ֱ��ɾ��Reg
	{
		RegDeleteValue(hKey,RegClientP12);
	}
	else if(Crypt("idea-cbc",strClientP12.GetBuffer(0),strClientP12.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegClientP12,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("����RegClientP12������",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strClientPwd.IsEmpty())//���Ϊ��,�򲻽��м���,ֱ��ɾ��Reg
	{
		RegDeleteValue(hKey,RegClientPwd);
	}
	else if(Crypt("idea-cbc",strClientPwd.GetBuffer(0),strClientPwd.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegClientPwd,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("����RegClientPwd������",M_ERROR);
		}	
		memset(encInfo,0,512);
		encLen=0;
	}

	if(strRootCert.IsEmpty())//���Ϊ��,�򲻽��м���,ֱ��ɾ��Reg
	{
		RegDeleteValue(hKey,RegRootCert);
	}
	else if(Crypt("idea-cbc",strRootCert.GetBuffer(0),strRootCert.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if(RegSetValueEx(hKey,RegRootCert,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("����RootCert������",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	::RegCloseKey(hKey); 
	
}
*/

void CCertDbPage::OnRclickTreeSetup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point;
	GetCursorPos(&point); // ��ǰ�������
	CPoint PointInTree = point;
	m_TreeSetup.ScreenToClient( &PointInTree);
	
	UINT nFlag;
	HTREEITEM hItem = m_TreeSetup.HitTest( PointInTree, &nFlag);
	if(hItem == NULL)
		return;

//	m_TreeSetup.SelectItem(hItem);//����ѡ��

	
	DWORD dData = m_TreeSetup.GetItemData(hItem);
	CString strName = m_TreeSetup.GetItemText(hItem);
	if(dData < 100 || dData %100 !=0 || strName == MiniCT_1803)
		return;
	
	
	if(nFlag&TVHT_ONITEMLABEL) //�鿴�˵�
	{
		BCMenu menu;
		menu.LoadMenu(IDR_MENU_CERTDB);
		menu.LoadToolbar(IDR_MINICAMENU);
		
		CLanguage::TranslateMenu(&menu, MAKEINTRESOURCE(IDR_MENU_SETUP));
		
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup);
		UINT nSelection = pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|
			TPM_NONOTIFY|TPM_RETURNCMD,point.x, point.y, this, NULL);
		menu.DestroyMenu();
		//���ز˵�id
		CString m_CdbPath = ((CMiniCaApp *)AfxGetApp())->GetAppPath() + "\\CertDB.CDB";
		CFile file;
		char buf[100] = {0};
		if(!file.Open(m_CdbPath,CFile::modeRead))
			return;
		for(;;)
		{
			stuCertDB CertDB;
			int len = file.Read(&CertDB,sizeof(stuCertDB));
			if(len==0)
				break;
			if(CertDB._uID != dData)
				continue;
			{
				if(nSelection == ID_MENUITEM_CERTVIEW)
				{
					((CMiniMainDlg *)GetParent())->ViewCertInfo(CertDB._chINFO,CertDB._uLENGTH,0);
				}
				else if(nSelection == ID_MENUITEM_CERTIMPORT)
				{
					CMiniCaApp::AddRootToStore(CertDB._chINFO, CertDB._uLENGTH); //����der��ʽ��֤��
				}
			}
			break;
		}
		file.Close();
	}
	*pResult = 0;
}

void CCertDbPage::OnBSavecdb()
{
	m_TreeSetup.Savecdb();
}

void CCertDbPage::OnBResertdb() //֤��⻹ԭ
{
	// TODO: Add your control notification handler code here
	UINT uCrc = 0;
	CString strCrc;
	if(::MessageBox(this->m_hWnd, MiniCT_1810, MiniCT_0010,MB_ICONQUESTION | MB_YESNO)!=IDYES)
		return;
	/*�õ�����*/
	HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_CERTDB_INI),"INI");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert=LoadResource(NULL,hRsrc);
	LPSTR lpCert=(LPSTR)LockResource(hgCert);

	CString m_IniPathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\CertDB.CDB";
	CFile file;
	file.Open(m_IniPathName,CFile::modeCreate|CFile::modeWrite);	//���ļ�
	{
		file.Write(lpCert,lenCert);
	}
	file.Close();
	m_TreeSetup.LoadCdb();

}

void CCertDbPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)GetParent())->AddMsg(MiniCT_0007, info, type);
}

BOOL CCertDbPage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

UINT CCertDbPage::GetNameType()
{
	UINT uSelect ;
	if(((CButton*)GetDlgItem(IDC_DB_RADIO1))->GetCheck())
		uSelect = 0;
	else if(((CButton*)GetDlgItem(IDC_DB_RADIO2))->GetCheck())
		uSelect = 1;
	else if(((CButton*)GetDlgItem(IDC_DB_RADIO3))->GetCheck())
		uSelect = 2;
	else if(((CButton*)GetDlgItem(IDC_DB_RADIO4))->GetCheck())
		uSelect = 3;
	return uSelect;
}

void CCertDbPage::OnBSelect() 
{
	// TODO: Add your control notification handler code here
	LPITEMIDLIST pidlRoot = NULL; 
	LPMALLOC pMalloc;   
	if(NOERROR == SHGetSpecialFolderLocation(m_hWnd,CSIDL_DRIVES ,&pidlRoot))	//leak
	{
		BROWSEINFO bi;   //���봫��Ĳ���,�����������ṹ�Ĳ����ĳ�ʼ�� 
		CString strDisplayName;// = CMiniCaApp::NormalCode("ѡ���ļ���");;   //�����õ�,��ѡ��Ļ�ҳ��·��,�൱���ṩһ�������� 
		bi.hwndOwner = GetSafeHwnd();   //�õ�������Handleֵ 
		bi.pidlRoot=0;   //���������������������õ���. 
		bi.pszDisplayName = strDisplayName.GetBuffer(MAX_PATH+1);   //�õ�������ָ��, 
		CString strTitle = MiniCT_1811;
		bi.lpszTitle = strTitle;  //���ñ��� 
		bi.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_DONTGOBELOWDOMAIN  ;   //���ñ�־ 
		bi.lpfn=NULL; 
		bi.lParam=0; 
		bi.iImage=0;   //���������һЩ�޹صĲ���������,�����������, 
		ITEMIDLIST * pidl;  
		pidl = SHBrowseForFolder(&bi);   //�򿪶Ի��� 
		if(!pidl)
			return;
		SHGetPathFromIDList(pidl,bi.pszDisplayName);	
		strDisplayName.ReleaseBuffer();   //�������GetBuffer()���Ӧ 
		SetCaPath(strDisplayName);
	
		if(SUCCEEDED( SHGetMalloc ( &pMalloc ))) // leak
		{ 
			pMalloc->Free(pidlRoot); 
			pMalloc->Release(); 
		} 
	}
}

void CCertDbPage::SetCaPath(CString strPath)
{

	//����Ŀ¼
	//�ṹ
	//MiniCA.exe attrib desktop.ini +h +s
	//	strPath + MiniCACert																
	//					+		 CaCert				//CA����֤����λ��			
	//                  +		WizardCert          //��֤��						

	SetDlgItemText(IDC_EDIT_SAVE, strPath);
	//�õ�֤������ѡ��

	WritePrivateProfileString("PATH", "Cert",strPath.GetBuffer(256), m_IniPathName);

//	SetPathInfo(strPath, "%SystemRoot%\\system32\\SHELL32.dll", "150", "MiniCA V2.0");  ����ϵͳͼ��

	TCHAR	szProgPath[MAX_PATH * 2];
	::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath)/sizeof(TCHAR));

	CString sMiniCA;
	sMiniCA.Format("%s", szProgPath);

	SetPathInfo(strPath, szProgPath, "12", MiniCT_1812);  //����minicaͼ��

	//���Ƚ��� strPath + MiniCACert
	DWORD dErrCode = 0;
	BOOL b = CreateDirectory(strPath + "\\MiniCACert", NULL);
	if(!b)
		dErrCode = GetLastError();

//	HTREEITEM hPathItem = m_PathTree.InsertItem(strPath + "\\MiniCACert",0,0);

//	SetPathInfo(strPath + "\\MiniCACert", "%SystemRoot%\\system32\\SHELL32.dll", "47", 
//		CMiniCaApp::NormalCode("MiniCA ֤������"));
	//�����ļ�������

	//���� strPath + MiniCACert + CaCert
	b = CreateDirectory(strPath + "\\MiniCACert\\CaCert", NULL);
	if(!b)
		dErrCode = GetLastError();

//	HTREEITEM hPathSecond = m_PathTree.InsertItem(strPath + "\\MiniCACert\\CaCert",hPathItem,0);

	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCaCenterPage * pCa = (CCaCenterPage *)(pMain->GetPage("CCaCenterPage"));

	//strPath���ܴ���\\����c:\\ʱ��ʹ���,��ʱ��Ҫȥ�����
	if(strPath.Right(1) == "\\")
	{
		strPath.Delete(strPath.GetLength() -1);
	}

	if(pCa)
	{
		pCa->SetNamePath(strPath + "\\MiniCACert\\CaCert");
	}


	//���� strPath + MiniCACert + CaCert + MiniCACert
//	b = CreateDirectory(strPath + "\\MiniCACert\\CaCert\\Cert", NULL);
//	if(!b)
//		dErrCode = GetLastError();
//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\CaCert\\Cert",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\CaCert\\Cert");


	//���� strPath + MiniCACert + CaCert + Crl
//	b = CreateDirectory(strPath + "\\MiniCACert\\CaCert\\Crl", NULL);
//	if(!b)
///		dErrCode = GetLastError();
//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\CaCert\\Crl",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\CaCert\\Crl");


	//���� strPath + MiniCACert + WizardCert
	b = CreateDirectory(strPath + "\\MiniCACert\\WizardCert", NULL);
	if(!b)
		dErrCode = GetLastError();

//	hPathSecond = m_PathTree.InsertItem(strPath + "\\MiniCACert\\WizardCert",hPathItem,0);
	CCaCertWizardSheet * pWizard = (CCaCertWizardSheet *)(pMain->GetPage("CCaCertWizardSheet"));
	if(pWizard)
	{
		CCaCertInfoPage * pCaInfo = (CCaCertInfoPage *)(pWizard->GetPage("CCaCertInfoPage"));
		CCaCertManPage * pCaMan = (CCaCertManPage *)(pWizard->GetPage("CCaCertManPage"));
		
		if(pCaInfo) 
		{
			pCaInfo->SetNamePath(strPath + "\\MiniCACert\\WizardCert");
		}
		if(pCaMan) 
		{
			pCaMan->SetNamePath(strPath + "\\MiniCACert\\WizardCert");
		}
	}


	//���� strPath + MiniCACert + WizardCert + Cert
//	b = CreateDirectory(strPath + "\\MiniCACert\\WizardCert\\Cert", NULL);
//	if(!b)
//		dErrCode = GetLastError();

//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\WizardCert\\Cert",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\WizardCert\\Cert");


	//���� strPath + MiniCACert + WizardCert + Crl
//	b = CreateDirectory(strPath + "\\MiniCACert\\WizardCert\\Crl", NULL);
//	if(!b)
//		dErrCode = GetLastError();

//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\WizardCert\\Crl",hPathSecond,0);
	//���� strPath + MiniCACert + WizardCert + Man
//	b = CreateDirectory(strPath + "\\MiniCACert\\WizardCert\\Man", NULL);
//	if(!b)
//		dErrCode = GetLastError();

//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\WizardCert\\Man",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\WizardCert\\Man");
}

void CCertDbPage::SetPathInfo(CString strPath, CString strIconFile, CString strIconIndex, 
							 CString strInfoTip)		//�����ļ���ͼ��
{
	//Ŀ¼���� %SystemRoot%\system32\SHELL32.dll
	//	[.ShellClassInfo]
	//	IconFile=.\MiniCA.exe
	//	IconIndex=0

	//���˸�desktop.ini����������,��Ҫ����ǰҪ���õ�·������ S ����

	SetFileAttributes(strPath + "\\desktop.ini", FILE_ATTRIBUTE_NORMAL); 
	WritePrivateProfileString(".ShellClassInfo", "ConfirmFileOp", "0", strPath + "\\desktop.ini");
	WritePrivateProfileString(".ShellClassInfo", "IconFile", strIconFile, strPath + "\\desktop.ini");
	WritePrivateProfileString(".ShellClassInfo", "InfoTip", strInfoTip, strPath + "\\desktop.ini");
	WritePrivateProfileString(".ShellClassInfo", "IconIndex", strIconIndex, strPath + "\\desktop.ini");
	SetFileAttributes(strPath + "\\desktop.ini", FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN); 
	SetFileAttributes(strPath , FILE_ATTRIBUTE_SYSTEM);
}

void CCertDbPage::TranslateCT()
{
	//�������ؼ�

	m_TreeSetup.SetItemText(m_hItem[0], MiniCT_1800);

	m_TreeSetup.SetItemText(m_hItem[1], MiniCT_1801);
	m_TreeSetup.SetItemText(m_hItem[2], MiniCT_1802);
	m_TreeSetup.SetItemText(m_hItem[3], MiniCT_1803);
	m_TreeSetup.SetItemText(m_hItem[4], MiniCT_1804);
	m_TreeSetup.SetItemText(m_hItem[5], MiniCT_1805);

	m_TreeSetup.SetItemText(m_hItem[7], MiniCT_1806);
	m_TreeSetup.SetItemText(m_hItem[8], MiniCT_1802);
	m_TreeSetup.SetItemText(m_hItem[9], MiniCT_1803);
	m_TreeSetup.SetItemText(m_hItem[10], MiniCT_1804);
	m_TreeSetup.SetItemText(m_hItem[11], MiniCT_1805);

	m_TreeSetup.SetItemText(m_hItem[13], MiniCT_1807);
	m_TreeSetup.SetItemText(m_hItem[14], MiniCT_1802);
	m_TreeSetup.SetItemText(m_hItem[15], MiniCT_1803);
	m_TreeSetup.SetItemText(m_hItem[16], MiniCT_1804);
	m_TreeSetup.SetItemText(m_hItem[17], MiniCT_1805);

	m_TreeSetup.SetItemText(m_hItem[19], MiniCT_1808);
	m_TreeSetup.SetItemText(m_hItem[20], MiniCT_1802);
	m_TreeSetup.SetItemText(m_hItem[21], MiniCT_1803);
	m_TreeSetup.SetItemText(m_hItem[22], MiniCT_1804);
	m_TreeSetup.SetItemText(m_hItem[23], MiniCT_1805);

	m_TreeSetup.SetItemText(m_hItem[25], MiniCT_1809);
	m_TreeSetup.SetItemText(m_hItem[26], MiniCT_1802);
	m_TreeSetup.SetItemText(m_hItem[27], MiniCT_1803);
	m_TreeSetup.SetItemText(m_hItem[28], MiniCT_1804);
	m_TreeSetup.SetItemText(m_hItem[29], MiniCT_1805);

	m_TreeSetup.LoadCdb();



	//����static�ؼ�
	SetDlgItemText(IDC_STATIC_SETUP2, MiniCT_11001);
	SetDlgItemText(IDC_STATIC_SETUP3, MiniCT_11002);
	SetDlgItemText(IDC_STATIC_SETUP4, MiniCT_11003);
	SetDlgItemText(IDC_STATIC_SETUP5, MiniCT_11004);
	SetDlgItemText(IDC_DB_RADIO1, MiniCT_11005);
	SetDlgItemText(IDC_DB_RADIO2, MiniCT_11006);
	SetDlgItemText(IDC_DB_RADIO3, MiniCT_11007);
	SetDlgItemText(IDC_DB_RADIO4, MiniCT_11008);
	SetDlgItemText(IDC_DB_SAVECDB, MiniCT_11009);
	SetDlgItemText(IDC_DB_RESERTDB, MiniCT_11010);
	SetDlgItemText(IDC_DB_SELECT, MiniCT_11011);
}
