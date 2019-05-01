#pragma once

class Connection {
public:
    Connection(int fd);

private:
    int _fd;
};
