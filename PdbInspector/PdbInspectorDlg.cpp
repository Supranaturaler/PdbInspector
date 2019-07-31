//	------------------------------------------------------------------------------------------------
//	File name:		PdbInspectorDlg.cpp
//	Author:			Marc Ochsenmeier
//	Email:			info@winitor.net
//	Web:			www.winitor.net - 
//	Updated:		27.06.2013
//
//	Description:	An MFC UI Application (PDB Inspector.exe) that consumes our PdbParser.dll
//					to inspect the content of PDB files.
//
//	------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "PdbParser.h"
using namespace PdbParser;

#include "PdbInspector.h"
#include "PdbInspectorDlg.h"
#include "PdbInspectorAbout.h"
#include "afxcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	------------------------------------------------------------------------------------------------
//	UI Messages
//	------------------------------------------------------------------------------------------------
const wchar_t*		UI_TEXT_BUTTON_CLOSE			= L"Close";
const wchar_t*		UI_TEXT_BUTTON_LOAD				= L"Load";
const wchar_t*		UI_TEXT_NO_FILE_LOADED			= L"No file loaded";
const wchar_t*		UI_TEXT_NO_MODULE_FOUND			= L"No module found";
const wchar_t*		UI_TEXT_SELECT_MODULE			= L"Please select a file in the Modules list";
const wchar_t*		UI_TEXT_CANNOT_LOAD_FILE		= L"Cannot load '%s'";
const wchar_t*		UI_TEXT_MODULES					= L"Modules";
const wchar_t*		UI_TEXT_SOURCES					= L"Source files";
const wchar_t*		UI_TEXT_NO_PDB_FILE_LOADED		= L"...";
const wchar_t*		UI_TEXT_STRIPPED_SYMBOLS		= L"Public symbols are available";
const wchar_t*		UI_TEXT_UNSTRIPPED_SYMBOLS		= L"Public and private symbols are available";
const wchar_t*		UI_TEXT_DOTS					= L"...";
const wchar_t*		UI_TEXT_CHECKSUM_TYPE_NONE		= L"None";
const wchar_t*		UI_TEXT_CHECKSUM_TYPE_MD5		= L"MD5";
const wchar_t*		UI_TEXT_CHECKSUM_TYPE_SHA1		= L"SHA1";
const wchar_t*		UI_TEXT_CHECKSUM_TYPE_UNKNOWN	= L"Unknown";

//	------------------------------------------------------------------------------------------------
// PdbInspectorDlg dialog
//	------------------------------------------------------------------------------------------------
PdbInspectorDlg::PdbInspectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PdbInspectorDlg::IDD, pParent)
{
	m_pIPdbParser	 = NULL;
	m_pIPdbfile		= NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
PdbInspectorDlg::~PdbInspectorDlg()
{
	//	Shutdown our PDB parser engine
	IPdbParserFactory::Destroy(); 
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PDB_GUID, m_sPdbGuid);
	DDX_Text(pDX, IDC_STATIC_STRIPPED, m_sStrippedMsg);
	DDX_Control(pDX, IDC_TREE_PDB, m_treeModules);
	DDX_Control(pDX, IDC_BTN_PDB_LOAD, m_btnLoadPdbFile);
	DDX_Control(pDX, IDC_LIST_SOURCES, m_listSources);
	DDX_Control(pDX, IDC_STATIC_STRIPPED_ICO, m_staticStippedIcon);
	DDX_Text(pDX, IDC_STATIC_GROUP_MODULES, m_sModules);
	DDX_Text(pDX, IDC_STATIC_GROOUP_SOURCES, m_sSources);
	DDX_Text(pDX, IDC_STATIC_COMPILER_NAME, m_compilerName);
	DDX_Text(pDX, IDC_STATIC_BUILD_NBR, m_buildNbr);
	DDX_Text(pDX, IDC_STATIC_CHECKSUM_TYPE, m_checksumType);
	DDX_Text(pDX, IDC_STATIC_CHECKSUM_VALUE, m_checksumValue);
}

//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(PdbInspectorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_PDB_LOAD, &OnBnClickedBtnPdbLoad)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PDB, &OnTreeModuleSelectionChanged)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_LIST_SOURCES, &OnSourceFileChanged)
END_MESSAGE_MAP()

//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
BOOL PdbInspectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	//	This Dialog accepts Drag and drop
	DragAcceptFiles();

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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//	Instantiate our PDB parser engine
	m_pIPdbParser = IPdbParserFactory::Create(); 

	//	Enable the Load button only when the PDB engine has been successfully started.
	m_btnLoadPdbFile.EnableWindow(m_pIPdbParser?TRUE:FALSE);
	InitControls();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		PdbInspectorAbout dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
HCURSOR PdbInspectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::OnBnClickedBtnPdbLoad()
{
	if( m_pIPdbfile )
	{
		InitControls();
		m_pIPdbfile = NULL;
	}
	else
	{
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY , L"PDB file (*.pdb)|*.pdb", this, 0, TRUE);
		if( dlg.DoModal()==IDOK )
		{
			if( LoadPdbFile( dlg.GetPathName()) )
			{
				m_btnLoadPdbFile.SetWindowText( UI_TEXT_BUTTON_CLOSE );
			}
			else
			{
				InitControls();
			}
		}
	}
	UpdateData( false);
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::InitControls()
{
	//	Tree
	m_treeModules.DeleteAllItems();
	m_treeModules.InsertItem( UI_TEXT_NO_FILE_LOADED );

	//	other controls
	m_listSources.ResetContent();
	m_listSources.AddString( UI_TEXT_NO_FILE_LOADED );
	m_sStrippedMsg		= "";
	m_sPdbGuid			= UI_TEXT_NO_PDB_FILE_LOADED;
	m_sModules			= UI_TEXT_MODULES;
	m_sSources			= UI_TEXT_SOURCES;
	m_compilerName		= UI_TEXT_DOTS;
	m_buildNbr			= UI_TEXT_DOTS;
	m_checksumType		= UI_TEXT_DOTS;
	m_checksumValue		= UI_TEXT_DOTS;

	m_btnLoadPdbFile.SetWindowText( UI_TEXT_BUTTON_LOAD );
	m_staticStippedIcon.ShowWindow( SW_HIDE );
	UpdateData( false );
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
bool PdbInspectorDlg::LoadPdbFile( const CString& sPdbFile )
{
	bool bRet = false;
	
	try
	{
		if( m_pIPdbParser )
		{
			m_pIPdbfile = m_pIPdbParser->OpenFile((wstring)sPdbFile);
			if( m_pIPdbfile )
			{
				m_treeModules.DeleteAllItems();
				bRet = true;

				//	Root
				HTREEITEM hItemRoot = m_treeModules.InsertItem( sPdbFile );

				//	Retrieve the file GUID
				LPOLESTR lpGuid = NULL;
				HRESULT hr = StringFromCLSID( m_pIPdbfile->GetGuid(), &lpGuid );
				if( hr==S_OK )
				{
					m_sPdbGuid = lpGuid;
					CoTaskMemFree(lpGuid);
				}

				//	Check whether the PDB file has been stripped.
				bool bStripped = m_pIPdbfile->IsStripped();

				//	Init Sources list message
				m_listSources.ResetContent();
				m_listSources.AddString( UI_TEXT_SELECT_MODULE );

				//	Update UI consequently
				m_staticStippedIcon.ShowWindow(SW_NORMAL);
				m_staticStippedIcon.UpdateWindow();
				HICON hIcon = ::LoadIcon( 
					AfxGetApp()->m_hInstance, 
					bStripped?MAKEINTRESOURCE(IDI_ICON_RED):MAKEINTRESOURCE(IDI_ICON_GREEN));

				CRect rect;
				m_staticStippedIcon.GetClientRect(&rect);
				CDC* pDC = m_staticStippedIcon.GetDC();
				DrawIcon(pDC->m_hDC, rect.left, rect.top,hIcon);
				ReleaseDC(pDC);
				m_sStrippedMsg = bStripped?UI_TEXT_STRIPPED_SYMBOLS:UI_TEXT_UNSTRIPPED_SYMBOLS;

				//	Retrieve the Modules.
				vector<IPdbModule*> vModules = m_pIPdbfile->GetModules();
				if( vModules.size() )
				{
					//	Show number of Compilands.
					CString s;
					s.Format(L" %i", vModules.size());
					m_sModules = UI_TEXT_MODULES + s;
					
					//	Populate UI with the Compilands.
					vector<IPdbModule*>::iterator it = vModules.begin();
					for( ;it!=vModules.end();it++)
					{
						IPdbModule* module = *it;
						HTREEITEM hItem = m_treeModules.InsertItem( module->GetName().c_str(), hItemRoot );
						m_treeModules.SetItemData( hItem, (DWORD_PTR) module);
					}
					m_treeModules.Expand(hItemRoot, TVE_EXPAND);
				}
				else
				{
					m_treeModules.InsertItem( UI_TEXT_NO_MODULE_FOUND, hItemRoot );
					m_treeModules.Expand( hItemRoot, TVE_EXPAND );
				}
			}
			else
			{
				//	File cannot be loaded!
				CString sMsg;
				sMsg.Format( UI_TEXT_CANNOT_LOAD_FILE, sPdbFile);
				MessageBox( sMsg, L"Error", MB_ICONERROR);
			}
			UpdateData( false );
		}
	}
	catch(...)
	{
		bRet = false;
	}
	return bRet;
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::ShowModuleSources( IPdbModule* module )
{
	//	Clear some UI elements
	m_listSources.ResetContent();
	m_listSources.AddString( UI_TEXT_SELECT_MODULE );
	m_checksumType		= UI_TEXT_DOTS;
	m_checksumValue		= UI_TEXT_DOTS;

	UpdateData( false );

	if( m_pIPdbfile && module )
	{
		std::vector< IPdbSourceFile* > vectorSources = module->GetSourceFiles();
		size_t iSize = vectorSources.size();
		if( iSize )
		{
			//	Show their numbers
			CString sNbr;
			sNbr.Format( L" %i", vectorSources.size() );
			m_sSources = UI_TEXT_SOURCES + sNbr;
		}

		//	Clear Sources UI before adding items
		m_listSources.ResetContent();

		int iPos = 0;
		std::vector<IPdbSourceFile*>::iterator it = vectorSources.begin();
		for( ;it!=vectorSources.end(); it++)
		{
			IPdbSourceFile* source = *it;
			m_listSources.InsertString( iPos, source->GetSourceFileName().c_str() );

			//	Populate list of sources
			m_listSources.SetItemDataPtr( iPos, (void*)source );
		}

		//	Show first item in the Sources list
		if( m_listSources.GetCount()>0 )
		{
			m_listSources.SetCurSel( 0 );
			OnSourceFileChanged();
		}
	}
	UpdateData( false );
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::OnTreeModuleSelectionChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_treeModules.GetSelectedItem();
	IPdbModule* module = (IPdbModule*)m_treeModules.GetItemData(hItem);
	if( module )
	{
		//	Retrieve the Module's details
		IPdbModuleDetails* details = module->GetModuleDetails();
		if( details )
		{
			std::wstring s = details->GetCompilerName();
			m_buildNbr = details->GetBackEndBuildNumber().c_str();
			//PdbTargetCPU cpu = details->GetTargetCPU();
			m_compilerName = s.c_str();
			UpdateData( false );
		}

		//	Show the source of the selected module
		ShowModuleSources( module );
	}
	else
	{
		m_listSources.ResetContent();
		m_listSources.AddString( UI_TEXT_SELECT_MODULE );
		m_sSources = UI_TEXT_SOURCES;
		UpdateData( false );
	}
	*pResult = 0;
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::OnDropFiles(HDROP hDropInfo)
{
	//	Retrieve the files dropped
	vector<CString> vFiles;
	int nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	for( int i=0; i<nFiles; i++)
	{
		//	Test whether file(s) or directory has been dropped!
		CString s;
		LPTSTR pFileName = s.GetBufferSetLength(_MAX_PATH);
		::DragQueryFile(hDropInfo, i, pFileName, _MAX_PATH);
		vFiles.push_back(s);
		s.ReleaseBuffer();
	}
	::DragFinish(hDropInfo);

	if( vFiles.size()>0)
	{
		//	Open the first (valid) file
		for( int i=0; i<nFiles; i++)
		{
			CString sFilePath = vFiles.at(i);
			if( IsFileValid(sFilePath)==true )
			{
				if( LoadPdbFile( sFilePath) )
				{
					m_btnLoadPdbFile.SetWindowText( UI_TEXT_BUTTON_CLOSE );
				}
				else
				{
					InitControls();
				}
				UpdateData( false );
				break;
			}
		}
	}

	//	Default processing.
	CDialog::OnDropFiles(hDropInfo);
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
void PdbInspectorDlg::OnSourceFileChanged()
{
	m_checksumType		= UI_TEXT_DOTS;
	m_checksumValue		= UI_TEXT_DOTS;
	UpdateData( false );

	int iSel = m_listSources.GetCurSel();
    if( iSel != -1 )
    {
		wchar_t buffer[ _MAX_PATH ] = { 0 };
		int size = m_listSources.GetText( iSel, (LPTSTR) &buffer );
		if( size!=0 )
		{
			std::wstring entry;
			entry.assign( buffer, size );
			IPdbSourceFile* source = (IPdbSourceFile*)m_listSources.GetItemDataPtr( iSel );
			if( source )
			{
				wstring file =	source->GetSourceFileName();
				//m_sSourceUID.Format( L"%i", source->GetUniqueId() );

				//	Get CheckSum
				BYTE buffer[ _MAX_PATH ] = { 0 };
				DWORD size = sizeof(buffer);
				CheckSumType type = source->GetCheckSumType();
				switch( type )
				{
				case CheckSumType_None:
					m_checksumType = UI_TEXT_CHECKSUM_TYPE_NONE;
					break;

				case CheckSumType_MD5:
					m_checksumType = UI_TEXT_CHECKSUM_TYPE_MD5;
					break;

				case CheckSumType_SHA1:
					m_checksumType = UI_TEXT_CHECKSUM_TYPE_SHA1;
					break;

				default:
					m_checksumType = UI_TEXT_CHECKSUM_TYPE_UNKNOWN;
					break;
				}

				const char* p = source->GetCheckSum();
				if( p )
				{
					m_checksumValue = p;
				}
				else
				{
					//m_checksumValue = 
				}
				UpdateData( false );
			}
		}
    }
}
//	------------------------------------------------------------------------------------------------
//	------------------------------------------------------------------------------------------------
bool PdbInspectorDlg::IsFileValid( const CString& file )
{
	bool bRet = true;
	return bRet;
}