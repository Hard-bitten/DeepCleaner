
// DeepCleaner.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDeepCleanerApp: 
// �йش����ʵ�֣������ DeepCleaner.cpp
//

class CDeepCleanerApp : public CWinApp
{
public:
	CDeepCleanerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDeepCleanerApp theApp;