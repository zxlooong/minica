// ExtRichEdit.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "ExtRichEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtRichEdit

CExtRichEdit::CExtRichEdit()
{
}

CExtRichEdit::~CExtRichEdit()
{
}


BEGIN_MESSAGE_MAP(CExtRichEdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CExtRichEdit)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtRichEdit message handlers

int CExtRichEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CHARFORMAT cf;

	cf.cbSize = sizeof (CHARFORMAT);  
	cf.dwMask = CFM_FACE | CFM_SIZE; 
	//cf.dwEffects; 
	cf.yHeight = 180; 
	//cf.yOffset; 
	//cf.crTextColor=RGB(250,0,0); 
	//cf.bCharSet; 
	//cf.bPitchAndFamily; 
	sprintf(cf.szFaceName, "MS Sans Serif"); 
 
	SetDefaultCharFormat(cf); 
	
	return 0;
}

void CExtRichEdit::AddMsg(CString strMsg, COLORREF crNewColor, 
							BOOL bUnderLine, 
							BOOL bBold)
{
	int iTotalLength = GetWindowTextLength();
	SetSel(iTotalLength, iTotalLength);
	ReplaceSel((LPCTSTR)strMsg);
	int iStartPos = iTotalLength;
	int iEndPos = GetWindowTextLength();

	CHARFORMAT cf;

	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_UNDERLINE;
	cf.dwEffects = (unsigned long)~(CFE_UNDERLINE | CFE_BOLD | CFE_AUTOCOLOR);
	cf.crTextColor = crNewColor;
	cf.dwEffects |= bUnderLine ? CFE_UNDERLINE : cf.dwEffects ;
	cf.dwEffects |= bBold ? CFE_BOLD : cf.dwEffects;

	SetSel(iStartPos, iEndPos);
//		TRACE(_T("AddMsg=====Start:%d, End:%d, crNewColor:%ld\n"), iStartPos, iEndPos, cf.crTextColor);
	SetSelectionCharFormat(cf);
	HideSelection(TRUE, FALSE);

	for(int i = 0,index = 0; ; i++,index++)
	{
		index = strMsg.Find("\n",index);
		if(index == -1)
			break;
	}
	LineScroll(i);//检索有几个\n，就滚动几行
}


void CExtRichEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//AfxMessageBox("OK");
//	SetSel(0,10);
//	int nCount = GetLineCount( ) ;
//	int iIndex = LineIndex(0);
//	CString str;
//	GetLine(10,str.GetBuffer(0),255);
//	AfxMessageBox(str);
//	CRichEditCtrl::OnLButtonDblClk(nFlags, point);
}
