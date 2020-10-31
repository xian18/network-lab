#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H
#include "RdtReceiver.h"
class TCPRdtReceiver :public RdtReceiver
{
private:
	unsigned int base;
    unsigned int windowLen;
	Packet lastAckPkt;				//上次发送的确认报文
    Packet* rcvdPktBuffer;
    bool* rcvdPkt;
    void printWindow();
    bool needReplyAck(int seqNum);
    bool needStageBuffer(int seqNum);
public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

public:
	
	void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};

#endif

