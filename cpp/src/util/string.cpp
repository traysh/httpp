#include "util/string.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <regex>

namespace Util::String {
    std::string ToUpper(const std::string& s) {
        std::string transformed;
		std::transform(s.begin(), s.end(), std::back_inserter(transformed),
            [](unsigned char c) -> unsigned char {
                return std::toupper(c);
            });

        return transformed;
    }

    std::pair<std::string, std::string> Split(const std::string& s, const char& c) {
        const size_t i = s.find(c);

        if (i == std::string::npos) {
            return { s, "" };
        }

        const auto& s2 = s.substr(i + 1, s.size() -i -1);
        const auto& s1 = s.substr(0, i);
        
        return { s1, s2 };
    }

    std::pair<std::string, std::string> Split(const std::string& s, const std::string& sep) {
        const size_t i = s.find(sep);

        if (i == std::string::npos) {
            return { s, "" };
        }

        const auto& s2 = s.substr(i + sep.size(), s.size() -i -sep.size());
        const auto& s1 = s.substr(0, i);

        return { s1, s2 };
    }

    std::string Trim(const std::string& str) {
        using namespace std;
        return regex_replace(str, regex("^\\s+|\\s+$"), string());
    }

    std::string RemoveAll(const std::string& str, const char c) {
        std::string clean = str;
        clean.erase(std::remove(clean.begin(), clean.end(), c), clean.end());
        return clean;
    }

    std::string RemoveLeading(const std::string& str, const char c) {
       const auto begin = str.find_first_not_of(c);
       return begin != std::string::npos ? str.substr(begin) : std::string();
    }

    std::string RemoveTrailing(const std::string& str, const char c) {
       const auto end = str.find_last_not_of(c);
       return end != std::string::npos ? str.substr(0, end+1) : std::string();
    }

    std::string RemoveLeadingOrTrailing(const std::string& str, const char c) {
       return RemoveTrailing(RemoveLeading(str, c), c);
    }
}

