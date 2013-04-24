//////////////////////////////////////////////////////////////////////
//
// DropTarget.h: interface for the CDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDROPTARGET_H__04E739F2_5474_4A14_97CB_BDF035E0B8B2__INCLUDED_)
#define AFX_MYDROPTARGET_H__04E739F2_5474_4A14_97CB_BDF035E0B8B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxole.h"

#define DROPM_DROPOK	WM_USER + 200  //ÍÏ¶¯½áÊø
//////////////////////////////////////////////////////////////////////
// CDropTarget implements a drop target for the MultiFiler dialog.
// I was also messing around with the IDropTargetHelper stuff in 
// Win2K, which lets the shell draw the nifty shaded drag image when
// you drag into the dialog.  If you're using 9x or NT 4, that stuff
// is disabled.
 
struct IDropTargetHelper;   // forward reference, in case the latest PSDK isn't installed.

class CDropTarget : public COleDropTarget  
{
public:
	BOOL Reg(CWnd * pWnd, char * lpstrClipbrdFormat)
	{
		m_uCustomClipbrdFormat = RegisterClipboardFormat ( _T(lpstrClipbrdFormat));
		return Register(pWnd);
	}
	UINT GetClipbrdFormat()
	{
		return m_uCustomClipbrdFormat;
	}
    CDropTarget();
	virtual ~CDropTarget();

protected:
	CStringArray m_StrArray;
	UINT m_uCustomClipbrdFormat;
    IDropTargetHelper* m_piDropHelper;
    bool               m_bUseDnDHelper;
    BOOL ReadHdropData ( COleDataObject* pDataObject );
	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject,
		                     DWORD dwKeyState, CPoint point );

	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject,
		                    DWORD dwKeyState, CPoint point );

	BOOL OnDrop (CWnd* pWnd, COleDataObject* pDataObject,
		          DROPEFFECT dropEffect, CPoint point );

	void OnDragLeave (CWnd* pWnd);

};

#endif // !defined(AFX_MYDROPTARGET_H__04E739F2_5474_4A14_97CB_BDF035E0B8B2__INCLUDED_)
