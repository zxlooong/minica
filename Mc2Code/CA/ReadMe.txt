========================================================================
       DYNAMIC LINK LIBRARY : CA
========================================================================


AppWizard has created this CA DLL for you.  

This file contains a summary of what you will find in each of the files that
make up your CA application.

CA.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

CA.cpp
    This is the main DLL source file.

	When created, this DLL does not export any symbols. As a result, it 
	will not produce a .lib file when it is built. If you wish this project
	to be a project dependency of some other project, you will either need to 
	add code to export some symbols from the DLL so that an export library 
	will be produced, or you can check the "doesn't produce lib" checkbox in 
	the Linker settings page for this project. 

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named CA.pch and a precompiled types file named StdAfx.obj.


/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.


/////////////////////////////////////////////////////////////////////////////
////////////一些概念////////////////
证书签名请求Certificate Signing Request (CSR)


如何创建证书:
首先，生成密钥对（公钥和私钥）;
接下来，创建一个所谓的证书签名请求Certificate Signing Request (CSR)。
CSR只不过是一个数据集，其中包含了证书中所包括的全部信息（包括公开密钥）
并且使用所生成的私钥对其进行签名。
根据CSR创建证书并用私钥签署证书。
REQ请求包含用户信息（公钥）和私钥，这个过程不需要根证书