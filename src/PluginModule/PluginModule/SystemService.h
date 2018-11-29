#pragma once

BOOL GetSystemServicesList()
{
	BOOL bResult = FALSE;

	//打开系统服务控制器
	SC_HANDLE SCMan = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (SCMan == NULL)
	{
		_tprintf(_T("OpenSCManager Eorror\n"));
		return bResult;
	}
	//保存系统服务的结构
	LPENUM_SERVICE_STATUS service_status; 
	DWORD cbBytesNeeded = NULL;
	DWORD ServicesReturned = NULL;
	DWORD ResumeHandle = NULL;


	//获取系统服务的简单信息
	bResult = EnumServicesStatus(SCMan, //系统服务句柄
		SERVICE_WIN32, //服务的类型
		SERVICE_STATE_ALL,  //服务的状态
		NULL,  //输出参数，系统服务的结构
		0,  // 结构的大小
		&cbBytesNeeded, //输出参数，接收返回所需的服务
		&ServicesReturned, //输出参数，接收返回服务的数量
		&ResumeHandle); //输入输出参数，第一次调用必须为0，返回为0代表成功
	if (!bResult && GetLastError() == ERROR_MORE_DATA)
	{
		//分配内存，注意大小
		service_status = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, cbBytesNeeded);

		//获取系统服务的简单信息
		bResult = EnumServicesStatus(SCMan, //系统服务句柄
			SERVICE_WIN32, //服务的类型
			SERVICE_STATE_ALL,  //服务的状态
			(LPENUM_SERVICE_STATUS)service_status,  //输出参数，系统服务的结构
			cbBytesNeeded,  // 结构的大小
			&cbBytesNeeded, //输出参数，接收返回所需的服务
			&ServicesReturned, //输出参数，接收返回服务的数量
			&ResumeHandle); //输入输出参数，第一次调用必须为0，返回为0代表成功
	}
	if (!bResult)
	{
		_tprintf(_T("EnumServicesStatus Eorror\n"));
		return bResult;
	}
	for (int i = 0; i < ServicesReturned; i++)
	{
		_tprintf(_T("服务名: %s "), service_status[i].lpDisplayName);
		switch (service_status[i].ServiceStatus.dwCurrentState)// 服务状态
		{
		case SERVICE_CONTINUE_PENDING:
			_tprintf(_T("状态: 挂起"));
			break;
		case SERVICE_PAUSE_PENDING:
			_tprintf(_T("状态: 正在暂停 "));
			break;
		case SERVICE_PAUSED:
			_tprintf(_T("状态: 已暂停 "));
			break;
		case SERVICE_RUNNING:
			_tprintf(_T("状态: 运行 "));
			break;
		case SERVICE_START_PENDING:
			_tprintf(_T("状态: 启动 "));
			break;
		case SERVICE_STOPPED:
			_tprintf(_T("状态: 停止 "));
			break;
		default:
			_tprintf(_T("状态: 未知 "));
			break;
		}
		LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL; //服务详细信息结构
		SC_HANDLE service_curren = NULL; //当前的服务句柄
		service_curren = OpenService(SCMan, service_status[i].lpServiceName, SERVICE_QUERY_CONFIG); //打开当前服务

		bResult = QueryServiceConfig(service_curren, lpServiceConfig, 0, &cbBytesNeeded);
		if (!bResult && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, cbBytesNeeded);      //分配内存， 最大为8kb 

			bResult = QueryServiceConfig(service_curren, lpServiceConfig, cbBytesNeeded, &cbBytesNeeded);
		}
		if (!bResult)
		{
			_tprintf(_T("QueryServiceConfig Error\n"));
			return bResult;
		}
		_tprintf(_T("路径: %s "), lpServiceConfig->lpBinaryPathName);//服务的路径
		_tprintf(_T("\n"));
		bResult = CloseServiceHandle(service_curren);//关闭当前服务的句柄
	}
	
	bResult = CloseServiceHandle(SCMan);//关闭服务管理器的句柄
	
	return bResult;
}