#pragma once

#include <netinet/in.h>

class Socket
{
public:
    Socket();
    virtual ~Socket();

    bool Ready() { return _ready; }

private:
    struct sockaddr_in _addr;
	char _send_buff[1025];
    int _listen_fd;
    int _conn_fd;
    bool _ready = false;
};
