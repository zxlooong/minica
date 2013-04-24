//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <tchar.h>
#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <cryptuiapi.h>


//#include <WinError.h>
#include "CBase64.h"
#include "MemTools.h"
#include "Certificate.h"
#include <assert.h>

#pragma comment(lib, "Crypt32.lib")
//#pragma comment(lib, "Rpcrt4.lib")
//#pragma comment(lib, "Cryptui.lib")

using namespace MemTools;



//////////////////////////////////////////////////////////////////////////
CCertificate::CCertificate()
{
    m_pCertContext = NULL;

	Reset();
}


CCertificate::~CCertificate()
{
    if(m_pCertContext)
        CertFreeCertificateContext(m_pCertContext);
	if (m_hProv)
		CryptReleaseContext(m_hProv, 0);
}

CCertificate::CCertificate(PCCERT_CONTEXT context)
{
    m_pCertContext = NULL;

    if(context)
        m_pCertContext = CertDuplicateCertificateContext(context);

	Reset();
}

CCertificate::CCertificate(const CCertificate &cert)
{
    m_pCertContext = NULL;

    m_pCertContext = CertDuplicateCertificateContext(cert.m_pCertContext);

    m_sCtnName	= cert.m_sCtnName;
    m_sProvName = cert.m_sProvName;
    m_iHashAlg	= cert.m_iHashAlg;
    m_lProvType = cert.m_lProvType;
    m_lKeySpec	= cert.m_lKeySpec;
    m_hProv		= NULL;
}


CCertificate::CCertificate(const TCHAR* szCert)
{
    m_pCertContext = NULL;
    BYTE*	cert;
    DWORD	certLen;
    CByteBuf buf(_tcslen(szCert)*2);

    do 
    {
        if (0 != CBase64::IsBase64String(szCert))
            break;

        CBase64::Decode(szCert, buf.ptr, &certLen);
        cert = buf.ptr;

        PCCERT_CONTEXT pCert = CertCreateCertificateContext(XENCODETYPE, cert, certLen);

        if(!pCert) 
            break;

        _Attach(pCert);

    } while (0);

}

LONG 
CCertificate::Reset()
{
	if (m_hProv)
		CryptReleaseContext(m_hProv, 0);

	m_hProv = NULL;
	m_lKeySpec = 0;
	m_lProvType = 1;

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
BOOL 
CCertificate::IsAttached()
{
	if (m_pCertContext)
		return TRUE;
	else
		return FALSE;
}

LONG
CCertificate::_Attach(PCCERT_CONTEXT context)
{
    if (!context)
        return FALSE;
	
    if(m_pCertContext)
        CertFreeCertificateContext(m_pCertContext);
	
    m_pCertContext = context;
	
	Reset();
	
    return TRUE;
}


LONG
CCertificate::Attach(PCCERT_CONTEXT context)
{
    if (!context)
        return FALSE;

    if(m_pCertContext)
        CertFreeCertificateContext(m_pCertContext);

    m_pCertContext = CertDuplicateCertificateContext(context);

	Reset();

    return TRUE;
}

LONG 
CCertificate::Attach(const TCHAR* szCert)
{
    BYTE*	cert;
    DWORD	certLen;
    LONG    bRet = FALSE;

    CByteBuf buf(_tcslen(szCert)*2);

    do 
    {
        if (0 != CBase64::IsBase64String(szCert))
            break;

        CBase64::Decode(szCert, buf.ptr, &certLen);
        cert = buf.ptr;

        PCCERT_CONTEXT pCert=CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, cert, certLen);

        if(!pCert) 
            break;

        _Attach(pCert);

		Reset();

        bRet = TRUE;

    } while (0);

    return bRet;
}


LONG 
CCertificate::Attach(const uchar* szCert, ulong certlen)
{
	if(!szCert)
		return ERROR_INVALID_PARAMETER;

    LONG  lRet = FALSE;
	
    do 
    {
        PCCERT_CONTEXT pCert=CertCreateCertificateContext( 
						X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, szCert, certlen);
		
        if(!pCert) 
            break;
		
        _Attach(pCert);
		
		Reset();
		
        lRet = TRUE;
		
    } while (0);
	
    return lRet;
}




PCCERT_CONTEXT 
CCertificate::Detach()
{
    //!!!!!!! check null
    PCCERT_CONTEXT pCert=m_pCertContext;
    m_pCertContext=NULL;
    return pCert;
}


LONG 
CCertificate::SetContextProperty(TCHAR* szProvName, TCHAR* szCtnName, DWORD lProvType, DWORD lKeySpec)
{
    if(!m_pCertContext) 
        return E_POINTER;
	if (!szProvName || !szCtnName )
		return ERROR_INVALID_PARAMETER;
	LONG lRet = ERROR_SUCCESS;

	m_sProvName = szProvName;
	m_sCtnName = szCtnName;	
	m_lProvType = lProvType;
	m_lKeySpec = lKeySpec;

	CRYPT_KEY_PROV_INFO keyProvInfo;
#if defined(UNICODE) || defined(_UNICODE)
	keyProvInfo.pwszProvName = (TCHAR*)m_sProvName.c_str();
	keyProvInfo.pwszContainerName = (TCHAR*)m_sCtnName.c_str();	
#else
	CA2WCharBuf bufProvName(m_sProvName.c_str());
	CA2WCharBuf bufCtnName(m_sCtnName.c_str());
	//TODO: ......... buf don't release ?
	keyProvInfo.pwszProvName = bufProvName.ptr;
	keyProvInfo.pwszContainerName = bufCtnName.ptr;
#endif

	keyProvInfo.dwProvType = m_lProvType;
	keyProvInfo.dwFlags = 0;
	keyProvInfo.cProvParam = 0;
	keyProvInfo.rgProvParam  = NULL;
	keyProvInfo.dwKeySpec  = m_lKeySpec;

	lRet = CertSetCertificateContextProperty( m_pCertContext, CERT_KEY_PROV_INFO_PROP_ID, 
										CERT_STORE_NO_CRYPT_RELEASE_FLAG, &keyProvInfo );
	lRet=lRet?0:GetLastError();
	
	return lRet;
}

LONG 
CCertificate::operator ==(CCertificate &cert)
{
	LONG lRet = ERROR_SUCCESS;
    lRet = CertCompareCertificate(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                      m_pCertContext->pCertInfo, cert.m_pCertContext->pCertInfo);
	lRet=lRet?0:GetLastError();

	return lRet;
}

PCCERT_CONTEXT
CCertificate::GetContext()
{
    PCCERT_CONTEXT context = NULL;

    if(m_pCertContext){
        //CertFreeCertificateContext(m_pCertContext);
        context = CertDuplicateCertificateContext(m_pCertContext);
    }

    return context;
}

LONG
CCertificate::Remove()
{
    if(!m_pCertContext) 
        return E_POINTER;
	LONG lRet = ERROR_SUCCESS;

    lRet = CertDeleteCertificateFromStore(m_pCertContext);
    lRet=lRet?0:GetLastError();

    return lRet;
}



//  system CryptUIDlgViewCertificate functon  ; 
//  older sdk not provide
/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct tagCRYPTUI_VIEWCERTIFICATE_STRUCTW_MY {
    DWORD                       dwSize;
    HWND                        hwndParent;                     // OPTIONAL
    DWORD                       dwFlags;                        // OPTIONAL
    LPCWSTR                     szTitle;                        // OPTIONAL
    PCCERT_CONTEXT              pCertContext;
    LPCSTR *                    rgszPurposes;                   // OPTIONAL
    DWORD                       cPurposes;                      // OPTIONAL
    union
    {
        CRYPT_PROVIDER_DATA const * pCryptProviderData;         // OPTIONAL
        HANDLE                      hWVTStateData;              // OPTIONAL
    };
    BOOL                        fpCryptProviderDataTrustedUsage;// OPTIONAL
    DWORD                       idxSigner;                      // OPTIONAL
    DWORD                       idxCert;                        // OPTIONAL
    BOOL                        fCounterSigner;                 // OPTIONAL
    DWORD                       idxCounterSigner;               // OPTIONAL
    DWORD                       cStores;                        // OPTIONAL
    HCERTSTORE *                rghStores;                      // OPTIONAL
    DWORD                       cPropSheetPages;                // OPTIONAL
    LPCPROPSHEETPAGEW           rgPropSheetPages;               // OPTIONAL
    DWORD                       nStartPage;
} CRYPTUI_VIEWCERTIFICATE_STRUCTW_MY, *PCRYPTUI_VIEWCERTIFICATE_STRUCTW_MY;
typedef const CRYPTUI_VIEWCERTIFICATE_STRUCTW_MY *PCCRYPTUI_VIEWCERTIFICATE_STRUCTW_MY;

typedef struct tagCRYPTUI_VIEWCERTIFICATE_STRUCTA_MY {
    DWORD                       dwSize;
    HWND                        hwndParent;                     // OPTIONAL
    DWORD                       dwFlags;                        // OPTIONAL
    LPCSTR                      szTitle;                        // OPTIONAL
    PCCERT_CONTEXT              pCertContext;
    LPCSTR *                    rgszPurposes;                   // OPTIONAL
    DWORD                       cPurposes;                      // OPTIONAL
    union
    {
        CRYPT_PROVIDER_DATA const * pCryptProviderData;         // OPTIONAL
        HANDLE                      hWVTStateData;              // OPTIONAL
    };
    BOOL                        fpCryptProviderDataTrustedUsage;// OPTIONAL
    DWORD                       idxSigner;                      // OPTIONAL
    DWORD                       idxCert;                        // OPTIONAL
    BOOL                        fCounterSigner;                 // OPTIONAL
    DWORD                       idxCounterSigner;               // OPTIONAL
    DWORD                       cStores;                        // OPTIONAL
    HCERTSTORE *                rghStores;                      // OPTIONAL
    DWORD                       cPropSheetPages;                // OPTIONAL
    LPCPROPSHEETPAGEA           rgPropSheetPages;               // OPTIONAL
    DWORD                       nStartPage;
} CRYPTUI_VIEWCERTIFICATE_STRUCTA_MY, *PCRYPTUI_VIEWCERTIFICATE_STRUCTA_MY;
typedef const CRYPTUI_VIEWCERTIFICATE_STRUCTA_MY *PCCRYPTUI_VIEWCERTIFICATE_STRUCTA_MY;


// BOOL
// WINAPI
// CryptUIDlgViewCertificateW(
// 						   IN  PCCRYPTUI_VIEWCERTIFICATE_STRUCTW   pCertViewInfo,
// 						   OUT BOOL                                *pfPropertiesChanged  // OPTIONAL
// 						   );
// 
// BOOL
// WINAPI
// CryptUIDlgViewCertificateA(
// 						   IN  PCCRYPTUI_VIEWCERTIFICATE_STRUCTA   pCertViewInfo,
// 						   OUT BOOL                                *pfPropertiesChanged  // OPTIONAL
// 						   );

#ifdef UNICODE
//#define CryptUIDlgViewCertificate           CryptUIDlgViewCertificateW
#define PCRYPTUI_VIEWCERTIFICATE_STRUCT_MY     PCRYPTUI_VIEWCERTIFICATE_STRUCTW_MY
#define CRYPTUI_VIEWCERTIFICATE_STRUCT_MY      CRYPTUI_VIEWCERTIFICATE_STRUCTW_MY
#define PCCRYPTUI_VIEWCERTIFICATE_STRUCT_MY    PCCRYPTUI_VIEWCERTIFICATE_STRUCTW_MY
#else
//#define CryptUIDlgViewCertificate           CryptUIDlgViewCertificateA
#define PCRYPTUI_VIEWCERTIFICATE_STRUCT_MY     PCRYPTUI_VIEWCERTIFICATE_STRUCTA_MY
#define CRYPTUI_VIEWCERTIFICATE_STRUCT_MY      CRYPTUI_VIEWCERTIFICATE_STRUCTA_MY
#define PCCRYPTUI_VIEWCERTIFICATE_STRUCT_MY    PCCRYPTUI_VIEWCERTIFICATE_STRUCTA_MY
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////
LONG
CCertificate::ViewDlg()
{
    if(!m_pCertContext) 
        return FALSE;

    LONG lRet = FALSE;
    CRYPTUI_VIEWCERTIFICATE_STRUCTW_MY ViewInfoST;

    ZeroMemory(&ViewInfoST, sizeof(ViewInfoST));
    ViewInfoST.dwSize = sizeof(ViewInfoST);
    ViewInfoST.hwndParent = GetAppParentWindow();
    //ViewInfoST.szTitle = L"Certificate Detail";
    ViewInfoST.pCertContext = m_pCertContext;
    ViewInfoST.cStores = 1; 
    ViewInfoST.rghStores = (HCERTSTORE *) &(ViewInfoST.pCertContext->hCertStore); 

	typedef BOOL (WINAPI *FP_CryptUIDlgViewCertificate)
            (PCRYPTUI_VIEWCERTIFICATE_STRUCTW_MY pCertViewInfo, BOOL pfPropertiesChanged);
	
	HMODULE	hModule=LoadLibrary(_T("CryptUI.dll"));
	if(!hModule)
		return GetLastError();
	FP_CryptUIDlgViewCertificate fpCryptUIDlgViewCertificate=
                (FP_CryptUIDlgViewCertificate)GetProcAddress(hModule,"CryptUIDlgViewCertificateW");
	
    if(!fpCryptUIDlgViewCertificate)
		return GetLastError();
	lRet = (*fpCryptUIDlgViewCertificate)(&ViewInfoST,0);
	FreeLibrary(hModule);

	lRet=lRet?0:GetLastError();

    return lRet;
}


void 
CCertificate::FlipBuffer(uchar *pBuf, ulong ulLen)
{
	if(0 == ulLen)
		return;
	
	unsigned char ucTemp;
	for(unsigned long i = 0; i < ulLen >> 1; ++i)
	{
		ucTemp = pBuf[i];
		pBuf[i] = pBuf[ulLen - i - 1];
		pBuf[ulLen - i - 1] = ucTemp;
	}
}

LONG
CCertificate::GetModulus(uchar *cert_modulus, ulong *modulus_len)
{
	if(!m_pCertContext)
		return ERROR_INVALID_HANDLE;
	if(!cert_modulus)
		return ERROR_INVALID_PARAMETER;
	
	BYTE	*pData = NULL;
	UINT	len_len = 0;
	short	lenMod = 0;
	long	mode_len = 0;
	
	
	CRYPT_BIT_BLOB &blob = m_pCertContext->pCertInfo->SubjectPublicKeyInfo.PublicKey;
	pData = blob.pbData;
	if(*pData != 0x30){
		return ERROR_INVALID_HANDLE;
	}

	pData += 1;
	if(*pData > 0x80){
		len_len = *pData & 0x7f;
		pData += 1;
		pData += len_len;
	}
	else
		pData += 1;
	
	if(*pData != 0x02){
		return ERROR_INVALID_HANDLE;
	}
	pData += 1;
	if(*pData > 0x80)
	{
		//*pData > 0x80,so len_len > 0
		len_len = *pData & 0x7f;
		pData += 1;
		if(len_len == 1){
			mode_len = *pData;
		}
		else{
			FlipBuffer(pData,len_len);
			memcpy(&lenMod,pData,len_len);
			mode_len = lenMod;			
		}
		pData += len_len;
	}
	else
	{
		mode_len = *pData;
		pData += 1;
	}
	
	//抠掉开头的0x00？
	ulong ulModulusLen = 0;
	if(*pData == 0x00)
	{
		pData += 1;
		ulModulusLen = mode_len-1;		
	}
	else
		ulModulusLen = mode_len;
	
	if(cert_modulus == NULL)
	{
		*modulus_len = ulModulusLen;
	}
	else
	{
		if(*modulus_len < ulModulusLen)
		{
			return ERROR_INVALID_HANDLE;
		}
		*modulus_len = ulModulusLen;
		memcpy(cert_modulus,pData,ulModulusLen);
	}
	return ERROR_SUCCESS;
}




LONG
CCertificate::GetEncode(tstring& szCert)
{
    if(!m_pCertContext) 
        return FALSE;



    return TRUE;
}


CCertificate 
CCertificate::GetParentCert()
{
    HCERTSTORE hStore=CertOpenSystemStore(NULL,_T("CA"));

    if (NULL == hStore)
	{
        hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0,NULL, 
            CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_READONLY_FLAG, L"CA");
    }

    DWORD Flags=0;
    CCertificate cert;
	if (m_pCertContext)
	{
		PCCERT_CONTEXT bufcertcontext = ::CertGetIssuerCertificateFromStore(hStore,m_pCertContext,NULL,&Flags);
		cert.Attach(bufcertcontext);
	}

    CertCloseStore(hStore,0);

    return cert;
}

LONG 
CCertificate::SignDataP7(const uchar* toSign, int toSignLen, ulong opt, uchar* outsign, ulong& outlen)
{
	if (!m_pCertContext)
		return ERROR_INVALID_HANDLE;
	if (!toSign)
        return ERROR_INVALID_PARAMETER;

    HCRYPTHASH hHash = NULL;
    LONG lret = ERROR_SUCCESS;
    DWORD dwSigLen= 0;

    do 
    {
		if (!m_hProv)
		{
			if (0 == m_sProvName.length()){
				if(ERROR_SUCCESS != GetProvName(m_sProvName)){
					lret = GetLastError();
					break;
				}
			}

			if (0 == m_sCtnName.length()){
				if (ERROR_SUCCESS != GetCtnName(m_sCtnName)){
					lret = GetLastError();
					break;
				}
			}

			if (0 == m_lKeySpec){
				if (ERROR_SUCCESS != GetKeySpec(m_lKeySpec)){
					lret = GetLastError();
					break;
				}
			}
	
			if (!CryptAcquireContext(&m_hProv, m_sCtnName.c_str(), m_sProvName.c_str(), m_lProvType, NULL)){
				lret = GetLastError();
				break;
			}
		}




		if(!CryptCreateHash(m_hProv, m_iHashAlg, 0, 0, &hHash)){
            lret = GetLastError();
            break;
        }

        if(!CryptHashData(hHash, toSign, toSignLen, 0)){
            lret = GetLastError();
            break;
        }
// 
//         if(!CryptSignHash(hHash, m_lKeySpec, NULL, 0, NULL, &dwSigLen)) {
//             lret = GetLastError();
//             break;
//         }

		CByteBuf buf;
		DWORD len=4096;
		if (!CryptGetHashParam(hHash, HP_HASHVAL, buf.ptr, &len, 0))
		{
			lret = GetLastError();
			break;
		}


		CRYPT_DATA_BLOB orgBlob;
		memset(&orgBlob, 0, sizeof(orgBlob));
		orgBlob.cbData = len;
		orgBlob.pbData = buf.ptr;

		SigMsgStuff(orgBlob);

// 		outlen = dwSigLen;
// 		if (NULL != outsign)
// 		{
// 			CByteBuf hashbuf(dwSigLen);
// 			if(!CryptSignHash(hHash, m_lKeySpec, NULL, 0, hashbuf.ptr, &dwSigLen)){
// 				lret = GetLastError();
// 				break;
// 		    }  
// 			memcpy(outsign, hashbuf.ptr, dwSigLen);
// 		}



        lret = ERROR_SUCCESS;

    } while (0);

    if(hHash) 
        CryptDestroyHash(hHash);

    return lret;
}

LONG 
CCertificate::SignData(const uchar* toSign, int toSignLen, ulong opt, tstring& szSignOut)
{
	LONG lRet = ERROR_SUCCESS;
	ulong outlen;

	lRet = SignData(toSign, toSignLen, opt, NULL, outlen);
	if(ERROR_SUCCESS != lRet)
		return lRet;

	CByteBuf signoutbuf(outlen+100);
	lRet = SignData(toSign, toSignLen, opt, signoutbuf.ptr, outlen);
	if(ERROR_SUCCESS != lRet)
		return lRet;
	
	CTCharBuf szSignBuf(outlen);
	unsigned long ullen;
	CBase64::Encode(signoutbuf.ptr, outlen, szSignBuf.ptr, &ullen);
	
	szSignOut = szSignBuf.ptr;

	return lRet;
}

// HCRYPTMSG WINAPI CryptMsgOpenToEncode(
//     __in      DWORD dwMsgEncodingType,
//     __in      DWORD dwFlags,
//     __in      DWORD dwMsgType,
//     __in      const void *pvMsgEncodeInfo,
//     __in_opt  LPSTR pszInnerContentObjID,
//     __in      PCMSG_STREAM_INFO pStreamInfo
//     );


LONG CCertificate::SigMsgStuff(CRYPT_DATA_BLOB& orgBlob)
{
    LONG lRet = ERROR_SUCCESS;

	// 准备数据
	//CRYPT_DATA_BLOB orgBlob;
	//memset(&orgBlob, 0, sizeof(orgBlob));
	// 设置签名算法
	CRYPT_ALGORITHM_IDENTIFIER HashAlgorithm;
	memset(&HashAlgorithm, 0, sizeof(HashAlgorithm));
	HashAlgorithm.pszObjId = szOID_RSA_SHA1RSA;

	// 设置签名编码参数
	CMSG_SIGNER_ENCODE_INFO SignerEncodeInfo;
	CMSG_SIGNER_ENCODE_INFO SignerEncodeInfoArray[1];
	memset(&SignerEncodeInfo, 0, sizeof(CMSG_SIGNER_ENCODE_INFO));
	SignerEncodeInfo.cbSize = sizeof(CMSG_SIGNER_ENCODE_INFO);
	SignerEncodeInfo.pCertInfo = m_pCertContext->pCertInfo;
	SignerEncodeInfo.hCryptProv = m_hProv;
	SignerEncodeInfo.dwKeySpec = AT_KEYEXCHANGE;
	SignerEncodeInfo.HashAlgorithm = HashAlgorithm;
	SignerEncodeInfo.pvHashAuxInfo = NULL;
	SignerEncodeInfoArray[0] = SignerEncodeInfo;

	// 设置签名证书
	CERT_BLOB SignerCertBlob;
	CERT_BLOB SignerCertBlobArray[1];
	SignerCertBlob.cbData = m_pCertContext->cbCertEncoded;
	SignerCertBlob.pbData = m_pCertContext->pbCertEncoded;
	SignerCertBlobArray[0] = SignerCertBlob;

	// 设置签名编码参数
	CMSG_SIGNED_ENCODE_INFO SignedMsgEncodeInfo;
	memset(&SignedMsgEncodeInfo, 0, sizeof(CMSG_SIGNED_ENCODE_INFO));
	SignedMsgEncodeInfo.cbSize = sizeof(CMSG_SIGNED_ENCODE_INFO);
	SignedMsgEncodeInfo.cSigners = 1;
	SignedMsgEncodeInfo.rgSigners = SignerEncodeInfoArray;
	SignedMsgEncodeInfo.cCertEncoded = 1;
	SignedMsgEncodeInfo.rgCertEncoded = SignerCertBlobArray;
	SignedMsgEncodeInfo.cCrlEncoded = 0;
	SignedMsgEncodeInfo.rgCrlEncoded = NULL;

	// 获取编码输出的签名块长度
	CRYPT_DATA_BLOB sigBlob;
	memset(&sigBlob, 0, sizeof(sigBlob));

	// 编码签名消息
	HCRYPTMSG hMsg;

	do 
	{
		sigBlob.cbData = CryptMsgCalculateEncodedLength( XENCODETYPE, 0, CMSG_SIGNED, &SignedMsgEncodeInfo, NULL, orgBlob.cbData);
// 		if(sigBlob.pbData == NULL)
// 			break;

		CByteBuf sigBuf(sigBlob.cbData);
		sigBlob.pbData = sigBuf.ptr;     //(BYTE *) new char[sigBlob.cbData];


		hMsg = CryptMsgOpenToEncode(
									XENCODETYPE,
									0,        
									CMSG_SIGNED,   
									&SignedMsgEncodeInfo,
									NULL,               
									NULL);             
		if(hMsg == NULL)
			break;

		if(!CryptMsgUpdate(
			hMsg,                // handle to the message
			orgBlob.pbData,      // pointer to the content
			orgBlob.cbData,      // size of the content
			TRUE))               // last call
		{
			break;;
		}

		// 获取签名结果
		if(!CryptMsgGetParam(
			hMsg,                        // handle to the message
			CMSG_CONTENT_PARAM,            // parameter type
			0,                            // index
			sigBlob.pbData,                // pointer to the BLOB
			&sigBlob.cbData))            // size of the BLOB
		{
			break;
		}

	} while (0);


	lRet = GetLastError();

// 	if(hMsg != NULL)
// 	{
// 		CryptMsgClose(hMsg);
// 		hMsg = NULL;
// 	}



//     HCRYPTMSG msg;
//     CMSG_SIGNED_ENCODE_INFO signedinof;
//     CMSG_STREAM_INFO streaminfo;
//     msg = CryptMsgOpenToEncode(XENCODETYPE, 0, CMSG_SIGNED, szOID_RSA_signedData, &streaminfo);


    return lRet;
}



LONG 
    CCertificate::SignData(const uchar* toSign, int toSignLen, ulong opt, uchar* outsign, ulong& outlen)
{
    if (!m_pCertContext)
        return ERROR_INVALID_HANDLE;
    if (!toSign)
        return ERROR_INVALID_PARAMETER;

    HCRYPTHASH hHash = NULL;
    LONG lret = ERROR_SUCCESS;
    DWORD dwSigLen= 0;

    do 
    {
        if (!m_hProv)
        {
            if (0 == m_sProvName.length()){
                if(ERROR_SUCCESS != GetProvName(m_sProvName)){
                    lret = GetLastError();
                    break;
                }
            }

            if (0 == m_sCtnName.length()){
                if (ERROR_SUCCESS != GetCtnName(m_sCtnName)){
                    lret = GetLastError();
                    break;
                }
            }

            if (0 == m_lKeySpec){
                if (ERROR_SUCCESS != GetKeySpec(m_lKeySpec)){
                    lret = GetLastError();
                    break;
                }
            }

            if (!CryptAcquireContext(&m_hProv, m_sCtnName.c_str(), m_sProvName.c_str(), m_lProvType, NULL)){
                lret = GetLastError();
                break;
            }
        }

        if(!CryptCreateHash(m_hProv, m_iHashAlg, 0, 0, &hHash)){
            lret = GetLastError();
            break;
        }

        if(!CryptHashData(hHash, toSign, toSignLen, 0)){
            lret = GetLastError();
            break;
        }

        if(!CryptSignHash(hHash, m_lKeySpec, NULL, 0, NULL, &dwSigLen)) {
            lret = GetLastError();
            break;
        }

        outlen = dwSigLen;
        if (NULL != outsign)
        {
            CByteBuf hashbuf(dwSigLen);
            if(!CryptSignHash(hHash, m_lKeySpec, NULL, 0, hashbuf.ptr, &dwSigLen)){
                lret = GetLastError();
                break;
            }  
            memcpy(outsign, hashbuf.ptr, dwSigLen);
        }

        lret = ERROR_SUCCESS;

    } while (0);

    if(hHash) 
        CryptDestroyHash(hHash);

    return lret;
}




LONG 
    CCertificate::SignDataP7(const uchar* toSign, int toSignLen, ulong opt, tstring& szSignOut)
{
    LONG lRet = ERROR_SUCCESS;
    ulong outlen;

    lRet = SignDataP7(toSign, toSignLen, opt, NULL, outlen);
    if(ERROR_SUCCESS != lRet)
        return lRet;

    CByteBuf signoutbuf(outlen);
    lRet = SignDataP7(toSign, toSignLen, opt, signoutbuf.ptr, outlen);
    if(ERROR_SUCCESS != lRet)
        return lRet;

    CTCharBuf szSignBuf(outlen);
    unsigned long ullen;
    CBase64::Encode(signoutbuf.ptr, outlen, szSignBuf.ptr, &ullen);

    szSignOut = szSignBuf.ptr;

    return lRet;
}

LONG
    CCertificate::SignMessage(const uchar* toSign, int toSignLen, ulong opt, uchar* outsign, ulong& outlen)
{
    if (!m_pCertContext)
        return ERROR_INVALID_HANDLE;
    LONG bDetached = FALSE;
    LONG lRet = ERROR_SUCCESS;

    CRYPT_SIGN_MESSAGE_PARA SigParams;
    std::vector<PCCERT_CONTEXT> mySignerCertChain;
    PCCERT_CONTEXT signer = m_pCertContext;
    mySignerCertChain.push_back(signer);
    SigParams.cMsgCert = 1;


    PCCERT_CONTEXT tmpCertContext = CCertificate(m_pCertContext).GetParentCert().Detach();
    while(tmpCertContext) 
    {
        mySignerCertChain.push_back(tmpCertContext);
        SigParams.cMsgCert++;
        tmpCertContext =  CCertificate(tmpCertContext).GetParentCert().Detach();
    }

    SigParams.rgpMsgCert = &mySignerCertChain[0];
    SigParams.cbSize = sizeof(CRYPT_SIGN_MESSAGE_PARA);
    SigParams.dwMsgEncodingType = X509_ASN_ENCODING | PKCS_7_ASN_ENCODING;
    SigParams.pSigningCert = m_pCertContext;
    SigParams.HashAlgorithm.pszObjId = szOID_RSA_SHA1RSA;
    SigParams.HashAlgorithm.Parameters.cbData = NULL;
    SigParams.pvHashAuxInfo=NULL;
    SigParams.cMsgCrl = 0;
    SigParams.cAuthAttr = 0;
    SigParams.rgAuthAttr = NULL;
    SigParams.cUnauthAttr = 0;
    SigParams.dwFlags = 0;
    SigParams.dwInnerContentType = 0;

    do 
    {
        unsigned long oSize;
        unsigned long iSize = toSignLen;
        lRet=CryptSignMessage(&SigParams,bDetached,1,&toSign,&iSize,NULL,&oSize);
        lRet=lRet?0:GetLastError();

        if(lRet!=0)
            break;

        outlen =  oSize;
        if (NULL != outsign)
        {
            CByteBuf signout(oSize);
            lRet=CryptSignMessage(&SigParams,bDetached,1,&toSign,&iSize,signout.ptr,&oSize);
            lRet=lRet?0:GetLastError();

            if(lRet!=0)
                break;

            memcpy(outsign, signout.ptr, oSize);
        }

    } while (0);

    std::vector<const CERT_CONTEXT*>::iterator it; 
    for(it = mySignerCertChain.begin(); it != mySignerCertChain.end(); it++)
    {
        CertFreeCertificateContext(*it);
    }

    return lRet;
}


LONG
CCertificate::SignMessage(const uchar* toSign, int toSignLen, ulong opt, tstring& szSignOut)
{
	LONG lRet = ERROR_SUCCESS;
	ulong ioutlen;

	lRet = SignMessage(toSign, toSignLen, opt, NULL, ioutlen);
	if(ERROR_SUCCESS != lRet)
		return lRet;

	CByteBuf signoutbuf(ioutlen);
	lRet = SignMessage(toSign, toSignLen, opt, signoutbuf.ptr, ioutlen);
	if (ERROR_SUCCESS != lRet)
		return lRet;

	CTCharBuf szb64(ioutlen*2);
	ulong ulb64len;
	CBase64::Encode(signoutbuf.ptr, ioutlen, szb64.ptr, &ulb64len);
	
	szSignOut = szb64.ptr;

	return lRet;
}


//////////////////////////////////////////////////////////////////////////
LONG 
CCertificate::GetProvName(tstring& szProvName)
{
    if(!m_pCertContext) 
        return E_POINTER;
    
    LONG lRet = ERROR_SUCCESS;
    DWORD size;

    do 
    {
        if(!CertGetCertificateContextProperty(m_pCertContext,CERT_KEY_PROV_INFO_PROP_ID,NULL,&size))
			{ lRet=lRet?0:GetLastError();  break; }

        CByteBuf buf(size);
        if(!CertGetCertificateContextProperty(m_pCertContext,CERT_KEY_PROV_INFO_PROP_ID,buf.ptr,&size))
			{ lRet=lRet?0:GetLastError();  break; }

        CW2TCharBuf w2tBuf(((CRYPT_KEY_PROV_INFO*)buf.ptr)->pwszProvName);
        szProvName = w2tBuf.ptr;

        m_sProvName = szProvName;

    } while (0);

    return lRet;
}

LONG 
CCertificate::SetProvName(const TCHAR* szProvName)
{
    if(!m_pCertContext) 
        return E_POINTER;
	if(!szProvName)
		return ERROR_INVALID_PARAMETER;

	m_sProvName = szProvName;

	return ERROR_SUCCESS;
}


LONG CCertificate::GetCtnName(tstring& szCtnName)
{
    if(!m_pCertContext) 
        return E_POINTER;

    LONG lRet = ERROR_SUCCESS;
    DWORD size;

    do 
    {
        if(!CertGetCertificateContextProperty(m_pCertContext,CERT_KEY_PROV_INFO_PROP_ID,NULL,&size))
        { lRet = GetLastError(); break; }

        CByteBuf buf(size);
        CertGetCertificateContextProperty(m_pCertContext,CERT_KEY_PROV_INFO_PROP_ID,buf.ptr,&size);

        CW2TCharBuf w2tBuf(((CRYPT_KEY_PROV_INFO*)buf.ptr)->pwszContainerName);
        szCtnName = w2tBuf.ptr;

        m_sCtnName = szCtnName;

        lRet = TRUE;
    } while (0);


    return lRet;
}

LONG 
CCertificate::SetCtnName(const TCHAR* szCtnName)
{
    if(!m_pCertContext) 
        return E_POINTER;
	if(!szCtnName)
		return ERROR_INVALID_PARAMETER;
	
	m_sCtnName = szCtnName;
	
	return ERROR_SUCCESS;	
}


LONG 
CCertificate::GetSubjectRDN(TCHAR* szRdn, tstring& szCertSubjectRDN)
{
    LONG bRet = FALSE;


    return bRet;
}

LONG 
CCertificate::GetIssuerRDN(TCHAR* szRdn, tstring& szCertIssuerRDN)
{
    LONG bRet = FALSE;


    return bRet;
}

LONG 
CCertificate::GetSN(tstring& szSN)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;

    const unsigned char* p=m_pCertContext->pCertInfo->SerialNumber.pbData + 
                           m_pCertContext->pCertInfo->SerialNumber.cbData-1;
    if(*p==0)							//remove leading 0x00
        while(*p==0 && *(p-1)&0x80==0 && p>m_pCertContext->pCertInfo->SerialNumber.pbData)
            p--;
    else if(*p==0xFF)					// remove leading 0xff
        while(*p==0xFF && *(p-1)&0x80 && p>m_pCertContext->pCertInfo->SerialNumber.pbData)
            p--;

    TCHAR* str=new TCHAR[(p-m_pCertContext->pCertInfo->SerialNumber.pbData)*2+3];
    for(TCHAR* pstr=str; p>=m_pCertContext->pCertInfo->SerialNumber.pbData; p--)
        pstr+=_stprintf(pstr,_T("%02x"),*p);

    szSN = str;
	m_sCertSN = str;

    return lRet;
}

LONG 
CCertificate::GetSubjectRdnCN(tstring& szCN)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;

    tstring szSubject;
    GetSubject(szSubject);

    TCHAR*	CNBegin=_tcsstr((TCHAR*)szSubject.c_str(),_T("CN="));
    if(!CNBegin)
        return FALSE;

    CNBegin += 3;
    TCHAR*	CNEnd;
    if(*CNBegin==_T('"'))
    {
        CNBegin++;
        CNEnd=_tcschr(CNBegin,_T('"'));
    }
    else
        CNEnd=_tcschr(CNBegin,_T(','));
    
    if(!CNEnd)
        for(CNEnd=CNBegin;*CNEnd; CNEnd++);

    CTCharBuf buf(CNEnd-CNBegin);
    _tcsncpy(buf.ptr, CNBegin, CNEnd-CNBegin);

    szCN = buf.ptr;
   	m_sCertCN = buf.ptr;

    return lRet;
}

LONG 
CCertificate::GetIssuer(tstring& szIS)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;
 
    DWORD size=CertNameToStr(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                             &m_pCertContext->pCertInfo->Issuer,CERT_X500_NAME_STR,NULL,0);

    CTCharBuf buf(size);
    CertNameToStr(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                     &m_pCertContext->pCertInfo->Issuer,CERT_X500_NAME_STR,buf.ptr,size);

    szIS = buf.ptr;
	m_sCertIssuer = buf.ptr;

    return lRet;
}

LONG 
CCertificate::GetSubject(tstring& szSub)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;

    DWORD size=CertNameToStr(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                    &m_pCertContext->pCertInfo->Subject,CERT_X500_NAME_STR,NULL,0);

    CTCharBuf buf(size);
    CertNameToStr(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                    &m_pCertContext->pCertInfo->Subject,CERT_X500_NAME_STR, buf.ptr,size);

    szSub = buf.ptr;
	m_sCertSubject = buf.ptr;

    return lRet;
}

LONG 
CCertificate::GetValidFrom(SYSTEMTIME& tFrom)
{
    if(!m_pCertContext) return E_POINTER;
    LONG lRet = ERROR_SUCCESS;

    SYSTEMTIME	systime;
    FILETIME ftime;
    FileTimeToLocalFileTime(&m_pCertContext->pCertInfo->NotBefore,&ftime);
    FileTimeToSystemTime(&ftime,&systime);

    tFrom = systime;
	m_tCertFrom = systime;

    return TRUE;
}

LONG 
CCertificate::GetValidTo(SYSTEMTIME& tTo)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;

    SYSTEMTIME	systime;
    FILETIME	ftime;
    FileTimeToLocalFileTime(&m_pCertContext->pCertInfo->NotAfter, &ftime);
    FileTimeToSystemTime(&ftime,&systime);

    tTo = systime;
	m_tCertTo = systime;

    return lRet;
}

LONG 
CCertificate::GetKeySpec(DWORD& lKeySpec)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;
	
	lKeySpec = m_lKeySpec;

	return lRet;
}

LONG 
CCertificate::SetKeySpec(DWORD lKeySpec)
{
    if(!m_pCertContext) return E_POINTER;
    LONG lRet = ERROR_SUCCESS;	

	m_lKeySpec = lKeySpec;

	return lRet;
}

// LONG 
// CCertificate::GetSignAlg(ALG_ID& iAlg)
// {
//     if(!m_pCertContext) return E_POINTER;
//     LONG lRet = ERROR_SUCCESS;
// 	
// 	iAlg = m_iSignAlg;
// 	
// 	return lRet;
// }
// 
// LONG 
// CCertificate::SetSignAlg(ALG_ID iAlg)
// {
//     if(!m_pCertContext) return E_POINTER;
//     LONG lRet = ERROR_SUCCESS;	
// 	
// 	m_iSignAlg = iAlg;
// 	
// 	return lRet;
// }

LONG 
CCertificate::GetHashAlg(ALG_ID& iAlg)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;
	
	iAlg = m_iHashAlg;
	
	return lRet;
}

LONG
CCertificate::SetHashAlg(ALG_ID iAlg)
{
    if(!m_pCertContext) 
        return E_POINTER;
    LONG lRet = ERROR_SUCCESS;	
	
	m_iHashAlg = iAlg;
	
	return lRet;
}

HWND 
CCertificate::GetAppParentWindow()
{
	HWND hWnd=GetForegroundWindow();
	if(!hWnd)	
		return NULL;
	
	DWORD pid;
	GetWindowThreadProcessId(hWnd,&pid);
	if(pid==GetCurrentProcessId())
		return hWnd;
	else 
		return NULL;
}



LONG
CCertificate::CertInfoFind( tstring &szProvName, tstring &szCtnName, DWORD &dKeySpec, DWORD &dProvType )
{
//     if(!m_pCertContext) 
//         return E_POINTER;
// 
//     uchar ucmodulus[4096] = {0}; 
//     ulong ulmodlen =sizeof ucmodulus; 
// 
// 
//     LONG lRet = ERROR_SUCCESS;
//     BOOL bFind = FALSE;
// 
//     //enumcsp
// 
//     HCRYPTPROV hProv = NULL;
//     lRet = EnumCtn();
//     if(lRet) return lRet;
// 
//     if(m_CtnList.size()==0)
//         return ERROR_NO_MORE_ITEMS;
// 
//     for (unsigned int i = 0; i<m_CtnList.size();i++)
//     {
//         lRet = CryptAcquireContext(&hProv, m_CtnList[i].c_str(), m_sCspName.c_str(), m_lProvType, 0);
//         if (!lRet)
//             continue;
// 
//         ulong nPubkeyInfo;
//         lRet = CryptExportPublicKeyInfo(hProv, m_lKeySpec, XENCODETYPE, NULL, &nPubkeyInfo );
//         if (!lRet)
//             continue;
// 
//         CByteBuf bufPubkeyInfo(nPubkeyInfo);
//         lRet = CryptExportPublicKeyInfo(hProv, m_lKeySpec, XENCODETYPE, 
//             (PCERT_PUBLIC_KEY_INFO) bufPubkeyInfo.ptr, &nPubkeyInfo );
//         if (!lRet)
//             continue;
// 
//         uchar ucbuf[4097]={0};
//         ulong ulbuflen = 4096;
//         lRet = GetPubkeyModulus((PCERT_PUBLIC_KEY_INFO)bufPubkeyInfo.ptr, ucbuf, &ulbuflen);
//         if(lRet)
//             continue;
// 
//         if(0 == memcmp(ucbuf, ucmodulus, ulmodlen))
//         {
//             bFind = TRUE;
//             szProvName = m_sCspName;
//             szCtnName = m_CtnList[i].c_str();
//             dKeySpec = m_lKeySpec;
//             dProvType = m_lProvType;
//             break;
//         }
//     }
// 
//     lRet = bFind?ERROR_SUCCESS:GetLastError();
//     return lRet;

    LONG lRet = ERROR_SUCCESS;
    return lRet;
}


LONG
CCertificate::GetPubkeyModulus(uchar *cert_modulus, ulong *modulus_len)
{
	if(!cert_modulus)
		return ERROR_INVALID_PARAMETER;
	
	BYTE	*pData = NULL;
	UINT	len_len = 0;
	short	lenMod = 0;
	long	mode_len = 0;
	
	
	CRYPT_BIT_BLOB &blob = m_pCertContext->pCertInfo->SubjectPublicKeyInfo.PublicKey;
	pData = blob.pbData;
	if(*pData != 0x30){
		return ERROR_INVALID_HANDLE;
	}

	pData += 1;
	if(*pData > 0x80){
		len_len = *pData & 0x7f;
		pData += 1;
		pData += len_len;
	}
	else
		pData += 1;
	
	if(*pData != 0x02){
		return ERROR_INVALID_HANDLE;
	}
	pData += 1;
	if(*pData > 0x80)
	{
		//*pData > 0x80,so len_len > 0
		len_len = *pData & 0x7f;
		pData += 1;
		if(len_len == 1){
			mode_len = *pData;
		}
		else{
			FlipBuffer(pData,len_len);
			memcpy(&lenMod,pData,len_len);
			mode_len = lenMod;			
		}
		pData += len_len;
	}
	else
	{
		mode_len = *pData;
		pData += 1;
	}
	
	//抠掉开头的0x00？
	ulong ulModulusLen = 0;
	if(*pData == 0x00)
	{
		pData += 1;
		ulModulusLen = mode_len-1;		
	}
	else
		ulModulusLen = mode_len;
	
	if(cert_modulus == NULL)
	{
		*modulus_len = ulModulusLen;
	}
	else
	{
		if(*modulus_len < ulModulusLen)
		{
			return ERROR_INVALID_HANDLE;
		}
		*modulus_len = ulModulusLen;
		memcpy(cert_modulus,pData,ulModulusLen);
	}
	return ERROR_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
