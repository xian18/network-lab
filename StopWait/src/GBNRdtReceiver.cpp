#include "Global.h"
#include "GBNRdtReceiver.h"


GBNRdtReceiver::GBNRdtReceiver():expectSequenceNumberRcvd(0)
{
	windowLen=0x1<<(Configuration::BIT_NUM-1);
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}


GBNRdtReceiver::~GBNRdtReceiver()
{
}

void GBNRdtReceiver::receive(const Packet &packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);
	if(checkSum!=packet.checksum){
        pUtils->printPacket("接收方收到的校验出错",packet);
        return ;
    }
	if(expectSequenceNumberRcvd!=packet.seqnum){
		pUtils->printPacket("接收方收到的窗口序号不是需要的",packet);
		pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
		return ;
	}
	pUtils->printPacket("接收方正确收到发送方的报文", packet);

	Message msg;
	memcpy(msg.data, packet.payload, sizeof(packet.payload));
	pns->delivertoAppLayer(RECEIVER, msg);

	lastAckPkt.acknum = packet.seqnum; 
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	pUtils->printPacket("接收方发送确认报文", lastAckPkt);
	pns->sendToNetworkLayer(SENDER, lastAckPkt);
	expectSequenceNumberRcvd = (expectSequenceNumberRcvd+1)%(0x1<<Configuration::BIT_NUM);
	
	
}