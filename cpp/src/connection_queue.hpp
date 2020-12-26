#pragma once

#include <list>
#include <functional>
#include <memory>
#include <mutex>

namespace Connection {
class Connection;
}

class ConnectionQueue {
    using ConnectionPtr = std::unique_ptr<Connection::Connection>;

public:
    ConnectionQueue();

    void PushBack(ConnectionPtr& connection);
    const ConnectionPtr PopFront();
    bool Empty() { return _queue.empty(); }
    size_t Size() { return _queue.size(); }

private:
    using Queue = std::list<ConnectionPtr>;

    Queue _queue;
    std::mutex _m;
};
