#pragma once

#include "httprequest.hpp""
#include "requestparser.hpp"
#include "socketstreambuffer.hpp"

template <class Connection>
class RequestHandler {
    enum class ProcessingStep {
        Parse, ProcessMiddlewares, ProcessRoute,
    };

public:
    enum class Result {
        WaitingForClientData, Finished,
    };

    RequestHandler(Connection& connection) : _buffer(connection) {}

    Result Process() {
    }

private:
    SocketStreamBuffer<Connection> _buffer;
    _request HTTPRequest;
}

