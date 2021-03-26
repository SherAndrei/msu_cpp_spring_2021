#include <charconv>
#include <cctype>
#include "parser.h"

void TokenParser::set_start_callback(DefaultCallBack startCB)  { _startCB = startCB;  }
void TokenParser::set_number_callback(NumberCallBack numberCB) { _numCB   = numberCB; }
void TokenParser::set_string_callback(StringCallBack stringCB) { _strCB   = stringCB; }
void TokenParser::set_end_callback(DefaultCallBack endCB)      { _endCB   = endCB;    }

namespace {

void LeftStrip(std::string_view& sv) {
    while (!sv.empty() && std::isspace(sv[0]))
        sv.remove_prefix(1);
}

std::string_view ReadToken(std::string_view& sv) {
    LeftStrip(sv);

    auto pos = sv.find_first_of(" \t\n");
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos : sv.size());
    return result;
}

bool IsEndOfToken(char ch) {
    return (ch == '\0' || std::isspace(ch));
}

}  // namespace

void TokenParser::run(std::string_view sv) const {
    _startCB();
    uint64_t number;

    while (!sv.empty()) {
        if (auto token = ReadToken(sv);
            !token.empty()) {
            auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);
            (errc == std::errc() && IsEndOfToken(*ptr))
                ? _numCB(number)
                : _strCB(token);
        }
    }
    _endCB();
}
