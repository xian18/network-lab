#include "socket_lab.h"
#pragma warning(disable : 4996)
using namespace std;
int InitServerSocket(SOCKET& serverSoc, WSADATA& wsaData, sockaddr_in& addr)
{
	int nRc;
	nRc = WSAStartup(0x0101, &wsaData);
	if (nRc)
	{
		nRc = WSAGetLastError();
		cout << "WSAStartup Error" << endl;
		OutPutErrorMessage(nRc);
		return nRc;
	}
	if (wsaData.wVersion != 0x0101) 
	{
		cout << "Could not find a usable version of Winsock.dll" << endl;
		return ERROR; 
	}
	cout << "WSAStartup succeed!" << endl;

	serverSoc=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serverSoc==INVALID_SOCKET)
	{
		nRc = WSAGetLastError();
		cout << "Init Socket Error" << endl;
		OutPutErrorMessage(nRc);
		WSACleanup();
		return nRc;
	}
	nRc = bind(serverSoc, (sockaddr*)&addr, sizeof(addr));
	if (nRc==SOCKET_ERROR)
	{
		nRc = WSAGetLastError();
		cout << "Socket Bind Error" << endl;
		OutPutErrorMessage(nRc);
		WSACleanup();
		return nRc;
	}
	cout << "socket bind port:" << ntohs(addr.sin_port) << " succeed" << endl;

	nRc = listen(serverSoc, SOMAXCONN);
	if (nRc==SOCKET_ERROR)
	{
		nRc = WSAGetLastError();
		cout << "Socket Listen Error" << endl;
		OutPutErrorMessage(nRc);
		WSACleanup();
		return nRc;
	}
	cout << "socket start listen request" << endl;

	return NO_ERROR;
}

int DeliverRequest(SOCKET& serverSoc)
{
	int continueFlag = 1;
	SOCKET* clientSoc;
	sockaddr_in addr;
	int addrLen;
	int nRc;
	cout << "please input something contain char q to stop this server." << endl;
	_beginthread(ListenInputChar, 0, &continueFlag);
	while (continueFlag)
	{
		addrLen = sizeof(addr);
		clientSoc = (SOCKET*)malloc(sizeof(SOCKET));
		if (clientSoc == NULL) {
			cout << "malloc new socket error" << endl;
			OutPutErrorMessage(WSA_NOT_ENOUGH_MEMORY);
			continue;
		}
		*clientSoc = accept(serverSoc, (sockaddr*)&addr, &addrLen);
		if (*clientSoc == INVALID_SOCKET)
		{
			nRc = WSAGetLastError();
			cout << "accept client socket request error" << endl;
			OutPutErrorMessage(nRc);
			free(clientSoc);
			continue;
		}
		cout << "Request From :";
		PrintIPV4AddrInfo(&addr);
		_beginthread(HandleHttpRequset, 0, clientSoc);
	}
	return 0;
}

void HandleHttpRequset(void* ctSoc)
{
	SOCKET* ct = (SOCKET*)ctSoc;
	int len = 0;
	char RequestMethod[8];
	char url[2048];
	wchar_t urls[4096];
	DWORD urlBufferLen = 2047;
	memset(RequestMethod, 0, 8);
	if (recv(*ct, RequestMethod, 4, 0) == SOCKET_ERROR)
	{
		OutPutErrorMessage(WSAGetLastError());
		if (closesocket(*ct) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
		free(ct);
		_endthread();
		return;
	}
	if (strcmp(RequestMethod, "GET "))
	{
		SendHttpNotOK(405, ct);
		_endthread();
		return;
	}

	memset(url, 0, 2048);
	if (recv(*ct, url, 2047, 0) == SOCKET_ERROR)
	{
		OutPutErrorMessage(WSAGetLastError());
		if (closesocket(*ct) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
		free(ct);
		_endthread();
		return;
	}
	for (len = 0; ; len++)
	{
		if (len == 2048)
		{
			SendHttpNotOK(414, ct);
			_endthread();
			return;
		}
		if (url[len] != ' ' && url[len] != '?' && url[len] != '#') { continue; }
		memset(url + len, 0, 2048 - len);
		break;
	}
	if (UrlUnescapeA(url, NULL, &urlBufferLen, URL_UNESCAPE_INPLACE | URL_UNESCAPE_AS_UTF8) != S_OK)
	{
		SendHttpNotOK(414, ct);
		_endthread();
		return;
	}
	memset(urls, 0, sizeof(urls));
	memcpy(urls, HomeDir, 2048);
	MultiByteToWideChar(CP_UTF8, 0, url, -1, urls + HomeDirLenght, 2048);
	FILE* fp;
	fp = _wfopen(urls, L"rb");
	if (fp == NULL)
	{
		SendHttpNotOK(404, ct);
		_endthread();
		return;
	}
	SendHttpOK(fp, ct);
	fclose(fp);
	free(ct);
	_endthread();
	return;
}

