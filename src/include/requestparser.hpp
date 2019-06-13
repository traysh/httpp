#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "httpexceptions.hpp"
#include "httprequest.hpp"
#include "connection.hpp"
#include "socketstreambuffer.hpp"
#include "streamprocessor.hpp"
#include "util_string.hpp"

template<class T = Connection>
class RequestParser {
    using _ParseStep = StreamProcessor::Result(RequestParser::*)(std::iostream&, HTTPRequest&);
    using _Iterator = typename std::list<_ParseStep>::const_iterator;

public:
    using Result = StreamProcessor::Result;

    RequestParser(SocketStreamBuffer<T>& buffer, _Iterator currentStep = _parseSequence.begin()) 
        : _buffer(buffer), _currentStep(currentStep) {}

    Result Parse(HTTPRequest& request);

private:
    SocketStreamBuffer<T> _buffer;
    const static std::list<_ParseStep> _parseSequence;
    _Iterator _currentStep;

    Result parseRequestLine(std::iostream& stream, HTTPRequest& request);
    Result parseHeaders(std::iostream& stream, HTTPRequest& request);
    Result parseBody(std::iostream& stream, HTTPRequest& request);

    inline Result streamGood(std::iostream& stream);

    HTTPRequest::ProtocolType mapProtocol(const std::string& str);
    HTTPRequest::MethodType mapMethod(const std::string& str);
};

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::streamGood(std::iostream& stream) {
    if (stream.bad()) {
        return Result::Failed;
    }
    if (stream.fail()) {
        auto* buf = static_cast<SocketStreamBuffer<T>*>(stream.rdbuf());
        buf->Reset();
        stream.clear();
        return Result::IncompleteInputData;
    }

    return Result::Success;
}

template<typename T>
const std::list<typename RequestParser<T>::_ParseStep>
    RequestParser<T>::_parseSequence = {
        &RequestParser<T>::parseRequestLine,
        &RequestParser<T>::parseHeaders,
        &RequestParser<T>::parseBody
    };

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::Parse(HTTPRequest& request) {
    std::iostream dataStream(&_buffer);

    for (; _currentStep != _parseSequence.end(); ++_currentStep) {
        auto f = *_currentStep;
        auto result = (this->*f)(dataStream, request);
        if (result != Result::Success) {
            return result;
        }
    }

    return Result::Success;
}

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::parseRequestLine(std::iostream& stream, HTTPRequest& request) {
    std::string method, path, protocol, protocolVersion, rest;
    std::array<std::string*, 3> values { &method, &path, &protocol };

    std::istream::sentry sentry(stream);
    StreamProcessor processor(stream);
    for (auto value : values) {
        auto result = processor.ExtractWord(*value);
        if (result != Result::Success) {
            return result;
        }

        // the request line must end with a newline
        if (value != values.back() && processor.NewLine()) {
            return Result::Failed;
        }
    }
    getline(stream, rest);
    auto result = streamGood(stream);
    if (result != Result::Success) {
        return result;
    }

    const auto protocolData = Util::String::Split(protocol, '/');
    protocol = protocolData.first;
    protocolVersion = protocolData.second;

    try {
        request.Method = mapMethod(Util::String::ToUpper(method));
        request.Path = path;
        request.Protocol = mapProtocol(Util::String::ToUpper(protocol));
        request.ProtocolVersion = protocolVersion;
    }
    catch (const std::out_of_range&) {
        return Result::Failed;
    }

    return Result::Success;
}

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::parseHeaders(std::iostream& stream, HTTPRequest& request) {
    std::istream::sentry sentry(stream);
    StreamProcessor processor(stream);
    while (stream.good() && !processor.NewLine()) {
        std::string key, value;
        std::string rest;

        auto initial_pos = stream.tellg();

        auto separators = processor.DefaultSeparators({':'});
        auto result = processor.ExtractWord(key, false, true,  separators);
        if (result != Result::Success) {
            stream.seekg(initial_pos);
            return result;
        }

        if (stream.get() != ':' || !stream.good()) {
            return Result::Failed;
        }

        result = processor.ExtractWord(value);
        if (result != Result::Success) {
            stream.seekg(initial_pos);
            return result;
        }

        if (!processor.NewLine()) {
            return Result::Failed;
        }

        getline(stream, rest);

        request.Header.insert_or_assign(key, value);
    }

    return Result::Success;
}

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::parseBody(std::iostream& stream,
                             HTTPRequest& request) {
    // FIXME check content-length
    stream << '\0';

    std::stringbuf rest;
    stream >> &rest;
    request.Body = rest.str();

    return Result::Success;
}

template<typename T>
HTTPRequest::MethodType RequestParser<T>::mapMethod(const std::string& str) {
    const static std::map<std::string, HTTPRequest::MethodType> m {
        { "GET", HTTPRequest::MethodType::Get },
        { "HEAD", HTTPRequest::MethodType::Head },
        { "POST", HTTPRequest::MethodType::Post },
        { "PUT", HTTPRequest::MethodType::Put },
        { "DELETE", HTTPRequest::MethodType::Delete },
        { "CONNECT", HTTPRequest::MethodType::Connect },
        { "OPTIONS", HTTPRequest::MethodType::Options },
        { "TRACE", HTTPRequest::MethodType::Trace },
        { "PATCH", HTTPRequest::MethodType::Patch },
    };

    HTTPRequest::MethodType type;
    try {
        type = m.at(str);
    }
    catch (const std::out_of_range& e) {
        return HTTPRequest::MethodType::Unknown;
    }

    return type;
}

template<typename T>
HTTPRequest::ProtocolType RequestParser<T>::mapProtocol(const std::string& str) {
    const static std::map<std::string, HTTPRequest::ProtocolType> m {
        { "HTTP", HTTPRequest::ProtocolType::HTTP },
    };

    HTTPRequest::ProtocolType type;
    try {
        type = m.at(str);
    }
    catch (const std::out_of_range& e) {
        return HTTPRequest::ProtocolType::Unknown;
    }

    return type;
}

