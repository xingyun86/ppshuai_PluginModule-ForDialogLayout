#pragma once

BOOL GetSystemServicesList()
{
	BOOL bResult = FALSE;

	//��ϵͳ���������
	SC_HANDLE SCMan = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (SCMan == NULL)
	{
		_tprintf(_T("OpenSCManager Eorror\n"));
		return bResult;
	}
	//����ϵͳ����Ľṹ
	LPENUM_SERVICE_STATUS service_status; 
	DWORD cbBytesNeeded = NULL;
	DWORD ServicesReturned = NULL;
	DWORD ResumeHandle = NULL;


	//��ȡϵͳ����ļ���Ϣ
	bResult = EnumServicesStatus(SCMan, //ϵͳ������
		SERVICE_WIN32, //���������
		SERVICE_STATE_ALL,  //�����״̬
		NULL,  //���������ϵͳ����Ľṹ
		0,  // �ṹ�Ĵ�С
		&cbBytesNeeded, //������������շ�������ķ���
		&ServicesReturned, //������������շ��ط��������
		&ResumeHandle); //���������������һ�ε��ñ���Ϊ0������Ϊ0����ɹ�
	if (!bResult && GetLastError() == ERROR_MORE_DATA)
	{
		//�����ڴ棬ע���С
		service_status = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, cbBytesNeeded);

		//��ȡϵͳ����ļ���Ϣ
		bResult = EnumServicesStatus(SCMan, //ϵͳ������
			SERVICE_WIN32, //���������
			SERVICE_STATE_ALL,  //�����״̬
			(LPENUM_SERVICE_STATUS)service_status,  //���������ϵͳ����Ľṹ
			cbBytesNeeded,  // �ṹ�Ĵ�С
			&cbBytesNeeded, //������������շ�������ķ���
			&ServicesReturned, //������������շ��ط��������
			&ResumeHandle); //���������������һ�ε��ñ���Ϊ0������Ϊ0����ɹ�
	}
	if (!bResult)
	{
		_tprintf(_T("EnumServicesStatus Eorror\n"));
		return bResult;
	}
	for (int i = 0; i < ServicesReturned; i++)
	{
		_tprintf(_T("������: %s "), service_status[i].lpDisplayName);
		switch (service_status[i].ServiceStatus.dwCurrentState)// ����״̬
		{
		case SERVICE_CONTINUE_PENDING:
			_tprintf(_T("״̬: ����"));
			break;
		case SERVICE_PAUSE_PENDING:
			_tprintf(_T("״̬: ������ͣ "));
			break;
		case SERVICE_PAUSED:
			_tprintf(_T("״̬: ����ͣ "));
			break;
		case SERVICE_RUNNING:
			_tprintf(_T("״̬: ���� "));
			break;
		case SERVICE_START_PENDING:
			_tprintf(_T("״̬: ���� "));
			break;
		case SERVICE_STOPPED:
			_tprintf(_T("״̬: ֹͣ "));
			break;
		default:
			_tprintf(_T("״̬: δ֪ "));
			break;
		}
		LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL; //������ϸ��Ϣ�ṹ
		SC_HANDLE service_curren = NULL; //��ǰ�ķ�����
		service_curren = OpenService(SCMan, service_status[i].lpServiceName, SERVICE_QUERY_CONFIG); //�򿪵�ǰ����

		bResult = QueryServiceConfig(service_curren, lpServiceConfig, 0, &cbBytesNeeded);
		if (!bResult && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, cbBytesNeeded);      //�����ڴ棬 ���Ϊ8kb 

			bResult = QueryServiceConfig(service_curren, lpServiceConfig, cbBytesNeeded, &cbBytesNeeded);
		}
		if (!bResult)
		{
			_tprintf(_T("QueryServiceConfig Error\n"));
			return bResult;
		}
		_tprintf(_T("·��: %s "), lpServiceConfig->lpBinaryPathName);//�����·��
		_tprintf(_T("\n"));
		bResult = CloseServiceHandle(service_curren);//�رյ�ǰ����ľ��
	}
	
	bResult = CloseServiceHandle(SCMan);//�رշ���������ľ��
	
	return bResult;
}