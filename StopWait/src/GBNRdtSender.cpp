
#include "Global.h"
#include "GBNRdtSender.h"


GBNRdtSender::GBNRdtSender():base(0),nextseq(0),packet(nullptr),windowLen(0){
    windowLen=0x1<<(Configuration::BIT_NUM-1);
    packet=(Packet*)malloc(sizeof(Packet)*windowLen);
}


GBNRdtSender::~GBNRdtSender(){
    if(packet) {free(packet);}
    packet=nullptr;
    windowLen=0;
    base=0;
    nextseq=0;
}



bool GBNRdtSender::getWaitingState() {
	return ((nextseq+1)%windowLen)==(base%windowLen);
}




bool GBNRdtSender::send(const Message &message) {
	if (getWaitingState()) { 
		return false;
	}

    Packet& tempPac=packet[nextseq%windowLen];

    tempPac.acknum=-1;
    tempPac.seqnum=nextseq;
    tempPac.checksum=0;
    memcpy(tempPac.payload,message.data,sizeof(message.data));
    tempPac.checksum=pUtils->calculateCheckSum(tempPac);

    pUtils->printPacket("发送方发送报文",tempPac);
    printWindow();

    if(base==nextseq) {pns->startTimer(SENDER,Configuration::TIME_OUT,0);}//global timer id 0

	pns->sendToNetworkLayer(RECEIVER, tempPac);

    nextseq=(nextseq+1)%(0x1<<Configuration::BIT_NUM);

    printWindow();
	return true;
}

void GBNRdtSender::receive(const Packet &ackPkt) {
    int checkSum=pUtils->calculateCheckSum(ackPkt);
    if(checkSum!=ackPkt.checksum){
        pUtils->printPacket("发送方收到的确认校验出错",ackPkt);
        printWindow();
        return ;
    }
    if(!acknumInSeq(ackPkt.acknum)){
        pUtils->printPacket("发送方收到的确认不在接受范围内",ackPkt);
        printWindow();
        return ;
    }
    pUtils->printPacket("发送方正确收到确认", ackPkt);
    printWindow();

    unsigned int ackNum=ackPkt.acknum;
    while(base!=ackNum){base=(base+1)%(0x1<<Configuration::BIT_NUM);}
    base=(base+1)%(0x1<<Configuration::BIT_NUM);
    pns->stopTimer(SENDER,0);
    if(base!=nextseq){
        pns->startTimer(SENDER,Configuration::TIME_OUT,0);
    }
    printWindow();
}

void GBNRdtSender::timeoutHandler(int seqNum) {
    printf("发送方定时器时间到，重发窗口中的报文\n");
    printWindow();

	pns->stopTimer(SENDER,0);								
	pns->startTimer(SENDER, Configuration::TIME_OUT,0);

    for(unsigned int i=base;i!=nextseq;i=(i+1)%(0x1<<Configuration::BIT_NUM)){
	    pns->sendToNetworkLayer(RECEIVER,packet[i%windowLen]);			//重新发送数据包
    }

}

bool GBNRdtSender::acknumInSeq(int ackNum){
    unsigned int b=base%windowLen;
    unsigned int s=nextseq%windowLen;
    unsigned int a=ackNum%windowLen;
    return b>s?a>=b|a<s:a>=b&&a<s;
}

void GBNRdtSender::printWindow(){
    printf("发送窗口：[");
    for(unsigned int i=base;i!=nextseq;i=(i+1)%(0x1<<Configuration::BIT_NUM)){
        printf("%d,",i);
    }
    printf("]\n");
}