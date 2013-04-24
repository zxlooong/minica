//////////////////////////////////////////////////////////////////////

#if !defined(F50B3333_5FD4_6DC8_A654_E7D19F14AA4EB3)
#define F50B3333_5FD4_6DC8_A654_E7D19F14AA4EB3


#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable : 4786)


#include <string>
#include <sstream>
#include <vector>

#include <tchar.h>
#include <windows.h>
#include <wincrypt.h>


#include "MemTools.h"

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

#if !defined(XENCODETYPE)
#   define XENCODETYPE			X509_ASN_ENCODING | PKCS_7_ASN_ENCODING
#endif

typedef unsigned char uchar;
typedef unsigned long ulong;

//////////////////////////////////////////////////////////////////////////

class CCertificate
{
public:
    CCertificate();
    CCertificate(const CCertificate &cert);
    CCertificate(PCCERT_CONTEXT context);
    CCertificate(const TCHAR* szCert);
    ~CCertificate();
public:
	BOOL IsAttached();
    LONG Attach(PCCERT_CONTEXT context);
	LONG _Attach(PCCERT_CONTEXT context);
    LONG Attach(const TCHAR* szCert);
	LONG Attach(const uchar* szCert, ulong certlen);
    PCCERT_CONTEXT Detach();
	LONG Reset();
    LONG Remove();
    LONG ViewDlg();
    LONG Valid();
    PCCERT_CONTEXT GetContext();
    LONG operator ==(CCertificate &cert);
	CCertificate GetParentCert();
	LONG SetContextProperty(TCHAR* szProvName, TCHAR* szCtnName, DWORD lProvType, DWORD lKeySpec);
	LONG SignData(const uchar* toSign, int toSignLen, ulong opt, uchar* outsign, ulong& outlen);
    LONG SignData(const uchar* toSign, int toSignLen, ulong opt, tstring& szSignOut);
    LONG SignDataP7(const uchar* toSign, int toSignLen, ulong opt, uchar* outsign, ulong& outlen);
    LONG SignDataP7(const uchar* toSign, int toSignLen, ulong opt, tstring& szSignOut);
	LONG SigMsgStuff(CRYPT_DATA_BLOB& orgBlob);
    LONG SignMessage(const uchar* toSign, int toSignLen, ulong opt, uchar* outsign, ulong& outlen);
    LONG SignMessage(const uchar* toSign, int toSignLen, ulong opt, tstring& szSignOut);    
    LONG ExportPKCS12Dlg();
    LONG ExportPKCS12();
    LONG GetEncode(tstring& szCert);
    LONG IsValid(SYSTEMTIME *pCurTime);
    LONG Encode(tstring& szCert);
	LONG GetModulus(uchar *cert_modulus, ulong *modulus_len);

public:
    LONG GetProvName(tstring& szProvName);
	LONG SetProvName(const TCHAR* szProvName);
    LONG GetCtnName(tstring& szCtnName);
	LONG SetCtnName(const TCHAR* szCtnName);
    LONG GetIssuer(tstring& szIssuer);
    LONG GetSubject(tstring& szSub);
    LONG GetSubjectRdnCN(tstring& szCN);
    LONG GetSubjectRDN(TCHAR* szRdn, tstring& szCertSubjectRDN);
    LONG GetIssuerRDN(TCHAR* szRdn, tstring& szCertIssuerRDN);
    LONG GetSN(tstring& szSN);

    LONG GetValidFrom(SYSTEMTIME& tFrom);
    LONG GetValidTo(SYSTEMTIME& tTo);
    //LONG GetValidFrom(tstring& sztFrom);
    //LONG GetValidTo(tstring& sztTo);

    LONG GetHashAlg(ALG_ID& iAlg);
    LONG SetHashAlg(ALG_ID iAlg);
	LONG GetKeySpec(DWORD& lKeySpec); //SignData use
	LONG SetKeySpec(DWORD lKeySpec);
	LONG GetProvType(DWORD& lProvType);
	LONG SetProvType(DWORD lProvType);

    LONG GetSignatureAlgOid(tstring& szSignatureAlgOid);

//     LONG GetSignAlg(ALG_ID& iAlg);
//     LONG SetSignAlg(ALG_ID iAlg);

public:
    LONG GetPubkeyModulus( uchar *cert_modulus, ulong *modulus_len );
    LONG CertInfoFind( tstring &szProvName, tstring &szCtnName, DWORD &dKeySpec, DWORD &dProvType );
	HWND GetAppParentWindow();
	void FlipBuffer(uchar* pBuf, ulong ulLen);
private:
    PCCERT_CONTEXT m_pCertContext;
	HCRYPTPROV	m_hProv;

	DWORD   m_lProvType;
	DWORD	m_lKeySpec; //enc | sign
    ALG_ID  m_iHashAlg;
    ALG_ID	m_iSignAlg;
    tstring m_sProvName;
    tstring m_sCtnName;
    tstring m_sCertDN;
    tstring m_sCertSN;

    tstring m_sCertCN;
    tstring m_sCertIssuer;
    tstring m_sCertSubject;

    SYSTEMTIME m_tCertFrom;
    SYSTEMTIME m_tCertTo;

//	ReleasePtrs AutoFreePtrs;
};

//////////////////////////////////////////////////////////////////////////

#endif // !defined(F50B3333_5FD4_6DC8_A654_E7D19F14AA4EB3)
