#include "stdafx.h"
#include "Errors.h"

string  GetErrorMsgText(int code)  
{
	switch (code)
	{
	case WSAEINTR:			return "������ ������� �������� ";
	case WSAEACCES:			return "���������� ����������";
	case WSAEFAULT:			return "��������� �����";
	case WSAEINVAL:			return "������ � ���������";
	case WSAEMFILE:			return "������� ����� ������ �������";
	case WSAEWOULDBLOCK:	return "������ �������� ����������";
	case WSAEINPROGRESS:	return "�������� � �������� ��������";
	case WSAEALREADY:		return "�������� ��� �����������";
	case WSAENOTSOCK:		return "����� ����� �����������";
	case WSAEDESTADDRREQ:	return "��������� ����� ������������";
	case WSAEMSGSIZE:		return "��������� ������� �������";
	case WSAEPROTOTYPE:		return "������������ ��� ��������� ��� ������";
	case WSAENOPROTOOPT:	return "������ � ����� ���������";
	case WSAEPROTONOSUPPORT: return "�������� �� ��������������";
	case WSAESOCKTNOSUPPORT: return "��� ������ �� ��������������";
	case WSAEOPNOTSUPP:		return "�������� �� ��������������";
	case WSAEPFNOSUPPORT:	return "��� ���������� �� ��������������";
	case WSAEAFNOSUPPORT:	return "��� ������� �� �������������� ����������";
	case WSAEADDRINUSE:		return "����� ��� ������������";
	case WSAEADDRNOTAVAIL:	return "����������� ����� �� ����� ���� �����������";
	case WSAENETDOWN:		return "���� ���������";
	case WSAENETUNREACH:	return "���� �� ���������";
	case WSAENETRESET:		return "���� ��������� ����������";
	case WSAECONNABORTED:	return "����������� ����� �����";
	case WSAECONNRESET:		return "����� �������������";
	case WSAENOBUFS:		return "�� ������� ������ ��� �������";
	case WSAEISCONN:		return "����� ��� ���������";
	case WSAENOTCONN:		return "����� �� ���������";
	case WSAESHUTDOWN:		return "������ ��������� send : ����� �������� ������";
	case WSAETIMEDOUT:		return "���������� ���������� ��������  �������";
	case WSAECONNREFUSED:	return "���������� ���������";
	case WSAEHOSTDOWN:		return "���� � ����������������� ���������";
	case WSAEHOSTUNREACH:	return "��� �������� ��� �����";
	case WSAEPROCLIM:		return "������� ����� ���������";
	case WSASYSNOTREADY:	return "���� �� ��������";
	case WSAVERNOTSUPPORTED: return "������ ������ ����������";
	case WSANOTINITIALISED:	return "�� ��������� ������������� WS2_32.DLL";
	case WSAEDISCON:		return "����������� ����������";
	case WSATYPE_NOT_FOUND: return "����� �� ������";
	case WSAHOST_NOT_FOUND:	return "���� �� ������";
	case WSATRY_AGAIN:		return "������������������ ���� �� ������";
	case WSANO_RECOVERY:	return "��������������  ������";
	case WSANO_DATA:		return "��� ������ ������������ ����";
	case WSA_INVALID_HANDLE: return "��������� ���������� �������  � �������";
	case WSA_INVALID_PARAMETER: return "���� ��� ����� ���������� � �������";
	case WSA_IO_INCOMPLETE:	return "������ ����� - ������ �� � ���������� ���������";
	case WSA_IO_PENDING:	return "�������� ���������� �����";
	case WSA_NOT_ENOUGH_MEMORY: return "�� ���������� ������";
	case WSA_OPERATION_ABORTED: return "�������� ����������";
	case WSASYSCALLFAILURE: return "��������� ���������� ���������� ������";
	default:				return "**ERROR**";
	};
};

string  SetErrorMsgText(string msgText, int code)
{
	return  msgText + GetErrorMsgText(code);
};

string  SetPipeError(string msgText, int code)
{
	return  msgText + GetErrorMsgText(code);
};
