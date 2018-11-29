#pragma once

#define _WIN32_DCOM

#include <tchar.h>
#include <stdio.h>
#include <locale.h>
#include <atlconv.h>
#include <windows.h>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <locale.h>
#include <winsock.h>
#include <iphlpapi.h>
#include <tlhelp32.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

//  Include the task header file.
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

#include <lm.h>
#include <lmaccess.h>
#include <lmerr.h>
#include <lmapibuf.h>
#include <comdef.h>
#include <lmcons.h>
#include <lmat.h>
#pragma comment(lib, "netapi32")

#include <initguid.h>
#include <ole2.h>
#include <mstask.h>
#include <msterr.h>
#pragma comment(lib,"mstask.lib")

#include <winnetwk.h>
#pragma comment(lib, "mpr.lib")

#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#pragma comment(lib,"setupapi.lib")

#if !defined(_UNICODE) && !defined(UNICODE)
#define tstring std::string
#define to_tstring std::to_string
#else
#define tstring std::wstring
#define to_tstring std::to_wstring
#endif

#define TSTRING tstring
#define TO_TSTRING to_tstring

__inline static BOOL ElevationPrivilege(HANDLE hProcess, LPCTSTR lpszPrivilegeName = SE_DEBUG_NAME, DWORD dwPrivilegeAttributes = SE_PRIVILEGE_ENABLED)
{
	LUID luid = { 0 };
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp = { 0 };
	if (::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) &&
		::LookupPrivilegeValue(NULL, lpszPrivilegeName, &luid))
	{
		tp.PrivilegeCount = (1L);
		(*tp.Privileges).Luid = luid;
		(*tp.Privileges).Attributes = dwPrivilegeAttributes;
		bResult = ::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	}
	return bResult;
}
__inline static BOOL ElevationDebugPrivilege(HANDLE hProcess)
{
	return ElevationPrivilege(hProcess, SE_DEBUG_NAME, SE_PRIVILEGE_ENABLED);
}
__inline static BOOL ElevationBackupPrivilege(HANDLE hProcess)
{
	return ElevationPrivilege(hProcess, SE_BACKUP_NAME, SE_PRIVILEGE_ENABLED);
}
__inline static BOOL ElevationRestorePrivilege(HANDLE hProcess)
{
	return ElevationPrivilege(hProcess, SE_RESTORE_NAME, SE_PRIVILEGE_ENABLED);
}
__inline static BOOL ElevationTrustedCredmanAccessPrivilege(HANDLE hProcess)
{
	return ElevationPrivilege(hProcess, SE_TRUSTED_CREDMAN_ACCESS_NAME, SE_PRIVILEGE_ENABLED);
}
__inline static BOOL ElevationImpersonatePrivilege(HANDLE hProcess)
{
	return ElevationPrivilege(hProcess, SE_IMPERSONATE_NAME, SE_PRIVILEGE_ENABLED);
}

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef struct _PEB_LDR_DATA {
	BYTE Reserved1[8];
	PVOID Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;
typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef VOID(WINAPI*PPS_POST_PROCESS_INIT_ROUTINE)(VOID);

 typedef struct _PEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	BYTE Reserved4[104];
	PVOID Reserved5[52];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved6[128];
	PVOID Reserved7[1];
	ULONG SessionId;
	
} PEB, *PPEB;

__inline static PPEB GetCurrentPeb()
{
	return ((PPEB(WINAPI*)())::GetProcAddress(GetModuleHandle(_T("NTDLL.DLL")), "NtGetCurrentPeb"))();
}
__inline static BOOL GetWindowsVersionEx(OSVERSIONINFOEX * pOSVIEX = NULL)
{
	BOOL bResult = FALSE;
	RTL_OSVERSIONINFOEXW rtl_osviex = { 0 };
	rtl_osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(((LONG(WINAPI*)(PRTL_OSVERSIONINFOEXW))::GetProcAddress(GetModuleHandle(_T("NTDLL.DLL")), "RtlGetVersion"))(&rtl_osviex) == 0)
	{
		if (pOSVIEX)
		{
			USES_CONVERSION_EX;
			pOSVIEX->dwOSVersionInfoSize = rtl_osviex.dwOSVersionInfoSize;
			pOSVIEX->dwMajorVersion = rtl_osviex.dwMajorVersion;
			pOSVIEX->dwMinorVersion = rtl_osviex.dwMinorVersion;
			pOSVIEX->dwBuildNumber = rtl_osviex.dwBuildNumber;
			pOSVIEX->dwPlatformId = rtl_osviex.dwPlatformId;
			// Maintenance string for PSS usage
			memcpy(pOSVIEX->szCSDVersion, W2T_EX(rtl_osviex.szCSDVersion, sizeof(rtl_osviex.szCSDVersion) / sizeof(rtl_osviex.szCSDVersion)), sizeof(pOSVIEX->szCSDVersion));
			pOSVIEX->wServicePackMajor = rtl_osviex.wServicePackMajor;
			pOSVIEX->wServicePackMinor = rtl_osviex.wServicePackMinor;
			pOSVIEX->wSuiteMask = rtl_osviex.wSuiteMask;
			pOSVIEX->wProductType = rtl_osviex.wProductType;
			pOSVIEX->wReserved = rtl_osviex.wReserved;
			bResult = TRUE;
		}
	}
	return bResult;
}
//读取操作系统的名称
__inline static TSTRING GetWindowsSystemVersionName(OSVERSIONINFOEX * pOSVIEX = NULL)
{
	TSTRING tsOSName = _T("Unknown Operating System.");
	//用SYSTEM_INFO结构判断64位AMD处理器
	SYSTEM_INFO si = { 0 };
	OSVERSIONINFOEX osviex = { 0 };

	OSVERSIONINFOEX * p_osviex = (pOSVIEX ? pOSVIEX : &osviex);

	p_osviex->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	//调用GetSystemInfo函数填充结构 
	GetSystemInfo(&si);

	if (GetWindowsVersionEx((OSVERSIONINFOEX *)p_osviex))
	{
		//下面根据版本信息判断操作系统名称 
		switch (p_osviex->dwMajorVersion)//判断主版本号
		{
		case 4:
			switch (p_osviex->dwMinorVersion)//判断次版本号 
			{
			case 0:
				if (p_osviex->dwPlatformId == VER_PLATFORM_WIN32_NT)
					tsOSName = _T("Microsoft Windows NT 4.0"); //1996年7月发布 
				else if (p_osviex->dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					tsOSName = _T("Microsoft Windows 95");
				break;
			case 10:
				tsOSName = _T("Microsoft Windows 98");
				break;
			case 90:
				tsOSName = _T("Microsoft Windows Me");
				break;
			}
			break;

		case 5:
			switch (p_osviex->dwMinorVersion)	//再比较dwMinorVersion的值
			{
			case 0:
				tsOSName = _T("Microsoft Windows 2000");//1999年12月发布
				break;

			case 1:
				tsOSName = _T("Microsoft Windows XP");//2001年8月发布
				break;

			case 2:
				if (p_osviex->wProductType == VER_NT_WORKSTATION
					&& si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					tsOSName = _T("Microsoft Windows XP Professional x64 Edition");
				}
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					tsOSName = _T("Microsoft Windows Server 2003");//2003年3月发布 
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					tsOSName = _T("Microsoft Windows Server 2003 R2");
				break;
			}
			break;

		case 6:
			switch (p_osviex->dwMinorVersion)
			{
			case 0:
				if (p_osviex->wProductType == VER_NT_WORKSTATION)
					tsOSName = _T("Microsoft Windows Vista");
				else
					tsOSName = _T("Microsoft Windows Server 2008");//服务器版本 
				break;
			case 1:
				if (p_osviex->wProductType == VER_NT_WORKSTATION)
					tsOSName = _T("Microsoft Windows 7");
				else
					tsOSName = _T("Microsoft Windows Server 2008 R2");
				break;
			case 2:
				if (p_osviex->wProductType == VER_NT_WORKSTATION)
					tsOSName = _T("Microsoft Windows 8");
				else
					tsOSName = _T("Microsoft Windows Server 2012");
				break;
			case 3:
				if (p_osviex->wProductType == VER_NT_WORKSTATION)
					tsOSName = _T("Microsoft Windows 8.1");
				else
					tsOSName = _T("Microsoft Windows Server 2012 R2");
				break;
			}
			break;

		case 10:
			switch (p_osviex->dwMinorVersion)
			{
			case 0:
				if (p_osviex->wProductType == VER_NT_WORKSTATION)
					tsOSName = _T("Microsoft Windows 10");
				else
					tsOSName = _T("Microsoft Windows Server 2016 Technical Preview");//服务器版本 
				break;
			}
			break;
		}
		if (p_osviex->wServicePackMajor > 0)
		{
			tsOSName.append(_T(" Service Pack ")).append(TO_TSTRING(p_osviex->wServicePackMajor));
			if (p_osviex->wServicePackMinor)
			{
				tsOSName.append(_T(".")).append(TO_TSTRING(p_osviex->wServicePackMinor));
			}
		}
		else if (p_osviex->wServicePackMinor)
		{
			tsOSName.append(_T(" Service Pack ")).append(TO_TSTRING(p_osviex->wServicePackMajor)).append(_T(".")).append(TO_TSTRING(p_osviex->wServicePackMinor));
		}
		else
		{
			//no-op
		}
	}

	//https://msdn.microsoft.com/en-us/library/ms724832.aspx

	return tsOSName;
}

//////////////////////////////////////////////////////////////////////////
// 函数说明：遍历目录获取指定文件列表
// 参    数：输出的文件行内容数据、过滤后缀名、过滤的前缀字符
// 返 回 值：bool返回类型，成功返回true；失败返回false
// 编 写 者: ppshuai 20141112
//////////////////////////////////////////////////////////////////////////
__inline static BOOL DirectoryTraversal(std::map<TSTRING, TSTRING> * pTTMAP, LPCTSTR lpFindPath = _T(".\\"), LPCTSTR lpRootPath = _T(".\\"), LPCTSTR lpExtension = _T("*.*"))
{
	BOOL bResult = FALSE;
	HANDLE hFindFile = NULL;
	WIN32_FIND_DATA wfd = { 0 };
	_TCHAR tChildPath[MAX_PATH + 1] = { 0 };
	_TCHAR tFindFileName[MAX_PATH + 1] = { 0 };

	if ((lstrlen(lpFindPath) >= lstrlen(lpRootPath)) && (TSTRING(lpFindPath).find(lpRootPath) != TSTRING::npos))
	{
		wsprintf(tChildPath, _T("%s"), lpFindPath + lstrlen(lpRootPath));
	}

	//构建遍历根目录
	wsprintf(tFindFileName, TEXT("%s%s"), lpFindPath, lpExtension);

	hFindFile = FindFirstFileEx(tFindFileName, FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	//hFindFile = FindFirstFile(tRootPath, &wfd);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				pTTMAP->insert(std::map<TSTRING, TSTRING>::value_type((TSTRING(lpFindPath) + wfd.cFileName), TSTRING(tChildPath) + wfd.cFileName));
			}
			else
			{
				if (lstrcmp(wfd.cFileName, _T(".")) && lstrcmp(wfd.cFileName, _T("..")))
				{
					pTTMAP->insert(std::map<TSTRING, TSTRING>::value_type(TSTRING(lpFindPath) + wfd.cFileName, TSTRING(tChildPath) + wfd.cFileName + _T("\\")));
					bResult = DirectoryTraversal(pTTMAP, TSTRING(TSTRING(lpFindPath) + wfd.cFileName + _T("\\")).c_str(), lpRootPath, lpExtension);
				}
			}
		} while (FindNextFile(hFindFile, &wfd));

		FindClose(hFindFile);
		hFindFile = NULL;
		bResult = TRUE;
	}

	return bResult;
}

typedef struct
{
	DWORD   dwState;        	// 连接状态
	DWORD   dwLocalAddr;    	// 本地地址
	DWORD   dwLocalPort;    	// 本地端口
	DWORD   dwRemoteAddr;   	// 远程地址
	DWORD   dwRemotePort;   	// 远程端口
	DWORD	  dwProcessId;		// 进程ID号
} MIB_TCPEXROW, *PMIB_TCPEXROW;

typedef struct
{
	DWORD			dwNumEntries;
	MIB_TCPEXROW	table[ANY_SIZE];
} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

typedef struct
{
	DWORD   dwLocalAddr;    	// 本地地址
	DWORD   dwLocalPort;    	// 本地端口
	DWORD	  dwProcessId;		// 进程ID号
} MIB_UDPEXROW, *PMIB_UDPEXROW;

typedef struct
{
	DWORD			dwNumEntries;
	MIB_UDPEXROW	table[ANY_SIZE];
} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;


// 扩展函数原型
typedef DWORD(WINAPI *PFNAllocateAndGetTcpExTableFromStack)(
	PMIB_TCPEXTABLE *pTcpTable,
	BOOL bOrder,
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

typedef DWORD(WINAPI *PFNAllocateAndGetUdpExTableFromStack)(
	PMIB_UDPEXTABLE *pUdpTable,
	BOOL bOrder,
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);
typedef DWORD(WINAPI *PFNGetExtendedTcpTable)(
	_Out_   PVOID           pTcpTable,
	_Inout_ PDWORD          pdwSize,
	_In_    BOOL            bOrder,
	_In_    ULONG           ulAf,
	_In_    TCP_TABLE_CLASS TableClass,
	_In_    ULONG           Reserved
	);
typedef DWORD(WINAPI *PFNGetExtendedUdpTable)(
	_Out_   PVOID           pUdpTable,
	_Inout_ PDWORD          pdwSize,
	_In_    BOOL            bOrder,
	_In_    ULONG           ulAf,
	_In_    UDP_TABLE_CLASS TableClass,
	_In_    ULONG           Reserved
	);

// 将进程ID号（PID）转化为进程名称
__inline static LPTSTR ProcessPidToName(LPTSTR ProcessName, DWORD ProcessId, HANDLE hSnap = NULL)
{
	PROCESSENTRY32 processEntry = { 0 };
	processEntry.dwSize = sizeof(processEntry);

	// 找不到的话，默认进程名为“???”
	lstrcpy(ProcessName, _T("???"));

	HANDLE hSnapAll = hSnap ? hSnap : CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapAll && hSnapAll != INVALID_HANDLE_VALUE)
	{
		if (!::Process32First(hSnapAll, &processEntry))
		{
			return ProcessName;
		}
		do
		{
			if (processEntry.th32ProcessID == ProcessId) // 就是这个进程
			{
				lstrcpy(ProcessName, processEntry.szExeFile);
				break;
			}
		} while (::Process32Next(hSnapAll, &processEntry));

		if (!hSnap)
		{
			CloseHandle(hSnapAll);
		}
	}
	return ProcessName;
}