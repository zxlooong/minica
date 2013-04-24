// SafeDataSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "SafeDataSetPage.h"
#include "MiniMainDlg.h"
#include "Evp.h"
#include "rc.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

const UINT BUFLEN = 4096;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//加密后文件格式如下
/*
------------------------------------信息头开始-------------------------------------------------
文件版本标记 - Evp20
信息颈长度 - DWORD
信息颈长度CRC码 - UINT
------------------------------------信息颈开始-------------------------------------------------
信息颈内容  处理方式 公钥加密
             包括:
			      加密方式 ENUM
				  信息体个数n INT
			      n个信息体CDataSafe

---------------------------------对于数字信封:这里还要附加-------------------------------------
			------------------------EVP信息头开始----------------------------------
//			加密后EVP信息颈长度	DWORD
//			加密后EVP信息颈长度 CRC校验码	UINT
//			加密后EVP信息颈
//			-------------------------------EVP信息颈开始---------------------------
//			公钥个数n
//			n个
//				公钥加密的对称密钥长度
//				公钥加密的对称密钥内容
//			向量IV
//			-------------------------------EVP信息颈结束---------------------------
------------------------------------------------------------------------------------------------
------------------------------------n个加密文件内容开始------------------------------------------------
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
------------------------------------n个加密文件内容结束------------------------------------------------


*/

/////////////////////////////////////////////////////////////////////////////
// CSafeDataSetPage property page
const char m_sHeadMark[6] = "Evp20";
CProgressCtrlST * CSafeDataSetPage::m_pProgOne = NULL;

IMPLEMENT_DYNCREATE(CSafeDataSetPage, CPropertyPage)

CSafeDataSetPage::CSafeDataSetPage() : CPropertyPage(CSafeDataSetPage::IDD)
{
	//{{AFX_DATA_INIT(CSafeDataSetPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_uIndex = 0;
	m_EncType = NONE;
	m_bIsEnc = TRUE;
	m_pParent = NULL;
	m_pPageCdb = NULL;
}

CSafeDataSetPage::~CSafeDataSetPage()
{
	m_Image.Detach();
}

void CSafeDataSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSafeDataSetPage)
//	DDX_Control(pDX, IDC_EDIT_PWD, m_XpEditPwd);
	DDX_Control(pDX, IDC_B_ENC, m_BEnc);
	DDX_Control(pDX, IDC_PROGRESS_ALL, m_ProgAll);
	DDX_Control(pDX, IDC_PROGRESS_ONE, m_ProgOne);
	DDX_Control(pDX, IDC_LIST_DATASET, m_DataSetList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSafeDataSetPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSafeDataSetPage)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DATASET, OnRclickListDataset)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DATASET, OnDblclkListDataset)
	ON_BN_CLICKED(IDC_B_ENC, OnBEnc)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_DATASET, OnKeydownListDataset)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_NOTIFY(HDN_BEGINDRAG, IDC_LIST_DATASET, OnBegindragListDataset)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_DATASET, OnBegindragListDataset)
	//}}AFX_MSG_MAP
	ON_MESSAGE(DROPM_DROPOK, OnDropOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSafeDataSetPage message handlers
CWinThread	* pThreadEvpSet = NULL;		
UINT _EvpSetThread(LPVOID lparam)
{
	CSafeDataSetPage * p=(CSafeDataSetPage *)lparam;
	((CMiniMainDlg *)p->GetParentWnd())->SetItemIcon(MiniCT_0002,16);
	
	
	//解决XP进度条不刷新问题
	//解决方法,刷新父窗口
	p->m_ProgOne.SetPos(0);
	//得到进度条控件区域
	CRect rectProg;
	p->m_ProgOne.GetWindowRect(&rectProg);
	p->ScreenToClient(rectProg);//
	
	  //指定父窗口刷新区域
	p->InvalidateRect(rectProg, TRUE);

	if(p->GetIsEnc())//加密
	{
		switch(p->m_EncType)
		{
		case CSafeDataSetPage::NONE:
			break;
		case CSafeDataSetPage::CRYPT:
			p->DoCrypt();
			break;
		case CSafeDataSetPage::DIGEST:
			break;
		case CSafeDataSetPage::SIGN:
			p->DoSign();
			break;
		case CSafeDataSetPage::SEAL:
			p->DoSeal();
			break;
		case CSafeDataSetPage::PubEnc:
			p->DoRSAPubEnc();
			break;
		case CSafeDataSetPage::PrivEnc:
			p->DoRSAPrivEnc();
			break;
		default:
			break;
		}
	}
	else	//解密
	{
		switch(p->m_EncType)
		{
		case CSafeDataSetPage::NONE:
			break;
		case CSafeDataSetPage::CRYPT:
			p->DoDecCrypt();
			break;
		case CSafeDataSetPage::DIGEST:
			break;
		case CSafeDataSetPage::SIGN:
			break;
		case CSafeDataSetPage::SEAL:
			p->DoOpenSeal();
			break;
		case CSafeDataSetPage::PubEnc:
			p->DoRSAPriDec();
			break;
		case CSafeDataSetPage::PrivEnc:
			p->DoRSAPubDec();
			break;
		default:
			break;
		}
	}
	
	((CMiniMainDlg *)p->GetParentWnd())->SetItemIcon(MiniCT_0002, 4);
	return 0;
}

BOOL CSafeDataSetPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CXPStyleButtonST::SetAllThemeHelper(this, ((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
	m_BEnc.SetIcon(IDI_ICON_DENC);
	m_BEnc.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
//		m_Progress.SetBitmap(IDB_BITMAP_PROG);
		ClassXP(GetDlgItem(IDC_COMBO_CRYPT)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO1)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO2)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO3)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO4)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO5)->m_hWnd,TRUE);
		ClassXP(GetDlgItem(IDC_RADIO6)->m_hWnd,TRUE);
	}


	//初始化图像列表
	SHFILEINFO shfi = {0};

	HIMAGELIST m_hImlIcons = (HIMAGELIST)SHGetFileInfo(NULL, 0, 
			&shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	//得到系统ImageList
	//以后通过得到文件的图标索引添加图标

	m_Image.Attach(m_hImlIcons);

	m_DataSetList.SetImageList(&m_Image, LVSIL_SMALL);

	m_DataSetList.SetExtendedStyle(LVS_EX_GRIDLINES);

	m_DataSetList.InsertColumn(0, MiniCT_1300, LVCFMT_LEFT, 150);
	m_DataSetList.InsertColumn(1, MiniCT_1301, LVCFMT_LEFT, 75);
	m_DataSetList.InsertColumn(2, MiniCT_1302, LVCFMT_LEFT, 70);
	m_DataSetList.InsertColumn(3, MiniCT_1303, LVCFMT_LEFT, 70);
	m_DataSetList.InsertColumn(4, MiniCT_1304, LVCFMT_LEFT, 100);
//	m_DataSetList.InsertColumn(3, "进度", LVCFMT_LEFT, 100);

	SetDlgItemText(IDC_COMBO_CRYPT,"idea-cbc");
	SetDlgItemText(IDC_EDIT_PWD,"MiniCA");
	((CComboBox *)GetDlgItem(IDC_COMBO_CRYPT))->SetCurSel(16);
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(1);

	m_pParent = AfxGetMainWnd();

	CSafeDataSetPage::m_pProgOne = &m_ProgOne;
	
	m_Target.Reg(this, "SafeData");

	CMiniMainDlg * pMain = (CMiniMainDlg *)AfxGetMainWnd();
	if(pMain)
	{
		CWinEvpSheet * pSheet = (CWinEvpSheet *)(pMain->GetPage("CWinEvpSheet"));
		if(pSheet)
			m_pPageCdb = (CSafeDataCdbPage*)pSheet->GetPage("CSafeDataCdbPage");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*
函数功能:向列表中添加记录
strDataSet - 记录名称
strFileLen - 记录大小
strDate - 修改时间
uImage - 显示的图像索引
pParent - 父节点句柄,NULL表示添加为根节点
*/
CSuperGridCtrl::CTreeItem * CSafeDataSetPage::AddDataSet(const CDataSafe * pDataSafe,
														 const CString strDataSet, 
														 const UINT uImage,
														 const DWORD & dOldLen, 
														 const DWORD dNewLen, 
														 CString strType,
														 CString strTime,
														 CSuperGridCtrl::CTreeItem * pParent)
{
	CString strOldLen(""), strNewLen("");
	strOldLen.Format("%d", dOldLen);
	if(dNewLen != 0 )
		strNewLen.Format("%d", dNewLen);

	CItemInfo* lpItemInfo = new CItemInfo();
	lpItemInfo->SetDataSafe(pDataSafe);
	lpItemInfo->SetImage(uImage);
	lpItemInfo->SetItemText(strDataSet);
	lpItemInfo->AddSubItemText(strOldLen);		//原始大小
	lpItemInfo->AddSubItemText(strNewLen);		//加密后大小
	lpItemInfo->AddSubItemText(strType);	//类型说明
	lpItemInfo->AddSubItemText(strTime);	//修改时间

	if(!pParent)
	{
//		lpRoot->m_uIndex = m_uIndex + 1;
		return m_DataSetList.InsertRootItem(lpItemInfo);//previous on N.Y.P.D we call it CreateTreeCtrl(lp)
	}
	else
	{
//		lpItemInfo->m_uIndex = m_uIndex + 1;
		return m_DataSetList.InsertItem(pParent, lpItemInfo, FALSE);
	}
}

//添加文件到结构MAP
//枚举到文件时候的操作
void CSafeDataSetPage::AddFileList(const WIN32_FIND_DATA & FileData, 
								   CSuperGridCtrl::CTreeItem * hPitem,
								   const UINT uTempParent,
								   CString sFile,
								   DWORD & dFileLen)
{

	SHFILEINFO sfi = {0};
	SHGetFileInfo(FileData.cFileName, FILE_ATTRIBUTE_NORMAL, &sfi,
		sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_TYPENAME);
	
	//ftLastWriteTime
	
	SYSTEMTIME stime;
	FILETIME ftime; 
	FileTimeToLocalFileTime(&FileData.ftLastWriteTime, &ftime); // 转换成本地时间 
	FileTimeToSystemTime(&ftime, &stime); // 转换成系统时间格式
	
	//文件大小
  //  StrFormatByteSize(rFind.nFileSizeLow, szFileLen, 64 );

	dFileLen = (FileData.nFileSizeHigh * (MAXDWORD+1)) + FileData.nFileSizeLow;
	
	//时间
	CString strTime;
	strTime.Format("%d-%d-%d %d:%d", stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute);

	CDataSafe DataSafe(FileData.cFileName, dFileLen, 
		FileData.ftLastWriteTime, TRUE, uTempParent, sFile.GetBuffer(0));

	AddDataSet(&DataSafe, FileData.cFileName, sfi.iIcon, dFileLen, 0, sfi.szTypeName, strTime, hPitem);
	
//	++m_uIndex; 
	
}

BOOL CSafeDataSetPage::AddList(CString sLocalDir, UINT uPid, 
							   CSuperGridCtrl::CTreeItem * hPitem)
{
    POSITION pos = m_DirName.GetHeadPosition();
    while ( NULL != pos )
    {
		if(m_DirName.GetNext(pos).Compare(sLocalDir) == 0)//重复
			return TRUE;
    }
	m_DirName.AddHead(sLocalDir);
	//判断是否为磁盘,如果为磁盘,则后面添加*.*
	if(sLocalDir.GetLength() == 3 && sLocalDir.Find(":") != -1)
		sLocalDir += "*.*";
	return EnumDir(sLocalDir, uPid, hPitem);
}

/*BOOL CSafeDataSetPage::GetDirTime(CString DirName, FILETIME &ftime, CString & strTime)
{
	FILETIME lpCreationTime; // 文件夹的创建时间 
	FILETIME lpLastAccessTime; // 对文件夹的最近访问时间 
	FILETIME lpLastWriteTime; // 文件夹的最近修改时间 
	// 打开文件夹
	HANDLE hDir = CreateFile (DirName, GENERIC_READ, 
		FILE_SHARE_READ|FILE_SHARE_DELETE, 
		NULL, OPEN_EXISTING, 
		FILE_FLAG_BACKUP_SEMANTICS, NULL); 

	if(INVALID_HANDLE_VALUE == hDir)
		return FALSE;
	// 获取文件夹时间属性信息
	SYSTEMTIME stime;

	if (GetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
	{ 
		FileTimeToLocalFileTime(&lpLastWriteTime, &ftime); // 转换成本地时间 
		FileTimeToSystemTime(&ftime, &stime); // 转换成系统时间格式
	} 
	CloseHandle(hDir); // 关闭打开过的文件夹

	strTime.Format("%d-%d-%d %d:%d", stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute);

	return TRUE;
}*/


//枚举目录,如果是文件,直接添加到列表中
BOOL CSafeDataSetPage::EnumDir(CString strPath, UINT uPid, 
							   CSuperGridCtrl::CTreeItem * hPitem)
{
	DWORD dDirLen = 0;
	WIN32_FIND_DATA wfd = {0};

	HANDLE hFind = FindFirstFile (strPath, &wfd);
	BOOL bResult = (hFind == INVALID_HANDLE_VALUE) ? FALSE : TRUE;

	CSuperGridCtrl::CTreeItem *  pItem = NULL;
	UINT uTempParent = 0;

	CString strTime;
	SYSTEMTIME stime;
	FILETIME ftime; 

	while (bResult)
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && wfd.cFileName[0] != '.' && wfd.cFileName[0] != '..')	// folder
		{
			SHFILEINFO sfi = {0};
			SHGetFileInfo (NULL, FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(SHFILEINFO),
						SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_TYPENAME);

			FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &ftime); // 转换成本地时间 
			FileTimeToSystemTime(&ftime, &stime); // 转换成系统时间格式

			strTime.Format("%d-%d-%d %d:%d", stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute);

			CString strFileName = strPath;
			strFileName.Replace("*.*", wfd.cFileName);

			CDataSafe DataSafe(wfd.cFileName, 0, wfd.ftLastWriteTime, FALSE, uPid, strFileName);

			pItem = AddDataSet(&DataSafe, wfd.cFileName, sfi.iIcon, 0, 0, sfi.szTypeName, strTime, hPitem);

			uTempParent = ++m_uIndex;

			//进入目录
			CString strPathName = strPath;	//枚举此文件夹下面的所有内容
			strPathName.Replace("*.*", wfd.cFileName);
			strPathName += "\\*.*";
			EnumDir(strPathName, uTempParent, pItem);
		}
		bResult = FindNextFile (hFind, &wfd);
	}
	FindClose (hFind);

	// read files
	hFind = FindFirstFile (strPath, &wfd);
	bResult = (hFind == INVALID_HANDLE_VALUE) ? FALSE : TRUE;

	while (bResult)
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && wfd.cFileName[0] != '.' && wfd.cFileName[0] != '\0')		// no folder or parent folder
		{
			CString strFileName = strPath;
			strFileName.Replace("*.*", wfd.cFileName);
			DWORD dFileLen = 0;
			AddFileList(wfd, hPitem, uPid, strFileName, dFileLen);
			dDirLen += dFileLen;
			//累加所有文件长度,然后显示在列表中,并修改父结构长度
		}
		bResult = FindNextFile (hFind, &wfd);
	}
	FindClose (hFind);
	CString str;
	str.Format("%d", dDirLen);
	if(hPitem)
	{
		int index = m_DataSetList.GetCurIndex(hPitem);
		if(index != -1)
			m_DataSetList.SetItemText(index, 1, str);
	}


	return bResult;
}


void CSafeDataSetPage::OnRclickListDataset(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int i = m_DataSetList.GetSelectedItem();
	if(i == -1)
		return;
	CSuperGridCtrl::CTreeItem * pItem = m_DataSetList.GetTreeItem(i);
	CItemInfo * pInfo = m_DataSetList.GetData(pItem ); 
	
	const CDataSafe * pDataSafe = pInfo->GetDataSafe();
	

	CString str;
	str.Format("文件名%s 偏移%d 长度%d", pDataSafe->cName, pDataSafe->dBeginSeek, pDataSafe->dNewLen);
	AfxMessageBox(str);
	*pResult = 0;
}


//加载硬盘里面的.evp文件,读取文件信息,并写入MAP,然后显示在列表中
BOOL CSafeDataSetPage::ParseEvp(CString strEvpFile, EncType & encType)
{
	m_uIndex = 0; //添加目录时候不清0,因为可能多次添加

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(MiniCT_1320);
		return FALSE;
	}
	//读取文件前5个字母,判断是否为Evp20
	char sHeadMark[6] = {0};
	File.Read(sHeadMark, strlen(m_sHeadMark));
	if(strcmp(sHeadMark, m_sHeadMark)!=0)
	{
		//非EVP文件,返回
		File.Close();
		AfxMessageBox(MiniCT_1305);
		return FALSE;
	}
	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));
	//校验CRC,类型UINT
	UINT uCrcCheck = 0;
	File.Read(&uCrcCheck, sizeof(UINT));
	if(!CRC::CheckCrc((char *)&dFileInfoLen, sizeof(DWORD), uCrcCheck))
	{
		AfxMessageBox(MiniCT_1306);
		File.Close();
		return FALSE;
	}

	//开辟缓冲区,容纳加密后的信息体
	BYTE * pIn = new BYTE[dFileInfoLen + 1];
	File.Read(pIn, dFileInfoLen);

	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_KEY),"CERT");
	DWORD lenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey = LoadResource(NULL,hRsrc);
	LPSTR lpKey = (LPSTR)LockResource(hgKey);

	DWORD dOutLen = dFileInfoLen;

	//开辟缓冲区,容纳解密后的信息体
	BYTE * pOut = new BYTE[dOutLen];

	CString outMsg;


	if(!CEvp::RSAPrivDec((BYTE *)lpKey, lenKey, "MiniCA", pIn, 
		dFileInfoLen, pOut, dOutLen, outMsg.GetBuffer(256)))
	{
		//解密失败
		AfxMessageBox(MiniCT_1307);
		File.Close();
		delete [] pIn;
		delete [] pOut;
		return FALSE;
	}

	CMemFile MemFile;
	MemFile.Attach(pOut, dOutLen);

	//首先是加密方式
	MemFile.Read(&encType, sizeof(EncType));
	m_EncType = encType;

	//然后是结构体个数
	UINT uIndex = 0;
	MemFile.Read(&uIndex, sizeof(uIndex));

	CSuperGridCtrl::CTreeItem * pItem[1024];	//只记录文件夹
	memset(pItem, 0, sizeof(CSuperGridCtrl::CTreeItem * ) * 1024);
	
	CMap<UINT, UINT, CSuperGridCtrl::CTreeItem *, CSuperGridCtrl::CTreeItem * > ItemMap;//记录目录ITEM的MAP
	ItemMap.SetAt(0, NULL);

	SHFILEINFO sfi = {0};


	//最后是n个结构体
	for(UINT index = 1, j = 1; index <= uIndex; index++, m_uIndex++)	//读取文件信息到结构MAP
	{
		CDataSafe DataSafe;
		DataSafe.Read(MemFile);
		
		CSuperGridCtrl::CTreeItem * pParentItem = NULL;
		if(ItemMap.Lookup(DataSafe.uParentID, pParentItem))
		{
			
			SYSTEMTIME stime;
			FILETIME ftime; 
			FileTimeToLocalFileTime(&DataSafe.ftLastWriteTime, &ftime); // 转换成本地时间 
			FileTimeToSystemTime(&ftime, &stime); // 转换成系统时间格式
			
			//时间
			CString strTime;
			strTime.Format("%d-%d-%d %d:%d", stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute);
			
			if(!DataSafe.bIsFile)//目录,记录下来
			{
				
				SHGetFileInfo (NULL, FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(SHFILEINFO),
					SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_TYPENAME);
				
				pItem[j] = AddDataSet(&DataSafe, DataSafe.cName, sfi.iIcon, 
					DataSafe.dOldLen, 0, sfi.szTypeName, strTime, pParentItem);
				ItemMap.SetAt(j, pItem[j]);
				j++;
			}
			else
			{
				SHGetFileInfo(DataSafe.cName, FILE_ATTRIBUTE_NORMAL, &sfi,
					sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX| SHGFI_TYPENAME );
				
				AddDataSet(&DataSafe, DataSafe.cName, sfi.iIcon, DataSafe.dOldLen, DataSafe.dNewLen,
					sfi.szTypeName, strTime, pParentItem);
			}
		}
		
	}

	delete [] pIn;
	delete [] pOut;

	MemFile.Close();
	File.Close();


/*	for(int index = 1, j = 0; index <= m_DataMap.GetCount(); index++, m_uIndex++)
	{
	}*/
	//如果分析成功,则转换为解密方式,需要完成,按钮等的变化
//	SetDlgItemText(IDC_B_ENC, "解密");
	m_BEnc.SetIcon(IDI_ICON_DDEC);

	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(0);
	((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(0);
	((CButton *)GetDlgItem(IDC_RADIO4))->SetCheck(0);
	((CButton *)GetDlgItem(IDC_RADIO5))->SetCheck(0);
	((CButton *)GetDlgItem(IDC_RADIO6))->SetCheck(0);

	GetDlgItem(IDC_RADIO1)->EnableWindow(0);
	GetDlgItem(IDC_RADIO2)->EnableWindow(0);
	GetDlgItem(IDC_RADIO3)->EnableWindow(0);
	GetDlgItem(IDC_RADIO4)->EnableWindow(0);
	GetDlgItem(IDC_RADIO5)->EnableWindow(0);
	GetDlgItem(IDC_RADIO6)->EnableWindow(0);

	switch(m_EncType)
	{
	case NONE:
		break;
	case CRYPT:	//对称 1
		OnRadio1();
		GetDlgItem(IDC_RADIO1)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		break;
	case DIGEST:	//摘要 3
		OnRadio3();
		GetDlgItem(IDC_RADIO3)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		break;
	case SIGN:		//签名 4
		OnRadio3();
		GetDlgItem(IDC_RADIO4)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO4))->SetCheck(1);
		break;
	case SEAL:	//信封 2
		OnRadio4();
		GetDlgItem(IDC_RADIO2)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(1);
		break;
	case PubEnc:
		OnRadio5();
		GetDlgItem(IDC_RADIO5)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO5))->SetCheck(1);
		break;
	case PrivEnc:
		OnRadio6();
		GetDlgItem(IDC_RADIO6)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO6))->SetCheck(1);
		break;
	default:
		break;
	}	
	m_bIsEnc = FALSE;
	m_strEvpPath = strEvpFile;	//設置解密文件
	return TRUE;
}

CProgressCtrlST * CSafeDataSetPage::ShowProg(int nItem, int nCol)
{
//	CString strFind = GetItemText(nItem, nCol);

	//basic code start
	CRect rect;
	int offset = 0;
	// Make sure that the item is visible
	if( !m_DataSetList.EnsureVisible(nItem, TRUE)) return NULL;
	m_DataSetList.GetSubItemRect(nItem, nCol, LVIR_BOUNDS, rect);
	// Now scroll if we need to expose the column
	CRect rcClient;
	m_DataSetList.GetClientRect(rcClient);
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		m_DataSetList.Scroll(size);
		rect.left -= size.cx;
	}
	
	rect.left += offset;	
	rect.right = rect.left + m_DataSetList.GetColumnWidth(nCol);
	if(rect.right > rcClient.right) 
	   rect.right = rcClient.right;
	//basic code end

	rect.bottom += rect.Height();//dropdown area
	
	DWORD dwStyle =  WS_CHILD | WS_VISIBLE;
	CProgressCtrlST *pProg = new CProgressCtrlST;
	pProg->Create(dwStyle, rect, this, 100050);
	pProg->SetBitmap(IDB_BITMAP_PROG);
	pProg->SetPos(40);
//	pList->ModifyStyleEx(0,WS_EX_CLIENTEDGE);//can we tell at all
//	pList->SetHorizontalExtent(CalcHorzExtent(pList, lstItems));
//	pList->ShowDropDown();
//	pList->SelectString(-1, strFind.GetBuffer(1));
	// The returned pointer should not be saved
	return pProg;
}


//功能:
		//写入 版本Evp20, 加密结构长度, 加密结构校验
		//移动文件到加密位置(掠过信息颈)
//参数:
		//File - 用来加密文件的
		//nCount - 文件信息体个数
		//dFileInfoLen - 返回加密后长度
		//strMsg - 返回操作错误信息
BOOL CSafeDataSetPage::MadeFileHead(CFile & File, const int nCount, DWORD & dFileInfoLen, CString & strMsg)
{
	if(0 == nCount)
	{
		strMsg = MiniCT_1308;
		return FALSE;
	}
	
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_CERT),"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert = LoadResource(NULL,hRsrc);
	LPSTR lpCert = (LPSTR)LockResource(hgCert);
	
	
	//取得加密后文件信息体大小
	//文件信息体为: 加密方式sizeof(EncType) +  sizeof(int)(信息体个数 int 类型) + n个CDataSafe结构大小nCount * (sizeof(CDataSafe) - sizeof(char)*256
	//其中CDataSafe结构不写入文件全路径,所以 - sizeof(char)*256
	//
	dFileInfoLen = CEvp::GetEncLen(lpCert, lenCert, 
		sizeof(EncType) + sizeof(nCount) + nCount * (sizeof(CDataSafe) - sizeof(char)*256) );
	if(dFileInfoLen == -1)
	{
		strMsg = MiniCT_1309;
		return FALSE;
	}

	//写入版本标志Evp20
	File.Write(m_sHeadMark, strlen(m_sHeadMark));
	//写入加密后文件信息体大小,类型为DWORD
	File.Write(&dFileInfoLen, sizeof(DWORD));
	//写入CRC校验值,类型UINT
	UINT uCrcCheck = 0;
	if(CRC::GetCrc16((char *)&dFileInfoLen, sizeof(DWORD), uCrcCheck))
	{
		File.Write(&uCrcCheck, sizeof(UINT));
	}
	else
	{
		strMsg = MiniCT_1310;	//"生成信息颈长度CRC校验失败";
		AddMsg(strMsg, M_ERROR);
		return FALSE;
	}
	
	//预留信息头 
	File.Seek(dFileInfoLen, CFile::current);
	return TRUE;
}

//功能:	- 加密文件完毕后调用
		//写入 加密(加密方式+文件个数+文件信息体加密后长度)后内容
		//移动文件到加密位置
//参数:
		//File - 用来加密文件的
		//encType - 加密方式
		//nCount - 文件信息体个数
		//dFileInfoLen - 加密后长度,用于分配内存
		//strMsg - 返回操作错误信息

BOOL CSafeDataSetPage::MadeFileNeck(CList<CDataSafe *, CDataSafe *> * pDataList, CFile &File, 
									const EncType encType, const int nCount, 
									const DWORD dFileInfoLen, CString &strMsg)
{
	//开始屏蔽进度显示 , 完毕后打开
	CSafeDataSetPage::m_pProgOne = NULL;

	//构造内存文件,写入 加密方式 + 信息体个数n + n个信息体
	CMemFile MemFile;

	MemFile.Write(&encType, sizeof(EncType));

	MemFile.Write(&nCount, sizeof(int));

	//枚举目录列表
	POSITION pos = pDataList->GetHeadPosition();
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = pDataList->GetNext(pos);
		if(pDataSafe)
			pDataSafe->Write(MemFile);
	}
	//加密内存文件
	//构造加密结果 长度+1 内存
	DWORD dOutLen = -1;
	BYTE * pOut = new BYTE[dFileInfoLen + 1];
	dOutLen = dFileInfoLen;

	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_CERT),"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert = LoadResource(NULL,hRsrc);
	LPSTR lpCert = (LPSTR)LockResource(hgCert);

	//构造加密输入
	DWORD dInLen = MemFile.GetLength();
	BYTE * pIn = MemFile.Detach();	//同时关闭MemFile.Close(); 此时不要重新开辟内存,而且Detach用malloc,所以释放用free
	

	if(!CEvp::RSAPubEnc((BYTE *)lpCert, lenCert, "", pIn, dInLen, 
		pOut, dOutLen, strMsg.GetBuffer(256)))
	{
		//加密失败
		strMsg.ReleaseBuffer();
		free(pIn);
		delete [] pOut;
		CSafeDataSetPage::m_pProgOne = &m_ProgOne;
		return FALSE;
	}
	else if(dOutLen != dFileInfoLen)
	{
		//加密失败
		strMsg.ReleaseBuffer();
		strMsg = MiniCT_1311;//"信息颈长度与预期不符";
		free(pIn);
		delete [] pOut;
		CSafeDataSetPage::m_pProgOne = &m_ProgOne;
		return FALSE;
	}

	CSafeDataSetPage::m_pProgOne = &m_ProgOne;
	
	//移动文件指针到开头 + "Evp20" + 加密后信息体大小DWORD + CRC校验UINT
	File.Seek(strlen(m_sHeadMark) + sizeof(DWORD) + sizeof(UINT), CFile::begin);
	File.Write(pOut, dFileInfoLen);
	free(pIn);

	delete [] pOut;
	return TRUE;
}

/////////////////////////////////////////////////////加密文件部分////////////////////////////////////////////////
//功能:对称加密文件、目录
//参数:
//		strCpName - 算法名称
//		strPwd- 对称密钥
//		strFileName - 加密后文件
//		outMsg - 返回操作信息
BOOL CSafeDataSetPage::Crypt(CString strCpName, CString strPwd, CString strFileName, CString & outMsg)
{
	//输入检查
	if(strCpName.IsEmpty() || strPwd.IsEmpty() || strFileName.IsEmpty())
	{
		outMsg = MiniCT_1312; //"算法名称或对称密钥或输出文件非法";
		return FALSE;
	}

	CFile File;//文件头加版本信息
	if(!File.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outMsg.Format("%s : %s", MiniCT_1313,strFileName);	//创建文件失败
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//创建信息头
	if(!MadeFileHead(File, nCount, dFileInfoLen, outMsg))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;

	EVP_CIPHER_CTX Ctx, CtxTemp;
	if(!CEvp::InitCrypt2(strCpName, strPwd, 1, Ctx))
	{
		outMsg = MiniCT_1314; //"初始化加密结构失败";
		File.Close();
		return FALSE;
	}

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;	//包含目录和文件得全部信息列表

	EnumList(&DataList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;

	CString strInfo;

	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			if(pDataSafe->bIsFile) //文件则加密
			{
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
				AddOnceMsg(strInfo);
				CtxTemp = Ctx;
				//结构中加密起始位置赋值
				pDataSafe->dBeginSeek = File.GetPosition();
				dOutLen = CEvp::Crypt2(&CtxTemp, pDataSafe->cPathName, File, 0, outMsg);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s, %s%s", pDataSafe->cName, MiniCT_1318, MiniCT_1319, outMsg);//对称加密失败 失败原因:
					outMsg = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//结构赋值
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}
		}
	
		m_ProgAll.SetPos(++iFinish);
	}

	//创建信息颈
	if(!MadeFileNeck(&DataList, File, CRYPT, nCount, dFileInfoLen, outMsg))
	{
		File.Close();
		CEvp::CleanCrypt2(Ctx);
		CleanList(&DataList);
		return FALSE;
	}

	CEvp::CleanCrypt2(Ctx);
	CleanList(&DataList);

	File.Close();
	return TRUE;
}

//对称解密
BOOL CSafeDataSetPage::DecCrypt(CString strCpName, CString strPwd, CString strEvpFile, CString & outMsg)
{
	//解密信息
	DWORD dOutLen = -1;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outMsg.Format("%s%s", MiniCT_1320, strEvpFile);//打开文件失败
		return FALSE;
	}

	EVP_CIPHER_CTX Ctx, CtxTemp;
	if(!CEvp::InitCrypt2(strCpName, strPwd, 0, Ctx))
	{
		outMsg = MiniCT_1314;//"初始化加密结构失败";
		File.Close();
		return FALSE;
	}

	CString strPathName;

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;	//文件和目录信息列表
	CList<CDataSafe *, CDataSafe *> DirList;	//目录信息列表

	EnumList(&DataList, &DirList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			strPathName = GetPathName(pDataSafe, &DirList);
			if(pDataSafe->bIsFile) //文件
			{
				//结构中加密起始位置赋值
				CtxTemp = Ctx;
				File.Seek(pDataSafe->dBeginSeek, CFile::begin);
				dOutLen = CEvp::Crypt2(&CtxTemp, strPathName, 
					File, pDataSafe->dNewLen, outMsg);
				if(-1 == dOutLen)
				{
					bRet =  FALSE;
					break;
				}
				else if(dOutLen != pDataSafe->dOldLen)
				{
					outMsg.Format("%s%s", strPathName, MiniCT_1321);	//文件大小校验失败
					bRet =  FALSE;
					break;
				}
			}
			else//目录 ->建立目录
			{
				if(!Mdir(strPathName, outMsg))
				{
					bRet =  FALSE;
					break;
				}
			}

			m_ProgAll.SetPos(++iFinish);

		}
	}

	File.Close();
	CleanList(&DataList);
	CEvp::CleanCrypt2(Ctx);
	return bRet;
}

//数集摘要
//参数:	strMdName - 摘要算法名称
//		strOutName - 保存结果的EVP文件名称
//		outMsg - 返回操作信息
BOOL CSafeDataSetPage::Digest(CString strMdName, CString strOutName, CString & outMsg)
{

	//输入检查
	if(strMdName.IsEmpty() || strOutName.IsEmpty())
	{
		outMsg = MiniCT_1322;//"算法名称或输出文件非法";
		return FALSE;
	}

	CFile File;//文件头加版本信息
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outMsg = MiniCT_1313;//"创建文件失败";
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//创建信息头
	if(!MadeFileHead(File, nCount, dFileInfoLen, outMsg))
		return FALSE;

	DWORD dOutLen = -1;

	EVP_MD_CTX mdCtx, mdCtxTemp;

	//初始化CTX
	CEvp::InitDigest2(strMdName, mdCtx);

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;
	EnumList(&DataList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			if(pDataSafe->bIsFile) //文件则加密
			{
				mdCtxTemp = mdCtx;
				//结构中加密起始位置赋值
				pDataSafe->dBeginSeek = File.GetPosition();
				dOutLen = CEvp::Digest2(&mdCtxTemp, pDataSafe->cPathName, File, outMsg);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s", MiniCT_1323, pDataSafe->cName); //摘要文件失败
					outMsg = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//结构赋值
				{
					pDataSafe->dNewLen = dOutLen;
				}

			}

			m_ProgAll.SetPos(++iFinish);
			
		}
	}
	
	//创建信息颈
	if(!MadeFileNeck(&DataList, File, DIGEST, nCount, dFileInfoLen, outMsg))
	{
		File.Close();
		CEvp::CleanDigest2(mdCtx);
		CleanList(&DataList);
		return FALSE;
	}
	File.Close();
	CEvp::CleanDigest2(mdCtx);
	CleanList(&DataList);
	return TRUE;
}

//摘要验证
BOOL CSafeDataSetPage::VerifyDigest(CString strMdName, CString strEvpFile, CString & outStr)
{
	EVP_MD_CTX mdCtx, mdCtxTemp;

	//解密信息

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile); //打开文件失败
		return FALSE;
	}

//	文件版本标记 - Evp20
//	信息颈长度 - DWORD
//	信息颈长度CRC码 - UINT
	File.Seek(strlen(m_sHeadMark), CFile::begin);

	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));

	File.Seek(sizeof(UINT), CFile::current);

	File.Seek(dFileInfoLen, CFile::current);


	//初始化CTX
	CEvp::InitDigest2(strMdName, mdCtx);
	{
		File.Close();
		return FALSE;
	}

	CString strPathName;

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;	//文件和目录信息列表
	CList<CDataSafe *, CDataSafe *> DirList;	//目录信息列表

	EnumList(&DataList, &DirList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			strPathName = GetPathName(pDataSafe, &DirList);
			if(pDataSafe->bIsFile) //文件
			{
				//结构中加密起始位置赋值
				mdCtxTemp = mdCtx;
				File.Seek(pDataSafe->dBeginSeek, CFile::begin);
			
				bRet = CEvp::VerifyDigest2(&mdCtxTemp, strPathName, File, pDataSafe->dNewLen, outStr);

			}
			m_ProgAll.SetPos(++iFinish);

		}
	}

	CleanList(&DataList);
	File.Close();
	CEvp::CleanDigest2(mdCtx);
	return bRet;
}

//数字签名
BOOL CSafeDataSetPage::Sign(CString strMdName, const BYTE * pCertBuf, const UINT nCertLen, CString strPwd,
								  CString strOutName, CString & outStr)
{

	EVP_MD_CTX md_Ctx = {0};
	EVP_PKEY * pkey = NULL;
	X509 * px509 = NULL;
	CEvp::InitSign2(strMdName, md_Ctx, (char *)pCertBuf, nCertLen, strPwd, px509, pkey);

	CFile File;//文件头加版本信息
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outStr.Format("%s%s", MiniCT_1313, strOutName);//创建文件失败
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//创建信息头
	if(!MadeFileHead(File, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;
	
	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;
	EnumList(&DataList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	CString strInfo;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			if(pDataSafe->bIsFile) //文件则加密
			{
				//结构中加密起始位置赋值
//				strInfo.Format("文件 %s ,大小 %d 加密中...", pDataSafe->cName, pDataSafe->dOldLen);
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
				AddOnceMsg(strInfo);

				pDataSafe->dBeginSeek = File.GetPosition();

				dOutLen = CEvp::Sign2(&md_Ctx, pkey, pDataSafe->cPathName, File, outStr);
				
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s, %s%s", MiniCT_1324, pDataSafe->cName, MiniCT_1319, outStr);	//签名失败%s, 失败原因:%s
					outStr = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//结构赋值
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}

			m_ProgAll.SetPos(++iFinish);
			
		}
		
	}

	//创建信息颈
	if(!MadeFileNeck(&DataList, File, SIGN, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		CleanList(&DataList);
		CEvp::CleanSign2(md_Ctx, px509, pkey);
		return FALSE;
	}
	CEvp::CleanSign2(md_Ctx, px509, pkey);
	File.Close();
	CleanList(&DataList);
	return TRUE;
}

//验证数字签名
BOOL CSafeDataSetPage::VerifySign(CString strMdName, const BYTE * pCertBuf, const UINT nCertLen,
								  CString strEvpFile, CString & outStr)
{

	EVP_MD_CTX mdCtx = {0},
		mdCtxTemp = {0};
	EVP_PKEY * pkey = NULL;
	X509 * px509 = NULL;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//"打开文件失败%s"
		return FALSE;
	}

	CEvp::InitSign2(strMdName, mdCtx, (char *)pCertBuf, nCertLen, NULL, px509, pkey);

//	文件版本标记 - Evp20
//	信息颈长度 - DWORD
//	信息颈长度CRC码 - UINT
	File.Seek(strlen(m_sHeadMark), CFile::begin);

	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));

	File.Seek(sizeof(UINT), CFile::current);

	File.Seek(dFileInfoLen, CFile::current);
	CString strPathName;

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;	//文件和目录信息列表
	CList<CDataSafe *, CDataSafe *> DirList;	//目录信息列表

	EnumList(&DataList, &DirList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			strPathName = GetPathName(pDataSafe, &DirList);
			if(pDataSafe->bIsFile) //文件
			{
				//结构中加密起始位置赋值
				mdCtxTemp = mdCtx;
				File.Seek(pDataSafe->dBeginSeek, CFile::begin);
				bRet = CEvp::VerifySign2(&mdCtxTemp, pkey, strPathName, File, pDataSafe->dNewLen, outStr);
			}
			m_ProgAll.SetPos(++iFinish);

		}
	}

	CleanList(&DataList);
	File.Close();
	CEvp::CleanDigest2(mdCtx);
	return TRUE;
}

//功能:多证书数字信封
//参数:
//		pCERT -- [in]合法用户公钥链
//		strCpName -- 算法名称
//		outMsg -- [OUT]返回错误信息
BOOL CSafeDataSetPage::Seal(CEvp::stuCertLink * pCERT, CString strCpName,	
							CString strOutName, CString & outMsg)
{
	CFile File;//文件头加版本信息
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outMsg.Format("%s%s", MiniCT_1313, strOutName);//"创建文件失败%s"
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//创建信息头
	if(!MadeFileHead(File, nCount, dFileInfoLen, outMsg))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;

	EVP_CIPHER_CTX eCtx, eCtxTemp;

	//初始化CTX
	if(!CEvp::InitSeal2(pCERT, strCpName, File, eCtx, outMsg))
	{
		File.Close();
		return FALSE;
	}

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;
	EnumList(&DataList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	CString strInfo;

//	CDC* pDC = m_DataSetList.GetDC();
	while(pos != NULL)
	{
//		m_DataSetList.DrawProgressBar(pDC, 0, 2);

		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			if(pDataSafe->bIsFile) //文件则加密
			{
				//初始化CTX
		//		strInfo.Format("文件 %s ,大小 %d 加密中...", pDataSafe->cName, pDataSafe->dOldLen);
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
				AddOnceMsg(strInfo);
				eCtxTemp = eCtx;
				//结构中加密起始位置赋值
				pDataSafe->dBeginSeek = File.GetPosition();
				dOutLen = CEvp::Seal2(&eCtxTemp, pDataSafe->cPathName, File, outMsg);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s, %s%s", MiniCT_1325, pDataSafe->cName, MiniCT_1319, outMsg);//"封装文件失败%s, 失败原因:%s"
					outMsg = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//结构赋值
				{
					pDataSafe->dNewLen = dOutLen;
				}	

			}

			m_ProgAll.SetPos(++iFinish);
			
		}
		
	}

//	m_DataSetList.ReleaseDC(pDC);


	//创建信息颈
	if(!MadeFileNeck(&DataList, File, SEAL, nCount, dFileInfoLen, outMsg))
	{
		File.Close();
		CEvp::CleanSeal2(eCtx);
		CleanList(&DataList);
		return FALSE;
	}
	File.Close();
	CEvp::CleanSeal2(eCtx);
	CleanList(&DataList);
	return TRUE;
}

//拆封数字信封
BOOL CSafeDataSetPage::OpenSeal(char * pKey, UINT ukeyLen, char * pwd, CString strCpName,
							CString strEvpFile, CString & outStr)
{
	//解密信息
	DWORD dOutLen = -1;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//"打开文件%s失败"
		return FALSE;
	}

//	文件版本标记 - Evp20
//	信息颈长度 - DWORD
//	信息颈长度CRC码 - UINT

	File.Seek(strlen(m_sHeadMark), CFile::begin);

	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));

	File.Seek(sizeof(UINT), CFile::current);

	File.Seek(dFileInfoLen, CFile::current);


	EVP_CIPHER_CTX Ctx, CtxTemp;

	if(!CEvp::InitOpenSeal2(pKey, ukeyLen, pwd, strCpName, File, Ctx, outStr))
	{
		File.Close();
		outStr.Format(MiniCT_1314);//"初始化加密结构失败"
		return FALSE;
	}

	CString strPathName;

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;	//文件和目录信息列表
	CList<CDataSafe *, CDataSafe *> DirList;	//目录信息列表

	EnumList(&DataList, &DirList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			strPathName = GetPathName(pDataSafe, &DirList);
			if(pDataSafe->bIsFile) //文件
			{
				//结构中加密起始位置赋值
				CtxTemp = Ctx;
				File.Seek(pDataSafe->dBeginSeek, CFile::begin);
	

				dOutLen = CEvp::OpenSeal2(CtxTemp, File, pDataSafe->dNewLen, strPathName, outStr);
				if(-1 == dOutLen)
				{
					bRet =  FALSE;
					break;
				}
				else if(dOutLen != pDataSafe->dOldLen)
				{
					outStr.Format("%s%s", MiniCT_1321, strPathName);//文件%s大小校验失败
					bRet =  FALSE;
					break;
				}
			}
			else//目录 ->建立目录
			{
				if(!Mdir(strPathName, outStr))
				{
					bRet =  FALSE;
					break;
				}
			}

			m_ProgAll.SetPos(++iFinish);

		}
	}

	CleanList(&DataList);
	File.Close();
	CEvp::CleanOpenSeal2(Ctx);
	return bRet;
}

//公钥加密
BOOL CSafeDataSetPage::RSAPubEnc(const BYTE * pCertBuf, const UINT nCertLen,
								  CString strOutName, CString & outStr)
{

	RSA * pRsa = NULL;
	if(!CEvp::GetPubRsa(pCertBuf, nCertLen,pRsa))
	{
		outStr.Format("%s", MiniCT_1326); //取得公钥RSA结构失败
		return FALSE;
	}

	CFile File;//文件头加版本信息
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outStr.Format("%s%s", MiniCT_1313, strOutName);//创建文件失败%s
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//创建信息头
	if(!MadeFileHead(File, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;
	EnumList(&DataList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	CString strInfo;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			if(pDataSafe->bIsFile) //文件则加密
			{
				//结构中加密起始位置赋值
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
//				strInfo.Format("文件 %s ,大小 %d 加密中...", pDataSafe->cName, pDataSafe->dOldLen);
				AddOnceMsg(strInfo);
				pDataSafe->dBeginSeek = File.GetPosition();
				
				dOutLen = CEvp::RSAPubEnc2(pRsa, pDataSafe->cPathName, File, outStr);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s%s%s", MiniCT_1327, pDataSafe->cName, MiniCT_1319, outStr); //公钥加密文件失败%s,失败原因:%s
					outStr = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//结构赋值
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}

			m_ProgAll.SetPos(++iFinish);

			
		}
		
	}

	//创建信息颈
	if(!MadeFileNeck(&DataList, File, PubEnc, nCount, dFileInfoLen, outStr))
	{
		CleanList(&DataList);
		File.Close();
		CEvp::FreeRsa(pRsa);
		return FALSE;
	}
	CEvp::FreeRsa(pRsa);
	CleanList(&DataList);
	File.Close();
	return TRUE;

}

//私钥解密
BOOL CSafeDataSetPage::RSAPriDec(const BYTE * pCertBuf, const UINT nCertLen, CString strPwd, CString strEvpFile,
								  CString & outStr)
{
	RSA * pRsa = NULL;
	if(!CEvp::GetPrivRsa(pCertBuf, nCertLen, strPwd, pRsa))
	{
		outStr.Format("%s", MiniCT_1326);//"取得私钥RSA结构失败"
		return FALSE;
	}

	DWORD dOutLen = -1;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//打开文件失败%s
		return FALSE;
	}

	CString strPathName;

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;	//文件和目录信息列表
	CList<CDataSafe *, CDataSafe *> DirList;	//目录信息列表

	EnumList(&DataList, &DirList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			strPathName = GetPathName(pDataSafe, &DirList);
			if(pDataSafe->bIsFile) //文件
			{
				File.Seek(pDataSafe->dBeginSeek, CFile::begin);
				dOutLen = CEvp::RSAPrivDec2(pRsa, File, pDataSafe->dNewLen, strPathName, outStr);
				if(-1 == dOutLen)
				{
					bRet =  FALSE;
					break;
				}
				else if(dOutLen != pDataSafe->dOldLen)
				{
					outStr.Format("%s%s", MiniCT_1321, strPathName);
					bRet =  FALSE;
					break;
				}
			}
			else//目录 ->建立目录
			{
				if(!Mdir(strPathName, outStr))
				{
					bRet =  FALSE;
					break;
				}
			}

			m_ProgAll.SetPos(++iFinish);

		}
	}

	CleanList(&DataList);
	CEvp::FreeRsa(pRsa);
	File.Close();
	return bRet;
}

//私钥加密
BOOL CSafeDataSetPage::RSAPrivEnc(const BYTE * pCertBuf, const UINT nCertLen, CString strPwd,
								  CString strOutName, CString & outStr)//私钥加密
{

	RSA * pRsa = NULL;
	if(!CEvp::GetPrivRsa(pCertBuf, nCertLen, strPwd, pRsa))
	{
		outStr.Format("%s", MiniCT_1326);//"取得私钥RSA结构失败"
		return FALSE;
	}

	CFile File;//文件头加版本信息
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outStr.Format("%s%s", MiniCT_1313, strOutName);
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//创建信息头
	if(!MadeFileHead(File, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;
	
	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;
	EnumList(&DataList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	CString strInfo;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			if(pDataSafe->bIsFile) //文件则加密
			{
				//结构中加密起始位置赋值
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
	//			strInfo.Format("文件 %s ,大小 %d 加密中...", pDataSafe->cName, pDataSafe->dOldLen);
				AddOnceMsg(strInfo);

				pDataSafe->dBeginSeek = File.GetPosition();
				
				dOutLen = CEvp::RSAPrivEnc2(pRsa, pDataSafe->cPathName, File, outStr);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s%s%s", MiniCT_1328, pDataSafe->cName, MiniCT_1319, outStr);//私钥加密文件失败%s, 失败原因:%s
					outStr = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//结构赋值
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}

			m_ProgAll.SetPos(++iFinish);
			
		}
		
	}

	//创建信息颈
	if(!MadeFileNeck(&DataList, File, PrivEnc, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		CleanList(&DataList);
		CEvp::FreeRsa(pRsa);
		return FALSE;
	}
	CEvp::FreeRsa(pRsa);
	File.Close();
	CleanList(&DataList);
	return TRUE;

}

//公钥解密
BOOL CSafeDataSetPage::RSAPubDec(const BYTE * pCertBuf, const UINT nCertLen, CString strEvpFile,
								 CString & outStr)
{
	RSA * pRsa = NULL;
	if(!CEvp::GetPubRsa(pCertBuf, nCertLen, pRsa))
	{
		outStr.Format("%s", MiniCT_1326);
		return FALSE;
	}

	DWORD dOutLen = -1;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//打开文件失败%s
		return FALSE;
	}

	CString strPathName;

	//枚举目录列表加密信息
	CList<CDataSafe *, CDataSafe *> DataList;	//文件和目录信息列表
	CList<CDataSafe *, CDataSafe *> DirList;	//目录信息列表

	EnumList(&DataList, &DirList);
	POSITION pos = DataList.GetHeadPosition();
	int iCount = DataList.GetCount();
	m_ProgAll.SetRange(0, iCount);
	int iFinish = 0;
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = DataList.GetNext(pos);
		if(pDataSafe)
		{
			strPathName = GetPathName(pDataSafe, &DirList);
			if(pDataSafe->bIsFile) //文件
			{
				File.Seek(pDataSafe->dBeginSeek, CFile::begin);
				dOutLen = CEvp::RSAPubDec2(pRsa, File, pDataSafe->dNewLen, strPathName, outStr);
				if(-1 == dOutLen)
				{
					CString str;
					str.Format("%s%s%s%s", MiniCT_1329, pDataSafe->cPathName, MiniCT_1319, outStr);//公钥解密文件失败%s, 失败原因:%s
					outStr = str;
					bRet =  FALSE;
					break;
				}
				else if(dOutLen != pDataSafe->dOldLen)
				{
					outStr.Format("%s%s", MiniCT_1321, strPathName);
					bRet =  FALSE;
					break;
				}
			}
			else//目录 ->建立目录
			{
				if(!Mdir(strPathName, outStr))
				{
					bRet =  FALSE;
					break;
				}
			}

			m_ProgAll.SetPos(++iFinish);
		}
	}
	CleanList(&DataList);
	CEvp::FreeRsa(pRsa);
	File.Close();
	return bRet;
}
////////////////////////////////////////////////加密部分结束///////////////////////////////////////////////////


void CSafeDataSetPage::OnDblclkListDataset(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	//双击,如果是未加密的,则打开LIST中目录,文件则打开文件
	//如果是加密的,就解密到临时目录,并打开
	int nIndex = m_DataSetList.GetSelectedItem();
	if(nIndex == -1)
		return;
	CSuperGridCtrl::CTreeItem * pItem = m_DataSetList.GetTreeItem(nIndex);
	if(!pItem)
		return;
	CItemInfo * pInfo = m_DataSetList.GetData(pItem ); 
	
	const CDataSafe * pDataSafe = pInfo->GetDataSafe();
	if(pDataSafe == NULL)
		return;
	
	if(pDataSafe->bIsFile) //执行
	{
		CString strOut;
		CString strPathName;
		BOOL bSucceed = TRUE;
		if(m_bIsEnc)	//未加密文件
		{
			strPathName.Format("%s", pDataSafe->cPathName);
		}
		else	//解密到临时目录
		{
			//取得临时路径
			TCHAR szTempPath[MAX_PATH];
			DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
			ASSERT (dwResult);
			
			//创建临时文件
		//	TCHAR szTempFile[MAX_PATH];
		//	UINT nResult = GetTempFileName (szTempPath, _T ("~mc"), 0, szTempFile);

			strPathName.Format("%s\\%s",szTempPath, pDataSafe->cName);
			bSucceed = DecOneFile(m_strEvpPath, strPathName, pDataSafe->dBeginSeek, 
				pDataSafe->dNewLen, pDataSafe->dOldLen, strOut);
		}

		if(bSucceed)
			ShellExecute(this->m_hWnd, "open", strPathName, "", "", SW_SHOWNORMAL);
		else
			AddMsg(strOut, M_ERROR);
	}
	else//展开
	{
		BOOL bRedraw=0;
		if(m_DataSetList.ItemHasChildren(pItem))
		{
			m_DataSetList.SetRedraw(0);
			int nScrollIndex=0;
			if(m_DataSetList.IsCollapsed(pItem))
			{		
				if(m_DataSetList.OnItemExpanding(pItem, nIndex))
				{
					nScrollIndex = m_DataSetList.Expand(pItem, nIndex);
					m_DataSetList.OnItemExpanded(pItem, nIndex);
					bRedraw=1;
				}
			}	
			
			else 
			{
				if(m_DataSetList.OnCollapsing(pItem))
				{
					m_DataSetList.Collapse(pItem);
					m_DataSetList.OnItemCollapsed(pItem);
					bRedraw=1;
				}
			}
			m_DataSetList.SetRedraw(1);
			
			if(bRedraw)
			{
				CRect rc;
				m_DataSetList.GetItemRect(nIndex,rc,LVIR_BOUNDS);
				m_DataSetList.InvalidateRect(rc);
				m_DataSetList.UpdateWindow();
				m_DataSetList.EnsureVisible(nScrollIndex,1);
			}
		}//ItemHasChildren	
	}
	
	*pResult = 0;
}

//只是解密时候用,加密时候结构中保存了路径信息
//给定某个结构信息,附加父信息在其上面
//例如,结构为文件结构,文件名为minica.rar
//父结构肯定为目录结构,目录名为minica
//minica的父结构也存在,目录名为vc
//vc没有分目录
//最后的附加信息为:vc\\minica\\minica.rar
//参数:	pDataSafe - 当前的结构信息
//		uMinPid - 最小父结构ID
//解密时候可能不从根开始解密,这时候需要传递父lpParentItem
CString CSafeDataSetPage::GetPathName(CDataSafe* pDataSafe, 
									  CList<CDataSafe *, CDataSafe *> * pDataList)
{
	CString strAllPath, strPathName;//全部路径

	if(!pDataSafe && !pDataList)
		return strAllPath;

	if(pDataSafe->uParentID == 0)//标示没有父
		strPathName = pDataSafe->cName;
	else
	{
		
		for(CDataSafe* pDataTemp = pDataSafe; ;)
		{
			strPathName = pDataTemp->cName + strPathName;
			strPathName = "\\" + strPathName;
			if(pDataTemp->uParentID == 0)
				break;
			POSITION pos = pDataList->FindIndex( pDataTemp->uParentID - 1);
			if(pos)
			{
				pDataTemp = pDataList->GetAt(pos);
			}
			else
				break;
		}
	}


	strAllPath.Format("%s\\%s", m_strSavePath, strPathName);					//解压到
	if(strAllPath.GetAt(strAllPath.GetLength() - 1) == '\\')
		strAllPath = strAllPath.Left(strAllPath.GetLength() - 1);
	return strAllPath;
}

BOOL CSafeDataSetPage::Mdir(CString strPathName, CString & strInfo)
{
	strPathName += "\\";
	return CMiniCaApp::MakeSureDirectoryPathExists(strPathName);
	/*if(_mkdir(strPathName) == -1)
	{
		if(errno==ENOENT)//目录没有发现
		{
			strInfo.Format("建立目录%s失败!",strPathName);
			return FALSE;
		}
	}*/

}

void CSafeDataSetPage::OnRadio1() //对称加密按钮
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_CHIPNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_PWD)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_CRYPT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PWD)->EnableWindow(TRUE);
}

void CSafeDataSetPage::OnRadio2() //信封加密按钮
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_CHIPNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_PWD)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_CRYPT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PWD)->EnableWindow(FALSE);
}

void CSafeDataSetPage::OnRadio3() //摘要
{
	// TODO: Add your control notification handler code here
	
}

void CSafeDataSetPage::OnRadio4() //签名
{
	// TODO: Add your control notification handler code here
	
}

void CSafeDataSetPage::OnRadio5() //公钥加密按钮
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_CHIPNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_PWD)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_CRYPT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PWD)->EnableWindow(FALSE);
}

void CSafeDataSetPage::OnRadio6() //私钥加密按钮
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_CHIPNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_PWD)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_CRYPT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PWD)->EnableWindow(FALSE);
}

void CSafeDataSetPage::OnBEnc() 
{
	// TODO: Add your control notification handler code here
	if(pThreadEvpSet != NULL )
		return;
	
	//检测列表中是否有记录
//	int nCount = ;

	if(m_DataSetList.GetCount() == 0)
	{
		AddMsg(MiniCT_1308, M_ERROR);
		return;
	}

	CEvp::SetProgRess(CSafeDataSetPage::m_pProgOne);

	if(m_bIsEnc)//加密
	{
		if(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck())//对称加密
		{
			m_EncType = CRYPT;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())//信封加密
		{
			m_EncType = SEAL;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck())//摘要
		{
			m_EncType = DIGEST;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO4))->GetCheck())//签名
		{
			m_EncType = SIGN;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO5))->GetCheck())//公钥加密
		{
			m_EncType = PubEnc;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO6))->GetCheck())//私钥加密
		{
			m_EncType = PrivEnc;
		}
		//出现令存对话框
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT /*| OFN_NOCHANGEDIR */,
			MiniCT_1345, NULL);
		dlgOpen.m_ofn.lStructSize = sizeof(OPENFILENAME);
		CString str = MiniCT_1343;
		dlgOpen.m_ofn.lpstrTitle = str;//标题条 LPCTSTR
		if(dlgOpen.DoModal()!=IDOK) return;
		m_strEvpPath = dlgOpen.GetPathName();
		CString strExt = m_strEvpPath.Right(4);
		if(strExt.CollateNoCase(".Evp") != 0)
			m_strEvpPath += ".Evp";
		
	}
	else
	{
		//出现解密到对话框
		if(!SelectPath())
			return;
	}

	pThreadEvpSet = ::AfxBeginThread(_EvpSetThread,this,THREAD_PRIORITY_IDLE);	

}

void CSafeDataSetPage::OnKeydownListDataset(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(!m_bIsEnc) //解密,不能删除
		return;
	switch(pLVKeyDow->wVKey)
	{
	case VK_DELETE: //只能删除父目录
		{
			int nItem = m_DataSetList.GetSelectedItem();
			if(nItem!=-1)
			{
				CSuperGridCtrl::CTreeItem * pSelItem = m_DataSetList.GetTreeItem(nItem);
				
				CItemInfo * pInfo = m_DataSetList.GetData(pSelItem ); 
				if(pInfo->GetDataSafe()->uParentID != 0) //0为根的父
				{
					return;
				}
				
				//删除拖动链表
				POSITION pos = m_DirName.GetHeadPosition();
				while ( NULL != pos )
				{
					if(m_DirName.GetAt(pos).Compare(pInfo->GetDataSafe()->cPathName) == 0)//删除
					{
						m_DirName.RemoveAt(pos);
						break;
					}
					m_DirName.GetNext(pos);
				}
				
				m_DataSetList.DeleteItemEx(pSelItem, nItem);
			}					
		}
		break;
	}
	
	
	*pResult = 0;
}

//记录结构信息的MAP,在加密解密之前先要枚举列表到MAP中
void CSafeDataSetPage::EnumList(CList<CDataSafe *, CDataSafe *> * pDataList, CList<CDataSafe *, CDataSafe *> * pDirList)
{
	//枚举目录列表加密信息
	POSITION pos = m_DataSetList.GetRootHeadPosition();
	while(pos != NULL)
	{
		CSuperGridCtrl::CTreeItem *pParent = m_DataSetList.GetNextRoot(pos); 
		CSuperGridCtrl::CTreeItem *pItem = pParent;
		CItemInfo* lp = m_DataSetList.GetData(pParent);
		CDataSafe * pDataSafe = new CDataSafe();
		lp->CopyDataSafe(pDataSafe);
		pDataList->AddTail(pDataSafe);
		if(pDirList) //解密时候需要建立目录,此时需要添加目录列表
		{
			if(!pDataSafe->bIsFile)//目录
				pDirList->AddTail(pDataSafe);
		}
		
		//GetNext ....loop through all children 
		for(;;)
		{
			CSuperGridCtrl::CTreeItem *pCur = m_DataSetList.GetNext(pParent, pItem, TRUE, FALSE/*regardless of the item are hidden or not, set to TRUE if only visible items must be included in the search*/);	  
			if(!m_DataSetList.IsChildOf(pParent, pCur))
				break;
			else if(pCur==pItem)
				break;
			CItemInfo* lp = m_DataSetList.GetData(pCur);
			pItem = pCur;//next;
			CDataSafe * pDataSafe = new CDataSafe();
			lp->CopyDataSafe(pDataSafe);
			pDataList->AddTail(pDataSafe);
			if(pDirList) //解密时候需要建立目录,此时需要添加目录列表
			{
				if(!pDataSafe->bIsFile)//目录
					pDirList->AddTail(pDataSafe);
			}

		}
	}
}

void CSafeDataSetPage::CleanList(CList<CDataSafe *, CDataSafe *> * pDataList)
{
	POSITION pos = pDataList->GetHeadPosition();
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = pDataList->GetNext(pos);
		if(pDataSafe)
			delete pDataSafe;
	}

}

void CSafeDataSetPage::DoCrypt()
{
	CString strCpName, strPwd;
	GetDlgItemText(IDC_COMBO_CRYPT,strCpName);
	GetDlgItemText(IDC_EDIT_PWD,strPwd);
	if(strCpName.IsEmpty() || strPwd.IsEmpty())
	{
		AddMsg(MiniCT_1330, M_ERROR);//请输入加密密钥和算法名称
		pThreadEvpSet = NULL;
		return;
	}
	CString outMsg;
	if(!Crypt(strCpName, strPwd, m_strEvpPath, outMsg))
	{
		AddMsg(outMsg, M_ERROR);
	}
	else
		AddMsg(MiniCT_1331);//对称加密成功
	pThreadEvpSet = NULL;
	
}

void CSafeDataSetPage::DoSeal()
{
	//创建合法用户公钥链
	CEvp::stuCertLink * pCertLink = NULL;
	int nCount = 0;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SEAL, TRUE, m_strKey, 
		m_lenKey,m_p12Pwd,&((CMiniMainDlg *)m_pParent)->m_ListBox))//得到公钥
	{
		//可能为多证书,这时查找;,然后添加每一个
		if(m_lenKey == 0) //外部证书
		{
			CString strSub,
				    strPath;
			while(AfxExtractSubString(strSub, m_strKey, nCount++, ';'))	//未公开 分解字符串
			{
				if(nCount == 1) //第一个文件,取得路径
				{
					int nPos = strSub.ReverseFind('\\');
					strPath = strSub.Left(nPos + 1);//附带
					strSub.TrimLeft();
					pCertLink->AddCert(pCertLink, strSub, m_lenKey);
				}
				else
				{
					strSub.TrimLeft();
					pCertLink->AddCert(pCertLink, strPath + strSub, m_lenKey);
				}

			}
		}
		else
			pCertLink->AddCert(pCertLink, m_strKey, m_lenKey);
	}
	
	CString strCpName;
	GetDlgItemText(IDC_COMBO_CRYPT,strCpName);
	if(strCpName.IsEmpty())
	{
		AddMsg(MiniCT_1332, M_ERROR); //请输入加密算法名称
		pThreadEvpSet = NULL;
		return;
	}
	CString outMsg;
	if(!Seal(pCertLink, strCpName, m_strEvpPath, outMsg))
	{
		AddMsg(outMsg, M_ERROR);
	}
	else
		AddMsg(MiniCT_1333);//封装信封成功
	pCertLink->RemoveAll(pCertLink);
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoRSAPubEnc()
{
	CString outStr;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, TRUE, m_strKey,
		m_lenKey,m_p12Pwd, &((CMiniMainDlg *)m_pParent)->m_ListBox))//得到公钥
	{
		if(!RSAPubEnc((UCHAR *)m_strKey, m_lenKey, m_strEvpPath, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1334);//公钥加密成功
	}
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoRSAPrivEnc()
{
	CString outStr;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, FALSE, m_strKey, 
		m_lenKey, m_p12Pwd, &((CMiniMainDlg *)m_pParent)->m_ListBox))//得到私钥
	{ 
 		if(!RSAPrivEnc((UCHAR *)m_strKey, m_lenKey, m_p12Pwd, m_strEvpPath, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1335);//私钥加密成功
	}
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoDecCrypt()
{
	CString strCpName, strPwd, outStr;
	GetDlgItemText(IDC_COMBO_CRYPT, strCpName);
	GetDlgItemText(IDC_EDIT_PWD, strPwd);
	CString strEvpFile(m_strEvpPath);
	if(strPwd.IsEmpty() || strCpName.IsEmpty())
	{
		AddMsg(MiniCT_1336, M_ERROR);		 //请选择算法并输入加密密码
		pThreadEvpSet = NULL;
		return;
	}
	if(!DecCrypt(strCpName, strPwd, strEvpFile, outStr))
	{
		AddMsg(outStr, M_ERROR);
	}
	else
		AddMsg(MiniCT_1337); //对称解密成功
	pThreadEvpSet = NULL;
	
}

void CSafeDataSetPage::DoOpenSeal()
{
	CString strCpName, outStr;
	GetDlgItemText(IDC_COMBO_CRYPT, strCpName);
	CString strEvpFile(m_strEvpPath);
	if(strCpName.IsEmpty())
	{
		AddMsg(MiniCT_1338, M_ERROR);		 //"请选择算法"
		pThreadEvpSet = NULL;
		return;
	}
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SEAL, FALSE, m_strKey, m_lenKey, m_p12Pwd,
		&((CMiniMainDlg *)m_pParent)->m_ListBox))//得到私钥
	{
		if(!OpenSeal(m_strKey, m_lenKey, m_p12Pwd.GetBuffer(0), strCpName, strEvpFile, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1339); //拆封信封成功
		m_p12Pwd.ReleaseBuffer();
	}
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoRSAPriDec()
{
	CString outStr;
	CString strEvpFile(m_strEvpPath);
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, FALSE, m_strKey, m_lenKey, m_p12Pwd,
		&((CMiniMainDlg *)m_pParent)->m_ListBox))//得到私钥
	{
		if(!RSAPriDec((UCHAR *)m_strKey, m_lenKey, m_p12Pwd, strEvpFile, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1340); //私钥解密成功
	}
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoRSAPubDec()
{
	CString outStr;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PrivEnc, TRUE, m_strKey, m_lenKey, m_p12Pwd,
		&((CMiniMainDlg *)m_pParent)->m_ListBox))
	{
		if(!RSAPubDec((UCHAR *)m_strKey, m_lenKey, m_strEvpPath, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1341);
	}
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)m_pParent)->AddMsg(MiniCT_0002,info, type);
}

void CSafeDataSetPage::AddOnceMsg(CString info, DWORD type)
{
//	((CMiniMainDlg *)m_pParent)->AddMsg("WinEvp",info, type);
}

void CSafeDataSetPage::DoSign()
{
	CString strCpName;
	GetDlgItemText(IDC_COMBO_CRYPT,strCpName);
	if(strCpName.IsEmpty())
	{
		AddMsg(MiniCT_1332, M_ERROR);
		pThreadEvpSet = NULL;
		return;
	}
	CString outStr;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SIGN, FALSE, m_strKey, m_lenKey, 
		m_p12Pwd, &((CMiniMainDlg *)m_pParent)->m_ListBox))//得到私钥
	{
		if(!Sign("md5", (UCHAR *)m_strKey, m_lenKey, m_p12Pwd,
			m_strEvpPath, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1342); //"数字签名成功"
	}
	pThreadEvpSet = NULL;
}

LRESULT CSafeDataSetPage::OnDropOk(WPARAM wParam, LPARAM lParam)
{
	CStringArray * pStringArray = (CStringArray*) wParam;

	for(int i = 0 ;i < pStringArray->GetSize(); i++)
	{
		AddList(pStringArray->GetAt(i));		//如果扩展名不为.evp
	}
	return 0;
}

//解密文件
//参数:
//EVP文件名
//输出文件名
//文件在EVP文件中的偏移
//加密后文件长度(也就是解密时候要读取的长度)
//原始文件长度,用于校验
//返回错误信息
BOOL CSafeDataSetPage::DecOneFile(CString strEvpFile, CString strOutFile, DWORD dSeek, 
								  DWORD dFileLen, DWORD dOldLen, CString & outStr)
{
	CFile FileEvp;
	CString strCpName;
	DWORD dOutLen = -1;
	DWORD dFileInfoLen = 0; 

	BOOL bSucceed = TRUE;
	
	if(!FileEvp.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//打开文件失败%s
		return FALSE;
	}

	switch(m_EncType)
	{
	case NONE:
		break;
	case CRYPT:	//对称 1
		break;
	case DIGEST://摘要 3
		break;
	case SIGN:	//签名 4
		break;
	case SEAL:	//信封 2
			FileEvp.Seek(strlen(m_sHeadMark), CFile::begin);
			FileEvp.Read(&dFileInfoLen, sizeof(DWORD));
			FileEvp.Seek(sizeof(UINT), CFile::current);
			FileEvp.Seek(dFileInfoLen, CFile::current);
			GetDlgItemText(IDC_COMBO_CRYPT, strCpName);
			if(strCpName.IsEmpty())
			{
				AddMsg(MiniCT_1338,M_ERROR);//	"请选择算法"	
				break;
			}

			EVP_CIPHER_CTX Ctx;

			if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SEAL, FALSE, m_strKey, m_lenKey, m_p12Pwd,
									&((CMiniMainDlg *)m_pParent)->m_ListBox))//得到私钥

			if(!CEvp::InitOpenSeal2(m_strKey, m_lenKey, m_p12Pwd.GetBuffer(0), strCpName, FileEvp, Ctx, outStr))
			{
				m_p12Pwd.ReleaseBuffer();
				FileEvp.Close();
				return FALSE;
			}		
			m_p12Pwd.ReleaseBuffer();
			FileEvp.Seek(dSeek, CFile::begin);
			dOutLen = CEvp::OpenSeal2(Ctx, FileEvp, dFileLen, strOutFile, outStr);
			if(dOutLen != dOldLen)
			{
				bSucceed = FALSE;
				outStr = MiniCT_1321;
			}
			CEvp::CleanOpenSeal2(Ctx);
			break;
	case PubEnc:
		break;
	case PrivEnc:
		break;
	default:
		break;
	}	
	FileEvp.Close();
	return bSucceed;

}

void CSafeDataSetPage::OnBegindragListDataset(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW*    pNMLV = (NMLISTVIEW*) pNMHDR;
	COleDataSource datasrc;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	CStringList    lsDraggedFiles;
	POSITION       pos;
	int            nSelItem;
	CString        sFile;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	CString		   outStr;
	
    *pResult = 0;   // return value ignored
	
    // For every selected item in the list, put the filename into lsDraggedFiles.
	
	pos = m_DataSetList.GetFirstSelectedItemPosition();
	
	while ( NULL != pos )
	{
		nSelItem = m_DataSetList.GetNextSelectedItem ( pos );

		//如果是加密的,就解密到临时目录,并打开
		CSuperGridCtrl::CTreeItem * pItem = m_DataSetList.GetTreeItem(nSelItem);
		if(!pItem)
			return;
		CItemInfo * pInfo = m_DataSetList.GetData(pItem); 
	
		const CDataSafe * pDataSafe = pInfo->GetDataSafe();
		if(pDataSafe == NULL)
			return;

		//取得临时路径
		TCHAR szTempPath[MAX_PATH];
		DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
		ASSERT (dwResult);
		
		sFile.Format("%s\\%s",szTempPath, pDataSafe->cName);

		CString strOut;

		if(pDataSafe->bIsFile) //文件
		{
			if(!DecOneFile(m_strEvpPath, sFile, pDataSafe->dBeginSeek, pDataSafe->dNewLen,
				pDataSafe->dNewLen, strOut))
				AddMsg(strOut, M_ERROR);
		}
		else//目录 ->建立目录
		{
			if(!Mdir(sFile, outStr))
			{
				return;
			}
		}
		
		lsDraggedFiles.AddTail ( sFile );
		
		// Calculate the # of chars required to hold this string.
		
		uBuffSize += lstrlen ( sFile ) + 1;
	}
	
    // Add 1 extra for the final null char, and the size of the DROPFILES struct.
	
    uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);
	
    // Allocate memory from the heap for the DROPFILES struct.
	
    hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );
	
    if ( NULL == hgDrop )
        return;
	
    pDrop = (DROPFILES*) GlobalLock ( hgDrop );
	
    if ( NULL == pDrop )
	{
        GlobalFree ( hgDrop );
        return;
	}
	
    // Fill in the DROPFILES struct.
	
    pDrop->pFiles = sizeof(DROPFILES);
	
#ifdef _UNICODE
    // If we're compiling for Unicode, set the Unicode flag in the struct to
    // indicate it contains Unicode strings.
	
    pDrop->fWide = TRUE;
#endif
	
    // Copy all the filenames into memory after the end of the DROPFILES struct.
	
    pos = lsDraggedFiles.GetHeadPosition();
    pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));
	
    while ( NULL != pos )
	{
        lstrcpy ( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
        pszBuff = 1 + _tcschr ( pszBuff, '\0' );
	}
	
    GlobalUnlock ( hgDrop );
	
    // Put the data in the data source.
	
    datasrc.CacheGlobalData ( CF_HDROP, hgDrop, &etc );
	
    // Add in our own custom data, so we know that the drag originated from our 
    // window.  CMyDropTarget::DragEnter() checks for this custom format, and
    // doesn't allow the drop if it's present.  This is how we prevent the user
    // from dragging and then dropping in our own window.
    // The data will just be a dummy bool.
	HGLOBAL hgBool;
	
    hgBool = GlobalAlloc ( GHND | GMEM_SHARE, sizeof(bool) );
	
    if ( NULL == hgBool )
	{
        GlobalFree ( hgDrop );
        return;
	}
	
    // Put the data in the data source.
	
    etc.cfFormat = m_Target.GetClipbrdFormat();
    
    datasrc.CacheGlobalData (etc.cfFormat, hgBool, &etc );
	
	
    // Start the drag 'n' drop!
	
	DROPEFFECT dwEffect = datasrc.DoDragDrop (DROPEFFECT_MOVE);
	
    // If the DnD completed OK, we remove all of the dragged items from our
    // list.
				
    GlobalFree(hgDrop);
    GlobalFree(hgBool);
}

void CSafeDataSetPage::TranslateCT()
{
	CHeaderCtrl * pHeader = m_DataSetList.GetHeaderCtrl();
	if (pHeader)
	{
		UINT uCount = pHeader->GetItemCount();

		CString str = MiniCT_1300;
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(0, &hdi);

		str = MiniCT_1301;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(1, &hdi);

		str = MiniCT_1302;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(2, &hdi);

		str = MiniCT_1303;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(3, &hdi);

		str = MiniCT_1304;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(4, &hdi);
	}
	SetDlgItemText(IDC_STATICINFO,	MiniCT_11401);
	SetDlgItemText(IDC_RADIO1,		MiniCT_11402);
	SetDlgItemText(IDC_RADIO2,		MiniCT_11403);
	SetDlgItemText(IDC_RADIO3,		MiniCT_11404);
	SetDlgItemText(IDC_RADIO4,		MiniCT_11405);
	SetDlgItemText(IDC_RADIO5,		MiniCT_11406);
	SetDlgItemText(IDC_RADIO6,		MiniCT_11407);
	SetDlgItemText(IDC_STATICFUN,	MiniCT_11408);
	SetDlgItemText(IDC_STATIC_PWD,	MiniCT_11409);
	SetDlgItemText(IDC_STATIC_CHIPNAME,	MiniCT_11410);
}

BOOL CSafeDataSetPage::SelectPath() 
{
	// TODO: Add your control notification handler code here
	LPITEMIDLIST pidlRoot = NULL; 
	LPMALLOC pMalloc;   
	if(NOERROR == SHGetSpecialFolderLocation(m_pParent->m_hWnd, CSIDL_DRIVES, &pidlRoot))	//leak
	{
		BROWSEINFO bi;   //必须传入的参数,下面就是这个结构的参数的初始化 
		CString strDisplayName;// = CMiniCaApp::NormalCode("选择文件夹");;   //用来得到,你选择的活页夹路径,相当于提供一个缓冲区 
		bi.hwndOwner = GetSafeHwnd();   //得到父窗口Handle值 
		bi.pidlRoot=0;   //这个变量就是我们在上面得到的. 
		bi.pszDisplayName = strDisplayName.GetBuffer(MAX_PATH+1);   //得到缓冲区指针, 
		CString strTitle = MiniCT_1344;	//解密到
		bi.lpszTitle = strTitle;  //设置标题 
		bi.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_DONTGOBELOWDOMAIN  ;   //设置标志 
		bi.lpfn=NULL; 
		bi.lParam=0; 
		bi.iImage=0;   //上面这个是一些无关的参数的设置,最好设置起来, 
		ITEMIDLIST * pidl;  
		pidl = SHBrowseForFolder(&bi);   //打开对话框 
		if(!pidl)
			return FALSE;
		SHGetPathFromIDList(pidl,bi.pszDisplayName);	
		strDisplayName.ReleaseBuffer();   //和上面的GetBuffer()相对应 
	
		if(SUCCEEDED( SHGetMalloc ( &pMalloc ))) // leak
		{ 
			pMalloc->Free(pidlRoot); 
			pMalloc->Release(); 
		}
		m_strSavePath = strDisplayName;
	}
	return TRUE;
}
