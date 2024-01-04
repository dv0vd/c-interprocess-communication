#include<Windows.h>
#include <WindowsX.h>
#include <Shlobj_core.h>
#include <strsafe.h>

#define SETTEXT 1
#define COPYDATA 2
#define SEND 3
#define IDC_LIST 4

HINSTANCE handle;
HKEY hKey;
HWND hWnd = NULL, hInput,hListBoxSETTEXT, hListBoxCOPYDATA;

VOID CreatingElements(HINSTANCE hInstance, HWND hWnd)
{
	CreateWindow("BUTTON", "WM_SETTEXT", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 10, 10, 250, 20, hWnd, (HMENU)SETTEXT, hInstance, NULL);
	CreateWindow("BUTTON", "WM_COPYDATA", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 300, 10, 250, 20, hWnd, (HMENU)COPYDATA, hInstance, NULL);
	CheckRadioButton(hWnd, SETTEXT, COPYDATA, SETTEXT);
	hInput = CreateWindow("Edit", NULL, WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 40, 400, 20, hWnd, NULL, hInstance, 0);
	CreateWindow("BUTTON", "Отправить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 425,40, 100, 20, hWnd, (HMENU)SEND, hInstance, NULL);
	CreateWindow("static", "WM_SETTEXT", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10,70, 150, 20, hWnd, (HMENU)1, hInstance, NULL);
	CreateWindow("static", "WM_COPYDATA", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 70, 150, 20, hWnd, (HMENU)1, hInstance, NULL);
	hListBoxSETTEXT = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 10, 100, 280, 280, hWnd, NULL, hInstance, NULL);
	hListBoxCOPYDATA = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 300, 100, 280, 280, hWnd, NULL, hInstance, NULL);
	Edit_LimitText(hInput, 30);
}

VOID SendThroughSETTEXT()
{
	CHAR lpszBufferText[31];
	GetWindowText(hInput, lpszBufferText, MAX_PATH);
	Edit_SetText(hInput, NULL);
	DWORD subKeysCount = 0; 
	if(ERROR_SUCCESS ==  RegQueryInfoKey(hKey, NULL, NULL, NULL, &subKeysCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
	{
		HKEY  hSubKey = NULL;
		TCHAR szSubKey[20];
		for (DWORD dwIndex = 0; dwIndex < subKeysCount; ++dwIndex)
		{
			DWORD cchSubKey = _countof(szSubKey);
			if (ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex, szSubKey, &cchSubKey, NULL, NULL, NULL, NULL))
			{
				if (ERROR_SUCCESS == RegOpenKeyEx(hKey, szSubKey, 0, KEY_QUERY_VALUE, &hSubKey))
				{
					HWND destinationHWND;
					DWORD cb = sizeof(HWND);
					if (ERROR_SUCCESS == RegQueryValueEx(hSubKey, TEXT("hWnd"), NULL, NULL, (LPBYTE)&destinationHWND, &cb))
					{
						SendMessage(destinationHWND, WM_SETTEXT, 0, (LPARAM)lpszBufferText);
					}
					else
						MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
				}
				else
					MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
			}
			else
				MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		}
	} 
	else
		MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

VOID SendThroughCOPYDATA()
{
	CHAR lpszBufferText[31];
	GetWindowText(hInput, lpszBufferText, MAX_PATH);
	Edit_SetText(hInput, NULL);
	COPYDATASTRUCT cds;
	cds.lpData = (PVOID)lpszBufferText;
	cds.cbData = sizeof(lpszBufferText);
	cds.dwData = 0;
	DWORD subKeysCount = 0;
	if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, NULL, &subKeysCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
	{
		HKEY  hSubKey = NULL;
		TCHAR szSubKey[20];
		for (DWORD dwIndex = 0; dwIndex < subKeysCount; ++dwIndex)
		{
			DWORD cchSubKey = _countof(szSubKey);
			if (ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex, szSubKey, &cchSubKey, NULL, NULL, NULL, NULL))
			{
				if (ERROR_SUCCESS == RegOpenKeyEx(hKey, szSubKey, 0, KEY_QUERY_VALUE, &hSubKey))
				{
					HWND destinationHWND;
					DWORD cb = sizeof(HWND);
					if (ERROR_SUCCESS == RegQueryValueEx(hSubKey, TEXT("hWnd"), NULL, NULL, (LPBYTE)&destinationHWND, &cb))
					{
						SendMessage(destinationHWND, WM_COPYDATA, (WPARAM)0, (LPARAM)&cds);
					}
					else
						MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
				}
				else
					MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
			}
			else
				MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		}
	}
	else
		MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

VOID OnSETTEXT(HWND hwnd, LPCTSTR lpszText)
{
	SendMessage(hListBoxSETTEXT, LB_ADDSTRING, (WPARAM)0, (LPARAM)lpszText);
}

VOID OnCOPYDATA(HWND hwnd, HWND hwndFrom, PCOPYDATASTRUCT pcds)
{
	SendMessage(hListBoxCOPYDATA, LB_ADDSTRING, (WPARAM)0, (LPARAM)pcds->lpData);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	HANDLE_MSG(hWnd, WM_SETTEXT, OnSETTEXT);
	case WM_COPYDATA:
	{
		return HANDLE_WM_COPYDATA(hWnd, wParam, lParam, OnCOPYDATA);
	}
	case WM_CLOSE: {
		PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	case WM_QUIT: {
		DestroyWindow(hWnd);
		return 0;
	}
	case WM_CREATE: {
		CreatingElements(handle, hWnd);
		return 0;
	}
	case WM_COMMAND:{
		if (SETTEXT <= LOWORD(wParam) && LOWORD(wParam) <= COPYDATA)
			CheckRadioButton(hWnd, SETTEXT, COPYDATA, LOWORD(wParam));
		if (LOWORD(wParam) == SEND)
		{
			if (IsDlgButtonChecked(hWnd, SETTEXT))
				SendThroughSETTEXT();
			else
				SendThroughCOPYDATA();
		}
		return 0;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WNDCLASSEX RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX window = { sizeof(WNDCLASSEX) };
	window.style = CS_HREDRAW | CS_VREDRAW;
	window.lpfnWndProc = WindowProcedure;
	window.hInstance = hInstance;
	window.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	window.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	window.lpszMenuName = NULL;
	window.lpszClassName = TEXT("WindowClass");
	window.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	return window;
}

VOID MessagesProcessing() {
	MSG  msg;
	BOOL bRet;
	while (bRet = GetMessage(&msg, NULL, 0, 0) != FALSE)
	{
		if (bRet != -1)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

VOID WriteRegistry()
{
	DWORD pos;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\MyApp"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &pos) == ERROR_SUCCESS)
	{
		DWORD dwProcessId = GetCurrentProcessId();
		HKEY  hSubKey = NULL;
		TCHAR szSubKey[20]; 
		StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), dwProcessId);
		if (RegCreateKey(hKey, szSubKey, &hSubKey) == ERROR_SUCCESS)
		{
			RegSetValueEx(hSubKey, TEXT("ProcessId"), 0, REG_DWORD, (LPBYTE)&dwProcessId, sizeof(DWORD));
			RegSetValueEx(hSubKey, TEXT("hWnd"), 0, REG_BINARY, (LPBYTE)&hWnd, sizeof(HWND));
			RegCloseKey(hSubKey);
		}
		else
			MessageBox(hWnd, TEXT("Ошибка создания вложенного ключа реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
	}
	else
		MessageBox(hWnd, TEXT("Ошибка создания ключа реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

VOID RemoveRegistry()
{
	DWORD subKeysCount = 0;
	if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, NULL, &subKeysCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
	{
		if (subKeysCount == 1)
		{
			RegDeleteTree(hKey, NULL);
			RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\MyApp"));
		}
		else
		{
			TCHAR szSubKey[20]; 
			StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), GetCurrentProcessId());
			RegDeleteKey(hKey, szSubKey);
		}
		RegCloseKey(hKey);
	}
	else
		MessageBox(hWnd, TEXT("Ошибка получения информации о ключе реестра!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	handle = hInstance;
	if (!RegisterClassEx(&RegisterWindowClass(hInstance)))
		return EXIT_FAILURE;
	LoadLibrary(TEXT("ComCtl32.dll"));
	hWnd = CreateWindowEx(0, TEXT("WindowClass"), TEXT(""), WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, 100, 100, 600, 500, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
		return EXIT_FAILURE;
	WriteRegistry();
	ShowWindow(hWnd, nCmdShow);
	MessagesProcessing();
	RemoveRegistry();
	return EXIT_SUCCESS;
}