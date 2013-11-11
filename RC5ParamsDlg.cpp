// RC5ParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RC5Test.h"
#include "RC5ParamsDlg.h"
#include ".\rc5paramsdlg.h"


// CRC5ParamsDlg dialog

IMPLEMENT_DYNAMIC(CRC5ParamsDlg, CDialog)
CRC5ParamsDlg::CRC5ParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRC5ParamsDlg::IDD, pParent)
	, m_nWordSize(32)
	, m_nNoOfRounds(12)
	, m_nKeySize(16)
{
}

CRC5ParamsDlg::~CRC5ParamsDlg()
{
}

void CRC5ParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_WORD_SIZE, m_nWordSize);
	DDX_Text(pDX, IDC_KEY_SIZE, m_nKeySize);
	DDX_Text(pDX, IDC_NUM_ROUNDS, m_nNoOfRounds);
}


BEGIN_MESSAGE_MAP(CRC5ParamsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CRC5ParamsDlg message handlers

void CRC5ParamsDlg::OnBnClickedOk()
{
	if (!UpdateData())
	{
		return;
	}

	if ((m_nWordSize == 16) || (m_nWordSize == 32) || (m_nWordSize == 64))
	{
		if ((m_nNoOfRounds > 0) && (m_nNoOfRounds <= 255))
		{
			if ((m_nKeySize > 0) && (m_nKeySize <= 255))
			{
	            OnOK();
			}
			else
			{
				AfxMessageBox(_T("Number of bytes in secret key should be from 0 to 255 (inclusive)."));
			}
		}
		else
		{
			AfxMessageBox(_T("Number of rounds should be from 0 to 255 (inclusive)."));
		}
	}
	else
	{
		AfxMessageBox(_T("Word size should be 16 or 32 or 64."));
		return;
	}
}
