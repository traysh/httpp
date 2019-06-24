#pragma once

#include <map>
#include <sstream>

struct HTTPResponseStatus {
    enum class Type {
        Continue = 100,
        SwitchingProtocols = 101,
        OK = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        SeeOther = 303,
        NotModified = 304,
        UseProxy = 305,
        TemporaryRedirect = 307,
        BadRequest = 400,
        Unauthorized = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeOut = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        RequestEntityTooLarge = 413,
        RequestURITooLarge = 414,
        UnsupportedMediaType = 415,
        RequestedRangeNotSatisfiable = 416,
        ExpectationFailed = 417,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        GatewayTimeOut = 504,
        HTTPVersionNotSupported = 505,
    };

    const char* HTTPVersion = "HTTP/1.1"; // FIXME read from request
    Type Code;
    const char* Reason;

    HTTPResponseStatus(Type code = Type::OK) : Reason("") {
        SetStatus(code);
    };

    operator std::string() const {
        std::stringstream stream;
        stream << HTTPVersion << " "
               << static_cast<int>(Code) << " "
               << Reason << "\r\n";
        return stream.str();
    }

    void SetStatus(Type code) {
        static std::map<Type, const char*> code_to_reason = {
            { Type::Continue                    , "Continue" },
            { Type::SwitchingProtocols          , "Switching Protocols" },
            { Type::OK                          , "OK" },
            { Type::Created                     , "Created" },
            { Type::Accepted                    , "Accepted" },
            { Type::NonAuthoritativeInformation , "Non-Authoritative Information" },
            { Type::NoContent                   , "No Content" },
            { Type::ResetContent                , "Reset Content" },
            { Type::PartialContent              , "Partial Content" },
            { Type::MultipleChoices             , "Multiple Choices" },
            { Type::MovedPermanently            , "Moved Permanently" },
            { Type::Found                       , "Found" },
            { Type::SeeOther                    , "See Other" },
            { Type::NotModified                 , "Not Modified" },
            { Type::UseProxy                    , "Use Proxy" },
            { Type::TemporaryRedirect           , "Temporary Redirect" },
            { Type::BadRequest                  , "Bad Request" },
            { Type::Unauthorized                , "Unauthorized" },
            { Type::PaymentRequired             , "Payment Required" },
            { Type::Forbidden                   , "Forbidden" },
            { Type::NotFound                    , "Not Found" },
            { Type::MethodNotAllowed            , "Method Not Allowed" },
            { Type::NotAcceptable               , "Not Acceptable" },
            { Type::ProxyAuthenticationRequired , "Proxy Authentication Required" },
            { Type::RequestTimeOut              , "Request Time-out" },
            { Type::Conflict                    , "Conflict" },
            { Type::Gone                        , "Gone" },
            { Type::LengthRequired              , "Length Required" },
            { Type::PreconditionFailed          , "Precondition Failed" },
            { Type::RequestEntityTooLarge       , "Request Entity Too Large" },
            { Type::RequestURITooLarge          , "Request-URI Too Large" },
            { Type::UnsupportedMediaType        , "Unsupported Media Type" },
            { Type::RequestedRangeNotSatisfiable, "Requested range not satisfiable" },
            { Type::ExpectationFailed           , "Expectation Failed" },
            { Type::InternalServerError         , "Internal Server Error" },
            { Type::NotImplemented              , "Not Implemented" },
            { Type::BadGateway                  , "Bad Gateway" },
            { Type::ServiceUnavailable          , "Service Unavailable" },
            { Type::GatewayTimeOut              , "Gateway Time-out" },
            { Type::HTTPVersionNotSupported     , "HTTP Version not supported" },
        };

        Code = code;

        if (auto it = code_to_reason.find(code);
            it != code_to_reason.end()) {
            const auto& [unused, reason] = *it;
            Reason = reason;
        }
    }

};

inline bool operator==(const std::string& other,
                       const HTTPResponseStatus& status) {
    return static_cast<std::string>(status) == other;
}

inline bool operator==(const HTTPResponseStatus& status,
                       const std::string& other) {
    return other == status;
}

inline std::stringstream operator<<(std::stringstream& stream,
                                     const HTTPResponseStatus& status) {
    stream << status.HTTPVersion << " "
           << static_cast<int>(status.Code) << " "
           << status.Reason << "\r\n";
    return std::move(stream);
}

