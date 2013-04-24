


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 预处理
#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 宏定义
#define UNICODE					// 指定编译为 Unicode 版本
#define _UNICODE				// 指定编译为 Unicode 版本

#ifdef _USRDLL
#define CXP_DLLMODE				// 指定编译为动态连接库
#define CXP_DLLAUTO				// 指定动态连接库加载时自动设置 ClassXP
#endif

#define CXP_DETECTTHEME			// 指定检测主题
//#define CXP_MULTITHREAD			// 指定支持多线程用户界面
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 窗口类型
#define CXPC_BUTTON				0				// 按钮
#define CXPC_STATIC				1				// 静态控件
#define CXPC_EDITBOX			2				// 编辑框
#define CXPC_HOTKEY				3				// 热键框
#define CXPC_IPBOX				4				// IP 地址框
#define CXPC_LISTBOX			5				// 列表框
#define CXPC_LISTVIEW			6				// 列表视图
#define CXPC_TREEVIEW			7				// 树形控件
#define CXPC_COMBOBOX			8				// 组合框
#define CXPC_DATETIME			9				// 日期/时间选择
#define CXPC_SPINBOX			10				// 旋转控件
#define CXPC_SCROLLBAR			11				// 滚动条
#define CXPC_TRACKBAR			12				// 滑动条
#define CXPC_PROGRESS			13				// 进度条
#define CXPC_TABCTRL			14				// 制表控件
#define CXPC_CALENDER			15				// 日历控件
#define CXPC_ANIMATE			16				// 动画控件
#define CXPC_HEADER				17				// 标题控件
#define CXPC_TOOLBAR			18				// 工具栏
#define CXPC_STATUSBAR			19				// 状态栏

// 窗口状态
#define CXPS_DISABLED			0x00000001L		// 禁用状态
#define CXPS_PRESSED			0x00000002L		// 按下状态
#define CXPS_HOTLIGHT			0x00000004L		// 高亮状态 (鼠标在该窗口上)
#define CXPS_FOCUS				0x00000010L		// 具有键盘输入焦点

#define CXPS_DEFAULT			0x00000100L		// 按钮默认状态
#define CXPS_CHECKED			0x00000200L		// 按钮选中状态
#define CXPS_INDETERMINATE		0x00000400L		// 按钮未确定状态

#define CXPS_SIMPLECMB			0x00001000L		// 组合框简单属性
#define CXPS_READONLY			0x00002000L		// 编辑框只读状态

#define CXPS_LEFTSPIN			0x00004000L		// 旋转控件左边对齐
#define CXPS_RIGHTSPIN			0x00008000L		// 旋转控件右边对齐
#define CXPS_ACCELSPIN			0x00010000L		// 旋转控件按键加速
#define CXPS_HORIZON			0x00020000L		// 窗口具有水平属性
#define CXPS_FIRSTPART			0x00040000L		// 鼠标位于第一个区域

// 箭头类型
#define CXPA_SMALLLEFT			0				// 向左小箭头
#define CXPA_SMALLRIGHT			1				// 向右小箭头
#define CXPA_SMALLUP			2				// 向上小箭头
#define CXPA_SMALLDOWN			3				// 向下小箭头
#define CXPA_LARGELEFT			4				// 向左大箭头
#define CXPA_LARGERIGHT			5				// 向右大箭头
#define CXPA_LARGEUP			6				// 向上大箭头
#define CXPA_LARGEDOWN			7				// 向下大箭头

// 颜色
#define CXPR_CANVAS				GetSysColor(COLOR_BTNFACE)
#define CXPR_TEXT				GetSysColor(COLOR_BTNTEXT)
#define CXPR_DTEXT				GetSysColor(COLOR_GRAYTEXT)
#define CXPR_FRAME				0x00BD9E7B
#define CXPR_DFRAME				(CXPR_CANVAS - 0x00202020)
#define CXPR_WINDOW				GetSysColor(COLOR_WINDOW)
#define CXPR_DWINDOW			CXPR_CANVAS
#define CXPR_ARROW				0x0084614A
#define CXPR_DARROW				0x00C6CBCE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
