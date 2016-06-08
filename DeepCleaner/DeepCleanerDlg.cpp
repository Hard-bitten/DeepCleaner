
// DeepCleanerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DeepCleaner.h"
#include "DeepCleanerDlg.h"
#include "afxdialogex.h"
#include "path.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDeepCleanerDlg 对话框



CDeepCleanerDlg::CDeepCleanerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeepCleanerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDeepCleanerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_EDITROOT, m_sBaseFolder);
	DDX_Text(pDX, IDC_EDITOUT, m_sOutFolder);
	DDX_Control(pDX, IDC_PRO, m_pro);
}

BEGIN_MESSAGE_MAP(CDeepCleanerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BROWSE, &CDeepCleanerDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_SEARCH, &CDeepCleanerDlg::doSearch)
	ON_BN_CLICKED(IDC_SLOVE_D, &CDeepCleanerDlg::OnBnClickedSloveD)
	ON_BN_CLICKED(IDC_SLOVE_R, &CDeepCleanerDlg::OnBnClickedSloveR)
	ON_BN_CLICKED(IDC_SLOVE_D2, &CDeepCleanerDlg::OnBnClickedSloveD2)
	ON_BN_CLICKED(IDC_SLOVE_D3, &CDeepCleanerDlg::OnBnClickedSloveD3)
	ON_BN_CLICKED(IDC_EXE, &CDeepCleanerDlg::OnBnClickedExe)
	ON_BN_CLICKED(ID_BROWSE2, &CDeepCleanerDlg::OnBnClickedBrowse2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CDeepCleanerDlg::OnNMDblclkList)
	ON_BN_CLICKED(IDC_SEARCH2, &CDeepCleanerDlg::OnBnClickedSearch2)
	ON_BN_CLICKED(IDC_SLOVE_R3, &CDeepCleanerDlg::OnBnClickedSloveR3)
	ON_BN_CLICKED(IDC_CRYPTO, &CDeepCleanerDlg::OnBnClickedCrypto)
END_MESSAGE_MAP()



// CDeepCleanerDlg 消息处理程序

BOOL CDeepCleanerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_pro.SetRange(0, 100);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_list.InsertColumn(0, "文件名字", LVCFMT_LEFT, 130, 0);
	m_list.InsertColumn(1, "所在文件夹", LVCFMT_LEFT, 210, 1);
	m_list.InsertColumn(2, "大小", LVCFMT_LEFT, 40, 2);
	m_list.InsertColumn(3, "处理方式", LVCFMT_LEFT, 60, 3);

	_finder.SetCallback(FileFinderProc, this);
	
	try{
		CStdioFile config;
		CString oneLine;
		if(config.Open(_T("conf\\lib.config"), CFile::modeRead))
			while (config.ReadString(oneLine))
			{
				mi.Add(oneLine);
			}
		config.Close();
	}
	catch (CFileException* e)
	{
		if (e->m_cause == CFileException::accessDenied)
			AfxMessageBox("ERROR: Access Denied\n");
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDeepCleanerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDeepCleanerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDeepCleanerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDeepCleanerDlg::OnBnClickedBrowse()
{
	CString		sFolder;
	LPMALLOC	pMalloc;

	// Gets the Shell's default allocator
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;

		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;
		bi.lpszTitle = _T("Select a directory...");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;

		// This next call issues the dialog box.
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, pszBuffer))
			{
				// At this point pszBuffer contains the selected path
				sFolder = pszBuffer;
			}

			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free(pidl);
		}
		// Release the shell's allocator.
		pMalloc->Release();
	}

	GetDlgItem(IDC_EDITROOT)->SetWindowText(sFolder);
}


void CDeepCleanerDlg::doSearch()
{
	
	if (_bSearching)//如果上次查询正在进行，取消上次查询
	{
		_finder.StopSearch();
		return;
	}

	CFileFinder::CFindOpts	opts;

	UpdateData();

	// 设置 CFindOpts对象
	opts.sBaseFolder = m_sBaseFolder;
	opts.setMi(mi);
	opts.bSubfolders = true;//是否查找子目录
	opts.FindNormalFiles();//常规文件设置


	if (!m_sFindText.IsEmpty()) //是否要查询字符串
	{
		opts.FindText(m_sFindText);
	}

	// 需要查找文件
	if (m_nSearchType == 0)
	{
		m_list.DeleteAllItems();
		SetStatus();
	}

	_bSearching = true;
	GetDlgItem(IDC_SEARCH)->SetWindowText("停止检查");
	_finder.RemoveAll();
	_finder.Find(opts); //开始递归查找
	GetDlgItem(IDC_SEARCH)->SetWindowText("开始检查");
	_bSearching = false;

	SetStatus(_finder.GetFileCount());
}

//文件查找过程中的回调函数，用于查找文件中各种状态的显示
void CDeepCleanerDlg::FileFinderProc(CFileFinder *pFinder, DWORD dwCode, void *pCustomParam)
{
	CString			sText, sNewFile;
	MSG				msg;
	CDeepCleanerDlg	*pDlg = (CDeepCleanerDlg *)pCustomParam;
//	int				nListIndex;

	switch (dwCode)
	{
	case FF_FOUND://找到的是文件
		// 更新列表
		sNewFile = pFinder->GetFilePath(pFinder->GetFileCount() - 1);

	//	nListIndex = pDlg->FindInList(sNewFile);
	//	if (nListIndex == -1) pDlg->AddFileToList(sNewFile);
		pDlg->AddFileToList(sNewFile);

	case FF_FOLDER://找到的是文件夹
		pDlg->SetStatus(pFinder->GetFileCount(), pFinder->GetSearchingFolder());
	}

	// 处理消息，防止不响应
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CDeepCleanerDlg::SetStatus(int nCount, LPCTSTR szFolder)
{
	CString sStatus;

	if (szFolder != NULL)
		sStatus.Format("(%d) - %s", nCount, szFolder);
	else
		sStatus.Format("%d 个文件找到了", nCount);

	GetDlgItem(IDC_STATUS)->SetWindowText(sStatus);
}

void CDeepCleanerDlg::AddFileToList(LPCTSTR szFilename)
{
	int		nIndex;
	CPath	path(szFilename);
	__int64	nSize64;
	long	nSize;
	CString	sText;
	CTime	tModified;

	// File name
	nIndex = m_list.InsertItem(m_list.GetItemCount(), path.GetFileName(), 0);

	// File location
	m_list.SetItemText(nIndex, 1, path.GetLocation());

	// File size
	path.GetFileSize(nSize64);
	nSize = (long)(nSize64 / (__int64)1024);
	if (nSize < 10)
		sText.Format("%ld B", nSize64);
	else 
		sText.Format("%ld KB", nSize);

	m_list.SetItemText(nIndex, 2, sText);

	m_list.SetItemColor(nIndex, RGB(0, 0, 0), RGB(255, 255, 255));

}

CString	CDeepCleanerDlg::GetListFilename(int nIndex)
{
	return m_list.GetItemText(nIndex, 1) + m_list.GetItemText(nIndex, 0);
}

void CDeepCleanerDlg::OnCancel()
{
	if (!_bSearching) CDialog::OnCancel();
}

int	CDeepCleanerDlg::FindInList(LPCTSTR szFilename)
{
	int		nIndex;
	bool	bFound;

	for (nIndex = 0; nIndex < m_list.GetItemCount(); nIndex++)
	{
		bFound = (GetListFilename(nIndex).CompareNoCase(szFilename) == 0);
		if (bFound) break;
	}

	return (bFound ? nIndex : -1);
}




void CDeepCleanerDlg::OnBnClickedSloveD()
{
	int n = m_list.GetItemCount();
	for (int i = 0; i < n; i++){
		if (m_list.GetCheck(i)&&(m_list.GetItemText(i, 3) == "" || m_list.GetItemText(i, 3) != "删除"))
		{
			m_list.SetItemText(i, 3, "删除");
			m_list.SetItemColor(i, RGB(0, 0, 0), RGB(255, 0, 0));
		}
		m_list.SetCheck(i, 0);
	}
}


void CDeepCleanerDlg::OnBnClickedSloveR()
{
	int n = m_list.GetItemCount();
	for (int i = 0; i < n; i++){
		if (m_list.GetCheck(i) && (m_list.GetItemText(i, 3) == "" || m_list.GetItemText(i, 3) != "移动"))
		{
			m_list.SetItemText(i, 3, "移动");
			m_list.SetItemColor(i, RGB(0, 0, 0), RGB(255, 255, 0));
		}
		m_list.SetCheck(i, 0);
	}
}

void CDeepCleanerDlg::OnBnClickedSloveD2()
{
	int n = m_list.GetItemCount();
	for (int i = 0; i < n; i++){
		m_list.SetCheck(i, 1);
	}
}


void CDeepCleanerDlg::OnBnClickedSloveD3()
{
	int n = m_list.GetItemCount();
	for (int i = 0; i < n; i++){
		m_list.SetCheck(i, 0);
	}
}


void CDeepCleanerDlg::OnBnClickedExe()
{
	int n = m_list.GetItemCount();
	int sm=0,sd=0;
	for (int i = 0; i < n; i++){
		if (m_list.GetItemText(i, 3) == "移动")
			sm++;
		if (m_list.GetItemText(i, 3) == "删除")
			sd++;
	}
	m_pro.SetStep(100/(sm+sd));
	CString _filefrom;
	CString _fileout;
	CString sStatus;
	CString _hash;
	int _m=0, _d=0;
	CFile temp;
	CString _file;
	
	CString workfile;
	CStdioFile out;
	BOOL outopen =FALSE;
	if (out.Open(_T("out\\Crypto.out"), CFile::modeReadWrite | CFile::modeCreate))
		outopen = TRUE;
	if (outopen)
		out.WriteString(miyao);
	UpdateData(TRUE);
	for (int i = 0; i < n; i++){
		if (m_list.GetItemText(i, 3) == "移动"){
			_filefrom = m_list.GetItemText(i, 1);
			_hash = SHA1(_filefrom);
			_fileout = m_sOutFolder +"\\"+ _hash+"\\";

			if (!PathIsDirectory(_fileout)){
				if (CreateDirectory(_fileout, NULL)){
					_file = _fileout + "!来自.txt";
					temp.Open(_file, CFile::modeCreate |CFile::modeWrite| CFile::typeText);
					temp.Write(_filefrom, _filefrom.GetLength());
					temp.Close();
				}		
			}

			_filefrom += m_list.GetItemText(i, 0);	
			_fileout+=m_list.GetItemText(i, 0);
			TRY	{
				//if (CopyFile(_filefrom, _fileout,TRUE)){
				if (MoveFile(_filefrom, _fileout)){
					m_pro.StepIt();
					_m++;
					m_list.DeleteItem(i);
					n--;
					i--;
				}	
			}
			CATCH(CFileException, e)
			{
				if (e->m_cause == CFileException::accessDenied)
					AfxMessageBox("ERROR: Access Denied\n");
			}
			END_CATCH
		
		}
		else if (m_list.GetItemText(i, 3) == "删除")
		{
			_filefrom = (m_list.GetItemText(i, 1) + m_list.GetItemText(i, 0));
			TRY{
				if (DeleteFile(_filefrom)){
					m_pro.StepIt();
					_d++;
					m_list.DeleteItem(i);
					n--;
					i--;
				}
					
			}
			CATCH(CFileException, e)
			{
				if (e->m_cause == CFileException::accessDenied)
					AfxMessageBox("ERROR: Access Denied\n");
			}
			END_CATCH
		
		}
		else if (m_list.GetItemText(i, 3) == "加密"){

			try{
				BOOL CryptoFlag=FALSE;
				_filefrom = (m_list.GetItemText(i, 1) + m_list.GetItemText(i, 0));
				//加密文件

				if (outopen && CryptoFlag)
					out.WriteString(_filefrom);
			}
			catch (CFileException* e)
			{
				if (e->m_cause == CFileException::accessDenied)
					AfxMessageBox("ERROR: Access Denied\n");
			}
		}
	}
	out.Close();
	sStatus.Format("%d 个移动成功,%d 个删除成功，%d个失败！", _m,_d,(sm-_m+sd-_d));
	GetDlgItem(IDC_STATUS)->SetWindowText(sStatus);
}


void CDeepCleanerDlg::OnBnClickedBrowse2()
{
	CString		sFolder;
	LPMALLOC	pMalloc;

	// Gets the Shell's default allocator
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;

		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;
		bi.lpszTitle = _T("Select a directory...");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;

		// This next call issues the dialog box.
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, pszBuffer))
			{
				// At this point pszBuffer contains the selected path
				sFolder = pszBuffer;
			}

			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free(pidl);
		}
		// Release the shell's allocator.
		pMalloc->Release();
	}

	GetDlgItem(IDC_EDITOUT)->SetWindowText(sFolder);
}



CString CDeepCleanerDlg::SHA1(CString &inData)
{
	CString outData;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE bHash[32];
	DWORD dwHashLen = 20; // The SHA1 algorithm always returns 20 bytes. 
	DWORD cbContent = inData.GetLength() * sizeof(TCHAR);
	BYTE *pbContent = (BYTE *)(LPCTSTR)inData;


	if (!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		return _T("");
	if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
		return _T("");
	if (!CryptHashData(hHash, pbContent, cbContent, 0))
		return _T("");
	if (!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
		return _T("");

	CString tmp;
	for (unsigned int i = 0; i < dwHashLen; i += 4) {
		tmp.Format(_T("%02x%02x%02x%02x"), bHash[i], bHash[i + 1], bHash[i + 2], bHash[i + 3]);
		outData += tmp;
	}

	if (hHash)
		CryptDestroyHash(hHash);
	if (hProv)
		CryptReleaseContext(hProv, 0);

	return outData;
}

void CDeepCleanerDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CWaitCursor	waitCursor;
	int			nIndex = m_list.GetSelectionMark();

	ShellExecute(GetSafeHwnd(), NULL, GetListFilename(nIndex), NULL, NULL, SW_SHOW);

	*pResult = 0;
}

//全盘搜索
void CDeepCleanerDlg::OnBnClickedSearch2()
{

	if (_bSearching)//如果上次查询正在进行，取消上次查询
	{
		_finder.StopSearch();
		return;
	}

	CFileFinder::CFindOpts	opts;

	UpdateData();
	
	// 设置 CFindOpts对象
	//opts.sBaseFolder = m_sBaseFolder;

	opts.setMi(mi);
	opts.bSubfolders = true;//是否查找子目录
	opts.FindNormalFiles();//常规文件设置


	if (!m_sFindText.IsEmpty()) //是否要查询字符串
	{
		opts.FindText(m_sFindText);
	}

	// 需要查找文件
	if (m_nSearchType == 0)
	{
		m_list.DeleteAllItems();
		SetStatus();
	}

	_bSearching = true;
	GetDlgItem(IDC_SEARCH)->SetWindowText("停止检查");
	_finder.RemoveAll();
	_finder.Find(opts); //开始递归查找
	GetDlgItem(IDC_SEARCH)->SetWindowText("开始检查");
	_bSearching = false;

	SetStatus(_finder.GetFileCount());
}


void CDeepCleanerDlg::OnBnClickedSloveR3()
{
	//弹窗口 给密钥赋予初值

	//进行标记
	int n = m_list.GetItemCount();
	for (int i = 0; i < n; i++){
		if (m_list.GetCheck(i))
		{
			m_list.SetItemText(i, 3, "");
			m_list.SetItemColor(i, RGB(0, 0, 0), RGB(255, 255, 255));
		}
		m_list.SetCheck(i, 0);
	}
}


void CDeepCleanerDlg::OnBnClickedCrypto()
{
	int n = m_list.GetItemCount();
	for (int i = 0; i < n; i++){
		if (m_list.GetCheck(i))
		{
			m_list.SetItemText(i, 3, "加密");
			m_list.SetItemColor(i, RGB(0, 0, 0), RGB(255, 255, 255));
		}
		m_list.SetCheck(i, 0);
	}
}
