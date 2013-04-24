// Asn1ParsePage.cpp : implementation file
//

#include "stdafx.h"
#include "MiniCA.h"
#include "Asn1ParsePage.h"
#include "MiniMainDlg.h"
#include <locale.h>
#include ".\GenericClass\Language.h"
#include "minict.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsn1ParsePage property page

IMPLEMENT_DYNCREATE(CAsn1ParsePage, CPropertyPage)

CAsn1ParsePage::CAsn1ParsePage() : CPropertyPage(CAsn1ParsePage::IDD)
{
	//{{AFX_DATA_INIT(CAsn1ParsePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_TreeItem=NULL;
}

CAsn1ParsePage::~CAsn1ParsePage()
{
}

void CAsn1ParsePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAsn1ParsePage)
//	DDX_Control(pDX, IDC_EDIT_PATH, m_XpEdit);
	DDX_Control(pDX, IDC_B_READ, m_Bread);
	DDX_Control(pDX, IDC_ASN1_TREE, m_Tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAsn1ParsePage, CPropertyPage)
	//{{AFX_MSG_MAP(CAsn1ParsePage)
	ON_BN_CLICKED(IDC_B_READ, OnBRead)
	ON_NOTIFY(TVN_SELCHANGED, IDC_ASN1_TREE, OnSelchangedAsn1Tree)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_ASN1_TREE, OnRclickAsn1Tree)
	ON_COMMAND(ID_MENUITEM_EXPANE, OnMenuitemExpane)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsn1ParsePage message handlers
BOOL CAsn1ParsePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	m_Bread.SetThemeHelper(((CMiniCaApp *)AfxGetApp())->GetThemeHelperST());
//	m_Bread.DrawAsToolbar(TRUE);
	m_Bread.SetIcon(IDI_ICON_SELECTDIR);
	m_Bread.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);

	//m_Bread.SetFlat(FALSE);
	CImageList * pImgList = ((CMiniCaApp *)AfxGetApp())->GetImgList();
	m_Tree.SetImageList(pImgList,TVSIL_NORMAL);
	//XP风格菜单,表现为选中时候图标偏移
	BCMenu::SetMenuDrawMode( BCMENU_DRAWMODE_XP );

	m_toolTip.Create(this);
	m_toolTip.AddTool(GetDlgItem(IDC_B_READ), CMiniCaApp::NormalCode("选择被解码的文件\r文件长度小于10K字节"));


	if(!((CMiniCaApp *)AfxGetApp())->IsXpStyle())
	{
		ClassXP(GetDlgItem(IDC_EDIT_PATH)->m_hWnd,TRUE);
//		ClassXP(GetDlgItem(IDC_ASN1_TREE)->m_hWnd,TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAsn1ParsePage::OnBRead() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgOpen(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT /*| OFN_NOCHANGEDIR */,
	MiniCT_1600, NULL);

	dlgOpen.m_ofn.lStructSize = sizeof(OPENFILENAME);
	CString str = MiniCT_1601;
	dlgOpen.m_ofn.lpstrTitle = str;//标题条 LPCTSTR
	if(dlgOpen.DoModal()!=IDOK) return;
	CString fileName=dlgOpen.GetPathName();
	GetAsn1Info(fileName);
}


void CAsn1ParsePage::OnSelchangedAsn1Tree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM handle=m_Tree.GetSelectedItem();
	POSITION pos;
	stuNODE * tNode;
	CString str;
	for( pos = m_NodeList.GetHeadPosition(); pos != NULL; )
	{
		tNode=(stuNODE *)m_NodeList.GetNext(pos);
		if(tNode->NODE==handle)
		{
			str.Format("%s:%d\r\n%s:%d\r\n%s:%s", 
				MiniCT_1602, tNode->EXCURSION, MiniCT_1603,
				tNode->LENGTH, MiniCT_1604, tNode->INFO);
			SetDlgItemText(IDC_EDIT_INFO,str);
			break;
		}

	}
	*pResult = 0;
}

void CAsn1ParsePage::OnDestroy() 
{
	CPropertyPage::OnDestroy();	
	// TODO: Add your message handler code here
	stuNODE * LIST=NULL;
	for(;!m_NodeList.IsEmpty();)
	{
		LIST=(stuNODE *)m_NodeList.RemoveHead();
		delete LIST;
	}
}

void CAsn1ParsePage::OnRclickAsn1Tree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	CPoint point;
	GetCursorPos(&point); // 当前鼠标坐标
	CPoint PointInTree = point;
	m_Tree.ScreenToClient( &PointInTree);
	
	UINT nFlag;
	m_TreeItem = m_Tree.HitTest( PointInTree, &nFlag );

	if(m_TreeItem == NULL)
		return;
	
	m_Tree.SelectItem(m_TreeItem);//设置选中

	HTREEITEM item = m_Tree.GetChildItem(m_TreeItem);//是否有子项
	if(item==NULL)
		return;

	int nState= m_Tree.GetItemState(m_TreeItem,TVIF_STATE)& TVIS_EXPANDED ;//是否已经展开
	
	BCMenu m_PopMenu;
	m_PopMenu.LoadMenu(IDR_MENU_ASN1VIEW);
	m_PopMenu.LoadToolbar(IDR_MINICAMENU);

	CMenu * pPopup = m_PopMenu.GetSubMenu(0);
	SetForegroundWindow(); //点击可以关闭
	ASSERT(pPopup);

	//ModifyODMenuA
	if(nState!=0)//已经展开
		m_PopMenu.ModifyODMenu(MiniCT_1605, ID_MENUITEM_EXPANE);
	else
		m_PopMenu.ModifyODMenu(MiniCT_1606, ID_MENUITEM_EXPANE);//展开

	UINT nSelection = pPopup->TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_LEFTALIGN, point.x, 
		point.y,this, NULL);
	m_PopMenu.DestroyMenu();
	*pResult = 0;
}

void CAsn1ParsePage::OnMenuitemExpane() //展开
{
	// TODO: Add your command handler code here
	ExpandAll(m_TreeItem,TVE_TOGGLE);
}

void CAsn1ParsePage::ExpandAll(HTREEITEM item,UINT nCode )////展开,收缩
{
	m_Tree.SetRedraw(FALSE);
	if( m_Tree.ItemHasChildren( item ))
	{
		m_Tree.Expand( item, nCode );
		item = m_Tree.GetChildItem( item );
		do
		{
			ExpandAll(item,nCode);
		}while( (item = m_Tree.GetNextSiblingItem( item )) != NULL );
	}
	m_Tree.EnsureVisible( m_Tree.GetSelectedItem());
	m_Tree.SetRedraw();

}

void CAsn1ParsePage::GetAsn1Info(CString strpath)
{

	//由于PEM格式解码经常崩溃,所以在开始时候限制文件大小为2K
	CFile file;
	if(file.Open(strpath,CFile::modeRead))
	{
		if(file.GetLength() >= 10*1024)
		{
			file.Close();
			AddMsg(MiniCT_1607, M_WARING); //文件长度超过10K字节
			return;
		}
		file.Close();
	}
	else
		return;

	m_Tree.DeleteAllItems();
	SetDlgItemText(IDC_EDIT_INFO,"");
	SetDlgItemText(IDC_EDIT_PATH,strpath);
	
	stuNODE * LIST=NULL;
	for(;!m_NodeList.IsEmpty();)
	{
		LIST=(stuNODE *)m_NodeList.RemoveHead();
		delete [] LIST;
	}
	
	char buf[255]={0};
	stuTREE * TREEPEM = NULL, * TREEDER = NULL, * temp = NULL;

	if(Asn1Parse(strpath.GetBuffer(0),TREEDER,DER,buf))//==1 ok
	{
		HTREEITEM depth[20]={0};//深度
		depth[0]=TVI_ROOT;
		CString str;
		for(temp=TREEDER;temp!=NULL;temp=temp->Link)
		{
			stuNODE * pNode=new stuNODE;
			depth[temp->DEPTH+1]=m_Tree.InsertItem(temp->TYPE,10,10,depth[temp->DEPTH]);
			pNode->NODE=depth[temp->DEPTH+1];
			pNode->EXCURSION=temp->EXCURSION;
			pNode->LENGTH=temp->LENGTH;
			if(strlen(temp->INFO))
			{
				strncpy(pNode->INFO,temp->INFO,sizeof(pNode->INFO));
			}
			m_NodeList.AddHead(pNode);
		}
		FreeTree(TREEPEM);
		FreeTree(TREEDER);
		return;
	}
	strpath.ReleaseBuffer();
	

	if(Asn1Parse(strpath.GetBuffer(0),TREEPEM,PEM,buf))//==1 ok
	{
		HTREEITEM depth[20]={0};//深度
		depth[0]=TVI_ROOT;
		CString str;
		for(temp=TREEPEM;temp!=NULL;temp=temp->Link)
		{
			stuNODE * pNode=new stuNODE;
			depth[temp->DEPTH+1]=m_Tree.InsertItem(temp->TYPE,10,10,depth[temp->DEPTH]);
			pNode->NODE=depth[temp->DEPTH+1];
			pNode->EXCURSION=temp->EXCURSION;
			pNode->LENGTH=temp->LENGTH;
			if(strlen(temp->INFO))
			{
				strncpy(pNode->INFO,temp->INFO,sizeof(pNode->INFO));
			}
			m_NodeList.AddHead(pNode);
		}
	}
	else
		AddMsg(buf, M_ERROR);
	strpath.ReleaseBuffer();

	FreeTree(TREEPEM);
	FreeTree(TREEDER);
}

void CAsn1ParsePage::AddMsg(CString info, DWORD type)
{
	((CMiniMainDlg *)GetParent())->AddMsg(MiniCT_0005, info, type);

}


BOOL CAsn1ParsePage::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	m_toolTip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

/*添加链表节点*/
int CAsn1ParsePage::AddTree(stuTREE *& Head,UINT Excursion,UINT Depth,UINT Length,char * Type)//返回当前序号
{
	int index=0;
	stuTREE * End=new stuTREE(Excursion,Depth,Length,Type);//钥增加的节点
	if(Head==NULL)
	{
		Head=End;
	}
	else 
	{
		stuTREE * p=Head;
		index+=1;
		while(p->Link!=NULL)
		{
			index++;
			p=p->Link;
		}
		p->Link=End; 
	}

	return index;
}

void CAsn1ParsePage::FreeTree(stuTREE *& Head)
{
	stuTREE * temp =NULL;

	while(Head!=NULL)//遍历链表
	{
	    temp=Head;
		Head=Head->Link;
		delete [] temp;
	}

}
int CAsn1ParsePage::asn1_print_info(int tag, int xclass, int constructed,
					int indent,char * str)
{
//	if (constructed & V_ASN1_CONSTRUCTED)
//		p="cons: ";
//	else
//		p="prim: ";
//	if (BIO_write(bp,p,6) < 6) goto err;
//	BIO_indent(bp,indent,128);
	const char *p;
	if ((xclass & V_ASN1_PRIVATE) == V_ASN1_PRIVATE)
		sprintf(str,"Private[%d] ",tag);
	else if ((xclass & V_ASN1_CONTEXT_SPECIFIC) == V_ASN1_CONTEXT_SPECIFIC)
		sprintf(str,"Context[%d]",tag);
	else if ((xclass & V_ASN1_APPLICATION) == V_ASN1_APPLICATION)
		sprintf(str,"Application [%d]",tag);
	else 
	{
		p = ASN1_tag2str(tag);
		sprintf(str,p);
	}
	return(1);
}

void CAsn1ParsePage::Utf8ToAnsi(UCHAR * lpsrc, int srclen, LPSTR lpdst, int& dstlen)
{
	WCHAR * Unicode;
    int len = MultiByteToWideChar ( CP_UTF8 , 0 ,(char*) lpsrc ,-1 ,NULL,0);
    Unicode = new WCHAR[len * sizeof(WCHAR)];
    MultiByteToWideChar ( CP_UTF8 , 0 ,( char * ) lpsrc, -1, Unicode , len );
    len = WideCharToMultiByte(CP_ACP,0,Unicode,-1,NULL,0,NULL,NULL);
    dstlen = WideCharToMultiByte (CP_ACP,0,Unicode,-1,lpdst,len,NULL,NULL);
    delete []Unicode;
}

BOOL CAsn1ParsePage::GetAsn1Str(ASN1_STRING * str,char * outName)
{
	if(str==NULL)
		return FALSE;
	setlocale(LC_CTYPE, "");
	//判断str是否为UTF-8
	int len = 0;
	unsigned char * tmp = NULL; 
	if (str && ASN1_STRING_type(str) == V_ASN1_UTF8STRING)
	{
		len = ASN1_STRING_length(str);
		if (len >= 0)
		{
			tmp = (UCHAR *)OPENSSL_malloc(len+1);
			if (tmp)
			{
				memcpy(tmp, ASN1_STRING_data(str), len);
				tmp[len] = '\0';
			}
		}
	}
	else /* not a UTF8 name */		///////////转UTF-8
	{
		len = ASN1_STRING_to_UTF8(&tmp, str);
	}
	int dstlen = 0;
	Utf8ToAnsi(tmp, len, outName, dstlen);
	OPENSSL_free(tmp);
	return TRUE;
}

int CAsn1ParsePage::asn1_parse2(stuTREE *& TREE, unsigned char **pp, long length, int offset,
				int depth, int indent, int dump,char * msg)
{
	const unsigned char *p,*ep,*tot,*op,*opp;
	long len;
	int tag,xclass,ret=0;
	int nl,hl,j,r;
	ASN1_OBJECT *o=NULL;
	ASN1_OCTET_STRING *os=NULL;
	/* ASN1_BMPSTRING *bmp=NULL;*/
	int dump_indent = 6;	/* Because we know BIO_dump_indent() */
	p= *pp;
	tot=p+length;
	op=p-1;
	char  strInfo[128]={0};
	BUF_MEM *bptr;
	int biolen;
	char * pbuf;
	int index=0,i=0;
	ASN1_STRING * AsnStr;
	char buf[255]={0};

	BIO *mem = BIO_new(BIO_s_mem());
	BIO_set_close(mem, BIO_CLOSE); /*  BIO_free() free BUF_MEM  */

	while ((p < tot) && (op < p))
	{
		i=0;
		op=p;
		j=ASN1_get_object(&p,&len,&tag,&xclass,length);//p指向变化
		if (j & 0x80)
		{
			strcpy(msg,"Error in encoding\n");
			ret=0;
			goto end;
		}
		hl=(p-op);
		length-=hl;
		/* if j == 0x21 it is a constructed indefinite length object */
		if (!asn1_print_info(tag,xclass,j,(indent)?depth:0,strInfo))//得到strInfo(SEQUENCE)
			goto end;
		//增加链表
		index=AddTree(TREE,(long)offset+(long)(op- *pp),depth,len,strInfo);
		if (j & V_ASN1_CONSTRUCTED)
		{
			ep=p+len;
			if (len > length)
			{
				sprintf(msg,"length is greater than %ld",length);
				ret=0;
				goto end;
			}
			if ((j == 0x21) && (len == 0))
			{
				for (;;)
				{
					r=asn1_parse2(TREE,(UCHAR **)&p,(long)(tot-p),
						offset+(p - *pp),depth+1,
						indent,dump,msg);
					if (r == 0) { ret=0; goto end; }
					if ((r == 2) || (p >= tot)) break;
				}
			}
			else
				while (p < ep)
				{
					r=asn1_parse2(TREE,(UCHAR **)&p,(long)len,
						offset+(p - *pp),depth+1,
						indent,dump,msg);
					if (r == 0) { ret=0; goto end; }
				}
		}
		else if (xclass != 0)
		{
			p+=len;
		}
		else
		{
			nl=0;
			if (	(tag == V_ASN1_PRINTABLESTRING) ||
				(tag == V_ASN1_T61STRING) ||
				(tag == V_ASN1_IA5STRING) ||
				(tag == V_ASN1_VISIBLESTRING) ||
				(tag == V_ASN1_UTCTIME) ||
				(tag == V_ASN1_GENERALIZEDTIME))
			{
				if ((len > 0) &&
					BIO_write(mem,(char *)p,(int)len)
					!= (int)len)
					goto end;
			}
			else if (tag == V_ASN1_OBJECT)
			{
				opp=op;
				if (d2i_ASN1_OBJECT(&o,&opp,len+hl) != NULL)
				{
					i2a_ASN1_OBJECT(mem,o);
				}
				else
				{
					if (BIO_write(mem,":BAD OBJECT",11) <= 0)
						goto end;
				}
			}
			else if (tag == V_ASN1_BOOLEAN)
			{
				int ii;		
				opp=op;
				ii=d2i_ASN1_BOOLEAN(NULL,&opp,len+hl);
				if (ii < 0)
				{
					if (BIO_write(mem,"Bad boolean\n",12))
						goto end;
				}
				BIO_printf(mem,":%d",ii);

			}
			else if (tag == V_ASN1_BMPSTRING||tag ==V_ASN1_UTF8STRING)
			{
				AsnStr=ASN1_STRING_type_new(tag);
				ASN1_STRING_set(AsnStr,p,len);
				GetAsn1Str(AsnStr,buf);
				ASN1_STRING_free(AsnStr);

				if (BIO_write(mem,buf,strlen(buf)) < 0)
					goto end;

			}
			else if (tag == V_ASN1_OCTET_STRING)
			{
				int i,printable=1;
				opp=op;
				os=d2i_ASN1_OCTET_STRING(NULL,&opp,len+hl);
				if (os != NULL)
				{
					opp=os->data;
					for (i=0; i<os->length; i++)
					{
						if ((	(opp[i] < ' ') &&
							(opp[i] != '\n') &&
							(opp[i] != '\r') &&
							(opp[i] != '\t')) ||
							(opp[i] > '~'))
						{
							printable=0;
							break;
						}
					}
					if (printable && (os->length > 0))
					{
						if (BIO_write(mem,(char *)opp,
							os->length) <= 0)
							goto end;
					}
					if (!printable && (os->length > 0)
						&& dump)
					{
						if (!nl) 
						{
						}
						if (BIO_dump_indent(mem,(char *)opp,
							((dump == -1 || dump > os->length)?os->length:dump),
							dump_indent) <= 0)
							goto end;
						nl=1;
					}
					M_ASN1_OCTET_STRING_free(os);
					os=NULL;
				}
			}
			else if (tag == V_ASN1_INTEGER)
			{
				ASN1_INTEGER *bs;
				int i;
				
				opp=op;
				bs=d2i_ASN1_INTEGER(NULL,&opp,len+hl);
				if (bs != NULL)
				{
					for (i=0; i<bs->length; i++)
					{
						if (BIO_printf(mem,"%02X",bs->data[i]) <= 0)
							goto end;
					}
					if (bs->length == 0)
					{
						if (BIO_write(mem,"00",2) <= 0)
							goto end;
					}
				}
				else
				{
					if (BIO_write(mem,"BAD INTEGER",11) <= 0)
						goto end;
				}
				M_ASN1_INTEGER_free(bs);
			}
			else if (tag == V_ASN1_ENUMERATED)
			{
				ASN1_ENUMERATED *bs;
				int i;
				
				opp=op;
				bs=d2i_ASN1_ENUMERATED(NULL,&opp,len+hl);
				if (bs != NULL)
				{
					if (bs->type == V_ASN1_NEG_ENUMERATED)
						if (BIO_write(mem,"-",1) <= 0)
						for (i=0; i<bs->length; i++)
						{
							if (BIO_printf(mem,"%02X",bs->data[i]) <= 0)
								goto end;

						}
						if (bs->length == 0)
						{
							if (BIO_write(mem,"00",2) <= 0)
								goto end;
						}
				}
				else
				{
					if (BIO_write(mem,"BAD ENUMERATED",11) <= 0)
						goto end;
				}
				M_ASN1_ENUMERATED_free(bs);
			}
			else if (len > 0 && dump)
			{
				if (!nl) 
				{
				}
				if (BIO_dump_indent(mem,(char *)p,
					((dump == -1 || dump > len)?len:dump),
					dump_indent) <= 0)
					goto end;
				nl=1;
			}
			
			p+=len;
			if ((tag == V_ASN1_EOC) && (xclass == 0))
			{
				ret=2; // End of sequence 
				goto end;
			}
		}
		length-=len;
		if((p < tot) && (op < p))
		{
			BIO_get_mem_ptr(mem, &bptr);
//			BIO_get_mem_ptr(mem, &bptr);
			biolen=bptr->length;
			pbuf=new char[biolen+1];
			memset(pbuf,0,biolen+1);
			memcpy(pbuf,bptr->data,biolen);
			for(stuTREE * temp=TREE;temp!=NULL;temp=temp->Link,i++)
			{
				if(i==index)
				{
					strncpy(temp->INFO,pbuf,biolen+1>sizeof(temp->INFO)?128:biolen+1);
					break;
				}
			}
			delete [] pbuf;
			BIO_reset(mem);//恢复bio
		}
	}
	ret=1;
end:
		BIO_get_mem_ptr(mem, &bptr);
//		BIO_get_mem_ptr(mem, &bptr);
		biolen=bptr->length;
		pbuf=new char[biolen+1];
		memset(pbuf,0,biolen+1);
		memcpy(pbuf,bptr->data,biolen);
		for(stuTREE * temp=TREE;temp!=NULL;temp=temp->Link,i++)
		{
			if(i==index)
			{
				strncpy(temp->INFO,pbuf,biolen+1>sizeof(temp->INFO)?128:biolen+1);
				break;
			}
		}

		delete [] pbuf;
		if (mem != NULL) BIO_free(mem);
		if (o != NULL) ASN1_OBJECT_free(o);
		if (os != NULL) M_ASN1_OCTET_STRING_free(os);
		*pp=(UCHAR *)p;
		return(ret);
}

int CAsn1ParsePage::ASN1_parsedump(stuTREE *& TREE, unsigned char *pp, long len,char * msg)
{
	int indent=0;//不用格式化根据输出的数据自动缩进(不使用-i参数)
	int dump= 0;//输出16进制数据(0不使用 -dump参数,-1使用)
	return(asn1_parse2(TREE,&pp,len,0,0,indent,dump,msg));
}

BOOL CAsn1ParsePage::Asn1Parse(const char * infile,stuTREE *& TREE,const int informat,char * msg)
{
	int i,badops=0,ret=0;//offset=0,,j
	unsigned int length=0;
	long num;
	BIO *in=NULL,*b64=NULL;
	int indent=0;
	char *str=NULL;
	BUF_MEM *buf=NULL;

	OpenSSL_add_all_algorithms();
	in=BIO_new(BIO_s_file());
	if (in == NULL)
	{
		sprintf(msg,"opne %s error",infile);
		goto end;
	}

	if (BIO_read_filename(in,infile) <= 0)
	{
		strcpy(msg,"read file error");
		goto end;
	}

	if ((buf=BUF_MEM_new()) == NULL) goto end;
	if (!BUF_MEM_grow(buf,BUFSIZ*8)) goto end; /* Pre-allocate :-) changes the size */

	if (informat == PEM)//PEM格式的先解码为der
	{
		BIO *tmp;
		if ((b64=BIO_new(BIO_f_base64())) == NULL)
			goto end;
		BIO_push(b64,in);//该函数把in附加到b64上，并返回b64,形成b64-in链
		tmp=in;
		in=b64;
		b64=tmp;
	}

	num=0;
	for (;;)
	{
		if (!BUF_MEM_grow(buf,(int)num+BUFSIZ)) goto end;
		try
		{
			i = BIO_read(in,&(buf->data[num]),BUFSIZ);
		}
		catch(...)
		{
			return FALSE;
		}
		if (i <= 0) break;
		num+=i;
	}
	str=buf->data;

	/* If any structs to parse go through in sequence */

	if (length == 0) length = (unsigned int)num;
	if (length == 0) 
		goto end;
	if (!ASN1_parsedump(TREE,(unsigned char *)&(str)[0],length,msg))//str[offset]
	{
		strcpy(msg,"parse file error");
		goto end;
	}
	ret=1;
end:
	if (in != NULL) BIO_free(in);
	if (b64 != NULL) BIO_free(b64);
	if (ret == 0)
		strncpy(msg, MiniCT_1608, 20); //解码错误
	if (buf != NULL) BUF_MEM_free(buf);
	OBJ_cleanup();
	EVP_cleanup();
	return ret;
}

void CAsn1ParsePage::TranslateCT()	//繙譯諸如樹型控件,列錶控件等內容
{
	SetDlgItemText(IDC_B_READ, MiniCT_10001); //
}