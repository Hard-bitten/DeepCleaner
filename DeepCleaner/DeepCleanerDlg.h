
// DeepCleanerDlg.h : ͷ�ļ�
//

#pragma once
#include "FileFinder.h"
#include "LineColorListCtrl.h"
#include <tchar.h>
#include <wincrypt.h>
// CDeepCleanerDlg �Ի���

class CDeepCleanerDlg : public CDialogEx
{
// ����
public:
	CDeepCleanerDlg(CWnd* pParent = NULL);	// ��׼���캯��

	void	AddFileToList(LPCTSTR szFilename);
	CString	GetListFilename(int nIndex);
	int		FindInList(LPCTSTR szFilename);
	void	SetStatus(int nCount = 0, LPCTSTR szFolder = NULL);

// �Ի�������
	enum { IDD = IDD_DEEPCLEANER_DIALOG };
	CLineColorListCtrl 	m_list;
	CProgressCtrl m_pro;
	CString	m_sBaseFolder;
	CString	m_sOutFolder;
	CString	m_sFindText=_T("");
	int		m_nSearchType;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	static void FileFinderProc(CFileFinder *pFinder, DWORD dwCode, void *pCustomParam);

	// ���ɵ���Ϣӳ�亯��
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
		max = RAND_MAX;//rand()��������������ֵ
		int randnum = (int)(rand()*(8999999.00) / max + 1000000.00);
		shuzi.Format("%d", randnum);
		CString result = SHA1(shuzi);
		return result;
	}

};
