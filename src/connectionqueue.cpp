#include "connectionqueue.hpp"

ConnectionQueue::ConnectionQueue() {}

void ConnectionQueue::PushBack(Connection::Ptr& connection) {
    std::lock_guard<decltype(_m)> guard(_m);

    _queue.push_back(std::move(connection));
}

const Connection::Ptr ConnectionQueue::PopFront() {
    std::lock_guard<decltype(_m)> guard(_m);
    
    auto connection = std::move(_queue.front());
    _queue.pop_front();

    return connection;
}
