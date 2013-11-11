// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RC5Test.h"
#include "ProgressDlg.h"


// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)
CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
END_MESSAGE_MAP()


// CProgressDlg message handlers
void CProgressDlg::Display(CString szTitle, CWnd * pParentWnd)
{
	m_szTitle = szTitle;
	if (pParentWnd != NULL)
	{
		m_pParentWnd = pParentWnd;
	}

	if ((m_hWnd == NULL) || (!::IsWindow(m_hWnd)))
	{
		Create(IDD_PROGRESS);
	}
	SetWindowText((LPCTSTR) m_szTitle);
	CenterWindow();
	ShowWindow(SW_NORMAL);
}

void CProgressDlg::Display(UINT nID, CWnd * pParentWnd)
{
	CString szTitle;

	szTitle.LoadString(nID);
	Display(szTitle, pParentWnd);
}
