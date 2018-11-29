#pragma once

__inline static BOOL GetSystemAccountUserList()
{
	BOOL bResult = FALSE;
	DWORD dwIndex = 0L;
	DWORD dwLevel = 0L;
	DWORD dwEntriesRead = 0L;
	DWORD dwTotalEntries = 0L;
	DWORD dwResumeHandle = 0L;
	USER_INFO_0 * pUserInfo0 = NULL;
	USER_INFO_4 * pUserInfo4 = NULL;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	LPTSTR lpszServerName = _T("127.0.0.1");
	NET_API_STATUS netapiStatus = NERR_Success;

	USES_CONVERSION;

	//
	// Call the NetUserEnum function, specifying level 0;
	//   enumerate global user account types only.
	//
	do // begin do
	{
		dwLevel = 0L;
		dwEntriesRead = 0L;
		dwTotalEntries = 0L;
		dwResumeHandle = 0L;
		dwPrefMaxLen = MAX_PREFERRED_LENGTH;
		netapiStatus = ::NetUserEnum(T2W(lpszServerName),
			dwLevel,
			FILTER_NORMAL_ACCOUNT, // global users
			(LPBYTE*)&pUserInfo0,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);
		//
		// If the call succeeds,
		//
		if ((netapiStatus == NERR_Success) || (netapiStatus == ERROR_MORE_DATA))
		{
			if (pUserInfo0 != NULL)
			{
				//
				// Loop through the entries.
				//
				for (dwIndex = 0; dwIndex < dwEntriesRead; dwIndex++)
				{
					_tprintf(_T("=================================================\r\n"));
					_tprintf(_T("%s\r\n"), W2T(pUserInfo0[dwIndex].usri0_name));
					dwLevel = 4L;
					netapiStatus = ::NetUserGetInfo(lpszServerName, pUserInfo0[dwIndex].usri0_name, dwLevel, (LPBYTE*)&pUserInfo4);
					//
					// If the call succeeds,
					//
					if ((netapiStatus == NERR_Success) || (netapiStatus == ERROR_MORE_DATA))
					{
						if (pUserInfo4 != NULL)
						{
							_tprintf(_T("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n"),
								W2T(pUserInfo4->usri4_name),
								W2T(pUserInfo4->usri4_full_name),
								W2T(pUserInfo4->usri4_comment),
								W2T(pUserInfo4->usri4_home_dir),
								W2T(pUserInfo4->usri4_home_dir_drive),
								W2T(pUserInfo4->usri4_logon_server),
								W2T(pUserInfo4->usri4_parms),
								W2T(pUserInfo4->usri4_password),
								W2T(pUserInfo4->usri4_profile),
								W2T(pUserInfo4->usri4_script_path),
								W2T(pUserInfo4->usri4_usr_comment),
								W2T(pUserInfo4->usri4_workstations));
						}
					}
					// Otherwise, print the system error.
					//
					else
					{
						_tprintf(_T("A system error has occurred: %d\n"), netapiStatus);
					}
					//
					// Free the allocated buffer.
					//
					if (pUserInfo4 != NULL)
					{
						NetApiBufferFree(pUserInfo4);
						pUserInfo4 = NULL;
					}

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
		if (pUserInfo0 != NULL)
		{
			NetApiBufferFree(pUserInfo0);
			pUserInfo0 = NULL;
		}
	} 
	// Continue to call NetUserEnum while
	//  there are more entries.
	//
	while (netapiStatus == ERROR_MORE_DATA); // end do

	return TRUE;
}