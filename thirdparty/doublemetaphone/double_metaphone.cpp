#include "double_metaphone.h"
#include <cctype>

namespace doublemetaphone {

static char code_map(char c) {
    switch (c) {
        case 'b': case 'f': case 'p': case 'v': return '1';
        case 'c': case 'g': case 'j': case 'k': case 'q': case 's': case 'x': case 'z': return '2';
        case 'd': case 't': return '3';
        case 'l': return '4';
        case 'm': case 'n': return '5';
        case 'r': return '6';
        default: return '0';
    }
}

static std::string soundex(const std::string & word) {
    std::string result;
    char prev = '0';
    for (char ch : word) {
        char c = std::tolower(static_cast<unsigned char>(ch));
        if (!std::isalpha(c)) continue;
        if (result.empty()) {
            result.push_back(std::toupper(c));
            prev = code_map(c);
        } else {
            char code = code_map(c);
            if (code != '0' && code != prev) {
                result.push_back(code);
                if (result.size() == 4) break;
            }
            prev = code;
        }
    }
    while (result.size() < 4) result.push_back('0');
    return result;
}

std::pair<std::string, std::string> double_metaphone(const std::string & word) {
    std::string code = soundex(word);
    return {code, code};
}

} // namespace doublemetaphone
