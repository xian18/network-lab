#pragma once
#include "header.h"
#include "SERVER.h"
#include "CONFIG.h"
#include "utils.h"
using namespace std;
#ifdef DELETE
#undef DELETE
#define needReDefine
#endif // DELETE
class SocketWithAddr;
class HTTP
{
public:
	enum class HTTP_PROTOCOL { one, one_one, two };
	enum class REQUEST_METHOD { GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE, PATCH ,NOT_IN_LIST};
	enum class  CONNECTION { KEEP_ALIVE, CLOSE };
	class Request {
	public:
		Request();
		string ReqPath;
		HTTP_PROTOCOL version;
		CONNECTION connection;
		REQUEST_METHOD method;
		SOCKET* clientSocket;
		int content_length;
		int GetReqMethodFromSocket(string& info);//from socket
		int GetReqUrlFromSocket();//from socket
		int GetVersionFromSocket(string& info);//from socket
		int GetConnectionFromString(std::string& value);//from socket
		int GetContentLengthFromString(std::string& value);
		int GetHeadKeyFromSocket(string& head);//from socket 
		int GetHeadValueFromSocket(string& value);//from socket
		static REQUEST_METHOD StringToRequestMethod(string& reqm);
		static HTTP_PROTOCOL StringToHTTPVersion(string& str);
		static std::map<string, int (HTTP::Request::*)(std::string&)> ParserHeaderMap;
	};
	class Response {
	public:
		Response();
		Request* request;
		SOCKET* clientSocket;
		int httpStatusCode;
		FILE* filePoint;
		wstring filePath;
		int SendHTTPVersion(string& info);
		int SendHTTPCode(string& info);
		int SendResponseTransferEncoding(string& info);
		int SendResponseConnection(string& info);
		int SendResponseContentType(string& info);
		//int SendResponseContentLength(string& info);
		int SendFileBody();//using transfer-encodeing: chunked
		int ConvertGetPathToFilePoint();
		int SendResponseBuffer(const char* buffer,const u_int sendLen);;
		static std::vector<int (HTTP::Response::*)(std::string&)> SendResponseHeaderVector;
	};
	SOCKET* clientSocket;
	sockaddr_in addr;
	Request request;
	Response response;
	HTTP(SocketWithAddr* clientSoc);
	int ParseRequest();
	int GenerateResponse();
	int SendResponse();
	int mainLoop();
	static const string GetMimeType(const wstring& file);
	const static std::map<std::wstring, std::string> FileExtToMIMEType;
};

#ifdef needReDefine
#ifndef DELETE
#define DELETE                           (0x00010000L)
#endif
#undef needReDefine
#endif

