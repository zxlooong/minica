////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#include "CXPBase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 钩子句柄
#ifdef CXP_MULTITHREAD
__declspec(thread)
#endif
HHOOK g_hCXPHook = NULL;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 动态连接库主函数
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
// 设置或取消 ClassXP
BOOL WINAPI ClassXP(HWND hWnd, BOOL bClassXP)
{
#ifdef CXP_DETECTTHEME
	// 检测主题
	if (bClassXP)
	{
		FARPROC pIsAppThemed = GetProcAddress(GetModuleHandle(TEXT("UXTHEME")), "IsAppThemed");
		if (pIsAppThemed && pIsAppThemed())
		{
			return FALSE;
		}
	}
#endif

	// 是否指定了窗口
	_ReturnValIf(hWnd, CXPSet(hWnd, bClassXP));

	// 如果是设置 ClassXP，并且没有安装钩子
	if (bClassXP && (g_hCXPHook == NULL))
	{
		// 枚举当前线程中已存在的窗口，并设置为 ClassXP
		EnumThreadWindows(GetCurrentThreadId(), CXPEnumProc, TRUE);

		// 安装钩子，设置新创建的窗口为 ClassXP
		g_hCXPHook = SetWindowsHookEx(WH_CALLWNDPROC, CXPHookProc, 0, GetCurrentThreadId());

		return (g_hCXPHook != NULL);
	}
	// 如果是取消 ClassXP，并且已经安装过钩子
	else if ((bClassXP == FALSE) && g_hCXPHook)
	{
		// 枚举当前线程的窗口，并取消 ClassXP
		EnumThreadWindows(GetCurrentThreadId(), CXPEnumProc, FALSE);

		// 取消窗口钩子
		UnhookWindowsHookEx(g_hCXPHook);
		g_hCXPHook = NULL;

		return TRUE;
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
