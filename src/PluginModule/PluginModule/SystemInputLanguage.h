#pragma once

typedef struct tagInputLanguageNode
{
	_TCHAR  tzCode[MAXCHAR];   //输入法代号
	_TCHAR  tzName[MAXBYTE];//输入法的说明文字
	tagInputLanguageNode * pNext;
}INPUTLANGUAGENODE, *PINPUTLANGUAGENODE;

class CInputLanguage
{
public:
	CInputLanguage():m_pILNH(NULL) {};
	~CInputLanguage() { Clear(); }

public:
	//加载系统中所有已安装输入法
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
	//根据输入法名称查找输入法
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

	//根据输入法代码查找输入法
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
	//获取当前输入当代码
	BOOL  GetCurrentInputLanguageCode(LPTSTR lpszCode)
	{
		return GetKeyboardLayoutName(lpszCode);
	}

	//设置当前输入法
	BOOL SelectInputLanguage(PINPUTLANGUAGENODE pILN)
	{
		if (!pILN)
		{
			return FALSE;
		}
		HKL hkl = LoadKeyboardLayout(pILN->tzCode, KLF_ACTIVATE);//装载输入法  
		if (hkl == NULL)
		{
			return FALSE;
		}
		else
		{
			ActivateKeyboardLayout(hkl, KLF_SETFORPROCESS);//激活输入法
		}

		return TRUE;
	}
	//清空已加载输入法信息
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

	// 打印获取到的输入法列表信息
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