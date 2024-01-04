#include<Windows.h>
#include<iostream>
#include<locale.h>
#include <strsafe.h>
using namespace std;

HANDLE hMail;
SERVICE_STATUS gSvcStatus;
SERVICE_STATUS_HANDLE gSvcStatusHandle;
struct REQUEST
{
	DWORD dwProcessId;
	DWORD dwIndex;
};
constexpr LPCTSTR szStudents[] = {
	TEXT("Адлейба Эраст ИТ-32"),
	TEXT("Давыдов Вячеслав ИТ-32"),
	TEXT("Душенькин Кирилл ИТ-32"),
	TEXT("Жуков Владислав ИТ-32"),
	TEXT("Колесникова Олеся ИТ-32"),
	TEXT("Косовцов Николай ИТ-32"),
	TEXT("Марков Виталий ИТ-32"),
	TEXT("Махонин Вячеслав ИТ-32"),
	TEXT("Насыпалов Никита ИТ-32"),
	TEXT("Папков Владислав ИТ-32"),
	TEXT("Раковская Марина ИТ-32"),
	TEXT("Тараненко Александр ИТ-32"),
	TEXT("Чушкин Андрей ИТ-32"),
	TEXT("Шевцова Елена ИТ-32")
};

DWORD WINAPI SvcHandler(DWORD fdwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	if (SERVICE_CONTROL_STOP == fdwControl || SERVICE_CONTROL_SHUTDOWN == fdwControl)
	{
		CloseHandle(hMail);
		gSvcStatus.dwCurrentState = SERVICE_STOP_PENDING; 
	} 
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
	return NO_ERROR;
}

DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	TCHAR szNameMailslot[256];
	for (;;)
	{
		REQUEST Request;
		DWORD nBytes;
		BOOL bRet = ReadFile(hMail, &Request, sizeof(Request), &nBytes, NULL);
		if (FALSE != bRet)
		{
			StringCchPrintf(szNameMailslot, _countof(szNameMailslot), TEXT("\\\\.\\mailslot\\ClientMailSlot\\%u"), Request.dwProcessId);
			HANDLE hWriteMailslot = CreateFile(szNameMailslot, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (INVALID_HANDLE_VALUE != hWriteMailslot)
			{
				TCHAR szResponse[100] = TEXT("");
				Request.dwIndex--;
				if (Request.dwIndex < _countof(szStudents))
				{
					StringCchCopy(szResponse, _countof(szResponse), szStudents[Request.dwIndex]);
				}
				else
				{
					StringCchCopy(szResponse, _countof(szResponse), "Ошибка");

				}
				WriteFile(hWriteMailslot, szResponse, sizeof(szResponse), &nBytes, NULL);
				CloseHandle(hWriteMailslot);
			} 
		} 
		else
		{
			DWORD dwError = GetLastError();
			if (ERROR_INVALID_HANDLE == dwError)
			{
				break; 
			} 
		}
	} 
	return 0;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	gSvcStatusHandle = RegisterServiceCtrlHandlerEx("StudentsInformationService", SvcHandler, NULL);
	if (NULL != gSvcStatusHandle)
	{
		gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		gSvcStatus.dwCurrentState = SERVICE_START_PENDING;
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		gSvcStatus.dwWin32ExitCode = NO_ERROR;
		gSvcStatus.dwServiceSpecificExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 0;
		SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
		SECURITY_DESCRIPTOR sd; 
		if (FALSE != InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		{
			if (SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE) != FALSE)
			{
				SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
				sa.lpSecurityDescriptor = &sd;
				hMail = CreateMailslot("\\\\.\\mailslot\\ServerMailSlot", 0, MAILSLOT_WAIT_FOREVER, &sa);
				if (hMail != INVALID_HANDLE_VALUE)
				{
					gSvcStatus.dwCurrentState = SERVICE_RUNNING;
					SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
					DWORD dwExitCode = SvcMain(dwArgc, lpszArgv);
					if (dwExitCode != 0)
					{
						gSvcStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
						gSvcStatus.dwServiceSpecificExitCode = dwExitCode;
					}
					else
					{
						gSvcStatus.dwWin32ExitCode = NO_ERROR;
					}
				}
			}
		}
		gSvcStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
	}
}

INT main(INT argc, LPTSTR argv[])
{
	setlocale(LC_ALL, "rus");
	if (argc != 2)
	{
		cout << "Ошибка" << endl;
	} 
	else
	{
		string action = argv[1];
		if (action == "create")
		{
			SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (NULL == hSCM)
			{
				cout << "Ошибка открытия диспетчера управления службами Windows" << endl;
			} 
			else
			{
				TCHAR szCmdLine[MAX_PATH + 5];
				GetModuleFileName(NULL, szCmdLine, _countof(szCmdLine));
				StringCchCat(szCmdLine, _countof(szCmdLine), TEXT(" start"));
				SC_HANDLE hSvc = CreateService(hSCM, "StudentsInformationService", "Служба вывода информации о студентах",  0,SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,szCmdLine, NULL, NULL, NULL, NULL, NULL);
				if(hSvc == NULL)
					cout << "Ошибка создания службы" << endl;
				else
				{
					cout << "Служба успешно создана" << endl;
					CloseServiceHandle(hSvc);
				}
			}
			CloseServiceHandle(hSCM);
		}
		else
		{
			if (action == "remove")
			{
				SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
				if (NULL == hSCM)
				{
					cout << "Ошибка открытия диспетчера управления службами Windows" << endl;
				}
				else
				{
					SC_HANDLE hSvc = OpenService(hSCM, "StudentsInformationService", SERVICE_ALL_ACCESS);
					if (hSvc == NULL)
						cout << "Ошибка открытия службы" << endl;
					else
					{
						if (DeleteService(hSvc) != FALSE)
						{
							cout << "Служба успешно удалена" << endl;
						}
						else
							cout << "Ошибка удаления службы" << endl;
						CloseServiceHandle(hSvc);
					}
				}
				CloseServiceHandle(hSCM);
			}
			else
			{
				if (action == "start")
				{
					SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
					if (NULL == hSCM)
					{
						cout << "Ошибка открытия диспетчера управления службами Windows" << endl;
					}
					else
					{
						SC_HANDLE hSvc = OpenService(hSCM, "StudentsInformationService", SERVICE_ALL_ACCESS);
						if (hSvc == NULL)
							cout << "Ошибка открытия службы" << endl;
						else
						{
							SERVICE_TABLE_ENTRY svcDispatchTable[] =
							{
								{(LPSTR)"StudentsInformationService", ServiceMain},
								{NULL, NULL}
							};
							StartServiceCtrlDispatcher(svcDispatchTable);
							CloseServiceHandle(hSvc);
						}
					}
					CloseServiceHandle(hSCM);
				}
				else
					cout << "Ошибка" << endl;
			}
		}
	}
	system("pause");
}