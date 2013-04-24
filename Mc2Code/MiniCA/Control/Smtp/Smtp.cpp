// CSmtp.cpp: implementation of the CSmtp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smtp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CMailMessage
// Formats a message compliant with RFC 822.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailMessage::CMailMessage()
{
	m_sMailerName = "MiniCA Reg";
	SetCharsPerLine(76);
}

CMailMessage::~CMailMessage()
{
}

BOOL CMailMessage::AddRecipient(LPCTSTR szEmailAddress, LPCTSTR szFriendlyName)
{
	ASSERT(szEmailAddress != NULL);
	ASSERT(szFriendlyName != NULL);
	CRecipient to;
	to.m_sEmailAddress = szEmailAddress;
	to.m_sFriendlyName = szFriendlyName;
	m_Recipients.Add(to);
	return TRUE;
}

// sEmailAddress and sFriendlyName are OUTPUT parameters.
// If the function fails, it will return FALSE, and the OUTPUT
// parameters will not be touched.
BOOL CMailMessage::GetRecipient(CString & sEmailAddress, CString & sFriendlyName, int nIndex)
{
	CRecipient to;
	if(nIndex < 0 || nIndex > m_Recipients.GetUpperBound())
		return FALSE;
	to = m_Recipients[nIndex];
	sEmailAddress = to.m_sEmailAddress;
	sFriendlyName = to.m_sFriendlyName;
	return TRUE;
}

int CMailMessage::GetNumRecipients()
{ return m_Recipients.GetSize(); }

BOOL CMailMessage::AddMultipleRecipients(LPCTSTR szRecipients)
{
	TCHAR* buf;
	UINT pos;
	UINT start;
	CString sTemp;
	CString sEmail;
	CString sFriendly;
	UINT length;
	int nMark;
	int nMark2;

	ASSERT(szRecipients != NULL);

	// Add Recipients
	length = strlen(szRecipients);
	buf = new TCHAR[length + 1];	// Allocate a work area (don't touch parameter itself)
	strcpy(buf, szRecipients);
	for(pos = 0, start = 0; pos <= length; pos++) {
		if(buf[pos] == ';' || buf[pos] == 0) {
			// First, pick apart the sub-strings (separated by ';')
			//  Store it in sTemp.
			buf[pos] = 0;	// Redundant when at the end of string, but who cares.
			sTemp = &buf[start];
			// Now divide the substring into friendly names and e-mail addresses.
			nMark = sTemp.Find('<');
			if(nMark >= 0) {
				sFriendly = sTemp.Left(nMark);
				nMark2 = sTemp.Find('>');
				if(nMark2 < nMark) {
					delete[] buf;
					return FALSE;
				}
				// End of mark at closing bracket or end of string
				nMark2 > -1 ? nMark2 = nMark2 : nMark2 = sTemp.GetLength() - 1;
				sEmail = sTemp.Mid(nMark + 1, nMark2 - (nMark + 1));
			} else {
				sEmail = sTemp;
				sFriendly = _T("");
			}
			AddRecipient(sEmail, sFriendly);
			start = pos + 1;
		}
	}
	delete[] buf;
	return TRUE;
}

void CMailMessage::FormatMessage()
{
	start_header();
	prepare_header();
	end_header();
	prepare_body();
}

void CMailMessage::SetCharsPerLine(UINT nCharsPerLine)
{ m_nCharsPerLine = nCharsPerLine; }	

UINT CMailMessage::GetCharsPerLine()
{
	return m_nCharsPerLine;
}

// Create header as per RFC 822
//
void CMailMessage::prepare_header()
{
	CString sTemp;
	sTemp = _T("");

	// From:
	sTemp = _T("From: ") + m_sFrom;
	add_header_line((LPCTSTR) sTemp);

	// To:
	sTemp = _T("To: ");
	CString sEmail = _T("");
	CString sFriendly = _T("");
	for(int i = 0; i < GetNumRecipients(); i++) {
		GetRecipient(sEmail, sFriendly, i);
		sTemp += (i > 0 ? _T(",") : _T(""));
		sTemp += sFriendly;
		sTemp += _T("<");
		sTemp += sEmail;
		sTemp += _T(">");
	}
	add_header_line((LPCTSTR) sTemp);

	// Date:
	m_tDateTime = m_tDateTime.GetCurrentTime();

	// Format: Mon, 01 Jun 98 01:10:30 GMT
	sTemp = _T("Date: ");
	sTemp += m_tDateTime.Format("%a, %d %b %y %H:%M:%S %Z");
	add_header_line((LPCTSTR) sTemp);

	// Subject:
	sTemp = _T("Subject: ") + m_sSubject;
	add_header_line((LPCTSTR) sTemp);

	// X-Mailer
	sTemp = _T("X-Mailer: ") + m_sMailerName;
	add_header_line((LPCTSTR) sTemp);
}

void CMailMessage::prepare_body()
{
	// Append a CR/LF to body if necessary.
	if(m_sBody.Right(2) != _T("\r\n"))
		m_sBody += _T("\r\n");

}

void CMailMessage::start_header()
{ m_sHeader = _T(""); }

void CMailMessage::end_header()
{ m_sHeader += _T("\r\n"); }

void CMailMessage::add_header_line(LPCTSTR szHeaderLine)
{
	CString sTemp;
	sTemp.Format(_T("%s\r\n"), szHeaderLine);
	m_sHeader += sTemp;
}

//////////////////////////////////////////////////////////////////////
// CMIMEContentAgent
//////////////////////////////////////////////////////////////////////

CMIMEContentAgent::CMIMEContentAgent(int nMIMEType)
{ m_nMIMETypeIHandle = nMIMEType; }

CMIMEContentAgent::~CMIMEContentAgent()
{}

BOOL CMIMEContentAgent::QueryType(int nContentType)
{ return nContentType == m_nMIMETypeIHandle ? TRUE : FALSE; }


//////////////////////////////////////////////////////////////////////
// CMIMECode
//////////////////////////////////////////////////////////////////////

CMIMECode::CMIMECode()
{
}

CMIMECode::~CMIMECode()
{
}

//////////////////////////////////////////////////////////////////////
// CBase64
//////////////////////////////////////////////////////////////////////
// Static Member Initializers
//

// The 7-bit alphabet used to encode binary information
CString CBase64::m_sBase64Alphabet = 
_T( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" );

int CBase64::m_nMask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

CString CBase64::Encode(LPCTSTR szEncoding, int nSize)
{
	CString sOutput = _T( "" );
	int nNumBits = 6;
	UINT nDigit;
	int lp = 0;

	ASSERT( szEncoding != NULL );
	if( szEncoding == NULL )
		return sOutput;
	m_szInput = szEncoding;
	m_nInputSize = nSize;

	m_nBitsRemaining = 0;
	nDigit = read_bits( nNumBits, &nNumBits, lp );
	while( nNumBits > 0 )
	{
		sOutput += m_sBase64Alphabet[ (int)nDigit ];
		nDigit = read_bits( nNumBits, &nNumBits, lp );
	}
	// Pad with '=' as per RFC 1521
	while( sOutput.GetLength() % 4 != 0 )
	{
		sOutput += '=';
	}
	return sOutput;
}

// The size of the output buffer must not be less than
// 3/4 the size of the input buffer. For simplicity,
// make them the same size.
int CBase64::Decode(LPCTSTR szDecoding, LPTSTR szOutput)
{
	CString sInput;
    int c, lp =0;
	int nDigit;
    int nDecode[ 256 ];

	ASSERT( szDecoding != NULL );
	ASSERT( szOutput != NULL );
	if( szOutput == NULL )
		return 0;
	if( szDecoding == NULL )
		return 0;
	sInput = szDecoding;
	if( sInput.GetLength() == 0 )
		return 0;

	// Build Decode Table
	//
	for( int i = 0; i < 256; i++ ) 
		nDecode[i] = -2; // Illegal digit
	for( i=0; i < 64; i++ )
	{
		nDecode[ m_sBase64Alphabet[ i ] ] = i;
		nDecode[ m_sBase64Alphabet[ i ] | 0x80 ] = i; // Ignore 8th bit
		nDecode[ '=' ] = -1; 
		nDecode[ '=' | 0x80 ] = -1; // Ignore MIME padding char
    }

	// Clear the output buffer
	memset( szOutput, 0, sInput.GetLength() + 1 );

	// Decode the Input
	//
	for( lp = 0, i = 0; lp < sInput.GetLength(); lp++ )
	{
		c = sInput[ lp ];
		nDigit = nDecode[ c & 0x7F ];
		if( nDigit < -1 ) 
		{
			return 0;
		} 
		else if( nDigit >= 0 ) 
			// i (index into output) is incremented by write_bits()
			write_bits( nDigit & 0x3F, 6, szOutput, i );
    }	
	return i;
}




UINT CBase64::read_bits(int nNumBits, int * pBitsRead, int& lp)
{
    ULONG lScratch;
    while( ( m_nBitsRemaining < nNumBits ) && 
		   ( lp < m_nInputSize ) ) 
	{
		int c = m_szInput[ lp++ ];
        m_lBitStorage <<= 8;
        m_lBitStorage |= (c & 0xff);
		m_nBitsRemaining += 8;
    }
    if( m_nBitsRemaining < nNumBits ) 
	{
		lScratch = m_lBitStorage << ( nNumBits - m_nBitsRemaining );
		*pBitsRead = m_nBitsRemaining;
		m_nBitsRemaining = 0;
    } 
	else 
	{
		lScratch = m_lBitStorage >> ( m_nBitsRemaining - nNumBits );
		*pBitsRead = nNumBits;
		m_nBitsRemaining -= nNumBits;
    }
    return (UINT)lScratch & m_nMask[nNumBits];
}


void CBase64::write_bits(UINT nBits,
						 int nNumBits,
						 LPTSTR szOutput,
						 int& i)
{
	UINT nScratch;

	m_lBitStorage = (m_lBitStorage << nNumBits) | nBits;
	m_nBitsRemaining += nNumBits;
	while( m_nBitsRemaining > 7 ) 
	{
		nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
		szOutput[ i++ ] = nScratch & 0xFF;
		m_nBitsRemaining -= 8;
	}
}

//////////////////////////////////////////////////////////////////////
// CAppOctetStream
//////////////////////////////////////////////////////////////////////

// IMPORTANT: The number of bytes we read must be
//  a multiple of 3 because CBase64's Encode()
//  method will append padding characters ('=')
//  to make the output's size a multiple of 4.
//  (Base64 treats 3 8-bit bytes as 4 6-bit 'bytes').
//  MIME decoders are free to treat '=' as a signal
//  that there's no more data, so we don't want to pad
//  until we're supposed to.
// When at the end of the file, the # of bytes read
//  may not be a multiple of 3, but that's okay
//  because we DO want the padding chars then.

#define BYTES_TO_READ 54 // This number guarantess output won't
						 // won't exceed line-length limit

CAppOctetStream::CAppOctetStream(int nContentType)
	:CMIMEContentAgent(nContentType)
{
}

CAppOctetStream::~CAppOctetStream()
{
}

BOOL CAppOctetStream::AppendPart(LPCTSTR szContent, 
								 LPCTSTR szParameters, 
								 int nEncoding, 
								 BOOL bPath, 
								 CString & sDestination)
{
	CStdioFile fAttachment;

	ASSERT(szContent != NULL);
	// This class handles only file attachments, so
	// it ignores the bPath parameter.
	if(szContent == NULL)
		return FALSE;
	if(!fAttachment.Open(szContent, (CFile::modeRead|CFile::shareDenyWrite|CFile::typeBinary)))
		return FALSE;
	sDestination += build_sub_header(szContent,
								      szParameters,
									  nEncoding,
									  TRUE);
	attach_file(&fAttachment, CMIMEMessage::BASE64, sDestination );
	fAttachment.Close();
	return TRUE;
}


CString CAppOctetStream::build_sub_header(LPCTSTR szContent, 
										  LPCTSTR szParameters, 
										  int nEncoding, 
										  BOOL bPath)
{
	CString sSubHeader;
	CString sTemp;
	TCHAR szFName[ _MAX_FNAME ];
	TCHAR szExt[ _MAX_EXT ];

	_tsplitpath( szContent, NULL, NULL, szFName, szExt );

	// This class ignores szParameters and nEncoding.
	// It controls its own parameters and only handles
	// Base64 encoding.
	if( bPath )
		sTemp.Format( "; file=%s%s", szFName, szExt );
	else
		sTemp = _T( "" );
	sSubHeader.Format( _T( "Content-Type: %s%s\r\n" ),
						(LPCTSTR)GetContentTypeString(),
						(LPCTSTR)sTemp );
	sSubHeader += _T( "Content-Transfer-Encoding: base64\r\n" );
	sTemp.Format( _T( "Content-Disposition: attachment; filename=%s%s\r\n" ),
				  szFName, szExt );
	sSubHeader += sTemp;
	// Signal end of sub-header.
	sSubHeader += _T( "\r\n" ); // Warning: numerous concatenations
								// are inefficient.
	return sSubHeader;
}

CString CAppOctetStream::GetContentTypeString()
{
	CString s;
	s = _T( "application/octet-stream" );
	return s;
}

// Caller is responsible for opening and closing the file
void CAppOctetStream::attach_file(CStdioFile* pFileAtt, 
								  int nEncoding, 
								  CString & sDestination)
{
	CMIMECode* pEncoder;
	int nBytesRead;
	TCHAR szBuffer[ BYTES_TO_READ + 1 ];

	ASSERT( pFileAtt != NULL );
	if( pFileAtt == NULL )
		return;
	switch( nEncoding )
	{
		// This class handles only Base64 encoding, but others
		//  may be added here.
		default:
			// Fall through to...
		case CMIMEMessage::BASE64:
			try 
			{
				pEncoder = new CBase64;
			}
			catch( CMemoryException* e )
			{
				delete e;
				return;
			}
	}
	if( pEncoder == NULL )	// Old habits are hard to break
		return;
	do
	{
		try
		{
			nBytesRead = pFileAtt->Read( szBuffer, BYTES_TO_READ );
		}
		catch( CFileException* e )
		{
			delete e;
			break;
		}
		szBuffer[ nBytesRead ] = 0;	// Terminate the string
		sDestination += pEncoder->Encode( szBuffer, nBytesRead );
		sDestination += _T( "\r\n" );
	} while( nBytesRead == BYTES_TO_READ );
	sDestination += _T( "\r\n" );
	delete pEncoder;
}

//////////////////////////////////////////////////////////////////////
// CTextPlain
//////////////////////////////////////////////////////////////////////

CTextPlain::CTextPlain( int nContentType, UINT nWrapPos )
	:CMIMEContentAgent( nContentType )
{
	m_nWrapPos = nWrapPos;	
}

CTextPlain::~CTextPlain()
{

}

CString CTextPlain::GetContentTypeString()
{
	CString s;
	s = _T( "text/plain" );
	return s;
}

BOOL CTextPlain::AppendPart(LPCTSTR szContent, 
							LPCTSTR szParameters, 
							int nEncoding, 
							BOOL bPath, 
							CString & sDestination)
{
	CString sSubHeader;
	CString sWrapped;
	sSubHeader = build_sub_header( szContent,
								   szParameters,
								   nEncoding,
								   bPath );
	sWrapped = wrap_text( szContent );
	sDestination += (sSubHeader + sWrapped);
	return TRUE;
}

CString CTextPlain::build_sub_header(LPCTSTR szContent, 
									 LPCTSTR szParameters, 
									 int nEncoding, 
									 BOOL bPath)
{
	CString sSubHeader;
	sSubHeader.Format( _T( "Content-Type: %s%s\r\n" ),
					   (LPCTSTR)GetContentTypeString(),
					   szParameters );
	sSubHeader += _T( "Content-Transfer-Encoding: " );
	switch( nEncoding )
	{
		// This class handles only 7bit encoding, but others
		//  may be added here.
		default:
			//Fall through to...
		case CMIMEMessage::_7BIT:
			sSubHeader += _T( "7Bit" );
	}
	sSubHeader += _T( "\r\n\r\n" );
	return sSubHeader;
}

CString CTextPlain::wrap_text(LPCTSTR szText)
{
	CString sTemp;
	CString sLeft;
	CString sRight;
	int lp = 0;
	UINT nCount = 0;
	int nSpacePos = 0;

	ASSERT( szText != NULL );
	if( szText == NULL )
		return sTemp;
	sTemp = szText;
	while( lp < sTemp.GetLength() )
	{
		if( sTemp[ lp ] == ' ' )
			nSpacePos = lp;
		// Reset counter on newline
		if( sTemp.Mid( lp, 2 ) == _T( "\r\n" ) )
			nCount = 0;
		// Wrap text at last found space
		if( nCount > m_nWrapPos )
		{
			sLeft = sTemp.Left( nSpacePos );
			sRight = sTemp.Right( sTemp.GetLength() - nSpacePos );
			sLeft.TrimRight();
			sRight.TrimLeft();
			sLeft += _T( "\r\n" );
			sTemp = sLeft + sRight;
			nCount = 0;
		}
		else
			nCount++;
		lp++;
	}
	return sTemp;
}

//////////////////////////////////////////////////////////////////////
// CMIMEMessage
//////////////////////////////////////////////////////////////////////

// Static Member Initializers
CMIMEMessage::CMIMETypeManager CMIMEMessage::m_MIMETypeManager;

CMIMEMessage::CMIMEMessage()
{
	m_sMIMEContentType = _T( "multipart/mixed");
	m_sPartBoundary = _T( "WC_MAIL_PaRt_BoUnDaRy_05151998" );
	m_sNoMIMEText = _T( "This is a multi-part message in MIME format." );

	// Register the MIME types handled by this class
	//
	CMIMEContentAgent* pType;
	// These objects are deleted by CMIMTypeManager's destructor
	pType = new CTextPlain( TEXT_PLAIN, GetCharsPerLine() );
	register_mime_type( pType );
	pType = new CAppOctetStream( APPLICATION_OCTETSTREAM );
	register_mime_type( pType );
}

CMIMEMessage::~CMIMEMessage()
{
}

// This implementation adds the part to the part-list used
//  to build the body.
BOOL CMIMEMessage::AddMIMEPart(LPCTSTR szContent,
							   int nContentType, 
							   LPCTSTR szParameters, 
							   int nEncoding, 
							   BOOL bPath )
{
	CMIMEPart part;
	part.m_nContentType = nContentType;
	part.m_sParameters = szParameters;
	part.m_nEncoding = nEncoding;
	part.m_bPath = bPath;
	part.m_sContent = szContent;
	part.m_sContent.TrimLeft();
	part.m_sContent.TrimRight();
	if( nContentType == TEXT_PLAIN )
		m_MIMEPartList.AddHead( part );
	else
		m_MIMEPartList.AddTail( part );
	return TRUE;
}

void CMIMEMessage::prepare_header()
{
	CString sTemp;

	// Let the base class add its headers
	CMailMessage::prepare_header();
	
	add_header_line( _T( "MIME-Version: 1.0" ) );
	sTemp.Format( _T( "Content-Type: %s; boundary=%s" ),
				  (LPCTSTR)m_sMIMEContentType,
				  (LPCTSTR)m_sPartBoundary );
	add_header_line( (LPCTSTR)sTemp );
}

void CMIMEMessage::prepare_body()
{
	// Class user may have assigned body text directly.
	// Convert it to just another MIME part to be processed.
	// If this default Content-Type isn't good enough for the
	// class user, he or she should have used AddMIMEPart() instead.
	if( m_sBody != _T( "" ) )
		AddMIMEPart( (LPCTSTR)m_sBody, TEXT_PLAIN, "", _7BIT, FALSE );

	// Initialize the body (replace current contents).
	m_sBody = m_sNoMIMEText;
	m_sBody += _T( "\r\n\r\n" );
	append_mime_parts();
	insert_message_end( m_sBody );

	// Let the base class take me to Funky Town
	CMailMessage::prepare_body();
}

void CMIMEMessage::insert_boundary( CString& sText )
{
	CString sTemp;
	if( sText.Right( 2 ) != _T( "\r\n" ) )
		sText += _T( "\r\n" );
	sTemp.Format( _T( "--%s\r\n" ), (LPCTSTR)m_sPartBoundary );
	sText += sTemp;
}

void CMIMEMessage::insert_message_end( CString& sText )
{
	CString sTemp;
	if( sText.Right( 2 ) != _T( "\r\n" ) )
		sText += _T( "\r\n" );
	sTemp.Format( _T( "--%s--\r\n" ), (LPCTSTR)m_sPartBoundary );
	sText += sTemp;
}

void CMIMEMessage::register_mime_type(CMIMEContentAgent* pMIMEType)
{
	ASSERT( pMIMEType != NULL );
	if( pMIMEType == NULL )
		return;
	m_MIMETypeManager.RegisterMIMEType( pMIMEType );
}


void CMIMEMessage::append_mime_parts()
{
	POSITION part_position;
	CMIMEPart* pMIMEPart = NULL;
	CMIMEContentAgent* pMIMEType = NULL;

	part_position = m_MIMEPartList.GetHeadPosition();
	// Get each part from the list, retrieve a handler for it,
	//  and let the handler do its thing.
	while( part_position != NULL )
	{
		pMIMEPart = & m_MIMEPartList.GetNext( part_position );
		pMIMEType = m_MIMETypeManager.GetHandler( pMIMEPart->m_nContentType );
		if( pMIMEType != NULL )
		{
			insert_boundary( m_sBody );
			pMIMEType->AppendPart( pMIMEPart->m_sContent,
								   pMIMEPart->m_sParameters,
								   pMIMEPart->m_nEncoding,
								   pMIMEPart->m_bPath,
								   m_sBody );
		}
	}

}

//////////////////////////////////////////////////////////////////////
// CMIMETypeManager Implementation
//////////////////////////////////////////////////////////////////////

CMIMEMessage::CMIMETypeManager::CMIMETypeManager()
{
}

CMIMEMessage::CMIMETypeManager::~CMIMETypeManager()
{
	POSITION pos;
	CMIMEContentAgent* p;
	m_csAccess.Lock();
	pos = m_MIMETypeList.GetHeadPosition();
	while( pos != NULL )
	{
		p = m_MIMETypeList.GetNext( pos );
		delete p;
	}
}

void CMIMEMessage::CMIMETypeManager::RegisterMIMEType(CMIMEContentAgent *pMIMEType)
{
	ASSERT( pMIMEType != NULL );
	if( pMIMEType == NULL )
		return;
	m_csAccess.Lock();
	m_MIMETypeList.AddTail( pMIMEType );
}

CMIMEContentAgent* CMIMEMessage::CMIMETypeManager::GetHandler(int nContentType)
{
	POSITION pos;
	CMIMEContentAgent* pType = NULL;

	m_csAccess.Lock();
	pos = m_MIMETypeList.GetHeadPosition();
	while( pos != NULL )
	{
		pType = m_MIMETypeList.GetNext( pos );
		if( pType->QueryType( nContentType ) == TRUE )
			break;
	}
	return pType;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Static member initializers
//

// Note: the order of the entries is important.
//       They must be synchronized with eResponse entries. 
CSmtp::response_code CSmtp::response_table[] = {
	// GENERIC_SUCCESS
	{250, _T("SMTP server error")},
	// CONNECT_SUCCESS
	{220, _T("SMTP server not available")},
	// AUTHQUE_SUCCESS
	{334, _T("SMTP server authentication error")},
	// AUTH_SUCCESS
	{235, _T("Error username or password")},
	// DATA_SUCCESS
	{354, _T("SMTP server not ready for data")},
	// QUIT_SUCCESS
	{221, _T("SMTP server didn't terminate session")}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmtp::CSmtp(LPCTSTR szSMTPServerName, UINT nPort)
{
	ASSERT(szSMTPServerName != NULL);
	AfxSocketInit();
	m_sSMTPServerHostName = szSMTPServerName;
	m_nPort = nPort;
	m_bConnected = FALSE;
	m_bAuthed = FALSE;
	m_sError = _T("OK");
	response_buf = NULL;
}

CSmtp::~CSmtp()
{ Disconnect(); }

CString CSmtp::GetServerHostName()
{
	return m_sSMTPServerHostName;
}

BOOL CSmtp::Connect()
{
	CString sHello;
	TCHAR local_host[80];	// Warning: arbitrary size
	if(m_bConnected) return TRUE;

	try {
		// This will be deleted in Disconnect();
		response_buf = new TCHAR[RESPONSE_BUFFER_SIZE];
		// I can't count on all class users' applications
		// to have exception-throwing operator-new implementations,
		// so I'll soul-kiss the ones that don't.
		if(response_buf == NULL) {
			m_sError = _T("Not enough memory");
			return FALSE;
		}
	} catch(CException* e) {
		response_buf = NULL;
		m_sError = _T("Not enough memory");
		delete e;
		return FALSE;
	}

	if(!m_wsSMTPServer.Create()) {
		m_sError = _T("Unable to create the socket");
		delete response_buf;
		response_buf = NULL;
		return FALSE;
	}
	if( !m_wsSMTPServer.Connect(GetServerHostName(), GetPort())) {
		m_sError = _T("Unable to connect to server");
		m_wsSMTPServer.Close();
		delete response_buf;
		response_buf = NULL;
		return FALSE;
	}
	if(!get_response(CONNECT_SUCCESS)) {
		m_sError = _T( "Server didn't respond" );
		m_wsSMTPServer.Close();
		delete response_buf;
		response_buf = NULL;
		return FALSE;
	}
	gethostname(local_host, 80);
	sHello.Format(_T( "HELO %s\r\n" ), local_host);
	m_wsSMTPServer.Send((LPCTSTR)sHello, sHello.GetLength());
	if(!get_response(GENERIC_SUCCESS)) {
		m_wsSMTPServer.Close();
		delete response_buf;
		response_buf = NULL;
		return FALSE;
	}
	m_bConnected = TRUE;
	return TRUE;
}

BOOL CSmtp::Disconnect()
{
	BOOL ret;
	if(!m_bConnected) return TRUE;
	// Disconnect gracefully from the server and close the socket
	CString sQuit = _T("QUIT\r\n");
	m_wsSMTPServer.Send((LPCTSTR)sQuit, sQuit.GetLength());

	// No need to check return value here.
	// If it fails, the message is available with GetLastError
	ret = get_response(QUIT_SUCCESS);
	m_wsSMTPServer.Close();

	if(response_buf != NULL) {
		delete[] response_buf;
		response_buf = NULL;
	}
	m_bConnected = FALSE;
	return ret;
}

UINT CSmtp::GetPort()
{ return m_nPort; }

CString CSmtp::GetLastError()
{ return m_sError; }

BOOL CSmtp::SendMessage(CMailMessage* msg)
{
	ASSERT(msg != NULL);
	if(!m_bConnected) {
		m_sError = _T("Must be connected");
		return FALSE;
	}
	if(!m_bAuthed){
		m_sError = _T("Must be Authed");
		return FALSE;
	}
	if(FormatMailMessage(msg) == FALSE) {
		return FALSE;
	}
	if(transmit_message(msg) == FALSE) {
		return FALSE;
	}
	return TRUE;
}

BOOL CSmtp::FormatMailMessage(CMailMessage* msg)
{
	ASSERT(msg != NULL);
	if(msg -> GetNumRecipients() == 0) {
		m_sError = _T("No Recipients");
		return FALSE;
	}
	msg -> FormatMessage();
	return TRUE;
}

void CSmtp::SetServerProperties(LPCTSTR szSMTPServerName, UINT nPort)
{
	ASSERT(szSMTPServerName != NULL);
	// Needs to be safe in non-debug too
	if(szSMTPServerName == NULL) return;
	m_sSMTPServerHostName = szSMTPServerName;
	m_nPort = nPort;
}


CString CSmtp::cook_body(CMailMessage* msg)
{
	ASSERT(msg != NULL);
	CString sTemp;
	CString sCooked = _T("");
	LPTSTR szBad = _T("\r\n.\r\n");
	LPTSTR szGood = _T("\r\n..\r\n");
	int nPos;
	int nStart = 0;
	int nBadLength = strlen(szBad);
	sTemp = msg -> m_sBody;
	if(sTemp.Left(3) == _T(".\r\n"))
		sTemp = _T(".") + sTemp;
	//
	// This is a little inefficient because it beings a search
	// at the beginning of the string each time. This was
	// the only thing I could think of that handled ALL variations.
	// In particular, the sequence "\r\n.\r\n.\r\n" is troublesome. 
	// (Even CStringEx's FindReplace wouldn't handle that situation
	// with the global flag set.)
	//
	while((nPos = sTemp.Find(szBad)) > -1) {
		sCooked = sTemp.Mid(nStart, nPos);
		sCooked += szGood;
		sTemp = sCooked + sTemp.Right(sTemp.GetLength() - (nPos + nBadLength));
	}
	return sTemp;
}

BOOL CSmtp::transmit_message(CMailMessage* msg)
{
	CString sFrom;
	CString sTo;
	CString sTemp;
	CString sEmail;

	ASSERT(msg != NULL);
	if(!m_bConnected) {
		m_sError = _T("Must be connected");
		return FALSE;
	}

	if(!m_bAuthed){
		m_sError = _T("Must be Authed");
		return FALSE;
	}

	// Send the MAIL command
	sFrom.Format(_T( "MAIL From: <%s>\r\n" ), (LPCTSTR)msg->m_sFrom);
	m_wsSMTPServer.Send((LPCTSTR)sFrom, sFrom.GetLength());
	if(!get_response(GENERIC_SUCCESS)){
		m_sError = _T("Error sender address");
		return FALSE;
	}

	// Send RCPT commands (one for each recipient)
	for(int i = 0; i < msg->GetNumRecipients(); i++) {
		msg->GetRecipient(sEmail, sTemp, i);
		sTo.Format(_T("RCPT TO: <%s>\r\n"), (LPCTSTR)sEmail);
		m_wsSMTPServer.Send((LPCTSTR)sTo, sTo.GetLength());
		get_response(GENERIC_SUCCESS);
	}

	// Send the DATA command
	sTemp = _T("DATA\r\n");
	m_wsSMTPServer.Send((LPCTSTR)sTemp, sTemp.GetLength());
	if( !get_response(DATA_SUCCESS)) {
		return FALSE;
	}

	// Send the header
	m_wsSMTPServer.Send((LPCTSTR)msg -> m_sHeader, msg -> m_sHeader.GetLength());

	// Send the body
	sTemp = cook_body(msg);
	m_wsSMTPServer.Send((LPCTSTR)sTemp, sTemp.GetLength());

	// Signal end of data
	sTemp = _T("\r\n.\r\n");
	m_wsSMTPServer.Send((LPCTSTR)sTemp, sTemp.GetLength());
	if( !get_response(GENERIC_SUCCESS)) {
		return FALSE;
	}
	return TRUE;
}

BOOL CSmtp::get_response(UINT response_expected)
{
	ASSERT(response_expected >= GENERIC_SUCCESS);
	ASSERT(response_expected < LAST_RESPONSE);

	CString sResponse;
	UINT response;
	response_code* pResp;	// Shorthand

	if(m_wsSMTPServer.Receive(response_buf, RESPONSE_BUFFER_SIZE) == SOCKET_ERROR) {
		m_sError = _T("Socket Error");
		return FALSE;
	}
	sResponse = response_buf;
	sscanf((LPCTSTR)sResponse.Left(3), _T("%d"), &response);
	pResp = &response_table[response_expected];
	if(response != pResp -> nResponse) {
		m_sError.Format( _T("%d:%s"), response, (LPCTSTR)pResp -> sMessage);
		return FALSE;
	}
	return TRUE;
}

BOOL CSmtp::Auth()
{
	CString sAuth;
	if(!m_bConnected) return FALSE;

	sAuth.Format(_T( "auth login\r\n" ));	//construct auth quest
	m_wsSMTPServer.Send((LPCTSTR)sAuth, sAuth.GetLength());
	if(!get_response(AUTHQUE_SUCCESS)) {
		m_sError="SMTP server with no auth";
		m_bAuthed=TRUE;
		return TRUE;
	}

	sAuth.Empty();

	sAuth.Format(_T( "%s\r\n" ), m_User);	//m_User is an string encoded with CBASE64
	m_wsSMTPServer.Send((LPCTSTR)sAuth, sAuth.GetLength());
	if(!get_response(AUTHQUE_SUCCESS)) {
		m_wsSMTPServer.Close();
		delete response_buf;
		response_buf = NULL;
		return FALSE;
	}

	sAuth.Empty();

	sAuth.Format(_T( "%s\r\n" ), m_Pass);	//m_sSMTPPass is an string encoded with CBASE64
	m_wsSMTPServer.Send((LPCTSTR)sAuth, sAuth.GetLength());
	if(!get_response(AUTH_SUCCESS)) {
		m_wsSMTPServer.Close();
		delete response_buf;
		response_buf = NULL;
		return FALSE;
	}

	m_bAuthed = TRUE;
	return TRUE;

}