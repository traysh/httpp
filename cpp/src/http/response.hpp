#pragma once

#include <map>
#include <sstream>

#include "connection.hpp"
#include "http/headerkey.hpp"
#include "http/responsestatus.hpp"
#include "util_string.hpp"

namespace HTTP {
struct Response {
    public:
        using StatusType = HTTP::ResponseStatus::Type;
        using HeadersType = std::map<HTTP::HeaderKey, std::string>;

        enum class OperationMode { Sync, Async };

        HTTP::ResponseStatus Status;
        OperationMode Mode = OperationMode::Async;
        HeadersType Header;

        template <class ConnectionType>
        Response(ConnectionType& connection, StatusType code = StatusType::OK)
            : Status(code), _connection(connection) {}

        ~Response() {
            Flush();
        }

        template<class T>
        inline auto& operator <<(const T& data) {
            return Mode == OperationMode::Sync ? streamInsert(_connection,
                                                              data)
                                               : streamInsert(_buffer, data);
        }

        void Flush() {
            if (Mode == OperationMode::Async) {
                auto payload = _buffer.str();
                _connection << static_cast<std::string>(Status);
                _connection << generateAsyncHeaders(payload);
                _connection << payload;
                Clear();
            }
        }

        inline bool Clear() {
            if (Mode == OperationMode::Sync && _wroteHeader) {
                return false;
            }

            _buffer.clear();
            Status = StatusType::OK;
            return true;
        }

    private:
        Connection& _connection;
        std::stringstream _buffer;
        bool _wroteHeader = false;

        inline std::string generateAsyncHeaders(const std::string& payload) {
            using namespace Util::String;

            if (auto it = Header.find(HTTP::HeaderKey("Connection"));
                it == Header.end() || ToUpper(Trim(it->second)) != "CLOSE") {

                if (payload.size() > 0) {
                    Header.emplace(HTTP::HeaderKey("Content-Length"),
                                   std::to_string(payload.size()));
                }
            }

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
}
