// UpdatePage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "MiniMainDlg.h"
#include "UpdatePage.h"
#include "Evp.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#include "mmsystem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdatePage property page

IMPLEMENT_DYNCREATE(CUpdatePage, CPropertyPage)

CUpdatePage::CUpdatePage() : CPropertyPage(CUpdatePage::IDD)
{
	//{{AFX_DATA_INIT(CUpdatePage)
	//}}AFX_DATA_INIT
	m_sExePath = ((CMiniCaApp *)AfxGetApp())->GetAppPath();//��ִ���ļ�·��
	m_sIniGuid = "{4238FF04-E193-46e4-97A9-177238839CD2}";//iniGUID
	m_sIniUrl = "http://minica.91i.net/minica/miniup.txt";
	m_TaskLen = m_UpLen = m_OneLen = 0;
	m_bUpdate = FALSE;
	m_bReset = TRUE;//Ĭ������
	m_pParent = NULL;
}

CUpdatePage::~CUpdatePage()
{
}

void CUpdatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdatePage)
	DDX_Control(pDX, IDC_BUP, m_Bup);
	DDX_Control(pDX, IDC_PROGRESS2, m_Prog2);
	DDX_Control(pDX, IDC_PROGRESS1, m_Prog1);
	DDX_Control(pDX, IDC_LISTCTRL, m_UpList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdatePage, CPropertyPage)
	//{{AFX_MSG_MAP(CUpdatePage)
	ON_BN_CLICKED(IDC_BUP, OnBup)
	ON_NOTIFY(NM_RCLICK, IDC_LISTCTRL, OnRclickListctrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdatePage message handlers
//{4238FF04-E193-46e4-97A9-177238839CD2}

BOOL CUpdatePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_Bup.SetThemeHelper(((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
	m_Bup.SetIcon(IDI_ICON17);
	m_Bup.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	// TODO: Add extra initialization here

	m_UpList.SetExtendedStyle(LVS_EX_UNDERLINECOLD|LVS_EX_FULLROWSELECT);
	CImageList * pImgList = ((CMiniCaApp *)AfxGetApp())->GetImgList();
	m_UpList.SetImageList(pImgList,LVSIL_SMALL);//�����ı�LISTCTRL�п��

	m_UpList.InsertColumn(0, MiniCT_1700, LVCFMT_LEFT,100); //�����ļ�
	m_UpList.InsertColumn(1, MiniCT_1701, LVCFMT_LEFT,100); //�ļ���С
	m_UpList.InsertColumn(2, MiniCT_1702, LVCFMT_LEFT,100); //״̬
	m_UpList.InsertColumn(3, MiniCT_1703, LVCFMT_LEFT,50); //�ٶ�
	m_UpList.InsertColumn(4, MiniCT_1704, LVCFMT_LEFT,120); //�ļ�����
	m_pParent = GetParent();

	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		m_Prog1.SetBitmap(IDB_BITMAP_PROG);
		m_Prog2.SetBitmap(IDB_BITMAP_PROG);
	}

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		// TODO: Use one of the following forms to add controls:
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
		// m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
UINT CUpdatePage::DownHttpFile(LPCTSTR lpszUrl, LPCTSTR lpszLocal,UINT uIndex,
							  LPCTSTR szFileName,
							  BOOL bShowProg)	
{
	DWORD dwFlags;
	DWORD dwResult = INTERNET_ERROR_OPEN;
	if(!InternetGetConnectedState(&dwFlags, 0))
	{
		AddEvent(M_ERROR, MiniCT_1705); //����δ����
		return INTERNET_ERROR_OPEN;
	}
	if(dwFlags & INTERNET_CONNECTION_OFFLINE)//take appropriate steps
	{
		AddEvent(M_ERROR, MiniCT_1706); //��������
		return INTERNET_ERROR_OPEN;
	}
	CHAR strAgent[64]; //waring ��� strAgent = {0} ,���ܵ�������ʧ��
	sprintf(strAgent, "Agent%ld", timeGetTime());
	HINTERNET hOpen;
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
		hOpen = InternetOpen(strAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	else
		hOpen = InternetOpen(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if(hOpen)
	{
		LVITEM lvItem;
		lvItem.mask =  LVIF_IMAGE | LVIF_STATE; 
		lvItem.state = LVIS_SELECTED | LVIS_FOCUSED; 
        lvItem.iItem = uIndex;
        lvItem.iSubItem = 0;
		lvItem.iImage = 12;

		m_UpList.SetItem(&lvItem);//��ʼ����
		m_UpList.SetItemText(uIndex,2, MiniCT_1707); //������...

		dwResult = InternetGetFile(hOpen, lpszUrl, lpszLocal,uIndex,szFileName,bShowProg);//�Ƿ���ʾ����
		InternetCloseHandle(hOpen);
		return dwResult;
	}
	else 
	{
		GetLastError();
		return 0;
	}
}

UINT CUpdatePage::InternetGetFile (HINTERNET IN hOpen, // Handle from InternetOpen()	//SetItemState(n,  LVIS_SELECTED,  LVIS_SELECTED);  //����ѡ��
                 LPCTSTR  szUrl,        // Full URL
                 LPCTSTR  szFilePath, UINT uIndex,/*�б��еڼ����ļ�*/ LPCTSTR szFileName,BOOL bShowProg)
{
	DWORD dwSize;
	CHAR   szHead[] = "Accept: */*\r\n\r\n";
	VOID* szTemp[16384];
	HINTERNET  hConnect;
	FILE * pFile;
	
	if ( !(hConnect = InternetOpenUrl ( hOpen, szUrl, szHead,
		lstrlenA (szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
	{
		return INTERNET_ERROR_OPENURL;
	}
	
	if  ( !(pFile = fopen (szFilePath, "wb" ) ) )
	{
		return INTERNET_ERROR_FILEOPEN;
	}
	
	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;
	
	if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
		(LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
	{
		dwByteToRead = 0;
	}
	
	if(bShowProg)
	{
		char buf[300] = {0};
		wsprintf(buf,"%s%s...",MiniCT_1708, szFileName); //����
		AddEvent(M_WARING,buf);
	}

	DWORD start;
	DWORD end;
	DWORD time;
	time = 10;
	start = timeGetTime();

	do
	{
		// Keep coping in 16 KB chunks, while file has any data left.
		// Note: bigger buffer will greatly improve performance.
		if (!InternetReadFile (hConnect, szTemp, 16384,  &dwSize) )
		{
			fclose (pFile);
			GetLastError();
            return INTERNET_ERROR_READFILE;
		}
		if (!dwSize)
			break;  // Condition of dwSize=0 indicate EOF. Stop.
		else
			fwrite(szTemp, sizeof (char), dwSize , pFile);

		if(bShowProg)//��ʾ����
		{
			dwBytes += dwSize;
			m_UpLen += dwSize;
			m_Prog1.SetPos(dwBytes*100/m_OneLen);
			m_Prog2.SetPos(m_UpLen*100/m_TaskLen);
			FLOAT fSpeed = 0;
			fSpeed = (float)dwBytes;
			fSpeed /= ((float)time)/1000.0f;
			fSpeed /= 1024.0f;
			end = timeGetTime();
			time = end - start;
			if(time == 0)
				time = 10;
			CString strSpeed;
			strSpeed.Format("%2.2f",fSpeed);

			m_UpList.SetItemText(uIndex,3,strSpeed);

		}
	}   // do
	while (TRUE);
	fflush (pFile);
	fclose (pFile);
	return 0;//�ɹ�
}

void CUpdatePage::CheckDonwloadThread()
{
	m_bUpdate = TRUE;
	BOOL bNeedReset = FALSE;
	m_TaskLen = 0;
	
	m_UpList.DeleteAllItems();
	
	//�õ�ϵͳ��ʱĿ¼
//	AddEvent(M_WARING,"��ȡϵͳ��ʱĿ¼...");

	TCHAR szTempPath[MAX_PATH] = {0}; 
	if (GetTempPath(MAX_PATH, szTempPath) == 0)
	{
		AddEvent(M_ERROR, MiniCT_1709);
		m_bUpdate = FALSE;
		return;
	}

	//�õ���ʱ�ļ���
	char szIniPath[256] = {0};
	UINT nResult = GetTempFileName (szTempPath, _T("~mc"), 0, szIniPath);

	AddEvent(M_WARING, MiniCT_1710); //���ذ汾��Ϣ...

	//���Ƚ���m_sIniUrl,http -> DownHttpFile;ftp - > DownFtpFile

	if(_strnicmp(m_sIniUrl.GetBuffer(0), "http:", 5) == 0)//httpЭ��
	{
		m_sIniUrl.ReleaseBuffer();
		if(DownHttpFile(m_sIniUrl, szIniPath, 0 ,"",FALSE))//0 �ɹ�; 1 ʧ��
		{
			m_bUpdate = FALSE;
			return;
		}
	}
	else if(_strnicmp(m_sIniUrl.GetBuffer(0), "ftp:", 4) == 0)
	{
		m_sIniUrl.ReleaseBuffer();

		if(ConnectFtp("sServerName", 21, "sUserName", "sUserPwd"))
		{
			if(DownFtpFile(m_sIniUrl, szIniPath))//0 �ɹ�; 1 ʧ��
			{
				m_bUpdate = FALSE;
				return;
			}
		}
	}

	
	AddEvent(M_WARING, MiniCT_1711); //�����汾��Ϣ...
	
	//�����汾��Ϣ
	char buf[255]={0};
	
	//[MiniCA]
	//GUID={4238FF04-E193-46e4-97A9-177238839CD2}
	GetPrivateProfileString("MiniCA", "GUID","",buf,50, szIniPath);
	if(strcmp(buf,m_sIniGuid)!=0)//�汾�ļ�����
	{
		AddEvent(M_ERROR, MiniCT_1712);//�汾��Ϣ��Ч!
		m_bUpdate = FALSE;
		return;
	}
	
	GetPrivateProfileString("TOTAL", "ToTal","1",buf,255, szIniPath);
	int nUpdateCount = atoi(buf);//��Ҫ���µ��ļ�����
	if(nUpdateCount == 0)
	{
		AddEvent(M_ERROR, MiniCT_1713); //�汾��Ϣ��ָ�������ļ�Ϊ0!
		m_bUpdate = FALSE;
		return;//�����ļ�����Ϊ0
	}	
	stuUPDATE * pStuUPDATE = new stuUPDATE[nUpdateCount];
	memset(pStuUPDATE,0,sizeof(stuUPDATE)*nUpdateCount);
	
	char strIndex[7];
	for(int i=0,j=0,nCount = 0;i<nUpdateCount;i++)
	{
		//		stuUPDATE * pUPDATE = new stuUPDATE;
		wsprintf(strIndex,"INDEX%d",i+1);
		char sFileName[256] = {0}, //����
			sFileUrl[256] = {0}, //��ַ
			//		sPath[256] = {0},//����·��
			sFileLen[256] = {0},//�ļ�����
			sFileCrc[256] = {0},//�ļ�CRC
			sFilePath[256] = {0},//·��+�ļ���
			sFileDepict[256] = {0},//�ļ�����
			sFileOperate[5] = {0};//�ļ�������ʽ
		
		GetPrivateProfileString(strIndex, "FileName","",sFileName,255, szIniPath);//�ļ�����
		if(strlen(sFileName)==0)//ȱ���ļ�����
			continue;
		
		GetPrivateProfileString(strIndex, "FileUrl","",sFileUrl,255, szIniPath);//����·��
		if(strlen(sFileUrl)==0)//����·��ȱ��
			continue;
		
		GetPrivateProfileString(strIndex, "FilePath","",sFilePath,255, szIniPath);//���·��
		
		GetPrivateProfileString(strIndex, "FileLength","",sFileLen,255, szIniPath);//�ļ���С
		if(atoi(sFileLen)==0)//�ļ�����Ϊ0
			continue;
		
		GetPrivateProfileString(strIndex, "FileDepict","",sFileDepict,255, szIniPath);//�ļ�����
		
		GetPrivateProfileString(strIndex, "FileOperate","",sFileOperate,5, szIniPath);//�ļ�������ʽ
		
		GetPrivateProfileString(strIndex, "FileGuid","",sFileCrc,255, szIniPath);//�ļ�CRC,�Ƚ�
		
		wsprintf(buf,"%s %s ...",MiniCT_1714, sFileName); //���汾
		AddEvent(M_WARING,buf);
		
		char FilePath[255] = {0};//�����ļ�����·��
		char dir[255] = {0};
		wsprintf(FilePath,"%s\\%s\\%s",m_sExePath.GetBuffer(0),sFilePath,sFileName);

		unsigned char md_value[MAX_MD_SIZE] = "";
		unsigned int md_len;
		char outMsg[255] = {0};

		if(CEvp::Digest("md5",FilePath,0,md_value,&md_len, outMsg))//���ش����ļ�
		{
			if(sFileCrc == LmToHText(md_value,md_len))//�ļ���ͬ�����������
			{
				//����Ƿ�ע�������
				FileOperate(FilePath,sFileOperate);//�ļ�����
				continue;
			}

		}

		// Create a masked image list large enough to hold the icons. 
		//���
		//��list�������Ϣ stuUPDATE
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_IMAGE; 
		lvItem.state = 0; 
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        lvItem.pszText = sFileName;
		lvItem.iImage = 11;// 11 - 15
		//����DATAΪCRC,������Ϻ������
	//	lvItem.lParam = uCrc; m_PtrList
		m_UpList.InsertItem(&lvItem);
		m_UpList.SetItemText(j,1,sFileLen);
		m_UpList.SetItemText(j,2, MiniCT_1715);
//		m_UpList.SetItemText(j,3,");
		m_UpList.SetItemText(j,4,sFileDepict);
		
		//����������Ϣ
		strncpy(pStuUPDATE[j].FileGuid, sFileCrc,255);
		pStuUPDATE[j].FileLength = atoi(sFileLen);
		strncpy(pStuUPDATE[j].FileName, sFileName,255);
		strncpy(pStuUPDATE[j].FileOperate,sFileOperate,5);
		strncpy(pStuUPDATE[j].FilePath,sFilePath,255);
		strncpy(pStuUPDATE[j].FileUrl,sFileUrl,255);
		//�����ܳ�������
		j++;
		m_TaskLen += atoi(sFileLen);
		nCount++;
		
	}
	//�����ļ�
	if(nCount == 0)
	{
		AddEvent(M_OK, MiniCT_1716); //�������,û����Ҫ�������ļ�
		m_bUpdate = FALSE;
		return;
	}
	for(i=0;i<nCount;i++)
	{

//		::SetFocus(m_hListCtrl);

		m_UpList.SetItemText(i,2, MiniCT_1717); //������....

		m_UpList.EnsureVisible(i,FALSE);

		char dir[255] = {0};
		wsprintf(dir,"%s\\%s\\",m_sExePath.GetBuffer(0),pStuUPDATE[i].FilePath);//·��
		//����·���Ƿ����
		if(!CMiniCaApp::MakeSureDirectoryPathExists(dir))
		{
			
			char buf[300] = {0};
			wsprintf(buf,"%s%s", MiniCT_1718, dir); //�޷�����Ŀ¼
			m_UpLen+= pStuUPDATE[i].FileLength;
			m_Prog2.SetPos(m_UpLen*100/m_TaskLen);
			//				m_Prog2.Invalidate();
			m_UpList.SetItemText(i, 2, MiniCT_1719);
			AddEvent(M_ERROR,buf);
			continue;
		}
/*		if(_mkdir(dir) == -1)
		{
			if(errno==ENOENT)//Ŀ¼û�з���
			{
				char buf[300] = {0};
				wsprintf(buf,"�޷�����Ŀ¼%s",dir);
				m_UpLen+= pStuUPDATE[i].FileLength;
				m_Prog2.SetPos(m_UpLen*100/m_TaskLen);
//				m_Prog2.Invalidate();
				m_UpList.SetItemText(i,2,"����ʧ��");
				AddEvent(M_ERROR,buf);
				continue;
			}
		}*/
		
		char FilePath[255] = {0};//�����ļ�����·��
		char FilePathTemp[255] = {0};//�����ļ�����·��
		//���������ļ�
		wsprintf(FilePath,"%s%s...", MiniCT_1720, pStuUPDATE[i].FileName); //��������
		AddEvent(M_WARING,FilePath);
		
		wsprintf(FilePath,"%s\\%s\\%s",m_sExePath.GetBuffer(0),pStuUPDATE[i].FilePath,
			pStuUPDATE[i].FileName);//����ļ�
		wsprintf(FilePathTemp,"%s.tmp",FilePath);//������ʱ�ļ�
		
		m_OneLen = pStuUPDATE[i].FileLength;

		if(DownHttpFile(pStuUPDATE[i].FileUrl,FilePathTemp,i,pStuUPDATE[i].FileName) != 0)
		{
			LVITEM lvItem;
			lvItem.mask =  LVIF_IMAGE | LVIF_STATE; 
			lvItem.state = LVIS_SELECTED | LVIS_FOCUSED; 
			lvItem.iItem = i;
		    lvItem.iSubItem = 0;
			lvItem.iImage = 14;
			
			m_UpList.SetItem(&lvItem);
			m_UpList.SetItemText(i,2, MiniCT_1719);

			wsprintf(FilePath,"%s %s",pStuUPDATE[i].FileName, MiniCT_1719);
			AddEvent(M_ERROR,FilePath);
			m_UpLen+= pStuUPDATE[i].FileLength;
			m_Prog2.SetPos(m_UpLen*100/m_TaskLen);
//			m_Prog2.Invalidate();
			continue;
		}
		else
		{
			char temp[255] = {0};
			unsigned char md_value[MAX_MD_SIZE]="";
			unsigned int md_len;
			char outMsg[255] = {0};
			wsprintf(temp,"%s%s...",MiniCT_1721, pStuUPDATE[i].FileName); //У��
			AddEvent(M_WARING,temp);
			if(CEvp::Digest("md5",FilePathTemp,0,md_value,&md_len, outMsg))//���ش����ļ�
			{
				if(pStuUPDATE[i].FileGuid != LmToHText(md_value,md_len))//�ļ���ͬ�����������
				{
					LVITEM lvItem;
					lvItem.mask = LVIF_IMAGE | LVIF_STATE; 
					lvItem.state = LVIS_SELECTED | LVIS_FOCUSED; 
					lvItem.iItem = i;
					lvItem.iSubItem = 0;
					lvItem.iImage = 14;

					m_UpList.SetItem(&lvItem);
					m_UpList.SetItemText(i,2, MiniCT_1722); //У��ʧ��


					wsprintf(temp,"%s%s", pStuUPDATE[i].FileName, MiniCT_1722); //У��ʧ��!
					AddEvent(M_ERROR,temp);
					m_UpLen+= pStuUPDATE[i].FileLength;
					m_Prog2.SetPos(m_UpLen*100/m_TaskLen);

					//ɾ�������ļ�
					DeleteFile(FilePathTemp);
					continue;
				}
			}

			//�޸��ļ���
			if(MoveFileEx(FilePathTemp,FilePath,MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
			{
				LVITEM lvItem;
				lvItem.mask = LVIF_IMAGE | LVIF_STATE; 
				lvItem.state = LVIS_SELECTED | LVIS_FOCUSED; 
				lvItem.iItem = i;
			    lvItem.iSubItem = 0;
				lvItem.iImage = 13;

				m_UpList.SetItem(&lvItem);
				m_UpList.SetItemText(i, 2, MiniCT_1723); //�����ɹ�

				FileOperate(FilePath, pStuUPDATE[i].FileOperate);//�ļ�����
				wsprintf(temp,"%s%s", pStuUPDATE[i].FileName, MiniCT_1723);//�����ɹ�
				AddEvent(M_OK,temp);
			}
			else
			{
				if(MoveFileEx(FilePathTemp,FilePath,MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING))
				{
					LVITEM lvItem;
					lvItem.mask = LVIF_IMAGE | LVIF_STATE; 
					lvItem.state = LVIS_SELECTED | LVIS_FOCUSED; 
					lvItem.iItem = i;
					lvItem.iSubItem = 0;
					lvItem.iImage = 15;

					m_UpList.SetItem(&lvItem);
					m_UpList.SetItemText(i, 2, MiniCT_1724); //��Ҫ����

					
					wsprintf(temp,"%s%s",pStuUPDATE[i].FileName, MiniCT_1725); //�������������
					AddEvent(M_WARING,temp);
					bNeedReset = TRUE;
				}
				else
				{
					GetLastError();
					m_UpLen+= pStuUPDATE[i].FileLength;
					m_Prog2.SetPos(m_UpLen*100/m_TaskLen);
//					m_Prog2.Invalidate();
					continue;
				}
			}
		}
		
	}

	delete [] pStuUPDATE;
	m_bUpdate = FALSE;
	if(m_bReset && bNeedReset)//������������
	{
		//MiniCT_1726  MiniCT_1727 
		if(MessageBox(MiniCT_1726, MiniCT_1727, MB_ICONQUESTION | MB_DEFBUTTON2| MB_YESNO) == IDYES)
		{
			SystemShutdown(EWX_REBOOT | EWX_FORCE, 0);
		}
		else
			AddEvent(M_WARING, MiniCT_1728);//ϵͳ��Ҫ������Ӧ�ø���

	}
	else if(!m_bReset && bNeedReset)
	{
		AddEvent(M_WARING, MiniCT_1728);
	}
}


void CUpdatePage::GetLastError()
{
	LPVOID lpMsgBuf;
	DWORD dErrCode = ::GetLastError();
	if(FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,NULL,	dErrCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,	0,NULL ))
	{
		AddEvent(M_ERROR,(LPTSTR)lpMsgBuf);
	}
	else
	{
		char buf[256] = {0};
		GetErrInfo(dErrCode, buf);
		//wsprintf(buf,"�������%d",dErrCode);
		AddEvent(M_ERROR,buf); //ERROR_INTERNET_TIMEOUT
	}
	LocalFree( lpMsgBuf );
}

void CUpdatePage::AddEvent(int iState ,CString Info)
{
	AddMsg(Info,iState);
}

CWinThread	* ThreadUpdate = NULL;		//ö���߳�

UINT _CheckDonwloadThread(LPVOID lparam)
{
	CUpdatePage * p=(CUpdatePage*)lparam;
	p->CheckDonwloadThread();
	//�任ͼ��
	((CMiniMainDlg *)p->m_pParent)->SetItemIcon(MiniCT_0006, 12);
	return 1;
}

void CUpdatePage::OnBup() 
{
	// TODO: Add your control notification handler code here
	if(m_bUpdate)
		return ;

	m_sExePath += "\\";
	if(!CMiniCaApp::MakeSureDirectoryPathExists(m_sExePath))
	{
		CString str;
		str.Format("����Ŀ¼%sʧ��!",m_sExePath);
		AddEvent(M_ERROR,str.GetBuffer(0));
		return ;
	}

/*	if(_mkdir(m_sExePath) == -1)
	{
		if(errno==ENOENT)//Ŀ¼û�з���
		{
			CString str;
			str.Format("����Ŀ¼%sʧ��!",m_sExePath);
			AddEvent(M_ERROR,str.GetBuffer(0));
			return ;
		}
	}*/
	m_TaskLen = m_UpLen = m_OneLen = 0;

	UINT dwthreadId = 0;
	//�任ͼ��
	((CMiniMainDlg *)GetParent())->SetItemIcon(MiniCT_0006, 16);
	ThreadUpdate = ::AfxBeginThread(_CheckDonwloadThread,this,THREAD_PRIORITY_IDLE);	
}

BOOL CUpdatePage::SystemShutdown(UINT uFlags, DWORD dwReason)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
	
	// Get a token for this process. 
	
	if (!OpenProcessToken(GetCurrentProcess(), 
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 
	
	// Get the LUID for the shutdown privilege. 
	
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
        &tkp.Privileges[0].Luid); 
	
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	
	// Get the shutdown privilege for this process. 
	
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
        (PTOKEN_PRIVILEGES)NULL, 0); 
	
	if (::GetLastError() != ERROR_SUCCESS) 
		return FALSE; 
	
	// Shut down the system and force all applications to close. 
	
	if (!ExitWindowsEx(uFlags, dwReason)) 
		return FALSE; 
	
	return TRUE;
	
}

BOOL CUpdatePage::RegSvr(char *DllPath, BOOL bType)
{
	
	// Initialize OLE.
	if (FAILED(OleInitialize(NULL)))//û�д˾�,������ע��ʧ��
	{
		return FALSE;
	}
	
	HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);

	// Load the library.
	HINSTANCE hLib = LoadLibraryEx(DllPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
	if (hLib < (HINSTANCE)HINSTANCE_ERROR) 
	{
		TCHAR szError[12];
		wsprintf(szError, _T("0x%08lx"), ::GetLastError());
		//DisplayMessage(IDS_LOADLIBFAILED, pszDllName, szError);
		//iReturn = FAIL_LOAD;
		goto CleanupOle;
	}
	
	// Find the entry point.
	if(bType)
	{
		(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib,_T("DllRegisterServer"));
	}
	else
		(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib,_T("DllUnregisterServer"));
	
	if (lpDllEntryPoint == NULL)
	{
		TCHAR szExt[_MAX_EXT];
		_tsplitpath(DllPath, NULL, NULL, NULL, szExt);
		
/*		if ((_stricmp(szExt, ".dll") != 0) && (_stricmp(szExt, ".ocx") != 0))
			DisplayMessage(IDS_NOTDLLOROCX, pszDllName, pszDllEntryPoint);
		else
			DisplayMessage(IDS_NOENTRYPOINT, pszDllName, pszDllEntryPoint);
		
		iReturn = FAIL_ENTRY;*/
		goto CleanupLibrary;
	}
	
	// Call the entry point.
	if (FAILED((*lpDllEntryPoint)()))
	{
//		DisplayMessage(IDS_CALLFAILED, pszDllEntryPoint, pszDllName);
//		iReturn = FAIL_REG;
		goto CleanupLibrary;
	}
	
	
CleanupLibrary:
	FreeLibrary(hLib);
	
CleanupOle:
	OleUninitialize();	
	
	return TRUE;
}

BOOL CUpdatePage::FileOperate(char *FilePath, char *Operate)
{
	if(_strnicmp(Operate,"Reg",4) == 0)//ע��
	{
		RegSvr(FilePath);
	}
	else if(_strnicmp(Operate,"Run",4) == 0)//����
	{
		/*					PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si,0,sizeof(si));
		si.cb = sizeof(si);
		si.dwX = CreateWindow();
		si.dwY = CreateWindow();;
		si.wShowWindow = SW_SHOW;
		si.dwFlags = STARTF_USESHOWWINDOW;
		
		  bool fRet=CreateProcess(FilePath,NULL,NULL,FALSE,NULL,NULL,NULL,NULL,&si,&pi);
	*/	}
	else if(_strnicmp(Operate,"Open",4) == 0)//��
	{
		ShellExecute(this->m_hWnd,"open",FilePath,"","",SW_SHOW );
	}
	return TRUE;
					
}

CString CUpdatePage::LmToHText(UCHAR * pValue, UINT uLen) //����ת16����
{
	char * pBuf = new char[2*uLen+1];
	memset(pBuf,0,2*uLen+1);
	for(unsigned i=0;i<uLen;i++)
	{
		sprintf(pBuf+i*2,"%02X",pValue[i]);//02x��ʾ1��16���Ʊ�Ϊ2���ַ����ղ���
	}
	CString str;
	str.Format("%s",pBuf);
	delete pBuf;
	return str;
}

CString CUpdatePage::GetFileGuid(CString strFile)
{
	unsigned char md_value[MAX_MD_SIZE]="";
	unsigned int md_len;
	char outMsg[255] = {0};
	
	CEvp::Digest("md5",strFile.GetBuffer(0),0,md_value,&md_len, outMsg);//���ش����ļ�
	return LmToHText(md_value,md_len);
}

void CUpdatePage::OnRclickListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
/*	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	"�����ļ�(*.*)|*.*||",NULL);
	if(dlgOpen.DoModal()!=IDOK) return;
	CString strGuid = GetFileGuid(dlgOpen.GetPathName());
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, strGuid.GetLength()+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(strGuid.GetBuffer(0)));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}*/
	*pResult = 0;
}

void CUpdatePage::AddMsg(CString info, DWORD type)
{
	CMiniMainDlg * pMain = (CMiniMainDlg *)m_pParent;
	pMain->AddMsg(MiniCT_0006, info, type);
}

BOOL CUpdatePage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);
	}
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CUpdatePage::GetErrInfo(DWORD dError, char * tmp_msg)
{ 
	switch (dError)
	{
	case 12001:
		tmp_msg = ("Out of handles");
		break;
	case 12002:
		tmp_msg = ("Timeout");
	//	error_code = FETCH_TIMEOUT;
		break;
	case 12004:
		tmp_msg = ("Internal Error");
		break;
	case 12005:
		tmp_msg = ("Invalid URL");
	//	error_code = FETCH_BAD_URL;
		break;
	case 12007:
		tmp_msg = ("Service Name Not Resolved");
	//	error_code = FETCH_CANNOT_RESOLVE;
		break;
	case 12008:
		tmp_msg = ("Protocol Not Found");
	//	error_code = FETCH_BAD_URL;
		break;
	case 12013:
		tmp_msg = ("Incorrect User Name");
	//	error_code = FETCH_BAD_USERNAME;
		break;
	case 12014:
		tmp_msg = ("Incorrect Password");
	//	error_code = FETCH_BAD_PASSWORD;
		break;
	case 12015:
		tmp_msg = ("Login Failure");
	//	error_code = FETCH_LOGIN_FAILED;
		break;
	case 12016:
		tmp_msg = ("Invalid Operation");
		break;
	case 12017:
		tmp_msg = ("Operation Canceled");
		break;
	case 12020:
		tmp_msg = ("Not Proxy Request");
		break;
	case 12023:
		tmp_msg = ("No Direct Access");
		break;
	case 12026:
		tmp_msg = ("Request Pending");
		break;
	case 12027:
		tmp_msg = ("Incorrect Format");
		break;
	case 12028:
		tmp_msg = ("Item not found");
		break;
	case 12029:
		tmp_msg = ("Cannot connect");
	//	error_code = FETCH_CANNOT_CONNECT;
		break;
	case 12030:
		tmp_msg = ("Connection Aborted");
	//	error_code = FETCH_CONNECT_ABORT;
		break;
	case 12031:
		tmp_msg = ("Connection Reset");
	//	error_code = FETCH_CONNECT_RESET;
		break;
	case 12033:
		tmp_msg = ("Invalid Proxy Request");
		break;
	case 12034:
		tmp_msg = ("Need UI");
		break;
	case 12035:
		tmp_msg = ("Sec Cert Date Invalid");
		break;
	case 12038:
		tmp_msg = ("Sec Cert CN Invalid");
		break;
	case 12044:
		tmp_msg = ("Client Auth Cert Needed");
		break;
	case 12045:
		tmp_msg = ("Invalid CA Cert");
		break;
	case 12046:
		tmp_msg = ("Client Auth Not Setup");
		break;
	case 12150:
		tmp_msg = ("HTTP Header Not Found");
	//	error_code = FETCH_BAD_HTTP;
		break;
	case 12152:
		tmp_msg = ("Invalid HTTP Server Response");
	//	error_code = FETCH_BAD_HTTP;
		break;
	case 12153:
		tmp_msg = ("Invalid HTTP Header");
	//	error_code = FETCH_BAD_HTTP;
		break;
	case 120154:
		tmp_msg = ("Invalid Query Request");
	//	error_code = FETCH_BAD_HTTP;
		break;
	case 120156:
		tmp_msg = ("Redirect Failed");
	//	error_code = FETCH_BAD_HTTP;
		break;
	case 120159:
		tmp_msg = ("TCP/IP not installed");
		break;
	default:
		tmp_msg = ("Error");
	//	error_code = FETCH_FAILURE;
	}

}

void CALLBACK FtpCallbackStatus(HINTERNET hInternet,
									 DWORD dwContext,
									 DWORD dwInternetStatus,
									 LPVOID lpvStatusInformation,
									 DWORD dwStatusInformationLength)
{
	// ��ȡ��������Ϣ
	
	CUpdatePage::REQUEST_CONTEXT * pcFtpMsg = 
		reinterpret_cast<CUpdatePage::REQUEST_CONTEXT*>(dwContext);
	if(pcFtpMsg == NULL)
	{
        return;
	}

	LPDWORD pdwInformation(NULL);

	CUpdatePage * pUpdate = (CUpdatePage *)pcFtpMsg->pUpdate;
	char cout[255] = {0};
	// ��ѯ��ǰ�ļ�����״̬
	switch(dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:      // 10
		sprintf(cout,"Looking up the IP address for %s", (LPCTSTR)lpvStatusInformation);
//		sout.append(cout);
//		pUpdate->AddEvent(0,sout);
        break;
		
	case INTERNET_STATUS_NAME_RESOLVED:        // 11
 		sprintf(cout,"Name resolved %s", (LPCTSTR)lpvStatusInformation);
//		sout.append(cout);
//		pUpdate->AddEvent(0,sout);
        break;
		
	case INTERNET_STATUS_CONNECTING_TO_SERVER:       // 20
        // lpvStatusInformation��һ��SOCKADDR��ָ��
 		sprintf(cout,"Connecting to server--%s...", (LPCTSTR)lpvStatusInformation);
//		sout.append(cout);
//		pUpdate->AddEvent(0,sout);
        break;
		
	case INTERNET_STATUS_CONNECTED_TO_SERVER:  // 21
        // lpvStatusInformation��һ��SOCKADDR��ָ��
 		sprintf(cout,"Connected to server--%s", (LPCTSTR)lpvStatusInformation);
//		sout.append(cout);
//		pUpdate->AddEvent(0,sout);
        break;
		
	case INTERNET_STATUS_SENDING_REQUEST:           // 30
        break;
		
	case INTERNET_STATUS_REQUEST_SENT:                  // 31
        {
			// �������ݵ�������
			if(pcFtpMsg->bBegin)
			{
				// ֻ�е������ϴ�״̬��ʱ�򣬲Ŵ���
				if(pcFtpMsg->m_dwFtpHead > 0 && pcFtpMsg->m_dwFtpHead < 6)
				{
					// 5���ֵ��һ������ֵ������Ϊʲô������û�������о�rfc��Э��
					
					// ���Ի���֪�����������֪��ԭ�򣬿��Բ�����
					pcFtpMsg->m_dwFtpHead ++;
				}
				pdwInformation = (LPDWORD)lpvStatusInformation;
				if(pcFtpMsg->m_dwFtpHead == 6)
				{
					pcFtpMsg->dFinishLen += *pdwInformation;
					pcFtpMsg->dTotalFinish += *pdwInformation;
//					ATLTRACE("���η��͵����ݴ�СΪ: %d���ܹ�����Ϊ��%d\n", *pdwInformation,
//						pcFtpMsg->dFileLen);
					
					if(pcFtpMsg->hWindow != NULL)
					{
						// ���ȵĵ�һѡ���Ǹ����ڷ�����Ϣ
//						SendDlgItemMessage(pcFtpMsg->hWindow,IDC_PROGRESS_ONE,PBM_SETPOS,
//							(pcFtpMsg->dFinishLen*100.0/pcFtpMsg->dFileLen),0);
//						SendDlgItemMessage(pcFtpMsg->hWindow,IDC_PROGRESS_TOTAL,PBM_SETPOS,
//							(pcFtpMsg->dTotalFinish*100.0/pcFtpMsg->dTotalLen),0);
					}
				}// End if 5������ֵ
			}// end if �ļ��ϴ�
        }
		
        break;
		
	case INTERNET_STATUS_RESPONSE_RECEIVED:        // 41
		
        {
			// �ӷ��������յ�����Ӧ
			if(pcFtpMsg->bBegin)
			{
				// ֻ�е���������״̬��ʱ�򣬲Ŵ���
				pdwInformation = (LPDWORD)lpvStatusInformation;
				if(pcFtpMsg->m_dwFtpHead > 0 && pcFtpMsg->m_dwFtpHead < 6)
				{
					// 6���ֵ��һ������ֵ������Ϊʲô������û�������о�rfc��Э��
					
					// ���Ի���֪�����������֪��ԭ�򣬿��Բ�����
					
					pcFtpMsg->m_dwFtpHead ++;
					
					//  ATLTRACE("������֮���%d�ν������ݣ�\n", pcFtpMsg->m_dwFtpHead);
					
				}
				
				if(pcFtpMsg->m_dwFtpHead == 6)
					
				{
					
					pcFtpMsg->dFinishLen += *pdwInformation;
					pcFtpMsg->dTotalFinish += *pdwInformation;
//					ATLTRACE("���η��͵����ݴ�СΪ: %d���ܹ�����Ϊ��%d\n", *pdwInformation,
//						pcFtpMsg->dFileLen);
					
					if(pcFtpMsg->hWindow != NULL)
					{
						// ���ȵĵ�һѡ���Ǹ����ڷ�����Ϣ
//						SendDlgItemMessage(pcFtpMsg->hWindow,IDC_PROGRESS_ONE,PBM_SETPOS,
//							(pcFtpMsg->dFinishLen*100.0/pcFtpMsg->dFileLen),0);
//						SendDlgItemMessage(pcFtpMsg->hWindow,IDC_PROGRESS_TOTAL,PBM_SETPOS,
//							(pcFtpMsg->dTotalFinish*100.0/pcFtpMsg->dTotalLen),0);
					}
				}// End if 6������ֵ
			}// End if �ļ�����
        }
        break;
		
	case INTERNET_STATUS_RECEIVING_RESPONSE:             // 40
		
        // �ȴ�����������Ӧ��lpvStatusInformation = NULL
        break;
		
	case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:              // 42
	case INTERNET_STATUS_PREFETCH:                                 // 43
	case INTERNET_STATUS_CLOSING_CONNECTION:           // 50
	 	sprintf(cout,"Closing connection");
//		sout.append(cout);
//		pUpdate->AddEvent(0,sout);
        break;
		
	case INTERNET_STATUS_CONNECTION_CLOSED:                    // 51
	 	sprintf(cout,"Connection closed");
//		sout.append(cout);
//		pUpdate->AddEvent(0,sout);
        break;
		
	case INTERNET_STATUS_HANDLE_CREATED:                    // 60
        {
			LPINTERNET_ASYNC_RESULT ptIAR =
				reinterpret_cast<LPINTERNET_ASYNC_RESULT>(lpvStatusInformation);
//			if(hInternet == pcFtpMsg->GetCurrentConnect())
			{
				
//				ATLTRACE("Connect Handle Created!\n");
				
				pcFtpMsg->m_dwFtpHead = 1;
				
			}
			
//			else if(hInternet == pcFtpMsg->GetCurrentSession())
				
			{
				
//				ATLTRACE("Session handle created!\n");
				
			}
			
        }
		
        break;
		
	case INTERNET_STATUS_HANDLE_CLOSING:                    // 70
		
        pcFtpMsg->m_dwFtpHead = 0;
		
   //     ATLTRACE("Handle closed with dif %d!\n",
			
//			pcFtpMsg->GetCurrentFtpSize() - pcFtpMsg->GetTotalFileSize());
		
        break;
		
	case INTERNET_STATUS_DETECTING_PROXY:                  // 80
		
//       ATLTRACE("Detecting proxy\n");
		
        break;
		
	case INTERNET_STATUS_REQUEST_COMPLETE:               // 100
		
        // һ���첽�����Ѿ���ɣ�lpvStatusInformation��INTERNET_ASYNC_RESULT��ָ��
		
        {
			
			LPINTERNET_ASYNC_RESULT ptIAR =
				
				reinterpret_cast<LPINTERNET_ASYNC_RESULT>(lpvStatusInformation);
			
//			ATLTRACE("һ���첽������ɣ�����Ϊ��%d�����Ϊ��%d\n",
				
//				ptIAR->dwError, ptIAR->dwResult);
			
        }
		
        break;
		
	case INTERNET_STATUS_REDIRECT:                                  // 110
		
 //       ATLTRACE("HTTP request redirected to \n");
		
        break;
		
	case INTERNET_STATUS_INTERMEDIATE_RESPONSE:             // 120
		
   //     ATLTRACE("Received intermediate status message from the server.\n");
		
        break;
		
	case INTERNET_STATUS_STATE_CHANGE:                         // 200
		
//        ATLTRACE("'Moved between a secure and a nonsecure site\n");
		
        {
			
			LPDWORD pdwRead = (LPDWORD)lpvStatusInformation;
			
			switch(*pdwRead)
				
			{
				
			case INTERNET_STATE_CONNECTED:                  // 0x00000001
				
//				ATLTRACE("Connected state moved between secure and nonsecure site\n");
				
				break;
				
			case INTERNET_STATE_DISCONNECTED:            // 0x00000002
				
//				ATLTRACE("Disconnected from network\n");
				
				break;
				
			case INTERNET_STATE_DISCONNECTED_BY_USER:   // 0x00000010
				
//				ATLTRACE("Disconnected by user request\n");
				
				break;
				
			case INTERNET_STATE_IDLE:                        // 0x00000100
				
//				ATLTRACE("No network requests are being made (by Wininet).\n");
				
				break;
				
			case INTERNET_STATE_BUSY:                       // 0x00000200
				
//				ATLTRACE("Network requests are being made (by Wininet).\n");
				
				break;
				
			case INTERNET_STATUS_USER_INPUT_REQUIRED:    // 0x00000040
				
//				ATLTRACE("The request requires user input to complete\n");
				
				break;
				
			default:
				
				break;
				
			}
			
        }
		
        break;
		
	default:
		
        break;
		
	}// End Ftp_CallbackStatus
}


//���ӷ�����
BOOL CUpdatePage::ConnectFtp(CString sServerName,UINT uServerPort,
							CString sUserName,CString sUserPwd)
{
	char buf[256] = {0};
	try
	{
		m_hSession = InternetOpen( _T("Ftp_Ctrl"),
			INTERNET_OPEN_TYPE_DIRECT,
			"",
			"",
			INTERNET_FLAG_NO_CACHE_WRITE);
		
		if(!m_hSession)
		{
//			GetLastError(buf);
			return FALSE;
		}
		
/*		if(InternetSetStatusCallback(m_hSession, FtpCallbackStatus) ==
			INTERNET_INVALID_STATUS_CALLBACK)
		{
			InternetCloseHandle(m_hSession);
			GetLastError(buf);
			AddEvent(-1,buf);
			return FALSE;
		}*/
		
		
		m_hConnect = InternetConnect(m_hSession,
			sServerName,
			uServerPort,
			sUserName,
			sUserPwd,
			INTERNET_SERVICE_FTP,
			INTERNET_FLAG_PASSIVE |
			INTERNET_FLAG_EXISTING_CONNECT,
			(DWORD)&m_Request);
		if(!m_hConnect)
		{
//			GetLastError(buf);
//			AddEvent(-1,buf);
			return FALSE;
		}
	}
	catch(...)
	{
//		GetLastError(buf);
//		AddEvent(-1,buf);
		return FALSE;
	}
	return TRUE;
}

BOOL CUpdatePage::DownFtpFile(LPCTSTR lpszNewRemoteFile,LPCTSTR lpszLocalFile)
{
	BOOL bResult = FtpGetFile(m_hConnect,lpszNewRemoteFile,lpszLocalFile,FALSE,
		FILE_ATTRIBUTE_NORMAL,FTP_TRANSFER_TYPE_BINARY,(DWORD)&m_Request);//�ϴ�
	if(!bResult)
	{
		char buf[256] = {0};
//		GetLastError(buf);
	}
	return bResult;
}

void CUpdatePage::DisFtpConnect()
{
	// TODO: Add your implementation code here
	if(m_hSession)
	{
		InternetCloseHandle(m_hSession);
		m_hSession = NULL;
	}
	if(m_hConnect)
	{
		InternetCloseHandle(m_hConnect);
		m_hConnect = NULL;
	}
}

void CUpdatePage::TranslateCT()
{
	CHeaderCtrl * pHeader = m_UpList.GetHeaderCtrl();
	if (pHeader)
	{
		CString str = MiniCT_1700;
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(0, &hdi);
		
		str = MiniCT_1701;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(1, &hdi);

		str = MiniCT_1702;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(2, &hdi);

		str = MiniCT_1703;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(3, &hdi);

		str = MiniCT_1704;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(4, &hdi);
	}
}