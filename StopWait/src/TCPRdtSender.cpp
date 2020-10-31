
#include "Global.h"
#include "TCPRdtSender.h"


TCPRdtSender::TCPRdtSender():base(0),nextseq(0),packet(nullptr),windowLen(0){
    windowLen=0x1<<(Configuration::BIT_NUM-1);
    packet=(Packet*)malloc(sizeof(Packet)*windowLen);
    lastAck=-1;
    lastAckTimes=-1;
}


TCPRdtSender::~TCPRdtSender(){
    if(packet) {free(packet);}
    packet=nullptr;
    windowLen=0;
    base=0;
    nextseq=0;
}



bool TCPRdtSender::getWaitingState() {
	return ((nextseq+1)%windowLen)==(base%windowLen);
}




bool TCPRdtSender::send(const Message &message) {
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

void TCPRdtSender::receive(const Packet &ackPkt) {
    int checkSum=pUtils->calculateCheckSum(ackPkt);

    if(checkSum!=ackPkt.checksum){
        pUtils->printPacket("发送方收到的确认校验出错",ackPkt);
        printWindow();
        return ;
    }
    if(lastAck!=ackPkt.acknum){
        lastAckTimes=0;
        lastAck=ackPkt.acknum;
    }else{
        lastAckTimes++;
    }
    if(!acknumInSeq(ackPkt.acknum)){
        pUtils->printPacket("发送方收到的确认不在接受范围内",ackPkt);
        if(lastAckTimes==3){
            pns->stopTimer(SENDER,0);
            pns->startTimer(SENDER,Configuration::TIME_OUT,0);
	        pns->sendToNetworkLayer(RECEIVER,packet[base%windowLen]);			//重新发送数据包
            lastAckTimes=0;
            pUtils->printPacket("发送方进行快速重传",packet[base%windowLen]);
        }
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

void TCPRdtSender::timeoutHandler(int seqNum) {
    printf("发送方定时器时间到，重发窗口中的报文\n");
    printWindow();
    int f=false;
	pns->stopTimer(SENDER,0);								

    for(unsigned int i=base;i!=nextseq;i=(i+1)%(0x1<<Configuration::BIT_NUM)){
	    pns->sendToNetworkLayer(RECEIVER,packet[i%windowLen]);			//重新发送数据包
        f=true;
    }
	if(f){pns->startTimer(SENDER, Configuration::TIME_OUT,0);}

}

bool TCPRdtSender::acknumInSeq(int ackNum){
    unsigned int b=base%windowLen;
    unsigned int s=nextseq%windowLen;
    unsigned int a=ackNum%windowLen;
    return b>s?a>=b|a<s:a>=b&&a<s;
}

void TCPRdtSender::printWindow(){
    printf("发送窗口：[");
    for(unsigned int i=base;i!=nextseq;i=(i+1)%(0x1<<Configuration::BIT_NUM)){
        printf("%d,",i);
    }
    printf("]\n");
}