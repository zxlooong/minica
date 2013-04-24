// CSmtp.h: interface for the CSmtp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSMTP_H__A0D4A072_65DE_11D2_9816_9523BDBAF506__INCLUDED_)
#define AFX_CSMTP_H__A0D4A072_65DE_11D2_9816_9523BDBAF506__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////
// CMailMessage
// Formats a message compliant with RFC 822
#include <afxtempl.h>

class CMailMessage  
{
public:
	CMailMessage();
	virtual ~CMailMessage();

	void FormatMessage();
	int GetNumRecipients();
	BOOL GetRecipient(CString& sEmailAddress, CString& sFriendlyName, int nIndex = 0);
	BOOL AddRecipient(LPCTSTR szEmailAddress, LPCTSTR szFriendlyName = "");
	BOOL AddMultipleRecipients(LPCTSTR szRecipients = NULL);
	UINT GetCharsPerLine();
	void SetCharsPerLine(UINT nCharsPerLine);

	CString m_sFrom;
	CString m_sSubject;
	CString m_sEnvelope;
	CString m_sMailerName;
	CString m_sHeader;
	CTime m_tDateTime;	
	CString m_sBody;
private:
	UINT m_nCharsPerLine;
	class CRecipient
	{
	public:
		CString m_sEmailAddress;
		CString m_sFriendlyName;
	};
	CArray <CRecipient, CRecipient&> m_Recipients;
protected:
	// When overriding prepare_header(), call base class 
	// version first, then add specialized 
	// add_header_line calls.
	// This ensures that the base class has a chance to
	// create the header lines it needs.
	virtual void prepare_header();
	virtual void prepare_body();
	virtual void end_header();
	virtual void start_header();

	// This rarely needs overwriting, but is virtual just in case.
	// Do not include the trailing CR/LF in parameter.
	virtual void add_header_line(LPCTSTR szHeaderLine);
};

// CMIMEContentAgent
// Abstract base class. Content agents support MIME
// content types on behalf of CMIMEMessage
//
class CMIMEContentAgent  
{
public:
	CMIMEContentAgent(int nMIMEType);
	virtual ~CMIMEContentAgent();
	BOOL QueryType(int nContentType);
	virtual BOOL AppendPart(LPCTSTR szContent,
							 LPCTSTR szParameters, 
							 int nEncoding, 
							 BOOL bPath, 
							 CString& sDestination) = 0;
	virtual CString GetContentTypeString() = 0;

protected:
	virtual CString build_sub_header(LPCTSTR szContent,
									  LPCTSTR szParameters,
									  int nEncoding,
									  BOOL bPath) = 0;

private:
	int m_nMIMETypeIHandle;
};

//////////////////////////////////////////////////////////////////////
// CMIMECode

class CMIMECode
{
public:
	CMIMECode();
	virtual ~CMIMECode();

	virtual int Decode( LPCTSTR szDecoding, LPTSTR szOutput ) = 0;
	virtual CString Encode( LPCTSTR szEncoding, int nSize ) = 0;
};

//////////////////////////////////////////////////////////////////////
// CBase64
// An encoding agent that handles Base64
class CBase64 : public CMIMECode  
{
public:
	CBase64();
	virtual ~CBase64();

// Override the base class mandatory functions
	virtual int Decode( LPCTSTR szDecoding, LPTSTR szOutput );
	virtual CString Encode( LPCTSTR szEncoding, int nSize );

protected:
	void write_bits( UINT nBits, int nNumBts, LPTSTR szOutput, int& lp );
	UINT read_bits( int nNumBits, int* pBitsRead, int& lp );

	int m_nInputSize;
	int m_nBitsRemaining;
	ULONG m_lBitStorage;
	LPCTSTR m_szInput;

	static int m_nMask[];
	static CString m_sBase64Alphabet;
private:
};

//////////////////////////////////////////////////////////////////////
// CAppOctetStream

class CAppOctetStream : public CMIMEContentAgent
{
public:
	virtual CString GetContentTypeString();
	CAppOctetStream( int nContentType );
	virtual ~CAppOctetStream();

	virtual BOOL AppendPart( LPCTSTR szContent, 
							 LPCTSTR szParameters, 
							 int nEncoding, 
							 BOOL bPath, 
							 CString& sDestination );

protected:
	virtual void attach_file( CStdioFile* pFileAtt, int nEncoding, CString& sDestination );
	virtual CString build_sub_header( LPCTSTR szContent, 
									  LPCTSTR szParameters, 
									  int nEncoding, 
									  BOOL bPath );
};

// CTextPlain
// A MIME content agent that handles the "text/plain"
// content type
class CTextPlain : public CMIMEContentAgent  
{
public:
	CTextPlain(int nContentType, UINT nWrapPos = 72);
	virtual ~CTextPlain();
	virtual BOOL AppendPart(LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, BOOL bPath, CString& sDestination);
	virtual CString GetContentTypeString();

protected:
	UINT m_nWrapPos;
	CString wrap_text(LPCTSTR szText);
	virtual CString build_sub_header(LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, BOOL bPath);
};

#include <afxmt.h>

// CMIMEMessage
// Formats a message using the MIME specification.

class CMIMEMessage : public CMailMessage  
{
public:
	CMIMEMessage();
	virtual ~CMIMEMessage();
	// MIME Type Codes
	enum eMIMETypeCode {
		TEXT_PLAIN = 0,
		APPLICATION_OCTETSTREAM,
		NEXT_FREE_MIME_CODE
	};
	enum eMIMEEncodingCode {
		_7BIT = 0,
		_8BIT,
		BINARY,
		QUOTED_PRINTABLE,
		BASE64,
		NEXT_FREE_ENCODING_CODE
	};
	BOOL AddMIMEPart(LPCTSTR szContent,
					  int nContentType = APPLICATION_OCTETSTREAM,
					  LPCTSTR szParameters = _T(""), 
					  int nEncoding = BASE64, 
					  BOOL bPath = TRUE);
protected:
    void insert_message_end(CString& sText);
	void register_mime_type(CMIMEContentAgent* pMIMEType);
	void insert_boundary(CString& sText);

	virtual void append_mime_parts();
	virtual void prepare_header();
	virtual void prepare_body();

	CString m_sNoMIMEText;
	CString m_sPartBoundary;
	CString m_sMIMEContentType;
private:
	class CMIMEPart
	{
	public:
		int m_nEncoding;
		int m_nContentType;
		CString m_sParameters;
		BOOL m_bPath;
		CString m_sContent;
	};
	CList <CMIMEPart, CMIMEPart&> m_MIMEPartList;

	class CMIMETypeManager
	{
	public:
		CMIMEContentAgent* GetHandler(int nContentType);
		void RegisterMIMEType(CMIMEContentAgent* pMIMEType);
		virtual  ~CMIMETypeManager();
		CMIMETypeManager();
	private:
		CCriticalSection m_csAccess;
		CList <CMIMEContentAgent*, CMIMEContentAgent*> m_MIMETypeList;
	};

	static CMIMETypeManager m_MIMETypeManager;
};

//////////////////////////////////////////////////////////////////////
// CSmtp
// Main class for SMTP

#include <afxsock.h>

#define SMTP_PORT 25		// Standard port for SMTP servers
#define RESPONSE_BUFFER_SIZE 1024

class CSmtp  
{
public:
	CSmtp(LPCTSTR szSMTPServerName, UINT nPort = SMTP_PORT);
	virtual ~CSmtp();

	void SetServerProperties(LPCTSTR szSMTPServerName, UINT nPort = SMTP_PORT);
	CString GetLastError();
	UINT GetPort();
	BOOL Disconnect();
	BOOL Connect();
	BOOL Auth();
	virtual BOOL FormatMailMessage(CMailMessage* msg);
	BOOL SendMessage(CMailMessage* msg);
	CString GetServerHostName();
	CString m_User;
	CString m_Pass;

private:
	BOOL get_response(UINT response_expected);
	CString cook_body(CMailMessage* msg);

	CString m_sError;
	BOOL m_bConnected;
	BOOL m_bAuthed;
	UINT m_nPort;
	CString m_sSMTPServerHostName;
	CSocket m_wsSMTPServer;

protected:
	virtual BOOL transmit_message(CMailMessage* msg);

//
// Helper Code
//
	struct response_code {
		UINT nResponse;		// Response we're looking for
		TCHAR* sMessage;	// Error message if we don't get it
	};

	enum eResponse {
		GENERIC_SUCCESS = 0,
		CONNECT_SUCCESS,
		AUTHQUE_SUCCESS,
		AUTH_SUCCESS,
		DATA_SUCCESS,
		QUIT_SUCCESS,
		// Include any others here
		LAST_RESPONSE	// Do not add entries past this one
	};
	TCHAR* response_buf;
	static response_code response_table[];
};

#endif // !defined(AFX_CSMTP_H__A0D4A072_65DE_11D2_9816_9523BDBAF506__INCLUDED_)
