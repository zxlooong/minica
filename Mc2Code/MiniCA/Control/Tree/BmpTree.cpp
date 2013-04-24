// BmpTree.cpp : implementation file
//

#include "stdafx.h"
#include "..\..\minica.h"
#include "BmpTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpTree

CBmpTree::CBmpTree()
{
	m_hCursor = ::LoadCursor(NULL, IDC_HAND); // Load Windows' hand cursor
}

CBmpTree::~CBmpTree()
{
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();
}


BEGIN_MESSAGE_MAP(CBmpTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CBmpTree)
//	ON_WM_DROPFILES()
//	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_SETCURSOR()
//	ON_WM_PAINT()
//	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpTree message handlers
void CBmpTree::ExpandParent(HTREEITEM item,UINT nCode )////展开,收缩
{
	SetRedraw(FALSE);
	if(item)
		Expand( item, nCode );
	else
		return;
	HTREEITEM hParentItem =  GetParentItem( item );
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

HTREEITEM CBmpTree::FindItem(HTREEITEM hItem, CString pageName)
{
	if(NULL == hItem)
		return NULL;
	HTREEITEM hChildItem,hBrotherItem,hResult = NULL;
	CString strName = GetItemText(hItem);
	if(strName == pageName)
	{
		return hItem;
	}
	
	//查找子节点，没有就结束
	hChildItem = GetChildItem(hItem);
	if(hChildItem!=NULL)
	{
		//设置子节点的状态与当前节点的状态一致
		//再递归处理子节点的子节点和兄弟节点
		hResult = FindItem(hChildItem, pageName);
		if(hResult)
			return hResult;
		//处理子节点的兄弟节点和其子节点
		hBrotherItem = GetNextSiblingItem(hChildItem);
		while (hBrotherItem)
		{
			//设置子节点的兄弟节点状态与当前节点的状态一致

			//再递归处理子节点的兄弟节点的子节点和兄弟节点
			hResult = FindItem(hBrotherItem, pageName);
			if(hResult)
				return hResult;
			hBrotherItem = GetNextSiblingItem(hBrotherItem);
		}
	}
	return NULL;
}

/*void CBmpTree::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WORD wNumFilesDropped = DragQueryFile(hDropInfo, -1, NULL, 0);
    // 
	POINT  pt;
	DragQueryPoint(hDropInfo, &pt);
	CString FileName;
    for (WORD x = 0 ; x < wNumFilesDropped; x++)
	{
        // 得到PathName的长度,由于第三个参数为NULL，所以调用会失败并返回所需长度
        WORD wPathnameSize = DragQueryFile(hDropInfo, x, NULL, 0);
        // 分配memory
        char * npszFile = (char *) LocalAlloc(LPTR, wPathnameSize += 1);
        // 分配错
        if (npszFile == NULL) continue;
        // 再次得到文件名
        DragQueryFile(hDropInfo, x, npszFile, wPathnameSize);
		// 如果是快捷方式则进行转换得到正确的文件名,expandedFile为最后的文件名
		FileName = ExpandShortcut(npszFile);
        // 释放memory
        LocalFree(npszFile);
    }
    //完成
    DragFinish(hDropInfo);

	//得到文件扩展名 信息安全 *.* ; OCSP客户端 数证解析 *.pfx *.cer,*.der,*.pem ;  ASN1解码 ; 系统注册

	//展开树控件
	UINT uFlags = 0;
	HTREEITEM  hItem = HitTest(pt,&uFlags);
	if(hItem == NULL)
		return;

	if(uFlags & TVHT_ONITEMICON || uFlags & TVHT_ONITEMLABEL)
	{
		CString strName = GetItemText(hItem);
	//	if(strName == "信息安全")
		GotoPage(strName);


	}

	CTreeCtrl::OnDropFiles(hDropInfo);
}*/

/*CString CBmpTree::ExpandShortcut(char * inFile)
{
    CString outFile = "";
	// Make sure we have a path
	ASSERT(inFile != _T(""));
	IShellLink * psl;
	HRESULT hres;
	// Create instance for shell link
	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (LPVOID*) &psl);
	if (SUCCEEDED(hres))
	{
		// Get a pointer to the persist file interface
		IPersistFile* ppf;
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
		if (SUCCEEDED(hres))
		{
			// Make sure it's ANSI
			WORD wsz[MAX_PATH];
			::MultiByteToWideChar(CP_ACP, 0, inFile, -1, wsz, MAX_PATH);
			
			// Load shortcut
			hres = ppf->Load(wsz, STGM_READ);
			if (SUCCEEDED(hres))
			{
                WIN32_FIND_DATA wfd;
                // find the path from that
                HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH), 
					MAX_PATH,
					&wfd, 
					SLGP_UNCPRIORITY);
				
                outFile.ReleaseBuffer();
			}
			else 
				outFile = inFile;
			ppf->Release();
		}
		psl->Release();
	}
	
    // if this fails, outFile == ""
	return outFile;
}*/

BOOL CBmpTree::GotoPage(CString pageName)//转到某页
{
	//遍历树,查找名称,展开父,选择
	if(pageName.IsEmpty())
		return FALSE;
	HTREEITEM hItem = FindItem(GetRootItem(),pageName);
	if(hItem!=NULL)
	{
		SelectItem(hItem);
		EnsureVisible(hItem);
//		ExpandParent(hItem,TVE_EXPAND);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CBmpTree::SetBkImage(UINT nIDResource)
{
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();
	
	HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(nIDResource), IMAGE_BITMAP, 0, 0, 
		LR_CREATEDIBSECTION);
	
	if( hbmp == NULL ) 
		return FALSE;
	
	m_bitmap.Attach( hbmp );

	return TRUE;	
}

/*BOOL CBmpTree::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rcclient;
	GetClientRect(&rcclient);
	
	// create a compatible memory dc 
	CDC memdc;
	memdc.CreateCompatibleDC(pDC); 
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rcclient.Width(), rcclient.Height());
	memdc.SelectObject( &bitmap );
	DefWindowProc(WM_PAINT, (WPARAM)memdc.m_hDC , 0);
	
	CDC maskdc;
	maskdc.CreateCompatibleDC(pDC);
	CBitmap maskbitmap;
	maskbitmap.CreateCompatibleBitmap(pDC, rcclient.Width(), rcclient.Height());
	maskdc.SelectObject( &maskbitmap );
	CBrush brush;
	brush.CreatePatternBrush(&m_bitmap);
	maskdc.FillRect(rcclient, &brush);
	maskdc.BitBlt(0,0,rcclient.Width(), rcclient.Height(),&memdc,0,0,SRCAND);
	
	pDC->BitBlt(rcclient.left, rcclient.top, rcclient.Width(), rcclient.Height(), 
		&maskdc,rcclient.left, rcclient.top,SRCCOPY);
	
	return TRUE;
	return CTreeCtrl::OnEraseBkgnd(pDC);
}*/

void CBmpTree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	Invalidate();
	SetRedraw(TRUE);
	*pResult = 0;
}

void CBmpTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	SetRedraw(FALSE);
	*pResult = 0;
}

//void CBmpTree::OnPaint() 
//{
					   /*	CRect rcclient;
					   GetClientRect(&rcclient);//区域大小
					   
						 CDC dcMem;
						 dcMem.CreateCompatibleDC(&dc);
						 CBitmap* pOldBitmap = dcMem.SelectObject(&m_bitmap);
						 dc.BitBlt(0,0,rcclient.Width(),rcclient.Height(),&dcMem,
						 rcclient.left, rcclient.top,SRCCOPY);
	dcMem.SelectObject(pOldBitmap);*/
	
//	CPaintDC dc(this); // device context for painting
	//不能注视这句话,否则界面会停止刷新
	
/*	CRect rcclient;
	GetClientRect(&rcclient);
	
	// create a compatible memory dc 
	CDC memdc;
	memdc.CreateCompatibleDC(&dc); 
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rcclient.Width(), rcclient.Height());
	memdc.SelectObject( &bitmap );
	DefWindowProc(WM_PAINT, (WPARAM)memdc.m_hDC , 0);
	
	CDC maskdc;
	maskdc.CreateCompatibleDC(&dc);
	CBitmap maskbitmap;
	maskbitmap.CreateCompatibleBitmap(&dc, rcclient.Width(), rcclient.Height());
	maskdc.SelectObject( &maskbitmap );
	CBrush brush;
	brush.CreatePatternBrush(&m_bitmap);
	maskdc.FillRect(rcclient, &brush);
	maskdc.BitBlt(0,0,rcclient.Width(), rcclient.Height(),&memdc,0,0,SRCAND);
	
	dc.BitBlt(rcclient.left, rcclient.top, rcclient.Width(), rcclient.Height(), 
		&maskdc,rcclient.left, rcclient.top,SRCCOPY);*/
	
//}


/*HBRUSH CBmpTree::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CTreeCtrl::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	pDC->SetBkMode(TRANSPARENT);
	// TODO: Return a different brush if the default is not desired
	return hbr;
}*/

BOOL CBmpTree::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	::SetCursor(m_hCursor);
	return TRUE;	
//	return CTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
}
