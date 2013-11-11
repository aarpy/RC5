// RC5TestView.h : interface of the CRC5TestView class
//


#pragma once

#include <stdlib.h>

#include "Graph\\GraphWnd.h"
#include "rc5.h"

class CRC5TestView : public CFormView
{
protected: // create from serialization only
	CRC5TestView();
	DECLARE_DYNCREATE(CRC5TestView)

public:
	enum{ IDD = IDD_RC5TEST_FORM };

// Attributes
public:
	CRC5TestDoc* GetDocument() const;
	void AddToLog(LPCTSTR szLog, BOOL fRefresh = FALSE);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView *pSernder, LPARAM lHint, CObject *pHint);

// Implementation
public:
	virtual ~CRC5TestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_fRC5Started;
	CListCtrl m_ParamsListCtrl;
	CListCtrl m_PlainTextLenListCtrl;
	CEdit m_SecretKeyCtrl;
	CEdit m_PlainTextCtrl;
	CButton m_bnStart;
	CStatic m_KeyGraphStaticWnd;
	CStatic m_PrefGraphStaticWnd;

	long m_nKeyGraphCount;
	CGraphWnd m_KeyGraphWnd;

	UINT m_nMaxPlainTextLen;
	UINT m_nMaxKeyLen;

	long m_nPerfGraphCount;
	CGraphWnd m_PerfGraphWnd;
	
	double m_dKeyMaxY, m_dPrefMaxY;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnStart();
	CString m_szSecretKey;
	CString m_szPlainText;
	CString m_szLog;
	afx_msg void OnNMRclickParamsList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickPlainTextLengths(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFilePrintpreview();
	afx_msg void OnFileLoadplaintextfile();
	afx_msg void OnUpdateFilePrintpreview(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileLoadplaintextfile(CCmdUI *pCmdUI);
	afx_msg void OnEditAddnewwordset();
	afx_msg void OnEditAddnewplaintextlength();
	afx_msg void OnUpdateEditAddnewwordset(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditAddnewplaintextlength(CCmdUI *pCmdUI);

private:

	template <typename TWORD>
	void ExecuteSet()
	{
		BOOL fVerify;
		int nIndex;
		int nItem, nItem2;
		int nWord, nRounds, nBytes, nLength;
		long nKeyGraph, nEncryptGraph, nDecryptGraph;
		UINT nCipherTextLen, nDecryptTextLen;
		TWORD *pCipherText, *pDecryptText;
		CString szGraphTitle, szLog;

		nItem = -1;
		while ((nItem = m_ParamsListCtrl.GetNextItem(nItem, LVNI_ALL)) >= 0)
		{
			nWord = atoi((LPCTSTR) m_ParamsListCtrl.GetItemText(nItem, 0));
			if ((sizeof(TWORD)*8) != nWord)
			{
				continue;
			}
			nRounds = atoi((LPCTSTR) m_ParamsListCtrl.GetItemText(nItem, 1));
			nBytes = atoi((LPCTSTR) m_ParamsListCtrl.GetItemText(nItem, 2));

			CRC5<TWORD> rc5((LPCTSTR) m_szSecretKey, nRounds, nBytes);

			rc5.Key();

			szLog.Format("\r\nGenerated Key for %d/%d/%d. Time: %.0f.", nWord, nRounds, nBytes, rc5.m_dKeyGenTime);
			AddToLog((LPCTSTR) szLog, TRUE);

			if (m_nKeyGraphCount == 0)
			{
				m_KeyGraphWnd.SetAxisProps(_T("X-Axis"), _T(""), 0, GRAPH_X_AXIS, TRUE);
				m_KeyGraphWnd.SetAxisProps(_T("Y-Axis"), _T(""), 0, GRAPH_Y_AXIS, TRUE);
				m_KeyGraphWnd.SetGraphWorldCoords(0, 70, 0, m_dKeyMaxY, TRUE);
				//m_KeyGraphWnd.SetGraphFlags(GraphWnd.GetGraphFlags() | GRAPH_GRAPH_SCATTER, TRUE);
			};

			szGraphTitle.Format("%d/%d/%d", nWord, nRounds, nBytes);
			nKeyGraph = m_KeyGraphWnd.AddGraph(g_DefaultGraphColors[m_nKeyGraphCount++], (LPCTSTR) szGraphTitle);
			if (nKeyGraph != -1)
			{
				m_KeyGraphWnd.AddPoint(nKeyGraph, (double) nWord, rc5.m_dKeyGenTime);
				if (rc5.m_dKeyGenTime > m_dKeyMaxY)
				{
					m_dKeyMaxY = (rc5.m_dKeyGenTime*11.0)/10;
					m_KeyGraphWnd.SetGraphWorldCoords(0, 70, 0, m_dKeyMaxY, TRUE);
				}
			}

			if (m_nPerfGraphCount == 0)
			{
				m_PerfGraphWnd.SetAxisProps(_T("X-Axis"), _T(""), 0, GRAPH_X_AXIS, TRUE);
				m_PerfGraphWnd.SetAxisProps(_T("Y-Axis"), _T(""), 0, GRAPH_Y_AXIS, TRUE);
				m_PerfGraphWnd.SetGraphWorldCoords(0, (double) (m_nMaxPlainTextLen + 10), 0, 1000, TRUE);
				//m_PerfGraphWnd.SetGraphFlags(GraphWnd.GetGraphFlags() | GRAPH_GRAPH_SCATTER, TRUE);
			};

			szGraphTitle.Format("%d/%d/%d - Encrypt", nWord, nRounds, nBytes);
			nEncryptGraph = m_PerfGraphWnd.AddGraph(g_DefaultGraphColors[m_nPerfGraphCount++], (LPCTSTR) szGraphTitle);

			szGraphTitle.Format("%d/%d/%d - Decrypt", nWord, nRounds, nBytes);
			nDecryptGraph = m_PerfGraphWnd.AddGraph(g_DefaultGraphColors[m_nPerfGraphCount++], (LPCTSTR) szGraphTitle);

			nItem2 = -1;
			while ((nItem2 = m_PlainTextLenListCtrl.GetNextItem(nItem2, LVNI_ALL)) >= 0)
			{
				nLength = atoi((LPCTSTR) m_PlainTextLenListCtrl.GetItemText(nItem2, 0));

				rc5.Encrypt((LPCTSTR) m_szPlainText, nLength, &pCipherText, &nCipherTextLen);
				if (nEncryptGraph != -1)
				{
					m_PerfGraphWnd.AddPoint(nEncryptGraph, (double) nLength, rc5.m_dCipherTime);
					if (rc5.m_dCipherTime > m_dPrefMaxY)
					{
						m_dPrefMaxY = (rc5.m_dCipherTime*11.0)/10;
						m_PerfGraphWnd.SetGraphWorldCoords(0, (double) ((m_nMaxPlainTextLen*11.0)/10.0), 0, m_dPrefMaxY, TRUE);
					}
				}
				szLog.Format("\r\nEncrypted length: %d. Time: %.0f.", nLength, rc5.m_dCipherTime);
				AddToLog((LPCTSTR) szLog);

				rc5.Decrypt(pCipherText, nCipherTextLen, &pDecryptText, &nDecryptTextLen);
				if (nDecryptGraph != -1)
				{
					m_PerfGraphWnd.AddPoint(nDecryptGraph, nLength, rc5.m_dCipherTime);
					if (rc5.m_dCipherTime > m_dPrefMaxY)
					{
						m_dPrefMaxY = (rc5.m_dCipherTime*11.0)/10;
						m_PerfGraphWnd.SetGraphWorldCoords(0, (double) ((m_nMaxPlainTextLen*11.0)/10.0), 0, m_dPrefMaxY, TRUE);
					}
				}
				szLog.Format("\r\nDecrypted length: %d. Time: %.0f.", nLength, rc5.m_dCipherTime);
				AddToLog((LPCTSTR) szLog);

				//compare plain text and decrypted stringand add to log
				fVerify = TRUE;
				if (nDecryptTextLen >= (UINT) nLength)
				{
					for(nIndex = 0; nIndex < nLength; nIndex++)
					{
						if (pDecryptText[nIndex] != m_szPlainText[nIndex])
						{
							fVerify = FALSE;
							break;
						}
					}
				}
				else
				{
					fVerify = FALSE;
				}

				if (fVerify)
				{
					AddToLog("\r\nEncryption/Decryption verified successfully.");
				}
				else
				{
					AddToLog("Encryption/Decryption failed verification.");
				}

				free(pDecryptText); pDecryptText = NULL;
				free(pCipherText); pCipherText = NULL;
			}
		}
	}
};

#ifndef _DEBUG  // debug version in RC5TestView.cpp
inline CRC5TestDoc* CRC5TestView::GetDocument() const
   { return reinterpret_cast<CRC5TestDoc*>(m_pDocument); }
#endif

