#include "header.h"
#include "HTTP.h"
#include "SERVER.h"
#include "CONFIG.h"

#ifdef DELETE
#undef DELETE
#define needReDefine
#endif // DELETE

HTTP::Request::Request():ReqPath(""),version(HTTP::HTTP_PROTOCOL::one_one),
connection(HTTP::CONNECTION::CLOSE),method(HTTP::REQUEST_METHOD::GET),content_length(0),
clientSocket(nullptr){}

int HTTP::Request::GetReqMethodFromSocket(string& info)
{
	char buf;
	int nRc;
	info = "";
	while (true)
	{
		nRc = recv(*clientSocket, &buf, 1, 0);
		if (nRc == SOCKET_ERROR || !nRc)
		{
			nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
			cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
			return nRc;
		}
		if (buf == ' ') { break; }
		info += buf;
	}
	this->method = StringToRequestMethod(info);
	return NO_ERROR;
}

int HTTP::Request::GetReqUrlFromSocket()
{
	char buf;
	int nRc;
	this->ReqPath = "";
	while (true)
	{
		nRc = recv(*clientSocket, &buf, 1, 0);
		if (nRc == SOCKET_ERROR || !nRc)
		{
			nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
			cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
			return nRc;
		}
		if (buf == ' ') { break; }
		this->ReqPath += buf;
	}
	return NO_ERROR;
}

int HTTP::Request::GetVersionFromSocket(string& info)
{
	char buf;
	int nRc;
	info = "";
	while (true)
	{
		nRc = recv(*clientSocket, &buf, 1, 0);
		if (nRc == SOCKET_ERROR || !nRc)
		{
			nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
			cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
			return nRc;
		}
		if (buf == '\r' || buf == '\n') { break; }
		info += buf;
	}
	this->version = this->StringToHTTPVersion(info);

	nRc = recv(*clientSocket, &buf, 1, 0);
	if (nRc == SOCKET_ERROR || !nRc)
	{
		nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
		cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
		return nRc;
	}//skip \r or \n

	return NO_ERROR;
}

int HTTP::Request::GetConnectionFromString(std::string& value)
{
	if (value == "keep-alive")
	{
		this->connection = HTTP::CONNECTION::KEEP_ALIVE;
	}
	else
	{
		this->connection = HTTP::CONNECTION::CLOSE;
	}
	return NO_ERROR;
}

int HTTP::Request::GetContentLengthFromString(std::string& value)
{
	int nRc=NO_ERROR;
	try
	{
		this->content_length = std::stoi(value);
	}
	catch (std::invalid_argument const& e)
	{
		nRc = WSA_INVALID_PARAMETER;
		cout << SERVER::WSAErrorCodeToChar(nRc) << "in Content-Length" << endl;
	}
	catch (std::out_of_range const& e)
	{
		nRc = WSA_INVALID_PARAMETER;
		cout << SERVER::WSAErrorCodeToChar(nRc) << "in Content-Length" << endl;
	}
	return nRc;
}

int HTTP::Request::GetHeadKeyFromSocket(string& head)
{
	char buf;
	int nRc;
	head = "";
	while (true)
	{
		nRc = recv(*clientSocket, &buf, 1, 0);
		if (nRc == SOCKET_ERROR || !nRc)
		{
			nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
			cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
			return nRc;
		}
		if (buf == ':' || buf == '\r' || buf == '\n') { break; }
		head += buf;
	}
	nRc = recv(*clientSocket, &buf, 1, 0);
	if (nRc == SOCKET_ERROR || !nRc)
	{
		nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
		cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
		return nRc;
	}//skip \r or \n or \x20
	return NO_ERROR;
}

int HTTP::Request::GetHeadValueFromSocket(string& value)
{
	char buf;
	int nRc;
	value = "";
	while (true)
	{
		nRc = recv(*clientSocket, &buf, 1, 0);
		if (nRc == SOCKET_ERROR || !nRc)
		{
			nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
			cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
			return nRc;
		}
		if (buf == '\r' || buf == '\n') { break; }
		value += buf;
	}
	nRc = recv(*clientSocket, &buf, 1, 0);
	if (nRc == SOCKET_ERROR || !nRc)
	{
		nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
		cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
		return nRc;
	}//skip \r or \n
	return NO_ERROR;
}

HTTP::REQUEST_METHOD HTTP::Request::StringToRequestMethod(string& reqm)
{
	const static std::map<string, REQUEST_METHOD> StrToReqMap{
		{"CONNECT",HTTP::REQUEST_METHOD::CONNECT},
		{"DELETE",HTTP::REQUEST_METHOD::DELETE},
		{"GET",HTTP::REQUEST_METHOD::GET},
		{"HEAD",HTTP::REQUEST_METHOD::HEAD},
		{"OPTIONS",HTTP::REQUEST_METHOD::OPTIONS},
		{"PATCH",HTTP::REQUEST_METHOD::PATCH},
		{"POST",HTTP::REQUEST_METHOD::POST},
		{"PUT",HTTP::REQUEST_METHOD::PUT},
		{"TRACE",HTTP::REQUEST_METHOD::TRACE},
	};
	auto result = StrToReqMap.find(reqm);
	return result != StrToReqMap.end() ? result->second : HTTP::REQUEST_METHOD::NOT_IN_LIST;
}

HTTP::HTTP_PROTOCOL HTTP::Request::StringToHTTPVersion(string& str)
{
	const static std::map<string, HTTP::HTTP_PROTOCOL> StrToVer{
		{"HTTP/1.1",HTTP::HTTP_PROTOCOL::one_one},
		{"HTTP/2",HTTP::HTTP_PROTOCOL::two },
		{"HTTP/1.0",HTTP::HTTP_PROTOCOL::one},
		{"HTTP/1.1",HTTP::HTTP_PROTOCOL::one},
	};
	auto result = StrToVer.find(str);
	return result != StrToVer.end() ? result->second : HTTP::HTTP_PROTOCOL::one_one;
	return HTTP_PROTOCOL();
}

std::map<string, int (HTTP::Request::*)(std::string&)> HTTP::Request::ParserHeaderMap
{
	{"Connection",&HTTP::Request::GetConnectionFromString},
	{"Content-Length",&HTTP::Request::GetContentLengthFromString}
};

#ifdef needReDefine
#ifndef DELETE
#define DELETE                           (0x00010000L)
#endif
#undef needReDefine
#endif