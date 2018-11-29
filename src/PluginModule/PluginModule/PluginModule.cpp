#include "commonheader.h"
#include "SystemService.h"
#include "SystemAutoRunBoot.h"
#include "SystemTaskScheduler.h"
#include "SystemInputLanguage.h"
#include "SystemNetworkAdapter.h"
#include "SystemProcessCpuUse.h"
#include "SystemAccountUser.h"
#include "SystemNetworkConnection.h"
#include "SystemSharedFolder.h"
#include "SystemSharedSessions.h"
#include "SystemSharedConnection.h"
#include "SystemNetworkResource.h"
#include "SystemHardwareDevice.h"
#include "SystemFirewallManager.h"
#include "SystemCodePage.h"

int _tmain(int argc, _TCHAR ** argv)
{
	_tsetlocale(LC_ALL, _T("chs"));

	ElevationDebugPrivilege(GetCurrentProcess());
	ElevationBackupPrivilege(GetCurrentProcess());
	ElevationRestorePrivilege(GetCurrentProcess());
	ElevationTrustedCredmanAccessPrivilege(GetCurrentProcess());
	ElevationImpersonatePrivilege(GetCurrentProcess());
	
	_tprintf(_T("Current system=%s\r\n"), GetWindowsSystemVersionName().c_str());
	
	switch (argc)
	{
	case 2:
	case 3:
	{
		switch (*argv[1])
		{
		case '1':
		{
			// 获取系统服务列表
			{
				GetSystemServicesList();
			}
		}
		break;
		case '2':
		{
			// 获取系统任务计划列表
			{
				GetSystemTaskSchedulersList();
			}
		}
		break;
		case '3':
		{
			// 获取系统输入法列表
			{
				GetSystemInputLanguage();
			}
		}
		break;
		case '4':
		{
			// 获取系统自启动项列表
			{
				GetSystemAutoRunBootList();
			}
		}
		break;
		case '5':
		{
			// 获取系统网卡信息列表
			{
				GetSystemNetworkAdapterList();
				//GetSystemNetworkAdapterExList();
			}
		}
		break;
		case '6':
		{
			// 获取系统指定进程的CPU使用率
			{
				CPUUsage cpuUsage(_ttol(argv[2]));
				for (int i = 0; i >= 0; i++)
				{
					DOUBLE dCpuUsage = cpuUsage.GetCpuUsage();
					_tprintf(_T("[%s]: %.2f%%\n"), argv[2], dCpuUsage);
					Sleep(500);
				}
			}
		}
		break;
		case '7':
		{
			// 获取系统用户账号列表
			{
				GetSystemAccountUserList();
			}
		}
		break;
		case '8':
		{
			// 获取网络连接列表
			{
				CProtocolsConnection pc;
				pc.GetProtocolsConnectionInfoList();
			}
		}
		break;
		case '9':
		{
			// 获取共享文件夹列表
			{
				GetSystemSharedFolderList();
			}
		}
		break;
		case 'a':
		{
			// 获取共享会话列表
			{
				GetSystemSharedSessionList();
			}
		}
		break;
		case 'b':
		{
			// 获取共享连接列表
			{
				GetSystemSharedConnectionList();
			}
		}
		break;
		case 'c':
		{
			// 获取共享资源列表
			{
				GetSystemNetworkResourceList();
			}
		}
		break;
		case 'd':
		{
			// 获取硬件设备列表
			{
				GetSystemHardwareDeviceList();
			}
		}
		break;
		case 'e':
		{
			// 获取系统防火墙列表
			{
				GetSystemFirewallInfoList();
			}
		}
		break;
		case 'f':
		{
			// 获取系统代码页列表
			{
				GetSystemCodePageList();
			}
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}
	
	getchar();

	return 0;
}