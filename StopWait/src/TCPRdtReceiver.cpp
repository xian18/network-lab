
#include "Global.h"
#include "TCPRdtReceiver.h"


TCPRdtReceiver::TCPRdtReceiver()
{
    base=0;
	windowLen=0x1<<(Configuration::BIT_NUM-1);
    rcvdPktBuffer=(Packet*)malloc(sizeof(Packet)*windowLen);
    rcvdPkt=(bool*)malloc(sizeof(bool)*windowLen);
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);

    for(int i=0;i<windowLen;i++){
        rcvdPkt[i]=false;
    }
}


TCPRdtReceiver::~TCPRdtReceiver()
{
    if(rcvdPktBuffer){free(rcvdPktBuffer);}
    rcvdPktBuffer=nullptr;
    if(rcvdPkt){free(rcvdPkt);}
    rcvdPkt=nullptr;
    base=0;
    windowLen=0;
}

void TCPRdtReceiver::receive(const Packet &packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);
    
	if(checkSum!=packet.checksum){
        pUtils->printPacket("接收方收到的校验出错",packet);
        printWindow();
        return ;
    }
	if(!needReplyAck(packet.seqnum)){
		pUtils->printPacket("接收方收到的窗口序号不是需要的",packet);
        printWindow();
		return ;
	}
	pUtils->printPacket("接收方正确收到发送方的报文", packet);
    printWindow();
    
    if(needStageBuffer(packet.seqnum)){
		memcpy(rcvdPktBuffer[packet.seqnum%windowLen].payload, packet.payload, sizeof(packet.payload));
		rcvdPkt[packet.seqnum%windowLen]=true;
		Message msg;
    	while(rcvdPkt[base%windowLen]){
    	    rcvdPkt[base%windowLen]=false;
    	    memcpy(msg.data, rcvdPktBuffer[base%windowLen].payload, sizeof(packet.payload));
		    pns->delivertoAppLayer(RECEIVER, msg);
    	    base=(base+1)%(0x1<<Configuration::BIT_NUM);
    	}
	}
    

	lastAckPkt.acknum = (base+(0x1<<Configuration::BIT_NUM)-1)%(0x1<<Configuration::BIT_NUM); 
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	pUtils->printPacket("接收方发送确认报文", lastAckPkt);
	pns->sendToNetworkLayer(SENDER, lastAckPkt);

    printWindow();
}

void TCPRdtReceiver::printWindow(){
    printf("接收窗口：[");
    for(unsigned int i=base,j=0;j<windowLen;i=(i+1)%(0x1<<Configuration::BIT_NUM),j++){
        printf("%d:%s,",i,rcvdPkt[i%windowLen]?"received":"not");
    }
    printf("]\n");
}

bool TCPRdtReceiver::needReplyAck(int seqNum){
    unsigned int b=base;
    unsigned int s=(b+windowLen)%(0x1<<Configuration::BIT_NUM);
    unsigned int a=seqNum;
    if(b>s?a>=b|a<s:a>=b&&a<s){return true;}
    s=b;
    b=(b+(0x1<<Configuration::BIT_NUM)-windowLen)%(0x1<<Configuration::BIT_NUM);
    return b>s?a>=b|a<s:a>=b&&a<s;
}

bool TCPRdtReceiver::needStageBuffer(int seqNum){
    unsigned int b=base;
    unsigned int s=(b+windowLen)%(0x1<<Configuration::BIT_NUM);
    unsigned int a=seqNum;
    return b>s?a>=b|a<s:a>=b&&a<s;
}