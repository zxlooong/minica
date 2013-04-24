// ColorListBox.cpp : implementation file
//

#include "stdafx.h"
#include "..\..\MiniCA.h"
#include "ColorListBox.h"
#include "..\..\MiniMainDlg.h"
#include "..\..\GenericClass\Language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorListBox

CColorListBox::CColorListBox()
{
	m_BkColor = RGB(255,255,255);
	LOGFONT logFont;
	ZeroMemory((void*)&logFont,sizeof(logFont));
	strcpy(logFont.lfFaceName,"����");
	logFont.lfHeight = -12;
	logFont.lfWeight = 400;
	logFont.lfCharSet = GB2312_CHARSET;
	logFont.lfOutPrecision = 3;
	logFont.lfClipPrecision = 2; 
	logFont.lfQuality = 1;
	logFont.lfPitchAndFamily = 2;
	m_font.CreateFontIndirect(&logFont);
//	m_pButYes = NULL;
//	m_pButNo = NULL;

	m_FileName = "";
	m_bFirst = FALSE;

/*	LOGBRUSH  logBrush;  
	logBrush.lbStyle  =   BS_SOLID;  
	logBrush.lbColor  =   RGB(240,240,240);
	logBrush.lbHatch  =   HS_CROSS;  
	m_brBmp.CreateBrushIndirect(&logBrush);	*/
}

CColorListBox::~CColorListBox()
{
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();

	if( m_brBmp.m_hObject != NULL )
		m_brBmp.DeleteObject();

	if( m_font.m_hObject != NULL )
		m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CColorListBox, CListBox)
	//{{AFX_MSG_MAP(CColorListBox)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
//	ON_BN_CLICKED(IDC_B_NO, OnBNo)
//	ON_BN_CLICKED(IDC_B_YES, OnBYes)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
//	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorListBox message handlers
void CColorListBox::SetBkColor(COLORREF bkColor)//���ñ�����ɫ,����ȡ��ѡ���ʱ�ָ�ԭ����
{
	m_BkColor = bkColor;
}

void CColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	// TODO: Add your code to draw the specified item
	if ((int)lpDIS->itemID < 0)
		return; 

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	COLORREF crText;
	CString sText;
	COLORREF crNorm;	//������ɫ
	HICON icon;
	switch(lpDIS->itemData)
	{
		case M_OK:
			crNorm = RGB(0,128,0);
			icon = AfxGetApp()->LoadIcon(IDI_ICON_GREEN);
			break;
		case M_ERROR:
			crNorm = RGB(255,0,0);
			icon = AfxGetApp()->LoadIcon(IDI_ICON_RED);
			break;
		case M_WARING:
			crNorm = RGB(0,0,255);
			icon = AfxGetApp()->LoadIcon(IDI_ICON_BLUE);
			break;
	}


	// �ַ�����ɫ
	COLORREF crHilite = RGB(255,128,0);

//	COLORREF crHilite = RGB(255-GetRValue(crNorm), 255-GetGValue(crNorm), 255-GetBValue(crNorm));
	CRect rectBK(lpDIS->rcItem.left+20,lpDIS->rcItem.top,lpDIS->rcItem.right+18,lpDIS->rcItem.bottom);


	// �����ѡ�У�ʹ�ø���ɫ�����
/*	if ((lpDIS->itemState & ODS_SELECTED) &&
		 (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		pDC->DrawFocusRect(&lpDIS->rcItem); 
		CBrush brush(crNorm);
		pDC->FillRect(rectBK, &brush);
	}

	// ���ȡ��ѡ�У��ָ�
	if (!(lpDIS->itemState & ODS_SELECTED) &&	(lpDIS->itemAction & ODA_SELECT))
	{
		//ѡ��ԭ��������ɫ
		CBrush brush(m_BkColor);
		pDC->FillRect(rectBK, &brush);
	}	 	

	// ���ƽ������ 
	if ((lpDIS->itemAction & ODA_FOCUS) && (lpDIS->itemState & ODS_FOCUS))
		pDC->DrawFocusRect(&lpDIS->rcItem); 

	// ȡ���������
	if ((lpDIS->itemAction & ODA_FOCUS) &&	!(lpDIS->itemState & ODS_FOCUS))
		pDC->DrawFocusRect(&lpDIS->rcItem); 

*/
	// ͸������
	int nBkMode = pDC->SetBkMode(TRANSPARENT);

	if (lpDIS->itemData)		
	{
		if (lpDIS->itemState & ODS_SELECTED)
		{
			crText = pDC->SetTextColor(crHilite);
		}
		else if (lpDIS->itemState & ODS_DISABLED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		else
			crText = pDC->SetTextColor(crNorm);
	}
	
	else
	{
		if (lpDIS->itemState & ODS_SELECTED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		else if (lpDIS->itemState & ODS_DISABLED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		else
			crText = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	}


	// ��ʾ�ַ���
	GetText(lpDIS->itemID, sText);
	CRect rect = lpDIS->rcItem;

	// �ַ���ʽ
	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if (GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;


	pDC->DrawText(sText, -1, &rectBK, nFormat | DT_CALCRECT&DT_CENTER);
	DrawIconEx(lpDIS->hDC,rect.left+2,rect.top+1,icon,16,16,0,NULL, DI_NORMAL | DI_COMPAT);
	pDC->SetTextColor(crText); 
	pDC->SetBkMode(nBkMode);		
	// restore the device context and free the icon.

}

void CColorListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	//����ÿһ��ĸ߶�
	lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU);
}

int CColorListBox::AddMsg(CString lpszItem,	DWORD info)
{
	if(lpszItem.IsEmpty())
		return 0;
	int nItem = AddString(CMiniCaApp::NormalCode(lpszItem));
	if (nItem >= 0)
		SetItemData(nItem, info);
	int numList=GetCount()-1;
	SetTopIndex(numList);
	SetCurSel(numList);
	//ˮƽ����
	int max_width=0;
	CSize sz;
	CClientDC dc(this);
	for(int i=0;i<GetCount();i++)
	{
		GetText(i,lpszItem);
		sz=dc.GetTextExtent(lpszItem);
		if(max_width<sz.cx)
			max_width=sz.cx;
	}
	SendMessage(LB_SETHORIZONTALEXTENT,max_width,0);

	return nItem;
}

void CColorListBox::DeleteLast()
{
	//��һ�μ���,�Ժ�ÿ����ɾ���ϴε�,Ȼ������µ�
	int iCount = GetCount();

	if(iCount >= 1)
		DeleteString(iCount - 1);
}

void CColorListBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int iCurSel = GetCurSel();
	if(iCurSel != -1)
	{
		CString strCurSel,strPage;
		GetText(iCurSel,strCurSel);
		//�������ѡ��
		int iLeft,iRight;
		iLeft = strCurSel.Find('(',0);
		iRight = strCurSel.Find(')',iLeft);
		strPage = strCurSel.Mid(iLeft+1,(iRight - iLeft -1));
		if(!strPage.IsEmpty())
		{
			((CMiniMainDlg *)AfxGetMainWnd())->GotoPage(strPage);
		}
	}
	CListBox::OnLButtonDblClk(nFlags, point);
}

void CColorListBox::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default IDR_MENU_INFO
	CPoint pt;
	GetCursorPos(&pt); // ��ǰ�������
	if(GetCount()<=0)
		return;
	BCMenu m_PopMenu;
	m_PopMenu.LoadMenu(IDR_MENU_INFO);
	m_PopMenu.LoadToolbar(IDR_MINICAMENU);

	CLanguage::TranslateMenu(&m_PopMenu, MAKEINTRESOURCE(IDR_MENU_INFO));

	CMenu * pPopup = m_PopMenu.GetSubMenu(0);
	SetForegroundWindow(); //������Թر�
	ASSERT(pPopup);
	UINT nSelection = pPopup->TrackPopupMenu(TPM_RIGHTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|
		TPM_NONOTIFY|TPM_RETURNCMD,pt.x, pt.y,this, NULL);
	m_PopMenu.DestroyMenu();
	//���ز˵�id
	if(nSelection == ID_MENUITEM_DELINFO)//ɾ��
	{
		ResetContent(); 
	}
	else if(nSelection == ID_MENUITEM_SAVEINFO)//����
	{
		CFile file;
		if(file.Open(m_FileName,CFile::modeWrite|CFile::modeCreate))
		{
			CString str;

			for(int i=0; i<GetCount();i++)
			{
				GetText(i,str);
				str+="\r\n";
				file.Write(str.GetBuffer(0),strlen(str));
			}
			file.Close();
		}
		else
		{
			CString str;
			str.Format("�����ļ�%sʧ��",m_FileName);
			AddMsg(str,M_ERROR);
		}
	}
	
	CListBox::OnRButtonDown(nFlags, point);
}

int CColorListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetFont(&m_font);
	// TODO: Add your specialized creation code here
	
	return 0;
}

/*void CColorListBox::AddButInfo(CString lpszItem,DWORD info)
{
	if(lpszItem.IsEmpty())
		return ;
	int nItem = AddString(lpszItem);
	if (nItem >= 0)
		SetItemData(nItem,info);
	int numList=GetCount()-1;
	SetTopIndex(numList);
	SetCurSel(numList);

	DestroyBtn();

	m_pButYes = new CButtonST;
	m_pButNo = new CButtonST;
	ASSERT(m_pButYes);
	ASSERT(m_pButNo);
	CRect rText;
	GetItemRect(nItem, rText);

	UINT len = lpszItem.GetLength();
	
	CRect r;
	r.top = rText.top + (rText.Height() - 16)/2;
	r.left = rText.left + lpszItem.GetLength() * 8;
	r.right = r.left + 17; //Ask the edit box for the width to use
	r.bottom = r.top + 17;


	m_pButYes->Create("",WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, r, 
		this, IDC_B_YES);
	m_pButYes->SetIcon(IDI_ICON_YES);

	r.left = r.left + 25;
	r.right = r.left + 17; //Ask the edit box for the width to use
	m_pButNo->Create("",WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, r, 
		this, IDC_B_NO);
	m_pButNo->SetIcon(IDI_ICON_NO);
	//����
}*/

void CColorListBox::OnDestroy() 
{
	CListBox::OnDestroy();
	
	// TODO: Add your message handler code here
	DestroyBtn();	
}

void CColorListBox::DestroyBtn()
{
/*	if (m_pButYes)
	{
		m_pButYes->DestroyWindow();
		delete m_pButYes;
		m_pButYes = NULL;
	}

	if (m_pButNo)
	{
		m_pButNo->DestroyWindow();
		delete m_pButNo;
		m_pButNo = NULL;
	}*/
}

/*void CColorListBox::OnBNo() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox("NO");
	DestroyBtn();
}

void CColorListBox::OnBYes() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox("YES");
	DestroyBtn();
}*/

void CColorListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();		
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CColorListBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();		
	CListBox::OnHScroll(nSBCode, nPos, pScrollBar);
}

/*BOOL CColorListBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rectbk; // �ͻ����Ĵ�С 
	CDC dcMen; // �ڴ��豸������ 
	// �õ�λͼbitmap�Ĵ�С 
	BITMAP stBitmap; 
	m_bitmap.GetObject(sizeof(BITMAP),&stBitmap); 
	CSize bmsize(stBitmap.bmWidth,stBitmap.bmHeight); 

	dcMen.CreateCompatibleDC(pDC); // �������ݵ��豸������ 

	// ѡ���µ�λͼ���󲢱���ɵ�λͼ���� 
	CBitmap * pold = dcMen.SelectObject(&m_bitmap); 
	GetClientRect(&rectbk); 
	// ȡ�ÿͻ����Ĵ�С 
	// ���ڴ�����Ļ����λͼ���� 
	pDC->StretchBlt(rectbk.left,rectbk.top,rectbk.Width(), 
		rectbk.Height(),&dcMen,rectbk.left,rectbk.top,bmsize.cx,
		bmsize.cy,SRCCOPY); 

	dcMen.SelectObject(pold); 
	// �ָ��ɵ�λͼ���� 
	dcMen.DeleteDC(); 
	// �ͷ��ڴ��豸������ 
//	return FALSE;
	return CListBox::OnEraseBkgnd(pDC);
}*/

BOOL CColorListBox::SetBkImage(UINT nIDResource)
{
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();
	
	HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(nIDResource), IMAGE_BITMAP, 0, 0, 
		LR_CREATEDIBSECTION);

    LOGBRUSH LogBrush;
    LogBrush.lbStyle = BS_PATTERN;
    LogBrush.lbHatch = (LONG)hbmp;

	m_brBmp.CreateBrushIndirect(&LogBrush);	

	if( hbmp == NULL ) 
		return FALSE;
	
	m_bitmap.Attach( hbmp );
	return TRUE;	
}

HBRUSH CColorListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	return m_brBmp;	//��ʾͼ��
	// TODO: Return a non-NULL brush if the parent's handler should not be called
}

void CColorListBox::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	Invalidate();			
}

void CColorListBox::SetSaveFile(CString strFileName)
{
	m_FileName = strFileName;
}
