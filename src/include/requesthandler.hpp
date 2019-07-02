#pragma once

#include <array>
#include <functional>

#include "httprequest.hpp"
#include "requestparser.hpp"
#include "router.hpp"
#include "socketstreambuffer.hpp"

template <class Connection>
class RequestHandler {
public:
    enum class Step {
        Parse, ProcessMiddlewares, ProcessRoute, Finished,
    };
    enum class State {
        NotProcessed, Processing, Failed, Succeed,
    };

    RequestHandler(Connection& connection, Router<Connection>& router)
        : _buffer(connection), _parser(_buffer), _router(router),
          _request(), _response(connection) {}

    State Process() {
        if (_state > State::Processing) {
            return _state;
        }

        std::array<std::pair<Step, _Processor>, 4> steps = {
            std::make_pair(Step::Parse, [&]() {
                using Result = typename RequestParser<Connection>::Result;

                auto result = _parser.Parse(_request);
                if (result == Result::Failed) {
                    _state = State::Failed;
                }

                return result == Result::Success;
            }),
            { Step::ProcessMiddlewares, [&]() {
                // TODO
                return true;
            }},
            { Step::ProcessRoute, [&]() {
                auto& controller = _router.Get(_request.Path, _request.Method);
                controller(_request, _response);
                return true;
            }},
            { Step::Finished, [&]() {
                _state = State::Succeed;
                return true;
            }},
        }; 

        try {
            _state = State::Processing;
            for (auto& entry : steps) {
                const auto& [step, func] = entry;
                if (_step != step) {
                    continue;
                }

                bool step_finished = func();
                if (!step_finished || _state != State::Processing) {
                    break;
                }
                _step = static_cast<Step>(static_cast<int>(_step) + 1);
            }
        }
        catch (const std::exception&) {
            _state = State::Failed;

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

    Step _step = Step::Parse;
    State _state = State::NotProcessed;
    SocketStreamBuffer<Connection> _buffer;
    RequestParser<Connection> _parser;
    Router<Connection>& _router;
    HTTPRequest _request;
    HTTPResponse<Connection> _response;

};

