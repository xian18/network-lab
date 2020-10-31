#include "HTTP.h"

#ifdef DELETE
#undef DELETE
#define needReDefine
#endif // DELETE

HTTP::HTTP(SocketWithAddr* clientSoc):clientSocket(&(clientSoc->clientSoc)), request(), response(),addr(clientSoc->addr)
{
	request.clientSocket = this->clientSocket;
	response.clientSocket = this->clientSocket;
	response.request = &request;
}

int HTTP::ParseRequest()
{
	int nRc;
	string key, value,info,header;
	request.clientSocket = this->clientSocket;
	nRc = request.GetReqMethodFromSocket(info);
	if (nRc != NO_ERROR) { return nRc; }
	header = info+" ";
	nRc = request.GetReqUrlFromSocket();
	if (nRc != NO_ERROR) { return nRc; }
	header += request.ReqPath+" ";
	nRc = request.GetVersionFromSocket(info);
	if (nRc != NO_ERROR) { return nRc; }
	header += info;
	cout << "HTTP Request From:";
	SERVER::PrintIPV4AddrInfo(addr);
	cout << header << endl;
	request.content_length = -1;
	request.connection = HTTP::CONNECTION::CLOSE;
	while (true)
	{
		nRc = request.GetHeadKeyFromSocket(key);
		if (nRc != NO_ERROR) { return nRc; }
		if (key.length() == 0) { break; }
		nRc = request.GetHeadValueFromSocket(value);
		if (nRc != NO_ERROR) { return nRc; }
		auto it = HTTP::Request::ParserHeaderMap.find(key);
		if (it != HTTP::Request::ParserHeaderMap.end())
		{
			nRc=((this->request).*(it->second))(value);
			if (nRc != NO_ERROR) { return nRc; }
		}
		if (key == "") { break; }
		cout << key << ": " << value << endl;
	}
	cout << endl;
	return NO_ERROR;
}

int HTTP::GenerateResponse()
{
	int nRc;
	response.filePath.clear();
	response.httpStatusCode = 0;
	response.filePoint = NULL;
	nRc = response.ConvertGetPathToFilePoint();
	if (nRc != NO_ERROR) { return nRc; }

	int len = request.content_length;
	char buf;
	if (len <= 0) { return NO_ERROR; }
	while (len)
	{
		len--;
		nRc = recv(*clientSocket, &buf, 1, 0);
		if (nRc == SOCKET_ERROR || !nRc)
		{
			nRc = nRc ? WSAGetLastError() : WSAENOTCONN;
			cout << SERVER::WSAErrorCodeToChar(nRc) << endl;
			return nRc;
		}
	}
	return NO_ERROR;
}

int HTTP::SendResponse()
{
	int nRc;
	string info, head;
	nRc = response.SendHTTPVersion(info);
	if (nRc != NO_ERROR) { return nRc; }
	head = info;
	nRc = response.SendHTTPCode(info);
	if (nRc != NO_ERROR) { return nRc; }
	head += info;
	cout << head;
	for (auto func : HTTP::Response::SendResponseHeaderVector)
	{
		nRc=((this->response).*func)(info);
		if (nRc != NO_ERROR) { return nRc; }
		cout << info;
	}

	nRc = response.SendResponseBuffer("\r\n", 2);
	if (nRc != NO_ERROR) { return nRc; }

	nRc = response.SendFileBody();
	if (response.filePoint != NULL)
	{
		fclose(response.filePoint);
		response.filePoint = NULL;
	}
	return nRc;
}

int HTTP::mainLoop()
{
	int nRc;
	while (true)
	{
		nRc = ParseRequest();
		if (nRc != NO_ERROR) { return nRc; }
		nRc = GenerateResponse();
		if (nRc != NO_ERROR) { return nRc; }
		nRc = SendResponse();
		if (nRc != NO_ERROR) { return nRc; }
	}
	return 0;
}

const string HTTP::GetMimeType(const wstring& file)
{
	size_t pos = file.rfind('.');
	wstring ext = file.substr(pos);
	auto result = HTTP::FileExtToMIMEType.find(ext);
	return result != HTTP::FileExtToMIMEType.end() ? result->second : "application/octet-stream";
}

#ifdef needReDefine
#ifndef DELETE
#define DELETE                           (0x00010000L)
#endif
#undef needReDefine
#endif

