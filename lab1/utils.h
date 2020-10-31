#pragma once
#include "header.h"

void ListenInputChar(void* need);

void OutPutErrorMessage(int errorCode);

void PrintIPV4AddrInfo(sockaddr_in* addr);

int InitServerConfigFromSTDIN(sockaddr_in& addr);

int GetOntInt(std::istream& inStream, std::ostream& outStream, int numMin, int numMax);

int SendHttpNotOK(int httpcode, SOCKET* ct);

int SendHttpOK(FILE* fp, SOCKET* ct);
/*len = sprintf_s(sendBuffer, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
if (send(*ct, sendBuffer, len, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
len = sprintf_s(sendBuffer, "Unsupport Method Please Use GET Method");
lenLen = sprintf_s(sendLengthBuffer, "%x\r\n", len);
if (send(*ct, sendLengthBuffer, lenLen, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
if (send(*ct, sendBuffer, len, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
if (send(*ct, "\r\n", 2, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
len = sprintf_s(sendBuffer, "0\r\n\r\n");
*/