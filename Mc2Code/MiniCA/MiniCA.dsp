# Microsoft Developer Studio Project File - Name="MiniCA" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MiniCA - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MiniCA.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MiniCA.mak" CFG="MiniCA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MiniCA - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MiniCA - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MiniCA - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /I "..\openssl" /I "E:\SDK\INCLUDE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "SECURITY_WIN32" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\openssl\libeay32.lib ..\openssl\ssleay32.lib .\CA\release\CA.lib crypt32.lib winmm.lib shlwapi.lib secur32.Lib /nologo /subsystem:windows /machine:I386 /libpath:"E:\SDK\lib"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\openssl" /I ".\p11header" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "SECURITY_WIN32" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\openssl\libeay32.lib ..\openssl\ssleay32.lib .\CA\release\CA.lib crypt32.lib winmm.lib shlwapi.lib secur32.Lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"E:\SDK\lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "MiniCA - Win32 Release"
# Name "MiniCA - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Control\AboutCtrl\AboutCtrl.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\Ado\ado2.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Asn1ParsePage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\Base64.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\BCMenu.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\BitmapSlider\BitmapSlider.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\Tree\BmpTree.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\BtnST.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCenterPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCertExtPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCertInfoPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCertIniSetPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCertManPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCertReportPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCertTypePage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CaCertWizardSheet.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CertDbPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CertExtPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CertInfoPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenericClass\cgfiltyp.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\ClassXP.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ColorListBox\ColorListBox.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\Control\CreditsCtrl-About\CreditsCtrl.cpp"
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPBase.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPButton.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPColor.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPComboBox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPEditBox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPMessage.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPScrollBar.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPSpinBox.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPUtility.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\DirectedThreadPoolQueue.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\GenericClass\DynRes.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\CheckList\EditCell.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Evp.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ExtRichEdit.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\StatusBar\ExtStatusControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\FileEditCtr\FileEditCtrl.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\XColorStatic\FontSize.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenericClass\GetHardSoftInfo.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpCGI.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpClient.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpDirectory.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpISAPI.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpISAPIManager.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpMimeManager.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpRequest.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpResponseHeader.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpSocket.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\IOCPThreadPoolQueue.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenericClass\JXINIFile.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenericClass\Language.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\CheckList\ListCtrl.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MiniCA.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MiniCA.rc
# End Source File
# Begin Source File

SOURCE=.\MiniMainDlg.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenericClass\OleTarget.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\OpenSSLMfc.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\tooltip\OXToolTipCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\Splash\Picture.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\FileEditCtr\PJAImage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\PageLabel\PrefsStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\ProgressCtrlST\ProgressCtrlST.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SafeDataCdbPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SafeDataSetPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SafeInfoPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\Tree\SetupTree.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Shortcut.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\HtmlCtrl\SimpleBrowser.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\Smtp\Smtp.cpp
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\SocMFC.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\Splash\SplashWnd.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\StaticTree\StaticTreeCtrl.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Control\SuperList\supergridctrl.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\TabSheet\TabSheet.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\TaskbarNotifier\TaskbarNotifier.cpp
# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\ThemeHelperST.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\ThrdPool.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UpdatePage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\W3Mfc.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WebCertManPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WebServerPage.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WebServerSheet.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\Win32Handle.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WinEvpSheet.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\W3Mfc\WTimer.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\XColorStatic\XColorStatic.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\XPStyleButtonST.cpp

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Control\AboutCtrl\AboutCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Control\Ado\ado2.h
# End Source File
# Begin Source File

SOURCE=.\Asn1ParsePage.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\Base64.h
# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\BCMenu.h
# End Source File
# Begin Source File

SOURCE=.\Control\BitmapSlider\BitmapSlider.h
# End Source File
# Begin Source File

SOURCE=.\Control\Tree\BmpTree.h
# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\CaCenterPage.h
# End Source File
# Begin Source File

SOURCE=.\CaCertExtPage.h
# End Source File
# Begin Source File

SOURCE=.\CaCertInfoPage.h
# End Source File
# Begin Source File

SOURCE=.\CaCertIniSetPage.h
# End Source File
# Begin Source File

SOURCE=.\CaCertManPage.h
# End Source File
# Begin Source File

SOURCE=.\CaCertReportPage.h
# End Source File
# Begin Source File

SOURCE=.\CaCertTypePage.h
# End Source File
# Begin Source File

SOURCE=.\CaCertWizardSheet.h
# End Source File
# Begin Source File

SOURCE=.\CertDB.h
# End Source File
# Begin Source File

SOURCE=.\CertDbPage.h
# End Source File
# Begin Source File

SOURCE=.\CertExtPage.h
# End Source File
# Begin Source File

SOURCE=.\CertInfoPage.h
# End Source File
# Begin Source File

SOURCE=.\CertKey.h
# End Source File
# Begin Source File

SOURCE=.\GenericClass\cgfiltyp.h
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\ClassXP.h
# End Source File
# Begin Source File

SOURCE=.\Control\ColorListBox\ColorListBox.h
# End Source File
# Begin Source File

SOURCE=".\Control\CreditsCtrl-About\CreditsCtrl.h"
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPBase.h
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPColor.h
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPDefine.h
# End Source File
# Begin Source File

SOURCE=.\Control\ClassXP\CXPMacro.h
# End Source File
# Begin Source File

SOURCE=.\DataSafe.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\DirectedThreadPoolQueue.h
# End Source File
# Begin Source File

SOURCE=.\GenericClass\DynRes.h
# End Source File
# Begin Source File

SOURCE=.\Control\CheckList\EditCell.h
# End Source File
# Begin Source File

SOURCE=.\Evp.h
# End Source File
# Begin Source File

SOURCE=.\ExtRichEdit.h
# End Source File
# Begin Source File

SOURCE=.\Control\StatusBar\Extstatuscontrolbar.h
# End Source File
# Begin Source File

SOURCE=.\Control\FileEditCtr\FileEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Control\XColorStatic\FontSize.h
# End Source File
# Begin Source File

SOURCE=.\GenericClass\GetHardSoftInfo.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpCGI.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpClient.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpDirectory.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpISAPI.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpISAPIManager.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpMimeManager.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpRequest.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpResponseHeader.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\HttpSocket.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\IOCPThreadPoolQueue.h
# End Source File
# Begin Source File

SOURCE=.\GenericClass\JXINIFile.h

!IF  "$(CFG)" == "MiniCA - Win32 Release"

!ELSEIF  "$(CFG)" == "MiniCA - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenericClass\Language.h
# End Source File
# Begin Source File

SOURCE=.\Control\CheckList\ListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MiniCA.h
# End Source File
# Begin Source File

SOURCE=.\MiniCT.h
# End Source File
# Begin Source File

SOURCE=.\MiniMainDlg.h
# End Source File
# Begin Source File

SOURCE=.\GenericClass\OleTarget.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\OpenSSLMfc.h
# End Source File
# Begin Source File

SOURCE=.\Control\tooltip\OXDllExt.h
# End Source File
# Begin Source File

SOURCE=.\Control\tooltip\OXToolTipCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Control\Splash\Picture.h
# End Source File
# Begin Source File

SOURCE=.\Control\FileEditCtr\PJAImage.h
# End Source File
# Begin Source File

SOURCE=.\Control\PageLabel\PrefsStatic.h
# End Source File
# Begin Source File

SOURCE=.\Control\ProgressCtrlST\ProgressCtrlST.h
# End Source File
# Begin Source File

SOURCE=.\ProgressCtrlST\ProgressCtrlST.h
# End Source File
# Begin Source File

SOURCE=.\RC.h
# End Source File
# Begin Source File

SOURCE=.\RegPage.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SafeDataCdbPage.h
# End Source File
# Begin Source File

SOURCE=.\SafeDataSetPage.h
# End Source File
# Begin Source File

SOURCE=.\SafeInfoPage.h
# End Source File
# Begin Source File

SOURCE=.\Control\Tree\SetupTree.h
# End Source File
# Begin Source File

SOURCE=.\Shortcut.h
# End Source File
# Begin Source File

SOURCE=.\Control\HtmlCtrl\SimpleBrowser.h
# End Source File
# Begin Source File

SOURCE=.\Control\Smtp\Smtp.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\SocMFC.h
# End Source File
# Begin Source File

SOURCE=.\Control\Splash\SplashWnd.h
# End Source File
# Begin Source File

SOURCE=.\SslStruct.h
# End Source File
# Begin Source File

SOURCE=.\Control\StaticTree\StaticTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Control\SuperList\supergridctrl.h
# End Source File
# Begin Source File

SOURCE=.\Control\TabSheet\TabSheet.h
# End Source File
# Begin Source File

SOURCE=.\Control\TaskbarNotifier\TaskbarNotifier.h
# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\ThemeHelperST.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\ThrdPool.h
# End Source File
# Begin Source File

SOURCE=.\UpdatePage.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\W3Mfc.h
# End Source File
# Begin Source File

SOURCE=.\WebCertManPage.h
# End Source File
# Begin Source File

SOURCE=.\WebServerPage.h
# End Source File
# Begin Source File

SOURCE=.\WebServerSheet.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\Win32Handle.h
# End Source File
# Begin Source File

SOURCE=.\WinEvpSheet.h
# End Source File
# Begin Source File

SOURCE=.\W3Mfc\WTimer.h
# End Source File
# Begin Source File

SOURCE=.\Control\XColorStatic\XColorStatic.h
# End Source File
# Begin Source File

SOURCE=.\Control\XpButtonST\XPStyleButtonST.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\10.ico
# End Source File
# Begin Source File

SOURCE=.\res\11.ico
# End Source File
# Begin Source File

SOURCE=.\res\12.ico
# End Source File
# Begin Source File

SOURCE=.\res\13.ico
# End Source File
# Begin Source File

SOURCE=.\res\14.ico
# End Source File
# Begin Source File

SOURCE=.\res\15.ico
# End Source File
# Begin Source File

SOURCE=.\res\4.ico
# End Source File
# Begin Source File

SOURCE=.\res\5.ico
# End Source File
# Begin Source File

SOURCE=.\res\6.ico
# End Source File
# Begin Source File

SOURCE=.\res\7.ico
# End Source File
# Begin Source File

SOURCE=.\res\8.ico
# End Source File
# Begin Source File

SOURCE=.\res\81.ICO
# End Source File
# Begin Source File

SOURCE=.\res\9.ico
# End Source File
# Begin Source File

SOURCE=.\res\all.ico
# End Source File
# Begin Source File

SOURCE=.\res\asn1.ico
# End Source File
# Begin Source File

SOURCE=.\res\background.bmp
# End Source File
# Begin Source File

SOURCE=.\res\begin.ico
# End Source File
# Begin Source File

SOURCE=.\res\Blue.ico
# End Source File
# Begin Source File

SOURCE=.\res\ca.ICO
# End Source File
# Begin Source File

SOURCE=.\res\cert.ico
# End Source File
# Begin Source File

SOURCE=.\res\certext.ico
# End Source File
# Begin Source File

SOURCE=.\res\certinfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\certman.ico
# End Source File
# Begin Source File

SOURCE=.\res\certreport.ico
# End Source File
# Begin Source File

SOURCE=.\res\certsetup.ico
# End Source File
# Begin Source File

SOURCE=.\res\certtype.ico
# End Source File
# Begin Source File

SOURCE=.\res\CheckBox.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CIBAS.BMP
# End Source File
# Begin Source File

SOURCE=.\res\client.ico
# End Source File
# Begin Source File

SOURCE=.\res\clock.ico
# End Source File
# Begin Source File

SOURCE=.\csp11\CPEleven.h
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\data.ico
# End Source File
# Begin Source File

SOURCE=.\res\dataDEC.ico
# End Source File
# Begin Source File

SOURCE=.\res\dataENC.ico
# End Source File
# Begin Source File

SOURCE=.\res\Dec.ico
# End Source File
# Begin Source File

SOURCE=.\res\dir.ico
# End Source File
# Begin Source File

SOURCE=.\res\edit.ico
# End Source File
# Begin Source File

SOURCE=.\res\Enc.ico
# End Source File
# Begin Source File

SOURCE=.\res\error.ico
# End Source File
# Begin Source File

SOURCE=.\res\EVP.ico
# End Source File
# Begin Source File

SOURCE=.\res\evp1.ico
# End Source File
# Begin Source File

SOURCE=.\res\folders.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Green.ico
# End Source File
# Begin Source File

SOURCE=.\res\hpxs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon14.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon5.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_but.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_cer.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_dat.ico
# End Source File
# Begin Source File

SOURCE=.\res\info.ico
# End Source File
# Begin Source File

SOURCE=.\res\key.ico
# End Source File
# Begin Source File

SOURCE=.\res\key2.ico
# End Source File
# Begin Source File

SOURCE=.\res\last.ico
# End Source File
# Begin Source File

SOURCE=.\res\List.bmp
# End Source File
# Begin Source File

SOURCE=.\res\list.ico
# End Source File
# Begin Source File

SOURCE=.\res\LOCK.ico
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\makecrl.ico
# End Source File
# Begin Source File

SOURCE=.\res\man.ico
# End Source File
# Begin Source File

SOURCE=.\res\MiniCA.ico
# End Source File
# Begin Source File

SOURCE=.\res\MiniCA.rc2
# End Source File
# Begin Source File

SOURCE=.\res\mod_1.ico
# End Source File
# Begin Source File

SOURCE=.\res\mod_2.ico
# End Source File
# Begin Source File

SOURCE=.\res\mod_3.ico
# End Source File
# Begin Source File

SOURCE=.\res\my.ico
# End Source File
# Begin Source File

SOURCE=.\res\Net.ico
# End Source File
# Begin Source File

SOURCE=.\res\next.ico
# End Source File
# Begin Source File

SOURCE=.\res\NO.ico
# End Source File
# Begin Source File

SOURCE=.\res\none.ico
# End Source File
# Begin Source File

SOURCE=.\res\ocspc.ico
# End Source File
# Begin Source File

SOURCE=.\res\ocsps.ico
# End Source File
# Begin Source File

SOURCE=.\res\Ok.ico
# End Source File
# Begin Source File

SOURCE=.\res\Oth017.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Pfx.ico
# End Source File
# Begin Source File

SOURCE=.\res\prog.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Ra.ico
# End Source File
# Begin Source File

SOURCE=.\res\Radio.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Red.ico
# End Source File
# Begin Source File

SOURCE=.\res\Reg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Reg.ico
# End Source File
# Begin Source File

SOURCE=.\res\resert.ico
# End Source File
# Begin Source File

SOURCE=.\res\revoke.ico
# End Source File
# Begin Source File

SOURCE=.\res\SAVE.ico
# End Source File
# Begin Source File

SOURCE=.\res\select.ico
# End Source File
# Begin Source File

SOURCE=.\res\selectcert.ico
# End Source File
# Begin Source File

SOURCE=.\res\selectfile.ico
# End Source File
# Begin Source File

SOURCE=.\res\server.ICO
# End Source File
# Begin Source File

SOURCE=.\res\SETUP.ico
# End Source File
# Begin Source File

SOURCE=.\res\skinmsn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\slider.bmp
# End Source File
# Begin Source File

SOURCE=.\res\some.ico
# End Source File
# Begin Source File

SOURCE=.\res\Splsh24.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Splsh24.jpg
# End Source File
# Begin Source File

SOURCE=.\res\SplshCA.jpg
# End Source File
# Begin Source File

SOURCE=.\res\SplshEvp.jpg
# End Source File
# Begin Source File

SOURCE=.\res\Thumb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Thumb_Active.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\UP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\update.ICO
# End Source File
# Begin Source File

SOURCE=.\res\VCERT.ico
# End Source File
# Begin Source File

SOURCE=.\res\VERIFY.ico
# End Source File
# Begin Source File

SOURCE=.\res\viewcert.ico
# End Source File
# Begin Source File

SOURCE=.\res\Waring.ico
# End Source File
# Begin Source File

SOURCE=.\res\wizard.ico
# End Source File
# Begin Source File

SOURCE=.\res\Xptemp.ico
# End Source File
# Begin Source File

SOURCE=.\res\Yello.ico
# End Source File
# Begin Source File

SOURCE=.\res\yes.ico
# End Source File
# End Group
# Begin Group "÷§ È"

# PROP Default_Filter "cer,der,pme,ini"
# Begin Source File

SOURCE=.\res\CertDB.CDB
# End Source File
# Begin Source File

SOURCE=.\res\ClientCert.pem
# End Source File
# Begin Source File

SOURCE=..\Raclient\res\ClientCert.pem
# End Source File
# Begin Source File

SOURCE=.\res\ClientKey.pem
# End Source File
# Begin Source File

SOURCE=.\res\FileInfo.Cer
# End Source File
# Begin Source File

SOURCE=.\res\index.htm
# End Source File
# Begin Source File

SOURCE=.\res\MiniCA.ini
# End Source File
# Begin Source File

SOURCE=.\res\MiniCABig.ini
# End Source File
# Begin Source File

SOURCE=.\res\MiniExt.ini
# End Source File
# Begin Source File

SOURCE=.\res\MiniExtBig.ini
# End Source File
# Begin Source File

SOURCE=.\res\RA.mdb
# End Source File
# Begin Source File

SOURCE=.\res\RegCert.pem
# End Source File
# Begin Source File

SOURCE=.\res\RegECert.pem
# End Source File
# Begin Source File

SOURCE=.\res\RegEKey.pem
# End Source File
# Begin Source File

SOURCE=.\res\RegICert.pem
# End Source File
# Begin Source File

SOURCE=.\res\RegIKey.pem
# End Source File
# Begin Source File

SOURCE=.\res\RegKey.pem
# End Source File
# Begin Source File

SOURCE=.\res\RootCert.DER
# End Source File
# Begin Source File

SOURCE=.\res\RootCert.pem
# End Source File
# Begin Source File

SOURCE=.\res\RootKey.pem
# End Source File
# Begin Source File

SOURCE=.\res\ServerCert.PEM
# End Source File
# Begin Source File

SOURCE=.\res\ServerKey.PEM
# End Source File
# Begin Source File

SOURCE=.\res\SignCert.Cer
# End Source File
# Begin Source File

SOURCE=.\res\SignCert.pem
# End Source File
# Begin Source File

SOURCE=.\res\SignCert.pme
# End Source File
# Begin Source File

SOURCE=.\res\SignKey.Cer
# End Source File
# Begin Source File

SOURCE=.\res\SignKey.pem
# End Source File
# Begin Source File

SOURCE=.\Debug\WebServer.ini
# End Source File
# Begin Source File

SOURCE=.\res\WebServer.ini
# End Source File
# Begin Source File

SOURCE=.\lib\htmlhelp.lib
# End Source File
# End Group
# Begin Group "csp11"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\csp11\CBase64.cpp
# End Source File
# Begin Source File

SOURCE=.\csp11\CBase64.h
# End Source File
# Begin Source File

SOURCE=.\csp11\Certificate.cpp
# End Source File
# Begin Source File

SOURCE=.\csp11\Certificate.h
# End Source File
# Begin Source File

SOURCE=.\csp11\CertKitCsp.cpp
# End Source File
# Begin Source File

SOURCE=.\csp11\CertKitCsp.h
# End Source File
# Begin Source File

SOURCE=.\csp11\CPEleven.cpp
# End Source File
# Begin Source File

SOURCE=.\csp11\MemTools.cpp
# End Source File
# Begin Source File

SOURCE=.\csp11\MemTools.h
# End Source File
# Begin Source File

SOURCE=.\csp11\MemToolsX.cpp
# End Source File
# Begin Source File

SOURCE=.\csp11\MemToolsX.h
# End Source File
# Begin Source File

SOURCE=.\Ossl.cpp
# End Source File
# Begin Source File

SOURCE=.\Ossl.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\302.htm
# End Source File
# Begin Source File

SOURCE=.\res\400.htm
# End Source File
# Begin Source File

SOURCE=.\res\401.htm
# End Source File
# Begin Source File

SOURCE=.\res\404.htm
# End Source File
# Begin Source File

SOURCE=.\res\500.htm
# End Source File
# Begin Source File

SOURCE=.\res\501.htm
# End Source File
# Begin Source File

SOURCE=.\res\deleted_ok.htm
# End Source File
# Begin Source File

SOURCE=.\res\EncCert.pem
# End Source File
# Begin Source File

SOURCE=.\res\EncKey.pem
# End Source File
# Begin Source File

SOURCE=.\res\FileInfo.Key
# End Source File
# Begin Source File

SOURCE=.\res\gospel
# End Source File
# Begin Source File

SOURCE=.\res\MiniCA.manifest
# End Source File
# Begin Source File

SOURCE=.\res\MiniCA.mdb
# End Source File
# Begin Source File

SOURCE=".\OPENSSL±‡“Î.TXT"
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\VerifyCert.pem
# End Source File
# Begin Source File

SOURCE=.\res\VerifyKey.pem
# End Source File
# End Target
# End Project
