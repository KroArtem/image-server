#include <string>
#include <map>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>

#endif
using std::string;
using std::map;


#ifdef _WIN32
#else
#define SOCKET int
#endif

struct http_request
{
	SOCKET   s_;

	string                        method_;
	string                        path_;
	map<string, string> params_;

	string                        accept_;
	string                        accept_language_;
	string                        accept_encoding_;
	string                        user_agent_;
    
	/* status_: used to transmit server's error status, such as
		o  202 OK
		o  404 Not Found 
		and so on */
	string                        status_;
    
	string                        answer_;
    
	// for POST method
	string content_;
	string content_length_;
	string content_disposition_;
};
