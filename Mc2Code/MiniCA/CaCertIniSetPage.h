#if !defined(AFX_SETUPPAGE_H__4D0BFBFF_1819_496A_AA94_AD10CDAC2273__INCLUDED_)
#define AFX_SETUPPAGE_H__4D0BFBFF_1819_496A_AA94_AD10CDAC2273__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaCertIniSetPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CCaCertIniSetPage dialog
#include ".\Control\tooltip\OXToolTipCtrl.h"

class CCaCertIniSetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaCertIniSetPage)

// Construction
public:
	void TranslateCT();
	struct stuINI
	{
		BOOL bIsDisp;			//是否显示在界面中
		char cInsideName[256];
		char cDispName[256];
		char cInfo[256];
		BOOL IsCheck;			//是否选中
		stuINI()
		{
			memset(this, 0, sizeof(stuINI));
		}
		stuINI(BOOL bisDisp, char * pInsideName, char * pDispName, 
			char * pInfo, BOOL bisCheck)
		{
			this->bIsDisp = bisDisp;
			strncpy(this->cInsideName, pInsideName, 255);
			strncpy(this->cDispName, pDispName, 255);
			strncpy(this->cInfo, pInfo, 255);
			this->IsCheck = bisCheck;
		}
	};


	CCaCertIniSetPage();
	~CCaCertIniSetPage();

// Dialog Data
	//{{AFX_DATA(CCaCertIniSetPage)
	enum { IDD = IDD_PROPPAGE_CAINI };
	CTreeCtrl	m_ExtTree;
	CTreeCtrl	m_UserTree;
	CXPStyleButtonST	m_BSave;
	CXPStyleButtonST	m_BResert;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCaCertIniSetPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COXToolTipCtrl m_toolTip;
	// Generated message map functions
	//{{AFX_MSG(CCaCertIniSetPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEdit2();
	afx_msg void OnBSave();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnBResert();
	afx_msg void OnCheck1();
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeext(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeext(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()
private:
	void ClickTree(CTreeCtrl * pTree);
	enum eTreeType
	{
		USERTREE,
		EXTTREE
	} m_eTreeType;
	void UpdateInfo(CTreeCtrl * pTree, const HTREEITEM hItem, 
					CMap<UINT, UINT, stuINI *, stuINI * > * pMap);		//信息项发生改变时候,更新MAP

	void CCaCertIniSetPage::SelchangedTree(CTreeCtrl * pTree, NM_TREEVIEW* pNMTreeView,
									   CMap<UINT, UINT, stuINI *, stuINI * > * pMap);

	void GetIniInfo(eTreeType, CString strIniPath);	//加载ini信息到树控件
	HTREEITEM m_hOldItemUser, m_hOldItemExt;
	BOOL bIsSave;//是否保存标记,如果未保存,提示保存
	CMap<UINT, UINT, stuINI *, stuINI * > m_UserMap, m_ExtMap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPAGE_H__4D0BFBFF_1819_496A_AA94_AD10CDAC2273__INCLUDED_)
