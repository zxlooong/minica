


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 通用消息处理
LRESULT CXPCommonProc(PCLASSXP pCxp, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		if (!_CXPGetState(CXPS_HOTLIGHT) && !(wParam & MK_LBUTTON) && CXPTrackMouseLeave(pCxp->hWnd))
		{
			_CXPSetState(CXPS_HOTLIGHT);
			break;
		}
		return CallWindowProc(pCxp->wpPrev, pCxp->hWnd, uMsg, wParam, lParam);

	case WM_MOUSELEAVE:
		if (_CXPGetState(CXPS_HOTLIGHT))
		{
			_CXPClearState(CXPS_HOTLIGHT);
			break;
		}
		return CallWindowProc(pCxp->wpPrev, pCxp->hWnd, uMsg, wParam, lParam);

	case WM_ENABLE:
		_CXPSetState2(CXPS_DISABLED, !wParam);
		break;

	case WM_SETFOCUS:
		_CXPSetState(CXPS_FOCUS);
		break;

	case WM_KILLFOCUS:
		_CXPClearState(CXPS_FOCUS);
		break;

	case WM_NCDESTROY:
		lResult = CallWindowProc(pCxp->wpPrev, pCxp->hWnd, uMsg, wParam, lParam);
		CXPSet(pCxp->hWnd, FALSE);
		return lResult;

	default:
		return CallWindowProc(pCxp->wpPrev, pCxp->hWnd, uMsg, wParam, lParam);
	}

	lResult = CallWindowProc(pCxp->wpPrev, pCxp->hWnd, uMsg, wParam, lParam);
	pCxp->pDraw(pCxp);

	return lResult;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 按钮回调函数
LRESULT CALLBACK CXPButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG lStyle;
	PCLASSXP pCxp;
	LRESULT lResult;

	pCxp = (PCLASSXP) GetProp(hWnd, TEXT("ClassXP"));
	_Assert(pCxp);

	switch (uMsg)
	{
	case BM_SETSTYLE:
		_CXPSetState2(CXPS_DEFAULT, wParam & BS_DEFPUSHBUTTON);
		break;

	case BM_SETSTATE:
		_CXPSetState2(CXPS_PRESSED, wParam);
		lStyle = GetWindowLong(hWnd, GWL_STYLE);
		SetWindowLong(hWnd, GWL_STYLE, (lStyle & ~BS_TYPEMASK) | BS_GROUPBOX);
		lResult = CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
		SetWindowLong(hWnd, GWL_STYLE, lStyle);
		pCxp->pDraw(pCxp);
		return lResult;

	case BM_SETCHECK:
		_CXPSetState2(CXPS_CHECKED, (wParam == BST_CHECKED));
		_CXPSetState2(CXPS_INDETERMINATE, (wParam == BST_INDETERMINATE));
		break;

	case WM_PAINT:
	case WM_SETTEXT:
		lResult = DefWindowProc(hWnd, uMsg, wParam, lParam);
		pCxp->pDraw(pCxp);
		return lResult;

	case WM_ERASEBKGND:
		return TRUE;

	default:
		return CXPCommonProc(pCxp, uMsg, wParam, lParam);
	}

	lResult = CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
	pCxp->pDraw(pCxp);

	return lResult;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 编辑框回调函数
LRESULT CALLBACK CXPEditBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PCLASSXP pCxp;
	LRESULT lResult;

	pCxp = (PCLASSXP) GetProp(hWnd, TEXT("ClassXP"));
	_Assert(pCxp);

	switch (uMsg)
	{
	case EM_SETREADONLY:
		_CXPSetState2(CXPS_READONLY, wParam);

	case WM_NCPAINT:
		lResult = DefWindowProc(hWnd, uMsg, wParam, lParam);
		pCxp->pDraw(pCxp);
		return lResult;

	/*case WM_SYSCOMMAND:
	case WM_LBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_CHAR:
	case WM_PASTE:
		lStyle = GetWindowLong(hWnd, GWL_STYLE);
		SetWindowLong(hWnd, GWL_STYLE, lStyle & ~(WS_VSCROLL | WS_HSCROLL));
		lResult = CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
		SetWindowLong(hWnd, GWL_STYLE, lStyle);
		return lResult;*/

	default:
		return CXPCommonProc(pCxp, uMsg, wParam, lParam);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 组合框回调函数
LRESULT CALLBACK CXPComboBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PCLASSXP pCxp;
	LRESULT lResult;

	pCxp = (PCLASSXP) GetProp(hWnd, TEXT("ClassXP"));
	_Assert(pCxp);

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		_CXPSetState(CXPS_PRESSED);
		break;

	case WM_LBUTTONUP:
		_CXPClearState(CXPS_PRESSED);

	case WM_PAINT:
	case WM_NCPAINT:
		break;

	default:
		return CXPCommonProc(pCxp, uMsg, wParam, lParam);
	}

	lResult = CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
	pCxp->pDraw(pCxp);

	return lResult;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 旋转控件回调函数
LRESULT CALLBACK CXPSpinBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rtRect;
	PCLASSXP pCxp;
	LRESULT lResult;

	pCxp = (PCLASSXP) GetProp(hWnd, TEXT("ClassXP"));
	_Assert(pCxp);

	switch (uMsg)
	{
	case WM_STYLECHANGED:
		if (wParam == GWL_STYLE)
		{
			pCxp->lStyle = ((LPSTYLESTRUCT) lParam)->styleNew;
			CXPGetState(pCxp);
		}
		break;

	case WM_LBUTTONDOWN:
		_CXPSetState(CXPS_PRESSED);
		break;

	case WM_LBUTTONUP:
		_CXPClearState(CXPS_PRESSED);

	case WM_PAINT:
	case WM_NCPAINT:
	case WM_ERASEBKGND:
		lResult = DefWindowProc(hWnd, uMsg, wParam, lParam);
		pCxp->pDraw(pCxp);
		ReleaseCapture();
		return lResult;

	case WM_MOUSEMOVE:
		// 判断鼠标位于上下部分
		GetClientRect(hWnd, &rtRect);
		rtRect.right /= 2;
		rtRect.bottom /= 2;
		if (_CXPGetState(CXPS_FIRSTPART))
		{
			if ((_CXPGetState(CXPS_HORIZON) && (LOWORD(lParam) > rtRect.right)) ||
				(!_CXPGetState(CXPS_HORIZON) && (HIWORD(lParam) > rtRect.bottom)))
			{
				_CXPClearState(CXPS_FIRSTPART);
				break;
			}
		}
		else
		{
			if ((_CXPGetState(CXPS_HORIZON) && (LOWORD(lParam) <= rtRect.right)) ||
				(!_CXPGetState(CXPS_HORIZON) && (HIWORD(lParam) <= rtRect.bottom)))
			{
				_CXPSetState(CXPS_FIRSTPART);
				break;
			}
		}

	default:
		return CXPCommonProc(pCxp, uMsg, wParam, lParam);
	}

	lResult = CallWindowProc(pCxp->wpPrev, hWnd, uMsg, wParam, lParam);
	pCxp->pDraw(pCxp);

	return lResult;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
