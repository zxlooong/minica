// WebCertManPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "WebCertManPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebCertManPage property page

IMPLEMENT_DYNCREATE(CWebCertManPage, CPropertyPage)

CWebCertManPage::CWebCertManPage() : CPropertyPage(CWebCertManPage::IDD)
{
	//{{AFX_DATA_INIT(CWebCertManPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dCurrentPage = 1;	//��ǰҳ��
	m_bIsSignleRec = FALSE;
}

CWebCertManPage::~CWebCertManPage()
{
}

void CWebCertManPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebCertManPage)
	DDX_Control(pDX, IDC_SLIDER, m_Slider);
	DDX_Control(pDX, IDC_LIST_ONE, m_ListOne);
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebCertManPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWebCertManPage)
	ON_BN_CLICKED(IDC_BQUERY, OnBquery)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ONE, OnDblclkListOne)
	ON_BN_CLICKED(IDC_BCVIEW, OnBcview)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BITMAPSLIDER_MOVED, OnBitmapSliderMoved)
	ON_MESSAGE(WM_BITMAPSLIDER_MOVING, OnBitmapSliderMoving)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebCertManPage message handlers

BOOL CWebCertManPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_Image.Create(16,16,TRUE|ILC_COLOR24,20,1);
	//24��ÿ��ͼƬ�Ŀ��Ϊ24
	//24��ÿ��ͼƬ�ĸ߶�Ϊ24
	//TRUE  ��ͼ��͸����ʽ��ʾ
	//ILC_COLOR24,  ��ɫ��Ϊ24λ��������ѡ��������й�API)
	//16:    ����ʱ����Ϊ3��ͼƬ
	//1:    ͼƬ����������ǰ����ʱ���Զ����ݣ�ÿ������1��ͼƬ������

	HICON hIcon =  NULL;

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_ICON_YELLO), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//0 - ����
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_BLUE), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//1   �� �������
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_GREEN), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//2   �� �����ɹ�
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_RED), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//3   �� �Ѿ�����
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_LIST), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//4   �� ���¼��Ŀ,��չ������
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_MROOT), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//5   �� ����¼��Ŀ
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CLOCK), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//6   �� ʱ��
	DestroyIcon(hIcon);

	// TODO: Add extra initialization here
	//��ʼ����ҳ�б�
	m_List.InsertColumn(0,MiniCT_1100,LVCFMT_LEFT,80);					//MiniCT_1100 "ID"
//	m_List.InsertColumn(1,CMiniCaApp::NormalCode("��������Ϣ"),LVCFMT_LEFT,150);
	m_List.InsertColumn(1,MiniCT_1101,LVCFMT_LEFT,60);			//MiniCT_1101 "��Կ����"
	m_List.InsertColumn(2,MiniCT_1102,LVCFMT_LEFT,50);				//MiniCT_1102 "��Ч��"
	m_List.InsertColumn(3,MiniCT_1103,LVCFMT_LEFT,80);			//MiniCT_1103 "�ü�����"
	m_List.InsertColumn(4,MiniCT_1104,LVCFMT_LEFT,40);				//MiniCT_1104 "����"
	m_List.InsertColumn(5,MiniCT_1105,LVCFMT_LEFT,40);				//MiniCT_1105 "״̬"
	m_List.InsertColumn(6,MiniCT_1106,LVCFMT_LEFT,100);			//MiniCT_1106 "����ʱ��"
//	m_List.InsertColumn(6,CMiniCaApp::NormalCode("��Կ��;"),LVCFMT_LEFT,100);
//	m_List.InsertColumn(7,CMiniCaApp::NormalCode("��չ��;"),LVCFMT_LEFT,100);
//	m_List.SetExtendedStyle(LVS_EX_UNDERLINECOLD|LVS_EX_FULLROWSELECT);
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_List.SetImageList(&m_Image, LVSIL_SMALL);

	//��ʼ����ҳ�б�
	m_ListOne.SetImageList(&m_Image, LVSIL_SMALL);
	m_ListOne.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_ListOne.InsertColumn(0, MiniCT_1107, LVCFMT_LEFT, 200);	//MiniCT_1107 "������Ŀ"
	m_ListOne.InsertColumn(1, MiniCT_1108, LVCFMT_LEFT, 250);		//MiniCT_1108 "����"
	AddList();

	//������ʾ�б�
	if(m_bIsSignleRec)
	{
		GetDlgItem(IDC_LIST_ONE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_LIST_ONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST)->ShowWindow(SW_SHOW);
	}


	m_Slider.SetBitmapChannel( IDB_BITSLIDER, IDB_BITSLIDER, TRUE);	//͸�� Ҫ͸���Ĳ����ú�ɫ��ʾ
	m_Slider.SetBitmapThumb( IDB_BITMAP2, IDB_BITMAP3, TRUE);//͸��
	m_Slider.DrawFocusRect( FALSE );

	m_Slider.SetPageSize(1);
	m_Slider.Enable(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CWebCertManPage::OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam/*��ǰֵ*/)
{
	switch( wParam )
	{
	case IDC_SLIDER :
		int pos = m_Slider.GetPos();
		if(m_dCurrentPage == pos)
			break;
		m_dCurrentPage = pos;
		GoToPage(m_dCurrentPage);
		break;
	}
	return 0;
}

LRESULT CWebCertManPage::OnBitmapSliderMoving(WPARAM wParam, LPARAM lParam/*��ǰֵ*/)
{
	switch( wParam )
	{
	case IDC_SLIDER :
		int pos = m_Slider.GetPos();
		if(m_dCurrentPage == pos)
			break;
		m_dCurrentPage = pos;
		GoToPage(m_dCurrentPage);
		break;
	}
	return 0;
}


void CWebCertManPage::OnBquery() 
{
	// TODO: Add your control notification handler code here
	////////////�������ݿ�///////////////////////////////////////////////////////////
	CString lpszFile = ((CMiniCaApp *)AfxGetApp())->GetAppPath() + "\\MiniCA.mdb";

	CString strConnection ;
	strConnection.Format("Provider=Microsoft.Jet.OLEDB.4.0;\
		Data Source=%s;Persist Security Info=False;Jet OLEDB:Database Password=hpxs", lpszFile);
	
	try
	{
		m_pDb.Open(strConnection);
		m_pRs = CADORecordset(&m_pDb);//ID,USERINFO,KEYLEN,DAY,FRINEDNAME,CERTTYPE,KEYUSAGE,KEYEUSAGE,CERTSTATE,INPUTTIME,MADETIME, REVOKETIME
		m_pRs.Open("Select * From WebCert Order by ID", CADORecordset::openQuery); //adOpenDynamic

		if(!m_pRs.IsEof())		//���ڼ�¼
		{
			m_Slider.Enable();
			m_dCurrentPage = 1;
			GoToPage(m_dCurrentPage);
		}

	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		return;
	}
}

void CWebCertManPage::GoToPage(int nPageIndex, CString strUserId) //strIdֻ�ǵ���¼->���¼ʱ��,���ڸ������¼�ĵ�ǰ��Ŀ��
{
	try
	{
		if(!m_pRs.IsOpen())
			return;
		
		if(m_bIsSignleRec)
			m_pRs.SetPageSize(Single_RecodePerPage);	//����ÿҳ���ݸ���
		else
			m_pRs.SetPageSize(Multi_RecodePerPage);	//����ÿҳ���ݸ���

		DWORD dPageCount = m_pRs.GetPageCount();
		
		if(nPageIndex < 1)
		{
			m_dCurrentPage = 1;
			return;
		}
		else if( nPageIndex > dPageCount)
		{
			m_dCurrentPage = dPageCount;
			return;
		}

		//���û��췶Χ
		m_Slider.SetRange(1, dPageCount);
		m_Slider.SetPos(m_dCurrentPage);
		CString strText;
		strText.Format("%u/%u", m_dCurrentPage, dPageCount);
		SetDlgItemText(IDC_RECORD, strText);

//		m_Slider.SetMargin( 2, 3, 2, 0 );

		
		m_pRs.SetAbsolutePage(nPageIndex);
		
		int nIndex = -1; //���ڼ�¼���¼�ĵ�ǰ��
		
		if(m_bIsSignleRec)
		{
			m_ListOne.SetRedraw(FALSE);
			m_ListOne.DeleteAll();
			AddList();
		}
		else
		{
			nIndex = m_List.GetSelectionMark();	//��¼��ǰѡ�е���Ŀ,��ҳʱ���������
			m_List.SetRedraw(FALSE);
			m_List.DeleteAllItems();
		}
		
		CString strUserInfo,
			strId,
			strKeyLen,
			strDay,
			strFrendName,
			strCertType,
			strCertState,
			strKeyUsage,
			strEkeyUsage,
			strInputTime,
			strMadeTime,
			strRevokeTime;
		
		for(register int nRecNumber = 0; nRecNumber < m_pRs.GetPageSize(); nRecNumber++)
		{
			m_pRs.GetFieldValue("ID", strId);
			m_pRs.GetFieldValue("USERINFO", strUserInfo);
			m_pRs.GetFieldValue("KEYLEN", strKeyLen);
			m_pRs.GetFieldValue("DAY", strDay);
			m_pRs.GetFieldValue("FRINEDNAME", strFrendName);
			m_pRs.GetFieldValue("CERTTYPE", strCertType);
			m_pRs.GetFieldValue("KEYUSAGE", strKeyUsage);
			m_pRs.GetFieldValue("KEYEUSAGE", strEkeyUsage);
			m_pRs.GetFieldValue("CERTSTATE", strCertState);
			m_pRs.GetFieldValue("INPUTTIME", strInputTime);
			m_pRs.GetFieldValue("MADETIME", strMadeTime);
			m_pRs.GetFieldValue("REVOKETIME", strRevokeTime);
			
			int uCertState = 0;
			m_pRs.GetFieldValue("CERTSTATE", uCertState);	//֤��״̬�ֱ��Ӧ������ͼ��
			
			if(!m_bIsSignleRec)
			{
				//m_List����********************************begin*******************************
				
				m_List.InsertItem(0, strId, uCertState);
				//		m_List.SetItemText(0, 1, strUserInfo);
				m_List.SetItemText(0, 1, strKeyLen);
				m_List.SetItemText(0, 2, strDay);
				m_List.SetItemText(0, 3, strFrendName);
				m_List.SetItemText(0, 4, strCertType);
				m_List.SetItemText(0, 5, strCertState);
				m_List.SetItemText(0, 6, strInputTime);
				//		m_List.SetItemText(0, 6, strKeyUsage);
				//		m_List.SetItemText(0, 7, strEkeyUsage);
				//m_List����*********************************end*******************************
			}
			else
			{
				//m_ListOne����*****************************begin***********************************
				UINT nCount = 0;
				CString strSub,
					strLeft,
					strRight;
				
				//�޸ĸ���
				int iGetCurIndex = m_ListOne.GetCurIndex(m_pRootItem);
				m_ListOne.SetItemText(iGetCurIndex, 0, strId);
				CItemInfo *lp = m_ListOne.GetData(m_pRootItem);
				if(lp!=NULL)
				{
					lp->SetImage(uCertState);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				LVITEM lvItem;
				lvItem.mask =  LVIF_IMAGE; 
				lvItem.iItem = 0;
				lvItem.iImage = uCertState;
				m_ListOne.SetItem(&lvItem);				//����ͼ��
				
				
				while(AfxExtractSubString(strSub, strUserInfo, nCount++, ' '))	//�ֽ��û���Ϣ strUserInfo
				{	
					CItemInfo* lpItemInfoi = new CItemInfo();
					
					int n = strSub.Find('=');
					strLeft = strSub.Left(n);
					strRight = strSub.Right(strSub.GetLength() - n -1);
					lpItemInfoi->SetItemText(strLeft);
					lpItemInfoi->SetSubItemText(0, strRight);
					
					lpItemInfoi->SetImage(5);
					m_ListOne.InsertItem(m_ItemInfo[USERINFO], lpItemInfoi);
				}
				
				nCount = 0;
				while(AfxExtractSubString(strSub, strKeyUsage, nCount++, ' '))	//�ֽ���Կ��; strKeyUsage
				{
					CItemInfo* lpItemInfoi = new CItemInfo();
					lpItemInfoi->SetItemText(strSub);
					lpItemInfoi->SetImage(5);
					m_ListOne.InsertItem(m_ItemInfo[KEYUSAGE], lpItemInfoi);
				}
				
				nCount = 0;
				while(AfxExtractSubString(strSub, strEkeyUsage, nCount++, ' '))	//�ֽ���չ��Կ��; strEkeyUsage
				{
					CItemInfo* lpItemInfoi = new CItemInfo();
					lpItemInfoi->SetItemText(strSub);
					lpItemInfoi->SetImage(5);
					m_ListOne.InsertItem(m_ItemInfo[EKEYSUAGE], lpItemInfoi);
				}
				
				//��Ӻü�����
				lp = m_ListOne.GetData(m_ItemInfo[FRINEDNAME]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strFrendName);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[FRINEDNAME]);
				m_ListOne.SetItemText(iGetCurIndex, 1, strFrendName);
				
				//�����Կ����
				lp = m_ListOne.GetData(m_ItemInfo[KEYLEN]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strKeyLen);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[KEYLEN]);
				m_ListOne.SetItemText(iGetCurIndex, 1, strKeyLen);
				
				//�������ʱ��
				lp = m_ListOne.GetData(m_ItemInfo[INPUTIME]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strInputTime);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[INPUTIME]);
				m_ListOne.SetItemText(iGetCurIndex, 1, strInputTime);
				
				//�����Ч��
				lp = m_ListOne.GetData(m_ItemInfo[CERTDAY]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strDay);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[CERTDAY]);
				m_ListOne.SetItemText(iGetCurIndex, 1, strDay);
				
				//���֤������
				lp = m_ListOne.GetData(m_ItemInfo[CERTYPE]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strCertType);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[CERTYPE]);
				m_ListOne.SetItemText(iGetCurIndex, 1, strCertType);
				
				//���֤��״̬
				lp = m_ListOne.GetData(m_ItemInfo[CERTSTATE]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strCertState);
					lp->SetImage(uCertState);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[CERTSTATE]);
				
				lvItem.iItem = iGetCurIndex;
				lvItem.iImage = uCertState;
				
				m_ListOne.SetItem(&lvItem);				//����ͼ��
				m_ListOne.SetItemText(iGetCurIndex, 1, strCertState);
				
				
				//���֤������ʱ��
				lp = m_ListOne.GetData(m_ItemInfo[MADETIME]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strMadeTime);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[MADETIME]);
				m_ListOne.SetItemText(iGetCurIndex, 1, strMadeTime);
				
				//���֤������ʱ��
				lp = m_ListOne.GetData(m_ItemInfo[REVOKETIME]);
				if(lp!=NULL)
				{
					lp->SetSubItemText(0, strRevokeTime);
					//m_ListOne.UpdateData(m_ItemInfo[FRINEDNAME], lp); //update internal nodes
				}
				iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[REVOKETIME]);
				m_ListOne.SetItemText(iGetCurIndex, 1, strRevokeTime);
		}
		//m_ListOne����******************************end*********************************
		m_pRs.MoveNext();
		
		if(m_pRs.IsEof())
			break;
	}
	if(!m_bIsSignleRec)
	{
		//m_List����**********************
		//��λ����ǰ��¼nPageIndex
		if(!strUserId.IsEmpty())	//˵����ҳ���л�,ֻ��ҳ���л�ʱ��STRUSERID�Ų�Ϊ��
		{
			LVFINDINFO info;
			info.flags = LVFI_STRING;
			info.psz = strUserId;
			nIndex = m_List.FindItem(&info);
		}
		if(nIndex != -1)
		{
			m_List.SetFocus();									//����ѡ�� 2 - 1
			m_List.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);  //����ѡ�� 2 - 2
			m_List.SetSelectionMark(nIndex);
		}
		m_List.SetRedraw();
	}
	else
		m_ListOne.SetRedraw();
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		return;
	}
	m_Slider.SetFocus();

}

void CWebCertManPage::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CWebCertManPage::OnDblclkListOne(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ListOne.GetSelectedItem();
	if(nIndex == -1)
		return;
	CSuperGridCtrl::CTreeItem * pItem = m_ListOne.GetTreeItem(nIndex);
	if(!pItem)
		return;

	BOOL bRedraw=0;
	if(m_ListOne.ItemHasChildren(pItem))
	{
		m_ListOne.SetRedraw(0);
		int nScrollIndex=0;
		if(m_ListOne.IsCollapsed(pItem))
		{		
			if(m_ListOne.OnItemExpanding(pItem, nIndex))
			{
				nScrollIndex = m_ListOne.Expand(pItem, nIndex);
				m_ListOne.OnItemExpanded(pItem, nIndex);
				bRedraw=1;
			}
		}	
		
		else 
		{
			if(m_ListOne.OnCollapsing(pItem))
			{
				m_ListOne.Collapse(pItem);
				m_ListOne.OnItemCollapsed(pItem);
				bRedraw=1;
			}
		}
		m_ListOne.SetRedraw(1);
		
		if(bRedraw)
		{
			CRect rc;
			m_ListOne.GetItemRect(nIndex,rc,LVIR_BOUNDS);
			m_ListOne.InvalidateRect(rc);
			m_ListOne.UpdateWindow();
			m_ListOne.EnsureVisible(nScrollIndex,1);
		}
	}//ItemHasChildren	
	*pResult = 0;
}

void CWebCertManPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	try
	{
		if(m_pDb.IsOpen())
			m_pDb.Close();
		if(m_pRs.IsOpen())
			m_pRs.Close();
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		return;
	}

}

void CWebCertManPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)AfxGetMainWnd())->AddMsg(MiniCT_0003,info, type);		//MiniCT_0003
}

void CWebCertManPage::AddList()
{
	CItemInfo* lpRoot = new CItemInfo();
	lpRoot->SetImage(0);
	lpRoot->SetItemText(_T(MiniCT_1108));	//MiniCT_1108
	m_pRootItem = m_ListOne.InsertRootItem(lpRoot);//previous on N.Y.P.D we call it CreateTreeCtrl(lp)
	if( m_pRootItem == NULL )
		return;
	
	CStringArray strArray;
	strArray.Add(MiniCT_1109);	//USERINFO 0					//MiniCT_1109
	strArray.Add(MiniCT_1110);	//KEYUSAGE 1					//MiniCT_1110 "��Կ��;"
	strArray.Add(MiniCT_1111);	//EKEYSUAGE 2					//MiniCT_1111 "��չ��;"
	strArray.Add(MiniCT_1103);	//FRINEDNAME 3					//MiniCT_1103 "�ü�����"
	strArray.Add(MiniCT_1101);	//KEYLEN 4						//MiniCT_1101 "��Կ����"
	strArray.Add(MiniCT_1106);	//INPUTIME 5					//MiniCT_1106 "����ʱ��"
	strArray.Add(MiniCT_1102);		//CERTDAY 6						//MiniCT_1102 "��Ч��"
	strArray.Add(MiniCT_1112);	//CERTYPE 7						//MiniCT_1112 "֤������"
	strArray.Add(MiniCT_1113);		//CERTSTATE 8				//MiniCT_1113 "֤��״̬"
	strArray.Add(MiniCT_1114);		//MADETIME 9				//MiniCT_1114 "����ʱ��"
	strArray.Add(MiniCT_1115);		//REVOKETIME 10				//MiniCT_1115 "����ʱ��"
	
	for(int i = 0; i < ReqMaxEntry; i++)
	{
		CItemInfo* lpItemInfoi = new CItemInfo();
		lpItemInfoi->SetItemText(strArray[i]);
		if(i < FRINEDNAME)
		{
			lpItemInfoi->SetImage(4);	
		}
		else if(i!= 5 && i != 9 && i != 10)
			lpItemInfoi->SetImage(5);	
		else 
			lpItemInfoi->SetImage(6);	
		m_ItemInfo[i] = m_ListOne.InsertItem(m_pRootItem, lpItemInfoi, TRUE);
	}
}

void CWebCertManPage::OnBcview() 
{
	// TODO: Add your control notification handler code here
	m_bIsSignleRec = !m_bIsSignleRec;	
	if(m_bIsSignleRec)//���¼->����¼
	{
		GetDlgItem(IDC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_ONE)->ShowWindow(SW_SHOW);
		int nIndex = m_List.GetSelectionMark();
		if(nIndex != -1)
		{
			//ȡ��֤��ID
			CString strID = m_List.GetItemText(nIndex, 0);
			if(!strID.IsEmpty())
			{
				CString strSql;
				strSql.Format("ID = %s", strID);
				try
				{
					m_pRs.FindFirst(strSql);
					_variant_t  var = m_pRs.GetBookFind();
					m_dCurrentPage = var.dblVal;
				}
				catch(CADOException & eAdo)
				{
					AddMsg(eAdo.GetErrorMessage(), M_ERROR);
					return;
				}

			}
		}
		else
		{
			m_dCurrentPage = 1;
		}
		GoToPage(m_dCurrentPage);

	}
	else//����¼->���¼
	{
		GetDlgItem(IDC_LIST_ONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST)->ShowWindow(SW_SHOW);
		int iGetCurIndex = m_ListOne.GetCurIndex(m_pRootItem);
		CString strId = m_ListOne.GetItemText(iGetCurIndex, 0);
		
		if(strId.GetAt(0) >'0' && strId.GetAt(0) <= '9')	//˵��Ϊ����
		{
			CString strSql;
			strSql.Format("ID = %s", strId);
			UINT uIndex = 1;
			try
			{
				m_pRs.FindFirst(strSql);
				_variant_t  var = m_pRs.GetBookFind();
				if(var.vt)
					uIndex = var.dblVal;;
			}
			catch(CADOException & eAdo)
			{
				AddMsg(eAdo.GetErrorMessage(), M_ERROR);
				return;
			}

			m_dCurrentPage = (uIndex - 1)/Multi_RecodePerPage + 1;
			GoToPage(m_dCurrentPage, strId);
		}
	}
}

void CWebCertManPage::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//�ж��Ƿ�ѡ��֤��
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//�жϵ�ʱѡ�м���֤��
	UINT nSelect  =  m_List.GetSelectedCount();
	//�õ����ֵ,���������Ϊѡ��֤��
	CString strID = m_List.GetItemText(pNMListView->iItem,0);
	if(strID.IsEmpty())
	{
		//�ж��Ƿ�ѡ��������֤��
		//		CString strName = m_List.GetItemText(pNMListView->iItem, 0);
		//		if(strName == "������֤��")
		{
			//			AfxMessageBox("������");
		}
		return;
	}
	CPoint pt;
	GetCursorPos(&pt); // ��ǰ�������
	//	if(GetCount()<=0)
	//		return;
	BCMenu m_PopMenu;
	m_PopMenu.LoadMenu(IDR_MENU_WEBDBMEN);
	m_PopMenu.LoadToolbar(IDR_MINICAMENU);

	CLanguage::TranslateMenu(&m_PopMenu, MAKEINTRESOURCE(IDR_MENU_WEBDBMEN));

	CMenu * pPopup = m_PopMenu.GetSubMenu(0);
	SetForegroundWindow(); //������Թر�
	ASSERT(pPopup);
	if(nSelect > 1)
		pPopup->EnableMenuItem(ID_MENUITEM_VIEWCERT,MF_GRAYED | MF_BYCOMMAND);//���ѡ�����������β鿴֤��ѡ��
	UINT nSelection = pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|
		TPM_NONOTIFY|TPM_RETURNCMD,pt.x, pt.y,this, NULL);
	m_PopMenu.DestroyMenu();

	try
	{

		if(nSelection == ID_MENUITEM_MADECSR)//������csr,����,����֤������
		{
			MadeCsrOnly(strID);
		}
		else if(nSelection == ID_MENUITEM_MADE)//����(��ǰ�������,��������,Ϊ����CSR)
		{
			MadeCert(strID, m_pRs);
		}
		else if(nSelection == ID_MENUITEM_MARKCERT)//�������
		{
			MarkCert(strID);
		}
		else if(nSelection == ID_MENUITEM_MARKCANCLE)//ȡ�����
		{
			CancelMark(strID);
		}
		else if(nSelection == ID_MENUITEM_BATCHMADE)//��������
		{
			BatchCert();
		}
		else if(nSelection == ID_MENUITEM_REVOKEPFX)//����PFX
		{
			RevokeCert(strID);
		}
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		return;
	}
	*pResult = 0;
}

BOOL CWebCertManPage::GetRecordset(CString strId, CADORecordset & pRs)
{
	try
	{
		if(!pRs.IsOpen())
			return FALSE;
		CString strSql;
		strSql.Format("ID = %s", strId);
		pRs.FindFirst(strSql);
		_variant_t  var = pRs.GetBookFind();
		UINT uIndex = 1;
		if(var.vt)
			uIndex = var.dblVal;;
		pRs.SetPageSize(Single_RecodePerPage);	//����ÿҳ���ݸ���
		pRs.SetAbsolutePage(uIndex);
		return TRUE;
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		return FALSE;
	}

}

void CWebCertManPage::MadeCsrOnly(CString strID)	//������CSR
{
	//�õ���ǰ��¼��ID��
	if(!GetRecordset(strID, m_pRs))
		return;
	CString strUserInfo, strUserPwd;
	m_pRs.GetFieldValue("USERINFO", strUserInfo);
	m_pRs.GetFieldValue("QUERYPWD", strUserPwd);
	
	long lCertLen = 0;
	m_pRs.GetFieldValue("KEYLEN", lCertLen);


	char reqBuf[10240] = {0};
	char keyBuf[10240] = {0};
	char outMsg[256] = {0};
	
	UINT reqLen = 10240,
		keyLen = 10240;
	
	CString strSub,
		strLeft,
		strRight;
	stuSUBJECT *pCERT =	NULL;
	int nCount = 0;
	while(AfxExtractSubString(strSub, strUserInfo, nCount++, ' '))	//�ֽ��û���Ϣ strUserInfo Afx �ֽ�  �ָ�
	{	
		int n = strSub.Find('=');
		strLeft = strSub.Left(n);
		strRight = strSub.Right(strSub.GetLength() - n -1);
		pCERT->Add(pCERT, strLeft, strRight);
	}
	
	BOOL bReq = MakeReq(pCERT, lCertLen, strUserPwd.GetBuffer(0),
		reqBuf, &reqLen, keyBuf, &keyLen, outMsg, PEM);
	pCERT->RemoveAll(pCERT); //�ͷ�֤��ṹ
	strUserPwd.ReleaseBuffer();
	
	if(bReq)
	{
		m_pRs.Edit();
		m_pRs.AppendChunk("CSR", reqBuf, reqLen);
		m_pRs.AppendChunk("KEY", keyBuf, keyLen);
		
		m_pRs.SetFieldValue("CERTSTATE", MADECSR);	//�Ѿ�����CSR
		m_pRs.Update();
		
	}
	else
	{
		AddMsg(outMsg, M_ERROR);
	}
	
}

void CWebCertManPage::MarkCert(CString strID)
{
	//�õ���ǰ��¼��ID��
	if(!GetRecordset(strID, m_pRs))
		return;
	m_pRs.Edit();
	m_pRs.SetFieldValue("CERTSTATE", MARKCERT);	
	m_pRs.Update();
}

void CWebCertManPage::CancelMark(CString strID)
{
	//�õ���ǰ��¼��ID��
	if(!GetRecordset(strID, m_pRs))
		return;
	m_pRs.Edit();
	//�ж�CSR�ֶ��Ƿ�Ϊ��
	//���Ϊ��
	if(m_pRs.IsFieldNull("CSR"))
	{
		m_pRs.SetFieldValue("CERTSTATE", REQ);
	}
	else//����
	{
		m_pRs.SetFieldValue("CERTSTATE", MADECSR);
	}
	m_pRs.Update();
}


void CWebCertManPage::MadeCert(CString strID, CADORecordset & pRs)
{
	//�õ���ǰ��¼��ID��
	if(!GetRecordset(strID, pRs))
		return;
	CString strUserInfo,
		strUserPwd,
		strDay,
		strFrendName,
		strKeyUsage,
		strEkeyUsage;
	
	long lCertLen = 0;
	long lCertDay = 0;
	long lCertId = 0;

	pRs.GetFieldValue("ID", lCertId);
	pRs.GetFieldValue("KEYLEN", lCertLen);
	pRs.GetFieldValue("DAY", lCertDay);

	pRs.GetFieldValue("USERINFO", strUserInfo);
	pRs.GetFieldValue("QUERYPWD", strUserPwd);
	pRs.GetFieldValue("FRINEDNAME", strFrendName);
	pRs.GetFieldValue("KEYUSAGE", strKeyUsage);
	pRs.GetFieldValue("KEYEUSAGE", strEkeyUsage);
	
	
	char reqBuf[10240] = {0};
	char certBuf[10240] = {0};
	char keyBuf[10240] = {0};
	char pfxBuf[10240] = {0};
	char outMsg[256] = {0};
	
	UINT reqLen = 10240,
		keyLen = 10240,
		certLen = 10240,
		pfxLen = 10240;
	
	CString strSub,
		strLeft,
		strRight;

	stuSUBJECT *pCERT =	NULL;
	int nCount = 0;

	while(AfxExtractSubString(strSub, strUserInfo, nCount++, ' '))	//�ֽ��û���Ϣ strUserInfo
	{	
		int n = strSub.Find('=');
		strLeft = strSub.Left(n);
		strRight = strSub.Right(strSub.GetLength() - n -1);
		pCERT->Add(pCERT, strLeft, strRight);
	}
	
	BOOL bReq = MakeReq(pCERT, lCertLen, strUserPwd.GetBuffer(0),
		reqBuf, &reqLen, keyBuf, &keyLen, outMsg, PEM);
	pCERT->RemoveAll(pCERT); //�ͷ�֤��ṹ
	strUserPwd.ReleaseBuffer();
	
	if(bReq)	//����֤��
	{
		DWORD lenCert=0,lenKey=0;//��Կ����,˽Կ����
		CString strPwd;//��˽Կ·��������,p12��Կ
		char strCert[10240] = {0};
		char strKey[10240] = {0};
		
		if(((CMiniCaApp *)AfxGetApp())->GetRootCert(strCert, lenCert, strKey, lenKey,strPwd))
		{
			stuCertPair RootPair(strCert,lenCert,strKey,lenKey,
				strPwd.GetBuffer(0));
			
			strPwd.ReleaseBuffer();
			
			BOOL bCert = MakeCert(RootPair,	lCertId, 0, lCertDay,
				reqBuf, reqLen,	strKeyUsage.GetBuffer(0),
				strEkeyUsage.GetBuffer(0), NULL, certBuf,
				&certLen, outMsg, PEM);
			
			char * pPwd = strUserPwd.GetBuffer(0);
			
			if(bCert)	//�ϲ�P12
			{
				BOOL bPfx = CreatePfx(pfxBuf, pfxLen, pPwd,
					strFrendName.GetBuffer(0),/*IN �ü�����*/
					certBuf, certLen,
					keyBuf, keyLen, pPwd, outMsg);
				strUserPwd.ReleaseBuffer();
				
				if(bPfx)
				{
					pRs.Edit();
					pRs.AppendChunk("CSR", reqBuf, reqLen);
					pRs.AppendChunk("KEY", keyBuf, keyLen);
					pRs.AppendChunk("CERT", certBuf, certLen);
					pRs.AppendChunk("P12", pfxBuf, pfxLen);
					
					pRs.SetFieldValue("CERTSTATE", MADECERT);	//�Ѿ�����CERT
					
					COleDateTime dataTiem = COleDateTime::GetCurrentTime();
					
					pRs.SetFieldValue("MADETIME", dataTiem);
					
					pRs.Update();
					
					AddMsg(MiniCT_0124);		//MiniCT_0124 "����֤��ɹ�"
					
				}
				else
				{
					AddMsg(outMsg, M_ERROR);
				}
			}		
			else
			{
				AddMsg(outMsg, M_ERROR);
			}
		}
		else
		{
			AddMsg(MiniCT_0123, M_ERROR);		//MiniCT_0123 "ȡ�ø�֤��ʧ��"
		}
	}
	else
	{
		AddMsg(outMsg, M_ERROR);
	}
	
}

void CWebCertManPage::RevokeCert(CString strID)
{
	//�õ���ǰ��¼��ID��
	if(!GetRecordset(strID, m_pRs))
		return;
	m_pRs.Edit();
	//�ж�P12�ֶ��Ƿ�Ϊ��
	//���Ϊ��
	if(!m_pRs.IsFieldNull("P12"))
	{
		m_pRs.SetFieldValue("CERTSTATE", REVOKE);
	}
}

void CWebCertManPage::BatchCert()
{
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		if(pRs->Open("Select * From WebCert Where CERTSTATE = 2 Order by ID", CADORecordset::openQuery))
		{
			for(;;)
			{
				CString strID;
				pRs->GetFieldValue("ID", strID);
				MadeCert(strID, * pRs);
				pRs->MoveNext();
					
				if(pRs->IsEof())
					break;
			}
			pRs->Close();
		}
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
	}
	delete pRs;
}

void CWebCertManPage::TranslateCT()
{
	CHeaderCtrl * pHeader = m_List.GetHeaderCtrl();	//��ҳ
	if (pHeader)
	{
		CString str = MiniCT_1100;
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(0, &hdi);

		str = MiniCT_1101;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(1, &hdi);

		str = MiniCT_1102;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(2, &hdi);

		str = MiniCT_1103;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(3, &hdi);

		str = MiniCT_1104;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(4, &hdi);

		str = MiniCT_1105;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(5, &hdi);

		str = MiniCT_1106;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(6, &hdi);
	}

	pHeader = m_ListOne.GetHeaderCtrl();	//��ҳ
	if (pHeader)
	{
		CString str = MiniCT_1107;
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(0, &hdi);

		str = MiniCT_1108;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(1, &hdi);
	}
	SetDlgItemText(IDC_BCVIEW, MiniCT_11801);
	SetDlgItemText(IDC_BQUERY, MiniCT_11802);

	//���µ�ҳ������
	int nCountItem = m_ListOne.NumChildren(m_pRootItem);
	CStringArray strArray;
	strArray.Add(MiniCT_1109);	//USERINFO 0					//MiniCT_1109
	strArray.Add(MiniCT_1110);	//KEYUSAGE 1					//MiniCT_1110 "��Կ��;"
	strArray.Add(MiniCT_1111);	//EKEYSUAGE 2					//MiniCT_1111 "��չ��;"
	strArray.Add(MiniCT_1103);	//FRINEDNAME 3					//MiniCT_1103 "�ü�����"
	strArray.Add(MiniCT_1101);	//KEYLEN 4						//MiniCT_1101 "��Կ����"
	strArray.Add(MiniCT_1106);	//INPUTIME 5					//MiniCT_1106 "����ʱ��"
	strArray.Add(MiniCT_1102);		//CERTDAY 6						//MiniCT_1102 "��Ч��"
	strArray.Add(MiniCT_1112);	//CERTYPE 7						//MiniCT_1112 "֤������"
	strArray.Add(MiniCT_1113);		//CERTSTATE 8				//MiniCT_1113 "֤��״̬"
	strArray.Add(MiniCT_1114);		//MADETIME 9				//MiniCT_1114 "����ʱ��"
	strArray.Add(MiniCT_1115);		//REVOKETIME 10				//MiniCT_1115 "����ʱ��"

	for(int i = 0; i < ReqMaxEntry && i< nCountItem; i++)
	{
		int iGetCurIndex = m_ListOne.GetCurIndex(m_ItemInfo[i]);
		m_ListOne.SetItemText(iGetCurIndex, 0, strArray[i]);
	}
}

