#pragma once

static const UINT64 knMask = 0;
const static double kdPwRatio = 0.718281828549;
const static double kdQwRatio = 0.618033988749;

template <typename TWORD = UINT32>
class CRC5
{
public:

	////////////////////////////////////////////////////////
	//Construction and Destruction
	////////////////////////////////////////////////////////

	CRC5(TWORD *pKey, UINT16 nRounds = 12, UINT16 nBytes = 16)
	{
		m_nRounds = nRounds;
		m_nBytes = nBytes;

		m_Pw = 0;
		m_Qw = 0;
		m_fGeneratedKeys = FALSE;
		m_pSubKeys = NULL;
		m_pKey = pKey;
		m_szKey = NULL;
		m_pLKey = NULL;
		m_nLKeyCount = 0;

		m_lFrequency.QuadPart = 0;
		m_dKeyGenTime = 0;
		m_dCipherTime = 0;
	}

	CRC5(LPCTSTR szKey, UINT16 nRounds = 12, UINT16 nBytes = 16)
	{
		ASSERT(szKey != NULL);
		ASSERT((_tcslen(szKey)) >= nBytes);

		UINT16 nIndex;
		TWORD *pKey;

		pKey = (TWORD *) malloc(sizeof(TWORD)*nBytes);
		if (pKey == NULL)
		{
			return;
		}
		
		for (nIndex = 0; nIndex < nBytes; nIndex++)
		{
			pKey[nIndex] = szKey[nIndex];
		}

		new (this) CRC5((TWORD *) pKey, nRounds, nBytes);

		m_szKey = szKey;
	}

	virtual ~CRC5(void)
	{
		if (m_pSubKeys)
		{
			free(m_pSubKeys);
			m_pSubKeys = NULL;
		}
		if (m_pKey)
		{
			if (m_szKey)
			{
				free(m_pKey);
			}
			m_pKey = NULL;
		}
		if (m_pLKey)
		{
			free(m_pLKey);
			m_pLKey = NULL;
		}
		m_szKey = NULL;
	}

	////////////////////////////////////////////////////////
	// Public members
	////////////////////////////////////////////////////////

	BOOL Key()
	{
		LARGE_INTEGER lStartTime;
		LARGE_INTEGER lEndTime;

		if (!m_fGeneratedKeys)
		{
			if (m_lFrequency.QuadPart == 0)
			{
				SetThreadAffinityMask(GetCurrentThread(), 1);
				QueryPerformanceFrequency(&m_lFrequency);
			}

			QueryPerformanceCounter(&lStartTime);
			if (Init() && Convert() && Mix())
			{
				QueryPerformanceCounter(&lEndTime);
				m_dKeyGenTime = (double) (lEndTime.QuadPart - lStartTime.QuadPart);
				//m_dKeyGenTime = ((double)(lEndTime.QuadPart - lStartTime.QuadPart) / (double) m_lFrequency.QuadPart);
				m_fGeneratedKeys = TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	BOOL Encrypt(TWORD *pPlainText, UINT nPlainTextLen,
			TWORD **ppCipherText, UINT *pnCipherTextLen)
	{
		ASSERT(ppCipherText != NULL);
		ASSERT(pnCipherTextLen != NULL);

		UINT nIndex;
		UINT nRound;
		UINT nPlainTextBlockLen;
		TWORD *pPlainTextBlock;
		LARGE_INTEGER lStartTime;
		LARGE_INTEGER lEndTime;

		if (m_lFrequency.QuadPart == 0)
		{
			SetThreadAffinityMask(GetCurrentThread(), 1);
			QueryPerformanceFrequency(&m_lFrequency);
		}

		QueryPerformanceCounter(&lStartTime);

		*pnCipherTextLen = 0;
		*ppCipherText = NULL;
		if ((pPlainText == NULL) || (nPlainTextLen <= 0))
		{
			return TRUE;
		}

		if ((nPlainTextLen % 2) == 0)
		{
			nPlainTextBlockLen = nPlainTextLen;
		}
		else
		{
			nPlainTextBlockLen = ((int) (nPlainTextLen/2) + 1)*2;
		}
		pPlainTextBlock = (TWORD *) malloc(sizeof(TWORD)*nPlainTextBlockLen);
		if (pPlainTextBlock == NULL)
		{
			return FALSE;
		}
		memset(pPlainTextBlock, 0, sizeof(TWORD)*nPlainTextBlockLen);
		memcpy(pPlainTextBlock, pPlainText, sizeof(TWORD)*nPlainTextLen);

		*ppCipherText = (TWORD *) malloc(sizeof(TWORD)*nPlainTextBlockLen);
		if ((*ppCipherText) == NULL)
		{
			free(pPlainTextBlock);
			return FALSE;
		}
		*pnCipherTextLen = nPlainTextBlockLen;
		memset((*ppCipherText), 0, sizeof(TWORD)*nPlainTextBlockLen);

		// Generate Keys
		if (!Key())
		{
			free(pPlainTextBlock);
			free(*ppCipherText);
			return FALSE;
		}

		for (nIndex = 0; nIndex < (nPlainTextBlockLen/2) ; nIndex++)
		{
			(*ppCipherText)[2*nIndex] = pPlainTextBlock[2*nIndex] + m_pSubKeys[0];
			(*ppCipherText)[2*nIndex + 1] = pPlainTextBlock[2*nIndex + 1] + m_pSubKeys[1];

			for (nRound = 1; nRound <= m_nRounds; nRound++)
			{
				(*ppCipherText)[2*nIndex] = ShiftLeft((*ppCipherText)[2*nIndex]^(*ppCipherText)[2*nIndex + 1], (*ppCipherText)[2*nIndex + 1]) + m_pSubKeys[2*nRound];
				(*ppCipherText)[2*nIndex + 1] = ShiftLeft((*ppCipherText)[2*nIndex]^(*ppCipherText)[2*nIndex + 1], (*ppCipherText)[2*nIndex]) + m_pSubKeys[2*nRound + 1];
			}
		}
		free(pPlainTextBlock);

		QueryPerformanceCounter(&lEndTime);

		m_dCipherTime = (double) (lEndTime.QuadPart - lStartTime.QuadPart);
		//m_dCipherTime = ((double)(lEndTime.QuadPart - lStartTime.QuadPart) / (double) m_lFrequency.QuadPart);

		return TRUE;
	}

	BOOL Encrypt(LPCTSTR szPlainText, UINT nLength, TWORD **ppCipherText, UINT *pnCipherTextLen)
	{
		ASSERT(szPlainText != NULL);

		UINT nIndex;
		TWORD *pPlainText;
		BOOL fRet;

		if (nLength > _tcslen(szPlainText))
		{
			nLength = (UINT) _tcslen(szPlainText);
		}

		pPlainText = (TWORD *) malloc(sizeof(TWORD)*nLength);
		if (pPlainText == NULL)
		{
			return FALSE;
		}
		
		for (nIndex = 0; nIndex < nLength; nIndex++)
		{
			pPlainText[nIndex] = szPlainText[nIndex];
		}

		fRet = Encrypt(pPlainText, (UINT) nLength, ppCipherText, pnCipherTextLen);
		free(pPlainText);

		return fRet;
	}

	BOOL Decrypt(TWORD *pCipherText, UINT nCipherTextLen,
			TWORD **ppPlainText, UINT *pnPlainTextLen)
	{
		ASSERT(ppPlainText != NULL);
		ASSERT(pnPlainTextLen != NULL);

		UINT nRound;
		UINT nIndex;
		LARGE_INTEGER lStartTime;
		LARGE_INTEGER lEndTime;

		if (m_lFrequency.QuadPart == 0)
		{
			SetThreadAffinityMask(GetCurrentThread(), 1);
			QueryPerformanceFrequency(&m_lFrequency);
		}

		QueryPerformanceCounter(&lStartTime);

		*pnPlainTextLen = 0;
		*ppPlainText = NULL;
		if ((pCipherText == NULL) || (nCipherTextLen <= 0))
		{
			return TRUE;
		}

		*ppPlainText = (TWORD *) malloc(sizeof(TWORD)*nCipherTextLen);
		if ((*ppPlainText) == NULL)
		{
			return FALSE;
		}
		*pnPlainTextLen = nCipherTextLen;
		memset((*ppPlainText), 0, sizeof(TWORD)*nCipherTextLen);

		// Generate Keys
		if (!Key())
		{
			return FALSE;
		}

		for (nIndex = 0; nIndex < nCipherTextLen/2; nIndex++)
		{
			for (nRound = m_nRounds; nRound >= 1; nRound--)
			{
				pCipherText[2*nIndex + 1] = (ShiftRight(pCipherText[2*nIndex + 1] - m_pSubKeys[2*nRound + 1], pCipherText[2*nIndex]))^(pCipherText[2*nIndex]);
				pCipherText[2*nIndex] = (ShiftRight(pCipherText[2*nIndex] - m_pSubKeys[2*nRound], pCipherText[2*nIndex + 1]))^(pCipherText[2*nIndex  + 1]);
			}

			(*ppPlainText)[2*nIndex + 1] = pCipherText[2*nIndex + 1] - m_pSubKeys[1];
			(*ppPlainText)[2*nIndex] = pCipherText[2*nIndex] - m_pSubKeys[0];
		}

		QueryPerformanceCounter(&lEndTime);

		m_dCipherTime = (double) (lEndTime.QuadPart - lStartTime.QuadPart);
		//m_dCipherTime = ((double)(lEndTime.QuadPart - lStartTime.QuadPart) / (double) m_lFrequency.QuadPart);

		return FALSE;
	}

private:
	////////////////////////////////////////////////////////
	// Private helper and algorithms Functions
	////////////////////////////////////////////////////////

	UINT16 GetWordSize()
	{
		return (sizeof(TWORD) * 8);
	}

	UINT GetSubKeysCount()
	{
		return (2*m_nRounds + 2);
	}

	TWORD GetPw()
	{
		if (m_Pw == 0)
		{
			m_Pw = (TWORD) (pow((double) 2, (double) GetWordSize()) * kdPwRatio);
			if ((m_Pw % 2) == 0)
			{
				m_Pw++;
			}
		}

		return m_Pw;
	}

	TWORD GetQw()
	{
		if (m_Qw == 0)
		{
			m_Qw = (TWORD) (pow((double) 2, (double) GetWordSize()) * kdQwRatio);
			if ((m_Qw % 2) == 0)
			{
				m_Qw++;
			}
		}

		return m_Qw;
	}

	TWORD ShiftLeft(TWORD nWord, TWORD nAmountToShift)
	{
		TWORD nShiftAmount;
		TWORD nShiftBack;
		TWORD nShiftResult;

		nShiftAmount = (nAmountToShift & ((TWORD) (GetWordSize() - 1)));
		nShiftBack = (GetWordSize() - nShiftAmount);

		nShiftResult = ((nWord << nShiftAmount) | (nWord >> nShiftBack));

		return nShiftResult;
	}

	TWORD ShiftRight(TWORD nWord, TWORD nAmountToShift)
	{
		TWORD nShiftAmount;
		TWORD nShiftBack;
		TWORD nShiftResult;

		nShiftAmount = (nAmountToShift & ((TWORD) (GetWordSize() - 1)));
		nShiftBack = (GetWordSize() - nShiftAmount);

		nShiftResult = ((nWord >> nShiftAmount) | (nWord << nShiftBack));

		return nShiftResult;
	}

	BOOL Init()
	{
		UINT nIndex;

		m_pSubKeys = (TWORD *) malloc(sizeof(TWORD)*GetSubKeysCount());
		if (!m_pSubKeys)
		{
			return FALSE;
		}
		memset(m_pSubKeys, 0, sizeof(TWORD)*GetSubKeysCount());

		m_pSubKeys[0] = GetPw();
		for (nIndex = 1; nIndex < (GetSubKeysCount() - 1); nIndex++)
		{
			m_pSubKeys[nIndex] = (TWORD) (m_pSubKeys[nIndex - 1] + GetQw());
		}

		return TRUE;
	}

	BOOL Convert()
	{
		ASSERT(m_pKey != NULL);

		short nIndex;
		short nIndex2;

		m_nLKeyCount = m_nBytes/sizeof(TWORD);

		m_pLKey = (TWORD *) malloc(sizeof(TWORD)*m_nLKeyCount);
		if (!m_pLKey)
		{
			return FALSE;
		}
		memset(m_pLKey, 0, sizeof(TWORD)*m_nLKeyCount);

		for (nIndex = (m_nBytes - 1); nIndex >= 0; nIndex--)
		{
			nIndex2 = (nIndex / sizeof(TWORD));
			m_pLKey[nIndex2] = ShiftLeft(m_pLKey[nIndex2], 8) + m_pKey[nIndex];
		}

		return TRUE;
	}

	BOOL Mix()
	{
		UINT nIndex;
		TWORD i, j, X, Y;

		i = j = X = Y = 0;

		for (nIndex = 0; nIndex < (3 * max(GetSubKeysCount(), m_nLKeyCount)); nIndex++)
		{
			m_pSubKeys[i] = ShiftLeft(m_pSubKeys[i] + X + Y, 3);
			X = m_pSubKeys[i];
			i = ((i + 1) % GetSubKeysCount());

			m_pLKey[j] = ShiftLeft(m_pLKey[j] + X + Y, X + Y);
			Y = m_pLKey[j];
			j = ((j + 1) % m_nLKeyCount);
		}

		return TRUE;
	}

public:
	////////////////////////////////////////////////////////
	// Public variables
	////////////////////////////////////////////////////////

	double m_dKeyGenTime;
	double m_dCipherTime;

private:
	////////////////////////////////////////////////////////
	// Prrivate Variables
	////////////////////////////////////////////////////////

	UINT16 m_nRounds;
	UINT16 m_nBytes;

	TWORD m_Pw;
	TWORD m_Qw;
	TWORD *m_pSubKeys;
	TWORD *m_pKey;
	TWORD *m_pLKey;
	UINT16 m_nLKeyCount;
	BOOL m_fGeneratedKeys;
	LPCTSTR m_szKey;
	LARGE_INTEGER m_lFrequency;
};

////////////////////////////////////////////////////////
// Algorithm Type Defnitions for Word - 64/32/16
////////////////////////////////////////////////////////

typedef CRC5<UINT16> CRC5W16;
typedef CRC5<UINT32> CRC5W32;
typedef CRC5<UINT64> CRC5W64;
