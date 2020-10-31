#include "utils.h"
#include "socket_lab.h"
using namespace std;
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
wchar_t* HomeDir;
int HomeDirLenght;
int main()
{
	SetConsoleOutputCP(CP_UTF8);
	int t=1;
	WSADATA wsaData;
	SOCKET svrSoc;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	HomeDir = (wchar_t*)malloc(sizeof(wchar_t) * 2048);
	if (HomeDir == NULL) { return 0; }
	if (InitServerConfigFromSTDIN(addr) != NO_ERROR) { return 0; }
 	if (InitServerSocket(svrSoc, wsaData, addr) != NO_ERROR) { return 0; }
	DeliverRequest(svrSoc);
	return 0;
}