

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#include "Language.h"
#include "../Resource.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 语言文件节名和键名
#define LIN_Language			TEXT("Language")
#define LIN_Description			TEXT("Description")
#define LIN_FontName			TEXT("FontName")
#define LIN_FontSize			TEXT("FontSize")
#define LIN_Text				TEXT("Text")
#define LIN_String				TEXT("String")

#define EXT_Lng					TEXT(".lng")

#define _NumberOf(v)			(sizeof(v) / sizeof(v[0]))
#define _LengthOf(s)			(_NumberOf(s) - 1)

#define _StrEnd(t)				(t + lstrlen(t))
#ifdef _UNICODE
#define _StrStr(t1, t2)			wcsstr(t1, t2)
#define _StrChr(t, c)			wcschr(t, c)
#define _StrRChr(t, c)			wcsrchr(t, c)
#else if // _UNICODE
#define _StrStr(t1, t2)			strstr(t1, t2)
#define _StrChr(t, c)			strchr(t, c)
#define _StrRChr(t, c)			strrchr(t, c)
#endif // _UNICODE

#define _WStrToAStrN(a, w, n)	WideCharToMultiByte(CP_ACP, 0, w, -1, a, n, NULL, NULL)
#define _AStrToWStrN(w, a, n)	MultiByteToWideChar(CP_ACP, 0, a, -1, w, n)

#ifdef _UNICODE
#define _WStrToStrN(t, w, n)	lstrcpyn(t, w, n)
#else // _UNICODE
#define _WStrToStrN(t, w, n)	_WStrToAStrN(t, w, n)
#endif // _UNICODE
#define _WStrToStr(t, w)		_WStrToStrN(t, w, _NumberOf(t))

#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(r)		(((ULONG) (r) >> 16) == 0)
#endif
#ifndef CDSIZEOF_STRUCT
#define CDSIZEOF_STRUCT(s, m)	(((INT) ((PBYTE) (&((s *) 0)->m) - ((PBYTE) ((s *) 0)))) + sizeof(((s *) 0)->m))
#endif

extern HINSTANCE g_hInst;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLanguage 类静态变量
UINT CLanguage::m_uLang = IDM_View_Default;
UINT CLanguage::m_uMax = IDM_View_Default;
HFONT CLanguage::m_hFont = NULL;
TCHAR CLanguage::m_tzText[1024] = {0};
TCHAR CLanguage::m_tzFileName[MAX_PATH] = {0};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 初始化语言
LANGID CLanguage::Initialize()
{
	HANDLE hFind;
	LANGID liLang;
	LANGID liResult;
	WIN32_FIND_DATA fdFind;
	TCHAR tzPath[MAX_PATH];

	// 获取语言标识
	liResult = 0;
	
	// 下列配置文件名为临时代码，根据需要更改
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(tzPath + lstrlen(tzPath) - 4, TEXT(".ini"));

	// 从配置文件中获取语言设置
	liLang = GetPrivateProfileInt(TEXT("LANG"), LIN_Language, GetUserDefaultLangID(), tzPath);
	if (liLang)
	{
		// 查找语言文件
		GetModuleFileName(NULL, tzPath, MAX_PATH);
		lstrcpy(_StrRChr(tzPath, '\\') + 1, TEXT("*") EXT_Lng);
		hFind = FindFirstFile(tzPath, &fdFind);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				// 如果是指定的语言
				lstrcpy(_StrRChr(tzPath, '\\') + 1, fdFind.cFileName);
				if (liLang == GetPrivateProfileInt(LIN_Language, LIN_Language, 0, tzPath))
				{
					// 设置语言文件名
					liResult = liLang;
					lstrcpy(m_tzFileName, tzPath);
					Set(NULL, liResult);
					break;
				}
			}
			while (FindNextFile(hFind, &fdFind));
			FindClose(hFind);
		}
	}

	return liResult;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 列出语言
UINT CLanguage::List(HMENU hMenu)
{
	HANDLE hFind;
	LANGID liLang;
	LANGID liTemp;
	WIN32_FIND_DATA fdFind;
	TCHAR tzPath[MAX_PATH];

	// 下列配置文件名为临时代码，根据需要更改
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(tzPath + lstrlen(tzPath) - 4, TEXT(".ini"));

	// 从配置文件中获取语言设置
	liLang = GetPrivateProfileInt(TEXT("LANG"), LIN_Language, 0, tzPath);
	//liLang = CIni::GetInt(INI_Language);

	// 查找语言文件	
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(_StrRChr(tzPath, '\\') + 1, TEXT("*") EXT_Lng);
	hFind = FindFirstFile(tzPath, &fdFind);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 获取语言标识，判断是否为有效
			lstrcpy(_StrRChr(tzPath, '\\') + 1, fdFind.cFileName);
			liTemp = GetPrivateProfileInt(LIN_Language, LIN_Language, 0, tzPath);
			if (liTemp)
			{
				// 第一次添加，插入分隔符；大于 50 条，跳出
				if (m_uMax == IDM_View_Default)
				{
					AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
				}
				else if (m_uMax >= IDM_View_Default + 50)
				{
					break;
				}

				// 添加菜单项
				m_uMax++;
				fdFind.cFileName[lstrlen(fdFind.cFileName) - _LengthOf(EXT_Lng)] = 0;
				AppendMenu(hMenu, MF_BYCOMMAND, m_uMax, fdFind.cFileName);
				if (liTemp == liLang)
				{
					// 记录指定语言
					m_uLang = m_uMax;
					lstrcpy(m_tzFileName, tzPath);

				}
			}
		}
		while (FindNextFile(hFind, &fdFind));
		FindClose(hFind);
	}

	// 选择菜单项
	CheckMenuRadioItem(hMenu, IDM_View_Default, m_uMax, m_uLang, MF_BYCOMMAND);	//5309 IDM_View_Default

	return m_uLang;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 设置语言
VOID CLanguage::Set(HMENU hMenu, UINT uLang)
{
//	HDC hDC;
	LANGID liLang;
//	LOGFONT lfFont;
	TCHAR tzName[MAX_PATH];

	// 如果是初始化中设置语言
	if (hMenu == NULL)
	{
		liLang = uLang;
	}
	else
	{
		if((uLang <= IDM_View_Default) || (uLang > m_uMax))
		{
			// 切换到默认语言
			liLang = 0;
			m_tzFileName[0] = 0;
			m_uLang = IDM_View_Default;
		}
		else
		{
			// 切换到其它语言
			m_uLang = uLang;
			GetModuleFileName(NULL, m_tzFileName, MAX_PATH);
			GetMenuString(hMenu, uLang, tzName, MAX_PATH, MF_BYCOMMAND);
			wsprintf(_StrRChr(m_tzFileName, '\\'), TEXT("\\%s%s"), tzName, EXT_Lng);
			liLang = GetPrivateProfileInt(LIN_Language, LIN_Language, 0, m_tzFileName);
		}

		// 选择菜单项
		CheckMenuRadioItem(hMenu, IDM_View_Default, m_uMax, m_uLang, MF_BYCOMMAND);
	}

	// 下列配置文件名为临时代码，根据需要更改
	TCHAR tzPath[MAX_PATH];
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(tzPath + lstrlen(tzPath) - 4, TEXT(".ini"));

	// 保存语言标识到配置文件中
	wsprintf(tzName, TEXT("%d"), liLang);
	liLang = WritePrivateProfileString(TEXT("LANG"), LIN_Language, tzName, tzPath);
	//CIni::SetInt(INI_Language, liLang);

	// 创建字体
/*	Destroy();
	ZeroMemory(&lfFont, sizeof(LOGFONT));
	if (GetPrivateProfileString(LIN_Language, LIN_FontName, NULL, lfFont.lfFaceName, LF_FACESIZE, m_tzFileName))
	{
		lfFont.lfCharSet = DEFAULT_CHARSET;
		lfFont.lfHeight = GetPrivateProfileInt(LIN_Language, LIN_FontSize, 0, m_tzFileName);
		if (lfFont.lfHeight)
		{
			hDC = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
			lfFont.lfHeight = -MulDiv(lfFont.lfHeight, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			DeleteDC(hDC);
		}
		m_hFont = CreateFontIndirect(&lfFont);
	}*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 从菜单中获取语言文件名
PTSTR CLanguage::GetDescription(HMENU hMenu, UINT uLang)
{
	TCHAR tzName[MAX_PATH];

	GetModuleFileName(NULL, m_tzText, MAX_PATH);
	GetMenuString(hMenu, uLang, tzName, MAX_PATH, MF_BYCOMMAND);
	wsprintf(_StrRChr(m_tzText, '\\'), TEXT("\\%s%s"), tzName, EXT_Lng);
	if (GetPrivateProfileString(LIN_Language, LIN_Description, NULL, m_tzText, _NumberOf(m_tzText), m_tzText) == 0)
	{
		TranslateString(IDM_View_FirstLang);
	}

	return m_tzText;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取常规字符串
CString CLanguage::TranslateText(PCTSTR ptzName, PCTSTR ptzDefault)
{
#ifdef _MAKELANG
	WritePrivateProfileString(LIN_Text, ptzName, ptzDefault, m_tzFileName);
	return (PTSTR) ptzDefault;
#else // _MAKELANG
	CString str;
	if ((m_tzFileName[0] == 0) ||
		(GetPrivateProfileString(LIN_Text, ptzName, NULL, m_tzText, _NumberOf(m_tzText), m_tzFileName) == 0))
	{
		str.Format("%s", ptzDefault);
	}
	else
		str.Format("%s", m_tzText);
	return str;
#endif // _MAKELANG
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取常规字符串，并替换特殊字符
PTSTR CLanguage::TranslateTextEx(PCTSTR ptzName, PCTSTR ptzDefault)
{
#ifdef _MAKELANG
	// 判断是否有两个空字符
	BOOL bDoubleNull = FALSE;
	CopyMemory(m_tzText, ptzDefault, sizeof(m_tzText));
	for (UINT i = 0; i < _LengthOf(m_tzText); i++)
	{
		if ((m_tzText[i] == 0) && (m_tzText[i + 1] == 0))
		{
			bDoubleNull = TRUE;
			break;
		}
	}

	for (PTSTR p = m_tzText; ; p++)
	{
		if (*p == '\n')
		{
			*p = '~';
		}
		else if (*p == 0)
		{
			if (bDoubleNull)
			{
				*p = '`';
				if (*(p + 1) == 0)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	WritePrivateProfileString(LIN_Text, ptzName, m_tzText, m_tzFileName);
	return (PTSTR) ptzDefault;
#else // _MAKELANG
	if ((m_tzFileName[0] == 0) ||
		(GetPrivateProfileString(LIN_Text, ptzName, NULL, m_tzText, _NumberOf(m_tzText), m_tzFileName) == 0))
	{
		return (PTSTR) ptzDefault;
	}
	for (PTSTR p = m_tzText; *p; p++)
	{
		if (*p == '~')
		{
			*p = '\n';
		}
		else if (*p == '`')
		{
			*p = 0;
		}
	}
	return m_tzText;
#endif // _MAKELANG
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取资源字符串
PTSTR CLanguage::TranslateString(UINT uResID)
{
	TCHAR tzName[32];

	wsprintf(tzName, TEXT("%u"), uResID);

#ifdef _MAKELANG
	LoadString(g_hInst, uResID, m_tzText, _NumberOf(m_tzText));
	if (m_tzText[0])
	{
		WritePrivateProfileString(LIN_String, tzName, m_tzText, m_tzFileName);
	}
#else // _MAKELANG
	if ((m_tzFileName[0] == 0) ||
		(GetPrivateProfileString(LIN_String, tzName, NULL, m_tzText, _NumberOf(m_tzText), m_tzFileName) == 0))
	{
		if (LoadString(g_hInst, uResID, m_tzText, _NumberOf(m_tzText)) == 0)
		{
			return NULL;
		}
	}
#endif // _MAKELANG

	return m_tzText;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新菜单
VOID CLanguage::TranslateMenu(HMENU hMenu, PCTSTR ptzResName)
{
	TCHAR tzSection[32];

	if (m_tzFileName[0])
	{
		// 获取节名
		if (IS_INTRESOURCE(ptzResName))
		{
			wsprintf(tzSection, TEXT("%u"), ptzResName);
		}
		else
		{
			lstrcpy(tzSection, ptzResName);
		}

		// 从语言文件中更新菜单
		UpdateMenuFromLanguage(hMenu, tzSection);
	}
	else
	{
		// 从资源中更新菜单
		UpdateMenuFromResource(hMenu, ptzResName);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 更新菜单BCMenu
VOID CLanguage::TranslateMenu(BCMenu * pBMenu, PCTSTR ptzResName)
{
	TCHAR tzSection[32];
	
	if (m_tzFileName[0])
	{
		// 获取节名
		if (IS_INTRESOURCE(ptzResName))
		{
			wsprintf(tzSection, TEXT("%u"), ptzResName);
		}
		else
		{
			lstrcpy(tzSection, ptzResName);
		}
		
		// 从语言文件中更新菜单
		UpdateMenuFromLanguage(pBMenu, tzSection);
	}
	else
	{
		// 从资源中更新菜单
//		UpdateBMenuFromResource(hMenu, ptzResName);
	}
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新对话框
VOID CLanguage::TranslateDialog(HWND hWnd, PCTSTR ptzResName)
{
	TCHAR tzSection[32];

	if (m_tzFileName[0])	//如果语言文件存在
	{
		// 获取节名
		if (IS_INTRESOURCE(ptzResName))
		{
			wsprintf(tzSection, TEXT("%u"), ptzResName);
		}
		else
		{
			lstrcpy(tzSection, ptzResName);
		}

		// 修改对话框字符串
		UpdateDialogFromLanguage(hWnd, tzSection);

#ifndef _TRANSRECUR
		// 枚举并修改子窗口字符串
		EnumChildWindows(hWnd, (WNDENUMPROC) UpdateDialogFromLanguage, (LPARAM) tzSection);
#endif // _TRANSRECUR
	}
	else
	{
//		UpdateDialogFromResource(hWnd, ptzResName);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 设置菜单字符串
BOOL CLanguage::SetMenuString(HMENU hMenu, UINT uItemID, PCTSTR ptzString, BOOL bByPosition)
{
	MENUITEMINFO miiItem;

	miiItem.cbSize = CDSIZEOF_STRUCT(MENUITEMINFO, cch);
#if (_WINVER >= 0x0410)
	miiItem.fMask = MIIM_STRING;
#else (_WINVER >= 0x0410)
	miiItem.fMask = MIIM_TYPE;
	miiItem.cch = 0;
	miiItem.dwTypeData = NULL;
	GetMenuItemInfo(hMenu, uItemID, bByPosition, &miiItem);
#endif (_WINVER >= 0x0410)
	miiItem.dwTypeData = (PTSTR) ptzString;

	return SetMenuItemInfo(hMenu, uItemID, bByPosition, &miiItem);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 从语言文件中更新菜单
VOID CLanguage::UpdateMenuFromLanguage(HMENU hMenu, PCTSTR ptzSection, PCTSTR ptzPath)
{
	INT i;
	INT iNum;
	TCHAR tzName[256];
	TCHAR tzText[1024];
	MENUITEMINFO miiItem;

	// 初始化变量，获取菜单项数量
	miiItem.cbSize = CDSIZEOF_STRUCT(MENUITEMINFO, cch);
	miiItem.fMask = MIIM_SUBMENU | MIIM_ID;
	iNum = GetMenuItemCount(hMenu);
	for (i = 0; i < iNum; i++)
	{
		// 获取弹出菜单句柄和标识
		GetMenuItemInfo(hMenu, i, TRUE, &miiItem);
		if (miiItem.wID)
		{
			// 获取菜单标识文本
			if (miiItem.hSubMenu)
			{
				/*_Assert(ptzPath);
				_Assert(lstrlen(ptzPath) < _NumberOf(tzName) - 16);*/
				wsprintf(tzName, TEXT("%s|%u"), ptzPath, i);
			}
			else
			{
				wsprintf(tzName, TEXT("%u"), miiItem.wID);
			}

	#ifdef _MAKELANG
			// 生成语言文件，递归修改菜单字符串
			if ((miiItem.wID <= IDM_View_Default) || (miiItem.wID >= IDM_View_Default + 50) &&
				(miiItem.wID <= IDM_File_Recent) || (miiItem.wID >= IDM_File_Recent + 50))
			{
				GetMenuString(hMenu, i, tzText, _NumberOf(tzText), MF_BYPOSITION);
				WritePrivateProfileString(ptzSection, tzName, tzText, m_tzFileName);
			}
	#else // _MAKELANG
			// 设置菜单字符串
			if (GetPrivateProfileString(ptzSection, tzName, NULL, tzText, _NumberOf(tzText), m_tzFileName))
			{
				SetMenuString(hMenu, i, tzText, TRUE);
			}
	#endif // _MAKELANG

			// 递归修改菜单字符串
			if (miiItem.hSubMenu)
			{
				UpdateMenuFromLanguage(miiItem.hSubMenu, ptzSection, tzName);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 从语言文件中更新菜单
VOID CLanguage::UpdateMenuFromLanguage(BCMenu * pBMenu, PCTSTR ptzSection, PCTSTR ptzPath)
{
	//得到子菜單
	CMenu * pTempMenu = pBMenu->GetSubMenu(0);
	if (pTempMenu)
	{
		UINT uNum = pTempMenu->GetMenuItemCount();
		for(int i = 0; i < uNum; i++)
		{
			UINT uMenuID = pTempMenu->GetMenuItemID(i);
			CString strName;
			strName.Format("%u", uMenuID);
			TCHAR tzText[1024];
			if (GetPrivateProfileString(ptzSection, strName, NULL, tzText, _NumberOf(tzText), m_tzFileName))
			{
				pBMenu->ModifyODMenu(tzText, uMenuID);
			}

		}
		
	}
}////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 从资源中更新菜单
VOID CLanguage::UpdateMenuFromResource(HMENU hMenu, PCTSTR ptzResName)
{
	PBYTE p;
	PBYTE pbEnd;
	BOOL bMenuEx;
	WORD wOptions;
	DWORD dwMenuID;
	HRSRC hResource;
	HGLOBAL hGlobal;
	TCHAR tzText[1024];

	// 菜单层次堆栈，用于更新弹出菜单字符串
	INT i = 0;					// 层次
	INT iPos[32] = {0};			// 位置
	BOOL bHilite[32] = {FALSE};	// 回退
	HMENU hMenus[32] = {hMenu};	// 句柄

	// 载入资源
	hResource = FindResource(g_hInst, ptzResName, RT_MENU);
	if(!hResource)
		return;
	//_Assert(hResource);
	hGlobal = LoadResource(g_hInst, hResource);
	if(!hGlobal)
		return;
	//_Assert(hGlobal);

	// 获取资源的起始地址、终止地址和版本
	p = (PBYTE) LockResource(hGlobal);
	pbEnd = p + SizeofResource(g_hInst, hResource);
	bMenuEx = ((PMENUITEMTEMPLATEHEADER) p)->versionNumber;

	// 转到菜单项数据
	p += ((PMENUITEMTEMPLATEHEADER) p)->offset + sizeof(MENUITEMTEMPLATEHEADER);
	if (bMenuEx)
	{
		p += sizeof(DWORD);
	}

	// 更新菜单
	while (p < pbEnd)
	{
		// 获取菜单项标识、参数
		if (bMenuEx)
		{
			p += 3 * sizeof(DWORD);
			dwMenuID =  *((PDWORD) p);
			p += sizeof(DWORD) + sizeof(WORD);
		}
		else
		{
			wOptions = *((PWORD) p);
			p += sizeof(WORD);
			if ((wOptions & MF_POPUP) == FALSE)
			{
				dwMenuID = *((PWORD) p);
				p += sizeof(WORD);
			}
		}

		// 获取菜单项文本，转到下一个菜单项
		for (_WStrToStr(tzText, (PWSTR) p); *((PWSTR) p); p += sizeof(WCHAR));
		p += sizeof(WCHAR);

		if (bMenuEx)
		{
			// 根据命令标识设置菜单文本
			SetMenuString(hMenu, dwMenuID, tzText);
		}
		else
		{
			// 如果是弹出菜单项
			//_Assert(i < _NumberOf(iPos));
			iPos[i]++;
			if (wOptions & MF_POPUP)
			{
				// 根据位置设置菜单文本
				SetMenuString(hMenus[i], iPos[i] - 1, tzText, TRUE);

				// 获取下一级菜单，并把相关数据压入堆栈
				bHilite[i] = wOptions & MF_HILITE;
				hMenus[i + 1] = GetSubMenu(hMenus[i], iPos[i] - 1);
				i++;
				iPos[i] = 0;
				//_Assert(hMenus[i]);
			}
			else
			{
				// 根据命令标识设置菜单文本
				SetMenuString(hMenu, dwMenuID, tzText);

				if (wOptions & MF_HILITE)
				{
					// 弹出堆栈，如果已经完成则退出
					for (i--; (i >= 0) && bHilite[i]; i--);
					if (i < 0)
					{
						break;
					}
				}
			}
		}
	}

	FreeResource(hGlobal);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 从语言文件中更新对话框
BOOL CALLBACK CLanguage::UpdateDialogFromLanguage(HWND hWnd, PCTSTR ptzSection)
{
	TCHAR tzText[1024];
	TCHAR tzName[MAX_PATH];

	// 获取窗口标识字符串
	wsprintf(tzName, TEXT("%u"), GetDlgCtrlID(hWnd));

	// 设置窗口标题
#ifdef _MAKELANG
	GetWindowText(hWnd, tzText, _NumberOf(tzText));
	if (tzText[0] && ((tzText[0] < '0') || (tzText[0] > '9')))
	{
		WritePrivateProfileString(ptzSection, tzName, tzText, m_tzFileName);
	}
#else // _MAKELANG
	if (GetPrivateProfileString(ptzSection, tzName, NULL, tzText, _NumberOf(tzText), m_tzFileName))
	{
		SetWindowText(hWnd, tzText);
	}
#endif // _MAKELANG

#ifdef _TRANSRECUR
	// 枚举并修改子窗口字符串
	EnumChildWindows(hWnd, (WNDENUMPROC) UpdateDialogFromLanguage, (LPARAM) ptzSection);
#endif // _TRANSRECUR

	if (m_hFont)
	{
		SendMessage(hWnd, WM_SETFONT, (WPARAM) m_hFont, FALSE);
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 从资源中更新对话框
VOID CLanguage::UpdateDialogFromResource(HWND hWnd, PCTSTR ptzResName)
{
/*	PBYTE p;
	PBYTE pbEnd;
	BOOL bMenuEx;
	WORD wOptions;
	DWORD dwMenuID;
	HRSRC hResource;
	HGLOBAL hGlobal;
	TCHAR tzText[1024];

	CRes m_Res(g_hInst);
	res.FindResource(IDD_PROPPAGE_CAINI, RT_DIALOG);
	res.LoadAllResource();



	if (!m_Res.FindResource (IDD_PROPPAGE_CAINI, RT_DIALOG))
        return ;
    if (!m_Res.LoadResource ())
        return ;
	m_Res.LoadAllResource ();
	int n = m_Res.m_Infos.GetSize ();
    for ( int i =0 ;i< n ;i++)
    {
		 __ResInfo rest = m_Res.m_Infos.GetAt(i);
    }




// 载入资源
	hResource = FindResource(g_hInst,);
	if(!hResource)
		return;
	//_Assert(hResource);
	hGlobal = LoadResource(g_hInst, hResource);
	if(!hGlobal)
		return;
	//_Assert(hGlobal);

	// 获取资源的起始地址、终止地址和版本
	p = (PBYTE) LockResource(hGlobal);
	pbEnd = p + SizeofResource(g_hInst, hResource);
	bMenuEx = ((PMENUITEMTEMPLATEHEADER) p)->versionNumber;

	// 转到菜单项数据
	p += ((PMENUITEMTEMPLATEHEADER) p)->offset + sizeof(MENUITEMTEMPLATEHEADER);
	if (bMenuEx)
	{
		p += sizeof(DWORD);
	}

	// 更新菜单
	while (p < pbEnd)
	{
		// 获取菜单项标识、参数
		if (bMenuEx)
		{
			p += 3 * sizeof(DWORD);
			dwMenuID =  *((PDWORD) p);
			p += sizeof(DWORD) + sizeof(WORD);
		}
		else
		{
			wOptions = *((PWORD) p);
			p += sizeof(WORD);
			if ((wOptions & MF_POPUP) == FALSE)
			{
				dwMenuID = *((PWORD) p);
				p += sizeof(WORD);
			}
		}

		// 获取菜单项文本，转到下一个菜单项
		for (_WStrToStr(tzText, (PWSTR) p); *((PWSTR) p); p += sizeof(WCHAR));
		p += sizeof(WCHAR);

		if (bMenuEx)
		{
			// 根据命令标识设置菜单文本
			SetMenuString(hMenu, dwMenuID, tzText);
		}
		else
		{
			// 如果是弹出菜单项
			//_Assert(i < _NumberOf(iPos));
			iPos[i]++;
			if (wOptions & MF_POPUP)
			{
				// 根据位置设置菜单文本
				SetMenuString(hMenus[i], iPos[i] - 1, tzText, TRUE);

				// 获取下一级菜单，并把相关数据压入堆栈
				bHilite[i] = wOptions & MF_HILITE;
				hMenus[i + 1] = GetSubMenu(hMenus[i], iPos[i] - 1);
				i++;
				iPos[i] = 0;
				//_Assert(hMenus[i]);
			}
			else
			{
				// 根据命令标识设置菜单文本
				SetMenuString(hMenu, dwMenuID, tzText);

				if (wOptions & MF_HILITE)
				{
					// 弹出堆栈，如果已经完成则退出
					for (i--; (i >= 0) && bHilite[i]; i--);
					if (i < 0)
					{
						break;
					}
				}
			}
		}
	}
*/
//	FreeResource(hGlobal);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////