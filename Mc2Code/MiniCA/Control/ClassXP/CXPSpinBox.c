


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ����
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������ת�ؼ�
VOID CXPDrawSpinBox(PCCLASSXP pCxp)
{
	UINT x;
	UINT y;
	UINT i;
	UINT j;
	HDC hDC;
	RECT rtRect;
	PBYTE pbDib;
	BITMAPINFOHEADER biCmb;

	// ��ʼ����
	_CXPBeginDraw(hDC, FALSE, TRUE);

	// �������
	if (pCxp->lParam)
	{
		_FrameRect(_CXPGetState(CXPS_DISABLED) ? CXPR_DFRAME : CXPR_FRAME);
		InflateRect(&rtRect, -1, -1);

		x = (_CXPGetState(CXPS_LEFTSPIN)) ? rtRect.right : 0;
		i = (((PCLASSXP) pCxp->lParam)->lState & (CXPS_DISABLED | CXPS_READONLY)) ? CXPR_DWINDOW : CXPR_WINDOW;
		_DrawLine(x, rtRect.top, x, rtRect.bottom, i);
	}

	// �����ڿ�
	_FillRect(GetSysColor(COLOR_WINDOW));
	InflateRect(&rtRect, -1, -1);

	// ѡ��ͼ��
	if (_CXPGetState(CXPS_DISABLED))
	{
		i = 0;
		j = 0;
	}
	else
	{
		if (_CXPGetState(CXPS_PRESSED))
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

		if (_CXPGetState(CXPS_FIRSTPART))
		{
			j = 3;
		}
		else
		{
			j = i;
			i = 3;
		}
	}

	// ����ͼ��
	biCmb = c_biCXPCheck;

	if (_CXPGetState(CXPS_HORIZON))
	{
		x = _RectWidth(rtRect);
		biCmb.biWidth = x / 2;
		biCmb.biHeight = _RectHeight(rtRect);
		x = rtRect.left + biCmb.biWidth + (x & 1);
		y = rtRect.top;
	}
	else
	{
		y = _RectHeight(rtRect);
		biCmb.biWidth = _RectWidth(rtRect);
		biCmb.biHeight = y / 2;
		x = rtRect.left;
		y = rtRect.top + biCmb.biHeight + (y & 1);
	}

	biCmb.biSizeImage = biCmb.biWidth * biCmb.biHeight * 4;
	pbDib = (PBYTE) _HeapAlloc(biCmb.biSizeImage);
	if (pbDib)
	{
		CXPDibBlendExt((PBYTE) pbDib, biCmb.biWidth, biCmb.biHeight, (PBYTE) c_crCXPComboBox[i], CXPW_CMB, CXPH_CMB);
		_DrawDIB(rtRect.left, rtRect.top, biCmb.biWidth, biCmb.biHeight, pbDib, &biCmb);

		CXPDibBlendExt((PBYTE) pbDib, biCmb.biWidth, biCmb.biHeight, (PBYTE) c_crCXPComboBox[j], CXPW_CMB, CXPH_CMB);
		_DrawDIB(x, y, biCmb.biWidth, biCmb.biHeight, pbDib, &biCmb);

		_HeapFree(pbDib);
	}

	// ���Ƽ�ͷ
	i = _CXPGetState(CXPS_DISABLED) ? CXPR_DARROW : CXPR_ARROW;
	x = rtRect.left + (biCmb.biWidth / 2);
	y = rtRect.top + (biCmb.biHeight / 2);
	if (_CXPGetState(CXPS_HORIZON))
	{
		CXPDrawArrow(hDC, x, y, CXPA_SMALLLEFT, i);
		x += biCmb.biWidth;
		CXPDrawArrow(hDC, x, y, CXPA_SMALLRIGHT, i);
	}
	else
	{
		CXPDrawArrow(hDC, x, y, CXPA_SMALLUP, i);
		y += biCmb.biHeight;
		CXPDrawArrow(hDC, x, y, CXPA_SMALLDOWN, i);
	}

	// ��������
	_CXPEndDraw(hDC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
