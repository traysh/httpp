#pragma once

#include <map>
#include <sstream>

#include "connection.hpp"
#include "httpheaderkey.hpp"
#include "httpresponsestatus.hpp"

template<typename ConnectionType = Connection>
struct HTTPResponse {
    public:
        using StatusType = HTTPResponseStatus::Type;
        using HeadersType = std::map<HTTPHeaderKey, std::string>;

        enum class OperationMode { Sync, Async };

        HTTPResponseStatus Status;
        OperationMode Mode = OperationMode::Async;
        HeadersType Header;

        HTTPResponse(ConnectionType& connection,
                     StatusType code = StatusType::OK)
            : Status(code), _connection(connection) {}

        ~HTTPResponse() {
            Flush();
        }

        template<class T>
        inline auto& operator <<(const T& data) {
            return Mode == OperationMode::Sync ? streamInsert(_connection,
                                                              data)
                                               : streamInsert(_buffer, data);
        }

        inline void Flush() {
            if (Mode == OperationMode::Async) {
                auto payload = _buffer.str();
                _connection << static_cast<std::string>(Status);
                _connection << generateAsyncHeaders(payload);
                _connection << payload;
            }
        }

        inline bool Clear() {
            if (Mode == OperationMode::Sync && _wroteHeader) {
                return false;
            }

            _buffer.str("");
            Status = StatusType::OK;
            return true;
        }

    private:
        ConnectionType& _connection;
        std::stringstream _buffer;
        bool _wroteHeader = false;

        inline auto generateAsyncHeaders(const std::string& payload) {
            Header.emplace(HTTPHeaderKey("Content-Length"),
                             std::to_string(payload.size()));

            std::stringstream stream;
            for (const auto& [key, value] : Header) {
                stream << static_cast<std::string>(key) << ": "
                       << value << "\r\n";
            }
            
            stream << "\r\n";
            return stream.str();
        }

        template<class OutputStream, class T>
        inline auto& streamInsert(OutputStream& stream, const T& data) {
            if (!_wroteHeader && Mode == OperationMode::Sync) {
                stream << static_cast<std::string>(Status);
                //TODO set chunked encoding
                _wroteHeader = true;
            }

            stream << data;
            return *this;
        }
};
