#pragma once
#include "stdafx.h"
#include "Winsock2.h"
#include <string>

#pragma comment(lib, "WS2_32.lib") 
using namespace std;

string  GetErrorMsgText(int code);
string  SetErrorMsgText(string msgText, int code);

string  SetPipeError(string msgText, int code);