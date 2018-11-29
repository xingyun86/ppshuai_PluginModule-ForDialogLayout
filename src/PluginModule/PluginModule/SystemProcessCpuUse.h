#pragma once
///////////////////////////////////////////////////////////////////////////////////
//		C++ ��ȡ�ض����̹涨CPUʹ����
//ԭ������GetProcessTimes()�������ϴε��õõ��Ľ����������õ�ĳ��ʱ����CPU��ʹ��ʱ��  

class CPUUsage {

public:
	CPUUsage(DWORD dwProcessID)
	{
		Init();
		dwNumberOfProcessors = GetNumberOfProcessor();
		SetProcessID(dwProcessID);
	}
	CPUUsage()
	{ 
		Init(); 
		dwNumberOfProcessors = GetNumberOfProcessor(); 
	}
	~CPUUsage()
	{ 
		Clear();
	}

	//����ֵΪ���̾�������ж�OpenProcess�Ƿ�ɹ�  
	HANDLE SetProcessID(DWORD dwProcessID) 
	{
		//���֮ǰ���ӹ���һ�����̣����ȹر����ľ��  
		Clear();    
		Init();
		return (hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessID));
	}

	//���㲢��ȡCPUʹ���� 
	DOUBLE GetCpuUsage()
	{
		DWORD dwExitCode = 0L;
		DOUBLE dCpuUsage = 0.0f;
		FILETIME ftNow = { 0L };
		FILETIME ftCreationTime = { 0L };
		FILETIME ftExitTime = { 0L };
		FILETIME ftKernelTime = { 0L };
		FILETIME ftUserTime = { 0L };
		LONGLONG llSystemTime = 0LL;
		LONGLONG llTime = 0LL;
		LONGLONG llSystemTimeDelta = 0LL;
		LONGLONG llTimeDelta = 0LL;

		if (hProcess)
		{
			//�жϽ�����������
			if (::GetExitCodeProcess(hProcess, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
			{
				//��ȡ��ǰϵͳʱ��Ϊ�ļ�ʱ��
				::GetSystemTimeAsFileTime(&ftNow);
				
				//����ռ��CPU�İٷֱ�  
				if (::GetProcessTimes(hProcess, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime))
				{
					llTime = FileTime2UTC(&ftNow);
					llSystemTime = FileTime2UTC(&ftKernelTime) + FileTime2UTC(&ftUserTime);

					//�ж��Ƿ�Ϊ�״μ���  
					if ((llLastSystemTime != 0LL) && (llLastTime != 0LL))
					{
						llTimeDelta = llTime - llLastTime;
						llSystemTimeDelta = llSystemTime - llLastSystemTime;

						if (llTimeDelta != 0LL) 
						{
							dCpuUsage = (DOUBLE)((DOUBLE)((DOUBLE)llSystemTimeDelta * 100) / (llTimeDelta * dwNumberOfProcessors));
						}
					}
					llLastTime = llTime;
					llLastSystemTime = llSystemTime;
				}
				else
				{
					_tprintf(_T("GetProcessTimes failed! LastError = %ld\r\n"), GetLastError());
				}
			}
			else
			{
				_tprintf(_T("GetExitCodeProcess failed! LastError = %ld,ExitCode = %ld\r\n"), dwExitCode, GetLastError());
			}
		}
		
		return dCpuUsage;
	}

private:

	// ʱ��ת��    
	LONGLONG FileTime2UTC(const FILETIME* ftime)
	{
		LARGE_INTEGER li = { 0L,0L };
		li.LowPart = ftime->dwLowDateTime;
		li.HighPart = ftime->dwHighDateTime;
		return li.QuadPart;
	}

	// ���CPU�ĺ���    
	DWORD GetNumberOfProcessor()
	{
		SYSTEM_INFO si = { 0 };
		::GetSystemInfo(&si);
		return si.dwNumberOfProcessors;
	}

	//��ʼ��  
	void Init()
	{
		llLastSystemTime = 0LL;
		llLastTime = 0LL;
		hProcess = NULL;
	}

	//�رս��̾��  
	void Clear()
	{
		if (hProcess) 
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
	}

private:
	HANDLE		hProcess;				//���̾��
	LONGLONG	llLastTime;				//��һ��ʱ��    
	LONGLONG	llLastSystemTime;		//��һ��ϵͳʱ��
	DWORD		dwNumberOfProcessors;	//ϵͳCPU���������� 

};
