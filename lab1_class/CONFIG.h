#pragma once
#include "header.h"
#include "utils.h"
#include "SERVER.h"
using namespace std;
class CONFIG
{
public:
	static wstring HomeDir;
	static string IP;
	static u_short Port;
	static int InitConfig();
	static const wchar_t* GetHomeDir();
	static u_long GetHomeDirLength();
	static u_short GetHumanPortNumber();
	static u_short GetNetworkPortNumber();
	static u_short SetSocketPort(sockaddr_in& addr);
	static const char* GetHumanIP();
	static int SetSocketIP(sockaddr_in& addr);
};

