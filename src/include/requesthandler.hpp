#pragma once

#include <array>
#include <chrono>
#include <functional>
#include <thread>

#include "httprequest.hpp"
#include "requestparser.hpp"
#include "router.hpp"
#include "socketstreambuffer.hpp"

template <class Connection>
class RequestHandler {
public:
    enum class StepType {
        Parse, ProcessMiddlewares, ProcessRoute, Finished,
    };
    enum class StateType {
        NotProcessed, WaitingForData, Processing, Failed, Succeed,
    };

    RequestHandler(Connection& connection, Router<Connection>& router)
        : _buffer(connection), _parser(_buffer), _router(router),
          _request(), _response(connection),
          _createdAt(std::chrono::system_clock::now()) {}

    inline auto Age() {
       return std::chrono::system_clock::now() - _createdAt; 
    }
    
    inline auto Step() {
        return _step;
    }

    inline void Timeout() {
        _state = StateType::Failed;
        _response.Clear();
        _response.Status = HTTPResponseStatus::Type::RequestTimeOut;
    }

    StateType Process() {
        if (_state > StateType::Processing) {
            return _state;
        }

        std::array<std::pair<StepType, _Processor>, 4> steps = {
            std::make_pair(StepType::Parse, [&]() {
                using Result = typename RequestParser<Connection>::Result;

                auto result = _parser.Parse(_request);
                if (result == Result::Failed) {
                    _state = StateType::Failed;
                }

                if (result == Result::NoInputData) {
                    _state = StateType::WaitingForData;
                }

                return result != Result::IncompleteInputData
                    && result != Result::NoInputData;
            }),
            { StepType::ProcessMiddlewares, [&]() {
                // TODO
                return true;
            }},
            { StepType::ProcessRoute, [&]() {
                auto& controller = _router.Get(_request.Path, _request.Method);
                controller(_request, _response);
                return true;
            }},
            { StepType::Finished, [&]() {
                _state = StateType::Succeed;
                return true;
            }},
        }; 

        _state = StateType::Processing;
        try {
            for (auto& entry : steps) {
                const auto& [step, func] = entry;
                if (_step != step) {
                    continue;
                }

                bool step_finished = func();
                if (!step_finished || _state != StateType::Processing) {
                    break;
                }

                _step = static_cast<StepType>(static_cast<int>(_step) + 1);
            }
        }
        catch (const std::exception&) {
            _state = StateType::Failed;

            if (_response.Clear()) {
                const auto& errorController = _router.InternalServerErrorHandler();
                errorController(_request, _response);
                _response.Flush();
            }

            return _state;
        }

        return _state;
    }

private:
    using _Processor = std::function<bool()>;

    StepType _step = StepType::Parse;
    StateType _state = StateType::NotProcessed;
    SocketStreamBuffer<Connection> _buffer;
    RequestParser<Connection> _parser;
    Router<Connection>& _router;
    HTTPRequest _request;
    HTTPResponse<Connection> _response;
    std::chrono::system_clock::time_point _createdAt;

};

