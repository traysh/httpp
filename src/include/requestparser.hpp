#pragma once

#include <sstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "httprequest.hpp"
#include "connection.hpp"
#include "string_util.hpp"

struct RequestParser {
    template<class T = Connection::Ptr>
    static HTTPRequest parse(const T& connection) {
        auto rawData = connection->ReadData();
        std::stringstream dataStream(std::string(rawData.data(),
                                                 rawData.size()));
        rawData.clear();

        HTTPRequest request;

        parseRequestLine(dataStream, request);
        parseHeaders(dataStream, request);
        parseBody(dataStream, request);

        return request;
    }

private:
    static inline void parseRequestLine(std::stringstream& stream, HTTPRequest& request) {
        std::string line;

        getCleanLine(stream, line);
        std::stringstream lineStream(line);

        std::string method, path, protocol, protocolVersion;
        lineStream >> method >> path >> protocol;
        const auto protocolData = Util::String::Split(protocol, '/');
        protocol = protocolData.first;
        protocolVersion = protocolData.second;

		request.Method = mapMethod(Util::String::ToUpper(method));
        request.Path = path;
		request.Protocol = mapProtocol(Util::String::ToUpper(protocol));
        request.ProtocolVersion = protocolVersion;
    }

    static inline void parseHeaders(std::stringstream& stream,
                                    HTTPRequest& request) {
        std::stringstream lineStream;
        std::string line;

        for (getCleanLine(stream, line); 
                line.size() != 0 && stream.good();
                getCleanLine(stream, line)) {

            using namespace Util::String;
            auto header = Split(line, ':');
            auto key = Trim(ToUpper(header.first));
            auto value = Trim(header.second);
            request.Header.insert_or_assign(key, value);
        }
    }

    static inline void parseBody(std::stringstream& stream,
                                 HTTPRequest& request) {
        // FIXME check content-length
        stream << 0;
        request.Body = &stream.str()[stream.tellg()];
    }

    static HTTPRequest::MethodType mapMethod(const std::string& str) {
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

    static HTTPRequest::ProtocolType mapProtocol(const std::string& str) {
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

    static void getCleanLine(std::stringstream& ss, std::string& line) {
        std::getline(ss, line);
        line = Util::String::RemoveAll(line, '\r');
    }
};
