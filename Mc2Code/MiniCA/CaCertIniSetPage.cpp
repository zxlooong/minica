// CaCertIniSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CaCertIniSetPage.h"
#include "MiniMainDlg.h"
#include "CaCertWizardSheet.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCertIniSetPage property page

IMPLEMENT_DYNCREATE(CCaCertIniSetPage, CPropertyPage)

CCaCertIniSetPage::CCaCertIniSetPage() : CPropertyPage(CCaCertIniSetPage::IDD)
{
	//{{AFX_DATA_INIT(CCaCertIniSetPage)
	//}}AFX_DATA_INIT
	m_hOldItemUser = NULL;
	m_hOldItemExt = NULL;
	m_UserMap.InitHashTable(sizeof(stuINI) + 20);
	m_ExtMap.InitHashTable(sizeof(stuINI) + 20);
}

CCaCertIniSetPage::~CCaCertIniSetPage()
{
	//清除结构MAP,释放内存空间
	POSITION pos = m_UserMap.GetStartPosition();
	while(pos != NULL)
	{
		UINT uIndex = 0;
		stuINI * pstuIni = NULL;
		m_UserMap.GetNextAssoc(pos, uIndex, pstuIni);
		delete pstuIni;
	}
	m_UserMap.RemoveAll();


	pos = m_ExtMap.GetStartPosition();
	while(pos != NULL)
	{
		UINT uIndex = 0;
		stuINI * pstuIni = NULL;
		m_ExtMap.GetNextAssoc(pos, uIndex, pstuIni);
		delete pstuIni;
	}
	m_ExtMap.RemoveAll();
}

void CCaCertIniSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCertIniSetPage)
	DDX_Control(pDX, IDC_TREEEXT, m_ExtTree);
	DDX_Control(pDX, IDC_TREEUSER, m_UserTree);
	DDX_Control(pDX, IDC_B_SAVE, m_BSave);
	DDX_Control(pDX, IDC_B_RESERT, m_BResert);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaCertIniSetPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCertIniSetPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEUSER, OnSelchangedTree)
	ON_EN_KILLFOCUS(IDC_EDIT2, OnKillfocusEdit2)
	ON_BN_CLICKED(IDC_B_SAVE, OnBSave)
	ON_BN_CLICKED(IDC_CAINI_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_CAINI_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_B_RESERT, OnBResert)
	ON_BN_CLICKED(IDC_CAINI_CHECK1, OnCheck1)
	ON_NOTIFY(NM_CLICK, IDC_TREEUSER, OnClickTree)
	ON_NOTIFY(NM_CLICK, IDC_TREEEXT, OnClickTreeext)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEEXT, OnSelchangedTreeext)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCertIniSetPage message handlers
BOOL CCaCertIniSetPage::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
    UINT nID = pTTTStruct->idFrom;
	BOOL bInfoSetup	= ((CButton * )GetDlgItem(IDC_CAINI_RADIO1))->GetCheck();//數證信息
	CString strText;
	switch(nID)
	{
	case IDC_CAINI_CHECK1:
		if(bInfoSetup)
		{
			strText = "是否在信息选项卡列表中出现\r当前条目是否出现在信息\n选项卡的基本信息列表中";
		}
		else
		{
			strText = "是否在扩展选项卡列表中出现\r当前条目是否出现在扩展\n选项卡的列表中";
		}
		_tcscpy(pTTT->szText, strText);//设置
		return TRUE;
	case IDC_CAINI_CHECK2:
		if(bInfoSetup)
		{
			strText = "是否在信息选项卡列表中选中\r当前条目是否在信息选项卡\n列表中处于选中状态";
		}
		else
		{
			strText = "是否在扩展选项卡列表中选中\r当前条目是否在扩展选项卡\n列表中处于选中状态";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_EDIT1:
		if(bInfoSetup)
		{
			strText = "条目在证书信息中内部标识\r证书信息程序内部使用的标识\n详细信息请参阅RFC3383";
		}
		else
		{
			strText = "条目在证书扩展中内部标识\r证书扩展程序内部使用的标识\n详细信息请参阅RFC3383";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_EDIT2:
		if(bInfoSetup)
		{
			strText = "条目在信息选项卡列表中显示名称\r当前条目在信息选项卡\n基本信息列表中的显示名称";
		}
		else
		{
			strText = "条目在扩展选项卡列表中显示名称\r当前条目在扩展选项卡列表的显示名称";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_EDIT3:	//PKCS12
		if(bInfoSetup)
		{
			strText = "条目当前内容\r当前条目在信息选项卡\n基本信息列表中显示的内容";
		}
		else
		{
			strText = "条目当前内容\r当前条目在扩展选项卡\n列表中显示的内容";
		}
		_tcscpy(pTTT->szText,strText);//设置
		return TRUE;
	case IDC_B_RESERT: 
		if(bInfoSetup)
		{
			_tcscpy(pTTT->szText, "重置信息选项卡配置\r操作不可恢复");//设置
		}
		else
		{
			_tcscpy(pTTT->szText, "重置扩展选项卡配置\r操作不可恢复");//设置
		}
		return TRUE;
	case IDC_B_SAVE: //	 
		if(bInfoSetup)
		{
			_tcscpy(pTTT->szText, "保存信息选项卡列表\r保存后配置生效");//设置
		}
		else
		{
			_tcscpy(pTTT->szText, "保存扩展选项卡列表\r保存后配置生效");//设置
		}
		return TRUE;
	}
	return FALSE;
}
/*CString CCaCertIniSetPage::GetCertPath(int dIndex)
{
	//遍历树,得到dIndex对应的路径信息
	if(dIndex >= m_PathStrArray.GetSize() - 1)
		return "";

	return m_PathStrArray[dIndex];

}*/

BOOL CCaCertIniSetPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_CAINI_CHECK1)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CAINI_CHECK2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CAINI_RADIO1)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_CAINI_RADIO2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT1)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_EDIT3)->m_hWnd,TRUE);
	}

	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());

	m_BSave.SetIcon(IDI_ICON_VIEW);
	m_BSave.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	m_BResert.SetIcon(IDI_ICON19);
	m_BResert.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);


	((CButton *)GetDlgItem(IDC_CAINI_RADIO1))->SetCheck(1);

	CImageList * pImgList = ((CMiniCaApp *)AfxGetApp())->GetImgList();
	m_UserTree.SetImageList(pImgList,TVSIL_NORMAL);//用来改变LISTCTRL行宽度
	m_ExtTree.SetImageList(pImgList,TVSIL_NORMAL);//用来改变LISTCTRL行宽度
	
	GetIniInfo(EXTTREE, ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniExt.ini");
	GetIniInfo(USERTREE, ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniCA.ini");
	
	GetDlgItem(IDC_TREEUSER)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_TREEEXT)->ShowWindow(SW_HIDE);
	m_eTreeType = USERTREE;

	m_toolTip.Create(this);
	m_toolTip.AddTool(GetDlgItem(IDC_TREEUSER), "数证信息配置树");
	m_toolTip.AddTool(GetDlgItem(IDC_TREEEXT), "数证扩展配置树");
	m_toolTip.AddTool(GetDlgItem(IDC_CAINI_RADIO1), "切换到数证信息配置");
	m_toolTip.AddTool(GetDlgItem(IDC_CAINI_RADIO2), "切换到数证扩展配置");

	m_toolTip.AddTool(GetDlgItem(IDC_CAINI_CHECK1), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_CAINI_CHECK2), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT1), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT2), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT3), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_B_RESERT), LPSTR_TEXTCALLBACK);
	m_toolTip.AddTool(GetDlgItem(IDC_B_SAVE), LPSTR_TEXTCALLBACK);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCaCertIniSetPage::GetIniInfo(eTreeType eType, CString strIniPath)
{
	CMap<UINT, UINT, stuINI *, stuINI * > * pMap = NULL;
	CTreeCtrl * pTree = NULL;

	if(eType == USERTREE)
	{
		pMap = &m_UserMap;
		pTree = &m_UserTree;
	}
	else
	{
		pMap = &m_ExtMap;
		pTree = &m_ExtTree;
	}

	//清除结构MAP,释放内存空间
	POSITION pos = pMap->GetStartPosition();
	while(pos != NULL)
	{
		UINT uIndex = 0;
		stuINI * pstuIni = NULL;
		pMap->GetNextAssoc(pos, uIndex, pstuIni);
		delete pstuIni;
	}
	pMap->RemoveAll();

	pTree->SetRedraw(FALSE);
	//删除所有树信息
	pTree->DeleteAllItems();

	HTREEITEM hPathItem = TVI_ROOT;//pTree.InsertItem(strTreeName, 0, 0);
	pTree->SetItemData(hPathItem, -1);

	char isdisp[5] = {0};
	char info[255] = {0};
	char inside[255] = {0};
	char dispname[255] = {0};
	char ischeck[5] = {0};

	GetPrivateProfileString("TOTAL", "ToTal","0",info,50, strIniPath);
	CString strIndex;
	int uMax = atoi(info);
	for(int i = 0; i<uMax; i++)
	{
		strIndex.Format("INFO%d",i+1);
		//首先读取是否显示标志
		GetPrivateProfileString(strIndex, "IsDisp","0",isdisp,5, strIniPath);

		GetPrivateProfileString(strIndex, "InsideName","0",inside,254, strIniPath);//内部名称

		GetPrivateProfileString(strIndex, "DispName","0",dispname,254, strIniPath);

		GetPrivateProfileString(strIndex, "Info","0",info,254, strIniPath);

		GetPrivateProfileString(strIndex, "IsCheck","78",ischeck,5, strIniPath);//是否选中

		stuINI * pstuIni = new stuINI(atoi(isdisp), inside, dispname, info, atoi(ischeck));

		HTREEITEM hItem = pTree->InsertItem(dispname, 0, 0, hPathItem);

		//如果映射不合法,则显示图标为非法  显示在列表中,并且有空项为不合法
		if(atoi(isdisp) && (strlen(inside) == 0 || strlen(dispname) == 0 || strlen(info) == 0))
		{
			//非法
			pTree->SetItemImage(hItem, 16, 16);
		}
		else if(atoi(isdisp)) //如果显示
		{
			pTree->SetItemImage(hItem, 1, 1);
		}

		pTree->SetItemData(hItem, i);

		pMap->SetAt(i, pstuIni);
	}
	pTree->Expand(pTree->GetRootItem(), TVE_EXPAND);

	pTree->SelectItem(pTree->GetFirstVisibleItem());//设置选中

	pTree->SetRedraw();
}

void CCaCertIniSetPage::SelchangedTree(CTreeCtrl * pTree, NM_TREEVIEW* pNMTreeView,
									   CMap<UINT, UINT, stuINI *, stuINI * > * pMap)
{
	//保存旧信息,显示新信息
	if(pNMTreeView->itemOld.hItem)	//旧信息可能发生改变,这时候需要保存
	{
		UpdateInfo(pTree, pNMTreeView->itemOld.hItem, pMap);
	}
	if(pNMTreeView->itemNew.hItem)
	{
		DWORD dData = pTree->GetItemData(pNMTreeView->itemNew.hItem);
		if(dData != -1)
		{
			stuINI * pstuIni = (*pMap)[dData];
			if(pstuIni)
			{
				SetDlgItemText(IDC_EDIT1, pstuIni->cInsideName);
				SetDlgItemText(IDC_EDIT2, pstuIni->cDispName);
				SetDlgItemText(IDC_EDIT3, pstuIni->cInfo);
				((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(pstuIni->bIsDisp);
				((CButton *)GetDlgItem(IDC_CAINI_CHECK2))->SetCheck(pstuIni->IsCheck);
			}
		}
		else
		{
			SetDlgItemText(IDC_EDIT1, "");
			SetDlgItemText(IDC_EDIT2, "");
			SetDlgItemText(IDC_EDIT3, "");
			((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(0);
			((CButton *)GetDlgItem(IDC_CAINI_CHECK2))->SetCheck(0);
		}
		if(m_eTreeType == USERTREE)
			m_hOldItemUser = pNMTreeView->itemNew.hItem;
		else
			m_hOldItemExt = pNMTreeView->itemNew.hItem;

	}
	
}

void CCaCertIniSetPage::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	SelchangedTree(&m_UserTree, pNMTreeView, &m_UserMap);
	*pResult = 0;
}

void CCaCertIniSetPage::OnSelchangedTreeext(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	SelchangedTree(&m_ExtTree, pNMTreeView, &m_ExtMap);
	*pResult = 0;
}

void CCaCertIniSetPage::OnKillfocusEdit2() //更新左侧树中名称
{
	// TODO: Add your control notification handler code here
	CString strDisp;
	GetDlgItemText(IDC_EDIT2, strDisp);
	strDisp.TrimLeft();
	if(!strDisp.IsEmpty())
	{
		if(m_eTreeType == USERTREE)
			m_UserTree.SetItemText(m_hOldItemUser, strDisp);
		else
			m_ExtTree.SetItemText(m_hOldItemExt, strDisp);
	}

}


void CCaCertIniSetPage::OnBSave() 
{
	// TODO: Add your control notification handler code here

	CMap<UINT, UINT, stuINI *, stuINI * > * pMap = NULL;

	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();

	BOOL bInfo = ((CButton *)GetDlgItem(IDC_CAINI_RADIO1))->GetCheck();

	CString strIni;
	if(bInfo)//保存信息
	{
		//首先更新当前选中的树项信息,因为如果树项不发生改变,信息不会被更新,这时候需要强制更新
		strIni = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniCA.ini";
		pMap = &m_UserMap;
		UpdateInfo(&m_UserTree, m_UserTree.GetSelectedItem(), pMap);

	}
	else//保存扩展
	{
		//首先更新当前选中的树项信息,因为如果树项不发生改变,信息不会被更新,这时候需要强制更新
		strIni = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniExt.ini";
		pMap = &m_ExtMap;
		UpdateInfo(&m_ExtTree, m_ExtTree.GetSelectedItem(), pMap);
	}

	for(int index = 1; index <= pMap->GetCount(); index++)
	{
		stuINI * pstuOld = NULL;
		if(pMap->Lookup(index-1, pstuOld))	//结构MAP从1开始
		{
			CString strIndex, strTemp;
			strIndex.Format("INFO%d", index);

			strTemp.Format("%d", pstuOld->bIsDisp);
			WritePrivateProfileString(strIndex, "IsDisp", strTemp, strIni);

			WritePrivateProfileString(strIndex, "InsideName", pstuOld->cInsideName, strIni);
			WritePrivateProfileString(strIndex, "DispName", pstuOld->cDispName, strIni);
			WritePrivateProfileString(strIndex, "Info", pstuOld->cInfo, strIni);

			strTemp.Format("%d", pstuOld->IsCheck);
			WritePrivateProfileString(strIndex, "IsCheck", strTemp, strIni);

		}
	}
	if(bInfo)//保存信息
	{
		CCaCertWizardSheet * pWizard = (CCaCertWizardSheet *)(pMain->GetPage("CCaCertWizardSheet"));
		if(pWizard)
		{
			CCaCertInfoPage * pInfoPage = (CCaCertInfoPage *)(pWizard->GetPage("CCaCertInfoPage"));
			if(pInfoPage)
				pInfoPage->GetIniInfo(strIni);
		}
	}
	else
	{
		CCaCertWizardSheet * pWizard = (CCaCertWizardSheet *)(pMain->GetPage("CCaCertWizardSheet"));
		if(pWizard)
		{
			CCaCertExtPage * pExtPage = (CCaCertExtPage *)(pWizard->GetPage("CCaCertExtPage"));
			if(pExtPage)
				pExtPage->GetIniInfo(strIni);
		}
	}

}

//切换证书信息与证书配置
void CCaCertIniSetPage::OnRadio1() //从EXT -> USER
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_TREEUSER)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_TREEEXT)->ShowWindow(SW_HIDE);
	m_eTreeType = USERTREE;

	//更新EXT
	UpdateInfo(&m_ExtTree, m_ExtTree.GetSelectedItem(), &m_ExtMap);


	HTREEITEM hItem = m_UserTree.GetSelectedItem();
	if(hItem)
	{
		DWORD dData = m_UserTree.GetItemData(hItem);
		stuINI * pstuOld = NULL;
		if(m_UserMap.Lookup(dData, pstuOld))	//结构MAP从1开始
		{
			SetDlgItemText(IDC_EDIT1, pstuOld->cInsideName);
			SetDlgItemText(IDC_EDIT2, pstuOld->cDispName);
			SetDlgItemText(IDC_EDIT3, pstuOld->cInfo);
			((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(pstuOld->bIsDisp);
			((CButton *)GetDlgItem(IDC_CAINI_CHECK2))->SetCheck(pstuOld->IsCheck);

		}
	}

}

void CCaCertIniSetPage::OnRadio2() //从USER -> EXT
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_TREEUSER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TREEEXT)->ShowWindow(SW_SHOW);
	m_eTreeType = EXTTREE;

	//更新USER
	UpdateInfo(&m_UserTree, m_UserTree.GetSelectedItem(), &m_UserMap);

	HTREEITEM hItem = m_ExtTree.GetSelectedItem();
	if(hItem)
	{
		DWORD dData = m_ExtTree.GetItemData(hItem);
		stuINI * pstuOld = NULL;
		if(m_ExtMap.Lookup(dData, pstuOld))	//结构MAP从1开始
		{
			SetDlgItemText(IDC_EDIT1, pstuOld->cInsideName);
			SetDlgItemText(IDC_EDIT2, pstuOld->cDispName);
			SetDlgItemText(IDC_EDIT3, pstuOld->cInfo);
			((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(pstuOld->bIsDisp);
			((CButton *)GetDlgItem(IDC_CAINI_CHECK2))->SetCheck(pstuOld->IsCheck);
		}
	}
}

void CCaCertIniSetPage::OnBResert() 
{
	// TODO: Add your control notification handler code here
	
}

void CCaCertIniSetPage::UpdateInfo(CTreeCtrl * pTree, const HTREEITEM hItem, 
								   CMap<UINT, UINT, stuINI *, stuINI * > * pMap)	//信息项发生改变时候,更新MAP
{
	if(!pTree || !hItem || !pMap)
		return;
	DWORD dData = pTree->GetItemData(hItem);
	if(dData != -1)
	{
		CString strInside,
			strDisp,
			strInfo;
		GetDlgItemText(IDC_EDIT1, strInside);
		GetDlgItemText(IDC_EDIT2, strDisp);
		if(strDisp.IsEmpty())
		{
			strDisp = pTree->GetItemText(hItem);
		}
		GetDlgItemText(IDC_EDIT3, strInfo);

		BOOL bIsDisp = ((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->GetCheck();
		BOOL bIsCheck = ((CButton *)GetDlgItem(IDC_CAINI_CHECK2))->GetCheck();
		
		
		//删除原来映射
		stuINI * pstuOld = NULL;
		if(pMap->Lookup(dData, pstuOld))
		{
			delete [] pstuOld;
		}
		
		//增加新映射
		stuINI * pstuIni = new stuINI(bIsDisp, strInside.GetBuffer(255), 
			strDisp.GetBuffer(255), strInfo.GetBuffer(255), bIsCheck);
		pMap->SetAt(dData, pstuIni);
		
		//如果映射不合法,则显示图标为非法  显示在列表中,并且有空项为不合法
		if(bIsDisp)
		{
			if(strInside.IsEmpty() || strDisp.IsEmpty() || strInfo.IsEmpty())		//非法
				pTree->SetItemImage(hItem, 16, 16);
			else
				pTree->SetItemImage(hItem, 1, 1);
		}
		else//显示空白
		{
			pTree->SetItemImage(hItem, 0, 0);
		}
	}
	
}

void CCaCertIniSetPage::OnCheck1() //界面中显示
{
	// TODO: Add your control notification handler code here
	CTreeCtrl * pCtrl = NULL;
	if(m_eTreeType == USERTREE)
	{
		pCtrl = &m_UserTree;
	}
	else
	{
		pCtrl = &m_ExtTree;
	}
	HTREEITEM hItem = pCtrl->GetSelectedItem();
	BOOL bIsDisp =  ((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->GetCheck();
	if(bIsDisp) //如果显示
	{
		//判断是否合法,包括,右侧2项内容是否齐全
		CString strInside,
				strDisp,
				strInfo;
		GetDlgItemText(IDC_EDIT1, strInside);
		GetDlgItemText(IDC_EDIT3, strInfo);

		if(strInside.IsEmpty() || strInfo.IsEmpty())
		{
			pCtrl->SetItemImage(hItem, 16, 16);
		}
		else
			pCtrl->SetItemImage(hItem, 1, 1);

	}
	else
		pCtrl->SetItemImage(hItem, 0, 0);
}

void CCaCertIniSetPage::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) //用户信息
{
	// TODO: Add your control notification handler code here
	ClickTree(&m_UserTree);
	*pResult = 0;
}

void CCaCertIniSetPage::OnClickTreeext(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ClickTree(&m_ExtTree);
	*pResult = 0;
}

void CCaCertIniSetPage::ClickTree(CTreeCtrl *pTree)
{
	CPoint point;
	GetCursorPos(&point); // 当前鼠标坐标
	CPoint PointInTree = point;
	pTree->ScreenToClient( &PointInTree);
	
	UINT nFlag;
	HTREEITEM hItem = pTree->HitTest( PointInTree, &nFlag);
	if(hItem == NULL)
	{
		return;
	}

	pTree->SelectItem(hItem);
	
	if (nFlag&TVHT_ONITEMICON) //点种图标,则选择或不选
	{
		int nImage,nSelectedImage,nState;
		pTree->GetItemImage(hItem,nImage,nSelectedImage);

		CString strInside,
				strDisp,
				strInfo;
			GetDlgItemText(IDC_EDIT1, strInside);
			GetDlgItemText(IDC_EDIT3, strInfo);

		BOOL bInvalid = FALSE;
		if(strInside.IsEmpty() || strInfo.IsEmpty()) //非法
			bInvalid = TRUE;

		if(nImage <3) // 0 -> 1; 1 -> 0
		{
			
			if(nImage == 0) //0 -> 1,需要判断//判断是否合法,包括,右侧2项内容是否齐全
			{
				
				if(bInvalid) //非法
				{
					pTree->SetItemImage(hItem, 16, 16);
				}
				else
				{
					nState = !nImage;
					pTree->SetItemImage(hItem,nState,nState);
					//nState 标示选中状态 0-未选择 1-以选择
					((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(nState);
				}
			}
			else // 1 -> 0
			{
				nState = !nImage;
				pTree->SetItemImage(hItem,nState,nState);
				//nState 标示选中状态 0-未选择 1-以选择
				((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(nState);
			}
		}
		else if(nImage == 16)	//如果合法,可以变成选中,非法只能变成不选
		{
			if(bInvalid) //非法
			{
				pTree->SetItemImage(hItem, 0, 0);
				((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(0);
			}
			else
			{
				pTree->SetItemImage(hItem, 1, 1);
				((CButton *)GetDlgItem(IDC_CAINI_CHECK1))->SetCheck(1);
			}
		}
	}

}


void CCaCertIniSetPage::TranslateCT()
{
	SetDlgItemText(IDC_CAINI_RADIO1,	MiniCT_10401);
	SetDlgItemText(IDC_CAINI_RADIO2,	MiniCT_10402);
	SetDlgItemText(IDC_CAINI_CHECK1,	MiniCT_10403);
	SetDlgItemText(IDC_CAINI_CHECK2,	MiniCT_10404);
	SetDlgItemText(IDC_CAINI_STATIC1,	MiniCT_10405);
	SetDlgItemText(IDC_CAINI_STATIC2,	MiniCT_10406);
	SetDlgItemText(IDC_CAINI_STATIC3,	MiniCT_10407);
	SetDlgItemText(IDC_CAINI_STATIC4,	MiniCT_10408);
	SetDlgItemText(IDC_B_SAVE,			MiniCT_10409);
	SetDlgItemText(IDC_B_RESERT,		MiniCT_10410);
}

BOOL CCaCertIniSetPage::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}
