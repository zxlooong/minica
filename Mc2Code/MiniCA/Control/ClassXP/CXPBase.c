////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 钩子句柄
extern HHOOK g_hCXPHook;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 类名称
const CLASSXP c_cxpClass[] =
{
//	uClass				ptzClass					pDraw				wpProc
	{CXPC_BUTTON,		TEXT("Button"),				CXPDrawPushButton,	CXPButtonProc},		// 按钮
//	{CXPC_STATIC,		TEXT("Static"),				NULL,				NULL},				// 静态控件

	{CXPC_EDITBOX,		TEXT("Edit"),				CXPDrawEditBox,		CXPEditBoxProc},	// 编辑框
	{CXPC_HOTKEY,		TEXT("msctls_hotkey32"),	CXPDrawEditBox,		CXPEditBoxProc},	// 热键
	{CXPC_IPBOX,		TEXT("SysIPAddress32"),		CXPDrawEditBox,		CXPEditBoxProc},	// IP 地址
	{CXPC_LISTBOX,		TEXT("ListBox"),			CXPDrawEditBox,		CXPEditBoxProc},	// 列表框
	{CXPC_LISTBOX,		TEXT("ComboLBox"),			CXPDrawEditBox,		CXPEditBoxProc},	// 组合列表框
	{CXPC_LISTVIEW,		TEXT("SysListView32"),		CXPDrawEditBox,		CXPEditBoxProc},	// 列表查看
	{CXPC_TREEVIEW,		TEXT("SysTreeView32"),		CXPDrawEditBox,		CXPEditBoxProc},	// 树形查看

	{CXPC_COMBOBOX,		TEXT("ComboBox"),			CXPDrawComboBox,	CXPComboBoxProc},	// 组合框
	{CXPC_DATETIME,		TEXT("SysDateTimePick32"),	CXPDrawComboBox,	CXPComboBoxProc},	// 日期/时间选择

	{CXPC_SPINBOX,		TEXT("msctls_updown32"),	CXPDrawSpinBox,		CXPSpinBoxProc},	// 旋转控件
//	{CXPC_SCROLLBAR,	TEXT("ScrollBar"),			NULL,				NULL},				// 滚动条

//	{CXPC_TRACKBAR,		TEXT("msctls_trackbar32"),	NULL,				NULL},				// 滑动条
//	{CXPC_PROGRESS,		TEXT("msctls_progress32"),	NULL,				NULL},				// 进度条

//	{CXPC_TABCTRL,		TEXT("SysTabControl32"),	NULL,				NULL},				// 制表控件
//	{CXPC_CALENDER,		TEXT("SysMonthCal32"),		NULL,				NULL},				// 日历控件
//	{CXPC_ANIMATE,		TEXT("SysAnimate32"),		NULL,				NULL},				// 动画控件

//	{CXPC_HEADER,		TEXT("SysHeader32"),		NULL,				NULL},				// 标题控件
//	{CXPC_TOOLBAR,		TEXT("ToolbarWindow32"),	NULL,				NULL},				// 工具栏
//	{CXPC_STATUSBAR,	TEXT("msctls_statusbar32"),	NULL,				NULL},				// 状态栏
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 钩子函数
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
// 枚举函数
BOOL CALLBACK CXPEnumProc(HWND hWnd, LPARAM lParam)
{
	// 设置或取消窗口 ClassXP
	CXPSet(hWnd, (BOOL) lParam);

	// 枚举子窗体
	EnumChildWindows(hWnd, CXPEnumProc, lParam);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 设置或取消窗口 ClassXP
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
// 获取窗口类
BOOL CXPGetClass(PCLASSXP pCxp)
{
	UINT i;
	TCHAR tzClass[MAX_PATH];

	// 查找判断匹配的类名称
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
// 获取窗口状态
BOOL CXPGetState(PCLASSXP pCxp)
{
	PCLASSXP p;
	UINT uCheck;

	// 设置禁用和焦点状态
	pCxp->lState = _CXPGetStyle(WS_DISABLED) ? CXPS_DISABLED : 0;
	_ExIf(pCxp->hWnd == GetFocus(), _CXPSetState(CXPS_FOCUS));

	switch (pCxp->uClass)
	{
	case CXPC_BUTTON:
		switch (_CXPGetStyle(BS_TYPEMASK))
		{
		case BS_DEFPUSHBUTTON:		// 默认按钮
			_CXPSetState(CXPS_DEFAULT);

		case BS_PUSHBUTTON:			// 普通按钮
			break;

		case BS_CHECKBOX:			// 复选框
		case BS_AUTOCHECKBOX:		// 自动复选框
		case BS_3STATE:				// 三状态复选框
		case BS_AUTO3STATE:			// 自动三状态复选框

		case BS_RADIOBUTTON:		// 单选框
		case BS_AUTORADIOBUTTON:	// 自动单选框
			if (!_CXPGetStyle(BS_PUSHLIKE))
			{
				pCxp->pDraw = CXPDrawCheckBox;
			}
			break;

		default:					// 自绘按钮、组合框和其它未知类型
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

		// 设置旧伙伴
		p = (PCLASSXP) pCxp->lParam;
		_ExIf(p, p->lParam = 0);

		p = (PCLASSXP) GetProp((HWND) SendMessage(pCxp->hWnd, UDM_GETBUDDY, 0, 0), TEXT("ClassXP"));
		if (p && (p->uClass == CXPC_EDITBOX))
		{
			// 设置新伙伴
			p->lParam = (LPARAM) pCxp;
			pCxp->lParam = (LPARAM) p;
		}
		break;
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
