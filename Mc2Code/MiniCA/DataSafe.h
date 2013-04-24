// DataSafe.h: interface for the CDataSafe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASAFE_H__C83B38BD_951D_4CCE_93FB_63C591A85490__INCLUDED_)
#define AFX_DATASAFE_H__C83B38BD_951D_4CCE_93FB_63C591A85490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//总文件由 文件个数n(int类型) n个CDataSafe结构体 多个加密后文件组成
//主意文件个数类型为int
class CDataSafe  
{
public:
	//文件信息结构体
//	UINT uID;					//文件次序索引,通过ID可以找到文件信息
	char cName[256];			//文件名
	DWORD dOldLen;				//原始文件大小
	DWORD dBeginSeek;			//文件开始位置
	DWORD dNewLen;				//加密后文件大小 结束位置 = 开始位置 + 加密后文件大小
	FILETIME ftLastWriteTime;	//最后修改时间
	bool bIsFile;				//结构属性, true - 文件, false - 目录
	UINT uParentID;				//父ID号,父ID表示当前结构的上级目录
	char cPathName[256];		//文件名 - 文件时候有意义,用于加密,文件全路径
	
	CDataSafe()
	{
		memset(this, 0, sizeof(CDataSafe));
	}
	CDataSafe(CFile & File, const LONG lOff)
	{
	//	pFile->Seek(CFile::current, lOff);
	//	pFile->Read(this, sizeof(CDataSafe));
	}
	CDataSafe(const char * pFileName, const DWORD doldLen, const FILETIME & fileTime,
				const bool bisFile, const UINT uparentID, const char * pPathName = 0)
	{
//		this->uID = id;
		strncpy(this->cName, pFileName, 255);
		this->dOldLen = doldLen;
		this->ftLastWriteTime = fileTime;
		this->bIsFile = bisFile;
		this->dBeginSeek = 0;
		this->uParentID = uparentID;
		this->dNewLen = 0;
		if(pPathName)
			strncpy(this->cPathName, pPathName, 255);	//文件才有效
	}
	~CDataSafe()	//如果签名有virtual,则用new 声明 delete 会错误,不知为什么
	{
	}
	//写自身到文件 , 文件操作指针, 通过uID成员寻址到当前位置,写入文件
	//为文件头的大小 标示结构体个数的类型为INT
	void Write(CFile & File)			
	{
		File.Write(this, sizeof(CDataSafe) - sizeof(cPathName));
	}
	void Read(CFile & File)
	{
		File.Read(this, sizeof(CDataSafe) - sizeof(cPathName));
	}
	
};

#endif // !defined(AFX_DATASAFE_H__C83B38BD_951D_4CCE_93FB_63C591A85490__INCLUDED_)
