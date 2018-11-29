#pragma once

BOOL GetSystemNetworkAdapterList()
{
	// MAC ��ַ��Ϣ
	_TCHAR tzMacAddr[0x12] = { 0 };
	// ָ��������Ϣ�б�
	PIP_ADAPTER_INFO pIpAdapterInfoIterator = NULL;
	// ָ��IP��ַ�б�
	PIP_ADDR_STRING pIpAddrStringIterator = NULL;
	// ָ�������б�
	PIP_ADDR_STRING pIpAddrStringGateAwayIterator = NULL;
	// IP_ADAPTER_INFO�ṹ��
	PIP_ADAPTER_INFO pIpAdapterInfoList = NULL;
	// �ṹ���С
	ULONG nIpAdapterInfoListSize = 0L;
	//DNS�ṹ��
	PIP_PER_ADAPTER_INFO pPerAdapterInfoList = NULL;
	// �ṹ���С
	ULONG nPerAdapterInfoListSize = 0L;

	//��ȡ��������Ϣ
	ULONG nResult = 0L;
	
	USES_CONVERSION;

	nResult = GetAdaptersInfo(pIpAdapterInfoList, &nIpAdapterInfoListSize);

	if (ERROR_BUFFER_OVERFLOW == nResult)
	{
		//�ռ䲻�㣬�����С
		pIpAdapterInfoList = (PIP_ADAPTER_INFO)new BYTE[nIpAdapterInfoListSize];

		//��ȡ��������Ϣ
		nResult = GetAdaptersInfo(pIpAdapterInfoList, &nIpAdapterInfoListSize);

		//��ȡʧ��
		if (ERROR_SUCCESS != nResult)
		{
			goto __LEAVE_CLEAN__;
		}
	}

	//��ֵָ��
	pIpAdapterInfoIterator = pIpAdapterInfoList;
	while (pIpAdapterInfoIterator)
	{
		_tprintf(_T("�������ƣ�%s\r\n"), A2T(pIpAdapterInfoIterator->AdapterName));
		_tprintf(_T("����������%s\r\n"), A2T(pIpAdapterInfoIterator->Description));

		wsprintf(tzMacAddr, _T("%02X-%02X-%02X-%02X-%02X-%02X"),
			pIpAdapterInfoIterator->Address[0],
			pIpAdapterInfoIterator->Address[1],
			pIpAdapterInfoIterator->Address[2],
			pIpAdapterInfoIterator->Address[3],
			pIpAdapterInfoIterator->Address[4],
			pIpAdapterInfoIterator->Address[5]);
		_tprintf(_T("MAC ��ַ��%s\r\n"), tzMacAddr);

		//ָ��Dhcp Server�б�
		_tprintf(_T("�Ƿ�����DHCP��%s\r\n"), pIpAdapterInfoIterator->DhcpEnabled ? _T("������"):_T("δ����"));
		_tprintf(_T("DHCP��ַ��%s\r\n"), A2T(pIpAdapterInfoIterator->DhcpServer.IpAddress.String));
		_tprintf(_T("DHCP���ص�ַ��%s\r\n"), A2T(pIpAdapterInfoIterator->DhcpServer.IpMask.String));
		
		_tprintf(_T("IP��ַ�б�\r\n"));

		//ָ��IP��ַ�б�
		pIpAddrStringIterator = &pIpAdapterInfoIterator->IpAddressList;
		while (pIpAddrStringIterator)
		{
			_tprintf(_T("IP��ַ��  %s\r\n"), A2T(pIpAddrStringIterator->IpAddress.String));
			_tprintf(_T("IP�������룺  %s\r\n"), A2T(pIpAddrStringIterator->IpMask.String));

			//ָ�������б�
			pIpAddrStringGateAwayIterator = &pIpAdapterInfoIterator->GatewayList;
			while (pIpAddrStringGateAwayIterator)
			{
				_tprintf(_T("���أ�    %s\r\n"), A2T(pIpAddrStringGateAwayIterator->IpAddress.String));

				pIpAddrStringGateAwayIterator = pIpAddrStringGateAwayIterator->Next;
			}

			pIpAddrStringIterator = pIpAddrStringIterator->Next;
		}
		nResult = GetPerAdapterInfo(pIpAdapterInfoIterator->Index, pPerAdapterInfoList, &nPerAdapterInfoListSize);
		if (nResult == ERROR_BUFFER_OVERFLOW)
		{
			//�ռ䲻�㣬�����С
			pPerAdapterInfoList = (PIP_PER_ADAPTER_INFO)new BYTE[nPerAdapterInfoListSize];

			//��ȡ��������Ϣ
			nResult = GetPerAdapterInfo(pIpAdapterInfoIterator->Index, pPerAdapterInfoList, &nPerAdapterInfoListSize);

			//��ȡʧ��
			if (ERROR_SUCCESS != nResult)
			{
				goto __LEAVE_CLEAN__;
			}
		}
		_tprintf(_T("�Զ����ã�  %s\r\n"), pPerAdapterInfoList->AutoconfigEnabled?_T("������"):_T("δ����"));
		//ָ��IP��ַ�б�
		pIpAddrStringIterator = &pPerAdapterInfoList->DnsServerList;
		if (pIpAddrStringIterator && lstrcmp(A2T(pIpAddrStringIterator->IpAddress.String), _T("")) != 0)//�ֶ�DNS
		{
			_tprintf(_T("DNS��  %s\r\n"), A2T(pIpAddrStringIterator->IpAddress.String));
			if (pIpAddrStringIterator->Next)
			{
				pIpAddrStringIterator = pIpAddrStringIterator->Next;
				_tprintf(_T("����DNS��  %s\r\n"), A2T(pIpAddrStringIterator->IpAddress.String));
			}
		}
		
		_tprintf(_T("--------------------------------------------------\r\n"));

		pIpAdapterInfoIterator = pIpAdapterInfoIterator->Next;
	}

__LEAVE_CLEAN__:

	//����
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
	ULONG ulFlags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;//���� IPV4 ��IPV6 ����
	ULONG ulFamily = AF_UNSPEC;//���ذ��� IPV4 �� IPV6 ��ַ
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
				//��ʵ MAC ��ַ�ĳ��ȴ��� PhysicalAddressLength �У����������ȷ����ʽ���ĳ���
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
			switch (pIpAdapterAddressesIterator->IfType) //���ͣ��о��˼���
			{
			case MIB_IF_TYPE_ETHERNET:
				lstrcpy(tzType, _T("��̫���ӿ�"));
				break;
			case MIB_IF_TYPE_PPP:
				lstrcpy(tzType, _T("PPP�ӿ�"));
				break;
			case MIB_IF_TYPE_LOOPBACK:
				lstrcpy(tzType, _T("�����·�ӿ�"));
				break;
			case MIB_IF_TYPE_SLIP:
				lstrcpy(tzType, _T("ATM����ӿ�"));
				break;
			case IF_TYPE_IEEE80211:
				lstrcpy(tzType, _T("��������ӿ�"));
				break;
			}
			_tprintf(_T("��������: %s\r\n"), tzType);

			pUnicast = pIpAdapterAddressesIterator->FirstUnicastAddress;
			while (pUnicast)//����IP
			{
				CHAR szIPAddr[MAXCHAR] = { 0 };
				if (AF_INET == pUnicast->Address.lpSockaddr->sa_family)// IPV4 ��ַ��ʹ�� IPV4 ת��
				{
					inet_ntop(PF_INET, &((sockaddr_in*)pUnicast->Address.lpSockaddr)->sin_addr, szIPAddr, sizeof(szIPAddr));
				}
				else if (AF_INET6 == pUnicast->Address.lpSockaddr->sa_family)// IPV6 ��ַ��ʹ�� IPV6 ת��
				{
					inet_ntop(PF_INET6, &((sockaddr_in6*)pUnicast->Address.lpSockaddr)->sin6_addr, szIPAddr, sizeof(szIPAddr));
				}
				_tprintf(_T("����IP: %s\r\n"), szIPAddr);
				pUnicast = pUnicast->Next;
			}
			if (pIpAdapterAddressesIterator->Dhcpv4Server.lpSockaddr)//DHCP��������ַ
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
				_tprintf(_T("DHCP��ַ: %s\r\n"), szDhcp);
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

			//��������
			//_tprintf(_T("Send speed: %ld\r\n"), pIpAdapterAddressesIterator->TransmitLinkSpeed);

			//��������
			//_tprintf(_T("Recv speed: %ld\r\n"), pIpAdapterAddressesIterator->ReceiveLinkSpeed);
			PIP_ADAPTER_GATEWAY_ADDRESS_LH pGateway = pIpAdapterAddressesIterator->FirstGatewayAddress;
			while (pGateway)//����
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