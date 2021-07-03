#ifndef TOKEN_PARSER_H
#define TOKEN_PARSER_H

#include <string>
#include <string_view>
#include <functional>

class TokenParser {
 public:
    using DefaultCallBack = std::function<void(void)>;
    using StringCallBack  = std::function<void(const std::string&)>;
    using NumberCallBack  = std::function<void(uint64_t)>;

 public:
    TokenParser() = default;

    void set_start_callback(DefaultCallBack startCB);
    void set_number_callback(NumberCallBack numberCB);
    void set_string_callback(StringCallBack stringCB);
    void set_end_callback(DefaultCallBack endCB);

    void run(std::string_view strv_to_parse) const;

 private:
    DefaultCallBack _startCB = []{};
    NumberCallBack  _numCB   = [](uint64_t){};
    StringCallBack  _strCB   = [](const std::string&){};
    DefaultCallBack _endCB   = []{};
};

#endif  // TOKEN_PARSER_H
