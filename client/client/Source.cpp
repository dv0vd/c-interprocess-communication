#include<Windows.h>
#include<iostream>
#include<locale.h>
#include <strsafe.h>
using namespace std;

struct REQUEST
{
	DWORD dwProcessId;
	DWORD dwIndex;
}; 

INT main(INT argc, LPTSTR argv[])
{
	setlocale(LC_ALL, "rus");
	HANDLE hServerMail = CreateFile(TEXT("\\\\.\\mailslot\\ServerMailSlot"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hServerMail)
	{
		cout << "������ �������� ��������� �����" << endl;
	}
	else 
	{
		TCHAR szNameMailslot[256];
		StringCchPrintf(szNameMailslot, _countof(szNameMailslot), TEXT("\\\\.\\mailslot\\ClientMailslot\\%u"), GetCurrentProcessId());
		SECURITY_DESCRIPTOR sd; 
		if (FALSE != InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		{
			if (SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE) != FALSE)
			{
				SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
				sa.lpSecurityDescriptor = &sd;
				HANDLE hClientMailslot = CreateMailslot(szNameMailslot, 0, MAILSLOT_WAIT_FOREVER, &sa);
				if (INVALID_HANDLE_VALUE == hClientMailslot)
				{
					cout << "������ �������� ��������� �����" << endl;
				}
				else
				{
					REQUEST Request; 
					Request.dwProcessId = GetCurrentProcessId();
					DWORD nBytes;
					TCHAR szResponse[100];
					while (TRUE)
					{
						DWORD nBytes2;
						DWORD studentId;
						cout << "ID ��������:   ";
						cin >> studentId;
						Request.dwIndex = studentId;
						if (FALSE != WriteFile(hServerMail, &Request, sizeof(Request), &nBytes2, NULL))
						{
							if (FALSE != ReadFile(hClientMailslot, szResponse, sizeof(szResponse), &nBytes, NULL))
							{
								cout << szResponse<< endl;
							}
							else
							{
								cout << "������ ������ ������ �� ��������� �����" << endl;
								break;
							}
						}
						else
						{
							cout << "������ ������ ������ � �������� ����"<<endl;
							break;
						}
					}
				}
			}
			else
			{
				cout << "������ ���������� DACL" << endl;
			}
		}
		else
			cout << "������ ������������� ��" << endl;
	}
	system("pause");
}