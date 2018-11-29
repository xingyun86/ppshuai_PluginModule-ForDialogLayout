#pragma once

#include <windows.h>
#include <tchar.h>

#include <ras.h>
#pragma comment(lib, "rasapi32.lib")

//��Ż���Ӿ��, ���ڶϿ�����
HRASCONN hRasConn = NULL;
HRASCONN hRasReConn = NULL;

//-----ö�ٻ������-----
HRASCONN RasConnecting()
{
	RASCONN rc[10] = { 0 };
	DWORD dwSize;
	DWORD dwNumber = 0;
	rc[0].dwSize = sizeof(RASCONN);
	dwSize = sizeof(rc);

	DWORD dwRet;
	dwRet = RasEnumConnections(rc, &dwSize, &dwNumber);     //ö�������ӵ�����
	if (dwRet == 0)
	{
		for (UINT i = 0; i < dwNumber; i++)
		{
			if (lstrcmp(rc[i].szEntryName, _T("�������")) == 0)
			{
				return rc[i].hrasconn;      //����"�ҵ�����"��Ӧ�ľ��
			}
		}
	}

	return NULL;
}


//-----�Ͽ�����-----
BOOL RasClose(HRASCONN  hrasconn)
{
	//�Ͽ��ɹ�
	return (RasHangUp(hrasconn) == 0);
}
__inline static BOOL ADSLConnect()
{

	if ((hRasConn = RasConnecting()) != NULL)  //������Ѿ�����
	{
		if (RasClose(hRasConn) == TRUE)
		{
			MessageBox(NULL, _T("Error"), _T("�Ͽ�����"), 0);
		}
	}
	
	// ���²���
	if (hRasReConn)
	{
		RasHangUp(hRasReConn);
		hRasReConn = NULL;
		MessageBox(NULL, _T("Error"), _T("�Ͽ�����"), 0);
	}

	RASDIALPARAMS rdParams;
	DWORD dwRet;
	BOOL f_get_password;

	memset(&rdParams, 0, sizeof(rdParams));
	rdParams.dwSize = sizeof(RASDIALPARAMS);
	lstrcpy(rdParams.szEntryName, _T("�������"));
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