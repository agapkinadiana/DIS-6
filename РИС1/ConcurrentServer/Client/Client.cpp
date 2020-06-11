#include "pch.h"
#include "Winsock2.h"
#include <string>
#include <iostream>
#include <ctime>

#pragma comment(lib, "WS2_32.lib") 

using namespace std;

string  GetErrorMsgText(int code)
{
	switch (code)
	{
	case WSAEINTR:			return "Работа функции прервана ";
	case WSAEACCES:			return "Разрешение отвергнуто";
	case WSAEFAULT:			return "Ошибочный адрес";
	case WSAEINVAL:			return "Ошибка в аргументе";
	case WSAEMFILE:			return "Слишком много файлов открыто";
	case WSAEWOULDBLOCK:	return "Ресурс временно недоступен";
	case WSAEINPROGRESS:	return "Операция в процессе развития";
	case WSAEALREADY:		return "Операция уже выполняется";
	case WSAENOTSOCK:		return "Сокет задан неправильно";
	case WSAEDESTADDRREQ:	return "Требуется адрес расположения";
	case WSAEMSGSIZE:		return "Сообщение слишком длинное";
	case WSAEPROTOTYPE:		return "Неправильный тип протокола для сокета";
	case WSAENOPROTOOPT:	return "Ошибка в опции протокола";
	case WSAEPROTONOSUPPORT: return "Протокол не поддерживается";
	case WSAESOCKTNOSUPPORT: return "Тип сокета не поддерживается";
	case WSAEOPNOTSUPP:		return "Операция не поддерживается";
	case WSAEPFNOSUPPORT:	return "Тип протоколов не поддерживается";
	case WSAEAFNOSUPPORT:	return "Тип адресов не поддерживается протоколом";
	case WSAEADDRINUSE:		return "Адрес уже используется";
	case WSAEADDRNOTAVAIL:	return "Запрошенный адрес не может быть использован";
	case WSAENETDOWN:		return "Сеть отключена";
	case WSAENETUNREACH:	return "Сеть не достижима";
	case WSAENETRESET:		return "Сеть разорвала соединение";
	case WSAECONNABORTED:	return "Программный отказ связи";
	case WSAECONNRESET:		return "Связь восстановлена";
	case WSAENOBUFS:		return "Не хватает памяти для буферов";
	case WSAEISCONN:		return "Сокет уже подключен";
	case WSAENOTCONN:		return "Сокет не подключен";
	case WSAESHUTDOWN:		return "Нельзя выполнить send : сокет завершил работу";
	case WSAETIMEDOUT:		return "Закончился отведенный интервал  времени";
	case WSAECONNREFUSED:	return "Соединение отклонено";
	case WSAEHOSTDOWN:		return "Хост в неработоспособном состоянии";
	case WSAEHOSTUNREACH:	return "Нет маршрута для хоста";
	case WSAEPROCLIM:		return "Слишком много процессов";
	case WSASYSNOTREADY:	return "Сеть не доступна";
	case WSAVERNOTSUPPORTED: return "Данная версия недоступна";
	case WSANOTINITIALISED:	return "Не выполнена инициализация WS2_32.DLL";
	case WSAEDISCON:		return "Выполняется отключение";
	case WSATYPE_NOT_FOUND: return "Класс не найден";
	case WSAHOST_NOT_FOUND:	return "Хост не найден";
	case WSATRY_AGAIN:		return "Неавторизированный хост не найден";
	case WSANO_RECOVERY:	return "Неопределенная  ошибка";
	case WSANO_DATA:		return "Нет записи запрошенного типа";
	case WSA_INVALID_HANDLE: return "Указанный дескриптор события  с ошибкой";
	case WSA_INVALID_PARAMETER: return "Один или более параметров с ошибкой";
	case WSA_IO_INCOMPLETE:	return "Объект ввода - вывода не в сигнальном состоянии";
	case WSA_IO_PENDING:	return "Операция завершится позже";
	case WSA_NOT_ENOUGH_MEMORY: return "Не достаточно памяти";
	case WSA_OPERATION_ABORTED: return "Операция отвергнута";
	case WSASYSCALLFAILURE: return "Аварийное завершение системного вызова";
	default:				return "**ERROR**";
	};
};

string  SetErrorMsgText(string msgText, int code)
{
	return  msgText + GetErrorMsgText(code);
};

char* get_message(int msg)
{
	char echo[] = "Echo";
	char time[] = "Time";
	char rand[] = "0001";
	char r[] = "";
	switch (msg)
	{
	case 1: 	return echo;
	case 2: 	return  time;
	case 3: 	return  rand;
	default:
		return r;
	}
}

int main()
{
	SOCKET  cS;
	WSADATA wsaData;
	setlocale(0, "rus");
	try
	{
		SOCKET  cC;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((cS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("socket:", WSAGetLastError());

		if ((cC = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;  
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr("127.0.0.1");

		if ((connect(cC, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)
			throw  SetErrorMsgText("connect:", WSAGetLastError());

		char message[50];
		int  libuf = 0,	lobuf = 0, service;

		cout << "Выберите функцию\n1 - Echo\n2 - Time\n3 - Random\n";
		scanf("%d", &service);
		
		char* outMessage = new char[5];
		strcpy(outMessage, get_message(service));

		if ((lobuf = send(cC, outMessage, strlen(outMessage) + 1, NULL)) == SOCKET_ERROR)
			throw  SetErrorMsgText("send:", WSAGetLastError());

		printf("send: %s\n", outMessage);

		if ((libuf = recv(cC, message, sizeof(message), NULL)) == SOCKET_ERROR)
			throw  SetErrorMsgText("recv:", WSAGetLastError());

		if (!strcmp(message, "TimeOUT")) {
			cout << "Time out\n";
			return -1;
		}
		if (service == 1)
		{
			for (int j = 10; j >= 0; --j) {
				Sleep(1000);
				sprintf(outMessage, "%d", j);
				if ((lobuf = send(cC, outMessage, strlen(outMessage) + 1, NULL)) == SOCKET_ERROR)
					throw  SetErrorMsgText("send:", WSAGetLastError());

				printf("send: %s\n", outMessage);

				if ((libuf = recv(cC, message, sizeof(message), NULL)) == SOCKET_ERROR)
					throw  SetErrorMsgText("recv:", WSAGetLastError());

				printf("receive: %s\n", message);
			}
		}
		else if (service == 2 || service == 3) {
			printf("receive: %s\n", message);
		}
		else {
			printf("receive: %s\n", message);
		}
		if (closesocket(cS) == SOCKET_ERROR)
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}
	system("pause");
	return 0;
}
