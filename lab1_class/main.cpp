#include "header.h"
#include "SERVER.h"
#include "CONFIG.h"
int main()
{
	SERVER* svr;
	std::string input;
	if (CONFIG::InitConfig() != NO_ERROR) { return 0; }
	svr = new SERVER();
	if (svr->GetLastError() != NO_ERROR) { return 0; }
	while (true)
	{
		std::cout << "please input start to start server" << endl;
		cin >> input;
		if (input == "start") { break; }
	}
	std::thread listenThread(&SERVER::StartServer, svr);
	while (true)
	{
		std::cout << "please input quit to stop server" << endl;
		cin >> input;
		if (input == "stop") { break; }
	}
}