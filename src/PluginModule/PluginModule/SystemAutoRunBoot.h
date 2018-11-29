#pragma once

BOOL GetSystemAutoRunBoot(HKEY hKey, LPCTSTR lpszKey)
{
	DWORD   dwType = 0;
	DWORD   dwIndex = 0;
	DWORD   dwReserved = 0;
	_TCHAR  tzKeyName[MAX_PATH] = { 0 };
	DWORD   dwKeyNameSize = sizeof(tzKeyName) / sizeof(*tzKeyName);
	_TCHAR  tzValueName[MAX_PATH] = { 0 };
	DWORD   dwValueNameSize = sizeof(tzValueName) / sizeof(*tzValueName);
	_TCHAR  tzData[MAX_PATH] = { 0 };
	DWORD   dwDataSize = sizeof(tzData) / sizeof(*tzData);

	HKEY    hSubKey = NULL;
	LSTATUS lStatus = NO_ERROR;
	
	lStatus = RegOpenKey(hKey, lpszKey, &hSubKey);
	if (lStatus == ERROR_SUCCESS)
	{
		for (dwIndex = 0; ;dwIndex++)
		{
			memset(tzValueName, 0, sizeof(tzValueName));
			dwValueNameSize = sizeof(tzValueName) / sizeof(*tzValueName);
			memset(tzData, 0, sizeof(tzData));
			dwDataSize = sizeof(tzData) / sizeof(*tzData);
			lStatus = RegEnumValue(hSubKey, dwIndex, tzValueName, &dwValueNameSize, NULL, &dwType, (LPBYTE)tzData, &dwDataSize);
			if (lStatus == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			_tprintf(_T("%s,%s\r\n"), tzValueName, tzData);
		}
		for (dwIndex = 0; ;dwIndex++)
		{
			memset(tzKeyName, 0, sizeof(tzKeyName));
			dwKeyNameSize = sizeof(tzKeyName) / sizeof(*tzKeyName);
			lStatus = RegEnumKey(hSubKey, dwIndex, tzKeyName, dwKeyNameSize);
			if (lStatus == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			_tprintf(_T("%s\r\n"), tzKeyName);
		}

		RegCloseKey(hSubKey);
	}

	return TRUE;
}
__inline static BOOL GetSystemAutoRunBootList()
{
	struct {
		HKEY hKey;
		LPCTSTR lpKey;
		LPCTSTR lpszHKeyName;
	} hKeyValueList[] = {
		{ HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\"),_T("HKEY_CURRENT_USER") },
		{ HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\"),_T("HKEY_LOCAL_MACHINE") },
		{ HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Run\\"),_T("HKEY_CURRENT_USER") },
		{ HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Run\\"),_T("HKEY_LOCAL_MACHINE") },
		{ HKEY_CURRENT_USER, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run\\"),_T("HKEY_CURRENT_USER") },
		{ HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run\\"),_T("HKEY_LOCAL_MACHINE") },
		{ HKEY_CURRENT_USER, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Run\\"),_T("HKEY_CURRENT_USER") },
		{ HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Run\\"),_T("HKEY_LOCAL_MACHINE") },
	};

	for (auto v : hKeyValueList)
	{
		_tprintf(_T("lpszKey=%s\\%s\r\n"), v.lpszHKeyName, v.lpKey);
		GetSystemAutoRunBoot(v.hKey, v.lpKey);
	}

	return TRUE;
}