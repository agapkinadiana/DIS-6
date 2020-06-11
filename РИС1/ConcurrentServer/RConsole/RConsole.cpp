#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

string SetPipeError(string msgText, int code)
{
	char intStr[20];
	_itoa(code, intStr, 10);
	string str = string(intStr);
	return msgText + " " + str;
};

int main()
{
	setlocale(LC_ALL, "Russian");

	char rbuf[200];
	DWORD dwRead, dwWrite;
	HANDLE hPipe;
	int n;

	try
	{
		if ((hPipe = CreateFile(L"\\\\.\\pipe\\BOX", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
			throw  SetPipeError("createfile:", GetLastError());

		char wbuf[40] = "start";
		int command = 0;
		do {
			cout << "\nInput server command" << endl;
			cout << "1 - start" << endl;
			cout << "2 - stop" << endl;
			cout << "3 - wait" << endl;
			cout << "4 - statistics" << endl;
			cout << "5 - shutdown" << endl;
			cout << "6 - exit" << endl;
			cin >> command;

			switch (command) {
			case 1:
				strcpy(wbuf, "start");
				system("cls");
				break;
			case 2:
				strcpy(wbuf, "stop");
				system("cls");
				break;
			case 3:
				strcpy(wbuf, "wait");
				system("cls");
				break;
			case 4:
				strcpy(wbuf, "statistics");
				system("cls");
				break;
			case 5:
				strcpy(wbuf, "shutdown");
				system("cls");
				break;
			case 6:
				strcpy(wbuf, "exit");
				system("cls");
				break;
			}

			if (!WriteFile(hPipe, wbuf, sizeof(wbuf), &dwWrite, NULL))
				throw  SetPipeError("writefile:", GetLastError());

			if (ReadFile(hPipe, rbuf, sizeof(rbuf), &dwRead, NULL))
				cout << "receive:  " << rbuf << endl;
		} while (command != 6);

	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText << endl;
	}

	CloseHandle(hPipe);
	system("pause");
	return 0;
}

