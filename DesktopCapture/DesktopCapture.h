
// DeskTopCapture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDesktopCaptureApp:
// �йش����ʵ�֣������ DeskTopCapture.cpp
//

class CDesktopCaptureApp : public CWinApp
{
public:
	CDesktopCaptureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDesktopCaptureApp theApp;