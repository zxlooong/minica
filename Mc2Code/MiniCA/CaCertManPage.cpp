// CaCertManPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CaCertManPage.h"
#include "MiniMainDlg.h"
#include "CaCertIniSetPage.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCertManPage property page

IMPLEMENT_DYNCREATE(CCaCertManPage, CPropertyPage)

CCaCertManPage::CCaCertManPage() : CPropertyPage(CCaCertManPage::IDD)
{
	//{{AFX_DATA_INIT(CCaCertManPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pRootItem = NULL;
	m_pRevokwItem = NULL;
	m_bDbReady = TRUE;
}

CCaCertManPage::~CCaCertManPage()
{
}

void CCaCertManPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCertManPage)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCaCertManPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCertManPage)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCertManPage message handlers

BOOL CCaCertManPage::IsDbReady()
{
	return m_bDbReady;
}

BOOL CCaCertManPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_Image.Create(16,16,TRUE|ILC_COLOR24,20,1);
	//24：每个图片的宽度为24
	//24：每个图片的高度为24
	//TRUE  该图以透明方式显示
	//ILC_COLOR24,  颜色数为24位（其他可选常量祥见有关API)
	//16:    创建时容量为3个图片
	//1:    图片数量超出当前容量时，自动扩容，每次扩容1个图片的容量

	
	HICON hIcon =  NULL;

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_ENC), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//0 - 有效
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_REVOKE), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//1   － 作废
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_KEY2), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//2   － 目录
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERT), IMAGE_ICON, 16, 16, 0);
	m_Image.Add(hIcon);//3   － 证书
	DestroyIcon(hIcon);
	
	m_List.SetImageList(&m_Image, LVSIL_SMALL);

	m_List.SetExtendedStyle(LVS_EX_GRIDLINES);

	m_List.InsertColumn(0, MiniCT_0600, LVCFMT_LEFT, 130);	//MiniCT_0600 "证书分类"
	m_List.InsertColumn(1, MiniCT_0601, LVCFMT_LEFT, 40);		//MiniCT_0601 "序号"
	m_List.InsertColumn(2, MiniCT_0602, LVCFMT_LEFT, 40);		//MiniCT_0602 "密钥"
	m_List.InsertColumn(3, MiniCT_0603, LVCFMT_LEFT, 120);	//MiniCT_0603 "用户信息"
	m_List.InsertColumn(4, MiniCT_0604, LVCFMT_LEFT, 70);	//MiniCT_0604 "起始时间"
	m_List.InsertColumn(5, MiniCT_0605, LVCFMT_LEFT, 50);		//MiniCT_0605 "有效期"


	CItemInfo* lpRoot = new CItemInfo();
	lpRoot->SetImage(0);
	lpRoot->SetItemText(_T(MiniCT_0606));					//MiniCT_0606 "已颁发证书"
	m_pRootItem = m_List.InsertRootItem(lpRoot);//previous on N.Y.P.D we call it CreateTreeCtrl(lp)
	if( m_pRootItem == NULL )
		return 0;

	CItemInfo* lpRevoke = new CItemInfo();
	lpRevoke->SetImage(1);
	lpRevoke->SetItemText(_T(MiniCT_0607));				//MiniCT_0607 "已作废证书"
	m_pRevokwItem = m_List.InsertRootItem(lpRevoke);//previous on N.Y.P.D we call it CreateTreeCtrl(lp)

	////////////连接数据库///////////////////////////////////////////////////////////
	CString lpszFile = ((CMiniCaApp *)AfxGetApp())->GetAppPath() + "\\MiniCA.mdb";

	CString connect;
	connect.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;\
		Persist Security Info=False;Jet OLEDB:Database Password=hpxs",lpszFile);
	try
	{
		m_pDb.Open(connect);
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		m_bDbReady = FALSE;
		return FALSE;
	}

	if(GetCertType())
		QueryCert(0);

	m_toolTip.Create(this);
	m_toolTip.AddTool(GetDlgItem(IDC_LIST), "已颁发证书树列");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCaCertManPage::GetCertType()//查询数据库里面CERTTYPE表
{
	BOOL bSucceed = TRUE;
	CString SQL;
	if(CMiniCaApp::IsBig())
	{
		SQL.Format("select * from CERTTYPEbig"); //如果繁体环境,则打开CERTTYPEbig
	}
	else
		SQL.Format("select * from CERTTYPE");

	CADORecordset* pRs = new CADORecordset(&m_pDb);
	
	try
	{
		pRs->Open(SQL, CADORecordset::openQuery);
		int id = 0;
		CString strName;
		while(!pRs->IsEof())//记录
		{ 
			pRs->GetFieldValue("CERT_TYPE", id);
			pRs->GetFieldValue("CERT_NAME", strName);
			
			CItemInfo* lpItemInfo = new CItemInfo();
			lpItemInfo->SetItemText(strName);
			lpItemInfo->SetImage(2);
			stuCerType * pstuCert = new stuCerType;	//最后要销毁
			pstuCert->m_pItem = m_List.InsertItem(m_pRootItem, lpItemInfo, TRUE);
			//				pstuCert->m_pItem = m_List.InsertItem(m_pRevokwItem, lpItemInfo, TRUE);
			pstuCert->m_Type = id;
			m_ObCerType.AddTail(pstuCert);
			pRs->MoveNext();;//转到下一条纪录
		}
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		return FALSE;
	}
	if(pRs->IsOpen())
	{
		pRs->Close();
	}
	delete pRs;
	return bSucceed;
}


void CCaCertManPage::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	//销毁 m_ObCerType
	stuCerType * pstuCert = NULL;
	for(; !m_ObCerType.IsEmpty(); )
	{
		pstuCert = (stuCerType *)m_ObCerType.RemoveHead();
		delete pstuCert;
	}

	if(m_pDb.IsOpen())
		m_pDb.Close();
}

UINT CCaCertManPage::GetCertSn()
{
	int id = 0;
	if(!m_pDb.IsOpen())
		return -1;
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		pRs->Open("Select MAX_SN from CERTSN", CADORecordset::openQuery);
		if(!pRs->IsEof())//存在纪录
		{
			pRs->GetFieldValue("MAX_SN", id);
		}
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
		id = -2;
	}
	if(pRs->IsOpen())
	{
		pRs->Close();
	}
	delete pRs;
	return ++id;
}

void CCaCertManPage::SaveDb(stuSUBJECT * pSUBJECT, const int uCertLen,
							const int iCertType,const int uCertDay, 
							void * p12, const int p12l, CString pwd)
{
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		pRs->Open("CACERT", CADORecordset::openTable);
		pRs->AddNew();
		pRs->SetFieldValue("CERTTYPE", (long)iCertType);
		
		CString strCn = pSUBJECT->GetCN();
		pRs->SetFieldValue("CN", strCn);
		
		pRs->SetFieldValue("DAY", (long)uCertDay);
		
		//添加用户信息
		CString strUserInfo;
		while(pSUBJECT != NULL)//遍历链表
		{
			strUserInfo += pSUBJECT->cInfo;
			strUserInfo += " ";
			pSUBJECT = pSUBJECT->Link;
		}
		pRs->SetFieldValue("USERINFO", strUserInfo);

//		UINT uLen = strUserInfo.GetLength();
//		pRs->AppendChunk("USERINFO", strUserInfo.GetBuffer(0), uLen);
//		strUserInfo.ReleaseBuffer();
		
		pRs->AppendChunk("CERTPFX", p12, p12l);
	
		pRs->SetFieldValue("PFXPWD", pwd);
		
		pRs->SetFieldValue("KEYLEN", (long)uCertLen);
			
		COleDateTime time = COleDateTime::GetCurrentTime();
		pRs->SetFieldValue("BEGINTIME", time);
		
		pRs->Update();
		
		LONG lId = 0;
		pRs->GetFieldValue("ID", lId);//得到id
		SetCertSn(lId);

		//由于新证书制作成功,则数证管理界面中增加此证书
		CString sSN, sKey, sInfo, sDay, sTime;
		sSN.Format("%d", lId);
		sKey.Format("%d", uCertLen);
		sDay.Format("%d", uCertDay);
		sTime = time.Format();
		
		InsertCert(strCn, sSN, sKey, strUserInfo, sTime, sDay, iCertType);
		
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
	}
	if(pRs->IsOpen())
	{
		pRs->Close();
	}
	delete pRs;
}

void CCaCertManPage::SetCertSn(long lSn)	//设置数据库中证书序号
{
	CString SQL;
	SQL.Format("Update CERTSN set MAX_SN = %d",lSn);
	m_pDb.Execute(SQL);
}

BOOL CCaCertManPage::SelectCert(CString strSQL)
{
	BOOL bSucceed = FALSE;
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		pRs->Open(strSQL, CADORecordset::openQuery);
		while(!pRs->IsEof())//记录
		{ 
			CString sCN, 
					sSN,
					sKey,
					sInfo,
					sBegin,
					sDay;
			int Type = 0;

			pRs->GetFieldValue("CN", sCN);
			pRs->GetFieldValue("ID", sSN);
			pRs->GetFieldValue("KEYLEN", sKey);
			pRs->GetFieldValue("USERINFO", sInfo);
			pRs->GetFieldValue("BEGINTIME", sBegin);
			pRs->GetFieldValue("DAY", sDay);
			pRs->GetFieldValue("CERTTYPE", Type);

			InsertCert(sCN, sSN, sKey, sInfo, sBegin, sDay, Type);

			pRs->MoveNext();//转到下一条纪录
		}
		bSucceed = TRUE;
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
	}
	if(pRs->IsOpen())
	{
		pRs->Close();
	}
	delete pRs;
	return bSucceed;
}

BOOL CCaCertManPage::SelectRevoke(CString strSQL)
{
	BOOL bSucceed = FALSE;
	CADORecordset* pRs = new CADORecordset(&m_pDb);
	try
	{
		pRs->Open(strSQL, CADORecordset::openQuery);
		while(!pRs->IsEof())//记录
		{ 
			CString sCN, 
					sSN,
					sKey,
					sInfo,
					sBegin,
					sDay;
			int Type = 0;

			pRs->GetFieldValue("CN", sCN);
			pRs->GetFieldValue("ID", sSN);
			pRs->GetFieldValue("KEYLEN", sKey);
			pRs->GetFieldValue("USERINFO", sInfo);
			pRs->GetFieldValue("BEGINTIME", sBegin);
			pRs->GetFieldValue("DAY", sDay);
			pRs->GetFieldValue("CERTTYPE", Type);

			CItemInfo* lpItemInfoi = new CItemInfo();
			
			lpItemInfoi->SetItemText(sCN);
			lpItemInfoi->SetImage(3);
			//序号,密钥,用户信息,起始时间,有效期
			lpItemInfoi->AddSubItemText(sSN);
			
			lpItemInfoi->AddSubItemText(sKey);

			lpItemInfoi->AddSubItemText(sInfo);

			lpItemInfoi->AddSubItemText(sBegin);
			
			lpItemInfoi->AddSubItemText(sDay);
			
			stuCerType * pstu = NULL;
			
			m_List.InsertItem(m_pRevokwItem, lpItemInfoi);

			pRs->MoveNext();;//转到下一条纪录
		}
		bSucceed = TRUE;
	}
	catch(CADOException & eAdo)
	{
		AddMsg(eAdo.GetErrorMessage(), M_ERROR);
	}
	if(pRs->IsOpen())
	{
		pRs->Close();
	}
	delete pRs;
	return bSucceed;
}

void CCaCertManPage::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//判断是否选中证书
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//判断当时选中几个证书
	UINT nSelect  =  m_List.GetSelectedCount();
	//得到序号值,如果不空认为选中证书
	CString strID = m_List.GetItemText(pNMListView->iItem,1);
	if(strID.IsEmpty())
	{
		//判断是否选中以作废证书
		//		CString strName = m_List.GetItemText(pNMListView->iItem, 0);
		//		if(strName == "已作废证书")
		{
			//			AfxMessageBox("黑名单");
		}
		return;
	}
	CPoint pt;
	GetCursorPos(&pt); // 当前鼠标坐标
	//	if(GetCount()<=0)
	//		return;
	BCMenu m_PopMenu;
	m_PopMenu.LoadMenu(IDR_MENU_DBMEN);
	m_PopMenu.LoadToolbar(IDR_MINICAMENU);

	CLanguage::TranslateMenu(&m_PopMenu, MAKEINTRESOURCE(IDR_MENU_DBMEN));

	CMenu * pPopup = m_PopMenu.GetSubMenu(0);
	SetForegroundWindow(); //点击可以关闭
	ASSERT(pPopup);
	if(nSelect > 1)
		pPopup->EnableMenuItem(ID_MENUITEM_VIEWCERT,MF_GRAYED | MF_BYCOMMAND);//如果选择多个，则屏蔽查看证书选择
	UINT nSelection = pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|
		TPM_NONOTIFY|TPM_RETURNCMD,pt.x, pt.y,this, NULL);
	m_PopMenu.DestroyMenu();
	
	
/*	//循环多个选项,生成SQL语句
	
//	CUIntArray iniArray;
	CString strIDSet;//ID集合
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	while (pos)
	{
		CString strTemp;
		int iSelect = m_List.GetNextSelectedItem(pos);
		strTemp = m_List.GetItemText(iSelect, 1);
		if(!strTemp.IsEmpty())
		{
			strIDSet += strTemp;
			strIDSet += ",";
	//		iniArray.Add(iSelect);
		}
	}
	
	
	UINT m_NameType = 0;
	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	CCertDbPage * pDb = (CCertDbPage *)(pMain->GetPage("CCertDbPage"));
	if(pDb)
	{
		m_NameType = pDb->GetNameType();
	}
	
	
	CString SQL;
	SQL.Format("Select ID,CN,PFXPWD,CERTPFX,CERTSTATE from CACERT Where ID in (%s)",strIDSet);
	//Select PFXPWD,CERTPFX,CERTSTATE from CACERT Where ID in (1,2,3,4,5,6,7,8,9,) 
	UINT id = 0;
	_RecordsetPtr m_pRecordset;
	try
	{
		if (m_pConnection == NULL)
		{
			return;
		}		
		_variant_t Affected;//影响行数
		HRESULT hr = m_pRecordset.CreateInstance(_uuidof(Recordset));
		m_pRecordset->Open(_bstr_t(SQL), _variant_t((IDispatch*)m_pConnection,true),
			adOpenDynamic,adLockOptimistic,adCmdText);//打开表
		
		while(!m_pRecordset->adoEOF)//存在纪录
		{		
			//得到公钥、私钥、pfx文件名称
			_variant_t vt = m_pRecordset->Fields->GetItem("CN")->Value;
			CString strCN;
			Variant2CString(vt, strCN);
			
			vt = m_pRecordset->Fields->GetItem("ID")->Value;
			CString strID;
			Variant2CString(vt, strID);
			
			
			CString strName;
			if(0 == m_NameType)//用户名
			{
				strName.Format("\\M%s", strCN);
			}
			else if(1 == m_NameType)//序号
			{
				strName.Format("\\M%s", strID);
			}
			else if(2 == m_NameType)//用户名 + 序号
			{
				strName.Format("\\M%s%s", strCN, strID);
			}
			else if(3 == m_NameType)//序号 + 用户名
			{
				strName.Format("\\M%s%s", strID, strCN);
			}
			
			CString strCertName = m_Path + strName + "Cert.Cer";
			CString strKeyName = m_Path + strName + "Key.Cer";
			CString strPfxName = m_Path + strName + ".Pfx";
			
			_variant_t PFXPwd = m_pRecordset->Fields->GetItem("PFXPWD")->Value;
			CString strPfxPwd;
			Variant2CString(PFXPwd,strPfxPwd);
			
			_variant_t	varBLOB;
			long lDataSize = m_pRecordset->Fields->GetItem("CERTPFX")->ActualSize;
			if(lDataSize <= 0) return;
			varBLOB = m_pRecordset->Fields->GetItem("CERTPFX")->GetChunk(lDataSize);
			if(varBLOB.vt == (VT_ARRAY | VT_UI1))
			{
				BYTE * pBuf = new BYTE[lDataSize + 1];
				memset(pBuf, 0, lDataSize + 1);
				SafeArrayAccessData(varBLOB.parray,(void **)&pBuf);
				
				if(nSelection == ID_MENUITEM_VIEWCERT)//查看
				{
					X509 * x509 = NULL;
					char out[100] = {0};
					x509 = CCertKey::LoadCert((char *)pBuf,lDataSize,strPfxPwd.GetBuffer(0),out);
					strPfxPwd.ReleaseBuffer();
					
					if(x509)
						((CMiniMainDlg *)AfxGetMainWnd())->ViewCertInfo(0,0,x509);
				}
				else if(nSelection == ID_MENUITEM_DOWNCERT)//保存公钥
				{
					char out[100]={0};
					if(!ParsePfx((char *)pBuf, lDataSize, strPfxPwd.GetBuffer(0), strCertName,
						NULL,NULL,PEM,out))
						AddMsg(out, ERROR);
					strPfxPwd.ReleaseBuffer();
					
				}
				else if(nSelection == ID_MENUITEM_DOWNKEY)//保存私钥
				{
					char out[100]={0};
					if(!ParsePfx((char *)pBuf, lDataSize, strPfxPwd.GetBuffer(0), NULL,
						strKeyName,strPfxPwd.GetBuffer(0),PEM,out))
						AddMsg(out, ERROR);
					strPfxPwd.ReleaseBuffer();
				}
				else if(nSelection == ID_MENUITEM_DOWNPFX)//保存PFX
				{
					CFile file;
					if(file.Open(strPfxName,CFile::modeCreate|CFile::modeWrite))	//存文件
					{
						file.Write(pBuf,lDataSize);
						file.Close();
					}
				}
				else if(nSelection == ID_MENUITEM_REVOKEPFX)//作废PFX
				{
					m_pRecordset->Fields->GetItem("CERTSTATE")->PutValue(_variant_t((long)-1));
					m_pRecordset->Update();
					CString info;
					info.Format("证书%s作废成功", strID);
					AddMsg(info);
					int index = m_List.GetSelectedItem();
					CSuperGridCtrl::CTreeItem * pSelItem = m_List.GetTreeItem(index);
					if(pSelItem != NULL)
					{
						m_List.DeleteItemEx(pSelItem, index);
					}			//删除项目,移动项目到作废列表
					
				}
				
				SafeArrayUnaccessData (varBLOB.parray);//Decrements the lock count of an array
				
			}
			m_pRecordset->MoveNext();//转到下一条纪录
		}
		
		if(m_pRecordset!=NULL && m_pRecordset->State)
			m_pRecordset->Close();

//		AddMsg("导出成功");
	}
	
	catch (_com_error &e)
	{
		DisplayError(e);
	}
	
	*/
	*pResult = 0;
}

void CCaCertManPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)AfxGetMainWnd())->AddMsg(MiniCT_0000, info, type);
}

void CCaCertManPage::QueryCert(BOOL bDelAll)
{
	//得到查询期限

	CString strQuery, strQCert, strQRevoke;

	strQCert.Format("Select * from CACERT Where CERTSTATE = 1 %s", strQuery);
	SelectCert(strQCert);

	strQRevoke.Format("Select * from CACERT Where CERTSTATE = -1 %s", strQuery);
	SelectRevoke(strQRevoke);
}

void CCaCertManPage::InsertCert(CString CN, CString SN, CString Key, CString Info, 
								CString Begin, CString Day, UINT Type) //证书为3
{
	CItemInfo* lpItemInfoi = new CItemInfo();
	
	lpItemInfoi->SetItemText(CN);
	lpItemInfoi->SetImage(3);
	//序号,密钥,用户信息,起始时间,有效期
	lpItemInfoi->AddSubItemText(SN);
	lpItemInfoi->AddSubItemText(Key);
	lpItemInfoi->AddSubItemText(Info);
	lpItemInfoi->AddSubItemText(Begin);
	lpItemInfoi->AddSubItemText(Day);
	
	stuCerType * pstu = NULL;
	
	for(POSITION pos = m_ObCerType.GetHeadPosition(); pos != NULL ; m_ObCerType.GetNext(pos))
	{
		pstu = (stuCerType *)m_ObCerType.GetAt(pos);
		if(pstu->m_Type == Type)
			break;
	}
	m_List.InsertItem(pstu->m_pItem, lpItemInfoi, TRUE);
	
}

void CCaCertManPage::SetNamePath(const CString &strPath)
{
	m_Path = strPath;
}

void CCaCertManPage::TranslateCT()
{
	CHeaderCtrl * pHeader = m_List.GetHeaderCtrl();
	if (pHeader)
	{
		CString str = MiniCT_0600;
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(0, &hdi);

		str = MiniCT_0601;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(1, &hdi);

		str = MiniCT_0602;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(2, &hdi);

		str = MiniCT_0603;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(3, &hdi);

		str = MiniCT_0604;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(4, &hdi);

		str = MiniCT_0605;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(5, &hdi);

	}

	int nItem = m_List.GetCurIndex(m_pRootItem);
	m_List.SetItemText(nItem, 0, MiniCT_0606);

	nItem = m_List.GetCurIndex(m_pRevokwItem);
	m_List.SetItemText(nItem, 0, MiniCT_0607);
	
}

BOOL CCaCertManPage::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	{
		// Let the ToolTip process this message.
		m_toolTip.RelayEvent(pMsg);
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}
