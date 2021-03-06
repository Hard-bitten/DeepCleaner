
// DeepCleanerDlg.h : 头文件
//

#pragma once
#include "FileFinder.h"
#include "LineColorListCtrl.h"
#include <tchar.h>
#include <wincrypt.h>
// CDeepCleanerDlg 对话框

class CDeepCleanerDlg : public CDialogEx
{
// 构造
public:
	CDeepCleanerDlg(CWnd* pParent = NULL);	// 标准构造函数

	void	AddFileToList(LPCTSTR szFilename);
	CString	GetListFilename(int nIndex);
	int		FindInList(LPCTSTR szFilename);
	void	SetStatus(int nCount = 0, LPCTSTR szFolder = NULL);

// 对话框数据
	enum { IDD = IDD_DEEPCLEANER_DIALOG };
	CLineColorListCtrl 	m_list;
	CProgressCtrl m_pro;
	CString	m_sBaseFolder;
	CString	m_sOutFolder;
	CString	m_sFindText=_T("");
	int		m_nSearchType;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	static void FileFinderProc(CFileFinder *pFinder, DWORD dwCode, void *pCustomParam);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	afx_msg void doSearch();
	virtual void OnCancel();


private:
	bool		_bSearching;
	CStringArray mi;
	CFileFinder	_finder;
	CString SHA1(CString &inData);
	CString  miyao = "~#&AIE*(@a19GFd42$8$^3";
public:
	afx_msg void OnBnClickedSloveN();
	afx_msg void OnBnClickedSloveD();
	afx_msg void OnBnClickedSloveR();
	afx_msg void OnBnClickedSloveD2();
	afx_msg void OnBnClickedSloveD3();
	afx_msg void OnBnClickedExe();
	afx_msg void OnBnClickedBrowse2();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSearch2();
	afx_msg void OnBnClickedSloveR2();
	afx_msg void OnBnClickedSloveR3();
	afx_msg void OnBnClickedCrypto();
	CString GetRandString()

	{
		int max;
		CString shuzi;
		max = RAND_MAX;//rand()函数随机数的最大值
		int randnum = (int)(rand()*(8999999.00) / max + 1000000.00);
		shuzi.Format("%d", randnum);
		CString result = SHA1(shuzi);
		return result;
	}

};
