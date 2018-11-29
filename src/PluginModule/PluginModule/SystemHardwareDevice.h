#pragma once

__inline static BOOL GetSystemHardwareDeviceList()
{
	BOOL bResult = FALSE;

	HDEVINFO hDevInfo = NULL;
	DWORD dwDeviceIndex = 0L;
	LPBYTE lpPropertyData = NULL;
	DWORD dwPropertyDataSize = 0L;
	DWORD dwPropertyRegDataType = 0L;
	SP_DEVINFO_DATA spDeviceInfoData = { 0 };
	struct tagPropertyName {
		_TCHAR tzName[MAX_PATH];
		DWORD dwProperty;
		_TCHAR tzValue[MAX_PATH];
	} propertyNameList[] = {
		{ _T("SPDRP_DEVICEDESC"),SPDRP_DEVICEDESC,{ 0 } },
		{ _T("SPDRP_HARDWAREID"),SPDRP_HARDWAREID,{ 0 } },
		{ _T("SPDRP_FRIENDLYNAME"),SPDRP_FRIENDLYNAME,{ 0 } },
		{ _T("SPDRP_DRIVER"),SPDRP_DRIVER,{ 0 } },
		{ _T("SPDRP_DEVTYPE"),SPDRP_DEVTYPE,{ 0 } },
		{ _T("SPDRP_PHYSICAL_DEVICE_OBJECT_NAME"),SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,{ 0 } },
		{ _T("SPDRP_MFG"),SPDRP_MFG,{ 0 } },
		{ _T("SPDRP_CLASS"),SPDRP_CLASS,{ 0 } },
		{ _T("SPDRP_ADDRESS"),SPDRP_ADDRESS,{ 0 } },
		{ _T("SPDRP_LOCATION_PATHS"),SPDRP_LOCATION_PATHS,{ 0 } },
		{ _T("SPDRP_LOCATION_INFORMATION"),SPDRP_LOCATION_INFORMATION,{ 0 } },
		{ _T("SPDRP_ENUMERATOR_NAME"),SPDRP_ENUMERATOR_NAME,{ 0 } },
	};
	DWORD dwPropertyIndex = 0L;
	DWORD dwPropertyCount = sizeof(propertyNameList) / sizeof(*propertyNameList);

	// Create a HDEVINFO with all present devices.
	hDevInfo = ::SetupDiGetClassDevs(NULL,
		0, // Enumerator
		0,
		DIGCF_PRESENT | DIGCF_ALLCLASSES);

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		// Insert error handling here.
		return bResult;
	}

	// Enumerate through all devices in Set.

	spDeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (dwDeviceIndex = 0; ::SetupDiEnumDeviceInfo(hDevInfo, dwDeviceIndex, &spDeviceInfoData); dwDeviceIndex++)
	{
		for (dwPropertyIndex = 0L; dwPropertyIndex < dwPropertyCount; dwPropertyIndex++)
		{
			lpPropertyData = NULL;
			dwPropertyDataSize = 0L;
			dwPropertyRegDataType = 0L;

			// Call function with null to begin with, 
			// then use the returned buffer size 
			// to Alloc the buffer. Keep calling until
			// success or an unknown failure.
			bResult = ::SetupDiGetDeviceRegistryProperty(hDevInfo,
				&spDeviceInfoData,
				propertyNameList[dwPropertyIndex].dwProperty,
				&dwPropertyRegDataType,
				(PBYTE)lpPropertyData,
				dwPropertyDataSize,
				&dwPropertyDataSize);
			if (!bResult && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
			{
				//分配缓冲区大小
				lpPropertyData = (LPBYTE)malloc(dwPropertyDataSize * sizeof(*lpPropertyData));
				if (lpPropertyData)
				{
					bResult = ::SetupDiGetDeviceRegistryProperty(hDevInfo,
						&spDeviceInfoData,
						propertyNameList[dwPropertyIndex].dwProperty,
						&dwPropertyRegDataType,
						(PBYTE)lpPropertyData,
						dwPropertyDataSize,
						&dwPropertyDataSize);
				}
			}
			_tprintf(_T("[%s]=%s\r\n"), propertyNameList[dwPropertyIndex].tzName, (LPTSTR)(lpPropertyData));
			if (lpPropertyData)
			{
				free(lpPropertyData);
				lpPropertyData = NULL;
			}
		}
	}

	if (GetLastError() != NO_ERROR &&  GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		//在这里放错误处理代码
		bResult = FALSE;
	}

	// 清理
	::SetupDiDestroyDeviceInfoList(hDevInfo);

	return bResult;
}