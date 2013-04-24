

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ����
#include "Language.h"
#include "../Resource.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����ļ������ͼ���
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
// CLanguage �ྲ̬����
UINT CLanguage::m_uLang = IDM_View_Default;
UINT CLanguage::m_uMax = IDM_View_Default;
HFONT CLanguage::m_hFont = NULL;
TCHAR CLanguage::m_tzText[1024] = {0};
TCHAR CLanguage::m_tzFileName[MAX_PATH] = {0};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ʼ������
LANGID CLanguage::Initialize()
{
	HANDLE hFind;
	LANGID liLang;
	LANGID liResult;
	WIN32_FIND_DATA fdFind;
	TCHAR tzPath[MAX_PATH];

	// ��ȡ���Ա�ʶ
	liResult = 0;
	
	// ���������ļ���Ϊ��ʱ���룬������Ҫ����
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(tzPath + lstrlen(tzPath) - 4, TEXT(".ini"));

	// �������ļ��л�ȡ��������
	liLang = GetPrivateProfileInt(TEXT("LANG"), LIN_Language, GetUserDefaultLangID(), tzPath);
	if (liLang)
	{
		// ���������ļ�
		GetModuleFileName(NULL, tzPath, MAX_PATH);
		lstrcpy(_StrRChr(tzPath, '\\') + 1, TEXT("*") EXT_Lng);
		hFind = FindFirstFile(tzPath, &fdFind);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				// �����ָ��������
				lstrcpy(_StrRChr(tzPath, '\\') + 1, fdFind.cFileName);
				if (liLang == GetPrivateProfileInt(LIN_Language, LIN_Language, 0, tzPath))
				{
					// ���������ļ���
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
// �г�����
UINT CLanguage::List(HMENU hMenu)
{
	HANDLE hFind;
	LANGID liLang;
	LANGID liTemp;
	WIN32_FIND_DATA fdFind;
	TCHAR tzPath[MAX_PATH];

	// ���������ļ���Ϊ��ʱ���룬������Ҫ����
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(tzPath + lstrlen(tzPath) - 4, TEXT(".ini"));

	// �������ļ��л�ȡ��������
	liLang = GetPrivateProfileInt(TEXT("LANG"), LIN_Language, 0, tzPath);
	//liLang = CIni::GetInt(INI_Language);

	// ���������ļ�	
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(_StrRChr(tzPath, '\\') + 1, TEXT("*") EXT_Lng);
	hFind = FindFirstFile(tzPath, &fdFind);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// ��ȡ���Ա�ʶ���ж��Ƿ�Ϊ��Ч
			lstrcpy(_StrRChr(tzPath, '\\') + 1, fdFind.cFileName);
			liTemp = GetPrivateProfileInt(LIN_Language, LIN_Language, 0, tzPath);
			if (liTemp)
			{
				// ��һ����ӣ�����ָ��������� 50 ��������
				if (m_uMax == IDM_View_Default)
				{
					AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
				}
				else if (m_uMax >= IDM_View_Default + 50)
				{
					break;
				}

				// ��Ӳ˵���
				m_uMax++;
				fdFind.cFileName[lstrlen(fdFind.cFileName) - _LengthOf(EXT_Lng)] = 0;
				AppendMenu(hMenu, MF_BYCOMMAND, m_uMax, fdFind.cFileName);
				if (liTemp == liLang)
				{
					// ��¼ָ������
					m_uLang = m_uMax;
					lstrcpy(m_tzFileName, tzPath);

				}
			}
		}
		while (FindNextFile(hFind, &fdFind));
		FindClose(hFind);
	}

	// ѡ��˵���
	CheckMenuRadioItem(hMenu, IDM_View_Default, m_uMax, m_uLang, MF_BYCOMMAND);	//5309 IDM_View_Default

	return m_uLang;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
VOID CLanguage::Set(HMENU hMenu, UINT uLang)
{
//	HDC hDC;
	LANGID liLang;
//	LOGFONT lfFont;
	TCHAR tzName[MAX_PATH];

	// ����ǳ�ʼ������������
	if (hMenu == NULL)
	{
		liLang = uLang;
	}
	else
	{
		if((uLang <= IDM_View_Default) || (uLang > m_uMax))
		{
			// �л���Ĭ������
			liLang = 0;
			m_tzFileName[0] = 0;
			m_uLang = IDM_View_Default;
		}
		else
		{
			// �л�����������
			m_uLang = uLang;
			GetModuleFileName(NULL, m_tzFileName, MAX_PATH);
			GetMenuString(hMenu, uLang, tzName, MAX_PATH, MF_BYCOMMAND);
			wsprintf(_StrRChr(m_tzFileName, '\\'), TEXT("\\%s%s"), tzName, EXT_Lng);
			liLang = GetPrivateProfileInt(LIN_Language, LIN_Language, 0, m_tzFileName);
		}

		// ѡ��˵���
		CheckMenuRadioItem(hMenu, IDM_View_Default, m_uMax, m_uLang, MF_BYCOMMAND);
	}

	// ���������ļ���Ϊ��ʱ���룬������Ҫ����
	TCHAR tzPath[MAX_PATH];
	GetModuleFileName(NULL, tzPath, MAX_PATH);
	lstrcpy(tzPath + lstrlen(tzPath) - 4, TEXT(".ini"));

	// �������Ա�ʶ�������ļ���
	wsprintf(tzName, TEXT("%d"), liLang);
	liLang = WritePrivateProfileString(TEXT("LANG"), LIN_Language, tzName, tzPath);
	//CIni::SetInt(INI_Language, liLang);

	// ��������
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
// �Ӳ˵��л�ȡ�����ļ���
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
// ��ȡ�����ַ���
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
// ��ȡ�����ַ��������滻�����ַ�
PTSTR CLanguage::TranslateTextEx(PCTSTR ptzName, PCTSTR ptzDefault)
{
#ifdef _MAKELANG
	// �ж��Ƿ����������ַ�
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
// ��ȡ��Դ�ַ���
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
// ���²˵�
VOID CLanguage::TranslateMenu(HMENU hMenu, PCTSTR ptzResName)
{
	TCHAR tzSection[32];

	if (m_tzFileName[0])
	{
		// ��ȡ����
		if (IS_INTRESOURCE(ptzResName))
		{
			wsprintf(tzSection, TEXT("%u"), ptzResName);
		}
		else
		{
			lstrcpy(tzSection, ptzResName);
		}

		// �������ļ��и��²˵�
		UpdateMenuFromLanguage(hMenu, tzSection);
	}
	else
	{
		// ����Դ�и��²˵�
		UpdateMenuFromResource(hMenu, ptzResName);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ���²˵�BCMenu
VOID CLanguage::TranslateMenu(BCMenu * pBMenu, PCTSTR ptzResName)
{
	TCHAR tzSection[32];
	
	if (m_tzFileName[0])
	{
		// ��ȡ����
		if (IS_INTRESOURCE(ptzResName))
		{
			wsprintf(tzSection, TEXT("%u"), ptzResName);
		}
		else
		{
			lstrcpy(tzSection, ptzResName);
		}
		
		// �������ļ��и��²˵�
		UpdateMenuFromLanguage(pBMenu, tzSection);
	}
	else
	{
		// ����Դ�и��²˵�
//		UpdateBMenuFromResource(hMenu, ptzResName);
	}
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���¶Ի���
VOID CLanguage::TranslateDialog(HWND hWnd, PCTSTR ptzResName)
{
	TCHAR tzSection[32];

	if (m_tzFileName[0])	//��������ļ�����
	{
		// ��ȡ����
		if (IS_INTRESOURCE(ptzResName))
		{
			wsprintf(tzSection, TEXT("%u"), ptzResName);
		}
		else
		{
			lstrcpy(tzSection, ptzResName);
		}

		// �޸ĶԻ����ַ���
		UpdateDialogFromLanguage(hWnd, tzSection);

#ifndef _TRANSRECUR
		// ö�ٲ��޸��Ӵ����ַ���
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
// ���ò˵��ַ���
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
// �������ļ��и��²˵�
VOID CLanguage::UpdateMenuFromLanguage(HMENU hMenu, PCTSTR ptzSection, PCTSTR ptzPath)
{
	INT i;
	INT iNum;
	TCHAR tzName[256];
	TCHAR tzText[1024];
	MENUITEMINFO miiItem;

	// ��ʼ����������ȡ�˵�������
	miiItem.cbSize = CDSIZEOF_STRUCT(MENUITEMINFO, cch);
	miiItem.fMask = MIIM_SUBMENU | MIIM_ID;
	iNum = GetMenuItemCount(hMenu);
	for (i = 0; i < iNum; i++)
	{
		// ��ȡ�����˵�����ͱ�ʶ
		GetMenuItemInfo(hMenu, i, TRUE, &miiItem);
		if (miiItem.wID)
		{
			// ��ȡ�˵���ʶ�ı�
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
			// ���������ļ����ݹ��޸Ĳ˵��ַ���
			if ((miiItem.wID <= IDM_View_Default) || (miiItem.wID >= IDM_View_Default + 50) &&
				(miiItem.wID <= IDM_File_Recent) || (miiItem.wID >= IDM_File_Recent + 50))
			{
				GetMenuString(hMenu, i, tzText, _NumberOf(tzText), MF_BYPOSITION);
				WritePrivateProfileString(ptzSection, tzName, tzText, m_tzFileName);
			}
	#else // _MAKELANG
			// ���ò˵��ַ���
			if (GetPrivateProfileString(ptzSection, tzName, NULL, tzText, _NumberOf(tzText), m_tzFileName))
			{
				SetMenuString(hMenu, i, tzText, TRUE);
			}
	#endif // _MAKELANG

			// �ݹ��޸Ĳ˵��ַ���
			if (miiItem.hSubMenu)
			{
				UpdateMenuFromLanguage(miiItem.hSubMenu, ptzSection, tzName);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������ļ��и��²˵�
VOID CLanguage::UpdateMenuFromLanguage(BCMenu * pBMenu, PCTSTR ptzSection, PCTSTR ptzPath)
{
	//�õ��Ӳˆ�
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
// ����Դ�и��²˵�
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

	// �˵���ζ�ջ�����ڸ��µ����˵��ַ���
	INT i = 0;					// ���
	INT iPos[32] = {0};			// λ��
	BOOL bHilite[32] = {FALSE};	// ����
	HMENU hMenus[32] = {hMenu};	// ���

	// ������Դ
	hResource = FindResource(g_hInst, ptzResName, RT_MENU);
	if(!hResource)
		return;
	//_Assert(hResource);
	hGlobal = LoadResource(g_hInst, hResource);
	if(!hGlobal)
		return;
	//_Assert(hGlobal);

	// ��ȡ��Դ����ʼ��ַ����ֹ��ַ�Ͱ汾
	p = (PBYTE) LockResource(hGlobal);
	pbEnd = p + SizeofResource(g_hInst, hResource);
	bMenuEx = ((PMENUITEMTEMPLATEHEADER) p)->versionNumber;

	// ת���˵�������
	p += ((PMENUITEMTEMPLATEHEADER) p)->offset + sizeof(MENUITEMTEMPLATEHEADER);
	if (bMenuEx)
	{
		p += sizeof(DWORD);
	}

	// ���²˵�
	while (p < pbEnd)
	{
		// ��ȡ�˵����ʶ������
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

		// ��ȡ�˵����ı���ת����һ���˵���
		for (_WStrToStr(tzText, (PWSTR) p); *((PWSTR) p); p += sizeof(WCHAR));
		p += sizeof(WCHAR);

		if (bMenuEx)
		{
			// ���������ʶ���ò˵��ı�
			SetMenuString(hMenu, dwMenuID, tzText);
		}
		else
		{
			// ����ǵ����˵���
			//_Assert(i < _NumberOf(iPos));
			iPos[i]++;
			if (wOptions & MF_POPUP)
			{
				// ����λ�����ò˵��ı�
				SetMenuString(hMenus[i], iPos[i] - 1, tzText, TRUE);

				// ��ȡ��һ���˵��������������ѹ���ջ
				bHilite[i] = wOptions & MF_HILITE;
				hMenus[i + 1] = GetSubMenu(hMenus[i], iPos[i] - 1);
				i++;
				iPos[i] = 0;
				//_Assert(hMenus[i]);
			}
			else
			{
				// ���������ʶ���ò˵��ı�
				SetMenuString(hMenu, dwMenuID, tzText);

				if (wOptions & MF_HILITE)
				{
					// ������ջ������Ѿ�������˳�
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
// �������ļ��и��¶Ի���
BOOL CALLBACK CLanguage::UpdateDialogFromLanguage(HWND hWnd, PCTSTR ptzSection)
{
	TCHAR tzText[1024];
	TCHAR tzName[MAX_PATH];

	// ��ȡ���ڱ�ʶ�ַ���
	wsprintf(tzName, TEXT("%u"), GetDlgCtrlID(hWnd));

	// ���ô��ڱ���
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
	// ö�ٲ��޸��Ӵ����ַ���
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
// ����Դ�и��¶Ի���
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




// ������Դ
	hResource = FindResource(g_hInst,);
	if(!hResource)
		return;
	//_Assert(hResource);
	hGlobal = LoadResource(g_hInst, hResource);
	if(!hGlobal)
		return;
	//_Assert(hGlobal);

	// ��ȡ��Դ����ʼ��ַ����ֹ��ַ�Ͱ汾
	p = (PBYTE) LockResource(hGlobal);
	pbEnd = p + SizeofResource(g_hInst, hResource);
	bMenuEx = ((PMENUITEMTEMPLATEHEADER) p)->versionNumber;

	// ת���˵�������
	p += ((PMENUITEMTEMPLATEHEADER) p)->offset + sizeof(MENUITEMTEMPLATEHEADER);
	if (bMenuEx)
	{
		p += sizeof(DWORD);
	}

	// ���²˵�
	while (p < pbEnd)
	{
		// ��ȡ�˵����ʶ������
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

		// ��ȡ�˵����ı���ת����һ���˵���
		for (_WStrToStr(tzText, (PWSTR) p); *((PWSTR) p); p += sizeof(WCHAR));
		p += sizeof(WCHAR);

		if (bMenuEx)
		{
			// ���������ʶ���ò˵��ı�
			SetMenuString(hMenu, dwMenuID, tzText);
		}
		else
		{
			// ����ǵ����˵���
			//_Assert(i < _NumberOf(iPos));
			iPos[i]++;
			if (wOptions & MF_POPUP)
			{
				// ����λ�����ò˵��ı�
				SetMenuString(hMenus[i], iPos[i] - 1, tzText, TRUE);

				// ��ȡ��һ���˵��������������ѹ���ջ
				bHilite[i] = wOptions & MF_HILITE;
				hMenus[i + 1] = GetSubMenu(hMenus[i], iPos[i] - 1);
				i++;
				iPos[i] = 0;
				//_Assert(hMenus[i]);
			}
			else
			{
				// ���������ʶ���ò˵��ı�
				SetMenuString(hMenu, dwMenuID, tzText);

				if (wOptions & MF_HILITE)
				{
					// ������ջ������Ѿ�������˳�
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