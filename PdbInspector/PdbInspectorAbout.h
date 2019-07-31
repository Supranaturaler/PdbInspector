// PdbInspectorAbout dialog used for App About
class PdbInspectorAbout : public CDialog
{
public:
	PdbInspectorAbout() : CDialog(PdbInspectorAbout::IDD) {}
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange*);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg		void OnNMClickUrl(NMHDR*, LRESULT*);
	CLinkCtrl	m_linkUrl;
};