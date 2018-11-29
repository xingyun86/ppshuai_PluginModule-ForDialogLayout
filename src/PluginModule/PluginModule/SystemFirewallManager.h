#pragma once

#include <netfw.h>
typedef struct tagNetworksFirewallInstance
{
	INetFwMgr *pNetFwMgr;
	INetFwProfile *pNetFwProfile;
	INetFwPolicy *pNetFwProlicy;
}NETWORKSFIREWALLINSTANCE, *PNETWORKSFIREWALLINSTANCE;

// 初始化防火墙管理接口
HRESULT InitNetFwMgr(NETWORKSFIREWALLINSTANCE * pNFI)
{
	HRESULT hResult = S_FALSE;

	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	//  Create an instance of the firewall settings manager. 

	hResult = CoCreateInstance(__uuidof(NetFwMgr), 0, CLSCTX_INPROC_SERVER, __uuidof(INetFwMgr), reinterpret_cast<void **>(&pNFI->pNetFwMgr));

	if (SUCCEEDED(hResult) && (pNFI->pNetFwMgr != NULL))
	{
		hResult = pNFI->pNetFwMgr->get_LocalPolicy(&pNFI->pNetFwProlicy);//  Retrieve the local firewall policy. 
		if (SUCCEEDED(hResult) && (pNFI->pNetFwProlicy != NULL))
		{
			hResult = pNFI->pNetFwProlicy->get_CurrentProfile(&pNFI->pNetFwProfile); //  Retrieve the firewall profile currently in effect 
		}
	}

	return hResult;
}
void ExitNetFwMgr(NETWORKSFIREWALLINSTANCE * pNFI)
{
	if (pNFI)
	{
		pNFI->pNetFwProfile->Release();
		pNFI->pNetFwProfile = NULL;
		pNFI->pNetFwProlicy->Release();
		pNFI->pNetFwProlicy = NULL;
		pNFI->pNetFwMgr->Release();
		pNFI->pNetFwMgr = NULL;
	}
}
HRESULT GetNetFwServices(INetFwServices ** ppNFSS)
{
	HRESULT hResult = S_FALSE;
	LONG nServicesCount = 0L;
	IEnumVARIANT * pEnumVariant = NULL;
	VARIANT_BOOL varEnabled = VARIANT_FALSE;

	USES_CONVERSION;

	_tprintf(_T("防火墙服务配置\r\n"));

	hResult = (*ppNFSS)->get_Count(&nServicesCount);
	_tprintf(_T("\t防火墙服务个数:%ld\r\n"), nServicesCount);

	hResult = (*ppNFSS)->get__NewEnum((IUnknown**)&pEnumVariant);
	if (SUCCEEDED(hResult))
	{
		ULONG nCeltFetched = 0L;
		VARIANT varCurrPosition = { 0 };
		VariantInit(&varCurrPosition);
		while (1)
		{
			hResult = pEnumVariant->Next(1, (VARIANT *)&varCurrPosition, (ULONG*)&nCeltFetched);
			if (SUCCEEDED(hResult))
			{
				if (nCeltFetched > 0L)
				{
					LONG nPort = 0L;
					BSTR bstrName = NULL;
					BSTR bstrRemoteAddresses = NULL;
					INetFwService * pNFS = NULL;
					IDispatch *pDisp = V_DISPATCH(&varCurrPosition);
					NET_FW_IP_VERSION nfiv = NET_FW_IP_VERSION_V4;
					NET_FW_SERVICE_TYPE nfst = NET_FW_SERVICE_FILE_AND_PRINT;
					if (SUCCEEDED(pDisp->QueryInterface(__uuidof(INetFwService), (void**)&pNFS)))
					{
						hResult = pNFS->get_Enabled(&varEnabled);
						_tprintf(_T("\t启用状态:%s\r\n"), varEnabled ? _T("已启用") : _T("未启用"));
						hResult = pNFS->get_IpVersion(&nfiv);
						_tprintf(_T("\t启用状态:%ld\r\n"), nfiv);
						hResult = pNFS->get_Name(&bstrName);
						if (FAILED(hResult))
						{
							bstrName = SysAllocString(_T(""));
						}
						_tprintf(_T("\t名称:%s\r\n"), W2T(bstrName));

						hResult = pNFS->get_RemoteAddresses(&bstrRemoteAddresses);
						if (FAILED(hResult))
						{
							bstrRemoteAddresses = SysAllocString(_T(""));
						}
						_tprintf(_T("\t远程地址:%s\r\n"), W2T(bstrRemoteAddresses));

						hResult = pNFS->get_Type(&nfst);
						_tprintf(_T("\t服务类型:%ld\r\n"), nfst);

						SysFreeString(bstrName);
						bstrName = NULL;
						SysFreeString(bstrRemoteAddresses);
						bstrRemoteAddresses = NULL;

						pNFS->Release();
						pNFS = NULL;
					}
				}
				else
				{
					break;
				}
			}
		}
		pEnumVariant->Release();
		pEnumVariant = NULL;
	}
	(*ppNFSS)->Release();
	(*ppNFSS) = NULL;

	return hResult;
}
HRESULT GetNetFwOpenPorts(INetFwOpenPorts ** ppNFOPS)
{
	HRESULT hResult = S_FALSE;
	LONG nOpenPortsCount = 0L;
	IEnumVARIANT * pEnumVariant = NULL;
	VARIANT_BOOL varEnabled = VARIANT_FALSE;

	USES_CONVERSION;

	_tprintf(_T("开放端口配置\r\n"));

	hResult = (*ppNFOPS)->get_Count(&nOpenPortsCount);
	_tprintf(_T("\t开放端口个数:%ld\r\n"), nOpenPortsCount);

	hResult = (*ppNFOPS)->get__NewEnum((IUnknown**)&pEnumVariant);
	if (SUCCEEDED(hResult))
	{
		ULONG nCeltFetched = 0L;
		VARIANT varCurrPosition = { 0 };
		VariantInit(&varCurrPosition);
		while (1)
		{
			hResult = pEnumVariant->Next(1, (VARIANT *)&varCurrPosition, (ULONG*)&nCeltFetched);
			if (SUCCEEDED(hResult))
			{
				if (nCeltFetched > 0L)
				{
					LONG nPort = 0L;
					BSTR bstrName = NULL;
					BSTR bstrRemoteAddresses = NULL;
					INetFwOpenPort * pNFOP = NULL;
					IDispatch *pDisp = V_DISPATCH(&varCurrPosition);
					NET_FW_IP_VERSION nfiv = NET_FW_IP_VERSION_V4;
					NET_FW_IP_PROTOCOL nfip = NET_FW_IP_PROTOCOL_TCP;
					if (SUCCEEDED(pDisp->QueryInterface(__uuidof(INetFwOpenPort), (void**)&pNFOP)))
					{
						hResult = pNFOP->get_Enabled(&varEnabled);
						_tprintf(_T("\t启用状态:%s\r\n"), varEnabled ? _T("已启用") : _T("未启用"));
						hResult = pNFOP->get_IpVersion(&nfiv);
						_tprintf(_T("\t启用状态:%ld\r\n"), nfiv);
						hResult = pNFOP->get_Name(&bstrName);
						if (FAILED(hResult))
						{
							bstrName = SysAllocString(_T(""));
						}
						_tprintf(_T("\t名称:%s\r\n"), W2T(bstrName));

						hResult = pNFOP->get_Port(&nPort);
						_tprintf(_T("\t端口:%ld\r\n"), nPort);
						hResult = pNFOP->get_Protocol(&nfip);
						_tprintf(_T("\t协议类型:%ld\r\n"), nfip);
						hResult = pNFOP->get_RemoteAddresses(&bstrRemoteAddresses);
						if (FAILED(hResult))
						{
							bstrRemoteAddresses = SysAllocString(_T(""));
						}
						_tprintf(_T("\t远程地址:%s\r\n"), W2T(bstrRemoteAddresses));

						SysFreeString(bstrName);
						bstrName = NULL;
						SysFreeString(bstrRemoteAddresses);
						bstrRemoteAddresses = NULL;

						pNFOP->Release();
						pNFOP = NULL;
					}
				}
				else
				{
					break;
				}
			}
		}
		pEnumVariant->Release();
		pEnumVariant = NULL;
	}
	(*ppNFOPS)->Release();
	(*ppNFOPS) = NULL;

	return hResult;
}
HRESULT GetNetFwIcmpSettings(INetFwIcmpSettings ** ppNFISS)
{
	HRESULT hResult = S_FALSE;
	VARIANT_BOOL bEnabled = VARIANT_FALSE;

	USES_CONVERSION;

	_tprintf(_T("ICMP配置\r\n"));

	hResult = (*ppNFISS)->get_AllowInboundEchoRequest(&bEnabled);
	_tprintf(_T("\tAllowInboundEchoRequest:%s\r\n"), bEnabled?_T("已启用"):_T("未启用"));
	hResult = (*ppNFISS)->get_AllowInboundMaskRequest(&bEnabled);
	_tprintf(_T("\tAllowInboundMaskRequest:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFISS)->get_AllowInboundRouterRequest(&bEnabled);
	_tprintf(_T("\tAllowInboundRouterRequest:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFISS)->get_AllowInboundTimestampRequest(&bEnabled);
	_tprintf(_T("\tAllowInboundTimestampRequest:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFISS)->get_AllowOutboundDestinationUnreachable(&bEnabled);
	_tprintf(_T("\tAllowOutboundDestinationUnreachable:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFISS)->get_AllowOutboundPacketTooBig(&bEnabled);
	_tprintf(_T("\tAllowOutboundPacketTooBig:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFISS)->get_AllowOutboundParameterProblem(&bEnabled);
	_tprintf(_T("\tAllowOutboundParameterProblem:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFISS)->get_AllowOutboundSourceQuench(&bEnabled);
	_tprintf(_T("\tAllowOutboundSourceQuench:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFISS)->get_AllowOutboundTimeExceeded(&bEnabled);
	_tprintf(_T("\tAllowOutboundTimeExceeded:%s\r\n"), bEnabled?_T("已启用"):_T("未启用"));
	hResult = (*ppNFISS)->get_AllowRedirect(&bEnabled);
	_tprintf(_T("\tAllowRedirect:%s\r\n"), bEnabled ? _T("已启用") : _T("未启用"));
	
	(*ppNFISS)->Release();
	(*ppNFISS) = NULL;

	return hResult;
}
HRESULT GetNetFwRemoteAdminSettings(INetFwRemoteAdminSettings ** ppNFRASS)
{
	HRESULT hResult = S_FALSE;
	BSTR bstrRemoteAddresses = NULL;
	VARIANT_BOOL varEnabled = VARIANT_FALSE;
	NET_FW_IP_VERSION nfiv = NET_FW_IP_VERSION_V4;

	USES_CONVERSION;
	_tprintf(_T("远程管理配置\r\n"));

	hResult = (*ppNFRASS)->get_Enabled(&varEnabled);
	_tprintf(_T("\t启用状态:%s\r\n"), varEnabled ? _T("已启用") : _T("未启用"));
	hResult = (*ppNFRASS)->get_IpVersion(&nfiv);
	_tprintf(_T("\tIP版本:%ld\r\n"), nfiv);
	hResult = (*ppNFRASS)->get_RemoteAddresses(&bstrRemoteAddresses);
	if (FAILED(hResult))
	{
		bstrRemoteAddresses = SysAllocString(_T(""));
	}

	_tprintf(_T("\t远程地址:%s\t\n"), W2T(bstrRemoteAddresses));

	SysFreeString(bstrRemoteAddresses);
	bstrRemoteAddresses = NULL;

	(*ppNFRASS)->Release();
	(*ppNFRASS) = NULL;

	return hResult;
}
HRESULT GetNetFwAuthorizedApplications(INetFwAuthorizedApplications ** ppNFAAS)
{
	HRESULT hResult = S_FALSE;
	LONG nNFAASCount = 0;
	IEnumVARIANT * pEnumVariant = NULL;
	VARIANT_BOOL varEnabled = VARIANT_FALSE;

	USES_CONVERSION;

	_tprintf(_T("安全程序\r\n"));

	hResult = (*ppNFAAS)->get_Count(&nNFAASCount);

	_tprintf(_T("\t安全程序个数:%ld\r\n"), nNFAASCount);

	hResult = (*ppNFAAS)->get__NewEnum((IUnknown**)&pEnumVariant);
	if (SUCCEEDED(hResult))
	{
		ULONG nCeltFetched = 0L;
		VARIANT varCurrPosi = { 0 };
		VariantInit(&varCurrPosi);
		while (1)
		{
			hResult = pEnumVariant->Next(1, (VARIANT *)&varCurrPosi, (ULONG*)&nCeltFetched);
			if (SUCCEEDED(hResult))
			{
				if (nCeltFetched > 0L)
				{
					INetFwAuthorizedApplication *pNFA = NULL;
					IDispatch *pDisp = V_DISPATCH(&varCurrPosi);

					if (SUCCEEDED(pDisp->QueryInterface(__uuidof(INetFwAuthorizedApplication), (void**)&pNFA)))
					{
						BSTR bstrName = NULL;
						BSTR bstrProcessImageFileName = NULL;
						hResult = pNFA->get_Name(&bstrName);
						if (FAILED(hResult))
						{
							bstrName = SysAllocString(_T(""));
						}
						_tprintf(_T("\t应用程序:%s\r\n"), bstrName);

						hResult = pNFA->get_ProcessImageFileName(&bstrProcessImageFileName);
						if (FAILED(hResult))
						{
							bstrProcessImageFileName = SysAllocString(_T(""));
						}
						_tprintf(_T("\t路径:%s\r\n"), W2T(bstrProcessImageFileName));
						hResult = pNFA->get_Enabled(&varEnabled);
						_tprintf(_T("\t当前是否可用:%s\r\n"), (VARIANT_TRUE == varEnabled ? _T("可用") : _T("不可用")));

						SysFreeString(bstrName);
						bstrName = NULL;
						SysFreeString(bstrProcessImageFileName);
						bstrProcessImageFileName = NULL;

						pNFA->Release();
						pNFA = NULL;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				//异常退出
				_tprintf(_T("枚举过程中失败,程序异常退出!\r\n"));
			}
		}
		pEnumVariant->Release();
		pEnumVariant = NULL;
	}	

	(*ppNFAAS)->Release();
	(*ppNFAAS) = NULL;

	return hResult;
}

HRESULT EnumNetFwInfoList(NETWORKSFIREWALLINSTANCE *pNFI)
{
	HRESULT hResult = S_FALSE;
	
	INetFwServices * pNFSS = NULL;
	INetFwOpenPorts * pNFOPS = NULL;
	INetFwIcmpSettings * pNFISS = NULL;
	INetFwRemoteAdminSettings * pNFRASS = NULL;
	INetFwAuthorizedApplications * pNFAASS = NULL;

	VARIANT_BOOL varFirewallEnabled = VARIANT_FALSE;
		
	hResult = pNFI->pNetFwProfile->get_FirewallEnabled(&varFirewallEnabled);
	_tprintf(_T("防火墙启用：%s\r\n"), varFirewallEnabled ? _T("已启用"):_T("未启用"));

	hResult = pNFI->pNetFwProfile->get_Services(&pNFSS);
	GetNetFwServices(&pNFSS);

	hResult = pNFI->pNetFwProfile->get_GloballyOpenPorts(&pNFOPS);
	GetNetFwOpenPorts(&pNFOPS);

	hResult = pNFI->pNetFwProfile->get_IcmpSettings(&pNFISS);
	GetNetFwIcmpSettings(&pNFISS);

	hResult = pNFI->pNetFwProfile->get_RemoteAdminSettings(&pNFRASS);
	GetNetFwRemoteAdminSettings(&pNFRASS);
	
	hResult = pNFI->pNetFwProfile->get_AuthorizedApplications(&pNFAASS);
	GetNetFwAuthorizedApplications(&pNFAASS);
	
	return hResult;
}

__inline static BOOL GetSystemFirewallInfoList()
{
	HRESULT hResult = S_FALSE;
	NETWORKSFIREWALLINSTANCE nfi = { 0 };
	hResult = InitNetFwMgr(&nfi);
	if (SUCCEEDED(hResult))
	{
		EnumNetFwInfoList(&nfi);
	}
	ExitNetFwMgr(&nfi);

	return TRUE;
}