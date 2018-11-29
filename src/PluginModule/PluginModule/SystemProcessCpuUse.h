#pragma once
///////////////////////////////////////////////////////////////////////////////////
//		C++ 获取特定进程规定CPU使用率
//原理：调用GetProcessTimes()，并与上次调用得到的结果相减，即得到某段时间内CPU的使用时间  

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

	//返回值为进程句柄，可判断OpenProcess是否成功  
	HANDLE SetProcessID(DWORD dwProcessID) 
	{
		//如果之前监视过另一个进程，就先关闭它的句柄  
		Clear();    
		Init();
		return (hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessID));
	}

	//计算并获取CPU使用率 
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
			//判断进程正在运行
			if (::GetExitCodeProcess(hProcess, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
			{
				//获取当前系统时间为文件时间
				::GetSystemTimeAsFileTime(&ftNow);
				
				//计算占用CPU的百分比  
				if (::GetProcessTimes(hProcess, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime))
				{
					llTime = FileTime2UTC(&ftNow);
					llSystemTime = FileTime2UTC(&ftKernelTime) + FileTime2UTC(&ftUserTime);

					//判断是否为首次计算  
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

	// 时间转换    
	LONGLONG FileTime2UTC(const FILETIME* ftime)
	{
		LARGE_INTEGER li = { 0L,0L };
		li.LowPart = ftime->dwLowDateTime;
		li.HighPart = ftime->dwHighDateTime;
		return li.QuadPart;
	}

	// 获得CPU的核数    
	DWORD GetNumberOfProcessor()
	{
		SYSTEM_INFO si = { 0 };
		::GetSystemInfo(&si);
		return si.dwNumberOfProcessors;
	}

	//初始化  
	void Init()
	{
		llLastSystemTime = 0LL;
		llLastTime = 0LL;
		hProcess = NULL;
	}

	//关闭进程句柄  
	void Clear()
	{
		if (hProcess) 
		{
			CloseHandle(hProcess);
			hProcess = NULL;
		}
	}

private:
	HANDLE		hProcess;				//进程句柄
	LONGLONG	llLastTime;				//上一次时间    
	LONGLONG	llLastSystemTime;		//上一次系统时间
	DWORD		dwNumberOfProcessors;	//系统CPU处理器数量 

};
