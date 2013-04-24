========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : MiniCA--2003-8-23
========================================================================


AppWizard has created this MiniCA application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your MiniCA application.

MiniCA.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

MiniCA.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CMiniCAApp application class.

MiniCA.cpp
    This is the main application source file that contains the application
    class CMiniCAApp.

MiniCA.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

MiniCA.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

res\MiniCA.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file MiniCA.rc.

res\MiniCA.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.




/////////////////////////////////////////////////////////////////////////////

AppWizard creates one dialog class:

MiniCADlg.h, MiniCADlg.cpp - the dialog
    These files contain your CMiniCADlg class.  This class defines
    the behavior of your application's main dialog.  The dialog's
    template is in MiniCA.rc, which can be edited in Microsoft
	Visual C++.


/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named MiniCA.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is 
in a language other than the operating system's current language, you
will need to copy the corresponding localized resources MFC42XXX.DLL
from the Microsoft Visual C++ CD-ROM onto the system or system32 directory,
and rename it to be MFCLOC.DLL.  ("XXX" stands for the language abbreviation.
For example, MFC42DEU.DLL contains resources translated to German.)  If you
don't do this, some of the UI elements of your application will remain in the
language of the operating system.

/////////////////////////////////////////////////////////////////////////////
���ص�����ʹ�����Աʱ��(�ؼ�),����û��thisָ��,���Ǻ����ͨ����̬��Ա������ȡ,��������afxgetmain()
���߳���ʹ�����Աʱ��(�ؼ�),����Ҫ����һ������,���߳���õ��ؼ���ָ��,��afxget�õ����Ǵ����
���������evp

�Ի����в���ؼ���ǰ������
	AfxEnableControlContainer();
 	CoInitialize(NULL);

OPENSSL Ҫ��ʹ��
	Multithreaded

���ĳ���಻�����򵼼���ϵͳ��Ϣ��������Ҫ�ؽ�clw�ļ�


13. I think I've detected a memory leak, is this a bug?
In most cases the cause of an apparent memory leak is an OpenSSL internal table that is allocated when an application starts up. Since such tables do not grow in size over time they are harmless. 

These internal tables can be freed up when an application closes using various functions. Currently these include following: 

Thread-local cleanup functions: 


  ERR_remove_state()

Application-global cleanup functions that are aware of usage (and therefore thread-safe): 

  ENGINE_cleanup() and CONF_modules_unload()

"Brutal" (thread-unsafe) Application-global cleanup functions: 

  ERR_free_strings(), EVP_cleanup() and CRYPTO_cleanup_all_ex_data().

ǰ���죬Ϊ�˸�����ṩ�°汾��openssl������������һ��openssl�ı��빤�������Ƚ��˼��ֱ��뷽����д������¼򵥽���һ�£�ϣ���Դ���а�����
     �ұ���İ汾��0.9.6h��������ʹ��VC++6.0,��������˵�����ҽ��������µ�����Ͳ�����
     1.��װActivePerl-5.6.1.629-MSWin32-x86-multi-thread.˳��ͨ����
     2.�򿪿���̨������openssl��ѹ���Ŀ¼��ִ��Perl Configure VC-WIN32���ע�⣬һ��Ҫ�����Ŀ¼��ִ�и���������Ҳ���Configure�ļ�����Ȼ����Ҳ����ָ��������Configure�ļ�·����˳��ͨ����
     3.����openssl�İ���˵�����ڽ�ѹĿ¼��ִ��ms\do_masm(��ms_\do_ms)����ɹ�ͨ����
     4.����VC����������������ͬ�Ŀ���̨������ת��VC����Ŀ¼�µ�vc98\binĿ¼��ִ��vcvars32�������������VC�Ļ���������Ȼ����ת��openssl��ѹ���ĸ�Ŀ¼�¡�
     ע�⣺�ҵ�һ������Vcvars32��ʱ�򣬱�������Ϊ��Out of environment space���������Ĳ�������Ϊ���MS��DOS��������û�з����㹻���ڴ棬�����ͨ�����·��������
     a.�����ʼ, �������. 
     b.����sysedit��س����иó���. 
     c.Ȼ�����ֶ�����ڣ�ѡ��Config.sys����. 
     d.�������Shell= �����, ����ǰ������ REM ����ע�͸����. 
     e.�������䲻���ڣ���ֱ������SHELL=C:\COMMAND.COM /E:4096 /P ��ע�⣬Ҫ��֤������Ǵӵ�һ�е�һ��λ�ÿ�ʼ����ġ�
     f.���桢�˳������������Ծ�Ӧ�ÿ��Խ��������⡣
     ��������������������֮�������������Ժ����ֱ��ִ�е�4����
     5.��openssl��ѹĿ¼��ִ��nmake -f ms\ntdll.mak�����û�д��󣬾���ɱ����ˡ�������ļ���out32dll���棬����Ӧ�ó���Ŀ�ִ���ļ�������Lib�ļ�������dll�ļ��� 
     
     �Ҹ���openssl��˵�����������ֲ�ͨ�ı��룬һֱ�Ǽ���ASM������Ա�������һֱ��û�У����ڵ�������ʱ��ִ��do_masm��do_ms���ʱ����ǰ�ߴ��Ϊ3���ӣ��ҵĵ���������ͼ��������1.2���ڴ�256SDRM��,���ߴ����6���ӵ����ӡ�
     Ȼ����ʹ���˵������ṩ��һ�����뷽�����÷����Զ�������openssl����ģ���V����C��������������Ŀ��Ȼ���ٽ��б��룬�ٶȺ�������������20�������ϵ�ʱ�䣬���Ǻô��ǿ��Խ�һ��һ����ĿŪ�ıȽ���������������ҾͲ�˵�ˣ���ҿ��Բο���ص��ĵ���


certmgr.dll - ֤��ͼ��
shell32.dll
comres.dll - COMͼ��

EvpClean()����˵��,�˺����ڳ�������ʱ�����,������һ�������е��ù�,��ʹ��һ�����������¼������㷨,Ҳ���ܳ���û�м��ܿ�Ĵ���.

��������ת��:

1. static_cast 
��һ��ֵ�Ժ��߼��ķ�ʽת��.��ɿ�����"����ԭֵ�ؽ�һ����ʱ����,������
����ֵʱʹ���ͱ�ת��".Ψ�е��������ͱ�ת����������,����ת���Ż�ɹ�.
��ν��"��������",�����������ڽ�����,Ҳ�����ǳ���Ա�Զ���ת������.
����: 
float x; 
... 
cout << static_cast(x); // print x as int 
... 
f(static_cast("hello")); // call f() for string instead of char* 

2. dynamic_cast 
����̬�ͱ�(polymorphic type)����ת��(downcast)Ϊ��ʵ�ʾ�̬�ͱ�(real static 
type).����Ψһ��ִ���ڽ��м����ת�Ͷ���.���������������ĳ����̬����
���ͱ�,����: 
class Car; // abstract base class (has at least one virtual function) 
class Cabriolet : public Car { 
... 
}; 
class Limousine : public Car { 
... 
}; 
void f(Car* cp) 
{ 
Cabriolet* p = dynamic_cast(cp); 
if (p == NULL) { 
// did not refer to an object of type Cabriolet 
... 
} 
} 
�����������,���ʵ�ʾ�̬�ͱ�ΪCabriolet�Ķ���,f()������Ӧ����Ϊ.��
�����Ǹ�reference,�����ͱ�ת��ʧ��ʱ,dynamic_cast����һ��bad_cast��
��(bad_cast��������p26).ע��,����ƽǶȶ���,��Ӧ�������ö�̬������
������,�������� "������Ϊȡ���ھ����ͱ�"��д��. 

3. const_cast 
�趨��ȥ���ͱ�ĳ�����(constness),���ȥ��volatile�δ�.����֮�ⲻ��
���κ�ת��. 

4. reinterpret_cast 
�˲���������Ϊ��ʵ�ʱ���������.�������½���bits����,��Ҳ��һ�����.ʹ
�ô�һת�Ͷ���ͨ������������ֲ��. 
��Щ������ȡ��������СԲ����������ľ�ʽת��,�ܹ��������ת�͵�Ŀ��.СԲ��
��ת�Ϳ��滻dynamic_cast֮�����������ת��,Ҳ��˵���������ʱ,���޷���
ȷ��ʾʹ������ȷ������.��Щ��ʽת�Ͳ��������˱�����������Ϣ,�ñ��������֪
��ת�͵�����,����ת��ʧ��ʱ�ͳ�һ�ݴ��󱨸�. 
ע��,��Щ��������ֻ����һ������.�Կ���������: 
static_cast(15,100) // Oops, creates Fraction(100) 
�������������ò�������Ҫ�Ľ��.��ֻ��һ����ֵ100,��Fraction ��ʱ����
�趨��ֵ,�����趨����15,��ĸ100.���������ﲢ����ָ�����,�����γ�һ�� 
comma(����)������,���������ʽ���Ϊһ�����ʽ,�����صڶ����ʽ��Ϊ����
���.��15��100"ת��"Ϊ��������ȷ������: 
Fraction(15,100) // fine, creates Fraction(15,100)


CImageList::SetOverlayImage���������ص�ͼ��,������һ��ͼ���������һ����



���RELEASE������,��DEBUG���õĻ�,���һ��Project Settings �����C/C++->Optimizations������,����ѡ��Customizeѡ��,Ȼ���Լ���һ����,�鿴���,LEADTOOLS��Image.dll(ATL)�ͳ��ֹ��������.

CString -> char*  (LPSTR)(LPCTSTR)

m_Tree.SelectItem(m_TreeItem);//����ѡ��

AfxExtractSubString()