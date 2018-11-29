#pragma once

typedef struct tagInputLanguageNode
{
	_TCHAR  tzCode[MAXCHAR];   //���뷨����
	_TCHAR  tzName[MAXBYTE];//���뷨��˵������
	tagInputLanguageNode * pNext;
}INPUTLANGUAGENODE, *PINPUTLANGUAGENODE;

class CInputLanguage
{
public:
	CInputLanguage():m_pILNH(NULL) {};
	~CInputLanguage() { Clear(); }

public:
	//����ϵͳ�������Ѱ�װ���뷨
	BOOL LoadInputLanguage(void)
	{
		INT nValue = 1;
		HKEY hKeyHKCU = NULL;
		HKEY hKeyHKLM = NULL;
		DWORD dwType = REG_SZ;
		_TCHAR szId[MAXCHAR] = { 0 };
		_TCHAR  tzCode[MAXCHAR] = { 0 };
		DWORD   dwCodeSize = sizeof(tzCode) / sizeof(*tzCode);
		_TCHAR tzTemporaryKeyName[MAXBYTE] = { 0 };
		_TCHAR tzPreloadKeyName[] = _T("Keyboard Layout\\Preload");
		_TCHAR tzKeyboardLayoutsKeyName[] = _T("System\\CurrentControlSet\\Control\\Keyboard Layouts\\");

		_itot(nValue, szId, 10);

		if (::RegOpenKey(HKEY_CURRENT_USER, tzPreloadKeyName, &hKeyHKCU) == ERROR_SUCCESS)
		{
			while (::RegQueryValueEx(hKeyHKCU, szId, NULL, &dwType, (LPBYTE)tzCode, &dwCodeSize) == ERROR_SUCCESS)
			{
				wsprintf(tzTemporaryKeyName, _T("%s%s"), tzKeyboardLayoutsKeyName, tzCode);
				if (RegOpenKey(HKEY_LOCAL_MACHINE, tzTemporaryKeyName, &hKeyHKLM) == ERROR_SUCCESS)
				{
					TCHAR   tzName[MAXBYTE] = { 0 };
					DWORD   dwNameSize = sizeof(tzName) / sizeof(*tzName);
					if (RegQueryValueEx(hKeyHKLM, _T("Layout Text"), NULL, &dwType, (LPBYTE)tzName, &dwNameSize) == ERROR_SUCCESS)
					{
						PINPUTLANGUAGENODE pL = m_pILNH;
						PINPUTLANGUAGENODE pN = new INPUTLANGUAGENODE();
						_tcscpy(pN->tzCode, tzCode);
						_tcscpy(pN->tzName, tzName);
						pN->pNext = NULL;
						if (pL)
						{
							while (pL->pNext)
							{
								pL = pL->pNext;
							}
							pL->pNext = pN;
						}
						else
						{
							m_pILNH = pN;
						}
					}
				}
				::RegCloseKey(hKeyHKLM);
				nValue++;
				_itot(nValue, szId, 10);
			}
		}

		::RegCloseKey(hKeyHKCU);

		return (m_pILNH != NULL);
	}
	//�������뷨���Ʋ������뷨
	PINPUTLANGUAGENODE FindInputLanguageByName(LPCTSTR lpszName)
	{
		PINPUTLANGUAGENODE pL = m_pILNH;
		
		while (pL)
		{
			if (_tcscmp(lpszName, pL->tzName) == 0)
			{
				return pL;
			}
			pL = pL->pNext;
		}

		return NULL;
	}

	//�������뷨����������뷨
	PINPUTLANGUAGENODE FindILByCode(LPCTSTR lpszCode)
	{
		PINPUTLANGUAGENODE pL = m_pILNH;
		while (pL)
		{
			if (_tcscmp(lpszCode, pL->tzCode) == 0)
			{
				return pL;
			}
			pL = pL->pNext;
		}

		return NULL;
	}
	//��ȡ��ǰ���뵱����
	BOOL  GetCurrentInputLanguageCode(LPTSTR lpszCode)
	{
		return GetKeyboardLayoutName(lpszCode);
	}

	//���õ�ǰ���뷨
	BOOL SelectInputLanguage(PINPUTLANGUAGENODE pILN)
	{
		if (!pILN)
		{
			return FALSE;
		}
		HKL hkl = LoadKeyboardLayout(pILN->tzCode, KLF_ACTIVATE);//װ�����뷨  
		if (hkl == NULL)
		{
			return FALSE;
		}
		else
		{
			ActivateKeyboardLayout(hkl, KLF_SETFORPROCESS);//�������뷨
		}

		return TRUE;
	}
	//����Ѽ������뷨��Ϣ
	void Clear(void)
	{
		PINPUTLANGUAGENODE pN = NULL;
		PINPUTLANGUAGENODE pL = m_pILNH;

		while (pL)
		{
			pN = pL;
			pL = pL->pNext;
			delete(pN);
			pN = NULL;
		}

		m_pILNH = NULL;
	}

	// ��ӡ��ȡ�������뷨�б���Ϣ
	void Print()
	{
		PINPUTLANGUAGENODE pL = m_pILNH;
		while (pL)
		{
			_tprintf(_T("Code=%s, Name=%s\r\n"), pL->tzCode, pL->tzName);
			pL = pL->pNext;
		}
	}

private:
	PINPUTLANGUAGENODE m_pILNH;

};

void GetSystemInputLanguage()
{
	CInputLanguage il;
	il.LoadInputLanguage();
	il.Print();
}