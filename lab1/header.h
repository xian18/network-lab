#pragma once
#include <WinSock2.h>
#include <string.h>
#include <Windows.h>
#include <stdlib.h>
#include <process.h>
#include <iostream>
#include <stdio.h>
#include <ws2tcpip.h>
#include <Shlwapi.h>
#include "utils.h"
extern wchar_t* HomeDir;
extern int HomeDirLenght;
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")