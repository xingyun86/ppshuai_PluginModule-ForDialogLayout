#pragma once

typedef struct tagProtocolTableA {
	CHAR szProtocolName[MAXCHAR];
	CHAR szProcessName[MAX_PATH];
	DWORD dwProcessId;
	CHAR szLocalPort[MAXCHAR];
	CHAR szRemotePort[MAXCHAR];
	CHAR szStatus[MAXCHAR];
}PROTOCOLTABLEA, *PPROTOCOLTABLEA;

typedef struct tagProtocolTableW {
	WCHAR szProtocolName[MAXCHAR];
	WCHAR szProcessName[MAX_PATH];
	DWORD dwProcessId;
	WCHAR szLocalPort[MAXCHAR];
	WCHAR szRemotePort[MAXCHAR];
	WCHAR szStatus[MAXCHAR];
}PROTOCOLTABLEW, *PPROTOCOLTABLEW;

#if !defined(_UNICODE) && !defined(UNICODE)
typedef PROTOCOLTABLEA PROTOCOLTABLE;
#else
typedef PROTOCOLTABLEW PROTOCOLTABLE;
#endif

class CProtocolsConnection
{
public:
	CProtocolsConnection() {}
	~CProtocolsConnection() {}

public:
	int GetProtocolsConnectionInfo()
	{
		// 定义扩展函数指针
		PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;
		PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;

		// 获取扩展函数的入口地址	
		HMODULE hModule = ::GetModuleHandle(_T("IPHLPAPI.DLL"));

		pAllocateAndGetTcpExTableFromStack =
			(PFNAllocateAndGetTcpExTableFromStack)::GetProcAddress(hModule,
				"AllocateAndGetTcpExTableFromStack");

		pAllocateAndGetUdpExTableFromStack =
			(PFNAllocateAndGetUdpExTableFromStack)::GetProcAddress(hModule,
				"AllocateAndGetUdpExTableFromStack");

		if (pAllocateAndGetTcpExTableFromStack == NULL || pAllocateAndGetUdpExTableFromStack == NULL)
		{
			printf(" Ex APIs are not present \n ");
			// 说明你应该调用普通的IP帮助API去获取TCP连接表和UDP监听表
			return 0;
		}

		// 调用扩展函数，获取TCP扩展连接表和UDP扩展监听表

		PMIB_TCPEXTABLE pTcpExTable;
		PMIB_UDPEXTABLE pUdpExTable;

		// pTcpExTable和pUdpExTable所指的缓冲区自动由扩展函数在进程堆中申请
		if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)
		{
			printf(" Failed to snapshot TCP endpoints.\n");
			return -1;
		}
		if (pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)
		{
			printf(" Failed to snapshot UDP endpoints.\n");
			return -1;
		}

		// 给系统内的所有进程拍一个快照
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			printf(" Failed to take process snapshot. Process names will not be shown.\n\n");
			return -1;
		}

		printf(" Active Connections \n\n");
		char	szLocalAddr[128];
		char	szRemoteAddr[128];
		_TCHAR	szProcessName[128];
		in_addr inadLocal, inadRemote;
		char    strState[128];
		DWORD   dwRemotePort = 0;
		UINT i = 0;

		// 打印TCP扩展连接表信息
		for (i = 0; i < pTcpExTable->dwNumEntries; ++i)
		{
			// 状态
			switch (pTcpExTable->table[i].dwState)
			{
			case MIB_TCP_STATE_CLOSED:
				strcpy(strState, "CLOSED");
				break;
			case MIB_TCP_STATE_TIME_WAIT:
				strcpy(strState, "TIME_WAIT");
				break;
			case MIB_TCP_STATE_LAST_ACK:
				strcpy(strState, "LAST_ACK");
				break;
			case MIB_TCP_STATE_CLOSING:
				strcpy(strState, "CLOSING");
				break;
			case MIB_TCP_STATE_CLOSE_WAIT:
				strcpy(strState, "CLOSE_WAIT");
				break;
			case MIB_TCP_STATE_FIN_WAIT1:
				strcpy(strState, "FIN_WAIT1");
				break;
			case MIB_TCP_STATE_ESTAB:
				strcpy(strState, "ESTAB");
				break;
			case MIB_TCP_STATE_SYN_RCVD:
				strcpy(strState, "SYN_RCVD");
				break;
			case MIB_TCP_STATE_SYN_SENT:
				strcpy(strState, "SYN_SENT");
				break;
			case MIB_TCP_STATE_LISTEN:
				strcpy(strState, "LISTEN");
				break;
			case MIB_TCP_STATE_DELETE_TCB:
				strcpy(strState, "DELETE");
				break;
			default:
				printf("Error: unknown state!\n");
				break;
			}
			// 本地IP地址
			inadLocal.s_addr = pTcpExTable->table[i].dwLocalAddr;

			// 远程端口
			if (strcmp(strState, "LISTEN") != 0)
			{
				dwRemotePort = pTcpExTable->table[i].dwRemotePort;
			}
			else
			{
				dwRemotePort = 0;
			}
			// 远程IP地址
			inadRemote.s_addr = pTcpExTable->table[i].dwRemoteAddr;
			
			sprintf(szLocalAddr, "%s:%u", inet_ntoa(inadLocal),
				ntohs((unsigned short)(0x0000FFFF & pTcpExTable->table[i].dwLocalPort)));
			sprintf(szRemoteAddr, "%s:%u", inet_ntoa(inadRemote),
				ntohs((unsigned short)(0x0000FFFF & dwRemotePort)));

			// 打印出此入口的信息
			printf("%-5s %s:%d\n      State:   %s\n", "[TCP]",
				ProcessPidToName(szProcessName, pTcpExTable->table[i].dwProcessId, hProcessSnap),
				pTcpExTable->table[i].dwProcessId,
				strState);

			printf("      Local:   %s\n      Remote:  %s\n",
				szLocalAddr, szRemoteAddr);
		}

		// 打印UDP监听表信息
		for (i = 0; i < pUdpExTable->dwNumEntries; ++i)
		{
			// 本地IP地址
			inadLocal.s_addr = pUdpExTable->table[i].dwLocalAddr;

			sprintf(szLocalAddr, "%s:%u", inet_ntoa(inadLocal),
				ntohs((unsigned short)(0x0000FFFF & pUdpExTable->table[i].dwLocalPort)));

			// 打印出此入口的信息
			printf("%-5s %s:%d\n", "[UDP]",
				ProcessPidToName(szProcessName, pUdpExTable->table[i].dwProcessId, hProcessSnap),
				pUdpExTable->table[i].dwProcessId);
			printf("      Local:   %s\n      Remote:  %s\n",
				szLocalAddr, "*.*.*.*:*");
		}


		::CloseHandle(hProcessSnap);
		hProcessSnap = NULL;
		::LocalFree(pTcpExTable);
		pTcpExTable = NULL;
		::LocalFree(pUdpExTable);
		pUdpExTable = NULL;

		return 0;
	}
	BOOL GetProtocolsConnectionInfoList(DWORD dwOwningId = (-1L))
	{
		BOOL bResult = FALSE;
		PVOID pTcpTable = NULL;
		DWORD dwTcpTableSize = 0;
		PVOID pUdpTable = NULL;
		DWORD dwUdpTableSize = 0;
		BOOL bOrder = FALSE;
		ULONG ulAf = AF_INET;
		DWORD dwIndex = 0L;
		PROTOCOLTABLE tt = { 0 };
		PROTOCOLTABLE ut = { 0 };
		_TCHAR tzStatus[MAXCHAR] = { 0 };
		struct in_addr localaddr = { 0 };
		struct in_addr remoteaddr = { 0 };
		_TCHAR tzLocalAddr[MAXCHAR] = { 0 };
		_TCHAR tzRemoteAddr[MAXCHAR] = { 0 };
		_TCHAR tzProcessName[MAX_PATH] = { 0 };
		PMIB_TCPTABLE_OWNER_MODULE pMibTcpTable2 = NULL;
		PMIB_UDPTABLE_OWNER_MODULE pMibUdpTable2 = NULL;
		PFNGetExtendedTcpTable fnGetExtendedTcpTable = NULL;
		PFNGetExtendedUdpTable fnGetExtendedUdpTable = NULL;

		HMODULE hModule = ::GetModuleHandle(_T("IPHLPAPI.DLL"));

		USES_CONVERSION;

		if (hModule)
		{
			fnGetExtendedTcpTable = (PFNGetExtendedTcpTable)GetProcAddress(hModule, "GetExtendedTcpTable");
			fnGetExtendedUdpTable = (PFNGetExtendedUdpTable)GetProcAddress(hModule, "GetExtendedUdpTable");

			if (!fnGetExtendedTcpTable || !fnGetExtendedUdpTable)
			{
				goto __LEAVE_CLEAN__;
			}
	
			m_tcp_list.clear();
			m_udp_list.clear();
			m_all_list.clear();

			if (fnGetExtendedTcpTable(pTcpTable, &dwTcpTableSize, bOrder, ulAf, TCP_TABLE_OWNER_MODULE_ALL, 0) == ERROR_INSUFFICIENT_BUFFER)
			{
				pTcpTable = malloc(dwTcpTableSize);
				if (fnGetExtendedTcpTable(pTcpTable, &dwTcpTableSize, bOrder, ulAf, TCP_TABLE_OWNER_MODULE_ALL, 0) != NO_ERROR)
				{
					goto __LEAVE_CLEAN__;
				}
			}
			if (fnGetExtendedUdpTable(pTcpTable, &dwUdpTableSize, bOrder, ulAf, UDP_TABLE_OWNER_MODULE, 0) == ERROR_INSUFFICIENT_BUFFER)
			{
				pUdpTable = malloc(dwUdpTableSize);
				if (fnGetExtendedUdpTable(pUdpTable, &dwUdpTableSize, bOrder, ulAf, UDP_TABLE_OWNER_MODULE, 0) != NO_ERROR)
				{
					goto __LEAVE_CLEAN__;
				}
			}
#if defined(DEBUG) || defined(_DEBUG)
			_tprintf(_T("==================================================TCP=================================================\n"));
#endif
			pMibTcpTable2 = (PMIB_TCPTABLE_OWNER_MODULE)pTcpTable;
			for (dwIndex = 0; dwIndex < pMibTcpTable2->dwNumEntries; dwIndex++)
			{
				memset(&tt, 0, sizeof(tt));
				memset(tzStatus, 0, sizeof(tzStatus));
				memset(&localaddr, 0, sizeof(localaddr));
				memset(&remoteaddr, 0, sizeof(remoteaddr));
				memset(&tzLocalAddr, 0, sizeof(tzLocalAddr));
				memset(&tzRemoteAddr, 0, sizeof(tzRemoteAddr));
				memset(&tzProcessName, 0, sizeof(tzProcessName));
				if ((dwOwningId == (-1L)) || (pMibTcpTable2->table[dwIndex].dwOwningPid == dwOwningId))
				{
					localaddr.s_addr = pMibTcpTable2->table[dwIndex].dwLocalAddr;
					lstrcpy(tzLocalAddr, A2T(inet_ntoa(localaddr)));
					remoteaddr.s_addr = pMibTcpTable2->table[dwIndex].dwRemoteAddr;
					lstrcpy(tzRemoteAddr, A2T(inet_ntoa(remoteaddr)));

#if defined(DEBUG) || defined(_DEBUG)
					_tprintf(_T("协议名称:TCP,进程名称:%s,进程ID:%ld,本地:%s:%ld,远程:%s:%ld,状态:"),
						ProcessPidToName(tzProcessName, pMibTcpTable2->table[dwIndex].dwOwningPid),
						pMibTcpTable2->table[dwIndex].dwOwningPid,
						tzLocalAddr, (ntohs(pMibTcpTable2->table[dwIndex].dwLocalPort)),
						tzRemoteAddr, (ntohs(pMibTcpTable2->table[dwIndex].dwRemotePort)));
#endif
					lstrcpy(tt.szProtocolName, _T("TCP"));
					lstrcpy(tt.szProcessName, ProcessPidToName(tzProcessName, pMibTcpTable2->table[dwIndex].dwOwningPid));
					tt.dwProcessId = pMibTcpTable2->table[dwIndex].dwOwningPid;
					wsprintf(tt.szLocalPort, _T("%s:%ld"), tzLocalAddr, ntohs(pMibTcpTable2->table[dwIndex].dwLocalPort));
					wsprintf(tt.szRemotePort, _T("%s:%ld"), tzRemoteAddr, ntohs(pMibTcpTable2->table[dwIndex].dwRemotePort));

					// 状态
					switch (pMibTcpTable2->table[dwIndex].dwState)
					{
					case MIB_TCP_STATE_CLOSED:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("CLOSED"));
#endif
						lstrcat(tzStatus, _T("CLOSED"));
						break;
					case MIB_TCP_STATE_TIME_WAIT:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("TIME_WAIT"));
#endif
						lstrcat(tzStatus, _T("TIME_WAIT"));
						break;
					case MIB_TCP_STATE_LAST_ACK:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("LAST_ACK"));
#endif
						lstrcat(tzStatus, _T("LAST_ACK"));
						break;
					case MIB_TCP_STATE_CLOSING:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("CLOSING"));
#endif
						lstrcat(tzStatus, _T("CLOSING"));
						break;
					case MIB_TCP_STATE_CLOSE_WAIT:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("CLOSE_WAIT"));
#endif
						lstrcat(tzStatus, _T("CLOSE_WAIT"));
						break;
					case MIB_TCP_STATE_FIN_WAIT1:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("FIN_WAIT1"));
#endif
						lstrcat(tzStatus, _T("CLOSED"));
						break;
					case MIB_TCP_STATE_FIN_WAIT2:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("FIN_WAIT2"));
#endif
						lstrcat(tzStatus, _T("FIN_WAIT2"));
						break;
					case MIB_TCP_STATE_ESTAB:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("ESTAB"));
#endif
						lstrcat(tzStatus, _T("ESTAB"));
						break;
					case MIB_TCP_STATE_SYN_RCVD:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("SYN_RCVD"));
#endif
						lstrcat(tzStatus, _T("SYN_RCVD"));
						break;
					case MIB_TCP_STATE_SYN_SENT:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("SYN_SENT"));
#endif
						lstrcat(tzStatus, _T("SYN_SENT"));
						break;
					case MIB_TCP_STATE_LISTEN:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("LISTEN"));
#endif
						lstrcat(tzStatus, _T("LISTEN"));
						break;
					case MIB_TCP_STATE_DELETE_TCB:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("DELETE"));
#endif
						lstrcat(tzStatus, _T("DELETE"));
						break;
					default:
#if defined(DEBUG) || defined(_DEBUG)
						_tprintf(_T("Error: Unknown State"));
#endif
						lstrcat(tzStatus, _T("Error: Unknown State"));
						break;
					}
#if defined(DEBUG) || defined(_DEBUG)
					_tprintf(_T("\n=============================================================================================\n"));
#endif
					lstrcpy(tt.szStatus, tzStatus);
					m_tcp_list.push_back(tt);
					m_all_list.push_back(tt);
				}
			}

#if defined(DEBUG) || defined(_DEBUG)
			_tprintf(_T("==================================================UDP=================================================\n"));
#endif
			pMibUdpTable2 = (PMIB_UDPTABLE_OWNER_MODULE)pUdpTable;
			for (dwIndex = 0; dwIndex < pMibUdpTable2->dwNumEntries; dwIndex++)
			{
				memset(&ut, 0, sizeof(ut));
				memset(tzStatus, 0, sizeof(tzStatus));
				memset(&localaddr, 0, sizeof(localaddr));
				memset(&remoteaddr, 0, sizeof(remoteaddr));
				memset(&tzLocalAddr, 0, sizeof(tzLocalAddr));
				memset(&tzRemoteAddr, 0, sizeof(tzRemoteAddr));
				memset(&tzProcessName, 0, sizeof(tzProcessName));
				if ((dwOwningId == (-1L)) || (pMibUdpTable2->table[dwIndex].dwOwningPid == dwOwningId))
				{
					localaddr.s_addr = pMibUdpTable2->table[dwIndex].dwLocalAddr;
					lstrcpy(tzLocalAddr, A2T(inet_ntoa(localaddr)));
#if defined(DEBUG) || defined(_DEBUG)
					_tprintf(_T("协议名称:UDP,进程名称:%s,进程ID:%ld,本地:%s:%ld"),
						ProcessPidToName(tzProcessName, pMibUdpTable2->table[dwIndex].dwOwningPid),
						pMibUdpTable2->table[dwIndex].dwOwningPid,
						tzLocalAddr, ntohs(pMibUdpTable2->table[dwIndex].dwLocalPort));
#endif
					lstrcpy(ut.szProtocolName, _T("UDP"));
					lstrcpy(ut.szProcessName, ProcessPidToName(tzProcessName, pMibUdpTable2->table[dwIndex].dwOwningPid));
					ut.dwProcessId = pMibUdpTable2->table[dwIndex].dwOwningPid;
					wsprintf(ut.szLocalPort, _T("%s:%ld"), tzLocalAddr, ntohs(pMibUdpTable2->table[dwIndex].dwLocalPort));
					
#if defined(DEBUG) || defined(_DEBUG)
					_tprintf(_T("\n=============================================================================================\n"));
#endif
					m_udp_list.push_back(ut);
					m_all_list.push_back(ut);
				}
			}

			bResult = m_all_list.size();
		}

	__LEAVE_CLEAN__:
		
		if (pUdpTable)
		{
			free(pUdpTable);
			pUdpTable = NULL;
		}
		if (pTcpTable)
		{
			free(pTcpTable);
			pTcpTable = NULL;
		}

		return bResult;
	}

public:
	std::vector<PROTOCOLTABLE> m_tcp_list;
	std::vector<PROTOCOLTABLE> m_udp_list;
	std::vector<PROTOCOLTABLE> m_all_list;
};