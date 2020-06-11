// ClientU.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Winsock2.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <wchar.h>

#pragma comment(lib, "WS2_32.lib")

typedef HANDLE HDFS;

using namespace std;

string GetErrorMsgText(int code)
{
	string msgText;

	switch (code)
	{
	case WSAEINTR:				 msgText = "Ðàáîòà ôóíêöèè ïðåðâàíà\n";						  break;
	case WSAEACCES:				 msgText = "Ðàçðåøåíèå îòâåðãíóòî\n";						  break;
	case WSAEFAULT:				 msgText = "Îøèáî÷íûé àäðåñ\n";								  break;
	case WSAEINVAL:				 msgText = "Îøèáêà â àðãóìåíòå\n";							  break;
	case WSAEMFILE:				 msgText = "Ñëèøêîì ìíîãî ôàéëîâ îòêðûòî\n";				  break;
	case WSAEWOULDBLOCK:		 msgText = "Ðåñóðñ âðåìåííî íåäîñòóïåí\n";					  break;
	case WSAEINPROGRESS:		 msgText = "Îïåðàöèÿ â ïðîöåññå ðàçâèòèÿ\n";				  break;
	case WSAEALREADY: 			 msgText = "Îïåðàöèÿ óæå âûïîëíÿåòñÿ\n";					  break;
	case WSAENOTSOCK:   		 msgText = "Ñîêåò çàäàí íåïðàâèëüíî\n";						  break;
	case WSAEDESTADDRREQ:		 msgText = "Òðåáóåòñÿ àäðåñ ðàñïîëîæåíèÿ\n";				  break;
	case WSAEMSGSIZE:  			 msgText = "Ñîîáùåíèå ñëèøêîì äëèííîå\n";				      break;
	case WSAEPROTOTYPE:			 msgText = "Íåïðàâèëüíûé òèï ïðîòîêîëà äëÿ ñîêåòà\n";		  break;
	case WSAENOPROTOOPT:		 msgText = "Îøèáêà â îïöèè ïðîòîêîëà\n";					  break;
	case WSAEPROTONOSUPPORT:	 msgText = "Ïðîòîêîë íå ïîääåðæèâàåòñÿ\n";					  break;
	case WSAESOCKTNOSUPPORT:	 msgText = "Òèï ñîêåòà íå ïîääåðæèâàåòñÿ\n";				  break;
	case WSAEOPNOTSUPP:			 msgText = "Îïåðàöèÿ íå ïîääåðæèâàåòñÿ\n";					  break;
	case WSAEPFNOSUPPORT:		 msgText = "Òèï ïðîòîêîëîâ íå ïîääåðæèâàåòñÿ\n";			  break;
	case WSAEAFNOSUPPORT:		 msgText = "Òèï àäðåñîâ íå ïîääåðæèâàåòñÿ ïðîòîêîëîì\n";	  break;
	case WSAEADDRINUSE:			 msgText = "Àäðåñ óæå èñïîëüçóåòñÿ\n";						  break;
	case WSAEADDRNOTAVAIL:		 msgText = "Çàïðîøåííûé àäðåñ íå ìîæåò áûòü èñïîëüçîâàí\n";	  break;
	case WSAENETDOWN:			 msgText = "Ñåòü îòêëþ÷åíà\n";								  break;
	case WSAENETUNREACH:		 msgText = "Ñåòü íå äîñòèæèìà\n";							  break;
	case WSAENETRESET:			 msgText = "Ñåòü ðàçîðâàëà ñîåäèíåíèå\n";					  break;
	case WSAECONNABORTED:		 msgText = "Ïðîãðàììíûé îòêàç ñâÿçè\n";						  break;
	case WSAECONNRESET:			 msgText = "Ñâÿçü âîññòàíîâëåíà\n";							  break;
	case WSAENOBUFS:			 msgText = "Íå õâàòàåò ïàìÿòè äëÿ áóôåðîâ\n";				  break;
	case WSAEISCONN:			 msgText = "Ñîêåò óæå ïîäêëþ÷åí\n";							  break;
	case WSAENOTCONN:			 msgText = "Ñîêåò íå ïîäêëþ÷åí\n";							  break;
	case WSAESHUTDOWN:			 msgText = "Íåëüçÿ âûïîëíèòü send: ñîêåò çàâåðøèë ðàáîòó\n";  break;
	case WSAETIMEDOUT:			 msgText = "Çàêîí÷èëñÿ îòâåäåííûé èíòåðâàë  âðåìåíè\n";		  break;
	case WSAECONNREFUSED:		 msgText = "Ñîåäèíåíèå îòêëîíåíî\n";						  break;
	case WSAEHOSTDOWN:			 msgText = "Õîñò â íåðàáîòîñïîñîáíîì ñîñòîÿíèè\n";			  break;
	case WSAEHOSTUNREACH:		 msgText = "Íåò ìàðøðóòà äëÿ õîñòà\n";						  break;
	case WSAEPROCLIM:			 msgText = "Ñëèøêîì ìíîãî ïðîöåññîâ\n";						  break;
	case WSASYSNOTREADY:		 msgText = "Ñåòü íå äîñòóïíà\n";							  break;
	case WSAVERNOTSUPPORTED:	 msgText = "Äàííàÿ âåðñèÿ íåäîñòóïíà\n";					  break;
	case WSANOTINITIALISED:		 msgText = "Íå âûïîëíåíà èíèöèàëèçàöèÿ WS2_32.DLL\n";		  break;
	case WSAEDISCON:			 msgText = "Âûïîëíÿåòñÿ îòêëþ÷åíèå\n";						  break;
	case WSATYPE_NOT_FOUND:		 msgText = "Êëàññ íå íàéäåí\n";								  break;
	case WSAHOST_NOT_FOUND:		 msgText = "Õîñò íå íàéäåí\n";								  break;
	case WSATRY_AGAIN:			 msgText = "Íåàâòîðèçèðîâàííûé õîñò íå íàéäåí\n";			  break;
	case WSANO_RECOVERY:		 msgText = "Íåîïðåäåëåííàÿ îøèáêà\n";						  break;
	case WSANO_DATA:			 msgText = "Íåò çàïèñè çàïðîøåííîãî òèïà\n";				  break;
	case WSA_INVALID_HANDLE:	 msgText = "Óêàçàííûé äåñêðèïòîð ñîáûòèÿ  ñ îøèáêîé\n";		  break;
	case WSA_INVALID_PARAMETER:	 msgText = "Îäèí èëè áîëåå ïàðàìåòðîâ ñ îøèáêîé\n";			  break;
	case WSA_IO_INCOMPLETE:		 msgText = "Îáúåêò ââîäà-âûâîäà íå â ñèãíàëüíîì ñîñòîÿíèè\n"; break;
	case WSA_IO_PENDING:		 msgText = "Îïåðàöèÿ çàâåðøèòñÿ ïîçæå\n";					  break;
	case WSA_NOT_ENOUGH_MEMORY:	 msgText = "Íå äîñòàòî÷íî ïàìÿòè\n";						  break;
	case WSA_OPERATION_ABORTED:	 msgText = "Îïåðàöèÿ îòâåðãíóòà\n";							  break;
	case WSAEINVALIDPROCTABLE:	 msgText = "Îøèáî÷íûé ñåðâèñ\n";							  break;
	case WSAEINVALIDPROVIDER:	 msgText = "Îøèáêà â âåðñèè ñåðâèñà\n";						  break;
	case WSAEPROVIDERFAILEDINIT: msgText = "Íåâîçìîæíî èíèöèàëèçèðîâàòü ñåðâèñ\n";			  break;
	case WSASYSCALLFAILURE:		 msgText = "Àâàðèéíîå çàâåðøåíèå ñèñòåìíîãî âûçîâà\n";		  break;
	default:					 msgText = "Error\n";										  break;
	};
	return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
};

SOCKET cS;
SOCKADDR_IN serv;
int len;
char inBuf[30];
bool EnterCA(char* FileName)
{
	string str("enter");
	str.append(FileName);
	int c = sendto(cS, str.c_str(), str.length(), NULL, (sockaddr*)&serv, len);

	recvfrom(cS, inBuf, sizeof(inBuf), NULL, (sockaddr*)&serv, &len);

	if (inBuf)
	{
		return true;
	}
	return false;
}

bool LeaveCA(char* FileName)
{
	string str("leave");
	str.append(FileName);
	sendto(cS, str.c_str(), str.length(), NULL, (sockaddr*)&serv, len);
	return true;
}

HDFS OpenDFSFile(char* FileName, char* FileWay)
{
	if (EnterCA(FileName))
	{
		HANDLE ptrFile = CreateFileA(FileWay, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ptrFile != NULL)
		{
			return ptrFile;
		}
		return 0;
	}
	return 0;
}

int ReadDFSFile(HDFS hdfs, void* buf, int bufsize)
{
	int s;
	if (ReadFile(hdfs, buf, bufsize, (LPDWORD)&s, NULL))
	{
		return s;
	}
	else return -1;
}

int WriteDFSFile(HDFS hdfs, void* buf, int bufsize)
{
	int s;
	SetFilePointer(hdfs, NULL, NULL, FILE_END);
	if (WriteFile(hdfs, buf, bufsize, (LPDWORD)&s, NULL))
	{
		FlushFileBuffers(hdfs);//ñáðîñèòü áóôåð
		return s;
	}
	else return -1;
}

void CloseDFSFile(HDFS hdfs, char* FileName)
{
	CloseHandle(hdfs);
	LeaveCA(FileName);
}

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, "Russian");

	string IP = "127.0.0.1";
	time_t rawtime;
	struct tm * timeinfo;


	char buffer[80];                               
	cout << "ClientU" << endl;

	try
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		if ((cS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("Socket: ", WSAGetLastError());
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(IP.c_str());
		len = sizeof(serv);

		char buf[200];


		while (true)
		{
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			ZeroMemory(buffer, 80);
			ZeroMemory(buf, 200);
			strftime(buffer, 80, "%c", timeinfo);
			HDFS h;
			h = OpenDFSFile("111", "С:\\lab2-1.txt");
			if (h)
			{
				/*if (ReadDFSFile(h, buf, 10) > 0)
					strcat(buffer, " ");

				strcat(buffer, inBuf);
					cout << "111: " << buffer << endl;

					WriteDFSFile(h, buffer, strlen(buffer));
					WriteDFSFile(h, "\r\n", strlen("\r\n"));
			}
			else break;
			CloseDFSFile(h, "111");
			Sleep(5000);*/

				cout << "OpenDFSFile" << endl;
				if (ReadDFSFile(h, buf, 10) > 0)
				{
					cout << "ReadDFSFile" << endl;
					cout << buf << endl;

					strcat(buffer, " ");

					strcat(buffer, inBuf);
					for (int i = 0; i < 10; i++)
					{
						cout << "111: " << buffer << endl;

						WriteDFSFile(h, buffer, strlen(buffer));
						WriteDFSFile(h, "\r\n", strlen("\r\n"));
					}
					cout << "TEXT WRITED" << endl;

					ZeroMemory(buffer, 80);
					Sleep(5000);
					CloseDFSFile(h, "111");
				}
			}
		}


		if (closesocket(cS) == SOCKET_ERROR)
			throw SetErrorMsgText("Closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText << endl;
	}
	cout << endl;
	return 0;
}