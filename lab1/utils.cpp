#include "utils.h"
void ListenInputChar(void* need)
{
	int* ti = (int*) need;
	char c='q';
	while (getchar() != c);
	*ti = 0;
	_endthread();
	return;
}

void PrintIPV4AddrInfo(sockaddr_in* addr)
{
	char ip[20];
	memset(ip, 0, 20);
	inet_ntop(AF_INET, &(addr->sin_addr), ip, 20);
	printf("IP Address: %s\tPort: %d\n", ip, ntohs(addr->sin_port));
}

int InitServerConfigFromSTDIN(sockaddr_in& addr)
{
	int len, cf, cp;
	char c1, c2;
	char dirBuf[2048];
	cf = 1;
	cp = 0;
	do {
		std::cout << "Please Input Server Home Dir:";
		memset(dirBuf, 0, 2048);
		for (len = 0; len < 2048; len++)
		{
			c1 = fgetc(stdin);
			if (c1 == '\n'&&len>3) { cf = 0; cp = 1; break; }
			dirBuf[len] = c1;
		}
		if (!cp) {
			std::cout << "input too long or short" << std::endl;
		}
	} while (cf);
	if (dirBuf[len - 1] == '/' || dirBuf[len - 1] == '\\') { dirBuf[len - 1] = '\0'; len--; }
	MultiByteToWideChar(CP_ACP, 0, dirBuf, -1, HomeDir,2048);
	HomeDirLenght = lstrlenW(HomeDir);
	std::cout << "Please Input Port Number(1024-65535):";
	addr.sin_port = htons(GetOntInt(std::cin, std::cout, 1025, 65535));
	return NO_ERROR;
}

void OutPutErrorMessage(int errorCode)
{
	printf("The Error Code is %ld:", errorCode);
	switch (errorCode)
	{
	case WSA_INVALID_HANDLE:
		printf("Specified event object handle is invalid.\n");
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		printf("Insufficient memory available.\n");
		break;
	case WSA_INVALID_PARAMETER:
		printf("One or more parameters are invalid.\n");
		break;
	case WSA_OPERATION_ABORTED:
		printf("Overlapped operation aborted.\n");
		break;
	case WSA_IO_INCOMPLETE:
		printf("Overlapped I/O event object not in signaled state.\n");
		break;
	case WSA_IO_PENDING:
		printf("Overlapped operations will complete later.\n");
		break;
	case WSAEINTR:
		printf("Interrupted function call.\n");
		break;
	case WSAEBADF:
		printf("File handle is not valid.\n");
		break;
	case WSAEACCES:
		printf("Permission denied.\n");
		break;
	case WSAEFAULT:
		printf("Bad address.\n");
		break;
	case WSAEINVAL:
		printf("Invalid argument.\n");
		break;
	case WSAEMFILE:
		printf("Too many open files.\n");
		break;
	case WSAEWOULDBLOCK:
		printf("Resource temporarily unavailable.\n");
		break;
	case WSAEINPROGRESS:
		printf("Operation now in progress.\n");
		break;
	case WSAEALREADY:
		printf("Operation already in progress.\n");
		break;
	case WSAENOTSOCK:
		printf("Socket operation on nonsocket.\n");
		break;
	case WSAEDESTADDRREQ:
		printf("Destination address required.\n");
		break;
	case WSAEMSGSIZE:
		printf("Message too long.\n");
		break;
	case WSAEPROTOTYPE:
		printf("Protocol wrong type for socket.\n");
		break;
	case WSAENOPROTOOPT:
		printf("Bad protocol option.\n");
		break;
	case WSAEPROTONOSUPPORT:
		printf("Protocol not supported.\n");
		break;
	case WSAESOCKTNOSUPPORT:
		printf("Socket type not supported.\n");
		break;
	case WSAEOPNOTSUPP:
		printf("Operation not supported.\n");
		break;
	case WSAEPFNOSUPPORT:
		printf("Protocol family not supported.\n");
		break;
	case WSAEAFNOSUPPORT:
		printf("Address family not supported by protocol family.\n");
		break;
	case WSAEADDRINUSE:
		printf("Address already in use.\n");
		break;
	case WSAEADDRNOTAVAIL:
		printf("Cannot assign requested address.\n");
		break;
	case WSAENETDOWN:
		printf("Network is down.\n");
		break;
	case WSAENETUNREACH:
		printf("Network is unreachable.\n");
		break;
	case WSAENETRESET:
		printf("Network dropped connection on reset.\n");
		break;
	case WSAECONNABORTED:
		printf("Software caused connection abort.\n");
		break;
	case WSAECONNRESET:
		printf("Connection reset by peer.\n");
		break;
	case WSAENOBUFS:
		printf("No buffer space available.\n");
		break;
	case WSAEISCONN:
		printf("Socket is already connected.\n");
		break;
	case WSAENOTCONN:
		printf("Socket is not connected.\n");
		break;
	case WSAESHUTDOWN:
		printf("Cannot send after socket shutdown.\n");
		break;
	case WSAETOOMANYREFS:
		printf("Too many references.\n");
		break;
	case WSAETIMEDOUT:
		printf("Connection timed out.\n");
		break;
	case WSAECONNREFUSED:
		printf("Connection refused.\n");
		break;
	case WSAELOOP:
		printf("Cannot translate name.\n");
		break;
	case WSAENAMETOOLONG:
		printf("Name too long.\n");
		break;
	case WSAEHOSTDOWN:
		printf("Host is down.\n");
		break;
	case WSAEHOSTUNREACH:
		printf("No route to host.\n");
		break;
	case WSAENOTEMPTY:
		printf("Directory not empty.\n");
		break;
	case WSAEPROCLIM:
		printf("Too many processes.\n");
		break;
	case WSAEUSERS:
		printf("User quota exceeded.\n");
		break;
	case WSAEDQUOT:
		printf("Disk quota exceeded.\n");
		break;
	case WSAESTALE:
		printf("Stale file handle reference.\n");
		break;
	case WSAEREMOTE:
		printf("Item is remote.\n");
		break;
	case WSASYSNOTREADY:
		printf("Network subsystem is unavailable.\n");
		break;
	case WSAVERNOTSUPPORTED:
		printf("Winsock.dll version out of range.\n");
		break;
	case WSANOTINITIALISED:
		printf("Successful WSAStartup not yet performed.\n");
		break;
	case WSAEDISCON:
		printf("Graceful shutdown in progress.\n");
		break;
	case WSAENOMORE:
		printf("No more results.\n");
		break;
	case WSAECANCELLED:
		printf("Call has been canceled.\n");
		break;
	case WSAEINVALIDPROCTABLE:
		printf("Procedure call table is invalid.\n");
		break;
	case WSAEINVALIDPROVIDER:
		printf("Service provider is invalid.\n");
		break;
	case WSAEPROVIDERFAILEDINIT:
		printf("Service provider failed to initialize.\n");
		break;
	case WSASYSCALLFAILURE:
		printf("System call failure.\n");
		break;
	case WSASERVICE_NOT_FOUND:
		printf("Service not found.\n");
		break;
	case WSATYPE_NOT_FOUND:
		printf("Class type not found.\n");
		break;
	case WSA_E_NO_MORE:
		printf("No more results.\n");
		break;
	case WSA_E_CANCELLED:
		printf("Call was canceled.\n");
		break;
	case WSAEREFUSED:
		printf("Database query was refused.\n");
		break;
	case WSAHOST_NOT_FOUND:
		printf("Host not found.\n");
		break;
	case WSATRY_AGAIN:
		printf("Nonauthoritative host not found.\n");
		break;
	case WSANO_RECOVERY:
		printf("This is a nonrecoverable error.\n");
		break;
	case WSANO_DATA:
		printf("Valid name, no data record of requested type.\n");
		break;
	case WSA_QOS_RECEIVERS:
		printf("QoS receivers.\n");
		break;
	case WSA_QOS_SENDERS:
		printf("QoS senders.\n");
		break;
	case WSA_QOS_NO_SENDERS:
		printf("No QoS senders.\n");
		break;
	case WSA_QOS_NO_RECEIVERS:
		printf("QoS no receivers.\n");
		break;
	case WSA_QOS_REQUEST_CONFIRMED:
		printf("QoS request confirmed.\n");
		break;
	case WSA_QOS_ADMISSION_FAILURE:
		printf("QoS admission error.\n");
		break;
	case WSA_QOS_POLICY_FAILURE:
		printf("QoS policy failure.\n");
		break;
	case WSA_QOS_BAD_STYLE:
		printf("QoS bad style.\n");
		break;
	case WSA_QOS_BAD_OBJECT:
		printf("QoS bad object.\n");
		break;
	case WSA_QOS_TRAFFIC_CTRL_ERROR:
		printf("QoS traffic control error.\n");
		break;
	case WSA_QOS_GENERIC_ERROR:
		printf("QoS generic error.\n");
		break;
	case WSA_QOS_ESERVICETYPE:
		printf("QoS service type error.\n");
		break;
	case WSA_QOS_EFLOWSPEC:
		printf("QoS flowspec error.\n");
		break;
	case WSA_QOS_EPROVSPECBUF:
		printf("Invalid QoS provider buffer.\n");
		break;
	case WSA_QOS_EFILTERSTYLE:
		printf("Invalid QoS filter style.\n");
		break;
	case WSA_QOS_EFILTERTYPE:
		printf("Invalid QoS filter type.\n");
		break;
	case WSA_QOS_EFILTERCOUNT:
		printf("Incorrect QoS filter count.\n");
		break;
	case WSA_QOS_EOBJLENGTH:
		printf("Invalid QoS object length.\n");
		break;
	case WSA_QOS_EFLOWCOUNT:
		printf("Incorrect QoS flow count.\n");
		break;
	case WSA_QOS_EUNKOWNPSOBJ:
		printf("Unrecognized QoS object.\n");
		break;
	case WSA_QOS_EPOLICYOBJ:
		printf("Invalid QoS policy object.\n");
		break;
	case WSA_QOS_EFLOWDESC:
		printf("Invalid QoS flow descriptor.\n");
		break;
	case WSA_QOS_EPSFLOWSPEC:
		printf("Invalid QoS provider-specific flowspec.\n");
		break;
	case WSA_QOS_EPSFILTERSPEC:
		printf("Invalid QoS provider-specific filterspec.\n");
		break;
	case WSA_QOS_ESDMODEOBJ:
		printf("Invalid QoS shape discard mode object.\n");
		break;
	case WSA_QOS_ESHAPERATEOBJ:
		printf("Invalid QoS shaping rate object.\n");
		break;
	case WSA_QOS_RESERVED_PETYPE:
		printf("Reserved policy QoS element type.\n");
		break;
	default:
		printf("Some unknown issue happened\n");
		break;
	}
}

#ifdef max
#undef max
#endif // max

int GetOntInt(std::istream& inStream, std::ostream& outStream, int numMin, int numMax)
{
	char skiped, getChar;
	int result;
	bool inputedFlag, negativeFlag, syntaxErrorFlag;

	while (true)
	{
		inputedFlag = false;
		negativeFlag = false;
		syntaxErrorFlag = false;
		result = 0;

		skiped = inStream.get();
		if (skiped == '-')
			negativeFlag = true;
		else
			inStream.unget();
		while (true && !syntaxErrorFlag)//get int part
		{
			getChar = inStream.get();
			if (getChar >= '0' && getChar <= '9')//Input is number
			{
				inputedFlag = true;
				result = result * 10 + getChar - '0';
			}
			else//Input is other char
			{
				if (getChar != '\n')
					syntaxErrorFlag = true;
				else
					inStream.unget();
				break;
			}
		}

		if (syntaxErrorFlag)
			outStream << "Input SyntaxError! Please Try it Again!" << std::endl;
		else
		{
			if (inputedFlag)
			{
				result = negativeFlag ? -result : result;
				if (result < numMin)
				{
					outStream << "Input Int Too Small!Please Try It Again!" << std::endl;
				}
				else
				{
					if (result > numMax)
					{
						outStream << "Input Int Too Large!Please Try It Again!" << std::endl;
					}
					else
					{
						inStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						return result;
					}
				}
			}
			else
			{
				outStream << "Nothing Number Input!Please Try It Again!" << std::endl;
			}
		}
		inStream.clear();
		inStream.sync();
		inStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

int SendHttpNotOK(int httpcode,SOCKET* ct)
{
	int len;
	int nRc;
	const char* result;
	char sendBuffer[50];
	switch (httpcode)
	{
	case 400:
		result = "400 Bad Request";
		break;
	case 404:
		result = "404 Not Found";
		break;
	case 414:
		result = "414 URI Too Long";
		break;
	case 405:
		result = "405 Method Not Allowed";
		break;
	default:
		result = "404 Not Found";
		break;
	}
	std::cout << result << std::endl;
	len = sprintf_s(sendBuffer, "HTTP/1.1 %s\r\n\r\n",result);
	if (send(*ct, sendBuffer, len, 0) == SOCKET_ERROR) 
	{
		nRc = WSAGetLastError();
		OutPutErrorMessage(nRc);
		return nRc;
	}
	if (closesocket(*ct) == SOCKET_ERROR) 
	{
		nRc = WSAGetLastError();
		OutPutErrorMessage(nRc);
		return nRc;
	}
	free(ct);
	return NO_ERROR;
}

int SendHttpOK(FILE* fp, SOCKET* ct)
{
	int len;
	int lenLen;
	char lenBuffer[64];
	char sendBuffer[256];
	int nRc;
	len = sprintf_s(sendBuffer, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	if (send(*ct, sendBuffer, len, 0) == SOCKET_ERROR) 
	{
		nRc = WSAGetLastError();
		OutPutErrorMessage(nRc);
		return nRc;
	}
	fseek(fp, 0, SEEK_SET);
	while (len)
	{
		len = fread(sendBuffer, 1, 256, fp);
		lenLen = sprintf_s(lenBuffer, "%x\r\n", len);
		if (send(*ct, lenBuffer, lenLen, 0) == SOCKET_ERROR) 
		{
			nRc = WSAGetLastError();
			OutPutErrorMessage(nRc);
			return nRc;
		}
		if (send(*ct, sendBuffer, len, 0) == SOCKET_ERROR)
		{
			nRc = WSAGetLastError();
			OutPutErrorMessage(nRc);
			return nRc;
		}
		if (send(*ct, "\r\n", 2, 0) == SOCKET_ERROR)
		{
			nRc = WSAGetLastError();
			OutPutErrorMessage(nRc);
			return nRc;
		}
	}
	if (closesocket(*ct) == SOCKET_ERROR) 
	{
		nRc = WSAGetLastError();
		OutPutErrorMessage(nRc);
		return nRc;
	}
	std::cout << "200 OK" << std::endl;
	return NO_ERROR;

}
/*len = sprintf_s(sendBuffer, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
if (send(*ct, sendBuffer, len, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
len = sprintf_s(sendBuffer, "Unsupport Method Please Use GET Method");
lenLen = sprintf_s(sendLengthBuffer, "%x\r\n", len);
if (send(*ct, sendLengthBuffer, lenLen, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
if (send(*ct, sendBuffer, len, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
if (send(*ct, "\r\n", 2, 0) == SOCKET_ERROR) { OutPutErrorMessage(WSAGetLastError()); }
len = sprintf_s(sendBuffer, "0\r\n\r\n");
*/