#pragma once

#include <array>
#include <functional>

#include "httprequest.hpp"
#include "requestparser.hpp"
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

    HTTPRequest Request;

    RequestHandler(Connection& connection) : _buffer(connection), _parser(_buffer) {}

    State Process() {
        if (_state > State::Processing) {
            return _state;
        }

        std::array<std::pair<Step, _Processor>, 4> steps = {
            std::make_pair(Step::Parse, [&]() {
                using Result = typename RequestParser<Connection>::Result;

                auto result = _parser.Parse(Request);
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
                // TODO
                return true;
            }},
            { Step::Finished, [&]() {
                _state = State::Succeed;
                return true;
            }},
        }; 

        _state = State::Processing;
        for (auto& entry : steps) {
            auto& step = entry.first;
            if (_step != step) {
                continue;
            }

            auto& func = entry.second;
            bool step_finished = func();
            if (!step_finished || _state != State::Processing) {
                break;
            }

            _step = static_cast<Step>(static_cast<int>(_step) + 1);
        }

        return _state;
    }

private:
    using _Processor = std::function<bool()>;

    Step _step = Step::Parse;
    State _state = State::NotProcessed;
    SocketStreamBuffer<Connection> _buffer;
    RequestParser<Connection> _parser;
};

