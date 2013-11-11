Notes:

	UINT *pCiperText = NULL;
	UINT nCipherText = 0;
	CRC5W32 rc5((LPCTSTR) m_szSecretKey);

	rc5.Encrypt((LPCTSTR) m_szPlainText, m_szPlainText.GetLength(), &pCiperText, &nCipherText);

	UINT nIndex;
	CString szMsg;
	
	szMsg.Format("Encrypted Hex String(%d, %d) :", rc5.m_dKeyGenTime, rc5.m_dCipherTime);
	for (nIndex = 0; nIndex < min(nCipherText, 50); nIndex++)
	{
		szMsg.AppendFormat(" %X", pCiperText[nIndex]);
	}
	AfxMessageBox((LPCTSTR) szMsg);

	UINT *pPlainText = NULL;
	UINT nPlainText = 0;
	rc5.Decrypt(pCiperText, nCipherText, &pPlainText, &nPlainText);

	szMsg.Format("Decrypted Hex String(%ld, %ld) :", rc5.m_dKeyGenTime, rc5.m_dCipherTime);
	for (nIndex = 0; nIndex < min(nPlainText, 50); nIndex++)
	{
		szMsg.AppendFormat(" %c", pPlainText[nIndex]);
	}
	AfxMessageBox((LPCTSTR) szMsg);

	free(pCiperText);
	free(pPlainText);
