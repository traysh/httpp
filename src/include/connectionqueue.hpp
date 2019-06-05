#pragma once

#include <list>
#include <functional>
#include <mutex>

#include "connection.hpp"

class ConnectionQueue {
public:
    ConnectionQueue();

    void PushBack(Connection::Ptr& connection);
    const Connection::Ptr PopFront();
    bool Empty() { return _queue.empty(); }
    size_t Size() { return _queue.size(); }

private:
    using Queue = std::list<Connection::Ptr>;

    Queue _queue;
    std::mutex _m;
};
