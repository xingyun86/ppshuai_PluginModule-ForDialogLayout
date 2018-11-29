#pragma once

/********************************************************************
This sample enumerates through the tasks on the local computer and
displays their name and state.
********************************************************************/

HRESULT EnumTaskScheduler(ITaskFolder **ppTaskFolder)
{
	HRESULT hResult = S_FALSE;
	BSTR bstrFolderName = NULL;
	BSTR bstrFolderPath = NULL;
	LONG nTaskFolderCount = 0L;
	LONG nFolderTasksCount = 0L;
	BSTR bstrFolderTaskName = NULL;
	ITaskFolder * pTaskFolder = NULL;
	IRegisteredTask * pRegisteredTask = NULL;
	TASK_STATE eTaskState = TASK_STATE_UNKNOWN;
	ITaskFolderCollection * pTaskFolderCollection = NULL;
	IRegisteredTaskCollection * pRegisteredTaskCollection = NULL;

	pTaskFolder = (*ppTaskFolder);
	hResult = pTaskFolder->get_Name(&bstrFolderName);
	if (FAILED(hResult))
	{
		bstrFolderName = SysAllocString(_T(""));
		_tprintf(_T("Cannot get the task folder name: %x\r\n"), hResult);
	}
	_tprintf(_T("Folder=%s\r\n"), bstrFolderName);
	SysFreeString(bstrFolderName);
	bstrFolderName = NULL;

	hResult = pTaskFolder->get_Path(&bstrFolderPath);
	if (FAILED(hResult))
	{
		bstrFolderPath = SysAllocString(_T(""));
		_tprintf(_T("Cannot get the task folder name: %x\r\n"), hResult);
	}
	_tprintf(_T("Folder=%s\r\n"), bstrFolderPath);
	SysFreeString(bstrFolderPath);
	bstrFolderPath = NULL;

	////////////////////////////////////////////////////////////////////////////
	// Enum folder tasks

	//  -------------------------------------------------------
	//  Get the registered tasks in the folder.
	hResult = pTaskFolder->GetTasks(TASK_ENUM_HIDDEN, &pRegisteredTaskCollection);
	if (FAILED(hResult))
	{
		_tprintf(_T("Cannot get the registered tasks.: %x\r\n"), hResult);
	}
	else
	{
		hResult = pRegisteredTaskCollection->get_Count(&nFolderTasksCount);

		if (nFolderTasksCount == 0)
		{
			_tprintf(_T("\tNo Tasks are currently running\r\n"));
		}
		else
		{
			_tprintf(_T("Number of Tasks : %d\r\n"), nFolderTasksCount);
			for (LONG nFolderTaskIndex = 0; nFolderTaskIndex < nFolderTasksCount; nFolderTaskIndex++)
			{
				hResult = pRegisteredTaskCollection->get_Item(_variant_t(nFolderTaskIndex + 1), &pRegisteredTask);

				if (SUCCEEDED(hResult))
				{
					bstrFolderTaskName = NULL;
					hResult = pRegisteredTask->get_Name(&bstrFolderTaskName);
					if (FAILED(hResult))
					{
						bstrFolderTaskName = SysAllocString(_T(""));
						_tprintf(_T("Cannot get the registered task name: %x\r\n"), hResult);
					}

					_tprintf(_T("\tTask Name: %s\r\n"), bstrFolderTaskName);
					SysFreeString(bstrFolderTaskName);
					bstrFolderTaskName = NULL;

					hResult = pRegisteredTask->get_State(&eTaskState);
					if (SUCCEEDED(hResult))
					{
						_tprintf(_T("\tState: %d\r\n"), eTaskState);
					}
					else
					{
						printf("Cannot get the registered task state: %x\r\n", hResult);
					}
					BSTR bstrTaskURI = NULL;
					BSTR bstrTaskAutor = NULL;
					BSTR bstrTaskDescription = NULL;
					ITaskDefinition * pTaskDefinition = NULL;
					IRegistrationInfo * pRegistrationInfo = NULL;
					hResult = pRegisteredTask->get_Definition(&pTaskDefinition);
					if (SUCCEEDED(hResult)) 
					{
						hResult = pTaskDefinition->get_RegistrationInfo(&pRegistrationInfo);
						if (SUCCEEDED(hResult))
						{
							hResult = pRegistrationInfo->get_Author(&bstrTaskAutor);
							if (FAILED(hResult))
							{
								bstrTaskAutor = SysAllocString(_T(""));
								_tprintf(_T("Cannot get the task author: %x\r\n"), hResult);
							}
							hResult = pRegistrationInfo->get_Description(&bstrTaskDescription);
							if (FAILED(hResult))
							{
								bstrTaskDescription = SysAllocString(_T(""));
								_tprintf(_T("Cannot get the task description: %x\r\n"), hResult);
							}
							hResult = pRegistrationInfo->get_URI(&bstrTaskURI);
							if (FAILED(hResult))
							{
								bstrTaskURI = SysAllocString(_T(""));
								_tprintf(_T("Cannot get the task uri: %x\r\n"), hResult);
							}
							_tprintf(_T("\tTask Author: %s\r\n"), bstrTaskAutor);
							SysFreeString(bstrTaskAutor);
							bstrTaskAutor = NULL;
							_tprintf(_T("\tTask Description: %s\r\n"), bstrTaskDescription);
							SysFreeString(bstrTaskDescription);
							bstrTaskDescription = NULL;
							_tprintf(_T("\tTask URI: %s\r\n"), bstrTaskURI);
							SysFreeString(bstrTaskURI);
							bstrTaskURI = NULL;

							pRegistrationInfo->Release();
							pRegistrationInfo = NULL;
						}
						ITriggerCollection * pTriggerCollection = NULL;
						
						hResult = pTaskDefinition->get_Triggers(&pTriggerCollection);
						if (SUCCEEDED(hResult))
						{
							LONG lTriggerCount = 0L;
							LONG lTriggerIndex = 0L;
							hResult = pTriggerCollection->get_Count(&lTriggerCount);
							if (SUCCEEDED(hResult))
							{
								for (lTriggerIndex = 0L; lTriggerIndex < lTriggerCount; lTriggerIndex++)
								{
									ITrigger * pTrigger = NULL;
									hResult = pTriggerCollection->get_Item(lTriggerIndex, &pTrigger);
									if (SUCCEEDED(hResult))
									{
										BSTR bstrStartBoundary = NULL;
										BSTR bstrEndBoundary = NULL;
										hResult = pTrigger->get_StartBoundary(&bstrStartBoundary);
										if (FAILED(hResult))
										{
											bstrStartBoundary = SysAllocString(_T(""));
										}

										_tprintf(_T("StartBoundary=%s"), bstrStartBoundary);

										hResult = pTrigger->get_StartBoundary(&bstrEndBoundary);
										if (FAILED(hResult))
										{
											bstrEndBoundary = SysAllocString(_T(""));
										}

										_tprintf(_T("EndBoundary=%s"), bstrEndBoundary);
										SysFreeString(bstrStartBoundary);
										bstrStartBoundary = NULL;
										SysFreeString(bstrEndBoundary);
										bstrEndBoundary = NULL;
										pTrigger->Release();
										pTrigger = NULL;
									}
								}
							}

							pTriggerCollection->Release();
							pTriggerCollection = NULL;
						}
						pTaskDefinition->Release();
						pTaskDefinition = NULL;
					}

					pRegisteredTask->Release();
					pRegisteredTask = NULL;
				}
				else
				{
					_tprintf(_T("Cannot get the registered task item at index=%d: %x\r\n"), nFolderTaskIndex + 1, hResult);
				}
			}
		}
		pRegisteredTaskCollection->Release();
		pRegisteredTaskCollection = NULL;
	}

	////////////////////////////////////////////////////////////////////////////
	// Enum task folders
	hResult = pTaskFolder->GetFolders(0, &pTaskFolderCollection);
	if (FAILED(hResult))
	{
		_tprintf(_T("Cannot get all folders pointer: %x"), hResult);
		goto __LEAVE_CLEAN__;
	}
	
	pTaskFolder->Release();
	pTaskFolder = NULL;
	(*ppTaskFolder) = NULL;

	hResult = pTaskFolderCollection->get_Count(&nTaskFolderCount);
	if (nTaskFolderCount == 0)
	{
		_tprintf(_T("\tNo TaskFolder Found!\r\n"));
		goto __LEAVE_CLEAN__;
	}

	_tprintf(_T("\tNumber of TaskFolders : %d\r\n"), nTaskFolderCount);

	for (LONG nTaskFolderIndex = 0; nTaskFolderIndex < nTaskFolderCount; nTaskFolderIndex++)
	{
		pTaskFolder = NULL;
		hResult = pTaskFolderCollection->get_Item(_variant_t(nTaskFolderIndex + 1), &pTaskFolder);
		if (FAILED(hResult))
		{
			_tprintf(_T("Cannot get the task folder item at index=%d: %x%d\r\n"), nTaskFolderIndex, hResult);
		}
		else
		{
			hResult = EnumTaskScheduler(&pTaskFolder);
		}
	}

__LEAVE_CLEAN__:

	if (pTaskFolderCollection)
	{
		pTaskFolderCollection->Release();
		pTaskFolderCollection = NULL;
	}

	if (pTaskFolder)
	{
		pTaskFolder->Release();
		pTaskFolder = NULL;
		(*ppTaskFolder) = NULL;
	}

	return hResult;
}

HRESULT GetSystemTaskSchedulersListXPHigher()
{
	HRESULT hResult = S_FALSE;	
	ITaskService * pTaskService = NULL;
	BSTR bstrRootTaskFolder = _T("\\");
	ITaskFolder * pRootTaskFolder = NULL;
	//  ------------------------------------------------------
	//  Initialize COM.
	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hResult))
	{
		_tprintf(_T("CoInitializeEx failed: %x\r\n"), hResult);
		goto __LEAVE_CLEAN__;
	}

	//  Set general COM security levels.
	hResult = CoInitializeSecurity(
		NULL,
		(-1),
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		(0),
		NULL);

	if (FAILED(hResult))
	{
		_tprintf(_T("CoInitializeSecurity failed: %x"), hResult);
		goto __LEAVE_CLEAN__;
	}

	//  ------------------------------------------------------
	//  Create an instance of the Task Service. 
	hResult = CoCreateInstance(CLSID_TaskScheduler,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		(void**)&pTaskService);
	if (FAILED(hResult))
	{
		_tprintf(_T("Failed to CoCreate an instance of the TaskService class: %x"), hResult);
		goto __LEAVE_CLEAN__;
	}

	//  Connect to the task service.
	hResult = pTaskService->Connect(_variant_t(), _variant_t(),	_variant_t(), _variant_t());
	if (FAILED(hResult))
	{
		_tprintf(_T("ITaskService::Connect failed: %x"), hResult);
		goto __LEAVE_CLEAN__;
	}

	//  ------------------------------------------------------
	//  Get the pointer to the root task folder.
	hResult = pTaskService->GetFolder(_bstr_t(bstrRootTaskFolder), &pRootTaskFolder);
	pTaskService->Release();
	pTaskService = NULL;
	if (FAILED(hResult))
	{
		_tprintf(_T("Cannot get Root Folder pointer: %x"), hResult);
		goto __LEAVE_CLEAN__;
	}
	
	hResult = EnumTaskScheduler(&pRootTaskFolder);
	
__LEAVE_CLEAN__:

	if (pTaskService)
	{
		pTaskService->Release();
		pTaskService = NULL;
	}

	CoUninitialize();
	
	return hResult;
}

BOOL GetSysTaskSchedulersList_Temp()
{
	LPBYTE lpData = NULL;
	DWORD dwPrefferedMaximumLength = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0L;
	DWORD dwTotalEntries = 0L;
	DWORD dwResumeHandle = 0L;
	ITaskScheduler *pITS;

	HRESULT hr = CoInitialize(NULL);

	NET_API_STATUS netapiStatus = ::NetScheduleJobEnum(NULL, &lpData, dwPrefferedMaximumLength, &dwEntriesRead, &dwTotalEntries, &dwResumeHandle);
	_tprintf(_T("%ld, %ld,%ld,%ld\r\n"), GetLastError(), dwEntriesRead, dwTotalEntries, dwResumeHandle);

	CoUninitialize();

	return TRUE;
}

BOOL GetSystemTaskSchedulersListXPLowwer()
{
	HRESULT hr = S_OK;
	TSTRING tsMessage = _T("");
	ITaskScheduler *pITS = NULL;
	IEnumWorkItems *pIEnum = NULL;
	ITaskTrigger * pTaskTrigger = NULL;
	LPTSTR *lpszNames = NULL;
	DWORD dwFetchedTasks = 0;
	ITask *pITask = NULL;
	LPTSTR lpszCreator = NULL;
	LPTSTR lpszComment = NULL;
	LPTSTR lpszParameters = NULL;
	_TCHAR tzName[MAX_PATH] = { 0 };
	LPTSTR lpszApplicationName = NULL;
	WORD wTriggerCount = 0;
	WORD wTriggerIndex = 0;
	TASK_TRIGGER taskTrigger = { 0 };
	LPTSTR lpszTriggerString = NULL;

	/////////////////////////////////////////////////////////////////
	// Call CoInitialize to initialize the COM library and 
	// then call CoCreateInstance to get the Task Scheduler object. 
	/////////////////////////////////////////////////////////////////
	hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_CTaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskScheduler,
			(void **)&pITS);
		if (FAILED(hr))
		{
			goto __LEAVE_CLEAN__;
		}
	}
	else
	{
		goto __LEAVE_CLEAN__;
	}
	
	/////////////////////////////////////////////////////////////////
	// Call ITaskScheduler::Enum to get an enumeration object.
	/////////////////////////////////////////////////////////////////
	hr = pITS->Enum(&pIEnum);
	if (FAILED(hr))
	{
		goto __LEAVE_CLEAN__;
	}

	/////////////////////////////////////////////////////////////////
	// Call IEnumWorkItems::Next to retrieve tasks. Note that 
	// this example tries to retrieve five tasks for each call.
	/////////////////////////////////////////////////////////////////
	while (SUCCEEDED(pIEnum->Next(1,
		&lpszNames,
		&dwFetchedTasks))
		&& (dwFetchedTasks != 0L))
	{
		///////////////////////////////////////////////////////////////
		// Process each task. Note that this example prints the 
		// name of each task to the screen.
		//////////////////////////////////////////////////////////////
		while (dwFetchedTasks--)
		{
			pITask = NULL;
			memset(tzName, 0, sizeof(tzName));
			lpszApplicationName = NULL;
			wTriggerCount = 0;
			wTriggerIndex = 0;
			memset(&taskTrigger, 0, sizeof(taskTrigger));
			lpszTriggerString = NULL;

			_tprintf(_T("[%ld]TaskName=%s\r\n"), dwFetchedTasks, lpszNames[dwFetchedTasks]);

			_tcscpy(tzName, lpszNames[dwFetchedTasks]);
			(*_tcschr(tzName, _T('.'))) = _T('\x00');
			hr = pITS->Activate(tzName, IID_ITask, (IUnknown**)&pITask);
			if (FAILED(hr))
			{
				_tprintf(_T("[%ld]%s calling ITaskScheduler::Activate Error!Error=%08X.\r\n"), dwFetchedTasks, lpszNames[dwFetchedTasks], hr);
				goto __CHILD_LEAVE_CLEAN__;
			}
			_tprintf(_T("[%ld]ITaskScheduler::Activate success.\r\n"), dwFetchedTasks);

			hr = pITask->GetApplicationName(&lpszApplicationName);
			if (FAILED(hr))
			{
				_tprintf(_T("[%ld]%s calling ITask::GetApplicationName Error!Error=%08X.\r\n"), dwFetchedTasks, lpszNames[dwFetchedTasks], hr);
				tsMessage.append(lpszNames[dwFetchedTasks]).append(_T("\r\n"));
			}
			_tprintf(_T("[%ld]ITask::GetApplicationName success.Result=%s.\r\n"), dwFetchedTasks, lpszApplicationName);

			hr = pITask->GetCreator(&lpszCreator);
			if (FAILED(hr))
			{
				_tprintf(_T("[%ld]%s calling ITask::GetCreator Error!Error=%08X.\r\n"), dwFetchedTasks, lpszNames[dwFetchedTasks], hr);
				goto __CHILD_LEAVE_CLEAN__;
			}
			_tprintf(_T("[%ld]ITask::GetCreator success.Result=%s.\r\n"), dwFetchedTasks, lpszCreator);

			hr = pITask->GetComment(&lpszComment);
			if (FAILED(hr))
			{
				_tprintf(_T("[%ld]%s calling ITask::GetComment Error!Error=%08X.\r\n"), dwFetchedTasks, lpszNames[dwFetchedTasks], hr);
				goto __CHILD_LEAVE_CLEAN__;
			}

			_tprintf(_T("[%ld]ITask::GetComment success.Result=%s.\r\n"), dwFetchedTasks, lpszComment);

			hr = pITask->GetParameters(&lpszParameters);
			if (FAILED(hr))
			{
				_tprintf(_T("[%ld]%s calling ITask::GetParameters Error!Error=%08X.\r\n"), dwFetchedTasks, lpszNames[dwFetchedTasks], hr);
				goto __CHILD_LEAVE_CLEAN__;
			}
			_tprintf(_T("[%ld]ITask::GetParameters success.Result=%s.\r\n"), dwFetchedTasks, lpszParameters);

			hr = pITask->GetTriggerCount(&wTriggerCount);
			if (FAILED(hr))
			{
				_tprintf(_T("[%ld]%s calling ITask::GetTriggerCount Error!Error=%08X.\r\n"), dwFetchedTasks, lpszNames[dwFetchedTasks], hr);
				goto __CHILD_LEAVE_CLEAN__;
			}
			_tprintf(_T("[%ld]ITask::GetTriggerCount success.Result=%ld.\r\n"), dwFetchedTasks, wTriggerCount);

			for (wTriggerIndex = 0; wTriggerIndex < wTriggerCount; wTriggerIndex++)
			{
				hr = pITask->GetTrigger(wTriggerIndex, &pTaskTrigger);
				if (FAILED(hr))
				{
					_tprintf(_T("[%ld]->[%ld]%s calling ITask::GetTrigger Error!Error=%08X.\r\n"), dwFetchedTasks, wTriggerIndex, lpszNames[dwFetchedTasks], hr);
					goto ___CHILD_CHILD_LEAVE_CLEAN__;
				}
				_tprintf(_T("[%ld]->[%ld]ITask::GetTrigger success.\r\n"), dwFetchedTasks, wTriggerIndex);
				hr = pTaskTrigger->GetTriggerString(&lpszTriggerString);
				if (FAILED(hr))
				{
					_tprintf(_T("[%ld]->[%ld]%s calling ITaskTrigger::GetTriggerString Error!Error=%08X.\r\n"), dwFetchedTasks, wTriggerIndex, lpszNames[dwFetchedTasks], hr);
					goto ___CHILD_CHILD_LEAVE_CLEAN__;
				}
				_tprintf(_T("[%ld]->[%ld]ITask::GetTriggerString success.Result=%ld.\r\n"), dwFetchedTasks, wTriggerIndex, lpszTriggerString);
				hr = pTaskTrigger->GetTrigger(&taskTrigger);
				if (FAILED(hr))
				{
					_tprintf(_T("[%ld]->[%ld]%s calling ITaskTrigger::GetTrigger Error!Error=%08X.\r\n"), dwFetchedTasks, wTriggerIndex, lpszNames[dwFetchedTasks], hr);
					goto ___CHILD_CHILD_LEAVE_CLEAN__;
				}

				_tprintf(_T("[%ld]->[%ld]ITask::GetTrigger success.Result=[%04d-%02d-%02d, %04d-%02d-%02d](%02d:%02d).\r\n"), dwFetchedTasks, wTriggerIndex, 
					taskTrigger.wBeginYear, taskTrigger.wBeginMonth, taskTrigger.wBeginDay,
					taskTrigger.wEndYear, taskTrigger.wEndMonth, taskTrigger.wEndDay,
					taskTrigger.wStartHour, taskTrigger.wStartMinute);

			___CHILD_CHILD_LEAVE_CLEAN__:
				if (lpszTriggerString)
				{
					CoTaskMemFree(lpszTriggerString);
					lpszTriggerString = NULL;
				}
				if (pTaskTrigger)
				{
					pTaskTrigger->Release();
					pTaskTrigger = NULL;
				}
			}
		
		__CHILD_LEAVE_CLEAN__:

			if (lpszParameters)
			{
				CoTaskMemFree(lpszParameters);
				lpszParameters = NULL;
			}
			if (lpszComment)
			{
				CoTaskMemFree(lpszComment);
				lpszComment = NULL;
			}
			if (lpszApplicationName)
			{
				CoTaskMemFree(lpszApplicationName);
				lpszApplicationName = NULL;
			}
			if (lpszCreator)
			{
				CoTaskMemFree(lpszCreator);
				lpszCreator = NULL;
			}
			if (lpszNames[dwFetchedTasks])
			{
				CoTaskMemFree(lpszNames[dwFetchedTasks]);
				lpszNames[dwFetchedTasks] = NULL;
			}
		}
		if (lpszNames)
		{
			CoTaskMemFree(lpszNames);
			lpszNames = NULL;
		}
	}

__LEAVE_CLEAN__:
	
	if (pITask)
	{
		pITask->Release();
		pITask = NULL;
	}
	if (pIEnum)
	{
		pIEnum->Release();
		pIEnum = NULL;
	}
	if (pITS)
	{
		pITS->Release();
		pITS = NULL;
	}
	CoUninitialize();

	return hr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// XP 下也可以遍历目录
/*std::map<TSTRING, TSTRING> ttmap;
DirectoryTraversal(&ttmap, _T("C:\\Windows\\Tasks\\"), _T("C:\\Windows\\Tasks\\"));
for each (auto it in ttmap)
{
_tprintf(_T("%s,%s\r\n"), it.first.c_str(), it.second.c_str());
}*/
//////////////////////////////////////////////////////////////////////////////////////////

__inline static BOOL GetSystemTaskSchedulersList()
{
	BOOL bResult = FALSE;
	OSVERSIONINFOEX osviex = { 0 };
	BOOL bIsWindowsXPorLater = FALSE;
	
	GetWindowsSystemVersionName(&osviex);

	bIsWindowsXPorLater = (osviex.dwMajorVersion < 6);

	if (bIsWindowsXPorLater)
	{
		bResult = GetSystemTaskSchedulersListXPLowwer();
	}
	else
	{
		bResult = SUCCEEDED(GetSystemTaskSchedulersListXPHigher());
	}
	return bResult;
}