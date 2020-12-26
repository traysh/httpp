#pragma once

#include <array>
#include <chrono>
#include <functional>
#include <thread>

#include "http/request.hpp"
#include "requestparser.hpp"
#include "router.hpp"
#include "socketstreambuffer.hpp"

class RequestHandler {
   public:
    enum class StepType {
        Parse,
        ProcessMiddlewares,
        ProcessRoute,
        Finished,
    };
    enum class StateType {
        NotProcessed,
        WaitingForData,
        Processing,
        Failed,
        Succeed,
    };

    RequestHandler(Connection& connection, Router& router)
        : _buffer(connection),
          _parser(_buffer),
          _router(router),
          _request(),
          _response(connection),
          _createdAt(std::chrono::system_clock::now()) {}

    inline auto Age() { return std::chrono::system_clock::now() - _createdAt; }

    inline auto State() { return _state; }

    inline auto Step() { return _step; }

    inline void GenericError() {
        _state = StateType::Failed;
        _response.Clear();
        _response.Status = HTTP::ResponseStatus::Type::InternalServerError;
    }

    inline void Timeout() {
        _state = StateType::Failed;
        _response.Clear();
        _response.Status = HTTP::ResponseStatus::Type::RequestTimeOut;
    }

    StateType Process() {
        if (_state > StateType::Processing) {
            return _state;
        }

        std::array<std::pair<StepType, _Processor>, 4> steps = {
            std::make_pair(StepType::Parse,
                           [&]() {
                               using Result = typename RequestParser::Result;

                               auto result = _parser.Parse(_request);
                               if (result == Result::Failed) {
                                   _state = StateType::Failed;
                                   _step = StepType::Finished;
                               }

                               if (result == Result::NoInputData) {
                                   _state = StateType::WaitingForData;
                               }

                               return result != Result::IncompleteInputData &&
                                      result != Result::NoInputData;
                           }),
            {StepType::ProcessMiddlewares,
             [&]() {
                 // TODO
                 return true;
             }},
            {StepType::ProcessRoute,
             [&]() {
                 const auto& [controller, parameters] =
                     _router.Get(_request.Path, _request.Method);
                 _request.parameters = parameters;
                 controller(_request, _response);
                 return true;
             }},
            {StepType::Finished,
             [&]() {
                 if (_state != StateType::Failed)
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
        } catch (const std::exception&) {
            _state = StateType::Failed;

            if (_response.Clear()) {
                const auto& errorController =
                    _router.InternalServerErrorHandler();
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
    SocketStreamBuffer _buffer;
    RequestParser _parser;
    Router& _router;
    HTTP::Request _request;
    HTTP::Response _response;
    std::chrono::system_clock::time_point _createdAt;
};

