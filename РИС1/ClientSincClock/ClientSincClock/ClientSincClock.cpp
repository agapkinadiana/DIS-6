// ClientSincClock.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "Winsock2Error.h"

#pragma comment(lib, "WS2_32.lib")

struct GETSINCHRO
{
	string cmd;
	int curvalue;
};

struct SETSINCRO
{
	string cmd;
	int correction;
};



int main()
{
	setlocale(LC_CTYPE, "Russian");

	int Tc = 1000;
	int Cc = 0;
	int max = 0;
	int min = INT_MAX;

	GETSINCHRO getsincro;
	SETSINCRO setsincro;

	ZeroMemory(&setsincro, sizeof(setsincro));
	ZeroMemory(&getsincro, sizeof(getsincro));

	getsincro.cmd = "SINC";
	getsincro.curvalue = Cc;

	cout << "Client run" << endl;

	try
	{
		SOCKET cS;
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		if ((cS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("Socket: ", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr("127.0.0.1"); //ipconfig -all (из полученных найти ip воторого компа, который будет сервером)

		int len = sizeof(serv);
		sendto(cS, (char *)&getsincro, sizeof(getsincro), 0, (sockaddr*)&serv, sizeof(serv));
		recvfrom(cS, (char *)&setsincro, sizeof(setsincro), 0, (sockaddr*)&serv, &len);

		for (int i = 0; i < 10; i++)
		{
			Sleep(Tc);

			Cc = getsincro.curvalue + setsincro.correction + Tc;
			getsincro.curvalue = Cc;

			sendto(cS, (char *)&getsincro, sizeof(getsincro), 0, (sockaddr*)&serv, sizeof(serv));
			recvfrom(cS, (char *)&setsincro, sizeof(setsincro), 0, (sockaddr*)&serv, &len);

			max = max < setsincro.correction ? setsincro.correction : max;
			min = min > setsincro.correction ? setsincro.correction : min;

			cout << "[" << i + 1 << "]     Curvalue = " << getsincro.curvalue << "  Correction =" << setsincro.correction << endl << endl;

		}
		cout << "Max correction: " << max << endl;
		cout << "Min correction: " << min << endl;

		if (closesocket(cS) == SOCKET_ERROR)
			throw SetErrorMsgText("Closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText << endl;
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
