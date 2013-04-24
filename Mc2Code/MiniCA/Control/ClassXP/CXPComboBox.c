


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制组合框
VOID CXPDrawComboBox(PCCLASSXP pCxp)
{
	UINT i;
	HDC hDC;
	RECT rtRect;
	PBYTE pbDib;
	BITMAPINFOHEADER biCmb;

	// 开始绘制
	_CXPBeginDraw(hDC, FALSE, TRUE);

	if (_CXPGetState(CXPS_SIMPLECMB))
	{
		rtRect.bottom = GetSystemMetrics(SM_CYVTHUMB) + 3;
	}

	// 绘制边框
	_FrameRect(_CXPGetState(CXPS_DISABLED) ? CXPR_DFRAME : CXPR_FRAME);
	InflateRect(&rtRect, -1, -1);
	_FrameRect(_CXPGetState(CXPS_DISABLED) ? CXPR_DWINDOW : CXPR_WINDOW);

	if (!_CXPGetState(CXPS_SIMPLECMB))
	{
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

		// 清除下拉按钮
		rtRect.left = rtRect.right - GetSystemMetrics(SM_CXHTHUMB) - 1;
		_FillRect(_CXPGetState(CXPS_DISABLED) ? CXPR_CANVAS : GetSysColor(COLOR_WINDOW))

		// 绘制图象
		biCmb = c_biCXPCheck;
		biCmb.biWidth = CXPW_CMB;
		biCmb.biHeight = rtRect.bottom - 3;
		biCmb.biSizeImage = biCmb.biWidth * biCmb.biHeight * 4;
		pbDib = (PBYTE) _HeapAlloc(biCmb.biSizeImage);
		if (pbDib)
		{
			CXPDibBlendVertExt((PBYTE) pbDib, biCmb.biHeight, (PBYTE) c_crCXPComboBox[i], CXPW_CMB, CXPH_CMB);
			_DrawDIB(rtRect.right - biCmb.biWidth - 1, 2, biCmb.biWidth, biCmb.biHeight, pbDib, &biCmb);
			_HeapFree(pbDib);
		}

		// 绘制箭头
		i = _CXPGetState(CXPS_DISABLED) ? CXPR_DARROW : CXPR_ARROW;
		CXPDrawArrow(hDC, (rtRect.right - (biCmb.biWidth / 2) - 2), rtRect.bottom / 2, CXPA_LARGEDOWN, i);
	}

	// 结束绘制
	_CXPEndDraw(hDC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
