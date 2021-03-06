


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#pragma once
#include <Windows.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 功能: 设置或取消 ClassXP
// 参数: hWnd 指定窗口句柄。如果为 NULL 则针对当前线程中的所有窗口
//       bClassXP 指定设置或取消。 FALSE 表示取消，否则表示设置
// 返回: FALSE 表示失败，否则表示成功
#ifdef __cplusplus
extern "C"
#endif
BOOL WINAPI ClassXP(HWND hWnd, BOOL bClassXP);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
