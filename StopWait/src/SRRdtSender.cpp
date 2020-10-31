
#include "Global.h"
#include "SRRdtSender.h"


SRRdtSender::SRRdtSender():base(0),nextseq(0),packet(nullptr),windowLen(0),ackedNum(0){
    windowLen=0x1<<(Configuration::BIT_NUM-1);
    packet=(Packet*)malloc(sizeof(Packet)*windowLen);
    ackedNum=(bool*)malloc(sizeof(bool)*windowLen);
}


SRRdtSender::~SRRdtSender(){
    if(packet) {free(packet);}
    if(ackedNum) {free(ackedNum);}
    packet=nullptr;
    ackedNum=nullptr;
    windowLen=0;
    base=0;
    nextseq=0;
}



bool SRRdtSender::getWaitingState() {
	return ((nextseq+1)%windowLen)==(base%windowLen);
}




bool SRRdtSender::send(const Message &message) {
	if (getWaitingState()) { 
		return false;
	}
    ackedNum[nextseq%windowLen]=false;

    Packet& tempPac=packet[nextseq%windowLen];
    tempPac.acknum=-1;
    tempPac.seqnum=nextseq;
    tempPac.checksum=0;
    memcpy(tempPac.payload,message.data,sizeof(message.data));
    tempPac.checksum=pUtils->calculateCheckSum(tempPac);
    pUtils->printPacket("发送方发送报文",tempPac);
    printWindow();

    pns->startTimer(SENDER,Configuration::TIME_OUT,nextseq);//global timer id 0

	pns->sendToNetworkLayer(RECEIVER, tempPac);

    nextseq=(nextseq+1)%(0x1<<Configuration::BIT_NUM);
    printWindow();
	return true;
}

void SRRdtSender::receive(const Packet &ackPkt) {
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
    ackedNum[ackNum%windowLen]=true;
    pns->stopTimer(SENDER,ackNum);
    
    while(ackedNum[base%windowLen]&&base!=nextseq){
        base=(base+1)%(0x1<<Configuration::BIT_NUM);
    }
    printWindow();
}

void SRRdtSender::timeoutHandler(int seqNum) {
    printf("发送方定时器时间到，重发窗口中的报文\n");
    printWindow();
    for(unsigned int i=base;i!=nextseq;i=(i+1)%(0x1<<Configuration::BIT_NUM)){
        if(ackedNum[i]){continue;}
        pUtils->printPacket("发送方发送报文",packet[i%windowLen]);
	    pns->stopTimer(SENDER,i);								
	    pns->startTimer(SENDER, Configuration::TIME_OUT,i);
	    pns->sendToNetworkLayer(RECEIVER,packet[i%windowLen]);
    }
}

bool SRRdtSender::acknumInSeq(int ackNum){
    unsigned int b=base%windowLen;
    unsigned int s=nextseq%windowLen;
    unsigned int a=ackNum%windowLen;
    return b>s?a>=b|a<s:a>=b&&a<s;
}

void SRRdtSender::printWindow(){
    printf("发送窗口：[");
    for(unsigned int i=base;i!=nextseq;i=(i+1)%(0x1<<Configuration::BIT_NUM)){
        printf("%d:%s,",i,ackedNum[i%windowLen]?"acked":"not");
    }
    printf("]\n");
}