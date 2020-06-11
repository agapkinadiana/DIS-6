#include "stdafx.h"
#include <iostream>
#include "Winsock2.h"
#include "Errors.h"
#include <string>
#include <list>
#include <time.h>

#define AS_SQUIRT 10
using namespace std;

SOCKET sS;
SOCKET sSUDP;
int serverPort;
char dll[50];
char namedPipe[10];
volatile long accepted = 0, failed = 0, finished = 0, active = 0;

HANDLE hAcceptServer, hConsolePipe, hGarbageCleaner, hDispatchServer, hResponseServer;
HANDLE hClientConnectedEvent = CreateEvent(NULL, FALSE, FALSE, L"ClientConnected");;

DWORD WINAPI AcceptServer(LPVOID pPrm); 
DWORD WINAPI ConsolePipe(LPVOID pPrm);
DWORD WINAPI GarbageCleaner(LPVOID pPrm);
DWORD WINAPI DispatchServer(LPVOID pPrm);
DWORD WINAPI ResponseServer(LPVOID pPrm);
bool  GetRequestFromClient(char* name, short port, SOCKADDR_IN* from, int* flen);
bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto);

CRITICAL_SECTION scListContact;

enum TalkersCommand {
	START, STOP, EXIT, STATISTICS, WAIT, SHUTDOWN, GETCOMMAND
};
volatile TalkersCommand  previousCommand = GETCOMMAND;      // предыдуща€ команда клиента 

struct Contact         // элемент списка подключений       
{
	enum TE {               // состо€ние  сервера подключени€  
		EMPTY,              // пустой элемент списка подключений 
		ACCEPT,             // подключен (accept), но не обслуживаетс€
		CONTACT             // передан обслуживающему серверу  
	}    type;     // тип элемента списка подключений 
	enum ST {               // состо€ние обслуживающего сервера  
		WORK,               // идет обмен данными с клиентом
		ABORT,              // обслуживающий сервер завершилс€ не нормально 
		TIMEOUT,            // обслуживающий сервер завершилс€ по времени 
		FINISH              // обслуживающий сервер завершилс€  нормально 
	}      sthread; // состо€ние  обслуживающего сервера (потока)

	SOCKET      s;         // сокет дл€ обмена данными с клиентом
	SOCKADDR_IN prms;      // параметры  сокета 
	int         lprms;     // длина prms 
	HANDLE      hthread;   // handle потока (или процесса) 
	HANDLE      htimer;    // handle таймера
	HANDLE		serverHThtead;// handle обслуживающего сервера который в последствие может зависнуть

	char msg[50];           // сообщение 
	char srvname[15];       //  наименование обслуживающего сервера 

	Contact(TE t = EMPTY, const char* namesrv = "") // конструктор 
	{
		ZeroMemory(&prms, sizeof(SOCKADDR_IN));
		lprms = sizeof(SOCKADDR_IN);
		type = t;
		strcpy(srvname, namesrv);
		msg[0] = 0x00;
	};

	void SetST(ST sth, const char* m = "")
	{
		sthread = sth;
		strcpy(msg, m);
	}
};
typedef list<Contact> ListContact;

ListContact contacts;

bool AcceptCycle(int sq)
{
	bool rc = false;
	Contact c(Contact::ACCEPT, "EchoServer");
	while (sq-- > 0 && !rc)
	{
		if ((c.s = accept(sS, (sockaddr*)&c.prms, &c.lprms)) == INVALID_SOCKET)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				throw  SetErrorMsgText("accept:", WSAGetLastError());
		}
		else
		{
			rc = true;
			EnterCriticalSection(&scListContact);
			contacts.push_front(c);
			LeaveCriticalSection(&scListContact);
			puts("contact connected");
			InterlockedIncrement(&accepted);
			InterlockedIncrement(&active);
		}
	}
	return rc;
};

void openSocket() {
	SOCKADDR_IN serv, clnt;
	u_long nonblk = 1;

	if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
		throw  SetErrorMsgText("socket:", WSAGetLastError());

	int lclnt = sizeof(clnt);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(serverPort);
	serv.sin_addr.s_addr = INADDR_ANY;

	if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("bind:", WSAGetLastError());
	if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
		throw  SetErrorMsgText("listen:", WSAGetLastError());
	if (ioctlsocket(sS, FIONBIO, &nonblk) == SOCKET_ERROR)
		throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());
}

void closeSocket() {
	if (closesocket(sS) == SOCKET_ERROR)
		throw  SetErrorMsgText("closesocket:", WSAGetLastError());
}

void CommandsCycle(TalkersCommand& cmd)
{
	int sq = 0;
	while (cmd != EXIT)
	{
		switch (cmd)
		{
		case START: cmd = GETCOMMAND;
			if (previousCommand != START) {
				sq = AS_SQUIRT;
				cout << "Start command\n";
				openSocket();
				previousCommand = START;
			}
			else cout << "--Start already in use--\n";
			break;
		case STOP:  cmd = GETCOMMAND;  
			if (previousCommand != STOP) {
				sq = 0;
				cout << "Stop command\n";
				closeSocket();
				previousCommand = STOP;
			}	
			else cout << "--Stop already in use--\n";

			break;
		case WAIT:  cmd = GETCOMMAND;
			sq = 0;
			cout << "Wait command\n";
			cout << "--—окет закрыт дл€ ожидани€ завершени€ обслуживани€ клиентов--\n";
			closeSocket();
			while (contacts.size() != 0);
			cout << "--Size of contacts " << contacts.size() << "--\n";
			cmd = START;
			previousCommand = WAIT;
			cout << "--—окет открыт--\n";
			break;
		case SHUTDOWN:  
			sq = 0;
			cout << "SHUTDOWN command\n";
			cout << "----------¬ыключение----------\n";
			closeSocket();
			while (contacts.size() != 0);
			cout << "--Size of contacts " << contacts.size() << "--\n";
			cmd = EXIT;
			break;
		case GETCOMMAND:  cmd = GETCOMMAND;
			break;
		};
		if (cmd != STOP) {
			if (AcceptCycle(sq))
			{
				cmd = GETCOMMAND;
				SetEvent(hClientConnectedEvent);
			}
			else SleepEx(0, TRUE);
		}
	};
};

DWORD WINAPI AcceptServer(LPVOID pPrm)
{
	cout << "AcceptServer запущен;\n";
	DWORD rc = 0;
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		CommandsCycle(*((TalkersCommand*)pPrm));
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}
	cout << "AcceptServer остановлен;\n";
	ExitThread(*(DWORD*)pPrm);
}

TalkersCommand set_param(char* param) {
	if (!strcmp(param, "start")) return START;
	if (!strcmp(param, "stop")) return STOP;
	if (!strcmp(param, "exit")) return EXIT;
	if (!strcmp(param, "wait")) return WAIT;
	if (!strcmp(param, "shutdown")) return SHUTDOWN;
	if (!strcmp(param, "statistics")) return STATISTICS;
	if (!strcmp(param, "getcommand")) return GETCOMMAND;
}

DWORD WINAPI ConsolePipe(LPVOID pPrm)
{
	cout << "ConsolePipe запущен;\n";

	DWORD rc = 0;
	char rbuf[100];
	DWORD dwRead, dwWrite;
	HANDLE hPipe;
	try
	{
		char NPname[50];
		sprintf(NPname, "\\\\.\\pipe\\%s", namedPipe);
		if ((hPipe = CreateNamedPipeA(NPname,PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE)
			throw SetPipeError("create:", GetLastError());
		if (!ConnectNamedPipe(hPipe, NULL))
			throw SetPipeError("connect:", GetLastError());
		TalkersCommand& param = *((TalkersCommand*)pPrm);
		while (param != EXIT) {
			cout << "--Connecting to Named Pipe Client--\n";
			ConnectNamedPipe(hPipe, NULL);
			while (ReadFile(hPipe, rbuf, sizeof(rbuf), &dwRead, NULL))
			{
				cout << "—ообщение от клиента:  " << rbuf << endl;
				param = set_param(rbuf);
				if (param == STATISTICS)
				{
					char stat[200];
					sprintf(stat, "\n----STATISTICS----\n"\
						"общее количество подключений:    %d\n" \
						"общее количество отказов:        %d\n" \
						"завершилось успешно:             %d\n" \
						"количество активных подключений: %d\n" \
						"-------------------------------------", accepted, failed, finished, contacts.size());
					WriteFile(hPipe, stat, sizeof(stat), &dwWrite, NULL);
				}
				if (param != STATISTICS)
					WriteFile(hPipe, rbuf, strlen(rbuf) + 1, &dwWrite, NULL);
			}
			cout << "-------------- анал закрыт-----------------\n";
			DisconnectNamedPipe(hPipe);
		}
		CloseHandle(hPipe);
		cout << "ConsolePipe остановлен;\n" << endl;
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText << endl;
	}
	ExitThread(rc);
}

DWORD WINAPI GarbageCleaner(LPVOID pPrm)
{
	cout << "GarbageCleaner запущен;\n";

	DWORD rc = 0;	
	while (*((TalkersCommand*)pPrm) != EXIT) {
		int listSize = 0;
		int howMuchClean = 0;
		if (contacts.size() != 0) {
			for (auto i = contacts.begin(); i != contacts.end();) {
				if (i->type == i->EMPTY) {
					EnterCriticalSection(&scListContact);
					if (i->sthread == i->FINISH)
						InterlockedIncrement(&finished);
					if (i->sthread == i->ABORT || i->sthread == i->TIMEOUT)
						InterlockedIncrement(&failed);
					i = contacts.erase(i);
					howMuchClean++;
					listSize = contacts.size();
					LeaveCriticalSection(&scListContact); 
					Sleep(2000);
				}
				else ++i;
			}
		}
	}
	cout << "GarbageCleaner остановлен;\n";
	ExitThread(rc);
}

HANDLE(*ts)(char*, LPVOID);
HMODULE st;

void CALLBACK ASWTimer(LPVOID Prm, DWORD, DWORD) {
	Contact *contact = (Contact*)(Prm);
	cout << "ASWTimer is calling " << contact->hthread << endl;
	TerminateThread(contact->serverHThtead, NULL);
	send(contact->s, "TimeOUT", strlen("TimeOUT") + 1, NULL);
	EnterCriticalSection(&scListContact);
	CancelWaitableTimer(contact->htimer);
	contact->type = contact->EMPTY;
	contact->sthread = contact->TIMEOUT;
	LeaveCriticalSection(&scListContact);
}

DWORD WINAPI DispatchServer(LPVOID pPrm)
{
	cout << "DispathServer запущен;\n";

	DWORD rc = 0;
	TalkersCommand& command = *(TalkersCommand*)pPrm;
	while (command != EXIT)
	{
		if (command != STOP) {
			WaitForSingleObject(hClientConnectedEvent, 5000);
			ResetEvent(hClientConnectedEvent);
			EnterCriticalSection(&scListContact);
			for (auto i = contacts.begin(); i != contacts.end(); i++) {
				if (i->type == i->ACCEPT) {
					u_long nonblk = 0;
					if (ioctlsocket(i->s, FIONBIO, &nonblk) == SOCKET_ERROR)
						throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());

					char serviceType[5];
					clock_t start = clock();
					recv(i->s, serviceType, sizeof(serviceType), NULL);
					strcpy(i->msg, serviceType);

					clock_t delta = clock() - start;
					if (delta > 5000) {
						cout << "It's so long\n";
						i->sthread = i->TIMEOUT;
						if ((send(i->s, "TimeOUT", strlen("TimeOUT") + 1, NULL)) == SOCKET_ERROR)
							throw  SetErrorMsgText("send:", WSAGetLastError());
						if (closesocket(i->s) == SOCKET_ERROR)
							throw  SetErrorMsgText("closesocket:", WSAGetLastError());
						i->type = i->EMPTY;
					}
					else if (delta <= 5000) {
						if (strcmp(i->msg, "Echo") && strcmp(i->msg, "Time") && strcmp(i->msg, "0001")) {
							if ((send(i->s, "ErrorInquiry", strlen("ErrorInquiry") + 1, NULL)) == SOCKET_ERROR)
								throw  SetErrorMsgText("send:", WSAGetLastError());
							i->sthread = i->ABORT;
							i->type = i->EMPTY;
							if (closesocket(i->s) == SOCKET_ERROR)
								throw  SetErrorMsgText("closesocket:", WSAGetLastError());
						}
						else {
							i->type = i->CONTACT;
							i->hthread = hAcceptServer;
							i->serverHThtead = ts(serviceType, (LPVOID)&(*i));
							i->htimer = CreateWaitableTimer(0, FALSE, 0);
							LARGE_INTEGER Li;
							int seconds = 60;
							Li.QuadPart = -(10000000 * seconds);
							SetWaitableTimer(i->htimer, &Li, 0, ASWTimer, (LPVOID)&(*i), FALSE);
							SleepEx(0, TRUE);
						}
					}
				}
			}
			LeaveCriticalSection(&scListContact);
		}
	}
	cout << "DispathServer остановлен;\n";
	ExitThread(rc);
}

bool PutAnswerToClient(char * name, sockaddr* to, int * lto) {

	char msg[] = "You can connect to server ";
	if ((sendto(sSUDP, msg, sizeof(msg) + 1, NULL, to, *lto)) == SOCKET_ERROR)
		throw  SetErrorMsgText("sendto:", WSAGetLastError());
	return false;
}

bool  GetRequestFromClient(char* name, short port, SOCKADDR_IN* from, int* flen)
{
	SOCKADDR_IN clnt;
	int lc = sizeof(clnt);
	ZeroMemory(&clnt, lc);
	char ibuf[500]; 
	int  lb = 0;
	int optval = 1;
	int TimeOut = 1000;
	setsockopt(sSUDP, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int));
	setsockopt(sSUDP, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut));
	while (true) {
		if ((lb = recvfrom(sSUDP, ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, &lc)) == SOCKET_ERROR) return false;
		if (!strcmp(name, ibuf)) {
			*from = clnt;
			*flen = lc;
			return true;
		}
		cout << "\nBad name\n";
	}
	return false;
}

DWORD WINAPI ResponseServer(LPVOID pPrm)
{
	cout << "ResponseServer запущен;\n";

	DWORD rc = 0;
	WSADATA wsaData;
	SOCKADDR_IN serv;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		throw  SetErrorMsgText("Startup:", WSAGetLastError());
	if ((sSUDP = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw  SetErrorMsgText("socket:", WSAGetLastError());
	serv.sin_family = AF_INET;  
	serv.sin_port = htons(serverPort);
	serv.sin_addr.s_addr = INADDR_ANY; 

	if (bind(sSUDP, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("bind:", WSAGetLastError());
	
	SOCKADDR_IN someServer;
	int serverSize = sizeof(someServer);

	SOCKADDR_IN from;
	int lc = sizeof(from);
	ZeroMemory(&from, lc);
	int numberOfClients = 0;
	while (*(TalkersCommand*)pPrm != EXIT)
	{
		try
		{
			if (GetRequestFromClient("Hello", serverPort, &from, &lc))
			{
				printf("\nConnected Client: є%d, port: %d, ip: %s", ++numberOfClients, htons(from.sin_port), inet_ntoa(from.sin_addr));
				PutAnswerToClient("Hello", (sockaddr*)&from, &lc);
			}
		}
		catch (string errorMsgText)
		{
			cout << endl << errorMsgText;
		}
	}
	if (closesocket(sSUDP) == SOCKET_ERROR)
		throw  SetErrorMsgText("closesocket:", WSAGetLastError());
	if (WSACleanup() == SOCKET_ERROR)
		throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	cout << "ResponseServer остановлен;\n"; 
	ExitThread(rc);
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	if (argc == 2) {
		serverPort = atoi(argv[1]);
	}
	else if (argc == 3) {
		serverPort = atoi(argv[1]);
		strcpy(dll, argv[2]);
	}
	else if (argc == 4) {
		serverPort = atoi(argv[1]);
		strcpy(dll, argv[2]);
		strcpy(namedPipe, argv[3]);
	}
	else {
		serverPort = 2000;
		strcpy(dll, "Win32Project1.dll");
		strcpy(namedPipe, "BOX");
	}

	cout << "ѕараллельный сервер запущен:" << endl;
	cout << "Port - " << serverPort << endl;
	cout << "NamedPipe - " << namedPipe << endl;

	st = LoadLibraryA(dll);
	ts = (HANDLE(*)(char*, LPVOID))GetProcAddress(st, "SSS");
	if (st == NULL)
		cout << "ќшибка загрузки DLL" << endl;
	else
		cout << "«агруженна€ DLL - " << dll << endl << endl;

	volatile TalkersCommand cmd = START;

	InitializeCriticalSection(&scListContact);

	hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
	hDispatchServer = CreateThread(NULL, NULL, DispatchServer, (LPVOID)&cmd, NULL, NULL);
	hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
	hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
	hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);

	SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL);
	SetThreadPriority(hDispatchServer, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hConsolePipe, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);

	WaitForSingleObject(hAcceptServer, INFINITE);
	WaitForSingleObject(hDispatchServer, INFINITE);
	WaitForSingleObject(hGarbageCleaner, INFINITE);
	WaitForSingleObject(hConsolePipe, INFINITE);
	WaitForSingleObject(hResponseServer, INFINITE);

	CloseHandle(hAcceptServer);
	CloseHandle(hDispatchServer);
	CloseHandle(hGarbageCleaner);
	CloseHandle(hConsolePipe);
	CloseHandle(hResponseServer);

	DeleteCriticalSection(&scListContact);

	FreeLibrary(st);
	return 0;
};