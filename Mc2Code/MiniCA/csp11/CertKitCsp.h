//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CERTENROLLCSP_H__49BBAB4B_4E80_418C_AEAC_F3036FCF400D__INCLUDED_)
#define AFX_CERTENROLLCSP_H__49BBAB4B_4E80_418C_AEAC_F3036FCF400D__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Rpcrt4.lib")

#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <windows.h>
#include <wincrypt.h>
#include <tchar.h>


#include "MemTools.h"
#include "Certificate.h"


using namespace MemTools;

//////////////////////////////////////////////////////////////////////////
#if !defined(tstring)
#   if defined(UNICODE) || defined(_UNICODE)
#       define tstring std::wstring 
#   else
#       define tstring std::string
#   endif
#endif

#if !defined(tstrstream)
#   if defined(UNICODE) || defined(_UNICODE)
#       define tstrstream std::stringstream
#   else
#       define tstrstream std::wstringstream
#   endif
#endif

#define XENCODETYPE				X509_ASN_ENCODING | PKCS_7_ASN_ENCODING
#define REG_SUB_KEY_TO_WRITE	_T("XCertEnroll")
#define SZ_CERT_RDN_VALUE		_T("EnterSafe XCertEnroll Demo Cert")

#define SZ_CERT_PRVNAME			_T("ProviderName")
#define SZ_CERT_CTNNAME			_T("ContainerName")
#define SZ_CERT_KEYSPEC			_T("KeySpec")    
#define SZ_CERT_PRVTYPE			_T("ProviderType") 
#define SZ_CERT_GENTIME			_T("GenTime")
#define SZ_CERT_PUBHASH			_T("PublicKeyHash")

typedef std::vector<tstring> Tstrlist;
typedef std::vector<CCertificate> Tcerlist;
typedef std::multimap<tstring, tstring> Trdnmap;

typedef unsigned long ulong;
typedef unsigned char uchar;
//////////////////////////////////////////////////////////////////////////

class CCertKitCsp  
{
public:
	CCertKitCsp();
	virtual ~CCertKitCsp();
    BOOL Reset();

public:
	LONG CreateP10(tstring& rszP10);
	LONG AcceptP7(const TCHAR* szP7, int iType=0);
    LONG AcceptCert(const TCHAR* szCert, int iType=0);
	LONG AcceptP12(const TCHAR* szP12, const TCHAR* szPsw, int iOpt = 0);

	LONG InstallRootCert(TCHAR* szB64Cert);
	LONG InstallRootCert(PCERT_CONTEXT pCert);
	LONG InstallCA(BOOL bMachine, TCHAR* szB64Cert);

public:
	LONG VerifySignData(const TCHAR* szdata, const TCHAR* szSignature, const TCHAR* szCert, ulong ulOpt = 0);
	LONG VerifySignature(const TCHAR* szMsg, const TCHAR* szSignature, ulong ulOpt = 0);
	LONG HashData(const BYTE* pdata, long datalen, tstring& szHash);
public:
	BOOL SetKeySpec(LONG lKeySpec);
	BOOL GetKeySpec(LONG& lKeySpec);
	BOOL SetGenKeyFlag(LONG lFlag);
	BOOL GetGenKeyFlag(LONG& lFlag);
	BOOL SetCryptAlg(ALG_ID iAlg);
	BOOL GetCryptAlg(ALG_ID& iAlg);

	BOOL SetHashAlg(ALG_ID iAlgID);
	BOOL GetHashAlg(ALG_ID& iAlgID);
    BOOL SetReqSignAlgOid(const TCHAR* szAlgOID);
    BOOL GetReqSignAlgOid(tstring& iAlgOID);

	BOOL SetCspName(const TCHAR* szCspName);
	BOOL GetCspName(tstring& szCspName);
	BOOL SetCspType(LONG lPtype);
	BOOL GetCspType(LONG& lPtype);
	BOOL SetReqCtnName(const TCHAR* szCtnName);
	BOOL GetReqCtnName(tstring& szCtnName);
    BOOL SetCtnName(const TCHAR* szCtnName);
    BOOL GetCtnName(tstring& szCtnName);

	BOOL SetBWriteCert2Csp(BOOL bTrue);
	BOOL GetBWriteCert2Csp(BOOL& bTrue);
	BOOL SetBUsingExitingKeySet(BOOL bTrue);
	BOOL GetBUsingExitingKeySet(BOOL& bTrue);
	BOOL SetBUseReqRegEnv(BOOL bTrue);
	BOOL GetBUseReqRegEnv(BOOL& bTrue);

	BOOL SetCertReqCN(const TCHAR* szCertRdn);
    BOOL GetCertReqCN(tstring& szCertRdn);
    BOOL GetCertReqDnName(tstring& szCertDnName);
    BOOL SetCertReqDnName(const TCHAR* szCertDnName);
	BOOL GetCertReqRdnName(const TCHAR* szRdntype, tstring& szCertRdnName);
    BOOL SetCertReqRdnName(const TCHAR* szRdntype, const TCHAR* szCertRdn);


	BOOL SetReqRegKey(const TCHAR* szRegKey);
	BOOL GetReqRegKey(tstring& szRegKey);

    BOOL GetCspList(const ulong lType, Tstrlist &vcCspList);
	BOOL GetCtnList(Tstrlist &vcCtnList);
    BOOL GetCspCertList(Tcerlist &vcCertList);
    BOOL GetMyStroeCertList(Tcerlist &vcCertList);

public:
	tstring GenGUID();
	
    LONG AlgStr2szOID(const TCHAR* szAlg, tstring& szAlgOID);
	LONG AlgStr2uiAlg(const TCHAR* szAlg, ALG_ID& uiAlg);


private:
	BOOL  MsgGapFix(const TCHAR* szMsg, tstring& szMsgFix);
    BOOL  SzDN2iMap(const TCHAR* szDN);
	DWORD VerifySign(BYTE* bmsg, ULONG lmsg, BYTE* bsign, ULONG *lsign, DWORD opt);
	DWORD SignData(PCCERT_CONTEXT *pCertContext,BYTE* bSignBuf, ULONG ltoSign, BYTE* bsignedbuf, ULONG *lsigned);	
	BOOL  GetDefaultCSP(tstring &CspName, long lPtype);
	LONG  CertInfoBackup(tstring szProvName, tstring szCtnName, tstring szPubkeyHash, DWORD dKeySpec, DWORD dProvType);
	LONG  CertInfoRecovery(tstring &szPubkeyHash, tstring &szProvName, tstring &szCtnName, DWORD &dKeySpec, DWORD &dProvType);
	LONG  CertInfoFind(uchar *ucmodulus, ulong ulmodlen, tstring &szProvName, tstring &szCtnName, DWORD &dKeySpec, DWORD &dProvType);
	LONG  X509CNStuff(CERT_NAME_BLOB &certNameBlob);
    LONG  X509DNStuff(CERT_NAME_BLOB &certNameBlob);
    void  FlipBuffer(uchar *pBuf, ulong ulLen);
	void  MemoryReverse(PBYTE &pData,DWORD dwLen);
    BOOL  EnumCSP(const ulong lType, Tstrlist &vcCspList);
    LONG  EnumCtn();
	BOOL PemHandle(const TCHAR* szPem, tstring& szB64);
private:
	tstring HashPubKeyInfo(HCRYPTPROV hProv, CERT_PUBLIC_KEY_INFO *keyinfo);
	LONG    GetPubkeyModulus(PCERT_PUBLIC_KEY_INFO pPubkeyInfo, uchar *cert_modulus, ulong *modulus_len);

private:
	BOOL	m_bUseRegEnv;
	DWORD	m_lGenKeyFlag; //length,exportable,protected,etc
	DWORD	m_lKeySpec; //AT_KEYEXCHANGE //AT_SIGNATURE
	ALG_ID	m_iEncAlg;
	ALG_ID	m_iHashAlg;
    std::string  m_sReqSignAlgOid;   //this must ansi

	DWORD	m_lProvType;
	BOOL	m_bReqUsingExitKeySet;
	BOOL	m_bReqWriteCert2Csp;
	
	tstring m_sCspName;
	tstring m_sCtnName;
	tstring m_sReqCtnName;
    tstring m_sReqRegKey;
	tstring m_sCertReqCN;
	tstring m_sCertReqDN;

	Tstrlist m_CspList;
	Tstrlist m_CtnList;
    Tcerlist m_CspCertList;
    Tcerlist m_MyStoreCertList;
    Trdnmap  m_RdnMap;

	HCRYPTPROV m_hProv;
	HCRYPTKEY  m_hPrivKey;
	HCERTSTORE m_hStore;
	//CERT_CONTEXT *m_pCert;
    ReleasePtrs   m_Ptrs;
};

#endif // !defined(AFX_CERTENROLLCSP_H__49BBAB4B_4E80_418C_AEAC_F3036FCF400D__INCLUDED_)
