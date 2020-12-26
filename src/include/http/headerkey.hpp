#pragma once

#include <string>

#include "util_string.hpp"

namespace HTTP {
struct HeaderKey {
    HeaderKey(const std::string& str)
        : _data(Util::String::ToUpper(str)) {}

    operator const std::string&() const {
        return _data;
    }

    inline bool operator<(const HeaderKey& other) const { 
        const auto& this_str = static_cast<std::string>(*this);
        const auto& other_str = static_cast<std::string>(other);
        return this_str < other_str;
    }

    private:
        std::string _data;
};
}
