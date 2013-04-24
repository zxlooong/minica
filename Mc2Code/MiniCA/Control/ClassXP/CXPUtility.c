


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 箭头数据
POINT c_ptCXPArrow[][10] =
{
	{{-2, +0}, {-2, +1}, {-1, -1}, {-1, +2}, {+0, -2}, {+0, +3}, {+1, -3}, {+1, +0}, {+1, +1}, {+1, +4}},
	{{+2, +0}, {+2, +1}, {+1, -1}, {+1, +2}, {+0, -2}, {+0, +3}, {-1, -2}, {-1, +0}, {-1, +1}, {-1, +4}},
	{{-3, +1}, {+0, +1}, {+1, +1}, {+4, +1}, {-2, +0}, {+3, +0}, {-1, -1}, {+2, -1}, {+0, -2}, {+1, -2}},
	{{-3, -1}, {+0, -1}, {+1, -1}, {+4, -1}, {-2, +0}, {+3, +0}, {-1, +1}, {+2, +1}, {+0, +2}, {+1, +2}},

	{{+1, -4}, {-3, +0}, {+2, +5}, {+1, -3}, {-2, +0}, {+2, +4}, {+2, -3}, {-1, +0}, {+3, +4}, {+0, +0}},
	{{-1, -4}, {+3, +0}, {-2, +5}, {-1, -3}, {+2, +0}, {-2, +4}, {-2, -3}, {+1, +0}, {-3, +4}, {+0, +0}},
	{{-4, +1}, {+0, -3}, {+5, +2}, {-3, +1}, {+0, -2}, {+4, +2}, {-3, +2}, {+0, -1}, {+4, +3}, {+0, +0}},
	{{-4, -1}, {+0, +3}, {+5, -2}, {-3, -1}, {+0, +2}, {+4, -2}, {-3, -2}, {+0, +1}, {+4, -3}, {+0, +0}},
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 追踪鼠标移出消息
BOOL CXPTrackMouseLeave(HWND hWnd)
{
	typedef BOOL (WINAPI *TMEPROC)(LPTRACKMOUSEEVENT);
	static TMEPROC s_pTrackMouseEvent = NULL;
	static TRACKMOUSEEVENT tmeLeave = {sizeof(TRACKMOUSEEVENT), TME_LEAVE};

	if (s_pTrackMouseEvent == NULL)
	{
		s_pTrackMouseEvent = (TMEPROC) GetProcAddress(GetModuleHandle(TEXT("USER32")), "TrackMouseEvent");
		if (s_pTrackMouseEvent == NULL)
		{
			s_pTrackMouseEvent = (TMEPROC) GetProcAddress(GetModuleHandle(TEXT("COMCTL32")), "_TrackMouseEvent");
		}
	}

	tmeLeave.hwndTrack = hWnd;
	return s_pTrackMouseEvent ? s_pTrackMouseEvent(&tmeLeave) : FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘制箭头
VOID CXPDrawArrow(HDC hDC, UINT x, UINT y, UINT uDir, COLORREF crColor)
{
	HPEN hPen;

	hPen = (HPEN) SelectObject(hDC, CreatePen(PS_SOLID, 1, crColor));

	if (uDir < CXPA_LARGELEFT)
	{
		MoveToEx(hDC, x + c_ptCXPArrow[uDir][0].x, y + c_ptCXPArrow[uDir][0].y, NULL);
		LineTo(hDC, x+ c_ptCXPArrow[uDir][1].x, y + c_ptCXPArrow[uDir][1].y);
		MoveToEx(hDC, x+ c_ptCXPArrow[uDir][2].x, y + c_ptCXPArrow[uDir][2].y, NULL);
		LineTo(hDC, x+ c_ptCXPArrow[uDir][3].x, y + c_ptCXPArrow[uDir][3].y);
		MoveToEx(hDC, x+ c_ptCXPArrow[uDir][4].x, y + c_ptCXPArrow[uDir][4].y, NULL);
		LineTo(hDC, x+ c_ptCXPArrow[uDir][5].x, y + c_ptCXPArrow[uDir][5].y);
		MoveToEx(hDC, x+ c_ptCXPArrow[uDir][6].x, y + c_ptCXPArrow[uDir][6].y, NULL);
		LineTo(hDC, x+ c_ptCXPArrow[uDir][7].x, y + c_ptCXPArrow[uDir][7].y);
		MoveToEx(hDC, x+ c_ptCXPArrow[uDir][8].x, y + c_ptCXPArrow[uDir][8].y, NULL);
		LineTo(hDC, x+ c_ptCXPArrow[uDir][9].x, y + c_ptCXPArrow[uDir][9].y);
	}
	else
	{
		MoveToEx(hDC, x + c_ptCXPArrow[uDir][0].x, y + c_ptCXPArrow[uDir][0].y, NULL);
		LineTo(hDC, x + c_ptCXPArrow[uDir][1].x, y + c_ptCXPArrow[uDir][1].y);
		LineTo(hDC, x + c_ptCXPArrow[uDir][2].x, y + c_ptCXPArrow[uDir][2].y);
		MoveToEx(hDC, x + c_ptCXPArrow[uDir][3].x, y  + c_ptCXPArrow[uDir][3].y, NULL);
		LineTo(hDC, x + c_ptCXPArrow[uDir][4].x, y + c_ptCXPArrow[uDir][4].y);
		LineTo(hDC, x + c_ptCXPArrow[uDir][5].x, y + c_ptCXPArrow[uDir][5].y);
		MoveToEx(hDC, x + c_ptCXPArrow[uDir][6].x, y + c_ptCXPArrow[uDir][6].y, NULL);
		LineTo(hDC, x + c_ptCXPArrow[uDir][7].x, y + c_ptCXPArrow[uDir][7].y);
		LineTo(hDC, x + c_ptCXPArrow[uDir][8].x, y + c_ptCXPArrow[uDir][8].y);
	}

	DeleteObject(SelectObject(hDC, hPen));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 位图混合
VOID CXPDibBlend(PBYTE pbDst, PBYTE pbSrc, UINT uPixel)
{
	UINT i;
	BYTE r[4];

	*((PDWORD) r) = CXPR_CANVAS;
	for (i = uPixel; i; i--)
	{
		pbDst[0] = pbSrc[0] * pbSrc[3] / 255 + r[2] - r[2] * pbSrc[3] / 255;
		pbDst[1] = pbSrc[1] * pbSrc[3] / 255 + r[1] - r[1] * pbSrc[3] / 255;
		pbDst[2] = pbSrc[2] * pbSrc[3] / 255 + r[0] - r[0] * pbSrc[3] / 255;
		pbDst[3] = 0;

		pbDst += 4;
		pbSrc += 4;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 位图混合水平扩展
VOID CXPDibBlendHorzExt(PBYTE pbDst, UINT uDstWidth, PBYTE pbSrc, UINT uSrcWidth, UINT uHeight)
{
	INT x;
	INT y;
	DWORD dwColor;
	UINT uHalfWidth;

	uHalfWidth = (min(uDstWidth, uSrcWidth) + 1) / 2;
	for (y = uHeight; y; y--)
	{
		CXPDibBlend(pbDst, pbSrc, uHalfWidth);
		pbSrc += (uSrcWidth - uHalfWidth) * 4;
		pbDst += (uDstWidth - uHalfWidth) * 4;
		CXPDibBlend(pbDst, pbSrc, uHalfWidth);

		dwColor = *((PDWORD) pbDst);
		for (x = 0; x < (INT) (uDstWidth - uSrcWidth); x++)
		{
			pbDst -= 4;
			*((PDWORD) pbDst) = dwColor;
		}

		pbSrc += uHalfWidth * 4;
		pbDst += uHalfWidth * 4 + x * 4;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 位图混合垂直扩展
VOID CXPDibBlendVertExt(PBYTE pbDst, UINT uDstHeight, PBYTE pbSrc, UINT uWidth, UINT uSrcHeight)
{
	INT y;
	UINT uStride;
	UINT uHalfHeight;

	uStride = uWidth * 4;
	uHalfHeight = (min(uDstHeight, uSrcHeight) + 1) / 2;

	CXPDibBlend(pbDst, pbSrc, uWidth * uHalfHeight);
	pbSrc += uStride * (uSrcHeight  - uHalfHeight);
	pbDst += uStride * (uDstHeight  - uHalfHeight);
	CXPDibBlend(pbDst, pbSrc, uWidth * uHalfHeight);

	pbSrc = pbDst;
	for (y = uDstHeight - uSrcHeight; y > 0; y--)
	{
		pbDst -= uStride;
		CopyMemory(pbDst, pbSrc, uStride);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 背景混合扩展
VOID CXPDibBlendExt(PBYTE pbDst, UINT uDstWidth, UINT uDstHeight, PBYTE pbSrc, UINT uSrcWidth, UINT uSrcHeight)
{
	INT y;
	UINT uHalfHeight;

	if (uDstHeight < uSrcHeight)
	{
		y = 0;
	}

	uHalfHeight = (min(uDstHeight, uSrcHeight) + 1) / 2;

	CXPDibBlendHorzExt(pbDst, uDstWidth, pbSrc, uSrcWidth, uHalfHeight);
	pbSrc += uSrcWidth * 4 * (uSrcHeight - uHalfHeight);
	pbDst += uDstWidth * 4 * (uDstHeight - uHalfHeight);
	CXPDibBlendHorzExt(pbDst, uDstWidth, pbSrc, uSrcWidth, uHalfHeight);

	pbSrc = pbDst;
	for (y = uDstHeight - uSrcHeight; y > 0; y--)
	{
		pbDst -= uDstWidth * 4;
		CopyMemory(pbDst, pbSrc, uDstWidth * 4);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
