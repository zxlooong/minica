
/************************************
  REVISION LOG ENTRY
  Written and Revision By: Zhang, Zhefu
  Contact: codetiger@hotmail
  URL: http://www.chi.c.dendai.ac.jp/~zhang/index.htm
  Revised on 2001/09/27 in FujiSoftABC Ltd. Japan
  Comment: JXINIFile Header File
 ************************************/

#ifndef JX_INI_FILE
#define JX_INI_FILE

//
//namespace jxapi
//{

#ifdef _UNICODE
	#define JXGetPrivateProfileString    JXGetPrivateProfileStringW
	#define JXWritePrivateProfileString  JXWritePrivateProfileStringW
    #define JXRenameSectionEx            JXRenameSectionExW
    #define JXCopySectionExW             JXCopySectionExW
#else
    #define JXGetPrivateProfileString    JXGetPrivateProfileStringA
	#define JXWritePrivateProfileString  JXWritePrivateProfileStringA	
#endif /* !UNICODE */

typedef enum tagINIControlCode
{
	setSectionMapDirty = 0,  //Mark the Section Map out-of-date
    lockSectionMap = 1,      //Prevent Update Section Map
	unlockSectionMap = 2,    //Permit Update Section Map
	updateSectionMap = 3,    //Update Section Map
} INIControlCode;
#define INI_SET_DIRTY_MAP  0
#define INI_SET_LOCK_MAP   1
#define INI_SET_UNLOCK_MAP 2
#define INI_UPDATE_MAP     3
//?????---Warning: Using namespace for WinAPI have the same name
extern DWORD ControlINI(DWORD dwControl);
  

//UNICODE VERSION
extern BOOL JXRenameSectionExW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpNewName,        // new section name
  LPCWSTR lpFileName        // initialization file name
 );

extern BOOL JXCopySectionExW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpNewName,        // new section name
  LPCWSTR lpFileName        // initialization file name
 );

extern DWORD JXGetPrivateProfileStringW(
  LPCWSTR lpAppName,        // section name
  LPCWSTR lpKeyName,        // key name
  LPCWSTR lpDefault,        // default string
  LPWSTR lpReturnedString,  // destination buffer
  DWORD nSize,              // size of destination buffer
  LPCWSTR lpFileName        // initialization file name
 );

//Warning: Using namespace for WinAPI have the same name
extern BOOL JXWritePrivateProfileStringW(
  LPCWSTR lpAppName,  // section namen
  LPCWSTR lpKeyName,  // key name
  LPCWSTR lpString,   // string to add
  LPCWSTR lpFileName  // initialization file
 );


//ANSI VERSION
extern DWORD JXGetPrivateProfileStringA(
  LPCSTR lpAppName,        // section name
  LPCSTR lpKeyName,        // key name
  LPCSTR lpDefault,        // default string
  LPSTR lpReturnedString,  // destination buffer
  DWORD nSize,              // size of destination buffer
  LPCSTR lpFileName        // initialization file name
 );

//Warning: Using namespace for WinAPI have the same name
extern BOOL JXWritePrivateProfileStringA(
  LPCSTR lpAppName,  // section namen
  LPCSTR lpKeyName,  // key name
  LPCSTR lpString,   // string to add
  LPCSTR lpFileName  // initialization file
 );

//Flush Memory Data To Disk File and Close File Handle
extern BOOL JXUnloadPrivateProfile(); 
//}
//---end namespace
#define JXMegaByte 1024 * 1024  //1 Mb
#define JXPage     64 * 1024    //Win2000 ---- 1 Page = 64 Kb 
#define JXFlushIncrement  JXMegaByte  //1Mb

//Debug User
#define chSTR2(x)	   #x
#define chSTR(x)	chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)

#include <afxcoll.h> //To Use CMap
class CJXINIFile
{
public:
	//new Func
	//return FALSE if lpAppName noe exist
	BOOL RenameSectionExW(
       LPCWSTR lpAppName,        // section name
       LPCWSTR lpNewName,        // new section name
       LPCWSTR lpFileName);        // initialization file name
	//return FALSE if lpNewName already exist, lpAppName not Exist
    BOOL CopySectionExW(
       LPCWSTR lpAppName,        // section name
       LPCWSTR lpNewName,        // new section name
       LPCWSTR lpFileName);        // initialization file name
    ////in WCHARs, return 0 when err, Section Map Version
    UINT GetSectionSizeExW(
	   LPCWSTR lpAppName,        // section name
       LPCWSTR lpFileName);        // initialization file name
	////in WCHARs, return 0 when err,
	UINT GetSectionSizeW(
	   LPCWSTR lpAppName,        // section name
       LPCWSTR lpFileName);        // initialization file name

	//map mode extension
	CMapStringToPtr m_mapSection;  //Map Section To LPVOID 
	void UpdateMapA();
	void UpdateMapW();
	void ResizeMap(int nNewSize); //re-calulate the map class
	BOOL m_bSectionMapDirty; 
    BOOL m_bSectionMapLock;

	//--------------------------------------------------
	DWORD DeleteSectionExW(
		 LPCWSTR lpAppName        // section name
    );
	DWORD DeleteKeyExW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName  // key name
    );
	DWORD DeleteSectionW(
		 LPCWSTR lpAppName        // section name
    );
	DWORD DeleteKeyW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName  // key name
    );

	DWORD DeleteSectionExA(
		 LPCSTR lpAppName        // section name
    );
	DWORD DeleteKeyExA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName  // key name
    );
	DWORD DeleteSectionA(
		 LPCSTR lpAppName        // section name
    );
	DWORD DeleteKeyA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName  // key name
    );

	//Section Pointer Mapped Function 
	DWORD CJXINIFile::GetSectionsExA(
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize, // size of destination buffer, In WCHAR
         LPCSTR lpFileName        // initialization file name
    );
	DWORD GetSectionsExW(
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    );
	DWORD GetKeysExA(
		 LPCSTR lpAppName,        // section name
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    );
	DWORD GetKeysExW(
         LPCWSTR lpAppName,        // section name
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    );
	UINT GetKeyExA(LPSTR pHead, UINT uSize, 
						LPCSTR szSection, LPCSTR szKey, UINT& posSection);
    UINT GetKeyExW(LPWSTR pHead, UINT uSize, 
		LPCWSTR szSection, LPCWSTR szKey, UINT& posSection);
	DWORD GetPrivateProfileStringExA(
         LPCSTR lpAppName,        // section name
         LPCSTR lpKeyName,        // key name
         LPCSTR lpDefault,        // default string
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    );
	DWORD GetPrivateProfileStringExW(
         LPCWSTR lpAppName,        // section name
         LPCWSTR lpKeyName,        // key name
         LPCWSTR lpDefault,        // default string
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    );
	BOOL WritePrivateProfileStringExA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName,  // key name
         LPCSTR lpString,   // string to add
         LPCSTR lpFileName  // initialization file
    );
	BOOL WritePrivateProfileStringExW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName,  // key name
         LPCWSTR lpString,   // string to add
         LPCWSTR lpFileName  // initialization file
    );
	


	//If Programm Crashed Last Time, Cut the Zero Part Left on the Disk
	void CutZeroTail();
	BOOL UnloadIniFile();
	//0 -- Default,  1 -- ANSI, 2 -- UNICODE
	BOOL m_bUnicode;
	BOOL LoadIniFile(CString strFilename, DWORD dwMode = 0);
	CJXINIFile();
	~CJXINIFile(); 

	//UNICODE VERSION
	
	UINT GetKeyW(LPWSTR pHead, UINT uSize, 
		LPCWSTR szSection, LPCWSTR szKey, UINT& posSection);
	DWORD GetPrivateProfileStringW(
         LPCWSTR lpAppName,        // section name
         LPCWSTR lpKeyName,        // key name
         LPCWSTR lpDefault,        // default string
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    );
	
	BOOL WritePrivateProfileStringW(
         LPCWSTR lpAppName,  // section name
         LPCWSTR lpKeyName,  // key name
         LPCWSTR lpString,   // string to add
         LPCWSTR lpFileName  // initialization file
    );

	DWORD GetSectionsW(
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    );

	DWORD GetKeysW(
         LPCWSTR lpAppName,        // section name
         LPWSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCWSTR lpFileName        // initialization file name
    );

	//ANSI VERSION
	UINT GetKeyA(LPSTR pHead, UINT uSize, 
		LPCSTR szSection, LPCSTR szKey, UINT& posSection);
	DWORD GetPrivateProfileStringA(
         LPCSTR lpAppName,        // section name
         LPCSTR lpKeyName,        // key name
         LPCSTR lpDefault,        // default string
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    );
	
	BOOL WritePrivateProfileStringA(
         LPCSTR lpAppName,  // section name
         LPCSTR lpKeyName,  // key name
         LPCSTR lpString,   // string to add
         LPCSTR lpFileName  // initialization file
    );

	DWORD GetSectionsA(
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    );

	DWORD GetKeysA(
         LPCSTR lpAppName,        // section name
         LPSTR lpReturnedString,  // destination buffer
         DWORD nSize,              // size of destination buffer
         LPCSTR lpFileName        // initialization file name
    );

private:
	CString m_strFilename;
	HANDLE  m_hFile;  //File handle
	HANDLE  m_hMMF;   //Memory-Map File Handle
	DWORD   m_dwSize; //Current Open File Size in BYTEs
	DWORD   m_dwUseSize; //Curent Using Size in BYTEs,
	                     //not including heading 2 byte!!!!
	//m_dwSize <= m_dwUseSize <= m_dwSize + JXFlushIncrement
	LPVOID  m_pData;  //MMF pointer
	LPVOID  m_pTrueData;  //MMF pointer
	BOOL    m_bDirty;  //Modified? 
};


#endif
