# Microsoft Developer Studio Generated NMAKE File, Based on CA.dsp
!IF "$(CFG)" == ""
CFG=CA - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CA - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CA - Win32 Release" && "$(CFG)" != "CA - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CA.mak" CFG="CA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CA - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CA - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "CA - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\CA.dll"


CLEAN :
	-@erase "$(INTDIR)\CA.obj"
	-@erase "$(INTDIR)\CA.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\CA.dll"
	-@erase "$(OUTDIR)\CA.exp"
	-@erase "$(OUTDIR)\CA.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CA_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\CA.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CA.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\CA.pdb" /machine:I386 /def:".\CA.def" /out:"$(OUTDIR)\CA.dll" /implib:"$(OUTDIR)\CA.lib" 
DEF_FILE= \
	".\CA.def"
LINK32_OBJS= \
	"$(INTDIR)\CA.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\openssl\libeay32.lib"

"$(OUTDIR)\CA.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CA - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\CA.dll" "$(OUTDIR)\CA.bsc"


CLEAN :
	-@erase "$(INTDIR)\CA.obj"
	-@erase "$(INTDIR)\CA.pch"
	-@erase "$(INTDIR)\CA.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CA.bsc"
	-@erase "$(OUTDIR)\CA.dll"
	-@erase "$(OUTDIR)\CA.exp"
	-@erase "$(OUTDIR)\CA.ilk"
	-@erase "$(OUTDIR)\CA.lib"
	-@erase "$(OUTDIR)\CA.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /Gm /GX /ZI /Od /I "F:\资料\free_ssl\Win平台openssl-0.9.6h头文件\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CA_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CA.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CA.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CA.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\CA.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\CA.pdb" /debug /machine:I386 /def:".\CA.def" /out:"$(OUTDIR)\CA.dll" /implib:"$(OUTDIR)\CA.lib" /pdbtype:sept 
DEF_FILE= \
	".\CA.def"
LINK32_OBJS= \
	"$(INTDIR)\CA.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\openssl\libeay32.lib"

"$(OUTDIR)\CA.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("CA.dep")
!INCLUDE "CA.dep"
!ELSE 
!MESSAGE Warning: cannot find "CA.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CA - Win32 Release" || "$(CFG)" == "CA - Win32 Debug"
SOURCE=.\CA.cpp

!IF  "$(CFG)" == "CA - Win32 Release"


"$(INTDIR)\CA.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CA.pch"


!ELSEIF  "$(CFG)" == "CA - Win32 Debug"


"$(INTDIR)\CA.obj"	"$(INTDIR)\CA.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CA.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "CA - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CA_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\CA.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\CA.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CA - Win32 Debug"

CPP_SWITCHES=/nologo /MD /W3 /Gm /GX /ZI /Od /I "F:\资料\free_ssl\Win平台openssl-0.9.6h头文件\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CA_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CA.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\CA.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

