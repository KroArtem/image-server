#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32

#include <winsock2.h>
#include <process.h>

#else

#include <sys/socket.h>
#include <pthread.h>

#endif


#ifdef _WIN32
#define CloseSocket  closesocket
#else
#define CloseSocket close
#endif

/*
Project -> 
	... properties ->
	Configuration properties->
	Linker->
	Additional dependencies:
          ws2_32.lib
*/
#ifdef _WIN32

unsigned __stdcall ProcessRequest(void* ptr_s);

#endif

/*
unsigned __stdcall ProcessRequest1(void* ptr_s)
{
	printf("Hello\n");
	closesocket((int)ptr_s);
	return 0;
}
*/

int main()
{
	

#ifdef  _WIN32
	WSADATA info;
	if (WSAStartup(MAKEWORD(2,0), &info))
	{
		printf("Could not start WSA\n");
		exit(1);
	}
#endif
	
	int max_connections = 5;
	unsigned int port_to_listen = 8080;

	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));

	sa.sin_family = PF_INET;             
	sa.sin_port=htons(port_to_listen);

	SOCKET in_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(in_socket == INVALID_SOCKET) { exit(2); }

	if(bind(in_socket, (sockaddr *)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		return CloseSocket(in_socket);
	}

	listen(in_socket, max_connections);

	while (1)
	{
		// see WSAGetLastError()
		SOCKET new_socket = accept(in_socket, 0, 0);
		if (new_socket == INVALID_SOCKET) { break; }

		unsigned ret;
		_beginthreadex(0, 0, ProcessRequest, (void*)new_socket, 0, &ret);
	}
	CloseSocket(in_socket);
//	WSACleanup();

	return 0;
}
