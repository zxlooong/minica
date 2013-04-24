// DataSafe.h: interface for the CDataSafe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASAFE_H__C83B38BD_951D_4CCE_93FB_63C591A85490__INCLUDED_)
#define AFX_DATASAFE_H__C83B38BD_951D_4CCE_93FB_63C591A85490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���ļ��� �ļ�����n(int����) n��CDataSafe�ṹ�� ������ܺ��ļ����
//�����ļ���������Ϊint
class CDataSafe  
{
public:
	//�ļ���Ϣ�ṹ��
//	UINT uID;					//�ļ���������,ͨ��ID�����ҵ��ļ���Ϣ
	char cName[256];			//�ļ���
	DWORD dOldLen;				//ԭʼ�ļ���С
	DWORD dBeginSeek;			//�ļ���ʼλ��
	DWORD dNewLen;				//���ܺ��ļ���С ����λ�� = ��ʼλ�� + ���ܺ��ļ���С
	FILETIME ftLastWriteTime;	//����޸�ʱ��
	bool bIsFile;				//�ṹ����, true - �ļ�, false - Ŀ¼
	UINT uParentID;				//��ID��,��ID��ʾ��ǰ�ṹ���ϼ�Ŀ¼
	char cPathName[256];		//�ļ��� - �ļ�ʱ��������,���ڼ���,�ļ�ȫ·��
	
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
			strncpy(this->cPathName, pPathName, 255);	//�ļ�����Ч
	}
	~CDataSafe()	//���ǩ����virtual,����new ���� delete �����,��֪Ϊʲô
	{
	}
	//д�����ļ� , �ļ�����ָ��, ͨ��uID��ԱѰַ����ǰλ��,д���ļ�
	//Ϊ�ļ�ͷ�Ĵ�С ��ʾ�ṹ�����������ΪINT
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
