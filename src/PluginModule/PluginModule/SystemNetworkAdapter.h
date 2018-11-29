#pragma once

BOOL GetSystemNetworkAdapterList()
{
	// MAC 地址信息
	_TCHAR tzMacAddr[0x12] = { 0 };
	// 指针网卡信息列表
	PIP_ADAPTER_INFO pIpAdapterInfoIterator = NULL;
	// 指向IP地址列表
	PIP_ADDR_STRING pIpAddrStringIterator = NULL;
	// 指向网关列表
	PIP_ADDR_STRING pIpAddrStringGateAwayIterator = NULL;
	// IP_ADAPTER_INFO结构体
	PIP_ADAPTER_INFO pIpAdapterInfoList = NULL;
	// 结构体大小
	ULONG nIpAdapterInfoListSize = 0L;
	//DNS结构体
	PIP_PER_ADAPTER_INFO pPerAdapterInfoList = NULL;
	// 结构体大小
	ULONG nPerAdapterInfoListSize = 0L;

	//获取适配器信息
	ULONG nResult = 0L;
	
	USES_CONVERSION;

	nResult = GetAdaptersInfo(pIpAdapterInfoList, &nIpAdapterInfoListSize);

	if (ERROR_BUFFER_OVERFLOW == nResult)
	{
		//空间不足，分配大小
		pIpAdapterInfoList = (PIP_ADAPTER_INFO)new BYTE[nIpAdapterInfoListSize];

		//获取适配器信息
		nResult = GetAdaptersInfo(pIpAdapterInfoList, &nIpAdapterInfoListSize);

		//获取失败
		if (ERROR_SUCCESS != nResult)
		{
			goto __LEAVE_CLEAN__;
		}
	}

	//赋值指针
	pIpAdapterInfoIterator = pIpAdapterInfoList;
	while (pIpAdapterInfoIterator)
	{
		_tprintf(_T("网卡名称：%s\r\n"), A2T(pIpAdapterInfoIterator->AdapterName));
		_tprintf(_T("网卡描述：%s\r\n"), A2T(pIpAdapterInfoIterator->Description));

		wsprintf(tzMacAddr, _T("%02X-%02X-%02X-%02X-%02X-%02X"),
			pIpAdapterInfoIterator->Address[0],
			pIpAdapterInfoIterator->Address[1],
			pIpAdapterInfoIterator->Address[2],
			pIpAdapterInfoIterator->Address[3],
			pIpAdapterInfoIterator->Address[4],
			pIpAdapterInfoIterator->Address[5]);
		_tprintf(_T("MAC 地址：%s\r\n"), tzMacAddr);

		//指向Dhcp Server列表
		_tprintf(_T("是否启用DHCP：%s\r\n"), pIpAdapterInfoIterator->DhcpEnabled ? _T("已启用"):_T("未启用"));
		_tprintf(_T("DHCP地址：%s\r\n"), A2T(pIpAdapterInfoIterator->DhcpServer.IpAddress.String));
		_tprintf(_T("DHCP网关地址：%s\r\n"), A2T(pIpAdapterInfoIterator->DhcpServer.IpMask.String));
		
		_tprintf(_T("IP地址列表：\r\n"));

		//指向IP地址列表
		pIpAddrStringIterator = &pIpAdapterInfoIterator->IpAddressList;
		while (pIpAddrStringIterator)
		{
			_tprintf(_T("IP地址：  %s\r\n"), A2T(pIpAddrStringIterator->IpAddress.String));
			_tprintf(_T("IP子网掩码：  %s\r\n"), A2T(pIpAddrStringIterator->IpMask.String));

			//指向网关列表
			pIpAddrStringGateAwayIterator = &pIpAdapterInfoIterator->GatewayList;
			while (pIpAddrStringGateAwayIterator)
			{
				_tprintf(_T("网关：    %s\r\n"), A2T(pIpAddrStringGateAwayIterator->IpAddress.String));

				pIpAddrStringGateAwayIterator = pIpAddrStringGateAwayIterator->Next;
			}

			pIpAddrStringIterator = pIpAddrStringIterator->Next;
		}
		nResult = GetPerAdapterInfo(pIpAdapterInfoIterator->Index, pPerAdapterInfoList, &nPerAdapterInfoListSize);
		if (nResult == ERROR_BUFFER_OVERFLOW)
		{
			//空间不足，分配大小
			pPerAdapterInfoList = (PIP_PER_ADAPTER_INFO)new BYTE[nPerAdapterInfoListSize];

			//获取适配器信息
			nResult = GetPerAdapterInfo(pIpAdapterInfoIterator->Index, pPerAdapterInfoList, &nPerAdapterInfoListSize);

			//获取失败
			if (ERROR_SUCCESS != nResult)
			{
				goto __LEAVE_CLEAN__;
			}
		}
		_tprintf(_T("自动配置：  %s\r\n"), pPerAdapterInfoList->AutoconfigEnabled?_T("已启用"):_T("未启用"));
		//指向IP地址列表
		pIpAddrStringIterator = &pPerAdapterInfoList->DnsServerList;
		if (pIpAddrStringIterator && lstrcmp(A2T(pIpAddrStringIterator->IpAddress.String), _T("")) != 0)//手动DNS
		{
			_tprintf(_T("DNS：  %s\r\n"), A2T(pIpAddrStringIterator->IpAddress.String));
			if (pIpAddrStringIterator->Next)
			{
				pIpAddrStringIterator = pIpAddrStringIterator->Next;
				_tprintf(_T("备用DNS：  %s\r\n"), A2T(pIpAddrStringIterator->IpAddress.String));
			}
		}
		
		_tprintf(_T("--------------------------------------------------\r\n"));

		pIpAdapterInfoIterator = pIpAdapterInfoIterator->Next;
	}

__LEAVE_CLEAN__:

	//清理
	if (pPerAdapterInfoList)
	{
		delete[]pPerAdapterInfoList;
		pPerAdapterInfoList = NULL;
	}

	if (pIpAdapterInfoList)
	{
		delete[]pIpAdapterInfoList;
		pIpAdapterInfoList = NULL;
	}

	return TRUE;
}

/*
#define NTDDI_VERSION NTDDI_WINXP
#include <ws2tcpip.h>
BOOL GetSystemNetworkAdapterExList()
{
	ULONG ulResult = 0L;
	ULONG ulFlags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;//包括 IPV4 ，IPV6 网关
	ULONG ulFamily = AF_UNSPEC;//返回包括 IPV4 和 IPV6 地址
	PIP_ADAPTER_ADDRESSES pIpAdapterAddressesList = NULL;
	ULONG ulIpAdapterAddressesListSize = 0;
	
	PIP_ADAPTER_ADDRESSES pIpAdapterAddressesIterator = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
	PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
	PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
	IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
	IP_ADAPTER_PREFIX *pPrefix = NULL;

	ulResult = GetAdaptersAddresses(ulFamily, ulFlags, NULL, pIpAdapterAddressesList, &ulIpAdapterAddressesListSize);
	if (ulResult == ERROR_BUFFER_OVERFLOW)
	{
		pIpAdapterAddressesList = (IP_ADAPTER_ADDRESSES *)malloc(ulIpAdapterAddressesListSize);
		ulResult = GetAdaptersAddresses(ulFamily, ulFlags, NULL, pIpAdapterAddressesList, &ulIpAdapterAddressesListSize);
	}

	if (ulResult == NO_ERROR)
	{
		pIpAdapterAddressesIterator = pIpAdapterAddressesList;
		while (pIpAdapterAddressesIterator)
		{
			_tprintf(_T("Adapter Name:%s\r\n"), pIpAdapterAddressesIterator->AdapterName);
			_tprintf(_T("Adapter Description:%s\r\n"), pIpAdapterAddressesIterator->Description);
			_tprintf(_T("Adapter Friendly Name:%s\r\n"), pIpAdapterAddressesIterator->FriendlyName);

			if (pIpAdapterAddressesIterator->PhysicalAddressLength != 0)
			{
				_TCHAR tzMacAddr[0x12] = { 0 };
				//其实 MAC 地址的长度存在 PhysicalAddressLength 中，最好用它来确定格式化的长度
				wsprintf(tzMacAddr, _T("%02X-%02X-%02X-%02X-%02X-%02X"),
					pIpAdapterAddressesIterator->PhysicalAddress[0],
					pIpAdapterAddressesIterator->PhysicalAddress[1],
					pIpAdapterAddressesIterator->PhysicalAddress[2],
					pIpAdapterAddressesIterator->PhysicalAddress[3],
					pIpAdapterAddressesIterator->PhysicalAddress[4],
					pIpAdapterAddressesIterator->PhysicalAddress[5]);
				_tprintf(_T("Adapter Mac: %s\r\n"), tzMacAddr);
			}
			_TCHAR tzType[MAXBYTE] = { 0 };
			switch (pIpAdapterAddressesIterator->IfType) //类型，列举了几种
			{
			case MIB_IF_TYPE_ETHERNET:
				lstrcpy(tzType, _T("以太网接口"));
				break;
			case MIB_IF_TYPE_PPP:
				lstrcpy(tzType, _T("PPP接口"));
				break;
			case MIB_IF_TYPE_LOOPBACK:
				lstrcpy(tzType, _T("软件回路接口"));
				break;
			case MIB_IF_TYPE_SLIP:
				lstrcpy(tzType, _T("ATM网络接口"));
				break;
			case IF_TYPE_IEEE80211:
				lstrcpy(tzType, _T("无线网络接口"));
				break;
			}
			_tprintf(_T("网卡类型: %s\r\n"), tzType);

			pUnicast = pIpAdapterAddressesIterator->FirstUnicastAddress;
			while (pUnicast)//单播IP
			{
				CHAR szIPAddr[MAXCHAR] = { 0 };
				if (AF_INET == pUnicast->Address.lpSockaddr->sa_family)// IPV4 地址，使用 IPV4 转换
				{
					inet_ntop(PF_INET, &((sockaddr_in*)pUnicast->Address.lpSockaddr)->sin_addr, szIPAddr, sizeof(szIPAddr));
				}
				else if (AF_INET6 == pUnicast->Address.lpSockaddr->sa_family)// IPV6 地址，使用 IPV6 转换
				{
					inet_ntop(PF_INET6, &((sockaddr_in6*)pUnicast->Address.lpSockaddr)->sin6_addr, szIPAddr, sizeof(szIPAddr));
				}
				_tprintf(_T("单播IP: %s\r\n"), szIPAddr);
				pUnicast = pUnicast->Next;
			}
			if (pIpAdapterAddressesIterator->Dhcpv4Server.lpSockaddr)//DHCP服务器地址
			{
				CHAR szDhcp[MAXCHAR] = { 0 };
				if (AF_INET == pIpAdapterAddressesIterator->Dhcpv4Server.lpSockaddr->sa_family)
				{
					inet_ntop(PF_INET, &((sockaddr_in*)pIpAdapterAddressesIterator->Dhcpv4Server.lpSockaddr)->sin_addr, szDhcp, sizeof(szDhcp));
				}
				else if (AF_INET6 == pIpAdapterAddressesIterator->Dhcpv4Server.lpSockaddr->sa_family)
				{
					inet_ntop(PF_INET6, &((sockaddr_in6*)pIpAdapterAddressesIterator->Dhcpv4Server.lpSockaddr)->sin6_addr, szDhcp, sizeof(szDhcp));
				}
				_tprintf(_T("DHCP地址: %s\r\n"), szDhcp);
			}
			pDnServer = pIpAdapterAddressesIterator->FirstDnsServerAddress;
			while (pDnServer)//DNS
			{
				CHAR szDNS[MAXCHAR] = { 0 };
				if (AF_INET == pDnServer->Address.lpSockaddr->sa_family)
				{
					inet_ntop(PF_INET, &((sockaddr_in*)pDnServer->Address.lpSockaddr)->sin_addr, szDNS, sizeof(szDNS));
				}
				else if (AF_INET6 == pDnServer->Address.lpSockaddr->sa_family)
				{
					inet_ntop(PF_INET6, &((sockaddr_in6*)pDnServer->Address.lpSockaddr)->sin6_addr, szDNS, sizeof(szDNS));
				}
				_tprintf(_T("DNS: %s\r\n"), szDNS);

				pDnServer = pDnServer->Next;
			}
			_tprintf(_T("MTU: %ld\r\n"), pIpAdapterAddressesIterator->Mtu);

			//发送速率
			//_tprintf(_T("Send speed: %ld\r\n"), pIpAdapterAddressesIterator->TransmitLinkSpeed);

			//接收速率
			//_tprintf(_T("Recv speed: %ld\r\n"), pIpAdapterAddressesIterator->ReceiveLinkSpeed);
			PIP_ADAPTER_GATEWAY_ADDRESS_LH pGateway = pIpAdapterAddressesIterator->FirstGatewayAddress;
			while (pGateway)//网关
			{
				CHAR szGateWay[MAXCHAR] = { 0 };
				if (AF_INET == pGateway->Address.lpSockaddr->sa_family)
				{
					inet_ntop(PF_INET, &((sockaddr_in*)pGateway->Address.lpSockaddr)->sin_addr, szGateWay, sizeof(szGateWay));
				}
				else if (AF_INET6 == pGateway->Address.lpSockaddr->sa_family)
				{
					inet_ntop(PF_INET6, &((sockaddr_in6*)pGateway->Address.lpSockaddr)->sin6_addr, szGateWay, sizeof(szGateWay));
				}
				_tprintf(_T("Gateway: %s\r\n"), szGateWay);
				pGateway = pGateway->Next;
			}
			if (pIpAdapterAddressesIterator->Dhcpv6Server.lpSockaddr)//IPV6DHCP
			{
				CHAR szDhcpv6[MAXCHAR] = { 0 };
				if (AF_INET6 == pIpAdapterAddressesIterator->Dhcpv6Server.lpSockaddr->sa_family)
				{
					inet_ntop(PF_INET6, &((sockaddr_in6*)pIpAdapterAddressesIterator->Dhcpv6Server.lpSockaddr)->sin6_addr, szDhcpv6, sizeof(szDhcpv6));
					_tprintf(_T("DHCPv6: %s\r\n"), szDhcpv6);
				}
			}

			pIpAdapterAddressesIterator = pIpAdapterAddressesIterator->Next;
		}
	}
	else
	{
		_tprintf(_T("GetAdaptersAddresses failed, error code : %d\n"), GetLastError());
	}
	if (pIpAdapterAddressesIterator)
	{
		free(pIpAdapterAddressesIterator);
		pIpAdapterAddressesIterator = NULL;
	}
	return TRUE;
}*/