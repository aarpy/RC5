// RC5TestView.cpp : implementation of the CRC5TestView class
//

#include "stdafx.h"
#include "RC5Test.h"

#include "RC5TestDoc.h"
#include "RC5TestView.h"

#include "ProgressDlg.h"
#include "PTLengthDlg.h"
#include "RC5ParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEMO_MAX_GRAPHS 16

typedef struct _tagRC5Params
{
	UINT m_nWordSize;
	UINT m_nNoOfRounds;
	UINT m_nKeySize;
} RC5_PARAMS;

const static RC5_PARAMS kRC5DefaultParams[] = 
{
	{ 16, 12, 16},
	{ 32, 12, 16},
	{ 32, 12, 32},
	{ 64, 12, 16}
};

const static UINT kRC5TestLengths[] = {10000, 1000, 100, 10};

static COLORREF g_DefaultGraphColors[DEMO_MAX_GRAPHS] =
{
	RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), 
	RGB(255, 255, 0), RGB(0, 255, 255), 32768, 128, 
	8388736, 8421440, 12615808, 8421504, 33023, 
	16711935, 12632256, 32896, RGB(0, 0, 0)
};

// CRC5TestView

IMPLEMENT_DYNCREATE(CRC5TestView, CFormView)

BEGIN_MESSAGE_MAP(CRC5TestView, CFormView)
	ON_BN_CLICKED(IDC_BN_START, OnBnClickedBnStart)
	ON_NOTIFY(NM_RCLICK, IDC_PARAMS_LIST, OnNMRclickParamsList)
	ON_NOTIFY(NM_RCLICK, IDC_PLAIN_TEXT_LENGTHS, OnNMRclickPlainTextLengths)
	ON_COMMAND(ID_FILE_PRINTPREVIEW, OnFilePrintpreview)
	ON_COMMAND(ID_FILE_LOADPLAINTEXTFILE, OnFileLoadplaintextfile)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINTPREVIEW, OnUpdateFilePrintpreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOADPLAINTEXTFILE, OnUpdateFileLoadplaintextfile)
	ON_COMMAND(ID_EDIT_ADDNEWWORDSET, OnEditAddnewwordset)
	ON_COMMAND(ID_EDIT_ADDNEWPLAINTEXTLENGTH, OnEditAddnewplaintextlength)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDNEWWORDSET, OnUpdateEditAddnewwordset)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDNEWPLAINTEXTLENGTH, OnUpdateEditAddnewplaintextlength)
END_MESSAGE_MAP()

// CRC5TestView construction/destruction

CRC5TestView::CRC5TestView()
	: CFormView(CRC5TestView::IDD)
	, m_szPlainText(_T(""))
	, m_szLog(_T(""))
{
	m_fRC5Started = FALSE;
    m_nKeyGraphCount = 0;
	m_nPerfGraphCount = 0;
	m_szSecretKey = _T("ABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUVwxyzABCDEFghikjlmnopQRSTUV");
	m_szPlainText = _T("");
	m_nMaxPlainTextLen = 0;
	m_nMaxKeyLen = 0;
	m_dKeyMaxY = 100.0;
	m_dPrefMaxY = 100.0;
}

CRC5TestView::~CRC5TestView()
{
}

void CRC5TestView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PARAMS_LIST, m_ParamsListCtrl);
	DDX_Control(pDX, IDC_PLAIN_TEXT_LENGTHS, m_PlainTextLenListCtrl);
	DDX_Control(pDX, IDC_SECRET_KEY, m_SecretKeyCtrl);
	DDX_Control(pDX, IDC_PLAIN_TEXT, m_PlainTextCtrl);
	DDX_Control(pDX, IDC_BN_START, m_bnStart);
	DDX_Control(pDX, IDC_GRAPH_CTRL, m_KeyGraphStaticWnd);
	DDX_Control(pDX, IDC_PERF_GRAPH_CTRL, m_PrefGraphStaticWnd);

	DDX_Text(pDX, IDC_SECRET_KEY, m_szSecretKey);
	DDX_Text(pDX, IDC_PLAIN_TEXT, m_szPlainText);
	DDX_Text(pDX, IDC_LOG_TEXT, m_szLog);
}

BOOL CRC5TestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CRC5TestView::OnInitialUpdate()
{
	int nIndex;
	int nItem;
	CRect rect;
	CString szItem;

	CFormView::OnInitialUpdate();
	ResizeParentToFit();

	m_KeyGraphStaticWnd.GetClientRect(rect);
	m_KeyGraphStaticWnd.EnableWindow(FALSE);
	m_KeyGraphWnd.Create(_T("RC5 Key Graph"), rect, &m_KeyGraphStaticWnd, 11000);

	m_PrefGraphStaticWnd.GetClientRect(rect);
	m_PrefGraphStaticWnd.EnableWindow(FALSE);
	m_PerfGraphWnd.Create(_T("RC5 Performance Graph"), rect, &m_PrefGraphStaticWnd, 11001);

	m_ParamsListCtrl.SetExtendedStyle(m_ParamsListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ParamsListCtrl.InsertColumn(0, _T("Word Size"), LVCFMT_LEFT, 70, 0);
	m_ParamsListCtrl.InsertColumn(1, _T("No. Of Rounds"), LVCFMT_LEFT, 85, 1);
	m_ParamsListCtrl.InsertColumn(2, _T("Key Size"), LVCFMT_LEFT, 70, 2);

	for (nIndex = 0; nIndex < sizeof(kRC5DefaultParams)/sizeof(RC5_PARAMS); nIndex++)
	{
		szItem.Format("%d", kRC5DefaultParams[nIndex].m_nWordSize);
		nItem = m_ParamsListCtrl.InsertItem(0, (LPCTSTR) szItem);
		if (nItem >= 0)
		{
			szItem.Format("%d", kRC5DefaultParams[nIndex].m_nNoOfRounds);
			m_ParamsListCtrl.SetItemText(nItem, 1, (LPCTSTR) szItem);
			szItem.Format("%d", kRC5DefaultParams[nIndex].m_nKeySize);
			m_ParamsListCtrl.SetItemText(nItem, 2, (LPCTSTR) szItem);
			m_nMaxKeyLen= (m_nMaxKeyLen < kRC5DefaultParams[nIndex].m_nKeySize ? kRC5DefaultParams[nIndex].m_nKeySize : m_nMaxKeyLen);
		}
	}

	m_PlainTextLenListCtrl.SetExtendedStyle(m_PlainTextLenListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_PlainTextLenListCtrl.InsertColumn(0, _T("Length"), LVCFMT_LEFT, 50, 0);

	for (nIndex = 0; nIndex < sizeof(kRC5TestLengths)/sizeof(UINT); nIndex++)
	{
		szItem.Format("%d", kRC5TestLengths[nIndex]);
		nItem = m_PlainTextLenListCtrl.InsertItem(0, (LPCTSTR) szItem);
		m_nMaxPlainTextLen = (m_nMaxPlainTextLen < kRC5TestLengths[nIndex]) ? kRC5TestLengths[nIndex] : m_nMaxPlainTextLen;
	}
}

void CRC5TestView::OnUpdate(CView *pSernder, LPARAM lHint, CObject *pHint)
{
}

/////////////////////////////////////////////////////////////////////////////
// CRC5TestView drawing

void CRC5TestView::OnDraw(CDC* pDC)
{
	CRC5TestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CRC5TestView printing

BOOL CRC5TestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRC5TestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CRC5TestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CRC5TestView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
    CRect rect(100, 100, pDC->GetDeviceCaps(HORZRES) - 100, pDC->GetDeviceCaps(VERTRES) - 100);
    m_KeyGraphWnd.DrawGraphToDC(pDC, rect);
	
//	CFormView::OnPrint(pDC, pInfo);
}

// CRC5TestView diagnostics

#ifdef _DEBUG
void CRC5TestView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRC5TestView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRC5TestDoc* CRC5TestView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRC5TestDoc)));
	return (CRC5TestDoc*)m_pDocument;
}
#endif //_DEBUG

////////////////////////////////////////////////////////
// CRC5TestView message handlers
////////////////////////////////////////////////////////


void CRC5TestView::OnUpdateFilePrintpreview(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_fRC5Started);
}

void CRC5TestView::OnUpdateFileLoadplaintextfile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_fRC5Started);
}

void CRC5TestView::OnUpdateEditAddnewwordset(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_fRC5Started);
}

void CRC5TestView::OnUpdateEditAddnewplaintextlength(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_fRC5Started);
}

void CRC5TestView::AddToLog(LPCTSTR szLog, BOOL fRefresh)
{
	if (szLog == NULL)
	{
		return;
	}
	m_szLog.Append((LPCTSTR) szLog);

	if (fRefresh)
	{
		UpdateData(FALSE);
	}
}

void CRC5TestView::OnEditAddnewwordset()
{
	BOOL fFound;
	int nItem;
	int nWord, nRounds, nBytes;
	CString szItem;
	CRC5ParamsDlg dlg;

	while (dlg.DoModal() == IDOK)
	{
		fFound = FALSE;
		nItem = -1;
		while ((nItem = m_ParamsListCtrl.GetNextItem(nItem,LVNI_ALL)) >= 0)
		{
			nWord = atoi((LPCTSTR) m_ParamsListCtrl.GetItemText(nItem, 0));
			nRounds = atoi((LPCTSTR) m_ParamsListCtrl.GetItemText(nItem, 1));
			nBytes = atoi((LPCTSTR) m_ParamsListCtrl.GetItemText(nItem, 2));

			if ((nWord == dlg.m_nWordSize) && (nRounds == dlg.m_nNoOfRounds) && (nBytes == dlg.m_nKeySize))
			{
				fFound = TRUE;
				break;
			}
		}
		if (fFound)
		{
			AfxMessageBox("Specified set of values already exists. Please specify a new set.");
			continue;
		}

		szItem.Format("%d", dlg.m_nWordSize);
		nItem = m_ParamsListCtrl.InsertItem(0, (LPCTSTR) szItem);
		if (nItem >= 0)
		{
			szItem.Format("%d", dlg.m_nNoOfRounds);
			m_ParamsListCtrl.SetItemText(nItem, 1, (LPCTSTR) szItem);

			szItem.Format("%d", dlg.m_nKeySize);
			m_ParamsListCtrl.SetItemText(nItem, 2, (LPCTSTR) szItem);

			m_nMaxKeyLen= (m_nMaxKeyLen < (UINT) dlg.m_nKeySize ? dlg.m_nKeySize : m_nMaxKeyLen);
		}
		break;
	}
}

void CRC5TestView::OnEditAddnewplaintextlength()
{
	BOOL fFound;
	int nItem;
	int nLength;
	CString szItem;
	CPTLengthDlg dlg;

	while (dlg.DoModal() == IDOK)
	{
		fFound = FALSE;
		nItem = -1;
		while ((nItem = m_PlainTextLenListCtrl.GetNextItem(nItem,LVNI_ALL)) >= 0)
		{
			nLength = atoi((LPCTSTR) m_PlainTextLenListCtrl.GetItemText(nItem, 0));

			if (nLength == dlg.m_nLength)
			{
				fFound = TRUE;
				break;
			}
		}
		if (fFound)
		{
			AfxMessageBox("Specified set of length already exists. Please specify a new length.");
			continue;
		}

		szItem.Format("%d", dlg.m_nLength);
		m_PlainTextLenListCtrl.InsertItem(0, (LPCTSTR) szItem);
		m_nMaxPlainTextLen = (m_nMaxPlainTextLen < (UINT) dlg.m_nLength) ? dlg.m_nLength : m_nMaxPlainTextLen;
		break;
	}
}

void CRC5TestView::OnFilePrintpreview()
{
	CFormView::OnFilePrintPreview();
}

void CRC5TestView::OnFileLoadplaintextfile()
{
	UINT nReadSize;
	UINT nTotalReadSize;
	CString szBuffer;
	CString szMsg;
	CFile file;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					_T("Plain Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"), this);

	if (!UpdateData())
	{
		return;
	}

	if (dlg.DoModal() == IDOK)
	{
		if (file.Open(dlg.m_pOFN->lpstrFile, CFile::modeRead))
		{
			m_szPlainText.Empty();

			nReadSize = 0;
			nTotalReadSize = 0;
			while (nTotalReadSize < file.GetLength())
			{
				nReadSize = file.Read(szBuffer.GetBuffer(4096), 4096);
				szBuffer.ReleaseBuffer(nReadSize);

				m_szPlainText += szBuffer;
				nTotalReadSize += nReadSize;
			}
		}
		else
		{
			szMsg.Format("Failed to open file: %s", (LPCTSTR) dlg.m_pOFN->lpstrFile);
			AfxMessageBox((LPCTSTR) szMsg);
		}
	}

	UpdateData(FALSE);
}

void CRC5TestView::OnNMRclickParamsList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos;
	DWORD dwSelection;
	CRect rect;
	POINT point;
	CMenu PopupMenu;
	CMenu *pSubMenu;
	
	*pResult = 0;

	::GetCursorPos(&point);

	PopupMenu.LoadMenu(IDR_ADD_REMOVE_MENU);
	if (PopupMenu.GetSafeHmenu() == NULL)
	{
		return;
	}

	pSubMenu = PopupMenu.GetSubMenu(0);
	if (pSubMenu == NULL)
	{
		return;
	}

	if (m_ParamsListCtrl.GetFirstSelectedItemPosition() == NULL)
	{
		pSubMenu->RemoveMenu(ID_MENU_REMOVEITEM, MF_BYCOMMAND);
	}

	dwSelection = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
						point.x, point.y, this);
	switch (dwSelection)
	{
		case ID_MENU_ADDITEM:
		{
			OnEditAddnewwordset();
			break;
		}
		case ID_MENU_REMOVEITEM:
		{
			while (pos = m_ParamsListCtrl.GetFirstSelectedItemPosition())
			{
				m_ParamsListCtrl.DeleteItem(m_ParamsListCtrl.GetNextSelectedItem(pos));
			}
			break;
		}
	}
}

void CRC5TestView::OnNMRclickPlainTextLengths(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos;
	DWORD dwSelection;
	CRect rect;
	POINT point;
	CMenu PopupMenu;
	CMenu *pSubMenu;
	
	*pResult = 0;

	::GetCursorPos(&point);

	PopupMenu.LoadMenu(IDR_ADD_REMOVE_MENU);
	if (PopupMenu.GetSafeHmenu() == NULL)
	{
		return;
	}

	pSubMenu = PopupMenu.GetSubMenu(0);
	if (pSubMenu == NULL)
	{
		return;
	}

	if (m_PlainTextLenListCtrl.GetFirstSelectedItemPosition() == NULL)
	{
		pSubMenu->RemoveMenu(ID_MENU_REMOVEITEM, MF_BYCOMMAND);
	}

	dwSelection = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
						point.x, point.y, this);
	switch (dwSelection)
	{
		case ID_MENU_ADDITEM:
		{
			OnEditAddnewplaintextlength();
			break;
		}
		case ID_MENU_REMOVEITEM:
		{
			while (pos = m_PlainTextLenListCtrl.GetFirstSelectedItemPosition())
			{
				m_PlainTextLenListCtrl.DeleteItem(m_PlainTextLenListCtrl.GetNextSelectedItem(pos));
			}
			break;
		}
	}
}

void CRC5TestView::OnBnClickedBnStart()
{
	CProgressDlg dlg;

	if (!UpdateData(TRUE))
	{
		return;
	}

	if (m_szSecretKey.IsEmpty())
	{
		AfxMessageBox(_T("Please enter a secret key."));
		return;
	}

	if (m_szPlainText.IsEmpty())
	{
		AfxMessageBox(_T("Please specify the plain text."));
		return;
	}

	if (m_nMaxKeyLen > (UINT) m_szSecretKey.GetLength())
	{
		AfxMessageBox(_T("Specified key should be greater than all requested key bytes size."));
		return;
	}

	if (m_nMaxPlainTextLen > (UINT) m_szPlainText.GetLength())
	{
		AfxMessageBox(_T("Specified plain text size should be greater than all requested plain text sizes."));
		return;
	}

	m_ParamsListCtrl.EnableWindow(FALSE);
	m_PlainTextLenListCtrl.EnableWindow(FALSE);
	m_SecretKeyCtrl.EnableWindow(FALSE);
	m_PlainTextCtrl.EnableWindow(FALSE);
	m_bnStart.EnableWindow(FALSE);

	m_KeyGraphStaticWnd.EnableWindow(TRUE);
	m_PrefGraphStaticWnd.EnableWindow(TRUE);
	m_KeyGraphStaticWnd.Invalidate();
	m_PrefGraphStaticWnd.Invalidate();

	m_fRC5Started = TRUE;
	AddToLog("Starting RC5 Algorithm...", TRUE);
	dlg.Display(_T("Analysing RC5 algorithm with the given test cases...."), this);
	BeginWaitCursor();

	ExecuteSet<UINT16>();
	ExecuteSet<UINT32>();
	ExecuteSet<UINT64>();
	m_KeyGraphStaticWnd.Invalidate();
	m_PrefGraphStaticWnd.Invalidate();

	dlg.ShowWindow(SW_HIDE);
	AddToLog("\r\nDone RC5 Algorithm.", TRUE);

	EndWaitCursor();
}
