#include "util_stream.hpp"

namespace Util {
namespace Stream {
    bool NewLine(int value, std::istream& stream) {
        if (!stream.good()) {
            return false;
        }

        int next = stream.peek();
        return (value == '\r' && next == '\n') || value == '\n';
    }
}
}
