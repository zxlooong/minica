//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
using namespace std;

#include "openssl/asn1.h"
#include "openssl/x509.h"
#include "openssl/x509v3.h"
#include "openssl/pkcs7.h"
#include "openssl/evp.h"
#include "openssl/pkcs12.h"
#include "openssl/hmac.h"
//#include "openssl/err.h"
#include "openssl/objects.h"


#include "cryptoki.h"

#if defined(_WINDOWS) || defined(WIN32)
#   include <windows.h>
#   include <io.h>
#	pragma warning(disable : 4786)
#else
#	include <dlfcn.h>
#   include <unistd.h>
#endif


#define ESPTAP11_CFG_NAME           "esp11.cfg"
#define ESPTAP11_CFG_SECTION        "main"
#define ESPTAP11_CFG_KEY            "p11path"
//////////////////////////////////////////////////////////////////////////
//#include "config.h"
//#include "base_type.h"
//#include "funclog.h"
//#include "dbglog.h"
#include "MemToolsX.h"
#include "CPEleven.h"
//////////////////////////////////////////////////////////////////////////


using namespace MemToolsX;


//////////////////////////////////////////////////////////////////////////


LONG CPEleven::Reset()
{
    //lib token session
	if(m_hSession)
		m_FList->C_CloseSession(m_hSession);

	if(m_FList)
		m_FList->C_Finalize(NULL_PTR);

	if(m_hP11Lib)
	{
#if defined(WIN32) || defined(_WINDOWS)
		FreeLibrary(m_hP11Lib);
#else
		dlclose(m_hP11Lib);
#endif
	}

	m_hP11Lib  = CK_INVALID_HANDLE;
	m_FList    = CK_INVALID_HANDLE;
	m_hSession = CK_INVALID_HANDLE;

    
    
    // token cert list
	m_lTokenCount = 0;
	m_lCertCount  = 0;
	m_lCertIdx    = 0;
	m_lTokenIdx   = 0;
    m_usrPinlen   = 0;
    
	memset(m_SlotList,      0, sizeof(m_SlotList));
	memset(m_TokenInfoList, 0, sizeof(m_TokenInfoList));
	memset(m_CertList,      0, sizeof(m_CertList));
	memset(m_CertLen,       0, sizeof(m_CertLen));
	memset(m_usrPin,        0, sizeof(m_usrPin));
    
    
	//cert req
	m_lReqKeyLen  = 1024;
	m_lReqKeySpec = KEYSPEC_EXCHANGE;
	m_lReqCertValidDays = 365;
	m_tsReqRdns.clear();
	GenGUIDStr(m_sReqCtnName);

	
    //p11 cfg path
    m_szP11Path = "";
	m_tszP11Paths.clear();

    DefaultCfgPath(m_szCfgPath);    
	m_szCfgSection = ESPTAP11_CFG_SECTION;
	m_szCfgKey = ESPTAP11_CFG_KEY;
    
    //if(0 != LoadCfgP11Lib())
    //{
    //    printf("P11 path err or load p11 error!\n");
    //}
    
    // error code ret
    m_lLastError = 0;

	return CKR_OK;
}



CPEleven::CPEleven()
{
	m_hP11Lib  = CK_INVALID_HANDLE;
    m_FList    = CK_INVALID_HANDLE;
	m_hSession = CK_INVALID_HANDLE;

	Reset();
    
	LoadCfgP11Lib();
            

}


CPEleven::CPEleven(cchar* szP11Path)
{
	m_hP11Lib  = CK_INVALID_HANDLE;
    m_FList    = CK_INVALID_HANDLE;
	m_hSession = CK_INVALID_HANDLE;


	Reset();

	do{
		if(!LoadP11Lib(szP11Path))
			break;

		//ParseP11Path();

		//if(0 == m_tszP11Paths.size())
		//	break;

		//SetP11Path(szP11Path);

		m_szP11Path = szP11Path;

	}while(0);

//	OpenSSL_add_all_algorithms();

}

CPEleven::~CPEleven()
{
	Reset();
}

//////////////////////////////////////////////////////////////////////////////

LONG CPEleven::LoadP11Lib(cchar* szPath)
{
	LONG rv;
	if( !szPath || (access(szPath,0) != 0) )
	{
		return CKR_P11LIB_INVALID;
	}

#if defined(WIN32) || defined(_WINDOWS)
	m_hP11Lib = LoadLibraryA(szPath);
 #else
 	m_hP11Lib = dlopen(szPath, RTLD_LAZY);
 #endif

	if(m_hP11Lib==NULL)
	{
		return CKR_P11LIB_INVALID;
	}

	LONG (*p11_get_function_list_func)(CK_FUNCTION_LIST_PTR_PTR);
#if defined(WIN32) || defined(_WINDOWS)
	p11_get_function_list_func = 
                (LONG (*)(CK_FUNCTION_LIST_PTR_PTR)) 
                        GetProcAddress(m_hP11Lib, "C_GetFunctionList");
#else
	p11_get_function_list_func = 
                (LONG (*)(CK_FUNCTION_LIST_PTR_PTR)) 
                        dlsym(m_hP11Lib, "C_GetFunctionList");
#endif
	if(!p11_get_function_list_func)
	{
		return CKR_P11LIB_INVALID;
	}

	rv = p11_get_function_list_func(&m_FList);

	if (CKR_OK == rv) {
		P11Init();
	}

	if(rv)
		return rv;
	else
		return CKR_OK;
}




void CPEleven::OsslInit()
{
	OpenSSL_add_all_algorithms();	
}

void CPEleven::OsslFinal()
{
	
}

LONG CPEleven::P11Init()
{
    LONG rv = CKR_OK;

	if (!m_FList)
		return CKR_GENERAL_ERROR;

    rv = m_FList->C_Initialize(NULL_PTR);
	
	if ( CKR_OK != rv && CKR_CRYPTOKI_ALREADY_INITIALIZED != rv ) {
		return rv;
	} else {
		return CKR_OK;
    }
}

LONG CPEleven::P11Final()
{
	if(m_hSession)
		m_FList->C_CloseSession(m_hSession);

	if(m_FList)
		m_FList->C_Finalize(NULL_PTR);

	return CKR_OK;
}


LONG CPEleven::SetP11Path(cchar* szPath)
{
    LONG rv = CKR_OK;

    string szP11Pathtmp = m_szP11Path;

    do{
    	m_szP11Path = szPath;

    	Reset();

        if(CKR_OK == (rv = LoadP11Lib(szPath)))
        {
            if(CKR_OK == EnumTokens())
            	rv =LoadToken();

        	break;
        }

        m_szP11Path = szP11Pathtmp;

        if(CKR_OK != LoadP11Lib(szP11Pathtmp.c_str()))
        	break;

        if(CKR_OK == EnumTokens())
        	rv = LoadToken();

    }while(0);


    return rv;
}

LONG CPEleven::SetCfgPath(cchar* szPath)
{
    char  szcfgvalue[1024] = {0};
    int   valuelen = sizeof szcfgvalue;
    
    if(GetPrivateProfileString((char*)m_szCfgPath.c_str(),
                               (char*)m_szCfgSection.c_str(),
                               (char*)m_szCfgKey.c_str(),
                               szcfgvalue, &valuelen))
    	return -1;
    
    m_szP11Path = szPath;
    if('/' != szcfgvalue[0])
    	m_szP11Path += szcfgvalue;
    else
    	m_szP11Path = szcfgvalue;
    
    return SetP11Path(m_szP11Path.c_str());
}

LONG CPEleven::LoadCfgP11Lib()
{
    LONG rv = CKR_OK;

//     char *path = getenv("HOME");
//     if(0 == strlen(path))
//     	return -1;
	string szUserHome;
	GetUserHome(szUserHome);

    string szcfgpath = szUserHome;
	string::iterator itr = szcfgpath.end();

#if defined(WIN32) || defined(_WINDOWS)
    if('\\' != *(/*szcfgpath.end()*/ itr -- ))
    	szcfgpath += "\\";
#else
	if('/' != *(/*szcfgpath.end()*/ itr -- ))
    	szcfgpath += "/";
#endif
    szcfgpath += ESPTAP11_CFG_NAME;
    m_szCfgPath = szcfgpath;

    char  szcfgvalue[1024] = {0};
    int   valuelen = sizeof szcfgvalue;


    if(GetPrivateProfileString( (char*)m_szCfgPath.c_str(),
    							(char*)m_szCfgSection.c_str(),
    							(char*)m_szCfgKey.c_str(),
    							szcfgvalue, &valuelen))
    	return -1;

    m_szP11Path = szcfgpath;
    if('/' != szcfgvalue[0])
    	m_szP11Path += szcfgvalue;
    else
    	m_szP11Path = szcfgvalue;

    return SetP11Path(m_szP11Path.c_str());
}


LONG CPEleven::EnumTokens()
{
    LONG rv = CKR_OK;
	uint i = 0;
    m_lTokenCount = 0;
    memset(m_SlotList, 0, sizeof(m_SlotList));
	memset(m_TokenInfoList, 0, sizeof(m_TokenInfoList));

//    if(0 == (GetStatus() & STATUS_LIB_OK ))
//        return  CKR_P11LIB_INVALID;
//    if(0 == (GetStatus() & STATUS_INITED_OK))
//        return  CKR_CRYPTO_UNINIT;

    //CK_SLOT_ID  SlotList[16]={0};
	ULONG    SlotNum = 16;

	for(i = 0; i < 2; i++)
	{
		rv = m_FList->C_GetSlotList(CK_TRUE, m_SlotList, &SlotNum);
		if(rv == CKR_OK && SlotNum >= 1)
			break;
		else
			Sleep(500);
	}

	if(CKR_OK != rv)
		return CKR_DEVICE_REMOVED;

	if(SlotNum < 1)
		return CKR_DEVICE_REMOVED;

    m_lTokenCount = SlotNum;

    for(i=0; i<m_lTokenCount; i++)
    {
        rv = m_FList->C_GetTokenInfo(m_SlotList[i], &m_TokenInfoList[i]);
        if(CKR_OK != rv)
            return rv;
    }

    return rv;
}



//////////////////////////////////////////////////////////////////////////

LONG CPEleven::GetTokenSN(string &szUkeySN)
{
    LONG rv = CKR_OK;

    if(m_hP11Lib)
        return  CKR_P11LIB_INVALID;

    rv = EnumTokens();
    if(CKR_OK != rv)
        return rv;

    char sz[20]={0};
    int iSnLen = sizeof(m_TokenInfoList[m_lTokenIdx].serialNumber);
    memcpy(sz, m_TokenInfoList[m_lTokenIdx].serialNumber, iSnLen>16?16:iSnLen);
	szUkeySN = sz;

    return rv;
}

LONG CPEleven::SetTokenIdx(ULONG ulIdx)
{
    LONG rv = CKR_OK;

    if(ulIdx >= 0 && ulIdx < 16)
        m_lTokenIdx = ulIdx;

    //EnumToken();

    return rv;
}

LONG CPEleven::GetTokenCount(ULONG &ulCount)
{
    LONG rv = CKR_OK;

    ulCount = 0;

    rv = EnumTokens();
    if(CKR_OK != rv)
        return rv;

    ulCount = m_lTokenCount;

    return rv;
}

LONG CPEleven::GetTokenStatus()
{
    LONG ulStatus = 0;
    ulStatus = 0;

//    if(CK_INVALID_HANDLE != mhP11Lib)
//        ulStatus += STATUS_LIB_OK;
//    if(CK_INVALID_HANDLE != mFList)
//        ulStatus += STATUS_FLIST_OK;
//    if(0 != mlTokenCount)
//        ulStatus += STATUS_TOKEN_OK;
//    if(CK_TRUE == mbInited)
//        ulStatus += STATUS_INITED_OK;

    return ulStatus;
}

LONG CPEleven::LoginToken(cuchar* szPass, ULONG ulLen)
{
    LONG rv = CKR_OK;

	rv = m_FList->C_Login(m_hSession, CKU_USER, (uchar*)szPass, ulLen);
	if(!(CKR_OK == rv || CKR_USER_ALREADY_LOGGED_IN == rv))
		return rv;

    return CKR_OK;
}

LONG CPEleven::LoadToken(ULONG ulIdx)
{
    LONG rv = CKR_OK;

    rv = SetTokenIdx(ulIdx);
    if(CKR_OK != rv)
        return rv;

    rv = LoadToken();
    if(CKR_OK != rv)
        return rv;

    return rv;
}

LONG CPEleven::LoadToken()
{
    LONG rv = CKR_OK;

    rv = EnumTokens();
    if(CKR_OK != rv)
        return rv;

    rv = m_FList->C_OpenSession(m_SlotList[m_lTokenIdx],
								CKF_RW_SESSION | CKF_SERIAL_SESSION,
								NULL_PTR, 0, &m_hSession );
    if(CKR_OK != rv)
        return rv;

    return rv;
}

LONG CPEleven::EnumCerts()
{
    LONG rv = CKR_OK;

    rv = EnumTokens();
    if(CKR_OK != rv)
        return rv;

    rv = LoadToken();
    if(CKR_OK != rv)
        return rv;

	memset(m_CertList, 0, sizeof(m_CertList));
	memset(m_CertLen, 0, sizeof(m_CertLen));
	m_lCertCount = 0;

	CK_OBJECT_CLASS certClass = CKO_CERTIFICATE;
	CK_ATTRIBUTE CertTemplate[] =
	{
		{CKA_CLASS, &certClass, sizeof(CK_OBJECT_CLASS)}
	};

	rv = m_FList->C_FindObjectsInit(m_hSession, CertTemplate, 1);
    if(CKR_OK != rv)
        return rv;

	CK_OBJECT_HANDLE hCKObj = NULL;
	ULONG ulRetCount = 0;
	for(int i = 0; i < sizeof(m_CertList)/sizeof(*m_CertList); i++)
	{
		rv = m_FList->C_FindObjects(m_hSession, &hCKObj, 1, &ulRetCount);
		if(CKR_OK != rv || 1 != ulRetCount)
		{
			break;
		}

		CK_ATTRIBUTE pAttrTemp[] =
		{
			{CKA_VALUE, NULL, 0},
		};

		rv = m_FList->C_GetAttributeValue(m_hSession, hCKObj, pAttrTemp, 
                                          sizeof(pAttrTemp)/sizeof(CK_ATTRIBUTE));
		if(rv != CKR_OK)
			break;

		CByteBuf pValue(pAttrTemp[0].ulValueLen);

		pAttrTemp[0].pValue = pValue.ptr;

		rv = m_FList->C_GetAttributeValue(m_hSession, hCKObj, pAttrTemp, 
                                          sizeof(pAttrTemp)/sizeof(CK_ATTRIBUTE));
		if(rv != CKR_OK)
			break;

		memcpy(m_CertList[i], pAttrTemp[0].pValue, pAttrTemp[0].ulValueLen);
		m_CertLen[i] = pAttrTemp[0].ulValueLen;
        m_lCertCount++;
	}

	rv = m_FList->C_FindObjectsFinal(m_hSession);

    return rv;
}

LONG CPEleven::GetCertCount(ULONG &ulCount)
{
    LONG rv = CKR_OK;

    rv = EnumCerts();
    if(CKR_OK != rv)
        return rv;

    ulCount = m_lCertCount;

    return rv;
}


LONG CPEleven::CreateReqP10(string& szP10)
{
	LONG	rv = CKR_OK;
	int		iret = 0;

    rv = EnumTokens();
    if(CKR_OK != rv)
        return rv;

    rv = LoadToken();
    if(CKR_OK != rv)
        return rv;

	X509_REQ	*req = X509_REQ_new();
	RSA			*rsa = RSA_new();
	EVP_PKEY	*pkey = EVP_PKEY_new();
	X509_NAME	*name;// = X509_NAME_new();

	CK_MECHANISM rsasha1 = {CKM_SHA1_RSA_PKCS, 0, 0};
	//EVP_ENCODE_CTX ctx;

	CK_OBJECT_HANDLE hPubKey = NULL;
	CK_OBJECT_HANDLE hPrvKey = NULL;
	CK_BYTE_PTR pbPubMod = NULL;
	CK_BYTE_PTR pbPubExp = NULL;
	ULONG cbPubMod = 0;
	ULONG cbPubExp = 0;

	int reqinfodlen = 0;

	int reqderlen = 0;
	int reqB64len = 0;

	do
	{
		//stuff req_info ---------------
		rv = GenKeyPair(&hPubKey,  &hPrvKey);
		if (CKR_OK != rv)
			break;

		rv = GetPubKeyNE(hPubKey, &pbPubMod, &cbPubMod, &pbPubExp, &cbPubExp);
		if (CKR_OK != rv)
			break;

		rsa->n = BN_bin2bn((cuchar *)pbPubMod, cbPubMod, rsa->n);
		rsa->e = BN_bin2bn((cuchar *)pbPubExp, cbPubExp, rsa->e);

		EVP_PKEY_assign_RSA(pkey, rsa);

		X509_REQ_set_version(req, 0);
		X509_REQ_set_pubkey(req, pkey);

		//TODO:stuff dn
		name = X509_REQ_get_subject_name(req);
		X509_NAME_add_entry_by_txt(name,"CN", MBSTRING_ASC, (cuchar*)"P11_Eay", -1, -1, 0);
		if(!name)
			break;

		//stuff alg --------------
		if(!req->sig_alg)
			req->sig_alg = X509_ALGOR_new( );

		ASN1_OBJECT* algo = OBJ_dup(OBJ_nid2obj(NID_sha1WithRSAEncryption));
		req->sig_alg->algorithm = algo;
		req->sig_alg->parameter = ASN1_TYPE_new();
		ASN1_TYPE_set(req->sig_alg->parameter, V_ASN1_NULL, NULL_PTR);

		//stuff signature --------
		reqinfodlen = i2d_X509_REQ_INFO( req->req_info, NULL);
		CByteBuf reqinfod(reqinfodlen);
		reqinfodlen = i2d_X509_REQ_INFO( req->req_info, &reqinfod.ptr2);

		rv = m_FList->C_SignInit(m_hSession, &rsasha1, hPrvKey);
		if(rv != CKR_OK)
			break;
		ULONG reqinfodsignedlen = 0;
		rv = m_FList->C_Sign(m_hSession, reqinfod.ptr, reqinfodlen, 
                             NULL, &reqinfodsignedlen);
		if(rv != CKR_OK)
			break;
		CByteBuf reqinfodsigned((int)reqinfodsignedlen+128);
		rv = m_FList->C_Sign(m_hSession, reqinfod.ptr, reqinfodlen, 
                             reqinfodsigned.ptr, &reqinfodsignedlen);
		if(rv != CKR_OK)
			break;

		req->signature = ASN1_BIT_STRING_new();
		if(req->signature)
			iret = ASN1_BIT_STRING_set(req->signature, reqinfodsigned.ptr, reqinfodsignedlen);


		//verify -----------------
		EVP_PKEY *pktmp = NULL;
		pktmp = X509_REQ_get_pubkey(req);
		if (!pktmp)
			break;
		iret = X509_REQ_verify(req, pktmp);
 		if(1 != iret)
 			break;

		//b64 out ----------------
		reqderlen = i2d_X509_REQ(req,NULL);
		CByteBuf reqBuf(reqderlen);
		reqderlen = i2d_X509_REQ(req, &reqBuf.ptr2);
		CByteBuf reqB64(reqderlen*2);
		reqB64len = EVP_EncodeBlock(reqB64.ptr, reqBuf.ptr, reqderlen);

		szP10 = (char*)reqB64.ptr;

		//file log
		/*
		FILE *fp = NULL;
		fp=fopen("c:\\windows\\temp\\crypto11_reqp10.log","at");
		if(fp)
		{
			fprintf(fp,"%s",szP10.c_str());
			fclose(fp);
		}
		*/

	} while (0);

	if(pbPubMod)	delete[] pbPubMod;
	if(pbPubExp)	delete[] pbPubExp;
	if(pkey)		EVP_PKEY_free(pkey);
//	if(rsa)			RSA_free(rsa);
	if(req)			X509_REQ_free(req);

	return rv;
}


LONG CPEleven::GenKeyPair(ULONG* phPubObj, ULONG* phPriObj)
{
	LONG		trimlen = 0;

	CK_MECHANISM	rsaMechanism = {CKM_RSA_PKCS_KEY_PAIR_GEN, NULL_PTR, 0};
	CK_BBOOL		bTrue	= TRUE;
	CK_BBOOL		bFalse	= FALSE;
	LONG			modulusBits = m_lReqKeyLen;
	LONG			pubExp = 65537;

	char szKeyID[64] = {0};

	GenGUIDStr(m_sReqCtnName);
	strcpy(szKeyID, m_sReqCtnName.c_str());

	//Create a pubkey attribute
	CK_ATTRIBUTE	pPubTemplate[] ={
		{CKA_TOKEN,				&bTrue,			sizeof(CK_BBOOL)},
		{CKA_ENCRYPT,			&bTrue,			sizeof(CK_BBOOL)},
		{CKA_VERIFY,			&bTrue,			sizeof(CK_BBOOL)},
		{CKA_VERIFY_RECOVER,	&bTrue,			sizeof(CK_BBOOL)},
		{CKA_WRAP,				&bTrue,			sizeof(CK_BBOOL)},
		{CKA_MODULUS_BITS,		&modulusBits,	sizeof(modulusBits)},
		{CKA_PUBLIC_EXPONENT,	&pubExp,		sizeof(pubExp)},
		{CKA_ID,				szKeyID,		strlen(szKeyID) + 1},
	//	{CKA_CONTAINER_NAME,	szContainer,	strlen(szContainer) + 1}
	};
	LONG	ulPubCount = sizeof(pPubTemplate)/sizeof(CK_ATTRIBUTE);

	//create a private attribute
	CK_ATTRIBUTE	pPrvTemplate[] ={
		{CKA_TOKEN,			&bTrue,		sizeof(CK_BBOOL)},
		{CKA_PRIVATE,		&bTrue,		sizeof(CK_BBOOL)},
		{CKA_SENSITIVE,		&bTrue,		sizeof(CK_BBOOL)},
		{CKA_DECRYPT,		&bTrue,		sizeof(CK_BBOOL)},
		{CKA_SIGN,			&bTrue,		sizeof(CK_BBOOL)},
		{CKA_SIGN_RECOVER,	&bTrue,		sizeof(CK_BBOOL)},
		{CKA_UNWRAP,		&bTrue,		sizeof(CK_BBOOL)},
		{CKA_ID,			szKeyID,	strlen(szKeyID) + 1},
	//	{CKA_CONTAINER_NAME, szContainer, strlen(szContainer) + 1}
	};

	LONG ulPrvCount = sizeof(pPrvTemplate)/sizeof(CK_ATTRIBUTE);

	LONG ret =  m_FList->C_GenerateKeyPair(m_hSession, &rsaMechanism,
				pPubTemplate, ulPubCount, pPrvTemplate, ulPrvCount, phPubObj, phPriObj);

	return ret;
}


LONG CPEleven::GetPubKeyNE(	ULONG hPubObj, uchar** pbPubkeyNValue, ULONG *cbPubNLen, 
							uchar** pbPubkeyEValue, ULONG *cbPubELen)
{
	LONG rv = CKR_OK;
	int nlen = 0,elen = 0;

	CK_ATTRIBUTE attrPubKey[] =
	{
		{CKA_MODULUS,			NULL_PTR,	0},
		{CKA_PUBLIC_EXPONENT,	NULL_PTR,	0}
	};
	LONG ulAttrCount = sizeof(attrPubKey)/sizeof(CK_ATTRIBUTE);

	rv = m_FList->C_GetAttributeValue(m_hSession, hPubObj, attrPubKey, ulAttrCount);

	if(rv != CKR_OK || attrPubKey[0].ulValueLen <= 0 || attrPubKey[1].ulValueLen <= 0)
		return CKR_GENERAL_ERROR;


	nlen = attrPubKey[0].ulValueLen;
	elen = attrPubKey[1].ulValueLen;
	attrPubKey[0].pValue = new CK_BYTE[nlen];
	attrPubKey[1].pValue = new CK_BYTE[elen];

	rv = m_FList->C_GetAttributeValue(m_hSession, hPubObj, attrPubKey, ulAttrCount);
	if(rv != CKR_OK) {
		delete[] attrPubKey[0].pValue;
		delete[] attrPubKey[1].pValue;
		return rv;
	}

	*pbPubkeyNValue = (uchar*)attrPubKey[0].pValue;
	*pbPubkeyEValue = (uchar*)attrPubKey[1].pValue;

	*cbPubNLen = attrPubKey[0].ulValueLen;
	*cbPubELen = attrPubKey[1].ulValueLen;

	return rv;
}


LONG CPEleven::GenGUIDStr(string &szCtnName)
{
	//eg: 07DFB0CF-5A31-4D67-8FA7-7DD855C37404CB
	LONG rv = CKR_OK;
	szCtnName = "";

	CK_CHAR change[] = {"ABCDEF0123456789"};

	srand(time(NULL));

	CK_CHAR szCtn1[9]={0};
	for (int i1=0; i1<8; i1++) {
		int irand = rand();
		irand = irand %(sizeof change - 1);
		szCtn1[i1] = change[irand];
	}
	szCtnName += (char*)szCtn1;
	szCtnName += "-";

	CK_CHAR szCtn2[5]={0};
	for (int i2=0; i2<4; i2++) {
		int irand = rand();
		irand = irand %(sizeof change - 1);
		szCtn2[i2] = change[irand];
	}
	szCtnName += (char*)szCtn2;
	szCtnName += "-";

	CK_CHAR szCtn3[5]={0};
	for (int i3=0; i3<4; i3++) {
		int irand = rand();
		irand = irand %(sizeof change - 1);
		szCtn3[i3] = change[irand];
	}
	szCtnName += (char*)szCtn3;
	szCtnName += "-";

	CK_CHAR szCtn4[5]={0};
	for (int i4=0; i4<4; i4++) {
		int irand = rand();
		irand = irand %(sizeof change - 1);
		szCtn4[i4] = change[irand];
	}
	szCtnName += (char*)szCtn4;
	szCtnName += "-";

	CK_CHAR szCtn5[15]={0};
	for (int i5=0; i5<14; i5++) {
		int irand = rand();
		irand = irand %(sizeof change - 1);
		szCtn5[i5] = change[irand];
	}
	szCtnName += (char*)szCtn5;

	return rv;
}


LONG CPEleven::InstallP7Cert(cchar* szP7)
{
	LONG rv = CKR_OK;
	int   irv = 0;

	// Pem 2 B64
	string szP7B64;
	if(PemHandle(szP7, szP7B64))
		return CKR_ARGUMENTS_BAD;

	// B64 2 Der
	EVP_ENCODE_CTX	evpEncodeCtx;
	CByteBuf derP7(szP7B64.length()*2);
	uchar* pderP7 = derP7.ptr;
	int iderP7Len = szP7B64.length()*2;
	EVP_DecodeInit(&evpEncodeCtx);
	irv = EVP_DecodeUpdate(&evpEncodeCtx, derP7.ptr, &iderP7Len, 
                           (cuchar*)szP7B64.c_str(), szP7B64.length());
	irv = EVP_DecodeFinal(&evpEncodeCtx, derP7.ptr, &iderP7Len);
	if(irv)
		return rv = CKR_VENDOR_DEFINED + irv;

	X509*				signer = NULL;
	STACK_OF(X509)*		signers = NULL;
	PKCS7*				p7 = NULL;
	CK_OBJECT_HANDLE	hPubkey = NULL;

	bool bFind = false;
	int i = 0;
	int iCertCount = 0;
	uchar derCert[4096] = {0};
	LONG derCertLen = sizeof derCert;

	//p7 = d2i_PKCS7(NULL, (cuchar**)&derP7.ptr, iderP7Len);
	p7 = d2i_PKCS7(NULL, (cuchar**)&pderP7, iderP7Len);
	if(!p7)
		return rv = -1; //  ERR_get_error();

	if(!PKCS7_type_is_signed(p7))
		return CKR_ARGUMENTS_BAD;

	signers = p7->d.sign->cert;
	iCertCount = sk_X509_num(signers);
	if(!signers || !iCertCount)
		return CKR_ARGUMENTS_BAD;

	// find cert
	for(i =0; i<sk_X509_num(signers);i++)
	{

		CByteBuf pbCertBuf(4096);
		uchar* pderCertBuf = pbCertBuf.ptr;
		signer = sk_X509_value(signers, i);
		//int iCertBuflen = i2d_X509(signer, &pbCertBuf.ptr);
		int iCertBuflen = i2d_X509(signer, &pderCertBuf);
		if(iCertBuflen <=0 )
			break;


		if(CKR_OK == FindPubKey(pbCertBuf.ptr, iCertBuflen, &hPubkey))
		{
			memcpy(derCert, pbCertBuf.ptr, iCertBuflen);
			derCertLen = iCertBuflen;
			bFind = true;

			break;
		}
	}

	// Install Cert
	if (bFind)
	{
		rv = InstallCert(hPubkey, derCert, derCertLen);
		if (rv)
			return rv;
	}
	else
		return CKR_ARGUMENTS_BAD;

	return rv;
}

LONG CPEleven::FindPubKey(uchar* derCert, ULONG cbCertLen, ULONG* phPubObj)
{
	LONG rv = CKR_OK;

	CK_BYTE	 pbPubMod[4096] = {0};
	ULONG ulPubModLen	= sizeof pbPubMod;

	rv = GetCertPubMod(derCert, cbCertLen, pbPubMod, &ulPubModLen);
	if(rv != CKR_OK)
		return rv;

	CK_OBJECT_HANDLE	hPKObj = NULL;
	ULONG				ulFindRet = 0;
	CK_OBJECT_CLASS		cls = CKO_PUBLIC_KEY;

	CK_ATTRIBUTE	FindCertTemplate[] =  {
		{CKA_CLASS,		&cls,		sizeof(CK_OBJECT_CLASS)},
		{CKA_MODULUS,	 pbPubMod,	ulPubModLen}
	};

	rv = m_FList->C_FindObjectsInit(m_hSession, FindCertTemplate, 
                                    sizeof(FindCertTemplate) / sizeof(CK_ATTRIBUTE));

	if(CKR_OK != rv)
		return rv;

	rv = m_FList->C_FindObjects(m_hSession, &hPKObj, 1, &ulFindRet);
	m_FList->C_FindObjectsFinal(m_hSession);

	if(ulFindRet) {
		*phPubObj = hPKObj;
		return CKR_OK;
	}

	return rv;
}


LONG CPEleven::InstallCert(ULONG hPubObj, cuchar* derCert, ULONG cbCertLen)
{
	LONG rv = CKR_OK;

	CK_OBJECT_HANDLE		hObj = NULL;
	CK_OBJECT_CLASS			certClass = CKO_CERTIFICATE;
	CK_CERTIFICATE_TYPE		certType = CKC_X_509;
	CK_BBOOL				bTrue  = TRUE;
	CK_BBOOL				bFalse = FALSE;
	X509*					curcertEay = NULL_PTR;

	CK_ATTRIBUTE	templateCert[] = {
		CKA_CLASS,				&certClass,		sizeof(certClass),
		CKA_CERTIFICATE_TYPE,	&certType,		sizeof(certType),
		CKA_LABEL,				NULL_PTR,		0,
		CKA_SERIAL_NUMBER,		NULL_PTR,		0,
		CKA_VALUE,				NULL_PTR,		0,
		CKA_TOKEN,				&bTrue,			sizeof(bTrue),
		CKA_PRIVATE,			&bFalse,		sizeof(bFalse),
//		CKA_CONTAINER_NAME,		NULL_PTR,		0,
		CKA_ID,					NULL_PTR,		0,
		CKA_SUBJECT,			NULL_PTR,		0,
		CKA_ISSUER,				NULL_PTR,		0
	};

	rv = m_FList->C_GetAttributeValue(m_hSession, hPubObj, &templateCert[7], 1);
	if(CKR_OK != rv || templateCert[7].ulValueLen == 0 )
		return rv;

	CByteBuf ckaidBuf(templateCert[7].ulValueLen);

	templateCert[7].pValue = ckaidBuf.ptr;
	rv = m_FList->C_GetAttributeValue(m_hSession, hPubObj, &templateCert[7], 1);
	if(CKR_OK != rv)
		return rv;

	//TODO: stuff cert
	//curcertEay = d2i_X509(&curcertEay, &derCert, cbCertLen);
	//if(!curcertEay)
	//	return CKR_ARGUMENTS_BAD;
	templateCert[4].pValue = (void*)derCert;
	templateCert[4].ulValueLen = cbCertLen;

	rv = m_FList->C_CreateObject(m_hSession, templateCert, 
                                 sizeof(templateCert)/sizeof(CK_ATTRIBUTE), &hObj);
	if(rv != CKR_OK)
		return rv;

	return rv;
}

LONG CPEleven::InstallCert( cuchar* derCert, ULONG cbCertLen )
{
	LONG rv = CKR_OK;
	int  irv = 0;

	if (!derCert || cbCertLen < 100)
		return CKR_ARGUMENTS_BAD;

	CK_OBJECT_HANDLE	hPubkey = NULL;
	CK_BBOOL			bFind   = FALSE;

	if(CKR_OK == FindPubKey((unsigned char *)derCert, cbCertLen, &hPubkey)) {
		bFind = TRUE;
	}

	// Install Cert
	if (bFind) {
		rv = InstallCert(hPubkey, derCert, cbCertLen);
		if (rv){
			return rv;
		}
	}else{
		return CKR_ARGUMENTS_BAD;
	}

	return rv;	
}

LONG CPEleven::InstallCert( cchar* szCert )
{
	LONG rv = CKR_OK;
	int   irv = 0;
	
	// PEM 2 B64
	string szCertB64;
	if(PemHandle(szCert, szCertB64))
		return CKR_ARGUMENTS_BAD;
	
	// B64 2 Der
	EVP_ENCODE_CTX	evpEncodeCtx;
	CByteBuf derCert(szCertB64.length()*2);
	//uchar* pderCert = derCert.ptr;
	int iderCertLen = szCertB64.length()*2;
	EVP_DecodeInit(&evpEncodeCtx);
	irv = EVP_DecodeUpdate(&evpEncodeCtx, derCert.ptr, &iderCertLen, 
		(cuchar*)szCertB64.c_str(), szCertB64.length());
	irv = EVP_DecodeFinal(&evpEncodeCtx, derCert.ptr, &iderCertLen);
	if(irv)
		return rv = CKR_VENDOR_DEFINED + irv;	

	return InstallCert(derCert.ptr, iderCertLen);
}


LONG CPEleven::GetCertPubMod(X509* x509Cert, uchar* cert_modulus, ULONG* modulus_len)
{
	if(!cert_modulus || !x509Cert)
		return CKR_ARGUMENTS_BAD;

	LONG  rv = CKR_OK;
	int    iret = 0;
 
	uchar* 		pPubkey = NULL_PTR;
	LONG		cbPubkey = 0;
	EVP_PKEY*	pkey = NULL_PTR;
	RSA*		rsa = NULL_PTR;

	unsigned char Mod[4096] = {0};
	unsigned long ulModLen = 0;

	do
	{
		pkey = X509_get_pubkey(x509Cert);
		if(!pkey){
			rv =  -1;  //ERR_get_error();
			break;
		}
		rsa = pkey->pkey.rsa;

		ulModLen = BN_num_bits(rsa->n)/8;
		ulModLen = BN_bn2bin(rsa->n, Mod);

		if (*modulus_len < ulModLen) {
			*modulus_len = ulModLen;
			break;
		}

		*modulus_len = ulModLen;
		memcpy(cert_modulus, Mod, ulModLen);

	} while (0);

	return rv;
}

LONG CPEleven::GetCertPubMod(uchar* derCert, ULONG cbCertLen, 
                              uchar* cert_modulus, ULONG* modulus_len)
{
	LONG rv = CKR_OK;

	uchar* pDerCert = derCert;
	X509 *x509 = d2i_X509(/**x509*/NULL, (cuchar**)&pDerCert, cbCertLen);

	if (x509)
		rv = GetCertPubMod(x509, cert_modulus, modulus_len);

	X509_free(x509);

	return rv;
}


void CPEleven::FlipBuffer(uchar *pBuf, ULONG ulLen)
{
	if(0 == ulLen || pBuf == NULL)
		return;

	unsigned char ucTemp;
	for(unsigned long i = 0; i < ulLen >> 1; ++i) {
		ucTemp = pBuf[i];
		pBuf[i] = pBuf[ulLen - i - 1];
		pBuf[ulLen - i - 1] = ucTemp;
	}
}

////////////////////////////////////////////////////////////////////////////////
bool CPEleven::PemHandle(cchar* szPem, string& szB64)
{
	string szBuf = szPem;

	if (string::npos != szBuf.find("----"))
	{

		if(string::npos != szBuf.find_first_of("\r\n"))
		{
			size_t  size1 = 0, size2 = 0;
			size1 = szBuf.find_first_of("\r\n");
			size2 = szBuf.find_last_of("\r\n");

			szBuf = szBuf.substr(size1, size2 - size1);
		}
		else if(string::npos != szBuf.find_first_of("\n"))
		{
			size_t  size1 = 0, size2 = 0;
			size1 = szBuf.find_first_of("\n");
			size2 = szBuf.find_last_of("\n");

			szBuf = szBuf.substr(size1, size2 - size1);
		}
		else if(string::npos != szBuf.find_first_of("\r"))
		{
			size_t  size1 = 0, size2 = 0;
			size1 = szBuf.find_first_of("\r");
			size2 = szBuf.find_last_of("\r");

			szBuf = szBuf.substr(size1, size2 - size1);
		}
	}


	string::iterator itr = szBuf.end();
	itr--;
	for (; itr != szBuf.begin(); itr--)
	{
		if('\n' == *itr || '\r' == *itr){
			szBuf.erase(itr);
			itr = szBuf.end();
			itr--;
		}
	}

	szB64 = szBuf;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
LONG CPEleven::SetReqCertValidDays(ULONG ulDays)
{
	LONG rv = CKR_OK;
	m_lReqCertValidDays = ulDays;
	return rv;
}


LONG CPEleven::GetReqCertValidDays(ULONG &ulDays)
{
	LONG rv = CKR_OK;
	ulDays = m_lReqCertValidDays;
	return rv;
}

LONG CPEleven::SetReqKeySpec(ULONG ulKeySpec)
{
	LONG rv = CKR_OK;
	m_lReqKeySpec = ulKeySpec;
	return rv;
}

LONG CPEleven::GetReqKeySpec(ULONG &ulKeySpec)
{
	LONG rv = CKR_OK;
	ulKeySpec = m_lReqKeySpec;
	return rv;
}


LONG CPEleven::SetReqKeyLen(ULONG ulKeyLen)
{
	LONG rv = CKR_OK;
	m_lReqKeyLen = ulKeyLen;
	return rv;
}

LONG CPEleven::GetReqKeyLen(ULONG &ulKeyLen)
{
	LONG rv = CKR_OK;
	ulKeyLen = m_lReqKeyLen;
	return rv;
}

LONG CPEleven::GetLastError()
{
    return m_lLastError;
}

LONG CPEleven::SetLastError(ULONG lError)
{
    m_lLastError = lError;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int CPEleven::GetUserHome(string& szHome)
{
#if defined(WIN32) || defined(_WINDOWS)
	HKEY myKey; 
	char*  regpath = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER,regpath,&myKey)) 
		return 1; 
	
	DWORD dwType; 
	DWORD dwSize = 200; 
	char path[200];
	
	if (ERROR_SUCCESS != RegQueryValueEx(myKey,(LPSTR)"Personal",NULL,&dwType,(BYTE *)path, &dwSize)) 
		return 1; 
	
	RegCloseKey(myKey);
#else
	char *path = getenv("HOME");
#endif
	szHome = path;
	return 0;
}


int CPEleven::DefaultCfgPath(string& szPath)
{
	string szDefPath;
	GetUserHome(szDefPath);
	szDefPath += "/";
	szDefPath += ESPTAP11_CFG_NAME;

	szPath = szDefPath;
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
int CPEleven::getOneLineString(char *fileName, char *sectionName, 
                           char *keyName, char *returnedString, int *psizeString)
{
	if(!sectionName || !keyName || !returnedString || !fileName || !psizeString)
		return -1;

	FILE *fp = 0;
	char strLine[256] = {0};
	char strTemp[256] = {0};
	bool inSection = false;
	char *token = 0;

	fp=fopen(fileName,"rt");
	if(!fp)
		return -1;

	while(NULL!=fgets(strLine, 256, fp))
	{
		strcpy(strTemp, strLine);
		if(*strTemp=='#' || *strTemp==';')
		{
			continue;
		}
		if(*strTemp=='[')
		{
			token=strTemp+1;
			strtok(token, "]");
			if(!strcmp(sectionName,token))
			{
				inSection=true;
				continue;
			}
			else
			{
				inSection=false;
				continue;
			}
		}
		else if(inSection)
		{
			strtok(strTemp, " =\t\n");
			if(!strcmp(keyName,strTemp))
			{
				if(returnedString)
				{
					strcpy(returnedString, strLine);
				}

				*psizeString=strlen(strLine);

				return 0;
			}
		}
	}

	return -1;
}

int CPEleven::GetPrivateProfileString(char *fileName, char *sectionName, 
                            char *keyName, char *returnedString, int* sizeString)
{
	if(!sectionName || !keyName || !returnedString || !fileName || !sizeString)
		return -1;

	int len = 0;
	char strLine[256] = {0};
	char strTemp[256] = {0};
	char *token = 0;

	if(0 != getOneLineString(fileName, sectionName, keyName, strLine, &len))
		return -1;

	token=strchr(strLine, ';');
	if(token)
		*token='\0';

	strcpy(strTemp,strLine);
	token=strtok(strTemp, " =\t\n");
	token=strtok(NULL, " =\t\n");

	if(token)
	{
		if(returnedString == NULL || *sizeString <= strlen(token))
			return -1;

		strcpy(returnedString, token);
		*sizeString = strlen(returnedString);
		return 0;
	}
	else
	{

		strcpy(returnedString, "");
		*sizeString = 0;

		return -1;
	}

	return -1;
}

int CPEleven::GetPrivateProfileInt(char *fileName, char *sectionName, char *keyName, int* ival)
{
//	int ret = 0;
	int len = 0;
	char strLine[256] = {0};
	char strTemp[256] = {0};
	char *token =0;

	if(getOneLineString(fileName, sectionName, keyName, strLine, &len))
		return -1;

	token=strchr(strLine, ';');
	if(token)

		*token='\0';

	strcpy(strTemp,strLine);
	token=strtok(strTemp, " =\t\n");
	token=strtok(NULL, " =\t\n");
	if(token)
	{
		*ival = atoi(token);
		return 0;
	}

	return 1;
}


///////////////////////////////////////////////////////////////////////////////
