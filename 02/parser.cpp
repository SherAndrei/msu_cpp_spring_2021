#include <charconv>
#include <cctype>
#include "parser.h"

void Parser::set_start_callback(DefaultCallBack startCB)  { _startCB = startCB;  }
void Parser::set_number_callback(NumberCallBack numberCB) { _numCB   = numberCB; }
void Parser::set_string_callback(StringCallBack stringCB) { _strCB   = stringCB; }
void Parser::set_end_callback(DefaultCallBack endCB)      { _endCB   = endCB;    }

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

}  // namespace

void Parser::run(std::string_view sv) const {
    _startCB();
    uint64_t number = -1;

    while (!sv.empty()) {
        auto token = ReadToken(sv);
        auto [ptr, errc] = std::from_chars(token.begin(), token.end(), number);

        if (errc == std::errc() && (*ptr == '\0' || std::isspace(*ptr))) {
            _numCB(number);
        } else if (!sv.empty()) {
            _strCB(token);
        }
    }
    _endCB();
}
