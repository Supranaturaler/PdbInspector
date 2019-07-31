// PdbInspectorDlg.h : header file

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// PdbInspectorDlg dialog
class PdbInspectorDlg : public CDialog
{
public:
	PdbInspectorDlg(CWnd* pParent = NULL);	
	virtual ~PdbInspectorDlg();

	enum { IDD = IDD_PDBINSPECTORMFC_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange*);

protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnPdbLoad();
	afx_msg void OnTreeModuleSelectionChanged(NMHDR*, LRESULT*);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSourceFileChanged();
	DECLARE_MESSAGE_MAP()

private:
	void	InitControls();
	bool	IsFileValid( const CString& );
	bool	LoadPdbFile( const CString& );
	void	ShowModuleSources( IPdbModule* );

	HICON		m_hIcon;
	
	//	Module related UI
	CTreeCtrl	m_treeModules;
	CString		m_sModules;
	CString		m_sPdbGuid;
	CString		m_sStrippedMsg;
	CString		m_compilerName;
	CString		m_buildNbr;
	CString		m_checksumType;
	CString		m_checksumValue;

	//	Sources related UI
	CString		m_sSources;

	CButton		m_btnLoadPdbFile;
	CListBox	m_listSources;
	CStatic		m_staticStippedIcon;
	
	//	Our PDB parser engine interfaces.
	IPdbParser* m_pIPdbParser;
	IPdbFile*	m_pIPdbfile;
};
