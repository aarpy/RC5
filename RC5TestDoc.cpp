// RC5TestDoc.cpp : implementation of the CRC5TestDoc class
//

#include "stdafx.h"
#include "RC5Test.h"

#include "RC5TestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRC5TestDoc

IMPLEMENT_DYNCREATE(CRC5TestDoc, CDocument)

BEGIN_MESSAGE_MAP(CRC5TestDoc, CDocument)
END_MESSAGE_MAP()


// CRC5TestDoc construction/destruction

CRC5TestDoc::CRC5TestDoc()
{
	// TODO: add one-time construction code here

}

CRC5TestDoc::~CRC5TestDoc()
{
}

BOOL CRC5TestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CRC5TestDoc serialization

void CRC5TestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CRC5TestDoc diagnostics

#ifdef _DEBUG
void CRC5TestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRC5TestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRC5TestDoc commands
