#include "SERVER.h"

#pragma warning(disable : 4996)

SERVER::SERVER():svrSoc(INVALID_SOCKET)
{
	int nRc;
	char buf[20];
	lastError = NO_ERROR;
	if (CONFIG::GetHomeDirLength() == 0) { CONFIG::InitConfig(); }
	svrAddr.sin_family = AF_INET;
	CONFIG::SetSocketIP(svrAddr);
	CONFIG::SetSocketPort(svrAddr);

	nRc = WSAStartup(0x0101, &wsaData);
	if (nRc)
	{
		lastError = WSAGetLastError();
		cout << "WSAStartup Error: " << WSAErrorCodeToChar(lastError) << endl;
		return;
	}
	if (wsaData.wVersion != 0x0101)
	{
		cout << "Could not find a usable version of Winsock.dll" << endl;
		lastError=SOCKET_ERROR;
		return;
	}
	cout << "WSAStartup succeed!" << endl;

	svrSoc= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (svrSoc == INVALID_SOCKET)
	{
		lastError = WSAGetLastError();
		cout << "Init Socket Error: " << WSAErrorCodeToChar(lastError) << endl;
		return ;
	}
	nRc = bind(svrSoc, (sockaddr*)&svrAddr, sizeof(svrAddr));
	if (nRc == SOCKET_ERROR)
	{
		lastError = WSAGetLastError();
		cout << "Socket Bind Error: " << WSAErrorCodeToChar(lastError) << endl;
		return ;
	}
	cout << "socket bind IP: "<< ConvertNetworkIPToHuman(svrAddr,buf) <<" port:" << ntohs(svrAddr.sin_port) << " succeed" << endl;

}

SERVER::~SERVER()
{
	WSACleanup();
}

int SERVER::GetLastError()
{
	return lastError;
}

int SERVER::StartServer()
{
	int continueFlag = 1;
	SocketWithAddr* clientSoc;
	sockaddr_in addr;
	int addrLen;
	int nRc;
	nRc = listen(svrSoc, SOMAXCONN);
	if (nRc == SOCKET_ERROR)
	{
		lastError = WSAGetLastError();
		cout << "Socket Listen Error: " << WSAErrorCodeToChar(lastError) << endl;
		return lastError;
	}
	cout << "socket start listen request" << endl;


	//cout << "please input something contain char q to stop this server." << endl;
	//_beginthread(ListenInputChar, 0, &continueFlag);

	while (continueFlag)
	{
		addrLen = sizeof(sockaddr_in);
		clientSoc = (SocketWithAddr*)malloc(sizeof(SocketWithAddr));
		if (clientSoc == NULL) {
			cout << "malloc new socket error: " << WSAErrorCodeToChar(WSA_NOT_ENOUGH_MEMORY) << endl;
			continue;
		}
		(*clientSoc).clientSoc = accept(svrSoc, (sockaddr*)&((*clientSoc).addr), &addrLen);
		if ((*clientSoc).clientSoc == INVALID_SOCKET)
		{
			lastError = WSAGetLastError();
			cout << "accept client socket request error: " << WSAErrorCodeToChar(lastError) << endl;
			free(clientSoc);
			continue;
		}
		cout << "Request From :";
		_beginthread(SERVER::HandleHttpRequest, 0, clientSoc);
	}
	return 0;

}

void SERVER::HandleHttpRequest(void* clientSoc)
{
	SocketWithAddr* clientSocket = (SocketWithAddr*)clientSoc;
	HTTP clientHttp(clientSocket);
	clientHttp.mainLoop();
	closesocket(clientSocket->clientSoc);
	free(clientSocket);
}


const char* SERVER::WSAErrorCodeToChar(int errorCode)
{
	const char* r;
	switch (errorCode)
	{
	case WSA_INVALID_HANDLE:
		r="Specified event object handle is invalid.";
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		r="Insufficient memory available.";
		break;
	case WSA_INVALID_PARAMETER:
		r="One or more parameters are invalid.";
		break;
	case WSA_OPERATION_ABORTED:
		r="Overlapped operation aborted.";
		break;
	case WSA_IO_INCOMPLETE:
		r="Overlapped I/O event object not in signaled state.";
		break;
	case WSA_IO_PENDING:
		r="Overlapped operations will complete later.";
		break;
	case WSAEINTR:
		r="Interrupted function call.";
		break;
	case WSAEBADF:
		r="File handle is not valid.";
		break;
	case WSAEACCES:
		r="Permission denied.";
		break;
	case WSAEFAULT:
		r="Bad address.";
		break;
	case WSAEINVAL:
		r="Invalid argument.";
		break;
	case WSAEMFILE:
		r="Too many open files.";
		break;
	case WSAEWOULDBLOCK:
		r="Resource temporarily unavailable.";
		break;
	case WSAEINPROGRESS:
		r="Operation now in progress.";
		break;
	case WSAEALREADY:
		r="Operation already in progress.";
		break;
	case WSAENOTSOCK:
		r="Socket operation on nonsocket.";
		break;
	case WSAEDESTADDRREQ:
		r="Destination address required.";
		break;
	case WSAEMSGSIZE:
		r="Message too long.";
		break;
	case WSAEPROTOTYPE:
		r="Protocol wrong type for socket.";
		break;
	case WSAENOPROTOOPT:
		r="Bad protocol option.";
		break;
	case WSAEPROTONOSUPPORT:
		r="Protocol not supported.";
		break;
	case WSAESOCKTNOSUPPORT:
		r="Socket type not supported.";
		break;
	case WSAEOPNOTSUPP:
		r="Operation not supported.";
		break;
	case WSAEPFNOSUPPORT:
		r="Protocol family not supported.";
		break;
	case WSAEAFNOSUPPORT:
		r="Address family not supported by protocol family.";
		break;
	case WSAEADDRINUSE:
		r="Address already in use.";
		break;
	case WSAEADDRNOTAVAIL:
		r="Cannot assign requested address.";
		break;
	case WSAENETDOWN:
		r="Network is down.";
		break;
	case WSAENETUNREACH:
		r="Network is unreachable.";
		break;
	case WSAENETRESET:
		r="Network dropped connection on reset.";
		break;
	case WSAECONNABORTED:
		r="Software caused connection abort.";
		break;
	case WSAECONNRESET:
		r="Connection reset by peer.";
		break;
	case WSAENOBUFS:
		r="No buffer space available.";
		break;
	case WSAEISCONN:
		r="Socket is already connected.";
		break;
	case WSAENOTCONN:
		r="Socket is not connected.";
		break;
	case WSAESHUTDOWN:
		r="Cannot send after socket shutdown.";
		break;
	case WSAETOOMANYREFS:
		r="Too many references.";
		break;
	case WSAETIMEDOUT:
		r="Connection timed out.";
		break;
	case WSAECONNREFUSED:
		r="Connection refused.";
		break;
	case WSAELOOP:
		r="Cannot translate name.";
		break;
	case WSAENAMETOOLONG:
		r="Name too long.";
		break;
	case WSAEHOSTDOWN:
		r="Host is down.";
		break;
	case WSAEHOSTUNREACH:
		r="No route to host.";
		break;
	case WSAENOTEMPTY:
		r="Directory not empty.";
		break;
	case WSAEPROCLIM:
		r="Too many processes.";
		break;
	case WSAEUSERS:
		r="User quota exceeded.";
		break;
	case WSAEDQUOT:
		r="Disk quota exceeded.";
		break;
	case WSAESTALE:
		r="Stale file handle reference.";
		break;
	case WSAEREMOTE:
		r="Item is remote.";
		break;
	case WSASYSNOTREADY:
		r="Network subsystem is unavailable.";
		break;
	case WSAVERNOTSUPPORTED:
		r="Winsock.dll version out of range.";
		break;
	case WSANOTINITIALISED:
		r="Successful WSAStartup not yet performed.";
		break;
	case WSAEDISCON:
		r="Graceful shutdown in progress.";
		break;
	case WSAENOMORE:
		r="No more results.";
		break;
	case WSAECANCELLED:
		r="Call has been canceled.";
		break;
	case WSAEINVALIDPROCTABLE:
		r="Procedure call table is invalid.";
		break;
	case WSAEINVALIDPROVIDER:
		r="Service provider is invalid.";
		break;
	case WSAEPROVIDERFAILEDINIT:
		r="Service provider failed to initialize.";
		break;
	case WSASYSCALLFAILURE:
		r="System call failure.";
		break;
	case WSASERVICE_NOT_FOUND:
		r="Service not found.";
		break;
	case WSATYPE_NOT_FOUND:
		r="Class type not found.";
		break;
	case WSA_E_NO_MORE:
		r="No more results.";
		break;
	case WSA_E_CANCELLED:
		r="Call was canceled.";
		break;
	case WSAEREFUSED:
		r="Database query was refused.";
		break;
	case WSAHOST_NOT_FOUND:
		r="Host not found.";
		break;
	case WSATRY_AGAIN:
		r="Nonauthoritative host not found.";
		break;
	case WSANO_RECOVERY:
		r="This is a nonrecoverable error.";
		break;
	case WSANO_DATA:
		r="Valid name, no data record of requested type.";
		break;
	case WSA_QOS_RECEIVERS:
		r="QoS receivers.";
		break;
	case WSA_QOS_SENDERS:
		r="QoS senders.";
		break;
	case WSA_QOS_NO_SENDERS:
		r="No QoS senders.";
		break;
	case WSA_QOS_NO_RECEIVERS:
		r="QoS no receivers.";
		break;
	case WSA_QOS_REQUEST_CONFIRMED:
		r="QoS request confirmed.";
		break;
	case WSA_QOS_ADMISSION_FAILURE:
		r="QoS admission error.";
		break;
	case WSA_QOS_POLICY_FAILURE:
		r="QoS policy failure.";
		break;
	case WSA_QOS_BAD_STYLE:
		r="QoS bad style.";
		break;
	case WSA_QOS_BAD_OBJECT:
		r="QoS bad object.";
		break;
	case WSA_QOS_TRAFFIC_CTRL_ERROR:
		r="QoS traffic control error.";
		break;
	case WSA_QOS_GENERIC_ERROR:
		r="QoS generic error.";
		break;
	case WSA_QOS_ESERVICETYPE:
		r="QoS service type error.";
		break;
	case WSA_QOS_EFLOWSPEC:
		r="QoS flowspec error.";
		break;
	case WSA_QOS_EPROVSPECBUF:
		r="Invalid QoS provider buffer.";
		break;
	case WSA_QOS_EFILTERSTYLE:
		r="Invalid QoS filter style.";
		break;
	case WSA_QOS_EFILTERTYPE:
		r="Invalid QoS filter type.";
		break;
	case WSA_QOS_EFILTERCOUNT:
		r="Incorrect QoS filter count.";
		break;
	case WSA_QOS_EOBJLENGTH:
		r="Invalid QoS object length.";
		break;
	case WSA_QOS_EFLOWCOUNT:
		r="Incorrect QoS flow count.";
		break;
	case WSA_QOS_EUNKOWNPSOBJ:
		r="Unrecognized QoS object.";
		break;
	case WSA_QOS_EPOLICYOBJ:
		r="Invalid QoS policy object.";
		break;
	case WSA_QOS_EFLOWDESC:
		r="Invalid QoS flow descriptor.";
		break;
	case WSA_QOS_EPSFLOWSPEC:
		r="Invalid QoS provider-specific flowspec.";
		break;
	case WSA_QOS_EPSFILTERSPEC:
		r="Invalid QoS provider-specific filterspec.";
		break;
	case WSA_QOS_ESDMODEOBJ:
		r="Invalid QoS shape discard mode object.";
		break;
	case WSA_QOS_ESHAPERATEOBJ:
		r="Invalid QoS shaping rate object.";
		break;
	case WSA_QOS_RESERVED_PETYPE:
		r="Reserved policy QoS element type.";
		break;
	default:
		r="Some unknown issue happened";
		break;
	}
	return r;
}

char* SERVER::ConvertNetworkIPToHuman(sockaddr_in& addr, char buffer[20])
{
	inet_ntop(AF_INET, &(addr.sin_addr), buffer, 20);
	return buffer;
}

void SERVER::PrintIPV4AddrInfo(sockaddr_in& addr)
{
	char ip[20];
	memset(ip, 0, 20);
	inet_ntop(AF_INET, &(addr.sin_addr), ip, 20);
	printf("IP Address: %s\tPort: %d\n", ip, ntohs(addr.sin_port));
}
