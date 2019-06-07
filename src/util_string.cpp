#include "util_string.hpp"

#include <string>
#include <utility>
#include <regex>

namespace Util {
namespace String {
    std::string ToUpper(const std::string& s) {
        std::string transformed = s;
		std::transform(s.begin(), s.end(), transformed.begin(),
                       [](unsigned char c) -> unsigned char {
                           return std::toupper(c);
                      });

        return transformed;
    }

    std::pair<std::string, std::string> Split(const std::string& s, const char& c) {
        const size_t i = s.find(c);
        std::string s1, s2;

        s2 = s.substr(i + 1, s.size() -i -1);
        s1 = s.substr(0, i);
        
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
}
}

