//////////////////////////////////////////////////////////////////////
//	Description:
//		link to openssl libeay
//
//

//////////////////////////////////////////////////////////////////////
#if !defined(INCD_CPELEVEN_BC2968CF_89A1_B9A3_F930_FF97F5E5CDCC3F)
#define INCD_CPELEVEN_BC2968CF_89A1_B9A3_F930_FF97F5E5CDCC3F

//////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include "openssl/asn1.h"
#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/x509v3.h"
#include "openssl/pkcs7.h"
#include "openssl/pkcs12.h"

#include "cryptoki.h"

#if defined(WIN32) || defined(_WINDOWS)
#	include <windows.h>
#else
#	include <dlfcn.h>
#endif


//#include "base_type.h"

//////////////////////////////////////////////////////////////////////////
// == wincrypt32.h
#ifndef KEYSPEC_EXCHANGE
#	define KEYSPEC_EXCHANGE	1
#endif
#ifndef KEYSPEC_SIGNAUTRE
#	define KEYSPEC_SIGNAUTRE	2
#endif

//////////////////////////////////////////////////////////////////////////

#define CKR_P11LIB_INVALID				CKR_VENDOR_DEFINED+1
#define CKR_FUNCLIST_ERROR				CKR_VENDOR_DEFINED+2
#define CKR_CRYPTO_UNINIT				CKR_VENDOR_DEFINED+3
//#define CKR_

typedef unsigned int	       uint;
typedef unsigned long          ulong;
typedef unsigned char	       uchar;
typedef const unsigned char    cuchar;
typedef const char             cchar;


typedef vector<string> strlst;




///////////////////////////////////////////////////////////////////////////////
#define  STATUS_LIB_OK              0x00000001
#define  STATUS_FLIST_OK            0x00000010
#define  STATUS_INITED_OK           0x00000100
#define  STATUS_TOKEN_OK            0x00001000
#define  STATUS_SESSION_OK          0x00010000
#define  STATUS_LOGIN_OK            0x00100000
#define  STATUS_ALL_OK              0x00111111


///////////////////////////////////////////////////////////////////////////

typedef struct _Cert_info
{
    
    
    
} Cert_INFO;






//////////////////////////////////////////////////////////////////////////

class CPEleven
{
public: // Initialize & finalize
	CPEleven();
	~CPEleven();
	CPEleven(cchar* szP11Path);	// cfg or p11 path
	void OsslInit();  //Openssl Initalize
	void OsslFinal(); //Openssl Finalize

	LONG P11Init();
	LONG P11Final();

	LONG SetP11Path(cchar* szPath);
	LONG SetCfgPath(cchar* szPath);
	LONG LoadCfgP11Lib();
	LONG LoadP11Lib(cchar* szPath);

	LONG EnumTokens();
	LONG LoadToken();
	LONG LoadToken(ULONG ulIdx);



	LONG Reset();

public: // Impliments 
	LONG GetTokenCount(ULONG &ulCount);
	LONG SetTokenIdx(ULONG ulIdx);
	LONG GetTokenIdx(ULONG* ulIdx);
	LONG GetTokenSN(string &szUkeySN);
	LONG LoginToken(cuchar* szPass, ULONG ulLen);
	//////////////////////////////////////////////////////////////////////////
	LONG GetCertList(strlst& strlCertList);	// sn
	LONG GetCertCount(ULONG &ulCount);
	//////////////////////////////////////////////////////////////////////////
	LONG GenKeyPair(ULONG* phPubObj, ULONG* phPriObj);
	LONG GenKeyPair(EVP_PKEY* pkey);
	LONG CreateReqP10(string &szP10);
	LONG ReqDNStuff(string &szDN);
	LONG ReqDNStuff(string szOid, string szRdn);
	LONG ReqDNReset();
	//////////////////////////////////////////////////////////////////////////
	LONG InstallP7Cert(cchar* szP7);
	LONG InstallCert(ULONG hPubObj, cuchar* derCert, ULONG cbCertLen);
	LONG InstallCert(cuchar* derCert, ULONG cbCertLen);
	LONG InstallCert(cchar* szCert);
	//////////////////////////////////////////////////////////////////////////
	LONG SignData(uchar* pbToSign, ULONG *cbToSignLen, uchar* pbSignDataOut, ULONG *cbSignDataOutLen);
	LONG SignData(uchar* pbToSign, ULONG *cbToSignLen, string& szSignDataOut);
	LONG SignMessage(uchar* pbToSign, ULONG *cbToSignLen, uchar* pbSignDataOut, ULONG *cbSignDataOutLen);
	LONG SignMessage(uchar* pbToSign, ULONG *cbToSignLen, string& szSignDataOut);
	LONG VerifySignReqDNStuff();
	//////////////////////////////////////////////////////////////////////////
	LONG GetPubKeyNE(ULONG hPubObj, uchar** pbPubkeyNValue, ULONG *cbPubNLen, uchar** pbPubkeyEValue, ULONG *cbPubELen);
	LONG EnumCerts();
	LONG GetTokenStatus();		// Def Macros :  NoP11valid, NoTokenValid, etc.
	LONG FindPubKey(uchar* derCert, ULONG cbCertLen, ULONG* phPubObj);
	LONG GetCertPubMod(X509* x509Cert, uchar* cert_modulus, ULONG* modulus_len);
	LONG GetCertPubMod(uchar* derCert, ULONG cbCertLen, uchar* cert_modulus, ULONG* modulus_len);


private: //properties
	LONG SetReqKeyLen(ULONG ulKeyLen);
	LONG GetReqKeyLen(ULONG &ulKeyLen);
	LONG SetReqKeySpec(ULONG ulKeySpec);
	LONG GetReqKeySpec(ULONG &ulKeySpec);
	LONG SetReqCertValidDays(ULONG ulDays);
	LONG GetReqCertValidDays(ULONG &ulDays);
    LONG SetLastError(ULONG lError);
    LONG GetLastError();

private: //auxiliary
	////////////////////////////////////////
	bool PemHandle(cchar* szPem, string& szB64);
	void FlipBuffer(uchar *pBuf, ULONG ulLen);
	LONG GenGUIDStr(string &szCtnName);
	int  GetUserHome(string &szHome);
	int  DefaultCfgPath(string& szPath);
	int  GetPrivateProfileString(char *fileName, char *sectionName, char *keyName, char *returnedString, int* sizeString);
	int  GetPrivateProfileInt(char *fileName, char *sectionName, char *keyName,  int* ival);
	int  getOneLineString(char *fileName, char *sectionName, char *keyName, char *returnedString, int *psizeString);
	//////////////////////////////////////////////////////////////////////////
	
private: //
	bool m_LibOK;
	bool m_FListOK;
	bool m_TokenOK;
	bool m_bInited;
    
    //lib token session
#if defined(WIN32) || defined(_WINDOWS)
 	HMODULE                 m_hP11Lib;
#else
	void                   *m_hP11Lib;
#endif    
	CK_FUNCTION_LIST_PTR	m_FList;
	CK_SESSION_HANDLE		m_hSession;
	
    //cert & token list 
	CK_SLOT_ID		m_SlotList[16];
	CK_TOKEN_INFO	m_TokenInfoList[16];
	CK_BYTE			m_CertList[16][4096];
	ULONG			m_CertLen[16];
    
    ULONG m_lTokenCount;
	ULONG m_lTokenIdx;
	ULONG m_lCertCount;
	ULONG m_lCertIdx;
    
    //cert req
	ULONG   m_lReqKeyLen;
	ULONG   m_lReqKeySpec;
	ULONG   m_lReqCertValidDays;
	string  m_sReqCtnName;
	strlst m_tsReqRdns;

    
    //p11 path
	string m_szP11Path;
	strlst m_tszP11Paths;
	string m_szCfgPath;
	string m_szCfgSection;
	string m_szCfgKey;

    //user pin
	uchar m_usrPin[32];
	LONG m_usrPinlen;

    //error ret
    DWORD m_lLastError;


};





//////////////////////////////////////////////////////////////////////////

#endif // !defined(INCD_CPELEVEN_BC2968CF_89A1_B9A3_F930_FF97F5E5CDCC3F)
