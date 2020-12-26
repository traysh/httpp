#pragma once

#include <cctype>
#include <istream>
#include <set>
#include <sstream>
#include <string_view>

namespace Util {
class StreamProcessor {
public:
    enum class Result { Success, Failed, IncompleteInputData, NoInputData };

    using CharEvalFunc = bool(*)(int);

    // tem mesmo que ter um char, senão vou pegar outro do stream
    static inline bool NewLine(int value, std::istream& stream) {
        return value == '\n' || (value == '\r' && stream.peek() == '\n');
    }

    static inline bool NewLine(std::istream& stream) {
        int value = stream.get();
        bool new_line = NewLine(value, stream);
        stream.putback(value);

        return new_line;
    }

    inline bool NewLine(int value) {
        return NewLine(value, _stream);
    }

    inline bool NewLine() {
        return NewLine(_stream);
    }

    inline static std::set<char> Spaces(std::set<char> extra = {}) {
        static const std::set<char> separators = { ' ', '\t',  };
        extra.insert(separators.begin(), separators.end());
        return extra;
    }

    inline static std::set<char> DefaultInvalid(std::set<char> extra = {}) {
        static const std::set<char> invalid = {};
        extra.insert(invalid.begin(), invalid.end());
        return extra;
    }

    StreamProcessor(std::istream& stream) : _stream(stream) {}

    // FIXME include max string size
    inline Result ExtractWord(std::string& str,
                     bool multiline = false,
                     bool to_upper = false,
                     const std::set<char>& separators = Spaces(), // FIXME
                     const std::set<char>& skip = Spaces(), // FIXME
                     const std::set<char>& invalid = DefaultInvalid()) { // FIXME

        std::stringstream ss;
        int next_char;

        // skip allowed leading blank chars
        for (next_char = _stream.get();
             _stream.good() && (skip.find(next_char) != skip.end()
                 || (multiline && NewLine(next_char)));
             next_char = _stream.get());
            
        for (; _stream.good(); next_char = _stream.get()) {
            if (next_char == '\r') {
                continue;
            }
            if (NewLine(next_char) || separators.find(next_char) != separators.end()) {
                _stream.putback(next_char);
                break;
            }

            if (invalid.find(next_char) != invalid.end()) {
                return Result::Failed;
            }

            ss << static_cast<char>(to_upper ? ::toupper(next_char) : next_char);
        }

        if (_stream.eof()) {
            _stream.clear();
            return Result::IncompleteInputData;
        }

        if (ss.rdbuf()->in_avail() == 0) {
            return Result::Failed;
        }

        str = ss.str();
        _stream.get();
        return Result::Success;
    }

private:
    std::istream& _stream;
};
}
