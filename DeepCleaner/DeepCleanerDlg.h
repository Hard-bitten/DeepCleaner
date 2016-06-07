
// DeepCleanerDlg.h : 头文件
//

#pragma once
#include "FileFinder.h"
#include "LineColorListCtrl.h"
#define _WIN32_WINNT 0x0400
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
	CFileFinder	_finder;
	CString SHA1(CString &inData);
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
};
