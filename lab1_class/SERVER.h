#pragma once
#include "header.h"
#include "CONFIG.h"
#include "HTTP.h"
#include "utils.h"
#include <wincrypt.h>

class SERVER
{
public :
	SOCKET svrSoc;
	WSADATA wsaData;
	sockaddr_in svrAddr;
	int lastError;
	SERVER();
	~SERVER();
	int GetLastError();
	int StartServer();
	static void HandleHttpRequest(void* ctSocket);
	static const char* WSAErrorCodeToChar(int errorCode);
	static char* ConvertNetworkIPToHuman(sockaddr_in& addr, char buffer[20]);
	static void PrintIPV4AddrInfo(sockaddr_in& addr);
};

class SocketWithAddr {
public:
	SOCKET clientSoc;
	sockaddr_in addr;
};