#include "connection_queue.hpp"
#include "connection/connection.hpp"

ConnectionQueue::ConnectionQueue() {}

void ConnectionQueue::PushBack(ConnectionQueue::ConnectionPtr& connection) {
    std::lock_guard<decltype(_m)> guard(_m);

    _queue.push_back(std::move(connection));
}

const ConnectionQueue::ConnectionPtr ConnectionQueue::PopFront() {
    std::lock_guard<decltype(_m)> guard(_m);
    
    auto connection = std::move(_queue.front());
    _queue.pop_front();

    return connection;
}
