// COleTarget.cpp: implementation of the COleDropTargetEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\MiniCA.h"
#include "OleTarget.h"
#include <shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//********************************************************************
// If you don't have a recent Platform SDK installed, you'll get linker
// errors on CLSID_DragDropHelper and IID_IDropTargetHelper, and you
// won't have the definition of IDropTargetHelper.  Uncomment the
// following stuff to fix that.
/*
struct IDropTargetHelper : public IUnknown
{
// IUnknown methods
STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
STDMETHOD_(ULONG, Release) ( THIS ) PURE;

  // IDropTargetHelper
  STDMETHOD (DragEnter)(THIS_ HWND hwndTarget, IDataObject* pDataObject,
  POINT* ppt, DWORD dwEffect) PURE;
  STDMETHOD (DragLeave)(THIS) PURE;
  STDMETHOD (DragOver)(THIS_ POINT* ppt, DWORD dwEffect) PURE;
  STDMETHOD (Drop)(THIS_ IDataObject* pDataObject, POINT* ppt,
  DWORD dwEffect) PURE;
  STDMETHOD (Show)(THIS_ BOOL fShow) PURE;
  };
  
	// {4657278A-411B-11d2-839A-00C04FD918D0}
	extern "C" const GUID __declspec(selectany) CLSID_DragDropHelper = 
    { 0x4657278a, 0x411b, 0x11d2, { 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0 }};
	
	  // {4657278B-411B-11d2-839A-00C04FD918D0}
	  extern "C" const GUID __declspec(selectany) IID_IDropTargetHelper = 
	  { 0x4657278b, 0x411b, 0x11d2, { 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0 }};
*/
//********************************************************************


//////////////////////////////////////////////////////////////////////
// Construction/destruction

CDropTarget::CDropTarget() :m_piDropHelper(NULL), m_bUseDnDHelper(false)
{
    // Create an instance of the shell DnD helper object.
    if ( SUCCEEDED( CoCreateInstance ( CLSID_DragDropHelper, NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper, 
		(void**) &m_piDropHelper ) ))
	{
        m_bUseDnDHelper = true;
	}
}

CDropTarget::~CDropTarget()
{
    if ( NULL != m_piDropHelper )
        m_piDropHelper->Release();
}


//////////////////////////////////////////////////////////////////////
// IDropTarget methods

DROPEFFECT CDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject,
									   DWORD dwKeyState, CPoint point )
{
	m_StrArray.RemoveAll();

	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	
    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.
	
	 if ( NULL == pDataObject->GetGlobalData ( m_uCustomClipbrdFormat ))
	{
	// Look for CF_HDROP data in the data object, and accept the drop if
	// it's there.
	
	  if ( NULL != pDataObject->GetGlobalData ( CF_HDROP ) )
	  dwEffect = DROPEFFECT_COPY;
	}
	
    // Call the DnD helper.
	
    if ( m_bUseDnDHelper )
	{
        // The DnD helper needs an IDataObject interface, so get one from
        // the COleDataObject.  Note that the FALSE param means that
        // GetIDataObject will not AddRef() the returned interface, so 
        // we do not Release() it.
		
        IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
		
        m_piDropHelper->DragEnter ( pWnd->GetSafeHwnd(), piDataObj, 
			&point, dwEffect );
	}
	
    return dwEffect;
}

DROPEFFECT CDropTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject,
									  DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	
    // Check for our own custom clipboard format in the data object.  If it's
    // present, then the DnD was initiated from our own window, and we won't
    // accept the drop.
    // If it's not present, then we check for CF_HDROP data in the data object.
	
    if ( NULL == pDataObject->GetGlobalData ( m_uCustomClipbrdFormat ))
	{
        // Look for CF_HDROP data in the data object, and accept the drop if
        // it's there.
		
        if ( NULL != pDataObject->GetGlobalData ( CF_HDROP ) )
            dwEffect = DROPEFFECT_COPY;
	}
	
    // Call the DnD helper.
	
    if ( m_bUseDnDHelper )
	{
        m_piDropHelper->DragOver ( &point, dwEffect );
	}
	
    return dwEffect;
}

BOOL CDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
							DROPEFFECT dropEffect, CPoint point )
{
    // Read the CF_HDROP data and put the files in the main window's list.
	
   ReadHdropData (pDataObject);
	
    // Call the DnD helper.
	
    if ( m_bUseDnDHelper )
	{
        // The DnD helper needs an IDataObject interface, so get one from
        // the COleDataObject.  Note that the FALSE param means that
        // GetIDataObject will not AddRef() the returned interface, so 
        // we do not Release() it.
		
        IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
        m_piDropHelper->Drop ( piDataObj, &point, dropEffect );
	}

	ASSERT(::IsWindow(pWnd->m_hWnd));
	return ::SendMessage( pWnd->GetSafeHwnd(), DROPM_DROPOK, (WPARAM)&m_StrArray, 0 );
}

void CDropTarget::OnDragLeave ( CWnd* pWnd )
{
    if ( m_bUseDnDHelper )
	{
        m_piDropHelper->DragLeave();
	}
}

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.
BOOL CDropTarget::ReadHdropData (COleDataObject* pDataObject )
{
	HGLOBAL     hg;
	HDROP       hdrop;
	UINT        uNumFiles;
	TCHAR       szNextFile [MAX_PATH];
//	HANDLE      hFind;
//	WIN32_FIND_DATA rFind;
//	TCHAR       szFileLen [64];
	
    // Get the HDROP data from the data object.
	
    hg = pDataObject->GetGlobalData ( CF_HDROP );
    
    if ( NULL == hg )
	{
        return FALSE;
	}
	
    hdrop = (HDROP) GlobalLock ( hg );
	
    if ( NULL == hdrop )
	{
        GlobalUnlock ( hg );
        return FALSE;
	}
	
    // Get the # of files being dropped.
	
    uNumFiles = DragQueryFile ( hdrop, -1, NULL, 0 );
	
    for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
	{
        // Get the next filename from the HDROP info.
		
        if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
		{
            // If the filename is already in the list, skip it.
			
            // Get the index of the file's icon in the system image list and
            // it's type description.
			
/*            SHGetFileInfo ( szNextFile, 0, &rFileInfo, sizeof(rFileInfo),
				SHGFI_SYSICONINDEX | SHGFI_ATTRIBUTES |
				SHGFI_TYPENAME );*/

			m_StrArray.Add(szNextFile);
			
       /*     hFind = FindFirstFile ( szNextFile, &rFind );
			
            if ( INVALID_HANDLE_VALUE != hFind )
			{
                StrFormatByteSize ( rFind.nFileSizeLow, szFileLen, 64 );
                FindClose ( hFind );
			}*/
			
		}
	}   // end for
	
    GlobalUnlock ( hg );
	
    // Resize columns so all text is visible.
	
    return TRUE;
}
