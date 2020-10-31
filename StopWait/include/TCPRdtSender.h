#ifndef TCP_RDT_SENDER_H
#define TCP_RDT_SENDER_H
#include "RdtSender.h"
class TCPRdtSender :public RdtSender
{

private:
    unsigned int base; //待确认的序号
    unsigned int nextseq; //下一个要发送的序号
    Packet* packet; //窗口数组
    unsigned int windowLen; //窗口长度

    unsigned int lastAck;
    unsigned int lastAckTimes;
    
    bool acknumInSeq(int ackNum);
    void printWindow();
public:

	bool getWaitingState();
	bool send(const Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();
};

#endif