#pragma once

__inline static BOOL GetSystemSharedFolderList()
{
	BOOL bResult = FALSE;
	DWORD dwIndex = 0L;
	DWORD dwLevel = 0L;
	DWORD dwEntriesRead = 0L;
	DWORD dwTotalEntries = 0L;
	DWORD dwResumeHandle = 0L;
	SHARE_INFO_502 * pShareInfo502 = NULL;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	LPTSTR lpszServerName = _T("127.0.0.1");
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
		netapiStatus = ::NetShareEnum(T2W(lpszServerName),
			dwLevel,
			(LPBYTE*)&pShareInfo502,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);
		//
		// If the call succeeds,
		//
		if ((netapiStatus == NERR_Success) || (netapiStatus == ERROR_MORE_DATA))
		{
			if (pShareInfo502 != NULL)
			{
				//
				// Loop through the entries.
				//
				for (dwIndex = 0; dwIndex < dwEntriesRead; dwIndex++)
				{
					_tprintf(_T("=============================================================================\r\n"));
					_tprintf(_T("%s,%s,%s\r\n"), 
						W2T(pShareInfo502[dwIndex].shi502_netname), 
						W2T(pShareInfo502[dwIndex].shi502_remark),
						W2T(pShareInfo502[dwIndex].shi502_path));
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
		if (pShareInfo502 != NULL)
		{
			NetApiBufferFree(pShareInfo502);
			pShareInfo502 = NULL;
		}
	}
	// Continue to call NetUserEnum while
	//  there are more entries.
	//
	while (netapiStatus == ERROR_MORE_DATA); // end do

	return bResult;
}