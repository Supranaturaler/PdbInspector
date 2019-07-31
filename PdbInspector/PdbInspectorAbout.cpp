//	------------------------------------------------------------------------------------------------
//	File name:		PdbInspectorAbout.cpp
//	Author:			Marc Ochsenmeier
//	Email:			info@winitor.net
//	Web:			www.winitor.net - 
//	Updated:		27.06.2013
//
//	Description:	An MFC UI Application (PdbInspector.exe) that consumes our PdbParser.dll
//					to inspect the content of PDB files.
//
//	------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "PdbInspector.h"
#include "PdbInspectorAbout.h"
#include "afxcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP( PdbInspectorAbout, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_URL, &OnNMClickUrl)
END_MESSAGE_MAP()

//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorAbout::OnNMClickUrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShellExecuteW(NULL, L"open", L"http://www.winitor.net", NULL, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}

void PdbInspectorAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_URL, m_linkUrl);
}