// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A80D7EA4_BB4A_4622_A06C_5B59E8EC712D__INCLUDED_)
#define AFX_STDAFX_H__A80D7EA4_BB4A_4622_A06C_5B59E8EC712D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include "Sslstruct.h"
#include ".\CA\ca.h"
#include ".\Control\ColorListBox\ColorListBox.h"
#include ".\Control\XpButtonST\XPStyleButtonST.h"
#include ".\Control\XpButtonST\BCMenu.h"
#include ".\Control\ClassXP\ClassXP.h"
//#include <afxsock.h>		// MFC socket extensions

///////////////////////////
#define VC_EXTRALEAN

#include <afxext.h> 
#include <winsock2.h>
#include <afxmt.h>
#include <afxtempl.h>
#include <afxpriv.h>
#ifndef W3MFC_NO_ISAPI_SUPPORT
  #include <HttpExt.h>
#endif
#ifndef W3MFC_NO_SSPI_SUPPORT
  #include <sspi.h>
#endif
///////////////////////////

/*/////////////////////////////// leak ÄÚ´æÐ¹Â©¼ì²â 2 - 1////////////////////
#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h>
#include<crtdbg.h>
//////////////////////////////////////////////////*/

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A80D7EA4_BB4A_4622_A06C_5B59E8EC712D__INCLUDED_)
