
// DeepCleanerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DeepCleaner.h"
#include "DeepCleanerDlg.h"
#include "afxdialogex.h"
#include "path.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CDeepCleanerDlg �Ի���



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



// CDeepCleanerDlg ��Ϣ�������

BOOL CDeepCleanerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_pro.SetRange(0, 100);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_list.InsertColumn(0, "�ļ�����", LVCFMT_LEFT, 130, 0);
	m_list.InsertColumn(1, "�����ļ���", LVCFMT_LEFT, 210, 1);
	m_list.InsertColumn(2, "��С", LVCFMT_LEFT, 40, 2);
	m_list.InsertColumn(3, "����ʽ", LVCFMT_LEFT, 60, 3);

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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDeepCleanerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	
	if (_bSearching)//����ϴβ�ѯ���ڽ��У�ȡ���ϴβ�ѯ
	{
		_finder.StopSearch();
		return;
	}

	CFileFinder::CFindOpts	opts;

	UpdateData();

	// ���� CFindOpts����
	opts.sBaseFolder = m_sBaseFolder;
	opts.setMi(mi);
	opts.bSubfolders = true;//�Ƿ������Ŀ¼
	opts.FindNormalFiles();//�����ļ�����


	if (!m_sFindText.IsEmpty()) //�Ƿ�Ҫ��ѯ�ַ���
	{
		opts.FindText(m_sFindText);
	}

	// ��Ҫ�����ļ�
	if (m_nSearchType == 0)
	{
		m_list.DeleteAllItems();
		SetStatus();
	}

	_bSearching = true;
	GetDlgItem(IDC_SEARCH)->SetWindowText("ֹͣ���");
	_finder.RemoveAll();
	_finder.Find(opts); //��ʼ�ݹ����
	GetDlgItem(IDC_SEARCH)->SetWindowText("��ʼ���");
	_bSearching = false;

	SetStatus(_finder.GetFileCount());
}

//�ļ����ҹ����еĻص����������ڲ����ļ��и���״̬����ʾ
void CDeepCleanerDlg::FileFinderProc(CFileFinder *pFinder, DWORD dwCode, void *pCustomParam)
{
	CString			sText, sNewFile;
	MSG				msg;
	CDeepCleanerDlg	*pDlg = (CDeepCleanerDlg *)pCustomParam;
//	int				nListIndex;

	switch (dwCode)
	{
	case FF_FOUND://�ҵ������ļ�
		// �����б�
		sNewFile = pFinder->GetFilePath(pFinder->GetFileCount() - 1);

	//	nListIndex = pDlg->FindInList(sNewFile);
	//	if (nListIndex == -1) pDlg->AddFileToList(sNewFile);
		pDlg->AddFileToList(sNewFile);

	case FF_FOLDER://�ҵ������ļ���
		pDlg->SetStatus(pFinder->GetFileCount(), pFinder->GetSearchingFolder());
	}

	// ������Ϣ����ֹ����Ӧ
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
		sStatus.Format("%d ���ļ��ҵ���", nCount);

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
		if (m_list.GetCheck(i)&&(m_list.GetItemText(i, 3) == "" || m_list.GetItemText(i, 3) != "ɾ��"))
		{
			m_list.SetItemText(i, 3, "ɾ��");
			m_list.SetItemColor(i, RGB(0, 0, 0), RGB(255, 0, 0));
		}
		m_list.SetCheck(i, 0);
	}
}


void CDeepCleanerDlg::OnBnClickedSloveR()
{
	int n = m_list.GetItemCount();
	for (int i = 0; i < n; i++){
		if (m_list.GetCheck(i) && (m_list.GetItemText(i, 3) == "" || m_list.GetItemText(i, 3) != "�ƶ�"))
		{
			m_list.SetItemText(i, 3, "�ƶ�");
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
		if (m_list.GetItemText(i, 3) == "�ƶ�")
			sm++;
		if (m_list.GetItemText(i, 3) == "ɾ��")
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
		if (m_list.GetItemText(i, 3) == "�ƶ�"){
			_filefrom = m_list.GetItemText(i, 1);
			_hash = SHA1(_filefrom);
			_fileout = m_sOutFolder +"\\"+ _hash+"\\";

			if (!PathIsDirectory(_fileout)){
				if (CreateDirectory(_fileout, NULL)){
					_file = _fileout + "!����.txt";
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
		else if (m_list.GetItemText(i, 3) == "ɾ��")
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
		else if (m_list.GetItemText(i, 3) == "����"){

			try{
				BOOL CryptoFlag=FALSE;
				_filefrom = (m_list.GetItemText(i, 1) + m_list.GetItemText(i, 0));
				//�����ļ�

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
	sStatus.Format("%d ���ƶ��ɹ�,%d ��ɾ���ɹ���%d��ʧ�ܣ�", _m,_d,(sm-_m+sd-_d));
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

//ȫ������
void CDeepCleanerDlg::OnBnClickedSearch2()
{

	if (_bSearching)//����ϴβ�ѯ���ڽ��У�ȡ���ϴβ�ѯ
	{
		_finder.StopSearch();
		return;
	}

	CFileFinder::CFindOpts	opts;

	UpdateData();
	
	// ���� CFindOpts����
	//opts.sBaseFolder = m_sBaseFolder;

	opts.setMi(mi);
	opts.bSubfolders = true;//�Ƿ������Ŀ¼
	opts.FindNormalFiles();//�����ļ�����


	if (!m_sFindText.IsEmpty()) //�Ƿ�Ҫ��ѯ�ַ���
	{
		opts.FindText(m_sFindText);
	}

	// ��Ҫ�����ļ�
	if (m_nSearchType == 0)
	{
		m_list.DeleteAllItems();
		SetStatus();
	}

	_bSearching = true;
	GetDlgItem(IDC_SEARCH)->SetWindowText("ֹͣ���");
	_finder.RemoveAll();
	_finder.Find(opts); //��ʼ�ݹ����
	GetDlgItem(IDC_SEARCH)->SetWindowText("��ʼ���");
	_bSearching = false;

	SetStatus(_finder.GetFileCount());
}


void CDeepCleanerDlg::OnBnClickedSloveR3()
{
	//������ ����Կ�����ֵ

	//���б��
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
			m_list.SetItemText(i, 3, "����");
			m_list.SetItemColor(i, RGB(0, 0, 0), RGB(255, 255, 255));
		}
		m_list.SetCheck(i, 0);
	}
}
