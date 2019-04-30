#include "listensocket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

using namespace std;

ListenSocket::ListenSocket() : _fd(0)
{
	memset(&_addr, 0, sizeof(_addr));

    /*
	while(1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

		ticks = time(NULL);
		snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
		write(connfd, sendBuff, strlen(sendBuff));

		close(connfd);
		sleep(1);
    }
    */
}

ListenSocket::~ListenSocket()
{
    // TODO
}


bool ListenSocket::Listen(const string& address,
                          const unsigned short& port,
                          const int max_backlog)
{
    if (_ready) {
        // TODO exception
        return false;
    }

	_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct in_addr addr;
    if (inet_aton(address.c_str(), &addr) == 0) {
        // TODO exception
        return false;
    }

    struct sockaddr_in socket_addr;
	socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr = addr;
        
    
	socket_addr.sin_port = htons(port);

    if (::bind(_fd,
               (struct sockaddr*)&socket_addr,
               sizeof(socket_addr)) != 0) {
        _ready = false;       
        return false; // TODO
    }
    
	listen(_fd, max_backlog);

    _ready = true;
    
    return true;
}
