#include "http/requestparser.hpp"

namespace HTTP {
RequestParser::Result RequestParser::streamGood() {
    if (_stream.bad()) {
        return Result::Failed;
    }
    if (_stream.fail()) {
        _buffer.Reset();
        _stream.clear();
        return Result::IncompleteInputData;
    }

    return Result::Success;
}

const std::list<typename RequestParser::_ParseStep>
    RequestParser::_parseSequence = {
        &RequestParser::parseRequestLine,
        &RequestParser::parseHeaders,
        &RequestParser::parseBody
    };

typename RequestParser::Result RequestParser::Parse(HTTP::Request& request) {
    for (; _currentStep != _parseSequence.end(); ++_currentStep) {
        auto f = *_currentStep;
        auto result = (this->*f)(request);
        if (result != Result::Success) {
            return result;
        }
    }

    return Result::Success;
}

typename RequestParser::Result RequestParser::parseRequestLine(
        HTTP::Request& request) {
    std::string method, path, protocol, protocolVersion, rest;
    std::array<std::string*, 3> values { &method, &path, &protocol };

    auto initial_pos = _stream.tellg();

    std::istream::sentry sentry(_stream);
    (void)sentry;
    Util::StreamProcessor processor(_stream);
    for (auto value : values) {
        auto result = processor.ExtractWord(*value);
        if (result != Result::Success) {
            if (result == Result::IncompleteInputData) {
                result = Result::NoInputData;
            }
            
            _stream.seekg(initial_pos);
            return result;
        }

        // the request line must end with a newline
        if (value != values.back()) {
            if (processor.NewLine()) {
                return Result::Failed;
            }
        }
        else {
            if (!processor.NewLine() && _buffer.in_avail() == 0) {
                return Result::NoInputData;
            }
        }
    }

    auto result = streamGood();
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

typename RequestParser::Result RequestParser::parseHeaders(HTTP::Request& request) {
    std::istream::sentry sentry(_stream);
    (void)sentry;
    Util::StreamProcessor processor(_stream);
    std::string rest;

    while (_stream.good() && !processor.NewLine()) {
        std::string key, value;

        auto initial_pos = _stream.tellg();

        auto result = processor.ExtractWord(key, false, true, {':'});
        if (result != Result::Success) {
            if (result == Result::IncompleteInputData) {
                result = Result::NoInputData;
            }
            
            _stream.seekg(initial_pos);
            return result;
        }

        if (!_stream.good()) {
            return Result::Failed;
        }

        result = processor.ExtractWord(value, false, false, {'\r', '\n'});
        if (result != Result::Success) {
            if (result == Result::IncompleteInputData) {
                result = Result::NoInputData;
            }
 
            _stream.seekg(initial_pos);
            return result;
        }

        request.Header.insert_or_assign(key, value);
    }

    getline(_stream, rest);
    return Result::Success;
}

typename RequestParser::Result RequestParser::parseBody(HTTP::Request& request) {
    // TODO handle "Transfer-Encoding" as supposed:
    // https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
    const char content_length_key[] = "CONTENT-LENGTH";
    if (request.Header.find(content_length_key) == request.Header.end()) {
        return Result::Success;
    }

    // FIXME move this from here
    const size_t max_content_length = 1048576;
    size_t content_length = std::stoul(request.Header.at("CONTENT-LENGTH"));
    if (content_length == 0) {
        return Result::Success;
    }

    if (content_length > max_content_length) {
        return Result::Failed;
    }

    size_t available = static_cast<size_t>(_buffer.in_avail());
    if (request.Body.Empty() && _buffer.Continguous(content_length + 1)) {
        if (available != content_length) {
            return Result::NoInputData;
        }

        char* body = _buffer.GetInPtr();
        body[content_length] = 0;
        request.Body = RequestPayload(body, content_length);
        _buffer.pubseekoff(content_length + 1, std::ios_base::cur, std::ios_base::in);
    }
    else {
        if (request.Body.Empty()) {
            request.Body = RequestPayload(new char[content_length + 1], 0);
        }

        auto* data = request.Body.Buffer();
        _stream.read(&data[request.Body.Size()], available);
        auto read_size = _stream.gcount();
        if (read_size == 0) {
            return Result::NoInputData;
        }
        if (_stream.bad()) {
            return Result::Failed;
        }

        request.Body.SetSize(request.Body.Size() + static_cast<size_t>(read_size));
        data[content_length] = 0;

        if (request.Body.Size() < content_length) {
            return Result::IncompleteInputData;
        }
    }

    return Result::Success;
}

HTTP::MethodType RequestParser::mapMethod(const std::string& str) {
    const static std::map<std::string, HTTP::MethodType> m {
        { "GET", HTTP::MethodType::Get },
        { "HEAD", HTTP::MethodType::Head },
        { "POST", HTTP::MethodType::Post },
        { "PUT", HTTP::MethodType::Put },
        { "DELETE", HTTP::MethodType::Delete },
        { "CONNECT", HTTP::MethodType::Connect },
        { "OPTIONS", HTTP::MethodType::Options },
        { "TRACE", HTTP::MethodType::Trace },
        { "PATCH", HTTP::MethodType::Patch },
    };

    HTTP::MethodType type;
    try {
        type = m.at(str);
    }
    catch (const std::out_of_range& e) {
        return HTTP::MethodType::Unknown;
    }

    return type;
}

Server::ProtocolType RequestParser::mapProtocol(const std::string& str) {
    const static std::map<std::string, Server::ProtocolType> m {
        { "HTTP", Server::ProtocolType::HTTP },
    };

    Server::ProtocolType type;
    try {
        type = m.at(str);
    }
    catch (const std::out_of_range& e) {
        return Server::ProtocolType::Unknown;
    }

    return type;
}
}

