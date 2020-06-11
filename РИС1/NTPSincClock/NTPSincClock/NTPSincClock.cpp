// NTPSincClock.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <locale>
#include <time.h>
#include <chrono>
#include <thread>
#include <utility>
#include <string>
#include <chrono>

#include "Winsock2Error.h"

#pragma comment(lib, "WS2_32.lib")

double setAverageCorrection( long long averageCorrection[], int length)
{
	if (length == 1)
		return 0;
	else {
		double value = 0;
		for (int i = 1; i < length; i++)
			value += averageCorrection[i];

		return value / (length - 1);
	}
}


struct SETSINCRO
{
	long long correction;
};

struct GETSINCHRO
{
	 long long  curvalue;
};

struct NTP_packet
{
	CHAR		head[4];
	DWORD32		RootDelay; //Задержка. Общее время распространения сигнала в обе стороны в коротком формате NTP.
	DWORD32		RootDispersion; //Дисперсия. Общая дисперсия для источника времени в коротком формате NTP.
	CHAR		ReferenceIdentifier[4]; //Идентификатор источника. Код источника синхронизации.Зависит от значения в поле Часовой слой.
	DWORD		ReferenceTimestamp[2]; //Время обновления. Время, когда система последний раз устанавливала или корректировала время.
	DWORD64		OriginateTimestamp; //Начальное время. Время клиента, когда запрос отправляется серверу.
	DWORD64		TransmitTimestamp[2];// Время отправки. Время сервера, когда запрос отправляется клиенту
	DWORD32		KeyIdentifier;
	DWORD64		MessageDigest[2];
};


void Sinc( long long *l, clock_t *def)
{

	int d = 2208988800;//(70*365 + 17)*24*60*60 = 2208988800

	SOCKET s;
	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("88.147.254.228");
	server.sin_port = htons(123);

	NTP_packet out_buf, in_buf;
	ZeroMemory(&out_buf, sizeof(out_buf));
	ZeroMemory(&in_buf, sizeof(in_buf));
	out_buf.head[0] = 0x1B; //00 011 011
							//Индикатор коррекции Длина — 2 бита Целое число, показывающее предупреждение о секунде координации.
							   //Номер версии Длина — 3 бита Целое число, представляющее версию протокола.
								   //Режим Длина — 3 бита Целое число, представляющее режим. 3 - клиент

	out_buf.head[1] = 0x00; //Часовой слой. Целое число, представляющее часовой слой. 0 - Не определено или недопустим
	out_buf.head[2] = 4; //Интервал опроса.	Целое число со знаком, представляющее максимальный
						//интервал между последовательными сообщениями.Значение равно двоичному логарифму секунд.
	out_buf.head[3] = 0xEC; //Точность. Целое число со знаком, представляющее точность системных часов.Значение равно двоичному логарифму секунд.

	if ((s = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw SetErrorMsgText("Socket: ", WSAGetLastError());

	int lenout = 0, lenin = 0, lensockaddr = sizeof(server);

	if ((lenout = sendto(s, (char*)&out_buf, sizeof(out_buf), NULL, (sockaddr*)&server, lensockaddr)) == SOCKET_ERROR)
		throw  WSAGetLastError();

	if ((lenin = recvfrom(s, (char*)&in_buf, sizeof(in_buf), 0, (sockaddr*)&server, &lensockaddr)) == SOCKET_ERROR)
		throw  WSAGetLastError();
	*def = clock();


	in_buf.TransmitTimestamp[0] = ntohl(in_buf.TransmitTimestamp[0]) - d;
	in_buf.TransmitTimestamp[1] = ntohl(in_buf.TransmitTimestamp[1]);
	int ms = (int) 1000.0*((double)(in_buf.TransmitTimestamp[1]) / (double)0xffffffff);

	char buffer[30];

	time_t temp = in_buf.TransmitTimestamp[0];
	tm* timeinfo = localtime(&temp);
	strftime(buffer, 30, "%Y-%m-%d  %X.", timeinfo);

	std::cout << "NTP ServerTime:" << std::endl;
	std::cout << buffer << ms << std::endl;
	auto now = std::chrono::system_clock::now();
	*l = in_buf.TransmitTimestamp[0] * 1000 + ms;
	std::cout << *l- std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() << std::endl;
};

void run(int msec,long long *l, clock_t *def) {
	auto endless = [=]() {
		while (true) {
			Sinc(l, def);
			std::this_thread::sleep_for(std::chrono::milliseconds(msec));
		}
	};

	std::thread thread(endless);

	thread.detach();
};

int main()
{
	setlocale(LC_ALL, "Russian");
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		throw SetErrorMsgText("Startup: ", WSAGetLastError());
	SETSINCRO setsincro;
	GETSINCHRO getsincro;

	ZeroMemory(&setsincro, sizeof(setsincro));
	ZeroMemory(&getsincro, sizeof(getsincro));


	setsincro.correction = 0;

	long long Cs = 1, Cstemp;
	long long averageCorrection[11];
	double average = 0;

	cout << "Server Run" << endl;
	clock_t start;
	run(10000, &Cs, &start);
	try
	{
		SOCKET sS;
		
		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("Socket: ", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = INADDR_ANY;


		if (::bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
			throw SetErrorMsgText("Bind: ", WSAGetLastError());
			

		SOCKADDR_IN client;
		int len = sizeof(client);

		clock_t end;
		int count = 0;
		while (count != 11)
		{
			recvfrom(sS, (char *)&getsincro, sizeof(getsincro), NULL, (sockaddr*)&client, &len);
			
			end = clock();
			
			setsincro.correction = (Cs + (end-start)) - getsincro.curvalue ;
			
			
			sendto(sS, (char *)&setsincro, sizeof(setsincro), 0, (sockaddr*)&client, len);

			averageCorrection[count] = setsincro.correction;
			average = setAverageCorrection(averageCorrection, count + 1);

			cout << "[" << count << "] :   " << inet_ntoa(client.sin_addr) << endl << " Correction = " << setsincro.correction << ", Average Correction = " << average << endl << endl;
			count++;
		}
		cout << "Average Correction = " << average << endl << endl << endl;


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
