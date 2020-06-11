// ServerSincClock.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include "pch.h"

#include <locale>
#include <time.h>
#include <Winsock2.h>
#include "Winsock2Error.h"

#pragma comment(lib, "ws2_32.lib")

int setAverageCorrection(int averageCorrection[], int length)
{
	int value = 0;
	for (int i = 1; i < length; i++)
		value += averageCorrection[i];

	return value / length;
}

struct SETSINCRO      //response from server
{
	string cmd;
	int correction;   //add time
};

struct GETSINCHRO     //request for sync
{
	string cmd;
	int curvalue;     //time counter
};


int main()
{
	setlocale(LC_ALL, "Russian");

	SETSINCRO setsincro;
	GETSINCHRO getsincro;

	ZeroMemory(&setsincro, sizeof(setsincro));
	ZeroMemory(&getsincro, sizeof(getsincro));

	setsincro.cmd = "SINC";
	setsincro.correction = 0;

	clock_t Cs;
	int averageCorrection[11];
	int average = 0;

	cout << "Server Run" << endl;

	try
	{
		SOCKET sS;
		WSADATA wsaData;


		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("Socket: ", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr("127.0.0.1");

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
			throw SetErrorMsgText("Bind: ", WSAGetLastError());

		SOCKADDR_IN client;
		int len = sizeof(client);

		int count = 0;
		while (true)
		{
			recvfrom(sS, (char *)&getsincro, sizeof(getsincro), NULL, (sockaddr*)&client, &len);

			Cs = clock();
			setsincro.correction = Cs - getsincro.curvalue;

			sendto(sS, (char *)&setsincro, sizeof(setsincro), 0, (sockaddr*)&client, sizeof(client));

			averageCorrection[count] = setsincro.correction;
			average = setAverageCorrection(averageCorrection, count + 1);

			cout << "[" << count << "] :   " << inet_ntoa(client.sin_addr) << endl << " Correction = " << setsincro.correction << ", Average Correction = " << average << endl << endl;
			count++;
		}
		cout << "Average Correction = " << average;

		if (closesocket(sS) == SOCKET_ERROR)
			throw SetErrorMsgText("close socket: ", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}

	return 0;
}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
