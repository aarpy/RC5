// PTLengthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RC5Test.h"
#include "PTLengthDlg.h"
#include ".\ptlengthdlg.h"


// CPTLengthDlg dialog

IMPLEMENT_DYNAMIC(CPTLengthDlg, CDialog)
CPTLengthDlg::CPTLengthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPTLengthDlg::IDD, pParent)
	, m_nLength(0)
{
}

CPTLengthDlg::~CPTLengthDlg()
{
}

void CPTLengthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_NEW_LENGTH, m_nLength);
}


BEGIN_MESSAGE_MAP(CPTLengthDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CPTLengthDlg message handlers

void CPTLengthDlg::OnBnClickedOk()
{
	if (!UpdateData())
	{
		return;
	}

	if (m_nLength >= 0)
	{
		OnOK();
	}
	else
	{
		AfxMessageBox(_T("Length should be 0 or positive number for secret key."));
		return;
	}
}
