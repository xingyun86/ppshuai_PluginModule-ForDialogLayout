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
			// ��ȡϵͳ�����б�
			{
				GetSystemServicesList();
			}
		}
		break;
		case '2':
		{
			// ��ȡϵͳ����ƻ��б�
			{
				GetSystemTaskSchedulersList();
			}
		}
		break;
		case '3':
		{
			// ��ȡϵͳ���뷨�б�
			{
				GetSystemInputLanguage();
			}
		}
		break;
		case '4':
		{
			// ��ȡϵͳ���������б�
			{
				GetSystemAutoRunBootList();
			}
		}
		break;
		case '5':
		{
			// ��ȡϵͳ������Ϣ�б�
			{
				GetSystemNetworkAdapterList();
				//GetSystemNetworkAdapterExList();
			}
		}
		break;
		case '6':
		{
			// ��ȡϵͳָ�����̵�CPUʹ����
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
			// ��ȡϵͳ�û��˺��б�
			{
				GetSystemAccountUserList();
			}
		}
		break;
		case '8':
		{
			// ��ȡ���������б�
			{
				CProtocolsConnection pc;
				pc.GetProtocolsConnectionInfoList();
			}
		}
		break;
		case '9':
		{
			// ��ȡ�����ļ����б�
			{
				GetSystemSharedFolderList();
			}
		}
		break;
		case 'a':
		{
			// ��ȡ����Ự�б�
			{
				GetSystemSharedSessionList();
			}
		}
		break;
		case 'b':
		{
			// ��ȡ���������б�
			{
				GetSystemSharedConnectionList();
			}
		}
		break;
		case 'c':
		{
			// ��ȡ������Դ�б�
			{
				GetSystemNetworkResourceList();
			}
		}
		break;
		case 'd':
		{
			// ��ȡӲ���豸�б�
			{
				GetSystemHardwareDeviceList();
			}
		}
		break;
		case 'e':
		{
			// ��ȡϵͳ����ǽ�б�
			{
				GetSystemFirewallInfoList();
			}
		}
		break;
		case 'f':
		{
			// ��ȡϵͳ����ҳ�б�
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