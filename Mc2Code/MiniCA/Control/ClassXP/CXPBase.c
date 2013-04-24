////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ����
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���Ӿ��
extern HHOOK g_hCXPHook;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������
const CLASSXP c_cxpClass[] =
{
//	uClass				ptzClass					pDraw				wpProc
	{CXPC_BUTTON,		TEXT("Button"),				CXPDrawPushButton,	CXPButtonProc},		// ��ť
//	{CXPC_STATIC,		TEXT("Static"),				NULL,				NULL},				// ��̬�ؼ�

	{CXPC_EDITBOX,		TEXT("Edit"),				CXPDrawEditBox,		CXPEditBoxProc},	// �༭��
	{CXPC_HOTKEY,		TEXT("msctls_hotkey32"),	CXPDrawEditBox,		CXPEditBoxProc},	// �ȼ�
	{CXPC_IPBOX,		TEXT("SysIPAddress32"),		CXPDrawEditBox,		CXPEditBoxProc},	// IP ��ַ
	{CXPC_LISTBOX,		TEXT("ListBox"),			CXPDrawEditBox,		CXPEditBoxProc},	// �б��
	{CXPC_LISTBOX,		TEXT("ComboLBox"),			CXPDrawEditBox,		CXPEditBoxProc},	// ����б��
	{CXPC_LISTVIEW,		TEXT("SysListView32"),		CXPDrawEditBox,		CXPEditBoxProc},	// �б�鿴
	{CXPC_TREEVIEW,		TEXT("SysTreeView32"),		CXPDrawEditBox,		CXPEditBoxProc},	// ���β鿴

	{CXPC_COMBOBOX,		TEXT("ComboBox"),			CXPDrawComboBox,	CXPComboBoxProc},	// ��Ͽ�
	{CXPC_DATETIME,		TEXT("SysDateTimePick32"),	CXPDrawComboBox,	CXPComboBoxProc},	// ����/ʱ��ѡ��

	{CXPC_SPINBOX,		TEXT("msctls_updown32"),	CXPDrawSpinBox,		CXPSpinBoxProc},	// ��ת�ؼ�
//	{CXPC_SCROLLBAR,	TEXT("ScrollBar"),			NULL,				NULL},				// ������

//	{CXPC_TRACKBAR,		TEXT("msctls_trackbar32"),	NULL,				NULL},				// ������
//	{CXPC_PROGRESS,		TEXT("msctls_progress32"),	NULL,				NULL},				// ������

//	{CXPC_TABCTRL,		TEXT("SysTabControl32"),	NULL,				NULL},				// �Ʊ�ؼ�
//	{CXPC_CALENDER,		TEXT("SysMonthCal32"),		NULL,				NULL},				// �����ؼ�
//	{CXPC_ANIMATE,		TEXT("SysAnimate32"),		NULL,				NULL},				// �����ؼ�

//	{CXPC_HEADER,		TEXT("SysHeader32"),		NULL,				NULL},				// ����ؼ�
//	{CXPC_TOOLBAR,		TEXT("ToolbarWindow32"),	NULL,				NULL},				// ������
//	{CXPC_STATUSBAR,	TEXT("msctls_statusbar32"),	NULL,				NULL},				// ״̬��
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���Ӻ���
LRESULT CALLBACK CXPHookProc(INT iCode, WPARAM wParam, LPARAM lParam)
{
	if (((PCWPSTRUCT) lParam)->message == WM_CREATE)
	{
		CXPSet(((PCWPSTRUCT) lParam)->hwnd, TRUE);
	}

	return CallNextHookEx(g_hCXPHook, iCode, wParam, lParam);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ö�ٺ���
BOOL CALLBACK CXPEnumProc(HWND hWnd, LPARAM lParam)
{
	// ���û�ȡ������ ClassXP
	CXPSet(hWnd, (BOOL) lParam);

	// ö���Ӵ���
	EnumChildWindows(hWnd, CXPEnumProc, lParam);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���û�ȡ������ ClassXP
BOOL CXPSet(HWND hWnd, BOOL bClassXP)
{
	PCLASSXP pCxp;

	pCxp = (PCLASSXP) GetProp(hWnd, TEXT("ClassXP"));
	if (bClassXP && (pCxp == NULL))
	{
		pCxp = (PCLASSXP) _HeapAlloc(sizeof(CLASSXP));
		pCxp->hWnd = hWnd;
		pCxp->lStyle = GetWindowLong(hWnd, GWL_STYLE);

		if (CXPGetClass(pCxp) && CXPGetState(pCxp))
		{
			SetProp(hWnd, TEXT("ClassXP"), (HANDLE) pCxp);
			pCxp->wpPrev = (WNDPROC) SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR) pCxp->wpProc);
			RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);

			return TRUE;
		}

		_HeapFree(pCxp);
	}
	else if ((bClassXP == FALSE) && pCxp)
	{
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG_PTR) pCxp->wpPrev);
		RemoveProp(hWnd, TEXT("ClassXP"));
		_HeapFree(pCxp);
		RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);

		return TRUE;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ȡ������
BOOL CXPGetClass(PCLASSXP pCxp)
{
	UINT i;
	TCHAR tzClass[MAX_PATH];

	// �����ж�ƥ���������
	_Assert(pCxp);
	GetClassName(pCxp->hWnd, tzClass, MAX_PATH);
	for (i = 0; i < _NumOf(c_cxpClass); i++)
	{
		if (lstrcmpi(tzClass, c_cxpClass[i].ptzClass) == 0)
		{
			CopyMemory(pCxp, &c_cxpClass[i], sizeof(CLASSXP) - sizeof(HWND) * 2);
			return TRUE;
		}
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ȡ����״̬
BOOL CXPGetState(PCLASSXP pCxp)
{
	PCLASSXP p;
	UINT uCheck;

	// ���ý��úͽ���״̬
	pCxp->lState = _CXPGetStyle(WS_DISABLED) ? CXPS_DISABLED : 0;
	_ExIf(pCxp->hWnd == GetFocus(), _CXPSetState(CXPS_FOCUS));

	switch (pCxp->uClass)
	{
	case CXPC_BUTTON:
		switch (_CXPGetStyle(BS_TYPEMASK))
		{
		case BS_DEFPUSHBUTTON:		// Ĭ�ϰ�ť
			_CXPSetState(CXPS_DEFAULT);

		case BS_PUSHBUTTON:			// ��ͨ��ť
			break;

		case BS_CHECKBOX:			// ��ѡ��
		case BS_AUTOCHECKBOX:		// �Զ���ѡ��
		case BS_3STATE:				// ��״̬��ѡ��
		case BS_AUTO3STATE:			// �Զ���״̬��ѡ��

		case BS_RADIOBUTTON:		// ��ѡ��
		case BS_AUTORADIOBUTTON:	// �Զ���ѡ��
			if (!_CXPGetStyle(BS_PUSHLIKE))
			{
				pCxp->pDraw = CXPDrawCheckBox;
			}
			break;

		default:					// �Ի水ť����Ͽ������δ֪����
			return FALSE;
		}
		uCheck = (LONG) SendMessage(pCxp->hWnd, BM_GETCHECK, 0, 0);
		if (uCheck == BST_CHECKED)
		{
			_CXPSetState(CXPS_CHECKED);
		}
		else if (uCheck == BST_INDETERMINATE)
		{
			_CXPSetState(CXPS_INDETERMINATE);
		}
		break;

	case CXPC_EDITBOX:
		if (!_CXPGetStyle(WS_BORDER) && !(GetWindowLong(pCxp->hWnd, GWL_EXSTYLE) & (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)))
		{
			return FALSE;
		}
		_ExIf(_CXPGetStyle(ES_READONLY), _CXPSetState(CXPS_READONLY));
		break;

	case CXPC_COMBOBOX:
		if (_CXPGetStyle(CBS_TYPEMASK) == CBS_SIMPLE)
		{
			_CXPSetState(CXPS_SIMPLECMB);
		}
		break;

	case CXPC_SPINBOX:
		_ExIf(_CXPGetStyle(UDS_HORZ), _CXPSetState(CXPS_HORIZON));
		_ExIf(_CXPGetStyle(UDS_ARROWKEYS), _CXPSetState(CXPS_ACCELSPIN));
		_ExIf(_CXPGetStyle(UDS_ALIGNLEFT), _CXPSetState(CXPS_LEFTSPIN));
		_ExIf(_CXPGetStyle(UDS_ALIGNRIGHT), _CXPSetState(CXPS_RIGHTSPIN));

		// ���þɻ��
		p = (PCLASSXP) pCxp->lParam;
		_ExIf(p, p->lParam = 0);

		p = (PCLASSXP) GetProp((HWND) SendMessage(pCxp->hWnd, UDM_GETBUDDY, 0, 0), TEXT("ClassXP"));
		if (p && (p->uClass == CXPC_EDITBOX))
		{
			// �����»��
			p->lParam = (LPARAM) pCxp;
			pCxp->lParam = (LPARAM) p;
		}
		break;
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
