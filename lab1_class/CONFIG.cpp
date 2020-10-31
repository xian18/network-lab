#include "CONFIG.h"

wstring CONFIG::HomeDir = L"";
string CONFIG::IP = "";
u_short CONFIG::Port = 0;

int CONFIG::InitConfig()
{
	int nRc;
	sockaddr_in addr;
	string hd;
	wchar_t hdbuffer[2048];
	if (!SetConsoleOutputCP(936)) { printf("%x\n", GetLastError()); }
	do
	{
		std::cout << "输入主目录：" ;
		std::cin >> hd;
	} while (hd.size() < 2 || hd.size() > 2047);
	memset(hdbuffer, 0, 4096);
	MultiByteToWideChar(CP_ACP, 0, hd.c_str(), -1, hdbuffer, 2047);
	HomeDir = hdbuffer;
	if (CONFIG::HomeDir.back() == '\\' || CONFIG::HomeDir.back() == '/')
	{
		CONFIG::HomeDir.pop_back();
	}
	std::replace(HomeDir.begin(), HomeDir.end(), L'/', L'\\');
	wprintf(L"%s\n", HomeDir.c_str());
	ClearInStream(cin);

	cout << endl << "输入端口号：";
	CONFIG::Port = GetOntInt(cin, cout, 1025, 65535);
	while (true)
	{
		cout << "输入绑定地址：";
		cin >> CONFIG::IP;
		nRc=inet_pton(AF_INET,CONFIG::IP.c_str() , &addr.sin_addr);
		if (nRc == 1) { break; }
		if (nRc == 0) { cout << "输入的地址无效" << endl; }
		if (nRc == SOCKET_ERROR)
		{
			nRc = WSAGetLastError();
			cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
			return nRc;
		}
	}
	return NO_ERROR;
}

const wchar_t* CONFIG::GetHomeDir()
{
	return CONFIG::HomeDir.c_str();
}

u_long CONFIG::GetHomeDirLength()
{
	return CONFIG::HomeDir.length();
}

u_short CONFIG::GetHumanPortNumber()
{
	return CONFIG::Port;
}

u_short CONFIG::GetNetworkPortNumber()
{
	return htons(CONFIG::Port);
}

u_short CONFIG::SetSocketPort(sockaddr_in& addr)
{
	return addr.sin_port= htons(CONFIG::Port);
}

const char* CONFIG::GetHumanIP()
{
	return CONFIG::IP.c_str();
}

int CONFIG::SetSocketIP(sockaddr_in& addr)
{
	return inet_pton(AF_INET, CONFIG::IP.c_str(), &addr.sin_addr);
}
