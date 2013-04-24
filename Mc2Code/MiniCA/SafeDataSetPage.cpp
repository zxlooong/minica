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

//���ܺ��ļ���ʽ����
/*
------------------------------------��Ϣͷ��ʼ-------------------------------------------------
�ļ��汾��� - Evp20
��Ϣ������ - DWORD
��Ϣ������CRC�� - UINT
------------------------------------��Ϣ����ʼ-------------------------------------------------
��Ϣ������  ����ʽ ��Կ����
             ����:
			      ���ܷ�ʽ ENUM
				  ��Ϣ�����n INT
			      n����Ϣ��CDataSafe

---------------------------------���������ŷ�:���ﻹҪ����-------------------------------------
			------------------------EVP��Ϣͷ��ʼ----------------------------------
//			���ܺ�EVP��Ϣ������	DWORD
//			���ܺ�EVP��Ϣ������ CRCУ����	UINT
//			���ܺ�EVP��Ϣ��
//			-------------------------------EVP��Ϣ����ʼ---------------------------
//			��Կ����n
//			n��
//				��Կ���ܵĶԳ���Կ����
//				��Կ���ܵĶԳ���Կ����
//			����IV
//			-------------------------------EVP��Ϣ������---------------------------
------------------------------------------------------------------------------------------------
------------------------------------n�������ļ����ݿ�ʼ------------------------------------------------
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
------------------------------------n�������ļ����ݽ���------------------------------------------------


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
	
	
	//���XP��������ˢ������
	//�������,ˢ�¸�����
	p->m_ProgOne.SetPos(0);
	//�õ��������ؼ�����
	CRect rectProg;
	p->m_ProgOne.GetWindowRect(&rectProg);
	p->ScreenToClient(rectProg);//
	
	  //ָ��������ˢ������
	p->InvalidateRect(rectProg, TRUE);

	if(p->GetIsEnc())//����
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
	else	//����
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


	//��ʼ��ͼ���б�
	SHFILEINFO shfi = {0};

	HIMAGELIST m_hImlIcons = (HIMAGELIST)SHGetFileInfo(NULL, 0, 
			&shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	//�õ�ϵͳImageList
	//�Ժ�ͨ���õ��ļ���ͼ���������ͼ��

	m_Image.Attach(m_hImlIcons);

	m_DataSetList.SetImageList(&m_Image, LVSIL_SMALL);

	m_DataSetList.SetExtendedStyle(LVS_EX_GRIDLINES);

	m_DataSetList.InsertColumn(0, MiniCT_1300, LVCFMT_LEFT, 150);
	m_DataSetList.InsertColumn(1, MiniCT_1301, LVCFMT_LEFT, 75);
	m_DataSetList.InsertColumn(2, MiniCT_1302, LVCFMT_LEFT, 70);
	m_DataSetList.InsertColumn(3, MiniCT_1303, LVCFMT_LEFT, 70);
	m_DataSetList.InsertColumn(4, MiniCT_1304, LVCFMT_LEFT, 100);
//	m_DataSetList.InsertColumn(3, "����", LVCFMT_LEFT, 100);

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
��������:���б�����Ӽ�¼
strDataSet - ��¼����
strFileLen - ��¼��С
strDate - �޸�ʱ��
uImage - ��ʾ��ͼ������
pParent - ���ڵ���,NULL��ʾ���Ϊ���ڵ�
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
	lpItemInfo->AddSubItemText(strOldLen);		//ԭʼ��С
	lpItemInfo->AddSubItemText(strNewLen);		//���ܺ��С
	lpItemInfo->AddSubItemText(strType);	//����˵��
	lpItemInfo->AddSubItemText(strTime);	//�޸�ʱ��

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

//����ļ����ṹMAP
//ö�ٵ��ļ�ʱ��Ĳ���
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
	FileTimeToLocalFileTime(&FileData.ftLastWriteTime, &ftime); // ת���ɱ���ʱ�� 
	FileTimeToSystemTime(&ftime, &stime); // ת����ϵͳʱ���ʽ
	
	//�ļ���С
  //  StrFormatByteSize(rFind.nFileSizeLow, szFileLen, 64 );

	dFileLen = (FileData.nFileSizeHigh * (MAXDWORD+1)) + FileData.nFileSizeLow;
	
	//ʱ��
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
		if(m_DirName.GetNext(pos).Compare(sLocalDir) == 0)//�ظ�
			return TRUE;
    }
	m_DirName.AddHead(sLocalDir);
	//�ж��Ƿ�Ϊ����,���Ϊ����,��������*.*
	if(sLocalDir.GetLength() == 3 && sLocalDir.Find(":") != -1)
		sLocalDir += "*.*";
	return EnumDir(sLocalDir, uPid, hPitem);
}

/*BOOL CSafeDataSetPage::GetDirTime(CString DirName, FILETIME &ftime, CString & strTime)
{
	FILETIME lpCreationTime; // �ļ��еĴ���ʱ�� 
	FILETIME lpLastAccessTime; // ���ļ��е��������ʱ�� 
	FILETIME lpLastWriteTime; // �ļ��е�����޸�ʱ�� 
	// ���ļ���
	HANDLE hDir = CreateFile (DirName, GENERIC_READ, 
		FILE_SHARE_READ|FILE_SHARE_DELETE, 
		NULL, OPEN_EXISTING, 
		FILE_FLAG_BACKUP_SEMANTICS, NULL); 

	if(INVALID_HANDLE_VALUE == hDir)
		return FALSE;
	// ��ȡ�ļ���ʱ��������Ϣ
	SYSTEMTIME stime;

	if (GetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
	{ 
		FileTimeToLocalFileTime(&lpLastWriteTime, &ftime); // ת���ɱ���ʱ�� 
		FileTimeToSystemTime(&ftime, &stime); // ת����ϵͳʱ���ʽ
	} 
	CloseHandle(hDir); // �رմ򿪹����ļ���

	strTime.Format("%d-%d-%d %d:%d", stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute);

	return TRUE;
}*/


//ö��Ŀ¼,������ļ�,ֱ����ӵ��б���
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

			FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &ftime); // ת���ɱ���ʱ�� 
			FileTimeToSystemTime(&ftime, &stime); // ת����ϵͳʱ���ʽ

			strTime.Format("%d-%d-%d %d:%d", stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute);

			CString strFileName = strPath;
			strFileName.Replace("*.*", wfd.cFileName);

			CDataSafe DataSafe(wfd.cFileName, 0, wfd.ftLastWriteTime, FALSE, uPid, strFileName);

			pItem = AddDataSet(&DataSafe, wfd.cFileName, sfi.iIcon, 0, 0, sfi.szTypeName, strTime, hPitem);

			uTempParent = ++m_uIndex;

			//����Ŀ¼
			CString strPathName = strPath;	//ö�ٴ��ļ����������������
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
			//�ۼ������ļ�����,Ȼ����ʾ���б���,���޸ĸ��ṹ����
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
	str.Format("�ļ���%s ƫ��%d ����%d", pDataSafe->cName, pDataSafe->dBeginSeek, pDataSafe->dNewLen);
	AfxMessageBox(str);
	*pResult = 0;
}


//����Ӳ�������.evp�ļ�,��ȡ�ļ���Ϣ,��д��MAP,Ȼ����ʾ���б���
BOOL CSafeDataSetPage::ParseEvp(CString strEvpFile, EncType & encType)
{
	m_uIndex = 0; //���Ŀ¼ʱ����0,��Ϊ���ܶ�����

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(MiniCT_1320);
		return FALSE;
	}
	//��ȡ�ļ�ǰ5����ĸ,�ж��Ƿ�ΪEvp20
	char sHeadMark[6] = {0};
	File.Read(sHeadMark, strlen(m_sHeadMark));
	if(strcmp(sHeadMark, m_sHeadMark)!=0)
	{
		//��EVP�ļ�,����
		File.Close();
		AfxMessageBox(MiniCT_1305);
		return FALSE;
	}
	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));
	//У��CRC,����UINT
	UINT uCrcCheck = 0;
	File.Read(&uCrcCheck, sizeof(UINT));
	if(!CRC::CheckCrc((char *)&dFileInfoLen, sizeof(DWORD), uCrcCheck))
	{
		AfxMessageBox(MiniCT_1306);
		File.Close();
		return FALSE;
	}

	//���ٻ�����,���ɼ��ܺ����Ϣ��
	BYTE * pIn = new BYTE[dFileInfoLen + 1];
	File.Read(pIn, dFileInfoLen);

	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_KEY),"CERT");
	DWORD lenKey = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgKey = LoadResource(NULL,hRsrc);
	LPSTR lpKey = (LPSTR)LockResource(hgKey);

	DWORD dOutLen = dFileInfoLen;

	//���ٻ�����,���ɽ��ܺ����Ϣ��
	BYTE * pOut = new BYTE[dOutLen];

	CString outMsg;


	if(!CEvp::RSAPrivDec((BYTE *)lpKey, lenKey, "MiniCA", pIn, 
		dFileInfoLen, pOut, dOutLen, outMsg.GetBuffer(256)))
	{
		//����ʧ��
		AfxMessageBox(MiniCT_1307);
		File.Close();
		delete [] pIn;
		delete [] pOut;
		return FALSE;
	}

	CMemFile MemFile;
	MemFile.Attach(pOut, dOutLen);

	//�����Ǽ��ܷ�ʽ
	MemFile.Read(&encType, sizeof(EncType));
	m_EncType = encType;

	//Ȼ���ǽṹ�����
	UINT uIndex = 0;
	MemFile.Read(&uIndex, sizeof(uIndex));

	CSuperGridCtrl::CTreeItem * pItem[1024];	//ֻ��¼�ļ���
	memset(pItem, 0, sizeof(CSuperGridCtrl::CTreeItem * ) * 1024);
	
	CMap<UINT, UINT, CSuperGridCtrl::CTreeItem *, CSuperGridCtrl::CTreeItem * > ItemMap;//��¼Ŀ¼ITEM��MAP
	ItemMap.SetAt(0, NULL);

	SHFILEINFO sfi = {0};


	//�����n���ṹ��
	for(UINT index = 1, j = 1; index <= uIndex; index++, m_uIndex++)	//��ȡ�ļ���Ϣ���ṹMAP
	{
		CDataSafe DataSafe;
		DataSafe.Read(MemFile);
		
		CSuperGridCtrl::CTreeItem * pParentItem = NULL;
		if(ItemMap.Lookup(DataSafe.uParentID, pParentItem))
		{
			
			SYSTEMTIME stime;
			FILETIME ftime; 
			FileTimeToLocalFileTime(&DataSafe.ftLastWriteTime, &ftime); // ת���ɱ���ʱ�� 
			FileTimeToSystemTime(&ftime, &stime); // ת����ϵͳʱ���ʽ
			
			//ʱ��
			CString strTime;
			strTime.Format("%d-%d-%d %d:%d", stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute);
			
			if(!DataSafe.bIsFile)//Ŀ¼,��¼����
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
	//��������ɹ�,��ת��Ϊ���ܷ�ʽ,��Ҫ���,��ť�ȵı仯
//	SetDlgItemText(IDC_B_ENC, "����");
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
	case CRYPT:	//�Գ� 1
		OnRadio1();
		GetDlgItem(IDC_RADIO1)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		break;
	case DIGEST:	//ժҪ 3
		OnRadio3();
		GetDlgItem(IDC_RADIO3)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		break;
	case SIGN:		//ǩ�� 4
		OnRadio3();
		GetDlgItem(IDC_RADIO4)->EnableWindow(1);
		((CButton *)GetDlgItem(IDC_RADIO4))->SetCheck(1);
		break;
	case SEAL:	//�ŷ� 2
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
	m_strEvpPath = strEvpFile;	//�O�ý����ļ�
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


//����:
		//д�� �汾Evp20, ���ܽṹ����, ���ܽṹУ��
		//�ƶ��ļ�������λ��(�ӹ���Ϣ��)
//����:
		//File - ���������ļ���
		//nCount - �ļ���Ϣ�����
		//dFileInfoLen - ���ؼ��ܺ󳤶�
		//strMsg - ���ز���������Ϣ
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
	
	
	//ȡ�ü��ܺ��ļ���Ϣ���С
	//�ļ���Ϣ��Ϊ: ���ܷ�ʽsizeof(EncType) +  sizeof(int)(��Ϣ����� int ����) + n��CDataSafe�ṹ��СnCount * (sizeof(CDataSafe) - sizeof(char)*256
	//����CDataSafe�ṹ��д���ļ�ȫ·��,���� - sizeof(char)*256
	//
	dFileInfoLen = CEvp::GetEncLen(lpCert, lenCert, 
		sizeof(EncType) + sizeof(nCount) + nCount * (sizeof(CDataSafe) - sizeof(char)*256) );
	if(dFileInfoLen == -1)
	{
		strMsg = MiniCT_1309;
		return FALSE;
	}

	//д��汾��־Evp20
	File.Write(m_sHeadMark, strlen(m_sHeadMark));
	//д����ܺ��ļ���Ϣ���С,����ΪDWORD
	File.Write(&dFileInfoLen, sizeof(DWORD));
	//д��CRCУ��ֵ,����UINT
	UINT uCrcCheck = 0;
	if(CRC::GetCrc16((char *)&dFileInfoLen, sizeof(DWORD), uCrcCheck))
	{
		File.Write(&uCrcCheck, sizeof(UINT));
	}
	else
	{
		strMsg = MiniCT_1310;	//"������Ϣ������CRCУ��ʧ��";
		AddMsg(strMsg, M_ERROR);
		return FALSE;
	}
	
	//Ԥ����Ϣͷ 
	File.Seek(dFileInfoLen, CFile::current);
	return TRUE;
}

//����:	- �����ļ���Ϻ����
		//д�� ����(���ܷ�ʽ+�ļ�����+�ļ���Ϣ����ܺ󳤶�)������
		//�ƶ��ļ�������λ��
//����:
		//File - ���������ļ���
		//encType - ���ܷ�ʽ
		//nCount - �ļ���Ϣ�����
		//dFileInfoLen - ���ܺ󳤶�,���ڷ����ڴ�
		//strMsg - ���ز���������Ϣ

BOOL CSafeDataSetPage::MadeFileNeck(CList<CDataSafe *, CDataSafe *> * pDataList, CFile &File, 
									const EncType encType, const int nCount, 
									const DWORD dFileInfoLen, CString &strMsg)
{
	//��ʼ���ν�����ʾ , ��Ϻ��
	CSafeDataSetPage::m_pProgOne = NULL;

	//�����ڴ��ļ�,д�� ���ܷ�ʽ + ��Ϣ�����n + n����Ϣ��
	CMemFile MemFile;

	MemFile.Write(&encType, sizeof(EncType));

	MemFile.Write(&nCount, sizeof(int));

	//ö��Ŀ¼�б�
	POSITION pos = pDataList->GetHeadPosition();
	while(pos != NULL)
	{
		CDataSafe* pDataSafe = pDataList->GetNext(pos);
		if(pDataSafe)
			pDataSafe->Write(MemFile);
	}
	//�����ڴ��ļ�
	//������ܽ�� ����+1 �ڴ�
	DWORD dOutLen = -1;
	BYTE * pOut = new BYTE[dFileInfoLen + 1];
	dOutLen = dFileInfoLen;

	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FILE_CERT),"CERT");
	DWORD lenCert = SizeofResource(NULL, hRsrc); 
	HGLOBAL hgCert = LoadResource(NULL,hRsrc);
	LPSTR lpCert = (LPSTR)LockResource(hgCert);

	//�����������
	DWORD dInLen = MemFile.GetLength();
	BYTE * pIn = MemFile.Detach();	//ͬʱ�ر�MemFile.Close(); ��ʱ��Ҫ���¿����ڴ�,����Detach��malloc,�����ͷ���free
	

	if(!CEvp::RSAPubEnc((BYTE *)lpCert, lenCert, "", pIn, dInLen, 
		pOut, dOutLen, strMsg.GetBuffer(256)))
	{
		//����ʧ��
		strMsg.ReleaseBuffer();
		free(pIn);
		delete [] pOut;
		CSafeDataSetPage::m_pProgOne = &m_ProgOne;
		return FALSE;
	}
	else if(dOutLen != dFileInfoLen)
	{
		//����ʧ��
		strMsg.ReleaseBuffer();
		strMsg = MiniCT_1311;//"��Ϣ��������Ԥ�ڲ���";
		free(pIn);
		delete [] pOut;
		CSafeDataSetPage::m_pProgOne = &m_ProgOne;
		return FALSE;
	}

	CSafeDataSetPage::m_pProgOne = &m_ProgOne;
	
	//�ƶ��ļ�ָ�뵽��ͷ + "Evp20" + ���ܺ���Ϣ���СDWORD + CRCУ��UINT
	File.Seek(strlen(m_sHeadMark) + sizeof(DWORD) + sizeof(UINT), CFile::begin);
	File.Write(pOut, dFileInfoLen);
	free(pIn);

	delete [] pOut;
	return TRUE;
}

/////////////////////////////////////////////////////�����ļ�����////////////////////////////////////////////////
//����:�ԳƼ����ļ���Ŀ¼
//����:
//		strCpName - �㷨����
//		strPwd- �Գ���Կ
//		strFileName - ���ܺ��ļ�
//		outMsg - ���ز�����Ϣ
BOOL CSafeDataSetPage::Crypt(CString strCpName, CString strPwd, CString strFileName, CString & outMsg)
{
	//������
	if(strCpName.IsEmpty() || strPwd.IsEmpty() || strFileName.IsEmpty())
	{
		outMsg = MiniCT_1312; //"�㷨���ƻ�Գ���Կ������ļ��Ƿ�";
		return FALSE;
	}

	CFile File;//�ļ�ͷ�Ӱ汾��Ϣ
	if(!File.Open(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outMsg.Format("%s : %s", MiniCT_1313,strFileName);	//�����ļ�ʧ��
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//������Ϣͷ
	if(!MadeFileHead(File, nCount, dFileInfoLen, outMsg))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;

	EVP_CIPHER_CTX Ctx, CtxTemp;
	if(!CEvp::InitCrypt2(strCpName, strPwd, 1, Ctx))
	{
		outMsg = MiniCT_1314; //"��ʼ�����ܽṹʧ��";
		File.Close();
		return FALSE;
	}

	//ö��Ŀ¼�б������Ϣ
	CList<CDataSafe *, CDataSafe *> DataList;	//����Ŀ¼���ļ���ȫ����Ϣ�б�

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
			if(pDataSafe->bIsFile) //�ļ������
			{
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
				AddOnceMsg(strInfo);
				CtxTemp = Ctx;
				//�ṹ�м�����ʼλ�ø�ֵ
				pDataSafe->dBeginSeek = File.GetPosition();
				dOutLen = CEvp::Crypt2(&CtxTemp, pDataSafe->cPathName, File, 0, outMsg);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s, %s%s", pDataSafe->cName, MiniCT_1318, MiniCT_1319, outMsg);//�ԳƼ���ʧ�� ʧ��ԭ��:
					outMsg = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//�ṹ��ֵ
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}
		}
	
		m_ProgAll.SetPos(++iFinish);
	}

	//������Ϣ��
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

//�Գƽ���
BOOL CSafeDataSetPage::DecCrypt(CString strCpName, CString strPwd, CString strEvpFile, CString & outMsg)
{
	//������Ϣ
	DWORD dOutLen = -1;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outMsg.Format("%s%s", MiniCT_1320, strEvpFile);//���ļ�ʧ��
		return FALSE;
	}

	EVP_CIPHER_CTX Ctx, CtxTemp;
	if(!CEvp::InitCrypt2(strCpName, strPwd, 0, Ctx))
	{
		outMsg = MiniCT_1314;//"��ʼ�����ܽṹʧ��";
		File.Close();
		return FALSE;
	}

	CString strPathName;

	//ö��Ŀ¼�б������Ϣ
	CList<CDataSafe *, CDataSafe *> DataList;	//�ļ���Ŀ¼��Ϣ�б�
	CList<CDataSafe *, CDataSafe *> DirList;	//Ŀ¼��Ϣ�б�

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
			if(pDataSafe->bIsFile) //�ļ�
			{
				//�ṹ�м�����ʼλ�ø�ֵ
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
					outMsg.Format("%s%s", strPathName, MiniCT_1321);	//�ļ���СУ��ʧ��
					bRet =  FALSE;
					break;
				}
			}
			else//Ŀ¼ ->����Ŀ¼
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

//����ժҪ
//����:	strMdName - ժҪ�㷨����
//		strOutName - ��������EVP�ļ�����
//		outMsg - ���ز�����Ϣ
BOOL CSafeDataSetPage::Digest(CString strMdName, CString strOutName, CString & outMsg)
{

	//������
	if(strMdName.IsEmpty() || strOutName.IsEmpty())
	{
		outMsg = MiniCT_1322;//"�㷨���ƻ�����ļ��Ƿ�";
		return FALSE;
	}

	CFile File;//�ļ�ͷ�Ӱ汾��Ϣ
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outMsg = MiniCT_1313;//"�����ļ�ʧ��";
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//������Ϣͷ
	if(!MadeFileHead(File, nCount, dFileInfoLen, outMsg))
		return FALSE;

	DWORD dOutLen = -1;

	EVP_MD_CTX mdCtx, mdCtxTemp;

	//��ʼ��CTX
	CEvp::InitDigest2(strMdName, mdCtx);

	//ö��Ŀ¼�б������Ϣ
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
			if(pDataSafe->bIsFile) //�ļ������
			{
				mdCtxTemp = mdCtx;
				//�ṹ�м�����ʼλ�ø�ֵ
				pDataSafe->dBeginSeek = File.GetPosition();
				dOutLen = CEvp::Digest2(&mdCtxTemp, pDataSafe->cPathName, File, outMsg);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s", MiniCT_1323, pDataSafe->cName); //ժҪ�ļ�ʧ��
					outMsg = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//�ṹ��ֵ
				{
					pDataSafe->dNewLen = dOutLen;
				}

			}

			m_ProgAll.SetPos(++iFinish);
			
		}
	}
	
	//������Ϣ��
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

//ժҪ��֤
BOOL CSafeDataSetPage::VerifyDigest(CString strMdName, CString strEvpFile, CString & outStr)
{
	EVP_MD_CTX mdCtx, mdCtxTemp;

	//������Ϣ

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile); //���ļ�ʧ��
		return FALSE;
	}

//	�ļ��汾��� - Evp20
//	��Ϣ������ - DWORD
//	��Ϣ������CRC�� - UINT
	File.Seek(strlen(m_sHeadMark), CFile::begin);

	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));

	File.Seek(sizeof(UINT), CFile::current);

	File.Seek(dFileInfoLen, CFile::current);


	//��ʼ��CTX
	CEvp::InitDigest2(strMdName, mdCtx);
	{
		File.Close();
		return FALSE;
	}

	CString strPathName;

	//ö��Ŀ¼�б������Ϣ
	CList<CDataSafe *, CDataSafe *> DataList;	//�ļ���Ŀ¼��Ϣ�б�
	CList<CDataSafe *, CDataSafe *> DirList;	//Ŀ¼��Ϣ�б�

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
			if(pDataSafe->bIsFile) //�ļ�
			{
				//�ṹ�м�����ʼλ�ø�ֵ
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

//����ǩ��
BOOL CSafeDataSetPage::Sign(CString strMdName, const BYTE * pCertBuf, const UINT nCertLen, CString strPwd,
								  CString strOutName, CString & outStr)
{

	EVP_MD_CTX md_Ctx = {0};
	EVP_PKEY * pkey = NULL;
	X509 * px509 = NULL;
	CEvp::InitSign2(strMdName, md_Ctx, (char *)pCertBuf, nCertLen, strPwd, px509, pkey);

	CFile File;//�ļ�ͷ�Ӱ汾��Ϣ
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outStr.Format("%s%s", MiniCT_1313, strOutName);//�����ļ�ʧ��
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//������Ϣͷ
	if(!MadeFileHead(File, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;
	
	//ö��Ŀ¼�б������Ϣ
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
			if(pDataSafe->bIsFile) //�ļ������
			{
				//�ṹ�м�����ʼλ�ø�ֵ
//				strInfo.Format("�ļ� %s ,��С %d ������...", pDataSafe->cName, pDataSafe->dOldLen);
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
				AddOnceMsg(strInfo);

				pDataSafe->dBeginSeek = File.GetPosition();

				dOutLen = CEvp::Sign2(&md_Ctx, pkey, pDataSafe->cPathName, File, outStr);
				
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s, %s%s", MiniCT_1324, pDataSafe->cName, MiniCT_1319, outStr);	//ǩ��ʧ��%s, ʧ��ԭ��:%s
					outStr = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//�ṹ��ֵ
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}

			m_ProgAll.SetPos(++iFinish);
			
		}
		
	}

	//������Ϣ��
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

//��֤����ǩ��
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
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//"���ļ�ʧ��%s"
		return FALSE;
	}

	CEvp::InitSign2(strMdName, mdCtx, (char *)pCertBuf, nCertLen, NULL, px509, pkey);

//	�ļ��汾��� - Evp20
//	��Ϣ������ - DWORD
//	��Ϣ������CRC�� - UINT
	File.Seek(strlen(m_sHeadMark), CFile::begin);

	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));

	File.Seek(sizeof(UINT), CFile::current);

	File.Seek(dFileInfoLen, CFile::current);
	CString strPathName;

	//ö��Ŀ¼�б������Ϣ
	CList<CDataSafe *, CDataSafe *> DataList;	//�ļ���Ŀ¼��Ϣ�б�
	CList<CDataSafe *, CDataSafe *> DirList;	//Ŀ¼��Ϣ�б�

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
			if(pDataSafe->bIsFile) //�ļ�
			{
				//�ṹ�м�����ʼλ�ø�ֵ
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

//����:��֤�������ŷ�
//����:
//		pCERT -- [in]�Ϸ��û���Կ��
//		strCpName -- �㷨����
//		outMsg -- [OUT]���ش�����Ϣ
BOOL CSafeDataSetPage::Seal(CEvp::stuCertLink * pCERT, CString strCpName,	
							CString strOutName, CString & outMsg)
{
	CFile File;//�ļ�ͷ�Ӱ汾��Ϣ
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outMsg.Format("%s%s", MiniCT_1313, strOutName);//"�����ļ�ʧ��%s"
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//������Ϣͷ
	if(!MadeFileHead(File, nCount, dFileInfoLen, outMsg))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;

	EVP_CIPHER_CTX eCtx, eCtxTemp;

	//��ʼ��CTX
	if(!CEvp::InitSeal2(pCERT, strCpName, File, eCtx, outMsg))
	{
		File.Close();
		return FALSE;
	}

	//ö��Ŀ¼�б������Ϣ
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
			if(pDataSafe->bIsFile) //�ļ������
			{
				//��ʼ��CTX
		//		strInfo.Format("�ļ� %s ,��С %d ������...", pDataSafe->cName, pDataSafe->dOldLen);
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
				AddOnceMsg(strInfo);
				eCtxTemp = eCtx;
				//�ṹ�м�����ʼλ�ø�ֵ
				pDataSafe->dBeginSeek = File.GetPosition();
				dOutLen = CEvp::Seal2(&eCtxTemp, pDataSafe->cPathName, File, outMsg);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s, %s%s", MiniCT_1325, pDataSafe->cName, MiniCT_1319, outMsg);//"��װ�ļ�ʧ��%s, ʧ��ԭ��:%s"
					outMsg = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//�ṹ��ֵ
				{
					pDataSafe->dNewLen = dOutLen;
				}	

			}

			m_ProgAll.SetPos(++iFinish);
			
		}
		
	}

//	m_DataSetList.ReleaseDC(pDC);


	//������Ϣ��
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

//��������ŷ�
BOOL CSafeDataSetPage::OpenSeal(char * pKey, UINT ukeyLen, char * pwd, CString strCpName,
							CString strEvpFile, CString & outStr)
{
	//������Ϣ
	DWORD dOutLen = -1;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//"���ļ�%sʧ��"
		return FALSE;
	}

//	�ļ��汾��� - Evp20
//	��Ϣ������ - DWORD
//	��Ϣ������CRC�� - UINT

	File.Seek(strlen(m_sHeadMark), CFile::begin);

	DWORD dFileInfoLen = 0; 
	File.Read(&dFileInfoLen, sizeof(DWORD));

	File.Seek(sizeof(UINT), CFile::current);

	File.Seek(dFileInfoLen, CFile::current);


	EVP_CIPHER_CTX Ctx, CtxTemp;

	if(!CEvp::InitOpenSeal2(pKey, ukeyLen, pwd, strCpName, File, Ctx, outStr))
	{
		File.Close();
		outStr.Format(MiniCT_1314);//"��ʼ�����ܽṹʧ��"
		return FALSE;
	}

	CString strPathName;

	//ö��Ŀ¼�б������Ϣ
	CList<CDataSafe *, CDataSafe *> DataList;	//�ļ���Ŀ¼��Ϣ�б�
	CList<CDataSafe *, CDataSafe *> DirList;	//Ŀ¼��Ϣ�б�

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
			if(pDataSafe->bIsFile) //�ļ�
			{
				//�ṹ�м�����ʼλ�ø�ֵ
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
					outStr.Format("%s%s", MiniCT_1321, strPathName);//�ļ�%s��СУ��ʧ��
					bRet =  FALSE;
					break;
				}
			}
			else//Ŀ¼ ->����Ŀ¼
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

//��Կ����
BOOL CSafeDataSetPage::RSAPubEnc(const BYTE * pCertBuf, const UINT nCertLen,
								  CString strOutName, CString & outStr)
{

	RSA * pRsa = NULL;
	if(!CEvp::GetPubRsa(pCertBuf, nCertLen,pRsa))
	{
		outStr.Format("%s", MiniCT_1326); //ȡ�ù�ԿRSA�ṹʧ��
		return FALSE;
	}

	CFile File;//�ļ�ͷ�Ӱ汾��Ϣ
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outStr.Format("%s%s", MiniCT_1313, strOutName);//�����ļ�ʧ��%s
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//������Ϣͷ
	if(!MadeFileHead(File, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;

	//ö��Ŀ¼�б������Ϣ
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
			if(pDataSafe->bIsFile) //�ļ������
			{
				//�ṹ�м�����ʼλ�ø�ֵ
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
//				strInfo.Format("�ļ� %s ,��С %d ������...", pDataSafe->cName, pDataSafe->dOldLen);
				AddOnceMsg(strInfo);
				pDataSafe->dBeginSeek = File.GetPosition();
				
				dOutLen = CEvp::RSAPubEnc2(pRsa, pDataSafe->cPathName, File, outStr);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s%s%s", MiniCT_1327, pDataSafe->cName, MiniCT_1319, outStr); //��Կ�����ļ�ʧ��%s,ʧ��ԭ��:%s
					outStr = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//�ṹ��ֵ
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}

			m_ProgAll.SetPos(++iFinish);

			
		}
		
	}

	//������Ϣ��
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

//˽Կ����
BOOL CSafeDataSetPage::RSAPriDec(const BYTE * pCertBuf, const UINT nCertLen, CString strPwd, CString strEvpFile,
								  CString & outStr)
{
	RSA * pRsa = NULL;
	if(!CEvp::GetPrivRsa(pCertBuf, nCertLen, strPwd, pRsa))
	{
		outStr.Format("%s", MiniCT_1326);//"ȡ��˽ԿRSA�ṹʧ��"
		return FALSE;
	}

	DWORD dOutLen = -1;

	BOOL bRet = TRUE;

	CFile File;
	if(!File.Open(strEvpFile, CFile::modeRead | CFile::typeBinary))
	{
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//���ļ�ʧ��%s
		return FALSE;
	}

	CString strPathName;

	//ö��Ŀ¼�б������Ϣ
	CList<CDataSafe *, CDataSafe *> DataList;	//�ļ���Ŀ¼��Ϣ�б�
	CList<CDataSafe *, CDataSafe *> DirList;	//Ŀ¼��Ϣ�б�

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
			if(pDataSafe->bIsFile) //�ļ�
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
			else//Ŀ¼ ->����Ŀ¼
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

//˽Կ����
BOOL CSafeDataSetPage::RSAPrivEnc(const BYTE * pCertBuf, const UINT nCertLen, CString strPwd,
								  CString strOutName, CString & outStr)//˽Կ����
{

	RSA * pRsa = NULL;
	if(!CEvp::GetPrivRsa(pCertBuf, nCertLen, strPwd, pRsa))
	{
		outStr.Format("%s", MiniCT_1326);//"ȡ��˽ԿRSA�ṹʧ��"
		return FALSE;
	}

	CFile File;//�ļ�ͷ�Ӱ汾��Ϣ
	if(!File.Open(strOutName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ))
	{
		outStr.Format("%s%s", MiniCT_1313, strOutName);
		return FALSE;
	}

	int nCount = m_DataSetList.GetCount();

	DWORD dFileInfoLen = 0;

	//������Ϣͷ
	if(!MadeFileHead(File, nCount, dFileInfoLen, outStr))
	{
		File.Close();
		return FALSE;
	}

	DWORD dOutLen = -1;
	
	//ö��Ŀ¼�б������Ϣ
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
			if(pDataSafe->bIsFile) //�ļ������
			{
				//�ṹ�м�����ʼλ�ø�ֵ
				strInfo.Format("%s %s ,%s %d %s", MiniCT_1315, pDataSafe->cName, MiniCT_1316, pDataSafe->dOldLen, MiniCT_1317);
	//			strInfo.Format("�ļ� %s ,��С %d ������...", pDataSafe->cName, pDataSafe->dOldLen);
				AddOnceMsg(strInfo);

				pDataSafe->dBeginSeek = File.GetPosition();
				
				dOutLen = CEvp::RSAPrivEnc2(pRsa, pDataSafe->cPathName, File, outStr);
				if(dOutLen == -1)
				{
					CString str;
					str.Format("%s%s%s%s", MiniCT_1328, pDataSafe->cName, MiniCT_1319, outStr);//˽Կ�����ļ�ʧ��%s, ʧ��ԭ��:%s
					outStr = str;
					File.Close();
					CleanList(&DataList);
					return FALSE;
				}
				else//�ṹ��ֵ
				{
					pDataSafe->dNewLen = dOutLen;
				}
			}

			m_ProgAll.SetPos(++iFinish);
			
		}
		
	}

	//������Ϣ��
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

//��Կ����
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
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//���ļ�ʧ��%s
		return FALSE;
	}

	CString strPathName;

	//ö��Ŀ¼�б������Ϣ
	CList<CDataSafe *, CDataSafe *> DataList;	//�ļ���Ŀ¼��Ϣ�б�
	CList<CDataSafe *, CDataSafe *> DirList;	//Ŀ¼��Ϣ�б�

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
			if(pDataSafe->bIsFile) //�ļ�
			{
				File.Seek(pDataSafe->dBeginSeek, CFile::begin);
				dOutLen = CEvp::RSAPubDec2(pRsa, File, pDataSafe->dNewLen, strPathName, outStr);
				if(-1 == dOutLen)
				{
					CString str;
					str.Format("%s%s%s%s", MiniCT_1329, pDataSafe->cPathName, MiniCT_1319, outStr);//��Կ�����ļ�ʧ��%s, ʧ��ԭ��:%s
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
			else//Ŀ¼ ->����Ŀ¼
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
////////////////////////////////////////////////���ܲ��ֽ���///////////////////////////////////////////////////


void CSafeDataSetPage::OnDblclkListDataset(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	//˫��,�����δ���ܵ�,���LIST��Ŀ¼,�ļ�����ļ�
	//����Ǽ��ܵ�,�ͽ��ܵ���ʱĿ¼,����
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
	
	if(pDataSafe->bIsFile) //ִ��
	{
		CString strOut;
		CString strPathName;
		BOOL bSucceed = TRUE;
		if(m_bIsEnc)	//δ�����ļ�
		{
			strPathName.Format("%s", pDataSafe->cPathName);
		}
		else	//���ܵ���ʱĿ¼
		{
			//ȡ����ʱ·��
			TCHAR szTempPath[MAX_PATH];
			DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
			ASSERT (dwResult);
			
			//������ʱ�ļ�
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
	else//չ��
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

//ֻ�ǽ���ʱ����,����ʱ��ṹ�б�����·����Ϣ
//����ĳ���ṹ��Ϣ,���Ӹ���Ϣ��������
//����,�ṹΪ�ļ��ṹ,�ļ���Ϊminica.rar
//���ṹ�϶�ΪĿ¼�ṹ,Ŀ¼��Ϊminica
//minica�ĸ��ṹҲ����,Ŀ¼��Ϊvc
//vcû�з�Ŀ¼
//���ĸ�����ϢΪ:vc\\minica\\minica.rar
//����:	pDataSafe - ��ǰ�Ľṹ��Ϣ
//		uMinPid - ��С���ṹID
//����ʱ����ܲ��Ӹ���ʼ����,��ʱ����Ҫ���ݸ�lpParentItem
CString CSafeDataSetPage::GetPathName(CDataSafe* pDataSafe, 
									  CList<CDataSafe *, CDataSafe *> * pDataList)
{
	CString strAllPath, strPathName;//ȫ��·��

	if(!pDataSafe && !pDataList)
		return strAllPath;

	if(pDataSafe->uParentID == 0)//��ʾû�и�
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


	strAllPath.Format("%s\\%s", m_strSavePath, strPathName);					//��ѹ��
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
		if(errno==ENOENT)//Ŀ¼û�з���
		{
			strInfo.Format("����Ŀ¼%sʧ��!",strPathName);
			return FALSE;
		}
	}*/

}

void CSafeDataSetPage::OnRadio1() //�ԳƼ��ܰ�ť
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_CHIPNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_PWD)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_CRYPT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PWD)->EnableWindow(TRUE);
}

void CSafeDataSetPage::OnRadio2() //�ŷ���ܰ�ť
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_CHIPNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_PWD)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_CRYPT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PWD)->EnableWindow(FALSE);
}

void CSafeDataSetPage::OnRadio3() //ժҪ
{
	// TODO: Add your control notification handler code here
	
}

void CSafeDataSetPage::OnRadio4() //ǩ��
{
	// TODO: Add your control notification handler code here
	
}

void CSafeDataSetPage::OnRadio5() //��Կ���ܰ�ť
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_STATIC_CHIPNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_PWD)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_CRYPT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PWD)->EnableWindow(FALSE);
}

void CSafeDataSetPage::OnRadio6() //˽Կ���ܰ�ť
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
	
	//����б����Ƿ��м�¼
//	int nCount = ;

	if(m_DataSetList.GetCount() == 0)
	{
		AddMsg(MiniCT_1308, M_ERROR);
		return;
	}

	CEvp::SetProgRess(CSafeDataSetPage::m_pProgOne);

	if(m_bIsEnc)//����
	{
		if(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck())//�ԳƼ���
		{
			m_EncType = CRYPT;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())//�ŷ����
		{
			m_EncType = SEAL;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck())//ժҪ
		{
			m_EncType = DIGEST;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO4))->GetCheck())//ǩ��
		{
			m_EncType = SIGN;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO5))->GetCheck())//��Կ����
		{
			m_EncType = PubEnc;
		}
		else if(((CButton *)GetDlgItem(IDC_RADIO6))->GetCheck())//˽Կ����
		{
			m_EncType = PrivEnc;
		}
		//�������Ի���
		CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT /*| OFN_NOCHANGEDIR */,
			MiniCT_1345, NULL);
		dlgOpen.m_ofn.lStructSize = sizeof(OPENFILENAME);
		CString str = MiniCT_1343;
		dlgOpen.m_ofn.lpstrTitle = str;//������ LPCTSTR
		if(dlgOpen.DoModal()!=IDOK) return;
		m_strEvpPath = dlgOpen.GetPathName();
		CString strExt = m_strEvpPath.Right(4);
		if(strExt.CollateNoCase(".Evp") != 0)
			m_strEvpPath += ".Evp";
		
	}
	else
	{
		//���ֽ��ܵ��Ի���
		if(!SelectPath())
			return;
	}

	pThreadEvpSet = ::AfxBeginThread(_EvpSetThread,this,THREAD_PRIORITY_IDLE);	

}

void CSafeDataSetPage::OnKeydownListDataset(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(!m_bIsEnc) //����,����ɾ��
		return;
	switch(pLVKeyDow->wVKey)
	{
	case VK_DELETE: //ֻ��ɾ����Ŀ¼
		{
			int nItem = m_DataSetList.GetSelectedItem();
			if(nItem!=-1)
			{
				CSuperGridCtrl::CTreeItem * pSelItem = m_DataSetList.GetTreeItem(nItem);
				
				CItemInfo * pInfo = m_DataSetList.GetData(pSelItem ); 
				if(pInfo->GetDataSafe()->uParentID != 0) //0Ϊ���ĸ�
				{
					return;
				}
				
				//ɾ���϶�����
				POSITION pos = m_DirName.GetHeadPosition();
				while ( NULL != pos )
				{
					if(m_DirName.GetAt(pos).Compare(pInfo->GetDataSafe()->cPathName) == 0)//ɾ��
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

//��¼�ṹ��Ϣ��MAP,�ڼ��ܽ���֮ǰ��Ҫö���б�MAP��
void CSafeDataSetPage::EnumList(CList<CDataSafe *, CDataSafe *> * pDataList, CList<CDataSafe *, CDataSafe *> * pDirList)
{
	//ö��Ŀ¼�б������Ϣ
	POSITION pos = m_DataSetList.GetRootHeadPosition();
	while(pos != NULL)
	{
		CSuperGridCtrl::CTreeItem *pParent = m_DataSetList.GetNextRoot(pos); 
		CSuperGridCtrl::CTreeItem *pItem = pParent;
		CItemInfo* lp = m_DataSetList.GetData(pParent);
		CDataSafe * pDataSafe = new CDataSafe();
		lp->CopyDataSafe(pDataSafe);
		pDataList->AddTail(pDataSafe);
		if(pDirList) //����ʱ����Ҫ����Ŀ¼,��ʱ��Ҫ���Ŀ¼�б�
		{
			if(!pDataSafe->bIsFile)//Ŀ¼
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
			if(pDirList) //����ʱ����Ҫ����Ŀ¼,��ʱ��Ҫ���Ŀ¼�б�
			{
				if(!pDataSafe->bIsFile)//Ŀ¼
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
		AddMsg(MiniCT_1330, M_ERROR);//�����������Կ���㷨����
		pThreadEvpSet = NULL;
		return;
	}
	CString outMsg;
	if(!Crypt(strCpName, strPwd, m_strEvpPath, outMsg))
	{
		AddMsg(outMsg, M_ERROR);
	}
	else
		AddMsg(MiniCT_1331);//�ԳƼ��ܳɹ�
	pThreadEvpSet = NULL;
	
}

void CSafeDataSetPage::DoSeal()
{
	//�����Ϸ��û���Կ��
	CEvp::stuCertLink * pCertLink = NULL;
	int nCount = 0;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SEAL, TRUE, m_strKey, 
		m_lenKey,m_p12Pwd,&((CMiniMainDlg *)m_pParent)->m_ListBox))//�õ���Կ
	{
		//����Ϊ��֤��,��ʱ����;,Ȼ�����ÿһ��
		if(m_lenKey == 0) //�ⲿ֤��
		{
			CString strSub,
				    strPath;
			while(AfxExtractSubString(strSub, m_strKey, nCount++, ';'))	//δ���� �ֽ��ַ���
			{
				if(nCount == 1) //��һ���ļ�,ȡ��·��
				{
					int nPos = strSub.ReverseFind('\\');
					strPath = strSub.Left(nPos + 1);//����
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
		AddMsg(MiniCT_1332, M_ERROR); //����������㷨����
		pThreadEvpSet = NULL;
		return;
	}
	CString outMsg;
	if(!Seal(pCertLink, strCpName, m_strEvpPath, outMsg))
	{
		AddMsg(outMsg, M_ERROR);
	}
	else
		AddMsg(MiniCT_1333);//��װ�ŷ�ɹ�
	pCertLink->RemoveAll(pCertLink);
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoRSAPubEnc()
{
	CString outStr;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, TRUE, m_strKey,
		m_lenKey,m_p12Pwd, &((CMiniMainDlg *)m_pParent)->m_ListBox))//�õ���Կ
	{
		if(!RSAPubEnc((UCHAR *)m_strKey, m_lenKey, m_strEvpPath, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1334);//��Կ���ܳɹ�
	}
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoRSAPrivEnc()
{
	CString outStr;
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, FALSE, m_strKey, 
		m_lenKey, m_p12Pwd, &((CMiniMainDlg *)m_pParent)->m_ListBox))//�õ�˽Կ
	{ 
 		if(!RSAPrivEnc((UCHAR *)m_strKey, m_lenKey, m_p12Pwd, m_strEvpPath, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1335);//˽Կ���ܳɹ�
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
		AddMsg(MiniCT_1336, M_ERROR);		 //��ѡ���㷨�������������
		pThreadEvpSet = NULL;
		return;
	}
	if(!DecCrypt(strCpName, strPwd, strEvpFile, outStr))
	{
		AddMsg(outStr, M_ERROR);
	}
	else
		AddMsg(MiniCT_1337); //�Գƽ��ܳɹ�
	pThreadEvpSet = NULL;
	
}

void CSafeDataSetPage::DoOpenSeal()
{
	CString strCpName, outStr;
	GetDlgItemText(IDC_COMBO_CRYPT, strCpName);
	CString strEvpFile(m_strEvpPath);
	if(strCpName.IsEmpty())
	{
		AddMsg(MiniCT_1338, M_ERROR);		 //"��ѡ���㷨"
		pThreadEvpSet = NULL;
		return;
	}
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SEAL, FALSE, m_strKey, m_lenKey, m_p12Pwd,
		&((CMiniMainDlg *)m_pParent)->m_ListBox))//�õ�˽Կ
	{
		if(!OpenSeal(m_strKey, m_lenKey, m_p12Pwd.GetBuffer(0), strCpName, strEvpFile, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1339); //����ŷ�ɹ�
		m_p12Pwd.ReleaseBuffer();
	}
	pThreadEvpSet = NULL;
}

void CSafeDataSetPage::DoRSAPriDec()
{
	CString outStr;
	CString strEvpFile(m_strEvpPath);
	if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::PubEnc, FALSE, m_strKey, m_lenKey, m_p12Pwd,
		&((CMiniMainDlg *)m_pParent)->m_ListBox))//�õ�˽Կ
	{
		if(!RSAPriDec((UCHAR *)m_strKey, m_lenKey, m_p12Pwd, strEvpFile, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1340); //˽Կ���ܳɹ�
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
		m_p12Pwd, &((CMiniMainDlg *)m_pParent)->m_ListBox))//�õ�˽Կ
	{
		if(!Sign("md5", (UCHAR *)m_strKey, m_lenKey, m_p12Pwd,
			m_strEvpPath, outStr))
		{
			AddMsg(outStr, M_ERROR);
		}
		else
			AddMsg(MiniCT_1342); //"����ǩ���ɹ�"
	}
	pThreadEvpSet = NULL;
}

LRESULT CSafeDataSetPage::OnDropOk(WPARAM wParam, LPARAM lParam)
{
	CStringArray * pStringArray = (CStringArray*) wParam;

	for(int i = 0 ;i < pStringArray->GetSize(); i++)
	{
		AddList(pStringArray->GetAt(i));		//�����չ����Ϊ.evp
	}
	return 0;
}

//�����ļ�
//����:
//EVP�ļ���
//����ļ���
//�ļ���EVP�ļ��е�ƫ��
//���ܺ��ļ�����(Ҳ���ǽ���ʱ��Ҫ��ȡ�ĳ���)
//ԭʼ�ļ�����,����У��
//���ش�����Ϣ
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
		outStr.Format("%s%s", MiniCT_1320, strEvpFile);//���ļ�ʧ��%s
		return FALSE;
	}

	switch(m_EncType)
	{
	case NONE:
		break;
	case CRYPT:	//�Գ� 1
		break;
	case DIGEST://ժҪ 3
		break;
	case SIGN:	//ǩ�� 4
		break;
	case SEAL:	//�ŷ� 2
			FileEvp.Seek(strlen(m_sHeadMark), CFile::begin);
			FileEvp.Read(&dFileInfoLen, sizeof(DWORD));
			FileEvp.Seek(sizeof(UINT), CFile::current);
			FileEvp.Seek(dFileInfoLen, CFile::current);
			GetDlgItemText(IDC_COMBO_CRYPT, strCpName);
			if(strCpName.IsEmpty())
			{
				AddMsg(MiniCT_1338,M_ERROR);//	"��ѡ���㷨"	
				break;
			}

			EVP_CIPHER_CTX Ctx;

			if(m_pPageCdb->GetCertPair(CSafeDataCdbPage::SEAL, FALSE, m_strKey, m_lenKey, m_p12Pwd,
									&((CMiniMainDlg *)m_pParent)->m_ListBox))//�õ�˽Կ

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

		//����Ǽ��ܵ�,�ͽ��ܵ���ʱĿ¼,����
		CSuperGridCtrl::CTreeItem * pItem = m_DataSetList.GetTreeItem(nSelItem);
		if(!pItem)
			return;
		CItemInfo * pInfo = m_DataSetList.GetData(pItem); 
	
		const CDataSafe * pDataSafe = pInfo->GetDataSafe();
		if(pDataSafe == NULL)
			return;

		//ȡ����ʱ·��
		TCHAR szTempPath[MAX_PATH];
		DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
		ASSERT (dwResult);
		
		sFile.Format("%s\\%s",szTempPath, pDataSafe->cName);

		CString strOut;

		if(pDataSafe->bIsFile) //�ļ�
		{
			if(!DecOneFile(m_strEvpPath, sFile, pDataSafe->dBeginSeek, pDataSafe->dNewLen,
				pDataSafe->dNewLen, strOut))
				AddMsg(strOut, M_ERROR);
		}
		else//Ŀ¼ ->����Ŀ¼
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
		BROWSEINFO bi;   //���봫��Ĳ���,�����������ṹ�Ĳ����ĳ�ʼ�� 
		CString strDisplayName;// = CMiniCaApp::NormalCode("ѡ���ļ���");;   //�����õ�,��ѡ��Ļ�ҳ��·��,�൱���ṩһ�������� 
		bi.hwndOwner = GetSafeHwnd();   //�õ�������Handleֵ 
		bi.pidlRoot=0;   //���������������������õ���. 
		bi.pszDisplayName = strDisplayName.GetBuffer(MAX_PATH+1);   //�õ�������ָ��, 
		CString strTitle = MiniCT_1344;	//���ܵ�
		bi.lpszTitle = strTitle;  //���ñ��� 
		bi.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_DONTGOBELOWDOMAIN  ;   //���ñ�־ 
		bi.lpfn=NULL; 
		bi.lParam=0; 
		bi.iImage=0;   //���������һЩ�޹صĲ���������,�����������, 
		ITEMIDLIST * pidl;  
		pidl = SHBrowseForFolder(&bi);   //�򿪶Ի��� 
		if(!pidl)
			return FALSE;
		SHGetPathFromIDList(pidl,bi.pszDisplayName);	
		strDisplayName.ReleaseBuffer();   //�������GetBuffer()���Ӧ 
	
		if(SUCCEEDED( SHGetMalloc ( &pMalloc ))) // leak
		{ 
			pMalloc->Free(pidlRoot); 
			pMalloc->Release(); 
		}
		m_strSavePath = strDisplayName;
	}
	return TRUE;
}
