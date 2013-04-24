#if !defined(AFX_MINISAPI_H__A01E64A6_ED6F_4C3C_B1AD_63AA9BDED32E__INCLUDED_)
#define AFX_MINISAPI_H__A01E64A6_ED6F_4C3C_B1AD_63AA9BDED32E__INCLUDED_

// MINISAPI.H - Header file for your Internet Server
//    MiniSapi Extension
#include "..\Control\Ado\ado2.h"
#include "resource.h"

class CMiniSapiExtension : public CHttpServer
{
public:
	CMiniSapiExtension();
	~CMiniSapiExtension();

// Overrides
	// ClassWizard generated virtual function overrides
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_VIRTUAL(CMiniSapiExtension)
	public:
	virtual BOOL GetExtensionVersion(HSE_VERSION_INFO* pVer);
	virtual LPCTSTR GetTitle() const;
	//}}AFX_VIRTUAL
	virtual BOOL TerminateExtension(DWORD dwFlags);

	// TODO: Add handlers for your commands here.
	// For example:

	void Default(CHttpServerContext* pCtxt);
	void CertReq(CHttpServerContext* pCtxt, LPTSTR pszUserInfo, LPTSTR pszKeyUsage,
				 LPTSTR pszEkeyUsage, LPTSTR pszFriendName, short iCertKeyLen, short iCertValidity,
				 short iCertType);
	void Query(CHttpServerContext* pCtxt, LPCSTR sQueryPwd);

	DECLARE_PARSE_MAP()

	//{{AFX_MSG(CMiniSapiExtension)
	//}}AFX_MSG
	private:
		BOOL ConnectDB(CString strMdb, CString & Msg);
		BOOL InsertDB(const char * lpszUserInfo, const char * lpszKeyUsage,
								  const char * lpszKeyEusage, const char * lpszFriendName,
								  const int iKeyLen, const int iCertValidity,
								  const short iCertType, CString & strQueryPwd, CString & strMsg);
		void SetTitle(CString strTitle)
		{
			m_Title = strTitle;
		}
		CString m_Title;
		CADODatabase m_pDb;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINISAPI_H__A01E64A6_ED6F_4C3C_B1AD_63AA9BDED32E__INCLUDED)
