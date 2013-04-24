////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ����
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���Ӿ��
#ifdef CXP_MULTITHREAD
__declspec(thread)
#endif
HHOOK g_hCXPHook = NULL;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��̬���ӿ�������
#ifdef CXP_DLLMODE
//#pragma comment(linker, "/EXPORT:_ClassXP@8")
#pragma comment(linker, "/EXPORT:ClassXP=_ClassXP@8")
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

#ifdef CXP_DLLAUTO
		ClassXP(NULL, TRUE);
#endif
	}

#ifdef CXP_DLLAUTO
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		ClassXP(NULL, FALSE);
	}
#endif

	return TRUE;
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���û�ȡ�� ClassXP
BOOL WINAPI ClassXP(HWND hWnd, BOOL bClassXP)
{
#ifdef CXP_DETECTTHEME
	// �������
	if (bClassXP)
	{
		FARPROC pIsAppThemed = GetProcAddress(GetModuleHandle(TEXT("UXTHEME")), "IsAppThemed");
		if (pIsAppThemed && pIsAppThemed())
		{
			return FALSE;
		}
	}
#endif

	// �Ƿ�ָ���˴���
	_ReturnValIf(hWnd, CXPSet(hWnd, bClassXP));

	// ��������� ClassXP������û�а�װ����
	if (bClassXP && (g_hCXPHook == NULL))
	{
		// ö�ٵ�ǰ�߳����Ѵ��ڵĴ��ڣ�������Ϊ ClassXP
		EnumThreadWindows(GetCurrentThreadId(), CXPEnumProc, TRUE);

		// ��װ���ӣ������´����Ĵ���Ϊ ClassXP
		g_hCXPHook = SetWindowsHookEx(WH_CALLWNDPROC, CXPHookProc, 0, GetCurrentThreadId());

		return (g_hCXPHook != NULL);
	}
	// �����ȡ�� ClassXP�������Ѿ���װ������
	else if ((bClassXP == FALSE) && g_hCXPHook)
	{
		// ö�ٵ�ǰ�̵߳Ĵ��ڣ���ȡ�� ClassXP
		EnumThreadWindows(GetCurrentThreadId(), CXPEnumProc, FALSE);

		// ȡ�����ڹ���
		UnhookWindowsHookEx(g_hCXPHook);
		g_hCXPHook = NULL;

		return TRUE;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
