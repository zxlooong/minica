//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Rpc.h>
//#include "dbglog.h"
#include "CBase64.h"
#include "MemTools.h"
#include "Certificate.h"
#include "CertKitCsp.h"


using namespace MemTools;
//////////////////////////////////////////////////////////////////////////
CCertKitCsp::CCertKitCsp()
{	
	Reset();
}

BOOL 
CCertKitCsp::Reset()
{
	//  if not for specific csp, this must set to TRUE;
	m_bUseRegEnv = TRUE; 

	m_hProv = NULL;
	m_hPrivKey = NULL;
	m_hStore = NULL;

	//Microsoft RSA Strong:    default len = 1024bit
	m_lGenKeyFlag = CRYPT_EXPORTABLE | CRYPT_USER_PROTECTED;
	m_lKeySpec = AT_KEYEXCHANGE; //AT_SIGNATURE ;
	m_iEncAlg = CALG_DES;
	m_iHashAlg = CALG_SHA1;

    //support below 3 method:
    //#define szOID_RSA_MD5RSA        "1.2.840.113549.1.1.4"
    //#define szOID_RSA_SHA1RSA       "1.2.840.113549.1.1.5"
    //#define szOID_X957_SHA1DSA      "1.2.840.10040.4.3"
    m_sReqSignAlgOid = szOID_RSA_MD5RSA;

	m_bReqUsingExitKeySet = TRUE;
	m_bReqWriteCert2Csp = TRUE;
	
	m_sReqRegKey = REG_SUB_KEY_TO_WRITE;
	m_sCtnName = GenGUID();	
    m_sReqCtnName = GenGUID();
	
	m_lProvType = PROV_RSA_FULL;
	m_sCertReqCN = SZ_CERT_RDN_VALUE;
    m_CtnList.clear();
    m_CspCertList.clear();
    m_MyStoreCertList.clear();
    m_CspList.clear();
    m_RdnMap.clear();
	
	//RSA Full: Microsoft RSA Strong Cryptographic Provider
	if(!GetDefaultCSP(m_sCspName, m_lProvType))
        m_sCspName = _T("Microsoft RSA Strong Cryptographic Provider");
	
	return TRUE;
}

CCertKitCsp::~CCertKitCsp()
{
	if (m_hPrivKey) 
        CryptDestroyKey(m_hPrivKey);
	if (m_hProv) 
        CryptReleaseContext(m_hProv, 0);
	if (m_hStore) 
        CertCloseStore(m_hStore, 0);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


BOOL 
CCertKitCsp::SetReqRegKey(const TCHAR* szRegKey)
{
	if(szRegKey && _tcslen(szRegKey)>0)
		m_sReqRegKey = szRegKey;
	else
		return FALSE;

	return TRUE;
}

BOOL 
CCertKitCsp::GetReqRegKey(tstring &szRegKey)
{
	szRegKey = m_sReqRegKey;
	return TRUE;
}

BOOL 
CCertKitCsp::SetCspName(const TCHAR* szCspName)
{
	if(szCspName && _tcslen(szCspName)>0)
		m_sCspName = szCspName;
	else
		return FALSE;
	
	return TRUE;	
}

BOOL 
CCertKitCsp::GetCspName(tstring& szCspName)
{
	szCspName = m_sCspName;
	return TRUE;	
}

BOOL 
CCertKitCsp::SetCspType(long lPtype)
{
	m_lProvType = lPtype;
	return TRUE;
}

BOOL 
CCertKitCsp::GetCspType(long& lPtype)
{
	lPtype = m_lProvType;
	return TRUE;
}

BOOL 
CCertKitCsp::SetCtnName(const TCHAR* szCtnName)
{
	if(szCtnName && _tcslen(szCtnName)>0)
		m_sCtnName = szCtnName;
	else
		return false;
	
	return TRUE;	
}

BOOL 
CCertKitCsp::GetCtnName(tstring& szCtnName)
{
    szCtnName = m_sCtnName;

    return TRUE;		
}

BOOL 
CCertKitCsp::GetReqCtnName(tstring& szCtnName)
{
	szCtnName = m_sReqCtnName;

	return TRUE;		
}

BOOL 
CCertKitCsp::SetReqCtnName(const TCHAR* szCtnName)
{
    if(szCtnName && _tcslen(szCtnName)>0)
        m_sReqCtnName = szCtnName;
    else
        return false;

    return TRUE;	
}

BOOL 
CCertKitCsp::SetHashAlg(ALG_ID iAlgID)
{
	m_iHashAlg = iAlgID;
	return TRUE;	
}

BOOL 
CCertKitCsp::GetHashAlg(ALG_ID& iAlgID)
{
	iAlgID = m_iHashAlg;
	return TRUE;
}


BOOL CCertKitCsp::SetReqSignAlgOid(const TCHAR* szAlgOID)
{
    if(szAlgOID && _tcslen(szAlgOID)>0)
    {
        CT2ACharBuf buf(szAlgOID);
        m_sReqSignAlgOid = buf.ptr;
    }
    else
        return FALSE;

    return TRUE;
}

BOOL CCertKitCsp::GetReqSignAlgOid(tstring& szAlgOID)
{
    CA2TCharBuf buf(m_sReqSignAlgOid.c_str());
    szAlgOID = buf.ptr;
    return TRUE;
}


BOOL 
CCertKitCsp::SetCryptAlg(ALG_ID iAlg)
{
	m_iEncAlg = iAlg;
	return TRUE;
}

BOOL 
CCertKitCsp::GetCryptAlg(ALG_ID& iAlg)
{
	iAlg = m_iEncAlg;
	return TRUE;
}

BOOL 
CCertKitCsp::SetKeySpec(long lKeySpec)
{
	m_lKeySpec = lKeySpec;
	return TRUE;
}

BOOL 
CCertKitCsp::GetKeySpec(long& lKeySpec)
{
	lKeySpec = m_lKeySpec;
	return TRUE;
}

BOOL 
CCertKitCsp::SetGenKeyFlag(long lFlag)
{
	m_lGenKeyFlag = lFlag;
	return TRUE;	
}

BOOL 
CCertKitCsp::GetGenKeyFlag(long& lFlag)
{
	lFlag = m_lGenKeyFlag;
	return TRUE;
}

BOOL 
CCertKitCsp::SetBWriteCert2Csp(BOOL bTrue)
{
	m_bReqWriteCert2Csp = bTrue;
	return TRUE;
}

BOOL 
CCertKitCsp::GetBWriteCert2Csp(BOOL& bTrue)
{
	bTrue = m_bReqWriteCert2Csp;
	return TRUE;
}

BOOL 
CCertKitCsp::SetBUsingExitingKeySet(BOOL bTrue)
{
    m_bReqUsingExitKeySet = bTrue;	
    return TRUE;
}

BOOL 
CCertKitCsp::GetBUsingExitingKeySet(BOOL& bTrue)
{
	bTrue = m_bReqUsingExitKeySet;
	return TRUE;
}

BOOL
CCertKitCsp::SetBUseReqRegEnv(BOOL bTrue)
{
	m_bUseRegEnv = bTrue;
	return TRUE;
}

BOOL
CCertKitCsp::GetBUseReqRegEnv(BOOL& bTrue)
{
	bTrue = m_bUseRegEnv;
	return TRUE;
}


BOOL
CCertKitCsp::GetCtnList(Tstrlist &vcCtnList)
{
	LONG lRet;
	BOOL bRet;
	lRet = EnumCtn();
	bRet = lRet ? FALSE: TRUE;

	vcCtnList = m_CtnList;

	return bRet;
}

BOOL 
CCertKitCsp::SetCertReqCN(const TCHAR* szCertRdn)
{
    if(szCertRdn && _tcslen(szCertRdn)>0)
        m_sCertReqCN = szCertRdn;
    else
        return FALSE;

    return TRUE;	
}

BOOL 
CCertKitCsp::GetCertReqCN(tstring& szCertRdnName)
{
    szCertRdnName = m_sCertReqCN;
    return TRUE;
}

BOOL 
CCertKitCsp::SetCertReqRdnName(const TCHAR* szRdntype, const TCHAR* szCertRdn)
{

    return TRUE;
}

BOOL 
CCertKitCsp::GetCertReqRdnName(const TCHAR* szRdntype, tstring& szCertRdnName)
{

    return TRUE;
}

BOOL CCertKitCsp::GetCertReqDnName(tstring& szCertDnName)
{
	szCertDnName = m_sCertReqDN;

	return TRUE;
}
BOOL CCertKitCsp::SetCertReqDnName(const TCHAR* szCertDnName)
{
	//"cn =asdfasdf, M=asdfasdf"
	//std::map<tstring> tRdnList;

	//TODO: clip sz 2 list, & clip every list item'rdn sz, & turn rdn sz 2 oid


	return TRUE;
}

BOOL  CCertKitCsp::MsgGapFix(const TCHAR* szMsg, tstring& szMsgFix)
{
	// \r or \n  ---> \r\n

    szMsgFix = _T("");
    tstring szToFixBuf = szMsg;
    int iToFixLen = _tcslen(szMsg);   
    int ipos = 0;
    for(int i=0; i<iToFixLen; i++){
        if (    (_T('\n') == szToFixBuf[i]   || _T('\r') == szToFixBuf[i])
			&& !(_T('\r') == szToFixBuf[i-1] && _T('\n') == szToFixBuf[i])
			&& !(_T('\n') == szToFixBuf[i+1] && _T('\r') == szToFixBuf[i]))
        {
            szMsgFix += szToFixBuf.substr(ipos, i-ipos);
            ipos = i+1;
            szMsgFix += _T("\r\n");
        }
    }

    if(ipos<iToFixLen)
        szMsgFix += szToFixBuf.substr(ipos,iToFixLen-ipos+1);
    //MessageBox(NULL, szToSign.c_str(), NULL, MB_OK);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


long CCertKitCsp::AlgStr2uiAlg(const TCHAR* szAlg, ALG_ID& uiAlg)
{
	if (_tccmp(szAlg, _T("DES")))
	{ uiAlg = CALG_DES; return S_OK; }

	//hash
	else if (_tccmp(szAlg, _T("MD5")) || _tccmp(szAlg, _T("md5")))
	{ uiAlg = CALG_MD5; return S_OK; }

	else if (_tccmp(szAlg, _T("SHA1")) || _tccmp(szAlg, _T("sha1")))
	{ uiAlg = CALG_SHA1; return S_OK; }


	//symmetrical
	else if (_tccmp(szAlg, _T("DES")))
	{ uiAlg = CALG_DES; return S_OK; }

	else if (_tccmp(szAlg, _T("AES")))
	{ uiAlg = CALG_AES; return S_OK; }

	else if (_tccmp(szAlg, _T("3DES")))
	{ uiAlg = CALG_3DES; return S_OK; }

	//asymmetric


	else
		return S_FALSE;
}

LONG CCertKitCsp::AlgStr2szOID(const TCHAR* szAlg, tstring& szAlgOID)
{
//     if (_tccmp(szAlg, _T("DES")))
//     { szAlgOID = CALG_DES; return S_OK; }
// 
//     //hash
//     else if (_tccmp(szAlg, _T("MD5")) || _tccmp(szAlg, _T("md5")))
//     { szAlgOID = CALG_MD5; return S_OK; }
// 
//     else if (_tccmp(szAlg, _T("SHA1")) || _tccmp(szAlg, _T("sha1")))
//     { szAlgOID = CALG_SHA1; return S_OK; }
// 
// 
//     //symmetrical
//     else if (_tccmp(szAlg, _T("DES")))
//     { szAlgOID = CALG_DES; return S_OK; }
// 
//     else if (_tccmp(szAlg, _T("AES")))
//     { szAlgOID = CALG_AES; return S_OK; }
// 
//     else if (_tccmp(szAlg, _T("3DES")))
//     { szAlgOID = CALG_3DES; return S_OK; }
// 
//     //asymmetric

    
    //certreq
         if (_tccmp(szAlg, _T("MD5RSASIGN")) || _tccmp(szAlg, _T("md5rsasign")))
    { CA2TCharBuf buf(szOID_OIWSEC_md5RSASign); szAlgOID = buf.ptr; return S_OK; }

    else if (_tccmp(szAlg, _T("SHA1RSASIGN")) || _tccmp(szAlg, _T("sha1rsasign")))
    { CA2TCharBuf buf(szOID_OIWSEC_sha1RSASign); szAlgOID = buf.ptr; return S_OK; }

    else if (_tccmp(szAlg, _T("MD5RSA")) || _tccmp(szAlg, _T("md5rsa")))
    { CA2TCharBuf buf(szOID_OIWSEC_md5RSASign); szAlgOID = buf.ptr; return S_OK; }

    else if (_tccmp(szAlg, _T("SHA1RSA")) || _tccmp(szAlg, _T("sha1rsa")))
    { CA2TCharBuf buf(szOID_OIWSEC_sha1RSASign); szAlgOID = buf.ptr; return S_OK; }
    else
        return S_FALSE;
}

BOOL 
CCertKitCsp::GetDefaultCSP(tstring &CspName, long lPtype)
{
	BOOL bret = FALSE;	
	DWORD lenth;

	bret = CryptGetDefaultProvider(lPtype, NULL, CRYPT_MACHINE_DEFAULT, NULL, &lenth);
	if (!bret)
		return bret;

	CTCharBuf pname(lenth);
	bret = CryptGetDefaultProvider(lPtype, NULL, CRYPT_MACHINE_DEFAULT, pname.ptr, &lenth);

	CspName = pname.ptr;

	return bret;
}



tstring 
CCertKitCsp::GenGUID()
{
	tstring szname;
	UUID uuidName;

#if defined(UNICODE) || defined(_UNICODE)
	unsigned short* UNamePtr = NULL;
#else
	unsigned char* UNamePtr = NULL;	
#endif
    
	UuidCreate(&uuidName);
	UuidToString(&uuidName,&UNamePtr);

	szname = (TCHAR*)UNamePtr;

	RpcStringFree(&UNamePtr);

	return szname;
}

BOOL 
CCertKitCsp::GetCspList(const ulong lType, Tstrlist &vcCspList)
{
    return EnumCSP(lType, vcCspList);
}


BOOL 
CCertKitCsp::EnumCSP(const ulong lType, Tstrlist &vcCspList)
{
	vcCspList.clear();

	DWORD       nszName;
	DWORD       dwType;

	DWORD idx = 0;
	while( CryptEnumProviders( idx, NULL, 0, &dwType, NULL, &nszName ) )
	{
		TCHAR	*pszName = NULL; 
		pszName = (TCHAR *)new TCHAR[nszName];

		CryptEnumProviders(	idx, NULL, 0, &dwType, pszName, &nszName );
		idx++;

		tstring str = pszName;
		if(lType == dwType)
			vcCspList.push_back(str);

		delete []pszName;
	}

	if (vcCspList.size()==0)
		return FALSE;

	return TRUE;
}

LONG
CCertKitCsp::CertInfoFind(uchar *ucmodulus, ulong ulmodlen,
    tstring &szProvName, tstring &szCtnName, DWORD &dKeySpec, DWORD &dProvType)
{
    if(!ucmodulus)
        return ERROR_INVALID_PARAMETER;

    LONG lRet = ERROR_SUCCESS;
    BOOL bFind = FALSE;
    HCRYPTPROV hProv = NULL;
    lRet = EnumCtn();
    if(lRet) 
		return lRet;

    if(m_CtnList.size()==0)
        return ERROR_NO_MORE_ITEMS;

    for (unsigned int i = 0; i<m_CtnList.size();i++)
    {
        lRet = CryptAcquireContext(&hProv, m_CtnList[i].c_str(), m_sCspName.c_str(), m_lProvType, 0);
        if (!lRet)
            continue;

        ulong nPubkeyInfo;
        lRet = CryptExportPublicKeyInfo(hProv, m_lKeySpec, XENCODETYPE, NULL, &nPubkeyInfo );
        if (!lRet)
            continue;

        CByteBuf bufPubkeyInfo(nPubkeyInfo);
        lRet = CryptExportPublicKeyInfo(hProv, m_lKeySpec, XENCODETYPE, 
							(PCERT_PUBLIC_KEY_INFO) bufPubkeyInfo.ptr, &nPubkeyInfo );
        if (!lRet)
            continue;

        uchar ucbuf[4097]={0};
        ulong ulbuflen = 4096;
        lRet = GetPubkeyModulus((PCERT_PUBLIC_KEY_INFO)bufPubkeyInfo.ptr, ucbuf, &ulbuflen);
        if(lRet)
            continue;

        if(0 == memcmp(ucbuf, ucmodulus, ulmodlen))
        {
            bFind = TRUE;
            szProvName = m_sCspName;
            szCtnName = m_CtnList[i].c_str();
            dKeySpec = m_lKeySpec;
            dProvType = m_lProvType;
            break;
        }
    }

    lRet = bFind?ERROR_SUCCESS:GetLastError();
    return lRet;
}

LONG
CCertKitCsp::GetPubkeyModulus(PCERT_PUBLIC_KEY_INFO pPubkeyInfo, uchar *cert_modulus, ulong *modulus_len)
{
	if(!cert_modulus)
		return ERROR_INVALID_PARAMETER;
	
	BYTE	*pData = NULL;
	UINT	len_len = 0;
	short	lenMod = 0;
	long	mode_len = 0;
	
	
	CRYPT_BIT_BLOB &blob = pPubkeyInfo->PublicKey;
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
CCertKitCsp::EnumCtn()
{
    AutoHProv hProv;
	LONG lRet = ERROR_SUCCESS;
	m_CtnList.clear();

    do 
	{
		lRet = CryptAcquireContext(hProv,NULL,m_sCspName.c_str(),PROV_RSA_FULL,CRYPT_VERIFYCONTEXT);
		if(!lRet) 
            break;
		
		uchar ucCtnBuf[100]={0};
		DWORD ulCtnLen;
		lRet = CryptGetProvParam(hProv, PP_ENUMCONTAINERS, NULL, &ulCtnLen,CRYPT_FIRST);
		if(!lRet) 
            break;

		lRet = CryptGetProvParam(hProv, PP_ENUMCONTAINERS, ucCtnBuf, &ulCtnLen,CRYPT_FIRST);
		if(!lRet) 
            break;
		
		CA2TCharBuf tcbufa((char*)ucCtnBuf);
		m_CtnList.push_back(tcbufa.ptr);

		while(lRet)
		{
			lRet = CryptGetProvParam(hProv, PP_ENUMCONTAINERS, ucCtnBuf, &ulCtnLen, CRYPT_NEXT);
			if( !lRet && (ERROR_NO_MORE_ITEMS == GetLastError()) ){
				lRet = TRUE;
				break;
			}

			ucCtnBuf[ulCtnLen] = 0;

			CA2TCharBuf tcbuf((char*)ucCtnBuf);
			m_CtnList.push_back(tcbuf.ptr);
		}
	} while (0);

	lRet = lRet?ERROR_SUCCESS:GetLastError();
	return lRet;
}


void 
CCertKitCsp::FlipBuffer(uchar *pBuf, ulong ulLen)
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
//////////////////////////////////////////////////////////////////////////

LONG
CCertKitCsp::CreateP10(tstring& rszP10)
{
	HRESULT hRet = ERROR_SUCCESS;
	BOOL bRet;
    rszP10 = _T("");
	tstring szErrInfo;
	tstring strtmp;

	do{
		bRet = CryptAcquireContext(&m_hProv, m_sReqCtnName.c_str(), 
								m_sCspName.c_str(), m_lProvType, CRYPT_NEWKEYSET );
		if( !bRet ) { 
            hRet = GetLastError(); 
            break; 
        }

		bRet = CryptGenKey(m_hProv, m_lKeySpec, m_lGenKeyFlag, &m_hPrivKey);
		if( !bRet ) { 
            hRet = GetLastError(); 
            break; 
        }
		

		DWORD nPubkeyInfo;
		bRet = CryptExportPublicKeyInfo(m_hProv, m_lKeySpec, XENCODETYPE, NULL, &nPubkeyInfo );
		if( !bRet ) { 
            hRet = GetLastError(); 
            break; 
        }
		
		CByteBuf bufPubkeyInfo(nPubkeyInfo);
		bRet = CryptExportPublicKeyInfo(m_hProv, m_lKeySpec, XENCODETYPE, 
							(PCERT_PUBLIC_KEY_INFO) bufPubkeyInfo.ptr, &nPubkeyInfo );
		if( !bRet ) { 
            hRet = GetLastError(); 
            break; 
        }
		
		
		CERT_NAME_BLOB certNameBlob;
		CByteBuf bufCertNameBb(4096);
		certNameBlob.pbData = bufCertNameBb.ptr;

        if (m_RdnMap.size() == 0)
		    X509CNStuff(certNameBlob);
        else
            X509DNStuff(certNameBlob);
		
		CERT_REQUEST_INFO pCertReqInfo;
		pCertReqInfo.dwVersion = CERT_REQUEST_V1;
		pCertReqInfo.Subject = certNameBlob;
		pCertReqInfo.SubjectPublicKeyInfo = *(CERT_PUBLIC_KEY_INFO*)(bufPubkeyInfo.ptr);
		pCertReqInfo.cAttribute = 0;
		pCertReqInfo.rgAttribute = NULL;

		CRYPT_ALGORITHM_IDENTIFIER  SignatureAlgorithm;
		CRYPT_OBJID_BLOB  Parameters;
		memset(&Parameters, 0, sizeof(Parameters));

        //DBGLOG(("m_sReqAlg = %s, len=%d", m_sReqSignAlgOid.c_str(), m_sReqSignAlgOid.length()));
        //support below 3 method:
        //#define szOID_RSA_MD5RSA        "1.2.840.113549.1.1.4"
        //#define szOID_RSA_SHA1RSA       "1.2.840.113549.1.1.5"
        //#define szOID_X957_SHA1DSA      "1.2.840.10040.4.3"
		SignatureAlgorithm.pszObjId = (LPSTR)m_sReqSignAlgOid.c_str(); 
		SignatureAlgorithm.Parameters = Parameters;
		DWORD cbEncodeCertReqInfo;
		bRet = CryptSignAndEncodeCertificate( m_hProv, m_lKeySpec, X509_ASN_ENCODING, 
										X509_CERT_REQUEST_TO_BE_SIGNED, &pCertReqInfo,
										&SignatureAlgorithm, NULL, NULL, &cbEncodeCertReqInfo );
		if( !bRet ) { 
            hRet = GetLastError(); 
            break; 
        }


		CByteBuf pbEncodeCertReqInfo(cbEncodeCertReqInfo);
		bRet = CryptSignAndEncodeCertificate( m_hProv, m_lKeySpec, X509_ASN_ENCODING, 
							X509_CERT_REQUEST_TO_BE_SIGNED, &pCertReqInfo, 
							&SignatureAlgorithm, NULL, pbEncodeCertReqInfo.ptr, &cbEncodeCertReqInfo );
		if( !bRet ) { 
            hRet = GetLastError(); 
            break; 
        }
		
		tstring szhashPubkey = HashPubKeyInfo(m_hProv, &pCertReqInfo.SubjectPublicKeyInfo);
		
		if(m_bUseRegEnv)
			CertInfoBackup(m_sCspName, m_sReqCtnName, szhashPubkey, m_lKeySpec, m_lProvType);

		DWORD cbBase64EncodeCertReqInfo = cbEncodeCertReqInfo*2+128;
		CTCharBuf pbBase64EncodeCertReqInfo(cbBase64EncodeCertReqInfo);
		CBase64::Encode(pbEncodeCertReqInfo.ptr, cbEncodeCertReqInfo,
					pbBase64EncodeCertReqInfo.ptr, &cbBase64EncodeCertReqInfo);

		rszP10 = pbBase64EncodeCertReqInfo.ptr;

#if defined(_DEBUG) || defined(DEBUG)
		MessageBox(NULL, rszP10.c_str(), szhashPubkey.c_str(), MB_OK);
#endif

	}while(0);


	if( m_hPrivKey )	
        CryptDestroyKey(m_hPrivKey);
	if( m_hProv )		
        CryptReleaseContext(m_hProv,0);

	return hRet;
}

LONG 
CCertKitCsp::AcceptP7(const TCHAR* szP7, int iType)
{
	LONG lRet = 0;
	HRESULT hRet = S_OK;
	HCRYPTMSG hP7 = NULL;


	do{
		BOOL bRet;
		hP7 = CryptMsgOpenToDecode(XENCODETYPE, 0, 0, 0, NULL, NULL );

		tstring szP7b64;
		PemHandle(szP7, szP7b64);


		unsigned long binp7len = szP7b64.length(); 
		CByteBuf binp7(binp7len*2);
		CBase64::Decode(szP7b64.c_str(), binp7.ptr, &binp7len);
		
		bRet = CryptMsgUpdate(hP7, binp7.ptr, binp7len, TRUE);
        if( !bRet ) {	
            hRet = GetLastError();	
            break;	
        }
		
		DWORD certCount = 0;
		DWORD cbLen = sizeof(certCount);
				
		CryptMsgGetParam(hP7,CMSG_CERT_COUNT_PARAM,0,&certCount, &cbLen);
		if( certCount == 0 ) {	
            hRet = GetLastError();	
            break;	
        }
		

		for( ulong i = 0 ; i < (certCount > 10 ? 10:  certCount) ; i++ )
		{	
			DWORD cbCertLen = 0;
			
			CryptMsgGetParam(hP7,CMSG_CERT_PARAM,i,NULL, &cbCertLen);
			CByteBuf certBuf(cbCertLen);
			CryptMsgGetParam(hP7,CMSG_CERT_PARAM,i,certBuf.ptr, &cbCertLen);
			
			PCCERT_CONTEXT pCert=CertCreateCertificateContext(XENCODETYPE, certBuf.ptr,cbCertLen);
			
			tstring szProvName, szCtnName;
			DWORD dwKeySpec, dwProvType;
			uchar ucmodulus[4096] = {0};
			ulong ulmodlen;

			tstring szHashPubkey = HashPubKeyInfo(NULL, &pCert->pCertInfo->SubjectPublicKeyInfo);
			tstring szRegKey = m_sReqRegKey;
			szRegKey += _T("\\");
			szRegKey += szHashPubkey;

			HKEY hKeydir = NULL;
			lRet = RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey.c_str(), 0, KEY_ALL_ACCESS, &hKeydir );
			if( lRet == ERROR_SUCCESS ) {	
				RegCloseKey(hKeydir);
				lRet = CertInfoRecovery(szHashPubkey, szProvName, szCtnName, dwKeySpec, dwProvType);
			} else {
				CCertificate curcert;
				curcert.Attach(pCert);
				curcert.GetModulus(ucmodulus, &ulmodlen);
				lRet = CertInfoFind(ucmodulus, ulmodlen, szProvName, szCtnName, dwKeySpec, dwProvType);
			}
				
			if( lRet == ERROR_SUCCESS )
			{	

				HCERTSTORE hMyStore = NULL;
				hMyStore = CertOpenStore( CERT_STORE_PROV_SYSTEM, 0, NULL, 
											CERT_SYSTEM_STORE_CURRENT_USER, L"My" );
				if( hMyStore == NULL ){	hRet = GetLastError();	break;	}
				
				CRYPT_KEY_PROV_INFO keyProvInfo;
				CT2WCharBuf szwProvName(szProvName.c_str());
				CT2WCharBuf szwCtnName(szCtnName.c_str());
				keyProvInfo.pwszProvName = szwProvName.ptr;
				keyProvInfo.pwszContainerName = szwCtnName.ptr;
				keyProvInfo.dwProvType = dwProvType;
				keyProvInfo.dwFlags = 0;
				keyProvInfo.cProvParam = 0;
				keyProvInfo.rgProvParam  = NULL;
				keyProvInfo.dwKeySpec  = dwKeySpec;
				
				bRet = CertSetCertificateContextProperty( pCert, CERT_KEY_PROV_INFO_PROP_ID, 
									CERT_STORE_NO_CRYPT_RELEASE_FLAG, &keyProvInfo );
				if( !bRet ) { 
                    hRet = GetLastError(); 
                    break;	
                }
				
				bRet = CertAddCertificateContextToStore( hMyStore, pCert, 
										CERT_STORE_ADD_REPLACE_EXISTING, NULL);
				CertCloseStore(hMyStore,0);
				if( !bRet ) { 
                    hRet = GetLastError(); 
                    break;	
                }
				
				// 将证书写入CSP
				if( m_bReqWriteCert2Csp )
				{
					bRet = CryptAcquireContext(&m_hProv, szCtnName.c_str(),
												szProvName.c_str(), dwProvType,0);
					if( !bRet || m_hProv == NULL ){
						hRet = GetLastError();
						break; 
                    }
					
					bRet = CryptGetUserKey(m_hProv, dwKeySpec, &m_hPrivKey);
					if( !bRet || m_hPrivKey == NULL ){
						hRet = GetLastError();
						break; 
                    }
					
					bRet = CryptSetKeyParam(m_hPrivKey, KP_CERTIFICATE,certBuf.ptr,0);
				}

				m_sReqCtnName = szCtnName;
				m_sCspName = szProvName;

				if (m_bUseRegEnv)
					RegDeleteKey(HKEY_CURRENT_USER, szRegKey.c_str());
			}
			else
			{
				// 安装ROOT证书
				BOOL bRoot = FALSE;
				
				if(
					  (pCert->pCertInfo->Issuer.cbData == pCert->pCertInfo->Subject.cbData) 
					&&(memcmp(pCert->pCertInfo->Issuer.pbData,
							pCert->pCertInfo->Subject.pbData,
							pCert->pCertInfo->Subject.cbData) == 0)
					)
				{
					bRoot = TRUE;
				}
				
				void * storeName = L"Ca";
				if(bRoot) storeName = L"Root";
				
				HCERTSTORE hSysStore = NULL;
				hSysStore = CertOpenStore( CERT_STORE_PROV_SYSTEM, 0, 
								NULL, CERT_SYSTEM_STORE_CURRENT_USER, storeName);
				
				CertAddCertificateContextToStore(hSysStore,pCert,CERT_STORE_ADD_USE_EXISTING,NULL);
				CertCloseStore(hSysStore,0);
			}
			
			CertFreeCertificateContext(pCert);
		}
		
	}while(0);

	if( hP7 )			
        CryptMsgClose(hP7);
	if( m_hPrivKey )	
        CryptDestroyKey(m_hPrivKey);
	if( m_hProv )		
        CryptReleaseContext(m_hProv,0);

	return hRet;	
}




LONG 
CCertKitCsp::AcceptCert(const TCHAR* szCert, int iType)
{
    if(!szCert)
        return ERROR_INVALID_PARAMETER;

    LONG lRet = 0;
    HRESULT hRet = ERROR_SUCCESS;
    BOOL bRet;

	tstring szCertb64;
	PemHandle(szCert, szCertb64);
	
    unsigned long binCertLen = szCertb64.length(); 
    CByteBuf binCert(binCertLen);
    CBase64::Decode(szCertb64.c_str(), binCert.ptr, &binCertLen);

    do{
        PCCERT_CONTEXT pCert=CertCreateCertificateContext(XENCODETYPE, binCert.ptr, binCertLen);
        if(!pCert){ 
            hRet = GetLastError();
            break;
        }

        tstring szProvName, szCtnName;
        DWORD dwKeySpec, dwProvType;
        uchar ucmodulus[4096] = {0};
        ulong ulmodlen;

        tstring szHashPubkey = HashPubKeyInfo(NULL, &pCert->pCertInfo->SubjectPublicKeyInfo);
        tstring szRegKey = m_sReqRegKey;
        szRegKey += _T("\\");
        szRegKey += szHashPubkey;

        HKEY hKeydir = NULL;
        lRet = RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey.c_str(), 0, KEY_ALL_ACCESS, &hKeydir );
        if( lRet == ERROR_SUCCESS ) {	
            RegCloseKey(hKeydir);
            lRet = CertInfoRecovery(szHashPubkey, szProvName, szCtnName, dwKeySpec, dwProvType);
        } else {
            CCertificate curcert;
            curcert.Attach(pCert);
            curcert.GetModulus(ucmodulus, &ulmodlen);
            lRet = CertInfoFind(ucmodulus, ulmodlen, szProvName, szCtnName, dwKeySpec, dwProvType);
            if(lRet){
                hRet = lRet;
                break;
            }
        }

        if( lRet == ERROR_SUCCESS )
        {	

            HCERTSTORE hMyStore = NULL;
            hMyStore = CertOpenStore( CERT_STORE_PROV_SYSTEM, 0, NULL, 
                CERT_SYSTEM_STORE_CURRENT_USER, L"My" );
            if( hMyStore == NULL ){	hRet = GetLastError();	break;	}

            CRYPT_KEY_PROV_INFO keyProvInfo;
            CT2WCharBuf szwProvName(szProvName.c_str());
            CT2WCharBuf szwCtnName(szCtnName.c_str());
            keyProvInfo.pwszProvName = szwProvName.ptr;
            keyProvInfo.pwszContainerName = szwCtnName.ptr;
            keyProvInfo.dwProvType = dwProvType;
            keyProvInfo.dwFlags = 0;
            keyProvInfo.cProvParam = 0;
            keyProvInfo.rgProvParam  = NULL;
            keyProvInfo.dwKeySpec  = dwKeySpec;

            bRet = CertSetCertificateContextProperty( pCert, CERT_KEY_PROV_INFO_PROP_ID, 
                                    CERT_STORE_NO_CRYPT_RELEASE_FLAG, &keyProvInfo );
            if( !bRet ) { 
                hRet = GetLastError(); 
                break;	
            }

            bRet = CertAddCertificateContextToStore( hMyStore, pCert, 
                CERT_STORE_ADD_REPLACE_EXISTING, NULL);
            CertCloseStore(hMyStore,0);
            if( !bRet ) { 
                hRet = GetLastError(); 
                break;	
            }

            // 将证书写入CSP
            if( m_bReqWriteCert2Csp )
            {
                bRet = CryptAcquireContext(&m_hProv, szCtnName.c_str(),
                    szProvName.c_str(), dwProvType,0);
                if( !bRet || m_hProv == NULL ){
                    hRet = GetLastError();
                    break; 
                }

                bRet = CryptGetUserKey(m_hProv, dwKeySpec, &m_hPrivKey);
                if( !bRet || m_hPrivKey == NULL ){
                    hRet = GetLastError();
                    break; 
                }

                bRet = CryptSetKeyParam(m_hPrivKey, KP_CERTIFICATE,binCert.ptr,0);
            }

            m_sReqCtnName = szCtnName;
            m_sCspName = szProvName;

            if (m_bUseRegEnv)
                RegDeleteKey(HKEY_CURRENT_USER, szRegKey.c_str());

        }

    }while(0);

    if( m_hPrivKey )	
        CryptDestroyKey(m_hPrivKey);
    if( m_hProv )		
        CryptReleaseContext(m_hProv,0);

    return hRet;	
}


LONG CCertKitCsp::AcceptP12(const TCHAR* szP12, const TCHAR* szPsw, int iOpt)
{


    return S_OK;
}

// 
// int 
// CCertKitCsp::FormPrivateKeyBlob(PBYTE pKey, DWORD dwKeyLen, DWORD keyalg,
// 										PBYTE& pKeyBlob, DWORD &dwBlobLen)
// {
// 	ULONG	ulRet = 0x7300;
// 	BLOBHEADER	header;
// 	RSAPUBKEY	pubkey;
// 	
// 	header.bType	= PRIVATEKEYBLOB;
// 	header.bVersion = CUR_BLOB_VERSION;
// 	header.aiKeyAlg = keyalg;  
// 	header.reserved = 0;
// 	
// 	pubkey.bitlen	= 1024;
// 	pubkey.magic	= 0x32415352;
// 	pubkey.pubexp   = 65537;  
// 	
// 	PBYTE pPos		= pKey + 2;
// 	PBYTE pLen		= pKey + 1;
// 	WORD dwLen		=0;
// 	DWORD dwN,dwD,dwE,dwP,dwQ,dwDmp1,dwDmq1,dwIqmp;
// 	
// 	if(0x82 == *pLen)
// 	{
// 		dwLen	=  *pPos;
// 		dwLen	<<= 8;
// 		dwLen	|= *(pPos + 1);		
// 	}
// 	else if(0x81 == *pLen)
// 		dwLen = *(BYTE*)pPos;
// 	
// 	DWORD dwTotalLen = 0;
// 	
// 	//	if(dwKeyLen - 4 != dwLen) return ulRet;
// 	dwKeyLen = dwLen +4;	
// 	//n
// 	pPos += 3 + 2 + 2;	
// 	if(0x0 == *(pPos + 1)){
// 		dwN = *pPos - 1;	pPos += 2;
// 	}
// 	else{
// 		dwN = *pPos;		pPos ++;
// 	}
// 	dwTotalLen += dwN;
// 	PBYTE n = (PBYTE) malloc(dwN);
// 	memcpy(n,pPos,dwN);
// 	pPos += dwN + 1;
// 	
// 	//e
// 	dwE = *pPos;
// 	//	dwTotalLen += dwE;
// 	PBYTE e = (PBYTE)malloc(dwE);
// 	memcpy(e,pPos+1,dwE);
// 	pPos += dwE + 1 + 2;
// 	
// 	//skip d
// 	if(0x0 == *(pPos + 1)){
// 		dwD = *pPos - 1;	pPos += 2;
// 	}
// 	else{
// 		dwD = *pPos;		pPos ++;
// 	}
// 	dwTotalLen += dwD;
// 	PBYTE d = (PBYTE)malloc(dwD);
// 	memcpy(d,pPos,dwD);
// 	pPos += dwD + 1;
// 	
// 	//p
// 	if(0x0 == *(pPos + 1)){
// 		dwP = *pPos - 1;	pPos += 2;
// 	}
// 	else{
// 		dwP = *pPos;		pPos ++;
// 	}
// 	dwTotalLen += dwP;
// 	PBYTE p = (PBYTE)malloc(dwP);
// 	memcpy(p,pPos,dwP);
// 	pPos += dwP + 1;
// 	
// 	//q
// 	if(0x0 == *(pPos + 1)){
// 		dwQ = *pPos - 1;	pPos += 2;
// 	}
// 	else{
// 		dwQ = *pPos;		pPos ++;
// 	}
// 	dwTotalLen += dwQ;
// 	PBYTE q = (PBYTE)malloc(dwQ);
// 	memcpy(q,pPos,dwQ);
// 	pPos += dwQ + 1;
// 	
// 	//dmp1
// 	if(0x0 == *(pPos + 1)){
// 		dwDmp1 = *pPos - 1;	pPos += 2;
// 	}
// 	else{
// 		dwDmp1 = *pPos;		pPos ++;
// 	}
// 	dwTotalLen += dwDmp1;
// 	PBYTE dmp1 = (PBYTE)malloc(dwDmp1);
// 	memcpy(dmp1,pPos,dwDmp1);
// 	pPos += dwDmp1  + 1;
// 	
// 	//dmp1
// 	if(0x0 == *(pPos + 1)){
// 		dwDmq1 = *pPos - 1;	pPos += 2;
// 	}
// 	else{
// 		dwDmq1 = *pPos;		pPos ++;
// 	}
// 	dwTotalLen += dwDmq1;
// 	PBYTE dmq1 = (PBYTE)malloc(dwDmq1);
// 	memcpy(dmq1,pPos,dwDmq1);
// 	pPos += dwDmq1 + 1;
// 	
// 	//iqmp
// 	if(0x0 == *(pPos + 1)){
// 		dwIqmp = *pPos - 1;	pPos += 2;
// 	}
// 	else{
// 		dwIqmp = *pPos;		pPos ++;
// 	}
// 	dwTotalLen += dwIqmp;
// 	PBYTE iqmp = (PBYTE)malloc(dwIqmp);
// 	memcpy(iqmp,pPos,dwIqmp);
// 	
// 	if(dwTotalLen != pubkey.bitlen*9/16) return ulRet;
// 	
// 	dwBlobLen = sizeof(BLOBHEADER) + sizeof(RSAPUBKEY) + dwTotalLen;
// 	PBYTE pTemp = (PBYTE) malloc(dwBlobLen);
// 	
// 	pKeyBlob = pTemp;
// 	
// 	memcpy(pTemp,&header,sizeof(BLOBHEADER));
// 	pTemp += sizeof(BLOBHEADER);
// 	memcpy(pTemp,&pubkey,sizeof(RSAPUBKEY));
// 	pTemp += sizeof(RSAPUBKEY);
// 	
// 	//Reverse the byte sequence for private blob
// 	MemoryReverse(n,dwN);
// 	MemoryReverse(p,dwP);
// 	MemoryReverse(q,dwQ);
// 	MemoryReverse(dmp1,dwDmp1);
// 	MemoryReverse(dmq1,dwDmq1);
// 	MemoryReverse(iqmp,dwIqmp);
// 	MemoryReverse(d,dwD);
// 	/**/
// 	memcpy(pTemp,n,dwN);
// 	pTemp += dwN;
// 	memcpy(pTemp,p,dwP);
// 	pTemp += dwP;
// 	memcpy(pTemp,q,dwQ);
// 	pTemp += dwQ;
// 	memcpy(pTemp,dmp1,dwDmp1);
// 	pTemp += dwDmp1;
// 	memcpy(pTemp,dmq1,dwDmq1);
// 	pTemp += dwDmq1;
// 	memcpy(pTemp,iqmp,dwIqmp);
// 	pTemp += dwIqmp;
// 	memcpy(pTemp,d,dwD);
// 	pTemp += dwD;
// 	
// 	if(n) free(n);
// 	if(e) free(e);
// 	if(d) free(d);
// 	if(p) free(p);
// 	if(q) free(q);
// 	if(dmp1) free(dmp1);
// 	if(dmq1) free(dmq1);
// 	if(iqmp) free(iqmp);
// 	
// 	return 0;
// }

void 
CCertKitCsp::MemoryReverse(PBYTE &pData,DWORD dwLen)
{
	BYTE	b;
	PBYTE	pBegin,pEnd;
	DWORD	dwMid = dwLen /2;
	
	pBegin  = pData;
	pEnd	= pData + dwLen -1;
	
	while(pBegin < pEnd) {
		b = *pBegin;
		*pBegin	= *pEnd;
		*pEnd = b;
		pBegin++;
		pEnd--;
	}
}

LONG 
CCertKitCsp::CertInfoBackup(tstring szProvName, tstring szCtnName, 
						tstring szHashPubkey, DWORD dKeySpec, DWORD dProvType)
{
	HKEY hdir = NULL;
	LONG lRet = S_OK;
	tstring sztmp;

	tstring szRegKey = m_sReqRegKey;
	szRegKey += _T("\\");
	szRegKey += szHashPubkey;

	lRet = RegCreateKeyEx( HKEY_CURRENT_USER, szRegKey.c_str(), 0, NULL, 0, KEY_ALL_ACCESS,NULL, &hdir, NULL );
	if( lRet != 0 )
		return S_FALSE;
	
	// ProviderName
	RegSetValueEx(hdir, SZ_CERT_PRVNAME, 0, REG_SZ,(BYTE*)szProvName.c_str(), szProvName.length()*sizeof(TCHAR));
	
	// ContainerName
	RegSetValueEx(hdir, SZ_CERT_CTNNAME, 0, REG_SZ, (BYTE*)szCtnName.c_str(), szCtnName.length()*sizeof(TCHAR));
	
	// KeySpec
	RegSetValueEx(hdir, SZ_CERT_KEYSPEC, 0, REG_DWORD, (BYTE*)&dKeySpec, sizeof(DWORD));

	// ProviderType
	RegSetValueEx(hdir, SZ_CERT_PRVTYPE, 0, REG_DWORD, (BYTE*)&dProvType, sizeof(DWORD));

    // PublicKeyHash
	RegSetValueEx(hdir, SZ_CERT_PUBHASH, 0, REG_SZ, (BYTE *)szHashPubkey.c_str(), szHashPubkey.length()*sizeof(TCHAR));
	
	// GenTime
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	TCHAR timebuf[128];
	wsprintf(timebuf,_T("%04d-%02d-%02d %02d:%02d:%02d")
			, systime.wYear,systime.wMonth,systime.wDay
			, systime.wHour,systime.wMinute,systime.wSecond);

	sztmp = timebuf;
	RegSetValueEx(hdir, SZ_CERT_GENTIME, 0, REG_SZ, (BYTE*)sztmp.c_str(), sztmp.length()*sizeof(TCHAR));
	
	RegCloseKey(hdir);


    //TODO: error handle
	return TRUE;
}

LONG 
CCertKitCsp::CertInfoRecovery(tstring &szPubkeyHash, tstring &szProvName, 
								 tstring &szCtnName, DWORD &dwKeySpec, DWORD &dwProvType)
{
	HKEY hKeydir = NULL;
	LONG lRet = S_OK;
	tstring sztmp;

	tstring szRegKey = m_sReqRegKey;
	szRegKey += _T("\\");
	szRegKey += szPubkeyHash;

	do 
	{
		lRet = RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey.c_str(), 0, KEY_ALL_ACCESS, &hKeydir );

		if( lRet != ERROR_SUCCESS )
			break;


		DWORD vType = 0;
		DWORD vLen = 0;

		// ProviderName
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_PRVNAME, NULL, &vType, NULL, &vLen );
		if(lRet != ERROR_SUCCESS || vType != REG_SZ )
			break;

		CTCharBuf providerName(vLen+1);
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_PRVNAME, NULL, &vType, (BYTE*)providerName.ptr, &vLen);
		if(lRet != ERROR_SUCCESS || vType != REG_SZ )
			break;
		szProvName = providerName.ptr;

		// ContainerName
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_CTNNAME, NULL, &vType, NULL, &vLen );
		if(lRet != ERROR_SUCCESS || vType != REG_SZ )
			break;

		CTCharBuf containerName(vLen+1);
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_CTNNAME, NULL, &vType, (BYTE*)containerName.ptr, &vLen );
		if(lRet != ERROR_SUCCESS || vType != REG_SZ )
			break;
		szCtnName = containerName.ptr;

		// ProviderType
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_PRVTYPE, NULL, &vType, NULL, &vLen );
		if(lRet != ERROR_SUCCESS || vType != REG_DWORD )
			break;

		DWORD providerType = 0;
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_PRVTYPE, NULL, &vType, (BYTE *)&providerType, &vLen );
		if(lRet != ERROR_SUCCESS || vType != REG_DWORD )
			break;
		dwKeySpec = providerType;

		// KeySpec
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_KEYSPEC, NULL, &vType, NULL, &vLen);
		if(lRet != ERROR_SUCCESS || vType != REG_DWORD )
			break;

		DWORD keySpec = 0;
		lRet = RegQueryValueEx(hKeydir, SZ_CERT_KEYSPEC, NULL, &vType, (BYTE *)&keySpec, &vLen );
		if(lRet != ERROR_SUCCESS || vType != REG_DWORD )
			break;
		dwProvType = keySpec;

		RegCloseKey(hKeydir);
		hKeydir = NULL;

	} while (0);
	
	if(lRet != ERROR_SUCCESS) 
		lRet = GetLastError();
	return lRet;
}



tstring 
CCertKitCsp::HashPubKeyInfo(HCRYPTPROV hProv, CERT_PUBLIC_KEY_INFO *keyinfo)
{
	TCHAR hashed[256] = {0};
	BYTE hash[128];
	DWORD hashlen = sizeof(hash);
	tstring szHashed;

	CryptHashPublicKeyInfo(NULL, CALG_SHA1, 0, XENCODETYPE, keyinfo, hash, &hashlen);

	for( ulong i = 0 ; i < hashlen ; i++ )
		_stprintf(hashed + i * 2, _T("%02x"), hash[i]);
	szHashed = hashed;
		
	return szHashed;
}

LONG
CCertKitCsp::InstallRootCert(PCERT_CONTEXT pCert)
{
	HCERTSTORE hSysStore = NULL;
	TCHAR szStore[20] = {0};
	LONG lRet = ERROR_SUCCESS;

	
	return lRet;
}


LONG 
CCertKitCsp::InstallRootCert(TCHAR* szB64Cert)
{
	if (!szB64Cert)
		return ERROR_BAD_ARGUMENTS;
	LONG lRet = ERROR_SUCCESS;

	CCertificate Cert(szB64Cert);
	if (!Cert.IsAttached())
		return ERROR_INVALID_HANDLE;

	
	HCERTSTORE hSysStore = NULL;
	WCHAR szStore[20] = {0};

	do 
	{
		CByteBuf pbCertBuf(_tcslen(szB64Cert)*2);
		DWORD certlen = _tcslen(szB64Cert)*2;
		CBase64::Decode(szB64Cert, pbCertBuf.ptr, &certlen);
		
		if(CryptVerifyCertificateSignature(NULL, XENCODETYPE,
			pbCertBuf.ptr, certlen, &Cert.GetContext()->pCertInfo->SubjectPublicKeyInfo))
		{
			wcscpy(szStore, L"ROOT");
			break;
		}

		BOOL bIsCACert = FALSE, bIsEntity = FALSE, bHasCAUsage = FALSE;
		
		PCERT_EXTENSION pExt3;
		pExt3 = CertFindExtension(szOID_BASIC_CONSTRAINTS2, 
			Cert.GetContext()->pCertInfo->cExtension, Cert.GetContext()->pCertInfo->rgExtension);
		PCERT_BASIC_CONSTRAINTS2_INFO pInfo;
		DWORD cbInfo;
		
		if(NULL != pExt3)
		{
			if(TRUE == CryptDecodeObject(X509_ASN_ENCODING, X509_BASIC_CONSTRAINTS2,
				pExt3->Value.pbData, pExt3->Value.cbData, NULL, NULL, &cbInfo))
			{
				pInfo =(PCERT_BASIC_CONSTRAINTS2_INFO)LocalAlloc(LPTR, cbInfo);
				if(NULL != pInfo)
				{
					if(TRUE == CryptDecodeObject(X509_ASN_ENCODING, X509_BASIC_CONSTRAINTS2,
						pExt3->Value.pbData, pExt3->Value.cbData, NULL, pInfo, &cbInfo))
					{
						bIsCACert = pInfo->fCA;
						bIsEntity = !pInfo->fCA;
					}
				}
				
			}
		}

		BYTE  KeyUsageBits;    // Intended key usage bits copied to here.
		DWORD cbKeyUsageByteCount = 1; // 1 byte will be copied to *pbKeyUsage
		BOOL Return = CertGetIntendedKeyUsage(X509_ASN_ENCODING,
							Cert.GetContext()->pCertInfo,&KeyUsageBits,cbKeyUsageByteCount);
		if(Return)
		{
			if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) ||(KeyUsageBits & CERT_CRL_SIGN_KEY_USAGE))
			{
				bHasCAUsage = TRUE;
			}
		}
		
		if(bIsEntity)
		{
			wcscpy(szStore, L"ADDRESSBOOK");
			break;
		}
		
		if(bIsCACert || bHasCAUsage)
		{
			wcscpy(szStore, L"CA");	
			break;
		}

		wcscpy(szStore, L"CA");
	}while (0);


	do 
	{
		hSysStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,
		0, 0, CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_NO_CRYPT_RELEASE_FLAG, szStore);

		if (!hSysStore) { 
            lRet = GetLastError(); 
            break;	
        }

		if(!CertAddCertificateContextToStore(hSysStore,	Cert.GetContext(),	CERT_STORE_ADD_USE_EXISTING,NULL)){
		    lRet = GetLastError();
            break;
        }

	} while (0);

	if (hSysStore) 
        CertCloseStore(hSysStore, 0);

	return lRet;	
}


LONG 
CCertKitCsp::X509DNStuff(CERT_NAME_BLOB &certNameBlob)
{	
	LONG lret = ERROR_SUCCESS;
	BOOL bRet = FALSE;	

    //char* szRdn = (char*)malloc(m_sCertRdn.length()*2);
    //memset(szRdn, 0, m_sCertRdn.length()*2);
    CT2UTF8CharBuf t2aBuf(m_sCertReqCN.c_str());
    //strcpy(szRdn, t2aBuf.ptr);
    //m_Ptrs.add((byte*)szRdn);

	// RDN VALUE 
	CERT_RDN_VALUE_BLOB blobCn;
	blobCn.pbData = (BYTE *) t2aBuf.ptr;
	blobCn.cbData = strlen(t2aBuf.ptr);
	
	// RDN ATTR : CN = VALUE
	CERT_RDN_ATTR certRdnAttr[1];
	certRdnAttr[0].pszObjId = szOID_COMMON_NAME;
	certRdnAttr[0].dwValueType = CERT_RDN_PRINTABLE_STRING;//CERT_RDN_UTF8_STRING;//  | CERT_RDN_ENABLE_UTF8_UNICODE_FLAG;
	certRdnAttr[0].Value = blobCn;
	
	// DN : ...
	CERT_RDN certRdn[1];
	certRdn[0].cRDNAttr = 1;
	certRdn[0].rgRDNAttr = certRdnAttr;
	
	// NAME_INFO
	CERT_NAME_INFO certNameInfo;
	certNameInfo.cRDN = 1;
	certNameInfo.rgRDN = certRdn;
	
	DWORD cbEncoded;
	CryptEncodeObject(XENCODETYPE, X509_NAME, &certNameInfo, NULL, &cbEncoded );

	CByteBuf bufCertNameInfo(cbEncoded);
	CryptEncodeObject(XENCODETYPE, X509_NAME, &certNameInfo, bufCertNameInfo.ptr, &cbEncoded);
	
	memcpy(certNameBlob.pbData, bufCertNameInfo.ptr, cbEncoded);
	
	certNameBlob.cbData = cbEncoded;
//	certNameBlob.pbData = bufCertNameInfo.ptr;


	return lret;
}


LONG 
CCertKitCsp::X509CNStuff(CERT_NAME_BLOB &certNameBlob)
{	
    LONG lret = ERROR_SUCCESS;
    BOOL bRet = FALSE;	

    //char* szRdn = (char*)malloc(m_sCertRdn.length()*2);
    //memset(szRdn, 0, m_sCertRdn.length()*2);
    CT2UTF8CharBuf t2aBuf(m_sCertReqCN.c_str());
    //strcpy(szRdn, t2aBuf.ptr);
    //m_Ptrs.add((byte*)szRdn);

    // RDN VALUE 
    CERT_RDN_VALUE_BLOB blobCn;
    blobCn.pbData = (BYTE *) t2aBuf.ptr;
    blobCn.cbData = strlen(t2aBuf.ptr);

    // RDN ATTR : CN = VALUE
    CERT_RDN_ATTR certRdnAttr[1];
    certRdnAttr[0].pszObjId = szOID_COMMON_NAME;
    certRdnAttr[0].dwValueType = CERT_RDN_PRINTABLE_STRING;//CERT_RDN_UTF8_STRING;//  | CERT_RDN_ENABLE_UTF8_UNICODE_FLAG;
    certRdnAttr[0].Value = blobCn;

    // DN : ...
    CERT_RDN certRdn[1];
    certRdn[0].cRDNAttr = 1;
    certRdn[0].rgRDNAttr = certRdnAttr;

    // NAME_INFO
    CERT_NAME_INFO certNameInfo;
    certNameInfo.cRDN = 1;
    certNameInfo.rgRDN = certRdn;

    DWORD cbEncoded;
    CryptEncodeObject(XENCODETYPE, X509_NAME, &certNameInfo, NULL, &cbEncoded );

    CByteBuf bufCertNameInfo(cbEncoded);
    CryptEncodeObject(XENCODETYPE, X509_NAME, &certNameInfo, bufCertNameInfo.ptr, &cbEncoded);

    memcpy(certNameBlob.pbData, bufCertNameInfo.ptr, cbEncoded);

    certNameBlob.cbData = cbEncoded;
    //	certNameBlob.pbData = bufCertNameInfo.ptr;


    return lret;
}

LONG 
CCertKitCsp::HashData(const BYTE* pdata, long datalen,tstring& szHash)
{
	HCRYPTPROV	hCryptProv			= NULL; 
	HCRYPTHASH	hHash				= NULL;
	long		lRet				= S_FALSE;

	DWORD		dwHashLen			= 0;
	BYTE		pHashValue[512]		= {0};     //签名值
	int			i					= 0;
	TCHAR       szHashBuf[100]      = {0};

	do 
	{
		if(!CryptAcquireContext(&hCryptProv,NULL, NULL,PROV_RSA_FULL, 0))
			if(!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				break;

		if(!CryptCreateHash(hCryptProv, m_iHashAlg, 0, 0, &hHash)) 
			break;

		if (!CryptHashData(hHash, (const unsigned char *)pdata, datalen, 0 )) 
			break;

		if(!CryptGetHashParam(hHash,HP_HASHVAL ,NULL,&dwHashLen,0))
			break;

		if(!CryptGetHashParam(hHash,HP_HASHVAL,pHashValue,&dwHashLen,0))
			break;

		for (i=0; i<(int)dwHashLen; i++)
			_stprintf(&(szHashBuf[i*2]), _T("%02X"), pHashValue[i]);

		szHash = szHashBuf;

	} while (0);

	lRet =  GetLastError();
	if(hHash)  
        CryptDestroyHash(hHash);
	if(hCryptProv) 
        CryptReleaseContext(hCryptProv, 0);

	return lRet;
}



LONG 
CCertKitCsp::VerifySignData(const TCHAR* szdata, const TCHAR* szSignature, const TCHAR* szCert, ulong ulOpt)
{
	LONG lRet = ERROR_SUCCESS;
	if (!szdata || !szSignature || !szCert)
		return ERROR_BAD_ARGUMENTS;

	DWORD ldatalen = _tcslen(szdata)*2;
	CByteBuf pbDatebuf(ldatalen);
	CBase64::Decode(szdata, pbDatebuf.ptr, &ldatalen);

	DWORD lsiglen = _tcslen(szSignature)*2;
	CByteBuf pbSignature(lsiglen);
	CBase64::Decode(szSignature, pbSignature.ptr, &lsiglen);

	HCRYPTPROV hProv = NULL;
	HCRYPTKEY  hPubKey = NULL;
	HCRYPTHASH hHash = NULL;
	
	CCertificate Cert(szCert);

	do 
	{
		if (!Cert.IsAttached())
			break;
		
		if(!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, 0))
			break;

		if(!CryptImportPublicKeyInfo(hProv, XENCODETYPE, &(Cert.GetContext()->pCertInfo->SubjectPublicKeyInfo), &hPubKey))
			break;


		if(!CryptCreateHash(hProv, m_iHashAlg, 0, 0, &hHash)) 
			break;
		
		if (!CryptHashData(hHash, pbDatebuf.ptr, ldatalen, 0 )) 
			break;

		if (!CryptVerifySignature(hHash, pbSignature.ptr, lsiglen, hPubKey, NULL, 0))
			break;
		
	} while (0);

	lRet =  GetLastError();

	if(hHash)  
        CryptDestroyHash(hHash);
	if(hProv)  
        CryptReleaseContext(hProv, 0);

	return lRet;
}

LONG 
CCertKitCsp::VerifySignature(const TCHAR* szMsg, const TCHAR* szSignature, ulong ulOpt)
{
	LONG lRet = ERROR_SUCCESS;
	if (!szMsg || !szSignature)
		return ERROR_BAD_ARGUMENTS;

	CByteBuf pbMsgbuf(_tcslen(szMsg));
	DWORD lMsglen = _tcslen(szMsg);
	CBase64::Decode(szMsg, pbMsgbuf.ptr, &lMsglen);
	
	CByteBuf pbSignature(_tcslen(szSignature));
	DWORD lsiglen = _tcslen(szSignature);
	CBase64::Decode(szSignature, pbSignature.ptr, &lsiglen);
	
	const BYTE* pMsg = pbMsgbuf.ptr;
	
	PCCERT_CONTEXT pSignerCert;
	CRYPT_VERIFY_MESSAGE_PARA param={sizeof(param),XENCODETYPE,0,NULL,0};
	if(!CryptVerifyDetachedMessageSignature(&param,0,pbSignature.ptr,lsiglen,1,&pMsg, &lsiglen, &pSignerCert))
		return GetLastError();
	
	if (pSignerCert) 
        CertFreeCertificateContext(pSignerCert);
	return lRet;
}

BOOL  
CCertKitCsp::SzDN2iMap(const TCHAR* szDN)
{

    return TRUE;
}

BOOL 
CCertKitCsp::GetCspCertList(Tcerlist &vcCertList)
{
    vcCertList.clear();
    
    EnumCtn();
    int sizea = m_CtnList.size();
    if (m_CtnList.size() == 0)
        return FALSE;

    for(unsigned int i=0; i<m_CtnList.size(); i++)
    {
        AutoHProv hProv;
        AutoHPkey hUserKeyX;
        AutoHPkey hUserKeyS;
        CByteBuf certBufX(4096);
        CByteBuf certBufS(4096);
        DWORD	ulCertLen = 4096;

        if(!CryptAcquireContext(hProv, m_CtnList[i].c_str(), m_sCspName.c_str(), m_lProvType, 0))
            continue;

        if(CryptGetUserKey(hProv, AT_KEYEXCHANGE, hUserKeyX)){
            if(CryptGetKeyParam(hUserKeyX, KP_CERTIFICATE, certBufX.ptr,  &ulCertLen, 0)){

                CCertificate curCert;
                curCert.Attach(certBufX.ptr, ulCertLen);
                curCert.SetCtnName(m_CtnList[i].c_str());
                curCert.SetKeySpec(AT_KEYEXCHANGE);
                curCert.SetProvName(m_sCspName.c_str());


                if(curCert.IsAttached())
                    m_CspCertList.push_back(curCert);
            }
        }

        if(CryptGetUserKey(hProv, AT_SIGNATURE, hUserKeyS)){
            if(CryptGetKeyParam(hUserKeyS, KP_CERTIFICATE, certBufS.ptr,  &ulCertLen, 0)){

                CCertificate curCert;
                curCert.Attach(certBufS.ptr, ulCertLen);
                curCert.SetCtnName(m_CtnList[i].c_str());
                curCert.SetKeySpec(AT_SIGNATURE);
                curCert.SetProvName(m_sCspName.c_str());

                if(curCert.IsAttached())
                    m_CspCertList.push_back(curCert);
            }
        }
    }

    if (m_CspCertList.size() == 0)
        return FALSE;

    vcCertList = m_CspCertList;
    return  TRUE;
}

BOOL
CCertKitCsp::GetMyStroeCertList(Tcerlist &vcCertList)
{

    return TRUE;
}


BOOL CCertKitCsp::PemHandle(const TCHAR* szPem, tstring& szB64)
{
	tstring szBuf = szPem;

	if (tstring::npos != szBuf.find(_T("----")))
	{

		if(tstring::npos != szBuf.find_first_of(_T("\r\n")))
		{
			size_t  size1 = 0, size2 = 0;
			size1 = szBuf.find_first_of(_T("\r\n"));
			size2 = szBuf.find_last_of(_T("\r\n"));

			szBuf = szBuf.substr(size1, size2 - size1);
		}
		else if(tstring::npos != szBuf.find_first_of(_T("\n")))
		{
			size_t  size1 = 0, size2 = 0;
			size1 = szBuf.find_first_of(_T("\n"));
			size2 = szBuf.find_last_of(_T("\n"));
			
			szBuf = szBuf.substr(size1, size2 - size1);
		}
		else if(tstring::npos != szBuf.find_first_of(_T("\r")))
		{
			size_t  size1 = 0, size2 = 0;
			size1 = szBuf.find_first_of(_T("\r"));
			size2 = szBuf.find_last_of(_T("\r"));
			
			szBuf = szBuf.substr(size1, size2 - size1);			
		}
	}


	tstring::iterator itr = szBuf.end();
	itr--;
	for (; itr != szBuf.begin(); itr--)
	{
		if(_T('\n') == *itr || _T('\r') == *itr){
			szBuf.erase(itr);
			itr = szBuf.end();
			itr--;
		}
	}

	szB64 = szBuf;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
