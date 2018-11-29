#pragma once

__inline static BOOL GetSystemSharedSessionList()
{
	BOOL bResult = FALSE;
	DWORD dwIndex = 0L;
	DWORD dwLevel = 0L;
	DWORD dwEntriesRead = 0L;
	DWORD dwTotalEntries = 0L;
	DWORD dwResumeHandle = 0L;
	SESSION_INFO_502 * pSessionInfo502 = NULL;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	LPTSTR lpszServerName = _T("127.0.0.1");
	LPTSTR lpszClientName = _T("127.0.0.1");
	LPTSTR lpszUserName = _T("");
	NET_API_STATUS netapiStatus = NERR_Success;

	USES_CONVERSION;

	//
	// Call the NetUserEnum function, specifying level 502;
	//   enumerate global user account types only.
	//
	do // begin do
	{
		dwLevel = 502L;
		dwEntriesRead = 0L;
		dwTotalEntries = 0L;
		dwResumeHandle = 0L;
		dwPrefMaxLen = MAX_PREFERRED_LENGTH;
		netapiStatus = ::NetConnectionEnum(T2W(lpszServerName),
			T2W(lpszClientName),
			dwLevel,
			(LPBYTE*)&pSessionInfo502,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);
		//
		// If the call succeeds,
		//
		if ((netapiStatus == NERR_Success) || (netapiStatus == ERROR_MORE_DATA))
		{
			if (pSessionInfo502 != NULL)
			{
				//
				// Loop through the entries.
				//
				for (dwIndex = 0; dwIndex < dwEntriesRead; dwIndex++)
				{
					_tprintf(_T("=============================================================================\r\n"));
					_tprintf(_T("%s,%s,%s,%s\r\n"), 
						W2T(pSessionInfo502[dwIndex].sesi502_cname),
						W2T(pSessionInfo502[dwIndex].sesi502_cltype_name),
						W2T(pSessionInfo502[dwIndex].sesi502_transport),
						W2T(pSessionInfo502[dwIndex].sesi502_username));
				}
			}
		}
		//
		// Otherwise, print the system error.
		//
		else
		{
			_tprintf(_T("A system error has occurred: %d\n"), netapiStatus);
		}
		//
		// Free the allocated buffer.
		//
		if (pSessionInfo502 != NULL)
		{
			NetApiBufferFree(pSessionInfo502);
			pSessionInfo502 = NULL;
		}
	}
	// Continue to call NetUserEnum while
	//  there are more entries.
	//
	while (netapiStatus == ERROR_MORE_DATA); // end do

	return bResult;
}