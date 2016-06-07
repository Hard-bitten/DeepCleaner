//Download by http://www.NewXing.com

//////////////////////////////////////////////////////////////////////
//
// FileFinder.cpp: implementation of the CFileFinder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileFinder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileFinder::CFileFinder()
{
	_bStopSearch = false;
	_pFileFinderProc = NULL;
	_pCustomParam = NULL;
}

CFileFinder::~CFileFinder()
{

}

int CFileFinder::FindFiles(LPCTSTR szBaseFolder, LPCTSTR szFileMask, BOOL bSubFolders)
{
	CFindOpts opts;

	opts.sBaseFolder = szBaseFolder;
//	opts.sFileMask = szFileMask;
	opts.bSubfolders = bSubFolders;
	opts.FileMaskList.push_back("*��*");
	opts.FileMaskList.push_back("*��*");
	opts.FileMaskList.push_back("*��*");
	opts.FileMaskList.push_back("*��ź�*");
	opts.FileMaskList.push_back("*������*");
	opts.FileMaskList.push_back("*��Ϣ���̴�ѧ*");
	opts.FileMaskList.push_back("*�Ŵ�*");
	opts.FileMaskList.push_back("*.avi");
	opts.FileMaskList.push_back("*.rmvb");
	opts.FileMaskList.push_back("*.mkv");
	// Get all files, but no directories
	opts.FindAllFiles();

	Find(opts);

	return GetFileCount();
}


//����ָ����ʼĿ¼�·����������ļ�
int CFileFinder::Find(CFileFinder::CFindOpts &opts)
{
	CFileFind	finder;
	CString		sFullMask;
	CFindOpts	subOpts;
	BOOL		bFound, bValidFile;
	CTime		timeFile;

	_bStopSearch = false;

	opts.sBaseFolder = CPath::AddBackSlash(opts.sBaseFolder);//���"/"

	// �Ƿ���Ҫ������Ŀ¼
	if (opts.bSubfolders)
	{
		sFullMask = opts.sBaseFolder + CString("*.*");
		bFound = finder.FindFile(sFullMask);
		while ((bFound) && (!_bStopSearch))
		{
			bFound = finder.FindNextFile();
			if ((finder.IsDirectory()) && (!finder.IsDots())) //����Ŀ¼
			{
				subOpts = opts;
				subOpts.sBaseFolder = opts.sBaseFolder + finder.GetFileName();

				//�ݹ������Ŀ¼
				Find(subOpts);
			}
		}
	}

	finder.Close();

	_sSearchingFolder = opts.sBaseFolder;//���ڲ��ҵ�Ŀ¼

	// ���ûص�����
	if (_pFileFinderProc != NULL)
		_pFileFinderProc(this, FF_FOLDER, _pCustomParam); //�ҵ���Ŀ¼�����ûص���ʾĿ¼
	for (std::vector<CString>::iterator it = opts.FileMaskList.begin();it!=opts.FileMaskList.end();it++){
		sFullMask = opts.sBaseFolder + *it;
		bFound = finder.FindFile(sFullMask);
		while ((bFound) && (!_bStopSearch))
		{
			bFound = finder.FindNextFile();
			if (!finder.IsDots())
			{
				// �������
				bValidFile = TRUE;
				if (opts.dwOptionsFlags & FIND_ATTRIBUTES)
				{
					bValidFile = finder.MatchesMask(opts.dwFileAttributes);
				}

				if (bValidFile && (opts.dwOptionsFlags & FIND_SIZE))
				{
					bValidFile = ((opts.nMinSize <= finder.GetLength()) &&
								(opts.nMaxSize >= finder.GetLength()));
				}

				if (bValidFile && (opts.dwOptionsFlags & FIND_DATEMODIFIED))
				{
					finder.GetLastWriteTime(timeFile);
					bValidFile = ((timeFile >= opts.tMinModified) && 
									(timeFile <= opts.tMaxModified));
				}

				if (bValidFile && (opts.dwOptionsFlags & FIND_DATECREATED))
				{
					finder.GetCreationTime(timeFile);
					bValidFile = ((timeFile >= opts.tMinCreated) && 
									(timeFile <= opts.tMaxCreated));
				}

				if (bValidFile && (opts.dwOptionsFlags & FIND_DATEACCESSED))
				{
					finder.GetLastAccessTime(timeFile);
					bValidFile = ((timeFile >= opts.tMinAccessed) && 
									(timeFile <= opts.tMaxAccessed));
				}

				if (bValidFile && (opts.dwOptionsFlags & FIND_TEXT))
				{
					bValidFile = FindTextInFile(finder.GetFilePath(), opts.sFindText);
				}

				// ����������еĲ�������
				if (bValidFile)
				{
					CString sName = finder.GetFilePath();
					if (finder.IsDirectory()) sName += "\\";
					_aFilesFound.Add(sName);//����ҵ���·��
				}

				// ���ûص�����
				if (_pFileFinderProc != NULL)
					_pFileFinderProc(this, bValidFile ? FF_FOUND : FF_DISCARDED, _pCustomParam);//�ļ��ҵ���û�ҵ���״̬��ʾ
			}
		}
	}

	return GetFileCount();//�����ҵ����ļ���Ŀ
}


BOOL CFileFinder::FindTextInFile(LPCTSTR szFile, LPCTSTR szText)
{
	if ((szText == NULL) || (szText == "")) return FALSE;
	
	//txt
	CFile file;

	if (!file.Open(szFile, CFile::modeRead)) return FALSE;

	const UINT	nCMinBufSize = 128;
	CString		sText;
	CString		sFindText(szText);
	UINT		nFindTextLen = sFindText.GetLength();
	UINT		nBufSize = 128;
	UINT		nReadSize;
	UINT		nCharRead;
	LPSTR		pTextBuf;
	BOOL		bTextFound;
	int			nLoopCount = 0;

	if ((2 * nFindTextLen) > nCMinBufSize)
		nBufSize = (2 * nFindTextLen);

	nReadSize = nBufSize - nFindTextLen;
	sFindText.MakeUpper();

	do
	{
		pTextBuf = sText.GetBuffer(nBufSize);

		if (pTextBuf[0] == 0x0) 
			memset(pTextBuf, ' ', nFindTextLen);
		else
			memcpy(pTextBuf, pTextBuf + (nBufSize - nFindTextLen), nFindTextLen);

		nCharRead = file.Read(pTextBuf + nFindTextLen, nReadSize);
		sText.ReleaseBuffer(nFindTextLen + nCharRead);
		sText.Remove('\0');
		sText.MakeUpper();
		bTextFound = (sText.Find(sFindText) != -1);

		// ���ûص�����
		if (_pFileFinderProc != NULL)
		{
			nLoopCount++;
			if (nLoopCount == 10)
			{
				nLoopCount = 0;
				_pFileFinderProc(this, FF_FINDTEXT, _pCustomParam);
			}
		}
	} 
	while ((nCharRead == nReadSize) && !bTextFound);

	file.Close();

	return bTextFound;
}

int CFileFinder::GetFileCount()
{
	return _aFilesFound.GetSize();
}

int CFileFinder::FindPathItem(LPCTSTR szPath)
{
	bool	bFound;
	int		nIndex;

	for (nIndex = 0; nIndex < _aFilesFound.GetSize(); nIndex++)
	{
		bFound = (_aFilesFound[nIndex].CompareNoCase(szPath) == 0);
		if (bFound) break;
	}

	return (bFound ? nIndex : -1);
}

CPath CFileFinder::GetFilePath(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= GetFileCount())) return "";

	return _aFilesFound[nIndex];
}

LPCTSTR	CFileFinder::GetSearchingFolder()
{
	return _sSearchingFolder;
}

void CFileFinder::RemoveAt(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= GetFileCount())) return;

	_aFilesFound.RemoveAt(nIndex);
}

void CFileFinder::RemoveAll()
{
	if (GetFileCount() > 0) _aFilesFound.RemoveAll();
}

void CFileFinder::SetCallback(FILEFINDERPROC pFileFinderProc, void *pCustomParam)
{
	_pFileFinderProc = pFileFinderProc;
	_pCustomParam = pCustomParam;
}

void CFileFinder::StopSearch()
{
	_bStopSearch = true;
}

