// SetupTree.cpp : implementation file
//

#include "stdafx.h"
#include "..\..\minica.h"
#include "SetupTree.h"
#include "..\..\CertDbPage.h"
#include "..\..\MiniMainDlg.h"

#define SPACE 80
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupTree

CSetupTree::CSetupTree()
{
	m_pEdit = NULL;
	m_pButton = NULL;
	m_hControlItem = NULL;
	m_CdbPath = ((CMiniCaApp *)AfxGetApp())->GetAppPath() + "\\CertDB.CDB";
}

CSetupTree::~CSetupTree()
{
	m_HtreeList.RemoveAll();
}


BEGIN_MESSAGE_MAP(CSetupTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CSetupTree)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_BN_CLICKED(TREE_OPTIONS_BUTTON_ID, OnOptionsButtonId)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupTree message handlers
void CSetupTree::ExpandParent(HTREEITEM item,UINT nCode )////展开,收缩
{
	SetRedraw(FALSE);
	if(item)
		Expand( item, nCode );
	else
		return;
	HTREEITEM hParentItem = GetParentItem( item );
	if(hParentItem)//是否有父
	{
		Expand( hParentItem, nCode );
		ExpandParent(hParentItem, nCode);
	}
	else
		return;
//	EnsureVisible( GetSelectedItem());
	SetRedraw();
}

void CSetupTree::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	UpdateTreeControlTextFromChildControl();
	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSetupTree::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	UpdateTreeControlTextFromChildControl();
	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSetupTree::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point;
	GetCursorPos(&point); // 当前鼠标坐标
	CPoint PointInTree = point;
	ScreenToClient( &PointInTree);
	
	HTREEITEM hPItem;
	UINT nFlag;
	HTREEITEM hItem = HitTest( PointInTree, &nFlag);
	if(hItem == NULL)
	{
		UpdateTreeControlTextFromChildControl();
		return;
	}
	
	if (nFlag&TVHT_ONITEMICON) //点种图标,则选择或不选
	{
		int nImage,nSelectedImage,nState;
		GetItemImage(hItem,nImage,nSelectedImage);
		if(nImage <3)
		{
			nState = !nImage;
			SetItemImage(hItem,nState,nState);
			TravelChild(hItem, nState);
			hPItem = GetParentItem(hItem);
			if(hPItem!=NULL)
				TravelSiblingAndParent(hItem,nState);
		}
	}
	
	else if(nFlag&TVHT_ONITEMLABEL) //点种标签,增加证书,密码
	{	
		if ((nFlag & TVHT_ONITEM) && GetItemData(hItem) && m_hControlItem == NULL)
		{
			CreateNewChildControl(hItem);
		}
	}
	else
	{
		UpdateTreeControlTextFromChildControl();
	}
	
	*pResult = 0;
}

void CSetupTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (m_hControlItem)
	{
		UpdateTreeControlTextFromChildControl();
	}
//	//Create the new combo if need be
	if(pNMTreeView->itemOld.hItem)
		CreateNewChildControl(pNMTreeView->itemNew.hItem);
	
	*pResult = 0;
}

void CSetupTree::TravelChild(HTREEITEM hItem, int nState)
{
	HTREEITEM hChildItem,hBrotherItem;
	DWORD dData = GetItemData(hItem);
	if(dData>=10)
		return;
	
	//查找子节点，没有就结束
	hChildItem = GetChildItem(hItem);
	if(hChildItem!=NULL)
	{
		//设置子节点的状态与当前节点的状态一致
		int temp;
		GetItemImage( hChildItem,temp ,temp );
		if(temp<3)
			SetItemImage( hChildItem,nState ,nState );
		//再递归处理子节点的子节点和兄弟节点
		TravelChild(hChildItem, nState);
		
		//处理子节点的兄弟节点和其子节点
		hBrotherItem = GetNextSiblingItem(hChildItem);
		while (hBrotherItem)
		{
			//设置子节点的兄弟节点状态与当前节点的状态一致
			SetItemImage( hBrotherItem,nState ,nState );
			//再递归处理子节点的兄弟节点的子节点和兄弟节点
			TravelChild(hBrotherItem, nState);
			hBrotherItem = GetNextSiblingItem(hBrotherItem);
		}
	}
}

void CSetupTree::TravelSiblingAndParent(HTREEITEM hItem, int nState)
{
	HTREEITEM hNextSiblingItem,hPrevSiblingItem,hParentItem;
	//查找父节点，没有就结束
	hParentItem = GetParentItem(hItem);
	if(hParentItem==NULL)
		return;

	int nImage,nSelectedImage;
	BOOL bParent = TRUE;//需要继续比较
	//查找当前节点下面的兄弟节点的图像,如果nState!=nImage,则父节点为灰色
	hNextSiblingItem = GetNextSiblingItem(hItem);
	while(hNextSiblingItem!=NULL)
	{
		GetItemImage(hNextSiblingItem,nImage,nSelectedImage);
		if(nImage!=nState)
		{
			bParent = FALSE;//此次比较已经确认父为灰色
			break;
		}
		else 
			hNextSiblingItem = GetNextSiblingItem(hNextSiblingItem);
	}
	if(bParent)//继续比较兄弟节点
	{
		//查找当前节点上面的兄弟节点的状态
		hPrevSiblingItem = GetPrevSiblingItem(hItem);
		while(hPrevSiblingItem!=NULL)
		{
			GetItemImage(hPrevSiblingItem,nImage,nSelectedImage);
			if(nImage!=nState) 
			{
				bParent = FALSE;//此次比较已经确认父为灰色
				break;
			}
			else hPrevSiblingItem = GetPrevSiblingItem(hPrevSiblingItem);
		}
	}
	if(!bParent)//设置父节点为灰色,上一级为灰
	{
		SetItemImage(hParentItem, 2, 2);
		hParentItem = GetParentItem(hItem);
		while(hParentItem!=NULL)
		{
			SetItemImage( hParentItem, 2, 2);
			hParentItem = GetParentItem(hParentItem);
		}
	}
	else//设置父节点为当前选中状态
		SetItemImage(hParentItem,nState,nState );

	//再递归处理父节点的兄弟节点和其父节点
	TravelSiblingAndParent(hParentItem,nState);
}

void CSetupTree::OnOptionsButtonId() 
{
	// TODO: Add your control notification handler code here
	SelectFile();
}

void CSetupTree::SelectFile() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = GetSelectedItem ();
	UINT m_SelectID = GetItemData(hItem);
	CString sFilter,sTitle;//过滤器,标题条
	if(m_SelectID<100)
		return;
	switch(m_SelectID%100)
	{
	case 0://公钥
		sFilter = "公钥证书(*.pem;*.der;*.cer)|*.pem;*.der;*.cer|所有文件(*.*)|*.*||";
		sTitle = "公钥证书文件";
		break;
	case 10://私钥
		sFilter = "私钥证书(*.pem;*.der;*.cer)|*.pem;*.der;*.cer|所有文件(*.*)|*.*||";
		sTitle = "私钥证书文件";
		break;
	case 20://pfx
		sFilter = "个人信息交换(*.pfx;*.p12)|*.pfx;*.p12|所有文件(*.*)|*.*||";
		sTitle = "个人信息交换文件";
		break;
	default:
		return;
	}
	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		CMiniCaApp::NormalCode(sFilter),NULL);

	CString strTitle = CMiniCaApp::NormalCode(sTitle);
	dlgOpen.m_ofn.lpstrTitle = strTitle;//标题条

	if(dlgOpen.DoModal()==IDOK) 
	{
		CString sName = dlgOpen.GetPathName();
		CString strCause,strPwd;
//		CString sName = GetItemText(hItem);
//		sName += ": " + dlgOpen.GetPathName();
//		DWORD dData = GetItemData(hItem);
//		stuCertDB CertDB;
//		if(CertDB.LoadFromCert(sName.GetBuffer(0),dData,strPwd.GetBuffer(0),0,strCause))
		SetItemText(hItem,sName);
		UpdateTreeControlTextFromChildControl();

//		int nImage,nSelectedImage,nState;
//		HTREEITEM hPItem;
//		SetItemImage(hItem,1,1);
//		TravelChild(hItem, 1);
//		hPItem = GetParentItem(hItem);
//		if(hPItem!=NULL)
//			TravelSiblingAndParent(hItem,1);

	}
}


void CSetupTree::LoadCdb() //加载证书库
{
	UpdateTreeControlTextFromChildControl();
	CFile file;
	if(!file.Open(m_CdbPath,CFile::modeRead))
		return;
	for(;;)
	{
		stuCertDB CertDB;
		int len = file.Read(&CertDB,sizeof(stuCertDB));
		if(len==0)
			break;
		CString str;
		if(CertDB.LoadFromDB(str))
		{
			//查找链表,增加数据
			DWORD dData;
			HTREEITEM hTempItem;
			for(POSITION pos = m_HtreeList.GetHeadPosition(); pos!=NULL; )
			{
				hTempItem = (HTREEITEM)m_HtreeList.GetNext(pos);
				dData = GetItemData(hTempItem);
				if(dData == CertDB._uID)
				{
					SetItemText(hTempItem, CMiniCaApp::NormalCode(str));
					SetItemImage(hTempItem,4,4);
					if(dData%100 == 10)//私钥
					{
						SetItemText(GetNextSiblingItem(hTempItem),"********");
						if(strcmp(CertDB._chPWD,"")!=0)//密码有效
						{
							SetItemImage(GetNextSiblingItem(hTempItem),6,6);
						}
						else
						{
							SetItemImage(GetNextSiblingItem(hTempItem),5,5);
						}

					}

					SetItemImage(GetParentItem(hTempItem),CertDB._bUSED,CertDB._bUSED);
					TravelSiblingAndParent(GetParentItem(hTempItem),CertDB._bUSED);
					//展开有效项目
					ExpandParent(GetParentItem(hTempItem),TVE_EXPAND);
					break;
				}
				
			}
		}
	}
	file.Close();
}

void CSetupTree::Savecdb() //保存证书库
{
	// TODO: Add your control notification handler code here
//	ResetContent();
	//遍历数
	UpdateTreeControlTextFromChildControl();
	CFile file;
	if(!file.Open(m_CdbPath,CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate))
		return;
	HTREEITEM hTempItem = NULL;
	int nImage,nSelectedImage;
	CString strPwd,strName,strCause;
	HTREEITEM hPItem;
	DWORD dData,len = 0;
	for(POSITION pos = m_HtreeList.GetHeadPosition(); pos!=NULL;)
	{
		stuCertDB CertDB;
		hTempItem = (HTREEITEM)m_HtreeList.GetNext(pos);
		hPItem = GetParentItem(hTempItem);
		dData = GetItemData(hTempItem);
		GetItemImage(hPItem,nImage,nSelectedImage);//需要直接父窗口的图像
		strName = GetItemText(hTempItem);//得到文件或者证书名成
		if(strName == "公钥文件" || strName == "私钥文件")//表示此项没有加载
			continue;
	
		switch(dData%100)
		{
		case 0://公钥
			strPwd = "";
			break;
		case 10://私钥
			strPwd = GetItemText(GetNextSiblingItem(hTempItem));
			break;
		default:
			break;
		}

		if(strName.Find(":\\",0) == -1)//已经是证书库中内容,非文件,不包含路径,就是证书名
		{
			//判断使用情况
			file.SeekToBegin();
			for(int i = 0;;i++)
			{
				len = file.Read(&CertDB,sizeof(stuCertDB));
				if(len == 0)
					break;
				if(CertDB._uID == dData)
				{
					file.Seek(i*sizeof(stuCertDB),CFile::begin);
					CertDB._bUSED = nImage;
					if(dData%100 == 10)
					{
						if(strPwd.Find("********") != -1 && strcmp(CertDB._chPWD,"") == 0) //发现********
						{
							strPwd = "";
						}
						strcpy(CertDB._chPWD,strPwd.GetBuffer(0));//密码
					}
					//修改CRC
					CertDB.SetCRC();
					file.Write(&CertDB,sizeof(CertDB));
					break;
				}

			}
			continue;
		}
		else//文件
		{
			//新文件
			if(CertDB.LoadFromCert(strName.GetBuffer(0),dData,strPwd.GetBuffer(0),
				nSelectedImage,strCause))
			{
				BOOL bAlread = FALSE;
				len = 0;
				//判断证书库中是否已经存在
				file.SeekToBegin();
				for(int i = 0;;i++)
				{
					stuCertDB CertTemp;
					len = file.Read(&CertTemp,sizeof(stuCertDB));
					if(len == 0)
						break;
					if(CertTemp._uID == dData)
					{
						file.Seek(i*sizeof(stuCertDB),CFile::begin);
						CertDB._bUSED = nImage;
						if(dData%100 == 10)// && strPwd.Find("********")==-1
							strcpy(CertDB._chPWD,strPwd.GetBuffer(0));//密码
						//修改CRC
						CertDB.SetCRC();
						file.Write(&CertDB,sizeof(CertDB));
						bAlread = TRUE;
						break;
					}
					
				}
				if(!bAlread) //新文件
				{
					file.SeekToEnd();
					file.Write(&CertDB,sizeof(stuCertDB));
				}
			}
			else
			{
				strName = GetItemText(hPItem);
				strName += strCause;
				//得到父窗口
				CCertDbPage * pSetupPage =  (CCertDbPage *)GetParent();
				if(::IsWindow(pSetupPage->m_hWnd))
					pSetupPage->AddMsg(strName,M_WARING);
			}
		}
	}
	file.Close();
	LoadCdb();
}

void CSetupTree::UpdateTreeControlTextFromChildControl()
{
	HTREEITEM hItem = m_hControlItem;

	if (m_pEdit)
	{
		CString sText;
	    m_pEdit->GetWindowText(sText);
		if(hItem)
			SetItemText(hItem, sText);
		m_pEdit->DestroyWindow();
		delete m_pEdit;
		m_pEdit = NULL;
	}

	if (m_pButton)
	{
		m_pButton->DestroyWindow();
		delete m_pButton;
		m_pButton = NULL;
	}
	m_hControlItem = NULL;
}

void CSetupTree::OnDestroy() 
{
	CTreeCtrl::OnDestroy();
	// TODO: Add your message handler code here
	UpdateTreeControlTextFromChildControl();

}

void CSetupTree::CreateNewChildControl(HTREEITEM hItem)
{
	UINT m_SelectID = GetItemData(hItem);
//	SelectItem(hItem);
	CString str;
	str.Format("%d",m_SelectID);
	//	AfxMessageBox(str);
	
	if(m_SelectID >= 100)
	{
		UINT uTemp = m_SelectID%100;
		if((uTemp == 11)||(uTemp == 21))//密码 增加新控件
		{
			//	ModifyStyle(0, GetStyle() | TVS_EDITLABELS);
			//work out the rect for the combo box
			CString sEditText = GetItemText(hItem);
			
			CFont* pFont = GetFont();
			
			//work out the rect for the combo box
			CRect rText;
			GetItemRect(hItem, &rText, TRUE);
			
			CRect r;
			r.top = rText.top;
			r.left = rText.left ;
			r.right = r.left + 60; //Ask the edit box for the width to use
			r.bottom = rText.bottom + 5;
			
			//Create the new edit box
			m_pEdit = new CEdit;
			ASSERT(m_pEdit);
			m_pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("Edit"), sEditText,
				WS_VISIBLE | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL | ES_PASSWORD,
				r, this, TREE_OPTIONS_EDITBOX_ID);
			m_pEdit->SetLimitText(8);
			
			//set the font the edit box should use based on the font in the tree control
			m_pEdit->SetFont(pFont);
			m_pEdit->SendMessage(EM_SETSEL, 0, -1);
			m_pEdit->SetFocus();
		}
		else if((uTemp == 0)||(uTemp == 10)||(uTemp == 20)) //文件
		{
			m_pButton = new CXPStyleButtonST;
			ASSERT(m_pButton);
			CRect rText;
			GetItemRect(hItem, &rText, TRUE);
			
			CRect r;
			r.top = rText.top;
			r.left = rText.left + SPACE;
			r.right = r.left + 16; //Ask the edit box for the width to use
			r.bottom = rText.top + 16;
			
			m_pButton->Create("",WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, r, 
				this, TREE_OPTIONS_BUTTON_ID);
			m_pButton->SetIcon(IDI_ICON_BUTTON);
//			m_pButton->OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
//			m_pButton->SetFlat(FALSE);
			
		}
	}
	m_hControlItem = hItem;
}


