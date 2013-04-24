#if !defined(AFX_SAFEDATASETPAGE_H__9174F171_3067_4C73_AAD1_55D5475BDB1C__INCLUDED_)
#define AFX_SAFEDATASETPAGE_H__9174F171_3067_4C73_AAD1_55D5475BDB1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SafeDataSetPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSafeDataSetPage dialog
#include "DataSafe.H"
#include ".\Control\SuperList\SuperGridCtrl.h"
#include ".\Control\ProgressCtrlST\ProgressCtrlST.h"
#include "evp.h"
#include ".\GenericClass\OleTarget.h"
#include "SafeDataCdbPage.h"

class CSafeDataSetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSafeDataSetPage)

// Construction
public:
	void TranslateCT();
	void DoSign();			//ǩ��
	void DoRSAPubDec();		//RSA ���_����
	void DoRSAPriDec();		//RSA ˽�_����
	void DoOpenSeal();		//��������ŷ�
	void DoDecCrypt();		//�Գƽ���
	void DoRSAPrivEnc();	//RSA ˽�_����
	void DoRSAPubEnc();		//RSA ���_����
	void DoSeal();			//�����ŷ�
	void DoCrypt();			//�ԳƼ���
	BOOL GetIsEnc() const	//���߳��еõ���ǰ�Ĳ�����ʽ,���ܻ��ǽ���
	{
		return m_bIsEnc;
	}
	CWnd * GetParentWnd() const //������,�������Ķ����ܸı��Աֵ(m_pParent)
	{
		return m_pParent;
	}
	CString GetEvpPath() const
	{
		return m_strEvpPath;
	}

	enum EncType
	{
		NONE = 0,
		CRYPT = 1001,
		DIGEST = 2001,
		SIGN = 3001,
		SEAL = 4001,
		PubEnc = 5001,
		PrivEnc = 6001
	}m_EncType;

	BOOL ParseEvp(CString strEvpFile, EncType & encType);
	BOOL AddList(CString sLocalDir, UINT uPid = 0, CSuperGridCtrl::CTreeItem * hPitem = NULL);
	BOOL DecOneFile(CString strEvpFile, CString strOutFile, DWORD dSeek, DWORD dFileLen,
					DWORD dOldLen, CString & outStr);


	CSafeDataSetPage();
	~CSafeDataSetPage();

// Dialog Data
	//{{AFX_DATA(CSafeDataSetPage)
	enum { IDD = IDD_PROPPAGE_SAFED };
//	CXPEdit	m_XpEditPwd;
	CXPStyleButtonST	m_BEnc;
	CProgressCtrlST	m_ProgAll;
	CProgressCtrlST	m_ProgOne;
	CSuperGridCtrl	m_DataSetList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSafeDataSetPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSafeDataSetPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickListDataset(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListDataset(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBEnc();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnKeydownListDataset(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnBegindragListDataset(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnDropOk(WPARAM wParam,  LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	BOOL SelectPath();
	void AddMsg(CString info, DWORD type = M_OK);
	void AddOnceMsg(CString info, DWORD type = M_OK);

	BOOL Mdir(CString strPathName, CString & strInfo);

	BOOL Crypt(CString strCpName, CString strPwd, CString strFileName, CString & outMsg);
	BOOL DecCrypt(CString strCpName, CString strPwd, CString strEvpFile, CString & outMsg);

	BOOL Digest(CString strMdName, CString strOutName, CString & outMsg);
	BOOL VerifyDigest(CString strMdName, CString strEvpFile, CString & outStr);

	BOOL Sign(CString strMdName, const BYTE * pCertBuf, const UINT nCertLen, CString strPwd,
								  CString strOutName, CString & outStr);
	BOOL VerifySign(CString strMdName, const BYTE * pCertBuf, const UINT nCertLen,
								  CString strOutName, CString & outStr);

	BOOL Seal(CEvp::stuCertLink * pCERT, CString strCpName, CString strOutName, CString & outMsg);
	BOOL OpenSeal(char * pKey, UINT ukeyLen, char * pwd, CString strCpName,
							CString strEvpFile, CString & outStr);

	BOOL RSAPubEnc(const BYTE * pCertBuf, const UINT nCertLen, CString strOutName, CString & outStr);
	BOOL RSAPriDec(const BYTE * pCertBuf, const UINT nCertLen, CString strPwd, CString strEvpFile, CString & outStr);

	BOOL RSAPrivEnc(const BYTE * pCertBuf, const UINT nCertLen, CString strPwd,
								  CString strOutName, CString & outStr);
	BOOL RSAPubDec(const BYTE * pCertBuf, const UINT nCertLen, CString strEvpFile, CString & outStr);


	CString GetPathName(CDataSafe* pDataSafe, CList<CDataSafe *, CDataSafe *> * pDataList);

	CSuperGridCtrl::CTreeItem *  AddDataSet(const CDataSafe * pDataSafe,
											const CString strDataSet, const UINT uImage,
											const DWORD & dFileLen,	const DWORD dNewLen = 0,
											CString strTime = "", CString strType = "",
											CSuperGridCtrl::CTreeItem * pParent = NULL);


	BOOL EnumDir(CString sLocalDir, UINT uPid = 0, CSuperGridCtrl::CTreeItem * hPitem = NULL);

	BOOL MadeFileHead(CFile & File, const int nCount, DWORD & dFileInfoLen, CString & strMsg);

	BOOL MadeFileNeck(CList<CDataSafe *, CDataSafe *> * pDataList, CFile &File, 
							const EncType encType, const int nCount, 
									const DWORD dFileInfoLen, CString &strMsg);

	void AddFileList(const WIN32_FIND_DATA & FileData, 
					 CSuperGridCtrl::CTreeItem * hPitem,
					 const UINT uTempParent,
					 CString sFile,
					 DWORD & dFileLen);//ö�ٵ��ļ�ʱ��Ĳ���

	CProgressCtrlST * ShowProg(int nItem, int nCol);
	
	//���� pDirList �������pDataList�е�Ŀ¼��Ϣ,��ָ��,�����ͷ�,�ͷ�ʱ��ֻ�ͷ�pDataList�Ϳ�����,�����ͷ�pDirList
	void EnumList(CList<CDataSafe *, CDataSafe *> * pDataList, CList<CDataSafe *, CDataSafe *> * pDirList = NULL);

	void CleanList(CList<CDataSafe *, CDataSafe *> * pDataList);

	CImageList m_Image;//Must provide an imagelist
	CStringList m_DirName;//���ڱ����з��ļ���Ϣ,�����ظ����
	UINT m_uIndex;//�ļ���Ϣ�����
	static CProgressCtrlST * m_pProgOne;
	CWnd * m_pParent;	//������,������߳���ʹ��AfxGetMainWnd()�õ��Ļ����
	BOOL m_bIsEnc;//����,���ܷ�ʽ
	CString m_p12Pwd;	//ȫ��˽�_����,�����ڸ���������ʹ��
	char m_strKey[10240];//P12����,֤��·��,������
	DWORD m_lenKey;//����
	CDropTarget m_Target;
	CSafeDataCdbPage * m_pPageCdb;
	CString m_strEvpPath;	//����������ɵ��ļ���,�ⲿ�����.evp�ļ���
	CString m_strSavePath;	//���ܵ���·��

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAFEDATASETPAGE_H__9174F171_3067_4C73_AAD1_55D5475BDB1C__INCLUDED_)
