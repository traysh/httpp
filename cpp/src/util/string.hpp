#pragma once

#include <string>
#include <istream>

namespace Util {
namespace String {
    std::string ToUpper(const std::string& s);

    std::pair<std::string, std::string> Split(const std::string& s, const char& c);

    std::pair<std::string, std::string> Split(const std::string& s, const std::string& sep);

    std::string Trim(const std::string& str);

    std::string RemoveAll(const std::string& str, const char c);

    std::string RemoveLeading(const std::string& str, const char c);

    std::string RemoveTrailing(const std::string& str, const char c);

    std::string RemoveLeadingOrTrailing(const std::string& str, const char c);
}
}
