#pragma once

BOOL GetSystemCodePageList()
{
	BOOL bResult = TRUE;
	_tprintf(_T("已安装代码页\r\n"));
	::EnumSystemCodePages([](LPTSTR lpszCodePageString)->BOOL {
		BOOL bResult = FALSE;
		CPINFOEX cpinfoex = { 0 };
		_tprintf(_T("\t%s"), lpszCodePageString);
		bResult = ::GetCPInfoEx(std::stol(lpszCodePageString), 0, &cpinfoex);
		if (bResult)
		{
			_tprintf(_T("(%s)\r\n"), cpinfoex.CodePageName);
		}
		else
		{
			_tprintf(_T("(Error=%ld)\r\n"), GetLastError());
		}
		bResult = TRUE;
		return bResult;
	}, CP_INSTALLED);

	_tprintf(_T("已支持代码页\r\n"));
	::EnumSystemCodePages([](LPTSTR lpszCodePageString)->BOOL {
		BOOL bResult = FALSE;
		CPINFOEX cpinfoex = { 0 };
		_tprintf(_T("\t%s"), lpszCodePageString);
		bResult = ::GetCPInfoEx(std::stol(lpszCodePageString), 0, &cpinfoex);
		if (bResult)
		{
			_tprintf(_T("(%s)\r\n"), cpinfoex.CodePageName);
		}
		else
		{
			_tprintf(_T("(Error=%ld)\r\n"), GetLastError());
		}
		bResult = TRUE;
		return bResult;
	}, CP_SUPPORTED);

	_tprintf(_T("已安装语言\r\n"));
	EnumSystemLocales([](LPTSTR lpszSystemLocaleString)->BOOL {
		BOOL bResult = FALSE;
		_TCHAR tzCountry[MAX_PATH] = { 0 };
		_TCHAR tzLanguage[MAX_PATH] = { 0 };

		_tprintf(_T("\t%s"), lpszSystemLocaleString);

		bResult = GetLocaleInfo(std::stol(lpszSystemLocaleString, 0, 16), LOCALE_SCOUNTRY, (LPTSTR)tzCountry, sizeof(tzCountry) / sizeof(*tzCountry));
		_tprintf(_T("(%s)"), tzCountry);
		bResult = GetLocaleInfo(std::stol(lpszSystemLocaleString, 0, 16), LOCALE_SLANGUAGE, (LPTSTR)tzLanguage, sizeof(tzLanguage) / sizeof(*tzLanguage));
		_tprintf(_T("(%s)\r\n"), tzLanguage);

		//bResult = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLANGUAGE, (LPTSTR)tzLanguage, sizeof(tzLanguage) / sizeof(*tzLanguage));
		//_tprintf(_T("(%s)\r\n"), tzLanguage);

		bResult = TRUE;
		return bResult;
	}, LCID_INSTALLED);

	_tprintf(_T("已支持语言\r\n"));
	EnumSystemLocales([](LPTSTR lpszSystemLocaleString)->BOOL {
		BOOL bResult = FALSE;
		_TCHAR tzCountry[MAX_PATH] = { 0 };
		_TCHAR tzLanguage[MAX_PATH] = { 0 };

		_tprintf(_T("\t%s"), lpszSystemLocaleString);

		bResult = GetLocaleInfo(std::stol(lpszSystemLocaleString, 0, 16), LOCALE_SCOUNTRY, (LPTSTR)tzCountry, sizeof(tzCountry) / sizeof(*tzCountry));
		_tprintf(_T("(%s)"), tzCountry);
		bResult = GetLocaleInfo(std::stol(lpszSystemLocaleString, 0, 16), LOCALE_SLANGUAGE, (LPTSTR)tzLanguage, sizeof(tzLanguage) / sizeof(*tzLanguage));
		_tprintf(_T("(%s)\r\n"), tzLanguage);

		//bResult = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLANGUAGE, (LPTSTR)tzLanguage, sizeof(tzLanguage) / sizeof(*tzLanguage));
		//_tprintf(_T("(%s)\r\n"), tzLanguage);

		bResult = TRUE;
		return bResult;
	}, LCID_SUPPORTED);
	
	_tprintf(_T("枚举字体\r\n"));
	::EnumFonts(::GetDC(::GetDesktopWindow()), NULL, [](
		const LOGFONT    *lplf,
		const TEXTMETRIC *lptm,
		DWORD      dwType,
		LPARAM     lpData)->BOOL {
		BOOL bResult = FALSE;

		USES_CONVERSION;

		_tprintf(_T("\t%s\r\n"), W2T((LPTSTR)lplf->lfFaceName));

		bResult = TRUE;
		return bResult;
	}, NULL);

	_tprintf(_T("枚举字体名称\r\n"));
	::EnumFontFamilies(::GetDC(::GetDesktopWindow()), NULL, [](
		const LOGFONT *lplf,
		const TEXTMETRIC *lptm,
		DWORD dwType,
		LPARAM lpData)->int {
		BOOL bResult = FALSE;

		USES_CONVERSION;

		_tprintf(_T("\t%s\r\n"), W2T((LPTSTR)lplf->lfFaceName));

		bResult = TRUE;
		return bResult;
	}, NULL);

	return bResult;
}