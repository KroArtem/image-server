#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include "webserver.h"
#include "UrlHelper.h"
#include "base64.h"

#ifdef _WIN32

#include <process.h>
#define stdcall    __stdcall
#define CloseSocket  closesocket

#else

#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>	
#define stdcall    __attribute__((stdcall))
#define ioctlsocket ioctl
#define CloseSocket close

#endif

using namespace std;

string CompressSpaces(const string& s)
{
	string out = ""; 
	for(size_t j = 0 ; j < s.length() ; j++)
	{
		char cc = s[j];
		if(cc == ' ' || cc == '\n' || cc == '\r') continue;
		out += cc;
	}

	return out;
}

string ReceiveBytes(SOCKET sock)
{
	string ret;
	char buf[1024];

	for ( ; ; )
	{
		u_long arg = 0;
		if (ioctlsocket(sock, FIONREAD, &arg) != 0) break;

		if (arg == 0) break;
		if (arg > 1024) arg = 1024;
		int rv = recv (sock, buf, arg, 0);
		if (rv <= 0) break;
		string t;
		t.assign (buf, rv);
		ret += t;
	}
 
	return ret;
}

string ReceiveLine(SOCKET sock)
{
	string ret;

	while (1)
	{
		char r;

		switch(recv(sock, &r, 1, 0))
		{
			// not connected anymore;
			case 0:  return "";
			case -1: return (errno == EAGAIN) ? ret : "";
		}

		ret += r;
		if (r == '\n')  return ret;
	}
}

void SendLine(SOCKET sock, string s)
{
	s += '\n';
	send(sock,s.c_str(),s.length(),0);
}

void SendBytes(SOCKET sock, const string& s)
{
	send(sock,s.c_str(),s.length(),0);
}

void FormResponse(http_request* r);

#ifdef _WIN32
unsigned stdcall ProcessRequest(void* ptr_s)
#else
void *ProcessRequest(void *ptr_s)
#endif
{
	SOCKET s = (SOCKET)ptr_s;
  
	string line = ReceiveLine(s);
	if (line.empty()) { return 1; }

	http_request req;

	if (line.find("GET") == 0) 
	{
		req.method_="GET";
	}
	else if (line.find("POST") == 0) 
	{
		cout << "POST" << endl;
		req.method_="POST";
	}

	string path;
	map<string, string> params;

	// cut POST/GET from the beginning of the string
	size_t posStartPath = 
		line.find_first_not_of(
			" ",
			req.method_.length());

	SplitGetReq(
		line.substr(posStartPath),
		path, params);

	req.status_ = "202 OK";
	req.s_      = s;
	req.path_   = path;
	req.params_ = params;

	static const string accept          = "Accept: "             ;
	static const string accept_language = "Accept-Language: "    ;
	static const string accept_encoding = "Accept-Encoding: "    ;
	static const string user_agent      = "User-Agent: "         ;

	static const string content_length  = "Content-Length: ";

	static const string content_disposition = "Content-Disposition: ";

	string the_boundary;

	req.content_.clear();
	req.content_length_.clear();
	req.content_disposition_.clear();

	while(1)
	{
		line = ReceiveLine(s);

		cout <<
			"Parsing line: " << line << endl;

		if(req.method_ == "GET")
			if (line.empty())
			{ 
				cout << "Breaking in " << req.method_ << endl; 
				break;
			}

		unsigned int pos_cr_lf = line.find_first_of("\x0a\x0d");
		if(req.method_ == "GET")
			if (pos_cr_lf == 0) { cout << "Breaking2 in " << req.method_ << endl; break; }

		line = line.substr(0,pos_cr_lf);

		if(line.substr(0, 2) == "--") 
		{
			if(the_boundary.empty())
			{
				the_boundary = line;
				cout << "STORING Boundary: " << the_boundary << endl;
			} 
			else
			{
				cout << "Got terminator of content" << endl;
				break;
			}
		} 
		else if (line.substr(0, accept.size()) == accept) 
		{
			req.accept_ = line.substr(accept.size());
		}
		else if (line.substr(0, accept_language.size()) == accept_language) 
		{
			req.accept_language_ = line.substr(accept_language.size());
		}
		else if (line.substr(0, accept_encoding.size()) == accept_encoding) 
		{
			req.accept_encoding_ = line.substr(accept_encoding.size());
		}
		else if (line.substr(0, user_agent.size()) == user_agent) 
		{
			req.user_agent_ = line.substr(user_agent.size());
		}
		else if (line.substr(0, content_length.size()) == content_length) 
		{
			req.content_length_ = line.substr(content_length.size());
		}
		else if (line.substr(0, content_disposition.size()) == content_disposition) 
		{
			req.content_disposition_ = line.substr(content_disposition.size());
		}
		else if(!the_boundary.empty())
		{ 
			// request body for POST
			req.content_ += line + string("\r\n");
		}
	}

	// remove_if(req.content_.begin(), req.content_.end(), isspace);

	cout << "=======Data=========" << endl;
	cout << req.content_ << endl;
	cout << "==Compressed data===" << endl;

	req.content_ = CompressSpaces(req.content_);

	cout << req.content_ << endl;
	cout << "=======End data=====" << endl;

	FormResponse(&req);

	stringstream str_str;
	str_str << req.answer_.size();

	time_t ltime;
	time(&ltime);
	tm* gmt= gmtime(&ltime);

	static string const serverName = "KroArtem";

	char* asctime_remove_nl = asctime(gmt);
	asctime_remove_nl[24] = 0;

	SendBytes(s, "HTTP/1.1 ");

	SendLine(s, req.status_);

	SendLine(s, string("Date: ") + asctime_remove_nl + " GMT");
	SendLine(s, string("Server: ") +serverName);
	SendLine(s, "Connection: close");
	SendLine(s, "Content-Type: text/html; charset=ISO-8859-1");
	SendLine(s, "Content-Length: " + str_str.str());
	SendLine(s, "");
	SendLine(s, req.answer_);

	CloseSocket(s);
  
	return 0;
}
