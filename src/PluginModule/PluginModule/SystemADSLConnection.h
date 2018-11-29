#pragma once

#include <windows.h>
#include <tchar.h>

#include <ras.h>
#pragma comment(lib, "rasapi32.lib")

//存放活动连接句柄, 用于断开连接
HRASCONN hRasConn = NULL;
HRASCONN hRasReConn = NULL;

//-----枚举活动的连接-----
HRASCONN RasConnecting()
{
	RASCONN rc[10] = { 0 };
	DWORD dwSize;
	DWORD dwNumber = 0;
	rc[0].dwSize = sizeof(RASCONN);
	dwSize = sizeof(rc);

	DWORD dwRet;
	dwRet = RasEnumConnections(rc, &dwSize, &dwNumber);     //枚举已连接的网络
	if (dwRet == 0)
	{
		for (UINT i = 0; i < dwNumber; i++)
		{
			if (lstrcmp(rc[i].szEntryName, _T("宽带连接")) == 0)
			{
				return rc[i].hrasconn;      //返回"我的连接"对应的句柄
			}
		}
	}

	return NULL;
}


//-----断开连接-----
BOOL RasClose(HRASCONN  hrasconn)
{
	//断开成功
	return (RasHangUp(hrasconn) == 0);
}
__inline static BOOL ADSLConnect()
{

	if ((hRasConn = RasConnecting()) != NULL)  //如果是已经连接
	{
		if (RasClose(hRasConn) == TRUE)
		{
			MessageBox(NULL, _T("Error"), _T("断开连接"), 0);
		}
	}
	
	// 重新拨号
	if (hRasReConn)
	{
		RasHangUp(hRasReConn);
		hRasReConn = NULL;
		MessageBox(NULL, _T("Error"), _T("断开连接"), 0);
	}

	RASDIALPARAMS rdParams;
	DWORD dwRet;
	BOOL f_get_password;

	memset(&rdParams, 0, sizeof(rdParams));
	rdParams.dwSize = sizeof(RASDIALPARAMS);
	lstrcpy(rdParams.szEntryName, _T("宽带连接"));
	if (RasGetEntryDialParams(NULL, &rdParams, &f_get_password) != 0)
	{
		MessageBox(NULL, _T("Error"), _T("Dial Failed, get entry adsl failed"), 0);

		return -1;
	}
	if (f_get_password == false)
	{
		MessageBox(NULL, _T("Error"), _T("Dial Failed: get password failed"), 0);

		return -1;
	}

	if ((dwRet = RasDial(NULL, NULL, &rdParams, 0L, NULL, &hRasReConn)) != 0)
	{
		MessageBox(NULL, _T("Error"), _T("Dial Failed"), 0);

		return -1;
	}
	return 1;

}