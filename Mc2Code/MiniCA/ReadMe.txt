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
当回调函数使用类成员时候(控件),由于没有this指针,这是后可以通过静态成员变量获取,而避免了afxgetmain()
当线程中使用类成员时候(控件),事先要定义一个变量,在线程外得到控件的指针,而afxget得到的是错误的
具体操作看evp

对话框中插入控件的前提条件
	AfxEnableControlContainer();
 	CoInitialize(NULL);

OPENSSL 要求使用
	Multithreaded

如何某个类不能用向导加入系统消息，可能需要重建clw文件


13. I think I've detected a memory leak, is this a bug?
In most cases the cause of an apparent memory leak is an OpenSSL internal table that is allocated when an application starts up. Since such tables do not grow in size over time they are harmless. 

These internal tables can be freed up when an application closes using various functions. Currently these include following: 

Thread-local cleanup functions: 


  ERR_remove_state()

Application-global cleanup functions that are aware of usage (and therefore thread-safe): 

  ENGINE_cleanup() and CONF_modules_unload()

"Brutal" (thread-unsafe) Application-global cleanup functions: 

  ERR_free_strings(), EVP_cleanup() and CRYPTO_cleanup_all_ex_data().

前几天，为了给大家提供新版本的openssl，又重新做了一次openssl的编译工作，并比较了几种编译方法，写这个文章简单介绍一下，希望对大家有帮助。
     我编译的版本是0.9.6h，编译器使用VC++6.0,根据它的说明，我进行了如下的命令和操作：
     1.安装ActivePerl-5.6.1.629-MSWin32-x86-multi-thread.顺利通过！
     2.打开控制台程序，在openssl解压后的目录下执行Perl Configure VC-WIN32命令，注意，一定要在这个目录下执行该命令，否则找不到Configure文件，当然，你也可以指定完整的Configure文件路径。顺利通过。
     3.根据openssl的帮助说明，在解压目录下执行ms\do_masm(和ms_\do_ms)命令，成功通过。
     4.配置VC环境变量，我在相同的控制台程序下转到VC所在目录下的vc98\bin目录，执行vcvars32批处理命令，设置VC的环境变量，然后再转到openssl解压包的根目录下。
     注意：我第一次运行Vcvars32的时候，报错，错误为：Out of environment space，这个错误的产生是因为你的MS－DOS环境变量没有分配足够的内存，这可以通过如下方法解决：
     a.点击开始, 点击运行. 
     b.输入sysedit后回车运行该程序. 
     c.然后会出现多个窗口，选择Config.sys窗口. 
     d.如果存在Shell= 的语句, 在其前面输入 REM ，即注释该语句. 
     e.如果该语句不存在，就直接输入SHELL=C:\COMMAND.COM /E:4096 /P ，注意，要保证该语句是从第一行第一个位置开始输入的。
     f.保存、退出重新启动电脑就应该可以解决这个问题。
     按上述方法解决这个问题之后，重新启动电脑后可以直接执行第4步。
     5.在openssl解压目录下执行nmake -f ms\ntdll.mak，如果没有错误，就完成编译了。输出的文件在out32dll里面，包括应用程序的可执行文件、两个Lib文件和两个dll文件。 
     
     我根据openssl的说明，做了两种不通的编译，一直是加了ASM汇编语言编译器，一直是没有，即在第三步的时候执行do_masm或do_ms命令，时间上前者大概为3分钟（我的电脑配置是图拉丁塞杨1.2，内存256SDRM）,后者大概是6分钟的样子。
     然后，我使用了第三方提供的一个编译方法，该方法自动配置了openssl各个模块的V虚拟C工程连接配置项目，然后再进行编译，速度很慢，用了至少20分钟以上的时间，但是好处是可以将一个一个项目弄的比较清楚，其编译过程我就不说了，大家可以参看相关的文档。


certmgr.dll - 证书图标
shell32.dll
comres.dll - COM图标

EvpClean()函数说明,此函数在程序销毁时候调用,否则在一个函数中调用过,即使下一个函数中重新加载了算法,也可能出现没有加密库的错误.

关于类型转换:

1. static_cast 
将一个值以合逻辑的方式转型.这可看做是"利用原值重建一个临时对象,并在设
立初值时使用型别转换".唯有当上述的型别转换有所定义,整个转换才会成功.
所谓的"有所定义",可以是语言内建规则,也可以是程序员自定的转换动作.
例如: 
float x; 
... 
cout << static_cast(x); // print x as int 
... 
f(static_cast("hello")); // call f() for string instead of char* 

2. dynamic_cast 
将多态型别(polymorphic type)向下转型(downcast)为其实际静态型别(real static 
type).这是唯一在执行期进行检验的转型动作.你可以用它来检验某个多态对象
的型别,例如: 
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
在这个例子中,面对实际静态型别为Cabriolet的对象,f()有特殊应对行为.当
参数是个reference,而且型别转换失败时,dynamic_cast丢出一个bad_cast异
常(bad_cast的描述见p26).注意,以设计角度而言,你应该在运用多态技术的
程序中,避免这种 "程序行为取决于具体型别"的写法. 

3. const_cast 
设定或去除型别的常数性(constness),亦可去除volatile饰词.除此之外不允
许任何转换. 

4. reinterpret_cast 
此操作符的行为由实际编译器定义.可能重新解释bits意义,但也不一定如此.使
用此一转型动作通常带来不可移植性. 
这些操作符取代了以往小圆括号所代表的旧式转型,能够清楚阐明转型的目的.小圆括
号转型可替换dynamic_cast之外的其它三种转型,也因此当你运用它时,你无法明
确显示使用它的确切理由.这些新式转型操作符给了编译器更多信息,让编译器清楚知
道转型的理由,并在转型失败时释出一份错误报告. 
注意,这些操作符都只接受一个参数.试看以下例子: 
static_cast(15,100) // Oops, creates Fraction(100) 
在这个例子中你得不到你想要的结果.它只用一个数值100,将Fraction 暂时对象
设定初值,而非设定分子15,分母100.逗号在这里并不起分隔作用,而是形成一个 
comma(逗号)操作符,将两个表达式组合为一个表达式,并传回第二表达式做为最终
结果.将15和100"转换"为分数的正确做法是: 
Fraction(15,100) // fine, creates Fraction(15,100)


CImageList::SetOverlayImage可以设置重叠图标,比如在一个图标上面加上一个感



如果RELEASE不能用,而DEBUG能用的话,检查一下Project Settings 里面的C/C++->Optimizations的设置,可以选择Customize选项,然后自己逐一设置,查看结果,LEADTOOLS的Image.dll(ATL)就出现过这种情况.

CString -> char*  (LPSTR)(LPCTSTR)

m_Tree.SelectItem(m_TreeItem);//设置选中

AfxExtractSubString()