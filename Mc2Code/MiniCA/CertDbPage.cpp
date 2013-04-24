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


	//24：每个图片的宽度为24
	//24：每个图片的高度为24
	//TRUE  该图以透明方式显示
	//ILC_COLOR24,  颜色数为24位（其他可选常量祥见有关API)
	//16:    创建时容量为3个图片
	//1:    图片数量超出当前容量时，自动扩容，每次扩容1个图片的容量

/*	m_DbList.SetImageList(pImgList, LVSIL_SMALL);

	m_DbList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

	m_DbList.InsertColumn(0, CMiniCaApp::NormalCode("证书分类"), LVCFMT_LEFT, 130);	
	m_DbList.InsertColumn(1, CMiniCaApp::NormalCode("序号"), LVCFMT_LEFT, 40);	
	m_DbList.InsertColumn(2, CMiniCaApp::NormalCode("密钥"), LVCFMT_LEFT, 40);	
	m_DbList.InsertColumn(3, CMiniCaApp::NormalCode("用户信息"), LVCFMT_LEFT, 135);	
	m_DbList.InsertColumn(4, CMiniCaApp::NormalCode("起始时间"), LVCFMT_LEFT, 70);	
	m_DbList.InsertColumn(5, CMiniCaApp::NormalCode("有效期"), LVCFMT_LEFT, 50);	
*/

//	m_TreeSetup.m_HtreeList.RemoveAll();//清除列表

	m_hItem[0] = m_TreeSetup.InsertItem(MiniCT_1800, 0, 0); //证书库 m_hItem[0]
	m_TreeSetup.SetItemImage(m_hItem[0],0,0);
	m_TreeSetup.SetItemData(m_hItem[0],0);//根为0
	HTREEITEM hItemSecond = TVI_ROOT;//二级
	HTREEITEM hItemThird = TVI_ROOT;//三级
	HTREEITEM hItemFour = TVI_ROOT;//四级


	m_hItem[1] = m_TreeSetup.InsertItem(MiniCT_1801, 0, 0, m_hItem[0]); //根证书(Highest) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[1],0,0);
	m_TreeSetup.SetItemData(m_hItem[1],1);//根证书为1

	m_hItem[2] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[1]); //根证书公钥 hItemThird
	m_TreeSetup.SetItemImage(m_hItem[2],0,0);
	m_TreeSetup.SetItemData(m_hItem[2],10);//根证书公钥10

	m_hItem[3] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[2]); //公钥文件 hItemFour
	m_TreeSetup.SetItemImage(m_hItem[3],3,3);
	m_TreeSetup.SetItemData(m_hItem[3],100);//根证书公钥文件为100
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[3]);

	m_hItem[4] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[1]); //根证书私钥 hItemThird
	m_TreeSetup.SetItemImage(m_hItem[4],0,0); 
	m_TreeSetup.SetItemData(m_hItem[4],11);//根证书私钥为11

	m_hItem[5] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[4]); //私钥文件 hItemFour
	m_TreeSetup.SetItemImage(m_hItem[5],3,3);
	m_TreeSetup.SetItemData(m_hItem[5],110);//根证书私钥文件为110
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[5]);

	m_hItem[6] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[4]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[6],5,5);
	m_TreeSetup.SetItemData(m_hItem[6],111);//根证书私钥密钥111

	m_hItem[7] = m_TreeSetup.InsertItem(MiniCT_1806, 0, 0,m_hItem[0]); //根证书(Higher) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[7],0,0);
	m_TreeSetup.SetItemData(m_hItem[7],2);//服务器证书为2

	m_hItem[8] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[7]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[8],0,0);
	m_TreeSetup.SetItemData(m_hItem[8],20);//服务器公钥20

	m_hItem[9] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[8]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[9],3,3);
	m_TreeSetup.SetItemData(m_hItem[9],200);//公钥文件200
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[9]);

	m_hItem[10] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[7]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[10],0,0);
	m_TreeSetup.SetItemData(m_hItem[10],21);//服务器私钥为21

	m_hItem[11] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[10]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[11],3,3);
	m_TreeSetup.SetItemData(m_hItem[11],210);//私钥文件为210
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[11]);

	m_hItem[12] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[10]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[12],5,5);
	m_TreeSetup.SetItemData(m_hItem[12],211);//私钥密钥211

	m_hItem[13] = m_TreeSetup.InsertItem(MiniCT_1807, 0, 0,m_hItem[0]); //根证书(High) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[13],0,0);
	m_TreeSetup.SetItemData(m_hItem[13],3);//客户端证书为3

	m_hItem[14] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[13]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[14],0,0);
	m_TreeSetup.SetItemData(m_hItem[14],30);//客户端公钥为30

	m_hItem[15] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[14]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[15],3,3);
	m_TreeSetup.SetItemData(m_hItem[15],300);//客户端公钥文件为300
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[15]);

	m_hItem[16] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[13]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[16],0,0);
	m_TreeSetup.SetItemData(m_hItem[16],31);//客户端私钥为31

	m_hItem[17] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[16]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[17],3,3);
	m_TreeSetup.SetItemData(m_hItem[17],310);//私钥文件为310
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[17]);

	m_hItem[18] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[16]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[18],5,5);
	m_TreeSetup.SetItemData(m_hItem[18],311);//私钥密钥为311

	m_hItem[19] = m_TreeSetup.InsertItem(MiniCT_1808, 0, 0,m_hItem[0]); //根证书(Low) hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[19],0,0);
	m_TreeSetup.SetItemData(m_hItem[19],4);//加密证书4

	m_hItem[20] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[19]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[20],0,0);
	m_TreeSetup.SetItemData(m_hItem[20],40);//公钥为40

	m_hItem[21] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[20]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[21],3,3);
	m_TreeSetup.SetItemData(m_hItem[21],400);//公钥文件400
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[21]);

	m_hItem[22] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[19]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[22],0,0);
	m_TreeSetup.SetItemData(m_hItem[22],41);//私钥为41

	m_hItem[23] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[22]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[23],3,3);
	m_TreeSetup.SetItemData(m_hItem[23],410);//私钥文件为410
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[23]);

	m_hItem[24] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[22]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[24],5,5);
	m_TreeSetup.SetItemData(m_hItem[24],411);//私钥密钥为411

	m_hItem[25] = m_TreeSetup.InsertItem(MiniCT_1809, 0, 0,m_hItem[0]); //hItemSecond
	m_TreeSetup.SetItemImage(m_hItem[25],0,0);
	m_TreeSetup.SetItemData(m_hItem[25],5);//签名为5

	m_hItem[26] = m_TreeSetup.InsertItem(MiniCT_1802, 0, 0,m_hItem[25]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[26],0,0);
	m_TreeSetup.SetItemData(m_hItem[26],50);//验证公钥50

	m_hItem[27] = m_TreeSetup.InsertItem(MiniCT_1803, 0, 0,m_hItem[26]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[27],3,3);
	m_TreeSetup.SetItemData(m_hItem[27],500);//公钥文件为500
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[27]);

	m_hItem[28] = m_TreeSetup.InsertItem(MiniCT_1804, 0, 0,m_hItem[25]); //hItemThird
	m_TreeSetup.SetItemImage(m_hItem[28],0,0);
	m_TreeSetup.SetItemData(m_hItem[28],51);//签名私钥为51

	m_hItem[29] = m_TreeSetup.InsertItem(MiniCT_1805, 0, 0,m_hItem[28]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[29],3,3);
	m_TreeSetup.SetItemData(m_hItem[29],510);//私钥文件510
	m_TreeSetup.m_HtreeList.AddTail(m_hItem[29]);

	m_hItem[30] = m_TreeSetup.InsertItem("********", 0, 0,m_hItem[28]); //hItemFour
	m_TreeSetup.SetItemImage(m_hItem[30],5,5);
	m_TreeSetup.SetItemData(m_hItem[30],511);//私钥密钥511



//	hItemFour = m_TreeSetup.InsertItem(_T("Your First Name (Example of edit box)"), 10, 10);
 //  m_TreeSetup.AddEditBox(hItemFour, RUNTIME_CLASS(CDemoEdit));


	m_TreeSetup.EnsureVisible(m_hItem[0]);

//	m_TreeSetup.Expand(m_TreeSetup.GetRootItem(),TVE_EXPAND);
	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_toolTip.Create(this);
		m_toolTip.AddTool(GetDlgItem(IDC_TREE_SETUP), "根证书库\r可设置五个不同级别的根证书");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RESERTDB), "重置证书库\r操作不可恢复");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_SAVECDB), "保存证书库\r保存后新配置生效");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_SELECT), "选择全局证书存储路径");
		m_toolTip.AddTool(GetDlgItem(IDC_EDIT_SAVE), "全局证书存储路径\r默认情况下的证书存储路径");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO1), "全局证书命名方式\r行如:hpxs.Cer");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO2), "全局证书命名方式\r行如:hpxs100.Cer");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO3), "全局证书命名方式\r行如:100hpxs.Cer");
		m_toolTip.AddTool(GetDlgItem(IDC_DB_RADIO4), "全局证书命名方式\r行如:100.Cer");

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
	strPath.ReleaseBuffer();//在 GetBuffer 和 ReleaseBuffer 之间这个范围，一定不能使用你要操作的这个缓冲的 CString 对象的任何方法
	if(strPath.IsEmpty())
		SetCaPath(strRoot);
	else
		SetCaPath(strPath);



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/*void CCertDbPage::OnBSave() //写入注册表
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
		RegClientPwd;//注册表项
	
	RegMiniCA.LoadString(IDS_REG_KEY);
	RegRootCert.LoadString(IDS_ROOT_CERT);
	RegRootP12.LoadString(IDS_ROOT_P12);
	RegRootPwd.LoadString(IDS_ROOT_PWD);
	RegServerP12.LoadString(IDS_SERVER_P12);
	RegServerPwd.LoadString(IDS_SERVER_PWD);
	RegClientP12.LoadString(IDS_CLIENT_P12);
	RegClientPwd.LoadString(IDS_CLIENT_PWD);
	
	HKEY hKey = NULL; 
	
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,RegMiniCA, 0, KEY_SET_VALUE, &hKey)!=ERROR_SUCCESS) //打开错误
	{
		return;
	}
	
	//加密各信息 注册码作为pwd
	char encInfo[512]={0};
	UINT encLen=0;
	char outMsg[100]={0};
	
	
	if(strRootP12.IsEmpty())//如果为空,则不进行加密,直接删除Reg
	{
		RegDeleteValue(hKey,RegRootP12);
	}
	else if(Crypt("idea-cbc",strRootP12.GetBuffer(0),strRootP12.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegRootP12,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("创建RootP12键错误",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strRootPwd.IsEmpty())//如果为空,则不进行加密,直接删除Reg
	{
		RegDeleteValue(hKey,RegRootPwd);
	}
	else if(Crypt("idea-cbc",strRootPwd.GetBuffer(0),strRootPwd.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegRootPwd,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("创建RegRootPwd键错误",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strServerP12.IsEmpty())//如果为空,则不进行加密,直接删除Reg
	{
		RegDeleteValue(hKey,RegServerP12);
	}
	else if(Crypt("idea-cbc",strServerP12.GetBuffer(0),strServerP12.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegServerP12,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("创建RegServerP12键错误",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strServerPwd.IsEmpty())//如果为空,则不进行加密,直接删除Reg
	{
		RegDeleteValue(hKey,RegServerPwd);
	}
	else if(Crypt("idea-cbc",strServerPwd.GetBuffer(0),strServerPwd.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if(RegSetValueEx(hKey,RegServerPwd,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("创建RegServerPwd键错误",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strClientP12.IsEmpty())//如果为空,则不进行加密,直接删除Reg
	{
		RegDeleteValue(hKey,RegClientP12);
	}
	else if(Crypt("idea-cbc",strClientP12.GetBuffer(0),strClientP12.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegClientP12,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("创建RegClientP12键错误",M_ERROR);
		}
		memset(encInfo,0,512);
		encLen=0;
	}
	
	if(strClientPwd.IsEmpty())//如果为空,则不进行加密,直接删除Reg
	{
		RegDeleteValue(hKey,RegClientPwd);
	}
	else if(Crypt("idea-cbc",strClientPwd.GetBuffer(0),strClientPwd.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if (RegSetValueEx(hKey,RegClientPwd,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("创建RegClientPwd键错误",M_ERROR);
		}	
		memset(encInfo,0,512);
		encLen=0;
	}

	if(strRootCert.IsEmpty())//如果为空,则不进行加密,直接删除Reg
	{
		RegDeleteValue(hKey,RegRootCert);
	}
	else if(Crypt("idea-cbc",strRootCert.GetBuffer(0),strRootCert.GetLength(),encInfo,encLen,
		strRegCode.GetBuffer(0),1,outMsg,NULL))
	{
		if(RegSetValueEx(hKey,RegRootCert,0,REG_BINARY,(UCHAR *)encInfo,encLen)!=ERROR_SUCCESS)
		{
			AddMsg("创建RootCert键错误",M_ERROR);
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
	GetCursorPos(&point); // 当前鼠标坐标
	CPoint PointInTree = point;
	m_TreeSetup.ScreenToClient( &PointInTree);
	
	UINT nFlag;
	HTREEITEM hItem = m_TreeSetup.HitTest( PointInTree, &nFlag);
	if(hItem == NULL)
		return;

//	m_TreeSetup.SelectItem(hItem);//设置选中

	
	DWORD dData = m_TreeSetup.GetItemData(hItem);
	CString strName = m_TreeSetup.GetItemText(hItem);
	if(dData < 100 || dData %100 !=0 || strName == MiniCT_1803)
		return;
	
	
	if(nFlag&TVHT_ONITEMLABEL) //查看菜单
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
		//返回菜单id
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
					CMiniCaApp::AddRootToStore(CertDB._chINFO, CertDB._uLENGTH); //加入der格式根证书
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

void CCertDbPage::OnBResertdb() //证书库还原
{
	// TODO: Add your control notification handler code here
	UINT uCrc = 0;
	CString strCrc;
	if(::MessageBox(this->m_hWnd, MiniCT_1810, MiniCT_0010,MB_ICONQUESTION | MB_YESNO)!=IDYES)
		return;
	/*得到配置*/
	HRSRC hRsrc=FindResource(NULL,MAKEINTRESOURCE(IDR_CERTDB_INI),"INI");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert=LoadResource(NULL,hRsrc);
	LPSTR lpCert=(LPSTR)LockResource(hgCert);

	CString m_IniPathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\CertDB.CDB";
	CFile file;
	file.Open(m_IniPathName,CFile::modeCreate|CFile::modeWrite);	//存文件
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
		BROWSEINFO bi;   //必须传入的参数,下面就是这个结构的参数的初始化 
		CString strDisplayName;// = CMiniCaApp::NormalCode("选择文件夹");;   //用来得到,你选择的活页夹路径,相当于提供一个缓冲区 
		bi.hwndOwner = GetSafeHwnd();   //得到父窗口Handle值 
		bi.pidlRoot=0;   //这个变量就是我们在上面得到的. 
		bi.pszDisplayName = strDisplayName.GetBuffer(MAX_PATH+1);   //得到缓冲区指针, 
		CString strTitle = MiniCT_1811;
		bi.lpszTitle = strTitle;  //设置标题 
		bi.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_DONTGOBELOWDOMAIN  ;   //设置标志 
		bi.lpfn=NULL; 
		bi.lParam=0; 
		bi.iImage=0;   //上面这个是一些无关的参数的设置,最好设置起来, 
		ITEMIDLIST * pidl;  
		pidl = SHBrowseForFolder(&bi);   //打开对话框 
		if(!pidl)
			return;
		SHGetPathFromIDList(pidl,bi.pszDisplayName);	
		strDisplayName.ReleaseBuffer();   //和上面的GetBuffer()相对应 
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

	//创建目录
	//结构
	//MiniCA.exe attrib desktop.ini +h +s
	//	strPath + MiniCACert																
	//					+		 CaCert				//CA中心证书存放位置			
	//                  +		WizardCert          //数证向导						

	SetDlgItemText(IDC_EDIT_SAVE, strPath);
	//得到证书命名选项

	WritePrivateProfileString("PATH", "Cert",strPath.GetBuffer(256), m_IniPathName);

//	SetPathInfo(strPath, "%SystemRoot%\\system32\\SHELL32.dll", "150", "MiniCA V2.0");  利用系统图标

	TCHAR	szProgPath[MAX_PATH * 2];
	::GetModuleFileName(NULL, szProgPath, sizeof(szProgPath)/sizeof(TCHAR));

	CString sMiniCA;
	sMiniCA.Format("%s", szProgPath);

	SetPathInfo(strPath, szProgPath, "12", MiniCT_1812);  //利用minica图标

	//首先建立 strPath + MiniCACert
	DWORD dErrCode = 0;
	BOOL b = CreateDirectory(strPath + "\\MiniCACert", NULL);
	if(!b)
		dErrCode = GetLastError();

//	HTREEITEM hPathItem = m_PathTree.InsertItem(strPath + "\\MiniCACert",0,0);

//	SetPathInfo(strPath + "\\MiniCACert", "%SystemRoot%\\system32\\SHELL32.dll", "47", 
//		CMiniCaApp::NormalCode("MiniCA 证书中心"));
	//设置文件夹属性

	//建立 strPath + MiniCACert + CaCert
	b = CreateDirectory(strPath + "\\MiniCACert\\CaCert", NULL);
	if(!b)
		dErrCode = GetLastError();

//	HTREEITEM hPathSecond = m_PathTree.InsertItem(strPath + "\\MiniCACert\\CaCert",hPathItem,0);

	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCaCenterPage * pCa = (CCaCenterPage *)(pMain->GetPage("CCaCenterPage"));

	//strPath可能带有\\比如c:\\时候就带有,这时候要去掉这个
	if(strPath.Right(1) == "\\")
	{
		strPath.Delete(strPath.GetLength() -1);
	}

	if(pCa)
	{
		pCa->SetNamePath(strPath + "\\MiniCACert\\CaCert");
	}


	//建立 strPath + MiniCACert + CaCert + MiniCACert
//	b = CreateDirectory(strPath + "\\MiniCACert\\CaCert\\Cert", NULL);
//	if(!b)
//		dErrCode = GetLastError();
//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\CaCert\\Cert",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\CaCert\\Cert");


	//建立 strPath + MiniCACert + CaCert + Crl
//	b = CreateDirectory(strPath + "\\MiniCACert\\CaCert\\Crl", NULL);
//	if(!b)
///		dErrCode = GetLastError();
//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\CaCert\\Crl",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\CaCert\\Crl");


	//建立 strPath + MiniCACert + WizardCert
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


	//建立 strPath + MiniCACert + WizardCert + Cert
//	b = CreateDirectory(strPath + "\\MiniCACert\\WizardCert\\Cert", NULL);
//	if(!b)
//		dErrCode = GetLastError();

//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\WizardCert\\Cert",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\WizardCert\\Cert");


	//建立 strPath + MiniCACert + WizardCert + Crl
//	b = CreateDirectory(strPath + "\\MiniCACert\\WizardCert\\Crl", NULL);
//	if(!b)
//		dErrCode = GetLastError();

//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\WizardCert\\Crl",hPathSecond,0);
	//建立 strPath + MiniCACert + WizardCert + Man
//	b = CreateDirectory(strPath + "\\MiniCACert\\WizardCert\\Man", NULL);
//	if(!b)
//		dErrCode = GetLastError();

//	m_PathTree.InsertItem(strPath + "\\MiniCACert\\WizardCert\\Man",hPathSecond,0);
//	m_PathStrArray.Add(strPath + "\\MiniCACert\\WizardCert\\Man");
}

void CCertDbPage::SetPathInfo(CString strPath, CString strIconFile, CString strIconIndex, 
							 CString strInfoTip)		//设置文件夹图标
{
	//目录属性 %SystemRoot%\system32\SHELL32.dll
	//	[.ShellClassInfo]
	//	IconFile=.\MiniCA.exe
	//	IconIndex=0

	//除了给desktop.ini增加属性外,还要给当前要设置的路径增加 S 属性

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
	//更新树控件

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



	//更新static控件
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
