#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"

class CWordFrequencyAnalyzerApp : public CWinApp
{
public:
	CWordFrequencyAnalyzerApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CWordFrequencyAnalyzerApp theApp;
