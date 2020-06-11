#pragma once
#include "pch.h"
#include <iostream>
#include <string>
#include <Winsock2.h>
using namespace std;

string GetErrorMsgText(int code);

string SetErrorMsgText(string msgText, int code);
