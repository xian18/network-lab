#include "header.h"
#include "HTTP.h"

#pragma warning(disable : 4996)
#ifdef DELETE
#undef DELETE
#define needReDefine
#endif // DELETE

HTTP::Response::Response():request(NULL),clientSocket(NULL),httpStatusCode(0),
filePoint(NULL),filePath(L""){}

int HTTP::Response::SendHTTPVersion(string& info)
{
	int nRc=NO_ERROR;
	const static std::map<HTTP::HTTP_PROTOCOL, string>VerToStrMap
	{
		{HTTP::HTTP_PROTOCOL::one,"HTTP/1 "},
		{HTTP::HTTP_PROTOCOL::one_one,"HTTP/1.1 "},
		{HTTP::HTTP_PROTOCOL::two,"HTTP/2 "}
	};
	auto it = VerToStrMap.find(request->version);
	if (it != VerToStrMap.end())
	{
		info = it->second;
		nRc = SendResponseBuffer((it->second).c_str(), (it->second).length());
	}
	else
	{
		nRc = WSA_INVALID_PARAMETER;
		cout << SERVER::WSAErrorCodeToChar(nRc) <<"in send version" << endl;
	}
	return nRc;
}

int HTTP::Response::SendHTTPCode(string& info)
{

	const static std::map<int, string>CodeToStrMap
	{
		{101,"101 Switching Protocols\r\n"},
		{102,"102 Processing\r\n"},
		{103,"103 Early Hints\r\n"},
		{200,"200 OK\r\n"},
		{201,"201 Created\r\n"},
		{202,"202 Accepted\r\n"},
		{203,"203 Non-Authoritative Information\r\n"},
		{204,"204 No Content\r\n"},
		{205,"205 Reset Content\r\n"},
		{206,"206 Partial Content\r\n"},
		{207,"207 Multi-Status\r\n"},
		{208,"208 Already Reported\r\n"},
		{226,"226 IM Used\r\n"},
		{300,"300 Multiple Choices\r\n"},
		{301,"301 Moves Permanently\r\n"},
		{302,"302 Found\r\n"},
		{303,"303 See Other\r\n"},
		{304,"304 Not Modified\r\n"},
		{305,"305 Use Proxy\r\n"},
		//{306,"306 Switch Proxy\r\n"},
		{307,"307 Temporary Redirect\r\n"},
		{308,"308 Permanent Redirect\r\n"},
		{400,"400 Bad Request\r\n"},
		{401,"401 Unauthorized\r\n"},
		{402,"402 Payment Required\r\n"},
		{403,"403 Forbidden\r\n"},
		{404,"404 Not Found\r\n"},
		{405,"405 Method Not Allowed\r\n"},
		{406,"406 Not Acceptable\r\n"},
		{407,"407 Proxy Authentication Required\r\n"},
		{408,"408 Request Timeout\r\n"},
		{409,"409 Confict\r\n"},
		{410,"410 Gone\r\n"},
		{411,"411 Length Required\r\n"},
		{412,"412 Precondition Failed\r\n"},
		{413,"413 Payload Too Large\r\n"},
		{414,"414 URI Too Long\r\n"},
		{415,"415 Unsupported Media Type\r\n"},
		{416,"416 Range Not Satisfiable\r\n"},
		{417,"417 Excepection Failed\r\n"},
		{418,"418 I'm a teepot\r\n"},
		{421,"421 Misdirected Request\r\n"},
		{422,"422 Unprocessable Entity\r\n"},
		{423,"423 Locked\r\n"},
		{424,"424 Failed Dependency\r\n"},
		{425,"425 Too Early\r\n"},
		{426,"426 Upgrade Required\r\n"},
		{428,"428 Precondition Required\r\n"},
		{429,"429 Too Many Requests\r\n"},
		{431,"431 Request Header Fields Too Large\r\n"},
		{451,"451 Unavailable For Legal Reason\r\n"},
		{500,"500 Internal Server Error\r\n"},
		{501,"501 Not Implemented\r\n"},
		{502,"502 Bad Gateway\r\n"},
		{503,"503 Service Unavailable\r\n"},
		{504,"504 Gateway Timeout\r\n"},
		{505,"505 HTTP Version Not Supported\r\n"},
		{506,"506 Variant Also Negotiates\r\n"},
		{507,"507 Insufficient Storage\r\n"},
		{508,"508 Loop Detected\r\n"},
		{510,"510 Not Extended\r\n"},
		{511,"511 Network Authentication Required\r\n"}
	};
	auto it = CodeToStrMap.find(httpStatusCode);
	if (it == CodeToStrMap.end()) 
	{
		info = "400 Bad Request\r\n";
	}
	else
	{
		info = it->second;
	}
	return SendResponseBuffer(info.c_str(), info.size());
}

int HTTP::Response::SendResponseTransferEncoding(string& info)
{
	//if (httpStatusCode != 200) { return NO_ERROR; }
	info = "Transfer-Encoding: chunked\r\n";
	return SendResponseBuffer(info.c_str(), info.length());
}

int HTTP::Response::SendResponseConnection(string& info)
{
	info = "Connection: ";
	info += request->connection == HTTP::CONNECTION::KEEP_ALIVE ? "keep-alive\r\n" : "close";
	return SendResponseBuffer(info.c_str(), info.length());
}

int HTTP::Response::SendResponseContentType(string& info)
{
	info = "";
	if (httpStatusCode != 200) { return NO_ERROR; }
	info = "Content-Type: ";
	info += HTTP::GetMimeType(filePath);
	info += "\r\n";
	return SendResponseBuffer(info.c_str(),info.size());
}

int HTTP::Response::SendFileBody()
{
	int len=1;
	int lenLen = 0;
	char* lenBuffer;
	char* sendBuffer;
	int nRc;
	if (filePoint == NULL) 
	{
		return  SendResponseBuffer("0\r\n\r\n", 5);
	}
	sendBuffer = (char*)malloc(256);
	if (sendBuffer == NULL) { return WSA_NOT_ENOUGH_MEMORY; }
	lenBuffer = (char*)malloc(8);
	if (lenBuffer == NULL)
	{
		free(sendBuffer);
		return WSA_NOT_ENOUGH_MEMORY;
	}
	fseek(filePoint, 0, SEEK_SET);
	while(len)
	{
		len = fread(sendBuffer, 1, 256, filePoint);
		lenLen = sprintf_s(lenBuffer, 8,"%x\r\n", len);
		nRc = SendResponseBuffer(lenBuffer, lenLen);
		if(nRc!=NO_ERROR)
		{
			free(sendBuffer);
			free(lenBuffer);
			return nRc;
		}
		nRc = SendResponseBuffer(sendBuffer, len);
		if (nRc != NO_ERROR)
		{
			free(sendBuffer);
			free(lenBuffer);
			return nRc;
		}
		nRc = SendResponseBuffer("\r\n", 2);
		if (nRc != NO_ERROR)
		{
			free(sendBuffer);
			free(lenBuffer);
			return nRc;
		}
	}
	return NO_ERROR;
}

int HTTP::Response::ConvertGetPathToFilePoint()
{
	wchar_t* getUrl;
	DWORD urlLen;
	int nRc;
	if (request->method != HTTP::REQUEST_METHOD::GET)
	{
		httpStatusCode = 405;
		return NO_ERROR;
	}
	size_t pos1 = request->ReqPath.find('#');
	size_t pos2 = request->ReqPath.find('?');
	pos1 = pos1 == pos2 ? request->ReqPath.size() : (min(pos1, pos2));
	if (pos1 > 2048)
	{
		httpStatusCode = 414;
		return NO_ERROR;
	}
	urlLen = pos1+1;
	getUrl = (WCHAR*)malloc(urlLen * sizeof(wchar_t));
	if (getUrl == NULL)
	{
		nRc = WSA_NOT_ENOUGH_MEMORY;
		cout << SERVER::WSAErrorCodeToChar(nRc);
		return nRc;
	}
	memset(getUrl, 0, urlLen * sizeof(wchar_t));
	nRc = MultiByteToWideChar(CP_UTF8, 0, request->ReqPath.c_str(), pos1, getUrl, urlLen);
	if (!nRc) {
		nRc = GetLastError();
		free(getUrl);
		return nRc;
	}
	getUrl[pos1] = '\0';
	if (UrlUnescapeW(getUrl, NULL, &urlLen, URL_UNESCAPE_AS_UTF8 | URL_UNESCAPE_INPLACE) != S_OK)
	{
		httpStatusCode = 400;
		free(getUrl);
		return NO_ERROR;
	}

	filePath = CONFIG::HomeDir + getUrl;
	std::replace(filePath.begin(), filePath.end(), L'/', L'\\');
	filePoint = _wfopen(filePath.c_str(), L"rb");
	httpStatusCode = filePoint == NULL ? 404 : 200;
	free(getUrl);
	return NO_ERROR;
}

int HTTP::Response::SendResponseBuffer(const char* buffer,const u_int sendLen)
{
	int nRc=NO_ERROR;
	nRc = send(*clientSocket, buffer, sendLen, 0);
	if (nRc == SOCKET_ERROR)
	{
		nRc = WSAGetLastError();
		cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
		return nRc;
	}
	return NO_ERROR;
}

std::vector<int (HTTP::Response::*)(std::string&)> HTTP::Response::SendResponseHeaderVector
{
	&HTTP::Response::SendResponseTransferEncoding,
	&HTTP::Response::SendResponseConnection,
	&HTTP::Response::SendResponseContentType,
	//&HTTP::Response::SendResponseContentLength
};

#ifdef needReDefine
#ifndef DELETE
#define DELETE                           (0x00010000L)
#endif
#undef needReDefine
#endif