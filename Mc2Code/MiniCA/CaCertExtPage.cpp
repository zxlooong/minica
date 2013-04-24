// CaCertExtPage.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "CaCertExtPage.h"
#include "MiniMainDlg.h"
#include ".\GenericClass\Language.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaCertExtPage property page

IMPLEMENT_DYNCREATE(CCaCertExtPage, CPropertyPage)

CCaCertExtPage::CCaCertExtPage() : CPropertyPage(CCaCertExtPage::IDD)
{
	//{{AFX_DATA_INIT(CCaCertExtPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCaCertExtPage::~CCaCertExtPage()
{
}

void CCaCertExtPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaCertExtPage)
	DDX_Control(pDX, IDC_LIST, m_CheckList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaCertExtPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaCertExtPage)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST, OnItemchangingList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaCertExtPage message handlers

BOOL CCaCertExtPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	ClassXP(GetDlgItem(IDC_LIST)->m_hWnd,TRUE);


	CImageList * pImgList = ((CMiniCaApp *)AfxGetApp())->GetImgList();
	m_CheckList.SetImageList(pImgList,LVSIL_SMALL);//�����ı�LISTCTRL�п��

	ListView_SetExtendedListViewStyle(m_CheckList.m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_SUBITEMIMAGES |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect rect;
	m_CheckList.GetClientRect(rect);
	int width = rect.Width();

	m_CheckList.InsertColumn(0, MiniCT_0400, LVCFMT_LEFT,width/3); //MiniCT_0400 "��չ����"
	m_CheckList.InsertColumn(1, MiniCT_0401, LVCFMT_LEFT,2*width/3);		//MiniCT_0401 "����"


	CString m_IniPathName = ((CMiniCaApp *)AfxGetApp())->GetAppPath() +  "\\MiniExt.ini";

	//����������Ϣ
	//����Ƿ��Ѿ���,����򲻿�,������д��
	CFile file;
	if(!file.Open(m_IniPathName,CFile::modeRead))
	{
		/*�õ�����,�ж��Ƿ��廷��*/
		HRSRC hRsrc = 0;

		if(CMiniCaApp::IsBig())
			hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_MINICA_EXT_BG),"INI");
		else
			hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_MINICA_EXT),"INI");

		DWORD lenCert = SizeofResource(NULL, hRsrc); 
		HGLOBAL hgCert=LoadResource(NULL,hRsrc);
		LPSTR lpCert=(LPSTR)LockResource(hgCert);
		
		if(file.Open(m_IniPathName,CFile::modeCreate|CFile::modeWrite))	//���ļ�
		{
			file.Write(lpCert,lenCert);
		}
		
	}
	file.Close();

	GetIniInfo(m_IniPathName);

	/*LVS_EX_CHECKBOXES ʹ�ü��� 
	LVS_EX_FULLROWSELECT ѡ������ 
	LVS_EX_GRIDLINES ��REPORT�л����ָ��� 
	LVS_EX_HEADERDRAGDROP LVS_REPORTʱ��������drag-and-drop�������� 
	LVS_EX_SUBITEMIMAGES ��������������ʾimage 
	LVS_EX_TRACKSELECT �����ָ��ĳһ��ʱ���Զ�ѡ����� */

	m_toolTip.Create(this);
	m_toolTip.AddTool(GetDlgItem(IDC_LIST), "֤����չ����\r��ϸ������ת������ѡ�");


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCaCertExtPage::OnItemchangingList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
		return;	// No change

	BOOL bPrevState = (BOOL)(((pNMListView->uOldState & 
				LVIS_STATEIMAGEMASK)>>12)-1);   // Old check box state
	if (bPrevState < 0)	// On startup there's no previous state 
		bPrevState = 0; // so assign as false (unchecked)

	// New check box state
	BOOL bChecked=(BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK)>>12)-1);   
	if (bChecked < 0) // On non-checkbox notifications assume false
		bChecked = 0; 

	if (bPrevState == bChecked) // No change in check box
		return;
	
	// Now bChecked holds the new check box state

	// ....
}

void CCaCertExtPage::SetLVCheck (WPARAM ItemIndex, BOOL bCheck)
{
	ListView_SetItemState(m_CheckList.m_hWnd, ItemIndex, 
		UINT((int(bCheck) + 1) << 12), LVIS_STATEIMAGEMASK);
}

//�õ�����״̬

//ʹ����commctl.h�ж���ĺ� ListView_GetCheckState(hwndLV,i)��hwndLVΪCONTROL�ľ����iΪλ�������� 

void CCaCertExtPage::GetCertInfo(CStaticTreeCtrl * p_WizardTree,
										 HTREENODE & hInfoExt) 
{
	int nCount = m_CheckList.GetItemCount();
	CString info,strTemp,str,str1;
	for(int i = nCount - 1 ; i >= 0 ; i--)
	{
		//���״̬
		if(ListView_GetCheckState(m_CheckList.m_hWnd,i))//����ѡ��
		{
			str1 = m_CheckList.GetItemText(i,0);
			strTemp = m_CheckList.GetItemText(i,1);
			str.Format("%s(%d): %s",str1,m_CheckList.GetItemData(i),strTemp);
			str.Replace(",",", ");//���ź���ӿո������б����
			HTREENODE hTemp = p_WizardTree->InsertChild( hInfoExt, _T(str));
			if(strTemp.IsEmpty())
			{
				p_WizardTree->SetNodeColor(hTemp,RGB(128,0,0));
			}
		}
	}

}


/*void CCaCertExtPage::OnKillfocusRichedit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	AfxMessageBox("kill");
	*pResult = 0;
}

void CCaCertExtPage::OnReturnRichedit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	AfxMessageBox("enter");
	*pResult = 0;
}
*/
void CCaCertExtPage::GetCert(stuCERTEXT *& pCertExt)
{
	int nCount = m_CheckList.GetItemCount();
	CString strName,strInside;
	DWORD dData = 0;
	for(int i = 0;i<nCount;i++)
	{
		//���״̬
		if(ListView_GetCheckState(m_CheckList.m_hWnd,i))//����ѡ��
		{
			strName = m_CheckList.GetItemText(i,1);
			pCertExt->Add(pCertExt, m_ArrayInside[i], strName);
		}
	}
}

void CCaCertExtPage::GetIniInfo(CString strIniPath)
{
	//ɾ��ԭ������
	m_CheckList.DeleteAllItems();

	//�����������
	m_ArrayInside.RemoveAll();

	char buf[255] = {0};
	char info[255] = {0};
	char inside[255] = {0};

	GetPrivateProfileString("TOTAL", "ToTal","0",buf,50, strIniPath);
	CString strIndex;
	int uMax = atoi(buf);
	for(int i = 0, j = 0; i<uMax; i++)
	{
		strIndex.Format("INFO%d",i+1);
		//���ȶ�ȡ�Ƿ���ʾ��־
		GetPrivateProfileString(strIndex, "IsDisp","0",buf,50, strIniPath);
		if(atoi(buf) <= 0)
			continue;

		GetPrivateProfileString(strIndex, "InsideName","0",inside,254, strIniPath);//�ڲ�����
		if(strlen(inside) == 0)
			continue;
		else
			m_ArrayInside.Add(inside);

		GetPrivateProfileString(strIndex, "Info","0",info,254, strIniPath);
		if(strlen(info) == 0)
			continue;

		GetPrivateProfileString(strIndex, "DispName","0",buf,254, strIniPath);
		if(strlen(buf) == 0)
			continue;

		m_CheckList.InsertItem(j, buf, 7);
		m_CheckList.SetItemData(j, i);
		m_CheckList.SetItemText(j, 1, info);

		GetPrivateProfileString(strIndex, "IsCheck","78",buf,50, strIniPath);//�Ƿ�ѡ��
		SetLVCheck (j, atoi(buf));
		j++;
	}
}

void CCaCertExtPage::TranslateCT()
{
	CHeaderCtrl * pHeader = m_CheckList.GetHeaderCtrl();
	if (pHeader)
	{
		CString str = MiniCT_0400;
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(0, &hdi);

		str = MiniCT_0401;
		hdi.pszText = (LPTSTR)((LPCTSTR)str);
		pHeader->SetItem(1, &hdi);
	}
	
}

BOOL CCaCertExtPage::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTip.RelayEvent(pMsg);
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}
