// MiniMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "minica.h"
#include "MiniMainDlg.h"
#include "SafeDataSetPage.h"
#include ".\Control\aboutctrl\AboutCtrl.h"
#include ".\Control\Splash\SplashWnd.h"
#include ".\GenericClass\Language.h"
#include ".\Control\CreditsCtrl-About\CreditsCtrl.h"
#include "minict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] =  __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	static void TestFunction2(LPCTSTR lpszArg);
	static void TestFunction(LPCTSTR lpszArg);
	static void DrawCreditsBackground(CDC *pDC, RECT rect, BOOL bAnimate, DWORD lParam);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CAboutCtrl	m_AboutCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCreditsCtrl m_wndCredits;
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	CString strCredits= "\tMiniCA System V2.0 \n\n"
				   "\r�汾��Ϣ:\n"
				   "MiniCA Bulid 051201\n"
				   "OPENSSL 0.9.8\n"
				   "CXPStyleButtonST By Davide Calabro\n"
				   "CProgressCtrlST By Davide Calabro\n"
				   "CSuperGridCtrl By Allan Nielsen\n"
				   "CFileEditCtrl By PJ Arends\n"
				   "CStaticTreeCtrl By Franc Morales\n"
				   "CAboutCtrl By Pablo van der Meer\n"
				   "W3Mfc By PJ Naughter\n"
				   "ADO classes By Carlos Antollini\n"
				   "CSplashWnd By Kirk Stowell\n"
				   "CBitmapSlider By Junho Ryu\n"
				   "ClassXP 1.2 By Yonsm\n"
				   "\n\r�������:\n"
				   "hpxs email:hpxs@hotmail.com\n\n"
				   "\r��л:\n̨�����ſ���(Commer ����)"
				   "\nemail:commercity@hotmail.com\n"
				   "Э��MiniCA����\n";

	m_AboutCtrl.SetCredits(CMiniCaApp::NormalCode(strCredits), 240, 350, 350);
	//�������ֿ�����λ��,���,�߶�
//	____________________
//	|
//	|
//	|

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ABOUTCTRL, m_AboutCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
/*	srand((unsigned)time(NULL));
	
	// initialize credits control...
	
	// Content
	CString s;
	s = "<font color='255,255,255' face='arial' size='12' align='center'>";
	s += "<font size='28' face='Comic Sans MS' style='ui' color='255,255,255'>Demo Stuff </font><p>";
	s += "  <font style='bu' align='left'>Color:<br></font><font color='255,0,0'>y</font><font color='220,30,10'>e</font><font color='200,60,30'>e</font><font color='200,100,50'>e</font><font color='180,120,80'>e</font><font color='160,140,100'>e</font><font color='140,160,120'>e</font><font color='120,180,140'>e</font><font color='100,200,160'>e</font><font color='80,180,180'>e</font><font color='60,140,220'>e</font><font color='80,120,250'>h</font><font color='110,100,255'>a</font><font color='130,90,255'>a</font><font color='150,80,255'>a</font><font color='170,70,255'>a</font><font color='180,50,255'>a</font><br>";
	s += "  <font style='bu' align='left'>Font:<br></font><font color='0,200,100' size='14' face='Comic Sans MS'>Comic Sans MS</font><br><font color='0,200,100' size='14' face='Courier New'>Courier New</font><br><font color='0,200,100' size='14' face='Tahoma'>Tahoma</font><br>";
	s += "  <font style='bu' align='left'>Size:<br></font><font color='150,100,255'><font size='42'>W</font><font size='36'>H</font><font size='28'>O</font><font size='18'>O</font><font size='14'>O</font><font size='18'>O</font><font size='28'>O</font><font size='36'>O</font><font size='42'>W</font></font><br>";
	s += "  <font style='bu' align='left'>Styles:<br></font><font color='40,120,240' size='14'><font style='b'>bold</font>  <font style='i'>italic</font>  <font style='u'>underline</font>  <font style='s'>strikeout</font></font><br>";
	s += "  <font style='bu' align='left'>Background:<br></font><font color='255,255,255' size='14' style='b'><font background='0,0,255'>Blue</font>  <font background='255,0,0'>Red</font>  <font background='210,150,40'>Brown</font>  <font background='0,0,255' color='none'>Ghost Font</font></font><br>";
	s += "  <font style='bu' align='left'>Alignment:<br></font><font color='255,150,40' size='14' style='b'><font align='right'>right <br></font><font align='center'>center<br></font><font align='left'> left<br></font></font>";
	s += "  <font style='bu' align='left'>Vertical Alignment:<br></font><font color='0,220,160' size='12' style='b'><font valign='top'>top</font><font valign='middle'>middle</font><font valign='bottom'>bottom</font><font size='48'>BIG</font></font><br>";
	s += "  <font style='bu' align='left'>Resource Bitmap:<br></font><font color='30,70,255' size='12' style='b'><img src='#%d'><font color='255,0,0'>    red </font><font valign='bottom'>bo</font><font valign='middle'>rd</font><font valign='top'>er </font><img src='#%d' border='4' color='255,0,0'></font><br>";
	s += "  <font style='bu' align='left'>Links:<br></font><font color='100,160,220' size='14' style='b'><a href='#Hello World!#%d' color='100,220,160'>Click Me!</a><br><a href='notepad.exe' color='100,220,160' style='bui'>notepad</a><br><a href='http://www.w3c.org' color='100,220,160' face='Courier New' size='16'>www.w3c.org</a><br><a href='mailto:devix@devix.cjb.net?SUBJECT=Hello World!! ;)' color='100,220,160' face='Comic Sans MS' size='18'>devix@devix.cjb.net</a><br><font size='36' color='255,200,0' style='u'><a href='#\nDo or do not. There is no try.\n\n\t\t-Yoda#%d' color='none' background='255,200,0' style='-uis'>Click Here</a></font><vspace size='4'><font valign='top' size='12' style='-b'>image link:  </font><a href='#%d#%d' color='255,200,0'><img src='#%d' border='2' color='none'></a></font><vspace size='80'>";
	s += "<font size='28' face='Comic Sans MS' style='ui'>CCreditsCtrl </font><p><font size='18' style='u' color='255,200,0'>About</font><p>Coded in January 2001<br>by <font style='b'>Marc Richarme</font><vspace size='5'><font align='left' size='11'><hspace size='30'><font style='i'>mail:   </font><font style='ub'><a href='mailto:devix@devix.cjb.net?SUBJECT=Hello World!' color='255,200,0'>devix@devix.cjb.net</a></font><br><hspace size='30'><font style='i'>web:   </font><font style='ub'><a href='http://devix.cjb.net' color='255,200,0'>http://devix.cjb.net</a></font><p></font><font size='18' style='u' color='255,200,0'>Credits</font><p>Thanks to <font style='ub'><a href='http://www.codeproject.com' color='255,200,0'>The Code Project</a></font> for being<br> the coolest VC++/MFC site out there!<br></font><vspace size='80'>";
	
	// Content Data
	m_wndCredits.FormatDataString(s,IDB_BACKGROUND,IDB_BACKGROUND,(long)CAboutDlg::TestFunction,
		(long)CAboutDlg::TestFunction,(long)this,(long)CAboutDlg::TestFunction2,IDB_BITMAP2);
	
	// use our own function for background drawing
	m_wndCredits.m_pBackgroundPaint = CAboutDlg::DrawCreditsBackground;
	m_wndCredits.m_dwBackgroundPaintLParam = FALSE;
	
	// as our background is going to be black, use black as default transparent color
	m_wndCredits.m_crInternalTransparentColor = RGB(0,0,0);
	
	// now, we're ready to begin... create the window
	m_wndCredits.Create(WS_EX_CLIENTEDGE,WS_VISIBLE|WS_CHILD,IDC_ABOUTCTRL,this,0,IDC_SMILEY,0);
	*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::DrawCreditsBackground(CDC *pDC, RECT rect, BOOL bAnimate, DWORD lParam)
{
	static int on1,on2,oon1,oon2;
	pDC->FillSolidRect(&rect,0x00000000);
	
	// uncomment the following lines if you like small green squares :)

//	pDC->SetBkColor(0x000000);
//	CBrush bgBrush;
//	bgBrush.CreateHatchBrush(HS_CROSS,RGB(0,70,30));
//	pDC->FillRect(&rect,&bgBrush);
	
	if(bAnimate || (!lParam)) // return now if we are not supposed to do the animation
		return;

	int n1,n2;
	n1 = rand()*200/RAND_MAX-100;
	n2 = rand()*200/RAND_MAX-100;

	// 2/10 chance of (prehaps) making some wild stuff on one of the curves
	if(rand() < RAND_MAX/10)
		n1 = rand()*400/RAND_MAX-200;
	else if(rand() < RAND_MAX/10)
		n2 = rand()*400/RAND_MAX-200;

	POINT points1[4] = {
		rect.right,0,
		(rect.right-rect.left)/2+n1,(rect.bottom-rect.top)/2,
		(rect.right-rect.left)/2,(rect.bottom-rect.top)/2-n1,
		0,rect.bottom
	};
	POINT points2[4] = {
		0,0,
		(rect.right-rect.left)/2-n2,(rect.bottom-rect.top)/2,
		(rect.right-rect.left)/2,(rect.bottom-rect.top)/2-n2,
		rect.right,rect.bottom
	};
	POINT opoints1[4] = {
		rect.right,0,
		(rect.right-rect.left)/2+on1,(rect.bottom-rect.top)/2,
		(rect.right-rect.left)/2,(rect.bottom-rect.top)/2-on1,
		0,rect.bottom
	};
	POINT opoints2[4] = {
		0,0,
		(rect.right-rect.left)/2-on2,(rect.bottom-rect.top)/2,
		(rect.right-rect.left)/2,(rect.bottom-rect.top)/2-on2,
		rect.right,rect.bottom
	};
	POINT oopoints1[4] = {
		rect.right,0,
		(rect.right-rect.left)/2+oon1,(rect.bottom-rect.top)/2,
		(rect.right-rect.left)/2,(rect.bottom-rect.top)/2-oon1,
		0,rect.bottom
	};
	POINT oopoints2[4] = {
		0,0,
		(rect.right-rect.left)/2-oon2,(rect.bottom-rect.top)/2,
		(rect.right-rect.left)/2,(rect.bottom-rect.top)/2-oon2,
		rect.right,rect.bottom
	};

	CPen wpen(PS_SOLID,1,RGB(150,220,255));
	CPen pen(PS_SOLID,2,RGB(50,100,255));
	CPen open(PS_SOLID,1,RGB(50,100,255));
	CPen oopen(PS_SOLID,1,RGB(0,30,150));
	CPen *pOldPen = pDC->SelectObject(&oopen);
	pDC->PolyBezier(oopoints1,4);
	pDC->PolyBezier(oopoints2,4);
	pDC->SelectObject(&open);
	pDC->PolyBezier(opoints1,4);
	pDC->PolyBezier(opoints2,4);
	pDC->SelectObject(&pen);
	pDC->PolyBezier(points1,4);
	pDC->PolyBezier(points2,4);
	pDC->SelectObject(&wpen);
	pDC->PolyBezier(points1,4);
	pDC->PolyBezier(points2,4);
	pDC->SelectObject(pOldPen);

	oon1 = on1;
	oon2 = on2;
	on1 = n1;
	on2 = n2;
}

void CAboutDlg::TestFunction(LPCTSTR lpszArg)
{
	if(lpszArg == NULL)
		AfxMessageBox("(null)");
	else
		AfxMessageBox(lpszArg);
}

void CAboutDlg::TestFunction2(LPCTSTR lpszArg)
{
	static int cnt = 1;
	CAboutDlg *pDlg = (CAboutDlg*)atol(lpszArg);
	
	pDlg->m_wndCredits.m_nTimerSpeed = cnt==0 ? 30 : cnt * 40;
	
	if(++cnt > 3)
		cnt = 0;
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN)
	{
		OnOK();
	}
	return CDialog::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMiniMainDlg dialog
CMiniMainDlg::CMiniMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiniMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMiniMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon =  AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bDlgCreated =  false;
	m_RegFlag =  false;
	
	m_hPageCaItem = NULL;
	m_hPageCertWizardSheetItem = NULL;
	m_hPageExtItem = NULL;
	m_hPageWinEvpSheetItem = NULL;
	m_hPageWebSheetItem = NULL;
	m_hPageAsn1ParseItem = NULL;
	m_hPageCertInfoItem = NULL;
	m_hPageRegItem = NULL;
	m_hPageCertDbItem = NULL;
	m_hPageUpItem = NULL;

	m_bHide = FALSE;
	
	m_ImgList.DeleteImageList();
}


void CMiniMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiniMainDlg)
	DDX_Control(pDX, IDC_STATIC_LABLE, m_CaptionBarCtrl);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMiniMainDlg, CDialog)
	//{{AFX_MSG_MAP(CMiniMainDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE, OnSelchangingTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiniMainDlg message handlers

BOOL CMiniMainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) ==  IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu * pSysMenu =  GetSystemMenu(FALSE);
//	BCMenu * p = BCMenu::UpdateMenu(pSysMenu);
	if (pSysMenu   !=    NULL)
	{
		//ɾ����󻯲˵�
		pSysMenu->RemoveMenu(SC_ZOOM ,MF_BYCOMMAND); 
		//ɾ���ƶ�
//		pSysMenu->RemoveMenu(SC_MOVE ,MF_BYCOMMAND); 
		//ɾ���ָ�
		pSysMenu->RemoveMenu(SC_RESTORE ,MF_BYCOMMAND); 
		//ɾ����С
		pSysMenu->RemoveMenu(SC_SIZE ,MF_BYCOMMAND); 

		//�޸��ƶ�0xF010 - 61456
		pSysMenu->ModifyMenu(SC_MOVE , MF_BYCOMMAND, SC_MOVE, "�ƶ�(&M)"); 

		//�޸���С�� 0xF020 - 61472
		pSysMenu->ModifyMenu(SC_ICON , MF_BYCOMMAND, SC_ICON, "����(&H)"); 
		
		//�޸Ĺر�0xF060 - 61536
		pSysMenu->ModifyMenu(SC_CLOSE , MF_BYCOMMAND, SC_CLOSE, "�ر�(&C)	Alt + F4"); 
		
		
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
//			pSysMenu->AppendMenu(MF_STRING, IDM_SHOWSPLINT, "�ۿ�����(&S)	Ctrl+S");

			m_LangMenu.LoadMenu(IDR_MENU_LANGUAGE);

			// �г�����
			CLanguage::List(m_LangMenu.GetSubMenu(0)->m_hMenu);

			pSysMenu->InsertMenu(6, MF_BYPOSITION|MF_POPUP,
				(UINT)m_LangMenu.GetSubMenu(0)->m_hMenu, "LangUage");	//���Բ˵� ID_MENUSYS_LANGUAGE
			
//			pSysMenu->AppendMenu(MF_SEPARATOR);
//			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, "���� MiniCA(&A)...");	//0x0010

			// ����˵� bi xu cun zai , fou ze zai fan ti huan jing xiamian bu neng zhengque xianshi fanti zi
			CLanguage::TranslateMenu(pSysMenu->m_hMenu, "60000");

		}
	}



	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here


	//XP���˵�,����Ϊѡ��ʱ��ͼ��ƫ��
	BCMenu::SetMenuDrawMode( BCMENU_DRAWMODE_XP );
	BCMenu::SetSelectDisableMode(FALSE);
	BCMenu::SetXPBitmap3D(TRUE);
//	BCMenu::SetIconSize(24,24);

	m_ImgList.Create(24,24,TRUE|ILC_COLOR24,20,1);
	//24��ÿ��ͼƬ�Ŀ��Ϊ24
	//24��ÿ��ͼƬ�ĸ߶�Ϊ24
	//TRUE  ��ͼ��͸����ʽ��ʾ
	//ILC_COLOR24,  ��ɫ��Ϊ24λ��������ѡ��������й�API)
	//16:    ����ʱ����Ϊ3��ͼƬ
	//1:    ͼƬ����������ǰ����ʱ���Զ����ݣ�ÿ������1��ͼƬ������
	
	
	HICON hIcon =  NULL;

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_EXTERN), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//0
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_WIZARD), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//1   �� ��֤��
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CLIENT), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//2
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_SERVER), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//3
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_LOCK), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//4
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_RA), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//5
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_ASN1V), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//6
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_OCSPS), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//7
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_OCSPC), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//8
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_VIEWCERT), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//9
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_SETUP), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//10 -  ϵͳ����
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTYPE), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//11 - ��֤����
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_UP), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//12
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//13  ��   ��ͼ��
	DestroyIcon(hIcon);

	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTINFO), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//14 - ��֤��Ϣ
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTEXT), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//15 - ��֤��չ
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_RUN), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//16
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_CERTMAN), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//17 - ��֤����
	DestroyIcon(hIcon);
	
	hIcon =  (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_ICON_REG), IMAGE_ICON, 24, 24, 0);
	m_ImgList.Add(hIcon);//18 - ϵͳע��
	DestroyIcon(hIcon);


	//����Create�����ô���,��Ҫ��DestroyWindow����
	m_PageCa.Create(IDD_PROPPAGE_CA,this);


	m_PageCertWizardSheet.Create(IDD_PROPPAGE_CASHEET ,this);


	m_PageExt.Create(IDD_PROPPAGE_EXT,this);//֤����չ

	m_PageWinEvpSheet.Create(IDD_PROPPAGE_EVPSHEET,this);

	m_PageWebSheet.Create(IDD_PROPPAGE_WEBSHEET,this);

//	m_SslSheet.Create(IDD_PROPPAGE_SSLSHEET,this);

//	m_PageRa.Create(IDD_PROPPAGE_RASERVER,this);

	m_PageAsn1Parse.Create(IDD_PROPPAGE_ASN1,this);

//	m_OcspSheet.Create(IDD_PROPPAGE_OCSPSHEET,this);	//leak 908

	m_PageCertInfo.Create(IDD_PROPPAGE_CERTINFO,this);

	m_PageCertDb.Create(IDD_PROPPAGE_CERTSETUP,this);	//leak 1996

	m_PageUp.Create(IDD_PROPPAGE_UPDATE,this);

	m_PageReg.Create(IDD_PROPPAGE_REG,this);

//	m_TreeCtrl.SetBkImage(IDB_BITMAP);


	m_TreeCtrl.SetImageList(&m_ImgList,TVSIL_NORMAL);

	

	//�������Ϣ

	m_hPageCaItem =  m_TreeCtrl.InsertItem("MiniCA V2.0",0,0);

	m_TreeCtrl.SetItemImage(m_hPageCaItem,13,13);
	m_TreeCtrl.SetItemData(m_hPageCaItem,(DWORD)&m_PageCa);
	m_TreeCtrl.SelectItem(m_hPageCaItem);


	m_hPageCertWizardSheetItem =  m_TreeCtrl.InsertItem(MiniCT_0000, 0, 0,m_hPageCaItem);      //MiniCT 0
	m_TreeCtrl.SetItemImage(m_hPageCertWizardSheetItem,1,1);
	m_TreeCtrl.SetItemData(m_hPageCertWizardSheetItem,(DWORD)&m_PageCertWizardSheet);

	m_hPageExtItem =  m_TreeCtrl.InsertItem(MiniCT_0001, 0, 0,m_hPageCaItem);	//MiniCT 1
	m_TreeCtrl.SetItemImage(m_hPageExtItem,0,0);
	m_TreeCtrl.SetItemData(m_hPageExtItem,(DWORD)&m_PageExt);

	m_hPageWinEvpSheetItem =  m_TreeCtrl.InsertItem(MiniCT_0002, 0, 0,m_hPageCaItem);	////MiniCT 2
	m_TreeCtrl.SetItemImage(m_hPageWinEvpSheetItem,4,4);
	m_TreeCtrl.SetItemData(m_hPageWinEvpSheetItem,(DWORD)&m_PageWinEvpSheet);

	m_hPageWebSheetItem =  m_TreeCtrl.InsertItem(MiniCT_0003, 0, 0,m_hPageCaItem);		//MiniCT 3
	m_TreeCtrl.SetItemImage(m_hPageWebSheetItem,2,2);
	m_TreeCtrl.SetItemData(m_hPageWebSheetItem,(DWORD)&m_PageWebSheet);

	m_hPageCertInfoItem =  m_TreeCtrl.InsertItem("MiniCT_0004", 0, 0,m_hPageCaItem);	//MiniCT 4
	m_TreeCtrl.SetItemImage(m_hPageCertInfoItem,9,9);
	m_TreeCtrl.SetItemData(m_hPageCertInfoItem,(DWORD)&m_PageCertInfo);

	m_hPageAsn1ParseItem =  m_TreeCtrl.InsertItem(MiniCT_0005, 0, 0,m_hPageCaItem);	//MiniCT 5
	m_TreeCtrl.SetItemImage(m_hPageAsn1ParseItem,6,6);
	m_TreeCtrl.SetItemData(m_hPageAsn1ParseItem,(DWORD)&m_PageAsn1Parse);

	m_hPageUpItem =  m_TreeCtrl.InsertItem(MiniCT_0006, 0, 0,m_hPageCaItem);	////MiniCT 6
	m_TreeCtrl.SetItemImage(m_hPageUpItem,12,12);
	m_TreeCtrl.SetItemData(m_hPageUpItem,(DWORD)&m_PageUp);

	if(((CMiniCaApp *)AfxGetApp())->IsHpxs() || ((CMiniCaApp *)AfxGetApp())->IsReg())
	{
		m_hPageCertDbItem =  m_TreeCtrl.InsertItem(MiniCT_0007, 0, 0, m_hPageCaItem);		////MiniCT 7
		m_TreeCtrl.SetItemImage(m_hPageCertDbItem,10,10);
		m_TreeCtrl.SetItemData(m_hPageCertDbItem,(DWORD)&m_PageCertDb);
	}

// 	if(((CMiniCaApp *)AfxGetApp())->IsHpxs() || !((CMiniCaApp *)AfxGetApp())->IsReg())
// 	{
// 		m_hPageRegItem =  m_TreeCtrl.InsertItem(MiniCT_0008, 0, 0,m_hPageCaItem);		//MiniCT 8
// 		m_TreeCtrl.SetItemImage(m_hPageRegItem,18,18);
// 		m_TreeCtrl.SetItemData(m_hPageRegItem,(DWORD)&m_PageReg);
// 	}

	m_TreeCtrl.Expand(m_hPageCaItem, TVE_EXPAND );

	m_bDlgCreated =  true;

	m_RegFlag =  ((CMiniCaApp *)AfxGetApp())->IsReg();

	CRect rcDlgs;
	GetDlgItem(IDC_PAGE_AREA)->GetWindowRect(rcDlgs);
	ScreenToClient(rcDlgs);
	rcDlgs.top +=  10;
	rcDlgs.bottom -=  5;
	rcDlgs.left +=  5;
	rcDlgs.right -=  5;


	m_PageExt.MoveWindow(rcDlgs);

	m_PageCa.MoveWindow(rcDlgs);

	m_PageCertWizardSheet.MoveWindow(rcDlgs);

	m_PageWinEvpSheet.MoveWindow(rcDlgs);

	m_PageWebSheet.MoveWindow(rcDlgs);

//	m_SslSheet.MoveWindow(rcDlgs);

//	m_PageRa.MoveWindow(rcDlgs);

//	m_OcspSheet.MoveWindow(rcDlgs);

	m_PageAsn1Parse.MoveWindow(rcDlgs);

	m_PageCertInfo.MoveWindow(rcDlgs);

//	if(::IsWindow(m_PageSetup.m_hWnd))
//		m_PageSetup.MoveWindow(rcDlgs);

	if(::IsWindow(m_PageCertDb.m_hWnd))
		m_PageCertDb.MoveWindow(rcDlgs);

	if(::IsWindow(m_PageUp.m_hWnd))
		m_PageUp.MoveWindow(rcDlgs);

	if(::IsWindow(m_PageReg.m_hWnd))
		m_PageReg.MoveWindow(rcDlgs);

	m_PageCa.ShowWindow(SW_SHOW);

	////////////////////////ϵͳ����//////////////////////////////////////////////////////
	m_nid.cbSize =  sizeof( NOTIFYICONDATA );
	m_nid.hWnd =  m_hWnd; 
	m_nid.uID =  ID_SYSTEMTRAY; 
	m_nid.uFlags =  NIF_MESSAGE|NIF_ICON|NIF_TIP;
	m_nid.uCallbackMessage =  WM_SYSTEMTRAY;//�Զ�����Ϣ
	m_nid.hIcon =  AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	
	// ������
	m_CaptionBarCtrl.m_textClr     = ::GetSysColor(COLOR_3DFACE);
	m_CaptionBarCtrl.m_fontWeight  = FW_BOLD;
	m_CaptionBarCtrl.m_fontSize    = 14;
	m_CaptionBarCtrl.m_csFontName  = "Verdana";
	m_CaptionBarCtrl.SetConstantText("MiniCA V2.0");

	CString strName =  "MiniCA V2.0";
	if(!m_RegFlag)//0δע��
		strName+= CMiniCaApp::NormalCode("(�������)");
	SetWindowText(strName);
	strcpy(m_nid.szTip, strName);
	::Shell_NotifyIcon( NIM_ADD,&m_nid);

	m_CaptionBarCtrl.SetWindowText(strName);


	//�б��Ҽ������ļ���
	CString lpszFile = ((CMiniCaApp *)AfxGetApp())->GetAppPath() + "\\MiniLog.log";
	m_ListBox.SetSaveFile(lpszFile);

	//SetWindowLong(this->m_hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
	////////////////////////////////////////////////////////////////////////////////////
//	m_ListBox.SetBkColor(::GetSysColor(COLOR_BTNFACE));
//	m_TreeCtrl.SetBkColor(::GetSysColor(COLOR_BTNFACE));

	//�õ������в���,���ⲿ�������ļ�WinEvp
	CString strEvpFile;
	strEvpFile.Format("%s", AfxGetApp()->m_lpCmdLine);
	//�������Ǵ�""��,��Ҫȥ��""
	strEvpFile.Replace("\"", " ");
	strEvpFile.TrimLeft();
	strEvpFile.TrimRight();
	CString ext;
	ext = strEvpFile.Right(3);
	if(ext.CompareNoCase("Evp") == 0)
	{
		CSafeDataSetPage::EncType encType;
		CSafeDataSetPage * pPage = (CSafeDataSetPage * )m_PageWinEvpSheet.GetPage("CSafeDataSetPage");
		if(pPage)
		{
			if(!pPage->ParseEvp(strEvpFile, encType))
			{
				SendMessage(WM_CLOSE);
			}
			else
			{
				GotoPage("WinEvp");
				CWinEvpSheet * pEvpSheet = (CWinEvpSheet *)GetPage("CWinEvpSheet");
				if(pEvpSheet)	//goto datasafeset
				{
					pEvpSheet->m_PageWinEvpSheet.SetCurSel(1);
				}
			}
		}
	}

	//�ر�splash
	CSplashWnd::KillSplashScreen();

	Language();	

	m_wndTaskbarNotifier.Create(this);
	m_wndTaskbarNotifier.SetSkin(IDB_SKIN_MSN);
	m_wndTaskbarNotifier.SetTextFont("Arial",90,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE);
 	m_wndTaskbarNotifier.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_wndTaskbarNotifier.SetTextRect(CRect(10,40,m_wndTaskbarNotifier.m_nSkinWidth-10,
		m_wndTaskbarNotifier.m_nSkinHeight-25));

//	m_bar.Create(this);	
	
//	m_bar.AddPane(22222, 0);
//	m_bar.SetPaneInfo(0, 22222, SBPS_NOBORDERS, 300);


//	m_bar.GetStatusBarCtrl().SetBkColor(RGB(180,180,180));
		

//	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,
//		122222);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMiniMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) ==  IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID ==  IDM_SHOWSPLINT)
	{
		CSplashWnd::EnableSplashScreen(1);
		CSplashWnd::ShowSplashScreen(100000, "JPG", "IDR_SPLASHCA", this);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMiniMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon =  GetSystemMetrics(SM_CXICON);
		int cyIcon =  GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x =  (rect.Width() - cxIcon + 1) / 2;
		int y =  (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMiniMainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMiniMainDlg::OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView =  (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(m_bDlgCreated)
	{
		HTREEITEM hCurrent =  m_TreeCtrl.GetSelectedItem();
		if(hCurrent)
		{
			DWORD dData =  m_TreeCtrl.GetItemData(hCurrent);
			if(dData!=0)
				((CPropertyPage *)dData)->ShowWindow(SW_HIDE);
		}
	}
	*pResult =  0;
}

void CMiniMainDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView =  (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(m_bDlgCreated)
	{
		HTREEITEM hCurrent =  m_TreeCtrl.GetSelectedItem();
		if(hCurrent)
		{
			DWORD dData =  m_TreeCtrl.GetItemData(hCurrent);
			if(dData!=0)
			{
/*				if((DWORD)&m_PageCaWizard ==  dData)
				{
					m_PageCaWizard.ViewWizardInfo(); //���ɱ���
				}*/
				((CPropertyPage *)dData)->ShowWindow(SW_SHOW);
	//			((CPropertyPage *)dData)->SetActiveWindow();
				CString strName =  m_TreeCtrl.GetItemText(hCurrent);
				if(/*hCurrent == m_hPageWinEvpSheetItem ||*/ hCurrent == m_hPageAsn1ParseItem || hCurrent == m_hPageCertInfoItem)
				{
					DragAcceptFiles(TRUE);
				}
				else
					DragAcceptFiles(FALSE);


				if(strName.Find("MiniCA")== -1)
					strName =  "MiniCA "+strName;
				if(!m_RegFlag)//0δע��
					strName+= CMiniCaApp::NormalCode("(�������)");
				int nImage =  0, 
					nSelectedImage =  0;  
				m_TreeCtrl.GetItemImage(hCurrent,nImage,nSelectedImage);
				DestroyIcon(m_hIconSelect);
				m_hIconSelect =  m_ImgList.ExtractIcon(nImage);
			//	SendMessage(WM_SETICON,ICON_SMALL,(LPARAM)hIcon); 
				SetIcon(m_hIconSelect, FALSE);		// Set small icon

				SetWindowText(strName);
		
				m_CaptionBarCtrl.SetWindowText(strName);
			
				SetupShell(strName,m_hIconSelect);
			}
		}
	}
	*pResult =  0;
}

void CMiniMainDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	m_PageCa.DestroyWindow();
	
	m_PageCertWizardSheet.DestroyWindow();
	
	m_PageExt.DestroyWindow();

	m_PageWinEvpSheet.DestroyWindow();

	m_PageWebSheet.DestroyWindow();
	
//	m_SslSheet.DestroyWindow();
	
//	m_PageRa.DestroyWindow();
	
//	m_OcspSheet.DestroyWindow();
	
	m_PageAsn1Parse.DestroyWindow();
	m_PageCertInfo.DestroyWindow();
	m_PageReg.DestroyWindow();
	
	m_PageCertDb.DestroyWindow();
	
	m_PageUp.DestroyWindow();

	::Shell_NotifyIcon( NIM_DELETE,&m_nid );

	//ɾ��XP��������ȫ�ֹ���
	//ClassXP(NULL, FALSE);

}

LRESULT CMiniMainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_SYSTEMTRAY:  //�Զ�����Ϣ     
		if(lParam==WM_LBUTTONDOWN)
		{  
			//	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW);
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_RESTORE);
			AfxGetApp()->m_pMainWnd->SetForegroundWindow();
			m_bHide = FALSE;
			break;
		}
		else if(lParam==WM_RBUTTONDOWN)
		{ 

			//�õ�����״̬
	//		WINDOWPLACEMENT Ment;
	//		BOOL bGet = GetWindowPlacement(&Ment);
			
			POINT point;
			GetCursorPos(&point);

			BCMenu m_PopMenu;
			m_PopMenu.LoadMenu(IDR_MENU_SHELL);
			m_PopMenu.LoadToolbar(IDR_MINICAMENU);
			m_PopMenu.SetBitmapBackground(RGB(128,0,0));

			CLanguage::TranslateMenu(&m_PopMenu, MAKEINTRESOURCE(IDR_MENU_SHELL));

			CMenu * pPopup = m_PopMenu.GetSubMenu(0);
			SetForegroundWindow(); //������Թر�
			ASSERT(pPopup);
			UINT nSelection = pPopup->TrackPopupMenu(TPM_RIGHTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|
				TPM_NONOTIFY|TPM_RETURNCMD,point.x, point.y,this, NULL);
			m_PopMenu.DestroyMenu();
			//���ز˵�id
			if(nSelection == ID_MENUITEM_CLOSE)//MiniCA�ر�
			{
				SendMessage(WM_CLOSE,0,0);
			}
			else if(nSelection == ID_MENUITEM_SHOW)//��ʾ
			{
				AfxGetApp()->m_pMainWnd->ShowWindow(SW_RESTORE);
				AfxGetApp()->m_pMainWnd->SetForegroundWindow();
				m_bHide = FALSE;
			}
			else if(nSelection == ID_MENUITEM_HIDE)//����
			{
				AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
				m_bHide = TRUE;
			}
			break;
		}
		
	case WM_SYSCOMMAND: //ϵͳ��Ϣ   
		if(wParam==SC_MINIMIZE)
		{ 
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);//��������
			m_bHide = TRUE;
			return 0;
		}
		else if ((wParam >= IDM_View_Default) && (wParam <= IDM_View_Default + 49))
		{
			// �ı�����
			HMENU hMenu = GetSystemMenu(FALSE)->m_hMenu;
			if (((MF_CHECKED & GetMenuState(hMenu, wParam, MF_BYCOMMAND)) != MF_CHECKED))
			{
				CLanguage::Set(hMenu, wParam);
				Language();	//ת���Ի�������
				// ����˵�
				CLanguage::TranslateMenu(hMenu, "60000"); //ת��ϵͳ�˵�����
			}
			return 0;
		}

		break;
	}		
	return CDialog::WindowProc(message, wParam, lParam);
}


int CMiniMainDlg::AddMsg(CString caption, CString info, DWORD type)
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString time,msg;
	time.Format(_T("%04d%02d%02d%02d%02d%02d"), tm.wYear,
		tm.wMonth,tm.wDay,tm.wHour, tm.wMinute,tm.wSecond);
	msg.Format("%s (%s) : %s",time, caption, info);
	if(type == M_OK)
		ShowNotifier(info);
	return m_ListBox.AddMsg(msg,type);
}

void CMiniMainDlg::DeleteLast()
{
	m_ListBox.DeleteLast();
}

/*HBRUSH CMiniMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr =  CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(CTLCOLOR_LISTBOX  ==  nCtlColor)  
	{  
		LOGBRUSH  logBrush;  
		logBrush.lbStyle  =   BS_SOLID;  
		logBrush.lbColor  =   RGB(240,240,240);
		logBrush.lbHatch  =   HS_CROSS;  
		
		return  hbr  =   CreateBrushIndirect(&logBrush);  
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}*/

void CMiniMainDlg::SetRegFlag(BOOL bFlag)
{
	m_RegFlag =  bFlag;
}

void CMiniMainDlg::SetItemIcon(CString pageName,UINT uIcon)
{
	if(pageName.IsEmpty())
		return ;
	HTREEITEM hItem = m_TreeCtrl.FindItem(m_TreeCtrl.GetRootItem(),CMiniCaApp::NormalCode(pageName));
	if(hItem!=NULL)
	{
		m_TreeCtrl.SetItemImage(hItem,uIcon,uIcon);

		if(pageName.Find("MiniCA")== -1)
			pageName =  "MiniCA "+pageName;
		if(!m_RegFlag)//0δע��
			pageName += "(�������)";
		SetupShell(CMiniCaApp::NormalCode(pageName), m_ImgList.ExtractIcon(uIcon));

	}
	
}

BOOL CMiniMainDlg::GotoPage(CString pageName)//ת��ĳҳ
{
	//������,��������,չ����,ѡ��
/*	if(pageName.IsEmpty())
		return FALSE;
	HTREEITEM hItem = m_TreeCtrl.FindItem(m_TreeCtrl.GetRootItem(),pageName);
	if(hItem!=NULL)
	{
		m_TreeCtrl.SelectItem(hItem);
		m_TreeCtrl.EnsureVisible(hItem);
		m_TreeCtrl.ExpandParent(hItem,TVE_EXPAND);
		return TRUE;
	}
	else
		return FALSE;*/
	return m_TreeCtrl.GotoPage(pageName);
}

void CMiniMainDlg::ViewCertInfo(char * cert,UINT certlen, void * x509)
{
	GotoPage(MiniCT_0004);			////MiniCT 4 "��֤����"
	char buf[255] = {0};
	if(!m_PageCertInfo.ViewCertInfo(cert,certlen,x509,buf))
		if(certlen == 0 && x509 == 0)
		{
			AddMsg(MiniCT_0004, buf, M_ERROR);		////MiniCT 4
		}
}

CColorListBox * CMiniMainDlg::GetListBox()
{
	return & m_ListBox;
}

void CMiniMainDlg::OnOK ()
{
}

void CMiniMainDlg::OnCancel()
{
}

void CMiniMainDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//��WEB������������WM_CLOSE��Ϣ,��ʹWEB��������Ӧ,�ر��߳�,����
	//WEB������ӦWM_CLOSE�¼�
	//WM_CLOSE��Ӧ�¼����߳������ڴ�й©����,ֻ��ͨ����ʾ
	if(m_PageWebSheet.IsWebRun())
	{
//	m_PageWebServer.StopServer();
		GotoPage(MiniCT_0003);							//MiniCT 3 "Web����"
		CString lpText = MiniCT_0009;
		CString lpCaption = MiniCT_0010;

		MessageBox(MiniCT_0009,	//MiniCT 9
			MiniCT_0010, MB_ICONWARNING | MB_OK);		////MiniCT 10
		return;
	}
//	m_PageWebServer.SendMessage(WM_CLOSE);

/*	if(!m_PageSafeData.CanQuit())	//������
	{
		GotoPage(MiniCT_0002);				//MiniCT 2
		if(MessageBox(MiniCT_0011,		//MiniCT 11
			MiniCT_0010, MB_ICONWARNING |			//MiniCT 10
			MB_YESNO |MB_DEFBUTTON2 )!=IDYES)
		{
			return;
		}
	}*/
	DestroyIcon(m_hIconSelect);
	CDialog::OnOK();
}

void CMiniMainDlg::SetupShell(CString & strName,const HICON hIcon)
{
	m_nid.hIcon =  hIcon;
	SetWindowText(strName);
	strcpy(m_nid.szTip, strName);
	::Shell_NotifyIcon( NIM_MODIFY,&m_nid);
}

//ͨ�������������ƶ�����ҳ�Ķ���ָ��
CWnd * CMiniMainDlg::GetPage(CString strPageName)
{
	//ö������ҳ
	CRuntimeClass * prt = NULL;

	prt = m_PageCa.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageCa;

	prt = m_PageCertWizardSheet.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageCertWizardSheet;

	prt = m_PageExt.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageExt;

	prt = m_PageWinEvpSheet.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageWinEvpSheet;

/*	prt = m_SslSheet.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_SslSheet;

	prt = m_PageRa.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageRa;

	prt = m_OcspSheet.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_OcspSheet;

	prt = m_PageAsn1Parse.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageAsn1Parse;
*/
	prt = m_PageCertInfo.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageCertInfo;

	prt = m_PageReg.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageReg;

	prt = m_PageCertDb.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageCertDb;

	prt = m_PageUp.GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, strPageName )  == 0 )
		return &m_PageUp;

	return NULL;
}

void CMiniMainDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM hCurrent =  m_TreeCtrl.GetSelectedItem();
	
	WORD wNumFilesDropped = DragQueryFile(hDropInfo, -1, NULL, 0);
    // 
	POINT  pt;
	DragQueryPoint(hDropInfo, &pt);
	CString FileName;
    for (WORD x = 0 ; x < wNumFilesDropped; x++)
	{
        // �õ�PathName�ĳ���,���ڵ���������ΪNULL�����Ե��û�ʧ�ܲ��������賤��
        WORD wPathnameSize = DragQueryFile(hDropInfo, x, NULL, 0);
        // ����memory
        char * npszFile = (char *) LocalAlloc(LPTR, wPathnameSize += 1);
        // �����
        if (npszFile == NULL) continue;
        // �ٴεõ��ļ���
        DragQueryFile(hDropInfo, x, npszFile, wPathnameSize);
		// ����ǿ�ݷ�ʽ�����ת���õ���ȷ���ļ���,expandedFileΪ�����ļ���
		FileName = ExpandShortcut(npszFile);
		if(FileName.IsEmpty())
		{
			LocalFree(npszFile);
			continue;
		}

/*		if(hCurrent == m_hPageWinEvpSheetItem)
		{
			CString strDir;
			strDir = FileName.Right(3);
			
			CSafeDataSetPage::EncType encType;
			
			CSafeDataSetPage * pSafeData = (CSafeDataSetPage*)m_PageWinEvpSheet.GetPage("CSafeDataSetPage");
		
			if(pSafeData)
			{
				if(strDir.CompareNoCase("evp") == 0)
				{
					pSafeData->ParseEvp(FileName, encType);
				}
				else
					pSafeData->AddList(FileName);		//�����չ����Ϊ.evp
			}
		}*/
		else if(hCurrent == m_hPageAsn1ParseItem)
		{
			m_PageAsn1Parse.GetAsn1Info(FileName);
		}
		else if(hCurrent == m_hPageCertInfoItem)
		{
			ViewCertInfo(FileName.GetBuffer(0));
		}
		
        // �ͷ�memory
        LocalFree(npszFile);
    }
    //���
    DragFinish(hDropInfo);	
	CDialog::OnDropFiles(hDropInfo);
}

CString CMiniMainDlg::ExpandShortcut(char * inFile)
{
    CString outFile = "";
	// Make sure we have a path
	ASSERT(inFile != _T(""));
	IShellLink * psl;
	HRESULT hres;
	// Create instance for shell link
	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (LPVOID*) &psl);
	if (SUCCEEDED(hres))
	{
		// Get a pointer to the persist file interface
		IPersistFile* ppf;
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
		if (SUCCEEDED(hres))
		{
			// Make sure it's ANSI
			WORD wsz[MAX_PATH];
			::MultiByteToWideChar(CP_ACP, 0, inFile, -1, wsz, MAX_PATH);
			
			// Load shortcut
			hres = ppf->Load(wsz, STGM_READ);
			if (SUCCEEDED(hres))
			{
                WIN32_FIND_DATA wfd;
                // find the path from that
                HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH), 
					MAX_PATH,
					&wfd, 
					SLGP_UNCPRIORITY);
				
                outFile.ReleaseBuffer();
			}
			else 
				outFile = inFile;
			ppf->Release();
		}
		psl->Release();
	}
	
    // if this fails, outFile == ""
	return outFile;
}

BOOL CMiniMainDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::PreTranslateMessage(pMsg);
}

void CMiniMainDlg::Language()
{
	TranslateCT(); 

	CLanguage::TranslateDialog(m_PageCa.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CA));
	m_PageCa.TranslateCT();

	m_PageCertWizardSheet.Language();

	m_PageExt.TranslateCT();

	m_PageWinEvpSheet.Language();
	
	CLanguage::TranslateDialog(m_PageExt.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_EXT));

	m_PageWebSheet.Language();

	m_PageCertInfo.TranslateCT();

	m_PageAsn1Parse.TranslateCT();
	
	m_PageUp.TranslateCT();

	m_PageCertDb.TranslateCT();

	CLanguage::TranslateDialog(m_PageCertInfo.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CERTINFO));
	CLanguage::TranslateDialog(m_PageAsn1Parse.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_ASN1));
	CLanguage::TranslateDialog(m_PageReg.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_REG));
	CLanguage::TranslateDialog(m_PageCertDb.m_hWnd, MAKEINTRESOURCE(IDD_PROPPAGE_CERTSETUP));

	HTREEITEM hCurrent =  m_TreeCtrl.GetSelectedItem();
	CString strName = m_TreeCtrl.GetItemText(hCurrent);

	if(strName.Find("MiniCA")== -1)
		strName =  "MiniCA "+strName;
	if(!m_RegFlag)//0δע��
		strName+= CMiniCaApp::NormalCode("(�������)");

	m_CaptionBarCtrl.SetWindowText(strName);
	SetWindowText(strName);
}

void CMiniMainDlg::TranslateCT()	//���g�T����Ϳؼ�,���l�ؼ��ȃ���
{
	m_TreeCtrl.SetRedraw(FALSE);
	m_TreeCtrl.SetItemText(m_hPageCertWizardSheetItem, MiniCT_0000);	//MiniCT 0
	m_TreeCtrl.SetItemText(m_hPageExtItem, MiniCT_0001);	//MiniCT 1
	m_TreeCtrl.SetItemText(m_hPageWinEvpSheetItem, MiniCT_0002);	//MiniCT 2
	m_TreeCtrl.SetItemText(m_hPageWebSheetItem, MiniCT_0003);	//MiniCT 3
	m_TreeCtrl.SetItemText(m_hPageCertInfoItem, MiniCT_0004);	//MiniCT 4
	m_TreeCtrl.SetItemText(m_hPageAsn1ParseItem, MiniCT_0005);	//MiniCT 5
	m_TreeCtrl.SetItemText(m_hPageUpItem, MiniCT_0006);	//MiniCT 6
	m_TreeCtrl.SetItemText(m_hPageCertDbItem, MiniCT_0007);	//MiniCT 7
	m_TreeCtrl.SetItemText(m_hPageRegItem, MiniCT_0008);	//MiniCT 8
	m_TreeCtrl.SetRedraw();
}

void CMiniMainDlg::ShowNotifier(CString strNotifier)
{
	//�����жϴ����Ƿ�������
	if(m_bHide == TRUE)
		m_wndTaskbarNotifier.Show(strNotifier);
}