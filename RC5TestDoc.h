// RC5TestDoc.h : interface of the CRC5TestDoc class
//


#pragma once

class CRC5TestDoc : public CDocument
{
protected: // create from serialization only
	CRC5TestDoc();
	DECLARE_DYNCREATE(CRC5TestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CRC5TestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


