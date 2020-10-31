// StopWait.cpp : 定义控制台应用程序的入口点。
#define SR
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"

#ifdef GBN
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"
#endif

#ifdef SR
#include "SRRdtSender.h"
#include "SRRdtReceiver.h"
#endif

#ifdef TCP
#include "TCPRdtSender.h"
#include "TCPRdtReceiver.h"
#endif

int main(int argc, char* argv[])
{

	#ifdef GBN
		RdtSender *ps = new GBNRdtSender();
		RdtReceiver * pr = new GBNRdtReceiver();
	#endif
	#ifdef SR
		RdtSender *ps = new SRRdtSender();
		RdtReceiver * pr = new SRRdtReceiver();
	#endif
	#ifdef TCP
		RdtSender *ps = new TCPRdtSender();
		RdtReceiver * pr = new TCPRdtReceiver();
	#endif
	
//	pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("input.txt");
	pns->setOutputFile("output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

