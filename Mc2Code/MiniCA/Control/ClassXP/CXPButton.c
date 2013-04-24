


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制按钮
VOID CXPDrawPushButton(PCCLASSXP pCxp)
{
	UINT i;
	HDC hDC;
	RECT rtRect;
	PBYTE pbDib;
	TCHAR tzText[256];
	BITMAPINFOHEADER biBtn;

	// 开始绘制
	_CXPBeginDraw(hDC, FALSE, FALSE);

	// 选择图像
	if (_CXPGetState(CXPS_DISABLED))
	{
		i = 1;
	}
	else if (_CXPGetState(CXPS_PRESSED | CXPS_CHECKED | CXPS_INDETERMINATE))
	{
		i = 2;
	}
	else if (_CXPGetState(CXPS_HOTLIGHT))
	{
		i = 3;
	}
	else if (_CXPGetState(CXPS_FOCUS | CXPS_DEFAULT))
	{
		i = 0;
	}
	else
	{
		i = 4;
	}

	// 绘制图像
	biBtn = c_biCXPCheck;
	biBtn.biWidth = rtRect.right;
	biBtn.biHeight = rtRect.bottom;
	biBtn.biSizeImage = rtRect.right * rtRect.bottom * 4;
	pbDib = (PBYTE) _HeapAlloc(biBtn.biSizeImage);
	if (pbDib)
	{
		CXPDibBlendExt((PBYTE) pbDib, rtRect.right, rtRect.bottom, (PBYTE) c_crCXPPushButton[i], CXPW_BTN, CXPH_BTN);
		_DrawDIB(0, 0, rtRect.right, rtRect.bottom, pbDib, &biBtn);
		_HeapFree(pbDib);
	}

	// 绘制焦点框
	InflateRect(&rtRect, -3, -3);
	if (_CXPGetState(CXPS_FOCUS))
	{
		DrawFocusRect(hDC, &rtRect);
	}

	// 绘制文字
	if (GetWindowText(pCxp->hWnd, tzText, sizeof(tzText)))
	{
		i = _CXPGetStyle(BS_MULTILINE) ? (DT_WORDBREAK | DT_EDITCONTROL) : (DT_WORDBREAK | DT_EDITCONTROL | DT_SINGLELINE);
		i |= _TriIf(_CXPGetStyle(BS_LEFT), _CXPGetStyle(BS_RIGHT), DT_LEFT, DT_RIGHT, DT_CENTER);
		i |= _TriIf(_CXPGetStyle(BS_TOP), _CXPGetStyle(BS_BOTTOM), DT_TOP, DT_BOTTOM, DT_VCENTER);

		_ExIf(_CXPGetState(CXPS_PRESSED), OffsetRect(&rtRect, 1, 1));
		_DrawText(tzText, _CXPGetState(CXPS_INDETERMINATE | CXPS_DISABLED) ? CXPR_DTEXT: CXPR_TEXT, i);
	}

	// 结束绘制
	_CXPEndDraw(hDC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制选框
VOID CXPDrawCheckBox(PCCLASSXP pCxp)
{
	UINT i;
	HDC hDC;
	RECT rtRect;
	PBYTE pbDib;
	TCHAR tzText[256];
	COLORREF crDib[CXPW_CHK * CXPH_CHK];

	// 开始绘制
	_CXPBeginDraw(hDC, TRUE, FALSE);

	// 选择图像
	if (_CXPGetState(CXPS_DISABLED))
	{
		i = 0;
	}
	else if (_CXPGetState(CXPS_PRESSED))
	{
		i = 1;
	}
	else if (_CXPGetState(CXPS_HOTLIGHT))
	{
		i = 2;
	}
	else
	{
		i = 3;
	}

	_ExIf(_CXPGetState(CXPS_CHECKED), i += 4);

	if ((_CXPGetStyle(BS_TYPEMASK) == BS_RADIOBUTTON) || (_CXPGetStyle(BS_TYPEMASK) == BS_AUTORADIOBUTTON))
	{
		pbDib = (PBYTE) crDib;
		CXPDibBlend(pbDib, (PBYTE) c_crCXPRadioBox[i], CXPW_CHK * CXPH_CHK);
	}
	else
	{
		_ExIf(_CXPGetState(CXPS_INDETERMINATE), i += 8);
		pbDib = (PBYTE) c_crCXPCheckBox[i];
	}

	// 绘制图象
	_DrawDIB(0, (rtRect.bottom - CXPH_CHK) / 2, CXPW_CHK, CXPH_CHK, pbDib, &c_biCXPCheck);

	// 绘制文字
	if (GetWindowText(pCxp->hWnd, tzText, sizeof(tzText)))
	{
		rtRect.left = (CXPW_CHK + 5);
		_DrawFocusText(tzText, _CXPGetState(CXPS_DISABLED) ? CXPR_DTEXT: CXPR_TEXT);
	}

	// 结束绘制
	_CXPEndDraw(hDC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
