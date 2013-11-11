#pragma once


// CRC5ParamsDlg dialog

class CRC5ParamsDlg : public CDialog
{
	DECLARE_DYNAMIC(CRC5ParamsDlg)

public:
	CRC5ParamsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRC5ParamsDlg();

// Dialog Data
	enum { IDD = IDD_RC5_PARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nWordSize;
	int m_nNoOfRounds;
	int m_nKeySize;
	afx_msg void OnBnClickedOk();
};
