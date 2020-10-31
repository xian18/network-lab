#pragma once
#include "header.h"

int InitServerSocket(SOCKET& serverSoc, WSADATA& wsaData, sockaddr_in& addr);

int DeliverRequest(SOCKET& serverSoc);

void HandleHttpRequset(void* ctSoc);
