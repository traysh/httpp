#pragma once

#include <string>
#include <istream>

namespace Util {
namespace String {
    std::string ToUpper(const std::string& s);

    std::pair<std::string, std::string> Split(const std::string& s, const char& c);

    std::string Trim(const std::string& str);

    std::string RemoveAll(const std::string& str, const char c);
}
}
