#pragma once


// CPTLengthDlg dialog

class CPTLengthDlg : public CDialog
{
	DECLARE_DYNAMIC(CPTLengthDlg)

public:
	CPTLengthDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPTLengthDlg();

// Dialog Data
	enum { IDD = IDD_RC5_LENGTHS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nLength;
	afx_msg void OnBnClickedOk();
};
