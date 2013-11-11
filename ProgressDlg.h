#pragma once


// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	void Display(UINT nID, CWnd* pParentWnd);
	void Display(CString szTitle, CWnd* pParentWnd);
	virtual ~CProgressDlg();

// Dialog Data
	enum { IDD = IDD_PROGRESS };

protected:
	CString m_szTitle;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
