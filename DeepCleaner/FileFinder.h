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

		// 重置所有的值
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

		// 加入常规文件进行搜索(FILE_ATTRIBUTE_ARCHIVE) 
		void FindNormalFiles()
		{
			dwOptionsFlags |= FIND_ATTRIBUTES;
			dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE;
		}

		// 加入所有的文件进行搜索 (隐藏,只读, ...) 不包括目录
		void FindAllFiles()
		{
			dwOptionsFlags |= FIND_ATTRIBUTES;
			dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED | 
								FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | 
								FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY;
		}

		// 加入路径进行搜索
		void FindDirectories()
		{
			dwOptionsFlags |= FIND_ATTRIBUTES;
			dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
		}

		// 在文件中查找字符串
		void FindText(LPCTSTR szText)
		{
			dwOptionsFlags |= FIND_TEXT;
			sFindText = szText;
		}

		CString		sBaseFolder;			// 初始文件夹
		CString		sFileMask; 
		std::vector<CString> FileMaskList;
		/*
		CString		sFileMaskN[s_N] = {
									"*军*", 
									"*密*",
									"*党*",
									"*徐才厚*",
									"*郭伯雄*",
									"*信息工程大学*",
									"*信大*",
									"*.avi" ,
									"*.rmvb" ,
									"*.mkv" };				// 文件屏蔽码(例如: "*.txt")
		*/
		
		CString		sFindText;				// 要查找的字符串
		BOOL		bSubfolders;			// 是否递归查找
		DWORD		dwOptionsFlags;			// 在FindOptionsEnum中的值
		__int64		nMinSize;				// 最小文件尺寸
		__int64		nMaxSize;				// 最大文件尺寸
		CTime		tMinCreated;			// 最老的创建日期
		CTime		tMaxCreated;			// 最新的创建日期
		CTime		tMinModified;			// 最老的修改日期
		CTime		tMaxModified;			// 最新的修改日期
		CTime		tMinAccessed;			// 最老的访问日期
		CTime		tMaxAccessed;			// 最新的访问日期
		DWORD		dwFileAttributes;		// 如WIN32_FIND_DATA
		//	添加敏感词
		void setMi(CStringArray &list)
		{
			int n = list.GetCount();
			for (int i = 0; i < n; i++)
			{
				this->FileMaskList.push_back(list[i]);
			}
		}
	};

	// 找到从初始目录开始的符合szFileMask的文件
	int		FindFiles(LPCTSTR szBaseFolder, LPCTSTR szFileMask, BOOL bSubFolders = FALSE);
	// 找到符合类CFindOpts中参数所要求的文件
	int		Find(CFileFinder::CFindOpts &opts);

	// 在文件中找到指定的字符串
	BOOL	FindTextInFile(LPCTSTR szFile, LPCTSTR szText);

	// 返回找到的文件数目
	int		GetFileCount();
	// 返回路径为szPath的文件索引
	int		FindPathItem(LPCTSTR szPath);
	// 根据文件索引路径返回CPath对象
	CPath	GetFilePath(int nIndex);

	// 删除列表指定位置的项目
	void	RemoveAt(int nIndex);
	// 删除列表中所有的项目
	void	RemoveAll();

	// 回调函数，用于文件查找过程中状态的设定
	void	SetCallback(FILEFINDERPROC pFileFinderProc, void *pCustomParam);
	// 停止搜索文件
	void	StopSearch();
	// 返回正在查找的文件夹
	LPCTSTR	GetSearchingFolder();



private:
	CStringArray	_aFilesFound;
	bool			_bStopSearch;
	FILEFINDERPROC	_pFileFinderProc;
	void			*_pCustomParam;
	CString			_sSearchingFolder;
};

#endif // !defined(__FILEFINDER_H__)
