//Download by http://www.NewXing.com


#if !defined(__FILEFINDER_H__)
#define __FILEFINDER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define s_N 10
#include "Path.h"
#include <vector>

enum FindOptionsEnum 
{
	FIND_SIZE			= (1L << 0),
	FIND_DATEMODIFIED	= (1L << 1),
	FIND_DATECREATED	= (1L << 2),
	FIND_DATEACCESSED	= (1L << 3),
	FIND_ATTRIBUTES		= (1L << 4),
	FIND_TEXT			= (1L << 5),
};

enum FileFinderProcCodes
{
	FF_FOUND,
	FF_DISCARDED,
	FF_FOLDER,
	FF_FINDTEXT,
};



class CFileFinder;
typedef void (*FILEFINDERPROC) (CFileFinder *pFinder, DWORD dwCode, void *pCustomParam);

class CFileFinder  
{
public:
	CFileFinder();
	virtual ~CFileFinder();

	class CFindOpts
	{
	public:
		CFindOpts() 
		{	
			Reset();
		}

		~CFindOpts() {}

		// �������е�ֵ
		void Reset()
		{
			sBaseFolder.Empty();
			sFileMask = "*.*";
			bSubfolders = FALSE;
			nMinSize = nMaxSize = 0;
			dwFileAttributes = 0;
			dwOptionsFlags = 0;
			tMinCreated = CTime::GetCurrentTime();
			tMaxCreated = CTime::GetCurrentTime();
			tMinModified = CTime::GetCurrentTime();
			tMaxModified = CTime::GetCurrentTime();
			tMinAccessed = CTime::GetCurrentTime();
			tMaxAccessed = CTime::GetCurrentTime();
		}

		// ���볣���ļ���������(FILE_ATTRIBUTE_ARCHIVE) 
		void FindNormalFiles()
		{
			dwOptionsFlags |= FIND_ATTRIBUTES;
			dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
		}

		// �������е��ļ��������� (����,ֻ��, ...) ������Ŀ¼
		void FindAllFiles()
		{
			dwOptionsFlags |= FIND_ATTRIBUTES;
			dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED | 
								FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | 
								FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY;
		}

		// ����·����������
		void FindDirectories()
		{
			dwOptionsFlags |= FIND_ATTRIBUTES;
			dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
		}

		// ���ļ��в����ַ���
		void FindText(LPCTSTR szText)
		{
			dwOptionsFlags |= FIND_TEXT;
			sFindText = szText;
		}

		CString		sBaseFolder;			// ��ʼ�ļ���
		CString		sFileMask; 
		std::vector<CString> FileMaskList;
		/*
		CString		sFileMaskN[s_N] = {
									"*��*", 
									"*��*",
									"*��*",
									"*��ź�*",
									"*������*",
									"*��Ϣ���̴�ѧ*",
									"*�Ŵ�*",
									"*.avi" ,
									"*.rmvb" ,
									"*.mkv" };				// �ļ�������(����: "*.txt")
		*/
		
		CString		sFindText;				// Ҫ���ҵ��ַ���
		BOOL		bSubfolders;			// �Ƿ�ݹ����
		DWORD		dwOptionsFlags;			// ��FindOptionsEnum�е�ֵ
		__int64		nMinSize;				// ��С�ļ��ߴ�
		__int64		nMaxSize;				// ����ļ��ߴ�
		CTime		tMinCreated;			// ���ϵĴ�������
		CTime		tMaxCreated;			// ���µĴ�������
		CTime		tMinModified;			// ���ϵ��޸�����
		CTime		tMaxModified;			// ���µ��޸�����
		CTime		tMinAccessed;			// ���ϵķ�������
		CTime		tMaxAccessed;			// ���µķ�������
		DWORD		dwFileAttributes;		// ��WIN32_FIND_DATA
		//	������д�
		void setMi(CStringArray &list)
		{
			int n = list.GetCount();
			for (int i = 0; i < n; i++)
			{
				this->FileMaskList.push_back(list[i]);
			}
		}
	};

	// �ҵ��ӳ�ʼĿ¼��ʼ�ķ���szFileMask���ļ�
	int		FindFiles(LPCTSTR szBaseFolder, LPCTSTR szFileMask, BOOL bSubFolders = FALSE);
	// �ҵ�������CFindOpts�в�����Ҫ����ļ�
	int		Find(CFileFinder::CFindOpts &opts);

	// ���ļ����ҵ�ָ�����ַ���
	BOOL	FindTextInFile(LPCTSTR szFile, LPCTSTR szText);

	// �����ҵ����ļ���Ŀ
	int		GetFileCount();
	// ����·��ΪszPath���ļ�����
	int		FindPathItem(LPCTSTR szPath);
	// �����ļ�����·������CPath����
	CPath	GetFilePath(int nIndex);

	// ɾ���б�ָ��λ�õ���Ŀ
	void	RemoveAt(int nIndex);
	// ɾ���б������е���Ŀ
	void	RemoveAll();

	// �ص������������ļ����ҹ�����״̬���趨
	void	SetCallback(FILEFINDERPROC pFileFinderProc, void *pCustomParam);
	// ֹͣ�����ļ�
	void	StopSearch();
	// �������ڲ��ҵ��ļ���
	LPCTSTR	GetSearchingFolder();



private:
	CStringArray	_aFilesFound;
	bool			_bStopSearch;
	FILEFINDERPROC	_pFileFinderProc;
	void			*_pCustomParam;
	CString			_sSearchingFolder;
};

#endif // !defined(__FILEFINDER_H__)
