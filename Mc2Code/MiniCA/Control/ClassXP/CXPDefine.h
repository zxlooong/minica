


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ����
#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �궨��
#define UNICODE					// ָ������Ϊ Unicode �汾
#define _UNICODE				// ָ������Ϊ Unicode �汾

#ifdef _USRDLL
#define CXP_DLLMODE				// ָ������Ϊ��̬���ӿ�
#define CXP_DLLAUTO				// ָ����̬���ӿ����ʱ�Զ����� ClassXP
#endif

#define CXP_DETECTTHEME			// ָ���������
//#define CXP_MULTITHREAD			// ָ��֧�ֶ��߳��û�����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ��������
#define CXPC_BUTTON				0				// ��ť
#define CXPC_STATIC				1				// ��̬�ؼ�
#define CXPC_EDITBOX			2				// �༭��
#define CXPC_HOTKEY				3				// �ȼ���
#define CXPC_IPBOX				4				// IP ��ַ��
#define CXPC_LISTBOX			5				// �б��
#define CXPC_LISTVIEW			6				// �б���ͼ
#define CXPC_TREEVIEW			7				// ���οؼ�
#define CXPC_COMBOBOX			8				// ��Ͽ�
#define CXPC_DATETIME			9				// ����/ʱ��ѡ��
#define CXPC_SPINBOX			10				// ��ת�ؼ�
#define CXPC_SCROLLBAR			11				// ������
#define CXPC_TRACKBAR			12				// ������
#define CXPC_PROGRESS			13				// ������
#define CXPC_TABCTRL			14				// �Ʊ�ؼ�
#define CXPC_CALENDER			15				// �����ؼ�
#define CXPC_ANIMATE			16				// �����ؼ�
#define CXPC_HEADER				17				// ����ؼ�
#define CXPC_TOOLBAR			18				// ������
#define CXPC_STATUSBAR			19				// ״̬��

// ����״̬
#define CXPS_DISABLED			0x00000001L		// ����״̬
#define CXPS_PRESSED			0x00000002L		// ����״̬
#define CXPS_HOTLIGHT			0x00000004L		// ����״̬ (����ڸô�����)
#define CXPS_FOCUS				0x00000010L		// ���м������뽹��

#define CXPS_DEFAULT			0x00000100L		// ��ťĬ��״̬
#define CXPS_CHECKED			0x00000200L		// ��ťѡ��״̬
#define CXPS_INDETERMINATE		0x00000400L		// ��ťδȷ��״̬

#define CXPS_SIMPLECMB			0x00001000L		// ��Ͽ������
#define CXPS_READONLY			0x00002000L		// �༭��ֻ��״̬

#define CXPS_LEFTSPIN			0x00004000L		// ��ת�ؼ���߶���
#define CXPS_RIGHTSPIN			0x00008000L		// ��ת�ؼ��ұ߶���
#define CXPS_ACCELSPIN			0x00010000L		// ��ת�ؼ���������
#define CXPS_HORIZON			0x00020000L		// ���ھ���ˮƽ����
#define CXPS_FIRSTPART			0x00040000L		// ���λ�ڵ�һ������

// ��ͷ����
#define CXPA_SMALLLEFT			0				// ����С��ͷ
#define CXPA_SMALLRIGHT			1				// ����С��ͷ
#define CXPA_SMALLUP			2				// ����С��ͷ
#define CXPA_SMALLDOWN			3				// ����С��ͷ
#define CXPA_LARGELEFT			4				// ������ͷ
#define CXPA_LARGERIGHT			5				// ���Ҵ��ͷ
#define CXPA_LARGEUP			6				// ���ϴ��ͷ
#define CXPA_LARGEDOWN			7				// ���´��ͷ

// ��ɫ
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
